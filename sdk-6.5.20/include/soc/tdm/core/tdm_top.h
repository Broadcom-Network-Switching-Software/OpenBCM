/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for core scheduler
 */

#ifndef TDM_PREPROCESSOR_DIRECTIVES_H
#define TDM_PREPROCESSOR_DIRECTIVES_H

#ifdef _CMATH
	#include <math.h>
#endif

#ifdef _TDM_STANDALONE
	/* General headers */
	#include <alloca.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	/* Prints */
	#define TDM_PRINT0(a) printf(a)
	#define TDM_ERROR8(a,b,c,d,e,f,g,h,i) 			\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c,d,e,f,g,h,i)
	#define TDM_ERROR7(a,b,c,d,e,f,g,h) 			\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c,d,e,f,g,h)
	#define TDM_ERROR6(a,b,c,d,e,f,g) 				\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c,d,e,f,g)
	#define TDM_ERROR5(a,b,c,d,e,f) 				\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c,d,e,f)					
	#define TDM_ERROR4(a,b,c,d,e) 					\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c,d,e)
	#define TDM_ERROR3(a,b,c,d) 					\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c,d)
	#define TDM_ERROR2(a,b,c) 						\
					printf("TDM: _____ERROR: ");	\
					printf(a,b,c)
	#define TDM_ERROR1(a,b) 						\
					printf("TDM: _____ERROR: ");	\
					printf(a,b)
	#define TDM_ERROR0(a) 							\
					printf("TDM: _____ERROR: ");	\
					printf(a)
	#define TDM_WARN6(a,b,c,d,e,f,g)				\
					printf("TDM: _____WARNING: ");	\
					printf(a,b,c,d,e,f,g)
	#define TDM_WARN5(a,b,c,d,e,f)					\
					printf("TDM: _____WARNING: ");	\
					printf(a,b,c,d,e,f)
	#define TDM_WARN4(a,b,c,d,e)					\
					printf("TDM: _____WARNING: ");	\
					printf(a,b,c,d,e)
	#define TDM_WARN3(a,b,c,d)						\
					printf("TDM: _____WARNING: ");	\
					printf(a,b,c,d)
	#define TDM_WARN2(a,b,c)						\
					printf("TDM: _____WARNING: ");	\
					printf(a,b,c)
	#define TDM_WARN1(a,b)							\
					printf("TDM: _____WARNING: ");	\
					printf(a,b)
	#define TDM_WARN0(a)							\
					printf("TDM: _____WARNING: ");	\
					printf(a)
	#define TDM_BIG_BAR TDM_PRINT0("TDM: #################################################################################################################################\n");
	#define TDM_SML_BAR TDM_PRINT0("TDM: ---------------------------------------------------------------------------------------------------------------------------------\n");
	#define TDM_PRINT9(a,b,c,d,e,f,g,h,i,j) printf(a,b,c,d,e,f,g,h,i,j)
	#define TDM_PRINT8(a,b,c,d,e,f,g,h,i) printf(a,b,c,d,e,f,g,h,i)
	#define TDM_PRINT7(a,b,c,d,e,f,g,h) printf(a,b,c,d,e,f,g,h)
	#define TDM_PRINT6(a,b,c,d,e,f,g) printf(a,b,c,d,e,f,g)
	#define TDM_PRINT5(a,b,c,d,e,f) printf(a,b,c,d,e,f)	
	#define TDM_PRINT4(a,b,c,d,e) printf(a,b,c,d,e)
	#define TDM_PRINT3(a,b,c,d) printf(a,b,c,d)
	#define TDM_PRINT2(a,b,c) printf(a,b,c)
	#define TDM_PRINT1(a,b) printf(a,b)
	/* Compiler */
	#define LINKER_DECL extern
	#define TDM_ALLOC(_sz,_id) malloc(_sz)
	/* #define TDM_MSET(_str,_val,_len) memset(_str,_val,_len) */
	#define TDM_MSET(_str,_val,_len)												\
					{																\
						int TDM_MSET_IDX;											\
						for (TDM_MSET_IDX=0; TDM_MSET_IDX<_len; TDM_MSET_IDX++) {	\
							_str[TDM_MSET_IDX]=(unsigned short) _val;				\
						}															\
					}
	#define TDM_FREE(_sz) free(_sz)
	#define TDM_COPY(_dst,_src,_len) memcpy(_dst,_src,_len)
	/* TDM headers */
	#include <tdm_defines.h>
	#include <tdm_soc.h>
	#include <tdm_methods.h>
