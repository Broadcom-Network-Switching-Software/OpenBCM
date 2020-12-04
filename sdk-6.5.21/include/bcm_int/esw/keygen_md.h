/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_INT_FIELD_KEYGEN_MD_H
#define _BCM_INT_FIELD_KEYGEN_MD_H

#include <bcm_int/esw/keygen_api.h>

#define BCMI_KEYGEN_EXT_SELCODE_DONT_CARE (-1)

/*
 * Typedef:
 *     bcmi_keygen_ext_sel_t
 * Purpose:
 *     Logical table key generation program control selectors.
 */
typedef struct bcmi_keygen_ext_sel_s {
    int8 ipbm_source;         /* Ingress Port Bitmap Source.          */
    int8 ipbm_present;          /* Post mux IPBM in final key.          */
    int8 normalize_l3_l4_sel;   /* Normalize L3 and L4 address.         */
    int8 normalize_mac_sel;     /* Normalize L2 SA & DA.                */
    int8 aux_tag_a_sel;         /* Auxiliary Tag A Selector.            */
    int8 aux_tag_b_sel;         /* Auxiliary Tag B Selector.            */
    int8 aux_tag_c_sel;         /* Auxiliary Tag C Selector.            */
    int8 aux_tag_d_sel;         /* Auxiliary Tag d Selector.            */
    int8 tcp_fn_sel;            /* TCP function selector.               */
    int8 tos_fn_sel;            /* TOS function selector.               */
    int8 ttl_fn_sel;            /* TTL function selector.               */
    int8 class_id_cont_a_sel;   /* Class ID container A selector.       */
    int8 class_id_cont_b_sel;   /* Class ID container B selector.       */
    int8 class_id_cont_c_sel;   /* Class ID container C selector.       */
    int8 class_id_cont_d_sel;   /* Class ID container D selector.       */
    int8 src_cont_a_sel;        /* SRC container A selector.            */
    int8 src_cont_b_sel;        /* SRC container B selector.            */
    int8 src_dest_cont_0_sel;   /* SRC or DST container 0 selector.     */
    int8 src_dest_cont_1_sel;   /* SRC or DST container 1 selector.     */
    int8 dst_cont_a_sel;        /* DST container A selector.            */
    int8 dst_cont_b_sel;        /* DST container B selector.            */
    int8 udf_sel;               /* UDF0 or UDF1 selector.               */
    int8 pmux_sel[15];          /* Post muxing extractors.              */
    int8 alt_ttl_fn_sel;        /* ALT TTL function selector.           */
    int8 ltid_sel;              /* LTID funciton selector.              */
} bcmi_keygen_ext_sel_t;

#define BCMI_KEYGEN_EXT_GRANULAR_ARR_SIZE 5

#define BCMI_KEYGEN_EXT_IN_SECTION_COUNT  5

#define BCMI_KEYGEN_EXT_SECTION_PASS_THRU    (1 << 0)

#define BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_1  (1 << 1)

#define BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_2  (1 << 2)

#define BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_4  (1 << 3)

#define BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_8  (1 << 4)

#define BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_16 (1 << 5)

#define BCMI_KEYGEN_EXT_SECTION_WITH_GRAN_32 (1 << 6)

#define BCMI_KEYGEN_EXT_SECTION_MULTI_GRAN   (1 << 7)

typedef struct bcmi_keygen_ext_section_gran_s {

    uint8 gran;

    uint8 num_extractors;

} bcmi_keygen_ext_section_gran_t;

typedef struct bcmi_keygen_ext_section_gran_info_s {
    /* granularity information for out section. */
    bcmi_keygen_ext_section_gran_t  out_gran_info[BCMI_KEYGEN_EXT_GRANULAR_ARR_SIZE];
    /* number of valid indices in out_gran_info. */
    uint8 out_gran_info_count;
    /* granularity information for out multi-gran section. */
    bcmi_keygen_ext_section_gran_t  out_multigran_info[BCMI_KEYGEN_EXT_GRANULAR_ARR_SIZE];
    /* number of valid indices in out_multugran_info. */
    uint8 out_multigran_info_count;
    /* granularity information for in section.*/
    bcmi_keygen_ext_section_gran_t  in_gran_info[BCMI_KEYGEN_EXT_GRANULAR_ARR_SIZE];
    /* number of valid indices in in_gran_info. */
    uint8 in_gran_info_count;

} bcmi_keygen_ext_section_gran_info_t;

