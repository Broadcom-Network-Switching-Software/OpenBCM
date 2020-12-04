/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56870
 */

#ifndef TDM_HX5_PREPROCESSOR_DIRECTIVES_H
#define TDM_HX5_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_hx5_defines.h>
#else
	#include <soc/tdm/helix5/tdm_hx5_defines.h>
#endif

#define HX5_TOKEN_CHECK(a)  		\
        if (a!=HX5_NUM_EXT_PORTS && \
            a!=HX5_OVSB_TOKEN && 	\
            a!=HX5_CMIC_TOKEN && 	\
            a!=HX5_FAE_TOKEN &&	\
            a!=HX5_LPB0_TOKEN && 	\
            a!=HX5_ANCL_TOKEN &&	\
            a!=HX5_NULL_TOKEN &&	\
            a!=HX5_IDL1_TOKEN &&	\
            a!=HX5_IDL2_TOKEN) 		\

#define HX5_CMIC(a,b) {							\
			b[a]=HX5_CMIC_TOKEN; 				\
			TDM_PRINT0("CMIC/CPU\n");			\
			break;                              \
		}
#define HX5_LPB0(a,b) {							\
			b[a]=HX5_LPB0_TOKEN; 				\
			TDM_PRINT0("TILE 0 LOOPBACK\n");	\
			break;                              \
		}
#define HX5_OPT1(a,b) {							\
			b[a]=HX5_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 1\n");	\
			break;								\
		}
#define HX5_OPT2(a,b) {							\
			b[a]=HX5_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 2\n");	\
			break;								\
		}
#define HX5_SBUS(a,b) {							\
			b[a]=HX5_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}
#define HX5_NULL(a,b) {							\
			b[a]=HX5_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}
#define HX5_IDLE(a,b) {							\
			b[a]=HX5_IDL2_TOKEN; 				\
			TDM_PRINT0("IDLE\n");				\
			break;								\
		}
#define HX5_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==HX5_OVSB_TOKEN) {			\
					b[a]=HX5_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=HX5_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==HX5_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define HX5_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==HX5_OVSB_TOKEN) {				\
					b[a]=HX5_OVSB_TOKEN;					\
					break;								\
				}										\
			}											\
			if (b[a]==HX5_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define HX5_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==HX5_OVSB_TOKEN) {				\
					b[a]=HX5_OVSB_TOKEN;					\
					break;								\
				}										\
			}											\
			if (b[a]==HX5_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define HX5_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==HX5_OVSB_TOKEN) {			\
					b[a]=HX5_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=HX5_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==HX5_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct hx5_ll_node {
	unsigned short port;
	struct hx5_ll_node *next;
} hx5_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_hx5_methods.h>
#else
	#include <soc/tdm/helix5/tdm_hx5_methods.h>
#endif

/* API shim layer */

#endif /* TDM_HX5_PREPROCESSOR_DIRECTIVES_H */
