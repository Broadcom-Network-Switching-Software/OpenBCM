/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM function prototypes for core scheduler
 */

#ifndef TDM_PREPROCESSOR_PROTOTYPES_H
#define TDM_PREPROCESSOR_PROTOTYPES_H
/*
 * CORE function prototypes 
 */

/* Class functions managed by TDM core executive */
LINKER_DECL int tdm_core_init( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_post( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vbs_scheduler( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vbs_scheduler_lr( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vbs_scheduler_alloc_lr( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vbs_scheduler_ovs( tdm_mod_t *_tdm );
LINKER_DECL int tdm_pick_vec( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_pm( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_acc_alloc( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_null( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_vmap_prealloc(tdm_mod_t *_tdm);
LINKER_DECL int tdm_core_vmap_alloc(tdm_mod_t *_tdm);
LINKER_DECL int tdm_core_vmap_alloc_mix(tdm_mod_t *_tdm);
LINKER_DECL int tdm_core_vbs_scheduler_wrapper(tdm_mod_t *_tdm);
LINKER_DECL int tdm_core_ovsb_alloc(tdm_mod_t *_tdm);

/* Internal functions that can use class-heritable data */
LINKER_DECL void tdm_mem_transpose( tdm_mod_t *_tdm );
LINKER_DECL void tdm_sticky_transpose( tdm_mod_t *_tdm );
LINKER_DECL int tdm_type_chk( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_fastest_triport( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_fastest_port( tdm_mod_t *_tdm );
LINKER_DECL int tdm_find_fastest_spd( tdm_mod_t *_tdm );
LINKER_DECL int tdm_map_find_y_indx( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_rotate( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_clear( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fit_singular_col( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fit_prox( tdm_mod_t *_tdm );
LINKER_DECL int tdm_count_nonsingular( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fit_row_min( tdm_mod_t *_tdm );
LINKER_DECL int tdm_count_param_spd( tdm_mod_t *_tdm );
LINKER_DECL int tdm_nsin_row( tdm_mod_t *_tdm );
LINKER_DECL int tdm_core_filter_refactor( tdm_mod_t *_tdm );
LINKER_DECL int tdm_fill_ovs( tdm_mod_t *_tdm );
LINKER_DECL void tdm_col_transpose( tdm_mod_t *_tdm );

/* Linked list API partially merged with class method */
LINKER_DECL int tdm_ll_retrace(struct node *llist, tdm_mod_t *_tdm, int cadence_start_idx);
LINKER_DECL int tdm_ll_tsc_dist(struct node *llist, tdm_mod_t *_tdm, int idx);
LINKER_DECL void tdm_ll_strip(struct node *llist, tdm_mod_t *_tdm, int cadence_start_idx, int *pool, int *s_idx, int token);

/* Internal functions managed entirely by compiler */
LINKER_DECL void tdm_core_prealloc(unsigned short stack[TDM_AUX_SIZE], int buffer[TDM_AUX_SIZE], short *x, char *boolstr, int j);
LINKER_DECL int tdm_core_postalloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports);
LINKER_DECL int tdm_core_postalloc_vmap(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports, int vmap_wid, int vmap_len);
LINKER_DECL int tdm_slots(int port_speed);
LINKER_DECL int tdm_arr_exists(int element, int len, int arr[ARR_FIXED_LEN]);
LINKER_DECL int tdm_arr_append(int element, int len, int arr[ARR_FIXED_LEN], int num_ext_ports);
LINKER_DECL int tdm_empty_row(unsigned short **map, unsigned short y_idx, int num_ext_ports, int vec_map_wid);
LINKER_DECL void tdm_vector_rotate_step(unsigned short vector[], int size, int step);
LINKER_DECL int tdm_slice_size_2d(unsigned short **map, unsigned short y_idx, int lim, int num_ext_ports, int vec_map_wid);
LINKER_DECL int tdm_fit_singular_cnt(unsigned short **map, int node_y, int vec_map_wid, int num_ext_ports);
LINKER_DECL int tdm_map_cadence_count(unsigned short *vector, int idx, int vec_map_len);
LINKER_DECL int tdm_map_retrace_count(unsigned short **map, int x_idx, int y_idx, int vec_map_len, int vec_map_wid, int num_ext_ports);
LINKER_DECL int tdm_fill_ovs_simple(short *z, unsigned short ovs_buf[TDM_AUX_SIZE], int *bucket1, unsigned short *z11, int *bucket2, unsigned short *z22, int *bucket3, unsigned short *z33, int *bucket4, unsigned short *z44, int *bucket5, unsigned short *z55, int *bucket6, unsigned short *z66, int *bucket7, unsigned short *z77, int *bucket8, unsigned short *z88, int grp_len);

/* Linked list API managed entirely by compiler */
LINKER_DECL void tdm_ll_append(struct node *llist, unsigned short port_append);
LINKER_DECL void tdm_ll_insert(struct node *llist, unsigned short port_insert, int idx);
LINKER_DECL void tdm_ll_print(struct node *llist);
LINKER_DECL void tdm_ll_deref(struct node *llist, int *tdm, int lim);
LINKER_DECL int tdm_ll_delete(struct node *llist, int idx);
LINKER_DECL int tdm_ll_get(struct node *llist, int idx, int num_ext_ports);
LINKER_DECL int tdm_ll_dist(struct node *llist, int idx);
LINKER_DECL int tdm_ll_free(struct node *llist);

/* PARSE/PRINT API */
LINKER_DECL void tdm_class_data( tdm_mod_t *_tdm );
LINKER_DECL void tdm_class_output( tdm_mod_t *_tdm );
LINKER_DECL void tdm_print_stat( tdm_mod_t *_tdm );
LINKER_DECL void tdm_print_config( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_dump( tdm_mod_t *_tdm );
LINKER_DECL void tdm_print_vmap_vector( tdm_mod_t *_tdm );
LINKER_DECL void tdm_vector_zrow( tdm_mod_t *_tdm );

/* CMATH API */
LINKER_DECL int tdm_PQ(int f);
LINKER_DECL int tdm_sqrt(int input_signed);
LINKER_DECL int tdm_pow(int num, int pow);
LINKER_DECL int tdm_fac(int num_signed);
LINKER_DECL int tdm_abs(int num);
LINKER_DECL int tdm_math_div_ceil(int a, int b);
LINKER_DECL int tdm_math_div_floor(int a, int b);
LINKER_DECL int tdm_math_div_round(int a, int b);

/* TDM VMAP */
LINKER_DECL void tdm_vmap_print(tdm_mod_t *_tdm, unsigned short **vmap);
LINKER_DECL int tdm_vmap_chk_singularity(tdm_mod_t *_tdm, unsigned short **vmap);
LINKER_DECL int tdm_vmap_chk_sister(tdm_mod_t *_tdm);
LINKER_DECL int tdm_vmap_chk_same(tdm_mod_t *_tdm);
LINKER_DECL int tdm_vmap_chk_vmap_sister_xy(tdm_mod_t *_tdm, unsigned short **vmap, int col, int row);
LINKER_DECL int tdm_vmap_chk_vmap_sister_col(tdm_mod_t *_tdm, unsigned short **vmap, int col);
LINKER_DECL int tdm_vmap_chk_vmap_sister(tdm_mod_t *_tdm, unsigned short **vmap);
LINKER_DECL int tdm_vmap_get_port_pm(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_vmap_get_port_speed(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_vmap_get_speed_slots(tdm_mod_t *_tdm, int port_speed);
LINKER_DECL int tdm_vmap_get_port_slots(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_vmap_get_vmap_wid(tdm_mod_t *_tdm, unsigned short **vmap);
LINKER_DECL int tdm_vmap_get_vmap_len(tdm_mod_t *_tdm, unsigned short **vmap);
LINKER_DECL int tdm_vmap_get_port_space_sister(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_vmap_get_port_space_same(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_vmap_calc_port_dist_sister(tdm_mod_t *_tdm, int *cal, int cal_len, int espace, int idx, int dir);
LINKER_DECL int tdm_vmap_calc_port_dist_same(tdm_mod_t *_tdm, int *cal, int cal_len, int espace, int idx, int dir);
LINKER_DECL int tdm_vmap_switch_vmap_col(tdm_mod_t *_tdm, unsigned short **vmap, int col_x, int col_y);
LINKER_DECL int tdm_vmap_rotate_vmap(tdm_mod_t *_tdm, unsigned short **vmap);
LINKER_DECL int tdm_vmap_shift_port(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_vmap_filter_sister(tdm_mod_t *_tdm);
LINKER_DECL int tdm_vmap_filter_same(tdm_mod_t *_tdm);
LINKER_DECL int tdm_vmap_calc_slot_pos(int vmap_idx, int slot_idx, int vmap_idx_max, int slot_idx_max, int **r_a_arr);
LINKER_DECL int tdm_vmap_alloc_one_port(int port_token, int slot_req, int slot_left, int cal_len, int vmap_idx, int vmap_wid_max, int vmap_len_max, unsigned short **vmap, int **r_a_arr);
LINKER_DECL int tdm_vmap_alloc(tdm_mod_t *_tdm);
LINKER_DECL int tdm_vmap_alloc_mix(tdm_mod_t *_tdm);

/* TDM common API */
LINKER_DECL int tdm_cmn_chk_port_is_fp(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_cmn_chk_pipe_os_spd_types(tdm_mod_t *_tdm);
LINKER_DECL int tdm_cmn_get_port_pm(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_cmn_get_port_speed(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_cmn_get_port_speed_eth(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_cmn_get_port_speed_idx(tdm_mod_t *_tdm, int port_token);
LINKER_DECL int tdm_cmn_get_slot_unit(tdm_mod_t *_tdm);
LINKER_DECL int tdm_cmn_get_speed_slots(tdm_mod_t *_tdm, int port_speed);
LINKER_DECL int tdm_cmn_get_port_slots(tdm_mod_t *_tdm, int port_token);
LINKER_DECL tdm_calendar_t *tdm_cmn_get_pipe_cal(tdm_mod_t *_tdm);
LINKER_DECL tdm_calendar_t *tdm_cmn_get_pipe_cal_prev(tdm_mod_t *_tdm);

#endif /* TDM_PREPROCESSOR_PROTOTYPES_H */
