/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for BCM56860
 */

#ifndef TDM_TD2P_PREPROCESSOR_DIRECTIVES_H
#define TDM_TD2P_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_td2p_defines.h>
#else
	#include <soc/tdm/trident2p/tdm_td2p_defines.h>
#endif


 /* Excludes the management ports which are also physical port numbers */
#define TD2P_MGMT_CHECK(a)				\
			if (a!=TD2P_MGMT_PORT_0 && 	\
			a!=TD2P_MGMT_PORT_1 && 		\
			a!=TD2P_MGMT_PORT_2 && 		\
			a!=TD2P_MGMT_PORT_3) 		\

/* Excludes all tokenized port numbers */
#define TD2P_TOKEN_CHECK(a)  				\
			if (a!=TD2P_NUM_EXT_PORTS && 	\
			a!=TD2P_OVSB_TOKEN && 			\
			a!=TD2P_IDL1_TOKEN && 			\
			a!=TD2P_IDL2_TOKEN && 			\
			a!=TD2P_ANCL_TOKEN &&			\
			a!=TD2P_MGMT_TOKEN && 			\
			a!=TD2P_US1G_TOKEN && 			\
			a!=TD2P_LOOPBACK_PORT &&		\
			a!=TD2P_CPU_PORT) 				\

#define TD2P_CMIC(a) {						\
			mmu_tdm_tbl[a]=TD2P_CPU_PORT; 	\
			TDM_PRINT0("CMIC\n"); 			\
			break;							\
		}
#define TD2P_LPBK(a) {							\
			mmu_tdm_tbl[a]=TD2P_LOOPBACK_PORT; 	\
			TDM_PRINT0("LOOPBACK\n"); 			\
			break;								\
		}
#define TD2P_ANCL(a) {							\
			mmu_tdm_tbl[a]=TD2P_IDL1_TOKEN; 	\
			TDM_PRINT0("OPPORTUNISTIC\n");		\
			break;								\
		}
#define TD2P_REFR(a) {							\
			mmu_tdm_tbl[a]=TD2P_IDL2_TOKEN; 	\
			TDM_PRINT0("REFRESH\n");			\
			break;								\
		}
#define TD2P_MM13(a) {									\
			mmu_tdm_tbl[a]=TD2P_MGMT_PORT_0; 			\
			TDM_PRINT0("management phyport 13\n");		\
			break;										\
		}
#define TD2P_MM14(a) {									\
			mmu_tdm_tbl[a]=TD2P_MGMT_PORT_1; 			\
			TDM_PRINT0("management phyport 14\n");		\
			break;										\
		}
#define TD2P_MM15(a) {									\
			mmu_tdm_tbl[a]=TD2P_MGMT_PORT_2; 			\
			TDM_PRINT0("management phyport 15\n");		\
			break;										\
		}
#define TD2P_MM16(a) {									\
			mmu_tdm_tbl[a]=TD2P_MGMT_PORT_3; 			\
			TDM_PRINT0("management phyport 16\n");		\
			break;										\
		}
		
#define IARB_MAIN_TDM__TDM_SLOT_PGW_0         0
#define IARB_MAIN_TDM__TDM_SLOT_PGW_1         1
#define IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT     2
#define IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK   3
#define IARB_MAIN_TDM__TDM_SLOT_QGP_PORT      4
#define IARB_MAIN_TDM__TDM_SLOT_RESERVED      6
#define IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT  7

typedef struct ll_node {
	unsigned short port;
	struct ll_node *next;
} ll_node;

typedef struct {
	int pgw_tdm_tbl_x0[TD2P_LR_LLS_LEN];
	int ovs_tdm_tbl_x0[TD2P_OS_LLS_GRP_LEN]; 
	int ovs_spacing_x0[TD2P_OS_LLS_GRP_LEN];
	int pgw_tdm_tbl_x1[TD2P_LR_LLS_LEN];
	int ovs_tdm_tbl_x1[TD2P_OS_LLS_GRP_LEN];
	int ovs_spacing_x1[TD2P_OS_LLS_GRP_LEN];
	int pgw_tdm_tbl_y0[TD2P_LR_LLS_LEN];
	int ovs_tdm_tbl_y0[TD2P_OS_LLS_GRP_LEN];
	int ovs_spacing_y0[TD2P_OS_LLS_GRP_LEN];
	int pgw_tdm_tbl_y1[TD2P_LR_LLS_LEN];
	int ovs_tdm_tbl_y1[TD2P_OS_LLS_GRP_LEN];
	int ovs_spacing_y1[TD2P_OS_LLS_GRP_LEN];
	int iarb_tdm_tbl_x[TD2P_LR_IARB_STATIC_LEN];
	int iarb_tdm_tbl_y[TD2P_LR_IARB_STATIC_LEN];
	int op_flags_x[NUM_OP_FLAGS];
	int op_flags_y[NUM_OP_FLAGS];
} td2p_tdm_pgw_t;

typedef struct {
	int tdm_pipe_main[TD2P_LR_VBS_LEN];
	int tdm_ovs_grp_0[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_1[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_2[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_3[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_4[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_5[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_6[TD2P_OS_VBS_GRP_LEN];
	int tdm_ovs_grp_7[TD2P_OS_VBS_GRP_LEN];
} tdm_pipe_t;

typedef struct {
	enum port_speed_e speed[TD2P_NUM_EXT_PORTS];
	int clk_freq;
	enum port_state_e port_rates_array[TD2P_NUM_EXT_PORTS];
	int pm_encap_type[TD2P_NUM_PM_MOD];
} tdm_globals_t;

typedef struct {
	int pmap[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS];
	tdm_pipe_t tdm_pipe_table_x;
	tdm_pipe_t tdm_pipe_table_y;
	tdm_globals_t tdm_globals;
	td2p_tdm_pgw_t tdm_pgw;
} tdm_chip_legacy_t;

#ifdef _TDM_STANDALONE
	#include <tdm_td2p_methods.h>
#else
	#include <soc/tdm/trident2p/tdm_td2p_methods.h>
#endif

/* API shim layer */
LINKER_DECL int tdm_chip_td2p_shim_core( tdm_mod_t *_tdm, tdm_chip_legacy_t *_tdm_td2p );
LINKER_DECL int tdm_chip_td2p_shim__ing_wrap( tdm_mod_t *_tdm, tdm_chip_legacy_t *_tdm_td2p );
LINKER_DECL int tdm_chip_td2p_shim__egr_wrap( tdm_mod_t *_tdm, tdm_chip_legacy_t *_tdm_td2p );
LINKER_DECL tdm_mod_t* tdm_chip_td2p_shim__which_tsc( int port, int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS] );
LINKER_DECL tdm_mod_t* tdm_chip_td2p_shim__check_ethernet( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS], int traffic[TD2P_NUM_PM_MOD] );
LINKER_DECL tdm_mod_t* tdm_chip_td2p_shim__check_ethernet_d( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int **tsc, int traffic[TD2P_NUM_PM_MOD] );
LINKER_DECL tdm_mod_t* tdm_chip_td2p_shim__scan_which_tsc_alloc( int port, int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS] );
LINKER_DECL int tdm_chip_td2p_shim__scan_which_tsc_free(tdm_mod_t *_tdm );
LINKER_DECL tdm_mod_t* tdm_chip_td2p_shim__check_pipe_ethernet_alloc( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int **tsc, int traffic[TD2P_NUM_PM_MOD] );
LINKER_DECL int tdm_chip_td2p_shim__check_pipe_ethernet_free(tdm_mod_t *_tdm);

#endif /* TDM_TD2P_PREPROCESSOR_DIRECTIVES_H */
