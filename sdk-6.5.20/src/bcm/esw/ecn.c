/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ECN - Broadcom StrataSwitch ECN API.
 */

#include <sal/core/libc.h>
#include <soc/defs.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <bcm/error.h>

#include <bcm/ecn.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <bcm_int/esw/ecn.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw_dispatch.h>


#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)

int
_bcm_esw_ecn_init(int unit)
{
    responsive_protocol_match_entry_t   entry_res_protocol;
    ip_to_int_cn_mapping_entry_t        entry_ip_int_cn;
    int_cn_to_mmuif_mapping_entry_t     entry_int_cn_mmuif;
    egr_int_cn_update_entry_t           entry_egr_int_cn_update;
    egr_int_cn_to_ip_mapping_entry_t    entry_egr_int_cn_ip_mapping;
    int count, i;
    uint32  fld_val;
    uint64  fld64_val;
    soc_mem_t cfg_mem = RESPONSIVE_PROTOCOL_MATCHm;
    int table_adj = 1;

    if (!SOC_WARM_BOOT(unit)) {
        /* Responsive Protocol Match table */
        /* Set TCP(0x6) to 1 */
        COMPILER_64_ZERO(fld64_val);
        COMPILER_64_SET(fld64_val, 0x0, (0x1 << 6));
        sal_memset(&entry_res_protocol, 0, sizeof(entry_res_protocol));
        soc_mem_field64_set(unit, RESPONSIVE_PROTOCOL_MATCHm, 
                    &entry_res_protocol, RESPONSIVEf, fld64_val);
        soc_mem_write(unit, RESPONSIVE_PROTOCOL_MATCHm, MEM_BLOCK_ALL,
                0, (void *)&entry_res_protocol);

        /* 
         * IP_TO_INT_CN_MAPPING table
            Address (ECN + responsive)  :     INT_CN
                            0                           2'b01
                            1                           2'b00
                            2                           2'b10
                            3                           2'b10
                            4                           2'b10
                            5                           2'b10
                            6                           2'b11
                            7                           2'b11
         */
#if defined(BCM_TRIDENT3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit)) {
            cfg_mem = PKT_ECN_TO_INT_CN_MAPPINGm;
            table_adj = 8;

            count = soc_mem_index_count(unit, cfg_mem);
            count /= table_adj;
            for (i = 0; i < count; i++) {
                sal_memset(&entry_ip_int_cn, 0, sizeof(entry_ip_int_cn));
                switch (i) {
                    case 0:
                        fld_val = _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP;
                        break;
                    case 4:
                        fld_val = _BCM_ECN_INT_CN_RESPONSIVE_DROP;
                        break;
                    case 3:
                    case 7:
                        fld_val = _BCM_ECN_INT_CN_CONGESTION_MARK_ECN;
                        break;
                    default:
                        fld_val = _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN;
                        break;
                }
                soc_mem_field32_set(unit, cfg_mem,
                            &entry_ip_int_cn, INT_CNf, fld_val);
                soc_mem_write(unit, cfg_mem, MEM_BLOCK_ALL,
                    i, (void *)&entry_ip_int_cn);
            }
        } else
#endif
        {
            cfg_mem = IP_TO_INT_CN_MAPPINGm;
            table_adj = 1;

            count = soc_mem_index_count(unit, cfg_mem);
            count /= table_adj;
            for (i = 0; i < count; i++) {
                sal_memset(&entry_ip_int_cn, 0, sizeof(entry_ip_int_cn));
                switch (i) {
                    case 0:
                        fld_val = _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP;
                        break;
                    case 1:
                        fld_val = _BCM_ECN_INT_CN_RESPONSIVE_DROP;
                        break;
                    case 6:
                    case 7:
                        fld_val = _BCM_ECN_INT_CN_CONGESTION_MARK_ECN;
                        break;
                    default:
                        fld_val = _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN;
                        break;
                }
                soc_mem_field32_set(unit, cfg_mem,
                            &entry_ip_int_cn, INT_CNf, fld_val);
                soc_mem_write(unit, cfg_mem, MEM_BLOCK_ALL,
                    i, (void *)&entry_ip_int_cn);
            }
        }


        /* INT_CN_TO_MMUIF_MAPPING */
        count = soc_mem_index_count(unit, INT_CN_TO_MMUIF_MAPPINGm);
        for (i = 0; i < count; i++) {
            sal_memset(&entry_int_cn_mmuif, 0, sizeof(entry_int_cn_mmuif));
            switch (i) {
                case _BCM_ECN_INT_CN_RESPONSIVE_DROP:
                    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_RESPONSIVEf, 1);
                    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_MARK_ELIGIBLEf, 0);
                    break;
                case _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP:
                    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_RESPONSIVEf, 0);
                    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_MARK_ELIGIBLEf, 0);
                    break;
                case _BCM_ECN_INT_CN_CONGESTION_MARK_ECN:
                case _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN:
                    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_RESPONSIVEf, 1);
                    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_MARK_ELIGIBLEf, 1);
                    break;
                default:
                    break;
            }
            soc_mem_write(unit, INT_CN_TO_MMUIF_MAPPINGm, MEM_BLOCK_ALL,
                i, (void *)&entry_int_cn_mmuif);
        }

        /* EGR_INT_CN_UPDATE */
