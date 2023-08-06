static inline void RENAME(initFilter)(int16_t *dstFilter, int16_t *filterPos, int *filterSize, int xInc,

				      int srcW, int dstW, int filterAlign, int one)

{

	int i;

	double filter[8000];

#ifdef HAVE_MMX

	asm volatile("emms\n\t"::: "memory"); 

#endif



	if(ABS(xInc - 0x10000) <10) 

	{

		int i;

		*filterSize= (1 +(filterAlign-1)) & (~(filterAlign-1)); 

		for(i=0; i<dstW*(*filterSize); i++) filter[i]=0;



		for(i=0; i<dstW; i++)

		{

			filter[i*(*filterSize)]=1;

			filterPos[i]=i;

		}



	}

	else if(xInc <= (1<<16) || sws_flags==SWS_FAST_BILINEAR) 

	{

		int i;

		int xDstInSrc;

		if(sws_flags==SWS_BICUBIC) *filterSize= 4;

		else			   *filterSize= 2;



		*filterSize= (*filterSize +(filterAlign-1)) & (~(filterAlign-1));



		xDstInSrc= xInc/2 - 0x8000;

		for(i=0; i<dstW; i++)

		{

			int xx= (xDstInSrc>>16) - (*filterSize>>1) + 1;

			int j;



			filterPos[i]= xx;

			if(sws_flags == SWS_BICUBIC)

			{

				double d= ABS(((xx+1)<<16) - xDstInSrc)/(double)(1<<16);

				double y1,y2,y3,y4;

				double A= -0.75;

					

				y1 = (        +     A*d -       2.0*A*d*d +       A*d*d*d);

				y2 = (+ 1.0             -     (A+3.0)*d*d + (A+2.0)*d*d*d);

				y3 = (        -     A*d + (2.0*A+3.0)*d*d - (A+2.0)*d*d*d);

				y4 = (                  +           A*d*d -       A*d*d*d);





				filter[i*(*filterSize) + 0]= y1;

				filter[i*(*filterSize) + 1]= y2;

				filter[i*(*filterSize) + 2]= y3;

				filter[i*(*filterSize) + 3]= y4;



			}

			else

			{

				for(j=0; j<*filterSize; j++)

				{

					double d= ABS((xx<<16) - xDstInSrc)/(double)(1<<16);

					double coeff= 1.0 - d;

					if(coeff<0) coeff=0;

	

					filter[i*(*filterSize) + j]= coeff;

					xx++;

				}

			}

			xDstInSrc+= xInc;

		}

	}

	else 

	{

		int xDstInSrc;

		if(sws_flags==SWS_BICUBIC) *filterSize= (int)ceil(1 + 4.0*srcW / (double)dstW);

		else			   *filterSize= (int)ceil(1 + 2.0*srcW / (double)dstW);



		*filterSize= (*filterSize +(filterAlign-1)) & (~(filterAlign-1));



		xDstInSrc= xInc/2 - 0x8000;

		for(i=0; i<dstW; i++)

		{

			int xx= (int)((double)xDstInSrc/(double)(1<<16) - ((*filterSize)-1)*0.5 + 0.5);

			int j;



			filterPos[i]= xx;

			for(j=0; j<*filterSize; j++)

			{

				double d= ABS((xx<<16) - xDstInSrc)/(double)xInc;

				double coeff;

				if(sws_flags == SWS_BICUBIC)

				{

					double A= -0.75;



					

					if(d<1.0)

						coeff = (1.0 - (A+3.0)*d*d + (A+2.0)*d*d*d);

					else if(d<2.0)

						coeff = (-4.0*A + 8.0*A*d - 5.0*A*d*d + A*d*d*d);

					else

						coeff=0.0;

				}

				else

				{

					coeff= 1.0 - d;

					if(coeff<0) coeff=0;

				}



				filter[i*(*filterSize) + j]= coeff;

				xx++;

			}

			xDstInSrc+= xInc;

		}

	}



	

	for(i=0; i<dstW; i++)

	{

		int j;

		if(filterPos[i] < 0)

		{

			

			for(j=1; j<*filterSize; j++)

			{

				int left= MAX(j + filterPos[i], 0);

				filter[i*(*filterSize) + left] += filter[i*(*filterSize) + j];

				filter[i*(*filterSize) + j]=0;

			}

			filterPos[i]= 0;

		}



		if(filterPos[i] + (*filterSize) > srcW)

		{

			int shift= filterPos[i] + (*filterSize) - srcW;

			

			for(j=(*filterSize)-2; j>=0; j--)

			{

				int right= MIN(j + shift, (*filterSize)-1);

				filter[i*(*filterSize) +right] += filter[i*(*filterSize) +j];

				filter[i*(*filterSize) +j]=0;

			}

			filterPos[i]= srcW - (*filterSize);

		}

	}



	

	



	

	for(i=0; i<dstW; i++)

	{

		int j;

		double sum=0;

		double scale= one;

		for(j=0; j<*filterSize; j++)

		{

			sum+= filter[i*(*filterSize) + j];

		}

		scale/= sum;

		for(j=0; j<*filterSize; j++)

		{

			dstFilter[i*(*filterSize) + j]= (int)(filter[i*(*filterSize) + j]*scale);

		}

	}

}