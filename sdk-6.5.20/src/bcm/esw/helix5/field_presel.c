/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field_presel.c
 * Purpose:     Field Processor routines Specific to Preselector Module
 *              in Helix5.
 */

#include <soc/defs.h>

#if defined (BCM_HELIX5_SUPPORT) && defined(BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/keygen_api.h>

/*
 * Function:
 *    _field_hx5_ft_lt_entry_hw_remove
 *
 * Purpose:
 *    Remove preselector Entry from hardware at given index
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_presel   - (IN/OUT) Preselector entry.
 *    tcam_idx   - (IN) Hardware index
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ft_lt_entry_hw_remove(int unit,
                                 _field_presel_entry_t *f_presel,
                                 int tcam_idx)
{
    int rv = BCM_E_NONE;
    int index = 0;
    _field_stage_t *stage_fc = NULL;
    soc_mem_t lt_tcam_mem = INVALIDm;
    soc_mem_t lt_data_mem = INVALIDm;

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            f_presel->group->stage_id, &stage_fc));

    /* If lt slice is not NULL, use that to get correct memories */
    if (f_presel->lt_fs) {
        if (f_presel->lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
            index = f_presel->lt_fs->slice_number;
        }
    }

    /* Get TCAM/DATA memory */
    rv = _bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                    f_presel->group->instance,
                    _BCM_FIELD_MEM_TYPE_FT_LT_I(index),
                    _BCM_FIELD_MEM_VIEW_TYPE_TCAM | _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                    &lt_tcam_mem, &lt_data_mem);
    BCM_IF_ERROR_RETURN(rv);

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                            tcam_idx, soc_mem_entry_null(unit, lt_tcam_mem)));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_data_mem, MEM_BLOCK_ALL,
                            tcam_idx, soc_mem_entry_null(unit, lt_data_mem)));

    /* Keygen Remove */
    BCM_IF_ERROR_RETURN(_field_hx5_ft_group_keygen_remove(unit,
                            f_presel, tcam_idx));

    return rv;
}
/*
 * Function:
 *    _field_hx5_ft_presel_qualifiers_init
 *
 * Purpose:
 *    Initializes flowtracker stage presel qualifiers.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Field stage control structure.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_field_hx5_ft_presel_qualifiers_init(int unit,
                                     _field_stage_t *stage_fc)
{
    uint16 key_base_off;
    uint16 z1_uc_bp = 0;
    uint16 z2_uc_bp = 0;
    soc_mem_t mem = INVALIDm;
    soc_field_info_t *field_info = NULL;
    _FP_QUAL_DECL;

    if (stage_fc->f_presel_qual_arr == 0) {
        return (BCM_E_INTERNAL);
    }

    mem = BSK_FTFP_LTS_LOGICAL_TBL_SEL_TCAMm;

    field_info = soc_mem_fieldinfo_get(unit, mem, KEYf);
    if (field_info == NULL) {
       return BCM_E_PARAM;
    }
    key_base_off = field_info->bp;

    /* PARSER1_L4_VALID */
    field_info = soc_mem_fieldinfo_get(unit, mem, PARSER1_L4_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4Ports,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* PARSER2_L4_VALID */
    field_info = soc_mem_fieldinfo_get(unit, mem, PARSER2_L4_VALIDf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerL4Ports,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* PARSER1_IP_FRAG_INFO */
    field_info = soc_mem_fieldinfo_get(unit, mem, PARSER1_IP_FRAG_INFOf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 2);
    }

    /* PARSER2_IP_FRAG_INFO */
    field_info = soc_mem_fieldinfo_get(unit, mem, PARSER2_IP_FRAG_INFOf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerIpFrag,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 2);
    }

    /* HVE_RESULTS_1_Z1_UC */
    /* HVE_RESULTS_1_Z1_BC */
    /* HVE_RESULTS_1_Z2_UC */
    /* HVE_RESULTS_1_Z2_BC */
    field_info = soc_mem_fieldinfo_get(unit, mem, HVE_RESULTS_1_Z1_UCf);
    if (field_info != NULL) {
        z1_uc_bp = field_info->bp - key_base_off;
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, HVE_RESULTS_1_Z2_UCf);
    if (field_info != NULL) {
        z2_uc_bp = field_info->bp - key_base_off;
    }
    if (z2_uc_bp != 0) {
        _FP_PRESEL_QUAL_TWO_ADD(unit, stage_fc, bcmFieldQualifyPktDstAddrType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            z1_uc_bp, 2, z2_uc_bp, 2);
    }

    /* HVE_RESULTS_2_Z1_UC */
    /* HVE_RESULTS_2_Z1_BC */
    /* HVE_RESULTS_2_Z2_UC */
    /* HVE_RESULTS_2_Z2_BC */
    field_info = soc_mem_fieldinfo_get(unit, mem, HVE_RESULTS_2_Z1_UCf);
    if (field_info != NULL) {
        z1_uc_bp = field_info->bp - key_base_off;
    }
    field_info = soc_mem_fieldinfo_get(unit, mem, HVE_RESULTS_2_Z2_UCf);
    if (field_info != NULL) {
        z2_uc_bp = field_info->bp - key_base_off;
    }
    if (z2_uc_bp != 0) {
        _FP_PRESEL_QUAL_TWO_ADD(unit, stage_fc, bcmFieldQualifyPktInnerDstAddrType,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            z1_uc_bp, 2, z2_uc_bp, 2);
    }

    /* PARSER1_DOS_ATTACK */
    field_info = soc_mem_fieldinfo_get(unit, mem, PARSER1_DOS_ATTACKf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    /* PARSER2_DOS_ATTACK */
    field_info = soc_mem_fieldinfo_get(unit, mem, PARSER2_DOS_ATTACKf);
    if (field_info != NULL) {
        _FP_PRESEL_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerDosAttack,
            0, _bcmFieldSliceSelDisable, 0, _bcmFieldQualifierTypePresel,
            field_info->bp - key_base_off , 1);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_field_hx5_ft_lt_entry_data_value_set
 *
 * Purpose:
 *    Set presel entry data to sw copy of entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    stage_fc   - (IN) Stage field control structure.
 *    fg         - (IN) Field group structure.
 *    index      - (IN)
 *    lt_data_mem- (IN) presel sram hw table
 *    data       - (IN/OUT) entry data
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_lt_entry_data_value_set(int unit,
                                          _field_stage_t *stage_fc,
                                          _field_group_t *fg,
                                          int index,
                                          soc_mem_t lt_data_mem,
                                          uint32 *data)
{
    uint32 keytype = 0;
    int group_key_mode = 0;

    if ((fg == NULL) || (stage_fc == NULL)) {
        return BCM_E_PARAM;
    }

    /* Set main SRAM as in lt_data_mem */
    if (fg->flags & _FP_GROUP_DW_DEPTH_EXPANDED) {
        group_key_mode = 1;
    } else {
        group_key_mode = 0;
    }
    soc_mem_field32_set(unit, lt_data_mem,
        data, GROUP_KEY_MODEf, group_key_mode);

    soc_mem_field32_set(unit, lt_data_mem, data, GROUP_LOOKUP_ENABLEf,
        (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 1 : 0);

    keytype = (fg->lt_id >> _FP_GROUP_FT_KEYTYPE_SHIFT) &
                            (_FP_GROUP_FT_KEYTYPE_MASK);
    soc_mem_field32_set(unit, lt_data_mem, data, GROUP_KEY_TYPEf, keytype);

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_lt_entry_install
 *
 * Purpose:
 *    Install preselector entry for flowtracker stage in hw.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_presel   - (IN) Preselector entry.
 *    lt_fs      - (IN) Logical table field slice.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_lt_entry_install(int unit,
                                   _field_presel_entry_t *f_presel,
                                   _field_lt_slice_t *lt_fs)
{
    int rv = BCM_E_NONE;
    _field_stage_t *stage_fc = NULL;
    int valid = 0;
    int tcam_idx = 0;
    int index = 0;
    soc_mem_t lt_tcam_mem = INVALIDm;
    soc_mem_t lt_data_mem = INVALIDm;
    uint32 tcam_entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* TCAM Entry buffer.*/
    uint32 data_entry[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* DATA Entry buffer. */

    /* Validate input params */
    if ((f_presel == NULL) || (lt_fs == NULL)) {
        return BCM_E_INTERNAL;
    }
    if ((f_presel->lt_tcam.key == NULL) ||
        (f_presel->lt_tcam.mask == NULL) ||
        (f_presel->lt_data.data == NULL)) {
        return BCM_E_PARAM;
    }

    /* Get device stage control structure. */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                            f_presel->group->stage_id, &stage_fc));

    BCM_IF_ERROR_RETURN(_bcm_field_presel_entry_tcam_idx_get(unit,
                            f_presel, lt_fs, &tcam_idx));

    /* Call Group Keygen Install */
    BCM_IF_ERROR_RETURN(_field_hx5_ft_group_keygen_install(unit, f_presel));

    /* If lt slice is not NULL, use that to get correct memories */
    if (f_presel->lt_fs) {
        if (f_presel->lt_fs->slice_flags & _BCM_FIELD_SLICE_INDEPENDENT) {
            index = f_presel->lt_fs->slice_number;
        }
    }

    /* Get TCAM/DATA memory */
    rv = _bcm_field_th_lt_tcam_policy_mem_get(unit, stage_fc,
                    f_presel->group->instance,
                    _BCM_FIELD_MEM_TYPE_FT_LT_I(index),
                    _BCM_FIELD_MEM_VIEW_TYPE_TCAM | _BCM_FIELD_MEM_VIEW_TYPE_DATA,
                    &lt_tcam_mem, &lt_data_mem);
    BCM_IF_ERROR_RETURN(rv);

    valid = (f_presel->group->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3: 0;

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                            tcam_idx, tcam_entry));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_data_mem, MEM_BLOCK_ALL,
                            tcam_idx, data_entry));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, lt_tcam_mem, MEM_BLOCK_ANY, tcam_idx,
                            tcam_entry));
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, lt_data_mem, MEM_BLOCK_ANY, tcam_idx,
                            data_entry));

    soc_mem_field_set(unit, lt_tcam_mem, tcam_entry, KEYf,  f_presel->lt_tcam.key);
    soc_mem_field_set(unit, lt_tcam_mem, tcam_entry, MASKf, f_presel->lt_tcam.mask);
    soc_mem_field_set(unit, lt_data_mem, data_entry, DATAf, f_presel->lt_data.data);

    /* Set entry valid bit status. */
    soc_mem_field32_set(unit, lt_tcam_mem, tcam_entry, VALIDf, valid);

    /* Install entry in hardware. */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_data_mem, MEM_BLOCK_ALL,
                tcam_idx, data_entry));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, lt_tcam_mem, MEM_BLOCK_ALL,
                tcam_idx, tcam_entry));

    f_presel->flags &= ~_FP_ENTRY_DIRTY;
    f_presel->flags |= _FP_ENTRY_INSTALLED;
    f_presel->flags |= _FP_ENTRY_ENABLED;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_hx5_ft_lt_entry_hw_remove
 *
 * Purpose:
 *    Remove preselector Entry from hardware.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_presel   - (IN/OUT) Preselector entry.
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_MEMORY      - Allocation failure.
 *    BCM_E_INTERNAL    - Invalid CAP Stage ID.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_hx5_ft_lt_entry_hw_remove(int unit,
                                     _field_presel_entry_t *f_presel)
{
    int rv = BCM_E_NONE;
    int tcam_idx = 0;

    /* Validate input params */
    if (f_presel == NULL) {
        return BCM_E_INTERNAL;
    }

    if ((f_presel->flags & _FP_ENTRY_INSTALLED) == 0) {
        /* Entry is not installed in hw */
        return BCM_E_NONE;
    }

    /* Get Tcam Index */
    BCM_IF_ERROR_RETURN(_bcm_field_presel_entry_tcam_idx_get(unit,
                            f_presel, f_presel->lt_fs, &tcam_idx));

    BCM_IF_ERROR_RETURN(_field_hx5_ft_lt_entry_hw_remove(unit,
                            f_presel, tcam_idx));

    f_presel->flags |= _FP_ENTRY_DIRTY;
    f_presel->flags &= ~(_FP_ENTRY_INSTALLED);
    f_presel->flags &= ~(_FP_ENTRY_ENABLED);

    return rv;
}