#if defined(BCM_TRIDENT3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit)) {
            /* In TD3, CE_R & CE_Y & CE_G is replaced by CE. */
            count = soc_mem_index_count(unit, EGR_INT_CN_UPDATEm);
            for (i = 0; i < count; i++) {
                sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
                if (i < 0x8) {
                    /* INT_CN == 0 */
                    soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf,
                        _BCM_ECN_INT_CN_RESPONSIVE_DROP);
                } else if (i < 0x10) {
                    /* INT_CN == 1 */
                    soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf,
                        _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP);
                } else if (i < 0x18) {
                    /* INT_CN == 2 */
                    /* Change the INT_CN to 3 if local MMU is congested */
                    if (i & (1 << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_OFFSET_TD3)) {
                        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, INT_CNf,
                            _BCM_ECN_INT_CN_CONGESTION_MARK_ECN);
                        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, CONGESTION_MARKEDf, 1);
                    } else {
                        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, INT_CNf,
                            _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN);
                    }
                } else {
                    /* INT_CN ==  3 */
                    soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf,
                        _BCM_ECN_INT_CN_CONGESTION_MARK_ECN);
                }
                soc_mem_write(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    i, (void *)&entry_egr_int_cn_update);
            }
        }else
#endif
        {
            count = soc_mem_index_count(unit, EGR_INT_CN_UPDATEm);
            for (i = 0; i < count; i++) {
                sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
                if (i < 0x20) {
                    /* INT_CN == 0 */
                    soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf,
                        _BCM_ECN_INT_CN_RESPONSIVE_DROP);
                } else if (i < 0x40) {
                    /* INT_CN == 1 */
                    soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf,
                        _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP);
                } else if (i > 0x5f) {
                    /* INT_CN ==  3 */
                    soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf,
                        _BCM_ECN_INT_CN_CONGESTION_MARK_ECN);
                } else {
                    /* INT_CN == 2 */
                    /* Change the INT_CN to 3 if local MMU is congested */
                    switch (i) {
                        case 0x50:
                        case 0x54:
                        case 0x58:
                        case 0x5c:
                        case 0x4b:
                        case 0x4f:
                        case 0x5b:
                        case 0x5f:
                        case 0x45:
                        case 0x4d:
                        case 0x55:
                        case 0x5d:
                            soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                                &entry_egr_int_cn_update, INT_CNf,
                                _BCM_ECN_INT_CN_CONGESTION_MARK_ECN);
                            soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                                &entry_egr_int_cn_update, CONGESTION_MARKEDf, 1);
                            break;
                        default:
                            soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                                &entry_egr_int_cn_update, INT_CNf,
                                _BCM_ECN_INT_CN_NON_CONGESTION_MARK_ECN);
                            break;
                    }
                }
                soc_mem_write(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    i, (void *)&entry_egr_int_cn_update);
            }
        }

        /* EGR_INT_CN_TO_IP_MAPPING */
#if defined(BCM_TRIDENT3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit)) {
            cfg_mem = EGR_INT_CN_TO_PKT_ECN_MAPPINGm;
            table_adj = 32;
        } else
#endif
        {
            cfg_mem = EGR_INT_CN_TO_IP_MAPPINGm;
            table_adj = 1;
        }
        count = soc_mem_index_count(unit, cfg_mem);
        count /= table_adj;
        for (i = 0; i < count; i++) {
            sal_memset(&entry_egr_int_cn_ip_mapping, 0, 
                                sizeof(entry_egr_int_cn_ip_mapping));

            /* Change the ECN value from 2'b01 or 2'b10 to 2'b11 for INT_CN is 3 */
            if ((i == 13) || (i == 14)) {
                soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, CHANGE_PACKET_ECNf, 1);
                soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, ECNf, 3);
                soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, INCREMENT_ECN_COUNTERf, 1);
            }
            soc_mem_write(unit, cfg_mem, MEM_BLOCK_ALL,
                i, (void *)&entry_egr_int_cn_ip_mapping);
        }
        /* 
         * Configure the int_cn of Non-IP packets to Non-Responsive Dropping. 
         */
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_set(unit, 
                bcmSwitchEcnNonIpIntCongestionNotification,
                _BCM_ECN_INT_CN_NON_RESPONSIVE_DROP));
    } 
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(bcmi_xgs5_ecn_init(unit)); 
    }
#endif
    return BCM_E_NONE;
}

#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */

/*
 * Function:
 *      bcm_esw_ecn_responsive_protocol_get
 * Purpose:
 *      To get the value of responsive indication based on the IP protocol value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip_proto - (IN) IP Protocol value.
 *      responsice - (OUT) Responsice indication.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_ecn_responsive_protocol_get(
    int unit, uint8 ip_proto, int *responsive)
{

#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    uint64  fld64_val;
    int mem_idx, field_offset;
    responsive_protocol_match_entry_t   entry_res_protocol;

    if (!soc_feature(unit, soc_feature_ecn_wred)) {
        return BCM_E_UNAVAIL;
    }

    COMPILER_64_ZERO(fld64_val);
    mem_idx = ip_proto / 64;
    field_offset = ip_proto % 64;

    sal_memset(&entry_res_protocol, 0, sizeof(entry_res_protocol));
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, RESPONSIVE_PROTOCOL_MATCHm, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_res_protocol));
    soc_mem_field64_get(unit, RESPONSIVE_PROTOCOL_MATCHm, 
                    &entry_res_protocol, RESPONSIVEf, &fld64_val);

    if (COMPILER_64_BITTEST(fld64_val, field_offset)) {
        *responsive = TRUE;
    } else {
        *responsive = FALSE;
    }

    return BCM_E_NONE;
#else /* !BCM_GREYHOUND_SUPPORT && !BCM_TOMAHAWK_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */
    
}