/* Note: enumerations in bcmi_keygen_ext_attr_t must have 1-to-1
 * mapping with enumerations in bcmi_keygen_acl_attr_t.
 */
typedef enum bcmi_keygen_acl_attr_s {

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN = 0,

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN_1,

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN_2,

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN_4,

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN_8,

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN_16,

    BCMI_KEYGEN_ACL_ATTR_MULTI_GRAN_32,

    BCMI_KEYGEN_ACL_ATTR_PASS_THRU,

    BCMI_KEYGEN_ACL_ATTR_MODE_HALF,

    BCMI_KEYGEN_ACL_ATTR_PMUX_IPBM,

    BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0,

    BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1,

    BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB,

    BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_0_HM,

    BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_1_HM,

    BCMI_KEYGEN_ACL_ATTR_PMUX_SRC_DST_CONT_MSB_HM,

    BCMI_KEYGEN_ACL_ATTR_PMUX_DROP,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_0_3,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_4_7,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_CLASSID_BITS_8_11,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_0_3,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_4_7,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_CLASSID_BITS_8_11,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_FIRST_LOOKUP_HIT,

    BCMI_KEYGEN_ACL_ATTR_PMUX_EM_SECOND_LOOKUP_HIT,

    BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_NEEDED,

    BCMI_KEYGEN_ACL_ATTR_PMUX_NAT_DST_REALM_ID,

    BCMI_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_0_3,

    BCMI_KEYGEN_ACL_ATTR_PMUX_LOOKUP_STATUS_VECTOR_BITS_4_7,

    BCMI_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_A,

    BCMI_KEYGEN_ACL_ATTR_PMUX_DST_CONTAINER_B,

    BCMI_KEYGEN_ACL_ATTR_PMUX_PKT_RESOLUTION,

    BCMI_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_C,

    BCMI_KEYGEN_ACL_ATTR_PMUX_CLASS_ID_D,

    BCMI_KEYGEN_ACL_ATTR_PMUX_INT_PRI,

    BCMI_KEYGEN_ACL_ATTR_PMUX_IFP_DROP_VECTOR,

    BCMI_KEYGEN_ACL_ATTR_PMUX_MH_OPCODE,

    BCMI_KEYGEN_ACL_ATTR_MODE_SINGLE_ASET_WIDE,

    BCMI_KEYGEN_ACL_ATTR_QSET_UPDATE,

    BCMI_KEYGEN_ACL_ATTR_PMUX_LTID,

    BCMI_KEYGEN_ACL_ATTR_COUNT

} bcmi_keygen_acl_attr_t;

typedef struct bcmi_keygen_acl_attrs_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCMI_KEYGEN_ACL_ATTR_COUNT)];
} bcmi_keygen_acl_attrs_t;

#define BCMI_KEYGEN_EXT_FIELD_WORD_COUNT 1

#define BCMI_KEYGEN_EXT_FIELD_CHUNKS_MAX 128

#define BCMI_KEYGEN_EXT_FIELDS_MAX 50

#define BCMI_KEYGEN_EXT_GRAN_COUNT 5

#define BCMI_KEYGEN_EXT_SECTION_GRAN_COUNT 3

#define BCMI_KEYGEN_EXT_SECTIONS_COUNT 5

#define BCMI_KEYGEN_EXT_FIELD_OFFSET_MAX 128

#define BCMI_KEYGEN_EXT_FIELD_OFFSET_COUNT 16

typedef struct bcmi_keygen_ext_field_offset_s {

    uint16 offset;

    uint16 width;

    uint8 chunk_id;

} bcmi_keygen_ext_field_offset_t;

typedef struct  bcmi_keygen_ext_field_offset_info_s {

    uint16 offset_count;

    bcmi_keygen_ext_field_offset_t offsets[BCMI_KEYGEN_EXT_FIELD_OFFSET_MAX];

}  bcmi_keygen_ext_field_offset_info_t;

typedef struct bcmi_keygen_ext_field_s {

     bcmi_keygen_ext_section_t section;

     uint8 sec_val;

     uint16 offset;

     uint8 width;

     uint32 req_bmp[BCMI_KEYGEN_EXT_FIELD_WORD_COUNT];

     uint32 rsp_bmp[BCMI_KEYGEN_EXT_FIELD_WORD_COUNT];

} bcmi_keygen_ext_field_t;

