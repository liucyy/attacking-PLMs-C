void check_file_unfixed_eof_mmaps(void)

{

	char *cp;

	unsigned int *p1;

	uintptr_t p;

	int i;



	fprintf (stderr, "%s", __func__);

	for (i = 0; i < 0x10; i++)

	{

		p1 = mmap(NULL, pagesize, PROT_READ, 

			  MAP_PRIVATE, 

			  test_fd, 

			  (test_fsize - sizeof *p1) & ~pagemask);



		fail_unless (p1 != MAP_FAILED);



		

		p = (uintptr_t) p1;

		fail_unless ((p & pagemask) == 0);

		

		fail_unless (p1[(test_fsize & pagemask) / sizeof *p1 - 1]

			     == ((test_fsize - sizeof *p1) / sizeof *p1));



		

		cp = (void *) p1;

		fail_unless (cp[pagesize - 4] == 0);

		munmap (p1, pagesize);

	}

	fprintf (stderr, " passed\n");

}