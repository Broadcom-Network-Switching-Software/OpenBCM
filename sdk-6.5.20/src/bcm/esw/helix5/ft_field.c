/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_field.c
 * Purpose:     Flowtracker module specific routines in flowtracker
 *              stage in FP.
 */

#include <soc/defs.h>

#if defined (BCM_FLOWTRACKER_SUPPORT) && defined (BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>
#include <shared/pack.h>
#include <bcm/flowtracker.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/udf.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm_int/esw/flowtracker/ft_interface.h>
#include <bcm_int/esw_dispatch.h>

/* Internal MACRO to goto cleanup. */
#define BCMI_IF_ERROR_CLEANUP(_rv_)                                            \
        do {                                                                   \
            if (_rv_ != BCM_E_NONE) {                                          \
                goto cleanup;                                                  \
            }                                                                  \
        } while(0)

/* Macro to add group tracking elements map to database. */
#define _FP_FT_TRACKING_PARAM_QUAL_MAP_DECL                                    \
            _field_ft_tracking_param_qual_map_t _pq_map_

/* Macro to add tracking param to qualifier mapping database. */
#define _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(_unit_, _map_db_, _flags_,          \
                    _param_, _qual_, _start_, _width_, _ft_type_bmp_)          \
        do {                                                                   \
            _field_ft_tracking_param_qual_map_init((_unit_), &_pq_map_);       \
            _pq_map_.flags              = (_flags_);                           \
            _pq_map_.param              = (_param_);                           \
            _pq_map_.qual               = (_qual_);                            \
            _pq_map_.start              = (_start_);                           \
            _pq_map_.width              = (_width_);                           \
            _pq_map_.db_ft_type_bmp     = (_ft_type_bmp_);                     \
            rv = _field_ft_tracking_param_qual_map_insert((_unit_),            \
                        (_param_), &(_pq_map_), (_map_db_));                   \
            BCMI_IF_ERROR_CLEANUP(rv);                                         \
        } while(0)

/* Tracking param is not mapped to qualifier. */
#define _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED  (1 << 0)

/* Tracking param requires ALU 16-bit. */
#define _FP_FT_TRACKING_PARAM_F_ALU16_REQ       (1 << 1)

/* Tracking param required ALU 32-bit. */
#define _FP_FT_TRACKING_PARAM_F_ALU32_REQ       (1 << 2)

/* Tracking Param is mapped for ALU Data */
#define _FP_FT_TRACKING_PARAM_F_ALU_DATA        (1 << 3)

/* Tracking Param requires modulo 32 for Key */
#define _FP_FT_TRACKING_PARAM_F_KEY_MODULO_32   (1 << 4)

/* Tracking Param requires modulo 32 for Data */
#define _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32  (1 << 5)

/* Tracking Param does not generate Session Key/Data */
#define _FP_FT_TRACKING_PARAM_F_NO_FILL         (1 << 6)

/* Tracking param requires udf object */
#define _FP_FT_TRACKING_PARAM_F_UDF_MAPPED      (1 << 7)

/* Tracking Param may use 8b extractors for Data */
#define _FP_FT_TRACKING_PARAM_F_DATA_USE_GRAN8  (1 << 8)

/* Tracking param required ALU. */
#define _FP_FT_TRACKING_PARAM_F_ALU_REQ                                        \
                (_FP_FT_TRACKING_PARAM_F_ALU16_REQ |                           \
                _FP_FT_TRACKING_PARAM_F_ALU32_REQ)

/* Tracking info type is param info. */
#define _FP_FT_TRACKING_INFO_TYPE_PARAM         (1 << 0)

/* Tracking info type is flowcheck info. */
#define _FP_FT_TRACKING_INFO_TYPE_FLOWCHECK     (1 << 1)

/* Tracking info type is Alu data info */
#define _FP_FT_TRACKING_INFO_TYPE_ALU_DATA      (1 << 2)

/* Start Index of ALU Data in param_qual_map db */
#define _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START                               \
                (_FIELD_FT_TRACKING_ELEMENT_TYPE_MAX)
/*
 * Typedef:
 *   _field_ft_tracking_alu_data_info_t
 * Purpose:
 *   ALU Data related tracking information.
 */
typedef struct _field_ft_tracking_alu_data_info_s {
    /* ALU Data param */
    uint16 param;

    /* Corresponding qualifier */
    bcm_field_qualify_t qual;

    /* Offset within the qualifier */
    int offset;

    /* Width of ALU Data param */
    int width;

    /* Position after extraction */
    int extract_pos;
} _field_ft_tracking_alu_data_info_t;

/* FT Group Keygen Extraction process States */
#define _FIELD_FT_GROUP_KEYGEN_START                    0
#define _FIELD_FT_GROUP_KEYGEN_CFG_ALLOC                1
#define _FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD         2
#define _FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD          3
#define _FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD    4
#define _FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST     5
#define _FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST      6
#define _FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT        7
#define _FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT         8
#define _FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS        9
#define _FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS        10
#define _FIELD_FT_GROUP_KEYGEN_MFT_DATA_PROCESS        11
#define _FIELD_FT_GROUP_KEYGEN_EFT_DATA_PROCESS        12
#define _FIELD_FT_GROUP_KEYGEN_OPER_SAVE               13
#define _FIELD_FT_GROUP_KEYGEN_END                     14

/*
 * Typedef:
 *  _field_ft_group_keydata_udf_param_t
 * Purposes:
 *  UDF tracking params related information
 */
typedef struct _field_ft_group_keydata_udf_param_s {
    /* UDF Data qualifier */
    int qual_id;

    /* Tracking param */
    bcm_flowtracker_tracking_param_type_t param;

    /* Custom Id */
    uint16 custom_id;

    /* Chunk bitmap (2b per chunk) */
    uint32 chunk_hw_bmap;

} _field_ft_group_keydata_udf_param_t;

/*
 * Typedef:
 *   _field_ft_group_keydata_keygen_md_t
 * Purpose:
 *   Session key/data extraction related information.
 */
typedef struct _field_ft_group_keydata_keygen_md_s {
    /* List of allowed keygen mode for extraction. */
    bcmi_keygen_mode_t ext_modes[2];

    /* Number of valid indices in ext_modes. */
    int ext_modes_count;

    /* Keygen_cfg->qual_info_arr. */
    bcmi_keygen_qual_info_t *qual_info_arr;

    /* Corresponding to tracking param info index */
    uint16 qual_param_idx_arr[30];

    /* Total memory allocated to qual_info_arr. */
    uint16 max_qual_info_alloc;

    /* Actual number of qual info in qual_info_arr. */
    uint16 qual_info_count;

    /* Qset of qualifiers in qual_info_arr. */
    bcm_field_qset_t qset;

    /* Keygen_oper information after extraction. */
    bcmi_keygen_oper_t keygen_oper;

    /* Actual extraction Keygen mode. */
    bcmi_keygen_mode_t keygen_mode;

    /* Number of keygen parts. */
    uint16 num_parts;

    /* Udf data qualifier index to use */
    int next_data_qual_idx;

    /* Number of valid UDF param maps */
    int num_udf_maps;

    /* UDF data qualifiers mappings */
    _field_ft_group_keydata_udf_param_t param_udf_map[16];

} _field_ft_group_keydata_keygen_md_t;

/*
 * Typedef:
 *   _field_ft_group_keygen_oper_info_t
 * Purpose:
 *   Session key/data extraction information
 *   in the form required by FT module.
 */
typedef struct _field_ft_group_keygen_oper_info_s {
    /* keygen mode of session key. */
    int mode;

    /* Number of Session key info. */
    int num_skey_info;

    /* Session key info. */
    bcmi_ft_group_alu_info_t *skey_info;

    /* Number of elements in skey_ext_info. */
    int skey_ext_count;

    /* Encoded session Key ext codes. */
    bcmi_ft_hw_extractor_info_t *skey_ext_info;

    /* Number of Session data info. */
    int num_sdata_info;

    /* Session data info. */
    bcmi_ft_group_alu_info_t *sdata_info;

    /* Number of elements in sdata_ext_info. */
    int sdata_ext_count;

    /* Encoded Session data ext codes.  */
    bcmi_ft_hw_extractor_info_t *sdata_ext_info;

} _field_ft_group_keygen_oper_info_t;

/* Keygen flag for Session Key Extraction */
#define _FIELD_FTG_KEYGEN_F_KEY_REQ         (1 << 0)

/* Keygen flag for Session Data Extraction */
#define _FIELD_FTG_KEYGEN_F_DATA_REQ        (1 << 1)

/* Keygen flag for Session Alu data Extraction */
#define _FIELD_FTG_KEYGEN_F_ALU_DATA_REQ    (1 << 2)

/*
 * Typedef:
 *   _field_ft_group_keygen_md_t
 * Purpose:
 *   metadata for FT group keygen extraction for
 *   session key and session data.
 */
typedef struct _field_ft_group_keygen_md_s {
    uint32 flags;

    /* Current processing state in _FIELD_FT_GROUP_KEYGEN_XXX. */
    uint16 state;

    /* Previous processing state in _FIELD_FT_GROUP_KEYGEN_XXX. */
    uint16 prev_state;

    /* Return code for current processing state. */
    int state_rv;

    /* FT information in field stage struct. */
    _field_ft_info_t *ft_info;

    /* FT Group Identifier. */
    bcm_flowtracker_group_t ft_group_id;

    /* Number of elements in tracking_params_info. */
    int num_tracking_params;

    /* List of tracking params info for FT Group. */
    bcm_flowtracker_tracking_param_info_t *tracking_params_info;

    /* Number of elements in flowtracker_checks. */
    int num_flowchecks;

    /* List of flowcheck Ids for FT Group/ */
    bcm_flowtracker_check_t *flowtracker_checks;

    /* Number of ALU data elements. */
    int num_alu_data;

    /* Pointer to ALU Data info. */
    _field_ft_tracking_alu_data_info_t *alu_data_info;

    /* Metadata for session key keygen. */
    _field_ft_group_keydata_keygen_md_t skey_keygen_md;

    /* Metadata for session data keygen. */
    _field_ft_group_keydata_keygen_md_t sdata_keygen_md;

    /* Operational info for key/data extraction. */
    _field_ft_group_keygen_oper_info_t keydata_oper_info;

    /* Ft Type Bitmap */
    uint16 ft_type_bmp;

    /* Set if output is required to saved to FT Group DB. */
    uint8 save_op;

} _field_ft_group_keygen_md_t;

/*
 * Typedef:
 *   _bcm_field_ft_entry_traverse_cb
 * Purpose:
 *   Callback function pointer per entry in flowtracke stage.
 */
typedef int (*_bcm_field_ft_entry_traverse_cb)(int unit,
    bcm_field_entry_t entry, bcm_flowtracker_group_t id,
    void *user_data1, void *user_data2);

/* Local functions */
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
STATIC int
_field_ft_mmu_tracking_param_qual_map_db_init(int unit,
        _field_ft_tracking_param_qual_map_db_t *map_db);

STATIC int
_field_ft_egr_tracking_param_qual_map_db_init(int unit,
        _field_ft_tracking_param_qual_map_db_t *map_db);
#endif

/*
 * Function:
 *   _field_ft_keygen_udf_chunk_hw_bmap_get
 * Purpose:
 *   Get chunk hw bitmap based on offset/width
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   offset     - (IN) Offset within UDF object (in bytes)
 *   width      - (IN) Required width (in bytes)
 *   chunk_bmap - (IN) UDF chunk bitmap
 *   chunk_hw_bmap - (OUT) Chunk hw bitmap (2b per chunk)
 * Returns:
 */
