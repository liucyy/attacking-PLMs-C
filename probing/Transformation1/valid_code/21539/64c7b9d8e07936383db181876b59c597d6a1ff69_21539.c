void check_file_fixed_eof_mmaps(void)

{

	char *addr;

	char *cp;

	unsigned int *p1;

	uintptr_t p;

	int i;



	

	addr = mmap(NULL, pagesize * 44, PROT_READ, 

		    MAP_PRIVATE | MAP_ANONYMOUS,

		    -1, 0);



	fprintf (stderr, "%s addr=%p", __func__, (void *)addr);

	fail_unless (addr != MAP_FAILED);



	for (i = 0; i < 0x10; i++)

	{

		

		p1 = mmap(addr, pagesize, PROT_READ, 

			  MAP_PRIVATE | MAP_FIXED, 

			  test_fd, 

			  (test_fsize - sizeof *p1) & ~pagemask);



		fail_unless (p1 != MAP_FAILED);



		

		p = (uintptr_t) p1;

		fail_unless ((p & pagemask) == 0);



		

		fail_unless (p1[(test_fsize & pagemask) / sizeof *p1 - 1]

			     == ((test_fsize - sizeof *p1) / sizeof *p1));



		

		cp = (void *)p1;

		fail_unless (cp[pagesize - 4] == 0);

		munmap (p1, pagesize);

		addr += pagesize;

	}

	fprintf (stderr, " passed\n");

}