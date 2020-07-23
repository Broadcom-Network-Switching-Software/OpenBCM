/*! \file lb_flex_hash.h
 *
 * LB hash header file.
 * This file contains LB hash definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XGS_LB_FLEX_HASH_H
#define BCMINT_LTSW_XGS_LB_FLEX_HASH_H

#include <bcm/types.h>
#include <bcm_int/ltsw/lb_hash.h>
#include <bcmlt/bcmlt.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bcm_int/ltsw/generated/lb_hash_ha.h>

#define BCMI_LB_HASH_FLEX_ENTRY_MD_SUB_COMP_ID      1
#define BCMI_LB_HASH_FLEX_ENTRY_KEY_SUB_COMP_ID     2
#define BCMI_LB_HASH_FLEX_ENTRY_DATA_SUB_COMP_ID    3

#define BCMI_LB_HASH_FLEX_ENTRY_MAX                 8
#define BCMI_LB_HASH_FLEX_ENTRY_KEY_FIELDS          4

/* Generic memory allocation routine. */
#define LB_HASH_MEM_ALLOC(_ptr_,_size_,_descr_)                 \
       do {                                                     \
           if (NULL == (_ptr_)) {                               \
              SHR_ALLOC(_ptr_, _size_, _descr_);                \
              SHR_NULL_CHECK(_ptr_, SHR_E_MEMORY);              \
           }                                                    \
           sal_memset((_ptr_), 0, (_size_));                    \
       } while (0)

/* Generic memory free routine. */
#define LB_HASH_MEM_FREE(_ptr_)                                 \
       do {                                                     \
           if (NULL != (_ptr_)) {                               \
              SHR_FREE((_ptr_));                                \
              _ptr_ = NULL;                                     \
           } else {                                             \
              LOG_ERROR(BSL_LS_BCM_HASH, (BSL_META_U(unit,      \
              "LB Hash Error: Freeing NULL Ptr.\n\r")));        \
           }                                                    \
       } while (0)

/*!
 * \brief LB Hash Flex Entry structure
 */
typedef struct bcmi_lb_hash_flex_fields_sel_entry_s {
    /*! Pointer to Entry metadata */
    bcmi_lb_hash_flex_fields_sel_entry_md_t *md;

    /*! Pointer to Key info array */
    bcmi_lb_hash_flex_fields_sel_entry_key_info_t *key_info;

    /*! Pointer to Data info array */
    bcmi_lb_hash_flex_fields_sel_entry_data_info_t *data_info;
} bcmi_lb_hash_flex_fields_sel_entry_t;

/*!
 * \brief LB Hash Flex fields sel control structure.
 */
typedef struct bcmi_lb_hash_flex_fields_sel_ctrl_s {
    /*! Max Entry Id */
    uint8 max_id;

    /*! Min Entry Id */
    uint8 min_id;

    /*! Maximum number of entries */
    uint8 max_entries;

    /*! Number of entries installed */
    uint8 num_installed;

    /*! LB Hash Flex Entry list */
    bcmi_lb_hash_flex_fields_sel_entry_t *hash_entry_array;

    /*! LB Hash flex Entry MD list */
    bcmi_lb_hash_flex_fields_sel_entry_md_t *entry_md_array;

    /*! LB Hash flex entry Key info list */
    bcmi_lb_hash_flex_fields_sel_entry_key_info_t *key_info_array;

    /*! LB Hash flex entry Data info list */
    bcmi_lb_hash_flex_fields_sel_entry_data_info_t *data_info_array;

} bcmi_lb_hash_flex_fields_sel_ctrl_t;

/* Extern to flex hash control variable. */
extern bcmi_lb_hash_flex_fields_sel_ctrl_t
            *xgs_lb_hash_flex_fields_sel_ctrl[BCM_MAX_NUM_UNITS];

/* LB Hash flex fields sel ctrl */
#define LB_HASH_FLEX_FIELDS_SEL_CTRL(_u_)           \
            xgs_lb_hash_flex_fields_sel_ctrl[_u_]

/* Validate Hash flex Entry Id */
#define LB_HASH_FLEX_ENTRY_ID_VALIDATE(_u_, _id_)                   \
    do {                                                            \
        if ((_id_) < LB_HASH_FLEX_FIELDS_SEL_CTRL(_u_)->min_id ||   \
            (_id_) > LB_HASH_FLEX_FIELDS_SEL_CTRL(_u_)->max_id) {   \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);                   \
        }                                                           \
    } while (0)

