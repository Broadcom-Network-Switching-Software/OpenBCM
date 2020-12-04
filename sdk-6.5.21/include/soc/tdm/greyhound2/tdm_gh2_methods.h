/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM53570
 */

#ifndef TDM_GH2_PREPROCESSOR_PROTOTYPES_H
#define TDM_GH2_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_gh2_soc.h>
#else
	#include <soc/tdm/greyhound2/tdm_gh2_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int tdm_gh2_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_scheduler_wrap_pipe( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_scheduler_wrap( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_proc_cal( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_parse_tdm_tbl( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_corereq( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_filter( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_chk( tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_check_ethernet( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_pick_vec( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_free( tdm_mod_t *_tdm );

LINKER_DECL int tdm_gh2_proc_cal_tdm5( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_filter_tdm5( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_chk_tdm5( tdm_mod_t *_tdm);

/* Core executive polymorphic functions */
LINKER_DECL int tdm_gh2_which_tsc( tdm_mod_t *_tdm );
LINKER_DECL void tdm_gh2_parse_pipe( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk chip API */
LINKER_DECL int tdm_gh2_legacy_which_tsc(unsigned short port, int **tsc);
LINKER_DECL int tdm_gh2_proc_cal_ancl( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_proc_cal_acc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_proc_cal_idle( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_proc_cal_lr( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk chip based filter methods */
LINKER_DECL int tdm_gh2_filter_check_pipe_sister_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_filter_check_port_sister_min( tdm_mod_t *_tdm, int port );
LINKER_DECL int tdm_gh2_filter_check_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc);
LINKER_DECL int tdm_gh2_filter_sister_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_filter_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc);
LINKER_DECL int tdm_gh2_filter_migrate_lr_slot_up(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_migrate_lr_slot_dn(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_gh2_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir);
LINKER_DECL int tdm_gh2_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir);
LINKER_DECL int tdm_gh2_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min);
LINKER_DECL int tdm_gh2_filter_ovsb_p2(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_p1(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_5x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_4x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_3x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_2x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_1x(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb(tdm_mod_t *_tdm);

LINKER_DECL int tdm_gh2_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_calc_jitter(int speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL int tdm_gh2_filter_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len);
LINKER_DECL int tdm_gh2_filter_get_port_pm(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed);
LINKER_DECL int tdm_gh2_filter_lr(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_4x_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_3x_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_2x_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_ovsb_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_filter_mix(tdm_mod_t *_tdm);

/* TDM.4 - Tomahawk chip based check/scan methods */
LINKER_DECL int tdm_gh2_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_gh2_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_gh2_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_gh2_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_gh2_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_gh2_slice_size_local(unsigned short idx, int *tdm, int lim);
LINKER_DECL int tdm_gh2_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_slice_size(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_gh2_slice_idx(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_gh2_slice_prox_local(unsigned short idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_gh2_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_scan_lr_slot_num(int *tdm_tbl);
LINKER_DECL int tdm_gh2_scan_slice_min(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_gh2_scan_slice_max(unsigned short port, int *tdm, int lim, int *slice_start_idx, int pos);
LINKER_DECL int tdm_gh2_scan_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_gh2_scan_mix_slice_min(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_gh2_scan_mix_slice_max(unsigned short port, int *tdm, int lim, int *slice_idx, int pos);
LINKER_DECL int tdm_gh2_scan_mix_slice_size_local(unsigned short idx, int *tdm, int lim, int *slice_start_idx);
LINKER_DECL int tdm_gh2_check_shift_cond_pattern(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_gh2_check_shift_cond_local_slice(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int **tsc, int dir);
LINKER_DECL int tdm_gh2_check_slot_swap_cond(int idx, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_gh2_scan_which_tsc( int port, tdm_mod_t *_tdm );
LINKER_DECL int tdm_gh2_check_pipe_ethernet( int port, tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk linked list API */
LINKER_DECL void tdm_gh2_ll_print(struct gh2_ll_node *llist);
LINKER_DECL void tdm_gh2_ll_deref(struct gh2_ll_node *llist, int *tdm[GH2_LR_VBS_LEN], int lim);
LINKER_DECL void tdm_gh2_ll_append(struct gh2_ll_node *llist, unsigned short port_append, int *pointer);
LINKER_DECL void tdm_gh2_ll_insert(struct gh2_ll_node *llist, unsigned short port_insert, int idx);
LINKER_DECL int tdm_gh2_ll_delete(struct gh2_ll_node *llist, int idx);
LINKER_DECL int tdm_gh2_ll_get(struct gh2_ll_node *llist, int idx);
LINKER_DECL int tdm_gh2_ll_len(struct gh2_ll_node *llist);
LINKER_DECL void tdm_gh2_ll_strip(struct gh2_ll_node *llist, int *pool, int token);
LINKER_DECL int tdm_gh2_ll_count(struct gh2_ll_node *llist, int token);
LINKER_DECL void tdm_gh2_ll_weave(struct gh2_ll_node *llist, int token, tdm_mod_t *_tdm);
LINKER_DECL void tdm_gh2_ll_retrace(struct gh2_ll_node *llist, tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_ll_single_100(struct gh2_ll_node *llist);
LINKER_DECL int tdm_gh2_ll_free(struct gh2_ll_node *llist);

/* TDM.5 - Tomahawk chip-specific checker */
LINKER_DECL int tdm_gh2_chk_get_port_tsc(tdm_mod_t *_tdm, int port);
LINKER_DECL int tdm_gh2_chk_get_cal_len(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_get_speed_slot_num(int port_speed);
LINKER_DECL int tdm_gh2_chk_get_cal_token_num(tdm_mod_t *_tdm, int cal_id, int port_token);
LINKER_DECL int tdm_gh2_chk_is_pipe_ethernet(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_struct_freq(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_struct_calLength(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_struct_speed(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_struct(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_transcription(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_sister(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_sub_cpu(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_sub_lpbk(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_sub_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_sub_os_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_sub_lr(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_sub_os(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_sub(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_jitter_os_pipe_tdm5(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_jitter_os_tdm5(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_jitter_lr_pipe_tdm5(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_jitter_lr_tdm5(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_jitter_os_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_jitter_os(tdm_mod_t *_tdm);
LINKER_DECL int tdm_gh2_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int tdm_gh2_chk_jitter_lr(tdm_mod_t *_tdm);

/* Print */
LINKER_DECL void tdm_gh2_print_tbl(tdm_mod_t *_tdm);
LINKER_DECL void tdm_gh2_print_config(tdm_mod_t *_tdm);

#endif /* TDM_GH2_PREPROCESSOR_PROTOTYPES_H */
