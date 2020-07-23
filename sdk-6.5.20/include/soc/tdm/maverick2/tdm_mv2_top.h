/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for Maverick2
 */

#ifndef TDM_MV2_PREPROCESSOR_DIRECTIVES_H
#define TDM_MV2_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_mv2_defines.h>
#else
	#include <soc/tdm/maverick2/tdm_mv2_defines.h>
#endif

#define MV2_TOKEN_CHECK(a)  		\
        if (a!=MV2_NUM_EXT_PORTS && \
            a!=MV2_OVSB_TOKEN && 	\
            a!=MV2_CMIC_TOKEN && 	\
            a!=MV2_MGM1_TOKEN &&	\
            a!=MV2_MGM2_TOKEN &&	\
            a!=MV2_LPB0_TOKEN && 	\
            a!=MV2_LPB1_TOKEN && 	\
            a!=MV2_ANCL_TOKEN &&	\
            a!=MV2_NULL_TOKEN &&	\
            a!=MV2_IDL1_TOKEN &&	\
            a!=MV2_IDL2_TOKEN) 		\

#define MV2_CMIC(a,b) {							\
			b[a]=MV2_CMIC_TOKEN; 				\
			TDM_PRINT0("CMIC/CPU\n");			\
			break;                              \
		}
#define MV2_MGM1(a,b) {							\
			b[a]=MV2_MGM1_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-1\n");		\
			break;                              \
		}
#define MV2_MGM2(a,b) {							\
			b[a]=MV2_MGM2_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-2\n");		\
			break;                              \
		}
#define MV2_LPB0(a,b) {							\
			b[a]=MV2_LPB0_TOKEN; 				\
			TDM_PRINT0("TILE 0 LOOPBACK\n");	\
			break;                              \
		}
#define MV2_LPB1(a,b) {							\
			b[a]=MV2_LPB1_TOKEN; 				\
			TDM_PRINT0("TILE 1 LOOPBACK\n");	\
			break;                              \
		}
#define MV2_OPT1(a,b) {							\
			b[a]=MV2_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 1\n");	\
			break;								\
		}
#define MV2_OPT2(a,b) {							\
			b[a]=MV2_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 2\n");	\
			break;								\
		}
#define MV2_SBUS(a,b) {							\
			b[a]=MV2_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}
#define MV2_NULL(a,b) {							\
			b[a]=MV2_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}
#define MV2_IDLE(a,b) {							\
			b[a]=MV2_IDL2_TOKEN; 				\
			TDM_PRINT0("IDLE\n");				\
			break;								\
		}
#define MV2_DOT0(a,b) {							\
			for (i=0; i<256; i++) {				\
				if (b[i]==MV2_OVSB_TOKEN) {		\
					b[a]=MV2_OVSB_TOKEN;		\
					break;						\
				}								\
				else {							\
					b[a]=MV2_CMIC_TOKEN;		\
				}								\
			}									\
			if (b[a]==MV2_OVSB_TOKEN) {         \
				TDM_PRINT0("DOT OVERSUB\n");	\
			}                                   \
			else {                              \
				TDM_PRINT0("DOT CMIC/CPU\n");	\
			}                                   \
			break;                              \
		}
#define MV2_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==MV2_OVSB_TOKEN) {				\
					b[a]=MV2_OVSB_TOKEN;				\
					break;								\
				}										\
				else {									\
					b[a]=MV2_MGM1_TOKEN;				\
				}										\
			}											\
			if (b[a]==MV2_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                           \
			else {                                      \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");	\
			}                                           \
			break;                                      \
		}
#define MV2_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==MV2_OVSB_TOKEN) {				\
					b[a]=MV2_OVSB_TOKEN;				\
					break;								\
				}										\
				else {									\
					b[a]=MV2_MGM2_TOKEN;				\
				}										\
			}											\
			if (b[a]==MV2_OVSB_TOKEN) {                 \
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                           \
			else {                                      \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");	\
			}                                           \
			break;                                      \
		}
#define MV2_DOT3(a,b) {								    \
			for (i=0; i<256; i++) {					    \
				if (b[i]==MV2_OVSB_TOKEN) {			    \
					b[a]=MV2_OVSB_TOKEN;			    \
					break;							    \
				}									    \
				else {								    \
					b[a]=MV2_IDL2_TOKEN;			    \
				}									    \
			}										    \
			if (b[a]==MV2_OVSB_TOKEN) {    			    \
				TDM_PRINT0("DOT OVERSUB\n");		    \
			}                                           \
			else {                                      \
				TDM_PRINT0("DOT PURGE\n");			    \
			}                                           \
			break;                                      \
		}

typedef struct mv2_ll_node {
	unsigned short port;
	struct mv2_ll_node *next;
} mv2_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_mv2_methods.h>
#else
	#include <soc/tdm/maverick2/tdm_mv2_methods.h>
#endif

/* API shim layer */

#endif /* TDM_MV2_PREPROCESSOR_DIRECTIVES_H */