/*
 * Function:
 *    _field_hx5_ft_presel_delete_last_entry_hw
 *
 * Purpose:
 *    Remove preselector Entry at last index in lt slice
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    f_presel   - (IN/OUT) Preselector entry.
 *    last_hw_index   - (IN) Hardware index
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_presel_delete_last_entry_hw(int unit,
                                          _field_presel_entry_t *f_presel,
                                          int last_hw_index)
{
    int rv = BCM_E_NONE;
    int tcam_idx = 0;

    tcam_idx = f_presel->lt_fs->start_tcam_idx + last_hw_index;

    BCM_IF_ERROR_RETURN(_field_hx5_ft_lt_entry_hw_remove(unit,
                            f_presel, tcam_idx));

    f_presel->lt_fs->p_entries[last_hw_index] = NULL;

    return rv;
}

#define _BCM_FIELD_PKT_DST_ADDR_TYPE_MAC_MASK               \
            (BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC  |       \
             BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC  |       \
             BCM_FIELD_PKT_DST_ADDR_TYPE_BCAST_MAC)

#define _BCM_FIELD_PKT_DST_ADDR_TYPE_IP_MASK                \
            (BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP  |        \
             BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP  |        \
             BCM_FIELD_PKT_DST_ADDR_TYPE_BCAST_IP)

int
_bcm_field_hx5_qualify_PktDstAddrType(int unit,
                    bcm_field_qualify_t qual,
                    uint32 dst_addr_type,
                    uint32 *hw_data,
                    uint32 *hw_mask)
{
    uint32 dst_mac_type = 0;
    uint32 dst_ip_type = 0;
    uint32 dst_mac_flags = 0;
    uint32 dst_ip_flags = 0;
    uint32 bit = 0, flag = 0;
    uint32 mac_hw_data = 0, mac_hw_mask = 0;
    uint32 ip_hw_data = 0, ip_hw_mask = 0;
    uint32 dst_addr_type_arr[1] = {0};

    /* Validate input args */
    if ((hw_data == NULL) || (hw_mask == NULL)) {
        return BCM_E_PARAM;
    }

    dst_mac_type = dst_addr_type & _BCM_FIELD_PKT_DST_ADDR_TYPE_MAC_MASK;
    dst_ip_type = dst_addr_type & _BCM_FIELD_PKT_DST_ADDR_TYPE_IP_MASK;

    dst_mac_flags = _shr_popcount(dst_mac_type);
    dst_ip_flags = _shr_popcount(dst_ip_type);

    if ((dst_mac_flags > 1) || (dst_ip_flags > 1)) {
        return BCM_E_PARAM;
    }

    dst_addr_type_arr[0] = dst_addr_type;
    SHR_BIT_ITER(dst_addr_type_arr, 32, bit) {
        flag = (1 << bit);
        switch(flag) {
            case BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC:
                mac_hw_data = 0x1;
                mac_hw_mask = 0x3;
                break;
            case BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC:
                mac_hw_data = 0x0;
                mac_hw_mask = 0x3;
                break;
            case BCM_FIELD_PKT_DST_ADDR_TYPE_BCAST_MAC:
                mac_hw_data = 0x2;
                mac_hw_mask = 0x3;
                break;
            case BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP:
                ip_hw_data = 0x1;
                ip_hw_mask = 0x3;
                break;
            case BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP:
                ip_hw_data = 0x0;
                ip_hw_mask = 0x3;
                break;
            case BCM_FIELD_PKT_DST_ADDR_TYPE_BCAST_IP:
                ip_hw_data = 0x2;
                ip_hw_mask = 0x3;
                break;
            default:
                return BCM_E_PARAM;
        }
    }

    *hw_data = (ip_hw_data << 2) + mac_hw_data;
    *hw_mask = (ip_hw_mask << 2) + mac_hw_mask;

    return BCM_E_NONE;
}