#else
	/* General headers */
	#include <shared/bsl.h>
	#include <soc/debug.h>
	#include <soc/cm.h>
	#include <sal/core/alloc.h>
	/* Prints */
	#define TDM_PRINT0(a) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META("%s"),a))
	#define TDM_VERBOSE(_frmt,a) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(_frmt),a))
	#define TDM_ERROR8(a,b,c,d,e,f,g,h,i) LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i))
	#define TDM_ERROR7(a,b,c,d,e,f,g,h)   LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
	#define TDM_ERROR6(a,b,c,d,e,f,g) 	  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_ERROR5(a,b,c,d,e,f) 	  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_ERROR4(a,b,c,d,e) 		  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_ERROR3(a,b,c,d) 		  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_ERROR2(a,b,c) 			  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_ERROR1(a,b) 			  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_ERROR0(a) 				  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a)))
	#define TDM_WARN6(a,b,c,d,e,f,g) LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_WARN5(a,b,c,d,e,f)   LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_WARN4(a,b,c,d,e)     LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_WARN3(a,b,c,d)       LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_WARN2(a,b,c)         LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_WARN1(a,b)           LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_WARN0(a)             LOG_WARN(BSL_LS_SOC_TDM, (BSL_META("%s"),a))
	#define TDM_PRINT9(a,b,c,d,e,f,g,h,i,j) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i,j))
	#define TDM_PRINT8(a,b,c,d,e,f,g,h,i) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i))
	#define TDM_PRINT7(a,b,c,d,e,f,g,h)   LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
	#define TDM_PRINT6(a,b,c,d,e,f,g)     LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_PRINT5(a,b,c,d,e,f)       LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_PRINT4(a,b,c,d,e)         LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_PRINT3(a,b,c,d)           LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_PRINT2(a,b,c)             LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_PRINT1(a,b)               LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_BIG_BAR TDM_PRINT0(("#################################################################################################################################\n"));
	#define TDM_SML_BAR TDM_PRINT0(("---------------------------------------------------------------------------------------------------------------------------------\n"));
	#define TDM_DEBUG TDM_PRINT0(("--- DEBUG ---\n"));
	/* Compiler */
	#define LINKER_DECL
	#define TDM_ALLOC(_sz,_id) sal_alloc(_sz,_id)
	#define TDM_MEMSET(_str,_val,_len) sal_memset(_str,_val,_len) 
	#define TDM_MSET(_str,_val,_len)												\
					{																\
						int TDM_MSET_IDX;											\
						for (TDM_MSET_IDX=0; TDM_MSET_IDX<_len; TDM_MSET_IDX++) {	\
							_str[TDM_MSET_IDX]=(unsigned short) _val;				\
						}															\
					}
	#define TDM_FREE(_sz) sal_free(_sz)
	#define TDM_COPY(_dst,_src,_len) sal_memcpy(_dst,_src,_len)
	/* TDM headers */
	#include <soc/tdm/core/tdm_defines.h>
	#include <soc/tdm/core/tdm_soc.h>
	#include <soc/tdm/core/tdm_methods.h>
#endif

#define TOKEN_CHECK(a)  											\
			if (a>=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&	\
				a<=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi)	\
				
#define TDM_SEL_CAL(_cal_id,_cal_pntr)										\
			switch (_cal_id) {												\
				case 0: _cal_pntr=_tdm->_chip_data.cal_0.cal_main; break;	\
				case 1: _cal_pntr=_tdm->_chip_data.cal_1.cal_main; break;	\
				case 2: _cal_pntr=_tdm->_chip_data.cal_2.cal_main; break;	\
				case 3: _cal_pntr=_tdm->_chip_data.cal_3.cal_main; break;	\
				case 4: _cal_pntr=_tdm->_chip_data.cal_4.cal_main; break;	\
				case 5: _cal_pntr=_tdm->_chip_data.cal_5.cal_main; break;	\
				case 6: _cal_pntr=_tdm->_chip_data.cal_6.cal_main; break;	\
				case 7: _cal_pntr=_tdm->_chip_data.cal_7.cal_main; break;	\
				default:													\
					_cal_pntr=NULL;											\
					TDM_PRINT1("Invalid calendar ID - %0d\n",_cal_id);		\
					return (TDM_EXEC_CORE_SIZE+1);							\
			}
