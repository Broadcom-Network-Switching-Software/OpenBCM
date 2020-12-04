/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM vector library for BCM53570
 */

#ifndef TDM_GH2_VECTOR_LIBRARY_H
#define TDM_GH2_VECTOR_LIBRARY_H
/**
@name: tdm_gh2_vector_load
@param:

Generate vector at index in TDM vector map
 */
int
tdm_gh2_vec_load(unsigned short **map, int freq, int port_speed, int yy, int len)
{
    int i, n=0;
    int result=1; /* 1->PASS; 0->FAIL */

    switch (freq){
        case 850:
            switch(port_speed){
                /* Ethernet */
                case 10:
					for (i=0; i<len; i+=56) {
						map[yy][i]    = TOKEN_10G;
						n+=1; if (n==4) {break;}
					}
                    break;
                case 20:
					for (i=0; i<len; i+=28) {
						map[yy][i]    = TOKEN_20G;
						n+=1; if (n==8) {break;}
					}
                    break;
                case 25:
					for (i=0; i<len && (i+89)<len; i+=112) {
						map[yy][i]    = TOKEN_25G;
                        map[yy][i+22] = TOKEN_25G;
                        map[yy][i+44] = TOKEN_25G;
                        map[yy][i+67] = TOKEN_25G;
                        map[yy][i+89] = TOKEN_25G;
						n+=5; if (n==10) {break;}
					}
                    break;
                case 40:
					for (i=0; i<len; i+=14) {
						map[yy][i]    = TOKEN_40G;
						n+=1; if (n==16) {break;}
					}
                    break;
                case 41:
					for (i=0; i<len && (i+16)<len; i+=28) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+16] = TOKEN_40G;
						n+=2; if (n==16) {break;}
					}
                    break;
                case 50:
					for (i=0; i<len && (i+44)<len; i+=56) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+11] = TOKEN_50G;
                        map[yy][i+22] = TOKEN_50G;
                        map[yy][i+33] = TOKEN_50G;
                        map[yy][i+44] = TOKEN_50G;
						n+=5; if (n==20) {break;}
					}
                    break;
                case 100:
					for (i=0; i<len && (i+50)<len; i+=56) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+5]  = TOKEN_100G;
                        map[yy][i+11] = TOKEN_100G;
                        map[yy][i+16] = TOKEN_100G;
                        map[yy][i+22] = TOKEN_100G;
                        map[yy][i+28] = TOKEN_100G;
                        map[yy][i+33] = TOKEN_100G;
                        map[yy][i+39] = TOKEN_100G;
                        map[yy][i+44] = TOKEN_100G;
                        map[yy][i+50] = TOKEN_100G;
						n+=10; if (n==40) {break;}
					}
                    break;
                /* HiGig */
                case 11:
					for (i=0; i<len; i+=54) {
						map[yy][i]    = TOKEN_10G;
						n+=1; if (n==4) {break;}
					}
                    break;
                case 21:
					for (i=0; i<len; i+=27) {
						map[yy][i]    = TOKEN_20G;
						n+=1; if (n==8) {break;}
					}
                    break;
                case 27:
					for (i=0; i<len && (i+22)<len; i+=43) {
						map[yy][i]    = TOKEN_25G;
                        map[yy][i+22] = TOKEN_25G;
						n+=2; if (n==10) {break;}
					}
                    break;
                case 42:
					for (i=0; i<len && (i+13)<len; i+=27) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+13] = TOKEN_40G;
						n+=2; if (n==16) {break;}
					}
                    break;
                case 43:
					for (i=0; i<len && (i+84)<len; i+=113) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+14] = TOKEN_40G;
                        map[yy][i+27] = TOKEN_40G;
                        map[yy][i+43] = TOKEN_40G;
                        map[yy][i+57] = TOKEN_40G;
                        map[yy][i+70] = TOKEN_40G;
                        map[yy][i+84] = TOKEN_40G;
                        if (n==0 && (i+97)<len){
                            map[yy][i+97] = TOKEN_40G;
                        }
						n+=8; if (n==16) {break;}
					}
                    break;
                case 53:
					for (i=0; i<len && (i+33)<len; i+=43) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+11] = TOKEN_50G;
                        map[yy][i+22] = TOKEN_50G;
                        map[yy][i+33] = TOKEN_50G;
						n+=4; if (n==20) {break;}
					}
                    break;
                case 106:
					for (i=0; i<len && (i+38)<len; i+=43) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+5]  = TOKEN_100G;
                        map[yy][i+11] = TOKEN_100G;
                        map[yy][i+16] = TOKEN_100G;
                        map[yy][i+22] = TOKEN_100G;
                        map[yy][i+27] = TOKEN_100G;
                        map[yy][i+33] = TOKEN_100G;
                        map[yy][i+38] = TOKEN_100G;
						n+=8; if (n==40) {break;}
					}
                    break;
                case 107:
					for (i=0; i<len && (i+33)<len; i+=43) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+5]  = TOKEN_100G;
                        map[yy][i+11] = TOKEN_100G;
                        map[yy][i+16] = TOKEN_100G;
                        map[yy][i+22] = TOKEN_100G;
                        map[yy][i+27] = TOKEN_100G;
                        map[yy][i+33] = TOKEN_100G;
                        if (n<32 && (i+38)<len){
                            map[yy][i+38] = TOKEN_100G;
                        }
						n+=8; if (n==40) {break;}
					}
                    break;
                default:
                    result = 0;
                    break;
            }
            break;
        case 765:
            switch(port_speed){
                /* Ethernet */
                case 10:
					for (i=0; i<len && (i+50)<len; i+=101) {
						map[yy][i]    = TOKEN_10G;
                        map[yy][i+50] = TOKEN_10G;
						n+=2; if (n==4) {break;}
					}
                    break;
                case 20:
					for (i=0; i<len && (i+75)<len; i+=101) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+25] = TOKEN_20G;
                        map[yy][i+50] = TOKEN_20G;
                        map[yy][i+75] = TOKEN_20G;
						n+=4; if (n==8) {break;}
					}
                    break;
                case 25:
					for (i=0; i<len && (i+80)<len; i+=101) {
						map[yy][i]    = TOKEN_25G;
                        map[yy][i+20] = TOKEN_25G;
                        map[yy][i+40] = TOKEN_25G;
                        map[yy][i+60] = TOKEN_25G;
                        map[yy][i+80] = TOKEN_25G;
						n+=5; if (n==10) {break;}
					}
                    break;
                case 40:
					for (i=0; i<len && (i+88)<len; i+=101) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+13] = TOKEN_40G;
                        map[yy][i+25] = TOKEN_40G;
                        map[yy][i+38] = TOKEN_40G;
                        map[yy][i+50] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+75] = TOKEN_40G;
                        map[yy][i+88] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 41:
					for (i=0; i<len && (i+90)<len; i+=101) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+15] = TOKEN_40G;
                        map[yy][i+25] = TOKEN_40G;
                        map[yy][i+40] = TOKEN_40G;
                        map[yy][i+50] = TOKEN_40G;
                        map[yy][i+65] = TOKEN_40G;
                        map[yy][i+75] = TOKEN_40G;
                        map[yy][i+90] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 50:
					for (i=0; i<len && (i+90)<len; i+=101) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+10] = TOKEN_50G;
                        map[yy][i+20] = TOKEN_50G;
                        map[yy][i+30] = TOKEN_50G;
                        map[yy][i+40] = TOKEN_50G;
                        map[yy][i+50] = TOKEN_50G;
                        map[yy][i+60] = TOKEN_50G;
                        map[yy][i+70] = TOKEN_50G;
                        map[yy][i+80] = TOKEN_50G;
                        map[yy][i+90] = TOKEN_50G;
						n+=10; if (n==20) {break;}
					}
                    break;
                case 100:
					for (i=0; i<len && (i+95)<len; i+=101) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+5]  = TOKEN_100G;
                        map[yy][i+10] = TOKEN_100G;
                        map[yy][i+15] = TOKEN_100G;
                        map[yy][i+20] = TOKEN_100G;
                        map[yy][i+25] = TOKEN_100G;
                        map[yy][i+30] = TOKEN_100G;
                        map[yy][i+35] = TOKEN_100G;
                        map[yy][i+40] = TOKEN_100G;
                        map[yy][i+45] = TOKEN_100G;
                        map[yy][i+50] = TOKEN_100G;
                        map[yy][i+55] = TOKEN_100G;
                        map[yy][i+60] = TOKEN_100G;
                        map[yy][i+65] = TOKEN_100G;
                        map[yy][i+70] = TOKEN_100G;
                        map[yy][i+75] = TOKEN_100G;
                        map[yy][i+80] = TOKEN_100G;
                        map[yy][i+85] = TOKEN_100G;
                        map[yy][i+90] = TOKEN_100G;
                        map[yy][i+95] = TOKEN_100G;
						n+=20; if (n==40) {break;}
					}
                    break;
                /* HiGig */
                case 11:
					for (i=0; i<len && (i+48)<len; i+=97) {
						map[yy][i]    = TOKEN_10G;
                        map[yy][i+48] = TOKEN_10G;
						n+=2; if (n==4) {break;}
					}
                    break;
                case 21:
					for (i=0; i<len && (i+72)<len; i+=97) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+24] = TOKEN_20G;
                        map[yy][i+48] = TOKEN_20G;
                        map[yy][i+72] = TOKEN_20G;
						n+=4; if (n==8) {break;}
					}
                    break;
                case 27:
					for (i=0; i<len && (i+77)<len; i+=97) {
						map[yy][i]    = TOKEN_25G;
                        map[yy][i+20] = TOKEN_25G;
                        map[yy][i+39] = TOKEN_25G;
                        map[yy][i+58] = TOKEN_25G;
                        map[yy][i+77] = TOKEN_25G;
						n+=5; if (n==10) {break;}
					}
                    break;
                case 42:
					for (i=0; i<len && (i+84)<len; i+=97) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+12] = TOKEN_40G;
                        map[yy][i+24] = TOKEN_40G;
                        map[yy][i+36] = TOKEN_40G;
                        map[yy][i+48] = TOKEN_40G;
                        map[yy][i+60] = TOKEN_40G;
                        map[yy][i+72] = TOKEN_40G;
                        map[yy][i+84] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 43:
					for (i=0; i<len && (i+82)<len; i+=97) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+10] = TOKEN_40G;
                        map[yy][i+24] = TOKEN_40G;
                        map[yy][i+34] = TOKEN_40G;
                        map[yy][i+48] = TOKEN_40G;
                        map[yy][i+58] = TOKEN_40G;
                        map[yy][i+72] = TOKEN_40G;
                        map[yy][i+82] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 53:
					for (i=0; i<len && (i+87)<len; i+=97) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+10] = TOKEN_50G;
                        map[yy][i+20] = TOKEN_50G;
                        map[yy][i+29] = TOKEN_50G;
                        map[yy][i+39] = TOKEN_50G;
                        map[yy][i+48] = TOKEN_50G;
                        map[yy][i+58] = TOKEN_50G;
                        map[yy][i+68] = TOKEN_50G;
                        map[yy][i+77] = TOKEN_50G;
                        map[yy][i+87] = TOKEN_50G;
						n+=10; if (n==20) {break;}
					}
                    break;
                case 106:
					for (i=0; i<len && (i+92)<len; i+=97) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+5]  = TOKEN_100G;
                        map[yy][i+10] = TOKEN_100G;
                        map[yy][i+15] = TOKEN_100G;
                        map[yy][i+20] = TOKEN_100G;
                        map[yy][i+24] = TOKEN_100G;
                        map[yy][i+29] = TOKEN_100G;
                        map[yy][i+34] = TOKEN_100G;
                        map[yy][i+39] = TOKEN_100G;
                        map[yy][i+44] = TOKEN_100G;
                        map[yy][i+48] = TOKEN_100G;
                        map[yy][i+53] = TOKEN_100G;
                        map[yy][i+58] = TOKEN_100G;
                        map[yy][i+63] = TOKEN_100G;
                        map[yy][i+68] = TOKEN_100G;
                        map[yy][i+72] = TOKEN_100G;
                        map[yy][i+77] = TOKEN_100G;
                        map[yy][i+82] = TOKEN_100G;
                        map[yy][i+87] = TOKEN_100G;
                        map[yy][i+92] = TOKEN_100G;
						n+=20; if (n==40) {break;}
					}
                    break;
                default:
                    result = 0;
                    break;
            }
            break;
        case 672:
            switch(port_speed){
                /* Ethernet */
                case 10:
					for (i=0; i<len; i+=44) {
						map[yy][i]    = TOKEN_10G;
						n+=1; if (n==4) {break;}
					}
                    break;
                case 20:
					for (i=0; i<len; i+=21) {
						map[yy][i]    = TOKEN_20G;
						n+=1; if (n==8) {break;}
					}
                    break;
                case 25:
					for (i=0; i<len && (i+71)<len; i+=88) {
						map[yy][i]    = TOKEN_25G;
                        map[yy][i+18] = TOKEN_25G;
                        map[yy][i+36] = TOKEN_25G;
                        map[yy][i+53] = TOKEN_25G;
                        map[yy][i+71] = TOKEN_25G;
						n+=5; if (n==10) {break;}
					}
                    break;
                case 40:
					for (i=0; i<len; i+=11) {
						map[yy][i]    = TOKEN_40G;
						n+=1; if (n==16) {break;}
					}
                    break;
                case 41:
					for (i=0; i<len && (i+13)<len; i+=22) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+13] = TOKEN_40G;
						n+=2; if (n==16) {break;}
					}
                    break;
                case 50:
					for (i=0; i<len && (i+36)<len; i+=44) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+9]  = TOKEN_50G;
                        map[yy][i+18] = TOKEN_50G;
                        map[yy][i+27] = TOKEN_50G;
                        map[yy][i+36] = TOKEN_50G;
						n+=5; if (n==20) {break;}
					}
                    break;
                case 100:
					for (i=0; i<len && (i+40)<len; i+=44) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+4]  = TOKEN_100G;
                        map[yy][i+9]  = TOKEN_100G;
                        map[yy][i+13] = TOKEN_100G;
                        map[yy][i+18] = TOKEN_100G;
                        map[yy][i+22] = TOKEN_100G;
                        map[yy][i+27] = TOKEN_100G;
                        map[yy][i+31] = TOKEN_100G;
                        map[yy][i+36] = TOKEN_100G;
                        map[yy][i+40] = TOKEN_100G;
						n+=10; if (n==40) {break;}
					}
                    break;
                /* HiGig */
                case 11:
					for (i=0; i<len && (i+42)<len; i+=85) {
						map[yy][i]    = TOKEN_10G;
                        map[yy][i+42] = TOKEN_10G;
						n+=2; if (n==4) {break;}
					}
                    break;
                case 21:
					for (i=0; i<len && (i+63)<len; i+=85) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+21] = TOKEN_20G;
                        map[yy][i+42] = TOKEN_20G;
                        map[yy][i+63] = TOKEN_20G;
						n+=4; if (n==8) {break;}
					}
                    break;
                case 27:
					for (i=0; i<len; i+=17) {
						map[yy][i]    = TOKEN_25G;
						n+=1; if (n==10) {break;}
					}
                    break;
                case 42:
					for (i=0; i<len && (i+74)<len; i+=85) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+10] = TOKEN_40G;
                        map[yy][i+21] = TOKEN_40G;
                        map[yy][i+31] = TOKEN_40G;
                        map[yy][i+42] = TOKEN_40G;
                        map[yy][i+52] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+74] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 43:
					for (i=0; i<len && (i+72)<len; i+=85) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+9] = TOKEN_40G;
                        map[yy][i+21] = TOKEN_40G;
                        map[yy][i+30] = TOKEN_40G;
                        map[yy][i+42] = TOKEN_40G;
                        map[yy][i+51] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+72] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 53:
					for (i=0; i<len && (i+8)<len; i+=17) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+8] = TOKEN_50G;
						n+=2; if (n==20) {break;}
					}
                    break;
                case 106:
					for (i=0; i<len && (i+12)<len; i+=17) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+4]  = TOKEN_100G;
                        map[yy][i+8]  = TOKEN_100G;
                        map[yy][i+12] = TOKEN_100G;
						n+=4; if (n==40) {break;}
					}
                    break;
                default:
                    result = 0;
                    break;
            }
            break;
        case 585:
            switch(port_speed){
                /* Ethernet */
                case 10:
					for (i=0; i<len && (i+42)<len; i+=85) {
						map[yy][i]    = TOKEN_10G;
                        map[yy][i+42] = TOKEN_10G;
						n+=2; if (n==4) {break;}
					}
                    break;
                case 20:
					for (i=0; i<len && (i+63)<len; i+=85) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+21] = TOKEN_20G;
                        map[yy][i+42] = TOKEN_20G;
                        map[yy][i+63] = TOKEN_20G;
						n+=4; if (n==8) {break;}
					}
                    break;
                case 25:
					for (i=0; i<len; i+=17) {
						map[yy][i]    = TOKEN_25G;
						n+=1; if (n==10) {break;}
					}
                    break;
                case 40:
					for (i=0; i<len && (i+73)<len; i+=85) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+10] = TOKEN_40G;
                        map[yy][i+21] = TOKEN_40G;
                        map[yy][i+31] = TOKEN_40G;
                        map[yy][i+42] = TOKEN_40G;
                        map[yy][i+52] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+73] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 41:
					for (i=0; i<len && (i+72)<len; i+=85) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+9] = TOKEN_40G;
                        map[yy][i+21] = TOKEN_40G;
                        map[yy][i+30] = TOKEN_40G;
                        map[yy][i+42] = TOKEN_40G;
                        map[yy][i+51] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+72] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 50:
					for (i=0; i<len && (i+8)<len; i+=17) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+8]  = TOKEN_50G;
						n+=2; if (n==20) {break;}
					}
                    break;
                case 100:
					for (i=0; i<len && (i+12)<len; i+=17) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+4]  = TOKEN_100G;
                        map[yy][i+8]  = TOKEN_100G;
                        map[yy][i+12] = TOKEN_100G;
						n+=4; if (n==40) {break;}
					}
                    break;
                /* HiGig */
                case 11:
					for (i=0; i<len && (i+42)<len; i+=85) {
						map[yy][i]    = TOKEN_10G;
                        map[yy][i+42] = TOKEN_10G;
						n+=2; if (n==4) {break;}
					}
                    break;
                case 21:
					for (i=0; i<len && (i+63)<len; i+=85) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+21] = TOKEN_20G;
                        map[yy][i+42] = TOKEN_20G;
                        map[yy][i+63] = TOKEN_20G;
						n+=4; if (n==8) {break;}
					}
                    break;
                case 27:
					for (i=0; i<len; i+=17) {
						map[yy][i]    = TOKEN_25G;
						n+=1; if (n==10) {break;}
					}
                    break;
                case 42:
					for (i=0; i<len && (i+74)<len; i+=85) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+10] = TOKEN_40G;
                        map[yy][i+21] = TOKEN_40G;
                        map[yy][i+31] = TOKEN_40G;
                        map[yy][i+42] = TOKEN_40G;
                        map[yy][i+52] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+74] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 43:
					for (i=0; i<len && (i+72)<len; i+=85) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+9] = TOKEN_40G;
                        map[yy][i+21] = TOKEN_40G;
                        map[yy][i+30] = TOKEN_40G;
                        map[yy][i+42] = TOKEN_40G;
                        map[yy][i+51] = TOKEN_40G;
                        map[yy][i+63] = TOKEN_40G;
                        map[yy][i+72] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                case 53:
					for (i=0; i<len && (i+8)<len; i+=17) {
						map[yy][i]    = TOKEN_50G;
                        map[yy][i+8] = TOKEN_50G;
						n+=2; if (n==20) {break;}
					}
                    break;
                case 106:
					for (i=0; i<len && (i+12)<len; i+=17) {
						map[yy][i]    = TOKEN_100G;
                        map[yy][i+4]  = TOKEN_100G;
                        map[yy][i+8]  = TOKEN_100G;
                        map[yy][i+12] = TOKEN_100G;
						n+=4; if (n==40) {break;}
					}
                    break;
                default:
                    result = 0;
                    break;
            }
            break;
        case 545:
            switch(port_speed){
                /* Ethernet */
                case 10:
					for (i=0; i<len; i+=36) {
						map[yy][i]    = TOKEN_10G;
						n+=1; if (n==4) {break;}
					}
                    break;
                case 20:
					for (i=0; i<len && (i+17)<len; i+=36) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+17] = TOKEN_20G;
						n+=2; if (n==8) {break;}
					}
                    break;
                case 40:
					for (i=0; i<len && (i+25)<len; i+=36) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+8]  = TOKEN_40G;
                        map[yy][i+17] = TOKEN_40G;
                        map[yy][i+25] = TOKEN_40G;
						n+=4; if (n==16) {break;}
					}
                    break;
                /* HiGig */
                case 11:
					for (i=0; i<len && (i+34)<len; i+=69) {
						map[yy][i]    = TOKEN_10G;
                        map[yy][i+34] = TOKEN_10G;
						n+=2; if (n==4) {break;}
					}
                    break;
                case 21:
					for (i=0; i<len && (i+51)<len; i+=69) {
						map[yy][i]    = TOKEN_20G;
                        map[yy][i+17] = TOKEN_20G;
                        map[yy][i+34] = TOKEN_20G;
                        map[yy][i+51] = TOKEN_20G;
						n+=4; if (n==8) {break;}
					}
                    break;
                case 42:
					for (i=0; i<len && (i+60)<len; i+=69) {
						map[yy][i]    = TOKEN_40G;
                        map[yy][i+9]  = TOKEN_40G;
                        map[yy][i+17] = TOKEN_40G;
                        map[yy][i+26] = TOKEN_40G;
                        map[yy][i+34] = TOKEN_40G;
                        map[yy][i+43] = TOKEN_40G;
                        map[yy][i+51] = TOKEN_40G;
                        map[yy][i+60] = TOKEN_40G;
						n+=8; if (n==16) {break;}
					}
                    break;
                default:
                    result = 0;
                    break;
            }
            break;
        default:
            result = 0;
            break;
    }
    return result;
}
#endif /* TDM_GH2_VECTOR_LIBRARY_H */
