/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56970
 */

#ifndef TDM_TH2_PREPROCESSOR_DIRECTIVES_H
#define TDM_TH2_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th2_defines.h>
#else
	#include <soc/tdm/tomahawk2/tdm_th2_defines.h>
#endif

#define TH2_TOKEN_CHECK(a)  				\
			if (a!=TH2_NUM_EXT_PORTS && 	\
				a!=TH2_OVSB_TOKEN && 	\
				a!=TH2_NULL_TOKEN &&		\
				a!=TH2_RSVD_TOKEN &&		\
				a!=TH2_MGM1_TOKEN &&		\
				a!=TH2_LPB1_TOKEN && 	\
				a!=TH2_MGM2_TOKEN && 	\
				a!=TH2_LPB2_TOKEN && 	\
				a!=TH2_LPB3_TOKEN && 	\
				a!=TH2_CMIC_TOKEN && 	\
				a!=TH2_ANCL_TOKEN &&		\
				a!=TH2_IDL1_TOKEN &&		\
				a!=TH2_IDL2_TOKEN &&		\
				a!=TH2_LPB0_TOKEN) 		\

#define TH2_CMIC(a,b) {								\
			b[a]=TH2_CMIC_TOKEN; 					\
			TDM_PRINT0("CMIC/CPU\n");				\
			break;                                  \
		}
#define TH2_LPB0(a,b) {										\
			b[a]=TH2_LPB0_TOKEN; 							\
			TDM_PRINT0("LOOPBACK\n");						\
			break;                                          \
		}
#define TH2_MGM1(a,b) {										\
			b[a]=TH2_MGM1_TOKEN; 							\
			TDM_PRINT0("MANAGEMENT\n");						\
			break;                                          \
		}
#define TH2_LPB1(a,b) {										\
			b[a]=TH2_LPB1_TOKEN; 							\
			TDM_PRINT0("TILE 1 LOOPBACK\n");				\
			break;                                          \
		}
#define TH2_MGM2(a,b) {										\
			b[a]=TH2_MGM2_TOKEN; 							\
			TDM_PRINT0("IDB 2 MANAGEMENT\n");				\
			break;                                          \
		}
#define TH2_LPB2(a,b) {										\
			b[a]=TH2_LPB2_TOKEN; 							\
			TDM_PRINT0("TILE 2 LOOPBACK\n");				\
			break;                                          \
		}
#define TH2_OPRT(a,b) {							\
			b[a]=TH2_IDL2_TOKEN; 				\
			TDM_PRINT0("PURGE\n");				\
			break;								\
		}
#define TH2_SBUS(a,b) {							\
			b[a]=TH2_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}		
#define TH2_NULL(a,b) {							\
			b[a]=TH2_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}				
#define TH2_LPB3(a,b) {							\
			b[a]=TH2_LPB3_TOKEN; 				\
			TDM_PRINT0("TILE 3 LOOPBACK\n");	\
			break;								\
		}
#define TH2_OPT1(a,b) {										\
			b[a]=TH2_IDL1_TOKEN; 							\
			TDM_PRINT0("OPPORTUNISTIC 1\n");				\
			break;                                          \
		}
#define TH2_IDLE(a,b) {										\
			b[a]=TH2_IDL2_TOKEN; 							\
			TDM_PRINT0("IDLE\n");							\
			break;                                          \
		}
#define TH2_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TH2_OVSB_TOKEN) {			\
					b[a]=TH2_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TH2_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==TH2_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define TH2_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TH2_OVSB_TOKEN) {				\
					b[a]=TH2_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TH2_MGM1_TOKEN;					\
				}										\
			}											\
			if (b[a]==TH2_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}			
#define TH2_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TH2_OVSB_TOKEN) {				\
					b[a]=TH2_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TH2_MGM2_TOKEN;					\
				}										\
			}											\
			if (b[a]==TH2_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define TH2_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TH2_OVSB_TOKEN) {			\
					b[a]=TH2_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TH2_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==TH2_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

#ifdef _TDM_STANDALONE
	#include <tdm_th2_methods.h>
#else
	#include <soc/tdm/tomahawk2/tdm_th2_methods.h>
#endif

/* API shim layer */
LINKER_DECL tdm_mod_t* tdm_chip_th2_shim__which_tsc( int port, int tsc[TH2_NUM_PHY_PM][TH2_NUM_PM_LNS] );
LINKER_DECL tdm_mod_t* tdm_chip_th2_shim__check_ethernet_d( int port, enum port_speed_e speed[TH2_NUM_EXT_PORTS], enum port_state_e state[TH2_NUM_EXT_PORTS], int **tsc, int traffic[TH2_NUM_PM_MOD] );
LINKER_DECL int tdm_chip_th2_shim__core_vbs_scheduler_ovs( tdm_mod_t *_tdm );

#endif /* TDM_TH2_PREPROCESSOR_DIRECTIVES_H */
