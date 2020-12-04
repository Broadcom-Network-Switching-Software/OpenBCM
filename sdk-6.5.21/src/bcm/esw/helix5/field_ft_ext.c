/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field_ft.c
 * Purpose:     Field Processor routines Specific to Flowtracker stage
 *              keygen, extraction and FT module interaction in Helix5.
 */

#include <soc/defs.h>

#if defined (BCM_HELIX5_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>

#include <bcm/flowtracker.h>
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <soc/profile_mem.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm_int/esw/flowtracker/ft_interface.h>

/* Internal MACRO to goto cleanup. */
#define BCMI_IF_ERROR_CLEANUP(_rv_)                                                 \
        do {                                                                        \
            if (_rv_ != BCM_E_NONE) {                                               \
                goto cleanup;                                                       \
            }                                                                       \
        } while(0)


/* Create Extractor Id to be passed in Keygen extactor init. */
#define BCMI_FT_KEYGEN_EXT_ID_CREATE(_sec_, _multi_, _ext_id_)      \
        (_ext_id_) = ((((_multi_) << 6) | (_sec_)) & 0x3FF)

/* Get multiplexer number from extractor Id. */
#define BCMI_FT_KEYGEN_EXT_MULTIPLEX_NUM_GET(_ext_id_, _multi_)     \
        (_multi_) = (((_ext_id_) >> 6) & 0xF)

/* Get Extractor number from extractor Id. */
#define BCMI_FT_KEYGEN_EXT_NUM_GET(_ext_id_, _ext_num_)             \
        (_ext_num_) = ((_ext_id_) & 0x2F)

/*
 * Typedef:
 *     _bcm_field_ft_ext_codes_type_t
 *
 * Purpose:
 *     This enum is type values for the purpose of saving
 *     flowtracker ext codes, as bytestream.
 */
typedef enum _bcm_field_ft_ext_code_type_e {
    _bcmFieldFtExtCodeKeygenProfile     = 0,
    _bcmFieldFtExtCodeTypeA             = 1,
    _bcmFieldFtExtCodeTypeAl1e16Sel     = 2,
    _bcmFieldFtExtCodeTypeAl2e16Mode    = 3,
    _bcmFieldFtExtCodeTypeAl2e16Section = 4,
    _bcmFieldFtExtCodeTypeAMask         = 5,
    _bcmFieldFtExtCodeTypeB             = 6,
    _bcmFieldFtExtCodeTypeBl1e16Sel     = 7,
    _bcmFieldFtExtCodeTypeC             = 8,
    _bcmFieldFtExtCodeTypeCl1e1Sel      = 9,
    _bcmFieldFtExtCodeTypeCMask         = 10
} _bcm_field_ft_ext_code_type_t;

/* List of final keygen section. */
STATIC
bcmi_keygen_ext_section_t final_section_arr[3] = {
    BCMI_KEYGEN_EXT_SECTION_FKA,
    BCMI_KEYGEN_EXT_SECTION_FKB,
    BCMI_KEYGEN_EXT_SECTION_FKC
};

/* List of intermediate keygen sections. */
STATIC
bcmi_keygen_ext_section_t section_arr[3][10] = {
    {
        BCMI_KEYGEN_EXT_SECTION_L2AS1,
        BCMI_KEYGEN_EXT_SECTION_L2AS2,
        BCMI_KEYGEN_EXT_SECTION_L2AS3,
        BCMI_KEYGEN_EXT_SECTION_L2AS4,
        BCMI_KEYGEN_EXT_SECTION_L2AS5,
        BCMI_KEYGEN_EXT_SECTION_L2AS6,
        BCMI_KEYGEN_EXT_SECTION_L2AS7,
        BCMI_KEYGEN_EXT_SECTION_L2AS8,
        BCMI_KEYGEN_EXT_SECTION_L2AS9,
        BCMI_KEYGEN_EXT_SECTION_L2AS10
    },
    {
        BCMI_KEYGEN_EXT_SECTION_L2BS1,
        BCMI_KEYGEN_EXT_SECTION_L2BS2,
        BCMI_KEYGEN_EXT_SECTION_L2BS3,
        BCMI_KEYGEN_EXT_SECTION_L2BS4,
        BCMI_KEYGEN_EXT_SECTION_L2BS5,
        BCMI_KEYGEN_EXT_SECTION_L2BS6,
        BCMI_KEYGEN_EXT_SECTION_L2BS7,
        BCMI_KEYGEN_EXT_SECTION_L2BS8,
        BCMI_KEYGEN_EXT_SECTION_L2BS9,
        BCMI_KEYGEN_EXT_SECTION_L2BS10
    },
    {
        BCMI_KEYGEN_EXT_SECTION_L2CS1,
        BCMI_KEYGEN_EXT_SECTION_L2CS2,
        BCMI_KEYGEN_EXT_SECTION_L2CS3,
        BCMI_KEYGEN_EXT_SECTION_L2CS4,
        BCMI_KEYGEN_EXT_SECTION_L2CS5,
        BCMI_KEYGEN_EXT_SECTION_L2CS6,
        BCMI_KEYGEN_EXT_SECTION_L2CS7,
        BCMI_KEYGEN_EXT_SECTION_L2CS8,
        BCMI_KEYGEN_EXT_SECTION_L2CS9,
        BCMI_KEYGEN_EXT_SECTION_L2CS10
    }
};

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_db_create
 *
 * Purpose:
 *    Create keygen type A extractors database.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_cfg_db - (IN/OUT) extractors config database.
 *    part       - (IN) part number.
 *    drain_bits - (IN) Number of unused bits.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_a_extractors_db_create(int unit,
                                           bcmi_keygen_ext_cfg_db_t *ext_cfg_db,
                                           int part,
                                           int multiplex_num,
                                           int drain_bits)
{
    int ext_id = 0;
    int sec_num = 0;
    int offset = 0;
    int level = 0;
    bcmi_keygen_ext_attrs_t ext_attrs;
    bcmi_keygen_ext_attrs_t *attrs = &ext_attrs;
    int base_offset = 0;

    BCMI_KEYGEN_EXT_CFG_DECL;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    base_offset = 0;
    for (sec_num = 0; sec_num < 10; sec_num++) {
        offset = base_offset + 16 * sec_num;
        BCMI_FT_KEYGEN_EXT_ID_CREATE(sec_num, multiplex_num, ext_id);

        level = 1;
        sal_memset(attrs, 0, sizeof(bcmi_keygen_ext_attrs_t));
        BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level,
                16, ext_id, BCMI_KEYGEN_EXT_SECTION_L1E16,
                section_arr[multiplex_num][sec_num], 0, attrs);

        level = 2;
        SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN);
        SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_4);
        SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_8);
        SHR_BITSET(ext_attrs.w, BCMI_KEYGEN_EXT_ATTR_MULTI_GRAN_16);
        BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level,
                4, ext_id, section_arr[multiplex_num][sec_num],
                final_section_arr[multiplex_num], offset, attrs);
    }

    level = 3;
    sal_memset(attrs, 0, sizeof(bcmi_keygen_ext_attrs_t));
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level,
            drain_bits, 0, final_section_arr[multiplex_num],
            BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

exit:

    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function:
 *    _field_ft_keygen_type_b_extractors_db_create
 *
 * Purpose:
 *    Create keygen type B extractors database
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_cfg_db - (IN/OUT) extractors config database.
 *    part       - (IN) part number.
 *    drain_bits - (IN) Number of unused bits.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_b_extractors_db_create(int unit,
                                           bcmi_keygen_ext_cfg_db_t *ext_cfg_db,
                                           int part,
                                           int multiplex_num,
                                           int drain_bits)
{
    int ext_id = 0;
    int sec_num = 0;
    int offset = 0;
    int level = 0;
    bcmi_keygen_ext_attrs_t ext_attrs;
    bcmi_keygen_ext_attrs_t *attrs = &ext_attrs;
    int base_offset = 0;

    BCMI_KEYGEN_FUNC_ENTER(unit);

    BCMI_KEYGEN_EXT_CFG_DECL;

    base_offset = 0;
    sal_memset(attrs, 0, sizeof(bcmi_keygen_ext_attrs_t));
    for (sec_num = 0; sec_num < 8; sec_num++) {
        offset = base_offset + 16 * sec_num;
        BCMI_FT_KEYGEN_EXT_ID_CREATE(sec_num, multiplex_num, ext_id);

        level = 1;
        BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level,
                16, ext_id, BCMI_KEYGEN_EXT_SECTION_L1E16,
                section_arr[multiplex_num][sec_num], 0, attrs);

        level = 2;
        BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level,
                16, ext_id, section_arr[multiplex_num][sec_num],
                final_section_arr[multiplex_num], offset, attrs);
    }

    level = 3;
    BCMI_KEYGEN_EXT_CFG_ADD(unit, ext_cfg_db, part, level,
            160, 0, final_section_arr[multiplex_num],
            BCMI_KEYGEN_EXT_SECTION_DISABLE, 0, attrs);

    BCMI_KEYGEN_EXT_SECTION_DBITS_SET(unit, ext_cfg_db,
            final_section_arr[multiplex_num], drain_bits);

exit:

    BCMI_KEYGEN_FUNC_EXIT();
}