STATIC int
_field_ft_keygen_udf_chunk_hw_bmap_get(int unit,
    uint16 offset, uint16 width,
    uint32 chunk_bmap, uint32 *chunk_hw_bmap)
{
    int chunk = 0;
    int rem_width = width;
    int rem_offset = offset;
#define _FP_FT_UDF_TOTAL_CHUNKS       16
#define _FP_FT_UDF_CHUNK_BYTES         2

    chunk = 0;
    while ((chunk < _FP_FT_UDF_TOTAL_CHUNKS) && (rem_width > 0)) {

        if (chunk_bmap & (1 << chunk)) {

            if (rem_offset >= _FP_FT_UDF_CHUNK_BYTES) {

                /* Skip both byes of the chunk */
                rem_offset -= _FP_FT_UDF_CHUNK_BYTES;
            } else if (rem_offset == 1) {

                /* Skip first byte of the chunk. Add second byte */
                (*chunk_hw_bmap) |= (0x2 << (chunk * _FP_FT_UDF_CHUNK_BYTES));
                rem_width  -= 1; rem_offset -= 1;
            } else if (rem_width >= _FP_FT_UDF_CHUNK_BYTES) {

                /* Add both bytes of the chunk */
                (*chunk_hw_bmap) |= (0x3 << (chunk * _FP_FT_UDF_CHUNK_BYTES));
                rem_width -= _FP_FT_UDF_CHUNK_BYTES;
            } else if (rem_width == 1) {

                /* Add only first byte of the chunk */
                (*chunk_hw_bmap) |= (0x1 << (chunk * _FP_FT_UDF_CHUNK_BYTES));
                rem_width -= 1;
            }
        }
        chunk++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_ft_group_keygen_param_udf_map_add
 * Purpose:
 *   Add udf param map to keygen md for extraction
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   keydata_keygen_md - (IN) Session key/data md
 *   param      - (IN) Tracking param
 *   custom_id  - (IN) Custom Id of the tracking param
 *   qual_id    - (OUT) Qualifier for the param
 *   index      - (OUT) Insert index
 *   new_qual   - (OUT) If newly inserted
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
_field_ft_group_keygen_param_udf_map_add(int unit,
    _field_ft_group_keydata_keygen_md_t *keydata_keygen_md,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id, uint16 *qual_id, int *index,
    int *new_qual)
{
    int rv = BCM_E_NONE;
    int idx = 0, found = FALSE;
    int data_qual_idx = 0;
    uint32 param_udf_hw_bmap = 0;
    _bcm_udf_td3_obj_info_t *obj_info = NULL;
    bcm_flowtracker_udf_info_t param_udf_info;
    _field_ft_group_keydata_udf_param_t *param_udf_map = NULL;

    uint16 data_qual_list[] = {
        _bcmFieldQualifyData0, _bcmFieldQualifyData1,
        _bcmFieldQualifyData2, _bcmFieldQualifyData3,
        _bcmFieldQualifyData4, _bcmFieldQualifyData5,
        _bcmFieldQualifyData6, _bcmFieldQualifyData7,
        _bcmFieldQualifyData8, _bcmFieldQualifyData9,
        _bcmFieldQualifyData10, _bcmFieldQualifyData11,
        _bcmFieldQualifyData12, _bcmFieldQualifyData13,
        _bcmFieldQualifyData14, _bcmFieldQualifyData15
    };

    /* Get param udf mapping */
    rv = bcm_esw_flowtracker_udf_tracking_param_get(unit,
            0, param, custom_id, &param_udf_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Get UDF object info */
    rv = _bcm_udf_td3_object_info_get(unit, param_udf_info.udf_id, &obj_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Get valid param UDF chunk hw bitmap */
    _field_ft_keygen_udf_chunk_hw_bmap_get(unit, param_udf_info.offset/8,
            param_udf_info.width/8, obj_info->chunk_bmap, &param_udf_hw_bmap);

    /* Validate hw bitmap against previous maps */
    for (idx = 0, found = FALSE; idx < keydata_keygen_md->num_udf_maps; idx++) {

        param_udf_map = &keydata_keygen_md->param_udf_map[idx];

        if ((param_udf_map->param == param) &&
                (param_udf_map->custom_id == custom_id) &&
                (param_udf_map->chunk_hw_bmap == param_udf_hw_bmap)) {
            found = TRUE;
            break;
        }

        /* Current hw bitmap conflict with previos hw bitmap */
        if (param_udf_map->chunk_hw_bmap & param_udf_hw_bmap) {
            return BCM_E_CONFIG;
        }
    }

    /* Add to the map list if not found */
    if (found == FALSE) {

        if (keydata_keygen_md->next_data_qual_idx >= COUNTOF(data_qual_list)) {
            return BCM_E_INTERNAL;
        }

        data_qual_idx = keydata_keygen_md->next_data_qual_idx;
        param_udf_map = &keydata_keygen_md->param_udf_map[idx];
        param_udf_map->param = param;
        param_udf_map->custom_id = custom_id;
        param_udf_map->chunk_hw_bmap = param_udf_hw_bmap;
        param_udf_map->qual_id = data_qual_list[data_qual_idx];

        keydata_keygen_md->num_udf_maps++;
        keydata_keygen_md->next_data_qual_idx++;
        *new_qual = TRUE;
    } else {

        param_udf_map = &keydata_keygen_md->param_udf_map[idx];
        *new_qual = FALSE;
    }

    *qual_id = param_udf_map->qual_id;
    *index = idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_ft_group_keygen_udf_qual_cfg_init
 * Purpose:
 *   Init data qualifier in keygen qual cfg db
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   qual_cfg_info_db - (IN) Keygen qual cfg db
 *   qual_id    - (IN) Data qualifier Id
 *   chunk_hw_bmap - (IN) Chunk hw bitmap for data qualifier
 *   qual_width    - (OUT) Width of the qualifier
 *   qual_cfg_parts- (OUT) Number of parts of quaifier config in db
 * Returns:
 *   BCM_E_xxx
 */
STATIC int
_field_ft_group_keygen_udf_qual_cfg_init(int unit,
    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db,
    uint16 qual_id, uint32 chunk_hw_bmap, int *qual_width,
    int *qual_cfg_parts)
{
    uint8 hw_bmap = 0;
    int chunk = 0, cfg_num = 0;
    int offset = 0, qual_bits = 0;
    bcmi_keygen_qual_flags_bmp_t qual_flags;

    /* UDF Opaque fields in Ingress Field Bus */
    int in_field_bus_opaque_offset[] = {
        784, 768, 752, 736, 720, 704, 688, 672,
        944, 928, 896, 872, 848, 832, 816, 800
    };

    BCMI_KEYGEN_FUNC_ENTER(unit);
    BCMI_KEYGEN_QUAL_CFG_DECL;

    BCMI_KEYGEN_QUAL_CFG_INIT(unit);
    sal_memset(&qual_flags, 0, sizeof(bcmi_keygen_qual_flags_bmp_t));

    for (chunk = (_FP_FT_UDF_TOTAL_CHUNKS - 1); chunk >= 0; chunk--) {

        /* Get hw bitmap */
        hw_bmap = (chunk_hw_bmap >> (chunk * _FP_FT_UDF_CHUNK_BYTES)) & 0x3;


        qual_bits = 0;
        offset = in_field_bus_opaque_offset[chunk];
        switch(hw_bmap) {
            case 0x1: /* Second byte */
                offset += 8;
                qual_bits = 8;
                break;
            case 0x2: /* First byte */
                qual_bits = 8;
                break;
            case 0x3: /* Both bytes */
                qual_bits = 16;
                break;
            default: /* None */
                break;
        }
        if (qual_bits == 0) {
            continue;
        }

        /* Update Qualifier Width */
        *qual_width += qual_bits;

        /* If offset is non-multiple of 16, add single byte */
        if ((offset % 16) != 0) {
            BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(cfg_num,
                    BCMI_KEYGEN_EXT_SECTION_L1E16,
                    offset / 16, offset, 8);
            qual_bits -= 8;
            offset += 8;
            cfg_num++;
        }

        /* Add qual cfg */
        if (qual_bits != 0) {
            BCMI_KEYGEN_QUAL_CFG_OFFSET_ADD(cfg_num,
                    BCMI_KEYGEN_EXT_SECTION_L1E16,
                    offset / 16, offset, qual_bits);
            cfg_num++;
        }
    }

    BCMI_KEYGEN_QUAL_CFG_INSERT(unit, qual_cfg_info_db, qual_id, qual_flags);
    *qual_cfg_parts = cfg_num;

exit:

    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function:
 *   _field_ft_group_param_udf_qual_init
 * Purpose:
 *   Init data qualifier for given <param, custom>.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   keydata_keygen_md - (IN) Session key/data md
 *   param      - (IN) Tracking param
 *   custom_id  - (IN) Custom Id of the tracking param
 *   qual_id    - (OUT) Qualifier for the param
 *   flags      - (OUT) Qualifier flags
 * Returns:
 */
STATIC int
_field_ft_group_param_udf_qual_init(int unit,
    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db,
    _field_ft_group_keydata_keygen_md_t *keydata_keygen_md,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id,
    bcm_field_qualify_t *qual,
    uint32 *flags)
{
    int rv = 0;
    int index = 0;
    int qual_cfg_parts = 0;
    int qual_width = 0;
    int new_qual = FALSE;
    uint16 data_qual_id;

    /* validate and get udf data qualifier */
    rv = _field_ft_group_keygen_param_udf_map_add(unit,
            keydata_keygen_md, param, custom_id,
            &data_qual_id, &index, &new_qual);
    BCM_IF_ERROR_RETURN(rv);

    *qual = (bcm_field_qualify_t) data_qual_id;

    if (new_qual == FALSE) {
        return BCM_E_NONE;
    }

    /* Add udf qual cfg to db */
    rv = _field_ft_group_keygen_udf_qual_cfg_init(unit,
            qual_cfg_info_db, data_qual_id,
            keydata_keygen_md->param_udf_map[index].chunk_hw_bmap,
            &qual_width, &qual_cfg_parts);
    BCM_IF_ERROR_RETURN(rv);


    /* Derive qual flags from width and parts */
    if (((qual_width == 16) && (qual_cfg_parts == 2)) ||
            ((qual_width > 16) && (qual_cfg_parts > 2))) {
        *flags |= _FP_FT_TRACKING_PARAM_F_DATA_USE_GRAN8;
    } else if (qual_width > 16) {
        *flags |= _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32;
    }

    return rv;
}

/*
 * Function:
 *   _field_ft_group_param_udf_qual_get
 * Purpose:
 *   Get mapped data qualifier for given <param, custom>.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   element_map- (IN) group tracking element map
 * Returns:
 */
STATIC int
_field_ft_group_param_udf_qual_get(int unit,
    _field_ft_group_keydata_keygen_md_t *keydata_keygen_md,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 custom_id, bcm_field_qualify_t *qual)
{
    int idx;

    for (idx = 0; idx < keydata_keygen_md->num_udf_maps; idx++) {
        if ((keydata_keygen_md->param_udf_map[idx].param == param) &&
            (keydata_keygen_md->param_udf_map[idx].custom_id == custom_id)) {
            break;
        }
    }
    if (idx == keydata_keygen_md->num_udf_maps) {
        return BCM_E_NOT_FOUND;
    }

    *qual = keydata_keygen_md->param_udf_map[idx].qual_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_ft_tracking_param_qual_map_init
 * Purpose:
 *   Initializes group tracking element map
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   element_map- (IN) group tracking element map
 * Returns:
 */
STATIC void
_field_ft_tracking_param_qual_map_init(int unit,
                _field_ft_tracking_param_qual_map_t *pq_map)
{
    if (pq_map == NULL) {
        return;
    }

    sal_memset(pq_map, 0,
            sizeof(_field_ft_tracking_param_qual_map_t));

    return;
}

/*
 * Function:
 *   _field_ft_tracking_param_qual_map_insert
 * Purpose:
 *   Insert tracking param-qual map to map database.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   param      - (IN) tracking param
 *   pq_map     - (IN) tracking param-qual map
 *   map_db     - (IN/OUT) param and qual Map database
 * Returns:
 *   BCM_E_NONE
 */
STATIC int
_field_ft_tracking_param_qual_map_insert(int unit,
                uint16 param,
                _field_ft_tracking_param_qual_map_t *pq_map,
                _field_ft_tracking_param_qual_map_db_t *map_db)
{
    _field_ft_tracking_param_qual_map_t *temp_map = NULL;

    /* Validate input params. */
    if ((pq_map == NULL) || (map_db == NULL) ||
            (param >= _FIELD_FT_TRACKING_PARAM_MAX)) {
        return BCM_E_PARAM;
    }

    temp_map = map_db->param_qual_map[param];

    if (temp_map == NULL) {
        _FP_XGS3_ALLOC(temp_map,
                sizeof(_field_ft_tracking_param_qual_map_t),
                "tracking param-qual map");
    } else {
        return BCM_E_INTERNAL;
    }

    if (temp_map == NULL) {
        return BCM_E_MEMORY;
    }

    map_db->param_qual_map[param] = temp_map;
    sal_memcpy(temp_map, pq_map,
            sizeof(_field_ft_tracking_param_qual_map_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_ft_tracking_param_qual_map_db_cleanup
 * Purpose:
 *   Cleanup tracking param and qualifier map database
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   ft_info    - (IN) Flowtracker specific info
 * Returns:
 *   None.
 */
void
_field_ft_tracking_param_qual_map_db_cleanup(int unit,
                                             _field_ft_info_t *ft_info)
{
    uint16 param = 0;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    if ((ft_info == NULL) ||
        (ft_info->param_qual_map_db == NULL)) {
        return;
    }

    map_db = ft_info->param_qual_map_db;

    for (param = 0; param < _FIELD_FT_TRACKING_PARAM_MAX; param++) {
        if (map_db->param_qual_map[param]) {
            sal_free(map_db->param_qual_map[param]);
            map_db->param_qual_map[param] = NULL;
        }
    }

    sal_free(map_db);
    ft_info->param_qual_map_db = NULL;
}

STATIC int
_field_ft_stage_is_qual_supported(int unit,
        bcm_field_qualify_t qual,
        bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db)
{
    if (qual_cfg_info_db == NULL) {
        return FALSE;
    }

    if (qual_cfg_info_db->qual_cfg_info[qual] != NULL) {
        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 *   _field_ft_tracking_param_qual_map_db_init
 * Purpose:
 *   Initialize tracking params and qualifier map database
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   ft_info    - (IN/OUT) Flowtracker info structure
 * Returns:
 *   BCM_E_XXX
 */
int
_field_ft_tracking_param_qual_map_db_init(int unit,
        _field_ft_info_t *ft_info,
        bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db)
{
    int rv = BCM_E_NONE;
    uint16 shift = 0;
    uint16 ft_type_bmp = 0;
    uint32 flags = 0;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _FP_FT_TRACKING_PARAM_QUAL_MAP_DECL;

    if (ft_info == NULL) {
        return BCM_E_PARAM;
    }

    _FP_XGS3_ALLOC(map_db,
            sizeof(_field_ft_tracking_param_qual_map_db_t),
            "tracking param qual map db");
    if (map_db == NULL) {
        return BCM_E_MEMORY;
    }

    ft_info->param_qual_map_db = map_db;

    /* Tracking param qualifier map. */
    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeSrcIPv4,
            bcmFieldQualifySrcIp, 0, 32, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeDstIPv4,
            bcmFieldQualifyDstIp, 0, 32, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeSrcIPv6,
            bcmFieldQualifySrcIp6, 0, 128, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeDstIPv6,
            bcmFieldQualifyDstIp6, 0, 128, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeL4SrcPort,
            bcmFieldQualifyL4SrcPort, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeL4DstPort,
            bcmFieldQualifyL4DstPort, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeIPProtocol,
            bcmFieldQualifyIpProtocol, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeVRF,
            bcmFieldQualifyVrf, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTTL,
            bcmFieldQualifyTtl, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeIPLength,
            bcmFieldQualifyIp4Length, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTcpWindowSize,
            bcmFieldQualifyTcpWindowSize, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeL2ForwardingField,
            bcmFieldQualifyVpn, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeVxlanNetworkId,
            bcmFieldQualifyVxlanNetworkId, 0, 24, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeNextHeader,
            bcmFieldQualifyIp6PktNextHeader, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeHopLimit,
            bcmFieldQualifyIp6PktHopLimit, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeInnerSrcIPv4,
            bcmFieldQualifyInnerSrcIp, 0, 32, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeInnerDstIPv4,
            bcmFieldQualifyInnerDstIp, 0, 32, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerSrcIPv6,
            bcmFieldQualifyInnerSrcIp6, 0, 128, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerDstIPv6,
            bcmFieldQualifyInnerDstIp6, 0, 128, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerIPProtocol,
            bcmFieldQualifyInnerIpProtocol, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerTTL,
            bcmFieldQualifyInnerTtl, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerNextHeader,
            bcmFieldQualifyInnerIp6PktNextHeader, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerHopLimit,
            bcmFieldQualifyInnerIp6PktHopLimit, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerL4SrcPort,
            bcmFieldQualifyInnerL4SrcPort, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerL4DstPort,
            bcmFieldQualifyInnerL4DstPort, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTcpFlags,
            bcmFieldQualifyTcpControl, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerTcpFlags,
            bcmFieldQualifyInnerTcpControl, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeOuterVlanTag,
            bcmFieldQualifyOuterVlan, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeIP6Length,
            bcmFieldQualifyIp6Length, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerIPLength,
            bcmFieldQualifyTunnelPayloadIp4Length, 0, 16, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeInnerIP6Length,
            bcmFieldQualifyTunnelPayloadIp6Length, 0, 16, ft_type_bmp);

    if (_field_ft_stage_is_qual_supported(unit,
                bcmFieldQualifyGbpSrcId, qual_cfg_info_db)) {
        ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
        _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                bcmFlowtrackerTrackingParamTypeGbpSrcId,
                bcmFieldQualifyGbpSrcId, 0, 16, ft_type_bmp);
    }

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeTunnelClass,
            bcmFieldQualifyTunnelId, 0, 24, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags, bcmFlowtrackerTrackingParamTypeIngPacketTos,
            bcmFieldQualifyTos, 0, 8, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_UDF_MAPPED,
            bcmFlowtrackerTrackingParamTypeTcpSeqNumber,
            0, 0, 32, ft_type_bmp);

    /* Width unknown */
    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_UDF_MAPPED,
            bcmFlowtrackerTrackingParamTypeCustom,
            0, 0, 0, ft_type_bmp);

    /* Tracking param which uses ALU32 */
    flags = _FP_FT_TRACKING_PARAM_F_ALU32_REQ;

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeDosAttack,
            bcmFieldQualifyDosAttackEvents, 0, 32, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32,
            bcmFlowtrackerTrackingParamTypeInnerDosAttack,
            bcmFieldQualifyInnerDosAttackEvents, 0, 32, ft_type_bmp);

    /* Tracking param with no qualifier, used for counting */
    flags |= _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT |
            BCMI_FT_TYPE_F_AMFT | BCMI_FT_TYPE_F_AEFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypePktCount,
            0, 0, 32, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT |
            BCMI_FT_TYPE_F_AMFT | BCMI_FT_TYPE_F_AEFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeByteCount,
            0, 0, 32, ft_type_bmp);

    /* Timestamp tracking parameters are not sent to FT group. */
    flags = _FP_FT_TRACKING_PARAM_F_NO_FILL;
    flags |= _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;


    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampNewLearn,
            0, 0, 0, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampFlowStart,
            0, 0, 0, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampFlowEnd,
            0, 0, 0, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_MICRO);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1,
            0, 0, 0, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_MICRO);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2,
            0, 0, 0, ft_type_bmp);
    ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampIngress,
            0, 0, 0, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_EFT);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeTimestampEgress,
            0, 0, 0, ft_type_bmp);

    /* Map Tracking Params to ignore processing. */
    flags = _FP_FT_TRACKING_PARAM_F_NO_FILL;
    flags |= _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;

    ft_type_bmp = (BCMI_FT_TYPE_F_ANY);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeNone,
            0, 0, 0, ft_type_bmp);

    ft_type_bmp = (BCMI_FT_TYPE_F_ANY);
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            bcmFlowtrackerTrackingParamTypeFlowtrackerCheck,
            0, 0, 0, ft_type_bmp);

    /* Tracking ALU Data */
    shift = _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START;
    flags = _FP_FT_TRACKING_PARAM_F_ALU_DATA;
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsFIN + shift,
            bcmFieldQualifyTcpControl, 0, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsSYN + shift,
            bcmFieldQualifyTcpControl, 1, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsRST + shift,
            bcmFieldQualifyTcpControl, 2, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsPSH + shift,
            bcmFieldQualifyTcpControl, 3, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsACK + shift,
            bcmFieldQualifyTcpControl, 4, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsURG + shift,
            bcmFieldQualifyTcpControl, 5, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsECE + shift,
            bcmFieldQualifyTcpControl, 6, 1, ft_type_bmp);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
            _bcmFieldFtAluDataTcpFlagsCWR + shift,
            bcmFieldQualifyTcpControl, 7, 1, ft_type_bmp);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    /* Add MFT/EFT tracking params in db */
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        flags = _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;

        /* Enable chip and end-to-end delay tracking only if
         * feature is enabled which in turn is set based on
         * OTP bit (LATENCY_TRACKING_DISABLE) */
        if (soc_feature(unit, soc_feature_flowtracker_ver_2_chip_e2e_delay)) {
            ft_type_bmp = (BCMI_FT_TYPE_F_EFT);
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                    flags | _FP_FT_TRACKING_PARAM_F_ALU32_REQ,
                    bcmFlowtrackerTrackingParamTypeChipDelay,
                    0, 0, 32, ft_type_bmp);

            ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                    flags | _FP_FT_TRACKING_PARAM_F_ALU32_REQ,
                    bcmFlowtrackerTrackingParamTypeE2EDelay,
                    0, 0, 32, ft_type_bmp);
        }

        /* Enable inter packet arrival and departure delay tracking only
         * if feature is enabled which in turn is set based on
         * OTP bit (IAT_IDT_TRACKING_DISABLE) */
        if (soc_feature(unit, soc_feature_flowtracker_ver_2_iat_idt_delay)) {
            ft_type_bmp = (BCMI_FT_TYPE_F_IFT);
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                    flags | _FP_FT_TRACKING_PARAM_F_ALU32_REQ,
                    bcmFlowtrackerTrackingParamTypeIPATDelay,
                    0, 0, 32, ft_type_bmp);

            ft_type_bmp = (BCMI_FT_TYPE_F_EFT);
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                    flags | _FP_FT_TRACKING_PARAM_F_ALU32_REQ,
                    bcmFlowtrackerTrackingParamTypeIPDTDelay,
                    0, 0, 32, ft_type_bmp);
        }

        ft_type_bmp = (BCMI_FT_TYPE_F_MICRO);
        _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                flags | _FP_FT_TRACKING_PARAM_F_NO_FILL,
                bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3,
                0, 0, 0, ft_type_bmp);

        ft_type_bmp = (BCMI_FT_TYPE_F_MICRO);
        _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                flags | _FP_FT_TRACKING_PARAM_F_NO_FILL,
                bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4,
                0, 0, 0, ft_type_bmp);

        /* Enable alu delay computation only if feature
         * is enabled which in turn is set based on
         * OTP bit (ALU_LATENCY_SUPPORT_DISABLE) */
        if (soc_feature(unit, soc_feature_flowtracker_ver_2_alu_delay)) {
            ft_type_bmp = (BCMI_FT_TYPE_F_IFT | BCMI_FT_TYPE_F_AIFT);
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                    flags | _FP_FT_TRACKING_PARAM_F_ALU32_REQ,
                    bcmFlowtrackerTrackingParamTypeIngressDelay,
                    0, 0, 32, ft_type_bmp);

            ft_type_bmp = (BCMI_FT_TYPE_F_EFT | BCMI_FT_TYPE_F_AEFT);
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db,
                    flags | _FP_FT_TRACKING_PARAM_F_ALU32_REQ,
                    bcmFlowtrackerTrackingParamTypeEgressDelay,
                    0, 0, 32, ft_type_bmp);
        }

        /* MMU Tracking Params */
        rv = _field_ft_mmu_tracking_param_qual_map_db_init(unit, map_db);
        BCMI_IF_ERROR_CLEANUP(rv);

        /* Egress Tracking Params */
        rv = _field_ft_egr_tracking_param_qual_map_db_init(unit, map_db);
        BCMI_IF_ERROR_CLEANUP(rv);
    }
#endif

cleanup:

    if (rv != BCM_E_NONE) {
        (void) _field_ft_tracking_param_qual_map_db_cleanup(unit, ft_info);
    }

    return rv;
}

/* Alu Data info For Outer TCP Control */
static _field_ft_tracking_alu_data_info_t
_field_ft_alu_data_info_tcp_control[] = {
    {_bcmFieldFtAluDataTcpFlagsFIN, bcmFieldQualifyTcpControl, 0, 1, 2},
    {_bcmFieldFtAluDataTcpFlagsSYN, bcmFieldQualifyTcpControl, 1, 1, 0},
    {_bcmFieldFtAluDataTcpFlagsRST, bcmFieldQualifyTcpControl, 2, 1, 3},
    {_bcmFieldFtAluDataTcpFlagsPSH, bcmFieldQualifyTcpControl, 3, 1, 4},
    {_bcmFieldFtAluDataTcpFlagsACK, bcmFieldQualifyTcpControl, 4, 1, 1},
    {_bcmFieldFtAluDataTcpFlagsURG, bcmFieldQualifyTcpControl, 5, 1, 5},
    {_bcmFieldFtAluDataTcpFlagsECE, bcmFieldQualifyTcpControl, 6, 1, 6},
    {_bcmFieldFtAluDataTcpFlagsCWR, bcmFieldQualifyTcpControl, 7, 1, 7}
};

/* Alu Data info for Inner TCP Control */
static _field_ft_tracking_alu_data_info_t
_field_ft_alu_data_info_inner_tcp_control[] = {
    {_bcmFieldFtAluDataTcpFlagsFIN, bcmFieldQualifyInnerTcpControl, 0, 1, 2},
    {_bcmFieldFtAluDataTcpFlagsSYN, bcmFieldQualifyInnerTcpControl, 1, 1, 0},
    {_bcmFieldFtAluDataTcpFlagsRST, bcmFieldQualifyInnerTcpControl, 2, 1, 3},
    {_bcmFieldFtAluDataTcpFlagsPSH, bcmFieldQualifyInnerTcpControl, 3, 1, 4},
    {_bcmFieldFtAluDataTcpFlagsACK, bcmFieldQualifyInnerTcpControl, 4, 1, 1},
    {_bcmFieldFtAluDataTcpFlagsURG, bcmFieldQualifyInnerTcpControl, 5, 1, 5},
    {_bcmFieldFtAluDataTcpFlagsECE, bcmFieldQualifyInnerTcpControl, 6, 1, 6},
    {_bcmFieldFtAluDataTcpFlagsCWR, bcmFieldQualifyInnerTcpControl, 7, 1, 7}
};

/*
 * Function:
 *   _field_hx5_ftg_alu_data_info_get
 * Purpose:
 *   Get pointer to ALU Data info
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) flowtracker group id.
 *   info       - (OUT) Pointer to ALU Data Info.
 *   num_info   - (OUT) number of ALU Data info.
 * Returns:
 *   BCM_E_NONE
 * Notes:
 *   ALU Data info is fixed to extract TCP Flags.
 *   If ALU data needs to be extracted from other source
 *   flowtracker group need to convey it.
 *   This function set the pointer to fixed ALU Data info
 *   and no memory allocated is required for now.
 */
STATIC int
_field_hx5_ftg_alu_data_info_get(int unit,
                bcm_flowtracker_group_t id,
                _field_ft_tracking_alu_data_info_t **info,
                int *num_info)
{
    int rv = BCM_E_NONE;
    int alu_data_info_count = 0;
    bcm_flowtracker_state_transition_type_t st_type;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;

    rv = bcmi_esw_ft_group_state_transition_type_get(unit, id, &st_type);
    BCM_IF_ERROR_RETURN(rv);

    switch(st_type) {
        case bcmFlowtrackerStateTransitionTypeTcpControl:
            alu_data_info = _field_ft_alu_data_info_tcp_control;
            alu_data_info_count = COUNTOF(_field_ft_alu_data_info_tcp_control);
            break;

        case bcmFlowtrackerStateTransitionTypeInnerTcpControl:
            alu_data_info = _field_ft_alu_data_info_inner_tcp_control;
            alu_data_info_count = COUNTOF(_field_ft_alu_data_info_inner_tcp_control);
            break;

        default:
            return BCM_E_PARAM;
    }

    if (num_info != NULL) {
        *num_info = alu_data_info_count;
    }

    if (info != NULL) {
        *info = alu_data_info;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_qual_alloc
 * Purpose:
 *   Allocate memory for qual_info_arr for session key and
 *   session data.
 * Parameters:
 *   unit             - (IN) BCM device number.
 *   ftg_keygen_md    - (IN) Ft group Keygen metadata
 * Returns:
 *   None.
 */
STATIC int
_field_hx5_ftg_keygen_cfg_qual_alloc(int unit,
                            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int num_key_params = 0;
    int num_data_params = 0;
    int num_tracking_params = 0;
    int num_flowchecks = 0;
    int total_data_quals = 0;
    int num_alu_data = 0;
    bcm_field_qualify_t qual;
    bcm_field_qset_t qset;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    num_tracking_params = ftg_keygen_md->num_tracking_params;
    num_flowchecks = ftg_keygen_md->num_flowchecks;
    num_alu_data = ftg_keygen_md->num_alu_data;

    /* Calculate number of keys and data */
    for (idx = 0; idx < num_tracking_params; idx++) {
        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];
        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            num_key_params++;
        } else {
            num_data_params++;
        }
    }

    /* Validate number of key and data */
    if (ftg_keygen_md->flags & _FIELD_FTG_KEYGEN_F_KEY_REQ) {
        if (num_key_params == 0) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "FT Group=%d - Tracking Key and/or"
                                " Data not specified\n\r"),
                     ftg_keygen_md->ft_group_id));
            return BCM_E_CONFIG;
        }
    }

    if (ftg_keygen_md->flags & _FIELD_FTG_KEYGEN_F_DATA_REQ) {
        if (num_data_params == 0) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "FT Group=%d - Tracking Key and/or"
                                " Data not specified\n\r"),
                     ftg_keygen_md->ft_group_id));
            return BCM_E_CONFIG;
        }
    }

    /* Calculate number of session data qualifiers */
    total_data_quals = 0;

    /* Qualifiers for ALU Data */
    BCM_FIELD_QSET_INIT(qset);
    for (idx = 0; idx < num_alu_data; idx++) {
        alu_data_info = &ftg_keygen_md->alu_data_info[idx];
        qual = alu_data_info->qual;

        if (!(BCM_FIELD_QSET_TEST(qset, qual))) {
            total_data_quals++;
            BCM_FIELD_QSET_ADD(qset, qual);
        }
    }

    /*
     * Qualifiers in Flowtracker checks
     * Primary flowcheck can have action params also.
     * So, need to allocate memory for them too.
     */
    total_data_quals += (2 * num_flowchecks);

    /* Qualifiers in Tracking params fo session data */
    total_data_quals += num_data_params;

    /* Allocate qual_info_arr for session Key */
    _FP_XGS3_ALLOC(qual_info_arr,
            num_key_params * sizeof(bcmi_keygen_qual_info_t),
            "skey qual info arr");
    if (qual_info_arr == NULL) {
        rv = BCM_E_MEMORY;
    } else {
        ftg_keygen_md->skey_keygen_md.qual_info_arr = qual_info_arr;
        ftg_keygen_md->skey_keygen_md.max_qual_info_alloc = num_key_params;
    }

    /* Allocate qual_info_arr for session Data */
    if (BCM_SUCCESS(rv)) {
        qual_info_arr = NULL;
        _FP_XGS3_ALLOC(qual_info_arr,
                total_data_quals * sizeof(bcmi_keygen_qual_info_t),
                "sdata qual info arr");
        if (qual_info_arr == NULL) {
            rv = BCM_E_MEMORY;
        } else {
            ftg_keygen_md->sdata_keygen_md.qual_info_arr = qual_info_arr;
            ftg_keygen_md->sdata_keygen_md.max_qual_info_alloc = total_data_quals;
        }
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_qual_ext_hw_info_get
 * Purpose:
 *   Convert keygen_oper to qual_ext_offset_info and ext_codes.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   key_or_data       - (IN) Conversion of session key or data.
 *   ftg_keygen_md     - (IN) FT Group Keygen metadata.
 *   qual_ext_offset_info - (OUT) Pointer to qual-extractor info.
 *   ext_codes         - (OUT) Pointer to extraction codes.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_qual_ext_hw_info_get(int unit,
            int key_or_data,
            _field_ft_group_keygen_md_t *ftg_keygen_md,
            _field_ft_qual_ext_offset_info_t **qual_ext_offset_info,
            _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int keygen_profile = 0;
    int num_keygen_parts = 0;
    uint16 ext_ctrl_sel_info_count = 0;
    bcmi_keygen_oper_t *keygen_oper = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info = NULL;

    if ((ftg_keygen_md == NULL) ||
        (ext_codes == NULL) ||
        (qual_ext_offset_info == NULL)) {
        return BCM_E_PARAM;
    }

    if (key_or_data == 1) {
        keygen_oper = &ftg_keygen_md->skey_keygen_md.keygen_oper;
        num_keygen_parts = ftg_keygen_md->skey_keygen_md.num_parts;
        keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AAB;
    } else {
        keygen_oper = &ftg_keygen_md->sdata_keygen_md.keygen_oper;
        num_keygen_parts = ftg_keygen_md->sdata_keygen_md.num_parts;
        keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AB;
    }

    /* Get qual_ext_offset_info */
    ext_ctrl_sel_info = keygen_oper->ext_ctrl_sel_info;
    ext_ctrl_sel_info_count = keygen_oper->ext_ctrl_sel_info_count;

    /* Dump Qualifiers and Extractors info from Keygen_oper */
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FT Group=%d Keygen Info for %s\r\n"),
                ftg_keygen_md->ft_group_id,
                ((key_or_data == 1) ? "SESSION KEY" : "SESSION DATA")));
    (void) _bcm_field_keygen_qual_offset_info_dump(unit,
                num_keygen_parts, keygen_oper);

    (void) _bcm_field_ft_keygen_extractors_init(ext_codes);
    rv = _bcm_field_hx5_ft_keygen_extractors_set(unit, keygen_profile,
            ext_ctrl_sel_info_count, ext_ctrl_sel_info, ext_codes);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Process extracted qualifiers and extractors */
    if (ext_ctrl_sel_info != NULL) {
        rv = _field_hx5_ft_qual_ext_offset_info_get(unit,
                num_keygen_parts,
                keygen_oper, qual_ext_offset_info);
        BCMI_IF_ERROR_CLEANUP(rv);

        /* Update Mask of extractors */
        rv = _bcm_field_hx5_ft_keygen_extractors_mask_set(unit,
                *qual_ext_offset_info, ext_codes);
        BCMI_IF_ERROR_CLEANUP(rv);

        rv = _field_hx5_ft_qual_offset_ext_shift_apply(unit,
                0, ext_codes, *qual_ext_offset_info);
        BCMI_IF_ERROR_CLEANUP(rv);
    }

    /* Dump Hardware Extraction codes */
    (void) _bcm_field_hx5_ft_keygen_hw_extractors_dump(unit, ext_codes);