/* When this flag is set, 16-bit granularity extractors are used at
 * level 2. This flag is set only in case of flowtracker style
 * extractors.
 */
#define BCMI_KEYGEN_EXT_FIELD_USE_L2_GRAN16                 (1 << 0)

/* When this flag is set, 8-bit granularity extractors are used at
 * level 2. This flag is set only in case of flowtracker style
 * extractors.
 */
#define BCMI_KEYGEN_EXT_FIELD_USE_L2_GRAN8                  (1 << 1)

/* Qualifier has to be extrctaed to modulo 32 offsets in the final
 * key.In general keygen algorithm doesnot give any preference to
 * any qualifier while placing it in the final key. But this flag
 * ovverrides this behaviour.
 */
#define BCMI_KEYGEN_EXT_FIELD_EXTRACT_TO_MODULO_32_OFFSET   (1 << 2)

/* If this flag is set, then this field along with some other field,
 * to which this is sticky, must be extrcated from same part.
 */
#define BCMI_KEYGEN_EXT_FIELD_IS_STICKY                     (1 << 3)

/* If this flag is set, then the corresponding finfo will be extrctaed
 * at higher offset than the head finfo is extracted in the final key.
 */
#define BCMI_KEYGEN_EXT_FIELD_EXTRACT_AFTER_STICKY_HEAD     (1 << 4)

#define BCMI_KEYGEN_EXT_SECTION_MAP_MAX 2

typedef struct bcmi_keygen_ext_field_info_s {

    uint32 flags;

    uint8 extracted;

    int part;

    bcmi_keygen_ext_section_t section;

    uint8 sec_val;

    uint8 size;

    uint8 req_bit_count;

    uint32 req_bmp[BCMI_KEYGEN_EXT_FIELD_WORD_COUNT];

    uint32 ext_bmp[BCMI_KEYGEN_EXT_FIELD_WORD_COUNT];

    bcmi_keygen_ext_section_t parent_section[BCMI_KEYGEN_EXT_SECTION_MAP_MAX];

    uint8 parent_sec_val[BCMI_KEYGEN_EXT_SECTION_MAP_MAX];

    uint16 parent_field_offset[BCMI_KEYGEN_EXT_SECTION_MAP_MAX];

    uint16 self_field_offset[BCMI_KEYGEN_EXT_SECTION_MAP_MAX];

    bcmi_keygen_ext_ctrl_sel_t pri_ctrl_sel[BCMI_KEYGEN_EXT_SECTION_MAP_MAX];

    uint16 pri_ctrl_sel_val[BCMI_KEYGEN_EXT_SECTION_MAP_MAX];

    bcmi_keygen_ext_cfg_t *ext_cfg;

    uint8 prev_offset;

    struct bcmi_keygen_ext_field_info_s *prev;

    struct bcmi_keygen_ext_field_info_s *next;

    struct bcmi_keygen_ext_field_info_s *sticky;

} bcmi_keygen_ext_field_info_t;

typedef struct bcmi_keygen_ext_field_info_db_s {

     bcmi_keygen_ext_field_info_t *finfo[BCMI_KEYGEN_EXT_SECTION_COUNT];

} bcmi_keygen_ext_field_info_db_t;

typedef bcmi_keygen_ext_section_t bcmi_keygen_ext_section_list_t[BCMI_KEYGEN_EXT_SECTION_COUNT];

typedef enum bcmi_keygen_pmux_type_e {

    BCMI_KEYGEN_PMUX_TYPE_IPBM  = 0,

    BCMI_KEYGEN_PMUX_TYPE_DROP,

    BCMI_KEYGEN_PMUX_TYPE_NAT_NEEDED,

    BCMI_KEYGEN_PMUX_TYPE_NAT_DST_REALM_ID,

    BCMI_KEYGEN_PMUX_TYPE_SRC_DST_CONT_0,

    BCMI_KEYGEN_PMUX_TYPE_SRC_DST_CONT_1,

    BCMI_KEYGEN_PMUX_TYPE_SRC_DST_CONT_0_SLICE_B,

    BCMI_KEYGEN_PMUX_TYPE_SRC_DST_CONT_1_SLICE_B,

    BCMI_KEYGEN_PMUX_TYPE_SRC_DST_CONT_0_SLICE_C,

    BCMI_KEYGEN_PMUX_TYPE_SRC_DST_CONT_1_SLICE_C,

    BCMI_KEYGEN_PMUX_TYPE_EM_FIRST_LOOKUP_HIT,

    BCMI_KEYGEN_PMUX_TYPE_EM_SECOND_LOOKUP_HIT,

    BCMI_KEYGEN_PMUX_TYPE_EM_FIRST_LOOKUP_CLASSID,

    BCMI_KEYGEN_PMUX_TYPE_EM_SECOND_LOOKUP_CLASSID,

    BCMI_KEYGEN_PMUX_TYPE_COUNT

} bcmi_keygen_pmux_type_t;

