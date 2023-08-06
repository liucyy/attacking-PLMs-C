static void OPLWriteReg(FM_OPL *OPL, int r, int v)

{

	OPL_CH *CH;

	int slot;

	int block_fnum;



	switch(r&0xe0)

	{

	case 0x00: 

		switch(r&0x1f)

		{

		case 0x01:

			

			if(OPL->type&OPL_TYPE_WAVESEL)

			{

				OPL->wavesel = v&0x20;

				if(!OPL->wavesel)

				{

					

					int c;

					for(c=0;c<OPL->max_ch;c++)

					{

						OPL->P_CH[c].SLOT[SLOT1].wavetable = &SIN_TABLE[0];

						OPL->P_CH[c].SLOT[SLOT2].wavetable = &SIN_TABLE[0];

					}

				}

			}

			return;

		case 0x02:	

			OPL->T[0] = (256-v)*4;

			break;

		case 0x03:	

			OPL->T[1] = (256-v)*16;

			return;

		case 0x04:	

			if(v&0x80)

			{	

				OPL_STATUS_RESET(OPL,0x7f);

			}

			else

			{	

				UINT8 st1 = v&1;

				UINT8 st2 = (v>>1)&1;

				

				OPL_STATUS_RESET(OPL,v&0x78);

				OPL_STATUSMASK_SET(OPL,((~v)&0x78)|0x01);

				

				if(OPL->st[1] != st2)

				{

					double interval = st2 ? (double)OPL->T[1]*OPL->TimerBase : 0.0;

					OPL->st[1] = st2;

					if (OPL->TimerHandler) (OPL->TimerHandler)(OPL->TimerParam+1,interval);

				}

				

				if(OPL->st[0] != st1)

				{

					double interval = st1 ? (double)OPL->T[0]*OPL->TimerBase : 0.0;

					OPL->st[0] = st1;

					if (OPL->TimerHandler) (OPL->TimerHandler)(OPL->TimerParam+0,interval);

				}

			}

			return;

#if BUILD_Y8950

		case 0x06:		

			if(OPL->type&OPL_TYPE_KEYBOARD)

			{

				if(OPL->keyboardhandler_w)

					OPL->keyboardhandler_w(OPL->keyboard_param,v);

				else

					LOG(LOG_WAR,("OPL:write unmapped KEYBOARD port\n"));

			}

			return;

		case 0x07:	

			if(OPL->type&OPL_TYPE_ADPCM)

				YM_DELTAT_ADPCM_Write(OPL->deltat,r-0x07,v);

			return;

		case 0x08:	

			OPL->mode = v;

			v&=0x1f;	

		case 0x09:		

		case 0x0a:

		case 0x0b:		

		case 0x0c:

		case 0x0d:		

		case 0x0e:

		case 0x0f:		

		case 0x10: 		

		case 0x11: 		

		case 0x12: 		

			if(OPL->type&OPL_TYPE_ADPCM)

				YM_DELTAT_ADPCM_Write(OPL->deltat,r-0x07,v);

			return;

#if 0

		case 0x15:		

		case 0x16:

		case 0x17:		

			return;

		case 0x18:		

			if(OPL->type&OPL_TYPE_IO)

				OPL->portDirection = v&0x0f;

			return;

		case 0x19:		

			if(OPL->type&OPL_TYPE_IO)

			{

				OPL->portLatch = v;

				if(OPL->porthandler_w)

					OPL->porthandler_w(OPL->port_param,v&OPL->portDirection);

			}

			return;

		case 0x1a:		

			return;

#endif

#endif

		}

		break;

	case 0x20:	

		slot = slot_array[r&0x1f];

		if(slot == -1) return;

		set_mul(OPL,slot,v);

		return;

	case 0x40:

		slot = slot_array[r&0x1f];

		if(slot == -1) return;

		set_ksl_tl(OPL,slot,v);

		return;

	case 0x60:

		slot = slot_array[r&0x1f];

		if(slot == -1) return;

		set_ar_dr(OPL,slot,v);

		return;

	case 0x80:

		slot = slot_array[r&0x1f];

		if(slot == -1) return;

		set_sl_rr(OPL,slot,v);

		return;

	case 0xa0:

		switch(r)

		{

		case 0xbd:

			

			{

			UINT8 rkey = OPL->rythm^v;

			OPL->ams_table = &AMS_TABLE[v&0x80 ? AMS_ENT : 0];

			OPL->vib_table = &VIB_TABLE[v&0x40 ? VIB_ENT : 0];

			OPL->rythm  = v&0x3f;

			if(OPL->rythm&0x20)

			{

#if 0

				usrintf_showmessage("OPL Rythm mode select");

#endif

				

				if(rkey&0x10)

				{

					if(v&0x10)

					{

						OPL->P_CH[6].op1_out[0] = OPL->P_CH[6].op1_out[1] = 0;

						OPL_KEYON(&OPL->P_CH[6].SLOT[SLOT1]);

						OPL_KEYON(&OPL->P_CH[6].SLOT[SLOT2]);

					}

					else

					{

						OPL_KEYOFF(&OPL->P_CH[6].SLOT[SLOT1]);

						OPL_KEYOFF(&OPL->P_CH[6].SLOT[SLOT2]);

					}

				}

				

				if(rkey&0x08)

				{

					if(v&0x08) OPL_KEYON(&OPL->P_CH[7].SLOT[SLOT2]);

					else       OPL_KEYOFF(&OPL->P_CH[7].SLOT[SLOT2]);

				}

				if(rkey&0x04)

				{

					if(v&0x04) OPL_KEYON(&OPL->P_CH[8].SLOT[SLOT1]);

					else       OPL_KEYOFF(&OPL->P_CH[8].SLOT[SLOT1]);

				}

				

				if(rkey&0x02)

				{

					if(v&0x02) OPL_KEYON(&OPL->P_CH[8].SLOT[SLOT2]);

					else       OPL_KEYOFF(&OPL->P_CH[8].SLOT[SLOT2]);

				}

				

				if(rkey&0x01)

				{

					if(v&0x01) OPL_KEYON(&OPL->P_CH[7].SLOT[SLOT1]);

					else       OPL_KEYOFF(&OPL->P_CH[7].SLOT[SLOT1]);

				}

			}

			}

			return;

		}

		

		if( (r&0x0f) > 8) return;

		CH = &OPL->P_CH[r&0x0f];

		if(!(r&0x10))

		{	

			block_fnum  = (CH->block_fnum&0x1f00) | v;

		}

		else

		{	

			int keyon = (v>>5)&1;

			block_fnum = ((v&0x1f)<<8) | (CH->block_fnum&0xff);

			if(CH->keyon != keyon)

			{

				if( (CH->keyon=keyon) )

				{

					CH->op1_out[0] = CH->op1_out[1] = 0;

					OPL_KEYON(&CH->SLOT[SLOT1]);

					OPL_KEYON(&CH->SLOT[SLOT2]);

				}

				else

				{

					OPL_KEYOFF(&CH->SLOT[SLOT1]);

					OPL_KEYOFF(&CH->SLOT[SLOT2]);

				}

			}

		}

		

		if(CH->block_fnum != block_fnum)

		{

			int blockRv = 7-(block_fnum>>10);

			int fnum   = block_fnum&0x3ff;

			CH->block_fnum = block_fnum;



			CH->ksl_base = KSL_TABLE[block_fnum>>6];

			CH->fc = OPL->FN_TABLE[fnum]>>blockRv;

			CH->kcode = CH->block_fnum>>9;

			if( (OPL->mode&0x40) && CH->block_fnum&0x100) CH->kcode |=1;

			CALC_FCSLOT(CH,&CH->SLOT[SLOT1]);

			CALC_FCSLOT(CH,&CH->SLOT[SLOT2]);

		}

		return;

	case 0xc0:

		

		if( (r&0x0f) > 8) return;

		CH = &OPL->P_CH[r&0x0f];

		{

		int feedback = (v>>1)&7;

		CH->FB   = feedback ? (8+1) - feedback : 0;

		CH->CON = v&1;

		set_algorythm(CH);

		}

		return;

	case 0xe0: 

		slot = slot_array[r&0x1f];

		if(slot == -1) return;

		CH = &OPL->P_CH[slot/2];

		if(OPL->wavesel)

		{

			

			CH->SLOT[slot&1].wavetable = &SIN_TABLE[(v&0x03)*SIN_ENT];

		}

		return;

	}

}