cleanup:
    if (BCM_FAILURE(rv)) {
        if (*qual_ext_offset_info != NULL) {
            sal_free(*qual_ext_offset_info);
            *qual_ext_offset_info = NULL;
        }
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_qual_offset_width_get
 * Purpose:
 *   Get start and width from offset_arr for a qualifier.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   offset_arr        - (IN) Offset arr.
 *   start_bit         - (OUT) Start bit position.
 *   width             - (OUT) Number of bits.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_qual_offset_width_get(int unit,
                bcmi_keygen_qual_offset_t *offset_arr,
                int *start_bit, int *width)
{
    int offset = 0;
    int part_width = 0;
    int total_width = 0;
    int num_offsets = 0;

    for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
        part_width = offset_arr->width[offset];
        if (!part_width) {
            continue;
        }
        total_width += part_width;
        num_offsets += 1;
        if (num_offsets == offset_arr->num_offsets) {
            break;
        }
    }

    *start_bit = offset_arr->offset[0];

    /* Make width multiple of 8 */
    if ((total_width % 8) != 0) {
        *width = ((total_width + 8) / 8) * 8;
    } else {
        *width = total_width;
    }
    return BCM_E_NONE;
}

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
STATIC int
_field_ftv2_ftg_vfp_opaqueid2_get(int unit, bcm_flowtracker_group_t id,
                            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int q_idx = 0;
    int start_bit = 0, width = 0;
    _field_ft_keygen_ext_codes_t ext_codes;
    _field_ft_qual_ext_offset_info_t *qual_ext_offset_info = NULL;

    rv = _field_hx5_ftg_qual_ext_hw_info_get(unit,
            0, ftg_keygen_md, &qual_ext_offset_info, &ext_codes);
    BCM_IF_ERROR_RETURN(rv);

    if (qual_ext_offset_info != NULL) {
        for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
            if (qual_ext_offset_info->qid_arr[q_idx] != bcmFieldQualifyOpaqueObject2) {
                continue;
            }
            rv = _field_hx5_ftg_qual_offset_width_get(unit,
                                    &qual_ext_offset_info->offset_arr[q_idx],
                                                          &start_bit, &width);
            BCM_IF_ERROR_RETURN(rv);
        }
        BCMI_FT_GROUP_EXT_HW_INFO_VFP_OPAQUEID2_START_BIT(unit, id) = start_bit;
    }

    return rv;
}
#endif

/*
 * Function:
 *   _field_hx5_ftg_keygen_oper_dump
 * Purpose:
 *   Dump session key/data extraction information.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC void
_field_hx5_ftg_keygen_oper_dump(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    uint32 idx = 0;
    bcmi_ft_group_alu_info_t *keydata_info = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;

    oper_info = &ftg_keygen_md->keydata_oper_info;

    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "===Extraction Operation Info===\r\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "---Session Keys---\r\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "  Session Key Mode = %d\r\n"),
             oper_info->mode));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "  Number of Session Keys = %d\r\n"),
             oper_info->num_skey_info));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "|%5s |%10s |%10s |%10s |\r\n"),
             "Idx", "Element", "Offset", "Length"));
    for (idx = 0; idx < oper_info->num_skey_info; idx++) {
        keydata_info = &oper_info->skey_info[idx];
        LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "|%5d |%10d |%10d |%10d |\r\n"),
                 idx,
                 keydata_info->element_type1,
                 keydata_info->key1.location,
                 keydata_info->key1.length));
    }
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "---Session Data---\r\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "  Number of Session Data = %d\r\n"),
             oper_info->num_sdata_info));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "|%5s |%10s |%10s |"),
             "Idx", "FlowCheck", "Type"));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "%10s |%10s |%10s |%10s |\r\n"),
             "Element", "Offset", "Length", "ALU Req"));
    for (idx = 0; idx < oper_info->num_sdata_info; idx++) {
        keydata_info = &oper_info->sdata_info[idx];
        LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "|%5d |%10x |%10s |"),
                 idx,
                 keydata_info->flowchecker_id,
                 "Primary"));
        LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "%10d |"
                            "%10d |%10d |%10d |\r\n"),
                 keydata_info->element_type1,
                 keydata_info->key1.location,
                 keydata_info->key1.length,
                 keydata_info->key1.is_alu));

        /* Print secondary and Action key */
        if (keydata_info->flowchecker_id != 0) {
            if (keydata_info->action_element_type !=
                    bcmFlowtrackerTrackingParamTypeNone) {
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "|%5d |%10x |%10s |"),
                         idx,
                         keydata_info->flowchecker_id,
                         "Action"));
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "%10d |"
                                    "%10d |%10d |%10d |\r\n"),
                         keydata_info->action_element_type,
                         keydata_info->action_key.location,
                         keydata_info->action_key.length,
                         keydata_info->key1.is_alu));
            }
            if (keydata_info->element_type2 !=
                    bcmFlowtrackerTrackingParamTypeNone) {
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "|%5d |%10x |%10s |"),
                         idx,
                         BCMI_FT_FLOWCHECKER_INDEX_SET(keydata_info->flowchecker_id,
                             BCM_FT_IDX_TYPE_2ND_CHECK),
                         "Second"));
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "%10d |"
                                    "%10d |%10d |%10d |\r\n"),
                         keydata_info->element_type2,
                         keydata_info->key2.location,
                         keydata_info->key2.length,
                         keydata_info->key2.is_alu));
            }
        }
    }
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_initialize
 * Purpose:
 *   Initialize ft group keygen metadata struct.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_initialize(int unit,
            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    _field_stage_t *stage_fc = NULL;
    bcm_flowtracker_group_type_t group_type;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    /* Validate Flowtracker group */
    rv = bcmi_ft_group_is_invalid(unit, ftg_keygen_md->ft_group_id);

    /* Get Flowtracker Stage info */
    if (BCM_SUCCESS(rv)) {
        rv = _field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    }

    if (BCM_SUCCESS(rv)) {
        /* Ft info from stage */
        ftg_keygen_md->ft_info = stage_fc->ft_info;
    }

    group_type = BCMI_FT_GROUP_TYPE_GET(ftg_keygen_md->ft_group_id);

    ftg_keygen_md->ft_type_bmp = 0;
    if (group_type == bcmFlowtrackerGroupTypeNone) {
        ftg_keygen_md->ft_type_bmp = BCMI_FT_TYPE_F_MICRO;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_KEY_REQ;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_DATA_REQ;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_ALU_DATA_REQ;
    } else if (group_type == bcmFlowtrackerGroupTypeAggregateIngress) {
        ftg_keygen_md->ft_type_bmp = BCMI_FT_TYPE_F_AIFT;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_DATA_REQ;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_ALU_DATA_REQ;
    } else if (group_type == bcmFlowtrackerGroupTypeAggregateMmu) {
        ftg_keygen_md->ft_type_bmp = BCMI_FT_TYPE_F_AMFT;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_DATA_REQ;
    } else if (group_type == bcmFlowtrackerGroupTypeAggregateEgress) {
        ftg_keygen_md->ft_type_bmp = BCMI_FT_TYPE_F_AEFT;
        ftg_keygen_md->flags |= _FIELD_FTG_KEYGEN_F_DATA_REQ;
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_ALLOC;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_alloc
 * Purpose:
 *   Read FT group tracking param and flowchecks info and allocate
 *   memories to qual_info_arr for input to Keygen algo.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_alloc(int unit,
            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int num_tracking_params = 0;
    int num_flowchecks = 0;
    bcm_flowtracker_group_t ft_group_id;

    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    ft_group_id = ftg_keygen_md->ft_group_id;

    /* Get number of tracking params from group template */
    rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
            ft_group_id, 0, NULL, &num_tracking_params);

    /* Get number of flowcheck info from flowtracker group */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_flowtracker_check_get_all(unit,
                ft_group_id, 0, 0, NULL, &num_flowchecks);
    }

    /* Allocate memory for tracking params info */
    if (BCM_SUCCESS(rv)) {
        ftg_keygen_md->num_tracking_params = num_tracking_params;
        _FP_XGS3_ALLOC(ftg_keygen_md->tracking_params_info,
            num_tracking_params * sizeof(bcm_flowtracker_tracking_param_info_t),
            "tracking params info");
        if (ftg_keygen_md->tracking_params_info == NULL) {
            rv = BCM_E_MEMORY;
        }
    }

    /* Allocate memory for flowchecks */
    if (BCM_SUCCESS(rv)) {
        ftg_keygen_md->num_flowchecks = num_flowchecks;
        _FP_XGS3_ALLOC(ftg_keygen_md->flowtracker_checks,
                num_flowchecks * sizeof(bcm_flowtracker_check_t),
                "flowtracker checks");
        if (ftg_keygen_md->flowtracker_checks == NULL) {
            rv = BCM_E_MEMORY;
        }
    }

    /* Read tracking params info */
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
                ft_group_id, num_tracking_params,
                &ftg_keygen_md->tracking_params_info[0],
                &num_tracking_params);
    }

    /* Read flowtracker checks */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_flowtracker_check_get_all(unit,
                ft_group_id, 0, num_flowchecks,
                &ftg_keygen_md->flowtracker_checks[0],
                &num_flowchecks);
    }

    /* Read ALU Data */
    if (BCM_SUCCESS(rv) &&
            (ftg_keygen_md->flags & _FIELD_FTG_KEYGEN_F_ALU_DATA_REQ)) {
        rv = _field_hx5_ftg_alu_data_info_get(unit, ft_group_id,
                &ftg_keygen_md->alu_data_info,
                &ftg_keygen_md->num_alu_data);
    }

    /* Allocate memory for qual_info arr of session key/Data */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_keygen_cfg_qual_alloc(unit, ftg_keygen_md);
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_alu_qual_add
 * Purpose:
 *   Read FT group Alu data info and populate qual_info_arr
 *   for session data extraction.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Notes:
 *   1. Added logic to skip alu param extraction for non IFT stage.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_alu_qual_add(int unit,
                _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int new_idx = 0;
    int idx = 0;
    bcm_field_qualify_t qual;
    uint16 allocated_quals = 0;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;

    /* Check if ALU data is required */
    if (ftg_keygen_md->num_alu_data == 0) {
        goto cleanup;
    }

    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    /* Add to Session Data qual_info_arr */
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;
    allocated_quals = sdata_keygen_md->qual_info_count;
    qual_info_arr = sdata_keygen_md->qual_info_arr;

    for (idx = 0; idx < ftg_keygen_md->num_alu_data; idx++) {

        alu_data_info = &ftg_keygen_md->alu_data_info[idx];
        qual = alu_data_info->qual;

        /* Skip if it is previously added */
        if (BCM_FIELD_QSET_TEST(sdata_keygen_md->qset, qual)) {
            continue;
        }

        new_idx = allocated_quals;
        qual_info_arr[new_idx].qual_id = qual;
        qual_info_arr[new_idx].flags =
            BCMI_KEYGEN_QUAL_F_STICKY_CONTAINERS |
            BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW |
            BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16;
        allocated_quals++;
        BCM_FIELD_QSET_ADD(sdata_keygen_md->qset, qual);
    }

    sdata_keygen_md->qual_info_count = allocated_quals;

cleanup:

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD;

    return rv;
}

STATIC int
_field_ft_keygen_qual_info_flags_set(int unit,
        int is_key, uint32 param_qual_flags,
        uint32 udf_qual_flags, uint32 *qual_info_flags)
{

    (*qual_info_flags) = BCMI_KEYGEN_QUAL_F_STICKY_CONTAINERS;

    if (is_key) {

        (*qual_info_flags) |= BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8;
        (*qual_info_flags) |= BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16;

    } else {

        (*qual_info_flags) |= BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW;
        (*qual_info_flags) |= BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16;

        if ((param_qual_flags & _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32) ||
                (udf_qual_flags & _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32)) {
            (*qual_info_flags) |=
                BCMI_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET;
        }

        if (udf_qual_flags & _FP_FT_TRACKING_PARAM_F_DATA_USE_GRAN8) {
            (*qual_info_flags) |= BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_flowcheck_qual_add
 * Purpose:
 *   Read FT group Flowcheck info and populate qual_info_arr
 *   for session data extraction.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Notes:
 *   1. Tracking Params which belongs to MFT/EFT stage does not need
 *      extraction hence skipped while adding in qual extraction db.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_flowcheck_qual_add(int unit,
                _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int p_idx = 0;
    int new_idx = 0;
    int num_params = 0;
    int num_operands = 0;
    uint16 param;
    uint16 param_list[2];
    uint16 custom_id_list[2];
    uint16 chk_ft_type_bmp = 0;
    uint16 ext_ft_type_flag = 0;
    uint16 allocated_quals = 0;
    uint32 udf_qual_flags = 0;
    bcm_field_qualify_t qual;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_check_info_t fc_info;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;
    bcm_flowtracker_check_action_info_t action_info;

    /* validate inputs */
    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    /* Check if Flowcheck is required */
    if (ftg_keygen_md->num_flowchecks == 0) {
        goto cleanup;
    }

    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;

    /* Add to Session Data qual_info_arr */
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;
    allocated_quals = sdata_keygen_md->qual_info_count;
    qual_info_arr = sdata_keygen_md->qual_info_arr;

    if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_IFT) {
        ext_ft_type_flag = BCMI_FT_TYPE_F_IFT;
    } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AIFT) {
        ext_ft_type_flag = BCMI_FT_TYPE_F_AIFT;
    }

    for (idx = 0; idx < ftg_keygen_md->num_flowchecks; idx++) {
        num_params = 0;
        check_id = ftg_keygen_md->flowtracker_checks[idx];

        sal_memset(&fc_info, 0, sizeof(bcm_flowtracker_check_info_t));
        rv = bcmi_ft_check_get(unit, check_id, &fc_info, &chk_ft_type_bmp);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        /* Check must belong to only ft_type */
        if (!BCMI_FT_TYPE_F_IS_UNIQUE(chk_ft_type_bmp & ext_ft_type_flag)) {
            return BCM_E_INTERNAL;
        }

        /* Extracting only if checkers ft types match */
        if (!(chk_ft_type_bmp & ext_ft_type_flag)) {
            continue;
        }

        param_list[0] = fc_info.param;
        custom_id_list[0] = fc_info.custom_id;
        num_params++;

        /* If primary flowcheck, get action info */
        if (BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
            bcm_flowtracker_check_action_info_t_init(&action_info);
            rv = bcm_esw_flowtracker_check_action_info_get(unit,
                                           check_id, &action_info);
            BCMI_IF_ERROR_CLEANUP(rv);

            /* If Action does not requires param, continue */
            rv = bcmi_esw_ft_check_action_num_operands_get(unit,
                    action_info.action, &num_operands);
            BCM_IF_ERROR_CLEANUP(rv);

            if (num_operands != 0) {
                param_list[1] = action_info.param;
                custom_id_list[1]  = action_info.custom_id;
                num_params++;
            }
        }

        for (p_idx = 0; p_idx < num_params; p_idx++) {
            param = param_list[p_idx];
            pq_map = map_db->param_qual_map[param];

            if ((param > _FIELD_FT_TRACKING_PARAM_MAX) || (pq_map == NULL)) {
                rv = BCM_E_PARAM;
                BCMI_IF_ERROR_CLEANUP(rv);
            }

            if (pq_map->flags &
                _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
                continue;
            }

            if (pq_map->flags &
                _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
                rv = _field_ft_group_param_udf_qual_init(unit,
                                ftg_keygen_md->ft_info->flow_data_qual_cfg,
                                sdata_keygen_md, param, custom_id_list[p_idx],
                                &qual, &udf_qual_flags);
                BCMI_IF_ERROR_CLEANUP(rv);
            } else {
                udf_qual_flags = 0;
                qual = pq_map->qual;
            }

            if (!BCM_FIELD_QSET_TEST(sdata_keygen_md->qset, qual)) {
                new_idx = allocated_quals;
                qual_info_arr[new_idx].qual_id = qual;

                _field_ft_keygen_qual_info_flags_set(unit,
                        0, pq_map->flags, udf_qual_flags,
                        &qual_info_arr[new_idx].flags);

                allocated_quals++;
                BCM_FIELD_QSET_ADD(sdata_keygen_md->qset, qual);
            }
        }
    }

    sdata_keygen_md->qual_info_count = allocated_quals;

cleanup:
    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_data_key_qual_add
 * Purpose:
 *   Read FT group Tracking param info and populate qual_info_arr
 *   for session key and session data extraction.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Notes:
 *   1. Tracking Params which belongs to MFT/EFT stage does not need
 *      extraction hence skipped while adding in qual extraction db.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_data_key_qual_add(int unit,
                    _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    uint16 param;
    int new_idx = 0;
    int is_key = FALSE;
    bcm_field_qualify_t qual;
    int num_tracking_params = 0;
    uint16 ext_ft_type_flag = 0;
    uint16 *allocated_quals = NULL;
    uint16 *qual_param_idx_arr = NULL;
    uint32 udf_qual_flags = 0;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    num_tracking_params = ftg_keygen_md->num_tracking_params;

    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;

    if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_IFT) {
        ext_ft_type_flag = BCMI_FT_TYPE_F_IFT;
    } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AIFT) {
        ext_ft_type_flag = BCMI_FT_TYPE_F_AIFT;
    }

    /* Loop through tracking params */
    for (idx = 0; idx < num_tracking_params; idx++) {
        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            is_key = TRUE;
            qual_info_arr = skey_keygen_md->qual_info_arr;
            allocated_quals = &skey_keygen_md->qual_info_count;
            qual_param_idx_arr = &skey_keygen_md->qual_param_idx_arr[0];
        } else {
            is_key = FALSE;
            qual_info_arr = sdata_keygen_md->qual_info_arr;
            allocated_quals = &sdata_keygen_md->qual_info_count;
            qual_param_idx_arr = &sdata_keygen_md->qual_param_idx_arr[0];
        }

        /* Handling tracking param FlowtrackerCheck */
        if (tracking_param_info->param ==
                    bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            rv = bcmi_ft_group_check_validate(unit,
                     BCMI_FT_GROUP_CHECK_MATCH_GROUP,
                     ftg_keygen_md->ft_group_id, tracking_param_info->check_id);
            if ((rv != BCM_E_NONE) || (is_key == TRUE)) {
                rv = BCM_E_PARAM;
                BCMI_IF_ERROR_CLEANUP(rv);
            }
        }

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];

        if ((param > _FIELD_FT_TRACKING_PARAM_MAX) || (pq_map == NULL)) {
            rv = BCM_E_PARAM;
            BCMI_IF_ERROR_CLEANUP(rv);
        }

        /* Skip non IFT/AIFT qualifiers */
        if (!(pq_map->db_ft_type_bmp & ext_ft_type_flag)) {
            continue;
        }

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
            if (is_key == TRUE) {
                rv = _field_ft_group_param_udf_qual_init(unit,
                        ftg_keygen_md->ft_info->flow_key_qual_cfg,
                        skey_keygen_md, param, tracking_param_info->custom_id,
                        &qual, &udf_qual_flags);
            } else {
                rv = _field_ft_group_param_udf_qual_init(unit,
                        ftg_keygen_md->ft_info->flow_data_qual_cfg,
                        sdata_keygen_md, param, tracking_param_info->custom_id,
                        &qual, &udf_qual_flags);
            }
            BCMI_IF_ERROR_CLEANUP(rv);
        } else {
            udf_qual_flags = 0;
            qual = pq_map->qual;
        }

        /* Skip non mapped qualifiers */
        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
            if ((soc_feature(unit, soc_feature_flowtracker_ts_64_sw_war_support)) &&
               ((param == bcmFlowtrackerTrackingParamTypeTimestampNewLearn) ||
                (param == bcmFlowtrackerTrackingParamTypeTimestampFlowStart) ||
                (param == bcmFlowtrackerTrackingParamTypeTimestampFlowEnd) ||
                (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1) ||
                (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2) ||
                (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent3) ||
                (param == bcmFlowtrackerTrackingParamTypeTimestampCheckEvent4)) &&
               ((tracking_param_info->src_ts == bcmFlowtrackerTimestampSourceNTP) ||
                (tracking_param_info->src_ts == bcmFlowtrackerTimestampSourcePTP))) {
                qual = bcmFieldQualifyOpaqueObject2;
            } else {
                continue;
            }
        }

        if (((is_key == TRUE) &&
              BCM_FIELD_QSET_TEST(skey_keygen_md->qset, qual)) ||
            ((is_key == FALSE) &&
              BCM_FIELD_QSET_TEST(sdata_keygen_md->qset, qual))) {
            continue;
        }

        new_idx = *allocated_quals;
        qual_info_arr[new_idx].qual_id = qual;

        _field_ft_keygen_qual_info_flags_set(unit, is_key,
            pq_map->flags, udf_qual_flags, &qual_info_arr[new_idx].flags);

        if (is_key == TRUE) {
            qual_param_idx_arr[new_idx] = idx;
        }

        (*allocated_quals)++;
        if (is_key == TRUE) {
            BCM_FIELD_QSET_ADD(skey_keygen_md->qset, qual);
        } else {
            BCM_FIELD_QSET_ADD(sdata_keygen_md->qset, qual);
        }
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_qual_info_shift
 * Purpose:
 *   Shift qual info in qual_info_arr downward.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   start_idx         - (IN) Start index (lower number)
 *   end_idx           - (IN) End Index (higher number)
 *   qual_info_arr     - (IN/OUT) Qual info array.
 *   qual_size_arr     - (IN/OUT) Qual size array.
 * Returns:
 *   BCM_E_XXX
 */
