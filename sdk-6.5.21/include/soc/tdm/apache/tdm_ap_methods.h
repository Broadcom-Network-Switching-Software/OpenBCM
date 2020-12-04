/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56860
 */

#ifndef TDM_AP_PREPROCESSOR_PROTOTYPES_H
#define TDM_AP_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_ap_soc.h>
#else
	#include <soc/tdm/apache/tdm_ap_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int tdm_ap_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_lls_wrapper( tdm_mod_t *_tdm );
LINKER_DECL void tdm_ap_mv_fb_ovs_gen(tdm_ap_chip_legacy_t *_ap_chip,int *ovs_tdm_tbl, int *ovs_spacing, int pgw_num);
LINKER_DECL void tdm_ap_mv_fb_ovs_tbl_spacing(int *ovs_tdm_tbl, int *ovs_spacing);
LINKER_DECL void tdm_ap_tdm_print(int *tdm_tbl, int length);
LINKER_DECL void tdm_ap_check_mv_fb_config(ap_pgw_os_tdm_sel_e *ap_os_tdm_sel, tdm_ap_chip_legacy_t *ap_chip);
LINKER_DECL void tdm_ap_mv_fb_ovs_tbl(tdm_ap_chip_legacy_t *ap_chip,int *ovs_tdm_tbl, unsigned int *pgw0_ovs_tbl_template,ap_pgw_os_tdm_sel_e *ap_pgw_os_tdm_sel, int pgw_num);
LINKER_DECL int tdm_ap_vbs_wrapper( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_extract_cal( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_free( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_chk( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_corereq( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_post( tdm_mod_t *_tdm );
/* Core executive polymorphic functions */
LINKER_DECL int tdm_ap_which_tsc( tdm_mod_t *_tdm_s );
LINKER_DECL int tdm_ap_vmap_alloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_vmap_alloc_old( tdm_mod_t *_tdm );
LINKER_DECL int tdm_ap_parse_mmu_tdm_tbl( tdm_mod_t *_tdm );

/* Internalized functions */
LINKER_DECL int tdm_ap_check_ethernet( tdm_mod_t *_tdm_s );

/* TDM.4 - APACHE linked list API */
LINKER_DECL void tdm_ap_ll_print(struct ap_ll_node *llist);
LINKER_DECL void tdm_ap_ll_deref(struct ap_ll_node *llist, int *tdm[AP_LR_LLS_LEN], int lim);
LINKER_DECL void tdm_ap_ll_append(struct ap_ll_node *llist, unsigned short port_append, int *pointer);
LINKER_DECL void tdm_ap_ll_insert(struct ap_ll_node *llist, unsigned short port_insert, int idx);
LINKER_DECL int tdm_ap_ll_delete(struct ap_ll_node *llist, int idx);
LINKER_DECL int tdm_ap_ll_get(struct ap_ll_node *llist, int idx);
LINKER_DECL int tdm_ap_ll_len(struct ap_ll_node *llist);
LINKER_DECL void tdm_ap_ll_strip(struct ap_ll_node *llist, int *pool, int token);
LINKER_DECL int tdm_ap_ll_count(struct ap_ll_node *llist, int token);
LINKER_DECL void tdm_ap_ll_weave(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int token);
LINKER_DECL void tdm_ap_ll_retrace(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS]);
LINKER_DECL int tdm_ap_ll_single_100(struct ap_ll_node *llist);
LINKER_DECL void tdm_ap_ll_retrace_25(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int port[2], enum port_speed_e speed[2]);
LINKER_DECL void tdm_ap_ll_retrace_cl(struct ap_ll_node *llist, tdm_ap_chip_legacy_t *ap_chip, int port[2], enum port_speed_e speed[2]);
LINKER_DECL void tdm_ap_ll_weave_100(struct ap_ll_node *llist, int wc_array[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int token);
LINKER_DECL void tdm_ap_ll_remove_25(struct ap_ll_node *llist, int port);
LINKER_DECL void tdm_ap_ll_append_25(struct ap_ll_node *llist, int token, int wc_array[AP_NUM_PM_LNS]);
LINKER_DECL void tdm_ap_ll_reconfig_8x25(struct ap_ll_node *llist, int token);
LINKER_DECL int tdm_ap_ll_free(struct ap_ll_node *llist);

/* TDM.4 - APACHE chip API */
LINKER_DECL int tdm_ap_lls_scheduler(struct ap_ll_node *pgw_tdm, tdm_ap_chip_legacy_t *ap_chip, ap_pgw_pntrs_t *pntrs_pkg, ap_pgw_scheduler_vars_t *vars_pkg, int *pgw_tdm_tbl[AP_LR_LLS_LEN], int *ovs_tdm_tbl[AP_OS_LLS_GRP_LEN], int op_flags[2], int *ap_fb_num_ovs, int *ap_fb_num_lr);
LINKER_DECL int tdm_ap_vbs_scheduler( tdm_mod_t *_tdm );
LINKER_DECL void tdm_ap_ovs_spacer(int *ovs_tdm_tbl, int *ovs_spacing);
LINKER_DECL int tdm_ap_legacy_which_tsc(unsigned short port, int **tsc);
LINKER_DECL int tdm_ap_actual_pmap( tdm_mod_t *_tdm );
LINKER_DECL void tdm_ap_reconfig_ovs_tbl(int *ovs_tdm_tbl);
LINKER_DECL void tdm_ap_reconfig_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2);
LINKER_DECL void tdm_ap_append_ovs_8x25(int *ovs_tdm_tbl, int port1, int port2, int cxx_port, enum port_speed_e cxx_speed);
LINKER_DECL void tdm_ap_reconfig_ovs_4x25(int *ovs_tdm_tbl, int port);
LINKER_DECL void tdm_ap_scheduler_ovs_4x25(int *ovs_tdm_tbl, int port);
LINKER_DECL void tdm_ap_ovs_20_40_clport(int *ovs_tdm_tbl, tdm_ap_chip_legacy_t *ap_chip);
LINKER_DECL void tdm_ap_clport_ovs_scheduler(int *ovs_tdm_tbl, int port[2], enum port_speed_e speed[2],int cxx_port, enum port_speed_e cxx_speed, int clk);
LINKER_DECL int tdm_postalloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports);
LINKER_DECL int tdm_acc_alloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, int lr_idx_limit, int num_ext_ports);
LINKER_DECL void tdm_ap_reconfig_pgw_tbl(int *cal, int port[2], enum port_speed_e speed[2]);

/* TDM.4 - APACHE chip based prints and parses */
LINKER_DECL void tdm_ap_print_tbl(int *cal, int len, const char* name, int id);
LINKER_DECL void tdm_ap_print_tbl_ovs(int *cal, int *spc, int len, const char* name, int id);
LINKER_DECL void tdm_ap_print_quad(enum port_speed_e *speed, enum port_state_e *state, int len, int idx_start, int idx_end);

/* TDM.4 - APACHE chip based checks and scans */
LINKER_DECL int tdm_ap_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_ap_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_ap_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_ap_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_ap_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_ap_slice_size_local(unsigned short idx, int *tdm, int lim);
LINKER_DECL int tdm_ap_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_slice_size(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_ap_slice_idx(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_ap_slice_prox_local(unsigned short idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_ap_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_num_lr_slots(int *tdm_tbl);
LINKER_DECL int tdm_ap_scan_slice_min(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_ap_scan_slice_max(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_ap_scan_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_ap_scan_mix_slice_min(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_ap_scan_mix_slice_max(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_ap_scan_mix_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_ap_check_shift_cond_pattern(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_ap_check_shift_cond_local_slice(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_ap_check_slot_swap_cond(int idx, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_scan_which_tsc(int port, int tsc[AP_NUM_PHY_PM][AP_NUM_PM_LNS]);

/* TDM.4 - APACHE chip based filter methods */
LINKER_DECL int tdm_ap_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_ap_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir);
LINKER_DECL int tdm_ap_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir);
LINKER_DECL int tdm_ap_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_ap_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min);

/* TDM.4 - APACHE chip specific IARB calendars */
LINKER_DECL void tdm_ap_init_iarb_tdm_table (int core_bw, int *iarb_tdm_wrap_ptr_lr_x, int *iarb_tdm_tbl_lr_x); 
LINKER_DECL int tdm_ap_set_iarb_tdm(int core_bw, int is_x_ovs, int *iarb_tdm_wrap_ptr_x, int *iarb_tdm_tbl_x);

/* TDM.4 - APACHE self-check operations */
LINKER_DECL int tdm_ap_chk_ethernet(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_struc(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_tsc(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_min(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_sub_lr( tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_sub_os(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_sub_cpu( tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_sub_lpbk( tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_sub_acc( tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_subscription(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_os_jitter(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_lr_jitter(tdm_mod_t *_tdm, int fail[16]);
LINKER_DECL int tdm_ap_chk_lls(tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_ovs_space( tdm_mod_t *_tdm);
LINKER_DECL int tdm_ap_chk_pgw_min( tdm_mod_t *_tdm);
LINKER_DECL void tdm_ap_chk_chip_specific(tdm_mod_t *_tdm, int fail[16]);

#endif /* TDM_AP_PREPROCESSOR_PROTOTYPES_H */
