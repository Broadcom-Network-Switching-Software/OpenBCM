/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56870
 */

#ifndef TDM_FB6_PREPROCESSOR_DIRECTIVES_H
#define TDM_FB6_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_fb6_defines.h>
#else
	#include <soc/tdm/firebolt6/tdm_fb6_defines.h>
#endif

#define FB6_TOKEN_CHECK(a)  		\
        if (a!=FB6_NUM_EXT_PORTS && \
            a!=FB6_OVSB_TOKEN && 	\
            a!=FB6_CMIC_TOKEN && 	\
            a!=FB6_MGM1_TOKEN && 	\
            a!=FB6_MGM2_TOKEN && 	\
            a!=FB6_MGM3_TOKEN && 	\
            a!=FB6_MGM4_TOKEN && 	\
            a!=FB6_FAE_TOKEN &&	\
            a!=FB6_LPB0_TOKEN && 	\
            a!=FB6_ANCL_TOKEN &&	\
            a!=FB6_NULL_TOKEN &&	\
            a!=FB6_IDL1_TOKEN &&	\
            a!=FB6_IDL2_TOKEN) 		\

#define FB6_CMIC(a,b) {							\
			b[a]=FB6_CMIC_TOKEN; 				\
			TDM_PRINT0("CMIC/CPU\n");			\
			break;                              \
		}
#define FB6_MGM1(a,b) {							\
			b[a]=FB6_MGM1_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-1\n");		\
			break;                              \
		}
#define FB6_MGM2(a,b) {							\
			b[a]=FB6_MGM2_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-2\n");		\
			break;                              \
		}
#define FB6_MGM3(a,b) {							\
			b[a]=FB6_MGM3_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-3\n");		\
			break;                              \
		}
#define FB6_MGM4(a,b) {							\
			b[a]=FB6_MGM4_TOKEN; 				\
			TDM_PRINT0("MANAGEMENT-4\n");		\
			break;                              \
		}

#define FB6_LPB0(a,b) {							\
			b[a]=FB6_LPB0_TOKEN; 				\
			TDM_PRINT0("TILE 0 LOOPBACK\n");	\
			break;                              \
		}
#define FB6_OPT1(a,b) {							\
			b[a]=FB6_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 1\n");	\
			break;								\
		}
#define FB6_OPT2(a,b) {							\
			b[a]=FB6_IDL1_TOKEN; 				\
			TDM_PRINT0("OPPORTUNISTIC 2\n");	\
			break;								\
		}
#define FB6_SBUS(a,b) {							\
			b[a]=FB6_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}
#define FB6_NULL(a,b) {							\
			b[a]=FB6_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}
#define FB6_IDLE(a,b) {							\
			b[a]=FB6_IDL2_TOKEN; 				\
			TDM_PRINT0("IDLE\n");				\
			break;								\
		}
#define FB6_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==FB6_OVSB_TOKEN) {			\
					b[a]=FB6_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=FB6_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==FB6_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define FB6_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==FB6_OVSB_TOKEN) {				\
					b[a]=FB6_OVSB_TOKEN;					\
					break;								\
				}										\
			}											\
			if (b[a]==FB6_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define FB6_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==FB6_OVSB_TOKEN) {				\
					b[a]=FB6_OVSB_TOKEN;					\
					break;								\
				}										\
			}											\
			if (b[a]==FB6_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define FB6_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==FB6_OVSB_TOKEN) {			\
					b[a]=FB6_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=FB6_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==FB6_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct fb6_ll_node {
	unsigned short port;
	struct fb6_ll_node *next;
} fb6_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_fb6_methods.h>
#else
	#include <soc/tdm/firebolt6/tdm_fb6_methods.h>
#endif

/* API shim layer */

#endif /* TDM_FB6_PREPROCESSOR_DIRECTIVES_H */
