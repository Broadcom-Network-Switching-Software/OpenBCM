/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56860
 */

#ifndef TDM_MN_PREPROCESSOR_DIRECTIVES_H
#define TDM_MN_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_mn_defines.h>
#else
	#include <soc/tdm/monterey/tdm_mn_defines.h>
#endif


/* Excludes all tokenized port numbers */
#define MN_TOKEN_CHECK(a)  				\
			if (a!=MN_NUM_EXT_PORTS && 	\
			a!=MN_OVSB_TOKEN && 			\
			a!=MN_IDL1_TOKEN && 			\
			a!=MN_AUX_TOKEN && 			\
			a!=MN_IDL2_TOKEN && 			\
			a!=MN_ANCL_TOKEN &&			\
			a!=MN_SBUS_TOKEN &&			\
			a!=MN_MGMT_TOKEN && 			\
			a!=MN_US1G_TOKEN && 			\
			a!=MN_LOOPBACK_PORT &&		\
			a!=MN_MACSEC_PORT &&		\
			a!=MN_CPU_PORT) 				\

/* Excludes all tokenized port numbers except OVS */
#define MN_OVS_TOKEN_CHECK(a)  				\
			if (a!=MN_NUM_EXT_PORTS && 	\
			a!=MN_IDL1_TOKEN && 			\
			a!=MN_AUX_TOKEN && 			\
			a!=MN_IDL2_TOKEN && 			\
			a!=MN_ANCL_TOKEN &&			\
			a!=MN_MGMT_TOKEN && 			\
			a!=MN_US1G_TOKEN && 			\
			a!=MN_LOOPBACK_PORT &&		\
			a!=MN_CPU_PORT) 				\

#define MN_CMIC(a) {						\
			mmu_tdm_tbl[a]=MN_CPU_PORT; 	\
			TDM_PRINT0("CMIC\n"); 			\
			break;							\
		}
#define MN_LPBK(a) {							\
			mmu_tdm_tbl[a]=MN_LOOPBACK_PORT; 	\
			TDM_PRINT0("LOOPBACK\n"); 			\
			break;								\
		}
#define MN_IDLE(a) {							\
			mmu_tdm_tbl[a]=MN_NUM_EXT_PORTS; 	\
			TDM_PRINT0("IDLE\n"); 			\
			break;								\
		}
#define MN_MACSEC(a) {							\
			mmu_tdm_tbl[a]=MN_MACSEC_PORT; 	\
			TDM_PRINT0("MACSEC\n"); 			\
			break;								\
		}
#define MN_ANCL(a) {							\
			mmu_tdm_tbl[a]=MN_AUX_TOKEN; 	\
			TDM_PRINT0("AUXILLARY\n");		\
			break;								\
		}
#define MN_SBUS(a) {							\
			mmu_tdm_tbl[a]=MN_SBUS_TOKEN; 	\
			TDM_PRINT0("SBUS\n");		\
			break;								\
		}
#define MN_REFR(a) {							\
			mmu_tdm_tbl[a]=MN_IDL2_TOKEN; 	\
			TDM_PRINT0("REFRESH\n");			\
			break;								\
		}
		
#define IARB_MAIN_TDM__TDM_SLOT_PGW_0         0
#define IARB_MAIN_TDM__TDM_SLOT_PGW_1         1
#define IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT     2
#define IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK   3
#define IARB_MAIN_TDM__TDM_SLOT_RDB_PORT      4 
#define IARB_MAIN_TDM__TDM_SLOT_RESERVED      6
#define IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT  7

typedef struct mn_ll_node {
	unsigned short port;
	struct mn_ll_node *next;
} mn_ll_node;

typedef struct {
	int pgw_tdm_tbl_x0[MN_LR_LLS_LEN];
	int ovs_tdm_tbl_x0[MN_OS_LLS_GRP_LEN]; 
	int ovs_spacing_x0[MN_OS_LLS_GRP_LEN];
	int pgw_tdm_tbl_x1[MN_LR_LLS_LEN];
	int ovs_tdm_tbl_x1[MN_OS_LLS_GRP_LEN];
	int ovs_spacing_x1[MN_OS_LLS_GRP_LEN];
	int iarb_tdm_tbl_x[MN_LR_IARB_STATIC_LEN];
	int op_flags_x[NUM_OP_FLAGS];
} mn_tdm_pgw_t;

typedef struct {
	int tdm_pipe_main[MN_LR_VBS_LEN];
	int tdm_ovs_grp_0[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_1[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_2[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_3[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_4[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_5[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_6[MN_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_7[MN_OS_VBS_GRP_LEN];
} mn_tdm_pipe_t;

typedef struct {
	enum port_speed_e speed[MN_NUM_EXT_PORTS];
	int clk_freq;
	enum port_state_e port_rates_array[MN_NUM_EXT_PORTS];
	int pm_encap_type[MN_NUM_PM_MOD];
  int cl_flag;
  int num_1g;
} mn_tdm_globals_t;

typedef struct {
	int pmap[MN_NUM_PHY_PM][MN_NUM_PM_LNS];
	mn_tdm_pipe_t tdm_pipe_table_x;
	mn_tdm_globals_t tdm_globals;
	mn_tdm_pgw_t tdm_pgw;
} tdm_mn_chip_legacy_t;
typedef enum {
  MN_PGW_OS_FALCN_800G =0x0,
  MN_PGW_OS_FALCN_1000G =0x1,
  MN_PGW_OS_1000G =0x2 /* New OS Support  */
} mn_pgw_os_tdm_sel_e;


#ifdef _TDM_STANDALONE
	#include <tdm_mn_methods.h>
#else
	#include <soc/tdm/monterey/tdm_mn_methods.h>
#endif

/* API shim layer */
LINKER_DECL int tdm_chip_mn_shim_core( tdm_mod_t *_tdm, tdm_mn_chip_legacy_t *_tdm_mn );
LINKER_DECL int tdm_chip_mn_shim__ing_wrap( tdm_mod_t *_tdm, tdm_mn_chip_legacy_t *_tdm_mn );
LINKER_DECL int tdm_chip_mn_shim__egr_wrap( tdm_mod_t *_tdm, tdm_mn_chip_legacy_t *_tdm_mn );
LINKER_DECL tdm_mod_t* tdm_chip_mn_shim__which_tsc_alloc( int port, int tsc[MN_NUM_PHY_PM][MN_NUM_PM_LNS] );
LINKER_DECL int tdm_chip_mn_shim__which_tsc_free( tdm_mod_t *_tdm ); 
LINKER_DECL tdm_mod_t* tdm_chip_mn_shim__check_ethernet( int port, enum port_speed_e speed[MN_NUM_EXT_PORTS], int tsc[MN_NUM_PHY_PM][MN_NUM_PM_LNS], int traffic[MN_NUM_PM_MOD] );
LINKER_DECL tdm_mod_t* tdm_chip_mn_shim__check_ethernet_d( int port, enum port_speed_e speed[MN_NUM_EXT_PORTS], enum port_state_e state[MN_NUM_EXT_PORTS], int **tsc, int traffic[MN_NUM_PM_MOD] );
LINKER_DECL int tdm_chip_mn_shim__core_vbs_scheduler_ovs( tdm_mod_t *_tdm );


#endif /* TDM_MN_PREPROCESSOR_DIRECTIVES_H */
