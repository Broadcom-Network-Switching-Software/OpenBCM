/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56275
 */

#ifndef TDM_HR4_PREPROCESSOR_PROTOTYPES_H
#define TDM_HR4_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_hr4_soc.h>
#else
	#include <soc/tdm/hurricane4/tdm_hr4_soc.h>
#endif


/* Chip executive polymorphic functions 
        tdm_hr4_main_init
        tdm_hr4_main_transcription
        tdm_hr4_main_ingress
        tdm_hr4_filter
        tdm_hr4_parse_tdm_tbl
        tdm_hr4_chk
        tdm_hr4_main_free
        tdm_hr4_main_corereq
        tdm_hr4_main_post
*/

/* Core executive polymorphic functions 
        tdm_hr4_proc_cal
        tdm_hr4_shim_get_port_pm
        tdm_hr4_scan_pipe_ethernet
*/

/* main */
LINKER_DECL int  tdm_hr4_main_corereq(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_ingress_pipe(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_ingress(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_transcription(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_init(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_chk(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_post(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_main_free(tdm_mod_t *_tdm);

/* proc */
LINKER_DECL int  tdm_hr4_proc_cal_ancl(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_proc_cal_idle(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_proc_cal_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_proc_cal(tdm_mod_t *_tdm);

/* filter */
LINKER_DECL int tdm_hr4_same_port_1n6_filter( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_hr4_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm);
LINKER_DECL void tdm_hr4_filter_calc_jitter(int speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL int  tdm_hr4_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed);
LINKER_DECL int  tdm_hr4_filter_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_filter(tdm_mod_t *_tdm);

/* llist */
LINKER_DECL void tdm_hr4_ll_print(struct hr4_ll_node *llist);
LINKER_DECL int  tdm_hr4_ll_get_port_pm(int port, tdm_mod_t *_tdm);
LINKER_DECL void tdm_hr4_ll_deref(struct hr4_ll_node *llist, int *tdm[HR4_LR_VBS_LEN], int lim);
LINKER_DECL void tdm_hr4_ll_append(struct hr4_ll_node *llist, unsigned short port_append, int *pointer);
LINKER_DECL void tdm_hr4_ll_insert(struct hr4_ll_node *llist, unsigned short port_insert, int idx);
LINKER_DECL int  tdm_hr4_ll_delete(struct hr4_ll_node *llist, int idx);
LINKER_DECL int  tdm_hr4_ll_get(struct hr4_ll_node *llist, int idx);
LINKER_DECL int  tdm_hr4_ll_len(struct hr4_ll_node *llist);
LINKER_DECL void tdm_hr4_ll_strip(struct hr4_ll_node *llist, int *pool, int token);
LINKER_DECL int  tdm_hr4_ll_count(struct hr4_ll_node *llist, int token);
LINKER_DECL void tdm_hr4_ll_weave(struct hr4_ll_node *llist, int token, tdm_mod_t *_tdm);
LINKER_DECL void tdm_hr4_ll_retrace(struct hr4_ll_node *llist, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ll_free(struct hr4_ll_node *llist);

/* scan */
LINKER_DECL int  tdm_hr4_scan_port_pm(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_scan_pipe_ethernet(tdm_mod_t *_tdm);

/* hr4_vmap */
LINKER_DECL int  tdm_hr4_core_vmap_alloc_mix(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_core_vmap_prealloc(tdm_mod_t *_tdm);

/* ovsb */
LINKER_DECL int  tdm_hr4_ovsb_apply_constraints(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_part_halfpipe(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_fill_group(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_pkt_shaper(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_pkt_shaper_per_port(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_pkt_shaper_find_pm_indx(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_map_pm_num_to_pblk(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_flexmapping(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_ovsb_fixmapping(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_vbs_scheduler_ovs(tdm_mod_t *_tdm);

/* parse */
LINKER_DECL void tdm_hr4_print_pipe_config(tdm_mod_t *_tdm);
LINKER_DECL void tdm_hr4_print_pipe_cal(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_print_lr_cal(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_parse_mgmt(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_parse_acc_idb(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_parse_acc_mmu(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_parse_ancl(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_parse_tdm_tbl(tdm_mod_t *_tdm);

/* ovsb flexport */
LINKER_DECL int  tdm_hr4_flex_get_pm_flexport_state( tdm_mod_t *_tdm, enum hr4_flex_pm_state_e *pm_flexport_state);
LINKER_DECL int  tdm_hr4_flex_apply_constraints_flexport( tdm_mod_t *_tdm, enum hr4_flex_pm_state_e *pm_flexport_state);
LINKER_DECL int  tdm_hr4_flex_pkt_shaper_flex_port( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_hr4_flex_pkt_shaper_flexport_remove_port( tdm_mod_t *_tdm );
LINKER_DECL int  tdm_hr4_flex_map_pm_num_to_pblk_flex_port( tdm_mod_t *_tdm, enum hr4_flex_pm_state_e *pm_flexport_state );
LINKER_DECL int  tdm_hr4_flex_get_pm_hpipe( tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_update_grp_pms( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_init( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_evaluate( tdm_mod_t *_tdm, hr4_flxport_t *_flxport, int only_weight);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_remove( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_remove_port( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_consolidate( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_consolidate_speed( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_add_ports( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_add_ports_speed( tdm_mod_t *_tdm, hr4_flxport_t *_flxport );
LINKER_DECL int  tdm_hr4_flex_ovs_tables_add_pm( tdm_mod_t *_tdm, hr4_flxport_t *_flxport, int pm_num, int hp_indx );
LINKER_DECL int  tdm_hr4_sort_desc(int size, int *val_array, int *indx_array);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_rebalance( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_rebalance_speed( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_move_pm( tdm_mod_t *_tdm, hr4_flxport_t *_flxport, int src_grp, int dst_grp);
LINKER_DECL int  tdm_hr4_flex_ovs_tables_post( tdm_mod_t *_tdm, hr4_flxport_t *_flxport);
LINKER_DECL int  tdm_hr4_vbs_scheduler_ovs_flex_port_delta( tdm_mod_t *_tdm );
LINKER_DECL enum port_speed_e tdm_hr4_vbs_scheduler_ovs_indx_to_speed(enum hr4_port_speed_indx_e _speed_indx);
LINKER_DECL enum hr4_port_speed_indx_e tdm_hr4_vbs_scheduler_ovs_speed_to_indx(enum port_speed_e _speed);

/* check */
LINKER_DECL int  tdm_hr4_chk_get_speed_slots_5G(enum port_speed_e port_speed);
LINKER_DECL int  tdm_hr4_chk_get_pipe_token_cnt(tdm_mod_t *_tdm, int cal_id, int port_token);
LINKER_DECL int  tdm_hr4_chk_struct_os_ratio(tdm_mod_t *_tdm, int cal_id, int os_ratio_limit);
LINKER_DECL int  tdm_hr4_chk_struct(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_transcription(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_sister(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_same(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_bandwidth_ancl(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_bandwidth_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_chk_bandwidth_os_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_chk_bandwidth_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_bandwidth_os(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_bandwidth(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_jitter_cmic(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_chk_jitter_lr(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_chk_os_halfpipe(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_chk_os_pkt_sched(tdm_mod_t *_tdm, int cal_id);
LINKER_DECL int  tdm_hr4_chk(tdm_mod_t *_tdm);

/* shim */
LINKER_DECL int  tdm_hr4_shim_get_port_pm(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_shim_get_pipe_ethernet(tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_shim_core_vbs_scheduler_ovs(tdm_mod_t *_tdm);

/* common */
LINKER_DECL int  tdm_hr4_cmn_get_port_pm(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_port_speed(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_port_state(int port_token, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_speed_lanes(int port_speed);
LINKER_DECL int  tdm_hr4_cmn_get_speed_slots(int port_speed);
LINKER_DECL void tdm_hr4_cmn_get_speed_jitter(int port_speed, int cal_len, int *space_min, int *space_max);
LINKER_DECL void tdm_hr4_cmn_get_pipe_port_range(int cal_id, int *phy_lo, int *phy_hi);
LINKER_DECL int  tdm_hr4_cmn_get_pipe_ethernet(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_pipe_higig(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_pipe_cal_len_max(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_pipe_cal_len(int cal_id, tdm_mod_t *_tdm);
LINKER_DECL int  tdm_hr4_cmn_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len);

#endif /* TDM_HR4_PREPROCESSOR_PROTOTYPES_H */
