
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_KBP_ACCESS_H__
#define __DNX_FIELD_KBP_ACCESS_H__

#ifdef INCLUDE_KBP
#include <soc/dnx/swstate/auto_generated/types/dnx_field_kbp_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>


typedef int (*dnx_field_kbp_sw_is_init_cb)(
    int unit, uint8 *is_init);

typedef int (*dnx_field_kbp_sw_init_cb)(
    int unit);

typedef int (*dnx_field_kbp_sw_opcode_info_set_cb)(
    int unit, uint32 opcode_info_idx_0, CONST dnx_field_kbp_opcode_info_t *opcode_info);

typedef int (*dnx_field_kbp_sw_opcode_info_get_cb)(
    int unit, uint32 opcode_info_idx_0, dnx_field_kbp_opcode_info_t *opcode_info);

typedef int (*dnx_field_kbp_sw_opcode_info_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_set_cb)(
    int unit, uint32 opcode_info_idx_0, CONST dnx_field_kbp_master_key_info_t *master_key_info);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_get_cb)(
    int unit, uint32 opcode_info_idx_0, dnx_field_kbp_master_key_info_t *master_key_info);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_set_cb)(
    int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, CONST dnx_field_kbp_segment_info_t *segment_info);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_get_cb)(
    int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, dnx_field_kbp_segment_info_t *segment_info);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_set_cb)(
    int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 qual_idx);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_get_cb)(
    int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, uint32 qual_idx_idx_0, uint8 *qual_idx);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_set_cb)(
    int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t fg_id);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_get_cb)(
    int unit, uint32 opcode_info_idx_0, uint32 segment_info_idx_0, dnx_field_group_t *fg_id);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_set_cb)(
    int unit, uint32 opcode_info_idx_0, uint8 nof_fwd_segments);

typedef int (*dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_get_cb)(
    int unit, uint32 opcode_info_idx_0, uint8 *nof_fwd_segments);

typedef int (*dnx_field_kbp_sw_opcode_info_is_valid_set_cb)(
    int unit, uint32 opcode_info_idx_0, uint8 is_valid);

typedef int (*dnx_field_kbp_sw_opcode_info_is_valid_get_cb)(
    int unit, uint32 opcode_info_idx_0, uint8 *is_valid);



typedef struct {
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_set_cb set;
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_get_cb get;
} dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_cbs;

typedef struct {
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_set_cb set;
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_get_cb get;
} dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_cbs;

typedef struct {
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_set_cb set;
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_get_cb get;
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_qual_idx_cbs qual_idx;
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_fg_id_cbs fg_id;
} dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_cbs;

typedef struct {
    dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_set_cb set;
    dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_get_cb get;
} dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_cbs;

typedef struct {
    dnx_field_kbp_sw_opcode_info_master_key_info_set_cb set;
    dnx_field_kbp_sw_opcode_info_master_key_info_get_cb get;
    dnx_field_kbp_sw_opcode_info_master_key_info_segment_info_cbs segment_info;
    dnx_field_kbp_sw_opcode_info_master_key_info_nof_fwd_segments_cbs nof_fwd_segments;
} dnx_field_kbp_sw_opcode_info_master_key_info_cbs;

typedef struct {
    dnx_field_kbp_sw_opcode_info_is_valid_set_cb set;
    dnx_field_kbp_sw_opcode_info_is_valid_get_cb get;
} dnx_field_kbp_sw_opcode_info_is_valid_cbs;

typedef struct {
    dnx_field_kbp_sw_opcode_info_set_cb set;
    dnx_field_kbp_sw_opcode_info_get_cb get;
    dnx_field_kbp_sw_opcode_info_alloc_cb alloc;
    dnx_field_kbp_sw_opcode_info_master_key_info_cbs master_key_info;
    dnx_field_kbp_sw_opcode_info_is_valid_cbs is_valid;
} dnx_field_kbp_sw_opcode_info_cbs;

typedef struct {
    dnx_field_kbp_sw_is_init_cb is_init;
    dnx_field_kbp_sw_init_cb init;
    dnx_field_kbp_sw_opcode_info_cbs opcode_info;
} dnx_field_kbp_sw_cbs;





extern dnx_field_kbp_sw_cbs dnx_field_kbp_sw;
#endif  

#endif 
