/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56275
 */

#ifndef TDM_HR4_PREPROCESSOR_DIRECTIVES_H
#define TDM_HR4_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_hr4_defines.h>
#else
	#include <soc/tdm/hurricane4/tdm_hr4_defines.h>
#endif

#define HR4_TOKEN_CHECK(a)  		\
        if (a!=HR4_NUM_EXT_PORTS && \
            a!=HR4_OVSB_TOKEN && 	\
            a!=HR4_CMIC_TOKEN && 	\
            a!=HR4_FAE_TOKEN &&	\
            a!=HR4_LPB0_TOKEN && 	\
            a!=HR4_ANCL_TOKEN &&	\
            a!=HR4_NULL_TOKEN &&	\
            a!=HR4_IDL1_TOKEN &&	\
            a!=HR4_IDL2_TOKEN) 		\

#define HR4_CMIC(a,b) {							\
			b[a]=HR4_CMIC_TOKEN; 				\
			TDM_PRINT0("CMIC/CPU\n");			\
			break;                              \
		}
#define HR4_LPB0(a,b) {							\
			b[a]=HR4_LPB0_TOKEN; 				\
			TDM_PRINT0("TILE 0 LOOPBACK\n");	\
			break;                              \
		}
#define HR4_OPT1(a,b) {							\
			b[a]=HR4_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 1\n");	\
			break;								\
		}
#define HR4_OPT2(a,b) {							\
			b[a]=HR4_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 2\n");	\
			break;								\
		}
#define HR4_SBUS(a,b) {							\
			b[a]=HR4_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}
#define HR4_NULL(a,b) {							\
			b[a]=HR4_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}
#define HR4_IDLE(a,b) {							\
			b[a]=HR4_IDL2_TOKEN; 				\
			TDM_PRINT0("IDLE\n");				\
			break;								\
		}
#define HR4_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==HR4_OVSB_TOKEN) {			\
					b[a]=HR4_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=HR4_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==HR4_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define HR4_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==HR4_OVSB_TOKEN) {				\
					b[a]=HR4_OVSB_TOKEN;					\
					break;								\
				}										\
			}											\
			if (b[a]==HR4_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define HR4_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==HR4_OVSB_TOKEN) {				\
					b[a]=HR4_OVSB_TOKEN;					\
					break;								\
				}										\
			}											\
			if (b[a]==HR4_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define HR4_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==HR4_OVSB_TOKEN) {			\
					b[a]=HR4_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=HR4_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==HR4_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct hr4_ll_node {
	unsigned short port;
	struct hr4_ll_node *next;
} hr4_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_hr4_methods.h>
#else
	#include <soc/tdm/hurricane4/tdm_hr4_methods.h>
#endif

/* API shim layer */

#endif /* TDM_HR4_PREPROCESSOR_DIRECTIVES_H */