int
_field_hx5_ftg_keygen_qual_info_shift(int unit,
        int start_idx,
        int end_idx,
        bcmi_keygen_qual_info_t *qual_info_arr,
        uint16 *qual_size_arr,
        uint16 *qual_param_idx_arr)
{
    int idx = 0;
    uint16 p_idx = 0, qual_size = 0;
    bcmi_keygen_qual_info_t temp_qual_info;

    qual_size = qual_size_arr[end_idx];
    p_idx = qual_param_idx_arr[end_idx];
    sal_memcpy(&temp_qual_info, &qual_info_arr[end_idx],
            sizeof(bcmi_keygen_qual_info_t));

    for (idx = end_idx; idx > start_idx; idx--) {
        qual_size_arr[idx] = qual_size_arr[idx - 1];
        qual_param_idx_arr[idx] = qual_param_idx_arr[idx - 1];
        sal_memcpy(&qual_info_arr[idx], &qual_info_arr[idx - 1],
            sizeof(bcmi_keygen_qual_info_t));
    }

    qual_size_arr[start_idx] = qual_size;
    qual_param_idx_arr[start_idx] = p_idx;
    sal_memcpy(&qual_info_arr[start_idx], &temp_qual_info,
            sizeof(bcmi_keygen_qual_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_data_qual_adjust
 * Purpose:
 *   Re-arrange, if required, qual_info_arr for session data extraction.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_data_qual_adjust(int unit,
                            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int rank1_idx = 0;
    uint16 qual_size = 0;
    uint16 qual_info_count = 0;
    uint16 qual_size_arr[20];
    bcmi_keygen_cfg_t keygen_cfg;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    uint16 *qual_param_idx_arr = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    qual_info_arr = ftg_keygen_md->sdata_keygen_md.qual_info_arr;
    qual_info_count = ftg_keygen_md->sdata_keygen_md.qual_info_count;
    qual_param_idx_arr = ftg_keygen_md->sdata_keygen_md.qual_param_idx_arr;
    if ((qual_info_arr == NULL) || (qual_info_count <= 1)) {
        goto cleanup;
    }

    ft_info = ftg_keygen_md->ft_info;
    keygen_cfg.qual_cfg_info_db = ft_info->flow_data_qual_cfg;

    /* Create list of qualifier sizes */
    for (idx = 0; idx < qual_info_count; idx++) {
        rv = bcmi_keygen_qual_cfg_max_size_get(unit,
                &keygen_cfg, qual_info_arr[idx].qual_id, &qual_size);
        BCMI_IF_ERROR_CLEANUP(rv);

        qual_size_arr[idx] = qual_size;
    }

    /* Sort qualifiers based on size and usage. Mask is not considered */
    /* Set rank1_idx after ALU_DATA qualifier. */
    rank1_idx = 1;
    for (idx = rank1_idx; idx < qual_info_count; idx++) {

        if ((qual_size_arr[idx] % 8) != 0) {
            /* If qual_size is not multiple of 8, Rank 1 */

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank1_idx, idx, qual_info_arr, qual_size_arr,
                    qual_param_idx_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank1_idx++;
        }
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_cfg_key_qual_adjust
 * Purpose:
 *   Re-arrange, if required, qual_info_arr for session key extraction.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_key_qual_adjust(int unit,
                            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
/* Highest priority in the qual info arr */
#define _FP_FT_TRACKING_PARAM_USE_TYPE_HIGH     (1 << 0)

/* Use Keygen extractor A */
#define _FP_FT_TRACKING_PARAM_USE_TYPE_A        (1 << 1)

/* Use Keygen extractor A or B */
#define _FP_FT_TRACKING_PARAM_USE_TYPE_A_OR_B   (1 << 2)

    int rv = BCM_E_NONE;
    int idx = 0;
    int rank1_idx = 0;
    int rank2_idx = 0;
    int mask_present = 0;
    int partial_mask = 0;
    uint16 qual_size = 0;
    uint16 qual_info_count = 0;
    uint16 qual_size_arr[20];
    uint16 ext_type_arr[20];
    uint16 *qual_param_idx_arr = NULL;
    bcmi_keygen_cfg_t keygen_cfg;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    bcm_flowtracker_tracking_param_info_t *param_info = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    qual_info_arr = ftg_keygen_md->skey_keygen_md.qual_info_arr;
    qual_info_count = ftg_keygen_md->skey_keygen_md.qual_info_count;
    qual_param_idx_arr = ftg_keygen_md->skey_keygen_md.qual_param_idx_arr;
    if ((qual_info_arr == NULL) || (qual_info_count <= 1)) {
        goto cleanup;
    }

    ft_info = ftg_keygen_md->ft_info;
    keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;

    /* Create list of qualifier sizes */
    for (idx = 0; idx < qual_info_count; idx++) {
        rv = bcmi_keygen_qual_cfg_max_size_get(unit,
                &keygen_cfg, qual_info_arr[idx].qual_id, &qual_size);
        BCM_IF_ERROR_RETURN(rv);

        qual_size_arr[idx] = qual_size;

        if (qual_size > 104) {
            ext_type_arr[idx] = _FP_FT_TRACKING_PARAM_USE_TYPE_HIGH;
            continue;
        }

        mask_present = 0;
        partial_mask = -1;
        param_info = &ftg_keygen_md->tracking_params_info[qual_param_idx_arr[idx]];
        rv = bcmi_esw_ft_group_tracking_param_mask_status_get(unit,
                    param_info, qual_size/8, &mask_present, &partial_mask);
        BCMI_IF_ERROR_CLEANUP(rv);

        if (mask_present == 0) {
            if ((qual_size == 8) || (qual_size == 24)) {
                ext_type_arr[idx] = _FP_FT_TRACKING_PARAM_USE_TYPE_A;
            } else {
                ext_type_arr[idx] = _FP_FT_TRACKING_PARAM_USE_TYPE_A_OR_B;
            }
        } else {
            if (partial_mask != -1) {
                ext_type_arr[idx] = _FP_FT_TRACKING_PARAM_USE_TYPE_A;
            } else {
                ext_type_arr[idx] = _FP_FT_TRACKING_PARAM_USE_TYPE_A_OR_B;
            }
        }
    }

    /* Based on Keygen extractor type, shuffle qual_info_arr */
    /* Note that A_OR_B extractor types are pushed to the end */
    rank1_idx = rank2_idx = 0;
    for (idx = 0; idx < qual_info_count; idx++) {

        if (ext_type_arr[idx] == _FP_FT_TRACKING_PARAM_USE_TYPE_HIGH) {

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank1_idx, idx, qual_info_arr, qual_size_arr,
                    qual_param_idx_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank1_idx++; rank2_idx++;

        } else if (ext_type_arr[idx] == _FP_FT_TRACKING_PARAM_USE_TYPE_A) {

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank2_idx, idx, qual_info_arr, qual_size_arr,
                    qual_param_idx_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank2_idx++;
        }
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_data_oper_extract
 * Purpose:
 *   Call keygen algo to extract session data.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_data_oper_extract(int unit,
                        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int mode_idx = 0;
    int ext_modes_count = 0;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_mode_t ext_modes[2];
    bcmi_keygen_cfg_t keygen_cfg;
    bcmi_keygen_oper_t *keygen_oper = NULL;
    bcmi_keygen_mode_t keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
    bcmi_keygen_mode_t keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;

    ft_info = ftg_keygen_md->ft_info;
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;

    /* Session data extraction modes */
    if ((ftg_keygen_md->ft_type_bmp == BCMI_FT_TYPE_F_MICRO) ||
            (ftg_keygen_md->ft_type_bmp == BCMI_FT_TYPE_F_AIFT)) {
        ext_modes_count = 1;
        ext_modes[0] = BCMI_KEYGEN_MODE_SINGLE;
    }

    for (mode_idx = 0; mode_idx < ext_modes_count; mode_idx++) {

        switch(ext_modes[mode_idx]) {
            case BCMI_KEYGEN_MODE_SINGLE:

                keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
                keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
                sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
                keygen_cfg.qual_info_arr = sdata_keygen_md->qual_info_arr;
                keygen_cfg.qual_info_count = sdata_keygen_md->qual_info_count;
                keygen_cfg.qual_cfg_info_db = ft_info->flow_data_qual_cfg;
                keygen_cfg.ext_cfg_db = ft_info->flow_key_ext_cfg_arr[keygen_ext_mode];
                keygen_cfg.mode = keygen_mode;
                keygen_cfg.num_parts = 2;
                keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC;

                keygen_oper = &sdata_keygen_md->keygen_oper;
                sal_memset(keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
                rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, keygen_oper);
                break;
            default:
                rv = BCM_E_INTERNAL;
        }
        if (BCM_SUCCESS(rv)) {
            break;
        }
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        sdata_keygen_md->keygen_mode = keygen_mode;
        sdata_keygen_md->num_parts = keygen_cfg.num_parts;
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT;
    }

    return rv;
}

/*
 * Function:
 *   _field_ftg_keygen_key_qual_skip
 * Purpose:
 *   Check if it is required to skip first qualifier.
 *   In case there are two qualifiers which ar 128 bits,
 *   rest of qualifiers cannot be masked as all Type A
 *   are exhausted. This could be a problem with IPv6
 *   5-tuple. To prevent it, if 2 such qualifiers are
 *   present if is better to skip one qualifier and
 *   extract it statically at the end of Key.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (IN) FT Group Keygen metadata.
 *   skip_keygen_oper  - (OUT) Keygen operational state for skipped
 *                            qualifier.
 *   skipped           - (OUT) If qualifier is skipped.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 *  In function _field_hx5_ftg_keygen_cfg_key_qual_adjust(), we
 *  sort qualifiers are place 128-bit extractors in the front.
 *  It is pre-requisite to this function to work properly.
 */
STATIC int
_field_ftg_keygen_key_qual_skip(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md,
        bcmi_keygen_oper_t *skip_keygen_oper,
        int *skipped)
{
    int rv = BCM_E_NONE;
    int idx = 0, p_idx = 0;
    int qual_count = 0;
    int mask_present[2], partial_mask[2];
    int type_a_ext_req = 0;
    int type_b_ext_req = 0;
    bcm_field_qualify_t qual;
    uint16 qual_size = 0;
    bcmi_keygen_qual_info_t temp_qual_info;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;
    bcm_flowtracker_tracking_param_info_t *param_info = NULL;

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;

    for (idx = 0; idx < 2; idx++) {
        if (skey_keygen_md->qual_info_count > idx) {
            qual = skey_keygen_md->qual_info_arr[idx].qual_id;

            if ((qual == bcmFieldQualifySrcIp6) ||
                    (qual == bcmFieldQualifyDstIp6) ||
                    (qual == bcmFieldQualifyInnerSrcIp6) ||
                    (qual == bcmFieldQualifyInnerDstIp6)) {
                qual_count++;

                /* Update qual_param_idx_arr when we did shuffle */
                /* Get mask status */
                p_idx = skey_keygen_md->qual_param_idx_arr[idx];
                param_info = &ftg_keygen_md->tracking_params_info[p_idx];
                pq_map = map_db->param_qual_map[param_info->param];
                qual_size = pq_map->width/8;

                mask_present[idx] = 0;
                partial_mask[idx] = -1;
                rv = bcmi_esw_ft_group_tracking_param_mask_status_get(unit,
                        param_info, qual_size, &mask_present[idx],
                        &partial_mask[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }

    if (qual_count < 2) {
        *skipped = FALSE;
        return BCM_E_NONE;
    }

    /* Skipping is required */
    *skipped = TRUE;

    /* Skip qualifier which needs less type A */
    if (partial_mask[1] < partial_mask[0]) {
        p_idx = skey_keygen_md->qual_param_idx_arr[0];
        sal_memcpy(&temp_qual_info, &skey_keygen_md->qual_info_arr[0],
                sizeof(bcmi_keygen_qual_info_t));

        skey_keygen_md->qual_param_idx_arr[0] =
            skey_keygen_md->qual_param_idx_arr[1];
        sal_memcpy(&skey_keygen_md->qual_info_arr[0],
                &skey_keygen_md->qual_info_arr[1],
                sizeof(bcmi_keygen_qual_info_t));

        skey_keygen_md->qual_param_idx_arr[1] = p_idx;
        sal_memcpy(&skey_keygen_md->qual_info_arr[1], &temp_qual_info,
                sizeof(bcmi_keygen_qual_info_t));

        partial_mask[0] = partial_mask[1];
    }

    if (partial_mask[0] != -1) {
        type_a_ext_req = partial_mask[0]/2;
        type_b_ext_req = qual_size/2 - partial_mask[0]/2;
    }

    /* If partial mask is not present or required extractors for
       type B exceeds extraction boundary
     */
    if ((partial_mask[0] == -1) || (type_b_ext_req > 6)) {
        type_a_ext_req = 2;
        type_b_ext_req = 6;
    }

    /* Get required extractors */
    rv = _field_ft_keygen_oper_skip_ext_ctrl_form(unit,
            skey_keygen_md->qual_info_arr[0].qual_id,
            type_a_ext_req, type_b_ext_req, skip_keygen_oper);
    BCM_IF_ERROR_RETURN(rv);

    /* Form qual_offset_info */
    rv = _field_ft_keygen_oper_skip_qual_offset_info_form(unit,
            3, skey_keygen_md->qual_info_arr[0].qual_id,
            type_a_ext_req, type_b_ext_req, skip_keygen_oper);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_ftg_keygen_oper_extract_verify
 * Purpose:
 *   Verify extraction of qualifier in Keygen Type B.
 *   Since type B does not support masking, any qualifier
 *   with size not multiple of 16 will fail.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   keygen_cfg        - (IN) Keygen config.
 *   keygen_oper       - (IN) Keygen oper.
 * Returns:
 *   BCM_E_XXX
 * Notes:
 */
STATIC int
_field_ftg_keygen_oper_extract_verify(int unit,
        bcmi_keygen_cfg_t *keygen_cfg,
        bcmi_keygen_oper_t *keygen_oper)
{
    int i = 0;
    int num_offsets = 0;
    int offset = 0;
    int q_width = 0;
    bcmi_keygen_qual_offset_info_t *qual_offset_info = NULL;

    if (keygen_cfg->num_parts == 3) {
        qual_offset_info = &keygen_oper->qual_offset_info[2];
    } else {
        qual_offset_info = &keygen_oper->qual_offset_info[1];
    }

    for (i = 0; i < qual_offset_info->size; i++) {
        num_offsets = 0;
        for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
            if (!qual_offset_info->offset_arr[i].width[offset]) {
                continue;
            }

            q_width = qual_offset_info->offset_arr[i].width[offset];
            if (q_width != 16) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "Failed to"
                                " Allocate resources for Session"
                                " Key tracking param\n\r")));
                return BCM_E_RESOURCE;
            }

            num_offsets++;
            if (num_offsets ==
                    qual_offset_info->offset_arr[i].num_offsets) {
                break;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_key_oper_extract
 * Purpose:
 *   Call keygen algo to extract session key.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_key_oper_extract(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int part = 0;
    int mode_idx = 0;
    int ext_modes_count = 0;
    int skipped = FALSE;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_cfg_t keygen_cfg;
    bcmi_keygen_oper_t skip_keygen_oper;
    bcmi_keygen_oper_t *keygen_oper = NULL;
    bcmi_keygen_mode_t ext_modes[2] = {0};
    bcmi_keygen_mode_t keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
    bcmi_keygen_mode_t keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;

    ft_info = ftg_keygen_md->ft_info;
    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;

    sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
    sal_memset(&skip_keygen_oper, 0, sizeof (skip_keygen_oper));

    /* Skip if No Qualifier in Key */
    if (skey_keygen_md->qual_info_count == 0) {
        goto cleanup;
    }

    rv = _field_ftg_keygen_key_qual_skip(unit,
            ftg_keygen_md, &skip_keygen_oper, &skipped);
    BCMI_IF_ERROR_CLEANUP(rv);

    if (skipped == TRUE) {
        ext_modes[0] = BCMI_KEYGEN_MODE_DBLINTRA;
        ext_modes_count = 1;
    } else {
        ext_modes[0] = BCMI_KEYGEN_MODE_SINGLE;
        ext_modes[1] = BCMI_KEYGEN_MODE_DBLINTRA;
        ext_modes_count = 2;
    }

    for (mode_idx = 0; mode_idx < ext_modes_count; mode_idx++) {

        switch(ext_modes[mode_idx]) {
            case BCMI_KEYGEN_MODE_SINGLE:

                keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
                keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;

                sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
                keygen_cfg.qual_info_arr = skey_keygen_md->qual_info_arr;
                keygen_cfg.qual_info_count = skey_keygen_md->qual_info_count;
                keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
                keygen_cfg.ext_cfg_db = ft_info->flow_key_ext_cfg_arr[keygen_ext_mode];
                keygen_cfg.mode = keygen_mode;
                keygen_cfg.num_parts = 2;
                keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC;

                keygen_oper = &skey_keygen_md->keygen_oper;
                sal_memset(keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
                rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, keygen_oper);
                break;

            case BCMI_KEYGEN_MODE_DBLINTRA:

                keygen_mode = BCMI_KEYGEN_MODE_DBLINTRA;
                keygen_ext_mode = BCMI_KEYGEN_MODE_DBLINTRA;
                sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));

                if (skipped == TRUE) {
                    /* Remove static qual/extractor from qual_info_arr */
                    keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE;
                    keygen_cfg.qual_info_arr = &skey_keygen_md->qual_info_arr[1];
                    keygen_cfg.qual_info_count = skey_keygen_md->qual_info_count - 1;
                    keygen_cfg.ext_ctrl_sel_info = skip_keygen_oper.ext_ctrl_sel_info;
                    keygen_cfg.ext_ctrl_sel_info_count =
                        skip_keygen_oper.ext_ctrl_sel_info_count;
                } else {
                    keygen_cfg.qual_info_arr = &skey_keygen_md->qual_info_arr[0];
                    keygen_cfg.qual_info_count = skey_keygen_md->qual_info_count;
                }
                keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
                keygen_cfg.ext_cfg_db = ft_info->flow_key_ext_cfg_arr[keygen_ext_mode];
                keygen_cfg.mode = keygen_mode;
                keygen_cfg.num_parts = 3;
                keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC;

                keygen_oper = &skey_keygen_md->keygen_oper;
                sal_memset(keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
                rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, keygen_oper);
                break;

            default:
                rv = BCM_E_INTERNAL;
                BCMI_IF_ERROR_CLEANUP(rv);
        }
        if (BCM_SUCCESS(rv) && (skipped == TRUE)) {
            /* Merge skipped keygen cfg to get final keygen oper */
            rv = _field_ft_keygen_skip_qual_ext_ctrl_merge(unit,
                    keygen_cfg.num_parts, &skip_keygen_oper, keygen_oper);
        }
        if (BCM_SUCCESS(rv)) {
            /* Verify extraction */
            rv = _field_ftg_keygen_oper_extract_verify(unit,
                    &keygen_cfg, keygen_oper);
            if (BCM_SUCCESS(rv)) {
                /* Break if extraction is successful */
                break;
            }
        }
    }

cleanup:

    /* Free up skip_keygen_oper */
    if (skip_keygen_oper.qual_offset_info != NULL) {
        for (part = 0; part < 3; part++) {
            if (skip_keygen_oper.qual_offset_info[part].qid_arr != NULL) {
                sal_free(skip_keygen_oper.qual_offset_info[part].qid_arr);
                skip_keygen_oper.qual_offset_info[part].qid_arr = NULL;
            }
            if (skip_keygen_oper.qual_offset_info[part].offset_arr != NULL) {
                sal_free(skip_keygen_oper.qual_offset_info[part].offset_arr);
                skip_keygen_oper.qual_offset_info[part].offset_arr = NULL;
            }
        }
        sal_free(skip_keygen_oper.qual_offset_info);
    }

    if (skip_keygen_oper.ext_ctrl_sel_info != NULL) {
        sal_free(skip_keygen_oper.ext_ctrl_sel_info);
        skip_keygen_oper.ext_ctrl_sel_info = NULL;
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        skey_keygen_md->keygen_mode = keygen_mode;
        skey_keygen_md->num_parts = keygen_cfg.num_parts;
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS;
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_flowcheck_oper_process
 * Purpose:
 *   Process Session data keygen_oper for flowcheck info.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   qual_ext_offset_info - (IN) Qual-Extractor info
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Notes:
 *   1. Tracking Params which belongs to MFT/EFT stage are not
 *      processed here. Handled separately in new stage process
 *      added.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_flowcheck_oper_process(int unit,
            _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int sd_idx = 0;
    int data_idx = 0;
    int q_idx = 0;
    int width = 0;
    int start = 0;
    int temp_fc_idx = 0;
    int fc_idx = 0;
    int num_operands = 0;
    uint16 ext_ft_type_flag = 0;
    uint16 chk_ft_type_bmp = 0;
    bcmi_ft_alu_key_t *sdata_info_key = NULL;
    bcm_field_qualify_t qual;
    bcm_flowtracker_check_t check_id;
    bcmi_ft_group_alu_info_t *sdata_info = NULL;
    bcm_flowtracker_check_info_t fc_info;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcm_flowtracker_check_action_info_t action_info;
    bcmi_ft_type_t ext_ft_type = bcmiFtTypeNone;

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    sdata_info = ftg_keygen_md->keydata_oper_info.sdata_info;
    data_idx = ftg_keygen_md->keydata_oper_info.num_sdata_info;

    if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_IFT) {
        ext_ft_type = bcmiFtTypeIFT;
        ext_ft_type_flag = BCMI_FT_TYPE_F_IFT;
    } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AIFT) {
        ext_ft_type = bcmiFtTypeAIFT;
        ext_ft_type_flag = BCMI_FT_TYPE_F_AIFT;
    }

    for (idx = 0; idx < ftg_keygen_md->num_flowchecks; idx++) {

        check_id = ftg_keygen_md->flowtracker_checks[idx];
        bcm_flowtracker_check_info_t_init(&fc_info);
        rv = bcmi_ft_check_get(unit, check_id, &fc_info, &chk_ft_type_bmp);
        BCM_IF_ERROR_RETURN(rv);

        /* Skip non IFT checkers and add checkers with none as IFT */
        if (!(chk_ft_type_bmp & ext_ft_type_flag)) {
            continue;
        }

        fc_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

        /* Search if flowcheck is already added in sdata_info */
        for (sd_idx = 0; sd_idx < data_idx; sd_idx++) {
            temp_fc_idx =
               BCMI_FT_FLOWCHECKER_INDEX_GET(sdata_info[sd_idx].flowchecker_id);

            if (temp_fc_idx == fc_idx) {
                break;
            }
        }

        /* If not found, use last and increment data_idx */
        if (sd_idx == data_idx) {
            data_idx++;
        }

        sdata_info[sd_idx].flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(fc_idx, BCM_FT_IDX_TYPE_CHECK);

        if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            sdata_info[sd_idx].element_type2 = fc_info.param;
            sdata_info_key = &sdata_info[sd_idx].key2;
            sdata_info[sd_idx].alu_ft_type = ext_ft_type;
            sdata_info[sd_idx].custom_id2 = fc_info.custom_id;
        } else {
            sdata_info[sd_idx].element_type1 = fc_info.param;
            sdata_info_key = &sdata_info[sd_idx].key1;
            sdata_info[sd_idx].alu_ft_type = ext_ft_type;
            sdata_info[sd_idx].custom_id1 = fc_info.custom_id;
        }

        pq_map = map_db->param_qual_map[fc_info.param];
        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_FILL) {
            continue;
        }

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
            sdata_info_key->location = 0;
            sdata_info_key->length = pq_map->width;
            sdata_info_key->is_alu = 1;
        } else {
            if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
                rv = _field_ft_group_param_udf_qual_get(unit,
                        &ftg_keygen_md->sdata_keygen_md,
                        fc_info.param, fc_info.custom_id, &qual);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                qual = pq_map->qual;
            }
            for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
                if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                    continue;
                }
                start = width = 0;
                rv = _field_hx5_ftg_qual_offset_width_get(unit,
                        &qual_ext_offset_info->offset_arr[q_idx],
                        &start, &width);
                BCM_IF_ERROR_RETURN(rv);

                sdata_info_key->location = start;
                sdata_info_key->length = width;
                sdata_info_key->is_alu = 1;
                break;
            }
            if (q_idx == qual_ext_offset_info->size) {
                BCM_IF_ERROR_RETURN(BCM_E_INTERNAL);
            }
        }

        /* Set action key from primary flowcheck */
        if (!BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            bcm_flowtracker_check_action_info_t_init(&action_info);
            rv = bcm_esw_flowtracker_check_action_info_get(unit,
                                           check_id, &action_info);
            BCM_IF_ERROR_RETURN(rv);

            /* Check if Action is valid */
            if (action_info.action == bcmFlowtrackerCheckActionNone) {
                continue;
            }

            /* Set direction */
            sdata_info[sd_idx].direction = action_info.direction;

            /* If Action does not requires param, continue */
            rv = bcmi_esw_ft_check_action_num_operands_get(unit,
                                action_info.action, &num_operands);
            BCM_IF_ERROR_RETURN(rv);

            if (num_operands == 0) {
                continue;
            }
            pq_map = map_db->param_qual_map[action_info.param];

            /* Skip if non IFT/AIFT stage */
            if (!(pq_map->db_ft_type_bmp & ext_ft_type_flag)) {
                continue;
            }

            if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
                sdata_info[sd_idx].action_element_type = action_info.param;
                sdata_info[sd_idx].action_key.location = pq_map->start;
                sdata_info[sd_idx].action_key.length = pq_map->width;
                sdata_info[sd_idx].action_key.is_alu = 1;
            } else {
                if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
                    rv = _field_ft_group_param_udf_qual_get(unit,
                            &ftg_keygen_md->sdata_keygen_md,
                            action_info.param, action_info.custom_id, &qual);
                    BCM_IF_ERROR_RETURN(rv);
                } else {
                    qual = pq_map->qual;
                }
                for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
                    if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                        continue;
                    }
                    start = width = 0;
                    rv = _field_hx5_ftg_qual_offset_width_get(unit,
                                    &qual_ext_offset_info->offset_arr[q_idx],
                                    &start, &width);
                    BCM_IF_ERROR_RETURN(rv);

                    sdata_info[sd_idx].action_element_type = action_info.param;
                    sdata_info[sd_idx].action_key.location = start;
                    sdata_info[sd_idx].action_key.length = width;
                    sdata_info[sd_idx].action_key.is_alu = 1;
                    sdata_info[sd_idx].action_custom_id = action_info.custom_id;
                    break;
                }
                if (q_idx == qual_ext_offset_info->size) {
                    BCM_IF_ERROR_RETURN(BCM_E_INTERNAL);
                }
            }
        }
    }

    ftg_keygen_md->keydata_oper_info.num_sdata_info = data_idx;

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_tracking_params_oper_process
 * Purpose:
 *   Process Session data keygen_oper for tracking params..
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Notes:
 *   1. Tracking Params which belongs to MFT/EFT stage are not
 *      processed here. Handled separately in new stage process
 *      added.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_tracking_params_oper_process(int unit,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int width = 0;
    int idx = 0;
    int data_idx = 0;
    int q_idx = 0;
    int start_bit = 0;
    uint16 param = 0;
    uint16 ext_ft_type_flag = 0;
    bcm_field_qualify_t qual;
    bcmi_ft_type_t ext_ft_type = bcmiFtTypeNone;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;

    oper_info = &ftg_keygen_md->keydata_oper_info;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;

    if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_IFT) {
        ext_ft_type = bcmiFtTypeIFT;
        ext_ft_type_flag = BCMI_FT_TYPE_F_IFT;
    } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AIFT) {
        ext_ft_type = bcmiFtTypeAIFT;
        ext_ft_type_flag = BCMI_FT_TYPE_F_AIFT;
    }

    data_idx = oper_info->num_sdata_info;
    idx = 0;
    while(idx < ftg_keygen_md->num_tracking_params) {

        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            idx++;
            continue;
        }

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];

        if ((!(pq_map->db_ft_type_bmp & ext_ft_type_flag)) ||
            (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_FILL)) {
            idx++;
            continue;
        }

        if (tracking_param_info->flags &
                    BCM_FLOWTRACKER_TRACKING_PARAM_STICKYOR_UPDATE) {
            oper_info->sdata_info[data_idx].flags |=
                                    BCMI_FT_ALU_LOAD_STICKYOR_UPDATE;
        }

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
            oper_info->sdata_info[data_idx].key1.location = 0;
            oper_info->sdata_info[data_idx].key1.is_alu =
                    ((pq_map->flags & _FP_FT_TRACKING_PARAM_F_ALU_REQ) ? 1 : 0);
            oper_info->sdata_info[data_idx].key1.length = pq_map->width;
            oper_info->sdata_info[data_idx].element_type1 = param;
            oper_info->sdata_info[data_idx].flowchecker_id = 0;
            oper_info->sdata_info[data_idx].alu_ft_type = ext_ft_type;
            oper_info->sdata_info[data_idx].direction =
                                        tracking_param_info->direction;
            data_idx++;
        } else {

            if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
                rv = _field_ft_group_param_udf_qual_get(unit,
                        &ftg_keygen_md->sdata_keygen_md,
                        param, tracking_param_info->custom_id, &qual);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                qual = pq_map->qual;
            }
            for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
                if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                    continue;
                }
                start_bit = width = 0;
                rv = _field_hx5_ftg_qual_offset_width_get(unit,
                            &qual_ext_offset_info->offset_arr[q_idx],
                            &start_bit, &width);
                BCMI_IF_ERROR_CLEANUP(rv);

                oper_info->sdata_info[data_idx].key1.location = start_bit;
                oper_info->sdata_info[data_idx].key1.is_alu = 0;
                oper_info->sdata_info[data_idx].key1.length = width;
                oper_info->sdata_info[data_idx].element_type1 = param;
                oper_info->sdata_info[data_idx].flowchecker_id = 0;
                oper_info->sdata_info[data_idx].alu_ft_type = ext_ft_type;
                oper_info->sdata_info[data_idx].direction =
                                            tracking_param_info->direction;
                oper_info->sdata_info[data_idx].custom_id1 =
                                            tracking_param_info->custom_id;
                data_idx++;
                break;
            }
            if (q_idx == qual_ext_offset_info->size) {
                BCM_IF_ERROR_RETURN(BCM_E_INTERNAL);
            }
        }
        idx++;
    }

    oper_info->num_sdata_info = data_idx;

