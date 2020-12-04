/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM vector library for BCM56960
 */

#ifndef TDM_TH_VECTOR_LIBRARY_H
#define TDM_TH_VECTOR_LIBRARY_H
/**
@name: tdm_th_vector_load
@param:

Generate vector at index in TDM vector map
 */
int
tdm_th_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports)
{
	int i, j=0, n=0;
	
	switch (bw)
	{
		case 850: /* TH */
			switch (port_speed)
			{
				case 10:
					map[yy][0] = TOKEN_10G; 
					map[yy][53] = TOKEN_10G; 
					map[yy][107] = TOKEN_10G; 
					map[yy][160] = TOKEN_10G;
					break;
				case 11:
					map[yy][0] = TOKEN_10G; 
					map[yy][51] = TOKEN_10G; 
					map[yy][102] = TOKEN_10G;
					map[yy][153] = TOKEN_10G;
					break;
				case 20:
					map[yy][0] = TOKEN_20G; 
					map[yy][26] = TOKEN_20G; 
					map[yy][53] = TOKEN_20G; 
					map[yy][80] = TOKEN_20G;
					map[yy][107] = TOKEN_20G; 
					map[yy][133] = TOKEN_20G; 
					map[yy][160] = TOKEN_20G; 
					map[yy][187] = TOKEN_20G;
					break;
				case 21:
					map[yy][0] = TOKEN_20G; 
					map[yy][25] = TOKEN_20G; 
					map[yy][51] = TOKEN_20G; 
					map[yy][76] = TOKEN_20G;
					map[yy][102] = TOKEN_20G; 
					map[yy][127] = TOKEN_20G; 
					map[yy][153] = TOKEN_20G; 
					map[yy][179] = TOKEN_20G;
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][42] = TOKEN_25G;
					map[yy][64] = TOKEN_25G;
					map[yy][86] = TOKEN_25G;
					map[yy][107] = TOKEN_25G;
					map[yy][127] = TOKEN_25G;
					map[yy][149] = TOKEN_25G;
					map[yy][171] = TOKEN_25G;
					map[yy][193] = TOKEN_25G;
					break;
				case 27:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][41] = TOKEN_25G;
					map[yy][61] = TOKEN_25G;
					map[yy][82] = TOKEN_25G;
					map[yy][102] = TOKEN_25G;
					map[yy][122] = TOKEN_25G;
					map[yy][143] = TOKEN_25G;
					map[yy][164] = TOKEN_25G;
					map[yy][185] = TOKEN_25G;
					break;
				case 40:
					map[yy][0] = TOKEN_40G;
					map[yy][13] = TOKEN_40G;
					map[yy][26] = TOKEN_40G;
					map[yy][39] = TOKEN_40G;
					map[yy][53] = TOKEN_40G;
					map[yy][66] = TOKEN_40G;
					map[yy][80] = TOKEN_40G;
					map[yy][93] = TOKEN_40G;
					map[yy][107] = TOKEN_40G;
					map[yy][120] = TOKEN_40G;
					map[yy][133] = TOKEN_40G;
					map[yy][146] = TOKEN_40G;
					map[yy][160] = TOKEN_40G;
					map[yy][173] = TOKEN_40G;
					map[yy][187] = TOKEN_40G;
					map[yy][200] = TOKEN_40G;
					break;
				case 42:
					map[yy][0] = TOKEN_40G;
					map[yy][12] = TOKEN_40G;
					map[yy][25] = TOKEN_40G;
					map[yy][38] = TOKEN_40G;
					map[yy][51] = TOKEN_40G;
					map[yy][63] = TOKEN_40G;
					map[yy][76] = TOKEN_40G;
					map[yy][89] = TOKEN_40G;
					map[yy][102] = TOKEN_40G;
					map[yy][114] = TOKEN_40G;
					map[yy][127] = TOKEN_40G;
					map[yy][140] = TOKEN_40G;
					map[yy][153] = TOKEN_40G;
					map[yy][166] = TOKEN_40G;
					map[yy][179] = TOKEN_40G;
					map[yy][192] = TOKEN_40G;
					break;
				case 43:
					map[yy][4] = TOKEN_40G;
					map[yy][11] = TOKEN_40G;
					map[yy][18] = TOKEN_40G;
					map[yy][25] = TOKEN_40G;
					map[yy][40] = TOKEN_40G;
					map[yy][55] = TOKEN_40G;
					map[yy][70] = TOKEN_40G;
					map[yy][85] = TOKEN_40G;
					map[yy][100] = TOKEN_40G;
					map[yy][115] = TOKEN_40G;
					map[yy][130] = TOKEN_40G;
					map[yy][145] = TOKEN_40G;
					map[yy][160] = TOKEN_40G;
					map[yy][175] = TOKEN_40G;
					map[yy][190] = TOKEN_40G;
					break;
				case 44:
					map[yy][5] = TOKEN_40G;
					map[yy][12] = TOKEN_40G;
					map[yy][19] = TOKEN_40G;
					map[yy][30] = TOKEN_40G;
					map[yy][45] = TOKEN_40G;
					map[yy][60] = TOKEN_40G;
					map[yy][75] = TOKEN_40G;
					map[yy][90] = TOKEN_40G;
					map[yy][105] = TOKEN_40G;
					map[yy][120] = TOKEN_40G;
					map[yy][135] = TOKEN_40G;
					map[yy][150] = TOKEN_40G;
					map[yy][165] = TOKEN_40G;
					map[yy][180] = TOKEN_40G;
					map[yy][195] = TOKEN_40G;
					break;
				case 45:
					map[yy][6] = TOKEN_40G;
					map[yy][13] = TOKEN_40G;
					map[yy][20] = TOKEN_40G;
					map[yy][35] = TOKEN_40G;
					map[yy][50] = TOKEN_40G;
					map[yy][65] = TOKEN_40G;
					map[yy][80] = TOKEN_40G;
					map[yy][95] = TOKEN_40G;
					map[yy][110] = TOKEN_40G;
					map[yy][125] = TOKEN_40G;
					map[yy][140] = TOKEN_40G;
					map[yy][155] = TOKEN_40G;
					map[yy][170] = TOKEN_40G;
					map[yy][185] = TOKEN_40G;
					map[yy][200] = TOKEN_40G;
					break;
				case 50:
					map[yy][0] = TOKEN_50G;
					map[yy][10] = TOKEN_50G;
					map[yy][20] = TOKEN_50G;
					map[yy][32] = TOKEN_50G;
					map[yy][42] = TOKEN_50G;
					map[yy][53] = TOKEN_50G;
					map[yy][64] = TOKEN_50G;
					map[yy][74] = TOKEN_50G;
					map[yy][86] = TOKEN_50G;
					map[yy][96] = TOKEN_50G;
					map[yy][107] = TOKEN_50G;
					map[yy][117] = TOKEN_50G;
					map[yy][127] = TOKEN_50G;
					map[yy][139] = TOKEN_50G;
					map[yy][149] = TOKEN_50G;
					map[yy][160] = TOKEN_50G;					
					map[yy][171] = TOKEN_50G;
					map[yy][181] = TOKEN_50G;
					map[yy][193] = TOKEN_50G;
					map[yy][203] = TOKEN_50G;
					break;
				case 53:
					map[yy][0] = TOKEN_50G;
					map[yy][10] = TOKEN_50G;
					map[yy][20] = TOKEN_50G;
					map[yy][31] = TOKEN_50G;
					map[yy][41] = TOKEN_50G;
					map[yy][51] = TOKEN_50G;
					map[yy][61] = TOKEN_50G;
					map[yy][71] = TOKEN_50G;
					map[yy][82] = TOKEN_50G;
					map[yy][92] = TOKEN_50G;
					map[yy][102] = TOKEN_50G;
					map[yy][112] = TOKEN_50G;
					map[yy][122] = TOKEN_50G;
					map[yy][133] = TOKEN_50G;
					map[yy][143] = TOKEN_50G;
					map[yy][153] = TOKEN_50G;					
					map[yy][164] = TOKEN_50G;
					map[yy][174] = TOKEN_50G;
					map[yy][185] = TOKEN_50G;
					map[yy][195] = TOKEN_50G;
					break;					
				case 100:
					for (i=0; i<len; i+=VECTOR_QUANTA_F) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( (j==4)||(j==5)||(j==9)||(j==10)||(j==14)||(j==15)||(j==19)||(j==24)||(j==25)||(j==29)||(j==30)||(j==34)||(j==35) ) {
							i++;
						}
						j++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}				
					break;
				case 106:
					for (i=0; i<len; i+=VECTOR_QUANTA_F) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( (j==5)||(j==15)||(j==25)||(j==30)||(j==35) ) {
							i++;
						}
						j++;
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 107:
					for (i=0; i<len; i+=VECTOR_QUANTA_F) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( (j==0)||(j==1)||(j==2) ) {
							i+=2;
						}
						j++;
						if (n==39) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
		case 766:
		case 765: /* TH */
			switch (port_speed)
			{
				case 10:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*12)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;			
				case 11:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*9)+1)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;					
				case 21:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*4)+3)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((20*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][39] = TOKEN_25G;
					map[yy][58] = TOKEN_25G;
					map[yy][77] = TOKEN_25G;
					map[yy][96] = TOKEN_25G;
					map[yy][116] = TOKEN_25G;
					map[yy][135] = TOKEN_25G;
					map[yy][154] = TOKEN_25G;
					map[yy][173] = TOKEN_25G;
					break;				
				case 27:
					for (i=0; i<len; i+=((VECTOR_QUANTA_S*4)+2)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if ( (j==1)||(j==2)||(j==6)||(j==7) ) {
							i++;
						}
						j++;						
						if (n==((25*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*3)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;					
				case 42:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*2)+1)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if ((j%2)==0) {
							i++; j=0;
						}
						j++;						
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 50:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*2)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if ( (j==2)||(j==4)||(j==7)||(j==9)||(j==12)||(j==14)||(j==17) ) {
							i--;
						}
						j++;						
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 53:
					map[yy][0] = TOKEN_50G;
					map[yy][10] = TOKEN_50G;
					map[yy][19] = TOKEN_50G;
					map[yy][28] = TOKEN_50G;
					map[yy][38] = TOKEN_50G;
					map[yy][46] = TOKEN_50G;
					map[yy][56] = TOKEN_50G;
					map[yy][65] = TOKEN_50G;
					map[yy][74] = TOKEN_50G;
					map[yy][84] = TOKEN_50G;
					map[yy][92] = TOKEN_50G;
					map[yy][102] = TOKEN_50G;
					map[yy][111] = TOKEN_50G;
					map[yy][120] = TOKEN_50G;
					map[yy][130] = TOKEN_50G;
					map[yy][138] = TOKEN_50G;
					map[yy][148] = TOKEN_50G;
					map[yy][157] = TOKEN_50G;
					map[yy][166] = TOKEN_50G;
					map[yy][176] = TOKEN_50G;
					break;
				case 100:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F)) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ((((j+1)%5)==0)&&(j<40)) {
							i--;
						}
						j++;						
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;				
				case 106:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++; j++;
						if (j==1||j==2||j==3) {
							i++;
						}
						if (j==5) {
							j=0;
						}
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
		case 672: /* TH */
			switch (port_speed)
			{
				case 100:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( ((j+1)%5==0)&&(j<40)) {
							i++;
						}
						j++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 106:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 50:
					map[yy][0] = TOKEN_50G;
					map[yy][8] = TOKEN_50G;
					map[yy][16] = TOKEN_50G;
					map[yy][25] = TOKEN_50G;
					map[yy][33] = TOKEN_50G;
					map[yy][42] = TOKEN_50G;
					map[yy][50] = TOKEN_50G;
					map[yy][58] = TOKEN_50G;
					map[yy][67] = TOKEN_50G;
					map[yy][75] = TOKEN_50G;
					map[yy][84] = TOKEN_50G;
					map[yy][92] = TOKEN_50G;
					map[yy][100] = TOKEN_50G;
					map[yy][109] = TOKEN_50G;
					map[yy][117] = TOKEN_50G;
					map[yy][126] = TOKEN_50G;
					map[yy][134] = TOKEN_50G;
					map[yy][142] = TOKEN_50G;
					map[yy][151] = TOKEN_50G;
					map[yy][159] = TOKEN_50G;
					break;
				case 53:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*2)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((50*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					map[yy][0] = TOKEN_40G;
					map[yy][10] = TOKEN_40G;
					map[yy][21] = TOKEN_40G;
					map[yy][31] = TOKEN_40G;
					map[yy][42] = TOKEN_40G;
					map[yy][52] = TOKEN_40G;
					map[yy][63] = TOKEN_40G;
					map[yy][73] = TOKEN_40G;
					map[yy][84] = TOKEN_40G;
					map[yy][94] = TOKEN_40G;
					map[yy][105] = TOKEN_40G;
					map[yy][115] = TOKEN_40G;
					map[yy][126] = TOKEN_40G;
					map[yy][136] = TOKEN_40G;
					map[yy][147] = TOKEN_40G;
					map[yy][157] = TOKEN_40G;
					break;
				case 42:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*2)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((40*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][16] = TOKEN_25G;
					map[yy][33] = TOKEN_25G;
					map[yy][50] = TOKEN_25G;
					map[yy][67] = TOKEN_25G;
					map[yy][84] = TOKEN_25G;
					map[yy][100] = TOKEN_25G;
					map[yy][117] = TOKEN_25G;
					map[yy][134] = TOKEN_25G;
					map[yy][151] = TOKEN_25G;
					break;
				case 27:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*4)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((25*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					map[yy][0] = TOKEN_20G;
					map[yy][21] = TOKEN_20G;
					map[yy][42] = TOKEN_20G;
					map[yy][63] = TOKEN_20G;
					map[yy][84] = TOKEN_20G;
					map[yy][105] = TOKEN_20G;
					map[yy][126] = TOKEN_20G;
					map[yy][147] = TOKEN_20G;
					break;					
				case 21:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*4)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((20*10)/BW_QUANTA)) {
							break;
						}
					}
					break;	
				case 10:
					map[yy][0] = TOKEN_10G;
					map[yy][42] = TOKEN_10G;
					map[yy][84] = TOKEN_10G;
					map[yy][126] = TOKEN_10G;
					break;						
				case 11:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*10)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
		case 645: /* TH */
			switch (port_speed)
			{
				case 100:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;	
				case 50:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*2)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 42:
					map[yy][0] = TOKEN_40G;
					map[yy][9] = TOKEN_40G;
					map[yy][19] = TOKEN_40G;
					map[yy][28] = TOKEN_40G;
					map[yy][38] = TOKEN_40G;
					map[yy][47] = TOKEN_40G;
					map[yy][57] = TOKEN_40G;
					map[yy][66] = TOKEN_40G;
					map[yy][76] = TOKEN_40G;
					map[yy][85] = TOKEN_40G;
					map[yy][95] = TOKEN_40G;
					map[yy][104] = TOKEN_40G;
					map[yy][114] = TOKEN_40G;
					map[yy][124] = TOKEN_40G;
					map[yy][134] = TOKEN_40G;
					map[yy][144] = TOKEN_40G;
					break;					
				case 40:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*2)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*4)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 21:
					map[yy][0] = TOKEN_20G;
					map[yy][19] = TOKEN_20G;
					map[yy][38] = TOKEN_20G;
					map[yy][57] = TOKEN_20G;
					map[yy][76] = TOKEN_20G;
					map[yy][95] = TOKEN_20G;
					map[yy][114] = TOKEN_20G;
					map[yy][134] = TOKEN_20G;
					break;						
				case 20:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*4)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 11:
					map[yy][0] = TOKEN_10G;
					map[yy][38] = TOKEN_10G;
					map[yy][76] = TOKEN_10G;
					map[yy][114] = TOKEN_10G;
					break;					
				case 10:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*10)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;					
			}
			break;
		case 545: /* TH */
			switch (port_speed)
			{
				case 10:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*6)+3)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;			
				case 11:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*6)+2)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					map[yy][0] = TOKEN_20G;
					map[yy][16] = TOKEN_20G;
					map[yy][33] = TOKEN_20G;
					map[yy][49] = TOKEN_20G;
					map[yy][66] = TOKEN_20G;
					map[yy][82] = TOKEN_20G;
					map[yy][99] = TOKEN_20G;
					map[yy][116] = TOKEN_20G;
					break;						
				case 21:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*3)+1)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					map[yy][0] = TOKEN_40G;
					map[yy][8] = TOKEN_40G;
					map[yy][16] = TOKEN_40G;
					map[yy][24] = TOKEN_40G;
					map[yy][33] = TOKEN_40G;
					map[yy][41] = TOKEN_40G;
					map[yy][49] = TOKEN_40G;
					map[yy][57] = TOKEN_40G;
					map[yy][66] = TOKEN_40G;
					map[yy][74] = TOKEN_40G;
					map[yy][82] = TOKEN_40G;
					map[yy][90] = TOKEN_40G;
					map[yy][99] = TOKEN_40G;
					map[yy][108] = TOKEN_40G;
					map[yy][116] = TOKEN_40G;
					map[yy][125] = TOKEN_40G;
					break;						
				case 42:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F)+3)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
