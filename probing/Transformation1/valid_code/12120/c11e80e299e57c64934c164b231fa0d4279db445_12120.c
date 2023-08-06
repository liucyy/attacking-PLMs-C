void Y8950UpdateOne(FM_OPL *OPL, INT16 *buffer, int length)

{

    int i;

	int data;

	OPLSAMPLE *buf = buffer;

	UINT32 amsCnt  = OPL->amsCnt;

	UINT32 vibCnt  = OPL->vibCnt;

	UINT8 rythm = OPL->rythm&0x20;

	OPL_CH *CH,*R_CH;

	YM_DELTAT *DELTAT = OPL->deltat;



	

	YM_DELTAT_DECODE_PRESET(DELTAT);



	if( (void *)OPL != cur_chip ){

		cur_chip = (void *)OPL;

		

		S_CH = OPL->P_CH;

		E_CH = &S_CH[9];

		

		SLOT7_1 = &S_CH[7].SLOT[SLOT1];

		SLOT7_2 = &S_CH[7].SLOT[SLOT2];

		SLOT8_1 = &S_CH[8].SLOT[SLOT1];

		SLOT8_2 = &S_CH[8].SLOT[SLOT2];

		

		amsIncr = OPL->amsIncr;

		vibIncr = OPL->vibIncr;

		ams_table = OPL->ams_table;

		vib_table = OPL->vib_table;

	}

	R_CH = rythm ? &S_CH[6] : E_CH;

    for( i=0; i < length ; i++ )

	{

		

		

		ams = ams_table[(amsCnt+=amsIncr)>>AMS_SHIFT];

		vib = vib_table[(vibCnt+=vibIncr)>>VIB_SHIFT];

		outd[0] = 0;

		

		if( DELTAT->portstate )

			YM_DELTAT_ADPCM_CALC(DELTAT);

		

		for(CH=S_CH ; CH < R_CH ; CH++)

			OPL_CALC_CH(CH);

		

		if(rythm)

			OPL_CALC_RH(S_CH);

		

		data = Limit( outd[0] , OPL_MAXOUT, OPL_MINOUT );

		

		buf[i] = data >> OPL_OUTSB;

	}

	OPL->amsCnt = amsCnt;

	OPL->vibCnt = vibCnt;

	

	if( !DELTAT->portstate )

		OPL->status &= 0xfe;

}