cleanup:

    return rv;
}

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
/*
 * Function:
 *  _field_hx5_ftg_flowcheck_stage_oper_process
 * Purpose:
 *  Process Session data keygen_oper for flowcheck info.
 * Parameters:
 *  unit           - (IN) BCM device number.
 *  ft_type_in     - (IN) Input Flow Tracker Type.
 *  ftg_keygen_md  - (IN/OUT) FT Group Keygen metadata.
 * Notes:
 *  1. This api is added to process flow checkers for
 *     stage passed as input mostly mft/eft stages.
 *     ift stage is processed separately.
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_flowcheck_stage_oper_process(int unit,
                                    bcmi_ft_type_t ft_type_in,
                                    _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int num_operands = 0;
    bcm_flowtracker_check_t check_id;
    int idx = 0, sd_idx = 0, data_idx = 0, fc_idx = 0, temp_fc_idx = 0;
    bcmi_ft_alu_key_t *sdata_info_key = NULL;
    bcmi_ft_group_alu_info_t *sdata_info = NULL;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;

    /* Allocate if sdata_info is NULL */
    if ((ftg_keygen_md->keydata_oper_info.sdata_info == NULL) &&
            (ftg_keygen_md->sdata_keygen_md.max_qual_info_alloc)) {
        _FP_XGS3_ALLOC(ftg_keygen_md->keydata_oper_info.sdata_info,
                ftg_keygen_md->sdata_keygen_md.max_qual_info_alloc * \
                sizeof(bcmi_ft_group_alu_info_t),
                "session data info");
        if (ftg_keygen_md->keydata_oper_info.sdata_info == NULL) {
            return BCM_E_MEMORY;
        }
    }

    sdata_info = ftg_keygen_md->keydata_oper_info.sdata_info;
    if (sdata_info == NULL) {
        return BCM_E_PARAM;
    }
    data_idx = ftg_keygen_md->keydata_oper_info.num_sdata_info;

    /* add all flowcheckers */
    for (idx = 0; idx < ftg_keygen_md->num_flowchecks; idx++) {

        check_id = ftg_keygen_md->flowtracker_checks[idx];
        sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
        rv = bcmi_ft_flowchecker_get(unit, check_id, &fc_info);
        BCM_IF_ERROR_RETURN(rv);

        if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            pq_map = map_db->param_qual_map[fc_info.check2.param];
        } else {
            pq_map = map_db->param_qual_map[fc_info.check1.param];
        }

        /* Skip if ft type do not match, extract pending */
        if (!(fc_info.check_ft_type_bmp & (1 << ft_type_in))) {
            continue;
        }

        /* Skip if no need to add, nothing to extract */
        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_FILL) {
            continue;
        }

        /* Search if flowcheck is already added in sdata_info */
        fc_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);
        for (sd_idx = 0; sd_idx < data_idx; sd_idx++) {
            if (sdata_info[sd_idx].flowchecker_id) {
                temp_fc_idx =
                    BCMI_FT_FLOWCHECKER_INDEX_GET
                    (sdata_info[sd_idx].flowchecker_id);

                if (temp_fc_idx == fc_idx) {
                    break;
                }
            }
        }

        /* If not found, use last and increment data_idx */
        if (sd_idx == data_idx) {
            data_idx++;
        }

        sdata_info[sd_idx].flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(fc_idx, BCM_FT_IDX_TYPE_CHECK);

        if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            sdata_info[sd_idx].element_type2 = fc_info.check2.param;
            sdata_info_key = &sdata_info[sd_idx].key2;
        } else {
            sdata_info[sd_idx].element_type1 = fc_info.check1.param;
            sdata_info_key = &sdata_info[sd_idx].key1;
        }

        sdata_info_key->location = pq_map->start;
        sdata_info_key->length = pq_map->width;
        sdata_info_key->is_alu = 1;
        sdata_info[sd_idx].alu_ft_type = ft_type_in;

        /* Set action key from primary flowcheck */
        if (!BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            /* Check if Action is valid */
            if (fc_info.action_info.action == bcmFlowtrackerCheckActionNone) {
                continue;
            }

            /* Set direction */
            sdata_info[sd_idx].direction = fc_info.action_info.direction;

            /* If Action does not requires param, continue */
            rv = bcmi_esw_ft_check_action_num_operands_get(unit,
                                fc_info.action_info.action, &num_operands);
            BCM_IF_ERROR_RETURN(rv);

            if (num_operands == 0) {
                continue;
            }
            pq_map = map_db->param_qual_map[fc_info.action_info.param];
            sdata_info[sd_idx].action_element_type = fc_info.action_info.param;
            sdata_info[sd_idx].action_key.location = pq_map->start;
            sdata_info[sd_idx].action_key.length = pq_map->width;
            sdata_info[sd_idx].action_key.is_alu = 1;
        }
    }

    ftg_keygen_md->keydata_oper_info.num_sdata_info = data_idx;

    return rv;
}

/*
 * Function:
 *  _field_hx5_ftg_tracking_params_stage_oper_process
 * Purpose:
 *  Process Session data keygen_oper for tracking params..
 * Parameters:
 *  unit           - (IN) BCM device number.
 *  ft_type_in     - (IN) Input Flow Tracker Type.
 *  ftg_keygen_md  - (IN/OUT) FT Group Keygen metadata.
 * Notes:
 *  1. This api is added to process flow checkers for
 *     stage passed as input mostly mft/eft stages.
 *     ift stage is processed separately.
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_tracking_params_stage_oper_process(int unit,
                                 bcmi_ft_type_t ft_type_in,
                 _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int data_idx = 0;
    uint16 param = 0;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;

    

    oper_info = &ftg_keygen_md->keydata_oper_info;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;

    data_idx = oper_info->num_sdata_info;
    idx = 0;
    while(idx < ftg_keygen_md->num_tracking_params) {

        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];

        /* Skip if param is Key of belongs to different ft type  */
        if ((!(pq_map->db_ft_type_bmp & (1 << ft_type_in))) ||
                (tracking_param_info->flags &
                 BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY)) {
            idx++;
            continue;
        }

        /* Skip if no need to add */
        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_FILL) {
            idx++;
            continue;
        }

        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_STICKYOR_UPDATE) {
            oper_info->sdata_info[data_idx].flags |=
                                BCMI_FT_ALU_LOAD_STICKYOR_UPDATE;
        }

        oper_info->sdata_info[data_idx].key1.location = pq_map->start;
        oper_info->sdata_info[data_idx].key1.is_alu  =
                    ((pq_map->flags & _FP_FT_TRACKING_PARAM_F_ALU_REQ) ? 1 : 0);
        oper_info->sdata_info[data_idx].key1.length = pq_map->width;
        oper_info->sdata_info[data_idx].element_type1 = param;
        oper_info->sdata_info[data_idx].flowchecker_id = 0;
        oper_info->sdata_info[data_idx].alu_ft_type = ft_type_in;
        oper_info->sdata_info[data_idx].direction =
                                            tracking_param_info->direction;
        data_idx++;
        idx++;
    }

    oper_info->num_sdata_info = data_idx;

    return rv;
}
#endif

/*
 * Function:
 *   _field_hx5_ftg_alu_data_oper_process
 * Purpose:
 *   Output of session data keygen is processed for ALU data
 *   and results are saved in extractors codes.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   qual_ext_offset_info - (IN) Qual-Extractor info.
 *   ext_codes         - (OUT) extractor codes.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_alu_data_oper_process(int unit,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        _field_ft_keygen_ext_codes_t *ext_codes,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int bit = 0;
    int q_idx = 0;
    int index = 0;
    int width = 0;
    int location = 0;
    int extract_pos = 0;
    int start = 0;
    int qual_width = 0;
    uint32 type_mask[1] = {0};
    bcm_field_qualify_t qual;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;
    _field_ft_keygen_type_c_ext_t *type_c_extractors = NULL;

    if ((qual_ext_offset_info == NULL) ||
            (ext_codes == NULL) || (ftg_keygen_md == NULL)) {
        return BCM_E_PARAM;
    }

    /* Return if ALU_DATA is not required */
    if (!(ftg_keygen_md->flags & _FIELD_FTG_KEYGEN_F_ALU_DATA_REQ)) {
        return BCM_E_NONE;
    }

    type_c_extractors = &ext_codes->type_c_extractors;

    for (index = 0; index < ftg_keygen_md->num_alu_data; index++) {
        alu_data_info = &ftg_keygen_md->alu_data_info[index];
        qual = alu_data_info->qual;
        extract_pos = alu_data_info->extract_pos;
        width = alu_data_info->width;

        for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
            if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                continue;
            }
            start = qual_width = 0;
            rv = _field_hx5_ftg_qual_offset_width_get(unit,
                    &qual_ext_offset_info->offset_arr[q_idx],
                    &start, &qual_width);
            BCM_IF_ERROR_RETURN(rv);

            if ((start + width) > qual_width) {
                return BCM_E_INTERNAL;
            }
            location = start + alu_data_info->offset;

            for (bit = 0; bit < width; bit++) {
                type_c_extractors->l1_e1_sel[extract_pos + bit] = location + bit;
            }
            SHR_BITSET_RANGE(&type_mask[0], extract_pos, width);
        }
    }

    ext_codes->type_c_mask.mask = type_mask[0];

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_data_oper_process
 * Purpose:
 *   Process Session data keygen_oper.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_data_oper_process(int unit,
                    _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int num_sdata_info = 0;
    _field_ft_keygen_ext_codes_t ext_codes;
    _field_ft_qual_ext_offset_info_t *qual_ext_offset_info = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;

    oper_info = &ftg_keygen_md->keydata_oper_info;
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;

    /* Skip if nothing is extracted */
    if (sdata_keygen_md->keygen_oper.ext_ctrl_sel_info_count == 0) {
        goto cleanup;
    }

    /* Get qualifier/extractors hw info from keygen_oper */
    rv = _field_hx5_ftg_qual_ext_hw_info_get(unit,
                0, ftg_keygen_md, &qual_ext_offset_info, &ext_codes);

    if (BCM_FAILURE(rv) || (qual_ext_offset_info == NULL)) {
        goto cleanup;
    }

    /* Allocate memory for flowtracker group session data db */
    num_sdata_info = sdata_keygen_md->max_qual_info_alloc;
    _FP_XGS3_ALLOC(oper_info->sdata_info,
            num_sdata_info * sizeof(bcmi_ft_group_alu_info_t),
            "session data info");
    if (oper_info->sdata_info == NULL) {
        BCMI_IF_ERROR_CLEANUP(BCM_E_MEMORY);
    }

    /* Prepare Session data from flowchecks */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_flowcheck_oper_process(unit,
                         qual_ext_offset_info, ftg_keygen_md);
    }

    /* Prepare Session data from tracking params */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_tracking_params_oper_process(unit,
                          qual_ext_offset_info, ftg_keygen_md);
    }

    /* Process alu data oper info */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_alu_data_oper_process(unit,
                qual_ext_offset_info, &ext_codes, ftg_keygen_md);
    }

    /* Encode Session Data extractors. */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_field_hx5_ft_extractors_encode(unit, &ext_codes,
                &oper_info->sdata_ext_count, &oper_info->sdata_ext_info);
    }

cleanup:

    if (qual_ext_offset_info != NULL) {
        (void) _field_hx5_ft_qual_ext_offset_info_free(unit,
                &qual_ext_offset_info);
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS;
    }

    return rv;
}

STATIC int
_field_ftg_tracking_param_mask_apply(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md,
        int qual_idx,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        bcm_flowtracker_tracking_param_info_t *tracking_param_info,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int q_offset = 0;
    int q_width = 0;
    int ext_num = 0;
    int num_offsets = 0;
    int offset = 0;
    int prev_width = 0;
    int mask_present = 0;
    uint8 *ptr = NULL;
    uint32 qual_bitmap[4];
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_cfg_t keygen_cfg;
    uint16 qual_size = 0;

    ft_info = ftg_keygen_md->ft_info;
    keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
    rv = bcmi_keygen_qual_cfg_max_size_get(unit,
            &keygen_cfg, qual_ext_offset_info->qid_arr[qual_idx],
            &qual_size);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_esw_ft_group_tracking_param_mask_status_get(unit,
            tracking_param_info, qual_size/8, &mask_present, NULL);
    BCM_IF_ERROR_RETURN(rv);

    if (mask_present == 0) {
        return BCM_E_NONE;
    }

    num_offsets = 0;
    prev_width = 0;
    for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
        if (!qual_ext_offset_info->offset_arr[qual_idx].width[offset]) {
            continue;
        }

        q_offset = qual_ext_offset_info->offset_arr[qual_idx].offset[offset];
        q_width = qual_ext_offset_info->offset_arr[qual_idx].width[offset];
        ext_num = qual_ext_offset_info->sel_offset_arr[qual_idx].ext_num[offset];

        prev_width += q_width;
        ptr = tracking_param_info->mask + (qual_size/8) - (prev_width/8);
        if (q_width == 16) {
            _SHR_UNPACK_U16(ptr, qual_bitmap[0]);
        } else {
            _SHR_UNPACK_U8(ptr, qual_bitmap[0]);
        }

        rv = _bcm_field_ft_keygen_profile_extractors_mask_set(unit,
                    q_offset, q_width, ext_num, NULL,
                    0, qual_bitmap, ext_codes);
        BCM_IF_ERROR_RETURN(rv);

        num_offsets++;
        if (num_offsets ==
                qual_ext_offset_info->offset_arr[qual_idx].num_offsets) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_key_oper_process
 * Purpose:
 *   Process Session data keygen_oper.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Notes:
 *   1. Added code to skip non IFT stage key processing.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_key_oper_process(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int key_idx = 0;
    int qual_idx = 0;
    int start_bit = 0;
    int width = 0;
    int num_skey_info = 0;
    uint16 param  = 0;
    uint16 ext_ft_type_flag = 0;
    bcm_field_qualify_t qual;
    _field_ft_keygen_ext_codes_t ext_codes;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;
    _field_ft_qual_ext_offset_info_t *qual_ext_offset_info = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;

    /* Skip if Key extraction is not required */
    if (!(ftg_keygen_md->flags & _FIELD_FTG_KEYGEN_F_KEY_REQ)) {
        goto cleanup;
    }

    /* Get qualifier/extractors hw info from keygen_oper */
    rv = _field_hx5_ftg_qual_ext_hw_info_get(unit,
                1, ftg_keygen_md, &qual_ext_offset_info, &ext_codes);

    if (BCM_FAILURE(rv) || (qual_ext_offset_info == NULL)) {
        goto cleanup;
    }

    if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_IFT) {
        ext_ft_type_flag = BCMI_FT_TYPE_F_IFT;
    } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AIFT) {
        ext_ft_type_flag = BCMI_FT_TYPE_F_AIFT;
    }

    oper_info = &ftg_keygen_md->keydata_oper_info;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;

    /* Allocate memory for flowtracker group session key db */
    num_skey_info = skey_keygen_md->max_qual_info_alloc;
    _FP_XGS3_ALLOC(oper_info->skey_info,
            num_skey_info * sizeof(bcmi_ft_group_alu_info_t),
            "session key info");
    if (oper_info->skey_info == NULL) {
        BCMI_IF_ERROR_CLEANUP(BCM_E_MEMORY);
    }

    key_idx = 0;
    while(idx < ftg_keygen_md->num_tracking_params) {

        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        /* Skip if tracking param is not Key. */
        if (!(tracking_param_info->flags &
                    BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY)) {
            idx++;
            continue;
        }

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];

        /* Skip if param is valid for extraction ft type */
        if (!(pq_map->db_ft_type_bmp & ext_ft_type_flag)) {
            continue;
        }
        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
            rv = _field_ft_group_param_udf_qual_get(unit,
                    &ftg_keygen_md->skey_keygen_md,
                    param, tracking_param_info->custom_id, &qual);
            BCMI_IF_ERROR_CLEANUP(rv);
        } else {
            qual = pq_map->qual;
        }

        for (qual_idx = 0; qual_idx < qual_ext_offset_info->size; qual_idx++) {
            if (qual_ext_offset_info->qid_arr[qual_idx] != qual) {
                continue;
            }
            start_bit = width = 0;
            rv = _field_hx5_ftg_qual_offset_width_get(unit,
                    &qual_ext_offset_info->offset_arr[qual_idx],
                    &start_bit, &width);
            BCMI_IF_ERROR_CLEANUP(rv);

            /* Apply tracking param mask */
            rv = _field_ftg_tracking_param_mask_apply(unit, ftg_keygen_md,
                    qual_idx,
                    qual_ext_offset_info, tracking_param_info, &ext_codes);
            BCMI_IF_ERROR_CLEANUP(rv);

            oper_info->skey_info[key_idx].key1.location = start_bit;
            oper_info->skey_info[key_idx].key1.is_alu = 0;
            oper_info->skey_info[key_idx].key1.length = width;
            oper_info->skey_info[key_idx].element_type1 = param;
            oper_info->skey_info[key_idx].flowchecker_id = 0;
            oper_info->skey_info[key_idx].custom_id1 =
                                    tracking_param_info->custom_id;
            key_idx++;
            break;
        }
        if (qual_idx == qual_ext_offset_info->size) {
            BCM_IF_ERROR_RETURN(BCM_E_INTERNAL);
        }
        idx++;
    }
    oper_info->num_skey_info = key_idx;
    if (skey_keygen_md->keygen_mode == BCMI_KEYGEN_MODE_SINGLE) {
        oper_info->mode = bcmiFtGroupModeSingle;
    } else {
        oper_info->mode = bcmiFtGroupModeDouble;
    }

    /* Dump Hardware Extraction codes */
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "After"
                    " applying session key param mask...\n\r")));
    (void) _bcm_field_hx5_ft_keygen_hw_extractors_dump(unit, &ext_codes);

    /* Encode Session Key extractors. */
    rv = _bcm_field_hx5_ft_extractors_encode(unit, &ext_codes,
            &oper_info->skey_ext_count, &oper_info->skey_ext_info);

