/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        switch_appl_sign.c
 * Purpose:     The purpose of this file is to configure
 *              Applicate signature related functionalities.
 * Requires:
 */

#include <soc/defs.h>

#ifdef BCM_APPL_SIGNATURE_SUPPORT

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/pack.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/esw/cancun.h>
#include <bcm_int/esw/switch_appl_sign.h>

#if defined(BCM_WARM_BOOT_SUPPORT)
#include <soc/scache.h>
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>
#endif

/* Internal MACRO to goto cleanup. */
#define BCMI_IF_ERROR_CLEANUP(_rv_)                                     \
        do {                                                            \
            if (_rv_ != BCM_E_NONE) {                                   \
                goto cleanup;                                           \
            }                                                           \
        } while(0)

/* Alloc */
#define APPL_SIGN_ALLOC(_ptr_, _size_, _str_)                           \
        do {                                                            \
            if ((_ptr_) == NULL) {                                      \
                (_ptr_) = sal_alloc((_size_), (_str_));                 \
            }                                                           \
            if ((_ptr_) != NULL) {                                      \
                sal_memset((_ptr_), 0, _size_);                         \
            } else {                                                    \
                rv = BCM_E_MEMORY;                                      \
            }                                                           \
        } while (0)

/* Free */
#define APPL_SIGN_FREE(_ptr_)                                           \
        do {                                                            \
            if ((_ptr_) != NULL) {                                      \
                sal_free((_ptr_));                                      \
                (_ptr_) = NULL;                                         \
            }                                                           \
        } while(0)

/* Init check */
#define APPL_SIGN_INIT_CHECK(unit)                                      \
        if (!soc_feature(unit, soc_feature_appl_signature_support)) {   \
            return BCM_E_UNAVAIL;                                       \
        } else if (appl_sign_ctrl[unit] == NULL) {                      \
            return BCM_E_INIT;                                          \
        }

/* Index for stage entry part in application signature entry */
#define APPL_SIGN_PART_S3                       (0)
#define APPL_SIGN_PART_S4                       (1)
#define APPL_SIGN_PART_S2                       (2)
#define APPL_SIGN_PARTS_MAX                     (3)

/* Application signature control */
#define APPL_SIGN_CTRL(unit)  appl_sign_ctrl[unit]