/*
 * Function:
 *      bcm_esw_ecn_responsive_protocol_get
 * Purpose:
 *      To configure the value of responsive indication based on the IP protocol value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip_proto - (IN) IP Protocol value.
 *      responsice - (IN) Responsice indication.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_ecn_responsive_protocol_set(
    int unit, uint8 ip_proto, int responsive)
{
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    uint64  fld64_val, val64;
    int mem_idx, field_offset;
    responsive_protocol_match_entry_t   entry_res_protocol;

    if (!soc_feature(unit, soc_feature_ecn_wred)) {
        return BCM_E_UNAVAIL;
    }
    
    COMPILER_64_ZERO(fld64_val);
    mem_idx = ip_proto / 64;
    field_offset = ip_proto % 64;

    sal_memset(&entry_res_protocol, 0, sizeof(entry_res_protocol));
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, RESPONSIVE_PROTOCOL_MATCHm, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_res_protocol));
    soc_mem_field64_get(unit, RESPONSIVE_PROTOCOL_MATCHm, 
                    &entry_res_protocol, RESPONSIVEf, &fld64_val);

    COMPILER_64_ZERO(val64);
    COMPILER_64_SET(val64, 0x0, 0x1);
    COMPILER_64_SHL(val64, field_offset);
    if (responsive) {
        COMPILER_64_OR(fld64_val, val64);
    } else {
        COMPILER_64_NOT(val64);
        COMPILER_64_AND(fld64_val, val64);
    }

    soc_mem_field64_set(unit, RESPONSIVE_PROTOCOL_MATCHm, 
                    &entry_res_protocol, RESPONSIVEf, fld64_val);
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, RESPONSIVE_PROTOCOL_MATCHm, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_res_protocol));

    return BCM_E_NONE;
#else /* !BCM_GREYHOUND_SUPPORT && !BCM_TOMAHAWK_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */
}


/*
 * Function:
 *      bcm_esw_ecn_traffic_map_get
 * Purpose:
 *      To get the mapped internal congestion notification (int_cn) value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      map - (INOUT) Internal congestion notification map.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int bcm_esw_ecn_traffic_map_get(
    int unit, bcm_ecn_traffic_map_info_t *map)
{
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int mem_idx;
    ip_to_int_cn_mapping_entry_t        entry_ip_int_cn;
    uint32      fld_val;
    soc_mem_t cfg_mem = 0;

    if (!soc_feature(unit, soc_feature_ecn_wred)) {
        return BCM_E_UNAVAIL;
    }

    if (map == NULL) {
        return BCM_E_PARAM;
    }

    if (map->ecn > _BCM_ECN_VALUE_MAX) {
        return BCM_E_PARAM;
    }

    /* Check supported flag */
    if ((map->flags & ~BCM_ECN_TRAFFIC_MAP_RESPONSIVE) != 0) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)){
        cfg_mem = PKT_ECN_TO_INT_CN_MAPPINGm;
        mem_idx = map->ecn;
        if (map->flags & BCM_ECN_TRAFFIC_MAP_RESPONSIVE) {
            /* mem_idx = (resp<<2) + ecn */
            mem_idx +=4;
        }
    } else
#endif
    {
        cfg_mem = IP_TO_INT_CN_MAPPINGm;
        /* MEMORY INDEX : ECN + Responsive */
        mem_idx = (map->ecn << 1);
        if (map->flags & BCM_ECN_TRAFFIC_MAP_RESPONSIVE) {
            mem_idx++;
        }
    }
    sal_memset(&entry_ip_int_cn, 0, sizeof(entry_ip_int_cn));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, cfg_mem, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_ip_int_cn));

    fld_val = soc_mem_field32_get(unit, cfg_mem,
                        &entry_ip_int_cn, INT_CNf);

    map->int_cn = fld_val;
    

    return BCM_E_NONE;
#else /* !BCM_GREYHOUND_SUPPORT && !BCM_TOMAHAWK_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */
}
 
/*
 * Function:
 *      bcm_esw_ecn_traffic_map_set
 * Purpose:
 *      To set the mapped internal congestion notification (int_cn) value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      map - (INOUT) Internal congestion notification map.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_esw_ecn_traffic_map_set(
    int unit, bcm_ecn_traffic_map_info_t *map)
{
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int mem_idx;
    ip_to_int_cn_mapping_entry_t        entry_ip_int_cn;
    uint32      fld_val;
    soc_mem_t cfg_mem = 0;

    if (!soc_feature(unit, soc_feature_ecn_wred)) {
        return BCM_E_UNAVAIL;
    }

    if (map == NULL) {
        return BCM_E_PARAM;
    }

    if (map->ecn > _BCM_ECN_VALUE_MAX) {
        return BCM_E_PARAM;
    }

    if ((map->int_cn < 0) || (map->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)){
        cfg_mem = PKT_ECN_TO_INT_CN_MAPPINGm;
        mem_idx = map->ecn;
        if (map->flags & BCM_ECN_TRAFFIC_MAP_RESPONSIVE) {
            /* mem_idx =  (resp<<2) + ecn */
            mem_idx += 4;
        }
    } else
#endif
    {
   /* MEMORY INDEX : ECN + Responsive */
    mem_idx = (map->ecn << 1);
    if (map->flags & BCM_ECN_TRAFFIC_MAP_RESPONSIVE) {
        mem_idx++;
    }

        cfg_mem = IP_TO_INT_CN_MAPPINGm;
    }

    sal_memset(&entry_ip_int_cn, 0, sizeof(entry_ip_int_cn));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, cfg_mem, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_ip_int_cn));

    fld_val = map->int_cn;
    soc_mem_field32_set(unit, cfg_mem,
                        &entry_ip_int_cn, INT_CNf, fld_val);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, cfg_mem, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_ip_int_cn));

    return BCM_E_NONE;
#else /* !BCM_GREYHOUND_SUPPORT && !BCM_TOMAHAWK_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */
}