/* Flags for LB Flex Hash Entry */
#define BCMI_LB_HASH_FLEX_ENTRY_FREE                       (0)
#define BCMI_LB_HASH_FLEX_ENTRY_IN_USE                     (1 << 0)
#define BCMI_LB_HASH_FLEX_ENTRY_INSTALLED                  (1 << 1)

/* Flags for LB Flex Hash Entry Data Fields */
#define BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_DISABLED         (0)
#define BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_L4          (1 << 0)
#define BCMI_LB_HASH_FLEX_DATA_FIELD_EXTR_TYPE_UDF         (1 << 1)

/* LT Key information */
typedef struct bcmi_lb_hash_flex_fields_sel_lt_key_info_s {

    /* Number of Fields */
    int fields;

    /* Key Data fields */
    const char **lt_key_data;

    /* Key Mask fields */
    const char **lt_key_mask;

} bcmi_lb_hash_flex_fields_sel_lt_key_info_t;

typedef struct bcmi_lb_hash_flex_fields_select_info_s {

    /* Flags */
#define BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_SYM           (1 << 0)
#define BCMI_LB_HASH_FLEX_LT_DATA_FIELD_TYPE_ARRAY         (1 << 1)
#define BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_L4_SUPPORT    (1 << 2)
#define BCMI_LB_HASH_FLEX_LT_DATA_FIELD_EXTR_UDF_SUPPORT   (1 << 3)
    uint8 flags;

    /* Array index */
    int index;

    /* Selector field */
    const char *bin_field_extr;

    /* Offset field */
    const char *bin_l4_offset;

    /* UDF offset field */
    const char *bin_udf_offset;

    /* Field Mask */
    const char *bin_field_mask;

} bcmi_lb_hash_flex_fields_select_info_t;

/* LT Data information */
typedef struct bcmi_lb_hash_flex_fields_sel_lt_data_info_s {

    /* Number of fields */
    int fields;

    /* LT data field info */
    bcmi_lb_hash_flex_fields_select_info_t *field_info;

    /* Length of Mask field */
    uint8 mask_length;

} bcmi_lb_hash_flex_fields_sel_lt_data_info_t;

typedef struct bcmi_lb_hash_flex_udf_chunks_info_s {
    /* Number of UDF chunks for Flex hash */
    int chunks_count;

    /* List of Chunk Ids */
    uint8 chunk_ids[4];

} bcmi_lb_hash_flex_udf_chunks_info_t;

/* LB Hash flex fields sel LT info */
typedef struct bcmi_lb_hash_flex_fields_sel_lt_info_t {

    /* sid */
    const char *sid;

    /* LT Key fields info */
    bcmi_lb_hash_flex_fields_sel_lt_key_info_t lt_key_info;

    /* LT Data fields info */
    bcmi_lb_hash_flex_fields_sel_lt_data_info_t lt_data_info;

    /* UDF related info */
    bcmi_lb_hash_flex_udf_chunks_info_t udf_chunks_info;

} bcmi_lb_hash_flex_fields_sel_lt_info_t;

/* Extern xgs lb hash flex fields sel LT info variable */
extern bcmi_lb_hash_flex_fields_sel_lt_info_t
            *xgs_lb_hash_flex_fields_sel_lt_info[BCM_MAX_NUM_UNITS];

#define LB_HASH_FLEX_FIELDS_SEL_LT_INFO(_u_)                \
            xgs_lb_hash_flex_fields_sel_lt_info[_u_]

#define LB_HASH_FLEX_FIELDS_SEL_LT_KEY_FIELDS_INFO(_u_)     \
            &(LB_HASH_FLEX_FIELDS_SEL_LT_INFO(_u_)->lt_key_info)

#define LB_HASH_FLEX_FIELDS_SEL_LT_DATA_FIELDS_INFO(_u_)    \
            &(LB_HASH_FLEX_FIELDS_SEL_LT_INFO(_u_)->lt_data_info)

#define LB_HASH_FLEX_FIELDS_SEL_LT_UDF_CHUNKS_INFO(_u_)     \
            &(LB_HASH_FLEX_FIELDS_SEL_LT_INFO(_u_)->udf_chunks_info)

extern int
xgs_ltsw_lb_hash_flex_fields_sel_init(int unit);

extern int
xgs_ltsw_lb_hash_flex_fields_sel_deinit(int unit);

#endif /* BCMINT_LTSW_XGS_LB_FLEX_HASH_H */
