static void bswap_phdr(struct elf_phdr *phdr)

{

    bswap32s(&phdr->p_type);            

    bswaptls(&phdr->p_offset);          

    bswaptls(&phdr->p_vaddr);           

    bswaptls(&phdr->p_paddr);           

    bswaptls(&phdr->p_filesz);          

    bswaptls(&phdr->p_memsz);           

    bswap32s(&phdr->p_flags);           

    bswaptls(&phdr->p_align);           

}