/* ################################################################################################################## */
		case 760: /* TD2/+ */
			switch (port_speed)
			{
				case 1:
					map[yy][0] = TOKEN_1G;
					break;
				case 10:
				case 11:
					map[yy][0] = TOKEN_10G;
					map[yy][48] = TOKEN_10G;
					map[yy][96] = TOKEN_10G;
					map[yy][144] = TOKEN_10G;
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
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*3)) {
						map[yy][i] = TOKEN_40G;
					}					
					break;
				case 100:
				case 106:
				case 107:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_100G;
						map[yy][i+5] = TOKEN_100G;
						map[yy][i+10] = TOKEN_100G;
						map[yy][i+15] = TOKEN_100G;
						map[yy][i+19] = TOKEN_100G;
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
				default:
					return 0;
					break;
			}
			break;			
		case 609: /* TD2/+ */
		case 608:
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
						map[yy][108] = TOKEN_20G;
						map[yy][18] = TOKEN_20G;
						map[yy][54] = TOKEN_20G;
						map[yy][90] = TOKEN_20G;
						map[yy][126] = TOKEN_20G;
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
					default:
						return 0;
						break;
			}
			break;
		case 518: /* TD2/+ */
		case 517:
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
		case 415: /* TD2/+ */
		case 416:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						map[yy][0] = TOKEN_10G;
						map[yy][24] = TOKEN_10G;
						map[yy][48] = TOKEN_10G;
						map[yy][72] = TOKEN_10G;
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
						map[yy][12] = TOKEN_20G;
						map[yy][24] = TOKEN_20G;
						map[yy][36] = TOKEN_20G;
						map[yy][48] = TOKEN_20G;
						map[yy][60] = TOKEN_20G;
						map[yy][72] = TOKEN_20G;
						map[yy][84] = TOKEN_20G;						
						break;					
					case 40:
					case 41:
					case 42:
					case 43:
					case 44:
					case 45:
						map[yy][0] = TOKEN_40G;
						map[yy][6] = TOKEN_40G;
						map[yy][12] = TOKEN_40G;
						map[yy][18] = TOKEN_40G;
						map[yy][24] = TOKEN_40G;
						map[yy][30] = TOKEN_40G;
						map[yy][36] = TOKEN_40G;
						map[yy][42] = TOKEN_40G;
						map[yy][48] = TOKEN_40G;
						map[yy][54] = TOKEN_40G;
						map[yy][60] = TOKEN_40G;
						map[yy][66] = TOKEN_40G;
						map[yy][72] = TOKEN_40G;
						map[yy][78] = TOKEN_40G;
						map[yy][84] = TOKEN_40G;
						map[yy][90] = TOKEN_40G;						
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
#endif /* TDM_TH_VECTOR_LIBRARY_H */
