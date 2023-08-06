static void load_symbols(struct elfhdr *hdr, int fd, abi_ulong load_bias)

{

    int i, shnum, nsyms, sym_idx = 0, str_idx = 0;

    struct elf_shdr *shdr;

    char *strings = NULL;

    struct syminfo *s = NULL;

    struct elf_sym *new_syms, *syms = NULL;



    shnum = hdr->e_shnum;

    i = shnum * sizeof(struct elf_shdr);

    shdr = (struct elf_shdr *)alloca(i);

    if (pread(fd, shdr, i, hdr->e_shoff) != i) {

        return;

    }



    bswap_shdr(shdr, shnum);

    for (i = 0; i < shnum; ++i) {

        if (shdr[i].sh_type == SHT_SYMTAB) {

            sym_idx = i;

            str_idx = shdr[i].sh_link;

            goto found;

        }

    }



    

    return;



 found:

    

    s = g_try_new(struct syminfo, 1);

    if (!s) {

        goto give_up;

    }



    i = shdr[str_idx].sh_size;

    s->disas_strtab = strings = g_try_malloc(i);

    if (!strings || pread(fd, strings, i, shdr[str_idx].sh_offset) != i) {

        goto give_up;

    }



    i = shdr[sym_idx].sh_size;

    syms = g_try_malloc(i);

    if (!syms || pread(fd, syms, i, shdr[sym_idx].sh_offset) != i) {

        goto give_up;

    }



    nsyms = i / sizeof(struct elf_sym);

    for (i = 0; i < nsyms; ) {

        bswap_sym(syms + i);

        

        if (syms[i].st_shndx == SHN_UNDEF

            || syms[i].st_shndx >= SHN_LORESERVE

            || ELF_ST_TYPE(syms[i].st_info) != STT_FUNC) {

            if (i < --nsyms) {

                syms[i] = syms[nsyms];

            }

        } else {

#if defined(TARGET_ARM) || defined (TARGET_MIPS)

            

            syms[i].st_value &= ~(target_ulong)1;

#endif

            syms[i].st_value += load_bias;

            i++;

        }

    }



    

    if (nsyms == 0) {

        goto give_up;

    }



    

    new_syms = g_try_renew(struct elf_sym, syms, nsyms);

    if (new_syms == NULL) {

        goto give_up;

    }

    syms = new_syms;



    qsort(syms, nsyms, sizeof(*syms), symcmp);



    s->disas_num_syms = nsyms;

#if ELF_CLASS == ELFCLASS32

    s->disas_symtab.elf32 = syms;

#else

    s->disas_symtab.elf64 = syms;

#endif

    s->lookup_symbol = lookup_symbolxx;

    s->next = syminfos;

    syminfos = s;



    return;



give_up:

    g_free(s);

    g_free(strings);

    g_free(syms);

}