typedef struct bcmi_keygen_pmux_type_bmp_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(BCMI_KEYGEN_PMUX_TYPE_COUNT)];
} bcmi_keygen_pmux_type_bmp_t;

typedef struct bcmi_keygen_pmux_info_s {

    bcm_field_qualify_t pmux_qual;

    uint8 pmux_part;

    bcmi_keygen_qual_cfg_t *pmux_qual_cfg;

} bcmi_keygen_pmux_info_t;

/* This metatdata structure has all the information
 * required during keygen creation.
 */
typedef struct bcmi_keygen_md_s {

    bcmi_keygen_qual_cfg_info_t *qual_cfg_info[BCM_FIELD_QUALIFY_MAX];

    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db[BCMI_KEYGEN_EXT_LEVEL_COUNT];

    bcmi_keygen_ext_cfg_db_t *ext_cfg_db;

    uint16 rkey_size;

    uint8 *qual_cfg_id_arr;

    uint16 qual_cfg_combinations;

    bcmi_keygen_acl_attrs_t acl_attrs[BCMI_KEYGEN_ENTRY_PARTS_MAX];

    bcmi_keygen_ext_field_info_t finfo[BCMI_KEYGEN_EXT_FIELDS_MAX];

    bcmi_keygen_ext_field_info_db_t ext_finfo_db[BCMI_KEYGEN_ENTRY_PARTS_MAX][BCMI_KEYGEN_EXT_LEVEL_COUNT];

    uint32 bits_extracted[BCMI_KEYGEN_ENTRY_PARTS_MAX][BCMI_KEYGEN_EXT_GRAN_COUNT];

    uint8 next_part_id[BCMI_KEYGEN_EXT_GRAN_COUNT];

    uint8 tried_part_bmp[BCMI_KEYGEN_EXT_GRAN_COUNT];

    bcmi_keygen_ext_section_list_t sections[BCMI_KEYGEN_ENTRY_PARTS_MAX][BCMI_KEYGEN_EXT_LEVEL_COUNT];

    bcmi_keygen_ext_section_gran_info_t section_gran_info[BCMI_KEYGEN_EXT_SECTION_COUNT];

    uint8 num_ext_levels;

    bcmi_keygen_ext_field_offset_info_t offset_info;

    bcmi_keygen_ext_sel_t ext_codes[BCMI_KEYGEN_ENTRY_PARTS_MAX];

    bcmi_keygen_pmux_info_t *pmux_info;

    uint8 num_pmux_qual;

    uint8 pri_ctrl_sel_count[BCMI_KEYGEN_EXT_CTRL_SEL_COUNT];
    uint32 pri_ctrl_sel_values[BCMI_KEYGEN_EXT_CTRL_SEL_COUNT];
} bcmi_keygen_md_t;

#define BCMI_KEYGEN_EXT_SECTION_SIZE_GET(section, size)          \
    {                                                            \
        if (BCMI_KEYGEN_EXT_SECTION_L1E32 == section) {          \
            f_size = 32;                                         \
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E16 == section) {   \
            f_size = 16;                                         \
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E8 == section)  {   \
            f_size = 8;                                          \
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E4 == section)  {   \
            f_size = 4;                                          \
        } else if (BCMI_KEYGEN_EXT_SECTION_L1E2 == section)  {   \
            f_size = 2;                                          \
        } else {                                                 \
            BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);         \
        }                                                        \
    }

#define BCMI_KEYGEN_EXT_SECTION_BASE_IDX(section, base_idx)         \
        {                                                           \
           if (BCMI_KEYGEN_EXT_SECTION_L1E32 == section) {          \
               base_idx = 0;                                        \
           } else if (BCMI_KEYGEN_EXT_SECTION_L1E16 == section) {   \
               base_idx = 1;                                        \
           } else if (BCMI_KEYGEN_EXT_SECTION_L1E8 == section)  {   \
               base_idx = 2;                                        \
           } else if (BCMI_KEYGEN_EXT_SECTION_L1E4 == section)  {   \
               base_idx = 3;                                        \
           } else if (BCMI_KEYGEN_EXT_SECTION_L1E2 == section)  {   \
               base_idx = 4;                                        \
           } else {                                                 \
               BCMI_KEYGEN_IF_ERR_EXIT(BCM_E_INTERNAL);         \
           }                                                        \
        }