cleanup:

    if (qual_ext_offset_info != NULL) {
        (void) _field_hx5_ft_qual_ext_offset_info_free(unit,
                &qual_ext_offset_info);
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_MFT_DATA_PROCESS;
    }

    return rv;
}

/*
 * Function:
 *  _field_hx5_ftg_keygen_mft_stage_process
 *
 * Purpose:
 *   Process mft stage tracking param & flow checkers.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Notes:
 *    This step is added to process flow checkers &
 *    tracking params belonging to MFT stage alone.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_mft_stage_process(int unit,
                                        _field_ft_group_keygen_md_t
                                        *ftg_keygen_md)
{
    int rv = BCM_E_NONE;

    /* Input Param Check */
    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        bcmi_ft_type_t ext_ft_type = bcmiFtTypeNone;

        if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_MFT) {
            ext_ft_type = bcmiFtTypeMFT;
        } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AMFT) {
            ext_ft_type = bcmiFtTypeAMFT;
        }

        if (ext_ft_type != bcmiFtTypeNone) {
            /* Process Flow Checkers for MFT Stage */
            rv = _field_hx5_ftg_flowcheck_stage_oper_process(unit,
                    ext_ft_type,
                    ftg_keygen_md);
            BCMI_IF_ERROR_CLEANUP(rv);

            /* Process Tracking Params for MFT Stage */
            rv = _field_hx5_ftg_tracking_params_stage_oper_process(unit,
                    ext_ft_type,
                    ftg_keygen_md);
        }
    }

cleanup:

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_EFT_DATA_PROCESS;
    }

#else

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_EFT_DATA_PROCESS;
#endif

    return rv;
}

/*
 * Function:
 *  _field_hx5_ftg_keygen_eft_stage_process
 *
 * Purpose:
 *  Process eft stage tracking param & flow checkers.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Notes:
 *  This step is added to process flow checkers &
 *  tracking params belonging to EFT stage alone.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_eft_stage_process(int unit,
                                     _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;

    /* Input Param Check */
    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        bcmi_ft_type_t ext_ft_type = bcmiFtTypeNone;

        if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_EFT) {
            ext_ft_type = bcmiFtTypeEFT;
        } else if (ftg_keygen_md->ft_type_bmp & BCMI_FT_TYPE_F_AEFT) {
            ext_ft_type = bcmiFtTypeAEFT;
        }

        if (ext_ft_type != bcmiFtTypeNone) {
            /* Process Flow Checkers for EFT Stage */
            rv = _field_hx5_ftg_flowcheck_stage_oper_process(unit,
                    ext_ft_type,
                    ftg_keygen_md);
            BCMI_IF_ERROR_CLEANUP(rv);

            /* Process Tracking Params for EFT Stage */
            rv = _field_hx5_ftg_tracking_params_stage_oper_process(unit,
                    ext_ft_type,
                    ftg_keygen_md);
        }
    }

cleanup:
    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_OPER_SAVE;
    }

#else

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_OPER_SAVE;
#endif

    return rv;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_oper_save
 * Purpose:
 *   Save FT Group Session Key/Data to FT Group DB.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_PARAM       - Invalid inputs.
 *   BCM_E_MEMORY      - Failed to allocate memory.
 *   BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ftg_keygen_oper_save(int unit,
            _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_t ft_group_id;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;

    /* Dump Session Key and Session Data */
    _field_hx5_ftg_keygen_oper_dump(unit, ftg_keygen_md);

    if (ftg_keygen_md->save_op == FALSE) {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    ft_group_id = ftg_keygen_md->ft_group_id;
    oper_info = &ftg_keygen_md->keydata_oper_info;

    rv = bcmi_esw_ft_group_extraction_mode_set(unit,
            1, ft_group_id, oper_info->mode);

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_ft_group_extraction_mode_set(unit,
                0, ft_group_id, oper_info->mode);
    }

    if (BCM_SUCCESS(rv) && (oper_info->skey_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_alu_info_set(unit, ft_group_id,
                1, oper_info->num_skey_info, oper_info->skey_info);
    }

    if (BCM_SUCCESS(rv) && (oper_info->skey_ext_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_hw_info_set(unit, ft_group_id,
                1, oper_info->skey_ext_count, oper_info->skey_ext_info);
    }

    if (BCM_SUCCESS(rv) && (oper_info->sdata_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_alu_info_set(unit, ft_group_id,
                0, oper_info->num_sdata_info, oper_info->sdata_info);
    }

    if (BCM_SUCCESS(rv) && (oper_info->sdata_ext_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_hw_info_set(unit, ft_group_id,
                0, oper_info->sdata_ext_count, oper_info->sdata_ext_info);
    }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if ((soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) &&
        (BCM_SUCCESS(rv)) &&
        (soc_feature(unit, soc_feature_flowtracker_ts_64_sw_war_support)) &&
        (oper_info->sdata_ext_info != NULL)) {
        rv = _field_ftv2_ftg_vfp_opaqueid2_get(unit, ft_group_id, ftg_keygen_md);
    }
#endif

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;

    return rv;
}

/*
 * Function:
 *   _field_ftg_keygen_udf_qual_deinit
 * Purpose:
 *   Clear up udf data qualifiers from qual cfg info db
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   db                - (IN) Qual cfg info db
 * Returns:
 *   BCM_E_PARAM       - Invalid inputs.
 *   BCM_E_MEMORY      - Failed to allocate memory.
 *   BCM_E_NONE        - Success.
 */
STATIC int
_field_ftg_keygen_udf_qual_deinit(int unit,
        bcmi_keygen_qual_cfg_info_db_t *db)
{
    int ii = 0;
    int qual_id;

    uint16 udf_data_quals[] = {
        _bcmFieldQualifyData0,  _bcmFieldQualifyData1,
        _bcmFieldQualifyData2,  _bcmFieldQualifyData3,
        _bcmFieldQualifyData4,  _bcmFieldQualifyData5,
        _bcmFieldQualifyData6,  _bcmFieldQualifyData7,
        _bcmFieldQualifyData8,  _bcmFieldQualifyData9,
        _bcmFieldQualifyData10, _bcmFieldQualifyData11,
        _bcmFieldQualifyData12, _bcmFieldQualifyData13,
        _bcmFieldQualifyData14, _bcmFieldQualifyData15
    };

    if (db == NULL) {
        return BCM_E_NONE;
    }

    for (ii = 0; ii < COUNTOF(udf_data_quals); ii++) {
        qual_id = udf_data_quals[ii];
        if (db->qual_cfg_info[qual_id]) {
            BCMI_KEYGEN_FREE(db->qual_cfg_info[qual_id]->qual_cfg_arr);
            BCMI_KEYGEN_FREE(db->qual_cfg_info[qual_id]);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_end
 * Purpose:
 *   Deallocate memories in FT Group Keygen metadata.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_PARAM       - Invalid inputs.
 *   BCM_E_MEMORY      - Failed to allocate memory.
 *   BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ftg_keygen_end(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    if (ftg_keygen_md->tracking_params_info != NULL) {
        sal_free(ftg_keygen_md->tracking_params_info);
        ftg_keygen_md->tracking_params_info = NULL;
    }

    if (ftg_keygen_md->flowtracker_checks != NULL) {
        sal_free(ftg_keygen_md->flowtracker_checks);
        ftg_keygen_md->flowtracker_checks = NULL;
    }

    if (ftg_keygen_md->skey_keygen_md.qual_info_arr != NULL) {
        sal_free(ftg_keygen_md->skey_keygen_md.qual_info_arr);
        ftg_keygen_md->skey_keygen_md.qual_info_arr = NULL;
    }

    if (ftg_keygen_md->sdata_keygen_md.qual_info_arr != NULL) {
        sal_free(ftg_keygen_md->sdata_keygen_md.qual_info_arr);
        ftg_keygen_md->sdata_keygen_md.qual_info_arr = NULL;
    }

    (void) bcmi_keygen_oper_free(unit,
            ftg_keygen_md->skey_keygen_md.num_parts,
            &ftg_keygen_md->skey_keygen_md.keygen_oper);

    (void) bcmi_keygen_oper_free(unit,
            ftg_keygen_md->sdata_keygen_md.num_parts,
            &ftg_keygen_md->sdata_keygen_md.keygen_oper);

    if (ftg_keygen_md->keydata_oper_info.sdata_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.sdata_info);
        ftg_keygen_md->keydata_oper_info.sdata_info = NULL;
    }

    if (ftg_keygen_md->keydata_oper_info.skey_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.skey_info);
        ftg_keygen_md->keydata_oper_info.skey_info = NULL;
    }

    if (ftg_keygen_md->keydata_oper_info.skey_ext_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.skey_ext_info);
        ftg_keygen_md->keydata_oper_info.skey_ext_info = NULL;
    }

    if (ftg_keygen_md->keydata_oper_info.sdata_ext_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.sdata_ext_info);
        ftg_keygen_md->keydata_oper_info.sdata_ext_info = NULL;
    }

    /* Clear up udf data qualifiers */
    if (ftg_keygen_md->ft_info->flow_key_qual_cfg != NULL) {
        _field_ftg_keygen_udf_qual_deinit(unit,
                ftg_keygen_md->ft_info->flow_key_qual_cfg);
    }

    if (ftg_keygen_md->ft_info->flow_data_qual_cfg != NULL) {
        _field_ftg_keygen_udf_qual_deinit(unit,
                ftg_keygen_md->ft_info->flow_data_qual_cfg);
    }

    return (ftg_keygen_md->state_rv);
}

/*
 * Function:
 *   _field_hx5_ftg_keygen_process
 * Purpose:
 *   Gernerate session key, session data and alu data extractions
 *   corresponding to FT Group tracking params and flowchecks.
 * Parameters:
 *   unit              - (IN) BCM device number.
 *   ftg_keygen_md     - (IN) FT Group Keygen metadata.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_process(int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;

    do {
        switch(ftg_keygen_md->state) {
            case _FIELD_FT_GROUP_KEYGEN_START:
                rv = _field_hx5_ftg_keygen_initialize(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_START ret=%d\r\n"),
                                      ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_ALLOC:
                rv = _field_hx5_ftg_keygen_cfg_alloc(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_CFG_ALLOC ret=%d\r\n"),
                                          ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD:
                rv = _field_hx5_ftg_keygen_cfg_alu_qual_add(unit,
                                                            ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD ret=%d\r\n"),
                                                 ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD:
                rv = _field_hx5_ftg_keygen_cfg_flowcheck_qual_add(unit,
                                                                 ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD ret=%d\r\n"),
                                                ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD:
                rv = _field_hx5_ftg_keygen_cfg_data_key_qual_add(unit,
                                                                 ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD ret=%d\r\n"),
                                                      ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST:
                rv = _field_hx5_ftg_keygen_cfg_data_qual_adjust(unit,
                                                                ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "Completed state->"
                      "_FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST ret=%d\r\n"),
                                                      ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST:
                rv = _field_hx5_ftg_keygen_cfg_key_qual_adjust(unit,
                                                               ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST ret=%d\r\n"),
                                                    ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT:
                rv = _field_hx5_ftg_keygen_data_oper_extract(unit,
                                                             ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT ret=%d\r\n"),
                                                  ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT:
                rv = _field_hx5_ftg_keygen_key_oper_extract(unit,
                                                            ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT ret=%d\r\n"),
                                                  ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS:
                rv = _field_hx5_ftg_keygen_data_oper_process(unit,
                                                             ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS ret=%d\r\n"),
                                                   ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS:
                rv = _field_hx5_ftg_keygen_key_oper_process(unit,
                                                            ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS ret=%d\r\n"),
                                                  ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_MFT_DATA_PROCESS:
                rv = _field_hx5_ftg_keygen_mft_stage_process(unit,
                                                             ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_MFT_DATA_PROCESS ret=%d\r\n"),
                                                  ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_EFT_DATA_PROCESS:
                rv = _field_hx5_ftg_keygen_eft_stage_process(unit,
                                                             ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_EFT_DATA_PROCESS ret=%d\r\n"),
                                                  ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_OPER_SAVE:
                rv = _field_hx5_ftg_keygen_oper_save(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_OPER_SAVE ret=%d\r\n"),
                                           ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_END:
                rv = _field_hx5_ftg_keygen_end(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "Completed state->"
                     "_FIELD_FT_GROUP_KEYGEN_END ret=%d\r\n"),
                                     ftg_keygen_md->state_rv));
                return rv;
            default:
                /* Not a valid case */
                return rv;
        }
    } while(1);

    return rv;
}


/*
 * Function:
 *   _bcm_field_ft_entry_traverse
 * Purpose:
 *   Traverse fp entries in flowtracker stage and
 *   call callback to process the entry.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracekr group.
 *   cb_f       - (IN) Callback function pointer
 *   user_data1 - (IN) user data 1
 *   user_data2 - (IN) user data 2
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_bcm_field_ftfp_entry_traverse(int unit,
                               bcm_flowtracker_group_t id,
                               _bcm_field_ft_entry_traverse_cb cb_f,
                               void *user_data1,
                               void *user_data2)
{
    int rv = BCM_E_NONE;
    int eidx = 0;
    int entries = 0;
    _field_group_t *fg = NULL;
    _field_control_t *fc = NULL;
    bcm_field_entry_t *entry_ids = NULL;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Iterate FTFP Groups */
    for (fg = fc->groups; fg != NULL; fg = fg->next) {
        if ((fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) &&
            (fg->stage_id != _BCM_FIELD_STAGE_AMFTFP)      &&
            (fg->stage_id != _BCM_FIELD_STAGE_AEFTFP)) {
            continue;
        }

        rv = bcm_esw_field_entry_multi_get(unit, fg->gid, 0, NULL, &entries);

        if (BCM_SUCCESS(rv)) {
            entry_ids = sal_alloc(entries * sizeof (bcm_field_entry_t),
                    "Entry ID array");

            if (entry_ids == NULL) {
                rv = BCM_E_MEMORY;
                break;
            } else {
                rv = bcm_esw_field_entry_multi_get(unit,
                        fg->gid, entries, entry_ids, &entries);

                if (BCM_SUCCESS(rv)) {
                    /* Iterate FTFP Entries */
                    for (eidx = 0; eidx < entries; eidx++) {
                        /* Call callback */
                        rv = cb_f(unit, entry_ids[eidx],
                                id, user_data1, user_data2);
                        if (BCM_FAILURE(rv)) {
                            break;
                        }
                    }
                }
                sal_free(entry_ids);
                entry_ids = NULL;
            }
        }
    }

    return (rv);
}

/*
 * Function:
 *   _bcm_field_ft_qual_config_get
 * Purpose:
 *   Get Qualifier config info from Qual Cfg Info Db.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   qual_cfg_info_db - (IN) Qualifier cfg info db
 *   qual       - (IN) Qualifier
 *   offset     - (OUT) Offset of Qualifier in L0 Bus
 *   Width      - (OUT) Width of Qualifier in L0 Bus
 * Returns:
 *   BCM_E_INTERNAL    - Incoreect Input
 *   BCM_E_NONE        - Success.
 * Notes:
 *    This function is only valid if all offets of qualifier
 *    are consecutive and only one instance.
 */
STATIC int
_bcm_field_ft_qual_config_get(int unit,
                              bcmi_keygen_qual_cfg_info_db_t *db,
                              bcm_field_qualify_t qual,
                              uint32 *offset,
                              uint32 *width)
{
    uint8 num_chunks;
    uint8 chunk = 0;
    uint16 temp_width = 0;
    uint16 temp_offset = 0;
    bcmi_keygen_qual_offset_cfg_t *e_params = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    if (db == NULL) {
        return BCM_E_INTERNAL;
    }

    qual_cfg_info = db->qual_cfg_info[qual];
    qual_cfg_arr = qual_cfg_info->qual_cfg_arr;

    if (qual_cfg_arr == NULL) {
        return BCM_E_INTERNAL;
    }

    num_chunks = qual_cfg_arr->num_chunks;

    for (chunk = 0; chunk < num_chunks; chunk++) {
        e_params = &qual_cfg_arr->e_params[chunk];

        if (chunk == 0) {
            temp_offset = e_params->offset;
        }

        temp_width += e_params->width;
    }

    *offset = temp_offset;
    *width = temp_width;

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ft_normalize_format_set
 * Purpose:
 *   Set normalization format for given qualifier-pair.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   index      - (IN) Index in norm_qualifiers.
 *   norm_qualifier - (IN) Normalization qualifiers list.
 *   max_norm_commands - (IN) Maximum norm commands that can be
 *                      configured in hw memory.
 *   norm_cmds_buf - (IN/OUT) Buffer for norm commands format.
 *   valid_commands - (IN/OUT) valid commands in norm_cmds_bufs.
 * Returns:
 *   BCM_E_XXX
 */
int
_field_hx5_ft_normalize_format_set(int unit,
        bcm_flowtracker_tracking_param_info_t **norm_tracking_params_info,
        int max_norm_commands,
        uint32 *norm_cmds_buf,
        int *valid_commands)
{
    uint32 comm_idx = 0;
    uint32 src_offset = 0;
    uint32 dst_offset = 0;
    uint32 src_width = 0;
    uint32 dst_width = 0;
    bcm_field_qualify_t src_qual;
    bcm_field_qualify_t dst_qual;
    _field_stage_t *stage_fc = NULL;
    bcm_flowtracker_tracking_param_type_t param;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    if (*valid_commands >= max_norm_commands) {
      LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "All Keys in tracking params"
                            " of flowtracker group cannot be"
                            " normalized\n\r")));
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    map_db = stage_fc->ft_info->param_qual_map_db;
    qual_cfg_info_db = stage_fc->ft_info->flow_key_qual_cfg;

    /* Get Field Qualifiers */
    param = norm_tracking_params_info[0]->param;
    src_qual = map_db->param_qual_map[param]->qual;
    param = norm_tracking_params_info[1]->param;
    dst_qual = map_db->param_qual_map[param]->qual;

    BCM_IF_ERROR_RETURN(_bcm_field_ft_qual_config_get(unit,
                qual_cfg_info_db, src_qual, &src_offset, &src_width));
    BCM_IF_ERROR_RETURN(_bcm_field_ft_qual_config_get(unit,
                qual_cfg_info_db, dst_qual, &dst_offset, &dst_width));

    /* Validate alignemnt */
    if (((src_offset % 16) != 0) || ((dst_offset % 16) != 0) ||
            ((src_width % 16) != 0) || (dst_width != src_width)  ||
            (src_width == 0) || (dst_width == 0)) {
        return BCM_E_CONFIG;
    }
    src_offset /= 16;
    dst_offset /= 16;
    /* Formula for size is (n + 1)*2B */
    src_width = (src_width - 1) / 16;

    comm_idx = *valid_commands;

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_ENABLEf, 1);

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_SRC_START_OFFSETf,
            src_offset);

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_DST_START_OFFSETf,
            dst_offset);

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_SIZEf, src_width);

    *valid_commands = comm_idx + 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *   _field_hx5_ft_normalize_controls_set
 * Purpose:
 *   Set normalize controls of FTFP policy table.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id
 *   policy_mem - (IN) Policy Memory
 *   enable     - (IN) Normalization Enable/Disable
 *   data_buf   - (IN/OUT) Policy memory buffer
 * Returns:
 *   BCM_E_XXX
 */
int
_field_hx5_ft_normalize_controls_set(int unit,
                    bcm_flowtracker_group_t id,
                    soc_mem_t policy_mem,
                    uint32 enable,
                    uint32 *data_buf)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int valid_commands = 0;
    int num_tracking_params = 0;
    uint32 norm_cmds_buf[4] = {0};
    bcm_flowtracker_tracking_param_info_t *tracking_params_info = NULL;
    bcm_flowtracker_tracking_param_info_t
        *norm_tracking_params_info[BCMI_FT_GROUP_NORM_TRACKING_PARAMS] = {NULL};

    /* Normalize Commands */
    soc_field_t normalize_cmds_f[] = {
        NORMALIZE_0_COMMANDf, NORMALIZE_1_COMMANDf,
        NORMALIZE_2_COMMANDf, NORMALIZE_3_COMMANDf
    };

    /* If enable=FALSE, reset Normalize commands */
    if (enable == 0) {
        for (idx = 0; idx < COUNTOF(normalize_cmds_f); idx++) {
            if (SOC_MEM_FIELD_VALID(unit, policy_mem, normalize_cmds_f[idx])) {
                soc_mem_field_set(unit, policy_mem, (void *) data_buf,
                        normalize_cmds_f[idx], &norm_cmds_buf[idx]);
            }
        }
        goto cleanup;
    }

    /* Get number of tracking params from group template */
    rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
            id, 0, NULL, &num_tracking_params);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Allocate memory for tracking params info */
    _FP_XGS3_ALLOC(tracking_params_info, (num_tracking_params * \
            sizeof(bcm_flowtracker_tracking_param_info_t)),
            "tracking params info");
    if (tracking_params_info == NULL) {
        rv = BCM_E_MEMORY;
        BCMI_IF_ERROR_CLEANUP(rv);
    }

    /* Read tracking params info */
    rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
            id, num_tracking_params, &tracking_params_info[0],
            &num_tracking_params);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Get tracking params for normalization */
    rv = bcmi_ft_group_norm_tracking_params_info_get(unit,
            id, &norm_tracking_params_info[0]);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Normalize if both param in the pair are present */
    for (idx = 0; idx < BCMI_FT_GROUP_NORM_TRACKING_PARAMS; idx += 2) {
        if ((norm_tracking_params_info[idx] != NULL) &&
                (norm_tracking_params_info[idx + 1] != NULL)) {
            rv = _field_hx5_ft_normalize_format_set(unit,
                    &norm_tracking_params_info[idx], COUNTOF(normalize_cmds_f),
                    norm_cmds_buf, &valid_commands);
            BCMI_IF_ERROR_CLEANUP(rv);
        }
    }

    /* Set to policy table */
    for (idx = 0; idx < valid_commands; idx++) {
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, normalize_cmds_f[idx])) {
            soc_mem_field_set(unit, policy_mem, (void *) data_buf,
                    normalize_cmds_f[idx], &norm_cmds_buf[idx]);
        }
    }

