/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM53570
 */

#ifndef TDM_GH2_PREPROCESSOR_DIRECTIVES_H
#define TDM_GH2_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_gh2_defines.h>
#else
	#include <soc/tdm/greyhound2/tdm_gh2_defines.h>
#endif

#define GH2_TOKEN_CHECK(a)  				\
			if (a!=GH2_NUM_EXT_PORTS && 	\
				a!=GH2_OVSB_TOKEN && 	\
				a!=GH2_NULL_TOKEN &&		\
				a!=GH2_RSVD_TOKEN &&		\
				a!=GH2_MGM1_TOKEN &&		\
				a!=GH2_LPB1_TOKEN && 	\
				a!=GH2_MGM2_TOKEN && 	\
				a!=GH2_LPB2_TOKEN && 	\
				a!=GH2_LPB3_TOKEN && 	\
				a!=GH2_CMIC_TOKEN && 	\
				a!=GH2_ANCL_TOKEN &&		\
				a!=GH2_IDL1_TOKEN &&		\
				a!=GH2_IDL2_TOKEN &&		\
				a!=GH2_LPB0_TOKEN) 		\

#define GH2_CMIC(a,b) {								\
			b[a]=GH2_CMIC_TOKEN; 					\
			TDM_PRINT0("CMIC/CPU\n");				\
			break;                                  \
		}
#define GH2_LPB0(a,b) {										\
			b[a]=GH2_LPB0_TOKEN; 							\
			TDM_PRINT0("TILE 0 LOOPBACK\n");				\
			break;                                          \
		}
#define GH2_MGM1(a,b) {										\
			b[a]=GH2_MGM1_TOKEN; 							\
			TDM_PRINT0("IDB 1 MANAGEMENT\n");				\
			break;                                          \
		}
#define GH2_LPB1(a,b) {										\
			b[a]=GH2_LPB1_TOKEN; 							\
			TDM_PRINT0("TILE 1 LOOPBACK\n");				\
			break;                                          \
		}
#define GH2_MGM2(a,b) {										\
			b[a]=GH2_MGM2_TOKEN; 							\
			TDM_PRINT0("IDB 2 MANAGEMENT\n");				\
			break;                                          \
		}
#define GH2_LPB2(a,b) {										\
			b[a]=GH2_LPB2_TOKEN; 							\
			TDM_PRINT0("TILE 2 LOOPBACK\n");				\
			break;                                          \
		}
#define GH2_OPRT(a,b) {							\
			b[a]=GH2_IDL2_TOKEN; 				\
			TDM_PRINT0("PURGE\n");				\
			break;								\
		}
#define GH2_SBUS(a,b) {							\
			b[a]=GH2_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}		
#define GH2_NULL(a,b) {							\
			b[a]=GH2_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}				
#define GH2_LPB3(a,b) {							\
			b[a]=GH2_LPB3_TOKEN; 				\
			TDM_PRINT0("TILE 3 LOOPBACK\n");	\
			break;								\
		}
#define GH2_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==GH2_OVSB_TOKEN) {			\
					b[a]=GH2_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=GH2_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==GH2_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define GH2_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==GH2_OVSB_TOKEN) {				\
					b[a]=GH2_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=GH2_MGM1_TOKEN;					\
				}										\
			}											\
			if (b[a]==GH2_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}			
#define GH2_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==GH2_OVSB_TOKEN) {				\
					b[a]=GH2_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=GH2_MGM2_TOKEN;					\
				}										\
			}											\
			if (b[a]==GH2_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define GH2_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==GH2_OVSB_TOKEN) {			\
					b[a]=GH2_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=GH2_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==GH2_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct gh2_ll_node {
	unsigned short port;
	struct gh2_ll_node *next;
} gh2_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_gh2_methods.h>
#else
	#include <soc/tdm/greyhound2/tdm_gh2_methods.h>
#endif

/* API shim layer */
LINKER_DECL tdm_mod_t* tdm_chip_gh2_shim__which_tsc( int port, int tsc[GH2_NUM_PHY_PM][GH2_NUM_PM_LNS] );
LINKER_DECL tdm_mod_t* tdm_chip_gh2_shim__check_ethernet_d( int port, enum port_speed_e speed[GH2_NUM_EXT_PORTS], enum port_state_e state[GH2_NUM_EXT_PORTS], int **tsc, int traffic[GH2_NUM_PM_MOD] );

#endif /* TDM_GH2_PREPROCESSOR_DIRECTIVES_H */
