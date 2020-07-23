/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __BCM_INT_UDF_H__
#define __BCM_INT_UDF_H__

#include <bcm_int/esw/udf_common.h>
#if defined (BCM_TRIDENT3_SUPPORT)

/* Control Flags */
#define _BCM_UDF_TD3_CTRL_RANGE_CHECK   (1 << 1)

/* Object entry flags */
#define _BCM_UDF_TD3_OBJ_IFP            (1 << 1)
#define _BCM_UDF_TD3_OBJ_VFP            (1 << 2)
#define _BCM_UDF_TD3_OBJ_RANGE_CHECK    (1 << 3)

/* Number of stages to be used to extract bytes for a given parser */
#define _BCM_UDF_MAX_PARSER_STAGES 2

/*
 * UDF object information Structure.
 */
typedef struct _bcm_udf_td3_obj_info_s {
    bcm_udf_id_t                      udf_id;        /* UDF Objeect Id */
    uint16                            offset;        /* Start offset in bytes. */
    uint16                            width;         /* Bytes to be extracted. */
    uint32                            chunk_bmap;    /* Chunk bitmap allocated for
                                                        given UDF object */
    uint16                            hw_cmd_bmap[_BCM_UDF_MAX_PARSER_STAGES];
                                                     /* HW CMD_POLICY_DATA for each stage. */
    uint32                            flags;         /* misc. info */
    bcm_udf_abstract_pkt_format_t     abstr_pkt_fmt; /* Abstract packet format. */
    struct _bcm_udf_td3_obj_info_s *next;         /* Next Pointer of Linked-list. */
    struct _bcm_udf_td3_obj_info_s *prev;         /* Prev Pointer of Linked-list. */
} _bcm_udf_td3_obj_info_t;

/*
 * UDF Control Structure.
 */
typedef struct _bcm_udf_td3_ctrl_s {
    uint32                       flags;         /* misc. info */
    _bcm_udf_td3_obj_info_t      *udf_obj_head; /* UDF object linked-list
                                                   head node */
} _bcm_udf_td3_ctrl_t;

/*
 * Initialize for UDF control structure for MAX supported units.
 */
extern _bcm_udf_td3_ctrl_t *udf_td3_control[BCM_MAX_NUM_UNITS];

/*
 * UDF Packet Format HW Info Structure.
 */
typedef struct _bcm_udf_td3_abstr_pkt_fmt_hw_info_e {
    uint8                     parser;             /* Parser number */
    uint8                     num_stages;         /* Number stages supported for the given
                                                     abstract pkt format. */
    soc_mem_t                 stage_policy_mem[_BCM_UDF_MAX_PARSER_STAGES];
                                                  /* Parser1/2 Stage Policy Memory to
                                                     extract first 16 bytes for the given
                                                     abstract pkt format. */
    uint8                     hfe_prof_ptr_arr_len[_BCM_UDF_MAX_PARSER_STAGES];
                                                  /* HFE Profile pointer array len */
    uint32                    *hfe_profile_ptr[_BCM_UDF_MAX_PARSER_STAGES];
                                                  /* Array of HFE Profile pointers for
                                                     a given stage */
    uint8                     num_cmds[_BCM_UDF_MAX_PARSER_STAGES];
                                                  /* Number of commands supports
                                                     for a stage. */
    uint8                     max_extract_bytes;  /* Maximum Number of bytes can be extracted */
    uint8                     extracted_bytes;    /* Bytes already extracted */
    uint32                    chunk_bmap_used;    /* Allocated chunks for a given abstract pkt
                                                     format in bitmap */
    uint16                    valid_cmd_bmap[_BCM_UDF_MAX_PARSER_STAGES];
                                                  /* Valid Commands configured on respective
                                                     stage policy mem. */
    uint8                     *cmd_policy_data[_BCM_UDF_MAX_PARSER_STAGES];
                                                  /* Reference to default CMD_POLICY_DATA
                                                     for given stage hfe_profile_ptr. */
    bcm_udf_pkt_base_offset_t base_offset;        /* Start base offset. */
    uint32                    unavail_chunk_bmap; /* Unavailable chunk bitmap */
} _bcm_udf_td3_abstr_pkt_fmt_hw_info_t;

#if defined (BCM_WARM_BOOT_SUPPORT)
/*
 * Structure to be used for sync and recovery of UDF objects
 * in case of Warmboot.
 */
typedef struct _bcm_udf_td3_wb_obj_info_s {
    bcm_udf_id_t                      udf_id;        /* UDF Objeect Id */
    uint16                            offset;        /* Start offset in bytes. */
    uint32                            chunk_bmap;    /* Chunk bitmap allocated for
                                                        given UDF object */
    uint16                            hw_cmd_bmap1;  /* HW CMD_POLICY_DATA for stage1. */
    uint16                            hw_cmd_bmap2;  /* HW CMD_POLICY_DATA for stage2. */
    bcm_udf_abstract_pkt_format_t     abstr_pkt_fmt; /* Abstract packet format. */
} _bcm_udf_td3_wb_obj_info_t;

/*
 * Structure to be used for sync and recovery of UDF objects
 * in case of Warmboot for version 2.
 */
typedef struct _bcm_udf_td3_wb_obj_info_2_s {
    bcm_udf_id_t                      udf_id;        /* UDF Objeect Id */
    uint16                            offset;        /* Start offset in bytes. */
    uint32                            chunk_bmap;    /* Chunk bitmap allocated for
                                                        given UDF object */
    uint8                             num_stages;    /* Number of stages */
    uint16                            hw_cmd_bmap[_BCM_UDF_MAX_PARSER_STAGES];
                                                     /* HW CMD_POLICY_DATA for stage1. */
    bcm_udf_abstract_pkt_format_t     abstr_pkt_fmt; /* Abstract packet format. */
} _bcm_udf_td3_wb_obj_info_2_t;

#endif /* BCM_WARM_BOOT_SUPPORT */

extern int
_bcm_udf_td3_chunk_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_chunk_info_t  *udf_info,
    bcm_udf_id_t *udf_id);

extern int
_bcm_udf_td3_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_chunk_info_t *udf_chunk_info);

extern int
_bcm_udf_td3_abstract_pkt_format_object_list_get(
    int unit,
    bcm_udf_abstract_pkt_format_t abstract_pkt_format,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual);

extern int
_bcm_udf_td3_abstract_pkt_format_info_get(
    int unit,
    bcm_udf_abstract_pkt_format_t abstract_pkt_format,
    bcm_udf_abstract_pkt_format_info_t *pkt_format_info);

extern void
_bcm_udf_td3_functions_init(_bcm_udf_funct_t *functions);

extern int
_bcm_udf_td3_object_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    _bcm_udf_td3_obj_info_t **obj_info);

extern int
_bcm_udf_td3_chunk_id_multi_get(
    int unit,
    uint32 udf_chunk_bmap,
    int max,
    int *objects_list,
    int *actual);

#endif /* BCM_TRIDENT3_SUPPORT */

void
_bcm_esw_udf_sw_dump(int unit);
void
_bcm_esw_udf_sw_mem_dump(int unit, soc_mem_t mem, int idx);

#endif /* __BCM_INT_UDF_H__ */