int
_bcm_field_hx5_qualify_PktDstAddrType_get(int unit,
                    bcm_field_qualify_t qual,
                    uint32 hw_data,
                    uint32 hw_mask,
                    uint32 *dst_addr_type)
{
    uint32 ip_hw_data = 0;
    uint32 ip_hw_mask = 0;
    uint32 mac_hw_data = 0;
    uint32 mac_hw_mask = 0;

    ip_hw_data = (hw_data >> 2) & 0x3;
    ip_hw_mask = (hw_mask >> 2) & 0x3;
    mac_hw_data = hw_data & 0x3;
    mac_hw_mask = hw_mask & 0x3;

    *dst_addr_type = 0;

    if (mac_hw_mask != 0) {
        switch (mac_hw_data) {
            case 0:
                *dst_addr_type |= BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_MAC;
                break;
            case 1:
                *dst_addr_type |= BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_MAC;
                break;
            case 2:
                *dst_addr_type |= BCM_FIELD_PKT_DST_ADDR_TYPE_BCAST_MAC;
                break;
            default:
                *dst_addr_type |= 0;
        }
    }

    if (ip_hw_mask != 0) {
        switch (ip_hw_data) {
            case 0:
                *dst_addr_type |= BCM_FIELD_PKT_DST_ADDR_TYPE_MCAST_IP;
                break;
            case 1:
                *dst_addr_type |= BCM_FIELD_PKT_DST_ADDR_TYPE_UCAST_IP;
                break;
            case 2:
                *dst_addr_type |= BCM_FIELD_PKT_DST_ADDR_TYPE_BCAST_IP;
                break;
            default:
                *dst_addr_type |= 0;
        }
    }

    return BCM_E_NONE;
}

#endif