#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
static int
_bcm_esw_ecn_action_enqueue_get(int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config)
{

    int_cn_to_mmuif_mapping_entry_t     entry_int_cn_mmuif;
    int     mem_idx;
    uint32  fld_val;
    
    if ((ecn_config->int_cn < 0) || 
        (ecn_config->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

    mem_idx = ecn_config->int_cn;
    sal_memset(&entry_int_cn_mmuif, 0, sizeof(entry_int_cn_mmuif));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, INT_CN_TO_MMUIF_MAPPINGm, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_int_cn_mmuif));

    /* WRED RESPONSIVE */
    fld_val = soc_mem_field32_get(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_RESPONSIVEf);
    if (fld_val) {
        ecn_config->action_flags |= 
            BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE;
    } else {
        ecn_config->action_flags &= 
            ~BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE;
    }

    /* MARK ELIGIBLE */
    fld_val = soc_mem_field32_get(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_MARK_ELIGIBLEf);
    if (fld_val) {
        ecn_config->action_flags |= 
            BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE;
    } else {
        ecn_config->action_flags &= 
            ~BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE;
    }

    return BCM_E_NONE;
}

static int
_bcm_esw_ecn_action_enqueue_set(int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config)
{

    int_cn_to_mmuif_mapping_entry_t     entry_int_cn_mmuif;
    int     mem_idx;
    uint32  fld_val;
    
    if ((ecn_config->int_cn < 0) || 
        (ecn_config->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

    mem_idx = ecn_config->int_cn;
    sal_memset(&entry_int_cn_mmuif, 0, sizeof(entry_int_cn_mmuif));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, INT_CN_TO_MMUIF_MAPPINGm, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_int_cn_mmuif));

    /* WRED RESPONSIVE */
    if (ecn_config->action_flags & 
        BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE) {
        fld_val = 1;
    } else {
        fld_val = 0;
    }
    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_RESPONSIVEf, fld_val);

    /* MARK ELIGIBLE */
    if (ecn_config->action_flags & 
        BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE) {
        fld_val = 1;
    } else {
        fld_val = 0;
    }
    soc_mem_field32_set(unit, INT_CN_TO_MMUIF_MAPPINGm, 
                        &entry_int_cn_mmuif, WRED_MARK_ELIGIBLEf, fld_val);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, INT_CN_TO_MMUIF_MAPPINGm, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_int_cn_mmuif));

    return BCM_E_NONE;
}

static int
_bcm_esw_ecn_action_dequeue_get(int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config)
{
    int mem_idx, cng, ce_green, ce_yellow, ce_red, index_max;
    egr_int_cn_update_entry_t           entry_egr_int_cn_update;
    uint32      fld_val;
#if defined(BCM_TRIDENT3_SUPPORT)
    int ce = 0;
#endif
    
    if ((ecn_config->color != bcmColorGreen) && 
        (ecn_config->color != bcmColorYellow) &&
        (ecn_config->color != bcmColorRed)) {
        return BCM_E_PARAM;
    }

    if ((ecn_config->int_cn < 0) || 
        (ecn_config->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

    /* 
     * Memory index : int_cn(2 bits)+ce_green+ce_yellow+ce_red+cng(2bit) 
     * Only the ce_[color] match the value of cng is meaningful.
 */
    cng = _BCM_COLOR_ENCODING(unit, ecn_config->color);
    index_max = soc_mem_index_max(unit, EGR_INT_CN_UPDATEm);
    ce_green = 0;
    ce_yellow = 0;
    ce_red = 0;

#if defined(BCM_TRIDENT3_SUPPORT)
    if(SOC_IS_TRIDENT3X(unit)) {
        ce = 0;
        mem_idx =
            (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET_TD3)
            + (ce << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_OFFSET_TD3)
            + (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);
    
        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }

        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
        /* Get the updated int_cn when congestion is not exprienced */
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        fld_val = soc_mem_field32_get(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf);
        if (fld_val != ecn_config->int_cn) {
            ecn_config->action_flags |=
                BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE;
            ecn_config->non_congested_int_cn = fld_val;
        } else {
            ecn_config->action_flags &=
                ~BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE;
        }
    
        /* Get the updated int_cn when congestion is exprienced */
        ce = 1;
        mem_idx =
            (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET_TD3)
            + (ce << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_OFFSET_TD3)
            + (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);

        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }
        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        fld_val = soc_mem_field32_get(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf);

        if (fld_val != ecn_config->int_cn) {
            ecn_config->action_flags |=
                BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE;
            ecn_config->congested_int_cn = fld_val;
        } else {
            ecn_config->action_flags &=
                ~BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE;
        }
    } else
#endif
    {
        mem_idx = (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET) +
            (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);

        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }

        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
        /* Get the updated int_cn when congestion is not exprienced */
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        fld_val = soc_mem_field32_get(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf);
        if (fld_val != ecn_config->int_cn) {
            ecn_config->action_flags |=
                BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE;
            ecn_config->non_congested_int_cn = fld_val;
        } else {
            ecn_config->action_flags &=
                ~BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE;
        }

        /* Get the updated int_cn when congestion is exprienced */
        if (ecn_config->color == bcmColorGreen) {
            ce_green = 1;
        } else if (ecn_config->color == bcmColorYellow) {
            ce_yellow = 1;
        } else {
            /* Red */
            ce_red = 1;
        }

        mem_idx = (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET) +
            (ce_green << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_GREEN_OFFSET) +
            (ce_yellow << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_YELLOW_OFFSET) +
            (ce_red << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_RED_OFFSET) +
            (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);


        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }
        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        fld_val = soc_mem_field32_get(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf);

        if (fld_val != ecn_config->int_cn) {
            ecn_config->action_flags |=
                BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE;
            ecn_config->congested_int_cn = fld_val;
        } else {
            ecn_config->action_flags &=
                ~BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE;
        }
    }
    ecn_config->responsive = soc_mem_field32_get(unit, EGR_INT_CN_UPDATEm, 
                                                 &entry_egr_int_cn_update, 
                                                 RESPONSIVEf);    
    return BCM_E_NONE;
    
}


