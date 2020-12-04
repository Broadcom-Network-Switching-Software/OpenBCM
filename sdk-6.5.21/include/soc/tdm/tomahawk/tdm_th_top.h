/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56960
 */

#ifndef TDM_TH_PREPROCESSOR_DIRECTIVES_H
#define TDM_TH_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th_defines.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_defines.h>
#endif

#define TH_TOKEN_CHECK(a)  				\
			if (a!=TH_NUM_EXT_PORTS && 	\
				a!=TH_OVSB_TOKEN && 	\
				a!=TH_NULL_TOKEN &&		\
				a!=TH_RSVD_TOKEN &&		\
				a!=TH_MGM1_TOKEN &&		\
				a!=TH_LPB1_TOKEN && 	\
				a!=TH_MGM2_TOKEN && 	\
				a!=TH_LPB2_TOKEN && 	\
				a!=TH_LPB3_TOKEN && 	\
				a!=TH_CMIC_TOKEN && 	\
				a!=TH_ANCL_TOKEN &&		\
				a!=TH_IDL1_TOKEN &&		\
				a!=TH_IDL2_TOKEN &&		\
				a!=TH_LPB0_TOKEN) 		\

#define TH_CMIC(a,b) {								\
			b[a]=TH_CMIC_TOKEN; 					\
			TDM_PRINT0("CMIC/CPU\n");				\
			break;                                  \
		}
#define TH_LPB0(a,b) {										\
			b[a]=TH_LPB0_TOKEN; 							\
			TDM_PRINT0("TILE 0 LOOPBACK\n");				\
			break;                                          \
		}
#define TH_MGM1(a,b) {										\
			b[a]=TH_MGM1_TOKEN; 							\
			TDM_PRINT0("IDB 1 MANAGEMENT\n");				\
			break;                                          \
		}
#define TH_LPB1(a,b) {										\
			b[a]=TH_LPB1_TOKEN; 							\
			TDM_PRINT0("TILE 1 LOOPBACK\n");				\
			break;                                          \
		}
#define TH_MGM2(a,b) {										\
			b[a]=TH_MGM2_TOKEN; 							\
			TDM_PRINT0("IDB 2 MANAGEMENT\n");				\
			break;                                          \
		}
#define TH_LPB2(a,b) {										\
			b[a]=TH_LPB2_TOKEN; 							\
			TDM_PRINT0("TILE 2 LOOPBACK\n");				\
			break;                                          \
		}
#define TH_OPRT(a,b) {							\
			b[a]=TH_IDL2_TOKEN; 				\
			TDM_PRINT0("PURGE\n");				\
			break;								\
		}
#define TH_SBUS(a,b) {							\
			b[a]=TH_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}		
#define TH_NULL(a,b) {							\
			b[a]=TH_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}				
#define TH_LPB3(a,b) {							\
			b[a]=TH_LPB3_TOKEN; 				\
			TDM_PRINT0("TILE 3 LOOPBACK\n");	\
			break;								\
		}
#define TH_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TH_OVSB_TOKEN) {			\
					b[a]=TH_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TH_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==TH_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define TH_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TH_OVSB_TOKEN) {				\
					b[a]=TH_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TH_MGM1_TOKEN;					\
				}										\
			}											\
			if (b[a]==TH_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}			
#define TH_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TH_OVSB_TOKEN) {				\
					b[a]=TH_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TH_MGM2_TOKEN;					\
				}										\
			}											\
			if (b[a]==TH_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define TH_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TH_OVSB_TOKEN) {			\
					b[a]=TH_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TH_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==TH_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct th_ll_node {
	unsigned short port;
	struct th_ll_node *next;
} th_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_th_methods.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_methods.h>
#endif

/* API shim layer */
LINKER_DECL tdm_mod_t* tdm_chip_th_shim__which_tsc( int port, int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS] );
LINKER_DECL tdm_mod_t* tdm_chip_th_shim__check_ethernet_d( int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int **tsc, int traffic[TH_NUM_PM_MOD] );

#endif /* TDM_TH_PREPROCESSOR_DIRECTIVES_H */