/*
 * Function:
 *    _field_ft_keygen_profile_aab_extractors_db_create
 *
 * Purpose:
 *    Create keygen profile AAB extractors database
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_cfg_db - (IN/OUT) extractors config database.
 *    ext_mode   - (IN) Extractor mode
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_profile_aab_extractors_db_create(int unit,
                                            bcmi_keygen_ext_cfg_db_t *ext_cfg_db,
                                            int ext_mode)
{
    if (ext_cfg_db == NULL) {
        return BCM_E_PARAM;
    }

    switch(ext_mode) {
        case BCMI_KEYGEN_MODE_SINGLE:
            /*
             * Single-Wide extractors information
             * 128 bits in A + 52 bits in A + 0 bits in B
             */
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_a_extractors_db_create(unit,
                    ext_cfg_db, 0, 0, 128));
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_a_extractors_db_create(unit,
                    ext_cfg_db, 1, 1, 52));
            break;
        case BCMI_KEYGEN_MODE_DBLINTRA:
            /*
             * Double-Wide extractors information
             * 128 bits in A + 128 bits in A + 104 bits in B
             */
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_a_extractors_db_create(unit,
                    ext_cfg_db, 0, 0, 128));
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_a_extractors_db_create(unit,
                    ext_cfg_db, 1, 1, 128));
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_b_extractors_db_create(unit,
                    ext_cfg_db, 2, 2, 104));
            break;
        default:
            return BCM_E_PARAM;
    }

    ext_cfg_db->mode = ext_mode;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_profile_ab_extractors_db_create
 *
 * Purpose:
 *    Create keygen profile AB extractors database.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_cfg_db - (IN/OUT) extractors config database.
 *    ext_mode   - (IN) Extractor mode
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_profile_ab_extractors_db_create(int unit,
                                            bcmi_keygen_ext_cfg_db_t *ext_cfg_db,
                                            int ext_mode)
{
    if (ext_cfg_db == NULL) {
        return BCM_E_PARAM;
    }

    switch(ext_mode) {
        case BCMI_KEYGEN_MODE_SINGLE:
            /*
             * Single-Wide extractors information
             * 128 bits in A + 128 bits in B
             */
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_a_extractors_db_create(unit,
                                ext_cfg_db, 0, 0, 128));
            BCM_IF_ERROR_RETURN(_field_ft_keygen_type_b_extractors_db_create(unit,
                                ext_cfg_db, 1, 1, 128));
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_ft_keygen_extractors_init
 *
 * Purpose:
 *   Initialize keygen profile extractors.
 *
 * Parameters:
 *    ext_codes  - (IN/OUT) Extractor codes.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_ft_keygen_extractors_init(_field_ft_keygen_ext_codes_t *ext_codes)
{
    int i = 0;
    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_extractors = NULL;
    _field_ft_keygen_type_c_ext_t *type_c_extractors = NULL;

    if (ext_codes == NULL) {
        return BCM_E_NONE;
    }

    sal_memset(ext_codes, 0, sizeof (_field_ft_keygen_ext_codes_t));

    type_a_extractors = &ext_codes->type_a_extractors[0];
    for (i = 0; i < 10; i++) {
        type_a_extractors[0].l1_e16_sel[i] = -1;
        type_a_extractors[0].l2_e16_ext_mode[i] = -1;
        type_a_extractors[0].l2_e16_ext_section[i] = -1;

        type_a_extractors[1].l1_e16_sel[i] = -1;
        type_a_extractors[1].l2_e16_ext_mode[i] = -1;
        type_a_extractors[1].l2_e16_ext_section[i] = -1;
    }

    type_b_extractors = &ext_codes->type_b_extractors;
    for (i = 0; i < 8; i++) {
        type_b_extractors->l1_e16_sel[i] = -1;
    }

    type_c_extractors = &ext_codes->type_c_extractors;
    for (i =0; i< 16; i++) {
        type_c_extractors->l1_e1_sel[i] = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_set
 *
 * Purpose:
 *    Extract select codes for Keygen Type A.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_ctrl_sel_info - (IN) extractor control select information.
 *    type_a_extrators - (OUT) Type A extractors.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_a_extractors_set(int unit,
        bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info,
        _field_ft_keygen_type_a_ext_t *type_a_extrators)
{
    int rv = BCM_E_NONE;
    uint8 ctrl_sel = 0;
    uint8 ctrl_sel_val = 0;
    uint8 level = 0;
    uint8 gran = 0;
    uint8 ext_num = 0;

    ctrl_sel = ext_ctrl_sel_info->ctrl_sel;
    ctrl_sel_val = ext_ctrl_sel_info->ctrl_sel_val;
    level = ext_ctrl_sel_info->level;
    gran = ext_ctrl_sel_info->gran;
    BCMI_FT_KEYGEN_EXT_NUM_GET(ext_ctrl_sel_info->ext_num, ext_num);


    if (level == 1) {
        if (gran == 16) {
            if (ctrl_sel == BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER) {
                type_a_extrators->l1_e16_sel[ext_num] = ctrl_sel_val;
            }
        } else {
            rv = BCM_E_INTERNAL;
        }
    } else if (level == 2) {
        if (gran == 16) {
            type_a_extrators->l2_e16_ext_mode[ext_num] = 0; /* SUB_SELECT_16 */
            type_a_extrators->l2_e16_ext_section[ext_num] = (ctrl_sel_val % 1);
        } else if (gran == 8) {
            type_a_extrators->l2_e16_ext_mode[ext_num] = 1; /* SUB_SELECT_8 */
            type_a_extrators->l2_e16_ext_section[ext_num] = (ctrl_sel_val % 2);
        } else if (gran == 4) {
            type_a_extrators->l2_e16_ext_mode[ext_num] = 2; /* SUB_SELECT_4 */
            type_a_extrators->l2_e16_ext_section[ext_num] = (ctrl_sel_val % 4);
        } else {
            rv = BCM_E_INTERNAL;
        }
    } else {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_extractors_set
 *
 * Purpose:
 *    Extract select codes for Keygen Type B.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_ctrl_sel_info - (IN) extractor control select information.
 *    type_b_extrators - (OUT) Type B extractors.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_b_extractors_set(int unit,
        bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info,
        _field_ft_keygen_type_b_ext_t *type_b_extractors)
{
    int rv = BCM_E_NONE;
    uint8 ctrl_sel_val = 0;
    uint8 level = 0;
    uint8 gran = 0;
    uint8 ext_num = 0;

    ctrl_sel_val = ext_ctrl_sel_info->ctrl_sel_val;
    level = ext_ctrl_sel_info->level;
    gran = ext_ctrl_sel_info->gran;
    BCMI_FT_KEYGEN_EXT_NUM_GET(ext_ctrl_sel_info->ext_num, ext_num);

    if (level == 1) {
        if (gran == 16) {
            type_b_extractors->l1_e16_sel[ext_num] = ctrl_sel_val;
        } else {
            rv = BCM_E_INTERNAL;
        }
    } else if (level == 2) {
        rv = BCM_E_NONE; /* Ignore */
    } else {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_profile_aab_extractors_set
 *
 * Purpose:
 *   Extract extraction select codes for Keygen profile AAB.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_ctrl_sel_info - (IN) extraction control select codes
 *    ext_codes  - (IN/OUT) hw values of extraction codes
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_profile_aab_extractors_set(int unit,
        uint16 ext_ctrl_sel_info_count,
        bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int multiplex_num = 0;
    int ext_idx = 0;

    for (ext_idx = 0; ext_idx < ext_ctrl_sel_info_count; ext_idx++) {
        BCMI_FT_KEYGEN_EXT_MULTIPLEX_NUM_GET(ext_ctrl_sel_info[ext_idx].ext_num,
                                                multiplex_num);
        switch(multiplex_num) {
            case 0:
            case 1:
                rv = _field_ft_keygen_type_a_extractors_set(unit,
                        &ext_ctrl_sel_info[ext_idx],
                        &ext_codes->type_a_extractors[multiplex_num]);
                BCM_IF_ERROR_RETURN(rv);
                break;
            case 2:
                rv = _field_ft_keygen_type_b_extractors_set(unit,
                        &ext_ctrl_sel_info[ext_idx],
                        &ext_codes->type_b_extractors);
                BCM_IF_ERROR_RETURN(rv);
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_profile_ab_extractors_set
 *
 * Purpose:
 *   Extract extraction select codes for Keygen profile AB.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_ctrl_sel_info - (IN) extraction control select codes
 *    ext_codes  - (IN/OUT) hw values of extraction codes
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_profile_ab_extractors_set(int unit,
        uint16 ext_ctrl_sel_info_count,
        bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int multiplex_num = 0;
    int ext_idx = 0;

    for (ext_idx = 0; ext_idx < ext_ctrl_sel_info_count; ext_idx++) {
        BCMI_FT_KEYGEN_EXT_MULTIPLEX_NUM_GET(ext_ctrl_sel_info[ext_idx].ext_num,
                                                multiplex_num);
        switch(multiplex_num) {
            case 0:
                rv = _field_ft_keygen_type_a_extractors_set(unit,
                        &ext_ctrl_sel_info[ext_idx],
                        &ext_codes->type_a_extractors[0]);
                BCM_IF_ERROR_RETURN(rv);
                break;
            case 1:
                rv = _field_ft_keygen_type_b_extractors_set(unit,
                        &ext_ctrl_sel_info[ext_idx],
                        &ext_codes->type_b_extractors);
                BCM_IF_ERROR_RETURN(rv);
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_ext_mask_set
 *
 * Purpose:
 *    Set extract Mask for type A extractor select codes.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    offset     - (IN) Offset of bits in extracted key.
 *    width      - (IN) Number of bits valid from given offset.
 *    ext_num    - (IN) Extractor Number.
 *    shift_offsets_arr - (IN) Shift offsets per extractors in final key.
 *    mask_offset- (IN) Position of mask bites in 'mask'.
 *    mask       - (IN) Mask of the tracking param.
 *    ext_codes  - (IN/OUT) extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_type_a_ext_mask_set(int unit,
        int offset,
        int width,
        int ext_num,
        int *shift_offsets_arr,
        int mask_offset,
        uint32 *mask,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int multiplex_num = 0;
    int final_offset = 0;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;

    multiplex_num = ext_num / 10;
    type_a_mask = &ext_codes->type_a_mask[multiplex_num];

    if (shift_offsets_arr == NULL) {
        final_offset = offset;
    } else {
        final_offset = (offset % 16) + shift_offsets_arr[ext_num];
    }
    final_offset %= 128;

    if (mask == NULL) {
        SHR_BITSET_RANGE(type_a_mask->mask, final_offset, width);
    } else {
        SHR_BITCOPY_RANGE(type_a_mask->mask, final_offset,
                mask, mask_offset, width);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_ext_mask_set
 *
 * Purpose:
 *    Set extract Mask for type B extractor select codes.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    offset     - (IN) Offset of bits in extracted key.
 *    width      - (IN) Number of bits valid from given offset.
 *    ext_num    - (IN) Extractor Number.
 *    shift_offsets_arr - (IN) Shift offsets per extractors in final key.
 *    mask_offset- (IN) Position of mask bites in 'mask'.
 *    mask       - (IN) Mask of the tracking param.
 *    ext_codes  - (IN/OUT) extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_type_b_ext_mask_set(int unit,
        int offset,
        int width,
        int ext_num,
        int mask_offset,
        uint32 *mask,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int idx = 0;
    uint32 final_mask[1];

    if (mask == NULL) {
        return BCM_E_NONE;
    }

    SHR_BITCOPY_RANGE(final_mask, 0, mask, mask_offset, width);

    /* Disable the extractor */
    if (final_mask[0] == 0) {
        /*
         * Type B starts at multiple of 10 (After Type A).
         * Also, only 8 extractors in type B so 'mod 8'.
         */
        idx = (ext_num % 10) % 8;
        ext_codes->type_b_extractors.l1_e16_sel[idx] = -1;
    } else if (final_mask[0] != 0xFFFF) {
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_ft_keygen_profile_extractors_mask_set
 *
 * Purpose:
 *    Set extract Mask for extractor select codes.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    offset     - (IN) Offset of bits in extracted key.
 *    width      - (IN) Number of bits valid from given offset.
 *    ext_num    - (IN) Extractor Number.
 *    shift_offsets_arr - (IN) Shift offsets per extractors in final key.
 *    mask_offset- (IN) Position of mask bites in 'mask'.
 *    mask       - (IN) Mask of the tracking param.
 *    ext_codes  - (IN/OUT) extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_keygen_profile_extractors_mask_set(int unit,
        int offset,
        int width,
        int ext_num,
        int *shift_offsets_arr,
        int mask_offset,
        uint32 *mask,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int multiplex_num = 0;

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            multiplex_num = ext_num / 10; /* Type A has 10 extractors */
            if ((multiplex_num == 0) || (multiplex_num == 1)) {
                rv = _field_ft_keygen_type_a_ext_mask_set(unit,
                        offset, width, ext_num, shift_offsets_arr,
                        mask_offset, mask, ext_codes);
            } else {
                rv = _field_ft_keygen_type_b_ext_mask_set(unit,
                        offset, width, ext_num,
                        mask_offset, mask, ext_codes);
            }
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            multiplex_num = ext_num / 10; /* Type A has 10 extractors */
            if ((multiplex_num == 0) || (multiplex_num == 1)) {
                rv = _field_ft_keygen_type_a_ext_mask_set(unit,
                        offset, width, ext_num, shift_offsets_arr,
                        mask_offset, mask, ext_codes);
            } else {
                rv = _field_ft_keygen_type_b_ext_mask_set(unit,
                        offset, width, ext_num,
                        mask_offset, mask, ext_codes);
            }
            break;
        default:
            return BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen type A extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    extractors - (IN) Type A extractor select code.
 *    mem        - (IN) keygen type A memory.
 *    ent_buf    - (OUT) entry buffer
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_type_a_extractors_mem_set(int unit,
        _field_ft_keygen_type_a_ext_t *extractors,
        soc_mem_t mem,
        uint32 *ent_buf)
{
    int i = 0;
    int rv = BCM_E_NONE;

    static const soc_field_t l1_e16_sel[] = {
        EXTRACT_TYPE_A_0_L1_E16_SELf,
        EXTRACT_TYPE_A_1_L1_E16_SELf,
        EXTRACT_TYPE_A_2_L1_E16_SELf,
        EXTRACT_TYPE_A_3_L1_E16_SELf,
        EXTRACT_TYPE_A_4_L1_E16_SELf,
        EXTRACT_TYPE_A_5_L1_E16_SELf,
        EXTRACT_TYPE_A_6_L1_E16_SELf,
        EXTRACT_TYPE_A_7_L1_E16_SELf,
        EXTRACT_TYPE_A_8_L1_E16_SELf,
        EXTRACT_TYPE_A_9_L1_E16_SELf
    };

    static const soc_field_t l2_e16_ext_mode[] = {
        EXTRACT_TYPE_A_0_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_1_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_2_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_3_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_4_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_5_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_6_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_7_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_8_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_9_L2_E16_EXTRACT_MODEf
    };

    static const soc_field_t l2_e16_ext_section[] = {
        EXTRACT_TYPE_A_0_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_1_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_2_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_3_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_4_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_5_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_6_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_7_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_8_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_9_L2_E16_EXTRACT_SECTIONf
    };

    for (i = 0; i < 10; i++) {
        if ((extractors->l1_e16_sel[i] == -1) ||
            (extractors->l2_e16_ext_mode[i] ==  -1) ||
            (extractors->l2_e16_ext_section[i] == -1)) {
            continue;
        }
        soc_mem_field32_set(unit, mem, ent_buf,
                l1_e16_sel[i], extractors->l1_e16_sel[i]);
        soc_mem_field32_set(unit, mem, ent_buf,
                l2_e16_ext_mode[i], extractors->l2_e16_ext_mode[i]);
        soc_mem_field32_set(unit, mem, ent_buf,
                l2_e16_ext_section[i], extractors->l2_e16_ext_section[i]);
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_mask_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen type A extractors mask.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    extractors - (IN) Type A extractor select code mask.
 *    mem        - (IN) keygen type A mask memory.
 *    ent_buf    - (OUT) entry buffer
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_type_a_extractors_mask_mem_set(int unit,
        _field_ft_keygen_type_a_ext_mask_t *type_a_mask,
        soc_mem_t mem,
        uint32 *ent_buf)
{
    static const soc_field_t mask_field[] = {
        MASKf
    };

    soc_mem_field_set(unit, mem, ent_buf,
                    mask_field[0], type_a_mask->mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_extractors_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen type B extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    extractors - (IN) Type B extractor select codes.
 *    mem        - (IN) keygen type B memory.
 *    ent_buf    - (OUT) entry buffer
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_type_b_extractors_mem_set(int unit,
        _field_ft_keygen_type_b_ext_t *extractors,
        soc_mem_t mem,
        uint32 *ent_buf)
{
    int rv = BCM_E_NONE;
    int i = 0;
    uint32 en = 1;

    static const soc_field_t l1_e16_sel[] = {
        EXTRACT_TYPE_B_0_L1_E16_SELf,
        EXTRACT_TYPE_B_1_L1_E16_SELf,
        EXTRACT_TYPE_B_2_L1_E16_SELf,
        EXTRACT_TYPE_B_3_L1_E16_SELf,
        EXTRACT_TYPE_B_4_L1_E16_SELf,
        EXTRACT_TYPE_B_5_L1_E16_SELf,
        EXTRACT_TYPE_B_6_L1_E16_SELf,
        EXTRACT_TYPE_B_7_L1_E16_SELf
    };
    static const soc_field_t l1_e16_en[] = {
        EXTRACT_0_ENABLEf,
        EXTRACT_1_ENABLEf,
        EXTRACT_2_ENABLEf,
        EXTRACT_3_ENABLEf,
        EXTRACT_4_ENABLEf,
        EXTRACT_5_ENABLEf,
        EXTRACT_6_ENABLEf,
        EXTRACT_7_ENABLEf
    };

    for (i = 0; i < 8; i++) {
        if (extractors->l1_e16_sel[i] == -1) {
            continue;
        }
        soc_mem_field32_set(unit, mem, ent_buf, l1_e16_sel[i],
                extractors->l1_e16_sel[i]);
        soc_mem_field32_set(unit, mem, ent_buf, l1_e16_en[i], en);
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_c_extractors_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen type C extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    extractors - (IN) Type C extractor select codes.
 *    mem        - (IN) keygen type B memory.
 *    ent_buf    - (OUT) entry buffer
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_type_c_extractors_mem_set(int unit,
        _field_ft_keygen_type_c_ext_t *extractors,
        soc_mem_t mem,
        uint32 *ent_buf)
{
    int rv = BCM_E_NONE;
    int i = 0;

    static const soc_field_t l1_e1_sel[]  = {
        EXTRACT_TYPE_C_0_L1_E1_SELf,
        EXTRACT_TYPE_C_1_L1_E1_SELf,
        EXTRACT_TYPE_C_2_L1_E1_SELf,
        EXTRACT_TYPE_C_3_L1_E1_SELf,
        EXTRACT_TYPE_C_4_L1_E1_SELf,
        EXTRACT_TYPE_C_5_L1_E1_SELf,
        EXTRACT_TYPE_C_6_L1_E1_SELf,
        EXTRACT_TYPE_C_7_L1_E1_SELf,
        EXTRACT_TYPE_C_8_L1_E1_SELf,
        EXTRACT_TYPE_C_9_L1_E1_SELf,
        EXTRACT_TYPE_C_10_L1_E1_SELf,
        EXTRACT_TYPE_C_11_L1_E1_SELf,
        EXTRACT_TYPE_C_12_L1_E1_SELf,
        EXTRACT_TYPE_C_13_L1_E1_SELf,
        EXTRACT_TYPE_C_14_L1_E1_SELf,
        EXTRACT_TYPE_C_15_L1_E1_SELf
    };

    for (i = 0; i < COUNTOF(l1_e1_sel); i++) {
        if (extractors->l1_e1_sel[i] == -1) {
            continue;
        }
        soc_mem_field32_set(unit, mem, ent_buf,
            l1_e1_sel[i], extractors->l1_e1_sel[i]);
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_c_extractors_mask_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen type C extractors mask.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    extractors - (IN) Type C extractor select code mask.
 *    mem        - (IN) keygen type C mask memory.
 *    ent_buf    - (OUT) entry buffer
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_type_c_extractors_mask_mem_set(int unit,
        _field_ft_keygen_type_c_ext_mask_t *type_c_mask,
        soc_mem_t mem,
        uint32 *ent_buf)
{
    static const soc_field_t mask_field[] = {
        MASKf
    };

    soc_mem_field32_set(unit, mem, ent_buf,
                    mask_field[0], type_c_mask->mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_profile_aab_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen Profile AAB extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_codes  - (IN) Extractor select codes.
 *    mem_arr    - (IN) Profile AAB memory array.
 *    ent_buf_arr- (OUT) Profiel AAB entry buffer array.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_profile_aab_mem_set(int unit,
        _field_ft_keygen_ext_codes_t *ext_codes,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr)
{
    int rv = BCM_E_NONE;
    uint32 *ent_buf = NULL;

    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;
    _field_ft_keygen_type_a_ext_mask_t       *type_a_mask = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_extractors = NULL;

    /* Multiplex_num 0 Keygen Type A */
    ent_buf = ent_buf_arr[0];
    type_a_extractors = &ext_codes->type_a_extractors[0];
    rv = _field_ft_keygen_type_a_extractors_mem_set(unit,
                type_a_extractors, mem_arr[0], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 0 Keygen Type A Mask */
    ent_buf = ent_buf_arr[1];
    type_a_mask = &ext_codes->type_a_mask[0];
    rv = _field_ft_keygen_type_a_extractors_mask_mem_set(unit,
                type_a_mask, mem_arr[1], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 1 Keygen Type A */
    ent_buf = ent_buf_arr[2];
    type_a_extractors = &ext_codes->type_a_extractors[1];
    rv = _field_ft_keygen_type_a_extractors_mem_set(unit,
                type_a_extractors, mem_arr[2], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 1 Keygen Type A Mask */
    ent_buf = ent_buf_arr[3];
    type_a_mask = &ext_codes->type_a_mask[1];
    rv = _field_ft_keygen_type_a_extractors_mask_mem_set(unit,
                type_a_mask, mem_arr[3], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 2 Keygen Type B */
    ent_buf = ent_buf_arr[4];
    type_b_extractors = &ext_codes->type_b_extractors;
    rv = _field_ft_keygen_type_b_extractors_mem_set(unit,
                type_b_extractors, mem_arr[4], ent_buf);

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_profile_ab_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen Profile AB extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_codes  - (IN) Extractor select codes.
 *    mem_arr    - (IN) Profile AB memory array.
 *    ent_buf_arr- (OUT) Profiel AB entry buffer array.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_ft_keygen_profile_ab_mem_set(int unit,
        _field_ft_keygen_ext_codes_t *ext_codes,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr)
{
    int rv = BCM_E_NONE;
    uint32 *ent_buf = NULL;

    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_extractors = NULL;

    /* Multiplex_num 0 Keygen Type A */
    ent_buf = ent_buf_arr[0];
    type_a_extractors = &ext_codes->type_a_extractors[0];
    rv = _field_ft_keygen_type_a_extractors_mem_set(unit,
                type_a_extractors, mem_arr[0], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 0 Keygen Type A Mask */
    ent_buf = ent_buf_arr[1];
    type_a_mask = &ext_codes->type_a_mask[0];
    rv = _field_ft_keygen_type_a_extractors_mask_mem_set(unit,
                type_a_mask, mem_arr[1], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 1 Keygen Type B */
    ent_buf = ent_buf_arr[2];
    type_b_extractors = &ext_codes->type_b_extractors;
    rv = _field_ft_keygen_type_b_extractors_mem_set(unit,
                type_b_extractors, mem_arr[2], ent_buf);

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_keygen_extractors_set
 *
 * Purpose:
 *    Populate extractor codes from extractor control select
 *    code as given out by keygen algorithm.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    keygen_profile - (IN) Keygen profile for ext codes.
 *    ext_ctrl_sel_info_count - (IN) Number of elements in ext_ctrl_sel_info
 *    ext_ctrl_sel_info - (IN) extraction control select codes
 *    ext_codes  - (IN/OUT) hw values of extraction codes
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_keygen_extractors_set(int unit,
        int keygen_profile,
        uint16 ext_ctrl_sel_info_count,
        bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;

    if (ext_codes == NULL) {
        return BCM_E_PARAM;
    }

    if (ext_ctrl_sel_info == NULL) {
        return BCM_E_NONE;
    }
    ext_codes->keygen_profile = keygen_profile;
    switch(keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            rv = _field_ft_keygen_profile_aab_extractors_set(unit,
                    ext_ctrl_sel_info_count,
                    ext_ctrl_sel_info, ext_codes);
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            rv = _field_ft_keygen_profile_ab_extractors_set(unit,
                    ext_ctrl_sel_info_count,
                    ext_ctrl_sel_info, ext_codes);
            break;
        default:
            return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_keygen_profile_mem_set
 *
 * Purpose:
 *   Set memory fields for given keygen Profile extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_codes  - (IN) Extractor select codes.
 *    mem_count  - (IN) Number of memories in keygen profile.
 *    mem_arr    - (IN) Profile AB memory array.
 *    ent_buf_arr- (OUT) entry buffer array.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_ft_keygen_profile_mem_set(int unit,
        _field_ft_keygen_ext_codes_t *ext_codes,
        int mem_count,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr)
{
    int rv = BCM_E_NONE;

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            if (mem_count != 5) {
                return BCM_E_INTERNAL;
            }
            rv = _field_ft_keygen_profile_aab_mem_set(unit,
                        ext_codes, mem_arr, ent_buf_arr);
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            if (mem_count != 3) {
                return BCM_E_INTERNAL;
            }
            rv = _field_ft_keygen_profile_ab_mem_set(unit,
                        ext_codes, mem_arr, ent_buf_arr);
            break;
        default:
            rv = BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_keygen_type_c_extractors_mem_set
 *
 * Purpose:
 *   Set memory fields for keygen type C extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ext_codes  - (IN) Extractor select codes.
 *    mem_count  - (IN) Number of memories in keygen profile.
 *    mem_arr    - (IN) Profile AB memory array.
 *    ent_buf_arr- (OUT) entry buffer array.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_ft_keygen_type_c_extractors_mem_set(int unit,
        _field_ft_keygen_ext_codes_t *ext_codes,
        int mem_count,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr)
{
    int rv = BCM_E_NONE;

    if (ext_codes->keygen_profile != _BCM_FIELD_FT_KEYGEN_PROFILE_AB) {
        return BCM_E_INTERNAL;
    }

    rv = _field_ft_keygen_type_c_extractors_mem_set(unit,
                &ext_codes->type_c_extractors, mem_arr[0], ent_buf_arr[0]);
    BCM_IF_ERROR_RETURN(rv);

    rv = _field_ft_keygen_type_c_extractors_mask_mem_set(unit,
            &ext_codes->type_c_mask, mem_arr[0], ent_buf_arr[0]);

    return rv;
}

/*
 * Function:
 *    _keygen_profile_name
 *
 * Purpose:
 *   Get name of keygen profile.
 *
 * Parameters:
 *    keygen_profile - (IN) keygen profile
 *
 * Returns:
 *    String for name.
 */
STATIC char *
_keygen_profile_name(int keygen_profile)
{
    if (keygen_profile == _BCM_FIELD_FT_KEYGEN_PROFILE_AAB) {
        return "AAB";
    } else if (keygen_profile == _BCM_FIELD_FT_KEYGEN_PROFILE_AB) {
        return "AB";
    } else {
        return "INVALID";
    }
}

/*
 * Function:
 *    _keygen_type_a_ext_mode_name
 *
 * Purpose:
 *   Get name of keygen type A extractor Modes.
 *
 * Parameters:
 *    ext_mode   - (IN) extractor mode.
 * Returns:
 *    String for name.
 */
STATIC char *
_keygen_type_a_ext_mode_name(int ext_mode)
{
    if (ext_mode == 0) {
        return "SUB_SELECT_16";
    } else if (ext_mode == 1) {
        return "SUB_SELECT_8";
    } else if (ext_mode == 2) {
        return "SUB_SELECT_4";
    } else {
        return "DISABLE";
    }
}
/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_dump
 *
 * Purpose:
 *   Dump keygen type A extractor info.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    type_a_ext - (IN) extraction codes for type A.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_type_a_extractors_dump(int unit,
        int multiplex_num,
        _field_ft_keygen_type_a_ext_t *ext)
{
    int rv = BCM_E_NONE;
    int i = 0;

    for (i = 0; i < 10; i++) {
        if (ext->l1_e16_sel[i] == -1) {
            continue;
        }
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "hw_ei[A,%2d,%2d] ->"
                        " {l0=[%2d], l1=[%s,%2d]}\n\r"),
                    multiplex_num, i, ext->l1_e16_sel[i],
                    _keygen_type_a_ext_mode_name(ext->l2_e16_ext_mode[i]),
                    ext->l2_e16_ext_section[i]));
    }

    return rv;
}

/*
 * Function:
 *    _field_kt_keygen_type_a_ext_mask_dump
 *
 * Purpose:
 *   Dump keygen type A extractor Mask info.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    type_a_ext - (IN) extraction codes for type A.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_kt_keygen_type_a_ext_mask_dump(int unit,
        int multiplex_num,
        _field_ft_keygen_type_a_ext_mask_t *mask)
{
    int rv = BCM_E_NONE;
    int i = 0;

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "hw_ei_mask[A,%2d] -> { "),
                    multiplex_num));

    for (i = 3; i >= 0; i--) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "%08x "), mask->mask[i]));
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "}\n\r")));

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_extractors_dump
 *
 * Purpose:
 *   Dump keygen type A extractor info.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    type_a_ext - (IN) extraction codes for type A.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_type_b_extractors_dump(int unit,
        int multiplex_num,
        _field_ft_keygen_type_b_ext_t *ext)
{
    int rv = BCM_E_NONE;
    int i = 0;

    for (i = 0; i < 8; i++) {
        if (ext->l1_e16_sel[i] == -1) {
            continue;
        }
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "hw_ei[B,%2d,%2d] ->"
                        " {l0=[%2d]}\n\r"),
                    multiplex_num, i, ext->l1_e16_sel[i]));
    }
    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_profile_aab_dump
 *
 * Purpose:
 *   Dump keygen profile AAB extractor info.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    ext_codes  - (IN) extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_profile_aab_dump(int unit,
    _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    _field_ft_keygen_type_a_ext_t *type_a_ext = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_ext = NULL;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;

    type_a_ext = &ext_codes->type_a_extractors[0];
    (void)_field_ft_keygen_type_a_extractors_dump(unit, 0, type_a_ext);

    type_a_mask = &ext_codes->type_a_mask[0];
    (void) _field_kt_keygen_type_a_ext_mask_dump(unit, 0, type_a_mask);

    type_a_ext = &ext_codes->type_a_extractors[1];
    (void)_field_ft_keygen_type_a_extractors_dump(unit, 1, type_a_ext);

    type_a_mask = &ext_codes->type_a_mask[1];
    (void) _field_kt_keygen_type_a_ext_mask_dump(unit, 1, type_a_mask);


    type_b_ext = &ext_codes->type_b_extractors;
    (void)_field_ft_keygen_type_b_extractors_dump(unit, 2, type_b_ext);

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_profile_ab_dump
 *
 * Purpose:
 *   Dump keygen profile AB extractor info.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    ext_codes  - (IN) extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_profile_ab_dump(int unit,
    _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    _field_ft_keygen_type_a_ext_t *type_a_ext = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_ext = NULL;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;

    type_a_ext = &ext_codes->type_a_extractors[0];
    (void)_field_ft_keygen_type_a_extractors_dump(unit, 0, type_a_ext);

    type_a_mask = &ext_codes->type_a_mask[0];
    (void) _field_kt_keygen_type_a_ext_mask_dump(unit, 0, type_a_mask);

    type_a_mask = &ext_codes->type_a_mask[0];
    (void) _field_kt_keygen_type_a_ext_mask_dump(unit, 0, type_a_mask);

    type_b_ext = &ext_codes->type_b_extractors;
    (void)_field_ft_keygen_type_b_extractors_dump(unit, 1, type_b_ext);

    return rv;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_keygen_hw_extractors_dump
 *
 * Purpose:
 *   Dump hardware extractors keygen information.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    ext_codes  - (IN) extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_keygen_hw_extractors_dump(int unit,
    _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;

    if (ext_codes == NULL) {
        return BCM_E_NONE;
    }

    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit, "hw_ei[profile: %s]\n\r"),
            _keygen_profile_name(ext_codes->keygen_profile)));

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            return _field_ft_keygen_profile_aab_dump(unit, ext_codes);
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            return _field_ft_keygen_profile_ab_dump(unit, ext_codes);
        default:
            break;
    }
    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_group_keygen_remove
 *
 * Purpose:
 *   Remove flowtracker stage field group keygen extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    f_presel   - (IN) Field presel entry structure.
 *    tcam_idx   - (IN) Hw Index
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_group_keygen_remove(int unit,
        _field_presel_entry_t *f_presel,
        int hw_idx)
{
    int i = 0;

    static soc_mem_t keygen_mem_arr[] = {
        BSK_FTFP_LTS_MUX_CTRL_0m,
        BSK_FTFP_LTS_MASK_0m,
        BSK_FTFP_LTS_MUX_CTRL_1m,
        BSK_FTFP_LTS_MASK_1m,
        BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASKm
    };

    if ((f_presel == NULL) || (hw_idx < 0)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < COUNTOF(keygen_mem_arr); i++) {
        soc_mem_write(unit, keygen_mem_arr[i], MEM_BLOCK_ALL,
                hw_idx, soc_mem_entry_null(unit, keygen_mem_arr[i]));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_group_keygen_install
 *
 * Purpose:
 *   Install flowtracker stage field group keygen extractor codes.
 *
 * Parameters:
 *    unit       - (IN) BCM device number
 *    f_presel   - (IN) Field presel entry structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_group_keygen_install(int unit, _field_presel_entry_t *f_presel)
{
    int rv = BCM_E_NONE;
    int i = 0;
    int hw_idx = 0;
    int keygen_mem_count = 0;
    _field_ft_keygen_ext_codes_t *ext_codes = NULL;
    uint32 *ent_buf_arr[5] = {NULL};
    soc_mem_t *keygen_mem_arr = NULL;

    static soc_mem_t keygen_mem1_arr[] = {
        BSK_FTFP_LTS_MUX_CTRL_0m,
        BSK_FTFP_LTS_MASK_0m,
        BSK_FTFP_LTS_MUX_CTRL_1m,
        BSK_FTFP_LTS_MASK_1m,
        BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASKm
    };
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    static soc_mem_t keygen_mem2_arr[] = {
        BSK_FTFP2_LTS_MUX_CTRL_0m,
        BSK_FTFP2_LTS_MASK_0m,
        BSK_FTFP2_LTS_MUX_CTRL_1m,
        BSK_FTFP2_LTS_MASK_1m,
        BSK_FTFP2_LTS_MUX_CTRL_2_PLUS_MASKm
    };
#endif

    if (f_presel == NULL) {
        return BCM_E_PARAM;
    }
    if (f_presel->group->ft_ext_codes == NULL) {
        return BCM_E_NONE;
    }

    keygen_mem_arr = keygen_mem1_arr;
    keygen_mem_count = COUNTOF(keygen_mem1_arr);

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (f_presel->lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
        if (f_presel->lt_fs->slice_number == 1) {
            keygen_mem_arr = keygen_mem2_arr;
            keygen_mem_count = COUNTOF(keygen_mem2_arr);
        }
    }
#endif

    BCM_IF_ERROR_RETURN(_bcm_field_presel_entry_tcam_idx_get(unit,
                f_presel, f_presel->lt_fs, &hw_idx));

    /* Allocate memory buffers */
    for (i = 0; i < keygen_mem_count; i++) {
        ent_buf_arr[i] = sal_alloc(SOC_MAX_MEM_FIELD_WORDS, "Entry Buffer");
        if (ent_buf_arr[i] == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(ent_buf_arr[i], 0, SOC_MAX_MEM_FIELD_WORDS);
    }

    /* Set keygen memory entry buffers */
    ext_codes = f_presel->group->ft_ext_codes;
    rv = _bcm_field_ft_keygen_profile_mem_set(unit, ext_codes,
            keygen_mem_count, keygen_mem_arr, ent_buf_arr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Install at given Index */
    for (i = 0; i < keygen_mem_count; i++) {
        rv  = soc_mem_write(unit, keygen_mem_arr[i], MEM_BLOCK_ANY,
                hw_idx, ent_buf_arr[i]);
        if (SOC_FAILURE(rv)) {
            break;
        }
    }

cleanup:

    for (i = 0; i < keygen_mem_count; i++) {
        sal_free(ent_buf_arr[i]);
    }

    for (i = 0; i < keygen_mem_count; i++) {
        if (BCM_FAILURE(rv)) {
            soc_mem_write(unit, keygen_mem_arr[i], MEM_BLOCK_ALL,
                    hw_idx, soc_mem_entry_null(unit, keygen_mem_arr[i]));
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_keygen_type_a_shift_offsets_get
 *
 * Purpose:
 *    Calculate shift offsets for Keygen Type A extractor
 *    for given extractor codes.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    multiplex_num - (IN) Index of extractor in the keygen profile.
 *    extractors - (IN) extractor codes.
 *    shift_idx  - (IN/OUT) start index to fill shift_offsets_arr.
 *                  Also, updates to last fill index.
 *    shift_offsets_arr - (OUT) Shift offset array.
 *
 * Returns:
 *    None.
 */
STATIC void
_field_hx5_ft_keygen_type_a_shift_offsets_get(int unit,
                        int multiplex_num,
                        _field_ft_keygen_type_a_ext_t *extractors,
                        int *shift_idx,
                        int *shift_offsets_arr)
{
    int gran = 0;
    int e_num = 0;
    int base_offset = 0;

    if (multiplex_num == 0) {
        base_offset = 0;
    } else if (multiplex_num == 1) {
        base_offset = 128;
    }

    shift_offsets_arr[*shift_idx] = base_offset;
    for (e_num = 1; e_num < 10; e_num++) {
        gran = 0;
        if ((extractors->l2_e16_ext_mode[e_num - 1] == 0) ||
            (extractors->l1_e16_sel[e_num - 1] == -1)) {
            gran = 16;
        } else if (extractors->l2_e16_ext_mode[e_num - 1] == 1) {
            gran = 8;
        } else if (extractors->l2_e16_ext_mode[e_num - 1] == 2) {
            gran = 4;
        }
        (*shift_idx)++;
        shift_offsets_arr[(*shift_idx)] =
                    shift_offsets_arr[(*shift_idx) - 1] + gran;
    }
}

/*
 * Function:
 *    _field_hx5_ft_keygen_type_b_shift_offsets_get
 *
 * Purpose:
 *    Calculate shift offsets for Keygen Type B extractor
 *    for given extractor codes.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    multiplex_num - (IN) Index of extractor in the keygen profile.
 *    extractors - (IN) extractor codes.
 *    shift_idx  - (IN/OUT) start index to fill shift_offsets_arr.
 *                  Also, updates to last fill index.
 *    shift_offsets_arr - (OUT) Shift offset array.
 *
 * Returns:
 *    None.
 */
STATIC void
_field_hx5_ft_keygen_type_b_shift_offsets_get(int unit,
                    int multiplex_num,
                    _field_ft_keygen_type_b_ext_t *extractors,
                    int *shift_idx,
                    int *shift_offsets_arr)
{
    int e_num = 0;
    int base_offset = 0;

    if (multiplex_num == 1) {
        base_offset = 128;
    } else if (multiplex_num == 2) {
        base_offset = 256;
    }
    shift_offsets_arr[*shift_idx] = base_offset;
    for (e_num = 1; e_num < 8; e_num++) {
        (*shift_idx)++;
        shift_offsets_arr[(*shift_idx)] =
                shift_offsets_arr[(*shift_idx) - 1] + 16;
    }
}

/*
 * Function:
 *    _field_hx5_ft_keygen_extractors_shift_offsets_get
 *
 * Purpose:
 *    Calculate shift offsets for given extractor codes.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    ext_codes  - (IN) Extractor codes.
 *    shift_offset_arr - (OUT) shift offsets array.
 *
 * Returns:
 *    BCM_E_XXX
 */
void
_field_hx5_ft_keygen_ext_shift_offsets_get(int unit,
        _field_ft_keygen_ext_codes_t *ext_codes,
        int *shift_offsets_arr)
{
    int shift_idx = 0;
    _field_ft_keygen_type_a_ext_t *type_a_ext = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_ext = NULL;

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            type_a_ext = &ext_codes->type_a_extractors[0];
            (void) _field_hx5_ft_keygen_type_a_shift_offsets_get(unit,
                    0, type_a_ext, &shift_idx, &shift_offsets_arr[0]);
            shift_idx++;
            type_a_ext = &ext_codes->type_a_extractors[1];
            (void) _field_hx5_ft_keygen_type_a_shift_offsets_get(unit,
                    1, type_a_ext, &shift_idx, &shift_offsets_arr[0]);
            shift_idx++;
            type_b_ext = &ext_codes->type_b_extractors;
            (void) _field_hx5_ft_keygen_type_b_shift_offsets_get(unit,
                    2, type_b_ext, &shift_idx, &shift_offsets_arr[0]);
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            type_a_ext = &ext_codes->type_a_extractors[0];
            (void) _field_hx5_ft_keygen_type_a_shift_offsets_get(unit,
                    0, type_a_ext, &shift_idx, &shift_offsets_arr[0]);
            shift_idx++;
            type_b_ext = &ext_codes->type_b_extractors;
            (void) _field_hx5_ft_keygen_type_b_shift_offsets_get(unit,
                    1, type_b_ext, &shift_idx, &shift_offsets_arr[0]);
            break;
        default:
            return;
    }
    return;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_encode
 *
 * Purpose:
 *    This function converts keygen type A ext code into
 *    bytestream for purpose of saving.
 *
 * Parameters:
 *    uint          - (IN) Unit
 *    ext_codes     - (IN) Extractor codes.
 *    multiplex_num - (IN) Multiplexer number for this extractor.
 *    ext_ctrl_info - (IN/OUT) buffer.
 *    ext_index     - (IN/OUT) Index within buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_a_extractors_encode(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            int multiplex_num,
            bcmi_ft_hw_extractor_info_t *ext_ctrl_info,
            int *ext_index)
{
    int index = 0;
    int value = 0;
    int i = 0;
    _field_ft_keygen_type_a_ext_t *type_a_ext = NULL;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;

    index = *ext_index;

    type_a_ext = &ext_codes->type_a_extractors[multiplex_num];
    type_a_mask = &ext_codes->type_a_mask[multiplex_num];

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeA;
    ext_ctrl_info[index++] = multiplex_num;

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeAl1e16Sel;
    ext_ctrl_info[index++] = 10;
    for (i = 0; i < 10; i++) {
        value = type_a_ext->l1_e16_sel[i];
        ext_ctrl_info[index++] = value + 1;
    }
    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeAl2e16Mode;
    ext_ctrl_info[index++] = 10;
    for (i = 0; i < 10; i++) {
        value = type_a_ext->l2_e16_ext_mode[i];
        ext_ctrl_info[index++] = value + 1;
    }
    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeAl2e16Section;
    ext_ctrl_info[index++] = 10;
    for (i = 0; i < 10; i++) {
        value = type_a_ext->l2_e16_ext_section[i];
        ext_ctrl_info[index++] = value + 1;
    }
    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeAMask;
    ext_ctrl_info[index++] = 4;
    for (i = 0; i < 4; i++) {
        sal_memcpy(&ext_ctrl_info[index],
                &type_a_mask->mask[i], sizeof(uint32));
        index += 1;
    }

    *ext_index = index;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_extractors_encode
 *
 * Purpose:
 *    This function converts keygen type B ext code into
 *    bytestream for purpose of saving.
 *
 * Parameters:
 *    unit          - (IN) Unit number
 *    ext_codes     - (IN) Extractor codes.
 *    multiplex_num - (IN) Multiplexer number for this extractor.
 *    ext_ctrl_info - (IN/OUT) buffer.
 *    ext_index     - (IN/OUT) Index within buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_b_extractors_encode(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            int multiplex_num,
            bcmi_ft_hw_extractor_info_t *ext_ctrl_info,
            int *ext_index)
{
    int value = 0;
    int index = 0;
    int i = 0;
    _field_ft_keygen_type_b_ext_t *type_b_ext = NULL;

    index = *ext_index;

    type_b_ext = &ext_codes->type_b_extractors;

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeB;
    ext_ctrl_info[index++] = multiplex_num;

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeBl1e16Sel;
    ext_ctrl_info[index++] = 8;
    for (i = 0; i < 8; i++) {
        value = type_b_ext->l1_e16_sel[i];
        ext_ctrl_info[index++] = value + 1;
    }

    *ext_index = index;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_c_extractors_encode
 *
 * Purpose:
 *    This function converts keygen type C ext code into
 *    bytestream for purpose of saving.
 *
 * Parameters:
 *    unit          - (IN) Unit number
 *    ext_codes     - (IN) Extractor codes.
 *    multiplex_num - (IN) Multiplexer number for this extractor.
 *    ext_ctrl_info - (IN/OUT) buffer.
 *    ext_index     - (IN/OUT) Index within buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_c_extractors_encode(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            int multiplex_num,
            bcmi_ft_hw_extractor_info_t *ext_ctrl_info,
            int *ext_index)
{
    int value = 0;
    int index = 0;
    int i = 0;
    _field_ft_keygen_type_c_ext_t *type_c_ext = NULL;
    _field_ft_keygen_type_c_ext_mask_t *type_c_mask = NULL;

    index = *ext_index;

    type_c_ext = &ext_codes->type_c_extractors;
    type_c_mask = &ext_codes->type_c_mask;

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeC;
    ext_ctrl_info[index++] = multiplex_num;

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeCl1e1Sel;
    ext_ctrl_info[index++] = 16;
    for (i = 0; i < 16; i++) {
        value = type_c_ext->l1_e1_sel[i];
        ext_ctrl_info[index++] = value + 1;
    }

    ext_ctrl_info[index++] = _bcmFieldFtExtCodeTypeCMask;
    ext_ctrl_info[index++] = 2;
    sal_memcpy(&ext_ctrl_info[index],
                &type_c_mask->mask, sizeof(uint32));
    index += 1;

    *ext_index = index;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_extractors_encode
 *
 * Purpose:
 *    This function converts keygen extractors code into
 *    bytestream for purpose of saving.
 *
 * Parameters:
 *    uint          - (IN) Unit
 *    ext_codes     - (IN) Extractor codes.
 *    ext_count     - (OUT) Number of ext_ctrl_info bytes.
 *    ext_ctrl_info - (OUT) buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_extractors_encode(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            int *ext_count,
            bcmi_ft_hw_extractor_info_t **ext_ctrl_info)
{
    int count = 0;
    int index = 0;
    int ext_a_count = 0;
    int ext_b_count = 0;
    int ext_c_count = 0;

    ext_a_count = 1 + 1 +           /* Ext A Type */
                  1 + 1 + 10 +      /* Ext A L1 E16 Sel */
                  1 + 1 + 10 +      /* Ext A L2 E16 Mode */
                  1 + 1 + 10 +      /* Ext A L2 E16 Section */
                  1 + 1 + 4;        /* Ext A Mask */
    ext_b_count = 1 + 1 +           /* Ext B Type */
                  1 + 1 + 8;        /* Ext B L1 E16 Mask */
    ext_c_count = 1 + 1 +           /* Ext C Type */
                  1 + 1 + 16 +      /* Ext C L1 E1 Sel */
                  1 + 1 + 1;        /* Ext C Mask */

    if ((ext_codes == NULL) || (ext_count == NULL) || (ext_ctrl_info == NULL)) {
        return BCM_E_PARAM;
    }

    if (ext_codes->keygen_profile == _BCM_FIELD_FT_KEYGEN_PROFILE_AAB) {
        count = 1 + 1 + 2 * ext_a_count + ext_b_count;
    } else {
        count = 1 + 1 + ext_a_count + ext_b_count + ext_c_count;
    }

    _FP_XGS3_ALLOC(*ext_ctrl_info,
            count * sizeof (bcmi_ft_hw_extractor_info_t),
            " ext ctrl info");
    if (*ext_ctrl_info == NULL) {
        return BCM_E_MEMORY;
    }

    /* Write Keygen Profile */
    index = 0;
    (*ext_ctrl_info)[index++] = _bcmFieldFtExtCodeKeygenProfile;
    (*ext_ctrl_info)[index++] = ext_codes->keygen_profile;

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            _field_ft_keygen_type_a_extractors_encode(unit,
                    ext_codes, 0, *ext_ctrl_info, &index);
            _field_ft_keygen_type_a_extractors_encode(unit,
                    ext_codes, 1, *ext_ctrl_info, &index);
            _field_ft_keygen_type_b_extractors_encode(unit,
                    ext_codes, 0, *ext_ctrl_info, &index);
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            _field_ft_keygen_type_a_extractors_encode(unit,
                    ext_codes, 0, *ext_ctrl_info, &index);
            _field_ft_keygen_type_b_extractors_encode(unit,
                    ext_codes, 0, *ext_ctrl_info, &index);
            _field_ft_keygen_type_c_extractors_encode(unit,
                    ext_codes, 0, *ext_ctrl_info, &index);
            break;
    }
    *ext_count = index;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_extractors_decode
 *
 * Purpose:
 *    This function converts bytestream to keygen extractors code.
 *
 * Parameters:
 *    uint          - (IN) Unit
 *    ext_ctrl_info - (IN) buffer.
 *    ext_count     - (IN) Number of ext_ctrl_info bytes.
 *    ext_codes     - (OUT) Extractor codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_extractors_decode(int unit,
            bcmi_ft_hw_extractor_info_t *ext_ctrl_info,
            int ext_count,
            _field_ft_keygen_ext_codes_t *ext_codes)
{
    int value = 0;
    int index = 0;
    int e_index = 0;
    int i = 0;
    int count = 0;
    _field_ft_keygen_type_a_ext_t *type_a_ext = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_ext = NULL;
    _field_ft_keygen_type_c_ext_t *type_c_ext = NULL;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;
    _field_ft_keygen_type_c_ext_mask_t *type_c_mask = NULL;

    while(index < ext_count) {
        switch(ext_ctrl_info[index]) {
            case _bcmFieldFtExtCodeKeygenProfile:
                ext_codes->keygen_profile = ext_ctrl_info[index+1];
                index += 2;
                break;
            case _bcmFieldFtExtCodeTypeA:
                e_index = ext_ctrl_info[index+1];
                index += 2;
                break;
            case _bcmFieldFtExtCodeTypeAl1e16Sel:
                type_a_ext = &ext_codes->type_a_extractors[e_index];
                count = ext_ctrl_info[index+1];
                index += 2;
                for (i = 0; i < count; i++) {
                    value = ext_ctrl_info[i + index];
                    type_a_ext->l1_e16_sel[i] = value - 1;
                }
                index += count;
                break;
            case _bcmFieldFtExtCodeTypeAl2e16Mode:
                type_a_ext = &ext_codes->type_a_extractors[e_index];
                count = ext_ctrl_info[index+1];
                index += 2;
                for (i = 0; i < count; i++) {
                    value = ext_ctrl_info[i + index];
                    type_a_ext->l2_e16_ext_mode[i] = value - 1;
                }
                index += count;
                break;
            case _bcmFieldFtExtCodeTypeAl2e16Section:
                type_a_ext = &ext_codes->type_a_extractors[e_index];
                count = ext_ctrl_info[index+1];
                index += 2;
                for (i = 0; i < count; i++) {
                    value = ext_ctrl_info[i + index];
                    type_a_ext->l2_e16_ext_section[i] = value - 1;
                }
                index += count;
                break;
            case _bcmFieldFtExtCodeTypeAMask:
                type_a_mask = &ext_codes->type_a_mask[e_index];
                count = ext_ctrl_info[index+1];
                index += 2;
                for (i = 0; i < count; i++) {
                    sal_memcpy(&type_a_mask->mask[i],
                            &ext_ctrl_info[index], sizeof(uint32));
                    index += 1;
                }
                break;
            case _bcmFieldFtExtCodeTypeB:
                e_index = ext_ctrl_info[index+1];
                index += 2;
                break;
            case _bcmFieldFtExtCodeTypeBl1e16Sel:
                type_b_ext = &ext_codes->type_b_extractors;
                count = ext_ctrl_info[index+1];
                index += 2;
                for (i = 0; i < count; i++) {
                    value = ext_ctrl_info[i + index];
                    type_b_ext->l1_e16_sel[i] = value - 1;
                }
                index += count;
                break;
            case _bcmFieldFtExtCodeTypeC:
                e_index = ext_ctrl_info[index+1];
                index += 2;
                break;
            case _bcmFieldFtExtCodeTypeCl1e1Sel:
                type_c_ext = &ext_codes->type_c_extractors;
                count = ext_ctrl_info[index+1];
                index += 2;
                for (i = 0; i < count; i++) {
                    value = ext_ctrl_info[i + index];
                    type_c_ext->l1_e1_sel[i] = value - 1;
                }
                index += count;
                break;
            case _bcmFieldFtExtCodeTypeCMask:
                type_c_mask = &ext_codes->type_c_mask;
                count = ext_ctrl_info[index+1];
                index += 2;
                sal_memcpy(&type_c_mask->mask,
                        &ext_ctrl_info[index], sizeof(uint32));
                index += count;
                break;
            default:
                return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_session_key_profile_install
 *
 * Purpose:
 *    Install session key profile to hardware.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    user_profile-(IN) TRUE: Profile for user installed flows
 *    profiles   - (IN/OUT) session profile structure.
 *    ext_codes  - (IN) extractor codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_session_key_profile_install(int unit,
            int user_profile,
            bcmi_ft_session_profiles_t *profiles,
            _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int index = 0;
    uint32 profile_idx = 0;
    _field_stage_t *stage_fc = NULL;
    soc_profile_mem_t *profile = NULL;
    uint32 *ent_buf_arr[5] = {NULL};

    static soc_mem_t keygen_mem_arr[] = {
        BSK_SESSION_KEY_LTS_MUX_CTRL_0m,
        BSK_SESSION_KEY_LTS_MASK_0m,
        BSK_SESSION_KEY_LTS_MUX_CTRL_1m,
        BSK_SESSION_KEY_LTS_MASK_1m,
        BSK_SESSION_KEY_LTS_MUX_CTRL_2_PLUS_MASKm
    };

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                    _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    /* Allocate memory buffers */
    for (index = 0; index < COUNTOF(keygen_mem_arr); index++) {
        _FP_XGS3_ALLOC(ent_buf_arr[index],
                    SOC_MAX_MEM_FIELD_WORDS, "Entry Buffer");
        if (ent_buf_arr[index] == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
    }

    /* Set keygen memory entry buffers */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_field_ft_keygen_profile_mem_set(unit, ext_codes,
                    COUNTOF(keygen_mem_arr), keygen_mem_arr, ent_buf_arr);
    }

    /* Add to soc profile */
    if (BCM_SUCCESS(rv)) {
        if (user_profile == 1) {
            profile = &stage_fc->ft_info->session_key_user_profile;
        } else {
            profile = &stage_fc->ft_info->session_key_profile;
        }
        rv = soc_profile_mem_add(unit,
                    profile, (void **) ent_buf_arr, 1, &profile_idx);

        if (BCM_FAILURE(rv) && (profile->tables == NULL)) {
            rv = BCM_E_RESOURCE;
        }
    }

    if (BCM_SUCCESS(rv)) {
        profiles->session_key_profile_idx = (profile_idx + 1);
    }

cleanup:

    for (index = 0; index < COUNTOF(keygen_mem_arr); index++) {
        if (ent_buf_arr[index]) {
            sal_free(ent_buf_arr[index]);
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_session_data_profile_install
 *
 * Purpose:
 *    Install session data profile to hardware.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    id         - (IN) Flowtracker Group id
 *    profiles   - (IN/OUT) session profile structure.
 *    ext_codes  - (IN) extractor codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_session_data_profile_install(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_session_profiles_t *profiles,
            _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int index = 0;
    int mem_arr_count = 0;
    uint32 profile_idx = 0;
    _field_stage_t *stage_fc = NULL;
    soc_profile_mem_t *profile = NULL;
    uint32 *ent_buf_arr[3] = {NULL};
    soc_mem_t *mem_arr = NULL;

    soc_mem_t keygen_mem_arr[] = {
        BSK_SESSION_DATA_LTS_MUX_CTRL_0m,
        BSK_SESSION_DATA_LTS_MASK_0m,
        BSK_SESSION_DATA_LTS_MUX_CTRL_1_PLUS_MASKm
    };

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    soc_mem_t keygen_agg_mem_arr[] = {
        BSK_SESSION_DATA2_LTS_MUX_CTRL_0m,
        BSK_SESSION_DATA2_LTS_MASK_0m,
        BSK_SESSION_DATA2_LTS_MUX_CTRL_1_PLUS_MASKm
    };
#endif

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    if (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeNone) {
        mem_arr = &keygen_mem_arr[0];
        mem_arr_count = COUNTOF(keygen_mem_arr);
        profile = &stage_fc->ft_info->session_data_profile;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else {
        mem_arr = &keygen_agg_mem_arr[0];
        mem_arr_count = COUNTOF(keygen_agg_mem_arr);
        profile = &stage_fc->ft_info->agg_session_data_profile;
#endif
    }

    if ((mem_arr == NULL) || (mem_arr_count == 0)) {
        return BCM_E_PARAM;
    }

    /* Allocate memory buffers */
    for (index = 0; index < mem_arr_count; index++) {
        _FP_XGS3_ALLOC(ent_buf_arr[index],
                    SOC_MAX_MEM_FIELD_WORDS, "Entry Buffer");
        if (ent_buf_arr[index] == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
    }

    /* Set keygen memory entry buffers */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_field_ft_keygen_profile_mem_set(unit, ext_codes,
                    mem_arr_count, mem_arr, ent_buf_arr);
    }

    /* Add to soc profile */
    if (BCM_SUCCESS(rv)) {
        rv = soc_profile_mem_add(unit,
                    profile, (void **) ent_buf_arr, 1, &profile_idx);
    }

    if (BCM_SUCCESS(rv)) {
        profiles->session_data_profile_idx = (profile_idx + 1);
    }

cleanup:

    for (index = 0; index < mem_arr_count; index++) {
        if (ent_buf_arr[index]) {
            sal_free(ent_buf_arr[index]);
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_alu_data_profile_install
 *
 * Purpose:
 *    Install alu data profile to hardware.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    id         - (IN) Flowtracker Group id
 *    profiles   - (IN/OUT) session profile structure.
 *    ext_codes  - (IN) extractor codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_alu_data_profile_install(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_session_profiles_t *profiles,
            _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int index = 0;
    int mem_arr_count = 0;
    uint32 profile_idx = 0;
    _field_stage_t *stage_fc = NULL;
    soc_profile_mem_t *profile = NULL;
    uint32 *ent_buf_arr[1] = {NULL};
    soc_mem_t *mem_arr = NULL;

    soc_mem_t keygen_mem_arr[] = {
        BSK_ALU_DATA_LTS_MUX_CTRL_PLUS_MASKm
    };
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    soc_mem_t keygen_agg_mem_arr[] = {
        BSK_ALU_DATA2_LTS_MUX_CTRL_PLUS_MASKm
    };
#endif

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    if (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeNone) {
        mem_arr = &keygen_mem_arr[0];
        mem_arr_count = COUNTOF(keygen_mem_arr);
        profile = &stage_fc->ft_info->alu_data_profile;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else {
        mem_arr = &keygen_agg_mem_arr[0];
        mem_arr_count = COUNTOF(keygen_agg_mem_arr);
        profile = &stage_fc->ft_info->agg_alu_data_profile;
#endif
    }

    if ((mem_arr == NULL) || (mem_arr_count == 0)) {
        return BCM_E_PARAM;
    }

    /* Allocate memory buffers */
    for (index = 0; index < mem_arr_count; index++) {
        _FP_XGS3_ALLOC(ent_buf_arr[index],
                    SOC_MAX_MEM_FIELD_WORDS, "Entry Buffer");
        if (ent_buf_arr[index] == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
    }

    /* Set keygen memory entry buffers */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_field_ft_keygen_type_c_extractors_mem_set(unit,
                    ext_codes, mem_arr_count, mem_arr, ent_buf_arr);
    }

    /* Add to soc profile */
    if (BCM_SUCCESS(rv)) {
        rv = soc_profile_mem_add(unit, profile, (void **) ent_buf_arr,
                1, &profile_idx);
    }

    if (BCM_SUCCESS(rv)) {
        profiles->alu_data_profile_idx = (profile_idx + 1);
    }

cleanup:

    for (index = 0; index < mem_arr_count; index++) {
        if (ent_buf_arr[index]) {
            sal_free(ent_buf_arr[index]);
        }
    }

    return rv;
}
/*
 * Function:
 *    _field_hx5_ft_session_profiles_remove
 *
 * Purpose:
 *    Remove session profiles to hardware.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    id         - (IN) Flowtracker Group Id
 *    profiles   - (IN) Pointer to session Key/data profiles
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_session_profiles_remove(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_session_profiles_t *profiles)
{
    int rv = BCM_E_NONE;
    int user_entry = 0;
    int ref_count = 0;
    uint32 profile_index = 0;
    _field_stage_t *stage_fc = NULL;
    soc_profile_mem_t *key_profile = NULL;
    soc_profile_mem_t *data_profile = NULL;
    soc_profile_mem_t *alu_data_profile = NULL;

    /* Validate input params. */
    if (profiles == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    user_entry = (BCMI_FT_GROUP(unit, id)->flags
                      & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY);
    if (user_entry == 1) {
        key_profile = &stage_fc->ft_info->session_key_user_profile;
    } else {
        key_profile = &stage_fc->ft_info->session_key_profile;
    }

    if (BCMI_FT_GROUP_TYPE_GET(id) == bcmFlowtrackerGroupTypeNone) {
        data_profile = &stage_fc->ft_info->session_data_profile;
        alu_data_profile = &stage_fc->ft_info->alu_data_profile;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    } else {
        data_profile = &stage_fc->ft_info->agg_session_data_profile;
        alu_data_profile = &stage_fc->ft_info->agg_alu_data_profile;
#endif
    }

    if (profiles->session_key_profile_idx != 0) {
        profile_index = profiles->session_key_profile_idx - 1;

        rv = soc_profile_mem_ref_count_get(unit,
                key_profile, profile_index, &ref_count);
        BCM_IF_ERROR_RETURN(rv);
        if (ref_count) {
            rv = soc_profile_mem_delete(unit, key_profile, profile_index);
        }
        profiles->session_key_profile_idx = 0;
    }
    BCM_IF_ERROR_RETURN(rv);

    if (profiles->session_data_profile_idx != 0) {
        profile_index = profiles->session_data_profile_idx - 1;

        rv = soc_profile_mem_ref_count_get(unit,
                data_profile, profile_index, &ref_count);
        BCM_IF_ERROR_RETURN(rv);
        if (ref_count) {
            rv = soc_profile_mem_delete(unit, data_profile, profile_index);
        }
        profiles->session_data_profile_idx = 0;
    }
    BCM_IF_ERROR_RETURN(rv);

    if (profiles->alu_data_profile_idx != 0) {
        profile_index = profiles->alu_data_profile_idx - 1;
        rv = soc_profile_mem_ref_count_get(unit,
                alu_data_profile, profile_index, &ref_count);
        BCM_IF_ERROR_RETURN(rv);
        if (ref_count) {
            rv = soc_profile_mem_delete(unit, alu_data_profile, profile_index);
        }
        profiles->alu_data_profile_idx = 0;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_session_profiles_install
 *
 * Purpose:
 *    Install session profiles to hardware.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    id         - (IN) Flowtracker group id.
 *    profiles   - (OUT) Session Profiles.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_session_profiles_install(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_session_profiles_t *profiles)
{
    int rv = BCM_E_NONE;
    int ext_count = 0;
    int key_ext_count = 0;
    int data_ext_count = 0;
    bcmi_ft_hw_extractor_info_t *ext_ctrl_info = NULL;
    _field_ft_keygen_ext_codes_t ext_codes;

    /* Validate input params. */
    if (profiles == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
            bcmi_esw_ft_group_extraction_hw_info_get(unit,
                    1, id, 0, NULL, &key_ext_count));

    BCM_IF_ERROR_RETURN(
            bcmi_esw_ft_group_extraction_hw_info_get(unit,
                    0, id, 0, NULL, &data_ext_count));

    ext_count = (key_ext_count > data_ext_count) ?
                        key_ext_count : data_ext_count;

    _FP_XGS3_ALLOC(ext_ctrl_info,
            ext_count * sizeof(bcmi_ft_hw_extractor_info_t),
            " ext ctrl info");
    if (ext_ctrl_info == NULL) {
        return BCM_E_MEMORY;
    }

    /* Read Session Key extractor sel info */
    rv = bcmi_esw_ft_group_extraction_hw_info_get(unit, 1,
                id, ext_count, ext_ctrl_info, &key_ext_count);

    if (key_ext_count > 0) {
        _bcm_field_ft_keygen_extractors_init(&ext_codes);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_field_hx5_ft_extractors_decode(unit,
                    ext_ctrl_info, key_ext_count, &ext_codes);
        }

        _bcm_field_ft_keygen_extractors_init(&ext_codes);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_field_hx5_ft_extractors_decode(unit,
                    ext_ctrl_info, key_ext_count, &ext_codes);
        }

        if (BCM_SUCCESS(rv)) {
            rv = _field_hx5_ft_session_key_profile_install(unit,
                    (BCMI_FT_GROUP(unit, id)->flags
                     & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY),
                     profiles, &ext_codes);
        }
    }
    sal_memset(ext_ctrl_info, 0, ext_count * sizeof(uint8));
    _bcm_field_ft_keygen_extractors_init(&ext_codes);

    /* Read Session data + ALU Data extractors info */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_ft_group_extraction_hw_info_get(unit, 0,
                id, ext_count, ext_ctrl_info, &data_ext_count);
    }

    if (data_ext_count > 0) {
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_field_hx5_ft_extractors_decode(unit,
                    ext_ctrl_info, data_ext_count, &ext_codes);
        }

        /* Install session data profile */
        if (BCM_SUCCESS(rv)) {
            rv = _field_hx5_ft_session_data_profile_install(unit,
                    id, profiles, &ext_codes);
        }
    }

    /* Install Alu data profile. */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ft_alu_data_profile_install(unit,
                    id, profiles, &ext_codes);
    }

    if (ext_ctrl_info) {
        sal_free(ext_ctrl_info);
    }

    if (BCM_FAILURE(rv)) {
        (void) _field_hx5_ft_session_profiles_remove(unit,
                id, profiles);
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_keygen_ext_ctrl_info_count_get
 *
 * Purpose:
 *    Calculate number of keygen extractor info based on
 *    FT keygen hw extractor codes.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    ext_codes  - (IN) FT Keygen hw extractor codes.
 *    ext_ctrl_info_count - (OUT) Count of keygen algo extractor ctrl info.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_keygen_ext_ctrl_info_count_get(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            uint16 *ext_ctrl_info_count)
{
    int i = 0;
    int ext_num = 0;
    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_extractors = NULL;

    *ext_ctrl_info_count = 0;
    for (i = 0; i < 2; i++) {
        type_a_extractors = &ext_codes->type_a_extractors[i];
        for (ext_num = 0; ext_num < 10; ext_num++) {
            if (type_a_extractors->l1_e16_sel[ext_num] != -1) {
                *ext_ctrl_info_count += 2;
            }
        }
    }
    type_b_extractors = &ext_codes->type_b_extractors;
    for (ext_num = 0; ext_num < 8; ext_num++) {
        if (type_b_extractors->l1_e16_sel[ext_num] != -1) {
                *ext_ctrl_info_count +=2;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_ext_ctrl_info_get
 *
 * Purpose:
 *    Convert ft keygen type A extractor codes to keygen extractor info.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    type_b_extractors - (IN) Type B extractor struct.
 *    multiplex_num - (IN) Multiplexer number.
 *    level      - (IN) Level
 *    ext_ctrl_info - (INOUT) Keygen extractor control info.
 *    index      - (INOUT) index in ext_ctrl_info to start filling.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_type_a_ext_ctrl_info_get(int unit,
            _field_ft_keygen_type_a_ext_t *type_a_extractors,
            int multiplex_num,
            int level,
            bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info,
            uint16 *index)
{
    int ext_id = 0;
    int sec_num = 0;
    uint8 gran_arr[] = {16, 8, 4};

    for (sec_num = 0; sec_num < 10; sec_num++) {
        BCMI_FT_KEYGEN_EXT_ID_CREATE(sec_num, multiplex_num, ext_id);

        if (type_a_extractors->l1_e16_sel[sec_num] != -1) {
            if (level == 1) {
                ext_ctrl_info[*index].gran = 16;
                ext_ctrl_info[*index].ctrl_sel_val =
                    type_a_extractors->l1_e16_sel[sec_num];
            } else {
                ext_ctrl_info[*index].ctrl_sel_val =
                    type_a_extractors->l2_e16_ext_section[sec_num];
                ext_ctrl_info[*index].gran =
                    gran_arr[type_a_extractors->l2_e16_ext_mode[sec_num]];
            }
            ext_ctrl_info[*index].part = multiplex_num;
            ext_ctrl_info[*index].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
            ext_ctrl_info[*index].level = level;
            ext_ctrl_info[*index].ext_num = ext_id;
            (*index)++;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_ext_ctrl_info_get
 *
 * Purpose:
 *    Convert ft keygen type B extractor codes to keygen extractor info.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    type_b_extractors - (IN) Type B extractor struct.
 *    multiplex_num - (IN) Multiplexer number.
 *    level      - (IN) Level
 *    ext_ctrl_info - (INOUT) Keygen extractor control info.
 *    index      - (INOUT) index in ext_ctrl_info to start filling.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_ft_keygen_type_b_ext_ctrl_info_get(int unit,
            _field_ft_keygen_type_b_ext_t *type_b_extractors,
            int multiplex_num,
            int level,
            bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info,
            uint16 *index)
{
    int ext_id = 0;
    int sec_num = 0;

    for (sec_num = 0; sec_num < 8; sec_num++) {
        BCMI_FT_KEYGEN_EXT_ID_CREATE(sec_num, multiplex_num, ext_id);

        if (type_b_extractors->l1_e16_sel[sec_num] != -1) {
            ext_ctrl_info[*index].gran = 16;
            ext_ctrl_info[*index].ctrl_sel_val =
                type_b_extractors->l1_e16_sel[sec_num];
            ext_ctrl_info[*index].part = multiplex_num;
            ext_ctrl_info[*index].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
            ext_ctrl_info[*index].level = level;
            ext_ctrl_info[*index].ext_num = ext_id;
            (*index)++;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_keygen_ext_ctrl_info_get
 *
 * Purpose:
 *    Convert ft keygen hw extractor codes to keygen extractor info.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    ext_codes  - (IN) FT hw extractor codes
 *    keygen_cfg - (INOUT) keygen config struct.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_keygen_ext_ctrl_info_get(int unit,
            _field_ft_keygen_ext_codes_t *ext_codes,
            bcmi_keygen_ext_ctrl_sel_info_t **out_ext_ctrl_sel_info,
            uint16 *out_ext_ctrl_sel_info_count)
{
    int l = 0;
    uint16 index = 0;
    uint16 ext_ctrl_info_count = 0;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;

    /* Input parameters check. */
    if ((NULL == ext_codes) ||
            (NULL == out_ext_ctrl_sel_info) ||
            (NULL == out_ext_ctrl_sel_info_count)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_field_hx5_ft_keygen_ext_ctrl_info_count_get(unit,
                ext_codes, &ext_ctrl_info_count));
    if (ext_ctrl_info_count == 0) {
        return BCM_E_NONE;
    }
    _FP_XGS3_ALLOC(ext_ctrl_info, (ext_ctrl_info_count * \
                sizeof(bcmi_keygen_ext_ctrl_sel_info_t)),
            "Keygen Extractor control information");
    if (ext_ctrl_info == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(ext_ctrl_info, 0,
            ext_ctrl_info_count * sizeof(bcmi_keygen_ext_ctrl_sel_info_t));

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            for (l = 1; l <= 2; l++) {
                _field_ft_keygen_type_a_ext_ctrl_info_get(unit,
                        &ext_codes->type_a_extractors[0], 0, l, ext_ctrl_info, &index);
                _field_ft_keygen_type_a_ext_ctrl_info_get(unit,
                        &ext_codes->type_a_extractors[1], 1, l, ext_ctrl_info, &index);
                _field_ft_keygen_type_b_ext_ctrl_info_get(unit,
                        &ext_codes->type_b_extractors, 2, l, ext_ctrl_info, &index);
            }
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            for (l = 1; l <= 2; l++) {
                _field_ft_keygen_type_a_ext_ctrl_info_get(unit,
                        &ext_codes->type_a_extractors[0], 0, l, ext_ctrl_info, &index);
                _field_ft_keygen_type_b_ext_ctrl_info_get(unit,
                        &ext_codes->type_b_extractors, 1, l, ext_ctrl_info, &index);
            }
            break;
        default:
            sal_free(ext_ctrl_info);
            *out_ext_ctrl_sel_info = NULL;
            *out_ext_ctrl_sel_info_count = 0;
            return BCM_E_INTERNAL;
    }

    *out_ext_ctrl_sel_info = ext_ctrl_info;
    *out_ext_ctrl_sel_info_count = ext_ctrl_info_count;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_keygen_ext_ctrl_info_skip
 *
 * Purpose:
 *    keygen config extractor info for skipped qualifiers.
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    keygen_cfg - (INOUT) keygen config struct.
 *    skipped_size- (OUT) Number of bits skipped due to
 *                  skipping extractor codes for qualifier.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_keygen_ext_ctrl_info_skip(int unit,
            bcmi_keygen_cfg_t *keygen_cfg,
            uint16 *skipped_size)
{
    uint16 ext_ctrl_info_count = 0;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;

    /* Input parameters check. */
    if (NULL == keygen_cfg) {
        return (BCM_E_PARAM);
    }

    /* 1 extractor skipped at Level 1 and 2 each. */
    ext_ctrl_info_count = 2;
    _FP_XGS3_ALLOC(ext_ctrl_info,
            ext_ctrl_info_count * sizeof(bcmi_keygen_ext_ctrl_sel_info_t),
            "Keygen Extractor control information");
    if (ext_ctrl_info == NULL) {
        return BCM_E_MEMORY;
    }
    /* Fill Up ext ctrl sel info for InPort */
    ext_ctrl_info[0].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
    ext_ctrl_info[0].ctrl_sel_val = 133;
    ext_ctrl_info[0].part = 0;
    ext_ctrl_info[0].level = 1;
    ext_ctrl_info[0].gran = 16;
    BCMI_FT_KEYGEN_EXT_ID_CREATE(0, 0, ext_ctrl_info[0].ext_num);
    ext_ctrl_info[1].ctrl_sel = BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
    ext_ctrl_info[1].ctrl_sel_val = 0;
    ext_ctrl_info[1].part = 0;
    ext_ctrl_info[1].level = 2;
    ext_ctrl_info[1].gran = 8;
    BCMI_FT_KEYGEN_EXT_ID_CREATE(0, 0, ext_ctrl_info[1].ext_num);

    keygen_cfg->ext_ctrl_sel_info_count = ext_ctrl_info_count;
    keygen_cfg->ext_ctrl_sel_info = ext_ctrl_info;
    *skipped_size = 8;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_session_profiles_reference
 *
 * Purpose:
 *    Update references of profiles after warmboot
 *
 * Parameters:
 *    uint       - (IN) Unit
 *    id         - (IN) Flowtracker Group
 *    profiles   - (IN/OUT) session profile structure.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_session_profiles_reference(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_session_profiles_t *profiles)
{
    int rv = BCM_E_NONE;
    uint8 user_profile = 0;
    uint32 profile_idx = 0;
    _field_stage_t *stage_fc = NULL;
    soc_profile_mem_t *profile = NULL;

   /* Get flowtracker stage */
     BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                        _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    if (stage_fc->ft_info == NULL) {
        return BCM_E_INTERNAL;
    }

    user_profile = BCMI_FT_GROUP(unit, id)->flags
                    & BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY;

    /* Session Key Profile */
    if (profiles->session_key_profile_idx != 0) {
        profile_idx = profiles->session_key_profile_idx - 1;
        if (user_profile == 1) {
            profile = &stage_fc->ft_info->session_key_user_profile;
        } else {
            profile = &stage_fc->ft_info->session_key_profile;
        }
        rv  = soc_profile_mem_reference(unit, profile,
            profile_idx, 1);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Session Data Profile */
    if (profiles->session_data_profile_idx != 0) {
        profile_idx = profiles->session_data_profile_idx - 1;
        profile = &stage_fc->ft_info->session_data_profile;
        rv  = soc_profile_mem_reference(unit, profile,
            profile_idx, 1);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Alu Data Profile */
    if (profiles->alu_data_profile_idx != 0) {
        profile_idx = profiles->alu_data_profile_idx - 1;
        profile = &stage_fc->ft_info->alu_data_profile;
        rv  = soc_profile_mem_reference(unit, profile,
            profile_idx, 1);
        BCM_IF_ERROR_RETURN(rv);
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_mem_read
 *
 * Purpose:
 *   Read memory fields for keygen type A extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem        - (IN) keygen type A memory.
 *    ent_buf    - (IN) entry buffer
 *    extractors - (OUT) Type A extractor select code.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_a_extractors_mem_read(int unit,
        soc_mem_t mem,
        uint32 *ent_buf,
        _field_ft_keygen_type_a_ext_t *extractors)
{
    int i = 0;

    soc_field_t l1_e16_sel[] = {
        EXTRACT_TYPE_A_0_L1_E16_SELf,
        EXTRACT_TYPE_A_1_L1_E16_SELf,
        EXTRACT_TYPE_A_2_L1_E16_SELf,
        EXTRACT_TYPE_A_3_L1_E16_SELf,
        EXTRACT_TYPE_A_4_L1_E16_SELf,
        EXTRACT_TYPE_A_5_L1_E16_SELf,
        EXTRACT_TYPE_A_6_L1_E16_SELf,
        EXTRACT_TYPE_A_7_L1_E16_SELf,
        EXTRACT_TYPE_A_8_L1_E16_SELf,
        EXTRACT_TYPE_A_9_L1_E16_SELf
    };

    soc_field_t l2_e16_ext_mode[] = {
        EXTRACT_TYPE_A_0_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_1_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_2_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_3_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_4_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_5_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_6_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_7_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_8_L2_E16_EXTRACT_MODEf,
        EXTRACT_TYPE_A_9_L2_E16_EXTRACT_MODEf
    };

    soc_field_t l2_e16_ext_section[] = {
        EXTRACT_TYPE_A_0_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_1_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_2_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_3_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_4_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_5_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_6_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_7_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_8_L2_E16_EXTRACT_SECTIONf,
        EXTRACT_TYPE_A_9_L2_E16_EXTRACT_SECTIONf
    };

    if ((mem == INVALIDm) || (ent_buf == NULL) || (extractors == NULL)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < 10; i++) {
        extractors->l1_e16_sel[i] = soc_mem_field32_get(unit,
                mem, ent_buf, l1_e16_sel[i]);
        extractors->l2_e16_ext_mode[i] = soc_mem_field32_get(unit,
                mem, ent_buf, l2_e16_ext_mode[i]);
        extractors->l2_e16_ext_section[i] = soc_mem_field32_get(unit,
                mem, ent_buf, l2_e16_ext_section[i]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_mask_mem_read
 *
 * Purpose:
 *   Read memory fields for keygen type A extractors mask.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem        - (IN) keygen type A mask memory.
 *    ent_buf    - (IN) entry buffer
 *    type_a_mask - (OUT) Type A extractor select code mask.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_a_extractors_mask_mem_read(int unit,
        soc_mem_t mem,
        uint32 *ent_buf,
        _field_ft_keygen_type_a_ext_mask_t *type_a_mask)
{
    soc_field_t mask_field[] = {
        MASKf
    };

    if ((mem == INVALIDm) || (ent_buf == NULL) || (type_a_mask == NULL)) {
        return BCM_E_PARAM;
    }

    soc_mem_field_get(unit, mem, ent_buf,
                    mask_field[0], type_a_mask->mask);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_extractors_update
 *
 * Purpose:
 *   Update keygen type A extractors info based on mask.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    type_a_mask - (IN) Type A extractor select code mask.
 *    extractors - (OUT) Type A extractors.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_a_extractors_update(int unit,
        _field_ft_keygen_type_a_ext_mask_t *type_a_mask,
        _field_ft_keygen_type_a_ext_t *extractors)
{
    int i = 0;
    int gran = 0;
    int offset = 0;
    int unused = FALSE;
    uint32 mask = 0;

    if ((type_a_mask == NULL) || (extractors == NULL)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < 10; i++) {
        if (extractors->l2_e16_ext_mode[i] == 0) {
            gran = 16;
        } else if (extractors->l2_e16_ext_mode[i] == 1) {
            gran = 8;
        } else if (extractors->l2_e16_ext_mode[i] == 2) {
            gran = 4;
        }
        unused = FALSE;
        if (offset >= 128) {
           unused = TRUE;
        } else if (extractors->l1_e16_sel[i] == 0) {
            /* If bits are not set in mask, extractor is unused. */
            gran = ((offset + gran) >= 128) ? (128 - offset) : gran;
            SHR_BITTEST_RANGE(type_a_mask->mask, offset, gran, mask);
            if (mask == 0) {
                unused = TRUE;
            }
        }
        if (TRUE == unused) {
            extractors->l1_e16_sel[i] = -1;
            extractors->l2_e16_ext_mode[i] = -1;
            extractors->l2_e16_ext_section[i] = -1;
        }
        offset += gran;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_b_extractors_mem_read
 *
 * Purpose:
 *   Read memory fields for keygen type B extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem        - (IN) keygen type B memory.
 *    ent_buf    - (IN) entry buffer
 *    extractors - (OUT) Type B extractor select codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_b_extractors_mem_read(int unit,
        soc_mem_t mem,
        uint32 *ent_buf,
        _field_ft_keygen_type_b_ext_t *extractors)
{
    int rv = BCM_E_NONE;
    int i = 0;
    uint32 en = 0;

    soc_field_t l1_e16_sel[] = {
        EXTRACT_TYPE_B_0_L1_E16_SELf,
        EXTRACT_TYPE_B_1_L1_E16_SELf,
        EXTRACT_TYPE_B_2_L1_E16_SELf,
        EXTRACT_TYPE_B_3_L1_E16_SELf,
        EXTRACT_TYPE_B_4_L1_E16_SELf,
        EXTRACT_TYPE_B_5_L1_E16_SELf,
        EXTRACT_TYPE_B_6_L1_E16_SELf,
        EXTRACT_TYPE_B_7_L1_E16_SELf
    };
    soc_field_t l1_e16_en[] = {
        EXTRACT_0_ENABLEf,
        EXTRACT_1_ENABLEf,
        EXTRACT_2_ENABLEf,
        EXTRACT_3_ENABLEf,
        EXTRACT_4_ENABLEf,
        EXTRACT_5_ENABLEf,
        EXTRACT_6_ENABLEf,
        EXTRACT_7_ENABLEf
    };

    if ((mem == INVALIDf) || (ent_buf == NULL) || (extractors == NULL)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < 8; i++) {
        en = soc_mem_field32_get(unit, mem,
                    ent_buf, l1_e16_en[i]);
        if (en == 1) {
            extractors->l1_e16_sel[i] = soc_mem_field32_get(unit,
                    mem, ent_buf, l1_e16_sel[i]);
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_c_extractors_mem_read
 *
 * Purpose:
 *   Read memory fields for keygen type C extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem        - (IN) keygen type B memory.
 *    ent_buf    - (IN) entry buffer
 *    extractors - (OUT) Type C extractor select codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_c_extractors_mem_read(int unit,
        soc_mem_t mem,
        uint32 *ent_buf,
        _field_ft_keygen_type_c_ext_t *extractors)
{
    int rv = BCM_E_NONE;
    int i = 0;

    soc_field_t l1_e1_sel[]  = {
        EXTRACT_TYPE_C_0_L1_E1_SELf,
        EXTRACT_TYPE_C_1_L1_E1_SELf,
        EXTRACT_TYPE_C_2_L1_E1_SELf,
        EXTRACT_TYPE_C_3_L1_E1_SELf,
        EXTRACT_TYPE_C_4_L1_E1_SELf,
        EXTRACT_TYPE_C_5_L1_E1_SELf,
        EXTRACT_TYPE_C_6_L1_E1_SELf,
        EXTRACT_TYPE_C_7_L1_E1_SELf,
        EXTRACT_TYPE_C_8_L1_E1_SELf,
        EXTRACT_TYPE_C_9_L1_E1_SELf,
        EXTRACT_TYPE_C_10_L1_E1_SELf,
        EXTRACT_TYPE_C_11_L1_E1_SELf,
        EXTRACT_TYPE_C_12_L1_E1_SELf,
        EXTRACT_TYPE_C_13_L1_E1_SELf,
        EXTRACT_TYPE_C_14_L1_E1_SELf,
        EXTRACT_TYPE_C_15_L1_E1_SELf
    };

    if ((mem == INVALIDm) || (ent_buf == NULL) || (extractors == NULL)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < COUNTOF(l1_e1_sel); i++) {
        extractors->l1_e1_sel[i] = soc_mem_field32_get(unit,
            mem, ent_buf, l1_e1_sel[i]);
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_type_c_extractors_mask_mem_read
 *
 * Purpose:
 *   Set memory fields for keygen type C extractors mask.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem        - (IN) keygen type C mask memory.
 *    ent_buf    - (IN) entry buffer
 *    extractors - (OUT) Type C extractor select code mask.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_type_c_extractors_mask_mem_read(int unit,
        soc_mem_t mem,
        uint32 *ent_buf,
        _field_ft_keygen_type_c_ext_mask_t *type_c_mask)
{
    soc_field_t mask_field[] = {
        MASKf
    };

    if ((mem == INVALIDm) || (ent_buf ==NULL) || (type_c_mask ==NULL)) {
        return BCM_E_PARAM;
    }

    type_c_mask->mask = soc_mem_field32_get(unit,
            mem, ent_buf, mask_field[0]);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_profile_aab_mem_read
 *
 * Purpose:
 *   Read memory fields for keygen Profile AAB extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem_arr    - (IN) Profile AAB memory array.
 *    ent_buf_arr- (IN) Profiel AAB entry buffer array.
 *    ext_codes  - (OUT) Extractor select codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_profile_aab_mem_read(int unit,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    uint32 *ent_buf = NULL;

    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;
    _field_ft_keygen_type_a_ext_mask_t  *type_a_mask = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_extractors = NULL;

    if ((mem_arr == NULL) || (ent_buf_arr == NULL) || (ext_codes == NULL)) {
        return BCM_E_PARAM;
    }

    /* Multiplex_num 0 Keygen Type A */
    ent_buf = ent_buf_arr[0];
    type_a_extractors = &ext_codes->type_a_extractors[0];
    rv = _field_ft_keygen_type_a_extractors_mem_read(unit,
                mem_arr[0], ent_buf, type_a_extractors);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 0 Keygen Type A Mask */
    ent_buf = ent_buf_arr[1];
    type_a_mask = &ext_codes->type_a_mask[0];
    rv = _field_ft_keygen_type_a_extractors_mask_mem_read(unit,
                mem_arr[1], ent_buf, type_a_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Disable extractors if mask not set */
    rv = _field_ft_keygen_type_a_extractors_update(unit,
                type_a_mask, type_a_extractors);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 1 Keygen Type A */
    ent_buf = ent_buf_arr[2];
    type_a_extractors = &ext_codes->type_a_extractors[1];
    rv = _field_ft_keygen_type_a_extractors_mem_read(unit,
                mem_arr[2], ent_buf, type_a_extractors);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 1 Keygen Type A Mask */
    ent_buf = ent_buf_arr[3];
    type_a_mask = &ext_codes->type_a_mask[1];
    rv = _field_ft_keygen_type_a_extractors_mask_mem_read(unit,
                mem_arr[3], ent_buf, type_a_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Disable extractors if mask not set */
    rv = _field_ft_keygen_type_a_extractors_update(unit,
                type_a_mask, type_a_extractors);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 2 Keygen Type B */
    ent_buf = ent_buf_arr[4];
    type_b_extractors = &ext_codes->type_b_extractors;
    rv = _field_ft_keygen_type_b_extractors_mem_set(unit,
                type_b_extractors, mem_arr[4], ent_buf);

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_profile_ab_mem_read
 *
 * Purpose:
 *    Read memory fields for keygen Profile AB extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem_arr    - (IN) Profile AB memory array.
 *    ent_buf_arr- (IN) Profiel AB entry buffer array.
 *    ext_codes  - (OUT) Extractor select codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_profile_ab_mem_read(int unit,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    uint32 *ent_buf = NULL;
    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;
    _field_ft_keygen_type_a_ext_mask_t *type_a_mask = NULL;
    _field_ft_keygen_type_b_ext_t *type_b_extractors = NULL;

    if ((mem_arr == NULL) || (ent_buf_arr == NULL) || (ext_codes == NULL)) {
        return BCM_E_PARAM;
    }

    /* Multiplex_num 0 Keygen Type A */
    ent_buf = ent_buf_arr[0];
    type_a_extractors = &ext_codes->type_a_extractors[0];
    rv = _field_ft_keygen_type_a_extractors_mem_set(unit,
                type_a_extractors, mem_arr[0], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 0 Keygen Type A Mask */
    ent_buf = ent_buf_arr[1];
    type_a_mask = &ext_codes->type_a_mask[0];
    rv = _field_ft_keygen_type_a_extractors_mask_mem_set(unit,
                type_a_mask, mem_arr[1], ent_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Disable extractors if mask not set */
    rv = _field_ft_keygen_type_a_extractors_update(unit,
                type_a_mask, type_a_extractors);
    BCM_IF_ERROR_RETURN(rv);

    /* Multiplex_num 1 Keygen Type B */
    ent_buf = ent_buf_arr[2];
    type_b_extractors = &ext_codes->type_b_extractors;
    rv = _field_ft_keygen_type_b_extractors_mem_set(unit,
                type_b_extractors, mem_arr[2], ent_buf);

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_keygen_profile_mem_read
 *
 * Purpose:
 *   Read memory fields for given keygen Profile extractors.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem_count  - (IN) Number of memories in keygen profile.
 *    mem_arr    - (IN) Profile AB memory array.
 *    ent_buf_arr- (IN) entry buffer array.
 *    ext_codes  - (OUT) Extractor select codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_keygen_profile_mem_read(int unit,
        int mem_count,
        soc_mem_t *mem_arr,
        uint32 **ent_buf_arr,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;

    switch(ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
            if (mem_count != 5) {
                return BCM_E_INTERNAL;
            }
            rv = _field_ft_keygen_profile_aab_mem_read(unit,
                        mem_arr, ent_buf_arr, ext_codes);
            break;
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AB:
            if (mem_count != 3) {
                return BCM_E_INTERNAL;
            }
            rv = _field_ft_keygen_profile_ab_mem_read(unit,
                        mem_arr, ent_buf_arr, ext_codes);
            break;
        default:
            rv = BCM_E_INTERNAL;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_keygen_mem_buf_read
 *
 * Purpose:
 *   Read all entries in keygen memories
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    mem_count  - (IN) Number of memories.
 *    mem_arr    - (IN) Profile AB memory array.
 *    keygen_mem_buf_arr- (OUT) Pointer to keygen mem buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_keygen_mem_buf_read(int unit,
        int mem_count,
        soc_mem_t *mem_arr,
        uint32 **keygen_mem_buf_arr)
{
    int rv = BCM_E_NONE;
    int i = 0;
    int index_min = -1;
    int index_max = -1;

    for (i = 0; i < mem_count; i++) {
        keygen_mem_buf_arr[i] = soc_cm_salloc(unit,
            SOC_MEM_TABLE_BYTES(unit, mem_arr[i]),
            "FT keygen mem buffer");
        if (NULL == keygen_mem_buf_arr[i]) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        index_min = soc_mem_index_min(unit, mem_arr[i]);
        index_max = soc_mem_index_max(unit, mem_arr[i]);
        rv = soc_mem_read_range(unit, mem_arr[i], MEM_BLOCK_ALL,
                    index_min, index_max, keygen_mem_buf_arr[i]);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

cleanup:

    if (BCM_FAILURE(rv)) {
        for (i = 0; i < mem_count; i++) {
            if (NULL != keygen_mem_buf_arr[i]) {
               soc_cm_sfree(unit, keygen_mem_buf_arr[i]);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_group_ext_codes_recover
 *
 * Purpose:
 *    Recover ft ext codes in a field group.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    fg         - (IN/OUT) Field Group.
 *    keygen_mem_buf_arr- (OUT) Pointer to keygen mem buffer.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_group_ext_codes_recover(int unit,
        _field_group_t *fg,
        uint32 **keygen_mem_buf_arr)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int hw_idx = 0;
    _field_presel_entry_t *f_presel = NULL;
    uint32 *ent_buf_arr[5] = {NULL};

    soc_mem_t keygen_mem_arr[] = {
        BSK_FTFP_LTS_MUX_CTRL_0m,
        BSK_FTFP_LTS_MASK_0m,
        BSK_FTFP_LTS_MUX_CTRL_1m,
        BSK_FTFP_LTS_MASK_1m,
        BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASKm
    };

    for (idx = 0; idx <_FP_PRESEL_ENTRIES_MAX_PER_GROUP; idx++) {
        f_presel = fg->presel_ent_arr[idx];
        if (f_presel != NULL) {
            break;
        }
    }

    if (f_presel != NULL) {
        _FP_XGS3_ALLOC(fg->ft_ext_codes,
                sizeof (_field_ft_keygen_ext_codes_t),
                " fp ft ext codes");
        if (fg->ft_ext_codes == NULL) {
            return BCM_E_MEMORY;
        }

        hw_idx = f_presel->hw_index;

        for (idx = 0; idx < COUNTOF(keygen_mem_arr); idx++) {
            ent_buf_arr[idx] = soc_mem_table_idx_to_pointer(unit,
                    keygen_mem_arr[idx], void *, keygen_mem_buf_arr[idx], hw_idx);
        }

        _bcm_field_ft_keygen_extractors_init(fg->ft_ext_codes);
        fg->ft_ext_codes->keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AAB;
        rv = _bcm_field_ft_keygen_profile_mem_read(unit,
                COUNTOF(keygen_mem_arr), keygen_mem_arr,
                ent_buf_arr, fg->ft_ext_codes);
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_group_ext_codes_recover
 *
 * Purpose:
 *    Recover ft ext codes in a field groups
 *    in flowtracker stage.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_group_ext_codes_recover(int unit)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    _field_group_t *fg = NULL;
    _field_control_t *fc = NULL;
    _field_stage_t *stage_fc = NULL;
    uint32 *keygen_mem_buf_arr[5] = {NULL};

    soc_mem_t keygen_mem_arr[] = {
        BSK_FTFP_LTS_MUX_CTRL_0m,
        BSK_FTFP_LTS_MASK_0m,
        BSK_FTFP_LTS_MUX_CTRL_1m,
        BSK_FTFP_LTS_MASK_1m,
        BSK_FTFP_LTS_MUX_CTRL_2_PLUS_MASKm
    };

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    /* Read all memories */
    rv = _bcm_field_ft_keygen_mem_buf_read(unit,
            COUNTOF(keygen_mem_arr), keygen_mem_arr, keygen_mem_buf_arr);
    BCM_IF_ERROR_RETURN(rv);

    for (fg = fc->groups; fg != NULL; fg = fg->next) {
        if (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
            continue;
        }
        rv = _field_ft_group_ext_codes_recover(unit, fg, keygen_mem_buf_arr);
        if (BCM_FAILURE(rv)) {
            break;
        }

        rv = _field_hx5_ft_group_keytype_bmp_recover(unit, stage_fc, fg);
        if (BCM_FAILURE(rv)) {
            break;
        }

    }

    /* Free cm alloc */
    for (idx = 0; idx < COUNTOF(keygen_mem_arr); idx++) {
        if (NULL != keygen_mem_buf_arr[idx]) {
            soc_cm_sfree(unit, keygen_mem_buf_arr[idx]);
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_hx5_keygen_oper_qual_offset_info_merge
 *
 * Purpose:
 *    Merge qual_offset_info from keygen_oper and
 *    qual_offset_info of skipped qualifier.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_hx5_keygen_oper_qual_offset_info_merge(
        int unit,
        int keygen_parts_count,
        bcmi_keygen_oper_t *keygen_oper,
        bcmi_keygen_qual_offset_info_t *add_qual_offset_info)
{
    int keygen_part = 0;
    int additional_size = 0;
    int current_size = 0, new_size = 0;
    bcmi_keygen_qual_offset_info_t *curr_qual_offset_info = NULL;
    bcm_field_qualify_t *qid_arr = NULL;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;

    /* Loop through each keygen parts */
    for (keygen_part = 0; keygen_part < keygen_parts_count; keygen_part++) {
        additional_size = add_qual_offset_info[keygen_part].size;
        if (additional_size == 0) {
            continue;
        }

        /* Get Current size */
        curr_qual_offset_info = &(keygen_oper->qual_offset_info[keygen_part]);
        current_size = curr_qual_offset_info->size;

        /* Allocate new memory to merge */
        qid_arr = NULL;
        offset_arr = NULL;
        new_size = current_size + additional_size;
        _FP_XGS3_ALLOC(qid_arr, new_size * sizeof(bcm_field_qualify_t),
            "Group Qualifier Info");
        _FP_XGS3_ALLOC(offset_arr, new_size * sizeof(bcmi_keygen_qual_offset_t),
            "Group Qualifier Offset Info");

        /* Copy current qual_offset_info */
        sal_memcpy(qid_arr, curr_qual_offset_info->qid_arr,
                    current_size * sizeof (bcm_field_qualify_t));
        sal_memcpy(offset_arr, curr_qual_offset_info->offset_arr,
                    current_size * sizeof(bcmi_keygen_qual_offset_t));

        /* Copy additional qual_offset_info */
        sal_memcpy(qid_arr + current_size,
                    add_qual_offset_info[keygen_part].qid_arr,
                    additional_size * sizeof (bcm_field_qualify_t));
        sal_memcpy(offset_arr + current_size,
                    add_qual_offset_info[keygen_part].offset_arr,
                    additional_size * sizeof(bcmi_keygen_qual_offset_t));

        /* update current qual offset info */
        sal_free(curr_qual_offset_info->qid_arr);
        sal_free(curr_qual_offset_info->offset_arr);

        keygen_oper->qual_offset_info[keygen_part].size = new_size;
        keygen_oper->qual_offset_info[keygen_part].qid_arr = qid_arr;
        keygen_oper->qual_offset_info[keygen_part].offset_arr = offset_arr;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_type_a_filler_extractors_set
 *
 * Purpose:
 *    Enable filler extractors to fill unused bits
 *    to offset static qualifier extraction info
 *    in keytype A extractor.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *
 * Returns:
 *    BCM_E_XXX
 *
 * Notes:
 */
int
_field_ft_keygen_type_a_filler_extractors_set(
        int unit,
        _field_ft_keygen_type_a_ext_t *extractors)
{
    int ext_num = 0;
    uint8 gran = 0;
    int used_bits = 0,unused_bits = 0;
    int free_extractors = 0;
    int free_ext_num = 0, filler_extractor_required =FALSE;
    int used_extractors = 0, unused_extractors = 0;
    int filler_4bit_exts = 0, filler_8bit_exts = 0, filler_16bit_exts = 0;

    for (ext_num = 0; ext_num < 10; ext_num++) {
        if (extractors->l1_e16_sel[ext_num] == -1) {
            free_extractors++;
            /* If this is first free extractor, note down ext_num */
            if (free_extractors == 1) {
                free_ext_num = ext_num;
            }
            continue;
        }
        /* Cacluate in-use extractors and used bits */
        if (extractors->l2_e16_ext_mode[ext_num] == 0) {
            gran = 16;
        } else if (extractors->l2_e16_ext_mode[ext_num] == 1) {
            gran = 8;
        } else if (extractors->l2_e16_ext_mode[ext_num] == 2) {
            gran = 4;
        }
        used_bits += gran; used_extractors++;

        /* If this extractor is after free extractor, need to use dummy */
        if (free_extractors != 0) {
            filler_extractor_required  = TRUE;
        }
    }

    if ((filler_extractor_required == TRUE) && (used_bits != 128)) {
        unused_extractors = 10 - used_extractors;
        unused_bits = 128 - used_bits;

        filler_16bit_exts = (unused_bits / 8) - unused_extractors;

        if (filler_16bit_exts <= 0) {
            /* If 16b extractors <= 0 , use 8b and 4b */
            filler_16bit_exts = 0;

            filler_8bit_exts = (unused_bits / 4) - unused_extractors;
            filler_4bit_exts = free_extractors - filler_8bit_exts;
        } else {
            filler_8bit_exts = free_extractors - filler_16bit_exts;
            filler_4bit_exts = 0;
        }

        if (unused_extractors !=
            (filler_16bit_exts + filler_8bit_exts + filler_4bit_exts)) {
            return BCM_E_INTERNAL;
        }

        /* Enable 4-bit filler extractor */
        while(filler_4bit_exts) {
            extractors->l1_e16_sel[free_ext_num] = 0;
            extractors->l2_e16_ext_mode[free_ext_num] = 2;
            extractors->l2_e16_ext_section[free_ext_num] = 0;
            filler_4bit_exts--;
            free_ext_num++;
        }

        /* Enable 8-bit filler extractor */
        while(filler_8bit_exts) {
            extractors->l1_e16_sel[free_ext_num] = 0;
            extractors->l2_e16_ext_mode[free_ext_num] = 1;
            extractors->l2_e16_ext_section[free_ext_num] = 0;
            filler_8bit_exts--;
            free_ext_num++;
        }

        /* Enable 16-bit filler extractor */
        while(filler_16bit_exts) {
            extractors->l1_e16_sel[free_ext_num] = 0;
            extractors->l2_e16_ext_mode[free_ext_num] = 0;
            extractors->l2_e16_ext_section[free_ext_num] = 0;
            filler_16bit_exts--;
            free_ext_num++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_keygen_filler_extractors_set
 *
 * Purpose:
 *    Enable filler extractors to fill unused bits
 *    to offset static qualifier extraction info
 *    in keytype profile.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_hx5_ft_keygen_filler_extractors_set(
        int unit,
        _field_ft_keygen_ext_codes_t *ft_ext_codes)
{
    int rv = BCM_E_NONE;
    _field_ft_keygen_type_a_ext_t *type_a_extractors = NULL;

    switch(ft_ext_codes->keygen_profile) {
        case _BCM_FIELD_FT_KEYGEN_PROFILE_AAB:
        {
            type_a_extractors = &ft_ext_codes->type_a_extractors[0];
            rv = _field_ft_keygen_type_a_filler_extractors_set(
                    unit, type_a_extractors);
            BCM_IF_ERROR_RETURN(rv);

            type_a_extractors = &ft_ext_codes->type_a_extractors[1];
            rv = _field_ft_keygen_type_a_filler_extractors_set(
                    unit, type_a_extractors);
            /* Type B cannot have filler extractors */

            break;
        }
        default:
        {
            rv = BCM_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_oper_skip_ext_ctrl_form
 *
 * Purpose:
 *    Form extractor ctrl sel info
 *    for skipped qualifier to keygen_oper.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    qual_id    - (IN) Skipped qualifier.
 *    type_a2_exts- (IN) Number of Type A extractors required.
 *    type_b_exts- (IN) Number of Type B extractors required.
 *    keygen_oper- (IN/OUT) Keygen operational state.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_oper_skip_ext_ctrl_form(int unit,
                bcm_field_qualify_t qual_id,
                int type_a2_exts,
                int type_b_exts,
                bcmi_keygen_oper_t *keygen_oper)
{
    int level = 0;
    int idx = 0;
    int cont_id = 0;
    int cont_incr = 0;
    int part_num = 0;
    int num_exts[3] = {0};
    int start_ext[3] = {0};
    uint16 start_cont = 0;
    uint16 total_exts_req = 0;
    uint16 ext_idx = 0, ext_num = 0;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_info = NULL;

    switch(qual_id) {
        case bcmFieldQualifySrcIp6:
            start_cont = 14;
            break;

        case bcmFieldQualifyDstIp6:
            start_cont = 24;
            break;

        case bcmFieldQualifyInnerSrcIp6:
            start_cont = 78;
            break;

        case bcmFieldQualifyInnerDstIp6:
            start_cont = 88;
            break;

        default:
            return BCM_E_INTERNAL;
    }

    total_exts_req = type_a2_exts + type_b_exts;

    num_exts[0] = 0;
    start_ext[0] = 0;
    num_exts[1] = type_a2_exts;
    if (num_exts[1] != 0) {
        /* If there is no Type B extractors required, it means
           Type A is enough to extract. So, start at first ext
         */
        if (type_b_exts == 0) {
            start_ext[1] = 0;
        } else {
            start_ext[1] = 10 - type_a2_exts;
        }
    }
    num_exts[2] = type_b_exts;
    if (num_exts[2] != 0) {
        start_ext[2] = 0;
    }

    _FP_XGS3_ALLOC(ext_ctrl_info, (total_exts_req * 2 * \
                sizeof(bcmi_keygen_ext_ctrl_sel_info_t)),
            "Keygen Ext ctrl info");
    if (ext_ctrl_info == NULL) {
        return BCM_E_MEMORY;
    }
    ext_idx = 0;
    for (level = 1; level <= 2; level++) {
        if (level == 1) {
            cont_id = start_cont; cont_incr = 1;
        } else {
            cont_id = 0; cont_incr = 0;
        }
        for (part_num = 0; part_num < 3; part_num++) {
            if (num_exts[part_num] == 0) {
                continue;
            }
            for (idx = 0; idx < num_exts[part_num]; idx++) {
                ext_num = start_ext[part_num] + idx;
                ext_ctrl_info[ext_idx].ctrl_sel =
                    BCMI_KEYGEN_EXT_CTRL_SEL_MULTIPLEXER;
                ext_ctrl_info[ext_idx].ctrl_sel_val = cont_id;
                ext_ctrl_info[ext_idx].part = part_num;
                ext_ctrl_info[ext_idx].level = level;
                ext_ctrl_info[ext_idx].gran = 16; /* gran_parts[part_num][ext];*/

                BCMI_FT_KEYGEN_EXT_ID_CREATE(ext_num, \
                        part_num, ext_ctrl_info[ext_idx].ext_num);
                ext_idx++; cont_id += cont_incr;
            }
        }
    }

    keygen_oper->ext_ctrl_sel_info = ext_ctrl_info;
    keygen_oper->ext_ctrl_sel_info_count = total_exts_req * 2;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_keygen_oper_skip_qual_offset_info_form
 *
 * Purpose:
 *    Form ual_offset_info for skipped qualifier to keygen_oper.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    num_parts  - (IN) Number of parts of keygen extraction.
 *    qual_id    - (IN) Skipped qualifier.
 *    type_a2_exts- (IN) Number of Type A extractors required.
 *    type_b_exts- (IN) Number of Type B extractors required.
 *    keygen_oper- (IN/OUT) Keygen operational state.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_keygen_oper_skip_qual_offset_info_form(int unit,
                int num_parts,
                bcm_field_qualify_t qual_id,
                int type_a2_exts,
                int type_b_exts,
                bcmi_keygen_oper_t *keygen_oper)
{
    int rv = BCM_E_NONE;
    int part = 0;
    int offset = 0;
    int total_exts_assigned = 0;
    bcmi_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmi_keygen_qual_offset_t *offset_arr = NULL;

    if (keygen_oper == NULL) {
        return BCM_E_PARAM;
    }

    /* No qualifiers/extractors skipped */
    if (keygen_oper->ext_ctrl_sel_info_count == 0) {
        return BCM_E_NONE;
    }

    _FP_XGS3_ALLOC(qual_offset_info,  (num_parts * \
                sizeof(bcmi_keygen_qual_offset_info_t)), "Qual Offset Info");
    if (qual_offset_info == NULL) {
        return BCM_E_MEMORY;
    }

    /* Form qual_offset_info for given qualifier */
    switch(qual_id) {
        case bcmFieldQualifySrcIp6:
        case bcmFieldQualifyDstIp6:
        case bcmFieldQualifyInnerSrcIp6:
        case bcmFieldQualifyInnerDstIp6:

            total_exts_assigned = 0;
            for (part = 1; part < num_parts; part++) {

                _FP_XGS3_ALLOC(qual_offset_info[part].qid_arr,
                        sizeof (bcm_field_qualify_t), "keygen oper qual arr");

                _FP_XGS3_ALLOC(qual_offset_info[part].offset_arr,
                        sizeof (bcmi_keygen_qual_offset_t), "keygen oper offset arr");

                if ((qual_offset_info[part].qid_arr == NULL) ||
                        (qual_offset_info[part].offset_arr == NULL)) {
                    rv = BCM_E_MEMORY;
                    BCMI_IF_ERROR_CLEANUP(rv);
                }
                qual_offset_info[part].qid_arr[0] = qual_id;
                offset_arr = &qual_offset_info[part].offset_arr[0];
                qual_offset_info[part].size = 1;

                if (part == 1) {
                    offset_arr->num_offsets = type_a2_exts;
                    for (offset = 0; offset < type_a2_exts; offset++) {
                        if (type_b_exts == 0) {
                            offset_arr->offset[offset] = offset * 16;
                        } else {
                            offset_arr->offset[offset] =
                                (10 - type_a2_exts + offset) * 16;
                        }
                        offset_arr->width[offset] = 16;
                        total_exts_assigned++;
                    }
                } else {
                    offset_arr->num_offsets = type_b_exts;
                    for (; offset < (type_a2_exts + type_b_exts); offset++) {
                        offset_arr->offset[offset] = (offset - type_a2_exts) * 16;
                        offset_arr->width[offset] = 16;
                        total_exts_assigned++;
                    }
                }
                if (total_exts_assigned == (type_a2_exts + type_b_exts)) {
                    break;
                }
            }
            break;
        default:
            rv = BCM_E_INTERNAL;
            BCMI_IF_ERROR_CLEANUP(rv);
    }

    keygen_oper->qual_offset_info = qual_offset_info;

    return BCM_E_NONE;

cleanup:

    if (qual_offset_info != NULL) {
        for (part = 0; part < num_parts; part++) {
            if (qual_offset_info[part].qid_arr != NULL) {
                sal_free(qual_offset_info[part].qid_arr);
            }
            if (qual_offset_info[part].offset_arr != NULL) {
                sal_free(qual_offset_info[part].offset_arr);
            }
        }
        sal_free(qual_offset_info);
        qual_offset_info = NULL;
    }
    return rv;
}

/*
 * Function:
 *    _field_ft_keygen_skip_qual_ext_ctrl_merge
 *
 * Purpose:
 *    Merge extractor ctrl sel info for skipped qualifier
 *    to keygen_oper.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    qual_id    - (IN) Skipped qualifier.
 *    type_a2_exts- (IN) Number of Type A extractors required.
 *    type_b_exts- (IN) Number of Type B extractors required.
 *    keygen_oper- (IN/OUT) Keygen operational state.
 *
 * Returns:
 *    BCM_E_XXX
 * Notes: Steps
 *
 *  Get ext_ctrl_sel_info for given qualifier
 *  Create ext_codes with extracted extractor_info
 *  Apply static extractor_info to ext_codes
 *  Loop through each extractor keytype
 *  If extractors are disabled in-between
 *    This is candidate for inserting filler extractors
 *    Calculate filler bits
 *    Ensure that all in-between extractors are used (4-bit ??)
 *       No. of 16-bit Ext = [(Filler_bits / 8) - (Unused_Exts)
 *       No. of 8-bit = [Filler_Exts - No_of_16bit_ext]
 *         Such combination is not possible, FAIL
 *  Enabled in-between extractor to fill unused bits
 *  Convert ext_codes back to ext_ctrl_sel_info
 *  Use new ext_ctrl_sel_info
 */
int
_field_ft_keygen_skip_qual_ext_ctrl_merge(int unit,
        int num_parts,
        bcmi_keygen_oper_t *skip_keygen_oper,
        bcmi_keygen_oper_t *keygen_oper)
{
    int rv = BCM_E_NONE;
    int keygen_profile = 0;
    _field_ft_keygen_ext_codes_t ext_codes;
    uint16 ext_ctrl_sel_info_count = 0;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info = NULL;

    /* Merge new and old qual_offset_info */
    rv = _field_ft_hx5_keygen_oper_qual_offset_info_merge(unit,
            num_parts, keygen_oper, skip_keygen_oper->qual_offset_info);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Set extractor info for keygen oper */
    keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AAB;
    ext_ctrl_sel_info = keygen_oper->ext_ctrl_sel_info;
    ext_ctrl_sel_info_count = keygen_oper->ext_ctrl_sel_info_count;
    (void) _bcm_field_ft_keygen_extractors_init(&ext_codes);
    rv = _bcm_field_hx5_ft_keygen_extractors_set(unit, keygen_profile,
            ext_ctrl_sel_info_count, ext_ctrl_sel_info, &ext_codes);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Set extractor info for qualifier */
    ext_ctrl_sel_info = skip_keygen_oper->ext_ctrl_sel_info;
    ext_ctrl_sel_info_count = skip_keygen_oper->ext_ctrl_sel_info_count;
    rv = _bcm_field_hx5_ft_keygen_extractors_set(unit, keygen_profile,
            ext_ctrl_sel_info_count, ext_ctrl_sel_info, &ext_codes);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* set filler extractors to offset new extractors */
    rv = _bcm_field_hx5_ft_keygen_filler_extractors_set(unit, &ext_codes);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Convert ext code to ext_ctrl_sel_info */
    rv = _field_hx5_ft_keygen_ext_ctrl_info_get(unit,
            &ext_codes, &ext_ctrl_sel_info, &ext_ctrl_sel_info_count);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Update keygen_oper with new extractor info */
    sal_free(keygen_oper->ext_ctrl_sel_info);
    keygen_oper->ext_ctrl_sel_info = ext_ctrl_sel_info;
    keygen_oper->ext_ctrl_sel_info_count = ext_ctrl_sel_info_count;

cleanup:

    return BCM_E_NONE;
}

#endif