static int
_bcm_esw_ecn_action_dequeue_set(int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config)
{
    int mem_idx, cng, ce_green, ce_yellow, ce_red, index_max;
    egr_int_cn_update_entry_t           entry_egr_int_cn_update;
    uint32      fld_val;
#if defined(BCM_TRIDENT3_SUPPORT)
    int ce = 0;
#endif
    
    if ((ecn_config->color != bcmColorGreen) && 
        (ecn_config->color != bcmColorYellow) &&
        (ecn_config->color != bcmColorRed)) {
        return BCM_E_PARAM;
    }

    if ((ecn_config->int_cn < 0) || 
        (ecn_config->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

    /* 
     * Memory index : int_cn(2 bits)+ce_green+ce_yellow+ce_red+cng(2bit) 
     * Only the ce_[color] match the value of cng is meaningful.
 */
    cng = _BCM_COLOR_ENCODING(unit, ecn_config->color);
    index_max = soc_mem_index_max(unit, EGR_INT_CN_UPDATEm);
    ce_green = 0;
    ce_yellow = 0;
    ce_red = 0;

#if defined(BCM_TRIDENT3_SUPPORT)
    if(SOC_IS_TRIDENT3X(unit)) {
        /* Update the int_cn value when congestion is not experienced */
        ce = 0;
        mem_idx =
            (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET_TD3)
            + (ce << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_OFFSET_TD3)
            + (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);

        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }
        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        if ((ecn_config->action_flags &
            BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE) &&
            (ecn_config->non_congested_int_cn != ecn_config->int_cn)) {

            if ((ecn_config->non_congested_int_cn < 0) ||
                (ecn_config->non_congested_int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
                return BCM_E_PARAM;
            }
            fld_val = ecn_config->non_congested_int_cn;
        } else {
            fld_val = ecn_config->int_cn;
        }
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, INT_CNf,
                            fld_val);
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                &entry_egr_int_cn_update, RESPONSIVEf,
                ecn_config->responsive ? 1 : 0);

        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                        mem_idx, (void *)&entry_egr_int_cn_update));


        /* Update the int_cn value when congestion is experienced */
        ce = 1;
        mem_idx =
            (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET_TD3)
            + (ce << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_OFFSET_TD3)
            + (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);

        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }
        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        if ((ecn_config->action_flags &
            BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE) &&
            (ecn_config->congested_int_cn != ecn_config->int_cn)) {

            if ((ecn_config->congested_int_cn < 0) ||
                (ecn_config->congested_int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
                return BCM_E_PARAM;
            }
            fld_val = ecn_config->congested_int_cn;
        } else {
            fld_val = ecn_config->int_cn;
        }
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                        &entry_egr_int_cn_update, INT_CNf, fld_val);

        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                &entry_egr_int_cn_update, RESPONSIVEf,
                ecn_config->responsive ? 1 : 0);

        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
    } else
#endif
    {
        /* Update the int_cn value when congestion is not experienced */
        for (ce_green = 0; ce_green < 2; ce_green++) {
            if ((ecn_config->color == bcmColorGreen) && (ce_green == 1)) {
                /* Bypass the congestion case */
                continue;
            }
            for (ce_yellow = 0; ce_yellow < 2; ce_yellow++) {
                if ((ecn_config->color == bcmColorYellow) && (ce_yellow == 1)) {
                    /* Bypass the congestion case */
                    continue;
                }
                for (ce_red = 0; ce_red < 2; ce_red++) {
                    if ((ecn_config->color == bcmColorRed) && (ce_red == 1)) {
                        /* Bypass the congestion case */
                        continue;
                    }
        mem_idx =
            (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET) +
            (ce_green << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_GREEN_OFFSET) +
            (ce_yellow << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_YELLOW_OFFSET) +
            (ce_red << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_RED_OFFSET) +
            (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);

        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }
        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        if ((ecn_config->action_flags &
            BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE) &&
            (ecn_config->non_congested_int_cn != ecn_config->int_cn)) {

            if ((ecn_config->non_congested_int_cn < 0) ||
                (ecn_config->non_congested_int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
                return BCM_E_PARAM;
            }
            fld_val = ecn_config->non_congested_int_cn;
        } else {
            fld_val = ecn_config->int_cn;
        }
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, INT_CNf, fld_val);
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, RESPONSIVEf,
                                ecn_config->responsive ? 1 : 0);
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                        mem_idx, (void *)&entry_egr_int_cn_update));
                }
            }
        }

        /* Update the int_cn value when congestion is experienced */
        for (ce_green = 0; ce_green < 2; ce_green++) {
            if ((ecn_config->color == bcmColorGreen) && (ce_green == 0)) {
                /* Bypass the non-congestion case */
                continue;
            }
            for (ce_yellow = 0; ce_yellow < 2; ce_yellow++) {
                if ((ecn_config->color == bcmColorYellow) && (ce_yellow == 0)) {
                    /* Bypass the non-congestion case */
                    continue;
                }
                for (ce_red = 0; ce_red < 2; ce_red++) {
                    if ((ecn_config->color == bcmColorRed) && (ce_red == 0)) {
                        /* Bypass the non-congestion case */
                        continue;
                    }
        mem_idx =
            (ecn_config->int_cn << _BCM_ECN_DEQUEUE_MEM_INDEX_INT_CN_OFFSET) +
            (ce_green << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_GREEN_OFFSET) +
            (ce_yellow << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_YELLOW_OFFSET) +
            (ce_red << _BCM_ECN_DEQUEUE_MEM_INDEX_CE_RED_OFFSET) +
            (cng << _BCM_ECN_DEQUEUE_MEM_INDEX_CNG_OFFSET);

        if (mem_idx > index_max) {
            return BCM_E_PARAM;
        }
        sal_memset(&entry_egr_int_cn_update, 0,
                                    sizeof(entry_egr_int_cn_update));

        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
        if ((ecn_config->action_flags &
            BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE) &&
            (ecn_config->congested_int_cn != ecn_config->int_cn)) {

            if ((ecn_config->congested_int_cn < 0) ||
                (ecn_config->congested_int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
                return BCM_E_PARAM;
            }
            fld_val = ecn_config->congested_int_cn;
        } else {
            fld_val = ecn_config->int_cn;
        }
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm,
                            &entry_egr_int_cn_update, INT_CNf, fld_val);
        soc_mem_field32_set(unit, EGR_INT_CN_UPDATEm, 
                            &entry_egr_int_cn_update, RESPONSIVEf, 
                            ecn_config->responsive ? 1 : 0);
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, EGR_INT_CN_UPDATEm, MEM_BLOCK_ALL,
                    mem_idx, (void *)&entry_egr_int_cn_update));
                }
            }
        }
    }
    return BCM_E_NONE;
    
}


