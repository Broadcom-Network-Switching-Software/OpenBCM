/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56870
 */

#ifndef TDM_TD3_PREPROCESSOR_PROTOTYPES_H
#define TDM_TD3_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_td3_soc.h>
#else
	#include <soc/tdm/trident3/tdm_td3_soc.h>
#endif


/* Chip executive polymorphic functions 
        tdm_td3_main_init
        tdm_td3_main_transcription
        tdm_td3_main_ingress
        tdm_td3_filter
        tdm_td3_parse_tdm_tbl
        tdm_td3_chk
        tdm_td3_main_free
        tdm_td3_main_corereq
        tdm_td3_main_post
*/

/* Core executive polymorphic functions 
        tdm_td3_proc_cal
        tdm_td3_shim_get_port_pm
*/

/* main */
LINKER_DECL int  tdm_td3_main_corereq(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_ingress_pipe(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_ingress(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_transcription(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_init(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_chk(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_post(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_main_free(tdm_mod_t *_tdm);

/* proc */
LINKER_DECL int  tdm_td3_proc_cal_ancl(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_proc_cal_idle(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_proc_cal_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_proc_cal(tdm_mod_t *_tdm);

/* filter */
LINKER_DECL int  tdm_td3_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm);
LINKER_DECL void tdm_td3_filter_calc_jitter(int speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL int  tdm_td3_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed);
LINKER_DECL int  tdm_td3_filter_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_filter(tdm_mod_t *_tdm);

/* ovsb */
LINKER_DECL int  tdm_td3_ovsb_apply_constraints_init(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_apply_constraints_flex(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_sel_grp_4_prt(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_ovsb_gen_os_grps_init(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_gen_os_grps_flex(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_gen_os_grps(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_gen_pkt_shaper_init(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_gen_pkt_shaper_flex(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_gen_pkt_shaper(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_gen_pm2pblk_map(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_ovsb_consolidate_spipe(tdm_mod_t *_tdm, int spipe);
LINKER_DECL int  tdm_td3_vbs_scheduler_ovs(tdm_mod_t *_tdm);

/* parse */
LINKER_DECL void tdm_td3_print_pipe_config(tdm_mod_t *_tdm);
LINKER_DECL void tdm_td3_print_pipe_cal(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_print_lr_cal(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_parse_mgmt(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_parse_acc_idb(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_parse_acc_mmu(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_parse_ancl(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_parse_tdm_tbl(tdm_mod_t *_tdm);

/* check */
LINKER_DECL int  tdm_td3_chk_get_speed_slots_5G(enum port_speed_e port_speed);
LINKER_DECL int  tdm_td3_chk_get_pipe_token_cnt(tdm_mod_t *_tdm, int cal_id, int port_token);
LINKER_DECL int  tdm_td3_chk_struct_os_ratio(tdm_mod_t *_tdm, int cal_id, int os_ratio_limit);
LINKER_DECL int  tdm_td3_chk_struct(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_transcription(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_sister(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_same(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_bandwidth_ancl(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_bandwidth_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_chk_bandwidth_os_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_chk_bandwidth_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_bandwidth_os(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_bandwidth(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_jitter_cmic(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_chk_jitter_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_chk_os_halfpipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_chk_os_pkt_sched(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_chk(tdm_mod_t *_tdm);

/* shim */
LINKER_DECL int  tdm_td3_shim_get_port_pm(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_td3_shim_get_pipe_ethernet(tdm_mod_t *_tdm);

/* common */
LINKER_DECL int  tdm_td3_cmn_chk_port_is_fp(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_chk_port_is_lr(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_chk_port_is_os(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_chk_port_is_hg(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_chk_port_is_mgmt(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_get_port_pm(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_get_port_speed(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_get_port_speed_eth(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_get_port_state(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int  tdm_td3_cmn_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len);
LINKER_DECL int  tdm_td3_cmn_get_speed_lanes(int port_speed);
LINKER_DECL int  tdm_td3_cmn_get_speed_slots(int port_speed);
LINKER_DECL int  tdm_td3_cmn_get_port_slots(tdm_mod_t *_tdm, int port_token);
LINKER_DECL void tdm_td3_cmn_get_speed_jitter(int port_speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL void tdm_td3_cmn_get_pipe_port_range(int cal_id, int *phy_lo, int *phy_hi);
LINKER_DECL void tdm_td3_cmn_get_pipe_port_lo_hi(tdm_mod_t *_tdm, int *phy_lo, int *phy_hi);
LINKER_DECL void tdm_td3_cmn_get_pipe_pm_lo_hi(tdm_mod_t *_tdm, int *pm_lo, int *pm_hi);
LINKER_DECL int  tdm_td3_cmn_get_pipe_ethernet(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_cmn_get_pipe_higig(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_cmn_get_pipe_cal_len_max(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_td3_cmn_get_pipe_cal_len(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL tdm_calendar_t *tdm_td3_cmn_get_pipe_cal(tdm_mod_t *_tdm);
LINKER_DECL tdm_calendar_t *tdm_td3_cmn_get_pipe_cal_prev(tdm_mod_t *_tdm);


#endif /* TDM_TD3_PREPROCESSOR_PROTOTYPES_H */
