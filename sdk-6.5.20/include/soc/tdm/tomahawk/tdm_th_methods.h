/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56960
 */

#ifndef TDM_TH_PREPROCESSOR_PROTOTYPES_H
#define TDM_TH_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th_soc.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int tdm_th_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_scheduler_wrap_pipe( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_scheduler_wrap( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_proc_cal( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_parse_tdm_tbl( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_corereq( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_chk( tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_check_ethernet( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_pick_vec( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_free( tdm_mod_t *_tdm );

LINKER_DECL int tdm_th_proc_cal_tdm5( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter_tdm5( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_chk_tdm5( tdm_mod_t *_tdm);

/* Core executive polymorphic functions */
LINKER_DECL int tdm_th_which_tsc( tdm_mod_t *_tdm );
LINKER_DECL void tdm_th_parse_pipe( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk chip API */
LINKER_DECL int tdm_th_legacy_which_tsc(unsigned short port, int **tsc);
LINKER_DECL int tdm_th_proc_cal_ancl( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_proc_cal_acc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_proc_cal_idle( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_proc_cal_lr( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk chip based filter methods */
LINKER_DECL int tdm_th_filter_check_pipe_sister_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter_check_port_sister_min( tdm_mod_t *_tdm, int port );
LINKER_DECL int tdm_th_filter_check_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc);
LINKER_DECL int tdm_th_filter_sister_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_filter_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc);
LINKER_DECL int tdm_th_filter_migrate_lr_slot_up(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_migrate_lr_slot_dn(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_th_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir);
LINKER_DECL int tdm_th_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir);
LINKER_DECL int tdm_th_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min);
LINKER_DECL int tdm_th_filter_ovsb_p2(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_p1(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_5x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_4x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_3x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_2x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_1x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb(tdm_mod_t *_tdm);

LINKER_DECL int tdm_th_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_calc_jitter(int speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL int tdm_th_filter_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len);
LINKER_DECL int tdm_th_filter_get_port_pm(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed);
LINKER_DECL int tdm_th_filter_lr(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_4x_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_3x_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_2x_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_ovsb_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_filter_mix(tdm_mod_t *_tdm);

/* TDM.4 - Tomahawk chip based check/scan methods */
LINKER_DECL int tdm_th_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_th_slice_size_local(unsigned short idx, int *tdm, int lim);
LINKER_DECL int tdm_th_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_slice_size(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_th_slice_idx(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_th_slice_prox_local(unsigned short idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_th_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_th_scan_lr_slot_num(int *tdm_tbl);
LINKER_DECL int tdm_th_scan_slice_min(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_th_scan_slice_max(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_th_scan_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_th_scan_mix_slice_min(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_th_scan_mix_slice_max(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_th_scan_mix_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_th_check_shift_cond_pattern(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_th_check_shift_cond_local_slice(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_th_check_slot_swap_cond(int idx, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th_scan_which_tsc( int port, tdm_mod_t *_tdm );
LINKER_DECL int tdm_th_check_pipe_ethernet( int port, tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk linked list API */
LINKER_DECL void tdm_th_ll_print(struct th_ll_node *llist);
LINKER_DECL void tdm_th_ll_deref(struct th_ll_node *llist, int *tdm[TH_LR_VBS_LEN], int lim);
LINKER_DECL void tdm_th_ll_append(struct th_ll_node *llist, unsigned short port_append, int *pointer);
LINKER_DECL void tdm_th_ll_insert(struct th_ll_node *llist, unsigned short port_insert, int idx);
LINKER_DECL int tdm_th_ll_delete(struct th_ll_node *llist, int idx);
LINKER_DECL int tdm_th_ll_get(struct th_ll_node *llist, int idx);
LINKER_DECL int tdm_th_ll_len(struct th_ll_node *llist);
LINKER_DECL void tdm_th_ll_strip(struct th_ll_node *llist, int *pool, int token);
LINKER_DECL int tdm_th_ll_count(struct th_ll_node *llist, int token);
LINKER_DECL void tdm_th_ll_weave(struct th_ll_node *llist, int token, tdm_mod_t *_tdm);
LINKER_DECL void tdm_th_ll_retrace(struct th_ll_node *llist, tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_ll_single_100(struct th_ll_node *llist);
LINKER_DECL int tdm_th_ll_free(struct th_ll_node *llist);

/* TDM.5 - Tomahawk chip-specific checker */
LINKER_DECL int tdm_th_chk_get_port_tsc(tdm_mod_t *_tdm, int port);
LINKER_DECL int tdm_th_chk_get_cal_len(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_get_speed_slot_num(int port_speed);
LINKER_DECL int tdm_th_chk_get_cal_token_num(tdm_mod_t *_tdm, int cal_id, int port_token);
LINKER_DECL int tdm_th_chk_is_pipe_ethernet(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_struct_freq(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_struct_calLength(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_struct_speed(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_struct(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_transcription(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_sister(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_sub_cpu(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_sub_lpbk(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_sub_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_sub_os_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_sub_lr(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_sub_os(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_sub(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_jitter_os_pipe_tdm5(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_jitter_os_tdm5(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_jitter_lr_pipe_tdm5(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_jitter_lr_tdm5(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_jitter_os_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_jitter_os(tdm_mod_t *_tdm);
LINKER_DECL int tdm_th_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_th_chk_jitter_lr(tdm_mod_t *_tdm);

/* Print */
LINKER_DECL void tdm_th_print_tbl(tdm_mod_t *_tdm);
LINKER_DECL void tdm_th_print_config(tdm_mod_t *_tdm);

#endif /* TDM_TH_PREPROCESSOR_PROTOTYPES_H */