static int
_bcm_esw_ecn_action_egress_get(int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config)
{
    int mem_idx, index_max;
    egr_int_cn_to_ip_mapping_entry_t    entry_egr_int_cn_ip_mapping;
    uint32  fld_val;
    soc_mem_t cfg_mem = 0;
    
    if ((ecn_config->int_cn < 0) || 
        (ecn_config->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

    if (ecn_config->ecn > _BCM_ECN_VALUE_MAX) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TRIDENT3_SUPPORT)
    if(SOC_IS_TRIDENT3X(unit)){
        cfg_mem = EGR_INT_CN_TO_PKT_ECN_MAPPINGm;
    } else
#endif
    {
        cfg_mem = EGR_INT_CN_TO_IP_MAPPINGm;
    }

    mem_idx = (ecn_config->int_cn << _BCM_ECN_EGRESS_MEM_INDEX_INT_CN_OFFSET) +
        (ecn_config->ecn << _BCM_ECN_EGRESS_MEM_INDEX_ECN_OFFSET);
    index_max = soc_mem_index_max(unit, cfg_mem);

    if (mem_idx > index_max) {
        return BCM_E_PARAM;
    }
    sal_memset(&entry_egr_int_cn_ip_mapping, 0, 
                    sizeof(entry_egr_int_cn_ip_mapping));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, cfg_mem, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_egr_int_cn_ip_mapping));
    
    /* Check ECN MARKING action */
    fld_val = soc_mem_field32_get(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    CHANGE_PACKET_ECNf);
    
    if (fld_val) {
        ecn_config->action_flags |= 
            BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING;
        fld_val = soc_mem_field32_get(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, ECNf);
        ecn_config->new_ecn = fld_val;
    } else {
        ecn_config->action_flags &= 
            ~BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING;
    }

    /* Check Drop action */
    fld_val = soc_mem_field32_get(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, DROPf);
    if (fld_val) {
        ecn_config->action_flags |= 
            BCM_ECN_TRAFFIC_ACTION_EGRESS_DROP;
    } else {
        ecn_config->action_flags &= 
            ~BCM_ECN_TRAFFIC_ACTION_EGRESS_DROP;
    }
    
    
    return BCM_E_NONE;
}


static int
_bcm_esw_ecn_action_egress_set(int unit, 
    bcm_ecn_traffic_action_config_t *ecn_config)
{
    int mem_idx, index_max;
    egr_int_cn_to_ip_mapping_entry_t    entry_egr_int_cn_ip_mapping;
    uint32  fld_val;
    soc_mem_t cfg_mem = 0;
    
    if ((ecn_config->int_cn < 0) || 
        (ecn_config->int_cn > _BCM_ECN_INT_CN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

    if ((ecn_config->ecn > _BCM_ECN_VALUE_MAX) ||
        (ecn_config->new_ecn > _BCM_ECN_VALUE_MAX)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT3_SUPPORT)
        if(SOC_IS_TRIDENT3X(unit)){
            cfg_mem = EGR_INT_CN_TO_PKT_ECN_MAPPINGm;
        } else
#endif
        {
            cfg_mem = EGR_INT_CN_TO_IP_MAPPINGm;
        }

    mem_idx = (ecn_config->int_cn << _BCM_ECN_EGRESS_MEM_INDEX_INT_CN_OFFSET) +
        (ecn_config->ecn << _BCM_ECN_EGRESS_MEM_INDEX_ECN_OFFSET);
    index_max = soc_mem_index_max(unit, cfg_mem);

    if (mem_idx > index_max) {
        return BCM_E_PARAM;
    }
    sal_memset(&entry_egr_int_cn_ip_mapping, 0, 
                    sizeof(entry_egr_int_cn_ip_mapping));

    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, cfg_mem, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_egr_int_cn_ip_mapping));

    /* ECN Marking action */
    if (ecn_config->action_flags & BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING) {

        soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    CHANGE_PACKET_ECNf, 1);
        soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    INCREMENT_ECN_COUNTERf, 1);
        fld_val = ecn_config->new_ecn;
        soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    ECNf, fld_val);
    } else {
        soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    CHANGE_PACKET_ECNf, 0);
        soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    INCREMENT_ECN_COUNTERf, 0);
    }

    /* Drop action */
    if (ecn_config->action_flags & BCM_ECN_TRAFFIC_ACTION_EGRESS_DROP) {
        fld_val = 1;
    } else {
        fld_val = 0;
    }
    soc_mem_field32_set(unit, cfg_mem,
                    &entry_egr_int_cn_ip_mapping, 
                    DROPf, fld_val);

    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, cfg_mem, MEM_BLOCK_ALL,
                mem_idx, (void *)&entry_egr_int_cn_ip_mapping));
    
    
    return BCM_E_NONE;
}
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */

/*
 * Function:
 *      bcm_esw_ecn_traffic_action_config_get
 * Purpose:
 *      To get the actions of the specified ECN traffic.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecn_config - (INOUT) ECN traffic action configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int 
bcm_esw_ecn_traffic_action_config_get(
    int unit, bcm_ecn_traffic_action_config_t *ecn_config)
{
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int rv = BCM_E_NONE;
    
    if (!soc_feature(unit, soc_feature_ecn_wred)) {
        return BCM_E_UNAVAIL;
    }

    if (ecn_config == NULL) {
        return BCM_E_PARAM;
    }

    switch(ecn_config->action_type) {
        case BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE:
            rv = _bcm_esw_ecn_action_enqueue_get(unit, ecn_config);
            break;
        case BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE:
            rv = _bcm_esw_ecn_action_dequeue_get(unit, ecn_config);
            break;
        case BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS:
            rv = _bcm_esw_ecn_action_egress_get(unit, ecn_config);
            break;
        default:
            rv = BCM_E_PARAM;
    }
    
    return rv;
#else /* !BCM_GREYHOUND_SUPPORT && !BCM_TOMAHAWK_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */
}


/*
 * Function:
 *      bcm_esw_ecn_traffic_action_config_set
 * Purpose:
 *      Assign the actions of the specified ECN traffic.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecn_config - (IN) ECN traffic action configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int 
bcm_esw_ecn_traffic_action_config_set(
    int unit, bcm_ecn_traffic_action_config_t *ecn_config)
{
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    int rv = BCM_E_NONE;
    
    if (!soc_feature(unit, soc_feature_ecn_wred)) {
        return BCM_E_UNAVAIL;
    }

    if (ecn_config == NULL) {
        return BCM_E_PARAM;
    }

    if (ecn_config->action_flags &
        ~(BCM_ECN_TRAFFIC_ACTION_ENQUEUE_WRED_RESPONSIVE |
        BCM_ECN_TRAFFIC_ACTION_ENQUEUE_MARK_ELIGIBLE |
        BCM_ECN_TRAFFIC_ACTION_DEQUEUE_CONGESTION_INT_CN_UPDATE |
        BCM_ECN_TRAFFIC_ACTION_DEQUEUE_NON_CONGESTION_INT_CN_UPDATE |
        BCM_ECN_TRAFFIC_ACTION_EGRESS_ECN_MARKING |
        BCM_ECN_TRAFFIC_ACTION_EGRESS_DROP)) {
        return BCM_E_PARAM;
    }

    switch(ecn_config->action_type) {
        case BCM_ECN_TRAFFIC_ACTION_TYPE_ENQUEUE:
            rv = _bcm_esw_ecn_action_enqueue_set(unit, ecn_config);
            break;
        case BCM_ECN_TRAFFIC_ACTION_TYPE_DEQUEUE:
            rv = _bcm_esw_ecn_action_dequeue_set(unit, ecn_config);
            break;
        case BCM_ECN_TRAFFIC_ACTION_TYPE_EGRESS:
            rv = _bcm_esw_ecn_action_egress_set(unit, ecn_config);
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return rv;
#else /* !BCM_GREYHOUND_SUPPORT && !BCM_TOMAHAWK_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_GREYHOUND_SUPPORT || BCM_TOMAHAWK_SUPPORT */
}
#if defined(INCLUDE_L3)
/*
 * Function:
 *      bcm_esw_ecn_map_create
 * Purpose:
 *      To create an ECN mapping profile.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      flags         - (IN) ECN flags.
 *      ecn_map_id    - (IN) ECN map id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_map_create(int unit, uint32 flags, int *ecn_map_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_ecn_wred) &&  
        (flags & BCM_ECN_MAP_TUNNEL_TERM)) {
        rv = bcmi_xgs5_ecn_map_create(unit, flags, ecn_map_id);
        return rv;
    }
#endif
#if (defined(BCM_MPLS_SUPPORT) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
   defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)))
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        rv = bcmi_xgs5_mpls_ecn_map_create(unit, flags, ecn_map_id);
        return rv;
    }
#endif
    return rv;

}

/*
 * Function:
 *      bcm_esw_ecn_map_destroy
 * Purpose:
 *      To destroy an ECN mapping profile.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ecn_map_id  - (IN) ECN map id.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_map_destroy(int unit, int ecn_map_id)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_ecn_wred) &&  
        (_BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM == 
        (ecn_map_id & _BCM_XGS5_ECN_MAP_TYPE_MASK))) {
        rv = bcmi_xgs5_ecn_map_destroy(unit, ecn_map_id);
        return rv;
    }
#endif    
#if (defined(BCM_MPLS_SUPPORT) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
   defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)))
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        rv = bcmi_xgs5_mpls_ecn_map_destroy(unit, ecn_map_id);
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_ecn_map_set
 * Purpose:
 *      To set an ECN mapping entry in  an ECN mapping profile.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      options      - (IN) options, no used currently
 *      ecn_map_id   - (IN) ECN map id.
 *      ecn_map      - (IN)  ECN mapping  values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_map_set(int unit, uint32 options, int ecn_map_id, 
                    bcm_ecn_map_t *ecn_map)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_ecn_wred) && 
        (_BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM == 
        (ecn_map_id & _BCM_XGS5_ECN_MAP_TYPE_MASK))) {
        rv = bcmi_xgs5_ecn_map_set(unit, ecn_map_id, ecn_map);
        return rv;
    }
#endif    
#if (defined(BCM_MPLS_SUPPORT) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
   defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)))
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        rv = bcmi_xgs5_mpls_ecn_map_set(unit, ecn_map_id, ecn_map);
        return rv;
    }
#endif
    return rv;
}


/*
 * Function:
 *      bcm_esw_ecn_map_get
 * Purpose:
 *      To get an ECN mapping entry from  an ECN mapping profile.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      options     - (IN) options, no used currently
 *      ecn_map_id  - (IN) ECN map id.
 *      ecn_map     - (IN/OUT)  ECN mapping  values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */


int
bcm_esw_ecn_map_get(int unit, int ecn_map_id, bcm_ecn_map_t *ecn_map)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_ecn_wred) && 
        (_BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM == 
        (ecn_map_id & _BCM_XGS5_ECN_MAP_TYPE_MASK))) {
        rv = bcmi_xgs5_ecn_map_get(unit, ecn_map_id, ecn_map);
        return rv;
    }
#endif    
#if (defined(BCM_MPLS_SUPPORT) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
   defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)))
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        rv = bcmi_xgs5_mpls_ecn_map_get(unit, ecn_map_id, ecn_map);
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_ecn_port_map_get
 * Purpose:
 *      To get an ECN mapping info from a port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) gport.
 *      ecn_map - (IN/OUT)  ECN mapping  info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_port_map_get(int unit, bcm_gport_t port, bcm_ecn_port_map_t *ecn_map)

{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_ecn_wred) &&
#if defined(BCM_TOMAHAWK3_SUPPORT)
        (!SOC_IS_TOMAHAWK3(unit)) &&
#endif
        (BCM_ECN_INGRESS_PORT_TUNNEL_TERM_MAP & ecn_map->flags)) {
        rv = bcmi_xgs5_ecn_port_map_get(unit, port, ecn_map);
        return rv;
    }
#endif
#if (defined(BCM_MPLS_SUPPORT) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
   defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)))
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        rv = bcmi_xgs5_mpls_ecn_port_map_get(unit, port, ecn_map);
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_ecn_port_map_set
 * Purpose:
 *      To set an ECN mapping info from a port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) gport.
 *      ecn_map - (IN)  ECN mapping  info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_port_map_set(int unit, bcm_gport_t port, bcm_ecn_port_map_t *ecn_map)

{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_ecn_wred) &&
#if defined(BCM_TOMAHAWK3_SUPPORT)
        (!SOC_IS_TOMAHAWK3(unit)) &&
#endif
        (BCM_ECN_INGRESS_PORT_TUNNEL_TERM_MAP & ecn_map->flags)) {
        rv = bcmi_xgs5_ecn_port_map_set(unit, port, ecn_map);
        return rv;
    }
#endif    
#if (defined(BCM_MPLS_SUPPORT) && (defined(BCM_TOMAHAWK2_SUPPORT) || \
   defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)))
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        rv = bcmi_xgs5_mpls_ecn_port_map_set(unit, port, ecn_map);
        return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *      bcm_esw_ecn_map_mode_set
 * Purpose:
 *      To set an ECN mapping mode.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      ecn_map_mode - (IN)  ECN mapping mode info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_map_mode_set(int unit,
    bcm_ecn_map_mode_t *ecn_map_mode)
{
    int rv            = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2_SUPPORT)
    soc_mem_t mem     = EGR_DSCP_ECN_MAPm;
    soc_field_t field = ECN_MODEf;
    egr_dscp_ecn_map_entry_t map_entry;
    int idx_min, idx_max, idx;

    if (soc_feature(unit, soc_feature_ecn_dscp_map_mode)) {
        if (ecn_map_mode == NULL) {
            rv = BCM_E_PARAM;
            return rv;
        }

        if ((ecn_map_mode->dscp < -1) || (ecn_map_mode->dscp > 63)) {
            rv = BCM_E_PARAM;
            return rv;
        }

        if (ecn_map_mode->dscp_map_mode >= bcmEcnDscpMapModeCount ||
            ecn_map_mode->dscp_map_mode < 0) {
            rv = BCM_E_PARAM;
            return rv;
        }

        if (ecn_map_mode->dscp == -1) {
            idx_min = 0;
            idx_max = 64;
        } else {
            idx_min = ecn_map_mode->dscp;
            idx_max = idx_min+1;
        }

        for (idx = idx_min; idx < idx_max; idx++) {
            sal_memset(&map_entry, 0, sizeof(map_entry));
            BCM_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, (void*)&map_entry));
            soc_mem_field32_set(unit, mem, &map_entry, field,
                ecn_map_mode->dscp_map_mode);
            BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, (void*)&map_entry));
        }

        rv = BCM_E_NONE;
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcm_esw_ecn_map_mode_get
 * Purpose:
 *      To get an ECN mapping mode info.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      ecn_map_mode - (IN/OUT)  ECN mapping mode info
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_ecn_map_mode_get(int unit,
    bcm_ecn_map_mode_t *ecn_map_mode)
{
    int rv            = BCM_E_UNAVAIL;
#if defined(BCM_TRIDENT2_SUPPORT)
    soc_mem_t mem     = EGR_DSCP_ECN_MAPm;
    soc_field_t field = ECN_MODEf;
    egr_dscp_ecn_map_entry_t map_entry;
    int idx;
    uint32 fldval;

    if (soc_feature(unit, soc_feature_ecn_dscp_map_mode)) {
        if (ecn_map_mode == NULL) {
            rv = BCM_E_PARAM;
            return rv;
        }
        if ((ecn_map_mode->dscp < 0) || (ecn_map_mode->dscp > 63)) {
            rv = BCM_E_PARAM;
            return rv;
        }


        idx = ecn_map_mode->dscp;
        sal_memset(&map_entry, 0, sizeof(map_entry));
        BCM_IF_ERROR_RETURN(
            soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, (void*)&map_entry));
        fldval = soc_mem_field32_get(unit, mem, (void*)&map_entry, field);
        ecn_map_mode->dscp_map_mode = fldval;

        rv = BCM_E_NONE;
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return rv;
}

#endif /*INCLUDE_L3*/

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void
_bcm_esw_ecn_sw_dump(int unit)
{
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_TOMAHAWKX(unit)) {
        bcmi_xgs5_ecn_sw_dump(unit);
    }
#endif
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

