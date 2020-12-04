/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM vector library for BCM56860
 */

#ifndef TDM_MN_VECTOR_LIBRARY_H
#define TDM_MN_VECTOR_LIBRARY_H
/**
@name: tdm_mn_vector_load
@param:

Generate vector at index in TDM vector map
 */
int
tdm_mn_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports)
{
	int i, n=0;
	
	switch (bw)
	{
/* ################################################################################################################## */
    case 933: /*NAK AP*/
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
				/*		n=0;
						for (i=0; i<len; i+=(len/4)) {
							map[yy][i] = TOKEN_10G;
							if ((++n)==4) {
								break;
							}
						}*/
							map[yy][0] = TOKEN_10G;
							map[yy][58] = TOKEN_10G;
							map[yy][116] = TOKEN_10G;
							map[yy][175] = TOKEN_10G;
						break;
					case 12:
						n=0;
						for (i=0; i<len; i+=(len/5)) {
							map[yy][i] = TOKEN_12G;
							if ((++n)==5) {
								break;
							}
						}
						break;
					case 20:
					case 21:
						n=0;
						for (i=0; i<len; i+=(len/8)) {
							map[yy][i] = TOKEN_20G;
							if ((++n)==8) {
								break;
							}	
						}
						break;					
					case 40:
					case 41:
					case 42:
					/*	n=0;
						for (i=0; i<len; i+=(len/16)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==16) {
								break;
							}
						}*/
							map[yy][0] = TOKEN_40G;
							map[yy][15] = TOKEN_40G;
							map[yy][29] = TOKEN_40G;
							map[yy][44] = TOKEN_40G;
							map[yy][58] = TOKEN_40G;
							map[yy][73] = TOKEN_40G;
							map[yy][87] = TOKEN_40G;
							map[yy][102] = TOKEN_40G;
							map[yy][116] = TOKEN_40G;
							map[yy][131] = TOKEN_40G;
							map[yy][145] = TOKEN_40G;
							map[yy][160] = TOKEN_40G;
							map[yy][174] = TOKEN_40G;
							map[yy][189] = TOKEN_40G;
							map[yy][203] = TOKEN_40G;
							map[yy][218] = TOKEN_40G;
						break;
					case 100:
					case 106:
					case 107:
						n=0;
						for (i=0; i<len; i+=(len/40)) {
							map[yy][i] = TOKEN_100G;
							if ((++n)==40) {
								break;
							}
						}
						break;					
					case 120:
						n=0;
						for (i=0; i<len; i+=(len/48)) {
							map[yy][i] = TOKEN_120G;
							if ((++n)==48) {
								break;
							}
						}
						break;
				case 150: /* for ancl slots*/
						map[yy][0] = MN_ANCL_TOKEN;
						map[yy][26] = MN_ANCL_TOKEN;
						map[yy][52] = MN_ANCL_TOKEN;
						map[yy][78] = MN_ANCL_TOKEN;
						map[yy][104] = MN_ANCL_TOKEN;
						map[yy][129] = MN_ANCL_TOKEN;
						map[yy][155] = MN_ANCL_TOKEN;
						map[yy][181] = MN_ANCL_TOKEN;						
						map[yy][207] = MN_ANCL_TOKEN;						
						break;
					default:
						return 0;
						break;
			}
			break;
		case 793: /* TD2/+ */
		case 794: 
			switch (port_speed)
			{
				case 1:
					map[yy][0] = TOKEN_1G;
					break;
				case 10:
				case 11:
					map[yy][0] = TOKEN_10G;
					map[yy][50] = TOKEN_10G;
					map[yy][100] = TOKEN_10G;
					map[yy][150] = TOKEN_10G;
					break;
				case 12:
					map[yy][0] = TOKEN_12G;
					map[yy][36] = TOKEN_12G;
					map[yy][72] = TOKEN_12G;
					map[yy][120] = TOKEN_12G;
					map[yy][156] = TOKEN_12G;
					break;
				case 20:
				case 21:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_20G;
					}
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][40] = TOKEN_25G;
					map[yy][60] = TOKEN_25G;
					map[yy][80] = TOKEN_25G;
					map[yy][100] = TOKEN_25G;
					map[yy][120] = TOKEN_25G;
					map[yy][140] = TOKEN_25G;
					map[yy][160] = TOKEN_25G;
					map[yy][180] = TOKEN_25G;
					break;
				case 40:
				case 41:
				case 42:
				/*	for (i=0; i<len; i+=(VECTOR_QUANTA_S*3)) {
						map[yy][i] = TOKEN_40G;
					}*/			
		      map[yy][0] = TOKEN_40G;
		      map[yy][12] = TOKEN_40G;
		      map[yy][25] = TOKEN_40G;
		      map[yy][37] = TOKEN_40G;
		      map[yy][50] = TOKEN_40G;
		      map[yy][62] = TOKEN_40G;
		      map[yy][75] = TOKEN_40G;
		      map[yy][87] = TOKEN_40G;
		      map[yy][100] = TOKEN_40G;
		      map[yy][112] = TOKEN_40G;
		      map[yy][125] = TOKEN_40G;
		      map[yy][137] = TOKEN_40G;
		      map[yy][150] = TOKEN_40G;
		      map[yy][162] = TOKEN_40G;
		      map[yy][175] = TOKEN_40G;
		      map[yy][187] = TOKEN_40G;
					break;
				case 100:
				case 106:
				case 107:
			/*for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_100G;
						map[yy][i+5] = TOKEN_100G;
						map[yy][i+10] = TOKEN_100G;
						map[yy][i+15] = TOKEN_100G;
						map[yy][i+19] = TOKEN_100G;
					}
					break;*/
						n=0;
						for (i=0; i<len; i+=(len/40)) {
							map[yy][i] = TOKEN_100G;
							if ((++n)==40) {
								break;
							}
						}
          break;
				case 108:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_100G;
					}
					break;
				case 109:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_100G;
					}
					for (i=(VECTOR_QUANTA_S*5); i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = num_ext_ports;
					}
					break;				
				case 120:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_120G;
					}
					break;
				case 150: /* for ancl slots*/
						map[yy][0] = MN_ANCL_TOKEN;
						map[yy][25] = MN_ANCL_TOKEN;
						map[yy][50] = MN_ANCL_TOKEN;
						map[yy][75] = MN_ANCL_TOKEN;
						map[yy][100] = MN_ANCL_TOKEN;
						map[yy][150] = MN_ANCL_TOKEN;
						map[yy][175] = MN_ANCL_TOKEN;						
						map[yy][199] = MN_ANCL_TOKEN;						
						break;
				default:
					return 0;
					break;
			}
			break;			
		case 575:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						map[yy][0] = TOKEN_10G;
						map[yy][36] = TOKEN_10G;
						map[yy][72] = TOKEN_10G;
						map[yy][108] = TOKEN_10G;
						break;
					case 12:
						n=0;
						for (i=0; i<len; i+=(len/5)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==5) {
								break;
							}	
						}
						break;
					case 20:
					case 21:
						map[yy][0] = TOKEN_20G;
						map[yy][36] = TOKEN_20G;
						map[yy][72] = TOKEN_20G;
						map[yy][109] = TOKEN_20G;
						map[yy][18] = TOKEN_20G;
						map[yy][54] = TOKEN_20G;
						map[yy][90] = TOKEN_20G;
						map[yy][127] = TOKEN_20G;
						break;					
					case 40:
					case 41:
					case 42:
					/*n=0;
						for (i=0; i<len; i+=(len/16)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==16) {
								break;
							}	
						}*/
						map[yy][0] = TOKEN_40G; /*use 9 and 10*/
						map[yy][9] = TOKEN_40G;
						map[yy][18] = TOKEN_40G;
						map[yy][27] = TOKEN_40G;
						map[yy][36] = TOKEN_40G;
						map[yy][45] = TOKEN_40G;
						map[yy][54] = TOKEN_40G;
						map[yy][63] = TOKEN_40G;
						map[yy][72] = TOKEN_40G;
						map[yy][81] = TOKEN_40G;
						map[yy][90] = TOKEN_40G;
						map[yy][99] = TOKEN_40G;
						map[yy][108] = TOKEN_40G;
						map[yy][117] = TOKEN_40G;
						map[yy][126] = TOKEN_40G;
						map[yy][135] = TOKEN_40G;
						break;
					case 100:
						n=0;
						for (i=0; i<len; i+=(len/40)) {
							map[yy][i] = TOKEN_100G;
							if ((++n)==40) {
								break;
							}
						}
						break;					
					case 120:
						n=0;
						for (i=0; i<len; i+=(len/48)) {
							map[yy][i] = TOKEN_120G;
							if ((++n)==48) {
								break;
							}
						}
						break;
					case 150: /* for ancl slots*/
						map[yy][17] = MN_ANCL_TOKEN;
						map[yy][26] = MN_ANCL_TOKEN;
						map[yy][53] = MN_ANCL_TOKEN;
						map[yy][62] = MN_ANCL_TOKEN;
						map[yy][89] = MN_ANCL_TOKEN;						
						map[yy][98] = MN_ANCL_TOKEN;						
						map[yy][125] = MN_ANCL_TOKEN;						
						map[yy][134] = MN_ANCL_TOKEN;						
						map[yy][144] = MN_ANCL_TOKEN;						
						break;
					default:
						return 0;
						break;
			}
			break;
		case 510:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						n=0;
						for (i=0; i<len; i+=(len/4)) {
							map[yy][i] = TOKEN_10G;
							if ((++n)==4) {
								break;
							}
						}
						break;
					case 12:
						n=0;
						for (i=0; i<len; i+=(len/5)) {
							map[yy][i] = TOKEN_12G;
							if ((++n)==5) {
								break;
							}
						}
						break;
					case 20:
					case 21:
						n=0;
						for (i=0; i<len; i+=(len/8)) {
							map[yy][i] = TOKEN_20G;
							if ((++n)==8) {
								break;
							}	
						}
						break;					
					case 40:
					case 41:
					case 42:
					case 43:
					case 44:
					case 45:
						n=0;
						for (i=0; i<len; i+=(len/16)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==16) {
								break;
							}
						}
						break;
					default:
						return 0;
						break;
			}
			break;
		case 435:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						map[yy][0] = TOKEN_10G;
						map[yy][27] = TOKEN_10G;
						map[yy][55] = TOKEN_10G;
						map[yy][82] = TOKEN_10G;
						break;
					case 12:
						map[yy][0] = TOKEN_12G;
						map[yy][19] = TOKEN_12G;
						map[yy][39] = TOKEN_12G;
						map[yy][58] = TOKEN_12G;
						map[yy][78] = TOKEN_12G;
						break;
					case 20:
					case 21:
						map[yy][0] = TOKEN_20G;
						map[yy][14] = TOKEN_20G;
						map[yy][28] = TOKEN_20G;
						map[yy][41] = TOKEN_20G;
						map[yy][55] = TOKEN_20G;
						map[yy][69] = TOKEN_20G;
						map[yy][82] = TOKEN_20G;
						map[yy][96] = TOKEN_20G;						
						break;
					case 40:
					case 41:
					case 42:
						map[yy][0] = TOKEN_40G; /*use 7 and 5*/
						map[yy][7] = TOKEN_40G;
						map[yy][14] = TOKEN_40G;
						map[yy][21] = TOKEN_40G;
						map[yy][28] = TOKEN_40G;
						map[yy][35] = TOKEN_40G;
						map[yy][42] = TOKEN_40G;
						map[yy][49] = TOKEN_40G;
						map[yy][56] = TOKEN_40G;
						map[yy][63] = TOKEN_40G;
						map[yy][70] = TOKEN_40G;
						map[yy][77] = TOKEN_40G;
						map[yy][82] = TOKEN_40G;
						map[yy][89] = TOKEN_40G;
						map[yy][96] = TOKEN_40G;
						map[yy][103] = TOKEN_40G;						
						break;
					case 150: /* for ancl slots*/
						map[yy][6] = MN_ANCL_TOKEN;
						map[yy][13] = MN_ANCL_TOKEN;
						map[yy][33] = MN_ANCL_TOKEN;
						map[yy][40] = MN_ANCL_TOKEN;
						map[yy][54] = MN_ANCL_TOKEN;
						map[yy][61] = MN_ANCL_TOKEN;
						map[yy][68] = MN_ANCL_TOKEN;
						map[yy][88] = MN_ANCL_TOKEN;						
						map[yy][95] = MN_ANCL_TOKEN;						
						map[yy][109] = MN_ANCL_TOKEN;						
						break;
					default:
						return 0;
						break;
			}
			break;			
		default:
			switch (port_speed)
			{
				default:
					return 0;
					break;
			}
	}
	
	return 1;
	
}
#endif /* TDM_MN_VECTOR_LIBRARY_H */