cleanup:

    if (tracking_params_info != NULL) {
        sal_free(tracking_params_info);
    }

    return rv;
}

/*
 * Function:
 *   _field_ftfp_entry_control_set
 * Purpose:
 *   Apply flowtracker group control to given entry if associated
 *   action is given flowtracker group.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   entry      - (IN) Field entry.
 *   id         - (IN) Flowtracker group Id
 *   user_data1 - (IN) user data 1
 *   user_data2 - (IN) user data 2
 * Returns:
 *   BCM_E_PARAM       - If input parameters are not valid
 *   BCM_E_MEMORY      - If Failed to allocate required memory
 *   BCM_E_NONE        - Success.
 */
STATIC int
_field_ftfp_entry_control_set(int unit,
            bcm_field_entry_t entry,
            bcm_flowtracker_group_t id,
            void *user_data1,
            void *user_data2)
{
    int rv = BCM_E_NONE;
    int tcam_idx = 0;
    int param = 0;
    uint32 val = 0, val2 = 0;
    uint32 ft_group_id_set = 0;
    uint32 action_ft_set = 0;
    uint32 ent_action_ft = 0;
    uint32 action_new_learn_set = 0;
    uint32 ent_action_new_learn = 0;
    uint32 cur_param = 0;
    _field_action_t *fa = NULL;
    _field_entry_t *f_ent = NULL;
    soc_field_t field = INVALIDf;
    soc_field_t field2 = INVALIDf;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t policy_mem = INVALIDm;
    bcm_flowtracker_group_control_type_t control_type;
    bcm_flowtracker_group_t ent_flow_group_id = -1;
    uint32 data_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if ((user_data1 == NULL) || (user_data2 == NULL)) {
        return BCM_E_PARAM;
    }

    control_type = *((bcm_flowtracker_group_control_type_t *) user_data1);
    param = *((int *)user_data2);

    FP_LOCK(unit);

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return (rv);
    }

    /* Loop through all actions */
    for (fa = f_ent->actions;
            (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
            fa = fa->next) {
        switch(fa->action) {
            case bcmFieldActionFlowtrackerGroupId:
                if (!(fa->flags & _FP_ACTION_DIRTY)) {
                    ft_group_id_set = TRUE;
                    ent_flow_group_id = fa->param[0];
                }
                break;
            case bcmFieldActionFlowtrackerEnable:
                if (!(fa->flags & _FP_ACTION_DIRTY)) {
                    action_ft_set = TRUE;
                    ent_action_ft = fa->param[0];
                }
                break;
            case bcmFieldActionFlowtrackerNewLearnEnable:
                if (!(fa->flags & _FP_ACTION_DIRTY)) {
                    action_new_learn_set = 1;
                    ent_action_new_learn = fa->param[0];
                }
                break;
            default:
                break;
        }
    }

    if ((ft_group_id_set == FALSE) || (ent_flow_group_id != id)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Get Policy Index */
    rv = _bcm_field_entry_tcam_idx_get(unit, f_ent, &tcam_idx);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Get Policy Mem */
    rv = _bcm_field_th_tcam_policy_mem_get(unit,
            f_ent, &tcam_mem, &policy_mem);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
            tcam_idx, data_buf);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Supported ft_group Controls are DO_NOT_FT and LEARN_DISABLE */
    switch(control_type) {
        case bcmFlowtrackerGroupControlFlowtrackerEnable:
            field = DO_NOT_FTf;
            /* if Field Action is present, it takes priority. */
            if (action_ft_set == TRUE) {
                val = (ent_action_ft) ? 0 : 1;
            } else {
                val = (param) ? 0 : 1;
            }
            break;
        case bcmFlowtrackerGroupControlNewLearnEnable:
            field = LEARN_DISABLEf;
            /* if Field Action is present, it takes priority. */
            if (action_new_learn_set == TRUE) {
                val = (ent_action_new_learn) ? 0 : 1;
            } else {
                val = (param) ? 0 : 1;
            }
            break;
        case bcmFlowtrackerGroupControlFlowDirection:
            field = BIDIRECTIONAL_FLOWf;
            val= ((param == bcmFlowtrackerFlowDirectionBidirectional) ? 1 : 0);

            field2 = KMAP_CONTROL_1_2_PROFILE_IDXf;
            val2 = 0;

            rv = _field_hx5_ft_normalize_controls_set(unit,
                    id, policy_mem, val, data_buf);
           break;
        case bcmFlowtrackerGroupControlUdpFlowDirection:
           field = BIDIRECTIONAL_FLOWf;
            val= ((param == bcmFlowtrackerFlowDirectionBidirectional) ? 1 : 0);

            field2 = KMAP_CONTROL_1_2_PROFILE_IDXf;
            val2 = 1;

            rv = _field_hx5_ft_normalize_controls_set(unit,
                    id, policy_mem, val, data_buf);
            break;
        default:
            rv = BCM_E_PARAM;
            break;
    }

    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    if (SOC_MEM_FIELD_VALID(unit, policy_mem, field)) {
        cur_param = soc_mem_field32_get(unit,
                policy_mem, data_buf, field);

        if (cur_param != val) {
            soc_mem_field32_set(unit,
                    policy_mem, data_buf, field, val);
        }
    }
    if (SOC_MEM_FIELD_VALID(unit, policy_mem, field2)) {
        cur_param = soc_mem_field32_get(unit,
                policy_mem, data_buf, field2);

        if (cur_param != val2) {
            soc_mem_field32_set(unit,
                    policy_mem, data_buf, field2, val2);
        }
    }

    rv = soc_mem_write(unit, policy_mem,
            MEM_BLOCK_ALL, tcam_idx, data_buf);

    FP_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *   _field_ftfp_entry_action_stale_set
 * Purpose:
 *   Mark flowtracker group id action in ftfp entries as
 *   staled out.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   entry      - (IN) Field entry.
 *   id         - (IN) Flowtracker group Id
 *   user_data1 - (IN) user data 1
 *   user_data2 - (IN) user data 2
 * Returns:
 *   BCM_E_PARAM       - If input parameters are not valid
 *   BCM_E_NONE        - Success.
 *   BCM_E_XXX
 */
STATIC int
_field_ftfp_entry_action_stale_set(int unit,
                    bcm_field_entry_t entry,
                    bcm_flowtracker_group_t id,
                    void *user_data1,
                    void *user_data2)
{
    int rv = BCM_E_NONE;
    _field_entry_t *f_ent = NULL;

    FP_LOCK(unit);

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return (rv);
    }

    rv = _bcm_field_hx5_ft_actions_hw_free(unit, f_ent,
            _FP_ACTION_HW_FREE | _BCM_FIELD_ACTION_REF_STALE);

    FP_UNLOCK(unit);

    return rv;
}

/* Public Interfaces */

/*
 * Function:
 *   _bcm_field_ft_alu_data_config_get
 * Purpose:
 *   Get ALU Data config info
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) flowtracker group id.
 *   alu_data   - (IN) Alu Data param
 *   offset     - (OUT) Offset in ALU Data vector.
 *   width      - (OUT) Number of bits in ALU Data vector.
 * Returns:
 *   BCM_E_NOT_FOUND - If given alu_data is not found.
 *   BCM_E_NONE
 */
int
_bcm_field_ft_alu_data_config_get(int unit,
                    bcm_flowtracker_group_t id,
                    _bcm_field_ft_alu_data_t alu_data,
                    int *offset,
                    int *width)
{
    int rv = BCM_E_NONE;
    int index = 0;
    int num_alu_data = 0;
    uint8 found = FALSE;
    uint16 param = 0;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;

    alu_data_info = &_field_ft_alu_data_info_tcp_control[0];
    num_alu_data = COUNTOF(_field_ft_alu_data_info_tcp_control);

    for (index = 0; index < num_alu_data; index++) {
        param = alu_data_info[index].param;
        if (param == alu_data) {
            found = TRUE;
            if (offset != NULL) {
                *offset = alu_data_info[index].extract_pos;
            }

            if (width != NULL) {
                *width = alu_data_info[index].width;
            }
        }
    }

    if (found == FALSE) {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *   _bcm_field_ft_group_tracking_info_process
 * Purpose:
 *   Process FT group tracking information and update FT group
 *   database, if required
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) flowtracker group id.
 *   save_output- (IN) TRUE if output is required to be saved to
 *                 FT Group DB.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_group_tracking_info_process(int unit,
                                bcm_flowtracker_group_t id,
                                uint8 save_output)
{
    _field_ft_group_keygen_md_t ftg_keygen_md;

    sal_memset(&ftg_keygen_md, 0, sizeof(_field_ft_group_keygen_md_t));

    ftg_keygen_md.state_rv = BCM_E_NONE;
    ftg_keygen_md.ft_group_id = id;
    ftg_keygen_md.state = _FIELD_FT_GROUP_KEYGEN_START;
    ftg_keygen_md.prev_state = _FIELD_FT_GROUP_KEYGEN_START;
    ftg_keygen_md.save_op = save_output;

    return _field_hx5_ftg_keygen_process(unit, &ftg_keygen_md);
}

/*
 * Function:
 *   _bcm_field_ft_group_extraction_clear
 * Purpose:
 *   Cleanup FT group tracking params extraction information
 *   from FT group database
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) flowtracker group id.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_group_extraction_clear(int unit,
                            bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;

    /* Clear session key params extraction info */
    rv = bcmi_esw_ft_group_extraction_alu_info_set(unit,
                id, 1, 0, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear session key hw extractors info */
    rv = bcmi_esw_ft_group_extraction_hw_info_set(unit,
                id, 1, 0, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear session data params extraction info */
    rv = bcmi_esw_ft_group_extraction_alu_info_set(unit,
                id, 0, 0, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear session data hw extractors info */
    rv = bcmi_esw_ft_group_extraction_hw_info_set(unit,
                id, 0, 0, NULL);

    return rv;
}

/*
 * Function:
 *   _bcm_field_ft_group_control_set
 * Purpose:
 *   Apply flowtracker group controls to each entry in which
 *   action is given flowtracker group..
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker group
 *   control_type- (IN) control to apply
 *   param      - (IN) Parameter associated with control
 * Returns:
 *   BCM_E_PARAM       - Null field stage control structure.
 *   BCM_E_NONE        - Success.
 */
int
_bcm_field_ft_group_control_set(int unit,
            bcm_flowtracker_group_t id,
            bcm_flowtracker_group_control_type_t control_type,
            int param)
{
    return _bcm_field_ftfp_entry_traverse(unit, id,
            _field_ftfp_entry_control_set,
            (void *) &control_type, (void *) &param);
}

/*
 * Function:
 *   _bcm_field_ft_group_invalidate
 * Purpose:
 *   Mark flowtracker group Id action in ftfp entries
 *   stale.
 * Parameters:
 *   unit       - (IN) BCM device number.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_group_invalidate(int unit)
{
    return _bcm_field_ftfp_entry_traverse(unit, -1,
            _field_ftfp_entry_action_stale_set,
            NULL, NULL);
}

/*
 * Function:
 *   _bcm_field_ft_tracking_param_range_get
 * Purpose:
 *   Returns minimum and maximum values of tracking params.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   param      - (IN) Tracking param.
 *   min_value  - (OUT) Minimum value.
 *   max_value   - (OUT) Maximum value.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_tracking_param_range_get(int unit,
                                  bcm_flowtracker_tracking_param_type_t param,
                                  uint16 custom_id,
                                  int *num_bits,
                                  uint32 *min_value,
                                  uint32 *max_value)
{
    int rv = BCM_E_NONE;
    uint16 max_bits = 0;
    bcmi_keygen_cfg_t keygen_cfg;
    _field_stage_t *stage_fc = NULL;
    _field_ft_info_t *ft_info = NULL;
    bcm_flowtracker_udf_info_t param_udf_info;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    if (param >= _FIELD_FT_TRACKING_PARAM_MAX) {
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit,
            _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    ft_info = stage_fc->ft_info;
    map_db = ft_info->param_qual_map_db;
    pq_map = map_db->param_qual_map[param];
    if (pq_map == NULL) {
        return BCM_E_PARAM;
    }

    /* Get number of bits required to represent qualifier. */
    if (!(pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED)) {

        if ((pq_map->db_ft_type_bmp & (BCMI_FT_TYPE_F_IFT |
                        BCMI_FT_TYPE_F_AIFT)) &&
                (param != bcmFlowtrackerTrackingParamTypeIngressDelay)) {
            keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
            rv = bcmi_keygen_qual_cfg_max_size_get(unit,
                    &keygen_cfg, pq_map->qual, &max_bits);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            max_bits = pq_map->width;
        }
    }

    /* UDF tracking param */
    if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
        rv = bcm_esw_flowtracker_udf_tracking_param_get(unit,
                0, param, custom_id, &param_udf_info);
        BCM_IF_ERROR_RETURN(rv);
        max_bits = param_udf_info.width;
    }

    if (min_value != NULL) {
        *min_value = 0;
    }
    if (max_value != NULL) {
        if ((max_bits == 0) || (max_bits == 32)) {
            *max_value = 0xFFFFFFFF;
        } else {
            *max_value = (1 << max_bits) - 1;
        }
    }
    if (num_bits != NULL) {
        *num_bits = max_bits;
    }

    return rv;
}

/*
 * Function:
 *   _bcm_field_ft_tracking_param_type_get
 * Purpose:
 *   Returns tracking param type if supported.
 * Parameters:
 *   unit       - (IN)  BCM device number.
 *   param      - (IN)  Tracking param.
 *   flags      - (IN)  Tracking param flags.
 *   ft_type    - (OUT) Tracking param ft type.
 * Notes:
 *   This is a utility api to fetch ft_type of tracking
 *   param passed as input. Any tracking param should
 *   belong to either IFT/MFT/EFT stage currently supported.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_tracking_param_type_get(int unit,
                                    bcm_flowtracker_tracking_param_type_t param,
                                    uint16 custom_id,
                                    uint32 flags,
                                    uint16 *ft_type_bmp)
{
    int rv = BCM_E_NONE;
    _field_stage_t *stage_fc = NULL;
    _field_ft_info_t *ft_info = NULL;
    bcm_flowtracker_udf_info_t param_udf_info;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    const char *aludatalist[] = _BCM_FIELD_FT_ALU_DATA_STRINGS;

    /* Input Param Check */
    if (param >= _FIELD_FT_TRACKING_PARAM_MAX ||
        ft_type_bmp == NULL) {
        return BCM_E_PARAM;
    }

    /* Retreive flowtracker stage */
    rv = _field_stage_control_get(unit,
                                  _BCM_FIELD_STAGE_FLOWTRACKER,
                                  &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Retreive tracking param db */
    ft_info = stage_fc->ft_info;
    map_db = ft_info->param_qual_map_db;
    pq_map = map_db->param_qual_map[param];

    /* Return error if not present in db */
    if (pq_map == NULL) {
        if (param < _FIELD_FT_TRACKING_ELEMENT_TYPE_MAX) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Tracking param %s is not available\n"), plist[param]));
        } else if (param < _FIELD_FT_TRACKING_PARAM_MAX) {
            LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
               "Tracking param %s is not available\n"),
               aludatalist[param - _FIELD_FT_TRACKING_ELEMENT_TYPE_MAX]));
        }
        return BCM_E_PARAM;
    }

    /* UDF tracking param */
    if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
        rv = bcm_esw_flowtracker_udf_tracking_param_get(unit,
                0, param, custom_id, &param_udf_info);
        if (BCM_FAILURE(rv)) {
            if (param < _FIELD_FT_TRACKING_ELEMENT_TYPE_MAX) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "UDF Tracking param %s(%d) not configured\n"),
                            plist[param], custom_id));
            }
            return rv;
        }

        if ((!(flags & BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY)) &&
                (param_udf_info.width > 32)) {

            if (param < _FIELD_FT_TRACKING_ELEMENT_TYPE_MAX) {
                LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                                "Tracking param %s(%d) exceeds supported"
                                " width\n"), plist[param], custom_id));
            }
            return BCM_E_PARAM;
        }
    }

    /* Return ft type of provided tracking param */
    *ft_type_bmp = pq_map->db_ft_type_bmp;

    return rv;
}

/*
 * Function:
 *   _bcm_field_ft_session_profiles_install
 * Purpose:
 *   Install session profiles for ftfp policy
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id.
 *   profiles   - (OUT) Session Profiles.
 * Returns:
 *   BCM_E_XXX
 */
int _bcm_field_ft_session_profiles_install(
        int unit,
        bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    bcmi_ft_session_profiles_t profiles;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if ((BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeAggregateMmu) ||
        (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_NONE;
    }
#endif

    sal_memset(&profiles, 0, sizeof (bcmi_ft_session_profiles_t));
    rv = _field_hx5_ft_session_profiles_install(unit, id, &profiles);

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_ft_group_ftfp_profiles_set(unit, id, &profiles);
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_ft_group_session_key_type_set(unit,
                id, profiles.session_key_profile_idx - 1);
    }

    return rv;
}

/*
 * Function:
 *   _bcm_field_ft_session_profiles_uninstall
 * Purpose:
 *   Un-install session profiles for ftfp policy
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id.
 *   profiles   - (IN) Session Profiles.
 * Returns:
 *   BCM_E_XXX
 */
