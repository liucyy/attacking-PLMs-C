int mm_support(void)

{

    int rval;

    int eax, ebx, ecx, edx;

    

    __asm__ __volatile__ (

                          

                          

                          "pushf\n\t"

                          "pop %0\n\t"

                          "movl %0, %1\n\t"

                          

                          

                          

                          "xorl $0x200000, %0\n\t"

                          "push %0\n\t"

                          "popf\n\t"

                          

                          

                          "pushf\n\t"

                          "pop %0\n\t"

                          : "=a" (eax), "=c" (ecx)

                          :

                          : "cc" 

                          );

    

    if (eax == ecx)

        return 0; 

    

    cpuid(0, eax, ebx, ecx, edx);



    if (ebx == 0x756e6547 &&

        edx == 0x49656e69 &&

        ecx == 0x6c65746e) {

        

        

    inteltest:

        cpuid(1, eax, ebx, ecx, edx);

        if ((edx & 0x00800000) == 0)

            return 0;

        rval = MM_MMX;

        if (edx & 0x02000000) 

            rval |= MM_MMXEXT | MM_SSE;

        if (edx & 0x04000000) 

            rval |= MM_SSE2;

        return rval;

    } else if (ebx == 0x68747541 &&

               edx == 0x69746e65 &&

               ecx == 0x444d4163) {

        

        cpuid(0x80000000, eax, ebx, ecx, edx);

        if ((unsigned)eax < 0x80000001)

            goto inteltest;

        cpuid(0x80000001, eax, ebx, ecx, edx);

        if ((edx & 0x00800000) == 0)

            return 0;

        rval = MM_MMX;

        if (edx & 0x80000000)

            rval |= MM_3DNOW;

        if (edx & 0x00400000)

            rval |= MM_MMXEXT;

        return rval;

    } else if (ebx == 0x746e6543 &&

               edx == 0x48727561 &&

               ecx == 0x736c7561) {  

        

        cpuid(0x80000000, eax, ebx, ecx, edx);

        if ((unsigned)eax < 0x80000001)

            goto inteltest;	

	cpuid(0x80000001, eax, ebx, ecx, edx);

	rval = 0;      

	if( edx & ( 1 << 31) )

	  rval |= MM_3DNOW;

	if( edx & ( 1 << 23) )

	  rval |= MM_MMX;

	if( edx & ( 1 << 24) )

	  rval |= MM_MMXEXT;

        if(rval==0)

            goto inteltest;

	return rval;

    } else if (ebx == 0x69727943 &&

               edx == 0x736e4978 &&

               ecx == 0x64616574) {

        

        

        

        if (eax != 2) 

            goto inteltest;

        cpuid(0x80000001, eax, ebx, ecx, edx);

        if ((eax & 0x00800000) == 0)

            return 0;

        rval = MM_MMX;

        if (eax & 0x01000000)

            rval |= MM_MMXEXT;

        return rval;

    } else if (ebx == 0x756e6547 &&

               edx == 0x54656e69 &&

               ecx == 0x3638784d) {

        

        cpuid(0x80000000, eax, ebx, ecx, edx);

        if ((unsigned)eax < 0x80000001)

            return 0;

        cpuid(0x80000001, eax, ebx, ecx, edx);

        if ((edx & 0x00800000) == 0)

            return 0;

        return MM_MMX;

    } else {

        return 0;

    }

}