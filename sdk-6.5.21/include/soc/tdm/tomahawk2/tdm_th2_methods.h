/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for BCM56970
 */

#ifndef TDM_TH2_PREPROCESSOR_PROTOTYPES_H
#define TDM_TH2_PREPROCESSOR_PROTOTYPES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th2_soc.h>
#else
	#include <soc/tdm/tomahawk2/tdm_th2_soc.h>
#endif

/*
 * CHIP function prototypes 
 */

/* Functions managed by TDM chip executive */
LINKER_DECL int tdm_th2_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_dealloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_alloc_prev_chip_data( tdm_mod_t *_tdm_pkg );
LINKER_DECL int tdm_th2_pmap_transcription( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_scheduler_wrap( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_vmap_alloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_parse_tdm_tbl( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_parse_tdm_tbl_new( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_corereq( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_filter_chain( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_pick_vec( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_vbs_scheduler( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_vbs_scheduler_ovs( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_acc_alloc( tdm_mod_t *_tdm );

/* Core executive polymorphic functions */
LINKER_DECL int tdm_th2_which_tsc( tdm_mod_t *_tdm );
LINKER_DECL void tdm_th2_parse_pipe( tdm_mod_t *_tdm );

/* Internalized functions */
LINKER_DECL int tdm_th2_check_ethernet( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_apply_constraints( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_part_halfpipe( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_fill_group( tdm_mod_t *_tdm);
LINKER_DECL int tdm_th2_ovs_pkt_shaper( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_pkt_shaper_per_port( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_pkt_shaper_find_pm_indx( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_map_pm_num_to_pblk( tdm_mod_t *_tdm );

/* TDM.4 - Tomahawk2 chip API */
LINKER_DECL int tdm_th2_legacy_which_tsc(unsigned short port, int **tsc);
LINKER_DECL int tdm_th2_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports);

LINKER_DECL int tdm_th2_vmap_alloc_v2( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_get_pm_speed_and_place_table( enum port_speed_e *_pm_speeds, int num_lanes, int *place_tbl,  int *place_table_length);
LINKER_DECL int tdm_th2_find_first_avail_slot(int *tdm_pipe_main, int pm_slot_distance, int avail_token);
LINKER_DECL int tdm_th2_find_first_avail_slot_w_start(int *tdm_pipe_main, int tdm_cal_length, int avail_token, int start_pos);
LINKER_DECL int tdm_th2_place_lr_port(tdm_mod_t *_tdm, int *tdm_pipe_main, int phy_port, int ideal_pos, int port_jitter, int tdm_cal_length, int avail_token);
LINKER_DECL int tdm_th2_vbs_scheduler_place_pm( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_div_round_up(int nominator, int denominator);
LINKER_DECL int tdm_th2_div_round_down(int nominator, int denominator);
LINKER_DECL int tdm_th2_div_round(int nominator, int denominator);
LINKER_DECL int tdm_th2_get_min_max_jitter(int tbl_len, int port_slots, int lr_jitter_pct, int *min_spacing, int *max_spacing);
LINKER_DECL int tdm_th2_shift_left(tdm_mod_t *_tdm, int *tdm_pipe_main, int tdm_cal_length, int phy_port, int ideal_pos, int shift_jitter, int left_slice_length, int avail_token, int lr_jitter_pct, int try_en, int *is_sist_fail);
LINKER_DECL int tdm_th2_shift_right(tdm_mod_t *_tdm, int *tdm_pipe_main, int tdm_cal_length, int phy_port, int ideal_pos, int shift_jitter, int right_slice_length, int avail_token, int lr_jitter_pct, int try_en, int *is_sist_fail);
LINKER_DECL int tdm_th2_check_swap(tdm_mod_t *_tdm, int *tdm_pipe_main, int tbl_len, int pos_1, int pos_2, int avail_token, int lr_jitter_pct);
LINKER_DECL int tdm_th2_check_lr_placement(tdm_mod_t *_tdm, int *tdm_pipe_main, int tbl_len, int phy_port, int pos, int lr_jitter_pct, int avail_token);


/* FlexPort */
LINKER_DECL int tdm_th2_vbs_scheduler_ovs_flex_port( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_get_pm_flexport_state( tdm_mod_t *_tdm, enum flexport_pm_state_e *pm_flexport_state);
LINKER_DECL int tdm_th2_ovs_apply_constraints_flexport( tdm_mod_t *_tdm, enum flexport_pm_state_e *pm_flexport_state);
LINKER_DECL int tdm_th2_ovs_flexport_find_hp( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_pkt_shaper_flex_port( tdm_mod_t *_tdm );
LINKER_DECL int tdm_th2_ovs_pkt_shaper_flexport_remove_port( tdm_mod_t *_tdm );

/* OVS  tables */
LINKER_DECL int tdm_th2_flex_ovs_tables_init( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_evaluate( tdm_mod_t *_tdm, th2_flxport_t *_flxport, int only_weight);
LINKER_DECL int tdm_th2_flex_ovs_tables_remove( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_remove_port( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_consolidate( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_consolidate_speed( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_add_ports( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_add_ports_speed( tdm_mod_t *_tdm, th2_flxport_t *_flxport );
LINKER_DECL int tdm_th2_flex_ovs_tables_add_pm( tdm_mod_t *_tdm, th2_flxport_t *_flxport, int pm_num, int hp_indx );
LINKER_DECL int tdm_th2_sort_desc(int size, int *val_array, int *indx_array);
LINKER_DECL int tdm_th2_flex_ovs_tables_rebalance( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_rebalance_speed( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_move_pm( tdm_mod_t *_tdm, th2_flxport_t *_flxport, int src_grp, int dst_grp);
LINKER_DECL int tdm_th2_flex_ovs_tables_post( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_vbs_scheduler_ovs_flex_port_delta( tdm_mod_t *_tdm );
LINKER_DECL enum port_speed_e        tdm_th2_vbs_scheduler_ovs_indx_to_speed(enum port_speed_indx_e _speed_indx);
LINKER_DECL enum port_speed_indx_e   tdm_th2_vbs_scheduler_ovs_speed_to_indx(enum port_speed_e _speed);

LINKER_DECL int tdm_th2_print_th2_flxport_t( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_flex_ovs_tables_update_grp_pms( tdm_mod_t *_tdm, th2_flxport_t *_flxport);
LINKER_DECL int tdm_th2_ovs_map_pm_num_to_pblk_flex_port( tdm_mod_t *_tdm, enum flexport_pm_state_e *pm_flexport_state );

/* TDM.4 - Tomahawk2 chip based filter methods */
LINKER_DECL int tdm_th2_filter_vector_shift(int *tdm_tbl, int port, int dir);
LINKER_DECL void tdm_th2_filter_local_slice_dn(int port, int *tdm_tbl, int **tsc, enum port_speed_e *speed);
LINKER_DECL void tdm_th2_filter_local_slice_up(int port, int *tdm_tbl, int **tsc, enum port_speed_e *speed);
LINKER_DECL void tdm_th2_filter_ancillary_smooth(int port, int *tdm_tbl, int lr_idx_limit, int accessories);
LINKER_DECL int tdm_th2_filter_slice_dn(int port, int *tdm_tbl, int lr_idx_limit, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th2_filter_slice_up(int port, int *tdm_tbl, int lr_idx_limit, int **tsc, enum port_speed_e *speed);
LINKER_DECL void tdm_th2_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed);
LINKER_DECL int tdm_th2_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc);
LINKER_DECL int tdm_th2_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th2_check_same_port_dist_up(int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th2_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th2_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim);
LINKER_DECL int tdm_th2_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh);
LINKER_DECL int tdm_th2_slice_size_local(unsigned short idx, int *tdm, int lim);
LINKER_DECL int tdm_th2_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th2_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed);
LINKER_DECL int tdm_th2_slice_size(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_th2_slice_idx(unsigned short port, int *tdm, int lim);
LINKER_DECL int tdm_th2_slice_prox_local(unsigned short idx, int *tdm, int lim, int **tsc);
LINKER_DECL int tdm_th2_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed);
LINKER_DECL int tdm_th2_num_lr_slots(int *tdm_tbl);


#endif /* TDM_TH2_PREPROCESSOR_PROTOTYPES_H */