int _bcm_field_ft_session_profiles_uninstall(
        int unit,
        bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    bcmi_ft_session_profiles_t profiles;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if ((BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeAggregateMmu) ||
        (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_NONE;
    }
#endif

    sal_memset(&profiles, 0, sizeof (bcmi_ft_session_profiles_t));

    rv = bcmi_ft_group_ftfp_profiles_get(unit, id, &profiles);
    BCM_IF_ERROR_RETURN(rv);

    return _field_hx5_ft_session_profiles_remove(unit, id, &profiles);
}

/*
 * Function:
 *   _bcm_field_ft_session_profiles_recover
 * Purpose:
 *   Recover session profiles indices.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_session_profiles_recover(int unit,
                                       bcm_flowtracker_group_t id)
{
    bcmi_ft_session_profiles_t profiles;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if ((BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeAggregateMmu) ||
        (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeAggregateEgress)) {
        return BCM_E_NONE;
    }
#endif

    BCM_IF_ERROR_RETURN(bcmi_ft_group_ftfp_profiles_get(unit,
            id, &profiles));

    BCM_IF_ERROR_RETURN(_field_hx5_ft_session_profiles_reference(
            unit, id, &profiles));

    if (profiles.session_key_profile_idx != 0) {
        BCM_IF_ERROR_RETURN(bcmi_ft_group_session_key_type_set(unit,
                    id, profiles.session_key_profile_idx - 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_field_ft_group_checkbmp_get
 * Purpose:
 *   Get flowtracker check in ALU16 bitmap.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *  id         - (IN) Flowtracker Group Id.
 *  num_checks - (IN) Number of flowchecks.
 *  check_list - (IN) List of Flowchecks
 *  check_bmp_data - (OUT) Data bitmap
 *  check_bmp_mask - (OUT) Mask bitmap.
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_field_ft_group_checkbmp_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *check_bmp_data,
        uint32 *check_bmp_mask)
{
    return bcmi_esw_ft_group_checks_bitmap_get(unit,
                id, num_checks, check_list, check_bmp_data,
                check_bmp_mask);
}

/*
 * Function:
 *   _bcm_field_ft_group_checklist_get
 * Purpose:
 *   Get list of flowtracker check from ALU16 bitmap.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   id         - (IN) Flowtracker Group Id.
 *   check_bmp_data - (IN) Data bitmap.
 *   check_bmp_mask - (IN) Mask bitmap.
 *   num_checks - (IN) Number of flowchecks.
 *   check_list - (OUT) List of Flowchecks.
 *   actual_num_checks - (OUT) actual number of flowchecks.
 * Returns:
 *   BCM_E_XXX
 */
int _bcm_field_ft_group_checklist_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_bmp_data,
        uint32 check_bmp_mask,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *actual_num_checks)
{
    return bcmi_esw_ft_group_checks_list_get(unit,
                id, check_bmp_data, check_bmp_mask, num_checks,
                check_list, actual_num_checks);
}

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
/*
 * Function:
 *   _field_ft_mmu_tracking_param_qual_map_db_init
 * Purpose:
 *   Prepare database for mmu tracking params and corresponding
 *   qualifiers.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   map_db     - (IN/OUT) param-qualifier map
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_ft_mmu_tracking_param_qual_map_db_init(int unit,
            _field_ft_tracking_param_qual_map_db_t *map_db)
{
    int rv = BCM_E_NONE;
    int offset = 0;
    uint16 ft_type_bmp = 0;
    uint32 flags = 0, len = 0;
    soc_format_t fmt = INVALIDfmt;
    soc_field_info_t *field_info = NULL;

    _FP_FT_TRACKING_PARAM_QUAL_MAP_DECL;

    /* MFT Session Bus */
    fmt = MFT_SESSION_DATA_BUSfmt;
    field_info = soc_format_fieldinfo_get(unit, fmt, MMU_PERC_FILL_LEVELf);
    if (field_info == NULL) {
        return BCM_E_NONE;
    }

    offset = field_info->bp;

    flags = _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;
    ft_type_bmp = (BCMI_FT_TYPE_F_MFT | BCMI_FT_TYPE_F_AMFT);

    fmt = MMU_MFT_PERC_FILL_LEVELfmt;

    /* Enable congestion level tracking only if congestion feature
     * is enabled which in turn is set based on
     * OTP bit (CONGESTION_TRACKING_DISABLE)
     */
    if (soc_feature(unit, soc_feature_flowtracker_ver_2_mmu_congestion_level)) {
        /* Percentage fill-level of port-PG min guarantee space. This is valid for
           UC & MC.
         */
        if (soc_format_field_valid(unit, fmt, THDI_PTPG_MIN_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_PTPG_MIN_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPortPGMinCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of port-PG shared space. This is valid for UC & MC. */
        if (soc_format_field_valid(unit, fmt, THDI_PTPG_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_PTPG_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPortPGSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of port-SP ?min guarantee? space. Report the service
           pool fill level for one of the four SPs, that the current packet belongs to.
           This is valid for UC & MC.
         */
        if (soc_format_field_valid(unit, fmt, THDI_PTSP_MIN_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_PTSP_MIN_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPortPoolMinCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of port-SP shared space. Report the service pool
           fill level for one of the four SPs, that the current packet belongs to.
           This is valid for UC & MC.
         */
        if (soc_format_field_valid(unit, fmt, THDI_PTSP_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_PTSP_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPortPoolSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of global-SP shared space. Report the service pool
           fill level for one of the four SPs, that the current packet belongs to.
           This is valid for UC & MC.
         */
        if (soc_format_field_valid(unit, fmt, THDI_POOL_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_POOL_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPoolSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of PG headroom space. This is valid for UC & MC. */
        if (soc_format_field_valid(unit, fmt, THDI_PTPG_HDRM_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_PTPG_HDRM_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPortPGHeadroomCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of port-SP headroom space. Report the service pool
           fill level for one of the four SPs, that the current packet belongs to.
           This is valid for UC & MC.
         */
        if (soc_format_field_valid(unit, fmt, THDI_PTSP_HDRM_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_PTSP_HDRM_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngPortPoolHeadroomCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of global headroom space. This is valid for
           UC & MC.
         */
        if (soc_format_field_valid(unit, fmt, THDI_GLOBAL_HDRM_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDI_GLOBAL_HDRM_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIngGlobalHeadroomCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the Unicast & Multicast buffer combined,
           unless separate service pools are used for UC and MC traffic types.
         */
        if (soc_format_field_valid(unit, fmt, THDO_POOL_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDO_POOL_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrPoolSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the current UC queue minimum guarantee space.
           Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 0 (UC).
           Also, THDR_DB_Q_MIN_FILLf
           Percentage fill-level of the current RQE queue (for MC) minimum
           guarantee space. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 1 (MC).
         */
        if (soc_format_field_valid(unit, fmt, THDU_Q_MIN_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDU_Q_MIN_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrQueueMinCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the current UC queue shared space. Depending
           of the color of the packet, provide the percentage fill of the queue
           shared count, as compared to the green, yellow and red drop-limits of
           the queue. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 0 (UC).
           Also, THDR_DB_Q_SHR_FILLf
           Percentage fill-level of the current RQE queue shared space. Depending
           of the color of the packet, provide the percentage fill of the queue
           shared count, as compared to the green, yellow and red drop-limits of
           the queue. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 1 (MC).
         */
        if (soc_format_field_valid(unit, fmt, THDU_Q_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDU_Q_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrQueueSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the current UC queue-group minimum guarantee
           space. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 0 (UC).
           Also, THDR_DB_POOL_SHR_FILLf
           Percentage fill-level of the global buffer pool computed based on
           THDR_DB_POOL limit. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 1 (MC).
         */
        if (soc_format_field_valid(unit, fmt, THDU_QGRP_MIN_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDU_QGRP_MIN_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrQueueGroupMinCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the current UC queue-group shared space.
           Similar to the UC queue shared fill-level. Valid when
           MMU_MFT_PKT_FLAG.PKT_UC_MC is 0 (UC).
           Also, THDR_QE_Q_MIN_FILLf
           Percentage fill-level of the current RQE queue (for MC) minimum
           guarantee space. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 1 (MC).
         */
        if (soc_format_field_valid(unit, fmt, THDU_QGRP_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDU_QGRP_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrQueueGroupSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the current UC Port-SP shared space.
           Similar to the UC queue shared fill-level. Valid when
           MMU_MFT_PKT_FLAG.PKT_UC_MC is 0 (UC).
           Also, THDR_QE_Q_SHR_FILLf
           Percentage fill-level of the current RQE queue shared space. Depending
           of the color of the packet, provide the percentage fill of the queue
           shared count, as compared to the green, yellow and red drop-limits of
           the queue. Valid when MMU_MFT_PKT_FLAG.PKT_UC_MC is 1 (MC).
         */
        if (soc_format_field_valid(unit, fmt, THDU_PORTSP_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDU_PORTSP_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrPortPoolSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of the RQE Qentry FIFO, which indicates the number
           of MC packet burst that the chip can absorb. Valid when
           MMU_MFT_PKT_FLAG.PKT_UC_MC is 1 (MC)
         */
        if (soc_format_field_valid(unit, fmt, THDR_QE_POOL_SHR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, THDR_QE_POOL_SHR_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeEgrRQEPoolSharedCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* Percentage fill-level of free cell pointer pool. Valid for all
           pakcet types.
         */
        if (soc_format_field_valid(unit, fmt, CFAP_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, CFAP_FILLf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeCFAPCongestionLevel,
                        0, offset + field_info->bp, field_info->len, ft_type_bmp);
            }
        }

        /* MMU aggregate fill-level percentage based on various fill-levels
           per packet type (UC or MC)
         */
        if (soc_format_field_valid(unit, fmt, MMU_AGGR_FILLf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, MMU_AGGR_FILLf);
            if (field_info != NULL) {
                if ((SOC_IS_HURRICANE4(unit)) &&
                        (field_info->len == 16)) {
                    len = field_info->len - 8;
                } else {
                    len = field_info->len;
                }
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeMMUCongestionLevel,
                        0, field_info->bp, len, ft_type_bmp);
            }
        }
    }

    fmt = MFT_SESSION_DATA_BUSfmt;
    field_info = soc_format_fieldinfo_get(unit, fmt, MFT_IPIPE_LATE_STAGE_DATAf);
    if (field_info == NULL) {
        return BCM_E_INTERNAL;
    }

    offset = field_info->bp;


    fmt = IPIPE_LATE_STAGE_DATAfmt;

#if 0
    /* ECMP Member Id Level 1 */
    if (soc_format_field_valid(unit, fmt, ECMP_MEMBER_ID_LEVEL1f)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, ECMP_MEMBER_ID_LEVEL1f);
        len = soc_format_field_length(unit, fmt, ECMP_MEMBER_ID_LEVEL1_RESERVEDf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeECMPMemberLevel1,
                    0, offset + field_info->bp, len + field_info->len, ft_type_bmp);
        }
    }

    /* ECMP Member Level 2 */
    if (soc_format_field_valid(unit, fmt, ECMP_MEMBER_ID_LEVEL2f)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, ECMP_MEMBER_ID_LEVEL2f);
        len = soc_format_field_length(unit, fmt, ECMP_MEMBER_ID_LEVEL2_RESERVEDf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeECMPMemberLevel2,
                    0, offset + field_info->bp, len + field_info->len, ft_type_bmp);
        }
    }
#endif

    /* Enable load balancing tracking params based on feature
     * which in turn is set based on OTP bit (ECMP_LAG_TRACKING_DISABLE).
     */
    if (soc_feature(unit, soc_feature_flowtracker_ver_2_load_balancing)) {

        /* ECMP Group Id Level 1*/
        if (soc_format_field_valid(unit, fmt, ECMP_GROUP_ID_LEVEL1f)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, ECMP_GROUP_ID_LEVEL1f);
            len = soc_format_field_length(unit, fmt, ECMP_GROUP_ID_LEVEL1_RESERVEDf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel1,
                        0, offset + field_info->bp, len + field_info->len, ft_type_bmp);
            }
        }

        /* ECMP Group Id Level 2 */
        if (soc_format_field_valid(unit, fmt, ECMP_GROUP_ID_LEVEL2f)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, ECMP_GROUP_ID_LEVEL2f);
            len = soc_format_field_length(unit, fmt, ECMP_GROUP_ID_LEVEL2_RESERVEDf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeECMPGroupIdLevel2,
                        0, offset + field_info->bp, len + field_info->len, ft_type_bmp);
            }
        }

        /* Trunk Member Id */
        if (soc_format_field_valid(unit, fmt, TRUNK_MEMBER_IDf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, TRUNK_MEMBER_IDf);
            len = soc_format_field_length(unit, fmt, TRUNK_MEMBER_ID_RESERVEDf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeTrunkMemberId,
                        0, offset + field_info->bp, len + field_info->len, ft_type_bmp);
            }
        }

        /* Trunk Group Id */
        if (soc_format_field_valid(unit, fmt, TRUNK_GROUP_IDf)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, TRUNK_GROUP_IDf);
            len = soc_format_field_length(unit, fmt, TRUNK_GROUP_ID_RESERVEDf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeTrunkGroupId,
                        0, offset + field_info->bp, len + field_info->len, ft_type_bmp);
            }
        }
    }

    fmt = MFT_SESSION_DATA_BUSfmt;

    /* MMU Queue Id */
    if (soc_format_field_valid(unit, fmt, QUEUE_IDf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, QUEUE_IDf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeMMUQueueId,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    /* MMU Queue Group Id */
    if (soc_format_field_valid(unit, fmt, QGIDf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, QGIDf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeMMUQueueGroupId,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    /* Packet Tos */
    if (soc_format_field_valid(unit, fmt, PACKET_TOSf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, PACKET_TOSf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeMMUPacketTos,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    /* Ingress Port */
    if (soc_format_field_valid(unit, fmt, INGRESS_PORTf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, INGRESS_PORTf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeIngPort,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    /* Enable drop reason code tracking params based on feature
     * which in turn is set based on
     * OTP bit (DROP_ANALYSIS_DISABLE) */
    if (soc_feature(unit, soc_feature_flowtracker_ver_2_drop_code)) {
        /* Drop Reason Group Id Vector */
        ft_type_bmp = (BCMI_FT_TYPE_F_MFT | BCMI_FT_TYPE_F_AMFT);
        _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                bcmFlowtrackerTrackingParamTypeIngDropReasonGroupIdVector,
                0, 240, 16, ft_type_bmp);
    }

cleanup:

    return rv;
}

/*
 * Function:
 *   _field_ft_egr_tracking_param_qual_map_db_init
 * Purpose:
 *   Prepare database for Egress tracking params and corresponding
 *   qualifiers.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   map_db     - (IN/OUT) param-qualifier map
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
_field_ft_egr_tracking_param_qual_map_db_init(int unit,
            _field_ft_tracking_param_qual_map_db_t *map_db)
{
    int rv = BCM_E_NONE;
    uint16 ft_type_bmp = 0;
    uint16 field_len = 0;
    uint32 flags = 0;
    soc_format_t fmt = INVALIDfmt;
    soc_field_info_t *field_info = NULL;

    _FP_FT_TRACKING_PARAM_QUAL_MAP_DECL;

    flags = _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;
    ft_type_bmp = (BCMI_FT_TYPE_F_EFT | BCMI_FT_TYPE_F_AEFT);

    fmt = EFT_SESSION_DATA_BUSfmt;

    if (soc_format_field_valid(unit, fmt, NEXT_HOP_Bf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, NEXT_HOP_Bf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeNextHopB,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    if (soc_format_field_valid(unit, fmt, NEXT_HOP_Af)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, NEXT_HOP_Af);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeNextHopA,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    if (soc_format_field_valid(unit, fmt, IP_PAYLOAD_LENGTHf)) {

        if (!SOC_IS_HURRICANE4(unit)) {
            field_info = soc_format_fieldinfo_get(unit, fmt, IP_PAYLOAD_LENGTHf);
            if (field_info != NULL) {
                _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                        bcmFlowtrackerTrackingParamTypeIPPayloadLength,
                        0, field_info->bp, field_info->len, ft_type_bmp);
            }
        }
    }

    /* FWD_DOMAIN_ATTR */
    /* FORWARDING_ZONE_TTL */

    if (soc_format_field_valid(unit, fmt, PACKET_TOSf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, PACKET_TOSf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeEgrPacketTos,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    /* FORWARDING_TYPE */
    /* CONGESTION_EXPERIENCED */

    if (soc_format_field_valid(unit, fmt, QOS_ATTRf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, QOS_ATTRf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeQosAttr,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    if (soc_format_field_valid(unit, fmt, EGRESS_PORTf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, EGRESS_PORTf);
        if (field_info != NULL) {
            /* bits [15-8] in Egress port is reserved. So, ignore it */
            field_len = field_info->len - 8;
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeEgrPort,
                    0, field_info->bp, field_len, ft_type_bmp);
        }
    }

    /* DGPP */

    /* Hr4 has MMU_COS_CEf and FB6 has MMU_COS_CMf */
    if (soc_format_field_valid(unit, fmt, MMU_COS_CEf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, MMU_COS_CEf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeMMUCos,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    if (soc_format_field_valid(unit, fmt, MMU_COS_CMf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, MMU_COS_CMf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeMMUCos,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }

    if (soc_format_field_valid(unit, fmt, OPAQUE_4_CLASS_IDf)) {
        field_info = soc_format_fieldinfo_get(unit, fmt, OPAQUE_4_CLASS_IDf);
        if (field_info != NULL) {
            _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                    bcmFlowtrackerTrackingParamTypeEgrClassId,
                    0, field_info->bp, field_info->len, ft_type_bmp);
        }
    }
    /* Enable drop reason code tracking params based on feature
     * which in turn is set based on
     * OTP bit (DROP_ANALYSIS_DISABLE) */
    if (soc_feature(unit, soc_feature_flowtracker_ver_2_drop_code)) {
        ft_type_bmp = (BCMI_FT_TYPE_F_EFT | BCMI_FT_TYPE_F_AEFT);
        _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit, map_db, flags,
                bcmFlowtrackerTrackingParamTypeEgrDropReasonGroupIdVector,
                0, 240, 16, ft_type_bmp);
    }
cleanup:

    return rv;
}
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

/*
 * Function:
 *   bcmi_ft_group_tracking_params_per_ft_type_list
 * Purpose:
 *   Show list of tracking params for given ft type
 *   qualifiers.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   map_db     - (IN) param-qualifier map
 *   ft_type_bmp- (IN) FT type
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_tracking_params_per_ft_type_list(
    int unit,
    _field_ft_tracking_param_qual_map_db_t *map_db,
    uint16 ft_type_bmp)
{
    bcmi_ft_type_t ft_type = bcmiFtTypeNone;
    bcm_flowtracker_tracking_param_type_t param;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;

    char *ft_type_str[] = BCMI_FT_TYPE_STRINGS;
    char *tracking_param_str[] = BCM_FT_TRACKING_PARAM_STRINGS;

    BCMI_FT_TYPE_F_UNIQUE_GET(ft_type_bmp, ft_type);
    LOG_CLI((BSL_META_U(unit, "\n\rFlowtracker Stage : %s\n\r"),
                ft_type_str[ft_type]));
    LOG_CLI((BSL_META_U(unit, "------------------------------------"
                    "----------------\n\r")));
    LOG_CLI((BSL_META_U(unit, "  %40s | %s\n\r"),
                "Tracking Params", " Size"));
    LOG_CLI((BSL_META_U(unit, "  %40s | %s\n\r"),
                "-------------------------------------", "-------"));

    for (param = bcmFlowtrackerTrackingParamTypeNone;
            param < bcmFlowtrackerTrackingParamTypeCount; param++) {

        pq_map = map_db->param_qual_map[param];
        if (pq_map == NULL) {
            continue;
        }

        if (pq_map->db_ft_type_bmp & ft_type_bmp) {
            LOG_CLI((BSL_META_U(unit, "  %40s | %4d\n\r"),
                        tracking_param_str[param], pq_map->width/8));
        }
        pq_map = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_tracking_params_per_ft_type_list
 * Purpose:
 *   Show list of tracking params for given ft type
 *   qualifiers.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   req_ft_type_bmp- (IN) FT type
 * Returns:
 *   BCM_E_XXX
 */
void
bcmi_ft_group_tracking_params_list(
    int unit,
    uint16 req_ft_type_bmp)
{
    int rv = BCM_E_NONE;
    int support = FALSE;
    _field_stage_t *stage_fc = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    /* Get FT Info */
    rv = _field_stage_control_get(unit,
            _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    if (BCM_FAILURE(rv)) {
        return;
    }

    map_db = stage_fc->ft_info->param_qual_map_db;

    bcmi_esw_ft_type_support_check(unit, bcmiFtTypeIFT, &support);
    if ((support) && (req_ft_type_bmp & BCMI_FT_TYPE_F_IFT)) {
        bcmi_ft_group_tracking_params_per_ft_type_list(unit,
                map_db, BCMI_FT_TYPE_F_IFT);
    }
    bcmi_esw_ft_type_support_check(unit, bcmiFtTypeMFT, &support);
    if ((support) && (req_ft_type_bmp & BCMI_FT_TYPE_F_MFT)) {
        bcmi_ft_group_tracking_params_per_ft_type_list(unit,
                map_db, BCMI_FT_TYPE_F_MFT);
    }
    bcmi_esw_ft_type_support_check(unit, bcmiFtTypeEFT, &support);
    if ((support) && (req_ft_type_bmp & BCMI_FT_TYPE_F_EFT)) {
        bcmi_ft_group_tracking_params_per_ft_type_list(unit,
                map_db, BCMI_FT_TYPE_F_EFT);
    }
    bcmi_esw_ft_type_support_check(unit, bcmiFtTypeAIFT, &support);
    if ((support) && (req_ft_type_bmp & BCMI_FT_TYPE_F_AIFT)) {
        bcmi_ft_group_tracking_params_per_ft_type_list(unit,
                map_db, BCMI_FT_TYPE_F_AIFT);
    }
    bcmi_esw_ft_type_support_check(unit, bcmiFtTypeAMFT, &support);
    if ((support) && (req_ft_type_bmp & BCMI_FT_TYPE_F_AMFT)) {
        bcmi_ft_group_tracking_params_per_ft_type_list(unit,
                map_db, BCMI_FT_TYPE_F_AMFT);
    }
    bcmi_esw_ft_type_support_check(unit, bcmiFtTypeAEFT, &support);
    if ((support) && (req_ft_type_bmp & BCMI_FT_TYPE_F_AEFT)) {
        bcmi_ft_group_tracking_params_per_ft_type_list(unit,
                map_db, BCMI_FT_TYPE_F_AEFT);
    }

    return;
}

/*
 * Function:
 *   _bcm_field_ft_udf_tracking_param_supported
 * Purpose:
 *   Check if given tracking param need UDF for extraction.
 *   qualifiers.
 * Parameters:
 *   unit       - (IN) BCM device number.
 *   param      - (IN) Tracking parameter
 *   width      - (OUT) Width if tracking param supported
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_udf_tracking_param_supported(
    int unit,
    bcm_flowtracker_tracking_param_type_t param,
    uint16 *width)
{
    int rv = BCM_E_NONE;
    _field_stage_t *stage_fc = NULL;
    _field_ft_info_t *ft_info = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    if (param >= _FIELD_FT_TRACKING_PARAM_MAX) {
        return FALSE;
    }

    rv = _field_stage_control_get(unit,
            _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    if (BCM_FAILURE(rv)) {
        return FALSE;
    }

    ft_info = stage_fc->ft_info;
    map_db = ft_info->param_qual_map_db;
    pq_map = map_db->param_qual_map[param];
    if (pq_map == NULL) {
        return FALSE;
    }

    if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
        *width = pq_map->width;
        return TRUE;
    }

    return FALSE;
}

/*
 * Function:
 *   _bcm_field_ft_tracking_param_show
 * Purpose:
 *   Show all tracking params alon with ft type supported
 *   qualifiers.
 * Parameters:
 *   unit       - (IN) BCM device number.
 * Returns:
 *   BCM_E_XXX
 */
int
_bcm_field_ft_tracking_param_show(int unit)
{
    int rv = BCM_E_NONE;
    int support = FALSE;
    _field_stage_t *stage_fc = NULL;
    _field_ft_info_t *ft_info = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcm_flowtracker_tracking_param_type_t param;
    char *ft_type_str[] = BCMI_FT_TYPE_STRINGS;
    char *param_str[] = BCM_FT_TRACKING_PARAM_STRINGS;

    rv = _field_stage_control_get(unit,
            _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    if (BCM_FAILURE(rv)) {
        return FALSE;
    }

    ft_info = stage_fc->ft_info;
    map_db = ft_info->param_qual_map_db;

    for (param = 0; param < bcmFlowtrackerTrackingParamTypeCount; param++) {

        pq_map = map_db->param_qual_map[param];
        if (pq_map == NULL) {
            LOG_CLI((BSL_META_U(unit, "  %40s |  - Not Available -\n\r"),
                        param_str[param]));
            continue;
        }

        LOG_CLI((BSL_META_U(unit, "  %40s | %4d "),
                    param_str[param], pq_map->width/8));

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_UDF_MAPPED) {
            LOG_CLI((BSL_META_U(unit, "UDF | ")));
        } else {
            LOG_CLI((BSL_META_U(unit, "    | ")));
        }

        bcmi_esw_ft_type_support_check(unit, bcmiFtTypeIFT, &support);
        if ((support) && (pq_map->db_ft_type_bmp & BCMI_FT_TYPE_F_IFT)) {
            LOG_CLI((BSL_META_U(unit, "%s,"), ft_type_str[bcmiFtTypeIFT]));
        }
        bcmi_esw_ft_type_support_check(unit, bcmiFtTypeMFT, &support);
        if ((support) && (pq_map->db_ft_type_bmp & BCMI_FT_TYPE_F_MFT)) {
            LOG_CLI((BSL_META_U(unit, "%s,"), ft_type_str[bcmiFtTypeMFT]));
        }
        bcmi_esw_ft_type_support_check(unit, bcmiFtTypeEFT, &support);
        if ((support) && (pq_map->db_ft_type_bmp & BCMI_FT_TYPE_F_EFT)) {
            LOG_CLI((BSL_META_U(unit, "%s,"), ft_type_str[bcmiFtTypeEFT]));
        }
        bcmi_esw_ft_type_support_check(unit, bcmiFtTypeAIFT, &support);
        if ((support) && (pq_map->db_ft_type_bmp & BCMI_FT_TYPE_F_AIFT)) {
            LOG_CLI((BSL_META_U(unit, "%s,"), ft_type_str[bcmiFtTypeAIFT]));
        }
        bcmi_esw_ft_type_support_check(unit, bcmiFtTypeAMFT, &support);
        if ((support) && (pq_map->db_ft_type_bmp & BCMI_FT_TYPE_F_AMFT)) {
            LOG_CLI((BSL_META_U(unit, "%s,"), ft_type_str[bcmiFtTypeAMFT]));
        }
        bcmi_esw_ft_type_support_check(unit, bcmiFtTypeAEFT, &support);
        if ((support) && (pq_map->db_ft_type_bmp & BCMI_FT_TYPE_F_AEFT)) {
            LOG_CLI((BSL_META_U(unit, "%s,"), ft_type_str[bcmiFtTypeAEFT]));
        }
        LOG_CLI((BSL_META_U(unit, "\n\r")));
    }

    return BCM_E_NONE;
}

#endif /* BCM_FLOWTRACKER_SUPPORT */
