/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56870
 */

#ifndef TDM_TD3_PREPROCESSOR_DIRECTIVES_H
#define TDM_TD3_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_td3_defines.h>
#else
	#include <soc/tdm/trident3/tdm_td3_defines.h>
#endif

#define TD3_TOKEN_CHECK(a)  		\
        if (a!=TD3_NUM_EXT_PORTS && \
            a!=TD3_OVSB_TOKEN && 	\
            a!=TD3_CMIC_TOKEN && 	\
            a!=TD3_MGM1_TOKEN &&	\
            a!=TD3_MGM2_TOKEN &&	\
            a!=TD3_LPB0_TOKEN && 	\
            a!=TD3_LPB1_TOKEN && 	\
            a!=TD3_ANCL_TOKEN &&	\
            a!=TD3_NULL_TOKEN &&	\
            a!=TD3_IDL1_TOKEN &&	\
            a!=TD3_IDL2_TOKEN) 		\

#define TD3_CMIC(a,b) {							\
			b[a]=TD3_CMIC_TOKEN; 				\
			TDM_PRINT0("CMIC/CPU\n");			\
			break;                              \
		}
#define TD3_MGM1(a,b) {							\
			b[a]=TD3_MGM1_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-1\n");		\
			break;                              \
		}
#define TD3_MGM2(a,b) {							\
			b[a]=TD3_MGM2_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-2\n");		\
			break;                              \
		}
#define TD3_LPB0(a,b) {							\
			b[a]=TD3_LPB0_TOKEN; 				\
			TDM_PRINT0("TILE 0 LOOPBACK\n");	\
			break;                              \
		}
#define TD3_LPB1(a,b) {							\
			b[a]=TD3_LPB1_TOKEN; 				\
			TDM_PRINT0("TILE 1 LOOPBACK\n");	\
			break;                              \
		}
#define TD3_OPT1(a,b) {							\
			b[a]=TD3_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 1\n");	\
			break;								\
		}
#define TD3_OPT2(a,b) {							\
			b[a]=TD3_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 2\n");	\
			break;								\
		}
#define TD3_SBUS(a,b) {							\
			b[a]=TD3_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}
#define TD3_NULL(a,b) {							\
			b[a]=TD3_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}
#define TD3_IDLE(a,b) {							\
			b[a]=TD3_IDL2_TOKEN; 				\
			TDM_PRINT0("IDLE\n");				\
			break;								\
		}
#define TD3_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TD3_OVSB_TOKEN) {			\
					b[a]=TD3_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TD3_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==TD3_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define TD3_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TD3_OVSB_TOKEN) {				\
					b[a]=TD3_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TD3_MGM1_TOKEN;					\
				}										\
			}											\
			if (b[a]==TD3_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define TD3_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TD3_OVSB_TOKEN) {				\
					b[a]=TD3_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TD3_MGM2_TOKEN;					\
				}										\
			}											\
			if (b[a]==TD3_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define TD3_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TD3_OVSB_TOKEN) {			\
					b[a]=TD3_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TD3_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==TD3_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct td3_ll_node {
	unsigned short port;
	struct td3_ll_node *next;
} td3_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_td3_methods.h>
#else
	#include <soc/tdm/trident3/tdm_td3_methods.h>
#endif

/* API shim layer */

#endif /* TDM_TD3_PREPROCESSOR_DIRECTIVES_H */