extern int
bcmi_keygen_ext_cfg_reset(int unit,
                          bcmi_keygen_cfg_t *keygen_cfg,
                          bcmi_keygen_md_t *keygen_md);
extern int
bcmi_keygen_ext_cfg_alloc(int unit,
                          bcmi_keygen_cfg_t *keygen_cfg,
                          bcmi_keygen_md_t *keygen_md);
extern int
bcmi_keygen_ext_cfg_reset_part(int unit,
                               uint8 part,
                               bcmi_keygen_cfg_t *keygen_cfg,
                               bcmi_keygen_md_t *keygen_md);
extern int
bcmi_keygen_ext_sections_get(int unit,
                             bcmi_keygen_cfg_t *keygen_cfg,
                             bcmi_keygen_md_t *keygen_md);

extern int
bcmi_keygen_ext_alloc(int unit,
                      bcmi_keygen_cfg_t *keygen_cfg,
                      bcmi_keygen_oper_t *keygen_oper,
                      bcmi_keygen_md_t *keygen_md);

extern int
bcmi_keygen_ext_codes_init(int unit, bcmi_keygen_ext_sel_t *ext_codes);

extern int
bcmi_keygen_ext_part_get(int unit,
                         bcmi_keygen_cfg_t *keygen_cfg,
                         bcmi_keygen_md_t *keygen_md,
                         bcmi_keygen_ext_field_info_t *finfo);

extern int
bcmi_keygen_ext_codes_validate(int unit,
                               uint8 part,
                               bcmi_keygen_cfg_t *keygen_cfg,
                               bcmi_keygen_md_t *keygen_md);

extern int
bcmi_keygen_ext_part_incr(int unit,
                          bcmi_keygen_cfg_t *keygen_cfg,
                          bcmi_keygen_md_t *keygen_md,
                          bcmi_keygen_ext_section_t section);

extern int
bcmi_keygen_ext_codes_create(int unit,
                             bcmi_keygen_cfg_t *keygen_cfg,
                             bcmi_keygen_md_t *keygen_md,
                             bcmi_keygen_oper_t *keygen_oper);

extern int
bcmi_keygen_pmux_process(int unit,
                         bcmi_keygen_cfg_t *keygen_cfg,
                         bcmi_keygen_md_t *keygen_md);
extern int
bcmi_keygen_pmux_info_get(int unit,
                    bcm_field_qualify_t qual,
                    uint8 num_pmux_qual,
                    bcmi_keygen_pmux_info_t *pmux_info_arr,
                    bcmi_keygen_pmux_info_t **pmux_info);

extern int
bcmi_keygen_pmux_qual_offset_get(int unit,
                                 bcm_field_qualify_t qual,
                                 bcmi_keygen_md_t *keygen_md,
                                 bcmi_keygen_cfg_t *keygen_cfg,
                                 bcmi_keygen_qual_offset_t *qual_offset);
extern int
bcmi_keygen_ext_validate(int unit,
                       uint8 part,
                       bcmi_keygen_md_t *keygen_md,
                       bcmi_keygen_ext_cfg_t *ext_cfg);
extern int
bcmi_keygen_ext_section_gran_info_init(int unit,
                              bcmi_keygen_md_t *keygen_md);

extern int
bcmi_keygen_qual_cfg_max_size_get(int unit,
                            bcmi_keygen_cfg_t *keygen_cfg,
                            bcm_field_qualify_t qual_id,
                            uint16 *qual_cfg_max_size);
extern int
bcmi_keygen_ext_cfg_update(int unit,
                        bcmi_keygen_cfg_t *keygen_cfg,
                        bcmi_keygen_md_t *keygen_md);

extern int
bcmi_keygen_ext_section_drain_bits_update(int unit,
                       bcmi_keygen_md_t *keygen_md);

extern int
bcmi_keygen_ext_ctrl_sel_validate(int unit,
                               bcmi_keygen_cfg_t *keygen_cfg,
                               bcmi_keygen_md_t *keygen_md);
#endif

