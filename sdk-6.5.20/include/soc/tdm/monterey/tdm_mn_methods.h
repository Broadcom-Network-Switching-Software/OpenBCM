/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56860
 */

#ifndef TDM_MN_PREPROCESSOR_PROTOTYPES_H
#define TDM_MN_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_mn_soc.h>
#else
	#include <soc/tdm/monterey/tdm_mn_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int  tdm_mn_init( tdm_mod_t *_tdm );
LINKER_DECL int   mn_is_clport(int pnum); 
LINKER_DECL int  tdm_mn_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_lls_wrapper( tdm_mod_t *_tdm );
LINKER_DECL void tdm_mn_chip2core_init( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_vbs_wrapper( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_extract_cal( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_free( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_corereq( tdm_mod_t *_tdm );
LINKER_DECL  int chk_tdm_mn_tbl( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_post( tdm_mod_t *_tdm );
LINKER_DECL void tdm_mn_falcon_32_40__ovs_gen(tdm_mn_chip_legacy_t *_mn_chip,int *ovs_tdm_tbl, int *ovs_spacing, int pgw_num, int * is_falcon_32_40, tdm_mod_t *_tdm);
LINKER_DECL void tdm_mn_check_mv_fb_config(mn_pgw_os_tdm_sel_e *mn_pgw_os_tdm_sel, tdm_mn_chip_legacy_t *_mn_chipi, tdm_mod_t *_tdm);
LINKER_DECL void tdm_mn_falcon_32_40_ovs_tbl(tdm_mn_chip_legacy_t *mn_chip,int *ovs_tdm_tbl, unsigned int *pgw0_ovs_tbl_template, mn_pgw_os_tdm_sel_e *mn_pgw_os_tdm_sel, int pgw_num, int * is_falcon_32_40);
LINKER_DECL void tdm_mn_ovs_tbl(tdm_mn_chip_legacy_t *mn_chip,int *ovs_tdm_tbl, unsigned int *pgw0_ovs_tbl_template, mn_pgw_os_tdm_sel_e *mn_pgw_os_tdm_sel, int pgw_num);
LINKER_DECL void tdm_mn_falcon_32_40_ovs_tbl_spacing(int *ovs_tdm_tbl, int *ovs_spacing);
LINKER_DECL void tdm_mn_ovs_tbl_spacing(int *ovs_tdm_tbl, int *ovs_spacing);
LINKER_DECL void  tdm_mn_custom( tdm_mod_t *_tdm );
LINKER_DECL void  tdm_mn_custom_vars_backup( tdm_mod_t *_tdm );
LINKER_DECL void  tdm_mn_custom_restore_vars(tdm_mod_t *_tdm);
LINKER_DECL void  tdm_mn_custom_post_process_pgw( tdm_mod_t *_tdm );
LINKER_DECL void tdm_mn_custom_post_process_mmu(tdm_mod_t *_tdm);
LINKER_DECL void  tdm_mn_tdm_smooth( tdm_mod_t *_tdm);
LINKER_DECL int  tdm_mn_parse_mmu_tdm_tbl( tdm_mod_t *_tdm );
LINKER_DECL void  tdm_mn_round_pgw_spacing( float *io_min, float *io_max );
/* Core executive polymorphic functions */
LINKER_DECL int  tdm_mn_which_tsc( tdm_mod_t *_tdm_s );
LINKER_DECL int  tdm_mn_vmap_alloc( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_vmap_alloc_old( tdm_mod_t *_tdm );

/* Internalized functions */
LINKER_DECL int tdm_mn_check_ethernet( tdm_mod_t *_tdm_s );

/* TDM.4 - APACHE linked list API */
LINKER_DECL void tdm_mn_ll_print(struct mn_ll_node *llist);
LINKER_DECL void tdm_mn_cal_pgw_bw( tdm_mod_t *_tdm, int *pgw_io_bw, int start_offset, int * high_speed_port_slots);
LINKER_DECL void tdm_mn_ll_deref(struct mn_ll_node *llist, int *tdm[MN_LR_LLS_LEN], int lim);
LINKER_DECL void tdm_mn_ll_append(struct mn_ll_node *llist, unsigned short port_append, int *pointer);
LINKER_DECL void tdm_mn_ll_insert(struct mn_ll_node *llist, unsigned short port_insert, int idx);
LINKER_DECL int  tdm_mn_ll_delete(struct mn_ll_node *llist, int idx);
LINKER_DECL int  tdm_mn_ll_get(struct mn_ll_node *llist, int idx);
LINKER_DECL int  tdm_mn_ll_len(struct mn_ll_node *llist);
LINKER_DECL void tdm_mn_ll_strip(struct mn_ll_node *llist, int *pool, int token);
LINKER_DECL int  tdm_mn_ll_count(struct mn_ll_node *llist, int token);
LINKER_DECL void tdm_mn_ll_weave(struct mn_ll_node *llist, int wc_array[MN_NUM_PHY_PM][MN_NUM_PM_LNS], int token);
LINKER_DECL void tdm_mn_ll_retrace(struct mn_ll_node *llist, int wc_array[MN_NUM_PHY_PM][MN_NUM_PM_LNS]);
LINKER_DECL int  tdm_mn_ll_single_100(struct mn_ll_node *llist);
LINKER_DECL void tdm_mn_ll_retrace_25(struct mn_ll_node *llist, int wc_array[MN_NUM_PHY_PM][MN_NUM_PM_LNS], int port[5], enum port_speed_e speed[5]);
LINKER_DECL void tdm_mn_ll_retrace_cl(struct mn_ll_node *llist, tdm_mn_chip_legacy_t *mn_chip, int port[5], enum port_speed_e speed[5], int io_bw_based_tdm);
LINKER_DECL void tdm_mn_ll_weave_100(struct mn_ll_node *llist, int wc_array[MN_NUM_PHY_PM][MN_NUM_PM_LNS], int token);
LINKER_DECL void tdm_mn_ll_remove_25(struct mn_ll_node *llist, int port);
LINKER_DECL void tdm_mn_ll_append_25(struct mn_ll_node *llist, int token);
LINKER_DECL void tdm_mn_ll_reconfig_8x25(struct mn_ll_node *llist, int token);
LINKER_DECL int  tdm_mn_ll_free(struct mn_ll_node *llist);

/* TDM.4 - APACHE chip API */
LINKER_DECL int  tdm_mn_vbs_scheduler( tdm_mod_t *_tdm );
LINKER_DECL void tdm_mn_ovs_spacer(int *ovs_tdm_tbl, int *ovs_spacing);
LINKER_DECL int  tdm_mn_legacy_which_tsc(unsigned short port, int **tsc);
LINKER_DECL int  tdm_mn_actual_pmap( tdm_mod_t *_tdm );
LINKER_DECL void tdm_mn_reconfig_ovs_tbl(int *ovs_tdm_tbl);
LINKER_DECL void tdm_mn_reconfig_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2);
LINKER_DECL void tdm_mn_append_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2, int cxx_port, enum port_speed_e cxx_speed);
LINKER_DECL void tdm_mn_reconfig_ovs_4x25(int *ovs_tdm_tbl, int port);
LINKER_DECL void tdm_mn_scheduler_ovs_4x25(int *ovs_tdm_tbl, int port);
LINKER_DECL void tdm_mn_ovs_20_40_clport(int *ovs_tdm_tbl, tdm_mn_chip_legacy_t *mn_chip);
LINKER_DECL void tdm_mn_clport_ovs_scheduler(int *ovs_tdm_tbl, int port[5], enum port_speed_e speed[5],int cxx_port, enum port_speed_e cxx_speed, int clk);
LINKER_DECL void tdm_mn_clport_ovs_scheduler_32x25(int *ovs_tdm_tbl, int port[5], enum port_speed_e speed[5],int cxx_port, enum port_speed_e cxx_speed, int clk);
LINKER_DECL int  mn_tdm_postalloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports);
LINKER_DECL int  mn_tdm_acc_alloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, int lr_idx_limit, int num_ext_ports);
LINKER_DECL void tdm_mn_reconfig_pgw_tbl(int *cal, int port[5], enum port_speed_e speed[5]);
LINKER_DECL int  tdm_mn_proc_cal_idle( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_proc_cal_lr( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_proc_cal( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_mn_proc_cal_ancl(tdm_mod_t *_tdm);

/* TDM.4 - APACHE chip based prints and parses */
LINKER_DECL void tdm_mn_print_tbl(int *cal, int len, const char* name, int id);
LINKER_DECL void tdm_mn_print_tbl_ovs(int *cal, int *spc, int len, const char* name, int id);
LINKER_DECL void tdm_mn_print_quad(enum port_speed_e *speed, enum port_state_e *state, int len, int idx_start, int idx_end);

/* TDM.4 - APACHE chip based checks and scans */
LINKER_DECL int tdm_mn_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_mn_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_mn_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_mn_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_mn_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_mn_slice_size_local(unsigned short idx, int *tdm, int lim);
LINKER_DECL int tdm_mn_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_slice_size(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_mn_slice_idx(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_mn_slice_prox_local(unsigned short idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_mn_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_num_lr_slots(int *tdm_tbl);
LINKER_DECL int tdm_mn_scan_slice_min(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_mn_scan_slice_max(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_mn_scan_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_mn_scan_mix_slice_min(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_mn_scan_mix_slice_max(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_mn_scan_mix_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_mn_check_shift_cond_pattern(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_mn_check_shift_cond_local_slice(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_mn_check_slot_swap_cond(int idx, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_scan_which_tsc(int port, int tsc[MN_NUM_PHY_PM][MN_NUM_PM_LNS]);

/* TDM.4 - APACHE chip based filter methods */
LINKER_DECL int tdm_mn_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_mn_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir);
LINKER_DECL int tdm_mn_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir);
LINKER_DECL int tdm_mn_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_mn_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min);

/* TDM.4 - APACHE chip specific IARB calendars */
LINKER_DECL void tdm_mn_init_iarb_tdm_table (int core_bw, int *iarb_tdm_wrap_ptr_lr_x, int *iarb_tdm_tbl_lr_x, int sku_core_bw, int os_config, int is_macsec); 
LINKER_DECL int  tdm_mn_set_iarb_tdm(int core_bw, int is_x_ovs, int *iarb_tdm_wrap_ptr_x, int *iarb_tdm_tbl_x,int sku_core_bw,int is_macsec);


#endif /* TDM_mn_PREPROCESSOR_PROTOTYPES_H */
