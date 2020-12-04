/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM vector library for BCM56970
 */

#ifndef TDM_TH2_VECTOR_LIBRARY_H
#define TDM_TH2_VECTOR_LIBRARY_H
/**
@name: tdm_th2_vector_load
@param:

Generate vector at index in TDM vector map
 */
int
tdm_th2_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports)
{
	int i, n=0;
	
	switch (bw) {
		case 1700:
		case 1275:
		case 1133:
		case 850:
			switch (port_speed) {
				case 100:
					for (i=0; i<len; i+=(len/40)) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 106:
					for (i=0; i<len; i+=(len/40)) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 50:
					for (i=0; i<len; i+=(len/20)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 53:
					for (i=0; i<len; i+=(len/20)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((50*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					for (i=0; i<len; i+=(len/16)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 42:
					for (i=0; i<len; i+=(len/16)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((40*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					for (i=0; i<len; i+=(len/10)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 27:
					for (i=0; i<len; i+=(len/10)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((25*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					for (i=0; i<len; i+=(len/8)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 21:
					for (i=0; i<len; i+=(len/8)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((20*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 10:
					for (i=0; i<len; i+=(len/4)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 11:
					for (i=0; i<len; i+=(len/4)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 1:
					map[yy][0] = TOKEN_1G;
					break;
			}
			break;
		default:
			switch (port_speed) {
				default:
					return 0;
					break;
			}
	}
	
	return 1;
	
}
#endif /* TDM_TH2_VECTOR_LIBRARY_H */