/* Application signature Entry list */
#define BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, flags, list)             \
        if (flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {           \
            list = APPL_SIGN_CTRL(unit)->tnl_entry_list;                \
        } else {                                                        \
            list = APPL_SIGN_CTRL(unit)->entry_list;                    \
        }

/* Internal Context Id reference list */
#define BCMI_SWITCH_APPL_SIGN_CONTEXT_ID_REF_LIST(unit, flags, list)    \
        if (flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {           \
            list = APPL_SIGN_CTRL(unit)->tnl_context_id_ref;            \
        } else {                                                        \
            list = APPL_SIGN_CTRL(unit)->context_id_ref;                \
        }

/* Mutex */
static sal_mutex_t appl_sign_mutex[BCM_MAX_NUM_UNITS];

/* Application signature control */
static bcmi_switch_appl_sign_control_t *appl_sign_ctrl[BCM_MAX_NUM_UNITS];

#if defined(BCM_WARM_BOOT_SUPPORT)

/* Structure for tcam/data buffers for parser stages */
typedef struct bcmi_switch_appl_sign_parser_buf_s {
    uint32 *tcam_buf[APPL_SIGN_PARTS_MAX]; /* TCAM buffer for each stages */
    uint32 *data_buf[APPL_SIGN_PARTS_MAX]; /* Data buffer for each stages */
} bcmi_switch_appl_sign_parser_buf_t;

#endif /* BCM_WARM_BOOT_SUPPORT */


/* Lock */
int bcmi_switch_appl_sign_lock(int unit)
{
    if (appl_sign_mutex[unit] == NULL) {
        return BCM_E_INIT;
    }

    sal_mutex_take(appl_sign_mutex[unit], sal_mutex_FOREVER);
    return BCM_E_NONE;
}

/* Unlock */
int bcmi_switch_appl_sign_unlock(int unit)
{
    if (appl_sign_mutex[unit] == NULL) {
        return BCM_E_INIT;
    }
    if (sal_mutex_give(appl_sign_mutex[unit]) != 0) {
        return BCM_E_INTERNAL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_entry_field_value_get
 * Purpose:
 *      Read hw buffer and get data
 * Parameters:
 *      unit            - (IN) Unit number.
 *      offset          - (IN) Offset in buffer.
 *      width           - (IN) Width of value.
 *      buffer          - (IN) Buffer
 *      p_data          - (OUT) Data to populate
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_entry_field_value_get(
    int unit,
    int offset,
    int width,
    uint32 *buffer,
    uint32 *p_data)
{
    int len, idx, wp, bp;

    idx = 0;
    wp = offset / 32;
    bp = offset & (32 - 1);

    for (len = width; len > 0; len -=32) {
        /* If data starts in middle of word */
        if (bp != 0) {
            p_data[idx] = (buffer[wp] >> bp) & ((1 << (32 - bp)) - 1);
            if (len > (32 - bp)) {
                wp++;
                p_data[idx] |= buffer[wp] << (32 - bp);
            }
        } else {
            p_data[idx] = buffer[wp];
            wp++;
        }

        if (len < 32) {
            p_data[idx] &= ((1 << len) - 1);
        }

        idx++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_entry_field_value_set
 * Purpose:
 *      Write data to hw buffer
 * Parameters:
 *      unit            - (IN) Unit number.
 *      offset          - (IN) Offset in buffer.
 *      width           - (IN) Width of value.
 *      p_data          - (IN) Data to populate
 *      buffer          - (OUT) Buffer
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_entry_field_value_set(
    int unit,
    int offset,
    int width,
    uint32 *p_data,
    uint32 *buffer)
{
    int len, idx, wp, bp;
    uint32 u32_mask;

    idx = 0;
    wp = offset / 32;
    bp = offset & (32 - 1);

    for (len = width; len > 0; len -=32) {
        /* If data starts in middle of word */
        if (bp != 0) {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
            } else {
                u32_mask = 0xffffffff;
            }

            buffer[wp] &= ~(u32_mask << bp);
            buffer[wp] |= (p_data[idx] << bp);
            if (len > (32 - bp)) {
                wp++;
                buffer[wp] &= ~(u32_mask >> (32-bp));
                buffer[wp] |= p_data[idx] >> (32 - bp) & ((1 << bp) - 1);
            }
        } else {
            if (len < 32) {
                u32_mask = (1 << len) - 1;
                buffer[wp] &= ~u32_mask;
                buffer[wp] |= p_data[idx];
            } else {
                buffer[wp] = p_data[idx];
            }
            wp++;
        }
        idx++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_mem_get
 * Purpose:
 *      Get Parser Stage memory for tcam and data.
 * Parameters:
 *      unit     - (IN) UNIT Number.
 *      stage_id - (IN) Stage.
 *      tcam_mem - (OUT) Tcam memory.
 *      data_mem - (OUT) Data memory.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_mem_get(
    int unit,
    int stage_id,
    soc_mem_t *tcam_mem,
    soc_mem_t *data_mem)
{
    soc_mem_t tcam_table;
    soc_mem_t data_table;

    switch(stage_id) {
        case BCMI_SWITCH_PARSER1_STAGE2:
            tcam_table = IP_PARSER1_HME_STAGE_TCAM_NARROW_ONLY_2m;
            data_table = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_2m;
            break;

        case BCMI_SWITCH_PARSER2_STAGE2:
            tcam_table = IP_PARSER2_HME_STAGE_TCAM_NARROW_ONLY_2m;
            data_table = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_2m;
            break;

        case BCMI_SWITCH_PARSER1_STAGE3:
            tcam_table = IP_PARSER1_HME_STAGE_TCAM_ONLY_3m;
            data_table = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_3m;
            break;

        case BCMI_SWITCH_PARSER2_STAGE3:
            tcam_table = IP_PARSER2_HME_STAGE_TCAM_ONLY_3m;
            data_table = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_3m;
            break;

        case BCMI_SWITCH_PARSER1_STAGE4:
            tcam_table = IP_PARSER1_HME_STAGE_TCAM_ONLY_4m;
            data_table = IP_PARSER1_HME_STAGE_TCAM_DATA_ONLY_4m;
            break;

        case BCMI_SWITCH_PARSER2_STAGE4:
            tcam_table = IP_PARSER2_HME_STAGE_TCAM_ONLY_4m;
            data_table = IP_PARSER2_HME_STAGE_TCAM_DATA_ONLY_4m;
            break;

        default:
            tcam_table = INVALIDm;
            data_table = INVALIDm;
            return BCM_E_PARAM;
    }

    if (tcam_mem != NULL) {
        *tcam_mem = tcam_table;
    }
    if (data_mem != NULL) {
        *data_mem = data_table;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_flex_fields_set
 * Purpose:
 *      Write Key and Mask fields from flex cancun
 *      related fields.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      flex_data    - (IN) Flex data
 *      stage_entry  - (IN/OUT) Parser Stage Entry.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_flex_fields_set(
    int unit,
    uint32 entry_flags,
    int num_of_fields,
    bcmi_switch_appl_sign_entry_flex_field_t *flex_fields,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry)
{
    int idx = 0, j = 0;
    int offset,  width;
    int rv = BCM_E_NONE;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t data_mem = INVALIDm;
    uint32 value;
    uint32 key_base_offset = 0;
    uint32 mask_base_offset = 0;
    uint32 data_base_offset = 0;
    soc_field_info_t *fieldinfo = NULL;
    char *value_str[] = {"TCP", "UDP", "L4_ELIGIBLE"};
    soc_cancun_ceh_field_info_t ceh_info;

    /* Get Tcam/Data memory */
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_entry->stage_id, &tcam_mem, &data_mem);
    BCM_IF_ERROR_RETURN(rv);

    fieldinfo = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
    if (fieldinfo == NULL) {
        return BCM_E_INTERNAL;
    }
    key_base_offset = fieldinfo->bp;

    fieldinfo = soc_mem_fieldinfo_get(unit, tcam_mem, MASKf);
    if (fieldinfo == NULL) {
        return BCM_E_INTERNAL;
    }
    mask_base_offset = fieldinfo->bp;

    fieldinfo = soc_mem_fieldinfo_get(unit, data_mem, DATAf);
    if (fieldinfo == NULL) {
        return BCM_E_INTERNAL;
    }
    data_base_offset = fieldinfo->bp;

    sal_memset(&ceh_info, 0, sizeof (soc_cancun_ceh_field_info_t));

    for (idx = 0; idx < num_of_fields; idx++) {

        /* Value string index to be used for field in ceh based derivation */
        if (flex_fields[idx].arg == 1) {
            j = 2;
        } else if (entry_flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_UDP) {
            j = 1;
        } else {
            j = 0;
        }

        /* Get Ceh info for cancun derived */
        if (flex_fields[idx].type & 0x1) {
            rv = soc_cancun_ceh_obj_field_get(unit,
                    flex_fields[idx].obj_str, value_str[j], &ceh_info);
            if (rv == BCM_E_NOT_FOUND) {
                continue;
            } else if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }
        }

        switch(flex_fields[idx].type) {
            case 0x0: /* Fixed data */
                fieldinfo = soc_mem_fieldinfo_get(unit,
                        data_mem, flex_fields[idx].field);
                offset = fieldinfo->bp - data_base_offset;
                width = fieldinfo->len;
                value = flex_fields[idx].value;

                rv = bcmi_switch_entry_field_value_set(unit,
                        offset, width, &value, stage_entry->hw_data_buf);
                BCM_IF_ERROR_RETURN(rv);
                break;

            case 0x1: /* Cancun derived data */
                fieldinfo = soc_mem_fieldinfo_get(unit,
                        data_mem, flex_fields[idx].field);

                offset = fieldinfo->bp + ceh_info.offset - data_base_offset;
                width = ceh_info.width;
                value = ceh_info.value;

                rv = bcmi_switch_entry_field_value_set(unit,
                        offset, width, &value, stage_entry->hw_data_buf);
                BCM_IF_ERROR_RETURN(rv);
                break;

            case 0x2: /* Fixed key */
                fieldinfo = soc_mem_fieldinfo_get(unit,
                        tcam_mem, flex_fields[idx].field);
                offset = fieldinfo->bp - key_base_offset;
                width = fieldinfo->len;
                value = flex_fields[idx].value;

                rv = bcmi_switch_entry_field_value_set(unit,
                        offset, width, &value, stage_entry->hw_key_buf);
                BCM_IF_ERROR_RETURN(rv);
                break;

            case 0x3: /* Cancun derived key */
                fieldinfo = soc_mem_fieldinfo_get(unit,
                        tcam_mem, flex_fields[idx].field);

                offset = fieldinfo->bp + ceh_info.offset - key_base_offset;
                width = ceh_info.width;
                value = ceh_info.value;

                rv = bcmi_switch_entry_field_value_set(unit,
                        offset, width, &value, stage_entry->hw_key_buf);
                BCM_IF_ERROR_RETURN(rv);
                break;

            case 0x4: /* Fixed mask */
                fieldinfo = soc_mem_fieldinfo_get(unit,
                        tcam_mem, flex_fields[idx].field);
                offset = fieldinfo->bp - mask_base_offset;
                width = fieldinfo->len;
                value = flex_fields[idx].value;

                rv = bcmi_switch_entry_field_value_set(unit,
                        offset, width, &value, stage_entry->hw_mask_buf);
                BCM_IF_ERROR_RETURN(rv);
                break;

            case 0x5: /* Cancun derived mask */
                fieldinfo = soc_mem_fieldinfo_get(unit,
                        tcam_mem, flex_fields[idx].field);

                offset = fieldinfo->bp + ceh_info.offset - mask_base_offset;
                width = ceh_info.width;
                value = ceh_info.value;

                rv = bcmi_switch_entry_field_value_set(unit,
                        offset, width, &value, stage_entry->hw_mask_buf);
                BCM_IF_ERROR_RETURN(rv);
                break;

            default:
                return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_entry_free
 * Purpose:
 *      Free Buffers for parser stage entry.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      stage_entry - (IN/OUT) Parser Stage Entry.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_entry_free(
    int unit,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry)
{
    APPL_SIGN_FREE(stage_entry->hw_key_buf);
    APPL_SIGN_FREE(stage_entry->hw_mask_buf);
    APPL_SIGN_FREE(stage_entry->hw_data_buf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_entry_hw_get
 * Purpose:
 *      Get Hw Buffers for Stage Entry.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      stage_entry - (IN/OUT) Parser Stage Entry.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_entry_hw_get(
    int unit,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry)
{
    int rv = BCM_E_NONE;
    int key_words, data_words;
    soc_mem_t tcam_mem;
    soc_mem_t data_mem;

    /* Get Tcam/Data memory */
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_entry->stage_id, &tcam_mem, &data_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Get Key/Mask buffer words */
    key_words = BITS2WORDS(soc_mem_field_length(unit,
                tcam_mem, KEYf));

    /* Get Data Buffer words */
    data_words = BITS2WORDS(soc_mem_field_length(unit,
                data_mem, DATAf));

    stage_entry->key_size =  key_words;
    stage_entry->data_size = data_words;

    APPL_SIGN_ALLOC(stage_entry->hw_key_buf,
            sizeof(uint32) * stage_entry->key_size,
            "stage entry key buf");
    BCMI_IF_ERROR_CLEANUP(rv);

    APPL_SIGN_ALLOC(stage_entry->hw_mask_buf,
            sizeof(uint32) * stage_entry->key_size,
            "stage entry mask buf");
    BCMI_IF_ERROR_CLEANUP(rv);

    APPL_SIGN_ALLOC(stage_entry->hw_data_buf,
            sizeof(uint32) * stage_entry->data_size,
            "stage entry data buf");
    BCMI_IF_ERROR_CLEANUP(rv);

    return BCM_E_NONE;

cleanup:

    bcmi_switch_parser_stage_entry_free(unit, stage_entry);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser1_stage2_entry_init
 * Purpose:
 *      Initialize default parser stage entry
 *      for parser 1 stage 2.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN/OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser1_stage2_entry_init(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry = NULL;
    bcmi_switch_appl_sign_entry_flex_field_t flex_fields[] = {
        {0x5, IN_CONTEXT_ID_MASKf, "PARSER1_S2_IN_CONTEXT_ID_MASK", 1, 0},
        {0x5, HDR_TYPE_MASKf, "PARSER1_S2_HDR_TYPE_MASK", 0, 0},
        {0x5, VALID_BYTES_CHECK_VECTOR_MASKf, "PARSER1_S2_VALID_BYTES_CHECK_VECTOR_MASK", 0, 0},
        {0x3, IN_CONTEXT_IDf, "PARSER1_S2_IN_CONTEXT_ID", 1, 0},
        {0x3, HDR_TYPEf, "PARSER1_S2_HDR_TYPE", 0, 0},
        {0x3, VALID_BYTES_CHECK_VECTORf, "PARSER1_S2_VALID_BYTES_CHECK_VECTOR", 0, 0},
        {0x1, STAGE_MATCH_ID_MASKf, "PARSER1_S2_STAGE_MATCH_ID_MASK", 0, 0},
        {0x1, STAGE_MATCH_IDf, "PARSER1_S2_STAGE_MATCH_ID", 0, 0},
        {0x1, CONTEXT_IDf, "PARSER1_S2_CONTEXT_ID", 0, },
        {0x1, VALID_BYTES_NEEDEDf, "PARSER1_S2_VALID_BYTES_NEEDED", 0, 0},
        {0x1, BASE_SHIFT_AMOUNTf, "PARSER1_S2_BASE_SHIFT_AMOUNT", 0, 0},
        {0x1, STAGE_HFE_PROF_PTRf, "PARSER1_S2_STAGE_HFE_PROF_PTR", 0, 0},
        {0x1, STAGE_MATCH_ID_OFFSETf, "PARSER1_S2_STAGE_MATCH_ID_OFFSET", 0, 0},
        {0x0, ACTION_IF_BYTES_ENOUGHf, NULL, 0, 1},
        {0x0, ACTION_IF_BYTES_NOT_ENOUGHf, NULL, 0, 4},
        {0x0, CONTEXT_ID_UPDATE_MASKf, NULL, 0, 0xFFFF},
        {0x0, STAGE_HFE_PROF_PTR_VALIDf, NULL, 0, 1}
    };

    stage_entry = &entry->stage_entry[APPL_SIGN_PART_S2];
    stage_entry->stage_id = BCMI_SWITCH_PARSER1_STAGE2;

    rv = bcmi_switch_parser_stage_entry_hw_get(unit, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_flex_fields_set(unit,
            entry->flags, COUNTOF(flex_fields), flex_fields, stage_entry);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_parser_stage_entry_free(unit, stage_entry);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser1_stage3_entry_init
 * Purpose:
 *      Initialize default parser stage entry for
 *      parser 1 stage 3.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN/OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser1_stage3_entry_init(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry = NULL;
    bcmi_switch_appl_sign_entry_flex_field_t flex_fields[] = {
        {0x3, IN_CONTEXT_IDf, "PARSER1_S3_IN_CONTEXT_ID", 0, 0},
        {0x5, VALID_BYTES_CHECK_VECTOR_MASKf, "PARSER1_S3_VALID_BYTES_CHECK_VECTOR_MASK", 0, 0},
        {0x3, VALID_BYTES_CHECK_VECTORf, "PARSER1_S3_VALID_BYTES_CHECK_VECTOR", 0, 0},
        {0x5, IN_CONTEXT_ID_MASKf, "PARSER1_S3_IN_CONTEXT_ID_MASK", 0, 0},
        {0x4, HDR_TYPE_MASKf, NULL, 0, 0xFFFF},
        {0x0, CONTEXT_ID_UPDATE_MASKf, NULL, 0, 0x7F00},
        {0x0, VALID_BYTES_NEEDEDf, NULL, 0, 16},
        {0x0, ACTION_IF_BYTES_ENOUGHf, NULL, 0, 1},
        {0x0, ACTION_IF_BYTES_NOT_ENOUGHf, NULL, 0, 4},
        {0x0, BASE_SHIFT_AMOUNTf, NULL, 0, 8}
    };

    stage_entry = &entry->stage_entry[APPL_SIGN_PART_S3];
    stage_entry->stage_id = BCMI_SWITCH_PARSER1_STAGE3;

    rv = bcmi_switch_parser_stage_entry_hw_get(unit, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_flex_fields_set(unit,
            entry->flags, COUNTOF(flex_fields), flex_fields, stage_entry);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_parser_stage_entry_free(unit, stage_entry);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser1_stage4_entry_init
 * Purpose:
 *      Initialize default parser stage entry for
 *      parser 1 stage 4.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN/OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser1_stage4_entry_init(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry = NULL;
    bcmi_switch_appl_sign_entry_flex_field_t flex_fields[] = {
        {0x3, VALID_BYTES_CHECK_VECTORf,  "PARSER1_S4_VALID_BYTES_CHECK_VECTOR", 0, 0},
        {0x5, VALID_BYTES_CHECK_VECTOR_MASKf,   "PARSER1_S4_VALID_BYTES_CHECK_VECTOR_MASK", 0, 0},
        {0x4, IN_CONTEXT_ID_MASKf, NULL, 0, 0x7f00},
        {0x0, VALID_BYTES_NEEDEDf, NULL, 0, 16},
        {0x0, ACTION_IF_BYTES_ENOUGHf, NULL, 0, 1},
        {0x0, ACTION_IF_BYTES_NOT_ENOUGHf, NULL, 0, 4},
        {0x0, CONTEXT_ID_UPDATE_MASKf, NULL, 0, 0xFFFF}
    };

    stage_entry = &entry->stage_entry[APPL_SIGN_PART_S4];
    stage_entry->stage_id = BCMI_SWITCH_PARSER1_STAGE4;

    rv = bcmi_switch_parser_stage_entry_hw_get(unit, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_flex_fields_set(unit,
            entry->flags, COUNTOF(flex_fields), flex_fields, stage_entry);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_parser_stage_entry_free(unit, stage_entry);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser2_stage2_entry_init
 * Purpose:
 *      Initialize default parser stage entry for
 *      parser 2 stage 2.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN/OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser2_stage2_entry_init(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry = NULL;
    bcmi_switch_appl_sign_entry_flex_field_t flex_fields[] = {
        {0x5, IN_CONTEXT_ID_MASKf, "PARSER2_S2_IN_CONTEXT_ID_MASK", 1, 0},
        {0x5, HDR_TYPE_MASKf, "PARSER2_S2_HDR_TYPE_MASK", 0, 0},
        {0x5, VALID_BYTES_CHECK_VECTOR_MASKf, "PARSER2_S2_VALID_BYTES_CHECK_VECTOR_MASK", 0, 0},
        {0x3, IN_CONTEXT_IDf, "PARSER2_S2_IN_CONTEXT_ID", 1, 0},
        {0x3, HDR_TYPEf, "PARSER2_S2_HDR_TYPE", 0, 0},
        {0x3, VALID_BYTES_CHECK_VECTORf, "PARSER2_S2_VALID_BYTES_CHECK_VECTOR", 0, 0},
        {0x1, STAGE_MATCH_ID_MASKf, "PARSER2_S2_STAGE_MATCH_ID_MASK", 0, 0},
        {0x1, STAGE_MATCH_IDf, "PARSER2_S2_STAGE_MATCH_ID", 0, 0},
        {0x1, CONTEXT_IDf, "PARSER2_S2_CONTEXT_ID", 0, },
        {0x1, VALID_BYTES_NEEDEDf, "PARSER2_S2_VALID_BYTES_NEEDED", 0, 0},
        {0x1, BASE_SHIFT_AMOUNTf, "PARSER2_S2_BASE_SHIFT_AMOUNT", 0, 0},
        {0x1, STAGE_HFE_PROF_PTRf, "PARSER2_S2_STAGE_HFE_PROF_PTR", 0, 0},
        {0x1, STAGE_MATCH_ID_OFFSETf, "PARSER2_S2_STAGE_MATCH_ID_OFFSET", 0, 0},
        {0x0, ACTION_IF_BYTES_ENOUGHf, NULL, 0, 1},
        {0x0, ACTION_IF_BYTES_NOT_ENOUGHf, NULL, 0, 4},
        {0x0, CONTEXT_ID_UPDATE_MASKf, NULL, 0, 0xFFFF},
        {0x0, STAGE_HFE_PROF_PTR_VALIDf, NULL, 0, 1}
    };

    stage_entry = &entry->stage_entry[APPL_SIGN_PART_S2];
    stage_entry->stage_id = BCMI_SWITCH_PARSER2_STAGE2;

    rv = bcmi_switch_parser_stage_entry_hw_get(unit, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_flex_fields_set(unit,
            entry->flags, COUNTOF(flex_fields), flex_fields, stage_entry);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_parser_stage_entry_free(unit, stage_entry);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser2_stage3_entry_init
 * Purpose:
 *      Initialize default parser stage entry for
 *      parser 2 stage 3.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN/OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser2_stage3_entry_init(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry = NULL;
    bcmi_switch_appl_sign_entry_flex_field_t flex_fields[] = {
        {0x3, IN_CONTEXT_IDf, "PARSER2_S3_IN_CONTEXT_ID", 0, 0},
        {0x3, VALID_BYTES_CHECK_VECTORf, "PARSER2_S3_VALID_BYTES_CHECK_VECTOR", 0, 0},
        {0x5, VALID_BYTES_CHECK_VECTOR_MASKf, "PARSER2_S3_VALID_BYTES_CHECK_VECTOR_MASK", 0, 0},
        {0x5, IN_CONTEXT_ID_MASKf, "PARSER2_S3_IN_CONTEXT_ID_MASK", 0, 0},
        {0x4, HDR_TYPE_MASKf, NULL, 0, 0xFFFF},
        {0x0, CONTEXT_ID_UPDATE_MASKf, NULL, 0, 0x7F00},
        {0x0, VALID_BYTES_NEEDEDf, NULL, 0, 16},
        {0x0, ACTION_IF_BYTES_ENOUGHf, NULL, 0, 1},
        {0x0, ACTION_IF_BYTES_NOT_ENOUGHf, NULL, 0, 4},
        {0x0, BASE_SHIFT_AMOUNTf, NULL, 0, 8}
    };

    stage_entry = &entry->stage_entry[APPL_SIGN_PART_S3];
    stage_entry->stage_id = BCMI_SWITCH_PARSER2_STAGE3;

    rv = bcmi_switch_parser_stage_entry_hw_get(unit, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_flex_fields_set(unit,
            entry->flags, COUNTOF(flex_fields), flex_fields, stage_entry);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_parser_stage_entry_free(unit, stage_entry);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser2_stage4_entry_init
 * Purpose:
 *      Initialize default parser stage entry for
 *      parser 2 stage 4.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN/OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser2_stage4_entry_init(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry = NULL;
    bcmi_switch_appl_sign_entry_flex_field_t flex_fields[] = {
        {0x3, VALID_BYTES_CHECK_VECTORf,  "PARSER2_S4_VALID_BYTES_CHECK_VECTOR", 0, 0},
        {0x5, VALID_BYTES_CHECK_VECTOR_MASKf,   "PARSER2_S4_VALID_BYTES_CHECK_VECTOR_MASK", 0, 0},
        {0x4, IN_CONTEXT_ID_MASKf, NULL, 0, 0x7f00},
        {0x0, VALID_BYTES_NEEDEDf, NULL, 0, 16},
        {0x0, ACTION_IF_BYTES_ENOUGHf, NULL, 0, 1},
        {0x0, ACTION_IF_BYTES_NOT_ENOUGHf, NULL, 0, 4},
        {0x0, CONTEXT_ID_UPDATE_MASKf, NULL, 0, 0xFFFF}
    };

    stage_entry = &entry->stage_entry[APPL_SIGN_PART_S4];
    stage_entry->stage_id = BCMI_SWITCH_PARSER2_STAGE4;

    rv = bcmi_switch_parser_stage_entry_hw_get(unit, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_flex_fields_set(unit,
            entry->flags, COUNTOF(flex_fields), flex_fields, stage_entry);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_parser_stage_entry_free(unit, stage_entry);
    }

    return rv;
}

/*
 * Function:
 *      appl_parser_stage_entry_key_cmp
 * Purpose:
 *      Compare stage entries.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      stage_entry1- (IN) first parser stage entry
 *      stage_entry2- (IN) second parser stage entry
 * Returns:
 *      TRUE, if match other FALSE
 * Notes:
 */
STATIC int
appl_parser_stage_entry_key_cmp(
    int unit,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry1,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry2)
{
    int word = 0;
    uint32 key1_word, key2_word;

    if (stage_entry1->stage_id != stage_entry2->stage_id) {
        return FALSE;
    }

    for (word = 0; word < stage_entry1->key_size; word++) {
        key1_word = stage_entry1->hw_key_buf[word] &
            stage_entry1->hw_mask_buf[word];
        key2_word = stage_entry2->hw_key_buf[word] &
            stage_entry2->hw_mask_buf[word];

        if (key1_word != key2_word) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_context_get
 * Purpose:
 *      Get internal context Id for the parser stage entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry_flags - (IN) Entry flags
 *      stage_entry - (IN) parser stage entry
 *      context_id  - (OUT) Context Id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_context_get(
    int unit,
    uint32 entry_flags,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry,
    uint32 *context_id)
{
    int idx = 0, max_entries = 0;
    uint8 *ref_list = NULL;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;
    bcmi_switch_appl_sign_parser_stage_entry_t *temp_stage_entry = NULL;

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    BCMI_SWITCH_APPL_SIGN_CONTEXT_ID_REF_LIST(unit, entry_flags, ref_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            continue;
        }

        temp_stage_entry = &entry_list[idx]->stage_entry[APPL_SIGN_PART_S3];
        if (appl_parser_stage_entry_key_cmp(unit,
                stage_entry, temp_stage_entry) == TRUE) {
            break;
        }
    }
    if (idx < max_entries) {
        *context_id = entry_list[idx]->context_id;
        return BCM_E_NONE;
    }

    /* Get unique context Id */
    for (idx = 0; idx < max_entries; idx++) {
        if (ref_list[idx] == 0) {
            *context_id = idx + 1;
            break;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_entry_install
 * Purpose:
 *      Install parser stage entry to h/w
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      hw_index    - (IN) H/w index
 *      stage_entry - (IN) parser stage entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_entry_install(
    int unit,
    int hw_index,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry)
{
    int rv = BCM_E_NONE;
    uint32 valid;
    uint32 entry_buf[20];
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t data_mem = INVALIDm;

    /* Get TCAM/Data memory */
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_entry->stage_id, &tcam_mem, &data_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Get Valid */
    valid = soc_mem_field_length(unit, tcam_mem, VALIDf);
    valid = (1 << valid) - 1;

    /* Write to Data memory */
    sal_memset(entry_buf, 0, sizeof(entry_buf));

    soc_mem_field_set(unit, data_mem, entry_buf,
            DATAf, stage_entry->hw_data_buf);
    rv = soc_mem_write(unit, data_mem, MEM_BLOCK_ALL,
            hw_index, (void *) &entry_buf[0]);
    BCM_IF_ERROR_RETURN(rv);

    /* Write to Tcam memory */
    sal_memset(entry_buf, 0, sizeof(entry_buf));

    soc_mem_field_set(unit, tcam_mem, entry_buf,
            KEYf, stage_entry->hw_key_buf);
    soc_mem_field_set(unit, tcam_mem, entry_buf,
            MASKf, stage_entry->hw_mask_buf);
    soc_mem_field32_set(unit, tcam_mem, entry_buf,
            VALIDf, valid);

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
            hw_index, (void *) &entry_buf[0]);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_entry_remove
 * Purpose:
 *      Remove  parser stage entry from h/w
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      hw_index    - (IN) H/w index
 *      stage_entry - (IN) parser stage entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_entry_remove(
    int unit,
    int hw_index,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry)
{
    int rv = BCM_E_NONE;
    uint32 entry_buf[20];
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t data_mem = INVALIDm;

    /* Get TCAM/Data memory */
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_entry->stage_id, &tcam_mem, &data_mem);
    BCM_IF_ERROR_RETURN(rv);

    /* Write to Data memory */
    sal_memset(entry_buf, 0, sizeof(entry_buf));

    rv = soc_mem_write(unit, data_mem, MEM_BLOCK_ALL,
            hw_index, (void *) &entry_buf[0]);
    BCM_IF_ERROR_RETURN(rv);

    /* Write to Tcam memory */
    sal_memset(entry_buf, 0, sizeof(entry_buf));
    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
            hw_index, (void *) &entry_buf[0]);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_entry_move
 * Purpose:
 *      Move parser stage entry from old to new index
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      old_hw_index- (IN) Old h/w index
 *      new_hw_index    - (IN) New H/w index
 *      stage_entry - (IN) parser stage entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_entry_move(
    int unit,
    int old_hw_index,
    int new_hw_index,
    bcmi_switch_appl_sign_parser_stage_entry_t *stage_entry)
{
    int rv = BCM_E_NONE;

    rv = bcmi_switch_parser_stage_entry_install(unit,
            new_hw_index, stage_entry);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_parser_stage_entry_remove(unit,
            old_hw_index, stage_entry);
    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_exists
 * Purpose:
 *      Check if application sign entry exists
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      options     - (IN) Options
 *      appl_sign_id- (IN) Application sign Id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_exists(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_sign_id)
{
    int idx = 0, max_entries = 0;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            continue;
        }

        if (entry_list[idx]->eid == appl_sign_id) {
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_get
 * Purpose:
 *      Get pointer to application sign entry if exists
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      options     - (IN) Options
 *      appl_sign_id- (IN) Application sign Id
 *      entry       - (OUT) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_get(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_sign_id,
    bcmi_switch_appl_sign_entry_t **entry)
{
    int idx = 0, max_entries = 0;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            continue;
        }

        if (entry_list[idx]->eid == appl_sign_id) {
            *entry = entry_list[idx];
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_idx_assign
 * Purpose:
 *      Assign new application sign Id
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      options     - (IN) Options
 *      appl_sign_id- (OUT) Application sign Id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_idx_assign(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t *appl_sign_id)
{
    int rv = BCM_E_NONE;
    int free_eid_found = FALSE;
    bcm_switch_appl_signature_t start_eid;
    bcm_switch_appl_signature_t *next_alloc_eid;

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        next_alloc_eid = &APPL_SIGN_CTRL(unit)->tnl_next_alloc_eid;
    } else  {
        next_alloc_eid = &APPL_SIGN_CTRL(unit)->next_alloc_eid;
    }

    start_eid = *next_alloc_eid;
    do {
        if (*next_alloc_eid > BCMI_SWITCH_APPL_SIGN_ID_MAX) {
            *next_alloc_eid = BCMI_SWITCH_APPL_SIGN_ID_BASE;
        }

        rv = bcmi_switch_appl_sign_entry_exists(unit,
                options, *next_alloc_eid);
        if (rv == BCM_E_NOT_FOUND) {
            free_eid_found = TRUE;
            break;
        }
        *next_alloc_eid += 1;
    } while (*next_alloc_eid != start_eid);


    if (free_eid_found == TRUE) {
        *appl_sign_id = *next_alloc_eid;
        *next_alloc_eid += 1;
        return BCM_E_NONE;
    }

    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_idx_free
 * Purpose:
 *      Free newly assigned application sign Id
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      options     - (IN) Options
 *      appl_sign_id- (IN) Application sign Id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_idx_free(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_sign_id)
{
    bcm_switch_appl_signature_t *next_alloc_eid;

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        next_alloc_eid = &APPL_SIGN_CTRL(unit)->tnl_next_alloc_eid;
    } else  {
        next_alloc_eid = &APPL_SIGN_CTRL(unit)->next_alloc_eid;
    }

    if (*next_alloc_eid == BCMI_SWITCH_APPL_SIGN_ID_BASE) {
        *next_alloc_eid = BCMI_SWITCH_APPL_SIGN_ID_MAX;
    } else {
        *next_alloc_eid -= 1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_l4protocol_port_set
 * Purpose:
 *      Set L4 protocol port to the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      l4protocol_port- (IN) L4 Protocol port
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_l4protocol_port_set(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint16 l4protocol_port)
{
    int rv = BCM_E_NONE;
    int offset = 0;
    uint32 data = 0;
    soc_mem_t tcam_mem;
    soc_field_info_t *fieldinfo1 = NULL;
    soc_field_info_t *fieldinfo2 = NULL;

    data = l4protocol_port;

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER2_STAGE3, &tcam_mem, NULL);
    } else {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE3, &tcam_mem, NULL);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Writing to Parser stage 3 */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, HDR_TYPEf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_set(unit, offset, 16,
            &data, entry->stage_entry[APPL_SIGN_PART_S3].hw_key_buf);
    BCM_IF_ERROR_RETURN(rv);

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE) {
        if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
            rv = bcmi_switch_parser_stage_mem_get(unit,
                    BCMI_SWITCH_PARSER2_STAGE2, &tcam_mem, NULL);
        } else {
            rv = bcmi_switch_parser_stage_mem_get(unit,
                    BCMI_SWITCH_PARSER1_STAGE2, &tcam_mem, NULL);
        }
        BCM_IF_ERROR_RETURN(rv);

        /* Writing to Parser stage 2 Key */
        fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
        fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, PKT_DATAf);
        if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
            return BCM_E_INTERNAL;
        }
        /* PKT_DATA is 48b field but only upper 16b are used */
        offset = fieldinfo2->bp - fieldinfo1->bp + 32;
        rv = bcmi_switch_entry_field_value_set(unit, offset, 16,
                &data, entry->stage_entry[APPL_SIGN_PART_S2].hw_key_buf);
        BCM_IF_ERROR_RETURN(rv);

        /* Writing to Parser stage 2 Mask */
        fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, MASKf);
        fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, PKT_DATA_MASKf);
        if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
            return BCM_E_INTERNAL;
        }
        data = 0xFFFF;
        /* PKT_DATA is 48b field but only upper 16b are used */
        offset = fieldinfo2->bp - fieldinfo1->bp + 32;
        rv = bcmi_switch_entry_field_value_set(unit, offset, 16,
                &data, entry->stage_entry[APPL_SIGN_PART_S2].hw_mask_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_l4protocol_port_get
 * Purpose:
 *      Get L4 protocol port from the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      l4protocol_port- (OUT) L4 Protocol port
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_l4protocol_port_get(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint16 *l4protocol_port)
{
    int rv = BCM_E_NONE;
    int offset = 0;
    uint32 data = 0;
    soc_mem_t tcam_mem;
    soc_field_info_t *fieldinfo1 = NULL;
    soc_field_info_t *fieldinfo2 = NULL;

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER2_STAGE3, &tcam_mem, NULL);
    } else {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE3, &tcam_mem, NULL);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Reading from Parser stage 3 */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, HDR_TYPEf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_get(unit, offset, 16,
                entry->stage_entry[APPL_SIGN_PART_S3].hw_key_buf, &data);

    *l4protocol_port = data;

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_l4payload_set
 * Purpose:
 *      Set L4 Payload/mask to the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      l4payload   - (IN) L4 payload data
 *      l4payoad_mask- (IN) L4 payload mask
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_l4payload_set(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint8 *l4payload,
    uint8 *l4payload_mask)
{
    int rv = BCM_E_NONE;
    int i = 0, offset = 0;
    soc_mem_t tcam_mem, tcam_mem1;
    uint32 data[8], mask[8];
    soc_field_info_t *fieldinfo1 = NULL;
    soc_field_info_t *fieldinfo2 = NULL;

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        rv = bcmi_switch_parser_stage_mem_get(unit,
               BCMI_SWITCH_PARSER2_STAGE3, &tcam_mem, NULL);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER2_STAGE4, &tcam_mem1, NULL);
    } else {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE3, &tcam_mem, NULL);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE4, &tcam_mem1, NULL);
    }
    BCM_IF_ERROR_RETURN(rv);

    for (i = 7; i >= 0; i--) {
        _SHR_UNPACK_U32(l4payload, data[i]);
        _SHR_UNPACK_U32(l4payload_mask, mask[i]);

        data[i] = data[i] & mask[i];
    }

    /* Writing to Parser stage 3 (First 16 Bytes) */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, PKT_DATAf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    /* PKT_DATA is 136b field and upper 128b are used */
    offset = fieldinfo2->bp - fieldinfo1->bp + 8;
    rv = bcmi_switch_entry_field_value_set(unit, offset, 128,
                &data[4], entry->stage_entry[APPL_SIGN_PART_S3].hw_key_buf);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_entry_field_value_set(unit, offset, 128,
                &mask[4], entry->stage_entry[APPL_SIGN_PART_S3].hw_mask_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Writing to Parser stage 4 (Last 16 bytes) */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem1, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem1, PKT_DATAf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    /* PKT_DATA is 136b field and upper 128b are used */
    offset = fieldinfo2->bp - fieldinfo1->bp + 8;
    rv = bcmi_switch_entry_field_value_set(unit, offset, 128,
                &data[0], entry->stage_entry[APPL_SIGN_PART_S4].hw_key_buf);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_entry_field_value_set(unit, offset, 128,
                &mask[0], entry->stage_entry[APPL_SIGN_PART_S4].hw_mask_buf);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_l4payload_get
 * Purpose:
 *      Get L4 Payload/mask from the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      l4payload   - (OUT) L4 payload data
 *      l4payoad_mask- (OUT) L4 payload mask
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_l4payload_get(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint8 *l4payload,
    uint8 *l4payload_mask)
{
    int rv = BCM_E_NONE;
    int i = 0, offset = 0;
    soc_mem_t tcam_mem, tcam_mem1;
    uint32 data[8], mask[8];
    soc_field_info_t *fieldinfo1 = NULL;
    soc_field_info_t *fieldinfo2 = NULL;

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        rv = bcmi_switch_parser_stage_mem_get(unit,
               BCMI_SWITCH_PARSER2_STAGE3, &tcam_mem, NULL);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER2_STAGE4, &tcam_mem1, NULL);
    } else {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE3, &tcam_mem, NULL);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE4, &tcam_mem1, NULL);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Reading from Parser stage 3 (First 16 Bytes) */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, PKT_DATAf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    offset = fieldinfo2->bp - fieldinfo1->bp + 8;
    rv = bcmi_switch_entry_field_value_get(unit, offset, 128,
                entry->stage_entry[APPL_SIGN_PART_S3].hw_key_buf, &data[4]);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_entry_field_value_get(unit, offset, 128,
                entry->stage_entry[APPL_SIGN_PART_S3].hw_mask_buf, &mask[4]);
    BCM_IF_ERROR_RETURN(rv);

    /* Reading from Parser stage 4 (Last 16 bytes) */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem1, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem1, PKT_DATAf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    offset = fieldinfo2->bp - fieldinfo1->bp + 8;
    rv = bcmi_switch_entry_field_value_get(unit, offset, 128,
                entry->stage_entry[APPL_SIGN_PART_S4].hw_key_buf, &data[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_switch_entry_field_value_get(unit, offset, 128,
                entry->stage_entry[APPL_SIGN_PART_S4].hw_mask_buf, &mask[0]);

    for (i = 7; i >= 0; i--) {
        _SHR_PACK_U32(l4payload, data[i]);
        _SHR_PACK_U32(l4payload_mask, mask[i]);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_context_id_set
 * Purpose:
 *      Set context Id to the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_context_id_set(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    int offset = 0;
    uint8 *ref_list = NULL;
    uint32 context_id = 0;
    uint32 data = 0;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t data_mem = INVALIDm;
    soc_field_info_t *fieldinfo1 = NULL;
    soc_field_info_t *fieldinfo2 = NULL;

    rv = bcmi_switch_parser_stage_context_get(unit, entry->flags,
                &entry->stage_entry[APPL_SIGN_PART_S3], &context_id);
    BCM_IF_ERROR_RETURN(rv);

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER2_STAGE3, NULL, &data_mem);
    } else {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE3, NULL, &data_mem);
    }
    BCM_IF_ERROR_RETURN(rv);

    data = (((1 << 6) + context_id) << 8);

    /* Writing to Parser stage 3 data */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, data_mem, DATAf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, data_mem, CONTEXT_IDf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_set(unit, offset, 16,
            &data, entry->stage_entry[APPL_SIGN_PART_S3].hw_data_buf);
    BCM_IF_ERROR_RETURN(rv);

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER2_STAGE4, &tcam_mem, &data_mem);
    } else {
        rv = bcmi_switch_parser_stage_mem_get(unit,
                BCMI_SWITCH_PARSER1_STAGE4, &tcam_mem, &data_mem);
    }
    BCM_IF_ERROR_RETURN(rv);

    /* Writing to Parser stage 4 key */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem, KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem, IN_CONTEXT_IDf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }

    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_set(unit, offset, 16,
            &data, entry->stage_entry[APPL_SIGN_PART_S4].hw_key_buf);

    data = (((1 << 6) + entry->eid) << 8);

    /* Writing to Parser stage 4 data */
    fieldinfo1 = soc_mem_fieldinfo_get(unit, data_mem, DATAf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, data_mem, CONTEXT_IDf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        return BCM_E_INTERNAL;
    }
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_set(unit, offset, 16,
            &data, entry->stage_entry[APPL_SIGN_PART_S4].hw_data_buf);

    if (BCM_SUCCESS(rv)) {
        entry->context_id = context_id;

        /* Increment ref */
        BCMI_SWITCH_APPL_SIGN_CONTEXT_ID_REF_LIST(unit, entry->flags, ref_list);
        ref_list[context_id - 1] += 1;
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_unique_check
 * Purpose:
 *      Checks the uniqueness of application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      match_appl_sign_id - (OUT) Matched Application signature
 *                         id if match found
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_unique_check(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    bcm_switch_appl_signature_t *match_appl_sign_id)
{
    int match = FALSE;
    int idx = 0, max_entries = 0, part = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry->flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            continue;
        }

        /* Number of parts must match */
        if (entry->parts != entry_list[idx]->parts) {
            continue;
        }

        /* Key/Mask in each stage must match */
        for (part = 0; part < entry->parts; part++) {
            if (appl_parser_stage_entry_key_cmp(unit,
                        &entry->stage_entry[part],
                        &entry_list[idx]->stage_entry[part]) == FALSE) {
                break;
            }
        }
        if (part == entry->parts) {
            match = TRUE;
            break;
        }
    }

    /* Match not found */
    if (match == FALSE) {
        return BCM_E_NONE;
    }

    /* Match found, return BCM_E_EXISTS */
    *match_appl_sign_id = entry_list[idx]->eid;
    return BCM_E_EXISTS;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_info_set
 * Purpose:
 *      Set Application signature info to the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      appl_sign_info- (IN) Application sign info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_info_set(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    bcm_switch_appl_signature_info_t *appl_sign_info)
{
    int rv = BCM_E_NONE;

    /* L4 protocol port */
    rv = bcmi_switch_appl_sign_l4protocol_port_set(unit,
            entry, appl_sign_info->l4protocol_port);
    BCM_IF_ERROR_RETURN(rv);

    /* L4 Payload */
    rv = bcmi_switch_appl_sign_l4payload_set(unit,
            entry, appl_sign_info->l4_payload,
            appl_sign_info->l4_payload_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Context Id */
    return bcmi_switch_appl_sign_context_id_set(unit, entry);
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_info_get
 * Purpose:
 *      Get Application signature info from the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      appl_sign_info- (OUT) Application sign info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_info_get(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    bcm_switch_appl_signature_info_t *appl_sign_info)
{
    int rv = BCM_E_NONE;

    /* L4 protocol port */
    rv = bcmi_switch_appl_sign_l4protocol_port_get(unit,
            entry, &appl_sign_info->l4protocol_port);
    BCM_IF_ERROR_RETURN(rv);

    /* L4 payload */
    rv = bcmi_switch_appl_sign_l4payload_get(unit,
            entry, appl_sign_info->l4_payload,
            appl_sign_info->l4_payload_mask);

    appl_sign_info->priority = entry->prio;
    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_UDP) {
        appl_sign_info->flags |= BCM_SWITCH_APPL_SIGNATURE_TYPE_UDP;
    } else {
        appl_sign_info->flags |= BCM_SWITCH_APPL_SIGNATURE_TYPE_TCP;
    }
    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE) {
        appl_sign_info->flags |= BCM_SWITCH_APPL_SIGNATURE_TYPE_RESPONSE;
    }

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_free
 * Purpose:
 *      Free application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_free(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int part = 0;
    uint8 *ref_list = NULL;

    for (part = 0; part < entry->parts; part++) {
        bcmi_switch_parser_stage_entry_free(unit,
                &entry->stage_entry[part]);
    }
    APPL_SIGN_FREE(entry->stage_entry);

    if (entry->context_id != 0) {
        BCMI_SWITCH_APPL_SIGN_CONTEXT_ID_REF_LIST(unit, entry->flags, ref_list);
        ref_list[entry->context_id - 1] -= 1;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_set
 * Purpose:
 *      Set application application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      appl_sign_info- (IN) Application sign info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_set(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    bcm_switch_appl_signature_info_t *appl_sign_info)
{
    int rv = BCM_E_NONE;

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE) {
            rv = bcmi_switch_parser2_stage2_entry_init(unit, entry);
            BCM_IF_ERROR_RETURN(rv);
        }

        rv = bcmi_switch_parser2_stage3_entry_init(unit, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcmi_switch_parser2_stage4_entry_init(unit, entry);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE) {
            rv = bcmi_switch_parser1_stage2_entry_init(unit, entry);
            BCM_IF_ERROR_RETURN(rv);
        }

        rv = bcmi_switch_parser1_stage3_entry_init(unit, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = bcmi_switch_parser1_stage4_entry_init(unit, entry);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* User fields */
    rv = bcmi_switch_appl_sign_entry_info_set(unit, entry, appl_sign_info);

    if (BCM_FAILURE(rv)) {
        bcmi_switch_appl_sign_entry_free(unit, entry);
    }

    return rv;
}

/*
 * Function:
 *      appl_sign_entry_prio_cmp
 * Purpose:
 *      Compare application sign entry priorities
 * Parameters:
 *      prio1       - (IN) First priority
 *      prio2       - (IN) Second priority
 * Returns:
 *      0 for equal, -1 if first is smaller, otherwise 1
 * Notes:
 */
STATIC int
appl_sign_entry_prio_cmp(uint32 prio1, uint32 prio2)
{
    int diff;

    if (prio1 == prio2) {
        diff = 0;
    } else if (prio1 < prio2) {
        diff = -1;
    } else {
        diff = 1;
    }
    return diff;
}

/*
 * Function:
 *      appl_sign_entry_prio_move_req
 * Purpose:
 *      Check if movement of application sign entry is required
 *      due to priority change.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      prio        - (IN) New priority
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
appl_sign_entry_prio_move_req(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint32 prio)
{
    int idx = 0;
    int max_entries = 0;
    int prio_diff = 0, flags = -1;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry->flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == entry) {
            flags = 1;
            continue;
        }
        if (entry_list[idx] == NULL) {
            continue;
        }
        prio_diff = appl_sign_entry_prio_cmp(entry_list[idx]->prio, prio);
        if (flags == -1) {
            if (prio_diff < 0) {
                return TRUE;
            }
        } else {
            if (prio_diff > 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_move
 * Purpose:
 *      Move application sign entry to new h/w index
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      new_hw_index- (IN) New hw index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_move(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    int new_hw_index,
    int flag)
{
    int rv = BCM_E_NONE;
    int part = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    if (entry->hw_index == new_hw_index) {
        return BCM_E_NONE;
    }
    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry->flags, entry_list);

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED) {
        for (part = 0; part < entry->parts; part++) {
            rv = bcmi_switch_parser_stage_entry_move(unit,
                    entry->hw_index, new_hw_index,
                    &entry->stage_entry[part]);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (flag == FALSE) {
        entry_list[entry->hw_index] = NULL;
    }
    entry->hw_index = new_hw_index;
    entry_list[new_hw_index] = entry;

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_shift_down
 * Purpose:
 *      Move batch of application sign entry down
 *      in the h/w table.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry_list  - (IN) Application sign entry list
 *      target_index- (IN) h/w index to be freed
 *      next_null_index- (IN) Next Null index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_shift_down(
    int unit,
    bcmi_switch_appl_sign_entry_t **entry_list,
    int target_index,
    int next_null_index)
{
    int free_index = 0;
    int rv = BCM_E_NONE;

    free_index = next_null_index;
    while(free_index > target_index) {
        rv = bcmi_switch_appl_sign_entry_move(unit,
                entry_list[free_index - 1], free_index, 0);
        BCM_IF_ERROR_RETURN(rv);

        free_index--;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_shift_up
 * Purpose:
 *      Move batch of application sign entries up
 *      in the h/w table.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry_list  - (IN) Application sign entry list
 *      target_index- (IN) h/w index to be freed
 *      prev_null_index- (IN) Previous Null index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_shift_up(
    int unit,
    bcmi_switch_appl_sign_entry_t **entry_list,
    int target_index,
    int prev_null_index)
{
    int free_index = 0;
    int rv = BCM_E_NONE;

    free_index = prev_null_index;
    while (free_index < target_index) {
        rv = bcmi_switch_appl_sign_entry_move(unit,
                entry_list[free_index + 1], free_index, 0);
        BCM_IF_ERROR_RETURN(rv);

        free_index++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      appl_sign_no_free_entries
 * Purpose:
 *      Check if there is free entries in the list.
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry_list  - (IN) Application sign entry list
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
appl_sign_no_free_entries(
    int unit,
    bcmi_switch_appl_sign_entry_t **entry_list)
{
    int idx;

    for (idx = 0; idx < BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES; idx++) {
        if (entry_list[idx] == NULL) {
            return FALSE;
        }
    }
    return TRUE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_prio_set
 * Purpose:
 *      Set new priority to the application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      prio        - (IN) New priority
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_prio_set(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint32 prio)
{
    int rv = BCM_E_NONE;
    int max_entries = 0, dir = 0;
    int temp_idx = -1, target_idx = -1;
    int prev_null_idx = -1;
    int next_null_idx = -1;
    int first_null_idx = -1;
    int first_null_flag = 0;
    int pivot_found = FALSE;
    int only_move = FALSE;
    int flag_no_free_entries = FALSE;
    int decr_on_shift_up = TRUE;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    /* Check if movement is required */
    if (appl_sign_entry_prio_move_req(unit, entry, prio) == FALSE) {
        entry->prio = prio;
        return BCM_E_NONE;
    }

    /* Get entry list */
    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry->flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    if (appl_sign_no_free_entries(unit, entry_list) == TRUE) {
        if (!(entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED)) {
            entry_list[entry->hw_index] = NULL;
            flag_no_free_entries = TRUE;
        } else {
            return BCM_E_RESOURCE;
        }
    }

    /* Get nearest null index before given priority */
    for (target_idx = 0; target_idx < max_entries; target_idx++) {
        /* Skip Self */
        if (entry == entry_list[target_idx]) {
            continue;
        }
        if (entry_list[target_idx] == NULL) {
            prev_null_idx = target_idx;
            if (first_null_flag == 0) {
                first_null_idx = target_idx;
                first_null_flag = 1;
            }
            continue;
        } else {
            first_null_flag = 0;
        }

        if (appl_sign_entry_prio_cmp(prio,
                    entry_list[target_idx]->prio) > 0) {
            pivot_found = TRUE;
            break;
        }
    }
    /* Get nearest null index after given priority */
    for (temp_idx = target_idx; temp_idx < max_entries; temp_idx++) {
        if (entry_list[temp_idx] == NULL) {
            next_null_idx = temp_idx;
            break;
        }
    }

    /* Put back the entry */
    if (flag_no_free_entries == TRUE) {
        entry_list[entry->hw_index] = entry;
    }

    /* Find correct target index */
    if (pivot_found == FALSE) {
        if ((prev_null_idx != -1) &&
                (prev_null_idx == (max_entries - 1)) &&
                (first_null_flag == 1)) {
            target_idx = first_null_idx;
            only_move = TRUE;
        } else {
            target_idx = (max_entries - 1);
            decr_on_shift_up = FALSE;
        }
    }

    if (only_move == FALSE) {
        /* Get smallest move direction */
        if (prev_null_idx != -1) {
            if (next_null_idx != -1) {
                if ((target_idx - prev_null_idx) <
                        (next_null_idx - target_idx)) {
                    dir = -1; /* Shift Up */
                } else {
                    dir = 1; /* Shift Down */
                }
            } else {
                dir = -1; /* Shift Up */
            }
        } else {
            if (next_null_idx != -1) {
                dir = 1; /* Shift Down */
            } else {
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                                "No free entries for movement\n")));
                return BCM_E_CONFIG; /* No free entry to move */
            }
        }

        if (dir == 1) {
            if (entry_list[target_idx] != NULL) {
                rv = bcmi_switch_appl_sign_entry_shift_down(unit,
                        entry_list, target_idx, next_null_idx);
            }
        } else {
            if (decr_on_shift_up == TRUE) {
                target_idx--;
            }
            if (entry_list[target_idx] != NULL) {
                rv = bcmi_switch_appl_sign_entry_shift_up(unit,
                        entry_list, target_idx, prev_null_idx);
            }
        }
        BCM_IF_ERROR_RETURN(rv);
    }
    rv = bcmi_switch_appl_sign_entry_move(unit, entry,
            target_idx, flag_no_free_entries);

    entry->prio = prio;

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_install
 * Purpose:
 *      Install application sign entry to h/w
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 *      prio        - (IN) New priority
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_install(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry,
    uint32 prio)
{
    int rv = BCM_E_NONE;
    int idx = 0, max_entries = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry->flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    /* Assign a hw_index */
    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            entry->hw_index = idx;
            break;
        }
    }

    if(idx == max_entries) {
        return BCM_E_RESOURCE;
    }

    /* Reserve the index */
    entry_list[idx] = entry;

    /* Apply priority */
    rv = bcmi_switch_appl_sign_entry_prio_set(unit, entry, prio);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < entry->parts; idx++) {
        rv = bcmi_switch_parser_stage_entry_install(unit,
                    entry->hw_index, &entry->stage_entry[idx]);
        BCM_IF_ERROR_RETURN(rv);
    }

    entry->flags  |= BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED;
    entry_list[entry->hw_index] = entry;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_remove
 * Purpose:
 *      Remove application sign entry from h/w
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_remove(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    int part;

    /* If not installed, return */
    if (!(entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED)) {
        return BCM_E_NONE;
    }

    for (part = 0; part < entry->parts; part++) {

        rv = bcmi_switch_parser_stage_entry_remove(unit,
                    entry->hw_index, &entry->stage_entry[part]);
        BCM_IF_ERROR_RETURN(rv);
    }

    entry->flags &= ~BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_destroy
 * Purpose:
 *      Destroy application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      entry       - (IN) Application sign entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_destroy(
    int unit,
    bcmi_switch_appl_sign_entry_t *entry)
{
    int rv = BCM_E_NONE;
    int hw_index = -1;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    if (entry == NULL) {
        return BCM_E_PARAM;
    }

    /* Remove from hardware */
    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED) {
        bcmi_switch_appl_sign_entry_remove(unit, entry);
        hw_index = entry->hw_index;
    }

    if (hw_index != -1) {
        BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry->flags, entry_list);
        entry_list[hw_index] = NULL;
    }

    rv = bcmi_switch_appl_sign_entry_free(unit, entry);
    BCM_IF_ERROR_RETURN(rv);

    APPL_SIGN_FREE(entry);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_destroy_all
 * Purpose:
 *      Destroy all application sign entries in the system
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      options     - (IN) Options
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_destroy_all(
    int unit,
    uint32 options)
{
    int idx = 0, max_entries = 0;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            continue;
        }

        bcmi_switch_appl_sign_entry_destroy(unit, entry_list[idx]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_create
 * Purpose:
 *      Create application sign entry
 * Parameters:
 *      unit        - (IN) UNIT Number.
 *      appl_sign_id- (IN) Application sign id
 *      appl_sign_info- (IN) Application sign info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_create(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_sign_id,
    bcm_switch_appl_signature_info_t *appl_sign_info,
    bcm_switch_appl_signature_t *match_appl_sign_id)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_entry_t *entry = NULL;

    if (appl_sign_info == NULL) {
        return BCM_E_PARAM;
    }

    APPL_SIGN_ALLOC(entry, sizeof(bcmi_switch_appl_sign_entry_t), "Appl Sign Entry");
    if (entry == NULL) {
        return BCM_E_MEMORY;
    }

    entry->hw_index = -1;
    entry->eid = appl_sign_id;
    entry->parts = 2;

    if (appl_sign_info->flags & BCM_SWITCH_APPL_SIGNATURE_TYPE_UDP) {
        entry->flags |= BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_UDP;
    }
    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        entry->flags |= BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }
    if (appl_sign_info->flags & BCM_SWITCH_APPL_SIGNATURE_TYPE_RESPONSE) {
        entry->flags |= BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE;
        entry->parts = 3;
    }

    /* Allocate memory of parset stage entries */
    APPL_SIGN_ALLOC(entry->stage_entry,
            entry->parts * sizeof (bcmi_switch_appl_sign_parser_stage_entry_t),
            "Parser Stage Entries");
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Set hw key/mask/data for parser stages */
    rv = bcmi_switch_appl_sign_entry_set(unit, entry, appl_sign_info);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Check that this is unique Application sign entry */
    rv = bcmi_switch_appl_sign_entry_unique_check(unit,
            entry, match_appl_sign_id);
    if (rv == BCM_E_EXISTS) {
        bcmi_switch_appl_sign_entry_free(unit, entry);
        return rv;
    }

    /* Apply Priority and install */
    rv = bcmi_switch_appl_sign_entry_install(unit,
            entry, appl_sign_info->priority);
    return rv;

cleanup:

    bcmi_switch_appl_sign_entry_free(unit, entry);
    APPL_SIGN_FREE(entry);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_info_sanity
 * Purpose:
 *      Sanity check for application sign info
 * Parameters:
 *      unit - (IN) Unit number.
 *      appl_sign_info - (IN) Application sign info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_info_sanity(
    int unit,
    bcm_switch_appl_signature_info_t *appl_sign_info)
{
    uint32 flags = 0;

    if (appl_sign_info == NULL) {
        return BCM_E_PARAM;
    }

    /* Check flags */
    flags = appl_sign_info->flags;
    if (flags & BCM_SWITCH_APPL_SIGNATURE_TYPE_TCP) {
        if (flags & BCM_SWITCH_APPL_SIGNATURE_TYPE_UDP) {
            LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                            "Incorrect flags combination\n")));
            return BCM_E_PARAM;
        }
    } else {
        if (!(flags & BCM_SWITCH_APPL_SIGNATURE_TYPE_UDP)) {
            LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                            "Application signature L4 protocol"
                            " not configured\n")));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_signature_init
 * Purpose:
 *      Initialize switch application control.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_init(int unit)
{
    int rv = BCM_E_NONE;

    if (APPL_SIGN_CTRL(unit) != NULL) {
        bcmi_esw_switch_appl_signature_detach(unit);
    }

    /* Create mutex */
    if (appl_sign_mutex[unit] == NULL) {
        appl_sign_mutex[unit] = sal_mutex_create("appl_sign_mutex");
        if (appl_sign_mutex[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    /* Alloc for appl_sign_ctrl */
    APPL_SIGN_ALLOC(APPL_SIGN_CTRL(unit), sizeof (bcmi_switch_appl_sign_control_t),
            "Appl_sign_ctrl");
    if (BCM_FAILURE(rv)) {
        sal_mutex_destroy(appl_sign_mutex[unit]);
        return rv;
    }

    /* Alloc for appl sign entry for outer payload */
    APPL_SIGN_ALLOC(APPL_SIGN_CTRL(unit)->entry_list,
            sizeof(bcmi_switch_appl_sign_entry_t *) * BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES,
            "BCMI_SWITCH_APPL_SIGN_ENTRY_LIST");
    if (BCM_FAILURE(rv)) {
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit));
        sal_mutex_destroy(appl_sign_mutex[unit]);
        return rv;
    }

    /* Alloc for context id ref information */
    APPL_SIGN_ALLOC(APPL_SIGN_CTRL(unit)->context_id_ref,
            sizeof (uint8) * BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES,
            "contxt id ref");
    if (BCM_FAILURE(rv)) {
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->entry_list);
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit));
        sal_mutex_destroy(appl_sign_mutex[unit]);
        return rv;
    }

    /* Alloc for appl sign entry for inner payload */
    APPL_SIGN_ALLOC(APPL_SIGN_CTRL(unit)->tnl_entry_list,
            sizeof(bcmi_switch_appl_sign_entry_t *) * BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES,
            "BCMI_SWITCH_APPL_SIGN_ENTRY_LIST");
    if (BCM_FAILURE(rv)) {
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->entry_list);
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit));
        sal_mutex_destroy(appl_sign_mutex[unit]);
        return BCM_E_MEMORY;
    }

    /* Alloc for context id ref info for inner appl sign */
    APPL_SIGN_ALLOC(APPL_SIGN_CTRL(unit)->tnl_context_id_ref,
            sizeof (uint8) * BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES,
            "tnl contxt id ref");
    if (BCM_FAILURE(rv)) {
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->entry_list);
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->tnl_entry_list);
        APPL_SIGN_FREE(APPL_SIGN_CTRL(unit));
        sal_mutex_destroy(appl_sign_mutex[unit]);
        return rv;
    }

    APPL_SIGN_CTRL(unit)->next_alloc_eid = BCMI_SWITCH_APPL_SIGN_ID_BASE;
    APPL_SIGN_CTRL(unit)->tnl_next_alloc_eid = BCMI_SWITCH_APPL_SIGN_ID_BASE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_signature_detach
 * Purpose:
 *      De-initialize switch application control.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_detach(int unit)
{
    uint32 options = 0;

    /* If not initialized, return */
    if (APPL_SIGN_CTRL(unit) == NULL) {
        return BCM_E_NONE;
    }

    /* Destroy all appl signature entries for outer payload */
    bcmi_switch_appl_sign_entry_destroy_all(unit, options);

    /* Destroy all appl signature entries for inner payload */
    options = BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD;
    bcmi_switch_appl_sign_entry_destroy_all(unit, options);

    APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->entry_list);
    APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->tnl_entry_list);
    APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->context_id_ref);
    APPL_SIGN_FREE(APPL_SIGN_CTRL(unit)->tnl_context_id_ref);
    APPL_SIGN_FREE(APPL_SIGN_CTRL(unit));

    sal_mutex_destroy(appl_sign_mutex[unit]);
    appl_sign_mutex[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_create
 * Purpose:
 *      Create Application signature.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      appl_signature_info - (IN) Application signature Info.
 *      appl_signature  - (IN/OUT) Application signature
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_create(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_info_t *appl_signature_info,
    bcm_switch_appl_signature_t *appl_signature)
{
    int rv = BCM_E_NONE;
    int new_assigned = FALSE;
    bcm_switch_appl_signature_t match_appl_sign_id;

    APPL_SIGN_INIT_CHECK(unit);

    rv = bcmi_switch_appl_sign_info_sanity(unit, appl_signature_info);
    BCM_IF_ERROR_RETURN(rv);

    if (options & BCM_SWITCH_APPL_SIGNATURE_WITH_ID) {

        if ((*appl_signature < BCMI_SWITCH_APPL_SIGN_ID_BASE) ||
            (*appl_signature > BCMI_SWITCH_APPL_SIGN_ID_MAX)) {
            return BCM_E_PARAM;
        }

        /* Check if application signature is already created */
        rv = bcmi_switch_appl_sign_entry_exists(unit,
                        options, *appl_signature);
        if (BCM_SUCCESS(rv)) {
            return BCM_E_EXISTS;
        }
    } else {
        /* Generate Application Signature Id */
        rv = bcmi_switch_appl_sign_idx_assign(unit,
                    options, appl_signature);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        new_assigned = TRUE;
    }

    /* Create/Install Application Signature entry */
    match_appl_sign_id = BCMI_SWITCH_APPL_SIGNATURE_INVALID_ID;
    rv = bcmi_switch_appl_sign_entry_create(unit,
                options, *appl_signature, appl_signature_info,
                &match_appl_sign_id);

    /* Free Application signature if failure */
    if (BCM_FAILURE(rv) && (new_assigned == TRUE)) {
        (void) bcmi_switch_appl_sign_idx_free(unit,
                    options, *appl_signature);
    }

    /* Return back appl sign id if match is found */
    if (BCM_E_EXISTS == rv) {
        *appl_signature = match_appl_sign_id;
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_destroy
 * Purpose:
 *      Destroy Application signature.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      appl_signature  - (IN) Application signature.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_destroy(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_signature)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_entry_t *entry = NULL;

    APPL_SIGN_INIT_CHECK(unit);

    rv = bcmi_switch_appl_sign_entry_get(unit,
                options, appl_signature, &entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = bcmi_switch_appl_sign_entry_destroy(unit, entry);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_get
 * Purpose:
 *      Get Application signature Info.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      appl_signature  - (IN) Application signature
 *      appl_signature_info - (OUT) Application signature Info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_get(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_signature,
    bcm_switch_appl_signature_info_t *appl_signature_info)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_entry_t *entry = NULL;

    APPL_SIGN_INIT_CHECK(unit);

    if (appl_signature_info == NULL) {
        return BCM_E_PARAM;
    }

    rv = bcmi_switch_appl_sign_entry_get(unit,
                options, appl_signature, &entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = bcmi_switch_appl_sign_entry_info_get(unit,
            entry, appl_signature_info);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_get_all
 * Purpose:
 *      Get all application signature configured in the system.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      size            - (IN) Size of 'appl_signature_arr'
 *      appl_signature_arr-(OUT) Array of Application signatures
 *      actual_count    - (OUT) Valid entries in 'appl_signature_arr'
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_get_all(
    int unit,
    uint32 options,
    int size,
    bcm_switch_appl_signature_t *appl_signature_arr,
    int *actual_count)
{
    int idx = 0, c = 0;
    int max_entries = 0;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    APPL_SIGN_INIT_CHECK(unit);

    /* Verify input params */
    if ((size == 0) && (actual_count == NULL)) {
        return BCM_E_PARAM;
    }
    if ((size != 0) && (appl_signature_arr == NULL)) {
        return BCM_E_PARAM;
    }

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            continue;
        }

        if (appl_signature_arr != NULL) {
            appl_signature_arr[c] = entry_list[idx]->eid;
        }
        c++;
        if ((size != 0) && (c >= size)) {
            break;
        }
    }

    if (actual_count != NULL) {
        *actual_count = c;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_prio_set
 * Purpose:
 *      Set the priority of the application signature.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      appl_signature  - (IN) Application signature Id
 *      prio            - (IN) New Priority
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_prio_set(int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_signature,
    uint32 prio)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_entry_t *entry = NULL;

    APPL_SIGN_INIT_CHECK(unit);

    /* Get Appl sign entry */
    rv = bcmi_switch_appl_sign_entry_get(unit,
            options, appl_signature, &entry);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    rv = bcmi_switch_appl_sign_entry_prio_set(unit, entry, prio);
    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_show
 * Purpose:
 *      Show application signature entry
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      appl_signature  - (IN) Application signature Id
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_show(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_signature)
{
    int rv = BCM_E_NONE;
    bcmi_switch_appl_sign_entry_t *entry = NULL;
    bcm_switch_appl_signature_info_t appl_sign_info;
    char *str1[] = {"Outer Payload", "Inner Payload"};
    char *str2[] = {"Request", "Reponse"};
    char *str3[] = {"TCP", "UDP"};
    int i1 = 0, i2 = 0, i3 = 0;

    if (options == BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        i1 = 1;
    }

    /* Get Appl sign entry */
    rv = bcmi_switch_appl_sign_entry_get(unit,
            options, appl_signature, &entry);
    if (BCM_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit, "Application Signature Id: %d (%s)"
                        " Not Found\n"), appl_signature, str1[i1]));
        return rv;
    }

    bcm_switch_appl_signature_info_t_init(&appl_sign_info);
    rv = bcmi_switch_appl_sign_entry_info_get(unit, entry, &appl_sign_info);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE) {
        i2 = 1;
    }
    if (entry->flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_UDP) {
        i3 = 1;
    }

    LOG_CLI((BSL_META_U(unit, "Application Signature Id: %d (%s)\n"),
                appl_signature, str1[i1]));
    LOG_CLI((BSL_META_U(unit, "     H/W Index: %d, Prio: %d, Context: %d"),
                entry->hw_index, appl_sign_info.priority, entry->context_id));
    LOG_CLI((BSL_META_U(unit, " - [ %s, %s ]\n"), str2[i2], str3[i3]));
    LOG_CLI((BSL_META_U(unit, "     L4 Protocol Port     :  %d (0x%x)\n"),
                appl_sign_info.l4protocol_port, appl_sign_info.l4protocol_port));
    LOG_CLI((BSL_META_U(unit, "     L4 Payload      (hex): ")));
    for (i1 = 0; i1 < BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN; i1++) {
        LOG_CLI((BSL_META_U(unit, " %02x"), appl_sign_info.l4_payload[i1]));
        if (i1 == (BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN/2 - 1)) {
            LOG_CLI((BSL_META_U(unit, "    ")));
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n     L4 Payload Mask (hex): ")));
    for (i1 = 0; i1 < BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN; i1++) {
        LOG_CLI((BSL_META_U(unit, " %02x"), appl_sign_info.l4_payload_mask[i1]));
        if (i1 == (BCM_SWITCH_APPL_SIGNATURE_L4_PAYLOAD_MATCH_LEN/2 - 1)) {
            LOG_CLI((BSL_META_U(unit, "    ")));
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n\n")));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_show
 * Purpose:
 *      Show application signature entry
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      options         - (IN) Options
 *      appl_signature  - (IN) Application signature Id
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_show(
    int unit,
    uint32 options,
    bcm_switch_appl_signature_t appl_signature)
{
    int idx = 0, max_entries = 0;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    APPL_SIGN_INIT_CHECK(unit);

    if (options & BCM_SWITCH_APPL_SIGNATURE_INNER_PAYLOAD) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    if (appl_signature != BCMI_SWITCH_APPL_SIGNATURE_INVALID_ID) {
        bcmi_switch_appl_sign_entry_show(unit,
                options, appl_signature);
    } else {
        for (idx = 0; idx < max_entries; idx++) {
            if (entry_list[idx] != NULL) {
                bcmi_switch_appl_sign_entry_show(unit,
                        options, entry_list[idx]->eid);
            }
        }
    }

    return BCM_E_NONE;
}

#if defined(BCM_WARM_BOOT_SUPPORT)

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_wb_alloc
 * Purpose:
 *      Alloc scache for Application signatures.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
int bcmi_esw_switch_appl_signature_wb_alloc(int unit)
{
    int rv;
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    int req_scache_size = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCAHCE_PART_APPL_SIGN);

    /* Priority of application signatures */
    req_scache_size += (BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES * sizeof(uint32));
    req_scache_size += (BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES * sizeof(uint32));

    /* Allocate scache */
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
            TRUE, req_scache_size, &scache_ptr,
            BCM_SWITCH_WB_APPL_SIGN_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_scache_sync
 * Purpose:
 *      Sync Application signatures.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_scache_sync(int unit)
{
    int rv;
    int idx;
    uint8 *scache_ptr;
    uint32 entry_flags = 0;
    soc_scache_handle_t scache_handle;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
            BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCAHCE_PART_APPL_SIGN);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
            FALSE, 0, &scache_ptr,
            BCM_SWITCH_WB_APPL_SIGN_DEFAULT_VERSION, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Priority in Outer payload application signature */
    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    for (idx = 0; idx < BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES; idx++) {
        if (entry_list[idx] == NULL) {
            scache_ptr += sizeof(uint32);
            continue;
        }
        sal_memcpy(scache_ptr, &entry_list[idx]->prio, sizeof(uint32));
        scache_ptr += sizeof(uint32);
    }

    /* Priority in Inner payload application signature */
    entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    for (idx = 0; idx < BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES; idx++) {
        if (entry_list[idx] == NULL) {
            scache_ptr += sizeof(uint32);
            continue;
        }
        sal_memcpy(scache_ptr, &entry_list[idx]->prio, sizeof(uint32));
        scache_ptr += sizeof(uint32);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_parser_stage_buf_read
 * Purpose:
 *      DMA parser stage to buffer.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      stage_id        - (IN) Parser stage
 *      tcam_buffer     - (OUT) DMAed buffer for tcam table
 *      data_buffer     - (OUT) DMAed buffer for data table
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_parser_stage_buf_read(
    int unit,
    int stage_id,
    uint32 **tcam_buffer,
    uint32 **data_buffer)
{
    int rv = BCM_E_NONE;
    int num_entries = 0;
    int tcam_entry_size, data_entry_size;
    uint32 *tcam_buf = NULL;
    uint32 *data_buf = NULL;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t data_mem = INVALIDm;

    num_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;

    /* Get Memory */
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_id, &tcam_mem, &data_mem);

    tcam_entry_size = SOC_MEM_WORDS(unit, tcam_mem) * sizeof(uint32);
    data_entry_size = SOC_MEM_WORDS(unit, data_mem) * sizeof(uint32);

    /* Allocate buffer to store the DMAed table entries. */
    tcam_buf = soc_cm_salloc(unit, tcam_entry_size * num_entries,
            "parser stage tcam entry buffer");
    if (NULL == tcam_buf) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(tcam_buf, 0, sizeof(tcam_entry_size) * num_entries);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, tcam_mem,
            MEM_BLOCK_ALL, 0, (num_entries - 1), tcam_buf);
    if (BCM_FAILURE(rv)) {
        if (tcam_buf) {
            soc_cm_sfree(unit, tcam_buf);
        }
        return rv;
    }

    /* Allocate buffer to store the DMAed table entries. */
    data_buf = soc_cm_salloc(unit, data_entry_size * num_entries,
            "parser stage data entry buffer");
    if (NULL == data_buf) {
        soc_cm_sfree(unit, tcam_buf);
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(data_buf, 0, sizeof(data_entry_size) * num_entries);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, data_mem,
            MEM_BLOCK_ALL, 0, (num_entries - 1), data_buf);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, data_buf);
        soc_cm_sfree(unit, tcam_buf);
        return rv;
    }

    *tcam_buffer = tcam_buf;
    *data_buffer = data_buf;

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_parser_buffers_free
 * Purpose:
 *      Free DMAed buffer for parser stages
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      parser_buf      - (IN) DMAed buffer for parser stages
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_parser_buffers_free(
    int unit,
    bcmi_switch_appl_sign_parser_buf_t *parser_buf)
{
    int idx = 0;

    for (idx = 0; idx < APPL_SIGN_PARTS_MAX; idx++) {
        soc_cm_sfree(unit, parser_buf->tcam_buf[idx]);
        soc_cm_sfree(unit, parser_buf->data_buf[idx]);

        parser_buf->tcam_buf[idx] = NULL;
        parser_buf->data_buf[idx] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_parser_buffers_read
 * Purpose:
 *      Recover Application signatures in warmboot
 *      from h/w based on parser.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      parser_id       - (IN) Parser 1/2
 *      parser_buf      - (OUT) DMAed buffer for parser stages
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_parser_buffers_read(
    int unit,
    int parser_id,
    bcmi_switch_appl_sign_parser_buf_t *parser_buf)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int stage_id;
    int stage_id_arr[][3] = {
        {
            BCMI_SWITCH_PARSER1_STAGE3,
            BCMI_SWITCH_PARSER1_STAGE4,
            BCMI_SWITCH_PARSER1_STAGE2
        },
        {
            BCMI_SWITCH_PARSER2_STAGE3,
            BCMI_SWITCH_PARSER2_STAGE4,
            BCMI_SWITCH_PARSER2_STAGE2
        }
    };
    int part_arr[] = {
        APPL_SIGN_PART_S3,
        APPL_SIGN_PART_S4,
        APPL_SIGN_PART_S2
    };

    for (idx = 0; idx < COUNTOF(stage_id_arr[parser_id]); idx++) {
        stage_id = stage_id_arr[parser_id][idx];
        rv = bcmi_switch_parser_stage_buf_read(unit, stage_id,
                &parser_buf->tcam_buf[part_arr[idx]],
                &parser_buf->data_buf[part_arr[idx]]);
        BCMI_IF_ERROR_CLEANUP(rv);
    }

    return BCM_E_NONE;

cleanup:

    (void) bcmi_switch_appl_sign_parser_buffers_free(unit, parser_buf);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_recover
 * Purpose:
 *      Recover Application signatures in warmboot
 *      from given h/w index.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      entry_flags     - (IN) Entry flags
 *      hw_index        - (IN) H/w Index for Appl Sign entry
 *      parser_buf      - (IN) DMAed buffer for parser stages
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_recover(
    int unit,
    uint32 entry_flags,
    int hw_index,
    bcmi_switch_appl_sign_parser_buf_t *parser_buf)
{
    int rv = BCM_E_NONE;
    int parser_id = 0;
    int stage_id = 0, part_id = 0;
    int offset = 0;
    uint32 valid;
    uint8 *ref_list = NULL;
    bcmi_switch_appl_sign_entry_t *entry = NULL;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;
    soc_field_info_t *fieldinfo1 = NULL;
    soc_field_info_t *fieldinfo2 = NULL;
    uint32 data[1];
    uint32 *hw_entry = NULL;
    uint32 *hw_entries_buf = NULL;
    soc_mem_t tcam_mem[APPL_SIGN_PARTS_MAX];
    soc_mem_t data_mem[APPL_SIGN_PARTS_MAX];
    int entry_parts = 0;
    int stage_id_arr[][3] = {
        {
            BCMI_SWITCH_PARSER1_STAGE3,
            BCMI_SWITCH_PARSER1_STAGE4,
            BCMI_SWITCH_PARSER1_STAGE2
        },
        {
            BCMI_SWITCH_PARSER2_STAGE3,
            BCMI_SWITCH_PARSER2_STAGE4,
            BCMI_SWITCH_PARSER2_STAGE2
        }
    };

    parser_id = 0;
    if (entry_flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER) {
        parser_id = 1;
    }

    entry_parts = 0;

    /* Check if part in Stage 3 is valid */
    part_id = APPL_SIGN_PART_S3;
    stage_id = stage_id_arr[parser_id][part_id];
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_id, &tcam_mem[part_id], &data_mem[part_id]);
    BCM_IF_ERROR_RETURN(rv);

    hw_entries_buf = parser_buf->tcam_buf[part_id];
    hw_entry = soc_mem_table_idx_to_pointer(unit,
            tcam_mem[part_id], uint32 *, hw_entries_buf, hw_index);

    soc_mem_field_get(unit, tcam_mem[part_id],
            (void *) hw_entry, VALIDf, (uint32 *) &valid);

    if (valid) {

        /* Check if part in stage 2 is valid */
        part_id = APPL_SIGN_PART_S2;
        stage_id = stage_id_arr[parser_id][part_id];
        rv = bcmi_switch_parser_stage_mem_get(unit,
                stage_id, &tcam_mem[part_id], &data_mem[part_id]);
        BCM_IF_ERROR_RETURN(rv);

        hw_entries_buf = parser_buf->tcam_buf[part_id];
        hw_entry = soc_mem_table_idx_to_pointer(unit,
                tcam_mem[part_id], uint32 *, hw_entries_buf, hw_index);

        soc_mem_field_get(unit, tcam_mem[part_id],
                (void *) hw_entry, VALIDf, (uint32 *) &valid);
        if (valid) {
            entry_parts = 3;
            entry_flags |= BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE;
        } else {
            entry_parts = 2;
        }
    }

    if (entry_parts == 0) {
        return BCM_E_NONE;
    }

    /* Entry exists */
    APPL_SIGN_ALLOC(entry, sizeof(bcmi_switch_appl_sign_entry_t),
            "Appl Sign Entry");
    BCMI_IF_ERROR_CLEANUP(rv);

    entry->hw_index = hw_index;
    entry->parts = entry_parts;

    /* Allocate memory of parset stage entries */
    APPL_SIGN_ALLOC(entry->stage_entry,
            entry->parts * sizeof (bcmi_switch_appl_sign_parser_stage_entry_t),
            "Parser Stage Entries");
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Recover parser stage 3 */
    part_id = APPL_SIGN_PART_S3;
    entry->stage_entry[part_id].stage_id = stage_id_arr[parser_id][part_id];
    bcmi_switch_parser_stage_entry_hw_get(unit, &entry->stage_entry[part_id]);

    /* hw tcam buf */
    hw_entries_buf = parser_buf->tcam_buf[part_id];
    hw_entry = soc_mem_table_idx_to_pointer(unit,
            tcam_mem[part_id], uint32 *, hw_entries_buf, hw_index);

    soc_mem_field_get(unit, tcam_mem[part_id], hw_entry, KEYf,
            entry->stage_entry[part_id].hw_key_buf);
    soc_mem_field_get(unit, tcam_mem[part_id], hw_entry, MASKf,
            entry->stage_entry[part_id].hw_mask_buf);

    /* hw data buf */
    hw_entries_buf = parser_buf->data_buf[part_id];
    hw_entry = soc_mem_table_idx_to_pointer(unit,
            data_mem[part_id], uint32 *, hw_entries_buf, hw_index);
    soc_mem_field_get(unit, data_mem[part_id], hw_entry, DATAf,
            entry->stage_entry[part_id].hw_data_buf);

    /* Recover parser stage 4 */
    part_id = APPL_SIGN_PART_S4;
    entry->stage_entry[part_id].stage_id = stage_id_arr[parser_id][part_id];
    bcmi_switch_parser_stage_entry_hw_get(unit, &entry->stage_entry[part_id]);

    stage_id = stage_id_arr[parser_id][part_id];
    rv = bcmi_switch_parser_stage_mem_get(unit,
            stage_id, &tcam_mem[part_id], &data_mem[part_id]);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* hw tcam buf */
    hw_entries_buf = parser_buf->tcam_buf[part_id];
    hw_entry = soc_mem_table_idx_to_pointer(unit,
            tcam_mem[part_id], uint32 *, hw_entries_buf, hw_index);
    soc_mem_field_get(unit, tcam_mem[part_id], hw_entry, KEYf,
            entry->stage_entry[part_id].hw_key_buf);
    soc_mem_field_get(unit, tcam_mem[part_id], hw_entry, MASKf,
            entry->stage_entry[part_id].hw_mask_buf);

    /* hw data buf */
    hw_entries_buf = parser_buf->data_buf[part_id];
    hw_entry = soc_mem_table_idx_to_pointer(unit,
            data_mem[part_id], uint32 *, hw_entries_buf, hw_index);
    soc_mem_field_get(unit, data_mem[part_id], hw_entry, DATAf,
            entry->stage_entry[part_id].hw_data_buf);

    if (entry_flags & BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_RESPONSE) {

        /* Recover parser stage 2 */
        part_id = APPL_SIGN_PART_S2;
        entry->stage_entry[part_id].stage_id = stage_id_arr[parser_id][part_id];
        bcmi_switch_parser_stage_entry_hw_get(unit, &entry->stage_entry[part_id]);

        /* hw tcam buf */
        hw_entries_buf = parser_buf->tcam_buf[part_id];
        hw_entry = soc_mem_table_idx_to_pointer(unit,
                tcam_mem[part_id], uint32 *, hw_entries_buf, hw_index);
        soc_mem_field_get(unit, tcam_mem[part_id], hw_entry, KEYf,
                entry->stage_entry[part_id].hw_key_buf);
        soc_mem_field_get(unit, tcam_mem[part_id], hw_entry, MASKf,
                entry->stage_entry[part_id].hw_mask_buf);

        /* hw data buf */
        hw_entries_buf = parser_buf->data_buf[part_id];
        hw_entry = soc_mem_table_idx_to_pointer(unit,
                data_mem[part_id], uint32 *, hw_entries_buf, hw_index);
        soc_mem_field_get(unit, data_mem[part_id], hw_entry, DATAf,
                entry->stage_entry[part_id].hw_data_buf);
    }

    /* Recover TCP/UDP flags */
    part_id = APPL_SIGN_PART_S3;
    fieldinfo1 = soc_mem_fieldinfo_get(unit, tcam_mem[part_id], KEYf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, tcam_mem[part_id], IN_CONTEXT_IDf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        rv = BCM_E_INTERNAL;
        BCMI_IF_ERROR_CLEANUP(rv);
    }
    data[0] = 0;
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_get(unit, offset, 16,
            entry->stage_entry[part_id].hw_key_buf, data);
    BCMI_IF_ERROR_CLEANUP(rv);

    if (data[0] == 50) {
        entry_flags |= BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_UDP;
    }

    /* Recover eid from stage 4 data */
    part_id = APPL_SIGN_PART_S4;
    fieldinfo1 = soc_mem_fieldinfo_get(unit, data_mem[part_id], DATAf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, data_mem[part_id], CONTEXT_IDf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        rv = BCM_E_INTERNAL;
        BCMI_IF_ERROR_CLEANUP(rv);
    }
    data[0] = 0;
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_get(unit, offset, 16,
            entry->stage_entry[part_id].hw_data_buf, data);
    BCMI_IF_ERROR_CLEANUP(rv);

    entry->eid = (data[0] >> 8) & 0x3F;

    /* Recover context id from stage 3 data */
    part_id = APPL_SIGN_PART_S3;
    fieldinfo1 = soc_mem_fieldinfo_get(unit, data_mem[part_id], DATAf);
    fieldinfo2 = soc_mem_fieldinfo_get(unit, data_mem[part_id], CONTEXT_IDf);
    if ((fieldinfo1 == NULL) || (fieldinfo2 == NULL)) {
        rv = BCM_E_INTERNAL;
        BCMI_IF_ERROR_CLEANUP(rv);
    }
    data[0] = 0;
    offset = fieldinfo2->bp - fieldinfo1->bp;
    rv = bcmi_switch_entry_field_value_get(unit, offset, 16,
            entry->stage_entry[part_id].hw_data_buf, data);
    BCMI_IF_ERROR_CLEANUP(rv);

    entry->context_id = (data[0] >> 8) & 0x3F;
    BCMI_SWITCH_APPL_SIGN_CONTEXT_ID_REF_LIST(unit, entry_flags, ref_list);
    ref_list[entry->context_id - 1] += 1;

    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    entry_list[hw_index] = entry;
    entry->flags = entry_flags | BCMI_SWITCH_APPL_SIGN_ENTRY_INSTALLED;

    return BCM_E_NONE;

cleanup:

    bcmi_switch_appl_sign_entry_free(unit, entry);
    APPL_SIGN_FREE(entry);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_from_hw
 * Purpose:
 *      Recover Application signatures in warmboot
 *      from h/w based on parser.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      parser_id       - (IN) Parser 1/2
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_from_hw(
    int unit,
    int parser_id)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_parser_buf_t parser_buf;

    /* Read parser stages from h/w */
    sal_memset(&parser_buf, 0, sizeof(parser_buf));
    rv = bcmi_switch_appl_sign_parser_buffers_read(unit,
            parser_id, &parser_buf);
    BCMI_IF_ERROR_CLEANUP(rv);

    entry_flags = 0;
    if (parser_id == 1) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    /* Recover all application signatures */
    for (idx = 0; idx < BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES; idx++) {
        rv = bcmi_switch_appl_sign_entry_recover(unit,
                entry_flags, idx, &parser_buf);
        BCMI_IF_ERROR_CLEANUP(rv);
    }

cleanup:

    bcmi_switch_appl_sign_parser_buffers_free(unit, &parser_buf);

    return rv;
}

/*
 * Function:
 *      bcmi_switch_appl_sign_entry_reinit
 * Purpose:
 *      Reinit Application signatures in warmboot
 *      based on parser.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 *      parser_id       - (IN) Parser 1/2
 *      scache_ptr      - (IN/OUT) Scache pointer
 *      recovered_ver   - (IN) Recovered version
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_switch_appl_sign_entry_reinit(
    int unit,
    int parser_id,
    uint8 **scache_ptr,
    uint16 recovered_ver)
{
    int rv = BCM_E_NONE;
    int idx = 0, max_entries = 0;
    uint8 *local_scache_ptr = NULL;
    uint32 entry_flags = 0;
    bcmi_switch_appl_sign_entry_t **entry_list = NULL;

    local_scache_ptr = *scache_ptr;

    /* Recover appl sign entry from hw */
    rv = bcmi_switch_appl_sign_entry_from_hw(unit, parser_id);
    BCM_IF_ERROR_RETURN(rv);

    entry_flags = 0;
    if (parser_id == 1) {
        entry_flags = BCMI_SWITCH_APPL_SIGN_ENTRY_TYPE_INNER;
    }

    /* Update Priority from scache */
    BCMI_SWITCH_APPL_SIGN_ENTRY_LIST(unit, entry_flags, entry_list);
    max_entries = BCMI_SWITCH_APPL_SIGN_MAX_ENTRIES;
    for (idx = 0; idx < max_entries; idx++) {
        if (entry_list[idx] == NULL) {
            local_scache_ptr += sizeof(uint32);
            continue;
        }
        sal_memcpy(&entry_list[idx]->prio, local_scache_ptr, sizeof(uint32));
        local_scache_ptr += sizeof(uint32);
    }
    *scache_ptr = local_scache_ptr;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_switch_appl_signature_reinit
 * Purpose:
 *      Reinit Application signatures in warmboot.
 * Parameters:
 *      unit            - (IN) UNIT Number.
 * Returns:
 *       BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_switch_appl_signature_reinit(int unit)
{
    int rv = BCM_E_INTERNAL;
    uint8 *scache_ptr;
    uint16 recovered_ver = 0;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
          BCM_MODULE_SWITCH, BCM_SWITCH_WB_SCAHCE_PART_APPL_SIGN);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
            FALSE, 0, &scache_ptr,
            BCM_SWITCH_WB_APPL_SIGN_DEFAULT_VERSION,
            &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        return BCM_E_NONE;
    }

    /* Recover Application signatures for outer payload */
    rv = bcmi_switch_appl_sign_entry_reinit(unit,
            0, &scache_ptr, recovered_ver);
    BCM_IF_ERROR_RETURN(rv);

    /* Recover Application signatures for inner payload */
    rv = bcmi_switch_appl_sign_entry_reinit(unit,
            1, &scache_ptr, recovered_ver);

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#else
int switch_appl_sign_not_empty;
#endif /* BCM_APPL_SIGNATURE_SUPPORT */