#define TDM_SEL_GRP(_grp_id,_grp_pntr)										\
			switch (_grp_id) {												\
				case 0: _grp_pntr=_tdm->_chip_data.cal_0.cal_grp; break;	\
				case 1: _grp_pntr=_tdm->_chip_data.cal_1.cal_grp; break;	\
				case 2: _grp_pntr=_tdm->_chip_data.cal_2.cal_grp; break;	\
				case 3: _grp_pntr=_tdm->_chip_data.cal_3.cal_grp; break;	\
				case 4: _grp_pntr=_tdm->_chip_data.cal_4.cal_grp; break;	\
				case 5: _grp_pntr=_tdm->_chip_data.cal_5.cal_grp; break;	\
				case 6: _grp_pntr=_tdm->_chip_data.cal_6.cal_grp; break;	\
				case 7: _grp_pntr=_tdm->_chip_data.cal_7.cal_grp; break;	\
				default:													\
					_grp_pntr=NULL;											\
					TDM_PRINT1("Invalid group ID - %0d\n",_grp_id);			\
					return (TDM_EXEC_CORE_SIZE+1);							\
			}
#define TDM_PUSH(a,b,c) 												\
				{														\
					int TDM_PORT_POP=_tdm->_core_data.vars_pkg.port;	\
					_tdm->_core_data.vars_pkg.port=a;					\
					c=_tdm->_core_exec[b](_tdm);						\
					_tdm->_core_data.vars_pkg.port=TDM_PORT_POP;		\
				}
#ifdef _TDM_DB_STACK
    #define TDM_PRINT_STACK_SIZE(a)                                        \
                {                                                          \
                    size_t stack_check = 0;                                \
                    stack_check = (stack_size - ((size_t)&(stack_check))); \
                    TDM_PRINT2("TDM: [stack_size %d, %s]\n",               \
                    (int)stack_check, a);                                  \
                }
#else
    #define TDM_PRINT_STACK_SIZE(a)                                        \
                {                                                          \
                    TDM_PRINT2("TDM: [stack_size %d, %s]\n", 0, a);        \
                }
#endif

extern tdm_mod_t
*SOC_SEL_TDM(tdm_soc_t *chip);
extern tdm_mod_t
*_soc_set_tdm_tbl(tdm_mod_t *_tdm);
extern void
_soc_tdm_ver(int ver[8]);


#endif /* TDM_PREPROCESSOR_DIRECTIVES_H*/

/* ESW chip support */
/* Trident2plus */
#if ( defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_td2p_top.h>
	#else
		#include <soc/tdm/trident2p/tdm_td2p_top.h>
	#endif
#endif
/* Trident3 */
#if ( defined(BCM_TRIDENT3_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_td3_top.h>
	#else
		#include <soc/tdm/trident3/tdm_td3_top.h>
	#endif
#endif
/* Maverick2 */
#if ( defined(BCM_MAVERICK2_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_mv2_top.h>
	#else
		#include <soc/tdm/maverick2/tdm_mv2_top.h>
	#endif
#endif
/* Firebolt6 */
#if ( defined(BCM_FIREBOLT6_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_fb6_top.h>
	#else
		#include <soc/tdm/firebolt6/tdm_fb6_top.h>
	#endif
#endif
/* Helix5 */
#if ( defined(BCM_HELIX5_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_hx5_top.h>
	#else
		#include <soc/tdm/helix5/tdm_hx5_top.h>
    #endif
#endif        
/* HURRICANE4 */
#if ( defined(BCM_HURRICANE4_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_hr4_top.h>
	#else
		#include <soc/tdm/hurricane4/tdm_hr4_top.h>
    #endif
#endif        
/* Tomahawk */
#if ( defined(BCM_TOMAHAWK_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_th_top.h>
	#else
		#include <soc/tdm/tomahawk/tdm_th_top.h>
	#endif
#endif
/* Tomahawk2 */
#if ( defined(BCM_TOMAHAWK2_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_th2_top.h>
	#else
		#include <soc/tdm/tomahawk2/tdm_th2_top.h>
	#endif
#endif
/* Apache */
#if ( defined(BCM_APACHE_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_ap_top.h>
	#else
		#include <soc/tdm/apache/tdm_ap_top.h>
	#endif
#endif
/* Greyhound2 */
#if ( defined(BCM_GREYHOUND2_SUPPORT) )
    #ifdef _TDM_STANDALONE
        #include <tdm_gh2_top.h>
    #else
        #include <soc/tdm/greyhound2/tdm_gh2_top.h>
    #endif
#endif
/* Firelight */
#if ( defined(BCM_FIRELIGHT_SUPPORT) )
    #ifdef _TDM_STANDALONE
        #include <tdm_fl_top.h>
    #else
        #include <soc/tdm/firelight/tdm_fl_top.h>
    #endif
#endif
/* Monterey */
#if ( defined(BCM_MONTEREY_SUPPORT) )
	#ifdef _TDM_STANDALONE
		#include <tdm_mn_top.h>
	#else
		#include <soc/tdm/monterey/tdm_mn_top.h>
	#endif
#endif
