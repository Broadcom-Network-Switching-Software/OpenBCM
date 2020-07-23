/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for Firelight
 */

#ifndef TDM_FL_PREPROCESSOR_PROTOTYPES_H
#define TDM_FL_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_fl_soc.h>
#else
	#include <soc/tdm/firelight/tdm_fl_soc.h>
#endif


/* Chip executive polymorphic functions
        tdm_fl_main_init
        tdm_fl_main_transcription
        tdm_fl_main_ingress
        tdm_fl_filter
        tdm_fl_parse_tdm_tbl
        tdm_fl_chk
        tdm_fl_main_free
        tdm_fl_main_corereq
        tdm_fl_main_post
*/

/* Core executive polymorphic functions
        tdm_fl_proc_cal
        tdm_fl_shim_get_port_pm
        tdm_fl_scan_pipe_ethernet
*/

/* main */
LINKER_DECL int  tdm_fl_main_corereq(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_ingress_pipe(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_ingress(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_transcription(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_init(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_chk(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_post(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_main_free(tdm_mod_t *_tdm);

/* proc */
LINKER_DECL int  tdm_fl_proc_cal_ancl(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_proc_cal_idle(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_proc_cal_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_proc_cal(tdm_mod_t *_tdm);

/* filter */
LINKER_DECL int  tdm_fl_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm);
LINKER_DECL void tdm_fl_filter_calc_jitter(int speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL int  tdm_fl_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed);
LINKER_DECL int  tdm_fl_filter_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_filter(tdm_mod_t *_tdm);

/* scan */
LINKER_DECL int  tdm_fl_scan_port_pm(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_scan_pipe_ethernet(tdm_mod_t *_tdm);

/* parse */
LINKER_DECL void tdm_fl_print_pipe_config(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_print_lr_cal(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_parse_cmic(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_parse_ancl(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_parse_tdm_tbl(tdm_mod_t *_tdm);

/* check */
LINKER_DECL int  tdm_fl_chk_get_pipe_token_cnt(tdm_mod_t *_tdm, int cal_id, int port_token);
LINKER_DECL int  tdm_fl_chk_struct(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_transcription(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_sister(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_same(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_bandwidth_ancl(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_bandwidth_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_chk_bandwidth_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_bandwidth(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk_jitter_cmic(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_chk_jitter_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_chk(tdm_mod_t *_tdm);

/* shim */
LINKER_DECL int  tdm_fl_shim_get_port_pm(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_shim_get_pipe_ethernet(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_shim_core_vbs_scheduler_ovs(tdm_mod_t *_tdm);

/* common */
LINKER_DECL int  tdm_fl_cmn_print_pipe_lr_cal_matrix(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_fl_cmn_print_ms_cal_matrix(tdm_mod_t *_tdm);

LINKER_DECL int  tdm_fl_cmn_get_port_pm(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_port_speed(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_port_state(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_speed_lanes(int port_speed);
LINKER_DECL int  tdm_fl_cmn_get_speed_slots(int port_speed);
LINKER_DECL void tdm_fl_cmn_get_speed_jitter(int port_speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL void tdm_fl_cmn_get_pipe_port_range(int cal_id, int *phy_lo, int *phy_hi);
LINKER_DECL int  tdm_fl_cmn_get_pipe_ethernet(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_pipe_higig(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_pipe_cal_len_max(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_pipe_cal_len(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len);
LINKER_DECL int  tdm_fl_cmn_chk_port_is_lr(int port, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_fl_cmn_chk_port_is_os(int port, tdm_mod_t *_tdm);

/* TDM MacSec calendar API */
LINKER_DECL int  tdm_fl_macsec_proc(tdm_mod_t *_tdm);

#endif /* TDM_FL_PREPROCESSOR_PROTOTYPES_H */
