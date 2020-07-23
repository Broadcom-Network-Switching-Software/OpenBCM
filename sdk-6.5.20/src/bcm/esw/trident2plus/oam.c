/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:
 *  oam.c
 *
 * Purpose:
 *  OAM implementation for trident2p family of devices.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/hash.h>

#include <bcm/oam.h>
#include <bcm/field.h>

#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>

#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>
#include <soc/shared/oam_pm_shared.h>

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#include <bcm_int/esw/fp_oam.h>
#endif

#if defined(INCLUDE_BHH)
#include <bcm_int/esw/bhh_sdk_pack.h>
#endif

#if defined(INCLUDE_MPLS_LM_DM)
#include <bcm_int/esw/mpls_lm_dm.h>
#include <bcm_int/esw/mpls_lm_dm_feature.h>
#include <bcm_int/esw/mpls_lm_dm_sdk_msg.h>
#include <bcm_int/esw/mpls_lm_dm_sdk_pack.h>
#endif /* INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
#include <bcm_int/esw/flex_ctr.h>
#include <bcm_int/esw/firebolt.h>
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */


/* WB sequence for the different modules in OAM */
#if defined(BCM_WARM_BOOT_SUPPORT)
typedef enum bcm_fp_oam_wb_sequence_e {
    bcmFpOamWbSequenceBhh      = 0,
    bcmFpOamWbSequenceMplsLmDm = 1,
    bcmFpOamWbSequenceCount    = 2
} bcm_fp_oam_wb_sequence_t;
#endif


/*******************************************************************************/
/* OAM Hardware configuration for TD2+ & Apache Start                          */
/*******************************************************************************/
/* Magic port used for remote OLP over HG communication */
#define _BCM_OAM_TD2P_OLP_MAGIC_PORT 0x7F

#define _BCM_TD2P_OLP_HDR_TYPE_RX                1
#define _BCM_TD2P_OLP_HDR_TYPE_MACSEC_ENCRYPT    2
#define _BCM_TD2P_OLP_HDR_TYPE_MACSEC_DECRYPT    3

/* OLP_HDR_SUBTYPE filled with one of the following values */
/* Down mep subtypes */
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM             0x01
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM              0x02
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM              0x03
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC        0x04
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_CCM                 0x05
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_LM                  0x06
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_DM                  0x07
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_OTHER_OC            0x08
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BFD                     0x09
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM           0x0A
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DM            0x0B
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM_DM        0x0C
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM           0x0D
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM_DM        0x0E
#define _BCM_TD2P_OLP_HDR_SUBTYPE_SAT                     0x0F
#define _BCM_TD2P_OLP_HDR_SUBTYPE_OTH_ACH                 0x10

/* Up mep subtypes */
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP             0x11
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM_UP              0x12
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM_UP              0x13
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC_UP        0x14
#define _BCM_TD2P_OLP_HDR_SUBTYPE_SAT_UP                     0x15

#define _BCM_TD2P_OLP_HDR_SUBTYPE_MACSEC_ENCRYPT             0x00
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MACSEC_DECRYPT             0x01

typedef struct _bcm_td2p_olp_hdr_type_mapping_s {
    bcm_field_olp_header_type_t     field_olp_hdr_type;
    uint8                           subtype;
} _bcm_td2p_olp_hdr_type_mapping_t;

STATIC _bcm_td2p_olp_hdr_type_mapping_t td2p_olp_hdr_type_mapping[] = {

    /* Field OLP header type to Subtype mapping table
     * Note: Do not change the order, as EGR_OLP_HEADER_TYPE_MAPPING table is
     *       programmed in the same order.
     */

/* BFD */
{bcmFieldOlpHeaderTypeBfdOam, _BCM_TD2P_OLP_HDR_SUBTYPE_BFD},

/* ETH OAM Down MEP*/
{bcmFieldOlpHeaderTypeEthOamCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM},
{bcmFieldOlpHeaderTypeEthOamLm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM},
{bcmFieldOlpHeaderTypeEthOamDm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM},
{bcmFieldOlpHeaderTypeEthOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC},

/* BHH */
{bcmFieldOlpHeaderTypeBhhOamCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_CCM},
{bcmFieldOlpHeaderTypeBhhOamLm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_LM},
{bcmFieldOlpHeaderTypeBhhOamDm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_DM},
{bcmFieldOlpHeaderTypeBhhOamOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_OTHER_OC},

/* MPLS LM/DM */
{bcmFieldOlpHeaderTypeRfc6374Dlm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM},
{bcmFieldOlpHeaderTypeRfc6374Dm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DM},
{bcmFieldOlpHeaderTypeRfc6374DlmPlusDm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM_DM},
{bcmFieldOlpHeaderTypeRfc6374Ilm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM},
{bcmFieldOlpHeaderTypeRfc6374IlmPlusDm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM_DM},

/* Down SAT */
{bcmFieldOlpHeaderTypeSat, _BCM_TD2P_OLP_HDR_SUBTYPE_SAT},

/* Other ACH */
{bcmFieldOlpHeaderTypeOtherAch, _BCM_TD2P_OLP_HDR_SUBTYPE_OTH_ACH},

/* ETH-OAM Up MEP */
{bcmFieldOlpHeaderTypeEthOamUpMepCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepLm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepDm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC_UP},

/* Up SAT*/
{bcmFieldOlpHeaderTypeUpSat, _BCM_TD2P_OLP_HDR_SUBTYPE_SAT_UP},

/* Macsec */
{bcmFieldOlpHeaderTypeMacSecEncrypt, _BCM_TD2P_OLP_HDR_SUBTYPE_MACSEC_ENCRYPT},
{bcmFieldOlpHeaderTypeMacSecDecrypt, _BCM_TD2P_OLP_HDR_SUBTYPE_MACSEC_DECRYPT},

};

static uint8 td2p_olp_hdr_type_count = sizeof(td2p_olp_hdr_type_mapping) /
                                           sizeof(td2p_olp_hdr_type_mapping[0]);
int _bcm_fp_oam_trunk_ports_update_trunk_id(int unit, bcm_trunk_t trunk_id);

#if defined(INCLUDE_BHH)
static int fp_oam_mpls_tp_ach_channel_type = SHR_BHH_ACH_CHANNEL_TYPE;
#endif /* INCLUDE_BHH */

/*
 * Function:
 *    _bcm_td2p_oam_olp_header_type_mapping_set
 * Purpose:
 *     Miscellaneous OAM configurations:
 *         1. Enable IFP lookup on the CPU port.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_olp_header_type_mapping_set(int unit)
{
    int                                 index           = 0;
    int                                 mem_index_count = 0;
    egr_olp_header_type_mapping_entry_t entry;

    mem_index_count = soc_mem_index_count(unit, EGR_OLP_HEADER_TYPE_MAPPINGm);

    if (td2p_olp_hdr_type_count > mem_index_count) {
        return BCM_E_RESOURCE;
    }

    for (index = 0; index < td2p_olp_hdr_type_count; index++) {
        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                     &entry,
                                                     HDR_TYPEf,
                                                     _BCM_TD2P_OLP_HDR_TYPE_RX);

#if defined(BCM_MONTEREY_SUPPORT)
        if (soc_feature(unit, soc_feature_xflow_macsec)) {
            if (td2p_olp_hdr_type_mapping[index].field_olp_hdr_type
                                == bcmFieldOlpHeaderTypeMacSecEncrypt) {
                soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                            &entry,
                                                            HDR_TYPEf,
                                                            _BCM_TD2P_OLP_HDR_TYPE_MACSEC_ENCRYPT);
            } else if (td2p_olp_hdr_type_mapping[index].field_olp_hdr_type
                                == bcmFieldOlpHeaderTypeMacSecDecrypt) {
                soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                            &entry,
                                                            HDR_TYPEf,
                                                            _BCM_TD2P_OLP_HDR_TYPE_MACSEC_DECRYPT);
            }
        }
#endif
        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(
                                            unit,
                                            &entry,
                                            HDR_SUBTYPEf,
                                            td2p_olp_hdr_type_mapping[index].subtype);

        SOC_IF_ERROR_RETURN(
                     WRITE_EGR_OLP_HEADER_TYPE_MAPPINGm(unit,  MEM_BLOCK_ALL,
                                                        index, &entry));
#if defined (BCM_APACHE_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
        if (soc_feature(unit, soc_feature_ep_redirect_v2) ||
            soc_feature(unit, soc_feature_xflow_macsec)) {
            SOC_IF_ERROR_RETURN(
                       WRITE_EGR_OLP_HEADER_TYPE_MAPPING_1m(unit,  MEM_BLOCK_ALL,
                                                            index, &entry));
        }
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_oam_hg_olp_enable
 * Purpose:
 *    Enable OLP on HG ports by default
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_hg_olp_enable(int unit)
{
    bcm_pbmp_t                     ports;
    bcm_port_t                     port;
    iarb_ing_port_table_entry_t    entry;

    BCM_PBMP_ASSIGN(ports, PBMP_PORT_ALL(unit));

    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PORT_TABLEm,
                                             MEM_BLOCK_ANY, port, &entry));

            soc_IARB_ING_PORT_TABLEm_field32_set(unit, &entry, OLP_ENABLEf, 1);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PORT_TABLEm,
                                              MEM_BLOCK_ALL, port, &entry));
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_td2p_oam_olp_magic_port_set
 * Purpose:
 *     Set Magic port used for remote OLP over HG communication
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_magic_port_set(int unit)
{
    int    modid;

    /* Configure modid and the magic port */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));

    SOC_IF_ERROR_RETURN(
          soc_reg_field32_modify(unit, IARB_OLP_CONFIG_1r,
                                 REG_PORT_ANY, MY_MODIDf, modid));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                               IARB_OLP_CONFIG_1r,
                                               REG_PORT_ANY,
                                               MY_PORT_NUMf,
                                               _BCM_OAM_TD2P_OLP_MAGIC_PORT));

    return (BCM_E_NONE);
}

#if defined (BCM_APACHE_SUPPORT)
/*
 * Function:
 *      _bcm_apache_oam_drop_control_fp_encode
 * Purpose:
 *      Convert a Drop Control mep_type to FP
 *      data and mask
 * Parameters:
 *      mep_type - (IN)  Mep type
 *      data     - (OUT) Data for FP qualifier
 *      mask     - (OUT) Mask for FP qualifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_apache_oam_drop_control_fp_encode(bcm_field_oam_drop_mep_type_t mep_type,
                                       uint8 *data,
                                       uint8 *mask)
{

    int data_pkt = FALSE;
    int drop_vector_mask_index;

    switch (mep_type) {
        case bcmFieldOamDropPortDownMepData:
        case bcmFieldOamDropPortDownMepControl:
            drop_vector_mask_index = 0;
            break;

        case bcmFieldOamDropInnerVlanDownMepData:
        case bcmFieldOamDropInnerVlanDownMepControl:
            drop_vector_mask_index = 1;
            break;

        case bcmFieldOamDropOuterVlanDownMepData:
        case bcmFieldOamDropOuterVlanDownMepControl:
        case bcmFieldOamDropInnerPlusOuterVlanDownMepData:
        case bcmFieldOamDropInnerPlusOuterVlanDownMepControl:
            drop_vector_mask_index = 2;
            break;

        case bcmFieldOamDropInnerVlanUpMepData:
            drop_vector_mask_index = 3;
            break;

        case bcmFieldOamDropOuterVlanUpMepData:
        case bcmFieldOamDropInnerPlusOuterVlanUpMepData:
            drop_vector_mask_index = 4;
            break;

        case bcmFieldOamDropSectionPortMepData:
        case bcmFieldOamDropSectionInnerVlanMepData:
        case bcmFieldOamDropSectionOuterVlanMepData:
        case bcmFieldOamDropSectionInnerPlusOuterVlanMepData:
            drop_vector_mask_index = 5;
            break;

        case bcmFieldOamDropLSPMepData:
        case bcmFieldOamDropPwMepData:
            drop_vector_mask_index = 6;
            break;

        case bcmFieldOamDropSectionPortMepControl:
        case bcmFieldOamDropSectionInnerVlanMepControl:
        case bcmFieldOamDropSectionOuterVlanMepControl:
        case bcmFieldOamDropSectionInnerPlusOuterVlanMepControl:
        case bcmFieldOamDropLSPMepControl:
        case bcmFieldOamDropPwMepControl:
            drop_vector_mask_index = 7;
            break;

        default:
            return BCM_E_PARAM;
    }

    switch (mep_type) {
        case bcmFieldOamDropPortDownMepData:
        case bcmFieldOamDropInnerVlanDownMepData:
        case bcmFieldOamDropOuterVlanDownMepData:
        case bcmFieldOamDropInnerPlusOuterVlanDownMepData:
        case bcmFieldOamDropInnerVlanUpMepData:
        case bcmFieldOamDropOuterVlanUpMepData:
        case bcmFieldOamDropInnerPlusOuterVlanUpMepData:
        case bcmFieldOamDropSectionPortMepData:
        case bcmFieldOamDropSectionInnerVlanMepData:
        case bcmFieldOamDropSectionOuterVlanMepData:
        case bcmFieldOamDropSectionInnerPlusOuterVlanMepData:
        case bcmFieldOamDropLSPMepData:
        case bcmFieldOamDropPwMepData:
            data_pkt = TRUE;
            break;

        default:
            data_pkt = FALSE;
            break;
    }

    if (TRUE == data_pkt) {
        *data = 1 << drop_vector_mask_index;
    } else {
        *data = 0;
    }

    *mask = 1 << drop_vector_mask_index;

    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_apache_drop_vector_mask_default_set
 * Purpose:
 *     Program the DROP_VECTOR_MASKS_x with the drop reasons
 *     per MEP.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_apache_drop_vector_mask_default_set (int unit)
{
    int index;
    drop_vector_mask_entry_t drop_vector_mask_entry;
    int entry_count;
    uint32 entry[][3] = {
                            {0x00003200, 0x00000000, 0x00B013F0},
                            {0x0000B200, 0x00000800, 0x00B013F0},
                            {0x0000B200, 0x00008800, 0x00F013F0},
                            {0x0000B201, 0x00000800, 0x0CB013F0},
                            {0x0000B201, 0x0000E800, 0x0CF013F0},
                            {0x0000F201, 0x0000E800, 0x00F017F2},
                            {0x0000F201, 0x0030E800, 0x03F017F2},
                            {0x0000F201, 0x0000E800, 0x00F017F2},
                        };

    entry_count = soc_mem_index_count(unit, DROP_VECTOR_MASKm);

    for (index = 0; index < entry_count; index++) {
        soc_DROP_VECTOR_MASKm_field_set(unit,
                                        &drop_vector_mask_entry,
                                        MASKf,
                                        entry[index]);
        SOC_IF_ERROR_RETURN(
                       WRITE_DROP_VECTOR_MASKm(unit,
                                               MEM_BLOCK_ALL,
                                               index,
                                               &drop_vector_mask_entry));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_apache_oam_default_hw_config_set(int unit)
{
    egr_multicast_mac_addr_entry_t egr_multicast_mac_cfg;
    bcm_mac_t                      mac_addr;
    uint64                         mac_addr_64;

    /* Program EGR_MULTICAST_MAC_ADDR with OAM mulicast addresses */
    sal_memset(&egr_multicast_mac_cfg, 0, sizeof(egr_multicast_mac_cfg));

    /* Set ADDR1 to class1 address: 01:80:C2:00:00:30 */
    mac_addr[0] = 0x01;
    mac_addr[1] = 0x80;
    mac_addr[2] = 0xC2;
    mac_addr[3] = 0x00;
    mac_addr[4] = 0x00;
    mac_addr[5] = 0x30;
    soc_EGR_MULTICAST_MAC_ADDRm_mac_addr_set(unit,
                                             &egr_multicast_mac_cfg,
                                             ADDR1f,
                                             mac_addr);

    /* Set ADDR_2 & ADDR3 to class2 address: 01:80:C2:00:00:38 */
    mac_addr[0] = 0x01;
    mac_addr[1] = 0x80;
    mac_addr[2] = 0xC2;
    mac_addr[3] = 0x00;
    mac_addr[4] = 0x00;
    mac_addr[5] = 0x38;
    soc_EGR_MULTICAST_MAC_ADDRm_mac_addr_set(unit,
                                             &egr_multicast_mac_cfg,
                                             ADDR2f,
                                             mac_addr);
    soc_EGR_MULTICAST_MAC_ADDRm_mac_addr_set(unit,
                                             &egr_multicast_mac_cfg,
                                             ADDR3f,
                                             mac_addr);

    SOC_IF_ERROR_RETURN(
                        WRITE_EGR_MULTICAST_MAC_ADDRm(unit,
                                                      MEM_BLOCK_ALL,
                                                      0,
                                                      &egr_multicast_mac_cfg));

    /* Set EGR_MULTICAST_MAC_ADDR_MASK to FF:FF:FF:FF:FF:F8 */
    COMPILER_64_SET(mac_addr_64, 0xFFFF, 0xFFFFFFF8);
    SOC_IF_ERROR_RETURN(
                        WRITE_EGR_MULTICAST_MAC_ADDR_MASKr(unit, mac_addr_64));


    /* Set EGR_LBR_OPCODE to 0x2 */
    SOC_IF_ERROR_RETURN(WRITE_EGR_LBR_OPCODEr(unit, 0x2));

    /* Set default for DROP_VECTOR_MASK[1..8] */
    BCM_IF_ERROR_RETURN(_bcm_apache_drop_vector_mask_default_set(unit));

    return BCM_E_NONE;
}
#endif /* BCM_APACHE_SUPPORT */


/*
 * Function: _bcm_td2p_oam_olp_fp_hw_index_get
 *
 * Purpose:
 *     Get OLP_HDR_TYPE_COMPRESSED corresponding to subtype
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_fp_hw_index_get(int unit,
                                  bcm_field_olp_header_type_t olp_hdr_type,
                                  int *hwindex)
{
    int index;

    for (index = 0; index < td2p_olp_hdr_type_count; index++) {
        if (td2p_olp_hdr_type_mapping[index].field_olp_hdr_type == olp_hdr_type) {
            *hwindex = index;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function: _bcm_td2p_oam_olp_hw_index_olp_type_get
 *
 * Purpose:
 *     Get subtype corresponding to OLP_HDR_TYPE_COMPRESSED
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_hw_index_olp_type_get(int unit,
                                        int hwindex,
                                        bcm_field_olp_header_type_t *olp_hdr_type)
{
    if (hwindex >= td2p_olp_hdr_type_count) {
        return BCM_E_PARAM;
    }

    *olp_hdr_type = td2p_olp_hdr_type_mapping[hwindex].field_olp_hdr_type;

    return BCM_E_NONE;
}
/*******************************************************************************/
/* OAM Hardware configuration for TD2+ & Apache end                            */
/*******************************************************************************/
/*******************************************************************************/
/* PM MACROs for BHH & MPLS LM/DM App start                                    */
/*******************************************************************************/
#if defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
typedef struct _bcm_fp_oam_pm_profile_int_info_s {
    int id_status;
    bcm_oam_pm_profile_info_t pm_profile;
}_bcm_fp_oam_pm_profile_int_info_t;
typedef struct _bcm_fp_oam_pm_profile_control_s {
    _bcm_fp_oam_pm_profile_int_info_t profile_info[_BCM_OAM_MAX_PM_PROFILES];
}_bcm_fp_oam_pm_profile_control_t;

_bcm_fp_oam_pm_profile_control_t *pm_profile_control[SOC_MAX_NUM_DEVICES];

#define _BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, id)          \
                              (pmc->profile_info[id].id_status == 1)
#define _BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, id)             \
                              (&(pmc->profile_info[id].pm_profile))
#define _BCM_FP_OAM_SET_PM_CTRL_PROFILE_IN_USE(pmc, id)      \
                              (pmc->profile_info[id].id_status = 1)
#define _BCM_FP_OAM_SET_PM_CTRL_PROFILE_NOT_IN_USE(pmc, id)  \
                              (pmc->profile_info[id].id_status = 0)

#define _BCM_FP_OAM_PM_PROFILE_REPLACE_FLAG_SET(profile) (profile->flags & \
                                                     BCM_OAM_PM_PROFILE_REPLACE)

#define _BCM_FP_OAM_PM_PROFILE_WITH_ID_FLAG_SET(profile) (profile->flags & \
                                                     BCM_OAM_PM_PROFILE_WITH_ID)

#define _BCM_FP_OAM_PM_PROFILE_ID_VALID(profile_id) \
    ((profile_id >= 0) && (profile_id < _BCM_OAM_MAX_PM_PROFILES))

/* PM raw data events handling macros */
#define _BCM_FP_OAM_EVENT_TYPE_PM_EVENT(event_types) \
                    ((SHR_BITGET(event_types.w, bcmOAMEventBHHRawData)) ||              \
                     (SHR_BITGET(event_types.w, bcmOAMEventMplsLmDmRawData)))

#if defined (INCLUDE_BHH)
#define _BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc) \
                            (oc->pm_bhh_lmdm_data_collection_mode != \
                                _BCM_OAM_PM_COLLECTION_MODE_NONE)
#define _BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_RAW_DATA(oc) \
                            (oc->pm_bhh_lmdm_data_collection_mode == \
                                _BCM_OAM_PM_COLLECTION_MODE_RAW_DATA)
#define _BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc) \
                            (oc->pm_bhh_lmdm_data_collection_mode == \
                                _BCM_OAM_PM_COLLECTION_MODE_PROCESSED_STATS)
#else
#define _BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc) 0
#define _BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_RAW_DATA(oc) 0
#define _BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc) 0
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
#define _BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_ENABLED(oc) \
                            (oc->pm_mpls_lm_dm_data_collection_mode != \
                                _BCM_OAM_PM_COLLECTION_MODE_NONE)
#define _BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)\
                            (oc->pm_mpls_lm_dm_data_collection_mode ==\
                                _BCM_OAM_PM_COLLECTION_MODE_RAW_DATA)
#define _BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_PROCESSED(oc) \
                            (oc->pm_mpls_lm_dm_data_collection_mode ==\
                                _BCM_OAM_PM_COLLECTION_MODE_PROCESSED_STATS)
#else
#define _BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_ENABLED(oc)      0
#define _BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)  0
#define _BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_PROCESSED(oc) 0
#endif /* INCLUDE_MPLS_LM_DM */

#define _BCM_FP_OAM_PM_BHH_EVENT_IDX         0
#define _BCM_FP_OAM_PM_MPLS_LM_DM_EVENT_IDX  1

#define _BCM_FP_OAM_BHH_OAM_PM_EVENT_TYPE(event_type) \
                                 (event_type == bcmOAMEventBHHRawData)

#define PM_READ_DONE_INDEX(event_idx) \
                    (oc->pm_read_done_index[event_idx])
#define PM_WRITE_DONE_INDEX(event_idx) \
                    (oc->pm_write_done_index[event_idx])
#define INCREMENT_PM_WRITE_DONE_INDEX(event_idx) \
               (((oc->pm_write_done_index[event_idx])+1)%BCM_OAM_MAX_PM_BUFFERS)
#define NUM_FREE_BUFFERS(event_idx)\
                    (oc->pm_num_free_raw_data_buffers[event_idx])
#define SET_NUM_FREE_BUFFERS(event_idx, value)\
                    (oc->pm_num_free_raw_data_buffers[event_idx] = value)
#define DECR_NUM_FREE_BUFFERS(event_idx) \
    SET_NUM_FREE_BUFFERS(event_idx, (NUM_FREE_BUFFERS(event_idx) - 1))
#define INCR_NUM_FREE_BUFFERS(event_idx) \
    SET_NUM_FREE_BUFFERS(event_idx, (NUM_FREE_BUFFERS(event_idx) + 1))



#define VALID_PM_RAW_DATA_BUFFER_READ_WRITE_INDEX(index) \
        (index < BCM_OAM_MAX_PM_BUFFERS)

int bcm_fp_oam_pm_profile_detach(int unit, bcm_oam_endpoint_t endpoint_id,
                                 bcm_oam_pm_profile_t profile_id);

#ifdef BCM_WARM_BOOT_SUPPORT
/* Macro for PM profile information size */
#define _BCM_FP_OAM_PM_PROFILE_SCACHE_SIZE \
        (sizeof(_bcm_fp_oam_pm_profile_int_info_t) * (_BCM_OAM_MAX_PM_PROFILES))
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
/*******************************************************************************/
/* PM MACROs for BHH & MPLS LM/DM App end                                      */
/*******************************************************************************/

#if defined(INCLUDE_BHH)
/*
 * Macro:
 *     BCM_WB_XXX
 * Purpose:
 *    OAM module scache version information.
 * Parameters:
 *    (major number, minor number)
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_BHH_WB_VERSION_1_0          SOC_SCACHE_VERSION(1,0)
#define BCM_BHH_WB_VERSION_1_1          SOC_SCACHE_VERSION(1,1)
#define BCM_BHH_WB_VERSION_1_2          SOC_SCACHE_VERSION(1,2)
#define BCM_BHH_WB_VERSION_1_3          SOC_SCACHE_VERSION(1,3)
#define BCM_WB_BHH_DEFAULT_VERSION      BCM_BHH_WB_VERSION_1_3

#endif

#define BHH_COSQ_INVALID          0xFFFF

#define _BCM_FP_OAM_INVALID_NUM_SESSION_ENTRIES 0
#define _BCM_OAM_EP_LEVEL_COUNT (1 << (_BCM_OAM_EP_LEVEL_BIT_COUNT))
#define _BCM_OAM_EP_LEVEL_MAX (_BCM_OAM_EP_LEVEL_COUNT - 1)

/*
 *  * Macro:
 *     _BCM_OAM_BHH_IS_VALID (internal)
 * Purpose:
 *     Check that the BHH feature is available on this unit
 * Parameters:
 *     unit - BCM device number
 * Notes:
 *     Results in return(BCM_E_UNAVAIL),
 */
#define _BCM_OAM_BHH_IS_VALID(unit)                                          \
            do {                                                             \
                if (!soc_feature(unit, soc_feature_bhh)) {                   \
                    return (BCM_E_UNAVAIL);                                  \
                }                                                            \
            } while (0)

#define BCM_OAM_BHH_GET_UKERNEL_GROUP(group) \
        (group - oc->bhh_base_group_id)
#define BCM_OAM_BHH_GET_SDK_GROUP(group) \
        (group + oc->bhh_base_group_id)

#define BCM_OAM_BHH_GET_SDK_EP(ep) \
        (ep + oc->bhh_base_endpoint_id)
#define BCM_OAM_BHH_GET_UKERNEL_EP(ep) \
        (ep - oc->bhh_base_endpoint_id)

#define BCM_OAM_BHH_VALIDATE_EP(_ep_) \
            do {                                                       \
                if (((_ep_) < oc->bhh_base_endpoint_id) ||         \
                    ((_ep_) >= (oc->bhh_base_endpoint_id           \
                                         + oc->bhh_endpoint_count))) { \
                    LOG_ERROR(BSL_LS_BCM_OAM, \
                              (BSL_META_U(unit, \
                                          "OAM(unit %d) Error: Invalid Endpoint ID" \
                                          " = %d.\n"), unit, _ep_));   \
                    _BCM_OAM_UNLOCK(oc);                               \
                    return (BCM_E_PARAM);                              \
                }                                                      \
            } while (0);

#define BHH_TYPE(type) ( (type == bcmOAMEndpointTypeBHHMPLS) ||        \
                (type == bcmOAMEndpointTypeBHHMPLSVccv)||\
                (type == bcmOAMEndpointTypeBhhSection)||\
                (type == bcmOAMEndpointTypeBhhSectionInnervlan)||\
                (type == bcmOAMEndpointTypeBhhSectionOuterVlan)||\
                (type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)\
                         ) 

#define BHH_EP_TYPE(ep) (BHH_TYPE(ep->type)) 

#define BHH_EP_MPLS_SECTION_TYPE(ep) (\
                (ep->type == bcmOAMEndpointTypeBhhSection)||\
                (ep->type == bcmOAMEndpointTypeBhhSectionInnervlan)||\
                (ep->type == bcmOAMEndpointTypeBhhSectionOuterVlan)||\
                (ep->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)\
                                     )

#define BHH_EP_LSP_TYPE(ep) (ep->type == bcmOAMEndpointTypeBHHMPLS)

/*
 * Macro:
 *     _BCM_FP_OAM_BHH_GROUP_INDEX_VALIDATE
 * Purpose:
 *     Validate BHH Group ID value.
 * Parameters:
 *     _group_ - Group ID value.
 */
#define _BCM_FP_OAM_BHH_GROUP_INDEX_VALIDATE(_group_)               \
    do {                                                            \
        if (((_group_) < oc->bhh_base_group_id) ||               \
            ((_group_) >= (oc->bhh_base_group_id +               \
                           oc->bhh_endpoint_count))) {              \
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META("OAM Error: "       \
              "Invalid Group ID = %d.\n"), _group_));               \
            return (BCM_E_PARAM);                                   \
        }                                                           \
    } while (0);



/*
 * Macro:
 *     _BHH_FP_OAM_HASH_DATA_CLEAR
 * Purpose:
 *      Clear hash data memory occupied by one endpoint.
 * Parameters:
 *     _ptr_    - Pointer to endpoint hash data memory. 
 */
#define _BHH_FP_OAM_HASH_DATA_CLEAR(_ptr_) \
            sal_memset(_ptr_, 0, sizeof(_bhh_fp_oam_ep_data_t));

STATIC void
_bcm_fp_oam_bhh_callback_thread(void *param);
STATIC int
_bcm_fp_oam_bhh_event_mask_set(int unit);
STATIC int
_bcm_fp_oam_bhh_endpoint_create(int unit,
                                bcm_oam_endpoint_info_t *endpoint_info);

#endif /* INCLUDE_BHH */

/*******************************************************************************/
/* MPLS LM/DM specific MACROs                                                  */
/*******************************************************************************/
#if defined(INCLUDE_MPLS_LM_DM)
#define MPLS_LM_DM_MAX_RAW_DATA_SESSIONS 128 /* Max sessions when PM is operating
                                              * in Raw data mode
                                              */
#define MPLS_LM_DM_MAX_SESSIONS          256
#define _MPLS_LM_DM_INVALID_SESS_ID      0xFFFFFFFF
#define _MPLS_LM_DM_INVALID_EP_ID      0xFFFFFFFF

#define _MPLS_LM_DM_MPLS_MAX_LABELS    9   /* Max MPLS labels in Apache */

#define MPLS_LM_DM_RX_CHANNEL_UC_VIEW        1


#define _MPLS_LM_DM_ENDPOINT_TYPE(_type_)                                \
    (((_type_) == bcmOAMEndpointTypeMplsLmDmSectionPort)              || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmSectionInnervlan)         || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmSectionOuterVlan)         || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmSectionOuterPlusInnerVlan)|| \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmPw)                       || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmLsp))

#define _MPLS_LM_DM_SECTION_ENDPOINT_TYPE(_type_)                        \
    (((_type_) == bcmOAMEndpointTypeMplsLmDmSectionPort)              || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmSectionInnervlan)         || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmSectionOuterVlan)         || \
     ((_type_) == bcmOAMEndpointTypeMplsLmDmSectionOuterPlusInnerVlan))


#define _MPLS_LM_DM_EP_ID_RANGE(_id_) \
    (((_id_) >= oc->mpls_lm_dm_base_endpoint_id) &&  \
     ((_id_) < (oc->mpls_lm_dm_base_endpoint_id + oc->mpls_lm_dm_max_ep_count)))

#define _MPLS_LM_DM_SESS_ID_RANGE(_id_) \
    ( ( (_id_) >= 0 ) && ( (_id_) < oc->mpls_lm_dm_max_ep_count) )

#define _MPLS_LM_DM_EP_ID_TO_SESS_ID(_id_)                                     \
    _MPLS_LM_DM_EP_ID_RANGE(_id_) ? ((_id_) - oc->mpls_lm_dm_base_endpoint_id) \
                                  : _MPLS_LM_DM_INVALID_SESS_ID

#define _MPLS_LM_DM_SESS_ID_TO_EP_ID(_id_)                                       \
    _MPLS_LM_DM_SESS_ID_RANGE(_id_) ? ((_id_) + oc->mpls_lm_dm_base_endpoint_id) \
                                    : _MPLS_LM_DM_INVALID_EP_ID

#define _MPLS_LM_DM_HW_SESS_ID_GET(_pool_, _index_)  \
    ((((_pool_) << 12) & 0x7000) | ((_index_) & 0xfff))

#define _MPLS_LM_DM_EP_ITER(_ep_, _idx_)                          \
    for((_idx_) = 0, (_ep_) = &(oc->mpls_lm_dm_ep_data[0]);       \
        (_idx_) < oc->mpls_lm_dm_max_ep_count;                    \
        (_idx_)++, (_ep_)++)

#define _MPLS_LM_DM_OLP_REPL_OFFSET_DLM          24
#define _MPLS_LM_DM_OLP_REPL_OFFSET_DM           12

#if defined (BCM_WARM_BOOT_SUPPORT)
typedef struct _bcm_fp_oam_mpls_lm_dm_scache_v0_global_data_s {
    uint8              uc_num;
    uint8              ukernel_ready;
    uint8              rx_channel;
    uint16             max_ep_count;
    bcm_oam_endpoint_t base_endpoint_id;
} _bcm_fp_oam_mpls_lm_dm_scache_v0_global_data_t;

typedef struct _bcm_fp_oam_mpls_lm_dm_scache_v0_ep_data_s {
    uint8                      in_use;
    uint8                      type;
    uint8                      vccv_type;
    uint8                      trunk_index;
    uint8                      session_num_entries;
    uint8                      lm_ctr_offset[BCM_OAM_LM_COUNTER_MAX];
    uint8                      lm_ctr_action[BCM_OAM_LM_COUNTER_MAX];
    uint8                      lm_ctr_size;
    uint8                      dm_ctr_offset[BCM_OAM_LM_COUNTER_MAX];
    uint8                      dm_ctr_size;
    uint8                      lm_exp;
    uint8                      dm_exp;
    uint8                      outer_pri;
    uint8                      inner_pri;
    uint8                      lm_int_pri;
    uint8                      dm_int_pri;
    uint8                      dst_mac[6];
    uint8                      src_mac[6];
    uint8                      pm_profile;
    uint16                     inner_vlan;
    uint16                     outer_vlan;
    uint16                     vpn;
    uint16                     outer_tpid;
    uint16                     inner_tpid;
    uint32                     flags;
    uint32                     lm_ctr_base_id[BCM_OAM_LM_COUNTER_MAX];
    uint32                     dm_ctr_base_id[BCM_OAM_LM_COUNTER_MAX];
    uint32                     session_id;
    uint32                     egress_label;
    bcm_gport_t                gport;
    bcm_if_t                   intf_id;
} _bcm_fp_oam_mpls_lm_dm_scache_v0_ep_data_t;

#define _MPLS_LM_DM_SCACHE_V0_SIZE                                   \
   ((sizeof(_bcm_fp_oam_mpls_lm_dm_scache_v0_global_data_t)        +   \
    (sizeof(_bcm_fp_oam_mpls_lm_dm_scache_v0_ep_data_t) *            \
                                     oc->mpls_lm_dm_max_ep_count)) + \
    _BCM_FP_OAM_PM_PROFILE_SCACHE_SIZE)


#define BCM_WB_MPLS_LM_DM_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_MPLS_LM_DM_DEFAULT_VERSION  BCM_WB_MPLS_LM_DM_VERSION_1_0

#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int _bcm_fp_oam_mpls_lm_dm_loss_delete(int unit, bcm_oam_endpoint_t id);
STATIC int _bcm_fp_oam_mpls_lm_dm_delay_delete(int unit, bcm_oam_endpoint_t id);
STATIC int _bcm_fp_mpls_lm_dm_event_mask_set(int unit);

#endif /* INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
/*******************************************************************************/
/* Common supporting functions used by CCM and BHH Embedded Apps Start */
/*******************************************************************************/

/* External functions definition */
extern void _bcm_esw_stat_get_counter_id_info(
                                         int                   unit,
                                         uint32                stat_counter_id,
                                         bcm_stat_group_mode_t *group,
                                         bcm_stat_object_t     *object,
                                         uint32                *mode,
                                         uint32                *pool_number,
                                         uint32                *base_idx);

extern int
_bcm_olp_l2_hdr_get(int unit, int glp, uint8 nbo, soc_olp_l2_hdr_t *l2hdr);
extern int soc_uc_in_reset(int unit, int uC);
extern int soc_uc_status(int unit, int uC, int *status);

#define BCM_OAM_ABS(x)   (((x) < 0) ? (-(x)) : (x))
/*
 *Macro:
 *     _BCM_OAM_LOCK
 * Purpose:
 *     Lock take the OAM control mutex
 * Parameters:
 *     control - Pointer to OAM control structure.
 */
#define _BCM_OAM_LOCK(control) \
    sal_mutex_take((control)->oc_lock, sal_mutex_FOREVER)


/*
 * Macro:
 *     _BCM_OAM_UNLOCK
 * Purpose:
 *     Lock take the OAM control mutex
 * Parameters:
 *     control - Pointer to OAM control structure.
 */
#define _BCM_OAM_UNLOCK(control) \
    sal_mutex_give((control)->oc_lock);


/*
 *Macro:
 *     _BCM_OAM_IS_INIT
 * Purpose:
 *     Check if module is initialized
 * Parameters:
 *     unit - soc unit number.
 */
#define _BCM_OAM_IS_INIT(unit)                                         \
    do {                                                               \
        if (_fp_oam_control[unit] == NULL) {                           \
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Module " \
                "not initialized\n")));                                \
            return (BCM_E_INIT);                                       \
        }                                                              \
    } while (0)

/*
 * Macro:
 *     _BCM_OAM_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_OAM_ALLOC(_ptr_,_ptype_,_size_,_descr_)             \
    do {                                                         \
        if (NULL == (_ptr_)) {                                   \
           (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
        }                                                        \
        if((_ptr_) != NULL) {                                    \
            sal_memset((_ptr_), 0, (_size_));                    \
        }  else {                                                \
            LOG_ERROR(BSL_LS_BCM_OAM, \
                      (BSL_META("OAM Error: Allocation failure %s\n"), \
                       (_descr_)));                              \
        }                                                        \
    } while (0)

/*
 * Macro:
 *     _BCM_OAM_KEY_PACK
 * Purpose:
 *     Pack the hash table look up key fields.
 * Parameters:
 *     _dest_ - Hash key buffer.
 *     _src_  - Hash key field to be packed.
 *     _size_ - Hash key field size in bytes.
 */
#define _BCM_OAM_KEY_PACK(_dest_,_src_,_size_)            \
    do {                                          \
        sal_memcpy((_dest_), (_src_), (_size_));  \
        (_dest_) += (_size_);                     \
    } while (0)

 /* Device OAM control structure */
_bcm_fp_oam_control_t *_fp_oam_control[SOC_MAX_NUM_DEVICES];


/*
 * Function:
 *     _bcm_fp_oam_control_get
 * Purpose:
 *     Lookup a OAM control config from a bcm device id.
 * Parameters:
 *     unit -  (IN)BCM unit number.
 *     oc   -  (OUT) OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_control_get(int unit, _bcm_fp_oam_control_t **oc)
{
    if (NULL == oc) {
        return (BCM_E_PARAM);
    }

    /* Ensure oam module is initialized. */
    _BCM_OAM_IS_INIT(unit);

    *oc = _fp_oam_control[unit];

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_fp_oam_msg_send_receive
 * Purpose:
 *      Sends given control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      msg_class   - (IN) Message Class
 *      s_subclass  - (IN) Message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 */
STATIC int
_bcm_fp_oam_msg_send_receive(int unit, uint8 msg_class, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len)
{
    int rv;
    _bcm_fp_oam_control_t *oc;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer = NULL;
    int dma_buffer_len = 0, uc_num = 0;
    uint32 uc_rv;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = msg_class;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /* Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive. */
#if defined(INCLUDE_CCM)
    if (msg_class == MOS_MSG_CLASS_CCM) {
        dma_buffer = oc->dma_buffer;
        dma_buffer_len = oc->dma_buffer_len;
        uc_num = oc->ccm_uc_num;
    } else
#endif /* INCLUDE_CCM */
#if defined(INCLUDE_BHH)
    if (msg_class == MOS_MSG_CLASS_BHH) {
        dma_buffer = oc->bhh_dma_buffer;
        dma_buffer_len = oc->bhh_dma_buffer_len;
        uc_num = oc->bhh_uc_num;
    } else
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    if (msg_class == MOS_MSG_CLASS_MPLS_LM_DM) {
        dma_buffer = oc->mpls_lm_dm_dma_buffer;
        dma_buffer_len = oc->mpls_lm_dm_dma_buffer_len;
        uc_num = oc->mpls_lm_dm_uc_num;
    }
#endif /* INCLUDE_MPLS_LM_DM */

    if(dma_buffer == NULL){
        return (BCM_E_INTERNAL);
    }

    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, uc_num, &send, &reply,
                                      _UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    if (rv == SOC_E_TIMEOUT) {
        return (BCM_E_TIMEOUT);
    } else if (rv == SOC_E_INIT) {
        return (BCM_E_INIT);
    } else if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "UC Message Send/Rcv Failed %s\n"), bcm_errmsg(rv)));
        return (BCM_E_INTERNAL);
    }

    /* Convert uController error code to BCM */
    uc_rv = bcm_ntohl(reply.s.data);
    switch(uc_rv) {
    case SHR_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != msg_class) ||
        (reply.s.subclass != r_subclass)))
    {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
          "UC Err or Rcv Sclass not same as Expected %s\n"), bcm_errmsg(rv)));
        return BCM_E_INTERNAL;
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_endpoint_gport_resolve
 * Purpose:
 *     Resolve an endpoint GPORT value to SGLP and DGLP value.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     ep_info_p  - (IN/OUT) Pointer to endpoint information.
 *     src_glp    - (IN/OUT) Pointer to source generic logical port value.
 *     dst_glp    - (IN/OUT) Pointer to destination generic logical port value.
 *     tx_gport   - (OUT)    Physical gport on which OAM pkts should go out
 *     trunk_id   - (IN/OUT) Pointer to trunk id
 *     trunk_member - (OUT)  trunk member on which EP present 
 *
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_endpoint_gport_resolve(int unit,
                                   bcm_oam_endpoint_info_t *ep_info_p,
                                   int *src_glp,
                                   int *dst_glp,
                                   bcm_gport_t *tx_gport,
                                   bcm_trunk_t *trunk_id,
                                   bcm_trunk_member_t *trunk_member)
{
    bcm_module_t        module_id;              /* Module ID */
    bcm_port_t          port_id;                /* Port ID */
    int                 local_id;               /* Hardware ID */
    bcm_trunk_info_t    trunk_info;             /* Trunk information.  */
    bcm_trunk_member_t *member_array = NULL;   /* Trunk member array */
    int                 member_count = 0;       /* Trunk Member count */
    bcm_trunk_t         tid = BCM_TRUNK_INVALID;
    int                 tx_enabled = 1;         /* CCM Tx enabled */
    int                 rv = 0;                 /* Return Value */
    int                 is_local = 0;

    /* Get Trunk ID or (Modid + Port) value from Gport */
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, ep_info_p->gport,
                                               &module_id, &port_id, trunk_id,
                                               &local_id));

    /* Set CCM endpoint Tx status only for local endpoints. */
    if(ep_info_p->type == bcmOAMEndpointTypeEthernet) {
        if (!(ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE)) {
            tx_enabled =
                (ep_info_p->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) ? 1 : 0;
        }
    }

    if (SOC_GPORT_IS_MPLS_PORT(ep_info_p->gport)) {
        if(BCM_TRUNK_INVALID == (*trunk_id)){
            rv = _bcm_esw_modid_is_local(unit, module_id, &is_local);
            if(BCM_SUCCESS(rv) && (is_local)) {
                _bcm_esw_glp_construct(unit, *trunk_id, module_id, port_id,
                                       dst_glp);
                *src_glp = *dst_glp;
            }
        }
    }

    /*
     * If Gport is Trunk type, _bcm_esw_gport_resolve()
     * sets trunk_id. Using Trunk ID, get Dst Modid and Port value.
     */
    if (BCM_GPORT_IS_TRUNK(ep_info_p->gport)) {
        if (BCM_TRUNK_INVALID == *trunk_id)  {
            /* Has to be a valid Trunk. */
            return (BCM_E_PARAM);
        }
    }


    if (BCM_TRUNK_INVALID == *trunk_id) { /* Phy gport */
        BCM_GPORT_MODPORT_SET(*tx_gport, module_id, port_id);
    } else {

        /* Construct Hw SGLP value. */
        BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, *trunk_id,
                                                   module_id, port_id,
                                                   src_glp));

        /*
         * CCM Tx is enabled on a trunk member port.
         * trunk_index value is required to derive the Modid and Port info.
         */
        if (1 == tx_enabled) {

            if (_BCM_OAM_INVALID_INDEX == ep_info_p->trunk_index) {
                /* Invalid Trunk member index passed. */
                return (BCM_E_PORT);
            }

            /* Get count of ports in this trunk. */
            BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, *trunk_id, NULL,
                                                  0, NULL, &member_count));
            if (0 == member_count) {
                /* No members have been added to the trunk group yet */
                return BCM_E_PARAM;
            }

            _BCM_OAM_ALLOC(member_array, bcm_trunk_member_t,
                           sizeof(bcm_trunk_member_t) * member_count,
                           "Trunk info");
            if (NULL == member_array) {
                return (BCM_E_MEMORY);
            }

            /* Get Trunk Info for the Trunk ID. */
            rv = bcm_esw_trunk_get(unit, *trunk_id, &trunk_info,
                                   member_count, member_array, &member_count);
            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return (rv);
            }

            /* Check if the input trunk_index is valid. */
            if (ep_info_p->trunk_index >= member_count) {
                sal_free(member_array);
                return BCM_E_PARAM;
            }

            /* Get the Modid and Port value using Trunk Index value. */
            rv = _bcm_esw_gport_resolve(unit,
                                        member_array[ep_info_p->trunk_index].gport,
                                        &module_id, &port_id, &tid, &local_id);
            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return (rv);
            }

            sal_free(member_array);

            /* Construct Hw DGLP value. */
            BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID,
                                                       module_id, port_id,
                                                       dst_glp));
            *trunk_member = member_array[ep_info_p->trunk_index];

            *tx_gport = member_array[ep_info_p->trunk_index].gport;
        } else {
            *dst_glp = *src_glp;
        }
    }

    /*
     * Application can resolve the trunk and pass the desginated
     * port as Gport value. Check if the Gport belongs to a trunk.
     */
    if ((BCM_TRUNK_INVALID == (*trunk_id)) &&
        (BCM_GPORT_IS_MODPORT(ep_info_p->gport) ||
         BCM_GPORT_IS_LOCAL(ep_info_p->gport))) {

        /* When Gport is ModPort or Port type, _bcm_esw_gport_resolve()
         * returns Modid and Port value. Use these values to make the DGLP
         * value.
         */
        _bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID,
                               module_id, port_id, dst_glp);

        /* Use the Modid, Port value and determine if the port
         * belongs to a Trunk.
         */
        rv = bcm_esw_trunk_find(unit, module_id, port_id, trunk_id);
        if (BCM_SUCCESS(rv)) {
            /*
             * Port is member of a valid trunk.
             * Now create the SGLP value from Trunk ID.
             */
            _bcm_esw_glp_construct(unit, *trunk_id, module_id, port_id, src_glp);
        } else {
            /* Port not a member of trunk. DGLP and SGLP are the same. */
            *src_glp = *dst_glp;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_fp_oam_olp_l2_header_pack
 * Purpose:
 *     Pack OLP L2 Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack OLP Header
 *     l2         - (IN)     Pointer to OLP L2 Header fields structure.
 * Retruns:
 *     Pointer to end of packet buffer
 */
STATIC uint8 *
_bcm_fp_oam_olp_l2_header_pack(uint8 *buffer, soc_olp_l2_hdr_t *l2)
{

    int     i;

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buffer, l2->src_mac[i]);
    }

    /* Vlan tpid */
    _SHR_PACK_U16(buffer, l2->tpid);

    /* Vlan Id */
    _SHR_PACK_U16(buffer, l2->vlan);

    /* Ethertype */
    _SHR_PACK_U16(buffer, l2->etherType);

    return (buffer);
}

/*
 * Function:
 *     _bcm_fp_oam_olp_l2_header_unpack
 * Purpose:
 *     Unpack OLP L2 Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack OLP Header
 *     l2         - (IN)     Pointer to OLP L2 Header fields structure.
 * Retruns:
 *     Pointer to end of packet buffer
 */
STATIC uint8 *
_bcm_fp_oam_olp_l2_header_unpack(uint8 *buffer, soc_olp_l2_hdr_t *l2)
{

    int     i;

    for (i = 0; i < 6; i++) {
        _SHR_UNPACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < 6; i++) {
        _SHR_UNPACK_U8(buffer, l2->src_mac[i]);
    }

    /* Vlan tpid */
    _SHR_UNPACK_U16(buffer, l2->tpid);

    /* Vlan Id */
    _SHR_UNPACK_U16(buffer, l2->vlan);

    /* Ethertype */
    _SHR_UNPACK_U16(buffer, l2->etherType);

    return (buffer);
}

int
bcm_fp_oam_convert_ep_to_time_spec(bcm_time_spec_t* bts, int sec, int ns)
{
    int rv = BCM_E_NONE;

    if (bts != NULL) {
        /* if both seconds and nanoseconds are negative or both positive,
         * then the ucode's subtraction is ok.
         * if seconds and nanoseconds have different signs, then "borrow"
         * 1000000000 nanoseconds from seconds.
         */
        if ((sec < 0) && (ns > 0)) {
            ns -= 1000000000;
            sec += 1;
        } else if ((sec > 0) && (ns < 0)) {
            ns += 1000000000;
            sec -= 1;
        }

        if (ns < 0) {
            /* if still negative, then something else is wrong.
             * the nanoseconds field is the difference between two
             * (non-negative) time-stamps.
             */
            rv = BCM_E_INTERNAL;
        }

        /* if seconds is negative then set the bts is-negative flag,
         * and use the absolute value of seconds & nanoseconds.
         */
        bts->isnegative  = (sec < 0 ? 1 : 0);
        bts->seconds     = BCM_OAM_ABS(sec);
        bts->nanoseconds = BCM_OAM_ABS(ns);
    } else {
        rv = BCM_E_INTERNAL;
    }

    return rv;
}
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */


/*******************************************************************************/
/* PM routines for BHH & MPLS LM/DM app start                                  */
/*******************************************************************************/
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
/*
 * Function:
 *     _bcm_fp_oam_data_collection_modes_init
 * Purpose:
 *     Get the data collection mode for all apps
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     None
 */
void _bcm_fp_oam_data_collection_modes_init(int unit)
{
    _bcm_fp_oam_control_t   *oc = NULL; /* Pointer to control structure.    */
    int rv = BCM_E_NONE;

    rv = _bcm_fp_oam_control_get(unit, &oc);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Oam control get failed "
                            "- %s.\n"), unit, bcm_errmsg(rv)));
        return;
    }
#if defined (INCLUDE_BHH)
    oc->pm_bhh_lmdm_data_collection_mode = soc_property_get(unit,
            spn_BHH_DATA_COLLECTION_MODE, _BCM_OAM_PM_COLLECTION_MODE_NONE);
#endif /* INCLUDE_BHH */
#if defined (INCLUDE_MPLS_LM_DM)
    oc->pm_mpls_lm_dm_data_collection_mode = soc_property_get(unit,
            spn_MPLS_LMDM_DATA_COLLECTION_MODE, _BCM_OAM_PM_COLLECTION_MODE_NONE);
#endif /* INCLUDE_MPLS_LM_DM */
}

/*
 * Function:
 *     _bcm_fp_oam_pm_init
 * Purpose:
 *     Initialize PM module for all apps
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_fp_oam_pm_init(int unit)
{
    /* Profile data structure initialization */
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;
    _bcm_fp_oam_control_t            *oc  = NULL;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

     _bcm_fp_oam_data_collection_modes_init(unit);


    /* Allocate profiles only if one of them has processed stats
     * collection enabled
     */
    if ((_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) ||
        (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_PROCESSED(oc))) {

        _BCM_OAM_ALLOC(pmc, _bcm_fp_oam_pm_profile_control_t,
                sizeof (_bcm_fp_oam_pm_profile_control_t),
                "OAM PM profile control");
        if (!pmc) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_MEMORY;
        }

        pm_profile_control[unit] = pmc;
    }
    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

#if defined (INCLUDE_BHH)
/*
 * Function:
 *     _bcm_fp_oam_bhh_pm_init
 * Purpose:
 *     Initialize PM module for BHH app
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_fp_oam_bhh_pm_init(int unit)
{

    _bcm_fp_oam_control_t            *oc  = NULL;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);


    /*
     * Allocate DMA buffers and raw data buffers.
     *
     * 1) DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     *
     *
     * 2) Allocate RAW data buffers which is provided
     * to Application during PM event callback
     */
    /* DMA buffer */
    if (!oc->ukernel_not_ready) {
        if (_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc)) {
            oc->pm_bhh_dma_buffer_len = sizeof(shr_oam_pm_msg_ctrl_t);
            oc->pm_bhh_dma_buffer = soc_cm_salloc(unit,
                                                  oc->pm_bhh_dma_buffer_len,
                                                  "PM DMA buffer");
            if (!oc->pm_bhh_dma_buffer) {
                _BCM_OAM_UNLOCK(oc);
               return (BCM_E_MEMORY);
            }
            sal_memset(oc->pm_bhh_dma_buffer, 0, oc->pm_bhh_dma_buffer_len);
            if (_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)) {
                /* Raw data buffers */
                oc->pm_bhh_raw_data_num_buffers =
                                soc_property_get(unit,
                                                 spn_BHH_PM_RAW_DATA_BUFFERS, 4);
            }
            /* For first time, when we write this will be
               incremented to zero index */
            PM_WRITE_DONE_INDEX(_BCM_FP_OAM_PM_BHH_EVENT_IDX) = -1;
            /* Initially all the buffers are free */
            NUM_FREE_BUFFERS(_BCM_FP_OAM_PM_BHH_EVENT_IDX) = BCM_OAM_MAX_PM_BUFFERS;
        }
    }
    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_pm_init
 * Purpose:
 *     Initialize PM module for BHH app
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int _bcm_fp_oam_mpls_lm_dm_pm_init(int unit)
{

    _bcm_fp_oam_control_t            *oc  = NULL;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    /* DMA buffer */
    if (oc->mpls_lm_dm_ukernel_ready) {
        if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_ENABLED(oc)) {

            if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)) {
                /* Raw data buffers */
                oc->pm_mpls_lm_dm_raw_data_num_buffers =
                          soc_property_get(unit,
                                           spn_MPLS_LMDM_PM_RAW_DATA_BUFFERS, 4);
                /* For first time, when we write this will be incremented to
                 * zero index
                 */
                PM_READ_DONE_INDEX(_BCM_FP_OAM_PM_MPLS_LM_DM_EVENT_IDX) = -1;
                PM_WRITE_DONE_INDEX(_BCM_FP_OAM_PM_MPLS_LM_DM_EVENT_IDX) = -1;

                /* Initially all the buffers are free */
                NUM_FREE_BUFFERS(_BCM_FP_OAM_PM_MPLS_LM_DM_EVENT_IDX) =
                                                         BCM_OAM_MAX_PM_BUFFERS;

            }
        }
    }
    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}
#endif /* INCLUDE_MPLS_LM_DM */

/*
 * Function:
 *     _bcm_fp_oam_pm_dma_buffer_from_endpoint_type_get
 * Purpose:
 *     Get DMA buffer from endpoint type
 * Parameters:
 *     unit      - (IN)  BCM device number
 *     type      - (OUT) Endpoint type
 * Returns:
 *     Pointer to dma buffer
 */
uint8*
_bcm_fp_oam_pm_dma_buffer_from_endpoint_type_get(int unit,
                                                 bcm_oam_endpoint_type_t type)
{
    _bcm_fp_oam_control_t *oc = NULL;
    int rv = BCM_E_NONE;

    rv = _bcm_fp_oam_control_get(unit, &oc);
    if (BCM_FAILURE(rv)) {
        return NULL;
    }

#if defined (INCLUDE_BHH)
    if (BHH_TYPE(type)) {
        return oc->pm_bhh_dma_buffer;

    } else
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    if (_MPLS_LM_DM_ENDPOINT_TYPE(type)) {
        return oc->mpls_lm_dm_dma_buffer;
    }
#endif /* INCLUDE_MPLS_LM_DM */
    return NULL;
}

int
_bcm_fp_oam_pm_dma_buffer_and_len_from_class_get(int unit, uint8 class,
                                                 uint8 **buffer, int *buffer_len)
{
    _bcm_fp_oam_control_t *oc = NULL;
    int rv = BCM_E_NONE;

    rv = _bcm_fp_oam_control_get(unit, &oc);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    switch(class) {
#if defined(INCLUDE_BHH)
        case MOS_MSG_CLASS_BHH:
            *buffer = oc->pm_bhh_dma_buffer;
            *buffer_len = oc->pm_bhh_dma_buffer_len;
            break;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
        case MOS_MSG_CLASS_MPLS_LM_DM:
            *buffer = oc->mpls_lm_dm_dma_buffer;
            *buffer_len = oc->mpls_lm_dm_dma_buffer_len;
            break;
#endif /* INCLUDE_MPLS_LM_DM */
        default:
            return BCM_E_PARAM;
            break;
    }
    return BCM_E_NONE;
}

uint8* _bcm_fp_oam_pm_dma_buffer_get(int unit, bcm_oam_event_type_t event_type)
{
    _bcm_fp_oam_control_t *oc = NULL;
    int rv = BCM_E_NONE;
    uint8   *buffer = NULL;

    rv = _bcm_fp_oam_control_get(unit, &oc);
    if (BCM_FAILURE(rv)) {
        return buffer;
    }
    switch(event_type) {
#if defined(INCLUDE_BHH)
        case bcmOAMEventBHHRawData:
            buffer = oc->pm_bhh_dma_buffer;
            break;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
        case bcmOAMEventMplsLmDmRawData:
            buffer = oc->mpls_lm_dm_dma_buffer;
            break;
#endif /* INCLUDE_MPLS_LM_DM */
        default:
            buffer = NULL;
            break;
    }
    return buffer;
}

/*
 * Function:
 *      _bcm_fp_oam_pm_msg_send_receive
 * Purpose:
 *      Sends given PM control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      s_class     - (IN) PM message class.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 *
 *     PM Long Control message:
 *     - PM control messages that require send/receive of information
 *       that cannot fit in the uc_msg 'len' and 'data' fields need to
 *       use DMA operations to exchange information (long control message).
 *
 *     - PM convention for long control messages for
 *        'mos_msg_data_t' fields:
 *          'len'    size of the DMA buffer to send to uController
 *          'data'   physical DMA memory address to send or receive
 *
 *      DMA Operations:
 *      - DMA read/write operation is performed when a long BHH control
 *        message is involved.
 *
 *      - Messages that require DMA operation (long control message)
 *        is indicated by MOS_MSG_DMA_MSG().
 *
 *      - Callers must 'pack' and 'unpack' corresponding information
 *        into/from DMA buffer indicated by dma_buffer.
 */
STATIC int
_bcm_fp_oam_pm_msg_send_receive(int unit, uint8 s_class, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len)
{
    int rv = BCM_E_NONE;
    _bcm_fp_oam_control_t *oc;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer = NULL;
    int dma_buffer_len = 0;
    uint32 uc_rv;
    int uc_num = -1;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = s_class;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */

    BCM_IF_ERROR_RETURN(
              _bcm_fp_oam_pm_dma_buffer_and_len_from_class_get(unit, s_class,
                                                               &dma_buffer,
                                                               &dma_buffer_len));

    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    switch (s_class) {
#if defined(INCLUDE_BHH)
        case MOS_MSG_CLASS_BHH:
            uc_num = oc->bhh_uc_num;
            break;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
        case MOS_MSG_CLASS_MPLS_LM_DM:
            uc_num = oc->mpls_lm_dm_uc_num;
            break;
#endif /* INCLUDE_MPLS_LM_DM */
        default:
            uc_num = -1;
            break;
    }

    if (uc_num == -1) {
        return BCM_E_PARAM;
    }
    /* coverity[unreachable] */
    rv = soc_cmic_uc_msg_send_receive(unit, uc_num,
                                      &send, &reply,
                                      _PM_UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    /* We can expect the same class that was sent */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != s_class) ||
        (reply.s.subclass != r_subclass)) {
        return (BCM_E_INTERNAL);
    }

    /* Convert OAM PM uController error code to BCM */
    uc_rv = bcm_ntohl(reply.s.data);
    switch(uc_rv) {
    case SHR_OAM_PM_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_OAM_PM_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_OAM_PM_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_OAM_PM_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_OAM_PM_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_OAM_PM_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_OAM_PM_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_OAM_PM_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_OAM_PM_UC_E_VERSION:
        rv = BCM_E_CONFIG;
        break;
    case SHR_OAM_PM_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    *r_len = bcm_ntohs(reply.s.len);

    return (rv);
}

int _bcm_fp_oam_pm_profile_compare(bcm_oam_pm_profile_info_t *profile_info1,
                                   bcm_oam_pm_profile_info_t *profile_info2)
{
    if (!(sal_memcmp(profile_info1->bin_edges, profile_info2->bin_edges,
                     BCM_OAM_MAX_PM_PROFILE_BIN_EDGES * sizeof(uint32)))) {
        /* Match */
        return 1;
    }
    return 0;
}

int
_bcm_fp_oam_pm_profile_exists(int unit, bcm_oam_pm_profile_info_t *profile_info)
{
    int id = 0;
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return 0;
    }

    for (id = 0; id < _BCM_OAM_MAX_PM_PROFILES; id++) {
        /* If a profile is in use */
        if (_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, id)) {
            if (_bcm_fp_oam_pm_profile_compare(
                                      _BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, id),
                                      profile_info)) {
                return 1;
            }
        }
    }
    return 0;
}

int
_bcm_fp_oam_find_free_pm_profile_id(int unit, int *pm_profile_id)
{
    int id = 0;
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    /* return the lowest unused id */
    for (id = 0; id < _BCM_OAM_MAX_PM_PROFILES; id++) {
        if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, id))) {
            /* If a profile is not in use */
            /* Assign it and return */
            *pm_profile_id = id;
            return BCM_E_NONE;
        }
    }
    return BCM_E_RESOURCE;
}

int
_bcm_fp_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id, int all)
{
    _bcm_fp_oam_pm_profile_control_t  *pmc;
#if defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t             *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t  *ep;
    bcm_oam_endpoint_t                 endpoint_id;
    int                                idx;
#endif /* INCLUDE_MPLS_LM_DM */

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    /* Check it only if it is a delete for a particular profile */
    if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_id)) && !all) {
        /* It is not in use. Return error */
        return BCM_E_NOT_FOUND;
    }

    /* Detach all EPs having this profile */
#if defined(INCLUDE_MPLS_LM_DM)
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _MPLS_LM_DM_EP_ITER(ep, idx) {

        if (!ep->in_use) {
            continue;
        }

        if (ep->pm_profile == profile_id) {
            endpoint_id = _MPLS_LM_DM_SESS_ID_TO_EP_ID(idx);
            BCM_IF_ERROR_RETURN(bcm_fp_oam_pm_profile_detach(unit, endpoint_id,
                                                             profile_id));
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    /* Zero out that profile */
    sal_memset(_BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, profile_id), 0,
                sizeof(bcm_oam_pm_profile_info_t));
    /* Mark it un used */
    _BCM_FP_OAM_SET_PM_CTRL_PROFILE_NOT_IN_USE(pmc, profile_id);

    return BCM_E_NONE;
}

int
bcm_fp_oam_pm_profile_delete(int unit, bcm_oam_pm_profile_t profile_id)
{
    if (!(_BCM_FP_OAM_PM_PROFILE_ID_VALID(profile_id))) {
        return BCM_E_PARAM;
    }
    return _bcm_fp_oam_pm_profile_delete(unit, profile_id, 0);
}

int
bcm_fp_oam_pm_profile_delete_all(int unit)
{
    int id;
    for (id = 0; id < _BCM_OAM_MAX_PM_PROFILES; id++) {
        BCM_IF_ERROR_RETURN(_bcm_fp_oam_pm_profile_delete(unit, id, 1));
    }
    return BCM_E_NONE;
}

int
bcm_fp_oam_pm_profile_create(int unit, bcm_oam_pm_profile_info_t *profile_info)
{
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;
    bcm_oam_pm_profile_info_t        *profile_ptr = NULL;
    int                               pm_profile_id = 0;

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    if (_BCM_FP_OAM_PM_PROFILE_WITH_ID_FLAG_SET(profile_info)) {
        if (!(_BCM_FP_OAM_PM_PROFILE_ID_VALID(profile_info->id))) {
            return BCM_E_PARAM;
        }
    }

    if (_BCM_FP_OAM_PM_PROFILE_REPLACE_FLAG_SET(profile_info)) {
        if (!(_BCM_FP_OAM_PM_PROFILE_WITH_ID_FLAG_SET(profile_info))) {
            /* replace needs the id to be present */
            return BCM_E_PARAM;
        }

        if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_info->id))) {
            /* Trying to replace a non-existent PM profile */
            return BCM_E_NOT_FOUND;
        }

        /* Delete the previous profile. */
        BCM_IF_ERROR_RETURN(bcm_fp_oam_pm_profile_delete(unit,
                                                         profile_info->id));
    }

    if (_BCM_FP_OAM_PM_PROFILE_WITH_ID_FLAG_SET(profile_info)) {

        if (_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_info->id)) {
            /* That profile id is in use. */
            return BCM_E_EXISTS;
        }

        /* Else use that as the id */
        pm_profile_id = profile_info->id;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_fp_oam_find_free_pm_profile_id(unit,
                                                                &pm_profile_id));
    }

    /* Check if such a profile already exists. */
    if (_bcm_fp_oam_pm_profile_exists(unit, profile_info)) {
        return BCM_E_EXISTS;
    }

    /* Copy it to local profile information */
    profile_ptr = _BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, pm_profile_id);
    sal_memcpy(profile_ptr, profile_info,
                sizeof(bcm_oam_pm_profile_info_t));

    /* Update the id in the profile */
    profile_ptr->id = profile_info->id =  pm_profile_id;


    /* Current flags have only transient meaning, so clear them */
    profile_ptr->flags = 0;

    /* Mark that profile used */
    _BCM_FP_OAM_SET_PM_CTRL_PROFILE_IN_USE(pmc, pm_profile_id);


    return BCM_E_NONE;
}

int
bcm_fp_oam_pm_profile_get(int unit, bcm_oam_pm_profile_info_t *profile_info)
{
    _bcm_fp_oam_pm_profile_control_t *pmc;
    int                               profile_id = profile_info->id;

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    if (profile_info->id >= _BCM_OAM_MAX_PM_PROFILES) {
        return BCM_E_PARAM;
    }

    if (!(_BCM_FP_OAM_PM_PROFILE_ID_VALID(profile_id))) {
        return BCM_E_PARAM;
    }

    if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
        /* It is not in use. Return error */
        return BCM_E_NOT_FOUND;
    }

    /* Copy the profile to the profile structure passed */
    sal_memcpy(profile_info, _BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, profile_id),
               sizeof(bcm_oam_pm_profile_info_t));

    return BCM_E_NONE;
}

int bcm_fp_oam_pm_profile_attach(int unit, bcm_oam_endpoint_t endpoint_id,
                                 bcm_oam_pm_profile_t profile_id)
{
    _bcm_fp_oam_pm_profile_control_t    *pmc = NULL;
    bcm_oam_pm_profile_info_t           *profile_info = NULL;
    bcm_oam_pm_profile_t                *profile = NULL;
    _bcm_fp_oam_control_t               *oc = NULL;
    shr_oam_pm_msg_ctrl_profile_attach_t msg;
    uint8                                msg_class = 0;
    uint16                               reply_len = 0;
    uint8                                msg_subclass = 0;
    uint8                                msg_reply_subclass = 0;
    uint8                               *buffer = NULL, *buffer_ptr = NULL;
    uint16                               buffer_len;
#if defined(INCLUDE_BHH)
    _bhh_fp_oam_ep_data_t               *h_data_p = NULL;
    int                                  bhh_pool_ep_idx = 0;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_mpls_lm_dm_ep_data_t    *ep;
    uint16                               idx;
#endif /* INCLUDE_MPLS_LM_DM */
    int                                  ep_found = 0;
    int                                  rv = BCM_E_NONE;

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    if (!(_BCM_FP_OAM_PM_PROFILE_ID_VALID(profile_id))) {
        return BCM_E_PARAM;
    }

    /* Check profile id exists or not */
    if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
        /* It is not in use. Return error */
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined(INCLUDE_BHH)
    if ((endpoint_id >= oc->bhh_base_endpoint_id) &&
        (endpoint_id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count))) {

        ep_found = 1;

        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
        h_data_p =  &oc->bhh_ep_data[bhh_pool_ep_idx];

        if (0 == h_data_p->in_use) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                       unit, endpoint_id));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_NOT_FOUND);
        }

        /* Check if a profile id is already attached to this endpoint */
        if (h_data_p->pm_profile_attached != _BCM_OAM_INVALID_INDEX) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_EXISTS;
        }

        /* Attach it to the endpoint */
        h_data_p->pm_profile_attached = profile_id;
        profile = &(h_data_p->pm_profile_attached);

        /* Send info to u-Kernel about the profile attached with the endpoint */
        sal_memset(&msg, 0, sizeof(msg));
        msg_class                  = MOS_MSG_CLASS_BHH;
        msg_subclass               = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_ATTACH;
        msg_reply_subclass         = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_ATTACH_REPLY;
        msg.sess_id                = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
        buffer                     = oc->pm_bhh_dma_buffer;
    }
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    if(_MPLS_LM_DM_EP_ID_RANGE(endpoint_id)) {

        ep_found = 1;

        if (!oc->mpls_lm_dm_ukernel_ready) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INIT;
        }
        idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(endpoint_id);
        ep = &(oc->mpls_lm_dm_ep_data[idx]);

        if (!ep->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }

        if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED)) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }


        /* Check if a profile id is already attached to this endpoint */
        if (ep->pm_profile != _BCM_OAM_INVALID_INDEX) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_EXISTS;
        }

        /* Attach it to the endpoint */
        ep->pm_profile = profile_id;
        profile = &(ep->pm_profile);

        /* Send info to u-Kernel about the profile attached with the endpoint */
        sal_memset(&msg, 0, sizeof(msg));
        msg_class          = MOS_MSG_CLASS_MPLS_LM_DM;
        msg_subclass       = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_PROFILE_ATTACH;
        msg_reply_subclass = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_PROFILE_ATTACH_REPLY;
        msg.sess_id        = idx;
        buffer             = oc->mpls_lm_dm_dma_buffer;
    }
#endif /* INCLUDE_MPLS_LM_DM */

    if (!ep_found) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    profile_info = _BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, profile_id);

    msg.profile_id    = profile_id;
    msg.profile_flags = profile_info->flags;
    sal_memcpy(&(msg.profile_edges), &(profile_info->bin_edges),
               (sizeof(uint32) * SHR_OAM_PM_MAX_PM_BIN_EDGES));

    buffer_ptr = shr_oam_pm_msg_ctrl_profile_attach_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;


    rv = _bcm_fp_oam_pm_msg_send_receive(unit, msg_class, msg_subclass,
                                         buffer_len, 0, msg_reply_subclass,
                                         &reply_len);

    if(BCM_FAILURE(rv)) {
        *profile = _BCM_OAM_INVALID_INDEX;
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

int bcm_fp_oam_pm_profile_detach(int unit, bcm_oam_endpoint_t endpoint_id,
                                 bcm_oam_pm_profile_t profile_id)
{
    int                               rv = BCM_E_NONE;
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;
    _bcm_fp_oam_control_t            *oc = NULL;
    uint8                             msg_class = 0;
    uint16                            reply_len = 0;
    uint8                             msg_subclass = 0;
    uint8                             msg_reply_subclass = 0;
    uint16                            sess_id = 0;
    int                               ep_found = 0;
    bcm_oam_pm_profile_t             *profile = NULL;
#if defined(INCLUDE_BHH)
    _bhh_fp_oam_ep_data_t            *h_data_p = NULL;
    int                               bhh_pool_ep_idx = 0;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_mpls_lm_dm_ep_data_t  *ep;
    uint16                             idx;
#endif /* INCLUDE_MPLS_LM_DM */

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    if (!(_BCM_FP_OAM_PM_PROFILE_ID_VALID(profile_id))) {
        return BCM_E_PARAM;
    }

    /* Check profile id exists or not */
    if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
        /* It is not in use. Return error */
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined(INCLUDE_BHH)
    if ((endpoint_id >= oc->bhh_base_endpoint_id) &&
        (endpoint_id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count))) {

        ep_found = 1;

        /* Validate endpoint index value. */
        BCM_OAM_BHH_VALIDATE_EP(endpoint_id);

        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
        h_data_p =  &oc->bhh_ep_data[bhh_pool_ep_idx];
        profile = &(h_data_p->pm_profile_attached);

        if (0 == h_data_p->in_use) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                       unit, endpoint_id));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_NOT_FOUND);
        }

        /* Check if the passed profile id is the one that is attached */
        if (profile_id != h_data_p->pm_profile_attached) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }

        msg_class          = MOS_MSG_CLASS_BHH;
        msg_subclass       = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_DETACH;
        msg_reply_subclass = MOS_MSG_SUBCLASS_BHH_PM_PROFILE_DETACH_REPLY;
        sess_id            = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
    }
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    if(_MPLS_LM_DM_EP_ID_RANGE(endpoint_id)) {

        ep_found = 1;

        if (!oc->mpls_lm_dm_ukernel_ready) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INIT;
        }
        idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(endpoint_id);
        ep = &(oc->mpls_lm_dm_ep_data[idx]);

        if (!ep->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }


        /* Attach it to the endpoint */
        profile = &(ep->pm_profile);

        /* Check if the passed profile id is the one that is attached */
        if (profile_id != ep->pm_profile) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }


        /* Send info to u-Kernel about the profile attached with the endpoint */
        msg_class          = MOS_MSG_CLASS_MPLS_LM_DM;
        msg_subclass       = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_PROFILE_DETACH;
        msg_reply_subclass = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_PROFILE_DETACH_REPLY;
        sess_id            = idx;
    }
#endif /* INCLUDE_MPLS_LM_DM */

    if (!ep_found) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    rv = _bcm_fp_oam_pm_msg_send_receive(unit, msg_class, msg_subclass,
                                         sess_id, profile_id, msg_reply_subclass,
                                         &reply_len);
    if(BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }
    /* Detach the profile from endpoint */
    *profile = _BCM_OAM_INVALID_INDEX;

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

int bcm_fp_oam_pm_profile_traverse(int unit,
                                   bcm_oam_pm_profile_traverse_cb cb,
                                   void *user_data)
{
    int                               rv = BCM_E_NONE;
    bcm_oam_pm_profile_info_t        *profile_info = NULL;
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;
    int profile_id;

    /* Validate input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }
    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    for (profile_id = 0; profile_id < _BCM_OAM_MAX_PM_PROFILES; profile_id++) {
        /* Check profile id is in use or not */
        if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, profile_id))) {
            /* Skip that profile id */
            continue;
        }
        profile_info = _BCM_FP_OAM_PM_CTRL_PROFILE_PTR(pmc, profile_id);
        rv = cb(unit, profile_info, user_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: EP=%d callback "
                                  "failed - %s.\n"),
                       unit, profile_id, bcm_errmsg(rv)));
            return (rv);
        }
    }
    return (rv);
}

void
_bcm_fp_oam_copy_stats_msg_to_stats_ptr(
                                   shr_oam_pm_msg_ctrl_pm_stats_get_t *stats_msg,
                                   bcm_oam_pm_stats_t *stats_ptr,
                                   uint8 pm_stat_extra_elem_feature)
{
    stats_ptr->far_loss_min  = stats_msg->far_loss_min;
    stats_ptr->far_tx_min    = stats_msg->far_tx_min;
    stats_ptr->far_loss_max  = stats_msg->far_loss_max;
    stats_ptr->far_tx_max    = stats_msg->far_tx_max;
    stats_ptr->far_loss      = stats_msg->far_loss;
    stats_ptr->near_loss_min = stats_msg->near_loss_min;
    stats_ptr->near_tx_min   = stats_msg->near_tx_min;
    stats_ptr->near_loss_max = stats_msg->near_loss_max;
    stats_ptr->near_tx_max   = stats_msg->near_tx_max;
    stats_ptr->near_loss     = stats_msg->near_loss;
    stats_ptr->lm_tx_count   = stats_msg->lm_tx_count;
    stats_ptr->DM_min        = stats_msg->DM_min;
    stats_ptr->DM_max        = stats_msg->DM_max;
    if(stats_msg->dm_rx_count) {
        stats_ptr->DM_avg = stats_msg->DM_avg/stats_msg->dm_rx_count;
    }
    stats_ptr->dm_tx_count   = stats_msg->dm_tx_count;
    stats_ptr->profile_id    = stats_msg->profile_id;
    sal_memcpy(&(stats_ptr->bin_counters), &(stats_msg->bin_counters),
               sizeof(uint32) * (SHR_OAM_PM_MAX_PM_BIN_EDGES + 1));
    if(pm_stat_extra_elem_feature) {
        stats_ptr->lm_rx_count             = stats_msg->lm_rx_count;
        stats_ptr->dm_rx_count             = stats_msg->dm_rx_count;
        stats_ptr->far_total_tx_pkt_count  = stats_msg->far_total_tx_pkt_count;
        stats_ptr->near_total_tx_pkt_count = stats_msg->near_total_tx_pkt_count;
    }

    if (stats_msg->flags & SHR_OAM_PM_STATS_FLAG_COUNTER_ROLLOVER) {
        stats_ptr->flags |= BCM_OAM_PM_STATS_PROCESSED_COUNTER_ROLLOVER;
    }
}

int bcm_fp_oam_pm_stats_get(int unit,
                            bcm_oam_endpoint_t endpoint_id,
                            bcm_oam_pm_stats_t *stats_ptr)
{
    _bcm_fp_oam_pm_profile_control_t   *pmc = NULL;
    _bcm_fp_oam_control_t              *oc = NULL;
    shr_oam_pm_msg_ctrl_pm_stats_get_t  stats_msg;
    int                                 rv = BCM_E_NONE;
    uint16                              sess_id = 0;
    uint8                               msg_class = 0;
    uint16                              reply_len = 0;
    uint8                               msg_subclass = 0;
    uint8                               msg_reply_subclass = 0;
    uint8                              *dma_buffer = NULL;
    uint8                               pm_stat_extra_elem_feature = 0;
    int                                 ep_found = 0;
#if defined(INCLUDE_BHH)
    _bhh_fp_oam_ep_data_t              *h_data_p = NULL;
    int                                 bhh_pool_ep_idx = 0;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_mpls_lm_dm_ep_data_t  *ep;
    uint16                             idx;
#endif /* INCLUDE_MPLS_LM_DM */

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined(INCLUDE_BHH)
    if ((endpoint_id >= oc->bhh_base_endpoint_id) &&
        (endpoint_id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count))) {

        ep_found = 1;
        /* Validate endpoint index value. */
        BCM_OAM_BHH_VALIDATE_EP(endpoint_id);

        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
        /* Check endpoint status. */
        rv = shr_idxres_list_elem_state(oc->bhh_pool, bhh_pool_ep_idx);
        if ((BCM_E_EXISTS != rv)) {
            /* Endpoint not in use. */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                       unit, endpoint_id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        h_data_p =  &oc->bhh_ep_data[bhh_pool_ep_idx];

        if (0 == h_data_p->in_use) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                       unit, endpoint_id));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INTERNAL);
        }

        msg_class = MOS_MSG_CLASS_BHH;
        msg_subclass = MOS_MSG_SUBCLASS_BHH_PM_STATS_GET;
        msg_reply_subclass = MOS_MSG_SUBCLASS_BHH_PM_STATS_GET_REPLY;
        sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
        pm_stat_extra_elem_feature = BHH_UC_FEATURE_CHECK(
                                                        BHH_PM_STAT_EXTRA_ELEM);
        dma_buffer = _bcm_fp_oam_pm_dma_buffer_from_endpoint_type_get(unit,
                                                                      h_data_p->type);
    }
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    if(_MPLS_LM_DM_EP_ID_RANGE(endpoint_id)) {
        ep_found = 1;

        if (!oc->mpls_lm_dm_ukernel_ready) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INIT;
        }
        idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(endpoint_id);
        ep = &(oc->mpls_lm_dm_ep_data[idx]);

        if (!ep->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }

        if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_LOSS_ADDED) &&
            !(ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED)) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }

        if (!_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }

        msg_class          = MOS_MSG_CLASS_MPLS_LM_DM;
        msg_subclass       = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_STATS_GET;
        msg_reply_subclass = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_STATS_GET_REPLY;
        sess_id            = idx;
        pm_stat_extra_elem_feature = MPLS_LM_DM_UC_FEATURE_CHECK(
                                                  MPLS_LM_DM_PM_STAT_EXTRA_ELEM);
        dma_buffer = oc->mpls_lm_dm_dma_buffer;
    }
#endif /* INCLUDE_MPLS_LM_DM */

    if (!ep_found) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    /* Pass the buffer pointer in data and sess id in length fields.*/
    rv = _bcm_fp_oam_pm_msg_send_receive(unit, msg_class,
                                         msg_subclass,
                                         sess_id, soc_cm_l2p(unit, dma_buffer),
                                         msg_reply_subclass,
                                         &reply_len);
    if(BCM_FAILURE(rv) || reply_len == 0) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    shr_oam_pm_msg_ctrl_pm_stats_get_unpack(dma_buffer, &stats_msg,
                                            pm_stat_extra_elem_feature);

    _bcm_fp_oam_copy_stats_msg_to_stats_ptr(&stats_msg, stats_ptr,
                                            pm_stat_extra_elem_feature);

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}


int
bcm_fp_oam_pm_profile_attach_get(int unit,
                                 bcm_oam_endpoint_t endpoint_id,
                                 bcm_oam_pm_profile_t *profile_id)
{
    _bcm_fp_oam_pm_profile_control_t *pmc = NULL;
    _bcm_fp_oam_control_t            *oc = NULL;
    int                               ep_found = 0;
#if defined(INCLUDE_BHH)
    _bhh_fp_oam_ep_data_t            *h_data_p = NULL;
    int                               bhh_pool_ep_idx = 0;
    int                               rv = BCM_E_NONE;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_mpls_lm_dm_ep_data_t  *ep;
    uint16                             idx;
#endif /* INCLUDE_MPLS_LM_DM */

    pmc = pm_profile_control[unit];
    if (!pmc) {
        return BCM_E_INIT;
    }

    /* Check profile id exists or not */
    if (!(_BCM_FP_OAM_PM_CTRL_PROFILE_IN_USE(pmc, *profile_id))) {
        /* It is not in use. Return error */
        return BCM_E_PARAM;
    }


    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined(INCLUDE_BHH)
    if ((endpoint_id >= oc->bhh_base_endpoint_id) &&
        (endpoint_id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count))) {

        ep_found = 1;
        /* Validate endpoint index value. */
        BCM_OAM_BHH_VALIDATE_EP(endpoint_id);

        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_id);
        /* Check endpoint status. */
        rv = shr_idxres_list_elem_state(oc->bhh_pool, bhh_pool_ep_idx);
        if ((BCM_E_EXISTS != rv)) {
            /* Endpoint not in use. */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                       unit, endpoint_id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        h_data_p =  &oc->bhh_ep_data[bhh_pool_ep_idx];

        if (0 == h_data_p->in_use) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                       unit, endpoint_id));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INTERNAL);
        }
        /* Get the attached profile */
        *profile_id = h_data_p->pm_profile_attached;
    }
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
    if(_MPLS_LM_DM_EP_ID_RANGE(endpoint_id)) {

        ep_found = 1;

        if (!oc->mpls_lm_dm_ukernel_ready) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INIT;
        }
        idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(endpoint_id);
        ep = &(oc->mpls_lm_dm_ep_data[idx]);

        if (!ep->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_NOT_FOUND;
        }

        /* Check if a profile id is already attached to this endpoint */
        if (ep->pm_profile == _BCM_OAM_INVALID_INDEX) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_EXISTS;
        }
        /* Get the attached profile */
        *profile_id = ep->pm_profile;
    }
#endif /* INCLUDE_MPLS_LM_DM */

    if (!ep_found) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }
    if (*profile_id == _BCM_OAM_INVALID_INDEX) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

void
_bcm_fp_oam_write_dma_buffer_to_raw_buffer(int unit, uint8 *dma_buffer,
                                        bcm_oam_pm_raw_data_t *raw_data, int write_index)
{
    shr_oam_pm_msg_ctrl_raw_buffer_unpack(dma_buffer,
     (shr_oam_pm_msg_ctrl_raw_buffer_t *)(&(raw_data->raw_buffer[write_index])));
}

int _bcm_fp_oam_pm_raw_data_event_index_from_event_get(bcm_oam_event_type_t event_type)
{
    switch (event_type) {
#if defined(INCLUDE_BHH)
        case bcmOAMEventBHHRawData:
            return _BCM_FP_OAM_PM_BHH_EVENT_IDX;
            break;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
        case bcmOAMEventMplsLmDmRawData:
            return _BCM_FP_OAM_PM_MPLS_LM_DM_EVENT_IDX;
            break;
#endif /* INCLUDE_MPLS_LM_DM */
        default:
            return -1;
            break;
    }
    return -1;
}

void _bcm_fp_oam_update_num_free_buffers(_bcm_fp_oam_control_t *oc,
                                         int event_idx, int read_idx,
                                         int write_idx, int last_op)
{
    /* Normal case */
    if (read_idx < write_idx) {
        SET_NUM_FREE_BUFFERS(event_idx,
                (BCM_OAM_MAX_PM_BUFFERS - (write_idx - read_idx)));
    } else if (read_idx > write_idx) {
        /* rollover case */
        SET_NUM_FREE_BUFFERS(event_idx,
                (read_idx - write_idx));
    } else { /* read index = write index case */
        if (last_op == 1) {
            /* Last operation was write, means write idx has caught
               up with read_idx.
               Just decrement the number of buffers by 1.
               Ideally this would mean the num of free buffers would
               become zero */
            DECR_NUM_FREE_BUFFERS(event_idx);
        }  else if (last_op == 0) {
            /* Last operation was read, means read idx has caught
               up with write idx.
               Just increment the number of buffers by 1.
               Ideally this would mean the num of free buffers would
               have become max. */
            INCR_NUM_FREE_BUFFERS(event_idx);
        }
    }
}

/* Returns 0 - Not updated or 1 - Updated */
int
_bcm_fp_oam_update_dma_buffer_into_raw_buffer(int unit,
                                              bcm_oam_event_type_t event_type,
                                              uint8 *dma_buffer,
                                              bcm_oam_pm_raw_data_t *raw_data)
{
    int rv = BCM_E_NONE;
    int event_idx = -1;
    _bcm_fp_oam_control_t *oc;
    int read_idx = -1;
    int write_idx = -1;

    /* Get OAM Control Structure. */
    rv = _bcm_fp_oam_control_get(unit, &oc);
    if (BCM_FAILURE(rv)) {
        return FALSE;
    }

    event_idx = _bcm_fp_oam_pm_raw_data_event_index_from_event_get(event_type);
    if (event_idx == -1) {
        return FALSE;
    }

    if (NUM_FREE_BUFFERS(event_idx) > 0) {
        PM_WRITE_DONE_INDEX(event_idx) = INCREMENT_PM_WRITE_DONE_INDEX
                                                                  (event_idx);
        /* Writing at the incremented index */
        _bcm_fp_oam_write_dma_buffer_to_raw_buffer(unit, dma_buffer, raw_data,
                PM_WRITE_DONE_INDEX(event_idx));
    } else {
        /* There are no free buffers. Do not update */
        return FALSE;
    }
    read_idx = PM_READ_DONE_INDEX(event_idx);
    write_idx = PM_WRITE_DONE_INDEX(event_idx);

    /* 1 passed indicates last operation was write */
    _bcm_fp_oam_update_num_free_buffers(oc, event_idx, read_idx, write_idx, 1);

    raw_data->read_index = PM_READ_DONE_INDEX(event_idx);
    raw_data->write_index = PM_WRITE_DONE_INDEX(event_idx);

    return TRUE;
}

int
bcm_fp_oam_pm_raw_data_read_done(int unit,
                                  bcm_oam_event_types_t event_types,
                                  uint32 read_index)
{
    bcm_oam_event_type_t event_type;
    _bcm_fp_oam_control_t *oc;
    int event_idx = -1;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    /* Do sanity validation of event_types and read_index */
    if (!(_BCM_FP_OAM_EVENT_TYPE_PM_EVENT(event_types))) {
        return BCM_E_PARAM;
    }

    if (!(VALID_PM_RAW_DATA_BUFFER_READ_WRITE_INDEX(read_index))) {
        return BCM_E_PARAM;
    }

    for (event_type = bcmOAMEventBHHRawData;
         event_type <= bcmOAMEventMplsLmDmRawData; event_type++) {
        if (BCM_OAM_EVENT_TYPE_GET(event_types, event_type)) {
            event_idx = _bcm_fp_oam_pm_raw_data_event_index_from_event_get
                                                               (event_type);
            if (event_idx == -1) {
                return BCM_E_PARAM;
            }
            PM_READ_DONE_INDEX(event_idx) = read_index;
            /* 0 passed indicates last operation was read */
            _bcm_fp_oam_update_num_free_buffers(oc, event_idx, read_index,
                                             PM_WRITE_DONE_INDEX(event_idx), 0);
        }
    }
    return BCM_E_NONE;
}

bcm_oam_pm_raw_data_t*
_bcm_fp_oam_pm_raw_data_info_get(int unit,
                                 bcm_oam_event_type_t event_type)
{
    int rv = BCM_E_NONE;
    _bcm_fp_oam_control_t *oc;
    bcm_oam_pm_raw_data_t *buffer = NULL;

    rv = _bcm_fp_oam_control_get(unit, &oc);
    if (BCM_FAILURE(rv)) {
        return NULL;
    }
    switch (event_type) {
#if defined(INCLUDE_BHH)
        case bcmOAMEventBHHRawData:
            buffer = &(oc->pm_bhh_raw_data_info);
            break;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
        case bcmOAMEventMplsLmDmRawData:
            buffer = &(oc->pm_mpls_lm_dm_raw_data_info);
            break;
#endif /* INCLUDE_MPLS_LM_DM */
        default:
            buffer = NULL;
            break;
    }
    return buffer;
}

STATIC void _bcm_fp_oam_pm_event_msg_handle(_bcm_fp_oam_control_t *oc,
                                             mos_msg_data_t *event_msg)
{
    bcm_oam_event_type_t event_type = 0;
    _bcm_oam_pm_event_handler_t *pm_event_handler_p;
    bcm_oam_pm_raw_data_t *raw_data = NULL;
    uint8 *dma_buffer = NULL;
    int is_updated = 0;
#if defined (INCLUDE_MPLS_LM_DM)
    uint32       event_mask;
    int          sess_id;
    uint16       ep_id = 0;
    bcm_oam_event_types_t events;
    _bcm_oam_event_handler_t *event_handler_p;
#endif


    switch (event_msg->s.mclass) {
#if defined(INCLUDE_BHH)
        case MOS_MSG_CLASS_BHH_EVENT:
            event_type = bcmOAMEventBHHRawData;
            break;
#endif /* INCLUDE_BHH */
#if defined(INCLUDE_MPLS_LM_DM)
       case MOS_MSG_CLASS_MPLS_LM_DM_EVENT:
            event_mask = bcm_ntohl(event_msg->s.data);
            if (event_mask & MPLS_LM_DM_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER) {
                event_type = bcmOAMEventMplsLmDmPmCounterRollover;
                /* Get data from event message */
                sess_id = (int)bcm_ntohs(event_msg->s.len);
                ep_id = _MPLS_LM_DM_SESS_ID_TO_EP_ID(sess_id);
                /* Set events */
                sal_memset(&events, 0, sizeof(events));
                LOG_DEBUG(BSL_LS_BCM_OAM,
                          (BSL_META_U(oc->unit,
                           "****** OAM PM MPLS_LM_DM Counter Rollover ******\n")));
                if (oc->event_handler_cnt[bcmOAMEventMplsLmDmPmCounterRollover] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventMplsLmDmPmCounterRollover);
                }
            } else {
                event_type = bcmOAMEventMplsLmDmRawData;
            }
           break;
#endif /* INCLUDE_MPLS_LM_DM */
        default:
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META(
                              "OAM(unit %d) Error: %s called"
                              "for unknown protocol"
                              "- %d.\n"),
                     oc->unit, FUNCTION_NAME(), event_msg->s.mclass));
            return;
    }

    if (oc->event_handler_cnt[event_type] <= 0) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META(
                          "OAM(unit %d) Error: Received notif for"
                          "event without callback - %d.\n"),
                 oc->unit, event_type));
        return;
    }

    /* Loop over registered callbacks
     */
    if (event_type == bcmOAMEventMplsLmDmPmCounterRollover) {
#if defined (INCLUDE_MPLS_LM_DM)
        for (event_handler_p = oc->event_handler_list_p; event_handler_p != NULL;
             event_handler_p = event_handler_p->next_p) {
            if (SHR_BITGET(events.w, event_type)) {
                if (SHR_BITGET(event_handler_p->event_types.w, event_type)) {
                    event_handler_p->cb(oc->unit,
                                        0, event_type,
                                        0,
                                        ep_id,
                                        event_handler_p->user_data);
                }
            }
        }
#endif
    } else {
        for (pm_event_handler_p = oc->pm_event_handler_list_p;
                pm_event_handler_p != NULL;
                pm_event_handler_p = pm_event_handler_p->next_p) {
            if (SHR_BITGET(pm_event_handler_p->event_types.w,
                        event_type)) {
                raw_data   = _bcm_fp_oam_pm_raw_data_info_get(oc->unit, event_type);
                dma_buffer = _bcm_fp_oam_pm_dma_buffer_get(oc->unit, event_type);
                if (!raw_data) {
                    /* Should not happen. This would mean you have received
                     * a PM event for which you have not allocated buffers
                     * itself.
                     */
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(oc->unit,
                                        "OAM(unit %d) Error: Oam raw data get failed"
                                        " event - %d.\n"), oc->unit, event_type));
                    return;
                }
                is_updated = _bcm_fp_oam_update_dma_buffer_into_raw_buffer(oc->unit,
                        event_type,
                        dma_buffer,
                        raw_data);
                if (is_updated) {
                    pm_event_handler_p->cb(oc->unit,
                            event_type,
                            raw_data, /* The raw data */
                            pm_event_handler_p->user_data);
                } else {
                    /* Oops ! The event is lost ! */
                    LOG_WARN(BSL_LS_BCM_OAM,
                            (BSL_META_U(oc->unit,
                                        "OAM(unit %d) Error: PM event missed "
                                        "event - %d.\n"), oc->unit, event_type));
                }
            }
        }
    }
}

/*
 * Function:
 *     bcm_fp_oam_pm_event_register
 * Purpose:
 *     Register a callback for handling OAM PM events
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM PM events for which the specified
 *                        callback should be called.
 *     cb          - (IN) A pointer to the callback function to call for
 *                        the specified OAM PM events
 *     user_data   - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_pm_event_register(int unit, bcm_oam_event_types_t event_types,
                             bcm_oam_pm_event_cb cb, void *user_data)
{
    _bcm_fp_oam_control_t       *oc;
    _bcm_oam_pm_event_handler_t *event_h_p;
    _bcm_oam_pm_event_handler_t *prev_p = NULL;
    int                          rv;             /* Operation return status. */
    uint16                       reply_len = 0;
    uint8                        msg_class = 0;
    uint8                        msg_subclass = 0;
    uint8                        msg_reply_subclass = 0;
    uint8                       *dma_buffer = NULL;

    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Validate the event type to be of PM event type */
    if (!(_BCM_FP_OAM_EVENT_TYPE_PM_EVENT(event_types))) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->pm_event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {

        _BCM_OAM_ALLOC(event_h_p, _bcm_oam_pm_event_handler_t,
                       sizeof(_bcm_oam_pm_event_handler_t), "OAM event handler");

        if (NULL == event_h_p) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Event handler alloc failed -"
                                   " %s.\n"), unit, bcm_errmsg(BCM_E_MEMORY)));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_MEMORY);
        }

        event_h_p->next_p = NULL;
        event_h_p->cb = cb;

        SHR_BITCLR_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount);
        if (prev_p != NULL) {
            prev_p->next_p = event_h_p;
        } else {
            oc->pm_event_handler_list_p = event_h_p;
        }
    }

#if defined(INCLUDE_BHH)
    if (SHR_BITGET(event_types.w, bcmOAMEventBHHRawData)) {
        if (!SHR_BITGET(event_h_p->event_types.w, bcmOAMEventBHHRawData)) {
            /* Add this event to the registered events list. */
            SHR_BITSET(event_h_p->event_types.w, bcmOAMEventBHHRawData);
            msg_class          = MOS_MSG_CLASS_BHH;
            msg_subclass       = MOS_MSG_SUBCLASS_BHH_PM_RAW_DATA_EVENT_SET;
            msg_reply_subclass =
                         MOS_MSG_SUBCLASS_BHH_PM_RAW_DATA_EVENT_SET_REPLY;
            dma_buffer = oc->pm_bhh_dma_buffer;
            rv = _bcm_fp_oam_pm_msg_send_receive(unit, msg_class,
                                                 msg_subclass,
                                                 0, soc_cm_l2p(unit, dma_buffer),
                                                 msg_reply_subclass,
                                                 &reply_len);
            if(BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                return BCM_E_INTERNAL;
            }
        }
        oc->event_handler_cnt[bcmOAMEventBHHRawData]++;
    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    if (oc->mpls_lm_dm_ukernel_ready) {
        if (SHR_BITGET(event_types.w, bcmOAMEventMplsLmDmRawData)) {
            if (!SHR_BITGET(event_h_p->event_types.w, bcmOAMEventMplsLmDmRawData)) {
                /* Add this event to the registered events list. */
                SHR_BITSET(event_h_p->event_types.w, bcmOAMEventMplsLmDmRawData);
                msg_class   = MOS_MSG_CLASS_MPLS_LM_DM;
                msg_subclass = MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_RAW_DATA_EVENT_SET;
                msg_reply_subclass =
                    MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_RAW_DATA_EVENT_SET_REPLY;
                dma_buffer = oc->mpls_lm_dm_dma_buffer;

                rv = _bcm_fp_oam_pm_msg_send_receive(unit, msg_class,
                                                     msg_subclass,
                                                     0, soc_cm_l2p(unit,
                                                                   dma_buffer),
                                                     msg_reply_subclass,
                                                     &reply_len);
                if(BCM_FAILURE(rv)) {
                    _BCM_OAM_UNLOCK(oc);
                    return BCM_E_INTERNAL;
                }
            }
            oc->event_handler_cnt[bcmOAMEventMplsLmDmRawData]++;
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */



    event_h_p->user_data = user_data;

    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_fp_oam_pm_event_unregister
 * Purpose:
 *     Remove a registered event from the event handler list.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM PM events for which the specified
 *                        callback should not be called
 *     cb          - (IN) A pointer to the callback function to unregister
 *                        from the specified OAM PM events
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_pm_event_unregister(int unit, bcm_oam_event_types_t event_types,
                             bcm_oam_pm_event_cb cb)
{
    _bcm_fp_oam_control_t       *oc;
    _bcm_oam_pm_event_handler_t *event_h_p;
    _bcm_oam_pm_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t         e_type;
    int                          rv; /* Operation return status. */
    uint8                        msg_class = 0;
    uint16                       reply_len = 0;
    uint8                        msg_subclass = 0;
    uint8                        msg_reply_subclass = 0;

    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Validate the event type to be of PM event type */
    if (!_BCM_FP_OAM_EVENT_TYPE_PM_EVENT(event_types)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->pm_event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }
    if (NULL == event_h_p) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_NOT_FOUND);
    }

#if defined(INCLUDE_BHH)
    e_type = bcmOAMEventBHHRawData;
    if (SHR_BITGET(event_types.w, e_type)) {
        if ((oc->event_handler_cnt[e_type] > 0)
                && SHR_BITGET(event_h_p->event_types.w, e_type)) {

            /* Remove this event from the registered events list. */
            SHR_BITCLR(event_h_p->event_types.w, e_type);

            oc->event_handler_cnt[e_type] -= 1;

            if (oc->event_handler_cnt[e_type] == 0) {
                /* There are no event handlers for this event type
                 * Send msg to uKernel to stop notifying this event.
                 */
                if (e_type == bcmOAMEventBHHRawData) {
                    msg_class  = MOS_MSG_CLASS_BHH;
                    msg_subclass =
                        MOS_MSG_SUBCLASS_BHH_PM_RAW_DATA_EVENT_UNSET;
                    msg_reply_subclass =
                        MOS_MSG_SUBCLASS_BHH_PM_RAW_DATA_EVENT_UNSET_REPLY;
                }
                rv = _bcm_fp_oam_pm_msg_send_receive
                    (unit, msg_class,
                     msg_subclass,
                     0, 0,
                     msg_reply_subclass,
                     &reply_len);
                if(BCM_FAILURE(rv)) {
                    _BCM_OAM_UNLOCK(oc);
                    return BCM_E_INTERNAL;
                }
            }
        }
    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    e_type = bcmOAMEventMplsLmDmRawData;
    if (SHR_BITGET(event_types.w, e_type)) {
        if ((oc->event_handler_cnt[e_type] > 0)
                && SHR_BITGET(event_h_p->event_types.w, e_type)) {

            /* Remove this event from the registered events list. */
            SHR_BITCLR(event_h_p->event_types.w, e_type);

            oc->event_handler_cnt[e_type] -= 1;

            if (oc->event_handler_cnt[e_type] == 0) {
                /* There are no event handlers for this event type
                 * Send msg to uKernel to stop notifying this event.
                 */
                msg_class  = MOS_MSG_CLASS_MPLS_LM_DM;
                msg_subclass =
                    MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_RAW_DATA_EVENT_UNSET;
                msg_reply_subclass =
                    MOS_MSG_SUBCLASS_MPLS_LM_DM_PM_RAW_DATA_EVENT_UNSET_REPLY;

                rv = _bcm_fp_oam_pm_msg_send_receive(unit, msg_class,
                                                     msg_subclass,
                                                     0, 0,
                                                     msg_reply_subclass,
                                                     &reply_len);
                if(BCM_FAILURE(rv)) {
                    _BCM_OAM_UNLOCK(oc);
                    return BCM_E_INTERNAL;
                }
            }
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    if (!_BCM_FP_OAM_EVENT_TYPE_PM_EVENT(event_h_p->event_types)) {
        /* No more PM events for this callback registered
           Delete the node. */
        if (NULL != prev_p) {
            prev_p->next_p = event_h_p->next_p;

        } else {
            oc->pm_event_handler_list_p = event_h_p->next_p;
        }
        sal_free(event_h_p);
    }

    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

#if defined(INCLUDE_MPLS_LM_DM)
/*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_pm_callback_thread_func
 * Purpose:
 *      Thread to listen for event messages MPLS LM/DM uC for PM events.
 * Parameters:
 *      oc - Pointer to oam control structures.
 *      events - the set of events that we are listening.
 * Returns:
 *      None
 */
STATIC void
_bcm_fp_oam_mpls_lm_dm_pm_callback_thread_func(_bcm_fp_oam_control_t *oc,
                                               bcm_oam_event_types_t event_types)
{
    int            rv;
    mos_msg_data_t event_msg;

    if (!(BCM_OAM_EVENT_TYPE_GET(event_types, bcmOAMEventMplsLmDmRawData) ||
          BCM_OAM_EVENT_TYPE_GET(event_types, bcmOAMEventMplsLmDmPmCounterRollover))) {
        /* No other event is currently handled here. return. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(oc->unit,
                            "OAM(unit %d) Error: %s called for unknown "
                            "event.\n"), oc->unit, FUNCTION_NAME()));
        return;
    }

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(oc->unit, oc->mpls_lm_dm_uc_num,
                                     MOS_MSG_CLASS_MPLS_LM_DM_EVENT,
                                     &event_msg,
                                     sal_sem_FOREVER);

        if (BCM_FAILURE(rv)) {
            break;  /*  Thread exit */
        } else {
            if (event_msg.s.subclass == _MPLS_LM_DM_EVENT_OAM_PM_EVENT) {
                _bcm_fp_oam_pm_event_msg_handle(oc, &event_msg);
            } else {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META(
                                  "OAM(unit %d) Error: Unknown PM event"
                                  "- %d.\n"),
                         oc->unit, event_msg.s.subclass));
            }
        }
    }

    oc->mpls_lm_dm_pm_event_thread_id = NULL;
    sal_thread_exit(0);
}

STATIC void
_bcm_fp_oam_mpls_lm_dm_pm_thread(void *param)
{
    bcm_oam_event_types_t event_types = {{0}};
    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventMplsLmDmRawData);
    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventMplsLmDmPmCounterRollover);

    _bcm_fp_oam_mpls_lm_dm_pm_callback_thread_func((_bcm_fp_oam_control_t *)param,
                                                    event_types);
}
#endif /* INCLUDE_MPLS_LM_DM */


#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
/*******************************************************************************/
/* PM routines for BHH & MPLS LM/DM app start                                  */
/*******************************************************************************/


/*******************************************************************************/
/* BHH Embedded App supporting functions Start */
/*******************************************************************************/
#if defined (INCLUDE_BHH)
/*
 * Function:
 *      _bcm_fp_oam_bhh_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to OAM info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_fp_oam_bhh_callback_thread(void *param)
{
    int rv;
    _bcm_fp_oam_control_t *oc = (_bcm_fp_oam_control_t *)param;
    bcm_oam_event_types_t events;
    bcm_oam_event_type_t event_type;
    bhh_msg_event_t event_msg;
    int sess_id;
    uint32 event_mask;
    _bcm_oam_event_handler_t *event_handler_p;
    _bhh_fp_oam_ep_data_t *h_data_p;
    int ep_id = 0;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    uint8 msg_subclass = 0;

    thread_name[0] = 0;
    sal_thread_name(oc->event_thread_id, thread_name, sizeof (thread_name));

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(oc->unit, oc->bhh_uc_num,
                                     MOS_MSG_CLASS_BHH_EVENT, &event_msg,
                                     sal_sem_FOREVER);
        if (BCM_FAILURE(rv)) {
            break;  /*  Thread exit */
        }

        /* Get data from event message */
        sess_id = (int)bcm_ntohs(event_msg.s.len);
        ep_id = BCM_OAM_BHH_GET_SDK_EP(sess_id);

        msg_subclass = event_msg.s.subclass;
        if (msg_subclass == _BHH_EVENT_OAM_PM_EVENT) {
            /* Call PM message handling function and continue */
            _bcm_fp_oam_pm_event_msg_handle(oc, &event_msg);
            continue;
        }
        if (sess_id < 0 ||
            sess_id >= oc->bhh_endpoint_count) {
            LOG_CLI((BSL_META_U(oc->unit,
                                "%s: Invalid sess_id:%d \n"), FUNCTION_NAME(), sess_id));
            continue;
        }

        /* Check endpoint status. */
        rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
        if ((BCM_E_EXISTS != rv)) {
            /* Endpoint not in use. */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META("OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                       oc->unit, ep_id, bcm_errmsg(rv)));
            continue;
        }
        h_data_p = &oc->bhh_ep_data[sess_id];
        event_mask = bcm_ntohl(event_msg.s.data);

        /* Set events */
        sal_memset(&events, 0, sizeof(events));

        if (event_mask & BHH_BTE_EVENT_LB_TIMEOUT) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                "****** OAM BHH LB Timeout ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventBHHLBTimeout] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHLBTimeout);
            }
        }
        if (event_mask & BHH_BTE_EVENT_LB_DISCOVERY_UPDATE) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                               "****** OAM BHH LB Discovery Update ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHLBDiscoveryUpdate] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHLBDiscoveryUpdate);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_TIMEOUT) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                "****** OAM BHH CCM Timeout ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMTimeout] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMTimeout);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_TIMEOUT_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                "****** OAM BHH CCM Timeout Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMTimeoutClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMTimeoutClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_STATE) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                "****** OAM BHH State ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMState] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMState);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_RDI) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM RDI ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMRdi] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMRdi);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                            "****** OAM BHH CCM  Unknown MEG LEVEL ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegLevel] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegLevel);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                               "****** OAM BHH CCM  Unknown MEG ID ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegId] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegId);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                "****** OAM BHH CCM  Unknown MEP ID ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMepId] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMepId);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                "****** OAM BHH CCM  Unknown Period ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPeriod] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPeriod);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                              "****** OAM BHH CCM  Unknown Priority ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPriority] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPriority);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_RDI_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CCM  RDI Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMRdiClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMRdiClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                       "****** OAM BHH CCM  Unknown MEG Level Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegLevelClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegLevelClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                          "****** OAM BHH CCM  Unknown MEG ID Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMegIdClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMegIdClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                          "****** OAM BHH CCM  Unknown MEP ID Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownMepIdClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownMepIdClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                          "****** OAM BHH CCM  Unknown Period Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPeriodClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPeriodClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY_CLEAR) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                        "****** OAM BHH CCM  Unknown Priority Clear ******\n")));
            if (oc->event_handler_cnt[bcmOAMEventBHHCCMUnknownPriorityClear] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBHHCCMUnknownPriorityClear);
            }
        }
        if (event_mask & BHH_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                    (BSL_META_U(oc->unit,
                                "****** OAM PM BHH Counter Rollover ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventBhhPmCounterRollover] > 0) {
                SHR_BITSET(events.w, bcmOAMEventBhhPmCounterRollover);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_LOS) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CSF LOS ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventCsfLos] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfLos);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_FDI) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CSF FDI ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventCsfFdi] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfFdi);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_RDI) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CSF RDI ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventCsfRdi] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfRdi);
            }
        }
        if (event_mask & BHH_BTE_EVENT_CSF_DCI) {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                      (BSL_META_U(oc->unit,
                                  "****** OAM BHH CSF DCI ******\n")));

            if (oc->event_handler_cnt[bcmOAMEventCsfDci] > 0) {
                SHR_BITSET(events.w, bcmOAMEventCsfDci);
            }
        }
        /* Loop over registered callbacks,
         * If any match the events field, then invoke
         */
        for (event_handler_p = oc->event_handler_list_p;
             event_handler_p != NULL;
             event_handler_p = event_handler_p->next_p) {
            for (event_type = bcmOAMEventBHHLBTimeout; event_type < bcmOAMEventCount; ++event_type) {
                if (SHR_BITGET(events.w, event_type)) {
                    if (SHR_BITGET(event_handler_p->event_types.w,
                                   event_type)) {
                        event_handler_p->cb(oc->unit,
                                        0,
                                        event_type,
                                        h_data_p->group_index, /* Group index */
                                        ep_id, /* Endpoint index */
                                        event_handler_p->user_data);
                    }
                }
            }
        }
    }

    oc->bhh_event_thread_id   = NULL;
    LOG_VERBOSE(BSL_LS_BCM_OAM,
              (BSL_META_U(oc->unit,
                          "Thread Exit:%s\n"), thread_name));
    sal_thread_exit(0);
}

STATIC uint8 *
_bcm_fp_oam_ach_header_pack(uint8 *buffer, _ach_header_t *ach)
{
    uint32  tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) | ((ach->version & 0xf) << 24) |
        (ach->reserved << 16) | ach->channel_type;

    _BHH_ENCAP_PACK_U32(buffer, tmp);

    return (buffer);
}

STATIC uint8 *
_bcm_fp_oam_mpls_label_pack(uint8 *buffer, _mpls_label_t *mpls)
{
    uint32  tmp;
    
    tmp = ((mpls->label & 0xfffff) << 12) | ((mpls->exp & 0x7) << 9) |
        ((mpls->s & 0x1) << 8) | mpls->ttl;
    _BHH_ENCAP_PACK_U32(buffer, tmp);

    return (buffer);
}

STATIC uint8 *
_bcm_fp_oam_mpls_label_unpack(uint8 *buffer, _mpls_label_t *mpls)
{
     uint32  tmp;

    _SHR_UNPACK_U32(buffer, tmp);
    mpls->label = (tmp >> 12) & 0xfffff;
    mpls->exp = (tmp >> 9) & 0x7;
    mpls->s = (tmp >> 8) & 0x1;
    mpls->ttl = tmp & 0xffff;

    return (buffer);
}

STATIC uint8 *
_bcm_fp_oam_l2_header_pack(bcm_oam_endpoint_type_t type,
                                uint8 *buffer, _l2_header_t *l2)
{
    uint32  tmp;
    int     i;

    for (i = 0; i < _BHH_MAC_ADDR_LENGTH; i++) {
        _BHH_ENCAP_PACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < _BHH_MAC_ADDR_LENGTH; i++) {
        _BHH_ENCAP_PACK_U8(buffer, l2->src_mac[i]);
    }

    /* Vlan Tag tpid = 0 indicates untagged */
    if (0 != l2->vlan_tag.tpid) {
        tmp =  (l2->vlan_tag.tpid            << 16) |
              ((l2->vlan_tag.tci.prio & 0x7) << 13) |
              ((l2->vlan_tag.tci.cfi  & 0x1) << 12) |
               (l2->vlan_tag.tci.vid  & 0xfff);
        _BHH_ENCAP_PACK_U32(buffer, tmp);
    }
    /*Inner Vlan Tag inner_tpid = 0 indicates single tag  */
    if (0 != l2->vlan_tag_inner.tpid) {
        tmp = (l2->vlan_tag_inner.tpid            << 16) |
             ((l2->vlan_tag_inner.tci.prio & 0x7) << 13) |
             ((l2->vlan_tag_inner.tci.cfi  & 0x1) << 12) |
              (l2->vlan_tag_inner.tci.vid  & 0xfff);
        _BHH_ENCAP_PACK_U32(buffer, tmp);
    }
    _BHH_ENCAP_PACK_U16(buffer, l2->etype);

    return (buffer);
}

/*
 * Function:
 *      bcm_fp_oam_mpls_tp_channel_type_tx_get
 * Purpose:
 *      Get MPLS TP(BHH) ACH Channel type
 * Parameters:
 *      unit                (IN)  BCM device number
 *      channel_type        (IN)  Channel type is ignored in XGS devices
 *      Value               (OUT) User define MPLS TP(BHH) ACH channel type
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */
int
bcm_fp_oam_mpls_tp_channel_type_tx_get(int unit,
                                       bcm_oam_mpls_tp_channel_type_t channel_type,
                                       int *value)
{
    *value = fp_oam_mpls_tp_ach_channel_type;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fp_oam_mpls_tp_channel_type_tx_set
 * Purpose:
 *      Update MPLS TP(BHH) ACH Channel type with user defined value
 * Parameters:
 *      unit                (IN) BCM device number
 *      channel_type        (IN) Channel type is ignored in XGS devices
 *      Value               (IN) User define MPLS TP(BHH) ACH channel type
 *
 * Returns:
 *      BCM_E_NONE          No error
 *      BCM_E_XXXX          Error
 */
int
bcm_fp_oam_mpls_tp_channel_type_tx_set(int unit,
                                       bcm_oam_mpls_tp_channel_type_t channel_type,
                                       int value)
{
    uint8               *buffer, *buffer_ptr;
    uint16              buffer_len, reply_len;
    int                 rv = BCM_E_NONE;
    uint32              *msg;
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    if ((value != SHR_BHH_ACH_CHANNEL_TYPE) &&
        (value != SHR_BHH_ACH_CHANNEL_TYPE_1)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_CONFIG;
    }

    fp_oam_mpls_tp_ach_channel_type = value;
    msg = (uint32 *)&fp_oam_mpls_tp_ach_channel_type;

    /* Pack control message data into DMA buffer */
    buffer     = oc->dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_ach_channel_type_msg_pack(buffer, msg);
    buffer_len = buffer_ptr - buffer;

    rv = _bcm_fp_oam_msg_send_receive(
                                      unit,
                                      MOS_MSG_CLASS_BHH,
                                      MOS_MSG_SUBCLASS_BHH_ACH_CHANNEL_TYPE,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BHH_ACH_CHANNEL_TYPE_REPLY,
                                      &reply_len);

    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}


STATIC int
_bcm_fp_oam_ach_header_get(uint32 packet_flags, _ach_header_t *ach)
{
    sal_memset(ach, 0, sizeof(*ach));
    
    ach->f_nibble = SHR_BHH_ACH_FIRST_NIBBLE;
    ach->version  = SHR_BHH_ACH_VERSION;
    ach->reserved = 0;
    ach->channel_type = fp_oam_mpls_tp_ach_channel_type;

    return (BCM_E_NONE);
}
    
STATIC int
_bcm_fp_oam_mpls_label_get(uint32 label, uint8 exp, uint8 s, uint8 ttl,
                           _mpls_label_t *mpls)
{
    sal_memset(mpls, 0, sizeof(*mpls));
        
    mpls->label = label;
    mpls->exp   = exp & 0x07;
    mpls->s     = s;
    if(ttl){
        mpls->ttl   = ttl;
    } else {
        mpls->ttl   = _BHH_MPLS_DFLT_TTL;
    }
 
    return (BCM_E_NONE);
}

STATIC int
_bcm_fp_oam_mpls_gal_label_get(_mpls_label_t *mpls, uint8 oam_exp)
{
    return _bcm_fp_oam_mpls_label_get(SHR_BHH_MPLS_GAL_LABEL,
                                      oam_exp, 1, 1, mpls);
}

STATIC int
_bcm_fp_oam_mpls_labels_get(int unit, _bhh_fp_oam_ep_data_t *h_data_p,
                                uint32 packet_flags,
                                int max_count, 
                                _mpls_label_t *pw_label,
                                _mpls_label_t *mpls,
                                int *mpls_count, bcm_if_t *l3_intf_id, uint8 oam_exp,
                                uint8 oam_ttl)
{
    int count = 0;
    bcm_l3_egress_t l3_egress;
    bcm_mpls_port_t mpls_port;
    bcm_mpls_egress_label_t label_array[_BHH_FP_OAM_MPLS_MAX_LABELS];
    bcm_mpls_egress_label_t tmp_label;
    int label_count;
    int i = 0;

     /* Get L3 objects */
    bcm_l3_egress_t_init(&l3_egress);
    if (BCM_FAILURE(bcm_esw_l3_egress_get(unit, h_data_p->egress_if, &l3_egress))) {
        return (BCM_E_PARAM);
    }

    /* Look for a tunnel associated with this interface */
    if (BCM_SUCCESS
        (bcm_esw_mpls_tunnel_initiator_get(unit, l3_egress.intf,
                                           _BHH_FP_OAM_MPLS_MAX_LABELS,
                                           label_array, &label_count))) {
        /* We need to sawp the labels configured in tunnel initiator as
           it returns labels in reverse order than how it should be in pkt */
        for (i=0; i<(label_count/2); i++) {
            sal_memcpy(&tmp_label, &label_array[i], sizeof(tmp_label));
            sal_memcpy(&label_array[i], &label_array[label_count - 1 - i],
                       sizeof(tmp_label));
            sal_memcpy(&label_array[label_count - 1 - i], &tmp_label,
                       sizeof(tmp_label));
        }

        /* Add VRL Label */
        if (BHH_EP_LSP_TYPE(h_data_p)) {
            if ( (l3_egress.mpls_label != BCM_MPLS_LABEL_INVALID) &&
                 (label_count < _BHH_FP_OAM_MPLS_MAX_LABELS) ) {
                label_array[label_count].label = l3_egress.mpls_label;
                label_array[label_count].exp = l3_egress.mpls_exp;
                label_array[label_count].ttl = l3_egress.mpls_ttl;
                label_count++;
            }
        }
        /* Traverse through Label stack and match on control label, this
           label should be the one just above GAL in the label stack, any
           following labels should not be added */
        for (i=0;  i < label_count; i++) {
            if (label_array[i].label == h_data_p->egr_label) {
                /* Set the intended msg type EXP for control Label */
                label_array[i].exp = oam_exp;
                if (soc_property_get(unit, spn_MPLS_OAM_EGRESS_LABEL_TTL, 0) &&
                    oam_ttl) {
                    label_array[i].ttl = oam_ttl;
                }
                label_count = i;
                _bcm_fp_oam_mpls_label_get(label_array[i].label,
                        label_array[i].exp,
                        0,
                        label_array[i].ttl,
                        &mpls[count++]);
                break;
            }
            _bcm_fp_oam_mpls_label_get(label_array[i].label,
                    label_array[i].exp,
                    0,
                    label_array[i].ttl,
                    &mpls[count++]);
        }
    }

        /* MPLS Router Alert */
    if (packet_flags & _BHH_ENCAP_PKT_MPLS_ROUTER_ALERT) {
        /* Ignore overrun error as RAL is only added for PW MEPs and so overrun
         * condition will not exist.
         */
        /* coverity[overrun-local] */
        mpls[count].label = SHR_BHH_MPLS_ROUTER_ALERT_LABEL;
        mpls[count].exp = 0;
        mpls[count].s = 0;
        mpls[count].ttl = _BHH_MPLS_DFLT_TTL;
        count++;
    }

    /* Use GPORT to resolve interface */
    if (BCM_GPORT_IS_MPLS_PORT(h_data_p->gport)) {
        /* Get mpls port and label info */
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.mpls_port_id = h_data_p->gport;
        if (BCM_FAILURE(bcm_esw_mpls_port_get(unit, h_data_p->vpn,
                                              &mpls_port))) {
            return (BCM_E_PARAM);
        }
        if (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv &&
            h_data_p->vccv_type == bcmOamBhhVccvTtl) {
            mpls_port.egress_label.ttl = 0x1;
        }
        if (h_data_p->egr_label == mpls_port.egress_label.label) {
            mpls_port.egress_label.exp = oam_exp;
            if (soc_property_get(unit, spn_MPLS_OAM_EGRESS_LABEL_TTL, 0) &&
                oam_ttl) {
                mpls_port.egress_label.ttl = oam_ttl;
            }
        }

        _bcm_fp_oam_mpls_label_get(mpls_port.egress_label.label,
                                   mpls_port.egress_label.exp,
                                   0,
                                   mpls_port.egress_label.ttl,
                                   &mpls[count++]);
    }


    /* Set Bottom of Stack if there is no GAL label */
    if (!(packet_flags & _BHH_ENCAP_PKT_GAL)) {
        mpls[count-1].s = 1;
    }

    *mpls_count = count;

    return (BCM_E_NONE);

}

STATIC int
_bcm_fp_oam_l2_header_get(int unit, _bhh_fp_oam_ep_data_t *h_data_p,
                                bcm_port_t port, uint16 etype,
                                _l2_header_t *l2, uint8 *lm_pdu_counter_offset)
{
    uint16 tpid;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
    bcm_vlan_control_vlan_t vc;
    int tpid_select;
    bcm_pbmp_t pbmp, ubmp;

    sal_memset(l2, 0, sizeof(*l2));

    if (!BHH_EP_MPLS_SECTION_TYPE(h_data_p)) {
        /* Get L3 interfaces */
        bcm_l3_egress_t_init(&l3_egress);
        bcm_l3_intf_t_init(&l3_intf);

        if (BCM_FAILURE
                (bcm_esw_l3_egress_get(unit, h_data_p->egress_if, &l3_egress))) {
            return (BCM_E_PARAM);
        }

        l3_intf.l3a_intf_id = l3_egress.intf;
        if (BCM_FAILURE(bcm_esw_l3_intf_get(unit, &l3_intf))) {
            return (BCM_E_PARAM);
        }

        /* Get TPID */
        BCM_IF_ERROR_RETURN(
                       bcm_esw_vlan_control_port_get(unit,
                                                     port,
                                                     bcmVlanPortOuterTpidSelect,
                                                     &tpid_select));
        if (tpid_select == BCM_PORT_OUTER_TPID) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_tpid_get(unit, port, &tpid));
        } else {
            BCM_IF_ERROR_RETURN(
                     bcm_esw_vlan_control_vlan_get(unit, l3_intf.l3a_vid, &vc));
            tpid = vc.outer_tpid;
        }

        sal_memcpy(l2->dst_mac, l3_egress.mac_addr, _BHH_MAC_ADDR_LENGTH);
        sal_memcpy(l2->src_mac, l3_intf.l3a_mac_addr, _BHH_MAC_ADDR_LENGTH);
        *lm_pdu_counter_offset += 2*_BHH_MAC_ADDR_LENGTH;
        l2->vlan_tag.tpid     = tpid;
        l2->vlan_tag.tci.prio = h_data_p->vlan_pri;
        l2->vlan_tag.tci.cfi  = 0;
        l2->vlan_tag.tci.vid  = l3_intf.l3a_vid;
        *lm_pdu_counter_offset += 4;

        BCM_IF_ERROR_RETURN(bcm_esw_vlan_port_get(unit,
                                                  l2->vlan_tag.tci.vid,
                                                  &pbmp,
                                                  &ubmp));
        if (BCM_PBMP_MEMBER(ubmp, port)) {
            l2->vlan_tag.tpid = 0;  /* Set to 0 to indicate untagged */
        }

        if (l3_intf.l3a_inner_vlan != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_inner_tpid_get(unit, port, &tpid));
            l2->vlan_tag_inner.tpid     = tpid;
            l2->vlan_tag_inner.tci.prio = h_data_p->inner_vlan_pri;
            l2->vlan_tag_inner.tci.cfi  = 0;
            l2->vlan_tag_inner.tci.vid  = l3_intf.l3a_inner_vlan;
            *lm_pdu_counter_offset += 4;
        }
    }else {
        sal_memcpy(l2->dst_mac, h_data_p->dst_mac_address, _BHH_MAC_ADDR_LENGTH);
        sal_memcpy(l2->src_mac, h_data_p->src_mac_address, _BHH_MAC_ADDR_LENGTH);
        *lm_pdu_counter_offset += 2*_BHH_MAC_ADDR_LENGTH;
        if((h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterVlan) ||
           (h_data_p->type == bcmOAMEndpointTypeBhhSectionInnervlan) ||
           (h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)){
            if (h_data_p->vlan != 0) {
                l2->vlan_tag.tpid     = h_data_p->outer_tpid;
                l2->vlan_tag.tci.prio = h_data_p->vlan_pri;
                l2->vlan_tag.tci.cfi  = 0;
                l2->vlan_tag.tci.vid  = h_data_p->vlan;
                *lm_pdu_counter_offset += 4;
            }
        }
        if((h_data_p->type == bcmOAMEndpointTypeBhhSectionInnervlan) ||
           (h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)){
            l2->vlan_tag_inner.tpid     = h_data_p->inner_tpid;
            l2->vlan_tag_inner.tci.prio = h_data_p->inner_vlan_pri;
            l2->vlan_tag_inner.tci.cfi  = 0;
            l2->vlan_tag_inner.tci.vid  = h_data_p->inner_vlan;
            *lm_pdu_counter_offset += 4;
        }
        etype = SHR_BHH_L2_ETYPE_MPLS_UCAST;
    }
    l2->etype             = etype;
    *lm_pdu_counter_offset += 2;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_fp_oam_encap_data_dump
 * Purpose:
 *      Dumps buffer contents.
 * Parameters:
 *      buffer  - (IN) Buffer to dump data.
 *      length  - (IN) Length of buffer.
 * Returns:
 *      None
 */
void
_bcm_fp_oam_encap_data_dump(uint8 *buffer, int length)
{   
    int i;
    
    LOG_CLI((BSL_META("\nBHH encapsulation (length=%d):\n"), length));
    
    for (i = 0; i < length; i++) { 
        if ((i % 16) == 0) {
            LOG_CLI((BSL_META("\n")));
        }
        LOG_CLI((BSL_META(" %02x"), buffer[i]));
    }
    
    LOG_CLI((BSL_META("\n")));
    return;
}   

/*
 * Function:
 *      _bcm_fp_oam_olp_tx_header_create
 * Purpose:
 *      Adds OLP Tx header packet to encapsulation.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      mod             - (IN) module Id.
 *      port_id         - (IN) port Id.
 *      ts_mode         - (IN) Timestamp type for DM PDU.
 *      lm_pdu_counter_offset - (IN) Counter offset in LM PDU.
 *      encap_data      - (OUT) Buffer returning MPLS OAM encapsulation.
 *      encap_length    - (OUT) Buffer length which contains 
 *                              MPLS OAM encapsulation length.
 *      int_pri         - (IN) INT_PRI of OAM message.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning MPLS OAM encapsulation buffer includes 
 *      OLP TX header.
 */

STATIC int
_bcm_fp_oam_olp_tx_header_create(int unit,
                                 bcm_module_t mod, bcm_port_t port_id,
                                 uint8 counter_size,
                                 uint32  *counter_base_id,
                                 uint32  *counter_offset,
                                 int     *counter_action,
                                 uint32  *counter_byte_offset,
                                 uint8   ts_mode,
                                 uint8 lm_pdu_counter_offset,
                                 uint8  *encap_data,
                                 uint16 *encap_length,
                                 bcm_cos_t int_pri)
{
    int rv = 0, i;
    soc_olp_l2_hdr_t olp_l2_header;
    soc_olp_tx_hdr_t olp_oam_header;
    uint8 *buffer_p = encap_data;
    bcm_stat_group_mode_t stat_group;
    bcm_stat_object_t stat_object;
    uint32 stat_mode;
    uint32 stat_pool_number;
    uint32 base_index;
    bcm_module_t my_modid = 0;
    bcm_gport_t  cpu_dglp;


    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID, my_modid,
                                               CMIC_PORT(unit), &cpu_dglp));
    /* Get OLP L2 Header for CPU port */
    sal_memset(&olp_l2_header, 0, sizeof(olp_l2_header));
    rv = _bcm_olp_l2_hdr_get(unit, cpu_dglp, 0, &olp_l2_header);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                        "OLP Header get failed.\n")));
        return (rv);
    }
    buffer_p = _bcm_fp_oam_olp_l2_header_pack(buffer_p, &olp_l2_header);

    sal_memset(&olp_oam_header, 0, sizeof(olp_oam_header));
    /* Set Port number */
    SOC_OLP_TX_PORT(&olp_oam_header) = port_id;
    SOC_OLP_TX_MODID(&olp_oam_header) = mod;
    SOC_OLP_TX_PRI(&olp_oam_header) = int_pri;

    for (i=0; i< counter_size; i++) {
 
        _bcm_esw_stat_get_counter_id_info(unit, counter_base_id[i],
                                          &stat_group, &stat_object,
                                          &stat_mode, &stat_pool_number,
                                          &base_index);
        if (i == 0) {
            SOC_OLP_TX_CTR1_ID(&olp_oam_header) =
                SOC_OLP_TX_CTR(stat_pool_number, 
                                (base_index + counter_offset[i]));
            SOC_OLP_TX_CTR1_ACTION(&olp_oam_header) =
                                       counter_action[i];
            /* Counter location is Egress */
            SOC_OLP_TX_CTR1_LOCATION(&olp_oam_header) = 1;
        }
        if (i == 1) {
            SOC_OLP_TX_CTR2_ID_SET(&olp_oam_header,
                SOC_OLP_TX_CTR(stat_pool_number, 
                                (base_index + counter_offset[i])));
            SOC_OLP_TX_CTR2_ACTION(&olp_oam_header) =
                                       counter_action[i];
            SOC_OLP_TX_CTR2_LOCATION(&olp_oam_header) = 1;
        }
        if (i == 2) {
            SOC_OLP_TX_CTR3_ID_SET(&olp_oam_header,
                SOC_OLP_TX_CTR(stat_pool_number, 
                                (base_index + counter_offset[i])));
            SOC_OLP_TX_CTR3_ACTION(&olp_oam_header) =
                                       counter_action[i];
            SOC_OLP_TX_CTR3_LOCATION(&olp_oam_header) = 1;
        }
    }
    if(lm_pdu_counter_offset){
        /* COUNTER OFFSET in OLP Header =
         * (L2 Encap + Counter field offset in MPLS OAM Header - 14)/2 
         */
        SOC_OLP_TX_OAM_OFFSET(&olp_oam_header) =
            (lm_pdu_counter_offset - 14) >> 1;
    }

    SOC_OLP_TX_TIMESTAMP_ACTION(&olp_oam_header) = ts_mode;

    /* Pack OLP OAM Encap */
    buffer_p = shr_olp_tx_header_pack(buffer_p, &olp_oam_header);
    *encap_length += (buffer_p - encap_data);

#ifdef _BHH_DEBUG_DUMP
    _bcm_fp_oam_encap_data_dump(encap_data, *encap_length);
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_fp_oam_encap_build_pack
 * Purpose:
 *      Builds and packs the MPLS OAM packet encapsulation for a given
 *      tunnel type.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      mod             - (IN) Module.
 *      port            - (IN) Port.
 *      endpoint_config - (IN/OUT) Pointer to endpoint structure.
 *      packet_flags    - (IN) Flags for building packet.
 *      buffer          - (OUT) Buffer returning encapsulation.
 *      counter_info    - (IN/OUT) Pointer to LM counter info.
 *      int_pri         - (IN) INT_PRI of OAM message.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BHH encapsulation includes only all the
 *      encapsulation headers/labels and does not include
 *      the BHH control packet.
 */
STATIC int
_bcm_fp_oam_encap_build_pack(int unit, bcm_module_t mod, bcm_port_t port,
                             _bhh_fp_oam_ep_data_t *h_data_p,
                             uint32 packet_flags,
                             uint8 *buffer,
                             uint16 *encap_length, 
                             _bhh_oam_lm_dm_info_t *counter_info,
                             bcm_cos_t  int_pri, uint8 oam_exp,
                             uint8 oam_ttl)
{
    uint8          *cur_ptr = buffer;
    uint16         etype = 0;
    bcm_if_t       l3_intf_id = -1;
    _ach_header_t  ach;
    _mpls_label_t  mpls_gal;
    _mpls_label_t  mpls_labels[_BHH_MPLS_MAX_LABELS];
    _mpls_label_t  pw_label;
    int            mpls_count = 0;
    _l2_header_t   l2;
    int i;

    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if (packet_flags & _BHH_ENCAP_PKT_G_ACH) {
        BCM_IF_ERROR_RETURN
            (_bcm_fp_oam_ach_header_get(packet_flags, &ach));
        counter_info->oam_replacement_offset += 4;
    }

    if (packet_flags & _BHH_ENCAP_PKT_GAL) {
        if (h_data_p->egr_label == SHR_BHH_MPLS_GAL_LABEL) {
            BCM_IF_ERROR_RETURN
                (_bcm_fp_oam_mpls_gal_label_get(&mpls_gal,
                                                h_data_p->egr_label_exp));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_fp_oam_mpls_gal_label_get(&mpls_gal, 0));
        }
        counter_info->oam_replacement_offset += 4;
    }

    if (packet_flags & _BHH_ENCAP_PKT_MPLS) {
        etype = SHR_BHH_L2_ETYPE_MPLS_UCAST;
        BCM_IF_ERROR_RETURN
            (_bcm_fp_oam_mpls_labels_get(unit, h_data_p,
                                             packet_flags,    
                                             _BHH_FP_OAM_MPLS_MAX_LABELS,
                                             &pw_label,
                                             mpls_labels, 
                                             &mpls_count,
                                             &l3_intf_id,
                                             oam_exp, oam_ttl));
        counter_info->oam_replacement_offset += (mpls_count *4);
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_fp_oam_l2_header_get(unit, 
                                       h_data_p,
                                       port, 
                                       etype, 
                                       &l2,
                                       &counter_info->oam_replacement_offset));
    _bcm_fp_oam_olp_tx_header_create(unit, mod, port,
                                     counter_info->counter_size,
                                     counter_info->counter_base_id,
                                     counter_info->counter_offset,
                                     counter_info->counter_action,
                                     counter_info->byte_count_offset,
                                     counter_info->ts_mode, 
                                     counter_info->oam_replacement_offset,
                                     buffer,
                                     encap_length, int_pri);
    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer + *encap_length;

    /* L2 Header is always present */
    cur_ptr = _bcm_fp_oam_l2_header_pack(h_data_p->type, cur_ptr, &l2);

    if (packet_flags & _BHH_ENCAP_PKT_MPLS) {
        for (i = 0;i < mpls_count;i++) {
            cur_ptr = _bcm_fp_oam_mpls_label_pack(cur_ptr, &mpls_labels[i]);
        }
    }

    if (packet_flags & _BHH_ENCAP_PKT_GAL) {    
        cur_ptr = _bcm_fp_oam_mpls_label_pack(cur_ptr, &mpls_gal);
    }

    if (packet_flags & _BHH_ENCAP_PKT_G_ACH) {
        cur_ptr = _bcm_fp_oam_ach_header_pack(cur_ptr, &ach);
    }

#ifdef _BHH_DEBUG_DUMP
    _bcm_fp_oam_encap_data_dump(buffer, *encap_length);
#endif

    /* Set BHH encapsulation length */
    *encap_length = cur_ptr - buffer;

    return (BCM_E_NONE);
}

void
_bcm_fp_oam_bhh_hash_data_dump(int unit, _bhh_fp_oam_ep_data_t *h_data_p)
{
    int i = 0;
    LOG_CLI((BSL_META_U(unit,"Type = %d\r\n"),h_data_p->type));
    LOG_CLI((BSL_META_U(unit,"EP ID = %d\r\n"),h_data_p->ep_id));
    LOG_CLI((BSL_META_U(unit,"Remote = %d\r\n"),h_data_p->is_remote));
    LOG_CLI((BSL_META_U(unit,"Name = %d\r\n"),h_data_p->name));
    LOG_CLI((BSL_META_U(unit,"Level = %d\r\n"),h_data_p->level));
    LOG_CLI((BSL_META_U(unit,"Outer Vlan = %d\r\n"),h_data_p->vlan));
    LOG_CLI((BSL_META_U(unit,"Inner Vlan = %d\r\n"),h_data_p->inner_vlan));
    LOG_CLI((BSL_META_U(unit,"Gport = %d\r\n"),h_data_p->gport));
    LOG_CLI((BSL_META_U(unit,"SGLP = %d\r\n"),h_data_p->sglp));
    LOG_CLI((BSL_META_U(unit,"DGLP = %d\r\n"),h_data_p->dglp));
    LOG_CLI((BSL_META_U(unit,"Grp idx = %d\r\n"),h_data_p->group_index));
    LOG_CLI((BSL_META_U(unit,"VP = %d\r\n"),h_data_p->vp));
    LOG_CLI((BSL_META_U(unit,"Flags = 0x%x\r\n"),h_data_p->flags));
    LOG_CLI((BSL_META_U(unit,"Flags2 = 0x%x\r\n"),h_data_p->flags2));
    LOG_CLI((BSL_META_U(unit,"Period = %d\r\n"),h_data_p->period));
    LOG_CLI((BSL_META_U(unit,"Label = 0x%x\r\n"),h_data_p->label));
    LOG_CLI((BSL_META_U(unit,"Vlan Pri = %d\r\n"),h_data_p->vlan_pri));
    LOG_CLI((BSL_META_U(unit,"Egress if = 0x%x\r\n"),h_data_p->egress_if));
    LOG_CLI((BSL_META_U(unit,"Outer TPID = 0x%04X\r\n"),h_data_p->outer_tpid));
    LOG_CLI((BSL_META_U(unit,"Inner TPID = 0x%04X\r\n"),h_data_p->inner_tpid));
    LOG_CLI((BSL_META_U(unit,"Trunk ID = %d\r\n"),h_data_p->trunk_id));
    LOG_CLI((BSL_META_U(unit,"Trunk Index = %d\r\n"),h_data_p->trunk_index));
    LOG_CLI((BSL_META_U(unit,"Src Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             h_data_p->src_mac_address[0], h_data_p->src_mac_address[1],
             h_data_p->src_mac_address[2], h_data_p->src_mac_address[3],
             h_data_p->src_mac_address[4], h_data_p->src_mac_address[5]));
    LOG_CLI((BSL_META_U(unit,"Dst Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             h_data_p->dst_mac_address[0], h_data_p->dst_mac_address[1],
             h_data_p->dst_mac_address[2], h_data_p->dst_mac_address[3],
             h_data_p->dst_mac_address[4], h_data_p->dst_mac_address[5]));
    LOG_CLI((BSL_META_U(unit,"Inner vlan pri = %d\r\n"),
                        h_data_p->inner_vlan_pri));
    LOG_CLI((BSL_META_U(unit,"Vccv Type = %d\r\n"),h_data_p->vccv_type));
    LOG_CLI((BSL_META_U(unit,"VPN = %d\r\n"),h_data_p->vpn));
    LOG_CLI((BSL_META_U(unit,"PM profile attached = %d\r\n"),
                        h_data_p->pm_profile_attached));
    LOG_CLI((BSL_META_U(unit,"BHH Session Id = %u\r\n"),
                        h_data_p->uc_session_id));
    LOG_CLI((BSL_META_U(unit,"BHH CCM counter info :\n")));
    LOG_CLI((BSL_META_U(unit,"BHH CCM counter size = %u\r\n"),
                        h_data_p->ccm_tx_counter_size));
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        LOG_CLI((BSL_META_U(unit,"BHH counter %d base Id = %u\r\n"),
                    i, h_data_p->ccm_tx_update_lm_counter_base_id[i]));
        LOG_CLI((BSL_META_U(unit,"BHH counter %d offset = %u\r\n"),
                    i, h_data_p->ccm_tx_update_lm_counter_offset[i]));
    }
    LOG_CLI((BSL_META_U(unit,"BHH LM counter info :\n")));
    LOG_CLI((BSL_META_U(unit,"BHH LM counter size = %u\r\n"),
                        h_data_p->lm_counter_size));
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        LOG_CLI((BSL_META_U(unit,"BHH counter %d base Id = %u\r\n"),
                    i, h_data_p->lm_counter_base_id[i]));
        LOG_CLI((BSL_META_U(unit,"BHH counter %d offset = %u\r\n"),
                    i, h_data_p->lm_counter_offset[i]));
        LOG_CLI((BSL_META_U(unit,"BHH counter %d action = %u\r\n"),
                    i, h_data_p->lm_counter_action[i]));
    }
    LOG_CLI((BSL_META_U(unit,"BHH DM counter info :\n")));
    LOG_CLI((BSL_META_U(unit,"BHH DM counter size = %u\r\n"),
                        h_data_p->dm_tx_counter_size));
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        LOG_CLI((BSL_META_U(unit,"BHH counter %d base Id = %u\r\n"),
                    i, h_data_p->dm_tx_update_lm_counter_base_id[i]));
        LOG_CLI((BSL_META_U(unit,"BHH counter %d offset = %u\r\n"),
                    i, h_data_p->dm_tx_update_lm_counter_offset[i]));
    }
    LOG_CLI((BSL_META_U(unit,"BHH LB counter info :\n")));
    LOG_CLI((BSL_META_U(unit,"BHH LB counter size = %u\r\n"),
                        h_data_p->lb_tx_counter_size));
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        LOG_CLI((BSL_META_U(unit,"BHH counter %d base Id = %u\r\n"),
                    i, h_data_p->lb_tx_update_lm_counter_base_id[i]));
        LOG_CLI((BSL_META_U(unit,"BHH counter %d offset = %u\r\n"),
                    i, h_data_p->lb_tx_update_lm_counter_offset[i]));
    }
    LOG_CLI((BSL_META_U(unit,"HW session ID = %d\r\n"),
                        h_data_p->hw_session_id));
    LOG_CLI((BSL_META_U(unit,"HW no of session entries = %d\r\n"),
                        h_data_p->hw_session_num_entries));
    LOG_CLI((BSL_META_U(unit,"Resolved trunk gport based on trunk " 
                        "index 0x%x\r\n"),
                        h_data_p->resolved_trunk_gport));
}

/*
 * Function:
 *      _bcm_fp_oam_bhh_encap_create
 * Purpose:
 *      Creates a BHH packet encapsulation.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      mod             - (IN) Module.
 *      port_id         - (IN) Port.
 *      endpoint_config - (IN/OUT) Pointer to endpoint structure.
 *      encap_data      - (OUT) Buffer returning encapsulation.
 *      counter_info    - (IN/OUT) Pointer to LM counter info.
 *      int_pri         - (IN) INT_PRI of OAM message.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BHH encapsulation buffer includes all the
 *      corresponding headers/labels EXCEPT for the BHH control packet.
 */
STATIC int
_bcm_fp_oam_bhh_encap_create(int unit, bcm_module_t mod, 
                         bcm_port_t port_id,
                         _bhh_fp_oam_ep_data_t *h_data_p,
                         uint8 *encap_data,
                         uint8  *encap_type,
                         uint16 *encap_length,
                         _bhh_oam_lm_dm_info_t *counter_info,
                         bcm_cos_t int_pri, uint8 oam_exp,
                         uint8 oam_ttl)
{
    uint32 packet_flags;
    _bcm_fp_oam_control_t *oc;

    packet_flags = 0;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /*
     * Get BHH encapsulation packet format flags

     * Also, perform the following for each BHH tunnel type:
     * - Check for valid parameter values
     * - Set specific values required by the BHH tunnel definition 
     *   (e.g. such as ttl=1,...)
     */
    switch (h_data_p->type) {
    case bcmOAMEndpointTypeBHHMPLS:
        packet_flags |=
            (_BHH_ENCAP_PKT_MPLS |
             _BHH_ENCAP_PKT_GAL |
             _BHH_ENCAP_PKT_G_ACH);
        break;

    case bcmOAMEndpointTypeBHHMPLSVccv:
        switch(h_data_p->vccv_type) {

        case bcmOamBhhVccvChannelAch:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_G_ACH);
        break;

        case bcmOamBhhVccvRouterAlert:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_MPLS_ROUTER_ALERT |
                 _BHH_ENCAP_PKT_G_ACH); 
            break;

        case bcmOamBhhVccvTtl:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_G_ACH); 
            break;

    case bcmOamBhhVccvGal13:
            packet_flags |=
                (_BHH_ENCAP_PKT_MPLS |
                 _BHH_ENCAP_PKT_PW |
                 _BHH_ENCAP_PKT_GAL  |
                 _BHH_ENCAP_PKT_G_ACH);
            break;

    default:
        return (BCM_E_PARAM);
        break;
    }
        break;

    case bcmOAMEndpointTypeBhhSection:
    case bcmOAMEndpointTypeBhhSectionInnervlan:
    case bcmOAMEndpointTypeBhhSectionOuterVlan:
    case bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan:
        packet_flags |=
            (_BHH_ENCAP_PKT_GAL |
             _BHH_ENCAP_PKT_G_ACH);
        break;
    default:
        return (BCM_E_PARAM);
    }

    /* Build header/labels and pack in buffer */
    BCM_IF_ERROR_RETURN
        (_bcm_fp_oam_encap_build_pack(unit, mod, port_id,
                                      h_data_p,
                                      packet_flags,
                                      encap_data,
                                      encap_length, counter_info, int_pri,
                                      oam_exp, oam_ttl));

    /* Set encap type (indicates uC side that checksum is required) */
    *encap_type = SHR_BHH_ENCAP_TYPE_RAW;

#ifdef _BHH_DEBUG_DUMP
    _bcm_fp_oam_encap_data_dump(encap_data, *encap_length);
#endif

    if (*encap_length > oc->bhh_max_encap_length) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                "OAM(unit %d) Error: Encap length greater than max,"
                "encap_length=%u max=%u\n"),
                 unit, *encap_length, oc->bhh_max_encap_length));
        return BCM_E_CONFIG;
    }

    return (BCM_E_NONE);
}

/*  
 * Function:
 *     _bhh_fp_oam_ep_hash_key_construct
 * Purpose:
 *     Construct hash table key for a given endpoint information.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     oc      - (IN) Pointer to OAM control structure.
 *     ep_info - (IN) Pointer to endpoint information structure.
 *     key     - (IN/OUT) Pointer to hash key buffer.
 * Retruns:
 *     None
 */ 
STATIC void
_bhh_fp_oam_ep_hash_key_construct(int unit,
                               _bcm_fp_oam_control_t *oc,
                               bcm_oam_endpoint_info_t *ep_info,
                               _bhh_fp_oam_hash_key_t *key)
{
    uint8  *loc = *key;

    sal_memset(key, 0, sizeof(_bhh_fp_oam_hash_key_t));

    if (NULL != ep_info) {
        _BCM_OAM_KEY_PACK(loc, &ep_info->group, sizeof(ep_info->group));
    
        _BCM_OAM_KEY_PACK(loc, &ep_info->name, sizeof(ep_info->name));

        _BCM_OAM_KEY_PACK(loc, &ep_info->gport, sizeof(ep_info->gport));

        _BCM_OAM_KEY_PACK(loc, &ep_info->level, sizeof(ep_info->level));
    
        _BCM_OAM_KEY_PACK(loc, &ep_info->vlan, sizeof(ep_info->vlan));
    
        _BCM_OAM_KEY_PACK(loc, &ep_info->inner_vlan, 
                                          sizeof(ep_info->inner_vlan));
        _BCM_OAM_KEY_PACK(loc, &ep_info->mpls_label, 
                sizeof(ep_info->mpls_label));
    }

    /* End address should not exceed size of _bcm_oam_hash_key_t. */
    assert ((int) (loc - *key) <= sizeof(_bhh_fp_oam_hash_key_t));
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *       _bcm_fp_oam_bhh_scache_alloc
 * Purpose:
 *     Allocate scache memory for BHH module
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - OAM not support on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     BCM_E_NONE    - Success
 */
STATIC int
_bcm_fp_oam_bhh_scache_alloc(int unit) {
    _bcm_fp_oam_control_t *oc;
    soc_scache_handle_t scache_handle;
    uint8 *oam_scache;
    int alloc_sz = 0;


    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Number of BHH groups */
    alloc_sz += sizeof(int);

    alloc_sz += (sizeof(int) + sizeof(int) + sizeof(uint32) + 
                 BCM_OAM_GROUP_NAME_LENGTH) * 
                oc->bhh_endpoint_count;
 
    /* Number of BHH endpoints */
    alloc_sz += sizeof(int);

    alloc_sz += (sizeof(bcm_oam_endpoint_t) +
                sizeof(bcm_oam_endpoint_type_t) + sizeof(bcm_oam_group_t) +
                sizeof(bcm_gport_t) + sizeof(bcm_mpls_label_t) +
                sizeof(uint8) +
                sizeof(bcm_vpn_t) +
                sizeof(bcm_if_t) +
                sizeof(bcm_oam_pm_profile_t) +
                sizeof(uint8) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                sizeof(uint8) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                sizeof(uint8) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                sizeof(uint8) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX)) *
                oc->bhh_endpoint_count + 
                sizeof(uint32) +
                sizeof(uint8) +
                sizeof(int) +
                /* CSF counters */
                (sizeof(uint8) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) +
                (sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX) *
                 oc->bhh_endpoint_count);

    /* Allocate memory to store oam_mpls_tp_ach_channel_type */
    alloc_sz += sizeof(fp_oam_mpls_tp_ach_channel_type);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                          bcmFpOamWbSequenceBhh);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 1,
                        alloc_sz, &oam_scache, BCM_WB_BHH_DEFAULT_VERSION, NULL));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_sync
 * Purpose:
 *     Store BHH configuration to level two storage cache memory.
 * Parameters:
 *     unit - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_sync(int unit)
{
    int                 rv;  /* Operation return status. */
    _bcm_fp_oam_control_t  *oc;
    int                 alloc_size = 0, csf_cntr_size;
    int                 stable_size;
    soc_scache_handle_t scache_handle;
    uint8               *oam_scache;
    int                 grp_idx;
    _bhh_fp_oam_group_data_t *group_p;   /* Pointer to group list.         */
    int                 group_count = 0, ep_count = 0;
    int         idx = 0;
    _bhh_fp_oam_ep_data_t *ep_data;
    uint8               i = 0;

    /* Get OAM module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    /* Get handle to control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready == 0) {
        /* Initialize to group array pointer. */
        group_p = oc->bhh_group_data;

        for (grp_idx = 0; grp_idx < oc->bhh_endpoint_count; grp_idx++) {

            /* Check if the group is in use. */
            if (BCM_E_EXISTS
                    == shr_idxres_list_elem_state(oc->bhh_group_pool, grp_idx)) {
                /*Store Group ID of this group*/
                alloc_size += sizeof(int);
                /*Store LMEP ID associated with this group*/
                alloc_size += sizeof(int);
                /*Store flags associated with this group*/
                alloc_size += sizeof(uint32);
                /*Store name associated with this group*/
                alloc_size += BCM_OAM_GROUP_NAME_LENGTH;
                group_count++;
            }
        }
        /* To store OAM group count. */
        alloc_size += sizeof(int);

        ep_data = oc->bhh_ep_data;
        for (idx = 0; idx < oc->bhh_endpoint_count; idx++) {

            /* Check if the EP is in use. */
            if (BCM_E_EXISTS
                    == shr_idxres_list_elem_state(oc->bhh_pool, idx)) {
                /*Store ID of this EP*/
                alloc_size += sizeof(bcm_oam_endpoint_t);
                /*Store type of this EP*/
                alloc_size += sizeof(bcm_oam_endpoint_type_t);
                /*Store group ID associated with this EP*/
                alloc_size += sizeof(bcm_oam_group_t);
                /*Store gport associated with this EP*/
                alloc_size += sizeof(bcm_gport_t);
                /*Store egr label associated with this EP*/
                alloc_size += sizeof(bcm_mpls_label_t);
                /*Store egr label exp associated with this EP*/
                alloc_size += sizeof(uint8);
                /*Store vpn associated with this EP*/
                alloc_size += sizeof(bcm_vpn_t);
                /*Store intf associated with this EP*/
                alloc_size += sizeof(bcm_if_t);
                /*Store PM profile associated with this EP*/
                alloc_size += sizeof(bcm_oam_pm_profile_t);
                /*Store CCM TX counter info associated with this EP*/
                /*Store CCM TX counter size*/
                alloc_size += sizeof(uint8);
                /*Store CCM TX counters base_ids*/
                alloc_size += sizeof(uint32) * ep_data[idx].ccm_tx_counter_size;
                /*Store CCM TX counters offsets*/
                alloc_size += sizeof(uint32) * ep_data[idx].ccm_tx_counter_size;
                /*Store LM TX counter info associated with this EP*/
                /*Store LM TX counter size*/
                alloc_size += sizeof(uint8);
                /*Store LM TX counters base_ids*/
                alloc_size += sizeof(uint32) * ep_data[idx].lm_counter_size;
                /*Store LM TX counters offsets*/
                alloc_size += sizeof(uint32) * ep_data[idx].lm_counter_size;
                /*Store LM TX counters actions*/
                alloc_size += sizeof(uint32) * ep_data[idx].lm_counter_size;
                /*Store LB TX counter info associated with this EP*/
                /*Store LB TX counter size*/
                alloc_size += sizeof(uint8);
                /*Store LB TX counters base_ids*/
                alloc_size += sizeof(uint32) * ep_data[idx].lb_tx_counter_size;
                /*Store LB TX counters offsets*/
                alloc_size += sizeof(uint32) * ep_data[idx].lb_tx_counter_size;
                /*Store DM TX counter info associated with this EP*/
                /*Store DM TX counter size*/
                alloc_size += sizeof(uint8);
                /*Store DM TX counters base_ids*/
                alloc_size += sizeof(uint32) * ep_data[idx].dm_tx_counter_size;
                /*Store DM TX counters offsets*/
                alloc_size += sizeof(uint32) * ep_data[idx].dm_tx_counter_size;
                /*Store HW provided session ID*/
                alloc_size += sizeof(uint32);
                /*Store HW provided number of session IDs*/
                alloc_size += sizeof(uint8);
                /*Store Trunk index */
                alloc_size += sizeof(int);
                ep_count++;
            }
        }
        /* To store BHH EP count. */
        alloc_size += (sizeof(bcm_oam_endpoint_t) * ep_count);
        if (_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) { 
            alloc_size += _BCM_FP_OAM_PM_PROFILE_SCACHE_SIZE;
        }

        /* Scache Version 1 */

        /* Calculate size taken to accomadate CSF counters */
        csf_cntr_size = 0;
        /* Store CSF TX counter size */
        csf_cntr_size += sizeof(uint8);
        /* Store CSF TX counters base_ids */
        csf_cntr_size += sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX;
        /* Store CSF TX counters offsets */
        csf_cntr_size += sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX;
        /* Counters are per-endpoint */
        csf_cntr_size *= oc->bhh_endpoint_count;

        alloc_size +=  csf_cntr_size;

        /* Allocate memory to store oam_mpls_tp_ach_channel_type */
        alloc_size += sizeof(fp_oam_mpls_tp_ach_channel_type);

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                              bcmFpOamWbSequenceBhh);

        /* Check if memory has already been allocated */
        rv = _bcm_esw_scache_ptr_get(unit,
                scache_handle,
                0,
                0,
                &oam_scache,
                BCM_WB_BHH_DEFAULT_VERSION,
                NULL
                );
        if (!SOC_WARM_BOOT(unit) && (BCM_E_NOT_FOUND == rv)) {
            rv = _bcm_esw_scache_ptr_get(unit,
                    scache_handle,
                    1,
                    alloc_size,
                    &oam_scache,
                    BCM_WB_BHH_DEFAULT_VERSION,
                    NULL
                    );
            if (BCM_FAILURE(rv)
                    || (NULL == oam_scache)) {
                goto cleanup;
            }
        } else if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* To store OAM group count. */
        sal_memcpy(oam_scache, &group_count, sizeof(int));
        oam_scache += sizeof(int);

        for (grp_idx = 0; grp_idx < oc->bhh_endpoint_count; grp_idx++)
        {
            /* Check if the group is in use. */
            if (BCM_E_EXISTS
                    == shr_idxres_list_elem_state(oc->bhh_group_pool, 
                                                  grp_idx)) {

                sal_memcpy(oam_scache, &group_p[grp_idx].group_id,
                        sizeof(int));
                oam_scache += sizeof(int);
                sal_memcpy(oam_scache, &group_p[grp_idx].lmep_id,
                        sizeof(int));
                oam_scache += sizeof(int);
                sal_memcpy(oam_scache, &group_p[grp_idx].flags,
                        sizeof(uint32));
                oam_scache += sizeof(uint32);
                sal_memcpy(oam_scache, group_p[grp_idx].name,
                        BCM_OAM_GROUP_NAME_LENGTH);
                oam_scache += BCM_OAM_GROUP_NAME_LENGTH;
            }
        }
        /* To store BHH EP count. */
        sal_memcpy(oam_scache, &ep_count, sizeof(int));
        oam_scache += sizeof(int);
        for (idx = 0; idx < oc->bhh_endpoint_count; idx++) {

            /* Check if the EP is in use. */
            if (BCM_E_EXISTS
                    == shr_idxres_list_elem_state(oc->bhh_pool, idx)) {
                sal_memcpy(oam_scache, &ep_data[idx].ep_id,
                        sizeof(bcm_oam_endpoint_t));
                oam_scache += sizeof(bcm_oam_endpoint_t);
                sal_memcpy(oam_scache, &ep_data[idx].type,
                        sizeof(bcm_oam_endpoint_type_t));
                oam_scache += sizeof(bcm_oam_endpoint_type_t);
                sal_memcpy(oam_scache, &ep_data[idx].group_index,
                        sizeof(bcm_oam_group_t));
                oam_scache += sizeof(bcm_oam_group_t);
                sal_memcpy(oam_scache, &ep_data[idx].gport,
                        sizeof(bcm_gport_t));
                oam_scache += sizeof(bcm_gport_t);
                sal_memcpy(oam_scache, &ep_data[idx].egr_label,
                        sizeof(bcm_mpls_label_t));
                oam_scache += sizeof(bcm_mpls_label_t);
                sal_memcpy(oam_scache, &ep_data[idx].egr_label_exp,
                        sizeof(uint8));
                oam_scache += sizeof(uint8);
                sal_memcpy(oam_scache, &ep_data[idx].vpn,
                        sizeof(bcm_vpn_t));
                oam_scache += sizeof(bcm_vpn_t);
                sal_memcpy(oam_scache, &ep_data[idx].egress_if,
                        sizeof(bcm_if_t));
                oam_scache += sizeof(bcm_if_t);
                sal_memcpy(oam_scache, &ep_data[idx].pm_profile_attached,
                        sizeof(bcm_oam_pm_profile_t));
                oam_scache += sizeof(bcm_oam_pm_profile_t);
                /*Store CCM TX counter info associated with this EP*/
                /*Store CCM TX counter size*/
                sal_memcpy(oam_scache, &ep_data[idx].ccm_tx_counter_size,
                        sizeof(uint8));
                oam_scache += sizeof(uint8);
                /*Store CCM TX counters base_ids*/
                for (i = 0; i < ep_data[idx].ccm_tx_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].ccm_tx_update_lm_counter_base_id[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                } 
                /*Store CCM TX counters offsets*/
                for (i = 0; i < ep_data[idx].ccm_tx_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].ccm_tx_update_lm_counter_offset[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                } 
                /*Store LM TX counter info associated with this EP*/
                /*Store LM TX counter size*/
                sal_memcpy(oam_scache, &ep_data[idx].lm_counter_size,
                        sizeof(uint8));
                oam_scache += sizeof(uint8);
                /*Store LM TX counters base_ids*/
                for (i = 0; i < ep_data[idx].lm_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].lm_counter_base_id[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                } 
                /*Store LM TX counters offsets*/
                for (i = 0; i < ep_data[idx].lm_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].lm_counter_offset[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                }
                /*Store LM TX counters actions*/
                for (i = 0; i < ep_data[idx].lm_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].lm_counter_action[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                }
                /*Store LB TX counter info associated with this EP*/
                /*Store LB TX counter size*/
                sal_memcpy(oam_scache, &ep_data[idx].lb_tx_counter_size,
                        sizeof(uint8));
                oam_scache += sizeof(uint8);
                /*Store LB TX counters base_ids*/
                for (i = 0; i < ep_data[idx].lb_tx_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].lb_tx_update_lm_counter_base_id[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                } 
                /*Store LB TX counters offsets*/
                for (i = 0; i < ep_data[idx].lb_tx_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].lb_tx_update_lm_counter_offset[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                }  
                /*Store DM TX counter info associated with this EP*/
                /*Store DM TX counter size*/
                sal_memcpy(oam_scache, &ep_data[idx].dm_tx_counter_size,
                        sizeof(uint8));
                oam_scache += sizeof(uint8);
                /*Store DM TX counters base_ids*/
                for (i = 0; i < ep_data[idx].dm_tx_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].dm_tx_update_lm_counter_base_id[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                } 
                /*Store DM TX counters offsets*/
                for (i = 0; i < ep_data[idx].dm_tx_counter_size; i++) {
                    sal_memcpy(oam_scache,
                               &ep_data[idx].dm_tx_update_lm_counter_offset[i],
                               sizeof(uint32));
                    oam_scache += sizeof(uint32);
                }
                /*Store HW session ID*/
                sal_memcpy(oam_scache, &ep_data[idx].hw_session_id,
                        sizeof(uint32));
                oam_scache += sizeof(uint32);
                /*Store HW number of session IDs*/
                sal_memcpy(oam_scache, &ep_data[idx].hw_session_num_entries,
                        sizeof(uint8));
                oam_scache += sizeof(uint8);
                sal_memcpy(oam_scache, &ep_data[idx].trunk_index,
                        sizeof(int));
                oam_scache += sizeof(int);
            }
        }
        if (_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) {
            /* Store pm profile */
            int itr;
            _bcm_fp_oam_pm_profile_control_t *pmc = NULL;
            
            pmc = pm_profile_control[unit];
            if (!pmc) {
                goto cleanup;
            }
            for(itr = 0; itr < _BCM_OAM_MAX_PM_PROFILES; itr++) {
                sal_memcpy(oam_scache,
                        &(pmc->profile_info[itr]), sizeof(_bcm_fp_oam_pm_profile_int_info_t));
                oam_scache += sizeof(_bcm_fp_oam_pm_profile_int_info_t);
            }
        }

        for (idx = 0; idx < oc->bhh_endpoint_count; idx++) {
            /* Check if the EP is in use. */
            if (BCM_E_EXISTS != shr_idxres_list_elem_state(oc->bhh_pool, idx)) {
                continue;
            }
            /* Store CSF TX counter info associated with this EP */
            /* Store CSF TX counter size */
            sal_memcpy(oam_scache, &ep_data[idx].csf_tx_counter_size,
                       sizeof(uint8));
            oam_scache += sizeof(uint8);

            /* Store CSF TX counters base_ids */
            for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
                sal_memcpy(oam_scache,
                           &ep_data[idx].csf_tx_update_lm_counter_base_id[i],
                           sizeof(uint32));
                oam_scache += sizeof(uint32);
            }

            /* Store CSF TX counters offsets */
            for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
                sal_memcpy(oam_scache,
                           &ep_data[idx].csf_tx_update_lm_counter_offset[i],
                           sizeof(uint32));
                oam_scache += sizeof(uint32);
            }
        }
        for (idx = 0; idx < oc->bhh_endpoint_count; idx++) {
            /* Check if the EP is in use. */
            if (BCM_E_EXISTS != shr_idxres_list_elem_state(oc->bhh_pool, idx)) {
                continue;
            }
            sal_memcpy(oam_scache, &ep_data[idx].egr_label_ttl,
                    sizeof(uint8));
            oam_scache += sizeof(uint8);
        }
        sal_memcpy(oam_scache, &fp_oam_mpls_tp_ach_channel_type,
                   sizeof(fp_oam_mpls_tp_ach_channel_type));
        oam_scache += sizeof(fp_oam_mpls_tp_ach_channel_type);
    }

cleanup:
    _BCM_OAM_UNLOCK(oc);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_fp_oam_wb_group_recover
 * Purpose:
 *     Recover OAM group configuration.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     stable_size - (IN) OAM module Level2 storage size.
 *     oam_scache  - (IN) Pointer to scache address pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_wb_group_recover(int unit, int stable_size, uint8 **oam_scache)
{
    int                    index;                    /* Hw table index.     */
    _bhh_fp_oam_group_data_t  *group_p;              /* Group info pointer. */
    _bcm_fp_oam_control_t     *oc;                   /* Pointer to Control  */
                                                     /* structure.          */
    int                     rv;                      /* Operation return    */
                                                     /* status.             */
    uint32              group_count;                 /* Stored OAM group count*/
    int bhh_group_idx = 0, group_id = 0;

    /* Control lock taken by calling routine. */
    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Recover the OAM groups */
    sal_memcpy(&group_count, *oam_scache, sizeof(int));
    *oam_scache = (*oam_scache + sizeof(int));

    for (index = 0; index < group_count; index++) {

        /* Get the group id from stored info. */
        sal_memcpy(&group_id, *oam_scache, 
                sizeof(int));
        *oam_scache = (*oam_scache + sizeof(int));

        bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group_id);
        /* Reserve the group index. */
        rv = shr_idxres_list_reserve(oc->bhh_group_pool, bhh_group_idx,
                                     bhh_group_idx);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: (GID=%d) Index reserve "
                                " failed  - %s.\n"), unit, index, 
                                bcm_errmsg(rv)));
            rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
            goto cleanup;
        }
        /* Get the group memory pointer. */
        group_p = &oc->bhh_group_data[bhh_group_idx];
        if (1 == group_p->in_use) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        group_p->group_id = group_id;
        sal_memcpy(&group_p->lmep_id, *oam_scache, 
                sizeof(int));
        *oam_scache = (*oam_scache + sizeof(int));
        sal_memcpy(&group_p->flags, *oam_scache, 
                sizeof(uint32));
        *oam_scache = (*oam_scache + sizeof(uint32));

        sal_memcpy(group_p->name, *oam_scache, 
                BCM_OAM_GROUP_NAME_LENGTH);
        *oam_scache = (*oam_scache + BCM_OAM_GROUP_NAME_LENGTH);
        group_p->in_use = 1;
    }

    return (BCM_E_NONE);

cleanup:

    if (BCM_E_EXISTS
        == shr_idxres_list_elem_state(oc->group_pool, bhh_group_idx)) {
        shr_idxres_list_free(oc->group_pool, bhh_group_idx);
    }

    return (rv);

}


/*
 * Function:
 *     _bcm_fp_oam_endpoint_alloc
 * Purpose:
 *     Allocate an endpoint memory element.
 * Parameters:
 *     ep_pp - (IN/OUT) Pointer to endpoint address pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_endpoint_alloc(bcm_oam_endpoint_info_t **ep_pp)
{
    bcm_oam_endpoint_info_t *ep_p = NULL;

    _BCM_OAM_ALLOC(ep_p, bcm_oam_endpoint_info_t,
                   sizeof(bcm_oam_endpoint_info_t), "Endpoint info");
    if (NULL == ep_p) {
        return (BCM_E_MEMORY);
    }

    *ep_pp = ep_p;

    return (BCM_E_NONE);

}

/*
 * Function:
 *     _bcm_fp_oam_mpls_entry_label_lookup
 * Purpose:
 *     Finds  a MPLS_ENTRY corresponding to a MPLS label.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     label       - (IN) label to which MPLS_ENTRY object to found
 *     label       - (IN) index in MPLS_ENTRY object to found
 *     found_entry - (OUT) Pointer to MPLS_ENTRY object.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_entry_label_lookup(int unit, bcm_mpls_label_t label,
                                     int *mpls_index,
                                     mpls_entry_entry_t *found_entry)
{
#if defined(INCLUDE_L3)
    mpls_entry_entry_t mpls_key_entry;
    int rv = BCM_E_NONE;

    sal_memset(&mpls_key_entry, 0, sizeof(mpls_key_entry));

    soc_MPLS_ENTRYm_field32_set(unit, &mpls_key_entry, MPLS_LABELf, label);
    soc_MPLS_ENTRYm_field32_set(unit, &mpls_key_entry, VALIDf, 1);

    BCM_IF_ERROR_RETURN(bcm_tr_mpls_lock(unit));
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, mpls_index,
                        &mpls_key_entry, found_entry, 0);
    bcm_tr_mpls_unlock(unit);

    return rv;
#endif /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_fp_oam_wb_endpoints_recover
 * Purpose:
 *     Recover OAM endpoint configuration for DownMEPS.
 * Parameters:
 *     unit        - (IN) BCM device number
 *     stable_size - OAM module Level2 memory size.
 *     oam_scache  - Pointer to secondary storage buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_wb_endpoints_recover(int unit,
                                 int stable_size,
                                 uint8 **oam_scache)
{
    _bhh_fp_oam_ep_data_t *h_data_p = NULL; /* Endpoint hash data pointer. */
    _bcm_fp_oam_control_t      *oc;       /* Pointer to control structure.   */
    int                     rv;           /* Operation return status.        */
    int                     ep_id, ep_count;/* Endpoint ID.                  */
    _bhh_fp_oam_hash_key_t  hash_key;     /* Hash key buffer for lookup.     */
    bcm_oam_endpoint_info_t *ep_info = NULL;  /* Endpoint information.        */
    _bhh_fp_oam_group_data_t *group_p;        /* Group info pointer. */
    int                     idx      = 0, i = 0;
    bcm_oam_endpoint_t      bhh_endpoint_index = 0;
    bhh_sdk_msg_ctrl_sess_get_t msg;
    mpls_entry_entry_t mpls_entry;
    uint16 buffer_len, reply_len;
    uint8 *buffer, *buffer_ptr;
    int mpls_index = -1;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
    uint8*  encap_data = NULL;
    soc_olp_l2_hdr_t olp_l2_header;
    soc_olp_tx_hdr_t olp_oam_tx_header;
    int bhh_group_idx = 0;
    int                  sglp = 0;          /* Source global logical port.  */
    int                  dglp = 0;          /* Dest global logical port.    */
    bcm_gport_t          dgpp = 0;
    bcm_trunk_t          trunk_id = BCM_TRUNK_INVALID;
    bcm_trunk_member_t   trunk_member;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM(unit %d) Info: "
                          "_bcm_fp_oam_wb_endpoints_recover.\n"), unit));
    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Recover the OAM endpoint count */
    sal_memcpy(&ep_count, *oam_scache, sizeof(int));
    *oam_scache = (*oam_scache + sizeof(int));

    for (idx = 0; idx < ep_count; idx++) {

        sal_memcpy(&ep_id, *oam_scache, sizeof(bcm_oam_endpoint_t));
        *oam_scache = (*oam_scache + sizeof(bcm_oam_endpoint_t));

        bhh_endpoint_index = BCM_OAM_BHH_GET_UKERNEL_EP(ep_id);

        h_data_p = &oc->bhh_ep_data[bhh_endpoint_index];
        if (1 == h_data_p->in_use) {
            return (BCM_E_INTERNAL);
        }
        rv = shr_idxres_list_reserve(oc->bhh_pool, bhh_endpoint_index,
                bhh_endpoint_index);
        if (BCM_FAILURE(rv)) {
            rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH(unit %d) Error: Endpoint reserve (EP=%d) - %s.\n"),
                     unit, bhh_endpoint_index, bcm_errmsg(rv)));
            goto cleanup;
        }

        _BHH_FP_OAM_HASH_DATA_CLEAR(h_data_p);
        h_data_p->remote_index = _BCM_OAM_INVALID_INDEX;
        h_data_p->trunk_id =  BCM_TRUNK_INVALID;

        h_data_p->ep_id = ep_id;
        h_data_p->uc_session_id = bhh_endpoint_index;
 
        sal_memcpy(&h_data_p->type, *oam_scache, 
                   sizeof(bcm_oam_endpoint_type_t));
        *oam_scache = (*oam_scache + sizeof(bcm_oam_endpoint_type_t));
        sal_memcpy(&h_data_p->group_index, *oam_scache, 
                   sizeof(bcm_oam_group_t));
        *oam_scache = (*oam_scache + sizeof(bcm_oam_group_t));
        sal_memcpy(&h_data_p->gport, *oam_scache, 
                   sizeof(bcm_gport_t));
        *oam_scache = (*oam_scache + sizeof(bcm_gport_t));
        sal_memcpy(&h_data_p->egr_label, *oam_scache, 
                   sizeof(bcm_mpls_label_t));
        *oam_scache = (*oam_scache + sizeof(bcm_mpls_label_t));
        sal_memcpy(&h_data_p->egr_label_exp, *oam_scache, 
                   sizeof(uint8));
        *oam_scache = (*oam_scache + sizeof(uint8));
        sal_memcpy(&h_data_p->vpn, *oam_scache, 
                   sizeof(bcm_vpn_t));
        *oam_scache = (*oam_scache + sizeof(bcm_vpn_t));

        sal_memcpy(&h_data_p->egress_if, *oam_scache, 
                   sizeof(bcm_if_t));
        *oam_scache = (*oam_scache + sizeof(bcm_if_t));
        sal_memcpy(&h_data_p->pm_profile_attached, *oam_scache, 
                   sizeof(bcm_oam_pm_profile_t));
        *oam_scache = (*oam_scache + sizeof(bcm_oam_pm_profile_t));
        sal_memcpy(&h_data_p->ccm_tx_counter_size, *oam_scache, 
                   sizeof(uint8));
        *oam_scache = (*oam_scache + sizeof(uint8));
        for (i = 0; i < h_data_p->ccm_tx_counter_size; i++) {
            sal_memcpy(&h_data_p->ccm_tx_update_lm_counter_base_id[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        for (i = 0; i < h_data_p->ccm_tx_counter_size; i++) {
            sal_memcpy(&h_data_p->ccm_tx_update_lm_counter_offset[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        sal_memcpy(&h_data_p->lm_counter_size, *oam_scache, 
                   sizeof(uint8));
        *oam_scache = (*oam_scache + sizeof(uint8));
        for (i = 0; i < h_data_p->lm_counter_size; i++) {
            sal_memcpy(&h_data_p->lm_counter_base_id[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        for (i = 0; i < h_data_p->lm_counter_size; i++) {
            sal_memcpy(&h_data_p->lm_counter_offset[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        for (i = 0; i < h_data_p->lm_counter_size; i++) {
            sal_memcpy(&h_data_p->lm_counter_action[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        sal_memcpy(&h_data_p->lb_tx_counter_size, *oam_scache, 
                   sizeof(uint8));
        *oam_scache = (*oam_scache + sizeof(uint8));
        for (i = 0; i < h_data_p->lb_tx_counter_size; i++) {
            sal_memcpy(&h_data_p->lb_tx_update_lm_counter_base_id[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        for (i = 0; i < h_data_p->lb_tx_counter_size; i++) {
            sal_memcpy(&h_data_p->lb_tx_update_lm_counter_offset[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        sal_memcpy(&h_data_p->dm_tx_counter_size, *oam_scache, 
                   sizeof(uint8));
        *oam_scache = (*oam_scache + sizeof(uint8));
        for (i = 0; i < h_data_p->dm_tx_counter_size; i++) {
            sal_memcpy(&h_data_p->dm_tx_update_lm_counter_base_id[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        for (i = 0; i < h_data_p->dm_tx_counter_size; i++) {
            sal_memcpy(&h_data_p->dm_tx_update_lm_counter_offset[i],
                       *oam_scache,
                    sizeof(uint32));
            *oam_scache = (*oam_scache + sizeof(uint32));
        }
        sal_memcpy(&h_data_p->hw_session_id, *oam_scache, 
                   sizeof(uint32));
        *oam_scache = (*oam_scache + sizeof(uint32));
        sal_memcpy(&h_data_p->hw_session_num_entries, *oam_scache, 
                   sizeof(uint8));
        *oam_scache = (*oam_scache + sizeof(uint8));
        sal_memcpy(&h_data_p->trunk_index, *oam_scache, 
                   sizeof(int));
        *oam_scache = (*oam_scache + sizeof(int));

        h_data_p->level = _BCM_OAM_EP_LEVEL_MAX;
        /* Get the endpoint parameters from firmware */
        rv = _bcm_fp_oam_msg_send_receive(unit,
                MOS_MSG_CLASS_BHH, 
                MOS_MSG_SUBCLASS_BHH_SESS_GET,
                bhh_endpoint_index, 0,
                MOS_MSG_SUBCLASS_BHH_SESS_GET_REPLY,
                &reply_len);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH(unit %d) Error: ukernel msg " 
                                "failed for"
                                " EP=%d %s.\n"), unit, 
                     bhh_endpoint_index, 
                     bcm_errmsg(rv)));
            goto cleanup;
        }

        buffer = oc->bhh_dma_buffer;
        buffer_ptr = bhh_sdk_msg_ctrl_sess_get_unpack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            rv = BCM_E_INTERNAL;
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH(unit %d) Error: ukernel msg " 
                                "failed for"
                                " EP=%d %s.\n"), unit, 
                     bhh_endpoint_index, 
                     bcm_errmsg(rv)));
            goto cleanup;
        }

        h_data_p->name = msg.mep_id;
        h_data_p->period = msg.local_period;
        h_data_p->label = msg.mpls_label;
        /* Set the flags */
        if (msg.flags & SHR_BHH_SESS_SET_F_MIP)
            h_data_p->flags |= BCM_OAM_ENDPOINT_INTERMEDIATE;

        if (!BHH_EP_MPLS_SECTION_TYPE(h_data_p)) {
            /* port value from l3_egress_if*/
            bcm_l3_egress_t_init(&l3_egress);
            if (BCM_FAILURE (bcm_esw_l3_egress_get(unit,
                            h_data_p->egress_if, &l3_egress))) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Failed to get L3 "
                                    "egress object EP=%d egress_if=%d %s.\n"),
                                    unit, bhh_endpoint_index,
                         h_data_p->egress_if, bcm_errmsg(rv)));
                goto cleanup;
            }

            l3_intf.l3a_intf_id = l3_egress.intf;
            if (BCM_FAILURE(bcm_esw_l3_intf_get(unit, &l3_intf))) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Failed to get L3 "
                                    " interface EP=%d l3a_intf_id=%d %s.\n"),
                                    unit, bhh_endpoint_index,
                         l3_intf.l3a_intf_id, bcm_errmsg(rv)));
                goto cleanup;
            }
            sal_memcpy(h_data_p->dst_mac_address, l3_egress.mac_addr,
                       _BHH_MAC_ADDR_LENGTH);
            sal_memcpy(h_data_p->src_mac_address, l3_intf.l3a_mac_addr,
                       _BHH_MAC_ADDR_LENGTH);

            h_data_p->sglp = l3_egress.port;
            h_data_p->dglp = l3_egress.port;
            h_data_p->vlan = l3_intf.l3a_vid;
        } else {
            encap_data = msg.encap_data;
            encap_data = _bcm_fp_oam_olp_l2_header_unpack(encap_data, 
                                                          &olp_l2_header);
            encap_data = shr_olp_tx_header_unpack(encap_data,
                                                  &olp_oam_tx_header);
            h_data_p->int_pri = SOC_OLP_TX_PRI_GET(&olp_oam_tx_header);
            sal_memcpy(h_data_p->dst_mac_address, encap_data,
                       _BHH_MAC_ADDR_LENGTH);
            encap_data += _BHH_MAC_ADDR_LENGTH;

            sal_memcpy(h_data_p->src_mac_address, encap_data,
                       _BHH_MAC_ADDR_LENGTH);
            encap_data += _BHH_MAC_ADDR_LENGTH;
            if((h_data_p->type == bcmOAMEndpointTypeBhhSectionOuterVlan) ||
               (h_data_p->type == bcmOAMEndpointTypeBhhSectionInnervlan) ||
                (h_data_p->type == 
                           bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)){
                 encap_data += SHR_BHH_L2_ETYPE_LENGTH;
                 sal_memcpy(&h_data_p->vlan, encap_data, 
                       SHR_BHH_L2_VLAN_HEADER_LENGTH);
                 h_data_p->vlan = (h_data_p->vlan & _BHH_VLAN_MASK); 
                 encap_data += SHR_BHH_L2_VLAN_HEADER_LENGTH;
            }
            if(h_data_p->type == 
                         bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan){
                 sal_memcpy(&h_data_p->inner_vlan, encap_data, 
                       SHR_BHH_L2_VLAN_HEADER_LENGTH);
                 h_data_p->inner_vlan = (h_data_p->vlan & _BHH_VLAN_MASK);
                 encap_data += SHR_BHH_L2_VLAN_HEADER_LENGTH;
            }
        }
        bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(h_data_p->group_index);
        /* Get the group memory pointer. */
        group_p = &oc->bhh_group_data[bhh_group_idx];
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_TIMEOUT){
            group_p->faults |= BCM_OAM_BHH_FAULT_CCM_TIMEOUT;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_RDI){
            group_p->faults |= BCM_OAM_BHH_FAULT_CCM_RDI;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL){
            group_p->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID){
            group_p->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID){
            group_p->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD){
            group_p->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY){
            group_p->faults |=
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY;
        }

        if (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv) {
            /* get vccv_type from MPLS_ENTRY */
            rv = _bcm_fp_oam_mpls_entry_label_lookup(unit, h_data_p->label,
                    &mpls_index,
                    &mpls_entry);
            if(rv != BCM_E_NONE) {
                /* Not found or some other error */
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: (Label=%u) - %s.\n"),
                         unit, h_data_p->label, bcm_errmsg(rv)));
                goto cleanup;
            }
            h_data_p->vccv_type = soc_MPLS_ENTRYm_field32_get(unit, &mpls_entry,
                                                              PW_CC_TYPEf);
            h_data_p->vccv_type--;
        }
        rv = _bcm_fp_oam_endpoint_alloc(&ep_info);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        bcm_oam_endpoint_info_t_init(ep_info);

        /* Set up endpoint information for key construction. */
        ep_info->group = h_data_p->group_index;
        ep_info->name = h_data_p->name;
        ep_info->gport = h_data_p->gport;
        ep_info->level = h_data_p->level;
        ep_info->vlan = h_data_p->vlan;
        ep_info->inner_vlan = h_data_p->inner_vlan;
        ep_info->mpls_label = h_data_p->label;
        ep_info->type       = h_data_p->type;
        ep_info->trunk_index = h_data_p->trunk_index;
        ep_info->ccm_period = h_data_p->period;

        /* Resolve given endpoint gport value to SGLP and DGLP values. */
        rv = _bcm_fp_oam_endpoint_gport_resolve(unit, ep_info,
                                                &sglp, &dglp, &dgpp,
                                                &trunk_id, &trunk_member);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                          "OAM(unit %d) Error: Gport resolve (EP=%d) - %s.\n"),
                       unit, h_data_p->ep_id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
        if (BCM_TRUNK_INVALID != trunk_id) {
            h_data_p->trunk_id = trunk_id;
            h_data_p->resolved_trunk_gport = trunk_member.gport;
        }
        /*
         * Calculate hash key for hash table insert
         * operation.
         */
        _bhh_fp_oam_ep_hash_key_construct(unit, oc, ep_info, &hash_key);

        sal_free(ep_info);

        rv = shr_htb_insert(oc->bhh_mep_htbl, hash_key, h_data_p);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH(unit %d) Error: Hash table insert"
                                "(EP=%d) failed - %s.\n"), unit,
                     h_data_p->ep_id, bcm_errmsg(rv)));
            goto cleanup;
        } else {
            LOG_DEBUG(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "BHH(unit %d) Info: Hash Tbl (EP=%d)"
                                " inserted  - %s.\n"), unit, h_data_p->ep_id,
                     bcm_errmsg(rv)));
        }
        h_data_p->in_use = 1;
    } /* end of for loop */

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM(unit %d) Info: _bcm_fp_oam_wb_endpoints_recover"
                           " - done.\n"), unit));
    return (BCM_E_NONE);

cleanup:

    if (NULL != ep_info) {
        sal_free(ep_info);
    }

    if (BCM_E_EXISTS
         == shr_idxres_list_elem_state(oc->bhh_pool,bhh_endpoint_index)) {
            shr_idxres_list_free(oc->bhh_pool,bhh_endpoint_index);
    }

    _BHH_FP_OAM_HASH_DATA_CLEAR(h_data_p);

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM(unit %d) Info: _bcm_fp_oam_wb_endpoints_recover"
                           " - error_done.\n"), unit));
    return (rv);

}

/*
 * Function:
 *     _bcm_fp_oam_wb_csf_recover
 * Purpose:
 *     Recover BHH OAM CSF configuration
 * Parameters:
 *     unit        - (IN) BCM device number
 *     stable_size - OAM module Level2 memory size.
 *     oam_scache  - Pointer to secondary storage buffer pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_bhh_wb_csf_recover(int unit, int stable_size, uint8 **oam_scache)
{
    _bhh_fp_oam_ep_data_t  *h_data_p = NULL;
    _bcm_fp_oam_control_t  *oc;
    int                     idx, i;


    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM(unit %d) Info: "
                          "_bcm_fp_oam_bhh_wb_csf_recover.\n"), unit));
    /*
     * Get OAM control structure.
     *     Note: Lock taken by calling routine.
     */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    for (idx = 0; idx < oc->bhh_endpoint_count; idx++) {

        h_data_p = &oc->bhh_ep_data[idx];
        if (!h_data_p->in_use) {
            continue;
        }

        sal_memcpy(&h_data_p->csf_tx_counter_size, *oam_scache, sizeof(uint8));
        *oam_scache += sizeof(uint8);

        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            sal_memcpy(&h_data_p->csf_tx_update_lm_counter_base_id[i],
                       *oam_scache, sizeof(uint32));
            *oam_scache += sizeof(uint32);
        }
        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            sal_memcpy(&h_data_p->csf_tx_update_lm_counter_offset[i],
                       *oam_scache, sizeof(uint32));
            *oam_scache += sizeof(uint32);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_reinit
 * Purpose:
 *     Reconstruct BHH software state.
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_bhh_reinit(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *oam_scache = NULL;/* Pointer to scache memory. */
    soc_scache_handle_t scache_handle;     /* Scache memory handler.    */
    _bcm_fp_oam_control_t  *oc;            /* Pointer to OAM control structure*/
    int                 rv = BCM_E_NONE;   /* Operation return status.     */
    uint16              recovered_ver = 0;
    uint16              csf_cntr_size = 0;
    uint32              realloc_size = 0;
    uint16              ttl_cntr_size = 0;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "OAM(unit %d) Info: OAM warm boot recovery.....\n"),
               unit));

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NONE;
    }
    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                              bcmFpOamWbSequenceBhh);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &oam_scache, BCM_WB_BHH_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            _bcm_fp_oam_bhh_scache_alloc(unit);
            rv = BCM_E_NONE;
            goto cleanup;
        } else if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    } else {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    rv = _bcm_fp_oam_wb_group_recover(unit, stable_size, &oam_scache);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Group recovery failed  - "
                              "%s.\n"),
                   unit, bcm_errmsg(rv)));
        goto cleanup;
    }

    rv = _bcm_fp_oam_wb_endpoints_recover(unit, stable_size, &oam_scache);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint recovery failed  "
                              "- %s.\n"),
                   unit, bcm_errmsg(rv)));
        goto cleanup;
    }
    if (_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) {
        int itr;
        _bcm_fp_oam_pm_profile_control_t *pmc = NULL;

        pmc = pm_profile_control[unit];
        if (!pmc) {
            rv = BCM_E_INIT;
            goto cleanup;
        }
        for(itr = 0; itr < _BCM_OAM_MAX_PM_PROFILES; itr++) {
            sal_memcpy(&(pmc->profile_info[itr]), oam_scache,
                       sizeof(_bcm_fp_oam_pm_profile_int_info_t));
            oam_scache += sizeof(_bcm_fp_oam_pm_profile_int_info_t);
        }
    }

    if (recovered_ver >= BCM_BHH_WB_VERSION_1_2) {
        int idx = 0;
        _bhh_fp_oam_ep_data_t *h_data_p = NULL; /* Endpoint hash data pointer. */
        for (idx = 0; idx < oc->bhh_endpoint_count; idx++) {
            h_data_p = &oc->bhh_ep_data[idx];
            if (!h_data_p->in_use) {
                continue;
            }

            sal_memcpy(&h_data_p->egr_label_ttl, oam_scache,
                    sizeof(uint8));
            oam_scache = (oam_scache + sizeof(uint8));
            ttl_cntr_size += sizeof(uint8);
        }
    }

    if (recovered_ver >= BCM_BHH_WB_VERSION_1_1) {
        rv = _bcm_fp_oam_bhh_wb_csf_recover(unit, stable_size, &oam_scache);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: CSF recovery failed  "
                                  "- %s.\n"),
                       unit, bcm_errmsg(rv)));
            goto cleanup;
        }
    }

    if (recovered_ver >= BCM_BHH_WB_VERSION_1_3) {
        sal_memcpy(&fp_oam_mpls_tp_ach_channel_type, oam_scache,
                   sizeof(fp_oam_mpls_tp_ach_channel_type));
        oam_scache += sizeof(fp_oam_mpls_tp_ach_channel_type);
    }

    /* Increase scache size, in case of upgrading from older version */
    if (recovered_ver < BCM_BHH_WB_VERSION_1_1) {
        /* v1.1 add csf counters to scache */

        /* Calculate size taken to accomadate CSF counters */
        csf_cntr_size = 0;
        /* Store CSF TX counter size */
        csf_cntr_size += sizeof(uint8);
        /* Store CSF TX counters base_ids */
        csf_cntr_size += sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX;
        /* Store CSF TX counters offsets */
        csf_cntr_size += sizeof(uint32) * BCM_OAM_LM_COUNTER_MAX;
        /* Counters are per-endpoint */
        csf_cntr_size *= oc->bhh_endpoint_count;

        realloc_size +=  csf_cntr_size;
    }

    /* Increase scache size, in case of upgrading from older version */
    if (recovered_ver < BCM_BHH_WB_VERSION_1_2) {
        /* v1.2 add csf counters to scache */
        realloc_size +=  ttl_cntr_size;
    }

    if (recovered_ver < BCM_BHH_WB_VERSION_1_3) {
        /* Store the BHH ACH Channel Type */
        realloc_size += sizeof(fp_oam_mpls_tp_ach_channel_type);
    }

    if (realloc_size > 0) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                              bcmFpOamWbSequenceBhh);
        rv = soc_scache_realloc(unit, scache_handle, realloc_size);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,"OAM Error: scache alloc failed  - %s.\n"),
                     bcm_errmsg(rv)));
            goto cleanup;
        }
    }

cleanup:
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

#endif

STATIC uint8
bcm_fp_oam_bhh_get_oam_exp_from_encap(_bhh_fp_oam_ep_data_t *h_data_p,
                                      uint8 *encap_data, uint16 encap_length)
{
    uint16        oam_label_offset = 0;
    _mpls_label_t label;

    if (BHH_EP_MPLS_SECTION_TYPE(h_data_p)) {
        /* Go back ACH and GAL */
        oam_label_offset = encap_length -
                           (SHR_BHH_ACH_LENGTH + SHR_BHH_MPLS_LABEL_LENGTH);
    } else if (BHH_EP_LSP_TYPE(h_data_p)) {
        /* Go back ACH + GAL + OAM label */
        oam_label_offset = encap_length -
                          (SHR_BHH_ACH_LENGTH + (SHR_BHH_MPLS_LABEL_LENGTH * 2));
    } else { /* PW */
        if (h_data_p->vccv_type == bcmOamBhhVccvGal13) {
            /* Go back ACH + GAL + PW label */
            oam_label_offset = encap_length -
                          (SHR_BHH_ACH_LENGTH + (SHR_BHH_MPLS_LABEL_LENGTH * 2));
        } else {
            /* Go back ACH and PW label */
            oam_label_offset = encap_length -
                               (SHR_BHH_ACH_LENGTH + SHR_BHH_MPLS_LABEL_LENGTH);
        }
    }

    _bcm_fp_oam_mpls_label_unpack(&(encap_data[oam_label_offset]), &label);
    return label.exp;
}
/*
 * Function:
 *     _bcm_fp_oam_bhh_group_create
 * Purpose:
 *     Create or replace a BHH group object
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group information.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bhh_fp_oam_group_data_t *oam_group = NULL; /* OAM Group being created */
    int rv = 0;                                 /* return value */
    int bhh_group_idx = 0;
    uint32 session_flags = 0;
    bhh_sdk_msg_ctrl_sess_set_t msg_sess;
    uint16 buffer_len, reply_len;
    uint8 *buffer = NULL;
    uint8 *buffer_ptr = NULL;

    /* Validate input parameter. */
    if (NULL == group_info) {
        return (BCM_E_PARAM);
    }

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    if (oc->ukernel_not_ready == 1){
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: BTE(ukernel) "
                            "not ready.\n"), unit));
        return (BCM_E_INIT);
    }
    /* Validate group id. */
    if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        _BCM_FP_OAM_BHH_GROUP_INDEX_VALIDATE(group_info->id);
    }

    _BCM_OAM_LOCK(oc);

    /*
     * If MA group create is called with replace flag bit set.
     *  - Check and return error if a group does not exist with the ID.
     */
    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
            bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group_info->id);
            /* Search the list with the MA Group ID value. */
            rv = shr_idxres_list_elem_state(oc->bhh_group_pool, bhh_group_idx);
            if (BCM_E_EXISTS != rv) {
                _BCM_OAM_UNLOCK(oc);
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                          "OAM Error: Group does not exist.\n")));
                return (BCM_E_PARAM);
            }
        } else {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                      "OAM Error: Replace command needs a valid Group ID.\n")));
            return (BCM_E_PARAM);
        }
    } else if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group_info->id);
        /*
         * If MA group create is called with ID flag bit set.
         *  - Check and return error if the ID is already in use.
         */
        rv = shr_idxres_list_reserve(oc->bhh_group_pool, bhh_group_idx,
                                     bhh_group_idx);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
        }
    } else {
        /* Reserve the next available group index. */
         rv = shr_idxres_list_alloc(oc->bhh_group_pool,
                                    (shr_idxres_element_t *) &bhh_group_idx);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                      "OAM Error: Group allocation (GID=%d) %s\n"),
                      group_info->id, bcm_errmsg(rv)));
            return (rv);
        }
        group_info->id = BCM_OAM_BHH_GET_SDK_GROUP(bhh_group_idx);
    }

    /* Get this group memory to store group information. */
    oam_group = &(oc->bhh_group_data[bhh_group_idx]);
    
    /* Store the group name. */
    sal_memcpy(oam_group->name, group_info->name,
            BCM_OAM_GROUP_NAME_LENGTH);

    /* Store the group flags. */
    oam_group->flags = group_info->flags;

    /* Set LMEP as not configured */
    oam_group->lmep_id = _BHH_FP_OAM_INVALID_LMEP_ID;

    oam_group->group_id = group_info->id;

    oam_group->in_use = 1;

    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        /* Send message to uC to set the Soft RDI   */
        if (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) {

            /* Set the RDI flag in session bits */
            session_flags |= SHR_BHH_SESS_SET_F_RDI;

            /* Get the session is from endpoint */
            msg_sess.sess_id =
                BCM_OAM_BHH_GET_UKERNEL_EP(oam_group->lmep_id);

            /* Pack control message data into DMA buffer */
            msg_sess.flags = session_flags;

            buffer     = oc->bhh_dma_buffer;
            buffer_ptr =
                bhh_sdk_msg_ctrl_sess_set_pack(buffer, &msg_sess);
            buffer_len = buffer_ptr - buffer;

            /* Send BHH Session Update message to uC */
            rv =  _bcm_fp_oam_msg_send_receive(unit,
                    MOS_MSG_CLASS_BHH, 
                    MOS_MSG_SUBCLASS_BHH_SESS_SET,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_SESS_SET_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                                "OAM(unit %d) Error: ukernel msg failed for"
                                "%s.\n"), unit, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_INTERNAL);
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_group_get
 * Purpose:
 *     Get a BHH group object.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_group_get(int unit, bcm_oam_group_info_t *group_info) {

    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bhh_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    uint16 buffer_len, reply_len;
    uint8 *buffer = NULL;
    uint8 *buffer_ptr = NULL;
    bhh_sdk_msg_ctrl_sess_get_t msg;
    int bhh_group_idx = 0;
    int sess_id = 0;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));


    bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group_info->id);
    /* Get Handle to Local DB */
    group_p = &(oc->bhh_group_data[bhh_group_idx]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    if (group_p->lmep_id != _BHH_FP_OAM_INVALID_LMEP_ID) {
        sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(group_p->lmep_id);
        rv = _bcm_fp_oam_msg_send_receive(unit,
                MOS_MSG_CLASS_BHH, 
                MOS_MSG_SUBCLASS_BHH_SESS_GET,
                sess_id, 0,
                MOS_MSG_SUBCLASS_BHH_SESS_GET_REPLY,
                &reply_len);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: ukernel msg failed for"
                                " EP=%d %s.\n"), unit, group_p->lmep_id, 
                     bcm_errmsg(rv)));
            return (rv);
        }

        /* Pack control message data into DMA buffer */
        buffer = oc->bhh_dma_buffer;
        buffer_ptr = bhh_sdk_msg_ctrl_sess_get_unpack(buffer, &msg);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            rv = BCM_E_INTERNAL;
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: ukernel msg failed for"
                                " EP=%d %s.\n"), unit, group_p->lmep_id, 
                     bcm_errmsg(rv)));
            return (rv);
        } else {

            if(msg.fault_flags & BHH_BTE_EVENT_CCM_TIMEOUT){
                group_info->faults |= BCM_OAM_BHH_FAULT_CCM_TIMEOUT;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CCM_RDI){
                group_info->faults |= BCM_OAM_BHH_FAULT_CCM_RDI;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL){
                group_info->faults |= 
                    BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID){
                group_info->faults |= 
                    BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID){
                group_info->faults |= 
                    BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD){
                group_info->faults |= 
                    BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY){
                group_info->faults |= 
                    BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY;
            }  
            if(msg.fault_flags & BHH_BTE_EVENT_CSF_LOS) {
                group_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_LOS;
            }
            if(msg.fault_flags & BHH_BTE_EVENT_CSF_FDI) {
                group_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_FDI;
            }
            if(msg.fault_flags & BHH_BTE_EVENT_CSF_RDI) {
                group_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_RDI;
            }
        }
    }
    sal_memcpy(group_info->name, group_p->name, BCM_OAM_GROUP_NAME_LENGTH);
    group_info->flags = group_p->flags;

    return rv;
}

#define _BCM_FP_OAM_MAX_CCM_INTERVALS 8
/*  
 * FP OAM CCM intervals array initialization..
 */ 
STATIC uint32 _fp_oam_ccm_intervals[] =
{
    BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED,
    BCM_OAM_ENDPOINT_CCM_PERIOD_3MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_100MS,
    BCM_OAM_ENDPOINT_CCM_PERIOD_1S,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10S,
    BCM_OAM_ENDPOINT_CCM_PERIOD_1M,
    BCM_OAM_ENDPOINT_CCM_PERIOD_10M,
    _BCM_OAM_ENDPOINT_CCM_PERIOD_UNDEFINED
};


/*
 * Function:
 *     _bcm_fp_oam_period_validate
 * Purpose:
 *     Validates whether OAM period is in allowed range or not .
 * Parameters:
 *     period -  (IN) OAM PDU interval in milli seconds.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_period_validate(int period)
{
    int q_period = 0; /* OAM period value. */

    /* Find whether requested period is matching with supported period or not */
    for (q_period = 0; q_period <
            _BCM_FP_OAM_MAX_CCM_INTERVALS; q_period++) {
        if (period ==  _fp_oam_ccm_intervals[q_period]) {
            return BCM_E_NONE; 
        }
    }

    return BCM_E_PARAM;
}

/*
 * Function:
 *      _bcm_fp_oam_bhh_appl_callback
 * Purpose:
 *      Update FW BHH appl state
 * Parameters:
 *      unit  - (IN) Unit number.
 *      uC    - (IN) core number.
 *      stage - (IN) core reset stage.
 *      user_data - (IN) data pointer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int _bcm_fp_oam_bhh_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
    _bcm_fp_oam_control_t *oc;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);
    oc->ukernel_not_ready = 1;

    _BCM_OAM_UNLOCK(oc);

    return BCM_E_NONE;
}

/*
 *  * Function:
 *      _bcm_fp_oam_bhh_endpoint_create
 * Purpose:
 *      Create or replace a BHH FP OAM endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_fp_oam_bhh_endpoint_create(int unit, 
                                bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_fp_oam_control_t *oc;
    _bhh_fp_oam_ep_data_t *hash_data = NULL;/* Endpoint hash data pointer.  */
    _bhh_fp_oam_ep_data_t h_stored_data;    /* dummy Endpoint hash data     */
    _bhh_fp_oam_group_data_t *group_p;      /* Pointer to group data.       */
    _bhh_fp_oam_hash_key_t  hash_key;       /* Hash Key buffer.             */
    int                  rv = BCM_E_NONE;   /* Operation return status.     */
    int                  is_remote = 0;     /* Remote endpoint status.      */
    int                  is_replace;
    int                  is_local = 0;         
    int                  sglp = 0;          /* Source global logical port.  */
    int                  dglp = 0;          /* Dest global logical port.    */
    bcm_gport_t          dgpp = 0;
    bcm_module_t         module_id;         /* Module ID                    */
    bcm_port_t           port_id;
    bcm_trunk_t          trunk_id = BCM_TRUNK_INVALID;
    int                  local_id;
    bhh_sdk_msg_ctrl_rmep_create_t msg_rmep_create;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    uint32 session_flags;
    int bhh_pool_ep_idx = 0;
    int bhh_group_idx = 0;
    int i = 0;
    bcm_trunk_member_t   trunk_member;
    _bhh_oam_lm_dm_info_t counter_info;
    uint8 lm_pdu_counter_offset = 0;
    _l2_header_t l2_hdr;
    bcm_stat_group_mode_t stat_group;
    bcm_stat_object_t stat_object;
    uint32 stat_mode;
    uint32 stat_pool_number;
    uint32 base_index;

    LOG_DEBUG(BSL_LS_BCM_OAM,
              (BSL_META_U(unit,
                          "BHH(unit %d) Info: _bcm_fp_oam_bhh_endpoint_create"
                           "Endpoint ID=%d.\n"), unit, endpoint_info->id));

    _BCM_OAM_BHH_IS_VALID(unit);

    /* Validate input parameter. */
    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_FP_OAM_BHH_GROUP_INDEX_VALIDATE(endpoint_info->group);
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready == 1){
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: BTE(ukernel) "
                            "not ready.\n"), unit));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_INIT);
    }

    /* Perform BHH specific validation checks */
    /* BHH EP can only be at the MAX Level */
    if (endpoint_info->level != _BCM_OAM_EP_LEVEL_MAX ) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                   "OAM(unit %d) Error: EP Level should be equal to %d\n"),
                   unit, _BCM_OAM_EP_LEVEL_MAX));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_PARAM);
    }


    /* For replace operation, endpoint ID is required. */
    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)
        && !(endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_PARAM);
    }

    bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(endpoint_info->group);
    rv = shr_idxres_list_elem_state(oc->bhh_group_pool, bhh_group_idx);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group (GID:%d) does not exist.\n"), endpoint_info->group));
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_PARAM);
    }

    /*Validate CCM period */
    rv = _bcm_fp_oam_period_validate(endpoint_info->ccm_period);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit, "OAM Error:  Invalid CCM period : %d \n"),
                                      endpoint_info->ccm_period));
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    group_p = &(oc->bhh_group_data[bhh_group_idx]);
    if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {

        /* Only One LMEP should be configured per group */
        if (group_p->lmep_id != _FP_OAM_INVALID_LMEP_ID) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "LMEP already existst for the group\n")));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_EXISTS);
        }
        /*Atleast one valid session entry should be present for endpoint*/
        if (endpoint_info->session_num_entries ==
                _BCM_FP_OAM_INVALID_NUM_SESSION_ENTRIES) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: number of session entries "
                                "cannot be zero for EP\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_PARAM);
        }
    }

    /* Calculate the hash key for given enpoint input parameters. */
    _bhh_fp_oam_ep_hash_key_construct(unit, oc, endpoint_info, &hash_key);
    rv = shr_htb_find(oc->bhh_mep_htbl, hash_key,
                      (shr_htb_data_t *)&h_stored_data, 0);
    if (BCM_SUCCESS(rv)
        && !(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {

        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint ID=%d %s.\n"),
                   unit, endpoint_info->id, bcm_errmsg(BCM_E_EXISTS)));

        /* Endpoint must not be in use expect for Replace operation. */
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_EXISTS);

    } else {

        LOG_DEBUG(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Info: Endpoint ID=%d "
                              "Available. %s.\n"),
                   unit, endpoint_info->id, bcm_errmsg(rv)));

    }


    /* Get MEP remote endpoint status. */
    is_remote = (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) ? 1 : 0;

    is_replace = ((endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE) != 0);

    /* Validate EP Id if BCM_OAM_ENDPOINT_WITH_ID flag is set */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        if((endpoint_info->id < oc->bhh_base_endpoint_id) ||
           (endpoint_info->id >= (oc->bhh_base_endpoint_id 
                                  + oc->bhh_endpoint_count))) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_PARAM);
        }
    }

    /* Validate remote EP Id if BCM_OAM_ENDPOINT_REMOTE flag is set */
    if (is_remote) {
        if((endpoint_info->local_id < oc->bhh_base_endpoint_id) ||
           (endpoint_info->local_id >= (oc->bhh_base_endpoint_id
                                        + oc->bhh_endpoint_count))) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_PARAM);
        }
    }

    /* Validate if the gport is an MPLS port for PW MEP */
    if (endpoint_info->type == bcmOAMEndpointTypeBHHMPLSVccv) {
        if (-1 == BCM_GPORT_MPLS_PORT_ID_GET(endpoint_info->gport)) {
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
        }
    }

    if (is_replace) {
        if (is_remote) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: Replace is "
                                " not allowed on remote endpoint\n"),
                         unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_PARAM);
        }
        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_info->id);
        hash_data = &oc->bhh_ep_data[bhh_pool_ep_idx];
        if (!hash_data->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_NOT_FOUND);
        }
        if(!BHH_EP_MPLS_SECTION_TYPE(hash_data)){
            if ((hash_data->egress_if != endpoint_info->intf_id) ||
                (hash_data->label != endpoint_info->mpls_label) ||
                (hash_data->egr_label != endpoint_info->egress_label.label) ||
                (hash_data->gport != endpoint_info->gport) ||
                (hash_data->period != endpoint_info->ccm_period) ||
                (hash_data->vccv_type != endpoint_info->vccv_type) ||
                (hash_data->vpn  != endpoint_info->vpn) ||
                (hash_data->name != endpoint_info->name) || 
                (hash_data->level != endpoint_info->level) ||
                (hash_data->type != endpoint_info->type) ||
                (hash_data->ep_id != endpoint_info->id) ||
                (hash_data->group_index != endpoint_info->group)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Parameters passed are "
                                    " not valid to replace\n"),
                         unit));
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_PARAM);
            }
        } else {
            if ((hash_data->gport != endpoint_info->gport) ||
                (hash_data->period != endpoint_info->ccm_period) ||
                (hash_data->name != endpoint_info->name) ||
                (hash_data->level != endpoint_info->level) ||
                (hash_data->type != endpoint_info->type) ||
                (hash_data->ep_id != endpoint_info->id) ||
                (hash_data->group_index != endpoint_info->group) ||
                (hash_data->vlan != endpoint_info->vlan) ||
                (hash_data->inner_vlan != endpoint_info->inner_vlan) ||
                (hash_data->outer_tpid != endpoint_info->outer_tpid) ||
                (hash_data->inner_tpid != endpoint_info->inner_tpid) ||
                (hash_data->vlan_pri != endpoint_info->pkt_pri) ||
                (hash_data->inner_vlan_pri != endpoint_info->inner_pkt_pri)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Parameters passed are"
                                    " not valid to replace\n"),
                         unit));
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_PARAM);
            }
        }   
    }

    /* Interface validation check */
    if(!BHH_EP_MPLS_SECTION_TYPE(endpoint_info)){
        if ((!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, endpoint_info->intf_id)) && 
             (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, endpoint_info->intf_id))) { 
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: intf type"
                                " not valid. It should be DVP_EGRESS or "
                                "L3_EGRESS type.\n"),
                     unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_PARAM);
        }
    }
    /* Now that we passed the validation checks
     * Create a new endpoint with the requested ID. */
    if ( !is_remote && (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) ) {
        
        bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_info->id);

        hash_data = &oc->bhh_ep_data[bhh_pool_ep_idx];

        if (!is_replace && hash_data->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_EXISTS);
        }

        if(!is_replace) {
            rv = shr_idxres_list_reserve(oc->bhh_pool, bhh_pool_ep_idx,
                                     bhh_pool_ep_idx);
            if (BCM_FAILURE(rv)) {
                rv = (rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv;
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH(unit %d) Error: Endpoint reserve (EP=%d) - %s.\n"),
                           unit, endpoint_info->id, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
    } else {

        /* BHH uses local and remote same index */
        if(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            bhh_pool_ep_idx = BCM_OAM_BHH_GET_UKERNEL_EP
                              (endpoint_info->local_id);
        }
        else {
            /* Allocate the next available endpoint index. */
            rv = shr_idxres_list_alloc(oc->bhh_pool,
                                   (shr_idxres_element_t *)&bhh_pool_ep_idx);
            if (BCM_FAILURE(rv)) {
                rv = (rv == BCM_E_RESOURCE) ? (BCM_E_FULL) : rv;
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH(unit %d) Error: Endpoint alloc failed - %s.\n"),
                           unit, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
        endpoint_info->id = BCM_OAM_BHH_GET_SDK_EP(bhh_pool_ep_idx);
    }

    /* Get the hash data pointer where the data is to be stored. */
    hash_data = &oc->bhh_ep_data[bhh_pool_ep_idx];

    if (is_remote) {

        if (!hash_data->in_use) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_NOT_FOUND);
        } else if (hash_data->flags & BCM_OAM_ENDPOINT_REMOTE) {
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_EXISTS);
        }
            msg_rmep_create.sess_id = bhh_pool_ep_idx;
            msg_rmep_create.flags = 0;
            msg_rmep_create.enable = 1;
            msg_rmep_create.remote_mep_id = endpoint_info->name;
            msg_rmep_create.period = endpoint_info->ccm_period;

            /* Pack control message data into DMA buffer */
            buffer     = oc->bhh_dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_rmep_create_pack(buffer,
                                                           &msg_rmep_create);
            buffer_len = buffer_ptr - buffer;

            /* Send BHH Session Update message to uC */
            rv = _bcm_fp_oam_msg_send_receive(unit,
                    MOS_MSG_CLASS_BHH, 
                    MOS_MSG_SUBCLASS_BHH_RMEP_CREATE,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_RMEP_CREATE_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv) || (reply_len != 0)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Endpoint create " 
                                    "(EP=%d) - %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (BCM_E_INTERNAL);
            }
    } else {
        /* Resolve given endpoint gport value to SGLP and DGLP values. */
        rv = _bcm_fp_oam_endpoint_gport_resolve(unit, endpoint_info,
                                                &sglp, &dglp, &dgpp,
                                                &trunk_id, &trunk_member);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Gport resolve (EP=%d) - %s.\n"),
                       unit, endpoint_info->id, bcm_errmsg(rv)));
            _BCM_OAM_UNLOCK(oc);
            /* Return ID back to free MEP ID pool.*/
            rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
            return (rv);
        }

        /* Clear the hash data element contents before storing the values. */
        _BHH_FP_OAM_HASH_DATA_CLEAR(hash_data);
        hash_data->type = endpoint_info->type;
        hash_data->ep_id = endpoint_info->id;
        hash_data->group_index = endpoint_info->group;
        hash_data->level = endpoint_info->level;
        hash_data->vlan = endpoint_info->vlan;
        hash_data->inner_vlan = endpoint_info->inner_vlan;
        hash_data->outer_tpid = endpoint_info->outer_tpid;
        hash_data->inner_tpid = endpoint_info->inner_tpid;
        hash_data->vlan_pri = endpoint_info->pkt_pri;
        hash_data->inner_vlan_pri = endpoint_info->inner_pkt_pri;
        hash_data->gport = endpoint_info->gport;
        hash_data->sglp = sglp;
        hash_data->dglp = dglp;
        hash_data->flags = endpoint_info->flags;
        hash_data->flags2 = endpoint_info->flags2;
        hash_data->int_pri = endpoint_info->int_pri;
        hash_data->period = endpoint_info->ccm_period;
        hash_data->vccv_type = endpoint_info->vccv_type;
        hash_data->vpn  = endpoint_info->vpn;
        hash_data->name      = endpoint_info->name;
        hash_data->egress_if = endpoint_info->intf_id;
        hash_data->label     = endpoint_info->mpls_label;
        hash_data->egr_label     = endpoint_info->egress_label.label;
        hash_data->egr_label_exp = endpoint_info->egress_label.exp;
        hash_data->egr_label_ttl = endpoint_info->egress_label.ttl;
        hash_data->trunk_index = endpoint_info->trunk_index;
        hash_data->hw_session_id = endpoint_info->session_id;
        hash_data->hw_session_num_entries = endpoint_info->session_num_entries;
        
        sal_memcpy(hash_data->dst_mac_address, endpoint_info->dst_mac_address, 
                   _BHH_MAC_ADDR_LENGTH);
        sal_memcpy(hash_data->src_mac_address, endpoint_info->src_mac_address, 
                   _BHH_MAC_ADDR_LENGTH);
        hash_data->trunk_id = trunk_id;
        
        hash_data->pm_profile_attached = _BCM_OAM_INVALID_INDEX;
        hash_data->in_use = 1;
        group_p->lmep_id = endpoint_info->id;
       
        if (BCM_TRUNK_INVALID != trunk_id) {
            endpoint_info->gport = trunk_member.gport;
            hash_data->resolved_trunk_gport = trunk_member.gport;
        }
        /* Get the Trunk, Port and Modid info for this Gport */
        rv = _bcm_esw_gport_resolve(unit,
                endpoint_info->gport, &module_id, &port_id, 
                &trunk_id, &local_id);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "BHH(unit %d) Error: Gport resolve (EP=%d) -"
                                  " %s.\n"),
                       unit, endpoint_info->id, bcm_errmsg(rv)));
            /* Return ID back to free MEP ID pool.*/
            rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        /* Get local port used for TX BHH packet */
        rv = _bcm_esw_modid_is_local(unit, module_id, &is_local);
        if(BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "BHH(unit %d) Error: local port for "
                                  "BHH TX failed(ep=%d) - %s.\n"),
                       unit, endpoint_info->id, bcm_errmsg(rv)));
            /* Return ID back to free MEP ID pool.*/
            rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }

        if (!is_local) {    /* HG port */
            rv = bcm_esw_stk_modport_get(unit, module_id, &port_id);
            if(BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "BHH(unit %d) Error: HG port get failed "
                                      "(EP=%d) - %s.\n"),
                           unit, endpoint_info->id, bcm_errmsg(rv)));
                /* Return ID back to free MEP ID pool.*/
                rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
        /* Set Endpoint config entry */
        hash_data->uc_session_id = bhh_pool_ep_idx;

        sal_memset(&counter_info, 0, sizeof(counter_info));
        counter_info.counter_size = 
                                  endpoint_info->ccm_tx_update_lm_counter_size;
        hash_data->ccm_tx_counter_size = 
                                  endpoint_info->ccm_tx_update_lm_counter_size;
        for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
        {
            counter_info.counter_base_id[i] = 
                             endpoint_info->ccm_tx_update_lm_counter_base_id[i];
            hash_data->ccm_tx_update_lm_counter_base_id[i] = 
                       endpoint_info->ccm_tx_update_lm_counter_base_id[i];
            counter_info.counter_offset[i] = 
                             endpoint_info->ccm_tx_update_lm_counter_offset[i];
            hash_data->ccm_tx_update_lm_counter_offset[i] =
                             endpoint_info->ccm_tx_update_lm_counter_offset[i]; 
            counter_info.counter_action[i] = bcmOamCounterActionIncrement;
            counter_info.byte_count_offset[i] = 0;
        }
        counter_info.ts_mode = BCM_PKT_TIMESTAMP_MODE_NONE;
        counter_info.oam_replacement_offset = SHR_BHH_LM_OFFSET_INSIDE_CCM_PDU;

        session_flags = (is_replace) ? 0 : SHR_BHH_SESS_SET_F_CREATE;
        if(!is_replace) {

            bhh_sdk_msg_ctrl_sess_set_t msg_sess;
            uint16 encap_length = 0;

            sal_memset(&msg_sess, 0, sizeof(msg_sess));
            /*
             * Set the BHH encapsulation data
             *
             * The function _bcm_fp_oam_bhh_encap_create() is called first
             * since this sets some fields in 'hash_data' which are
             * used in the message.
             */
            rv = _bcm_fp_oam_bhh_encap_create(unit,
                    module_id,
                    port_id, 
                    hash_data,
                    msg_sess.encap_data,
                    &msg_sess.encap_type,
                    &encap_length,
                    &counter_info, hash_data->int_pri,
                    hash_data->egr_label_exp,
                    hash_data->egr_label_ttl);

            if(BCM_FAILURE(rv))
            {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Encap create failed "
                                    "(EP=%d) - %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                /* Return ID back to free MEP ID pool.*/
                rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }

            rv = _bcm_fp_oam_l2_header_get(unit,    hash_data,
                                           port_id, SHR_BHH_L2_ETYPE_MPLS_UCAST,
                                           &l2_hdr, &lm_pdu_counter_offset);
            if (BCM_FAILURE(rv)) {
                _BCM_OAM_UNLOCK(oc);
                return rv;
            }
            session_flags |= SHR_BHH_SESS_SET_F_CCM;

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)
                session_flags |= SHR_BHH_SESS_SET_F_MIP;

            if (group_p->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) {
                session_flags |= SHR_BHH_SESS_SET_F_RDI;
            }

            msg_sess.encap_length = encap_length;
            msg_sess.sess_id = bhh_pool_ep_idx;
            msg_sess.flags   = session_flags;

            msg_sess.mel     = hash_data->level;
            msg_sess.mep_id  = hash_data->name;
            sal_memcpy(msg_sess.meg_id, group_p->name, 
                       BCM_OAM_GROUP_NAME_LENGTH);
            msg_sess.period = endpoint_info->ccm_period;
            msg_sess.if_num  = endpoint_info->intf_id;
            msg_sess.tx_port = port_id;
            msg_sess.num_session_entries = endpoint_info->session_num_entries;
            _bcm_esw_stat_get_counter_id_info(unit, endpoint_info->session_id,
                    &stat_group, &stat_object,
                    &stat_mode, &stat_pool_number,
                    &base_index);
            msg_sess.lm_counter_index = SOC_OLP_TX_CTR(stat_pool_number,
                                                          base_index);

            if (!BHH_EP_MPLS_SECTION_TYPE(endpoint_info)) {
                msg_sess.mpls_label = endpoint_info->mpls_label;
            } else {
                msg_sess.mpls_label = SHR_BHH_MPLS_GAL_LABEL;
            }

            if (0 != l2_hdr.vlan_tag_inner.tpid) {
                msg_sess.inner_vlan = l2_hdr.vlan_tag_inner.tci.vid;
            }

            if (0 != l2_hdr.vlan_tag.tpid) {
                msg_sess.outer_vlan = l2_hdr.vlan_tag.tci.vid;
            }

            switch (endpoint_info->type) {
            case bcmOAMEndpointTypeBHHMPLS:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_LSP;
                msg_sess.priority   = endpoint_info->mpls_exp;
                break;

            case bcmOAMEndpointTypeBHHMPLSVccv:
                switch(endpoint_info->vccv_type) {

                case bcmOamBhhVccvChannelAch:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_1;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                case bcmOamBhhVccvRouterAlert:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_2;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                case bcmOamBhhVccvTtl:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_3;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                case bcmOamBhhVccvGal13:
                    msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PW_VCCV_4;
                    msg_sess.priority   = endpoint_info->mpls_exp;
                    break;

                default:
                    return BCM_E_PARAM;
                    break;
                }
                break;

            case bcmOAMEndpointTypeBhhSection:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION;
                msg_sess.priority   = 0;
                break;
            case bcmOAMEndpointTypeBhhSectionInnervlan:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION_INNERVLAN;
                msg_sess.priority   = endpoint_info->inner_pkt_pri;
                break;
            case bcmOAMEndpointTypeBhhSectionOuterVlan:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION_OUTERVLAN;
                msg_sess.priority   = endpoint_info->pkt_pri;
                break;
            case bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan:
                msg_sess.endpoint_type = _SHR_BHH_EP_TYPE_PORT_SECTION_OUTER_PLUS_INNERVLAN;
                msg_sess.priority   = endpoint_info->pkt_pri;
                break;

            default:
                return BCM_E_PARAM;
            }

            /* Pack control message data into DMA buffer */
            buffer     = oc->bhh_dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_sess_set_pack(buffer, &msg_sess);
            buffer_len = buffer_ptr - buffer;

            /* Send BHH Session Update message to uC */
            rv = _bcm_fp_oam_msg_send_receive(unit,
                    MOS_MSG_CLASS_BHH, 
                    MOS_MSG_SUBCLASS_BHH_SESS_SET,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_SESS_SET_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv) || (reply_len != 0)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Session SET failed "
                                    "(EP=%d) - %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                /* Return ID back to free MEP ID pool.*/
                rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }

            /* hash collision */
            rv = shr_htb_insert(oc->bhh_mep_htbl, hash_key, hash_data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Hash table insert "
                                    "failed EP=%d %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                /* Return ID back to free MEP ID pool.*/
                rv = shr_idxres_list_free(oc->bhh_pool, bhh_pool_ep_idx);
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }

            hash_data->in_use = 1;
        } else {
            bhh_sdk_msg_session_update_t msg_sess;
            uint8   encap_type;/* dummy*/

            sal_memset(&msg_sess, 0, sizeof(msg_sess));
            msg_sess.sess_id = bhh_pool_ep_idx;
            msg_sess.rdi = (group_p->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX);
            msg_sess.msg_type = BHH_CCM_MSG;

            rv = _bcm_fp_oam_bhh_encap_create(unit,
                    module_id,
                    port_id, 
                    hash_data,
                    msg_sess.encap_data,
                    &encap_type,
                    &msg_sess.encap_length,
                    &counter_info, hash_data->int_pri,
                    hash_data->egr_label_exp,
                    hash_data->egr_label_ttl);

            if(BCM_FAILURE(rv))
            {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Encap create failed "
                                    "(EP=%d) - %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
            /* Pack control message data into DMA buffer */
            buffer     = oc->bhh_dma_buffer;
            buffer_ptr = bhh_sdk_msg_ctrl_session_update_pack(buffer, &msg_sess);
            buffer_len = buffer_ptr - buffer;


            /* Send BHH Session Update message to uC */
            rv = _bcm_fp_oam_msg_send_receive(unit,
                    MOS_MSG_CLASS_BHH, 
                    MOS_MSG_SUBCLASS_BHH_SESS_UPDATE,
                    buffer_len, 0,
                    MOS_MSG_SUBCLASS_BHH_SESS_UPDATE_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv) || (reply_len != 0)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "BHH(unit %d) Error: Session Update failed "
                                    "(EP=%d) - %s.\n"),
                         unit, endpoint_info->id, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }
        }
    } 
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *      _bcm_fp_oam_bhh_endpoint_get
 * Purpose:
 *      Get an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (OUT) Pointer to OAM endpoint information buffer.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_endpoint_get(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    _bhh_fp_oam_ep_data_t *h_data_p;     /* Pointer to endpoint hash data.    */
    int                  rv = BCM_E_NONE;/* Operation return status.          */
    _bcm_fp_oam_control_t   *oc;         /* Pointer to OAM control structure. */
    bhh_sdk_msg_ctrl_sess_get_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0, i = 0;
    bcm_l3_egress_t l3_egress;
    bcm_l3_intf_t l3_intf;
    

    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));


    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(endpoint_info->id);
    h_data_p =  &oc->bhh_ep_data[sess_id];

    if (0 == h_data_p->in_use) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: h_data_p not in use EP=%d\n"),
                 unit, endpoint_info->id));
        return (BCM_E_NOT_FOUND);
    }

    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_SESS_GET,
            sess_id, 0,
            MOS_MSG_SUBCLASS_BHH_SESS_GET_REPLY,
            &reply_len);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: ukernel msg failed for"
                            " EP=%d %s.\n"), unit, endpoint_info->id, 
                 bcm_errmsg(rv)));
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_sess_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    if (reply_len != buffer_len) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: ukernel msg failed for"
                            " EP=%d %s.\n"), unit, endpoint_info->id, 
                 bcm_errmsg(rv)));
        return (rv);
    } else {
        endpoint_info->int_pri = msg.tx_cos;
        endpoint_info->pkt_pri = msg.tx_pri;
        endpoint_info->mpls_exp = msg.priority;

        if(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE){
            endpoint_info->name       = msg.remote_mep_id;
            endpoint_info->ccm_period = msg.remote_period;
        } else {
            endpoint_info->name       = msg.mep_id;
            endpoint_info->ccm_period = msg.local_period;
        }

        if(msg.fault_flags & BHH_BTE_EVENT_CCM_TIMEOUT){
            endpoint_info->faults |= BCM_OAM_BHH_FAULT_CCM_TIMEOUT;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_RDI){
            endpoint_info->faults |= BCM_OAM_BHH_FAULT_CCM_RDI;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL){
            endpoint_info->faults |= 
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID){
            endpoint_info->faults |= 
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID){
            endpoint_info->faults |= 
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD){
            endpoint_info->faults |= 
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY){
            endpoint_info->faults |= 
                BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY;
        }  
        if(msg.fault_flags & BHH_BTE_EVENT_CSF_LOS) {
            endpoint_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_LOS;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CSF_FDI) {
            endpoint_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_FDI;
        }
        if(msg.fault_flags & BHH_BTE_EVENT_CSF_RDI) {
            endpoint_info->faults |= BCM_OAM_ENDPOINT_BHH_FAULT_CSF_RDI;
        }
    }

    endpoint_info->intf_id    = h_data_p->egress_if;
    endpoint_info->mpls_label = h_data_p->label;
    endpoint_info->gport      = h_data_p->gport;
    endpoint_info->vpn        = h_data_p->vpn;
    endpoint_info->trunk_index = h_data_p->trunk_index;
    if(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE){
        endpoint_info->name = msg.remote_mep_id;
        endpoint_info->local_id = endpoint_info->id;
    }
    if(!BHH_EP_MPLS_SECTION_TYPE(h_data_p)){
        /*
         * Get MAC address
         */
        bcm_l3_egress_t_init(&l3_egress);
        bcm_l3_intf_t_init(&l3_intf);

        if (BCM_FAILURE
                (bcm_esw_l3_egress_get(unit, h_data_p->egress_if, &l3_egress))) {
            return (BCM_E_INTERNAL);
        }

        l3_intf.l3a_intf_id = l3_egress.intf;
        if (BCM_FAILURE(bcm_esw_l3_intf_get(unit, &l3_intf))) {
            return (BCM_E_INTERNAL);
        }

        sal_memcpy(endpoint_info->src_mac_address, l3_intf.l3a_mac_addr, 
                _BHH_MAC_ADDR_LENGTH);
    }else {
        sal_memcpy(endpoint_info->src_mac_address, h_data_p->src_mac_address,
                _BHH_MAC_ADDR_LENGTH);
    }



    endpoint_info->group = h_data_p->group_index;

    endpoint_info->vlan = h_data_p->vlan;
    endpoint_info->inner_vlan = h_data_p->inner_vlan;
    endpoint_info->level = h_data_p->level;
    endpoint_info->gport = h_data_p->gport;
    endpoint_info->flags |= h_data_p->flags;
    endpoint_info->flags &= ~(BCM_OAM_ENDPOINT_WITH_ID);
    endpoint_info->flags2 = h_data_p->flags2;
    endpoint_info->type = h_data_p->type;
    endpoint_info->vccv_type  = h_data_p->vccv_type;
    endpoint_info->outer_tpid = h_data_p->outer_tpid;
    endpoint_info->inner_tpid = h_data_p->inner_tpid;
    endpoint_info->egress_label.label = h_data_p->egr_label;
    endpoint_info->egress_label.exp = h_data_p->egr_label_exp;
    endpoint_info->egress_label.ttl = h_data_p->egr_label_ttl;
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        endpoint_info->ccm_tx_update_lm_counter_base_id[i] = 
                        h_data_p->ccm_tx_update_lm_counter_base_id[i];
        endpoint_info->ccm_tx_update_lm_counter_offset[i] = 
                        h_data_p->ccm_tx_update_lm_counter_offset[i];
        endpoint_info->ccm_tx_update_lm_counter_action[i] = 
                        bcmOamCounterActionIncrement; 
    }
    
    endpoint_info->session_id = h_data_p->hw_session_id;
    
    endpoint_info->session_num_entries = h_data_p->hw_session_num_entries;
    
    endpoint_info->ccm_tx_update_lm_counter_size = 
        h_data_p->ccm_tx_counter_size;
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_fp_oam_bhh_endpoint_destroy
 * Purpose:
 *     Delete an endpoint and free all its allocated resources.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     ep_id  - (IN) Endpoint ID value.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_bhh_endpoint_destroy(int unit,
                              bcm_oam_endpoint_t ep_id)
{
    _bcm_fp_oam_control_t    *oc;        /* Pointer to OAM control structure. */
    _bhh_fp_oam_ep_data_t    *h_data_p;  /* Pointer to endpoint data.         */
    _bhh_fp_oam_hash_key_t   hash_key;   /* Hash key buffer for lookup.       */
    bcm_oam_endpoint_info_t ep_info;    /* Endpoint information.             */
    _bhh_fp_oam_ep_data_t    h_data;     /* Pointer to endpoint data.         */
    _bhh_fp_oam_group_data_t *group_p;   /* Pointer to group data.       */
    uint16 reply_len;
    bcm_oam_endpoint_t bhh_pool_ep_id;
    int                  rv = BCM_E_NONE;/* Operation return status.          */
    int bhh_group_idx = 0;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    bhh_pool_ep_id = BCM_OAM_BHH_GET_UKERNEL_EP(ep_id);
    /* Get the hash data pointer. */
    h_data_p = &oc->bhh_ep_data[bhh_pool_ep_id];
    if(0 == h_data_p->in_use){
        return BCM_E_NOT_FOUND; 
    } 
    bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(h_data_p->group_index);
    group_p = &(oc->bhh_group_data[bhh_group_idx]);
    if (h_data_p->is_remote) {
        /*
         * BHH uses same index for local and remote.  
         * So, delete always goes through
         * local endpoint destory
         */
        return (BCM_E_NONE);
    } else {

        if (!SOC_WARM_BOOT(unit)) {
            /*Send BHH Session Delete message to uC.Error response is not an
              during switch re-init as uKernel is reloaded and its state is
              cleared before oam init. As such endpoint wont exist during 
              oam init*/
            rv = _bcm_fp_oam_msg_send_receive(unit,
                    MOS_MSG_CLASS_BHH, 
                    MOS_MSG_SUBCLASS_BHH_SESS_DELETE,
                    (int)bhh_pool_ep_id, 0,
                    MOS_MSG_SUBCLASS_BHH_SESS_DELETE_REPLY,
                    &reply_len);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: ukernel msg "
                                    "failed forEP=%d %s.\n"), unit, ep_id,
                         bcm_errmsg(rv)));
                return (rv);
            }

        }

        h_data_p->in_use = 0;

        /* Return ID back to free MEP ID pool.*/
        BCM_IF_ERROR_RETURN(shr_idxres_list_free(oc->bhh_pool, 
                    bhh_pool_ep_id));

        /* Set up endpoint information for key construction. */
        ep_info.group = h_data_p->group_index;
        ep_info.name = h_data_p->name;
        ep_info.gport = h_data_p->gport;
        ep_info.level = h_data_p->level;
        ep_info.vlan = h_data_p->vlan;
        ep_info.inner_vlan = h_data_p->inner_vlan;
        ep_info.flags = h_data_p->flags;
        ep_info.mpls_label = h_data_p->label;
        ep_info.type       = h_data_p->type;

        /* Construct hash key for lookup + delete operation. */
        _bhh_fp_oam_ep_hash_key_construct(unit, oc, &ep_info, &hash_key);

        /* Remove entry from hash table. */
        BCM_IF_ERROR_RETURN(shr_htb_find(oc->bhh_mep_htbl, hash_key,
                    (shr_htb_data_t *)&h_data,
                    1));

        /* Clear the hash data memory previously occupied by this endpoint*/
        _BHH_FP_OAM_HASH_DATA_CLEAR(h_data_p);
 
        group_p->lmep_id = _BHH_FP_OAM_INVALID_LMEP_ID;

    }
    
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_group_destroy
 * Purpose:
 *     Delete a BHH group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_id - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_group_destroy(int unit, bcm_oam_group_t group_id)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bhh_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    int bhh_group_idx = 0;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group_id);
    /* Get Handle to Local DB */
    group_p = &(oc->bhh_group_data[bhh_group_idx]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    if (group_p->lmep_id != _BHH_FP_OAM_INVALID_LMEP_ID) {
        rv = _bcm_fp_oam_bhh_endpoint_destroy(unit, group_p->lmep_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: EP destroy failed for " 
                                "(EP=%d) - %s.\n"), unit, 
                     group_p->lmep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }
    sal_memset(group_p, 0, sizeof(_bhh_fp_oam_group_data_t));
    group_p->lmep_id = _BHH_FP_OAM_INVALID_LMEP_ID;
    /* Return Group ID back to free group pool. */
    rv = shr_idxres_list_free(oc->bhh_group_pool, bhh_group_idx);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Free of group "
                            "index failed for group=%d %s.\n"), unit,
                 group_id,
                 bcm_errmsg(rv)));

        return (rv);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_endpoint_destroy_all
 * Purpose:
 *     Delete all endpointans associated with group and free all its allocated
 *     resources.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     ep_id  - (IN) Endpoint ID value.
 * Retruns:
 *     BCM_E_XXX
 */

int
_bcm_fp_oam_bhh_endpoint_destroy_all(int unit, bcm_oam_group_t group_id)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bhh_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    int bhh_group_idx = 0;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group_id);
    /* Get Handle to Local DB */
    group_p = &(oc->bhh_group_data[bhh_group_idx]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    if (group_p->lmep_id != _BHH_FP_OAM_INVALID_LMEP_ID) {
        rv = _bcm_fp_oam_bhh_endpoint_destroy(unit, group_p->lmep_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: EP destroy failed for " 
                                "(EP=%d) - %s.\n"), unit, 
                     group_p->lmep_id, bcm_errmsg(rv)));
            return rv;
        }
    }
    group_p->lmep_id = _BHH_FP_OAM_INVALID_LMEP_ID;

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_endpoint_traverse
 * Purpose:
 *     Traverse the entire set of BHH endpoints associated with group,
 *     calling a specified callback for each one
 * Parameters:
 *     unit  - (IN) BCM device number
 *     group     - (IN) Group ID of endpoints associated.
 *     cb        - (IN) Pointer to call back function.
 *     user_data - (IN) Pointer to user supplied data.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_endpoint_traverse(int unit, bcm_oam_group_t group,
                           bcm_oam_endpoint_traverse_cb cb,
                           void *user_data)
{
    _bcm_fp_oam_control_t    *oc;        /* OAM control structure pointer. */
    bcm_oam_endpoint_info_t endpoint_info; /* Group information to be set.   */
    _bhh_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    int bhh_group_idx = 0;

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Get device OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    bhh_group_idx = BCM_OAM_BHH_GET_UKERNEL_GROUP(group);
    /* Get Handle to Local DB */
    group_p = &(oc->bhh_group_data[bhh_group_idx]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }
    if (group_p->lmep_id != _BHH_FP_OAM_INVALID_LMEP_ID) {
        sal_memset(&endpoint_info, 0, sizeof(endpoint_info));
        endpoint_info.id = group_p->lmep_id;
        rv = _bcm_fp_oam_bhh_endpoint_get(unit, &endpoint_info);

        /* Call the user call back routine with group information. */
        rv = cb(unit, &endpoint_info, user_data);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: User call back " 
                                "routine (EP=%d) - %s.\n"), unit, 
                     endpoint_info.id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    return BCM_E_NONE;
}

/* BHH Event Handler */
typedef struct _event_handler_s {
    struct _event_handler_s *next;
    bcm_oam_event_types_t event_types;
    bcm_oam_event_cb cb;
    void *user_data;
} _event_handler_t;

/*
 * Function:
 *      _bcm_fp_oam_bhh_event_mask_set
 * Purpose:
 *      Set the BHH Events mask.
 *      Events are set per BHH module.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_fp_oam_bhh_event_mask_set(int unit)
{
    _bcm_fp_oam_control_t *oc;
    _bcm_oam_event_handler_t *event_handler_p;
    uint32 event_mask = 0;
    uint16 reply_len;
    int rv = BCM_E_NONE;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Get event mask from all callbacks */
    for (event_handler_p = oc->event_handler_list_p;
         event_handler_p != NULL;
         event_handler_p = event_handler_p->next_p) {

        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHLBTimeout)) {
            event_mask |= BHH_BTE_EVENT_LB_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHLBDiscoveryUpdate)) {
            event_mask |= BHH_BTE_EVENT_LB_DISCOVERY_UPDATE;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMTimeout)) {
            event_mask |= BHH_BTE_EVENT_CCM_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMTimeoutClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_TIMEOUT_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMState)) {
            event_mask |= BHH_BTE_EVENT_STATE;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMRdi)) {
            event_mask |= BHH_BTE_EVENT_CCM_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegLevel)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegId)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMepId)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPeriod)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPriority)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMRdiClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_RDI_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegLevelClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_LEVEL_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMegIdClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEG_ID_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownMepIdClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_MEP_ID_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPeriodClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PERIOD_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventBHHCCMUnknownPriorityClear)) {
            event_mask |= BHH_BTE_EVENT_CCM_UNKNOWN_PRIORITY_CLEAR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                    bcmOAMEventBhhPmCounterRollover)) {
            event_mask |= BHH_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfLos)) {
            event_mask |= BHH_BTE_EVENT_CSF_LOS;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfFdi)) {
            event_mask |= BHH_BTE_EVENT_CSF_FDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfRdi)) {
            event_mask |= BHH_BTE_EVENT_CSF_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w, bcmOAMEventCsfDci)) {
            event_mask |= BHH_BTE_EVENT_CSF_DCI;
        }
    }

    /* Update BHH event mask in uKernel */
    if (event_mask != oc->bhh_event_mask) {
        /* Send BHH Event Mask message to uC */
        rv = _bcm_fp_oam_msg_send_receive
                (unit,
                 MOS_MSG_CLASS_BHH, 
                 MOS_MSG_SUBCLASS_BHH_EVENT_MASK_SET,
                 0, event_mask,
                 MOS_MSG_SUBCLASS_BHH_EVENT_MASK_SET_REPLY,
                 &reply_len);

        if(BCM_SUCCESS(rv) && (reply_len != 0)) {
            rv = BCM_E_INTERNAL;
        }
    }

    oc->bhh_event_mask = event_mask;

    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_loopback_add
 * Purpose:
 *     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_loopback_add(int unit, bcm_oam_loopback_t *loopback_p)
{
    _bcm_fp_oam_control_t *oc;
    _bhh_fp_oam_ep_data_t *h_data_p;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_loopback_add_t msg;
    uint8 *buffer, *buffer_ptr, encap_type;
    uint16 buffer_len, reply_len;
    uint32 flags = 0;
    int sess_id = 0, i = 0;
    _bhh_oam_lm_dm_info_t counter_info;
    bcm_module_t         module_id;
    bcm_port_t           port_id;
    bcm_trunk_t          trunk_id;
    int                  local_id;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    BCM_OAM_BHH_VALIDATE_EP(loopback_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loopback_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /*Validate LB period */
    rv = _bcm_fp_oam_period_validate(loopback_p->period);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid period : %d\n"),
                   unit, loopback_p->period));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->bhh_ep_data[sess_id];

    h_data_p->lb_tx_counter_size = loopback_p->lb_tx_update_lm_counter_size;  
    sal_memset(&counter_info, 0, sizeof(counter_info));
    counter_info.counter_size = loopback_p->lb_tx_update_lm_counter_size; 
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        h_data_p->lb_tx_update_lm_counter_base_id[i] =
        loopback_p->lb_tx_update_lm_counter_base_id[i];
        h_data_p->lb_tx_update_lm_counter_offset[i] =
        loopback_p->lb_tx_update_lm_counter_offset[i];
        counter_info.counter_base_id[i] = 
        loopback_p->lb_tx_update_lm_counter_base_id[i];
        counter_info.counter_offset[i] = 
        loopback_p->lb_tx_update_lm_counter_offset[i];
        counter_info.counter_action[i] = bcmOamCounterActionIncrement;
        counter_info.byte_count_offset[i] = 0;
    }
    counter_info.ts_mode = BCM_PKT_TIMESTAMP_MODE_NONE;

    /*
     * Convert host space flags to uKernel space flags
     */

    if (loopback_p->flags & BCM_OAM_LOOPBACK_TX_ENABLE) {
        flags |= BCM_BHH_TX_ENABLE;
    }

    if (loopback_p->flags & BCM_OAM_BHH_INC_REQUESTING_MEP_TLV) {
        flags |= BCM_BHH_INC_REQUESTING_MEP_TLV;
    }

    if (loopback_p->flags & BCM_OAM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV) {
        flags |= BCM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV;
    } else if (loopback_p->flags & 
            BCM_OAM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV) {
        flags |= BCM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV;
    } else if (loopback_p->flags & BCM_OAM_BHH_LBM_ICC_MEP_TLV) {
        flags |= BCM_BHH_LBM_ICC_MEP_TLV;
    } else if (loopback_p->flags & BCM_OAM_BHH_LBM_ICC_MIP_TLV) {
        flags |= BCM_BHH_LBM_ICC_MIP_TLV;
    } else {
        flags |= BCM_BHH_LBM_ICC_MEP_TLV; /* Default */
    }

    if (loopback_p->flags & BCM_OAM_BHH_LBR_ICC_MEP_TLV) {
        flags |= BCM_BHH_LBR_ICC_MEP_TLV;
    } else if (loopback_p->flags & BCM_OAM_BHH_LBR_ICC_MIP_TLV) {
        flags |= BCM_BHH_LBR_ICC_MIP_TLV;
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags   = flags;
    msg.sess_id = sess_id;
    msg.int_pri = loopback_p->int_pri;
    msg.pkt_pri = loopback_p->pkt_pri;

    /*
     * Set period
     */
    msg.period  = loopback_p->period;

    /*
     * Check TTL
     */
    if (loopback_p->ttl == 0 || loopback_p->ttl > 255) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }
    /* Check for default usage of int_pri */
    if (loopback_p->int_pri == -1) {
        loopback_p->int_pri = 7; /*Use default */
    }
    /* Check for default usage of pkt_pri */
    if (loopback_p->pkt_pri == 0xff) {
        loopback_p->pkt_pri = 7; /*Use default */
    }
    if (BHH_EP_MPLS_SECTION_TYPE(h_data_p) ||
            (h_data_p->type == bcmOAMEndpointTypeBHHMPLSVccv &&
             h_data_p->vccv_type == bcmOamBhhVccvTtl)) {

        msg.ttl = 1; /* Only TTL 1 is valid */
    } else {
        msg.ttl = loopback_p->ttl;
    }
    if (h_data_p->trunk_id == BCM_TRUNK_INVALID) {
        rv = _bcm_esw_gport_resolve(unit,
                h_data_p->gport, &module_id, &port_id, &trunk_id, &local_id);
    } else {
        rv = _bcm_esw_gport_resolve(unit,
                h_data_p->resolved_trunk_gport, &module_id, &port_id, 
                &trunk_id, &local_id);
        
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "BHH(unit %d) Error: Gport resolve - %s.\n"),
                 unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

        
    /*
     * Set the BHH encapsulation data
     *
     * The function _bcm_fp_oam_bhh_encap_create() is called first
     * since this sets some fields in 'hash_data' which are
     * used in the message.
     */
    rv = _bcm_fp_oam_bhh_encap_create(unit,
            module_id,
            port_id, 
            h_data_p,
            msg.encap_data,
            &encap_type,
            &msg.encap_length,
            &counter_info, loopback_p->int_pri, loopback_p->pkt_pri,
            loopback_p->ttl);
    if(BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "BHH(unit %d) Error: Encap create failed "
                            " - %s.\n"),
                 unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_loopback_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_LOOPBACK_ADD,
            buffer_len, 0,
            MOS_MSG_SUBCLASS_BHH_LOOPBACK_ADD_REPLY,
            &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }


    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_loopback_get
 * Purpose:
 *     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_loopback_get(int unit, bcm_oam_loopback_t *loopback_p)
{
    _bcm_fp_oam_control_t *oc;
    int rv = BCM_E_NONE;
    bhh_sdk_msg_ctrl_loopback_get_t msg;
    _bhh_fp_oam_ep_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0, i = 0;
    uint8*  encap_data = NULL;
    soc_olp_l2_hdr_t olp_l2_header;
    soc_olp_tx_hdr_t olp_oam_tx_header;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    BCM_OAM_BHH_VALIDATE_EP(loopback_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loopback_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->bhh_ep_data[sess_id];


    sal_memset(&msg, 0, sizeof(msg));
    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_LOOPBACK_GET,
            sess_id, 0,
            MOS_MSG_SUBCLASS_BHH_LOOPBACK_GET_REPLY,
            &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_loopback_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;


    if (reply_len != buffer_len) {
        rv =  BCM_E_INTERNAL;
    } else {
        /*
         * Convert kernel space flags to host space flags
         */
        if (msg.flags & BCM_BHH_INC_REQUESTING_MEP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_INC_REQUESTING_MEP_TLV;
        }

        if (msg.flags & BCM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_LBM_INGRESS_DISCOVERY_MEP_TLV;
        } else if (msg.flags & BCM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_LBM_EGRESS_DISCOVERY_MEP_TLV;
        } else if (msg.flags & BCM_BHH_LBM_ICC_MEP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_LBM_ICC_MEP_TLV;
        } else if (msg.flags & BCM_BHH_LBM_ICC_MIP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_LBM_ICC_MIP_TLV;
        }

        if (msg.flags & BCM_BHH_LBR_ICC_MEP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_LBR_ICC_MEP_TLV;
        } else if (msg.flags & BCM_BHH_LBR_ICC_MIP_TLV) {
            loopback_p->flags |= BCM_OAM_BHH_LBR_ICC_MIP_TLV;
        }

        if (msg.flags & BCM_BHH_TX_ENABLE) {
            loopback_p->flags |= BCM_OAM_LOOPBACK_TX_ENABLE;
        }

        loopback_p->period                  = msg.period;
        loopback_p->ttl                     = msg.ttl;
        loopback_p->discovered_me.flags     = msg.discovery_flags;
        loopback_p->discovered_me.name      = msg.discovery_id;
        loopback_p->discovered_me.ttl       = msg.discovery_ttl;
        loopback_p->rx_count                = msg.rx_count;
        loopback_p->tx_count                = msg.tx_count;
        loopback_p->drop_count              = msg.drop_count;
        loopback_p->unexpected_response     = msg.unexpected_response;
        loopback_p->out_of_sequence         = msg.out_of_sequence;
        loopback_p->local_mipid_missmatch   = msg.local_mipid_missmatch;
        loopback_p->remote_mipid_missmatch  = msg.remote_mipid_missmatch;
        loopback_p->invalid_target_mep_tlv  = msg.invalid_target_mep_tlv;
        loopback_p->invalid_mep_tlv_subtype = msg.invalid_mep_tlv_subtype;
        loopback_p->invalid_tlv_offset      = msg.invalid_tlv_offset;
        for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
        {
            loopback_p->lb_tx_update_lm_counter_base_id[i] =
                h_data_p->lb_tx_update_lm_counter_base_id[i]; 
            loopback_p->lb_tx_update_lm_counter_offset[i] =
                h_data_p->lb_tx_update_lm_counter_offset[i]; 
        }
        loopback_p->lb_tx_update_lm_counter_size = h_data_p->lb_tx_counter_size;

        encap_data = msg.encap_data;
        encap_data = _bcm_fp_oam_olp_l2_header_unpack(encap_data,
                                                      &olp_l2_header);
        encap_data = shr_olp_tx_header_unpack(encap_data,
                                              &olp_oam_tx_header);

        loopback_p->int_pri = SOC_OLP_TX_PRI_GET(&olp_oam_tx_header);

        loopback_p->pkt_pri =
                   bcm_fp_oam_bhh_get_oam_exp_from_encap(h_data_p,
                                                         msg.encap_data,
                                                         msg.encap_length);
        rv = BCM_E_NONE;
    }


    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_loopback_delete
 * Purpose:
 *     
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_loopback_delete(int unit, bcm_oam_loopback_t *loopback_p)
{
    _bcm_fp_oam_control_t *oc;
    int rv = BCM_E_NONE;
    shr_bhh_msg_ctrl_loopback_delete_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;
    _bhh_fp_oam_ep_data_t *h_data_p;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    BCM_OAM_BHH_VALIDATE_EP(loopback_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loopback_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, loopback_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->bhh_ep_data[sess_id];


    msg.sess_id = h_data_p->uc_session_id;

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = shr_bhh_msg_ctrl_loopback_delete_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_LOOPBACK_DELETE,
            buffer_len, 0,
            MOS_MSG_SUBCLASS_BHH_LOOPBACK_DELETE_REPLY,
            &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0))
        rv =  BCM_E_INTERNAL;


    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_loss_add
 * Purpose:
 *        Loss Measurement add for BHH endpoints.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
_bcm_fp_oam_bhh_loss_add(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_fp_oam_control_t *oc;
    _bhh_fp_oam_ep_data_t *h_data_p;
    int rv = BCM_E_NONE;
    uint8 *buffer, *buffer_ptr, encap_type;
    uint16 buffer_len, reply_len;
    uint32 flags = 0;
    int sess_id = 0, i = 0;
    _bhh_oam_lm_dm_info_t counter_info;
    bcm_module_t         module_id;
    bcm_port_t           port_id;
    bcm_trunk_t          trunk_id;
    int                  local_id;
    
    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    /* Validate the index passed by user is within range */
    BCM_OAM_BHH_VALIDATE_EP(loss_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loss_p->id);
    h_data_p = &oc->bhh_ep_data[sess_id];

    if (!(h_data_p->in_use)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "Endpoint EP=%d not configured.\n"), unit, loss_p->id));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }
    /*Validate LM period */
    rv = _bcm_fp_oam_period_validate(loss_p->period);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid period : %d\n"),
                   unit, loss_p->period));
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    h_data_p->lm_counter_size = loss_p->lm_counter_size;  
    sal_memset(&counter_info, 0, sizeof(counter_info));
    counter_info.counter_size = loss_p->lm_counter_size;
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        h_data_p->lm_counter_base_id[i] = loss_p->lm_counter_base_id[i];
        h_data_p->lm_counter_offset[i] = loss_p->lm_counter_offset[i];
        h_data_p->lm_counter_action[i] = loss_p->lm_counter_action[i];
        counter_info.counter_base_id[i] = loss_p->lm_counter_base_id[i];
        counter_info.counter_offset[i] = loss_p->lm_counter_offset[i];
        counter_info.counter_action[i] = loss_p->lm_counter_action[i];
        counter_info.byte_count_offset[i] = 0;
    }
    counter_info.ts_mode = BCM_PKT_TIMESTAMP_MODE_NONE;
    counter_info.oam_replacement_offset = SHR_BHH_HEADER_LENGTH;

    bhh_sdk_msg_ctrl_loss_add_t msg;

    sal_memset(&msg, 0, sizeof(msg));

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
        

    /*
     * Convert host space flags to uKernel space flags
     */
    if (loss_p->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
        flags |= BCM_BHH_LM_SINGLE_ENDED;
    }

    if (loss_p->flags & BCM_OAM_LOSS_TX_ENABLE) {
        flags |= BCM_BHH_LM_TX_ENABLE;
    }

    if (loss_p->flags & BCM_OAM_LOSS_SLM) {
        flags |= BCM_BHH_LM_SLM;
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags   = flags;
    msg.sess_id = sess_id;

    /*
     * Set period
     */
    msg.period  = loss_p->period;
    msg.int_pri = loss_p->int_pri;
    msg.pkt_pri = loss_p->pkt_pri;
    if (h_data_p->trunk_id == BCM_TRUNK_INVALID) {
        rv = _bcm_esw_gport_resolve(unit,
                h_data_p->gport, &module_id, &port_id, &trunk_id, &local_id);
    } else {
        rv = _bcm_esw_gport_resolve(unit,
                h_data_p->resolved_trunk_gport, &module_id, &port_id, 
                &trunk_id, &local_id);
        
    } 
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "BHH(unit %d) Error: Gport resolve - %s.\n"),
                 unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

        
    /*
     * Set the BHH encapsulation data
     *
     * The function _bcm_fp_oam_bhh_encap_create() is called first
     * since this sets some fields in 'hash_data' which are
     * used in the message.
     */
    rv = _bcm_fp_oam_bhh_encap_create(unit,
            module_id,
            port_id, 
            h_data_p,
            msg.encap_data,
            &encap_type,
            &msg.encap_length,
            &counter_info, loss_p->int_pri, loss_p->pkt_pri,
            h_data_p->egr_label_ttl);
    if(BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "BHH(unit %d) Error: Encap create failed "
                            " - %s.\n"),
                 unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_loss_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
                      MOS_MSG_CLASS_BHH, 
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_ADD,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_ADD_REPLY,
                      &reply_len);

    loss_p->pm_id = sess_id;

    if(BCM_SUCCESS(rv) && (reply_len != 0)) 
        rv =  BCM_E_INTERNAL;
    

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_loss_get
 * Purpose:
 *     Loss Measurement get for BHH endpoints
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
_bcm_fp_oam_bhh_loss_get(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_fp_oam_control_t *oc;
    int rv = BCM_E_NONE;
    _bhh_fp_oam_ep_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0, i = 0;
    bhh_sdk_msg_ctrl_loss_get_t msg;
    uint8*  encap_data = NULL;
    soc_olp_l2_hdr_t olp_l2_header;
    soc_olp_tx_hdr_t olp_oam_tx_header;


    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    BCM_OAM_BHH_VALIDATE_EP(loss_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loss_p->id);
    h_data_p = &oc->bhh_ep_data[sess_id];
    if (!(h_data_p->in_use)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "Endpoint EP=%d %s.\n"), unit, loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }


    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    sal_memset(&msg, 0, sizeof(msg));
    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_GET,
            sess_id, 0,
            MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_GET_REPLY,
            &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_loss_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    if (reply_len != buffer_len) {
        rv =  BCM_E_INTERNAL;
    } else {
        /*
         * Convert kernel space flags to host space flags
         */
        if (msg.flags & BCM_BHH_LM_SINGLE_ENDED) {
            loss_p->flags |= BCM_OAM_LOSS_SINGLE_ENDED;
        }

        if (msg.flags & BCM_BHH_LM_TX_ENABLE) {
            loss_p->flags |= BCM_OAM_LOSS_TX_ENABLE;
        }

        if (msg.flags & BCM_BHH_LM_SLM) {
            loss_p->flags |= BCM_OAM_LOSS_SLM;
        }

        loss_p->period = msg.period;
        loss_p->loss_threshold = msg.loss_threshold;             
        loss_p->loss_nearend = msg.loss_nearend;               
        loss_p->loss_farend = msg.loss_farend;    
        loss_p->tx_nearend = msg.tx_nearend;            
        loss_p->rx_nearend = msg.rx_nearend;              
        loss_p->tx_farend = msg.tx_farend;               
        loss_p->rx_farend = msg.rx_farend;               
        loss_p->rx_oam_packets = msg.rx_oam_packets;          
        loss_p->tx_oam_packets = msg.tx_oam_packets;          
        loss_p->int_pri = msg.int_pri;
        loss_p->pkt_pri = msg.pkt_pri;         
        loss_p->pm_id = sess_id;
        for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
        {
            loss_p->lm_counter_base_id[i] =
                h_data_p->lm_counter_base_id[i]; 
            loss_p->lm_counter_offset[i] =
                h_data_p->lm_counter_offset[i]; 
            loss_p->lm_counter_action[i] =
                h_data_p->lm_counter_action[i]; 
        }
        loss_p->lm_counter_size = h_data_p->lm_counter_size;
        encap_data = msg.encap_data;
        encap_data = _bcm_fp_oam_olp_l2_header_unpack(encap_data, 
                &olp_l2_header);
        encap_data = shr_olp_tx_header_unpack(encap_data,
                &olp_oam_tx_header);
         loss_p->int_pri = SOC_OLP_TX_PRI_GET(&olp_oam_tx_header);

        loss_p->pkt_pri =
                 bcm_fp_oam_bhh_get_oam_exp_from_encap(h_data_p,
                                                       msg.encap_data,
                                                       msg.encap_length);
        rv = BCM_E_NONE;
    }

    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_loss_delete
 * Purpose:
 *    Loss Measurement Delete for BHH endpoints
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
_bcm_fp_oam_bhh_loss_delete(int unit, bcm_oam_loss_t *loss_p)
{
    _bcm_fp_oam_control_t *oc;
    int rv = BCM_E_NONE;
    _bhh_fp_oam_ep_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);
    BCM_OAM_BHH_VALIDATE_EP(loss_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(loss_p->id);
    h_data_p = &oc->bhh_ep_data[sess_id];
    if (!(h_data_p->in_use)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "Endpoint EP=%d %s.\n"), unit, loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }


    shr_bhh_msg_ctrl_loss_delete_t msg;

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, loss_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->bhh_ep_data[loss_p->id];

    msg.sess_id = sess_id;

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = shr_bhh_msg_ctrl_loss_delete_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_DELETE,
            buffer_len, 0,
            MOS_MSG_SUBCLASS_BHH_LOSS_MEASUREMENT_DELETE_REPLY,
            &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }


    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_delay_add
 * Purpose:
 *     Delay Measurement add for BHH endpoints.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
_bcm_fp_oam_bhh_delay_add(int unit, bcm_oam_delay_t *delay_p)
{
    _bcm_fp_oam_control_t *oc;
    _bhh_fp_oam_ep_data_t *h_data_p;
    int rv = BCM_E_NONE;
    uint8 *buffer, *buffer_ptr, encap_type;
    uint16 buffer_len, reply_len;
    uint32 flags = 0;
    int sess_id = 0, i = 0;
    _bhh_oam_lm_dm_info_t counter_info;
    bcm_module_t         module_id;
    bcm_port_t           port_id;
    bcm_trunk_t          trunk_id;
    int                  local_id;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);
    BCM_OAM_BHH_VALIDATE_EP(delay_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(delay_p->id);

    h_data_p = &oc->bhh_ep_data[sess_id];
    if (!(h_data_p->in_use)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "Endpoint EP=%d %s.\n"), unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }
    /*Validate LM period */
    rv = _bcm_fp_oam_period_validate(delay_p->period);
        if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid period : %d\n"),
                   unit, delay_p->period));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p->dm_tx_counter_size = delay_p->dm_tx_update_lm_counter_size;
    sal_memset(&counter_info, 0, sizeof(counter_info));
    counter_info.counter_size = delay_p->dm_tx_update_lm_counter_size;
    for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
    {
        h_data_p->dm_tx_update_lm_counter_base_id[i] =
        delay_p->dm_tx_update_lm_counter_base_id[i];
        h_data_p->dm_tx_update_lm_counter_offset[i] =
        delay_p->dm_tx_update_lm_counter_offset[i];
        counter_info.counter_base_id[i] = 
        delay_p->dm_tx_update_lm_counter_base_id[i];
        counter_info.counter_offset[i] = 
        delay_p->dm_tx_update_lm_counter_offset[i];
        counter_info.counter_action[i] = bcmOamCounterActionIncrement;
        counter_info.byte_count_offset[i] = 0;
    }
    counter_info.oam_replacement_offset = SHR_BHH_HEADER_LENGTH;


    bhh_sdk_msg_ctrl_delay_add_t msg;       
    sal_memset(&msg, 0, sizeof(msg));

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    /*
     * Convert host space flags to uKernel space flags
     */
    if (delay_p->flags & BCM_OAM_DELAY_ONE_WAY) {
        flags |= BCM_BHH_DM_ONE_WAY;
    }

    if (delay_p->flags & BCM_OAM_DELAY_TX_ENABLE) {
        flags |= BCM_BHH_DM_TX_ENABLE;
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags   = flags;
    msg.sess_id = sess_id;

    if(delay_p->timestamp_format == bcmOAMTimestampFormatIEEE1588v1) {
        msg.dm_format = BCM_BHH_DM_TYPE_PTP;
        counter_info.ts_mode = BCM_PKT_TIMESTAMP_MODE_PTP;
    } else {
        msg.dm_format = BCM_BHH_DM_TYPE_NTP;
        counter_info.ts_mode = BCM_PKT_TIMESTAMP_MODE_NTP;
    }
    /*
     * Set period
     */
    msg.period  = delay_p->period;

    if (h_data_p->trunk_id == BCM_TRUNK_INVALID) {
        rv = _bcm_esw_gport_resolve(unit,
                h_data_p->gport, &module_id, &port_id, &trunk_id, &local_id);
    } else {
        rv = _bcm_esw_gport_resolve(unit,
                h_data_p->resolved_trunk_gport, &module_id, &port_id, 
                &trunk_id, &local_id);
        
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "BHH(unit %d) Error: Gport resolve - %s.\n"),
                 unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }


    /*
     * Set the BHH encapsulation data
     *
     * The function _bcm_fp_oam_bhh_encap_create() is called first
     * since this sets some fields in 'hash_data' which are
     * used in the message.
     */
    rv = _bcm_fp_oam_bhh_encap_create(unit,
            module_id,
            port_id, 
            h_data_p,
            msg.encap_data,
            &encap_type,
            &msg.encap_length,
            &counter_info, delay_p->int_pri, delay_p->pkt_pri,
            h_data_p->egr_label_ttl);
    if(BCM_FAILURE(rv))
    {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "BHH(unit %d) Error: Encap create failed "
                            " - %s.\n"),
                 unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }
    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_delay_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_ADD,
            buffer_len, 0,
            MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_ADD_REPLY,
            &reply_len);

    delay_p->pm_id = sess_id;

    if(BCM_SUCCESS(rv) && (reply_len != 0)) { 
        rv =  BCM_E_INTERNAL;
    }


    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_delay_get
 * Purpose:
 *     Delay Measurements get for BHH endpoints.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
_bcm_fp_oam_bhh_delay_get(int unit, bcm_oam_delay_t *delay_p)
{
    _bcm_fp_oam_control_t *oc;
    int rv = BCM_E_NONE;
    _bhh_fp_oam_ep_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0, i = 0;
    bhh_sdk_msg_ctrl_delay_get_t msg;
    uint8*  encap_data = NULL;
    soc_olp_l2_hdr_t olp_l2_header;
    soc_olp_tx_hdr_t olp_oam_tx_header;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);
    BCM_OAM_BHH_VALIDATE_EP(delay_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(delay_p->id);

    h_data_p = &oc->bhh_ep_data[sess_id];
    if (!(h_data_p->in_use)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "Endpoint EP=%d %s.\n"), unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    sal_memset(&msg, 0, sizeof(msg));
    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_GET,
            sess_id, 0,
            MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_GET_REPLY,
            &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_delay_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;


    if (reply_len != buffer_len) {
        rv =  BCM_E_INTERNAL;
    } else {
        /*
         * Convert kernel space flags to host space flags
         */
        if (msg.flags & BCM_BHH_DM_ONE_WAY) {
            delay_p->flags |= BCM_OAM_DELAY_ONE_WAY;
        }

        if (msg.flags & BCM_BHH_DM_TX_ENABLE) {
            delay_p->flags |= BCM_OAM_DELAY_TX_ENABLE;
        }

        delay_p->period = msg.period;

        if(msg.dm_format == BCM_BHH_DM_TYPE_PTP)
            delay_p->timestamp_format = bcmOAMTimestampFormatIEEE1588v1;
        else
            delay_p->timestamp_format = bcmOAMTimestampFormatNTP;

        rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->delay), 
                msg.delay_seconds, msg.delay_nanoseconds);
        if(BCM_SUCCESS(rv)) {
            rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->txf), 
                    msg.txf_seconds, msg.txf_nanoseconds);
        }
        if(BCM_SUCCESS(rv)) {
            rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->rxf), 
                    msg.rxf_seconds, msg.rxf_nanoseconds);
        }
        if(BCM_SUCCESS(rv)) {
            rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->txb), 
                    msg.txb_seconds, msg.txb_nanoseconds);
        }
        if(BCM_SUCCESS(rv)) {
            rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->rxb), 
                    msg.rxb_seconds, msg.rxb_nanoseconds);
        }
        delay_p->rx_oam_packets = msg.rx_oam_packets;
        delay_p->tx_oam_packets = msg.tx_oam_packets;
        delay_p->int_pri = msg.int_pri;
        delay_p->pkt_pri = msg.pkt_pri;
        delay_p->pm_id = sess_id;
        for (i=0; i< BCM_OAM_LM_COUNTER_MAX; i++)
        {
            delay_p->dm_tx_update_lm_counter_base_id[i] =
                h_data_p->dm_tx_update_lm_counter_base_id[i]; 
            delay_p->dm_tx_update_lm_counter_offset[i] =
                h_data_p->dm_tx_update_lm_counter_offset[i]; 
        }
        delay_p->dm_tx_update_lm_counter_size = h_data_p->dm_tx_counter_size;
        encap_data = msg.encap_data;
        encap_data = _bcm_fp_oam_olp_l2_header_unpack(encap_data, 
                &olp_l2_header);
        encap_data = shr_olp_tx_header_unpack(encap_data,
                &olp_oam_tx_header);
        delay_p->int_pri = SOC_OLP_TX_PRI_GET (&olp_oam_tx_header);
        delay_p->pkt_pri =
                 bcm_fp_oam_bhh_get_oam_exp_from_encap(h_data_p,
                                                       msg.encap_data,
                                                       msg.encap_length);
    }


    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_delay_delete
 * Purpose:
 *     Delay Measurement Delete 
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
_bcm_fp_oam_bhh_delay_delete(int unit, bcm_oam_delay_t *delay_p)
{
    _bcm_fp_oam_control_t *oc;
    int rv = BCM_E_NONE;
    _bhh_fp_oam_ep_data_t *h_data_p;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int sess_id = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

    BCM_OAM_BHH_VALIDATE_EP(delay_p->id);

    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(delay_p->id);
    h_data_p = &oc->bhh_ep_data[sess_id];

    if (!(h_data_p->in_use)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "Endpoint EP=%d %s.\n"), unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NOT_FOUND;
    }


    shr_bhh_msg_ctrl_delay_delete_t msg;

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                 unit, delay_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    msg.sess_id = sess_id;

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = shr_bhh_msg_ctrl_delay_delete_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
            MOS_MSG_CLASS_BHH, 
            MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_DELETE,
            buffer_len, 0,
            MOS_MSG_SUBCLASS_BHH_DELAY_MEASUREMENT_DELETE_REPLY,
            &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }


    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *      _bcm_fp_oam_csf_add
 * Purpose:
 *      Start CSF PDU transmission
 * Parameters:
 *      unit    - (IN)    Unit number.
 *      csf_ptr - (INOUT) CSF object
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_fp_oam_csf_add(int unit, bcm_oam_csf_t *csf_p)
{
    _bcm_fp_oam_control_t      *oc;
    _bhh_fp_oam_ep_data_t      *h_data_p;
    bhh_sdk_msg_ctrl_csf_add_t  msg;
    uint8                      *buffer, *buffer_ptr;
    uint16                      buffer_len, reply_len;
    _bhh_oam_lm_dm_info_t       counter_info;
    bcm_module_t                module_id;
    bcm_port_t                  port_id;
    bcm_trunk_t                 trunk_id;
    int                         local_id;
    uint8                       encap_type;
    int                         sess_id = 0;
    int                         i;
    int                         rv = BCM_E_NONE;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(csf_p->id);
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(csf_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    if ((csf_p->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1S) &&
        (csf_p->period != BCM_OAM_ENDPOINT_CCM_PERIOD_1M)) {
        /* Only 1S and 1M are supported */
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    h_data_p = &oc->bhh_ep_data[sess_id];
    /*
     * Only BHH is supported
     */
    if (!BHH_EP_TYPE(h_data_p)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    sal_memset(&counter_info, 0, sizeof(counter_info));
    counter_info.counter_size     = csf_p->csf_tx_update_lm_counter_size;
    h_data_p->csf_tx_counter_size = csf_p->csf_tx_update_lm_counter_size;
    for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
        h_data_p->csf_tx_update_lm_counter_base_id[i] =
                                      csf_p->csf_tx_update_lm_counter_base_id[i];
        h_data_p->csf_tx_update_lm_counter_offset[i] =
                                      csf_p->csf_tx_update_lm_counter_offset[i];

        counter_info.counter_base_id[i] =
                                      csf_p->csf_tx_update_lm_counter_base_id[i];
        counter_info.counter_offset[i] =
                                      csf_p->csf_tx_update_lm_counter_offset[i];
        counter_info.counter_action[i] = bcmOamCounterActionIncrement;
    }
    counter_info.ts_mode = BCM_PKT_TIMESTAMP_MODE_NONE;

    sal_memset(&msg, 0, sizeof(msg));
    msg.sess_id = sess_id;
    msg.int_pri = csf_p->int_pri;
    msg.pkt_pri = csf_p->pkt_pri;

    switch(csf_p->type) {
        case BCM_OAM_CSF_LOS:
            msg.type = SHR_CSF_TYPE_LOS;
            break;

        case BCM_OAM_CSF_FDI:
            msg.type = SHR_CSF_TYPE_FDI;
            break;

        case BCM_OAM_CSF_RDI:
            msg.type = SHR_CSF_TYPE_RDI;
            break;

        case BCM_OAM_CSF_DCI:
            msg.type = SHR_CSF_TYPE_DCI;
            break;

        default:
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_PARAM;
    }

    /*
     * Set period
     */
    msg.period  = csf_p->period;

    if (h_data_p->trunk_id == BCM_TRUNK_INVALID) {
        rv = _bcm_esw_gport_resolve(unit, h_data_p->gport,
                                    &module_id, &port_id,
                                    &trunk_id, &local_id);
    } else {
        rv = _bcm_esw_gport_resolve(unit,
                                    h_data_p->resolved_trunk_gport,
                                    &module_id, &port_id,
                                    &trunk_id, &local_id);
    }
    if (rv != BCM_E_NONE) {
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    /*
     * Set the BHH encapsulation data
 */
    rv = _bcm_fp_oam_bhh_encap_create(unit, module_id, port_id, h_data_p,
                                       msg.encap_data, &encap_type,
                                       &msg.encap_length, &counter_info,
                                       csf_p->int_pri, csf_p->pkt_pri,
                                       h_data_p->egr_label_ttl);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "BHH(unit %d) Error: Encap create failed "
                              " - %s.\n"),
                   unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_csf_add_pack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
                                      MOS_MSG_CLASS_BHH,
                                      MOS_MSG_SUBCLASS_BHH_CSF_ADD,
                                      buffer_len, 0,
                                      MOS_MSG_SUBCLASS_BHH_CSF_ADD_REPLY,
                                      &reply_len);

    if(BCM_SUCCESS(rv) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *      _bcm_fp_oam_csf_get
 * Purpose:
 *      Get CSF info
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      csf_ptr - (OUT) CSF object
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_fp_oam_csf_get(int unit, bcm_oam_csf_t *csf_p)
{
    _bcm_fp_oam_control_t       *oc;
    bhh_sdk_msg_ctrl_csf_get_t   msg;
    _bhh_fp_oam_ep_data_t       *h_data_p;
    uint8                       *buffer, *buffer_ptr;
    uint16                       buffer_len, reply_len;
    uint8*                       encap_data = NULL;
    soc_olp_l2_hdr_t             olp_l2_header;
    soc_olp_tx_hdr_t             olp_oam_tx_header;
    int                          sess_id = 0;
    int                          i;
    int                          rv = BCM_E_NONE;


    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(csf_p->id);
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(csf_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->bhh_ep_data[sess_id];

    if (!BHH_EP_TYPE(h_data_p)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }


    sal_memset(&msg, 0, sizeof(msg));

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
                                      MOS_MSG_CLASS_BHH,
                                      MOS_MSG_SUBCLASS_BHH_CSF_GET,
                                      sess_id, 0,
                                      MOS_MSG_SUBCLASS_BHH_CSF_GET_REPLY,
                                      &reply_len);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Pack control message data into DMA buffer */
    buffer     = oc->bhh_dma_buffer;
    buffer_ptr = bhh_sdk_msg_ctrl_csf_get_unpack(buffer, &msg);
    buffer_len = buffer_ptr - buffer;


    if (reply_len != buffer_len) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INTERNAL;
    }

    switch(msg.type) {
        case SHR_CSF_TYPE_LOS:
            csf_p->type = BCM_OAM_CSF_LOS;
            break;

        case SHR_CSF_TYPE_FDI:
            csf_p->type = BCM_OAM_CSF_FDI;
            break;

        case SHR_CSF_TYPE_RDI:
            csf_p->type = BCM_OAM_CSF_RDI;
            break;

        case SHR_CSF_TYPE_DCI:
            csf_p->type = BCM_OAM_CSF_DCI;
            break;

        default:
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INTERNAL;
    }

    csf_p->period = msg.period;
    csf_p->flags  = msg.flags;

    for (i = 0; i< BCM_OAM_LM_COUNTER_MAX; i++) {
        csf_p->csf_tx_update_lm_counter_base_id[i] =
                                  h_data_p->csf_tx_update_lm_counter_base_id[i];
        csf_p->csf_tx_update_lm_counter_offset[i] =
                                  h_data_p->csf_tx_update_lm_counter_offset[i];
    }
    csf_p->csf_tx_update_lm_counter_size = h_data_p->csf_tx_counter_size;

    encap_data = msg.encap_data;
    encap_data = _bcm_fp_oam_olp_l2_header_unpack(encap_data,
                                                  &olp_l2_header);
    encap_data = shr_olp_tx_header_unpack(encap_data,
                                          &olp_oam_tx_header);
    csf_p->int_pri = SOC_OLP_TX_PRI_GET (&olp_oam_tx_header);
    csf_p->pkt_pri = bcm_fp_oam_bhh_get_oam_exp_from_encap(h_data_p,
                                                           msg.encap_data,
                                                           msg.encap_length);
    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_fp_oam_csf_delete
 * Purpose:
 *      Stop CSF PDU transmission
 * Parameters:
 *      unit    - (IN  Unit number.
 *      csf_ptr - (IN) CSF object
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_fp_oam_csf_delete(int unit, bcm_oam_csf_t *csf_p)
{
    _bcm_fp_oam_control_t   *oc;
    _bhh_fp_oam_ep_data_t   *h_data_p;
    uint16                   reply_len;
    int                      sess_id;
    int                      rv = BCM_E_NONE;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (oc->ukernel_not_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    BCM_OAM_BHH_VALIDATE_EP(csf_p->id);
    sess_id = BCM_OAM_BHH_GET_UKERNEL_EP(csf_p->id);

    /* Check endpoint status. */
    rv = shr_idxres_list_elem_state(oc->bhh_pool, sess_id);
    if ((BCM_E_EXISTS != rv)) {
        /* Endpoint not in use. */
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Endpoint EP=%d %s.\n"),
                   unit, csf_p->id, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    h_data_p = &oc->bhh_ep_data[sess_id];

    if (!BHH_EP_TYPE(h_data_p)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_UNAVAIL;
    }

    /* Send BHH Session Update message to uC */
    rv = _bcm_fp_oam_msg_send_receive(unit,
                                      MOS_MSG_CLASS_BHH,
                                      MOS_MSG_SUBCLASS_BHH_CSF_DELETE,
                                      sess_id, 0,
                                      MOS_MSG_SUBCLASS_BHH_CSF_DELETE_REPLY,
                                      &reply_len);

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_group_traverse
 * Purpose:
 *     Traverse the entire set of BHH groups, calling a specified
 *     callback for each one
 * Parameters:
 *     unit  - (IN) BCM device number
 *     cb        - (IN) Pointer to call back function.
 *     user_data - (IN) Pointer to user supplied data.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                           void *user_data)
{
    _bcm_fp_oam_control_t    *oc;        /* OAM control structure pointer. */
    bcm_oam_group_info_t  group_info; /* Group information to be set.   */
    bcm_oam_group_t       grp_idx;    /* MA Group index.                */
    int                   rv;         /* Operation return status.       */

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Get device OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    if (soc_feature(unit, soc_feature_bhh)) { 
        for (grp_idx = 0; grp_idx < oc->bhh_endpoint_count; grp_idx++) {

            /* Check if the group is in use. */
            if (BCM_E_EXISTS
                   == shr_idxres_list_elem_state(oc->bhh_group_pool, grp_idx)) {

                /* Initialize the group information structure. */
                bcm_oam_group_info_t_init(&group_info);

                group_info.id = BCM_OAM_BHH_GET_SDK_GROUP(grp_idx);
                /* Retrieve group information and set in group_info structure.*/
                rv = _bcm_fp_oam_bhh_group_get(unit, &group_info);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: "
                                        "_bcm_fp_oam_bhh_group_get "
                                        "(GID=%d) - %s.\n"), unit, grp_idx, 
                                        bcm_errmsg(rv)));
                    return (rv);
                }
                /* Call the user call back routine with group information. */
                rv = cb(unit, &group_info, user_data);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: User call back " 
                                        "routine (GID=%d) - %s.\n"), unit, 
                                        grp_idx, bcm_errmsg(rv)));
                    return (rv);
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fp_oam_bhh_group_destroy_all
 * Purpose:
 *     Traverse the entire set of BHH groups and deletes all of them
 * Parameters:
 *     unit  - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_bhh_group_destroy_all(int unit)
{
    _bcm_fp_oam_control_t    *oc;        /* OAM control structure pointer. */
    bcm_oam_group_t       grp_idx, group = 0;/* MA Group index.            */
    int                   rv;         /* Operation return status.       */

    /* Get device OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    for (grp_idx = 0; grp_idx < oc->bhh_endpoint_count; grp_idx++) {

        /* Check if the group is in use. */
        if (BCM_E_EXISTS
                == shr_idxres_list_elem_state(oc->bhh_group_pool, grp_idx)) {

            group = BCM_OAM_BHH_GET_SDK_GROUP(grp_idx);

            rv = _bcm_fp_oam_bhh_group_destroy(unit, group);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: "
                                    "_bcm_fp_oam_bhh_group_destroy "
                                    "(GID=%d) - %s.\n"), unit, group, 
                         bcm_errmsg(rv)));
                return (rv);
            }
        }
    }

    return BCM_E_NONE;
}
#endif /*(INCLUDE_BHH) */

/*******************************************************************************/
/* CCM Embedded App supporting functions Start */
/*******************************************************************************/
#if defined (INCLUDE_CCM)

#define _BCM_OAM_GLP_MODULE_ID_GET(_glp_) (0xFF & ((_glp_) >> 7))
#define _BCM_OAM_GLP_PORT_GET(_glp_)      (0x7F & (_glp_))

/*
 * Macro:
 *     _BCM_OAM_EP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Endpoint ID value.
 * Parameters:
 *     _ep_ - Endpoint ID value.
 */
#define _BCM_OAM_EP_INDEX_VALIDATE(_ep_)                       \
    do {                                                       \
        if ((_ep_) < 0 || (_ep_) >= oc->mep_count) {            \
            LOG_ERROR(BSL_LS_BCM_OAM,                          \
                      (BSL_META("OAM(unit %d) Error: Invalid Endpoint ID" \
                       " = %d.\n"), unit, _ep_));              \
            return (BCM_E_PARAM);                              \
        }                                                      \
    } while (0);

/*
 * Macro:
 *     _BCM_FP_OAM_GROUP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Group ID value.
 * Parameters:
 *     _group_ - Group ID value.
 */
#define _BCM_FP_OAM_GROUP_INDEX_VALIDATE(_group_)                   \
    do {                                                            \
        if ((_group_) < 0 || (_group_) >= oc->group_count) {        \
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META("OAM Error: "       \
              "Invalid Group ID = %d.\n"), _group_));               \
            return (BCM_E_PARAM);                                   \
        }                                                           \
    } while (0);


/*
 * Function:
 *     _bcm_fp_oam_group_set_msg_send
 * Purpose:
 *     Send Group Create/Update Message to UKERNEL
 * Parameters:
 *     unit          - (IN) BCM device number
 *     ep_data       - (IN) Pointer to endpoint information.
 *     msg_flags     - (IN) Msg control flags
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_fp_oam_group_set_msg_send(int unit, _bcm_fp_oam_group_data_t *grp_p,
                              uint32 msg_flags, uint16 *r_msg_len)
{
    shr_ccm_msg_ctrl_group_data_t group_msg;
    _bcm_fp_oam_control_t *oc = NULL;
    uint8 *dma_buffer1 = NULL;  /* DMA buffer */
    uint8 *dma_buffer2 = NULL;  /* DMA buffer ptr after pack */
    uint16 s_msg_len = 0;       /* Send Message length */
    int rv = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    dma_buffer1 = oc->dma_buffer;

    /* Validate input parameter. */
    if (NULL == grp_p) {
        return (BCM_E_INTERNAL);
    }

    /* Clear meo_msg structure */
    sal_memset(&group_msg, 0, sizeof(group_msg));
    group_msg.flags = msg_flags;

    /* Fill Mep data and Send MEP Create message to UKENREL */
    sal_memcpy(&(group_msg.group_data), &(grp_p->group_data),
               sizeof(ccm_group_data_t));

    /* Pack Mep data to buffer */
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_group_set_pack(dma_buffer1, &group_msg);
    s_msg_len = dma_buffer2 - dma_buffer1;
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_SET, s_msg_len, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_SET_REPLY, r_msg_len);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group Set Msg returned error %s.\n"), bcm_errmsg(rv)));
    }
    return rv;
}


/*
 * Function:
 *     bcm_fp_oam_ccm_group_create
 * Purpose:
 *     Create or replace an OAM group object
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group information.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bcm_fp_oam_group_data_t *oam_group = NULL; /* OAM Group being created */
    uint32 msg_flags = 0;                       /* Group Msg flags */
    uint16 r_len = 0;                           /* Received Msg length */
    int rv = 0;                                 /* return value */

    /* Validate input parameter. */
    if (NULL == group_info) {
        return (BCM_E_PARAM);
    }

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate group id. */
    if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        _BCM_FP_OAM_GROUP_INDEX_VALIDATE(group_info->id);
    }

    /*
     * If MA group create is called with replace flag bit set.
     *  - Check and return error if a group does not exist with the ID.
     */
    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {

            /* Search the list with the MA Group ID value. */
            rv = shr_idxres_list_elem_state(oc->group_pool, group_info->id);
            if (BCM_E_EXISTS != rv) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                          "OAM Error: Group does not exist.\n")));
                return (BCM_E_PARAM);
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                      "OAM Error: Replace command needs a valid Group ID.\n")));
            return (BCM_E_PARAM);
        }
    } else if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        /*
         * If MA group create is called with ID flag bit set.
         *  - Check and return error if the ID is already in use.
         */
        rv = shr_idxres_list_reserve(oc->group_pool, group_info->id,
                                     group_info->id);
        if (BCM_FAILURE(rv)) {
            return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
        }
    } else {
        /* Reserve the next available group index. */
         rv = shr_idxres_list_alloc(oc->group_pool,
                                    (shr_idxres_element_t *) &group_info->id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                      "OAM Error: Group allocation (GID=%d) %s\n"),
                      group_info->id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Get this group memory to store group information. */
    oam_group = &(oc->group_data[group_info->id]);

    if (!(group_info->flags & BCM_OAM_GROUP_REPLACE)) {

        /* Store the group name. */
        sal_memcpy(oam_group->group_data.name, group_info->name,
                   BCM_OAM_GROUP_NAME_LENGTH);

        /* Store Lowest Alarm Priority */
        oam_group->group_data.lowest_alarm_priority = group_info->lowest_alarm_priority;

        /* Initialize RMEP head to NULL.*/
        sal_memset(oam_group->group_data.rmep_bitmap, 0,
                                    sizeof(oam_group->group_data.rmep_bitmap));

        /* Check if software RDI flag bit needs to be set in hardware. */
        oam_group->group_data.flags |=
            ((group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) ?
              _FP_OAM_GROUP_SW_RDI_FLAG : 0);

        /* Set LMEP as not configured */
        oam_group->group_data.lmep_id = _FP_OAM_INVALID_LMEP_ID;

        oam_group->group_data.group_id = group_info->id;

        /* Send Group Create message to uKernel */
        rv = _bcm_fp_oam_group_set_msg_send(unit, oam_group, msg_flags, &r_len);
         if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Group create Msg failed, id:%d - %s"), group_info->id,
                bcm_errmsg(rv)));
        } else {
            oam_group->in_use = 1;
        }

    } else {
        /* Only lowest alarm prio and S/w RDI bit can be replaced */
        if ((oam_group->group_data.flags & _FP_OAM_GROUP_SW_RDI_FLAG) !=
            (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX)) {

            if (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) {
                oam_group->group_data.flags |= _FP_OAM_GROUP_SW_RDI_FLAG;
            } else {
                oam_group->group_data.flags &= ~(_FP_OAM_GROUP_SW_RDI_FLAG);
            }
            msg_flags |= _GRP_REPLACE;
            msg_flags |= _GRP_REPLACE_SW_RDI;
        }

        if (oam_group->group_data.lowest_alarm_priority !=
            group_info->lowest_alarm_priority) {

            oam_group->group_data.lowest_alarm_priority =
                group_info->lowest_alarm_priority;
            msg_flags |= _GRP_REPLACE;
            msg_flags |= _GRP_REPLACE_LOWEST_ALARM_PRIO;
        }

        /* Send Group Create message to uKernel */
        rv = _bcm_fp_oam_group_set_msg_send(unit, oam_group, msg_flags, &r_len);
         if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Group create Msg failed, id:%d - %s"), group_info->id,
                bcm_errmsg(rv)));
        }
    }

    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_ccm_group_get
 * Purpose:
 *     Delete an OAM group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_group_get(int unit, bcm_oam_group_info_t *group_info) {

    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bcm_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    uint16 reply_len;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;
    shr_ccm_msg_ctrl_group_data_t group_msg;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate Group Index */
    _BCM_FP_OAM_GROUP_INDEX_VALIDATE(group_info->id);

    /* Get Handle to Local DB */
    group_p = &(oc->group_data[group_info->id]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET_FAULTS_ONLY, group_info->id, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET_REPLY, &reply_len);

    /* UnPack control message data into DMA buffer */
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_group_get_unpack(dma_buffer1, &group_msg);

    if (reply_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Group get Msg lenght check failed, id:%d.\n"), group_info->id));
        return (rv);
    }
    group_info->faults = group_msg.group_data.faults;
    group_info->persistent_faults = group_msg.group_data.persistent_faults;

    sal_memcpy(group_info->name, group_p->group_data.name, _FP_OAM_MAID_LEN);
    group_info->lowest_alarm_priority =
        group_p->group_data.lowest_alarm_priority;

    if (group_p->group_data.flags & _FP_OAM_GROUP_SW_RDI_FLAG) {
        group_info->flags |= BCM_OAM_GROUP_REMOTE_DEFECT_TX;
    }

    return rv;
}

int
_bcm_fp_oam_ccm_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                           void *user_data)
{
    _bcm_fp_oam_control_t    *oc = NULL;        /* OAM control structure pointer. */
    bcm_oam_group_info_t     group_info;
    bcm_oam_group_t          grp_idx;    /* MA Group index.                */
    int                      rv;         /* Operation return status.       */
    _bcm_fp_oam_group_data_t *group_p;

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Get device OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

        for (grp_idx = 0; grp_idx < oc->group_count; grp_idx++) {
            group_p = &(oc->group_data[grp_idx]);
            if (group_p->in_use) {
                group_info.id = grp_idx;
                rv = bcm_fp_oam_ccm_group_get(unit, &group_info);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit,
                                         "OAM(unit %d) Error: "
                                         "_bcm_fp_oam_ccm_group_get "
                                         "(GID=%d) - %s.\n"), unit, grp_idx,
                                         bcm_errmsg(rv)));
                    return (rv);
                }
                /* Call the user call back routine with group information. */
                rv = cb(unit, &group_info, user_data);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM,
                             (BSL_META_U(unit,
                                        "OAM(unit %d) Error: User call back "
                                        "routine (GID=%d) - %s.\n"), unit,
                                        grp_idx, bcm_errmsg(rv)));
                    return (rv);
                }
            }
        }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fp_oam_ccm_group_destroy
 * Purpose:
 *     Delete an OAM group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_group_destroy(int unit, bcm_oam_group_t group_id)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bcm_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    uint16 r_len;                               /* Msg reply lenght */
    int i = 0;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate Group Index */
    _BCM_FP_OAM_GROUP_INDEX_VALIDATE(group_id);

    /* Get Handle to Local DB */
    group_p = &(oc->group_data[group_id]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_DELETE, group_id, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_DELETE_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Group Del Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
        return rv;
    }

    /* Clean up Local db, including LMEPs and RMEPs */
    if (group_p->group_data.lmep_id != _FP_OAM_INVALID_LMEP_ID) {
        sal_memset(&(oc->mep_data[group_p->group_data.lmep_id]), 0,
                   sizeof(_bcm_fp_oam_mep_data_t));

        /* Free the LMEP index */
        rv = shr_idxres_list_free(oc->mep_pool, group_p->group_data.lmep_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "LMEP index Free failed EP:%d %s.\n"),
                      group_p->group_data.lmep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    for (i=0; i<_CCM_MEP_COUNT; i++) {
        if (_GET_BIT_FLD(group_p->group_data.rmep_bitmap, i) ) {
            sal_memset(&(oc->mep_data[i]), 0, sizeof(_bcm_fp_oam_mep_data_t));

            /* Free the RMEP index */
            rv = shr_idxres_list_free(oc->mep_pool, i);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                          "RMEP index Free failed EP:%d %s.\n"), i,
                          bcm_errmsg(rv)));
                return (rv);
            }
        }
    }

    /* Free the Group index */
    rv = shr_idxres_list_free(oc->group_pool, group_id);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group index Free failed EP:%d %s.\n"), group_id,
                  bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(group_p, 0, sizeof(_bcm_fp_oam_group_data_t));
    group_p->group_data.lmep_id = _FP_OAM_INVALID_LMEP_ID;

    return rv;
}


/*
 * Function:
 *     _bcm_fp_oam_ep_hash_key_construct
 * Purpose:
 *     Construct hash table key for a given endpoint information.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     oc      - (IN) Pointer to OAM control structure.
 *     ep_info - (IN) Pointer to endpoint information structure.
 *     key     - (IN/OUT) Pointer to hash key buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_fp_oam_ep_hash_key_construct(int unit, _bcm_fp_oam_control_t *oc,
        bcm_oam_endpoint_info_t *ep_info, _bcm_fp_oam_hash_key_t *key)
{
    uint8   *loc = *key;
    uint8   direction = 0;
    uint8   isLocal = 1;

    sal_memset(key, 0, sizeof(_bcm_fp_oam_hash_key_t));

    if (NULL != ep_info) {

        if (ep_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            direction = 1;
        }
        if (ep_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            isLocal = 0;
        }
        _BCM_OAM_KEY_PACK(loc, &direction, sizeof(direction));
        _BCM_OAM_KEY_PACK(loc, &isLocal, sizeof(isLocal));
        _BCM_OAM_KEY_PACK(loc, &ep_info->group, sizeof(ep_info->group));
        _BCM_OAM_KEY_PACK(loc, &ep_info->name, sizeof(ep_info->name));
        _BCM_OAM_KEY_PACK(loc, &ep_info->gport, sizeof(ep_info->gport));
        _BCM_OAM_KEY_PACK(loc, &ep_info->level, sizeof(ep_info->level));
        _BCM_OAM_KEY_PACK(loc, &ep_info->vlan, sizeof(ep_info->vlan));
        _BCM_OAM_KEY_PACK(loc, &ep_info->inner_vlan,
                          sizeof(ep_info->inner_vlan));
    }

    /* End address should not exceed size of _bcm_oam_hash_key_t. */
    assert ((int) (loc - *key) <= sizeof(_bcm_oam_hash_key_t));
}


/*
 * Function:
 *     _bcm_fp_oam_endpoint_params_validate
 * Purpose:
 *     Validate an endpoint parameters.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     oc        - (IN) Pointer to OAM control structure.
 *     hash_key  - (IN) Pointer to endpoint hash key value.
 *     ep_info_p - (IN) Pointer to endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_endpoint_params_validate(int unit, _bcm_fp_oam_control_t *oc,
        _bcm_fp_oam_hash_key_t *hash_key,  bcm_oam_endpoint_info_t *ep_info_p)
{
    int rv = 0;
    _bcm_fp_oam_group_data_t *group_p = NULL;

    LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Info: "
              "_bcm_fp_oam_endpoint_params_validate.\n")));

    /* Endpoint must be 802.1ag/Ethernet OAM type. */
    if ((bcmOAMEndpointTypeEthernet != ep_info_p->type))
    {
        /* Other OAM types are not supported, return error. */
        return BCM_E_UNAVAIL;
    }

    /* Supported MDL level is 0 - 7. */
    if ((ep_info_p->level < 0) || (ep_info_p->level > _FP_OAM_MAX_MDL)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
          "EP Level should be in the range(0-%d).\n"), _FP_OAM_MAX_MDL ));
        return (BCM_E_PARAM);
     }

    /* Supported MEPID Name range is 1 - 8191, Skipping range check for MIP and MPLS LM/DM */
    if (((ep_info_p->name < _FP_OAM_EP_NAME_MIN)
        || (ep_info_p->name > _FP_OAM_EP_NAME_MAX)) &&
        !(ep_info_p->flags & BCM_OAM_ENDPOINT_INTERMEDIATE)
#if defined(INCLUDE_MPLS_LM_DM)
        && (!_MPLS_LM_DM_ENDPOINT_TYPE(ep_info_p->type))) {
#else
       ) {
#endif
       LOG_ERROR(BSL_LS_BCM_OAM,
                 (BSL_META_U(unit,
                             "OAM Error: MEP Name should be in the range(%d-%d).\n"),
                  _FP_OAM_EP_NAME_MIN, _FP_OAM_EP_NAME_MAX));
       return (BCM_E_PARAM);
    }

    /* For replace operation, endpoint ID is required. */
    if ((ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)
        && !(ep_info_p->flags & BCM_OAM_ENDPOINT_WITH_ID)) {

        return (BCM_E_PARAM);
    }

    /* If it is a Port + CVlan based MEP or Section Port + CVlan
     * based MEP and the CVlan (inner_vlan) is 0, return E_PARAM
     */
    if ((0 == ep_info_p->inner_vlan) &&
        (ep_info_p->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN)) {
        return (BCM_E_PARAM);
    }

    /* If it is a Port + S + CVlan based MEP or Section Port + S + CVlan
     * based MEP and one of SVlan(vlan) or CVlan (inner_vlan) is 0,
     * return E_PARAM
     */
    if (((0 == ep_info_p->inner_vlan) || (0 == ep_info_p->vlan)) &&
        ((ep_info_p->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN))) {
        return (BCM_E_PARAM);
    }

    /* Validate endpoint index value. */
    if (ep_info_p->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        _BCM_OAM_EP_INDEX_VALIDATE(ep_info_p->id);
    }

    /* MIP's are not created with non-zero ccm period */
    if ((ep_info_p->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) &&
        (ep_info_p->ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)) {
         return (BCM_E_PARAM);
    }

    /* Validate endpoint group id. */
    _BCM_FP_OAM_GROUP_INDEX_VALIDATE(ep_info_p->group);

    rv = shr_idxres_list_elem_state(oc->group_pool, ep_info_p->group);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group (GID:%d) does not exist.\n"), ep_info_p->group));
        return (BCM_E_PARAM);
    }

    /* Only One LMEP should be configured per group */
    group_p = &(oc->group_data[ep_info_p->group]);
    if (!(ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE)) {

        if ((group_p->group_data.lmep_id != _FP_OAM_INVALID_LMEP_ID) &&
             !(ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "LMEP already existst for the group\n")));
            return (rv);
        }
    }

#if 0
    
    rv = shr_htb_find(oc->ma_mep_htbl, *hash_key,
                      (shr_htb_data_t *)&h_stored_data, 0);

    if (BCM_SUCCESS(rv) && !(ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: %s\n"),
                  bcm_errmsg(BCM_E_EXISTS)));

        /* Endpoint must not be in use except for Replace operation. */
        return (BCM_E_EXISTS);
    } else {
        /* Cannot replace any Key params for an endpoint */
        if ((ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE) && !(BCM_SUCCESS(rv))) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Endpoint "
                "ID=%d, invalid params for replace operation or Endpoint "
                "doesn't exists\n"), ep_info_p->id));
        }
    }
#endif
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_fp_oam_get_ctr_info_from_stat_id
 * Purpose:
 *     Get OLP Formatted Counter Id from Stat Id
 * Parameters:
 *     unit          - (IN) BCM device number
 *     stat_id       - (IN) Stat group Id.
 *     offset        - (IN) Offset from Base
 *     counter_id    - (OUT) OLP Formatted counter value
 * Returns:
 *      void
 */
STATIC void
_bcm_fp_oam_get_ctr_info_from_stat_id(int unit, uint32 stat_id, uint8 offset,
                                                           uint32 *counter_id)
{
    bcm_stat_group_mode_t stat_group;
    bcm_stat_object_t stat_object;
    uint32 stat_mode;
    uint32 stat_pool_number;
    uint32 stat_base_idx;

    _bcm_esw_stat_get_counter_id_info(unit, stat_id,
      &stat_group, &stat_object, &stat_mode, &stat_pool_number, &stat_base_idx);

    *counter_id = ( ((stat_base_idx & _FP_OAM_OLP_CTR_WIDTH) + offset) |
                    ((stat_pool_number & _FP_OAM_OLP_CTR_POOL_WIDTH) <<
                     _FP_OAM_OLP_CTR_WIDTH) );

}


/*
 * Function:
 *     _bcm_fp_oam_olp_header_pack
 * Purpose:
 *     Pack OLP Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack OLP Header
 *     mep_data   - (IN)     Pointer to MEP data.
 * Retruns:
 *     BCM_E_XX
 */
STATIC int
_bcm_fp_oam_olp_header_pack(int unit, uint8 *buffer, ccm_mep_data_t *mep_data)
{

    soc_olp_l2_hdr_t olp_l2_hdr;
    soc_olp_tx_hdr_t olp_oam_header;
    int rv = BCM_E_NONE;
    uint32 counter_id;
    bcm_module_t my_modid = 0;
    bcm_gport_t  cpu_dglp;

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID, my_modid,
                                               CMIC_PORT(unit), &cpu_dglp));
    /* Get OLP L2 Header for CPU port */
    sal_memset(&olp_l2_hdr, 0, sizeof(olp_l2_hdr));
    rv = _bcm_olp_l2_hdr_get(unit, cpu_dglp, 0, &olp_l2_hdr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "OLP Header get failed.\n")));
        return (rv);
    }

    /* OLP OAM Header */
    sal_memset(&olp_oam_header, 0, sizeof(olp_oam_header));
    /* Set Port number */
    SOC_OLP_TX_PORT(&olp_oam_header) =
        _BCM_OAM_GLP_PORT_GET(mep_data->dglp);
    SOC_OLP_TX_MODID(&olp_oam_header) =
        _BCM_OAM_GLP_MODULE_ID_GET(mep_data->dglp);

    if (mep_data->flags & _FP_OAM_DIRECTION_UP) {
        SOC_OLP_TX_HDR_STYPE_GET(&olp_oam_header) = 1;
    }

    if (0 < mep_data->num_ctrs) {
        _bcm_fp_oam_get_ctr_info_from_stat_id(unit, mep_data->flex_stat_id[0],
            mep_data->counter_offset[0], &counter_id);
        SOC_OLP_TX_CTR1_ID(&olp_oam_header) = counter_id;
        SOC_OLP_TX_CTR1_LOCATION(&olp_oam_header) = _FP_OAM_OLP_CTR_LOC_EP;
        SOC_OLP_TX_CTR1_ACTION(&olp_oam_header) = _FP_OAM_OLP_CTR_ACTN_INC;
    }
    if (1 < mep_data->num_ctrs) {
        _bcm_fp_oam_get_ctr_info_from_stat_id(unit, mep_data->flex_stat_id[1],
            mep_data->counter_offset[1], &counter_id);
        SOC_OLP_TX_CTR2_ID_SET(&olp_oam_header, counter_id);
        SOC_OLP_TX_CTR2_LOCATION(&olp_oam_header) = _FP_OAM_OLP_CTR_LOC_EP;
        SOC_OLP_TX_CTR2_ACTION(&olp_oam_header) = _FP_OAM_OLP_CTR_ACTN_INC;
    }

    /* Now that we have both L2 and OAM OLP Headers, pack them */
    buffer = _bcm_fp_oam_olp_l2_header_pack(buffer, &olp_l2_hdr);
    buffer = shr_olp_tx_header_pack(buffer, &olp_oam_header);

    return (rv);
}


/*
 * Function:
 *     _bcm_fp_oam_l2_encap_pack
 * Purpose:
 *     Pack OLP Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack L2 Encap
 *     mep_data   - (IN)     Pointer to MEP data.
 * Retruns:
 *     BCM_E_XX
 */
STATIC uint8*
_bcm_fp_oam_l2_encap_pack(int unit, uint8 *buffer, ccm_mep_data_t *mep_data)
{
    uint32 vlan_tag = 0;
    uint16 cfm_ether_type = _CCM_ETHER_TYPE;
    int i;
    /* Pack DA */
    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buffer, mep_data->da[i]);
    }
    /* Pack SA */
    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buffer, mep_data->sa[i]);
    }
    /* Pack Outer vlan */
    if (mep_data->vlan) {
        vlan_tag = 0;
        vlan_tag = (mep_data->tpid << 16) |
                   ((mep_data->pkt_pri & 0x7) << 13) |
                   (mep_data->vlan & 0x0FFF);
        _SHR_PACK_U32(buffer, vlan_tag);
    }
    /* Pack Inner vlan */
    if (mep_data->inner_vlan) {
        vlan_tag = 0;
        vlan_tag = (mep_data->inner_tpid << 16) |
                   ((mep_data->inner_vlan_pri & 0x7) << 13) |
                   (mep_data->inner_vlan & 0x0FFF);
        _SHR_PACK_U32(buffer, vlan_tag);
    }
    /* Pack EtherType */
    _SHR_PACK_U16(buffer, cfm_ether_type);
    return buffer;
}


/*
 * Function:
 *     _bcm_fp_oam_mep_set_msg_send
 * Purpose:
 *     Send Mep Create/Update Message to UKERNEL
 * Parameters:
 *     unit          - (IN) BCM device number
 *     ep_data       - (IN) Pointer to endpoint information.
 *     uint32        - (IN) Msg Control flags
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_fp_oam_mep_set_msg_send(int unit, _bcm_fp_oam_mep_data_t *ep_data,
                            uint32 msg_flags, uint16 *r_msg_len)
{
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    _bcm_fp_oam_control_t *oc = NULL;
    uint8 *dma_buffer1 = NULL;  /* DMA buffer */
    uint8 *dma_buffer2 = NULL;  /* DMA buffer ptr after pack */
    uint16 s_msg_len = 0;       /* Send Message length */
    int rv = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    dma_buffer1 = oc->dma_buffer;

    /* Validate input parameter. */
    if (NULL == ep_data) {
        return (BCM_E_INTERNAL);
    }

    /* Clear mep_msg structure */
    sal_memset(&mep_msg, 0, sizeof(mep_msg));
    mep_msg.flags = msg_flags;

    if (msg_flags & _MEP_L2_ENCAP_RECREATE) {
        dma_buffer1 = _bcm_fp_oam_l2_encap_pack(unit, mep_msg.L2_encap,
                                                &(ep_data->mep_data));
        mep_msg.l2_encap_len = dma_buffer1 - mep_msg.L2_encap;
    }

    if (msg_flags & _MEP_OLP_RECREATE) {
        rv = _bcm_fp_oam_olp_header_pack(unit, mep_msg.OLP_Tx_Hdr, 
                                         &(ep_data->mep_data));
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "OLP Header Pack failed.\n")));
            return (rv);
        }
    }

    /* Fill Mep data and Send MEP Create message to UKENREL */
    sal_memcpy(&(mep_msg.mep_data), &(ep_data->mep_data),
               sizeof(ccm_mep_data_t));

    /* Pack Mep data to buffer */
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_mep_set_pack(dma_buffer1, &mep_msg);
    s_msg_len = dma_buffer2 - dma_buffer1;
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_SET, s_msg_len, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_SET_REPLY, r_msg_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "MEP Create Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
    }
    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_ccm_endpoint_create
 * Purpose:
 *     Create or replace a CCM OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_ccm_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_fp_oam_control_t *oc = NULL;
    _bcm_fp_oam_hash_key_t hash_key;
    _bcm_fp_oam_mep_data_t *ep_data;
    uint32 mep_index;
    int src_glp;
    int dst_glp;
    bcm_gport_t dgpp;
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    ccm_group_data_t *grp_data_ptr = NULL;
    uint32  msg_flags = 0;
    uint16  r_len = 0; /* Msg response lenght */
    int rv = 0;
    bcm_trunk_member_t   trunk_member;
    int i;

    /* Validate input parameter. */
    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }
    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Calculate the hash key for given enpoint input parameters. */
    _bcm_fp_oam_ep_hash_key_construct(unit, oc, endpoint_info, &hash_key);

    /* Validate endpoint input parameters. */
    rv = _bcm_fp_oam_endpoint_params_validate(unit, oc, &hash_key,
                                              endpoint_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Parameter "
            "Validation failed %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    rv = _bcm_fp_oam_endpoint_gport_resolve(unit, endpoint_info,
                                            &src_glp, &dst_glp, &dgpp,
                                            &trunk_id, &trunk_member);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Gport Resolve "
            "Failed %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    /* Now that EP passed all the validation checks, create the endpoint */
    if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {

        if (trunk_id != BCM_TRUNK_INVALID) {
            rv = _bcm_fp_oam_trunk_ports_update_trunk_id(unit, trunk_id);
            if (BCM_FAILURE(rv)) {
                /* Dont return just for this error */
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                                "Trunk map update failed EP:%d %s.\n"), endpoint_info->id,
                            bcm_errmsg(rv)));
            }       
        } 

        if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
            /* Reserve the Id passed by the user */
            mep_index = endpoint_info->id;
            rv = shr_idxres_list_reserve(oc->mep_pool, mep_index, mep_index);

        } else {
            /* Allocate the next Id from the pool */
            rv = shr_idxres_list_alloc(oc->mep_pool,
                    (shr_idxres_element_t *)&mep_index);
            endpoint_info->id = mep_index;
        }

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "MEP index alloc failed EP:%d %s.\n"), endpoint_info->id,
                      bcm_errmsg(rv)));
            return (rv);
        }       

        /* Shift to specific data element in the array */
        ep_data = &(oc->mep_data[mep_index]);

        /* If it is a Port + S + CVlan based MEP */
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
            ep_data->mep_data.type = _FP_OAM_MEP_TYPE_S_C_VLAN;

        } else {

            /* If it is a Port + CVlan based MEP */
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
                ep_data->mep_data.type = _FP_OAM_MEP_TYPE_CVLAN;

            } else {

                /* If it is a Port + SVlan based MEP */
                if (endpoint_info->vlan) {
                    ep_data->mep_data.type = _FP_OAM_MEP_TYPE_SVLAN;

                } else {
                    /* If it is a Port based MEP */
                    ep_data->mep_data.type = _FP_OAM_MEP_TYPE_PORT;
                }
            }
        }

        ep_data->mep_data.ep_id = mep_index;
        ep_data->mep_data.level = endpoint_info->level;
        ep_data->mep_data.period = endpoint_info->ccm_period;
        ep_data->mep_data.name = endpoint_info->name;

        ep_data->mep_data.gport = endpoint_info->gport;
        ep_data->mep_data.dglp = dst_glp;
        ep_data->mep_data.sglp = src_glp;
        ep_data->mep_data.trunk_index = endpoint_info->trunk_index;

        ep_data->mep_data.group_id = endpoint_info->group;

        ep_data->mep_data.vlan = endpoint_info->vlan;
        ep_data->mep_data.pkt_pri = endpoint_info->pkt_pri;
        ep_data->mep_data.tpid = endpoint_info->outer_tpid;

        ep_data->mep_data.inner_vlan = endpoint_info->inner_vlan;
        ep_data->mep_data.inner_vlan_pri = endpoint_info->inner_pkt_pri;
        ep_data->mep_data.inner_tpid = endpoint_info->inner_tpid;

        sal_memcpy(ep_data->mep_data.da,  endpoint_info->dst_mac_address,
                   sizeof(bcm_mac_t));
        sal_memcpy(ep_data->mep_data.sa,  endpoint_info->src_mac_address,
                   sizeof(bcm_mac_t));

        grp_data_ptr = &(oc->group_data[endpoint_info->group].group_data);
        if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {
                ep_data->mep_data.flags |= _FP_OAM_INTERFACE_STATUS_TX;
                ep_data->mep_data.intf_state = endpoint_info->interface_state;
            }

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
                ep_data->mep_data.flags |= _FP_OAM_PORT_STATUS_TX;
                ep_data->mep_data.port_state = endpoint_info->port_state;
            }

            

            if (endpoint_info->ccm_tx_update_lm_counter_size != 0) {
                ep_data->mep_data.num_ctrs =
                    endpoint_info->ccm_tx_update_lm_counter_size;
                for (i=0; i<ep_data->mep_data.num_ctrs; i++) {
                    ep_data->mep_data.flex_stat_id[i] =
                            endpoint_info->ccm_tx_update_lm_counter_base_id[i];
                    ep_data->mep_data.counter_offset[i] =
                            endpoint_info->ccm_tx_update_lm_counter_offset[i];
                }
            }

            /* Set LMEP ID in group */
            grp_data_ptr->lmep_id = mep_index;
            msg_flags |= _MEP_OLP_RECREATE;
            msg_flags |= (_MEP_L2_ENCAP_RECREATE);
        } else {
            ep_data->mep_data.flags |= _FP_OAM_REMOTE_MEP;
            /* RMEP ID in the group */
            _SET_BIT_FLD(grp_data_ptr->rmep_bitmap, mep_index);
        }

        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            ep_data->mep_data.flags |= _FP_OAM_DIRECTION_UP;
        }


        /* Send MEP Create Message to UKERNEL */
        rv = _bcm_fp_oam_mep_set_msg_send(unit, ep_data, msg_flags, &r_len);

        if (BCM_FAILURE(rv)) {

            /* Free the allocated MEP index */
            shr_idxres_list_free(oc->mep_pool, mep_index);

            /* Clear LMEP ID in Group */
            if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
                grp_data_ptr->lmep_id = _FP_OAM_INVALID_LMEP_ID;
            }
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "MEP Creation failed EP:%d %s.\n"), endpoint_info->id,
                  bcm_errmsg(rv)));
            return (rv);

        } else {
            ep_data->in_use = 1;
        }

    } else {
        /* Update case */
        ep_data = &(oc->mep_data[endpoint_info->id]);

        /* For Remote MEP, can't replace anything */
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "Cannot replace any parameter in RMEP.\n")));
            return BCM_E_PARAM;
        }

        /* For local MEP, can replace only Intf status, Port status, trunk index
           counter indices */
        if (((ep_data->mep_data.flags & _FP_OAM_INTERFACE_STATUS_TX) !=
            (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX)) ||
            (ep_data->mep_data.intf_state != endpoint_info->interface_state)) {

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {

                ep_data->mep_data.flags |= _FP_OAM_INTERFACE_STATUS_TX;
                ep_data->mep_data.intf_state = endpoint_info->interface_state;
            } else {
                ep_data->mep_data.flags &= ~(_FP_OAM_INTERFACE_STATUS_TX);
                ep_data->mep_data.intf_state = 0;
            }
            /* Something valid was found to be replaced */
            msg_flags |= _MEP_REPLACE;
            msg_flags |= _REPLACE_INTF_STATUS;

        }

        if (((ep_data->mep_data.flags & _FP_OAM_PORT_STATUS_TX) !=
            (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX)) ||
            (ep_data->mep_data.port_state != endpoint_info->port_state)) {

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
                ep_data->mep_data.flags |= _FP_OAM_PORT_STATUS_TX;
                ep_data->mep_data.port_state = endpoint_info->port_state;
            } else {
                ep_data->mep_data.flags &= ~(_FP_OAM_PORT_STATUS_TX);
                ep_data->mep_data.port_state = 0;
            }
            /* Something valid was found to be replaced */
            msg_flags |= _MEP_REPLACE;
            msg_flags |= _REPLACE_PORT_STATUS;
        }

        if (ep_data->mep_data.trunk_index != endpoint_info->trunk_index) {
            ep_data->mep_data.trunk_index = endpoint_info->trunk_index;
            ep_data->mep_data.dglp = dst_glp;

            msg_flags |= (_MEP_REPLACE | _MEP_OLP_RECREATE);
            msg_flags |= _REPLACE_TRUNK_INDEX;
        }

        if (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_UPDATE_COUNTER_ACTION) {
            if (endpoint_info->ccm_tx_update_lm_counter_size != 0) {
                ep_data->mep_data.num_ctrs =
                    endpoint_info->ccm_tx_update_lm_counter_size;
                for (i=0; i<ep_data->mep_data.num_ctrs; i++) {
                    ep_data->mep_data.flex_stat_id[i] =
                            endpoint_info->ccm_tx_update_lm_counter_base_id[i];
                    ep_data->mep_data.counter_offset[i] =
                            endpoint_info->ccm_tx_update_lm_counter_offset[i];
                }
            }
            msg_flags |= (_MEP_REPLACE | _MEP_OLP_RECREATE);
            msg_flags |= _REPLACE_CTRS;
        }

        if (!(msg_flags & _MEP_REPLACE)) {
            /* Nothing valid was found to be updated */
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Nothing valid to update:%d %s.\n"), endpoint_info->id,
                bcm_errmsg(rv)));
            return (rv);
        }

        /* Send MEP Update message to UKERNEL */
        rv = _bcm_fp_oam_mep_set_msg_send(unit, ep_data, msg_flags, &r_len);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "MEP Update failed EP:%d %s.\n"), endpoint_info->id,
                  bcm_errmsg(rv)));
            return (rv);
        }
    }

    return rv;

}


/*,
 * Function:
 *     bcm_fp_oam_ccm_endpoint_get
 * Purpose:
 *     Create or replace an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_ccm_endpoint_get(int unit, bcm_oam_endpoint_info_t *ep_info_p) {

    _bcm_fp_oam_control_t *oc = NULL;
    _bcm_fp_oam_mep_data_t *ep_data;
    uint16  r_len = 0; /* Msg response lenght */
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    int rv = 0;

    /* Validate input parameter. */
    if (NULL == ep_info_p) {
        return (BCM_E_PARAM);
    }
    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate EP Index */
    _BCM_OAM_EP_INDEX_VALIDATE(ep_info_p->id);

    /* Get pointer to S/w MEP data */
    ep_data = &(oc->mep_data[ep_info_p->id]);

    if (!(ep_data->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_GET_FAULTS_ONLY, ep_info_p->id, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_GET_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "MEP Update failed EP:%d %s.\n"), ep_info_p->id,
              bcm_errmsg(rv)));
        return (rv);
    }
    sal_memset(&mep_msg, 0, sizeof(shr_ccm_msg_ctrl_mep_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_mep_get_unpack(dma_buffer1, &mep_msg);

    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Mep get Msg lenght check failed, id:%d.\n"), ep_info_p->id));
        return (rv);
    }
    ep_info_p->faults = mep_msg.mep_data.faults;
    ep_info_p->persistent_faults = mep_msg.mep_data.persistent_faults;

    /* Fill rest of data from local db */
    ep_info_p->type = bcmOAMEndpointTypeEthernet;
    ep_info_p->group = ep_data->mep_data.group_id;
    ep_info_p->ccm_period = ep_data->mep_data.period;
    ep_info_p->gport = ep_data->mep_data.gport;
    ep_info_p->trunk_index = ep_data->mep_data.trunk_index;
    ep_info_p->name = ep_data->mep_data.name;
    ep_info_p->level = ep_data->mep_data.level;

    ep_info_p->vlan = ep_data->mep_data.vlan;
    ep_info_p->pkt_pri = ep_data->mep_data.pkt_pri;
    ep_info_p->outer_tpid = ep_data->mep_data.tpid;

    ep_info_p->inner_vlan = ep_data->mep_data.inner_vlan;
    ep_info_p->inner_pkt_pri = ep_data->mep_data.inner_vlan_pri;
    ep_info_p->inner_tpid = ep_data->mep_data.inner_tpid;

    if (!(ep_data->mep_data.flags & _FP_OAM_REMOTE_MEP)) {

        if (ep_data->mep_data.flags & _FP_OAM_INTERFACE_STATUS_TX) {
            ep_info_p->flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_TX;
            ep_info_p->interface_state = ep_data->mep_data.intf_state;
        }

        if (ep_data->mep_data.flags & _FP_OAM_PORT_STATUS_TX) {
            ep_info_p->flags |= BCM_OAM_ENDPOINT_PORT_STATE_TX;
            ep_info_p->port_state = ep_data->mep_data.port_state;
        }

        ep_info_p->lm_counter_base_id = ep_data->mep_data.flex_stat_id[0];
        sal_memcpy(ep_info_p->src_mac_address, ep_data->mep_data.sa,
                   sizeof(bcm_mac_t));
        sal_memcpy(ep_info_p->dst_mac_address, ep_data->mep_data.da,
                   sizeof(bcm_mac_t));
    } else {
        /* Set remote flag */
        ep_info_p->flags |= BCM_OAM_ENDPOINT_REMOTE;
        ep_info_p->interface_state = mep_msg.mep_data.intf_state;
        ep_info_p->port_state = mep_msg.mep_data.port_state;
        sal_memcpy(ep_info_p->src_mac_address, mep_msg.mep_data.sa, 
                   sizeof(bcm_mac_t));
        sal_memcpy(ep_info_p->dst_mac_address, mep_msg.mep_data.da, 
                   sizeof(bcm_mac_t));
    }

    return (rv);
}


int
_bcm_fp_oam_ccm_endpoint_traverse(int unit, bcm_oam_group_t group,
                                  bcm_oam_endpoint_traverse_cb cb, void *user_data)
{
    _bcm_fp_oam_control_t    *oc;        /* OAM control structure pointer. */
    _bcm_fp_oam_group_data_t *group_p;
    int rv = 0;                                 /* Return Value */
    ccm_group_data_t *grp_data_ptr = NULL;
    bcm_oam_endpoint_info_t endpoint_info = {0};
    bcm_oam_endpoint_t epid;

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    group_p = &(oc->group_data[group]);
    if (group_p->in_use) {
        grp_data_ptr = &(oc->group_data[group].group_data);
        epid = grp_data_ptr->lmep_id;
        if(epid >=0 && epid <oc->mep_count) {
            endpoint_info.id = epid;
            rv = bcm_fp_oam_ccm_endpoint_get(unit, &endpoint_info);

            rv = cb(unit, &endpoint_info, user_data);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM(unit %d) Error: User call back "
                                      "routine (EP=%d) - %s.\n"), unit,
                                      epid, bcm_errmsg(rv)));
                return (rv);
            }

        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_fp_oam_ccm_group_destroy
 * Purpose:
 *     Delete an OAM group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    _bcm_fp_oam_control_t *oc = NULL;      /* OAM control structure */
    _bcm_fp_oam_mep_data_t *ep_p;          /* Handle to S/w Group Data */
    int rv = 0;                            /* Return Value */
    uint16 r_len;                          /* Msg reply lenght */
    _bcm_fp_oam_group_data_t *grp_p;       /* Handle to S/w Group Data */

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate EP Index */
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint);

    /* Get Handle to Local DB */
    ep_p = &(oc->mep_data[endpoint]);
    if (!(ep_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_DELETE, endpoint, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_DELETE_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Mep Del Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
        return rv;
    }

    /* Clean up Group's LMEP/RMEP reference */
    grp_p = &(oc->group_data[ep_p->mep_data.group_id]);
    if (ep_p->mep_data.flags &  _FP_OAM_REMOTE_MEP) {
        _CLR_BIT_FLD(grp_p->group_data.rmep_bitmap, endpoint);
    } else {
        grp_p->group_data.lmep_id = _FP_OAM_INVALID_LMEP_ID;
    }

    /* Free the EP index */
    rv = shr_idxres_list_free(oc->mep_pool, endpoint);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "MEP index Free failed EP:%d %s.\n"), endpoint,
                  bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(ep_p, 0, sizeof(_bcm_fp_oam_mep_data_t));

    return rv;
}


/*
 * Function:
 *      _bcm_fp_oam_ccm_event_mask_set
 * Purpose:
 *      Set the CCM Events mask.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_fp_oam_ccm_event_mask_set(int unit)
{
    _bcm_fp_oam_control_t *oc;
    _bcm_oam_event_handler_t *event_handler_p;
    uint32 event_mask = 0;
    uint16 reply_len;
    int rv = BCM_E_NONE;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Get event mask from all callbacks */
    for (event_handler_p = oc->event_handler_list_p;
         event_handler_p != NULL;
         event_handler_p = event_handler_p->next_p) {

        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMxcon)) {
            event_mask |= G_XCON;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMError)) {
            event_mask |= G_CCM_ERROR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupRemote)) {
            event_mask |= G_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMTimeout)) {
            event_mask |= G_CCM_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupMACStatus)) {
            event_mask |= G_CCM_MAC;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMxconClear)) {
            event_mask |= G_XCON_CLR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMErrorClear)) {
            event_mask |= G_CCM_ERROR_CLR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupRemoteClear)) {
            event_mask |= G_RDI_CLR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMTimeoutClear)) {
            event_mask |= G_CCM_TIME_IN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupMACStatusClear)) {
            event_mask |= G_CCM_MAC_CLR;
        }

        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointPortDown)) {
            event_mask |= E_PORT_DOWN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointPortUp)) {
            event_mask |= E_PORT_UP;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceDown)) {
            event_mask |= E_INTF_DOWN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceUp)) {
            event_mask |= E_INTF_UP;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceTesting)) {
            event_mask |= E_INTF_TEST;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceUnkonwn)) {
            event_mask |= E_INTF_UK;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceDormant)) {
            event_mask |= E_INTF_DOR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceDormantToUp)) {
            event_mask |= E_INTF_DOR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceNotPresent)) {
            event_mask |= E_INTF_NP;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceLLDown)) {
            event_mask |= E_INTF_LL;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointCCMTimeout)) {
            event_mask |= E_CCM_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointCCMTimein)) {
            event_mask |= E_CCM_TIME_IN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointRemote)) {
            event_mask |= E_CCM_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointRemoteUp)) {
            event_mask |= E_CCM_RDI_CLR;
        }
    }

    /* Update CCM event mask in uKernel */
    if (event_mask != oc->event_mask) {
        /* Send CCM Event Mask message to uC */
        rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
                 MOS_MSG_SUBCLASS_CCM_EVENT_MASK_SET, 0, event_mask,
                 MOS_MSG_SUBCLASS_CCM_EVENT_MASK_SET_REPLY,
                 &reply_len);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Event Mask set Msg returned error %s.\n"), bcm_errmsg(rv)));
            rv = BCM_E_INTERNAL;
        }
    }

    oc->event_mask = event_mask;

    return (rv);
}


/*
 * Function:
 *      _bcm_fp_oam_ccm_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to CCM info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_fp_oam_ccm_callback_thread(void *param)
{
    int rv;
    _bcm_fp_oam_control_t *oc = (_bcm_fp_oam_control_t *)param;
    bcm_oam_event_types_t events;
    bcm_oam_event_type_t event_type;
    mos_msg_data_t event_msg;
    int data_index = 0;
    uint32 event_mask;
    _bcm_oam_event_handler_t *event_handler_p;
    int ep_id = BCM_OAM_ENDPOINT_INVALID;
    int group_id = BCM_OAM_GROUP_INVALID;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];

    thread_name[0] = 0;
    sal_thread_name(oc->event_thread_id, thread_name, sizeof (thread_name));

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(oc->unit, oc->ccm_uc_num,
                                     MOS_MSG_CLASS_CCM_EVENT, &event_msg,
                                     sal_sem_FOREVER);
        if (BCM_FAILURE(rv)) {
            break;  /*  Thread exit */
        }

        /* Get data from event message */
        data_index = (int)bcm_ntohs(event_msg.s.len);
        event_mask = bcm_ntohl(event_msg.s.data);

        /* One event Msg can either carry one or more Events related
           Group or Endpoint, Check whether Group event or EP event */

         /* Validate EP Index */
         if ( (data_index < 0) || (data_index >= oc->mep_count) ||
             !(oc->mep_data[data_index].in_use) ) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META("OAM Error: "
                "CCM Event with Invalid EP ID = %d.\n"), data_index));
            continue;
         }
         ep_id = data_index;
         group_id = oc->mep_data[data_index].mep_data.group_id;

        /* Set events */
        sal_memset(&events, 0, sizeof(events));
        if (event_mask & G_EVENT_MASK) {

            /* Group Events */
            if (event_mask & G_XCON) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMxcon - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMxcon] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMxcon);
                }
            }
            if (event_mask & G_CCM_ERROR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMError - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMError] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMError);
                }
            }

            if (event_mask & G_RDI) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupRemote] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupRemote);
                }
            }

            if (event_mask & G_CCM_TIMEOUT) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMTimeout - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMTimeout] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMTimeout);
                }
            }

            if (event_mask & G_CCM_MAC) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupMACStatus - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupMACStatus] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupMACStatus);
                }
            }

            /* Group Clear Events */
            if (event_mask & G_XCON_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMxconClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMxconClear);
                }
            }

            if (event_mask & G_CCM_ERROR_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMTimeout - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMErrorClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMErrorClear);
                }
            }

            if (event_mask & G_RDI_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupRemoteClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupRemoteClear);
                }
            }

            if (event_mask & G_CCM_TIME_IN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMTimeoutClear - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMTimeoutClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMTimeoutClear);
                }
            }

            if (event_mask & G_CCM_MAC_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupMACStatusClear - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupMACStatusClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupMACStatusClear);
                }
            }
        }
        if ( event_mask & E_EVENT_MASK) {

            /* Endpoint Events */
            if (event_mask & E_PORT_DOWN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointPortDown - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointPortDown] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointPortDown);
                }
            }

            if (event_mask & E_PORT_UP) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointPortUp - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointPortUp] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointPortUp);
                }
            }

            if (event_mask & E_INTF_DOWN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceDown - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceDown] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceDown);
                }
            }

            if (event_mask & E_INTF_UP) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceUp - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceUp] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceUp);
                }
            }

            if (event_mask & E_INTF_TEST) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceTesting - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceTesting] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceTesting);
                }
            }

            if (event_mask & E_INTF_UK) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceUnkonwn - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceUnkonwn] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceUnkonwn);
                }
            }

            if (event_mask & E_INTF_DOR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceDormant - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceDormant] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceDormant);
                }
            }

            if (event_mask & E_INTF_NP) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceNotPresent - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceNotPresent] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceNotPresent);
                }
            }

            if (event_mask & E_INTF_LL) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceLLDown - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceLLDown] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceLLDown);
                }
            }

            if (event_mask & E_CCM_TIMEOUT) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointCCMTimeout - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointCCMTimeout] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointCCMTimeout);
                }
            }

            if (event_mask & E_CCM_TIME_IN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointCCMTimein - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointCCMTimein] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointCCMTimein);
                }
            }

            if (event_mask & E_CCM_RDI) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointRemote] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointRemote);
                }
            }

            if (event_mask & E_CCM_RDI_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointRemoteUp - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointRemoteUp] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointRemoteUp);
                }
            }
        }

        /* Loop over registered callbacks,
         * If any match the events field, then invoke
         */
        for (event_handler_p = oc->event_handler_list_p; event_handler_p != NULL;
             event_handler_p = event_handler_p->next_p) {

            for (event_type = bcmOAMEventEndpointPortDown;
                 event_type < bcmOAMEventCount; ++event_type) {

                if (SHR_BITGET(events.w, event_type)) {
                    if (SHR_BITGET(event_handler_p->event_types.w, event_type)) {
                        event_handler_p->cb(oc->unit,
                        0,
                        event_type,
                        group_id, /* Group index */
                        ep_id, /* Endpoint index */
                        event_handler_p->user_data);
                    }
                }
            }
        }
    }

    oc->event_thread_id = NULL;
    LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit, "Thread Exit:%s\n"),
               thread_name));
    sal_thread_exit(0);
}


/*
 * Function:
 *      _bcm_fp_oam_cmm_group_regen
 * Purpose:
 *      Reconstruct CCM Group S/w State from UKENREL on WB
 *      Assumes Lock
 * Parameters:
 *      unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_cmm_group_regen(int unit, int grp_id) {

    int rv;
    _bcm_fp_oam_control_t *oc;  /* Pointer to OAM control structure */
    uint16 r_len;
    shr_ccm_msg_ctrl_group_data_t grp_msg;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET, grp_id, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "Group Get failed Id:%d %s.\n"), grp_id, bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(&grp_msg, 0, sizeof(shr_ccm_msg_ctrl_group_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_group_get_unpack(dma_buffer1, &grp_msg);

    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Group get Msg lenght check failed, Id:%d.\n"), grp_id));
        return (rv);
    }

    sal_memcpy(&(oc->group_data[grp_id].group_data) , &(grp_msg.group_data), 
               sizeof(ccm_group_data_t));

    rv = shr_idxres_list_reserve(oc->group_pool, grp_id, grp_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group index reservation failed, id:%d.\n"), grp_id));
        return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
    }
    oc->group_data[grp_id].in_use = 1;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_fp_oam_cmm_mep_regen
 * Purpose:
 *      Reconstruct CCM MEP S/w State from UKENREL on WB
 *      Assumes Lock
 * Parameters:
 *      unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_cmm_mep_regen(int unit, int mep_id) {

    int rv;
    _bcm_fp_oam_control_t *oc;  /* Pointer to OAM control structure */
    uint16 r_len;
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_GET, mep_id, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_GET_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "MEP Get failed EP:%d %s.\n"), mep_id, bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(&mep_msg, 0, sizeof(shr_ccm_msg_ctrl_mep_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_mep_get_unpack(dma_buffer1, &mep_msg);

    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Mep get Msg lenght check failed, id:%d.\n"), mep_id));
        return (rv);
    }

    sal_memcpy(&(oc->mep_data[mep_id].mep_data) , &(mep_msg.mep_data), 
               sizeof(ccm_mep_data_t));

    rv = shr_idxres_list_reserve(oc->mep_pool, mep_id, mep_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Mep index reservation failed, id:%d.\n"), mep_id));
        return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
    }
    oc->mep_data[mep_id].in_use = 1;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_fp_oam_ccm_reinit
 * Purpose:
 *      Reconstruct CCM S/w State from UKENREL on WB
 * Parameters:
 *      unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_ccm_reinit(int unit) {
    _bcm_fp_oam_control_t *oc;  /* Pointer to OAM control structure */
    int rv = BCM_E_NONE;        /* Operation return status */
    uint16 r_len = 0;
    shr_ccm_msg_ctrl_app_data_t app_data_msg;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;
    int i, j;
    ccm_group_data_t *group_ptr = NULL;

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    /* Recover group Bitmap from UKENREL */
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
        MOS_MSG_SUBCLASS_CCM_GET_APP_DATA, 0, 0,
        MOS_MSG_SUBCLASS_CCM_GET_APP_DATA_REPLY, &r_len);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "App data get failed %s\n"), bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    sal_memset(&app_data_msg, 0, sizeof(shr_ccm_msg_ctrl_app_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_app_data_unpack(dma_buffer1, &app_data_msg);
    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "App Data get Length mismatch\n")));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Recover Group Data from UKENREL */
    for (i=0; i<oc->group_count; i++) {
        if (_GET_BIT_FLD(app_data_msg.group_bitmap, i)) {
            rv = _bcm_fp_oam_cmm_group_regen(unit, i);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "Group Regen failed, Id:%d, %s\n"), i, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }

            /* Now that we have reconstrcuted the group, Reconstrcut its 
               LMEP & RMEPs */
            group_ptr = &(oc->group_data[i].group_data);
            if (group_ptr->lmep_id != _FP_OAM_INVALID_LMEP_ID) {

                /* Get LMEP */
                rv = _bcm_fp_oam_cmm_mep_regen(unit, group_ptr->lmep_id);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                          "LMEP Regen failed, Id:%d, %s\n"), group_ptr->lmep_id,
                          bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (rv);
                }

                /* Get RMEPs */
                for (j=0; j<oc->mep_count; j++) {
                    if (_GET_BIT_FLD(group_ptr->rmep_bitmap, j)) {
                        rv = _bcm_fp_oam_cmm_mep_regen(unit, j);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, 
                                "OAM Error: RMEP Regen failed Id:%d %s\n"), j,
                                bcm_errmsg(rv)));
                            _BCM_OAM_UNLOCK(oc);
                            return (rv);
                        }
                    }
                }
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

#endif /* INCLUDE_CCM */
/*******************************************************************************/
/* CCM Embedded App supporting functions End */
/*******************************************************************************/

/*******************************************************************************/
/* MPLS LM/DM App specific routines start                                      */
/*******************************************************************************/
#if defined(INCLUDE_MPLS_LM_DM)
#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_scache_v0sync
 * Purpose:
 *     Sync MPLS LM/DM scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_scache_v0_sync(int unit, uint8 *scache)
{
    int                                            i;
    uint16                                         idx;
    _bcm_fp_oam_control_t                         *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t              *ep;
    _bcm_fp_oam_pm_profile_control_t              *pmc = NULL;
    _bcm_fp_oam_mpls_lm_dm_scache_v0_global_data_t gdata;
   _bcm_fp_oam_mpls_lm_dm_scache_v0_ep_data_t      ep_sync;

    /* Get handle to control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));


    /* Sync global data to scache */
    gdata.uc_num           = oc->mpls_lm_dm_uc_num;
    gdata.ukernel_ready    = oc->mpls_lm_dm_ukernel_ready;
    gdata.rx_channel       = oc->mpls_lm_dm_rx_channel;
    gdata.max_ep_count     = oc->mpls_lm_dm_max_ep_count;
    gdata.base_endpoint_id = oc->mpls_lm_dm_base_endpoint_id;

    sal_memcpy(scache, &gdata, sizeof(gdata));
    scache += sizeof(gdata);

    /* Sync EP data to scache */
    _MPLS_LM_DM_EP_ITER(ep, idx) {
        ep_sync.in_use       = ep->in_use;
        ep_sync.flags        = ep->flags;
        ep_sync.type         = ep->type;
        ep_sync.vccv_type    = ep->vccv_type;
        ep_sync.gport        = ep->gport;
        ep_sync.trunk_index  = ep->trunk_index;
        ep_sync.outer_vlan   = ep->outer_vlan;
        ep_sync.outer_tpid   = ep->outer_tpid;
        ep_sync.outer_pri    = ep->outer_pri;
        ep_sync.inner_vlan   = ep->inner_vlan;
        ep_sync.inner_tpid   = ep->inner_tpid;
        ep_sync.inner_pri    = ep->inner_pri;
        ep_sync.egress_label = ep->egress_label;
        ep_sync.intf_id      = ep->intf_id;
        ep_sync.vpn          = ep->vpn;
        ep_sync.lm_exp       = ep->lm_exp;
        ep_sync.lm_int_pri   = ep->lm_int_pri;
        ep_sync.dm_exp       = ep->dm_exp;
        ep_sync.dm_int_pri   = ep->dm_int_pri;

        sal_memcpy(&(ep_sync.dst_mac), &(ep->dst_mac), sizeof(bcm_mac_t));
        sal_memcpy(&(ep_sync.src_mac), &(ep->src_mac), sizeof(bcm_mac_t));

        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            ep_sync.lm_ctr_base_id[i] = ep->lm_ctr_base_id[i];
            ep_sync.lm_ctr_offset[i]  = ep->lm_ctr_offset[i];
            ep_sync.lm_ctr_action[i]  = ep->lm_ctr_action[i];

            ep_sync.dm_ctr_base_id[i] = ep->dm_ctr_base_id[i];
            ep_sync.dm_ctr_offset[i]  = ep->dm_ctr_offset[i];
        }
        ep_sync.lm_ctr_size         = ep->lm_ctr_size;
        ep_sync.dm_ctr_size         = ep->dm_ctr_size;
        ep_sync.session_id          = ep->session_id;
        ep_sync.session_num_entries = ep->session_num_entries;
        ep_sync.pm_profile          = ep->pm_profile;

        sal_memcpy(scache, &ep_sync, sizeof(ep_sync));
        scache += sizeof(ep_sync);
    }

    /* Store pm profile */
    if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) {

        pmc = pm_profile_control[unit];
        if (!pmc) {
            return BCM_E_NOT_FOUND;
        }

        for(i = 0; i < _BCM_OAM_MAX_PM_PROFILES; i++) {
            sal_memcpy(scache, &(pmc->profile_info[i]),
                       sizeof(_bcm_fp_oam_pm_profile_int_info_t));

            scache += sizeof(_bcm_fp_oam_pm_profile_int_info_t);
        }
    } else {
        scache += _BCM_FP_OAM_PM_PROFILE_SCACHE_SIZE;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_scache_v0_recover
 * Purpose:
 *     Recover MPLS LM/DM software state from scache v0
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_scache_v0_recover(int unit, uint8 *scache)
{
    int                                            i;
    int                                            idx;
    _bcm_fp_oam_control_t                         *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t              *ep;
    _bcm_fp_oam_mpls_lm_dm_scache_v0_ep_data_t     ep_scache;
    _bcm_fp_oam_pm_profile_control_t              *pmc = NULL;
    bcm_oam_endpoint_info_t                        ep_info;
    int                                            sglp, dglp;
    bcm_trunk_t                                    trunk_id = BCM_TRUNK_INVALID;
    bcm_trunk_member_t                             trunk_member;
    bcm_stat_object_t                              object;
    uint32                                         pool;
    uint32                                         offset_mode;
    uint32                                         base_index;
    bcm_stat_group_mode_t                          group_mode;



    /* Get handle to control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Step over the global data, as these are either hardcoded or recovered
     * from other sources for now (BTE, config file)
     */
    scache += sizeof(_bcm_fp_oam_mpls_lm_dm_scache_v0_global_data_t);

    /* Recover EP data from scache */
    _MPLS_LM_DM_EP_ITER(ep, idx) {
        sal_memcpy(&ep_scache, scache, sizeof(ep_scache));
        scache += sizeof(ep_scache);

        ep->in_use       = ep_scache.in_use;
        if (!ep->in_use) {
            continue;
        }

        ep->flags        = ep_scache.flags;
        ep->type         = ep_scache.type;
        ep->vccv_type    = ep_scache.vccv_type;
        ep->gport        = ep_scache.gport;
        ep->trunk_index  = ep_scache.trunk_index;
        ep->outer_vlan   = ep_scache.outer_vlan;
        ep->outer_tpid   = ep_scache.outer_tpid;
        ep->outer_pri    = ep_scache.outer_pri;
        ep->inner_vlan   = ep_scache.inner_vlan;
        ep->inner_tpid   = ep_scache.inner_tpid;
        ep->inner_pri    = ep_scache.inner_pri;
        ep->egress_label = ep_scache.egress_label;
        ep->intf_id      = ep_scache.intf_id;
        ep->vpn          = ep_scache.vpn;
        ep->lm_exp       = ep_scache.lm_exp;
        ep->lm_int_pri   = ep_scache.lm_int_pri;
        ep->dm_exp       = ep_scache.dm_exp;
        ep->dm_int_pri   = ep_scache.dm_int_pri;

        sal_memcpy(&(ep->dst_mac), &(ep_scache.dst_mac), sizeof(bcm_mac_t));
        sal_memcpy(&(ep->src_mac), &(ep_scache.src_mac), sizeof(bcm_mac_t));

        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            ep->lm_ctr_base_id[i] = ep_scache.lm_ctr_base_id[i];
            ep->lm_ctr_offset[i]  = ep_scache.lm_ctr_offset[i];
            ep->lm_ctr_action[i]  = ep_scache.lm_ctr_action[i];

            ep->dm_ctr_base_id[i] = ep_scache.dm_ctr_base_id[i];
            ep->dm_ctr_offset[i]  = ep_scache.dm_ctr_offset[i];
        }
        ep->lm_ctr_size         = ep_scache.lm_ctr_size;
        ep->dm_ctr_size         = ep_scache.dm_ctr_size;
        ep->session_id          = ep_scache.session_id;
        ep->session_num_entries = ep_scache.session_num_entries;

        /* Reconstruct rest of the EP data using the recovered values */
        ep->id = _MPLS_LM_DM_SESS_ID_TO_EP_ID(idx);

        _bcm_esw_stat_get_counter_id_info(unit, ep->session_id,
                                          &group_mode, &object, &offset_mode,
                                          &pool, &base_index);
        ep->hw_base_session_id = _MPLS_LM_DM_HW_SESS_ID_GET(pool, base_index);

        bcm_oam_endpoint_info_t_init(&ep_info);
        ep_info.type = ep->type;
        ep_info.gport = ep->gport;
        ep_info.trunk_index = ep->trunk_index;
        BCM_IF_ERROR_RETURN(
               _bcm_fp_oam_endpoint_gport_resolve(unit, &ep_info,
                                                  &sglp, &dglp, &(ep->tx_gport),
                                                  &trunk_id, &trunk_member));

        BCM_IF_ERROR_RETURN(shr_idxres_list_reserve(oc->mpls_lm_dm_pool,
                                                    ep->id, ep->id));
    }


    if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_PROCESSED(oc)) {

        pmc = pm_profile_control[unit];
        if (!pmc) {
            return BCM_E_INIT;
        }

        for(i = 0; i < _BCM_OAM_MAX_PM_PROFILES; i++) {
            sal_memcpy(&(pmc->profile_info[i]), scache,
                       sizeof(_bcm_fp_oam_pm_profile_int_info_t));

            scache += sizeof(_bcm_fp_oam_pm_profile_int_info_t);
        }
    } else {
        scache += _BCM_FP_OAM_PM_PROFILE_SCACHE_SIZE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *       _bcm_fp_oam_mpls_lm_dm_scache_alloc
 * Purpose:
 *     Allocate scache memory for MPLS LM/DM module
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_scache_alloc(int unit)
{
    _bcm_fp_oam_control_t *oc;
    soc_scache_handle_t    scache_handle;
    uint8                 *scache;
    int                    alloc_size = 0;
    int                    rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    /* MPLS LM/DM Scache V0 size */
    alloc_size += _MPLS_LM_DM_SCACHE_V0_SIZE;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                          bcmFpOamWbSequenceMplsLmDm);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, alloc_size,
                                 &scache,
                                 BCM_WB_MPLS_LM_DM_DEFAULT_VERSION, NULL);

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_sync
 * Purpose:
 *     Store MPLS LM/DM configuration to level two storage cache memory.
 * Parameters:
 *     unit - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_sync(int unit)
{
    int                     rv = BCM_E_NONE;
    _bcm_fp_oam_control_t  *oc;
    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    /* Get OAM module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    /* Get handle to control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (!(oc->mpls_lm_dm_ukernel_ready)) {
        goto cleanup;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                          bcmFpOamWbSequenceMplsLmDm);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0, &scache,
                                 BCM_WB_MPLS_LM_DM_DEFAULT_VERSION, NULL);

    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    if (NULL == scache) {
        rv = BCM_E_INTERNAL;
        goto cleanup;
    }

    /* Sync MPLS LM/DM scache v0 */
    rv = _bcm_fp_oam_mpls_lm_dm_scache_v0_sync(unit, scache);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

 cleanup:
    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_wb_recover
 * Purpose:
 *     Recover MPLS LM/DM information from scache
 * Parameters:
 *     unit - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_wb_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_fp_oam_control_t  *oc;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (!oc->mpls_lm_dm_ukernel_ready) {
        goto cleanup;
    }

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_OAM,
                              bcmFpOamWbSequenceMplsLmDm);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_MPLS_LM_DM_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            _bcm_fp_oam_mpls_lm_dm_scache_alloc(unit);
            rv = BCM_E_NONE;
            goto cleanup;
        } else if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        if (NULL == scache) {
            rv =  BCM_E_NOT_FOUND;
            goto cleanup;
        }
    } else {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    rv = _bcm_fp_oam_mpls_lm_dm_scache_v0_recover(unit, scache);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

 cleanup:
    _BCM_OAM_UNLOCK(oc);
    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_init
 * Purpose:
 *     Initialize MPLS LM/DM app
 * Parameters:
 *     unit        - (IN) BCM device number
 *     oc          - (IN) OAM control block
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_init(uint8 unit, _bcm_fp_oam_control_t *oc)
{
    int                            rv = BCM_E_NONE;
    int                            uc = 0;
    uint8                         *buffer, *buffer_ptr;
    uint16                         buffer_len, reply_len;
    uint16                         size;
    mpls_lm_dm_sdk_msg_ctrl_init_t mpls_lm_dm_msg_init;
    int                            uc_status;

    /*
     * Start MPLS LM/DM application in BTE (Broadcom Task Engine) uController,
     * Determine which uController is running MPLS LM/DM by choosing the first
     * uC that returns successfully.
     */
    oc->mpls_lm_dm_ukernel_ready = 0;
    for (uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
        rv = soc_uc_status(unit, uc, &uc_status);
        if ((rv == SOC_E_NONE) && (uc_status != 0)) {
            rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_MPLS_LM_DM,
                                       _MPLS_LM_DM_UC_MSG_TIMEOUT_USECS,
                                       MPLS_LM_DM_SDK_VERSION,
                                       MPLS_LM_DM_UC_MIN_VERSION,
                                       _bcm_oam_mpls_lm_dm_appl_callback, NULL);
            if (SOC_E_NONE == rv) {
                /* MPLS_LM_DM started successfully */
                oc->mpls_lm_dm_uc_num = uc;
                break;
            }
        }
    }

    if (uc >= SOC_INFO(unit).num_ucs) {
        /* App not loaded return OK and skip rest of initialization */
        return BCM_E_NONE;
    }

    /*
     * Assign RX DMA channel to CPU COS Queue
     * (This is the RX channel to listen on for MPLS LM/DM packets).
     *
     * DMA channels (12) are assigned 4 per processor:
     * (see /src/bcm/common/rx.c)
     *   channels 0..3  --> PCI host
     *   channels 4..7  --> uController 0
     *   chnanels 8..11 --> uController 1
     *
     * The uControllers designate the 4 local DMA channels as follows:
     *   local channel  0     --> TX
     *   local channel  1..3  --> RX
     *
     * Each uController application needs to use a different
     * RX DMA channel to listen on.
     */
    oc->mpls_lm_dm_rx_channel = (BCM_RX_CHANNELS *
                                 (SOC_ARM_CMC(unit, oc->mpls_lm_dm_uc_num))) +
                                 MPLS_LM_DM_RX_CHANNEL_UC_VIEW;

    if (soc_feature(unit, soc_feature_oam_pm)) {
        oc->pm_mpls_lm_dm_data_collection_mode =
                             soc_property_get(unit,
                                              spn_MPLS_LMDM_DATA_COLLECTION_MODE,
                                              _BCM_OAM_PM_COLLECTION_MODE_NONE);
    }

    /*
     * Allocate DMA buffers
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    oc->mpls_lm_dm_dma_buffer_len = sizeof(mpls_lm_dm_sdk_msg_ctrl_t);
    if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_ENABLED(oc)) {
        /* If PM is enabled, increase the size of the buffer if required */
        if (sizeof(shr_oam_pm_msg_ctrl_t) > oc->mpls_lm_dm_dma_buffer_len) {
            oc->mpls_lm_dm_dma_buffer_len = sizeof(shr_oam_pm_msg_ctrl_t);
        }
    }
    oc->mpls_lm_dm_dma_buffer = soc_cm_salloc(unit,
                                              oc->mpls_lm_dm_dma_buffer_len,
                                              "MPLS_LM_DM DMA buffer");
    if (!oc->mpls_lm_dm_dma_buffer) {
        return (BCM_E_MEMORY);
    }
    sal_memset(oc->mpls_lm_dm_dma_buffer, 0, oc->mpls_lm_dm_dma_buffer_len);

    if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)) {
        oc->mpls_lm_dm_max_ep_count = MPLS_LM_DM_MAX_RAW_DATA_SESSIONS;
    } else {
        oc->mpls_lm_dm_max_ep_count = MPLS_LM_DM_MAX_SESSIONS;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) {
#endif /* BCM_WARM_BOOT_SUPPORT */

        /* Send Initialize MPLS LM/DM App on UKERNEL */
        /* Set control message data */
        sal_memset(&mpls_lm_dm_msg_init, 0, sizeof(mpls_lm_dm_msg_init));
        mpls_lm_dm_msg_init.num_sessions = oc->mpls_lm_dm_max_ep_count;
        mpls_lm_dm_msg_init.rx_channel   = MPLS_LM_DM_RX_CHANNEL_UC_VIEW;

        if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_ENABLED(oc)) {
            if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)) {
                mpls_lm_dm_msg_init.flags |= SHR_MPLS_LM_DM_PM_MODE_RAW;
            } else {
                mpls_lm_dm_msg_init.flags |= SHR_MPLS_LM_DM_PM_MODE_STATS;
            }
        }


        /* Pack control message data into DMA buffer */
        buffer = oc->mpls_lm_dm_dma_buffer;
        buffer_ptr = mpls_lm_dm_sdk_msg_ctrl_init_pack(buffer,
                                                       &mpls_lm_dm_msg_init);
        buffer_len = buffer_ptr - buffer;

        /* Send MPLS_LM_DM Init message to uC */
        rv = _bcm_fp_oam_msg_send_receive(unit,
                                          MOS_MSG_CLASS_MPLS_LM_DM,
                                          MOS_MSG_SUBCLASS_MPLS_LM_DM_INIT,
                                          buffer_len, 0,
                                          MOS_MSG_SUBCLASS_MPLS_LM_DM_INIT_REPLY,
                                          &reply_len);

        if (BCM_FAILURE(rv) || (reply_len != 0)) {
            /* Could not start MPLS_LM_DM appl */
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: "
                                  "MPLS LM DM Session alloc msg failed\n"), unit));
            return rv;
        }
#if defined(BCM_WARM_BOOT_SUPPORT)
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    oc->mpls_lm_dm_ukernel_ready = 1;

    oc->mpls_lm_dm_base_endpoint_id  =
                                soc_property_get(unit,
                                                 spn_MPLS_LM_DM_BASE_ENDPOINT_ID,
                                                 768);

    /* Check if the endpoint id space tramples over other apps */
#if defined(INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm)) {
        if (oc->ccm_ukernel_ready) {
            /* Base endpoint Id for CCM is 0 */
            if (oc->mep_count >= oc->mpls_lm_dm_base_endpoint_id) {
                return BCM_E_CONFIG;
            }
        }
    }
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh)) {
        if (!oc->ukernel_not_ready) {
            if (oc->mpls_lm_dm_base_endpoint_id <= oc->bhh_base_endpoint_id) {
                if ((oc->mpls_lm_dm_base_endpoint_id +
                     oc->mpls_lm_dm_max_ep_count)
                                             >= oc->bhh_base_endpoint_id) {
                    return BCM_E_CONFIG;
                }
            } else {
                if (oc->mpls_lm_dm_base_endpoint_id <=
                           (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)) {
                    return BCM_E_CONFIG;
                }
            }
        }
    }
#endif /* INCLUDE_BHH */

    /* Create Session Pool */
    rv = shr_idxres_list_create(&oc->mpls_lm_dm_pool,
                                oc->mpls_lm_dm_base_endpoint_id,
                                oc->mpls_lm_dm_base_endpoint_id +
                                                       oc->mpls_lm_dm_max_ep_count,
                                oc->mpls_lm_dm_base_endpoint_id,
                                oc->mpls_lm_dm_base_endpoint_id +
                                                       oc->mpls_lm_dm_max_ep_count,
                                "MPLS LM/DM pool");
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Allocate MPLS LM/DM EP data memory */
    size = sizeof(_bcm_fp_oam_mpls_lm_dm_ep_data_t) * oc->mpls_lm_dm_max_ep_count;
    _BCM_OAM_ALLOC(oc->mpls_lm_dm_ep_data,
                   _bcm_fp_oam_mpls_lm_dm_ep_data_t,
                   size, "MPLS LM/DM EP data");

    if (soc_feature(unit, soc_feature_oam_pm)) {
        rv = _bcm_fp_oam_mpls_lm_dm_pm_init(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }


#if defined(BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_fp_oam_mpls_lm_dm_wb_recover(unit);
    } else {
        rv = _bcm_fp_oam_mpls_lm_dm_scache_alloc(unit);
    }

    if (rv == BCM_E_NOT_FOUND) {
        /* Continue with initialization if scache not found */
        rv = BCM_E_NONE;
    }
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * Start event message callback thread
     */
    if (_BCM_FP_OAM_PM_MPLS_LM_DM_DATA_COLLECTION_MODE_IS_RAW_DATA(oc)) {
        if (oc->mpls_lm_dm_pm_event_thread_id == NULL) {
            if ((oc->mpls_lm_dm_pm_event_thread_id =
                 sal_thread_create("bcmMPLSLMDM", SAL_THREAD_STKSZ,
                                   MPLS_LMDM_EVENT_THREAD_PRIORITY,
                                   _bcm_fp_oam_mpls_lm_dm_pm_thread,
                                   (void *)oc)) == SAL_THREAD_ERROR) {
                oc->mpls_lm_dm_pm_event_thread_id = NULL;
                return (BCM_E_MEMORY);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_detach
 * Purpose:
 *     Uninitialize MPLS LM/DM app
 * Parameters:
 *     unit        - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_detach(uint8 unit)
{
    int                    rv = BCM_E_NONE;
    _bcm_fp_oam_control_t *oc = NULL;
    sal_usecs_t            timeout = sal_time_usecs() + 5000000;
    uint16                 reply_len;
    int                    uc_status;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (!oc->mpls_lm_dm_ukernel_ready) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_NONE;
    }


    while (NULL !=  oc->mpls_lm_dm_pm_event_thread_id) {
        soc_cmic_uc_msg_receive_cancel(unit, oc->mpls_lm_dm_uc_num,
                                       MOS_MSG_CLASS_MPLS_LM_DM_EVENT);

        if (sal_time_usecs() < timeout) {
            /*give some time to already running mpls_lm_dm callback thread
             * to schedule and exit */
            sal_usleep(10000);
        } else {
            /*timeout*/
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "MPLS_LM_DM event thread did not exit.\n")));
            _BCM_OAM_UNLOCK(oc);
            return BCM_E_INTERNAL;
        }
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Send MPLS_LM_DM Uninit message to uC. Ignore error as uC could have
         * died earlier.
         */
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, oc->mpls_lm_dm_uc_num, &uc_status));
        if (uc_status) {
            rv = _bcm_fp_oam_msg_send_receive(unit,
                                              MOS_MSG_CLASS_MPLS_LM_DM,
                                              MOS_MSG_SUBCLASS_MPLS_LM_DM_UNINIT,
                                              0, 0,
                                              MOS_MSG_SUBCLASS_MPLS_LM_DM_UNINIT_REPLY,
                                              &reply_len);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                         (BSL_META_U(unit,
                                     "MPLS LM/DM UNINIT failed - reply_len %u\n"),
                          reply_len));
                _BCM_OAM_UNLOCK(oc);
                return BCM_E_INTERNAL;
            }
            /* Ignore timeout/init error since that may indicate
             * uKernel was reloaded/msging is not up.*/
            if ((rv == BCM_E_TIMEOUT) || (rv == BCM_E_INIT)) {
                 LOG_DEBUG(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "MPLS LM/DM UNINIT failed - %s\n"),
                           bcm_errmsg(rv)));
                 rv = BCM_E_NONE;
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_endpoint_params_validate
 * Purpose:
 *     Validate MPLS LM/DM endpoint parameters
 * Parameters:
 *     unit           - (IN) BCM device number
 *     endpoint_info  - (IN) Endpoint parameters
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_endpoint_params_validate(uint8 unit,
                                                bcm_oam_endpoint_info_t *ep_info)
{
    int                               rv = BCM_E_NONE;
    _bcm_fp_oam_control_t            *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep;
    uint32                            pool;
    uint32                            offset_mode;
    uint32                            base_index;
    bcm_stat_object_t                 object;
    bcm_stat_group_mode_t             group_mode;
    bcm_stat_flex_direction_t         flex_direction;
    uint16                            hw_base_session_id;
    int16                             idx;


    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (!oc->mpls_lm_dm_ukernel_ready) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: BTE not ready \n"), unit));
        rv = BCM_E_INIT;
        goto cleanup;
    }

    if (ep_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {

        /* Check if it is within the range for MPLS LM/DM */
        if (!_MPLS_LM_DM_EP_ID_RANGE(ep_info->id)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Invalid id \n"), unit));
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(ep_info->id);
        ep = &(oc->mpls_lm_dm_ep_data[idx]);

        /* Check if id is already in use, unless it is a replace operation */
        if (!(ep_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
            if (ep->in_use) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "OAM(unit %d) Error: EP in use\n"), unit));
                rv = BCM_E_PARAM;
                goto cleanup;
            }
        }
    }

    if (ep_info->flags & BCM_OAM_ENDPOINT_REPLACE) {

        /* Check if it is within the range for MPLS LM/DM */
        if (!_MPLS_LM_DM_EP_ID_RANGE(ep_info->id)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Invalid id \n"), unit));
            rv = BCM_E_PARAM;
            goto cleanup;
        }

        ep = &(oc->mpls_lm_dm_ep_data[ep_info->id]);

        /* If it is a replace EP must already exist */
        if (!ep->in_use) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: EP not in use\n"), unit));
            rv = BCM_E_PARAM;
            goto cleanup;
        }
    }

    /* Validate if the gport is an MPLS port for PW MEP */
    if (ep_info->type == bcmOAMEndpointTypeMplsLmDmPw) {
        if (-1 == BCM_GPORT_MPLS_PORT_ID_GET(ep_info->gport)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: Invalid gport\n"), unit));
            rv = BCM_E_PARAM;
            goto cleanup;
        }
    }

    /* Interface validation check */
    if(ep_info->type == bcmOAMEndpointTypeMplsLmDmLsp) {
        if ((!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, ep_info->intf_id)) &&
            (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, ep_info->intf_id))) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "OAM(unit %d) Error: intf type not valid \n"),
                       unit));
            rv = BCM_E_PARAM;
            goto cleanup;
        }
    }

    /* At least one session entry needs to be present per endpouint */
    if (ep_info->session_num_entries == 0) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid session entries \n"),
                   unit));
        rv = BCM_E_PARAM;
        goto cleanup;
    }


    _bcm_esw_stat_get_counter_id_info(unit, ep_info->session_id,
                                      &group_mode, &object, &offset_mode,
                                      &pool, &base_index);

    rv = _bcm_esw_stat_validate_object(unit, object, &flex_direction);
    if (rv != BCM_E_NONE) {
        rv = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid session id\n"),unit));
        goto cleanup;
    }

    rv = _bcm_esw_stat_validate_group(unit, group_mode);
    if (rv != BCM_E_NONE) {
        rv = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid session id\n"),unit));
        goto cleanup;
    }

    if (flex_direction != bcmStatFlexDirectionIngress) {
        rv = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: Invalid session id\n"),unit));
        goto cleanup;
    }

    /* Check if the session id space for this endpoint is trampling on a
     * different endpoint.
     */
    hw_base_session_id = _MPLS_LM_DM_HW_SESS_ID_GET(pool, base_index);
    _MPLS_LM_DM_EP_ITER(ep, idx) {

        if (!ep->in_use) {
            continue;
        }

        if (ep_info->flags & BCM_OAM_ENDPOINT_REPLACE) {
            if (idx == ep_info->id) {
                continue;
            }
        }

        if ((ep->hw_base_session_id <= hw_base_session_id) &&
           ((ep->hw_base_session_id +
                         ep->session_num_entries - 1) >= hw_base_session_id)) {
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                            "OAM(unit %d) Error: Invalid session id\n"),unit));
            goto cleanup;
        }

        if ((ep->hw_base_session_id <=
                     (hw_base_session_id + ep_info->session_num_entries - 1))  &&
           ((ep->hw_base_session_id + ep->session_num_entries - 1) >=
                      (hw_base_session_id + ep_info->session_num_entries - 1))) {
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                            "OAM(unit %d) Error: Invalid session id\n"),unit));
            goto cleanup;
        }
    }

    /* Only pkt counter is supported */
    if (ep_info->lm_ctr_type != bcmOamLmCounterTypePkt) {
        rv = BCM_E_UNAVAIL;
        goto cleanup;
    }

    /* Only 32bit counter is supported */
    if (ep_info->lm_ctr_sample_size != bcmOamLmCounterSize32Bit) {
        rv = BCM_E_UNAVAIL;
        goto cleanup;
    }

 cleanup:
    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_endpoint_create
 * Purpose:
 *     Create MPLS LM/DM endpoint
 * Parameters:
 *     unit           - (IN) BCM device number
 *     endpoint_info  - (IN) Endpoint parameters
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_endpoint_create(uint8 unit,
                                       bcm_oam_endpoint_info_t *ep_info)
{
    _bcm_fp_oam_control_t            *oc;
    int                               rv = BCM_E_NONE;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t  ep;
    bcm_stat_object_t                 object;
    uint32                            pool;
    uint32                            offset_mode;
    uint32                            base_index;
    bcm_stat_group_mode_t             group_mode;
    bcm_port_t                        port;
    uint16                            tpid;
    bcm_oam_endpoint_t                id;
    bcm_vlan_control_vlan_t           vc;
    int                               sglp, dglp;
    bcm_trunk_t                       trunk_id = BCM_TRUNK_INVALID;
    bcm_trunk_member_t                trunk_member;
    bcm_mpls_port_t                   mpls_port;
    bcm_l3_egress_t                   l3_egress;
    bcm_l3_intf_t                     l3_intf;
    bcm_pbmp_t                        pbmp, ubmp;
    int                               tpid_select;
    uint16                            idx;

    rv = _bcm_fp_oam_mpls_lm_dm_endpoint_params_validate(unit, ep_info);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    sal_memset(&ep, 0, sizeof(ep));

    /* Copy parameters from user */
    ep.in_use    = 1;
    ep.type      = ep_info->type;
    ep.vccv_type = ep_info->vccv_type;

    ep.gport       = ep_info->gport;
    ep.trunk_index = ep_info->trunk_index;

    ep.intf_id      = ep_info->intf_id;
    ep.vpn          = ep_info->vpn;
    ep.egress_label = ep_info->egress_label.label;

    ep.session_id = ep_info->session_id;
    ep.session_num_entries = ep_info->session_num_entries;
    _bcm_esw_stat_get_counter_id_info(unit, ep_info->session_id,
                                      &group_mode, &object, &offset_mode,
                                      &pool, &base_index);
    ep.hw_base_session_id = _MPLS_LM_DM_HW_SESS_ID_GET(pool, base_index);


    BCM_IF_ERROR_RETURN(
                   _bcm_fp_oam_endpoint_gport_resolve(unit, ep_info,
                                                      &sglp, &dglp, &(ep.tx_gport),
                                                      &trunk_id, &trunk_member));
    port =  BCM_GPORT_MODPORT_PORT_GET(ep.tx_gport);
    if (port == -1) {
        return BCM_E_PORT;
    }

    ep.outer_pri  = ep_info->pkt_pri;
    ep.inner_pri  = ep_info->inner_pkt_pri;

    if (_MPLS_LM_DM_SECTION_ENDPOINT_TYPE(ep_info->type)) {
        ep.outer_vlan = ep_info->vlan;
        ep.outer_tpid = ep_info->outer_tpid;
        ep.inner_vlan = ep_info->inner_vlan;
        ep.inner_tpid = ep_info->inner_tpid;

        sal_memcpy(&(ep.dst_mac), &(ep_info->dst_mac_address), sizeof(bcm_mac_t));
        sal_memcpy(&(ep.src_mac), &(ep_info->src_mac_address), sizeof(bcm_mac_t));

    } else {
        /* Get L3 interfaces */
        bcm_l3_egress_t_init(&l3_egress);
        bcm_l3_intf_t_init(&l3_intf);

        if (ep_info->type == bcmOAMEndpointTypeMplsLmDmPw) {
            /* For PW endpoints derive intf_id from DVP */
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.mpls_port_id = ep_info->gport;
            BCM_IF_ERROR_RETURN(bcm_esw_mpls_port_get(unit, ep_info->vpn,
                                                      &mpls_port));
            ep.intf_id = mpls_port.egress_tunnel_if;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_get(unit,
                                                  ep.intf_id,
                                                  &l3_egress));
        l3_intf.l3a_intf_id = l3_egress.intf;
        BCM_IF_ERROR_RETURN(bcm_esw_l3_intf_get(unit, &l3_intf));

        /* Get TPID */
        BCM_IF_ERROR_RETURN(
                        bcm_esw_vlan_control_port_get(unit,
                                                      port,
                                                      bcmVlanPortOuterTpidSelect,
                                                      &tpid_select));
        if (tpid_select == BCM_PORT_OUTER_TPID) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_tpid_get(unit, port, &tpid));
        } else {
            BCM_IF_ERROR_RETURN(bcm_esw_vlan_control_vlan_get(unit,
                                                              l3_intf.l3a_vid,
                                                              &vc));
            tpid = vc.outer_tpid;
        }

        sal_memcpy(ep.dst_mac, l3_egress.mac_addr, sizeof(bcm_mac_t));
        sal_memcpy(ep.src_mac, l3_intf.l3a_mac_addr, sizeof(bcm_mac_t));

        ep.outer_tpid = tpid;
        ep.outer_vlan = l3_intf.l3a_vid;

        BCM_IF_ERROR_RETURN(bcm_esw_vlan_port_get(unit,
                                                  ep.outer_vlan,
                                                  &pbmp,
                                                  &ubmp));
        if (BCM_PBMP_MEMBER(ubmp, port)) {
            ep.outer_tpid = 0;  /* Set to 0 to indicate untagged */
        }

        if (l3_intf.l3a_inner_vlan != 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_inner_tpid_get(unit, port, &tpid));
            ep.inner_tpid = tpid;
            ep.inner_vlan = l3_intf.l3a_inner_vlan;
        }
    }

    /* All validations completed, add the EP */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (ep_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {

        if (!(ep_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {
            rv = shr_idxres_list_reserve(oc->mpls_lm_dm_pool,
                                         ep_info->id, ep_info->id);
        }
        id = ep_info->id;
    } else {
        rv = shr_idxres_list_alloc(oc->mpls_lm_dm_pool,
                                   (shr_idxres_element_t *)&id);
    }

    if (BCM_FAILURE(rv)) {
        rv = (rv == BCM_E_RESOURCE) ? (BCM_E_FULL) : rv;
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                            "MPLS_LM_DM(unit %d) Error: Endpoint alloc failed - %s.\n"),
                   unit, bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);
    sal_memcpy(&(oc->mpls_lm_dm_ep_data[idx]), &ep, sizeof(ep));

    /* Set PM profile default value */
    oc->mpls_lm_dm_ep_data[idx].pm_profile = _BCM_OAM_INVALID_INDEX;
    oc->mpls_lm_dm_ep_data[idx].in_use = 1;

    _BCM_OAM_UNLOCK(oc);
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_endpoint_get
 * Purpose:
 *     Create MPLS LM/DM endpoint
 * Parameters:
 *     unit           - (IN)  BCM device number
 *     endpoint       - (IN)  Endpoint id
 *     endpoint_info  - (OUT) Endpoint parameters
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_endpoint_get(int unit, bcm_oam_endpoint_t id,
                                    bcm_oam_endpoint_info_t *ep_info)
{
    _bcm_fp_oam_control_t            *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep;
    uint16                            idx;


    if (NULL == ep_info) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    if (!oc->mpls_lm_dm_ukernel_ready) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: BTE not ready \n"), unit));
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);

    if (!ep->in_use) {
        return BCM_E_NOT_FOUND;
    }

    /* Copy params from EP data */
    bcm_oam_endpoint_info_t_init(ep_info);
    ep_info->id = id;
    ep_info->type = ep->type;
    ep_info->vccv_type = ep->vccv_type;

    ep_info->gport       = ep->gport;
    ep_info->trunk_index = ep->trunk_index;

    ep_info->intf_id            = ep->intf_id;
    ep_info->vpn                = ep->vpn;
    ep_info->egress_label.label = ep->egress_label;

    ep_info->session_id          = ep->session_id;
    ep_info->session_num_entries = ep->session_num_entries;

    ep_info->vlan       = ep->outer_vlan;
    ep_info->outer_tpid = ep->outer_tpid;
    ep_info->pkt_pri    = ep->outer_pri;

    ep_info->inner_vlan    = ep->inner_vlan;
    ep_info->inner_tpid    = ep->inner_tpid;
    ep_info->inner_pkt_pri = ep->inner_pri;

    sal_memcpy(&(ep_info->dst_mac_address), &(ep->dst_mac),  sizeof(bcm_mac_t));
    sal_memcpy(&(ep_info->src_mac_address), &(ep->src_mac),  sizeof(bcm_mac_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_endpoint_destroy
 * Purpose:
 *     DEstroy MPLS LM/DM endpoint
 * Parameters:
 *     unit           - (IN)  BCM device number
 *     endpoint       - (IN)  Endpoint id
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_endpoint_destroy(int unit, bcm_oam_endpoint_t id)
{
    _bcm_fp_oam_control_t            *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep;
    uint16                            idx;
    int                               rv = BCM_E_NONE;


    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    if (!oc->mpls_lm_dm_ukernel_ready) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit,
                              "OAM(unit %d) Error: BTE not ready \n"), unit));
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);

    if (!(ep->in_use)) {
        _BCM_OAM_UNLOCK(oc);
        return BCM_E_PARAM;
    }

    if (ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED) {
        rv = _bcm_fp_oam_mpls_lm_dm_delay_delete(unit, id);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            return rv;
        }
    }

    if (ep->flags & SHR_MPLS_LM_DM_FLAG_LOSS_ADDED) {
        rv = _bcm_fp_oam_mpls_lm_dm_loss_delete(unit, id);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            return rv;
        }
    }

    /* Free the EP index */
    rv = shr_idxres_list_free(oc->mpls_lm_dm_pool, id);
    if (BCM_FAILURE(rv)) {
        _BCM_OAM_UNLOCK(oc);
        return rv;
    }

    sal_memset(ep, 0, sizeof(*ep));

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_period_validate
 * Purpose:
 *      Validate if the period passed by user is valid
 * Parameters:
 *      period    - (IN) period from user
 *
 * Returns:
 *       BCM_E_NONE  - If the period is valid
 *       BCM_E_PARAM - If the period is invalid
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_period_validate(int period)
{
    switch(period) {
        case BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_100MS:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1S:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10S:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1M:
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10M:
            return BCM_E_NONE;
        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_encap_data_dump
 * Purpose:
 *      Dumps MPLS LM/DM encap
 * Parameters:
 *      encap_data      - (IN) Buffer containing MPLS_LM_DM encapsulation.
 *      encap_length    - (IN) Length of the encap constructed
 */
#if defined(MPLS_LM_DM_DEBUG)
STATIC void
_bcm_fp_oam_mpls_lm_dm_encap_data_dump(uint8 *encap_data, uint16 encap_length)
{
    int i = 0;

    LOG_CLI((BSL_META("\nEncapsulation (length=%u):\n"), encap_length));
    for (i = 0; i < encap_length; i++) {
        if (i % 16 == 0) {
            LOG_CLI((BSL_META("\n")));
        }
        LOG_CLI((BSL_META("%02x "), encap_data[i]));
    }
    LOG_CLI((BSL_META("\n")));
}
#endif /* MPLS_LM_DM_DEBUG */

STATIC int
_bcm_fp_oam_mpls_lm_dm_label_get(uint32 label, uint8 exp, uint8 s,
                                 uint8 ttl, _mpls_label_t *mpls_hdr)
{
    sal_memset(mpls_hdr, 0, sizeof(*mpls_hdr));

    mpls_hdr->label = label;
    mpls_hdr->exp   = exp;
    mpls_hdr->s     = s;
    if (ttl) {
        mpls_hdr->ttl   = ttl;
    } else {
        mpls_hdr->ttl   = _MPLS_LM_DM_MPLS_DFLT_TTL;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_lsp_labels_get
 * Purpose:
 *      Get the LSP label stack
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      ep              - (IN)  Endpoint params
 *      max_labels      - (IN)  Max number of supported labels
 *      label_stack     - (OUT) LSP label stack
 *      label_count     - (OUT) Number of LSP labels
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_lsp_labels_get(int unit,
                                      _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep,
                                      int max_labels, _mpls_label_t *label_stack,
                                      int *label_count)
{

    bcm_l3_egress_t         l3_egress;
    bcm_l3_intf_t           l3_intf;
    bcm_mpls_egress_label_t label_array[_MPLS_LM_DM_MPLS_MAX_LABELS];
    int                     count = 0;
    int                     i, j;
    int                     egr_label_idx = 0;

    bcm_l3_egress_t_init(&l3_egress);
    bcm_l3_intf_t_init(&l3_intf);

    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_get(unit,
                                              ep->intf_id,
                                              &l3_egress));

    BCM_IF_ERROR_RETURN(
                   bcm_esw_mpls_tunnel_initiator_get(unit, l3_egress.intf,
                                                     _MPLS_LM_DM_MPLS_MAX_LABELS,
                                                     label_array,
                                                     &count));
    /* LSP endpoint specific */
    if (ep->type == bcmOAMEndpointTypeMplsLmDmLsp) {
        /* Add VRL if applicable */
        if ((l3_egress.mpls_label != BCM_MPLS_LABEL_INVALID) &&
            (count < _MPLS_LM_DM_MPLS_MAX_LABELS)) {
            label_array[count - 1].label = l3_egress.mpls_label;
            label_array[count - 1].exp   = l3_egress.mpls_exp;
            label_array[count - 1].ttl   = l3_egress.mpls_ttl;
            count++;
        }

        /* Find label on which OAM is running */
        for (i = 0; i < count; i++) {
            if (label_array[i].label == ep->egress_label) {
                *label_count = count - i;
                egr_label_idx = i;
                break;
            }
        }
        if (i >= count) {
            /* Could not find a matching label */
            return BCM_E_PARAM;
        }
        /* Remove all labels from BoS to egress_label */
        for (i = egr_label_idx, j = 0; i < count; i++, j++) {
            /* Copy labels */
            label_array[j].label = label_array[i].label;
            label_array[j].exp   = label_array[i].exp;
            label_array[j].ttl   = label_array[i].ttl;
        }
    } else { /* PW endpoint */
        *label_count = count;
    }

    if ((*label_count) > max_labels) {
        /* Too many labels */
        return BCM_E_PARAM;
    }


    for (i = 0; i < (*label_count); i++) {
        _bcm_fp_oam_mpls_lm_dm_label_get(label_array[i].label,
                                         label_array[i].exp, 0,
                                         label_array[i].ttl,
                                         &(label_stack[i]));

    }
    return BCM_E_NONE;
}

 /*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_l2_hdr_pack
 * Purpose:
 *      Packs the L2 header
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      ep              - (IN)  Endpoint params
 *      encap_data      - (OUT) Buffer returning MPLS_LM_DM encapsulation.
 *      encap_length    - (OUT) Length of the encap constructed
 * Returns:
 *      BCM_E_XXX
 */
STATIC void
_bcm_fp_oam_mpls_lm_dm_l2_hdr_pack(int unit,
                                   _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep,
                                   uint8 *encap_data, uint16 *encap_length)
{
    uint32     tmp;
    int        i;
    uint8     *cur_ptr = encap_data;

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(cur_ptr, ep->dst_mac[i]);
    }

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(cur_ptr, ep->src_mac[i]);
    }

    /* outer_tpid = 0 indicates no outer tag */
    tmp = 0;
    if (0 != ep->outer_tpid) {
        tmp =  (ep->outer_tpid         << 16) |
              ((ep->outer_pri  & 0x7)  << 13) |
               (ep->outer_vlan & 0xfff);
        _SHR_PACK_U32(cur_ptr, tmp);
    }

    /* inner_tpid = 0 indicates no inner tag*/
    tmp = 0;
    if (0 != ep->inner_tpid) {
        tmp =  (ep->inner_tpid         << 16) |
              ((ep->inner_pri  & 0x7)  << 13) |
               (ep->inner_vlan & 0xfff);
        _SHR_PACK_U32(cur_ptr, tmp);
    }

    _SHR_PACK_U16(cur_ptr, SHR_MPLS_LM_DM_L2_ETYPE_MPLS_UCAST);

    *encap_length = cur_ptr - encap_data;

}

 /*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_ach_header_pack
 * Purpose:
 *      Packs the ACH header.
 * Parameters:
 *      ach_header  - (IN)  ACH header
 *      buffer      - (OUT) Buffer containing packed ACH
 * Returns:
 *      Incremented buffer pointer
 */
STATIC uint8 *
_bcm_fp_oam_mpls_lm_dm_ach_header_pack(_ach_header_t *ach, uint8 *buffer)
{
    uint32  tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) |
          ((ach->version & 0xf) << 24)  |
           (ach->reserved << 16)        |
            ach->channel_type;

    _SHR_PACK_U32(buffer, tmp);

    return (buffer);
}

 /*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_label_pack
 * Purpose:
 *      Packs the MPLS label header.
 * Parameters:
 *      label     - (IN)  Label header
 *      buffer    - (OUT) Buffer containing packed Label
 * Returns:
 *      Incremented buffer pointer
 */
STATIC uint8 *
_bcm_fp_oam_mpls_lm_dm_mpls_label_pack(_mpls_label_t *label, uint8 *buffer)
{
    uint32  tmp;

    tmp = ((label->label & 0xfffff) << 12) |
          ((label->exp & 0x7) << 9)        |
          ((label->s & 0x1) << 8)          |
            label->ttl;
    _SHR_PACK_U32(buffer, tmp);

    return (buffer);
}

 /*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_mpls_hdr_build_pack
 * Purpose:
 *      Builds and packs the MPLS header
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      ep              - (IN)  Endpoint params
 *      channel_type    - (IN)  ACH channel type used in the packet
 *      oam_exp         - (IN)  EXP of the label running OAM
 *      oam_ttl         - (IN)  TTL of the label running OAM
 *      encap_data      - (OUT) Buffer returning MPLS_LM_DM encapsulation.
 *      encap_length    - (OUT) Length of the encap constructed
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning MPLS_LM_DM encapsulation includes only all the
 *      encapsulation headers/labels and does not include
 *      the MPLS_LM_DM control packet.
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_mpls_hdr_build_pack(int unit,
                                           _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep,
                                           uint32 channel_type,
                                           uint8  oam_exp,
                                           uint8 *encap_data,
                                           uint16 *encap_length)
{
    _mpls_label_t   label_stack[_MPLS_LM_DM_MPLS_MAX_LABELS];
    _ach_header_t   ach;
    uint32          packet_flags = 0;
    int             label_count = 0;
    int             lsp_label_count = 0;
    int             oam_label_idx = -1;
    uint8          *cur_ptr;
    int             i;
    bcm_mpls_port_t mpls_port;


    /*
     * Get MPLS_LM_DM encapsulation packet format flags
     *
     * Also, perform the following for each MPLS_LM_DM tunnel type:
     * - Check for valid parameter values
     * - Set specific values required by the MPLS_LM_DM tunnel definition 
     *   (e.g. such as ttl=1,...)
     */
    switch (ep->type) {
        case bcmOAMEndpointTypeMplsLmDmLsp:
            packet_flags |= (_MPLS_LM_DM_ENCAP_PKT_GAL |
                             _MPLS_LM_DM_ENCAP_PKT_MPLS_LSP);
            break;

        case bcmOAMEndpointTypeMplsLmDmPw:
            packet_flags |= (_MPLS_LM_DM_ENCAP_PKT_PW |
                             _MPLS_LM_DM_ENCAP_PKT_MPLS_LSP);
            switch(ep->vccv_type) {

                case bcmOamBhhVccvChannelAch:
                case bcmOamBhhVccvTtl:
                    break;

                case bcmOamBhhVccvRouterAlert:
                    packet_flags |= _MPLS_LM_DM_ENCAP_PKT_MPLS_ROUTER_ALERT;
                    break;

                case bcmOamBhhVccvGal13:
                    packet_flags |= _MPLS_LM_DM_ENCAP_PKT_GAL;
                    break;

                default:
                    return (BCM_E_PARAM);
                    break;
            }
            break;

        case bcmOAMEndpointTypeMplsLmDmSectionPort:
        case bcmOAMEndpointTypeMplsLmDmSectionInnervlan:
        case bcmOAMEndpointTypeMplsLmDmSectionOuterVlan:
        case bcmOAMEndpointTypeMplsLmDmSectionOuterPlusInnerVlan:
            packet_flags |= _MPLS_LM_DM_ENCAP_PKT_GAL;
            break;
        default:
            return (BCM_E_PARAM);
    }


    /*
     * Get necessary headers/labels information.
     *
     * The order is important, as the encap is built in this order.
     */
    /* Construct ACH */
    sal_memset(&ach, 0, sizeof(ach));
    ach.f_nibble     = SHR_MPLS_LM_DM_ACH_FIRST_NIBBLE;
    ach.version      = SHR_MPLS_LM_DM_ACH_VERSION;
    ach.reserved     = 0;
    ach.channel_type = channel_type;

    /* Construct GAL */
    if (packet_flags & _MPLS_LM_DM_ENCAP_PKT_GAL) {
        _bcm_fp_oam_mpls_lm_dm_label_get(SHR_MPLS_LM_DM_MPLS_GAL_LABEL,
                                         0, 1, 1, &(label_stack[label_count]));
        label_count++;
    }

    /* Add PW label */
    if (packet_flags & _MPLS_LM_DM_ENCAP_PKT_PW) {
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.mpls_port_id = ep->gport;
        BCM_IF_ERROR_RETURN(bcm_esw_mpls_port_get(unit, ep->vpn, &mpls_port));

        if (ep->vccv_type == bcmOamBhhVccvTtl) {
            mpls_port.egress_label.ttl = 0x1;
        }

        _bcm_fp_oam_mpls_lm_dm_label_get(mpls_port.egress_label.label,
                                         mpls_port.egress_label.exp, 0,
                                         mpls_port.egress_label.ttl,
                                         &(label_stack[label_count]));
        oam_label_idx = label_count; /* OAM label is the PW label */
        label_count++;
    }


    /* Construct RAL */
    if (packet_flags & _MPLS_LM_DM_ENCAP_PKT_MPLS_ROUTER_ALERT) {
        _bcm_fp_oam_mpls_lm_dm_label_get(SHR_MPLS_LM_DM_MPLS_ROUTER_ALERT_LABEL,
                                         0, 0, 0, &(label_stack[label_count]));
        label_count++;
    }

    /* Add LSP labels */
    if (packet_flags & _MPLS_LM_DM_ENCAP_PKT_MPLS_LSP) {
        BCM_IF_ERROR_RETURN(
           _bcm_fp_oam_mpls_lm_dm_lsp_labels_get(
                                       unit, ep,
                                       _MPLS_LM_DM_MPLS_MAX_LABELS - label_count,
                                       &(label_stack[label_count]),
                                       &lsp_label_count));

        /* Find the OAM label */
        if (ep->type == bcmOAMEndpointTypeMplsLmDmLsp) {
            oam_label_idx = label_count;
        }
        label_count += lsp_label_count;
    }

    /* Set the EXP of OAM label to user specified value */
    if ((ep->type == bcmOAMEndpointTypeMplsLmDmLsp) ||
        (ep->type == bcmOAMEndpointTypeMplsLmDmPw)) {
        label_stack[oam_label_idx].exp = oam_exp;
    }

    label_stack[0].s = 1; /* Set BoS*/

    /* Pack the MPLS header in Network Byte Order */
    cur_ptr = encap_data;
    for (i = (label_count -1); i >= 0; i--) {
        cur_ptr = _bcm_fp_oam_mpls_lm_dm_mpls_label_pack(&(label_stack[i]),
                                                         cur_ptr);
    }
    cur_ptr = _bcm_fp_oam_mpls_lm_dm_ach_header_pack(&ach, cur_ptr);
    *encap_length = cur_ptr - encap_data;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_encap_create
 * Purpose:
 *      Creates a MPLS_LM_DM packet encapsulation.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      ep              - (IN)  Endpoint params
 *      encap_data      - (OUT) Buffer returning MPLS_LM_DM encapsulation.
 *      encap_length    - (OUT) Length of the encap constructed
 *      ach             - (IN)  ACH of the packet
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning MPLS_LM_DM encapsulation buffer includes all the
 *      corresponding headers/labels EXCEPT for the MPLS_LM_DM control packet.
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_encap_create(int unit,
                                    _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep,
                                    uint8 *encap_data, uint16 *encap_length,
                                    uint32 ach, uint8 oam_exp)
{
    uint16 l2_encap_length, mpls_encap_length;

    /* Build and pack l2 header */
    _bcm_fp_oam_mpls_lm_dm_l2_hdr_pack(unit, ep, encap_data, &l2_encap_length);

    /* Build and pack MPLS header */
    BCM_IF_ERROR_RETURN(
         _bcm_fp_oam_mpls_lm_dm_mpls_hdr_build_pack(unit, ep, ach, oam_exp,
                                                    encap_data + l2_encap_length,
                                                    &mpls_encap_length));

    *encap_length = l2_encap_length + mpls_encap_length;

#if defined(MPLS_LM_DM_DEBUG)
    _bcm_fp_oam_mpls_lm_dm_encap_data_dump(encap_data, *encap_length);
#endif /* MPLS_LM_DM_DEBUG */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_loss_add
 * Purpose:
 *     Create MPLS LM/DM Loss session
 * Parameters:
 *     unit    - (IN) BCM device number
 *     loss_p  - (IN) Loss params
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_fp_oam_mpls_lm_dm_loss_add(uint8 unit, bcm_oam_loss_t *loss_p)
{
    _bcm_fp_oam_control_t              *oc = NULL;
    uint8                               num_sample_action = 0;
    mpls_lm_dm_sdk_msg_ctrl_loss_add_t  msg;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t   *ep;
    uint32                              ach = 0;
    uint32                              lm_int_flags = 0;
    bcm_module_t                        my_modid = 0;
    bcm_gport_t                         cpu_dglp;
    soc_olp_l2_hdr_t                    olp_l2_hdr;
    soc_olp_tx_hdr_t                    olp_oam_hdr;
    uint8                              *buffer, *cur_ptr;
    uint16                              buffer_len, reply_len;
    uint32                              ctr_pool;
    uint32                              ctr_mode;
    uint32                              base_index;
    bcm_stat_group_mode_t               ctr_group;
    bcm_stat_object_t                   ctr_object;
    uint16                              idx;
    int                                 rv = BCM_E_NONE;
    int                                 i;


    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validation checks */
    if (!oc->mpls_lm_dm_ukernel_ready) {
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(loss_p->id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);

    if (!ep->in_use) {
        return BCM_E_PARAM;
    }

    if (loss_p->lm_counter_size > BCM_OAM_LM_COUNTER_MAX) {
        return BCM_E_PARAM;
    }

    if ((loss_p->flags & BCM_OAM_COMBINED_LOSS_DELAY) &&
       !(loss_p->flags & BCM_OAM_LOSS_SLM)) {
        /* DLM + DM not supported */
        return BCM_E_PARAM;
    }

    /* ILM, only possible action is Increment */
    if ((loss_p->flags & BCM_OAM_LOSS_SLM)) {
        for(i = 0; i < loss_p->lm_counter_size; i++) {
            if (loss_p->lm_counter_action[i] != bcmOamCounterActionIncrement) {
                return BCM_E_PARAM;
            }
        }
    } else { /* DLM, exactly one sample action must be present */
        for(i = 0; i < loss_p->lm_counter_size; i++) {
            if (loss_p->lm_counter_action[i] == bcmOamCounterActionSample) {
                num_sample_action++;
            }
        }
        if (num_sample_action != 1) {
            return BCM_E_PARAM;
        }
    }

    if (ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED) {
        if (loss_p->flags & BCM_OAM_COMBINED_LOSS_DELAY) {
            if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED)) {
                /* Delay created without COMBINED flag, reject loss add with
                 * COMBINED flag
                 */
                return BCM_E_PARAM;
            }
        } else {
            if (ep->flags & SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED) {
                /* Delay created with COMBINED flag, reject loss add without
                 * COMBINED flag
                 */
                return BCM_E_PARAM;
            }
        }
    }

    /* Validate period */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_mpls_lm_dm_period_validate(loss_p->period));

    /* Set internal flags */
    if (loss_p->flags & BCM_OAM_COMBINED_LOSS_DELAY) {
        lm_int_flags |= SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED;
        lm_int_flags |= SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM_DM;
        ach = SHR_MPLS_LM_DM_ACH_TYPE_ILM_DM;
    } else if (loss_p->flags & BCM_OAM_LOSS_SLM) {
        lm_int_flags |= SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM;
        ach = SHR_MPLS_LM_DM_ACH_TYPE_ILM;
    } else {
        lm_int_flags |= SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DLM;
        ach = SHR_MPLS_LM_DM_ACH_TYPE_DLM;
    }

    /* Active or passive mode */
    if (!(loss_p->flags & BCM_OAM_LOSS_TX_ENABLE)) {
        lm_int_flags |= SHR_MPLS_LM_DM_PASSIVE_FLAG;
    }

    /* Single or dual ended mode */
    if (loss_p->flags & BCM_OAM_LOSS_SINGLE_ENDED) {
        lm_int_flags |= SHR_MPLS_LM_DM_SINGLE_ENDED_FLAG;
    }

    /* Copy user given values */
    ep->lm_exp     = loss_p->pkt_pri;
    ep->lm_int_pri = loss_p->int_pri;


    for(i = 0; i < loss_p->lm_counter_size; i++) {
        ep->lm_ctr_base_id[i] = loss_p->lm_counter_base_id[i];
        ep->lm_ctr_offset[i]  = loss_p->lm_counter_offset[i];
        ep->lm_ctr_action[i]  = loss_p->lm_counter_action[i];
    }
    ep->lm_ctr_size = loss_p->lm_counter_size;

    /* Construct the message */
    sal_memset(&msg, 0, sizeof(msg));
    msg.sess_id             = _MPLS_LM_DM_EP_ID_TO_SESS_ID(loss_p->id);
    msg.flags               = lm_int_flags;
    msg.period              = loss_p->period;
    msg.session_id          = ep->hw_base_session_id;
    msg.session_num_entries = ep->session_num_entries;

    /* Create packet encap */
    BCM_IF_ERROR_RETURN(
                      _bcm_fp_oam_mpls_lm_dm_encap_create(unit, ep,
                                                          msg.l2_encap_data,
                                                          &(msg.l2_encap_length),
                                                          ach, ep->lm_exp));

    /* Create OLP encap */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID, my_modid,
                                               CMIC_PORT(unit), &cpu_dglp));
    BCM_IF_ERROR_RETURN(_bcm_olp_l2_hdr_get(unit, cpu_dglp, 0, &olp_l2_hdr));

    cur_ptr = _bcm_fp_oam_olp_l2_header_pack(msg.olp_encap_data, &olp_l2_hdr);

    /* Construct OLP OAM header */
    sal_memset(&olp_oam_hdr, 0, sizeof(olp_oam_hdr));
    SOC_OLP_TX_PORT(&olp_oam_hdr)  = BCM_GPORT_MODPORT_PORT_GET(ep->tx_gport);
    SOC_OLP_TX_MODID(&olp_oam_hdr) = BCM_GPORT_MODPORT_MODID_GET(ep->tx_gport);

    /* Pack counter information in OLP header */
    for (i=0; i< ep->lm_ctr_size; i++) {
        _bcm_esw_stat_get_counter_id_info(unit, ep->lm_ctr_base_id[i],
                                          &ctr_group, &ctr_object,
                                          &ctr_mode, &ctr_pool,
                                          &base_index);
        if (i == 0) {
            SOC_OLP_TX_CTR1_ID(&olp_oam_hdr) =
                     SOC_OLP_TX_CTR(ctr_pool, base_index + ep->lm_ctr_offset[i]);
            SOC_OLP_TX_CTR1_ACTION(&olp_oam_hdr) = ep->lm_ctr_action[i];
            SOC_OLP_TX_CTR1_LOCATION(&olp_oam_hdr) = 1; /* Egress */

        } else if (i == 1) {
            SOC_OLP_TX_CTR2_ID_SET(&olp_oam_hdr,
                                   SOC_OLP_TX_CTR(ctr_pool,
                                                  base_index +
                                                     ep->lm_ctr_offset[i]));
            SOC_OLP_TX_CTR2_ACTION(&olp_oam_hdr) = ep->lm_ctr_action[i];
            SOC_OLP_TX_CTR2_LOCATION(&olp_oam_hdr) = 1; /* Egress */
        } else if (i == 2) {
            SOC_OLP_TX_CTR3_ID_SET(&olp_oam_hdr,
                                   SOC_OLP_TX_CTR(ctr_pool,
                                                  base_index +
                                                     ep->lm_ctr_offset[i]));
            SOC_OLP_TX_CTR3_ACTION(&olp_oam_hdr) = ep->lm_ctr_action[i];
            SOC_OLP_TX_CTR3_LOCATION(&olp_oam_hdr) = 1; /* Egress */
        }
    }
    SOC_OLP_TX_PRI(&olp_oam_hdr) = ep->lm_int_pri;

    /* Set generic repl. offset, BTE will adjust offset if it is LM+DM */
    SOC_OLP_TX_OAM_OFFSET(&olp_oam_hdr) =
               ((msg.l2_encap_length + _MPLS_LM_DM_OLP_REPL_OFFSET_DLM - 14) / 2);

    /* Pack OLP OAM Encap */
    shr_olp_tx_header_pack(cur_ptr, &olp_oam_hdr);

#if defined(MPLS_LM_DM_DEBUG)
    _bcm_fp_oam_mpls_lm_dm_encap_data_dump(msg.olp_encap_data,
                                           MPLS_LM_DM_OLP_HDR_LEN);
#endif /* MPLS_LM_DM_DEBUG */
    /* Send Loss Add message */
    buffer = oc->mpls_lm_dm_dma_buffer;
    buffer_len = mpls_lm_dm_sdk_msg_ctrl_loss_add_pack(buffer, &msg) - buffer;

    rv = _bcm_fp_oam_msg_send_receive(unit,
                          MOS_MSG_CLASS_MPLS_LM_DM,
                          MOS_MSG_SUBCLASS_MPLS_LM_DM_LOSS_MEASUREMENT_ADD,
                          buffer_len, 0,
                          MOS_MSG_SUBCLASS_MPLS_LM_DM_LOSS_MEASUREMENT_ADD_REPLY,
                          &reply_len);

    if ((BCM_SUCCESS(rv)) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    ep->flags |= lm_int_flags;
    ep->flags |= SHR_MPLS_LM_DM_FLAG_LOSS_ADDED;

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_loss_get
 * Purpose:
 *     Get MPLS LM/DM Loss session
 * Parameters:
 *     unit    - (IN) BCM device number
 *     loss_p  - (IN) Loss params
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_fp_oam_mpls_lm_dm_loss_get(uint8 unit, bcm_oam_loss_t *loss_p)
{
    _bcm_fp_oam_control_t               *oc = NULL;
    shr_mpls_lm_dm_msg_ctrl_loss_get_t   msg_req;
    shr_mpls_lm_dm_msg_ctrl_loss_data_t  msg_rsp;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t    *ep;
    uint8                               *buffer = NULL;
    uint16                               req_len, rsp_len, reply_len;
    uint16                               idx;
    int                                  rv = 0;
    int                                  i = 0;

    /* Get OAM control pointer */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validation checks */
    if (!oc->mpls_lm_dm_ukernel_ready) {
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(loss_p->id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);
    if (!ep->in_use) {
        return BCM_E_PARAM;
    }

    /* Check if the LM is enabled for this EP */
    if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_LOSS_ADDED)) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&msg_req, 0, sizeof(msg_req));
    msg_req.sess_id = _MPLS_LM_DM_EP_ID_TO_SESS_ID(loss_p->id);

    buffer = oc->mpls_lm_dm_dma_buffer;
    req_len = shr_mpls_lm_dm_msg_ctrl_loss_get_pack(buffer, &msg_req) - buffer;

    /* Send Loss GET request */
    rv = _bcm_fp_oam_msg_send_receive(unit,
                          MOS_MSG_CLASS_MPLS_LM_DM,
                          MOS_MSG_SUBCLASS_MPLS_LM_DM_LOSS_MEASUREMENT_GET,
                          req_len, 0,
                          MOS_MSG_SUBCLASS_MPLS_LM_DM_LOSS_MEASUREMENT_GET_REPLY,
                          &reply_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit, "MPLS_LM_DM (unit %d) Error:"
                              " Loss get Msg failed EP:%d %s.\n"),
                   unit, loss_p->id, bcm_errmsg(rv)));
        return rv;
    }

    /* Initialize the receive Message structure */
    sal_memset(&msg_rsp, 0, sizeof(msg_rsp));

    /* Unpack loss get data   */
    buffer = oc->mpls_lm_dm_dma_buffer;
    rsp_len = shr_mpls_lm_dm_msg_ctrl_loss_data_unpack(buffer, &msg_rsp) - buffer;

    if (reply_len != rsp_len) {
        rv =  BCM_E_INTERNAL;
    }

    /* Copy data from msg_rsp */
    if (msg_rsp.flags & (SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM_DM |
                         SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM )) {
        loss_p->flags |= BCM_OAM_LOSS_SLM;
    }

    if (!(msg_rsp.flags & SHR_MPLS_LM_DM_PASSIVE_FLAG)) {
        loss_p->flags |= BCM_OAM_LOSS_TX_ENABLE;
    }

    if (msg_rsp.flags & SHR_MPLS_LM_DM_SINGLE_ENDED_FLAG) {
       loss_p->flags |= BCM_OAM_LOSS_SINGLE_ENDED;
    }

    if (msg_rsp.flags & SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED) {
       loss_p->flags |= BCM_OAM_COMBINED_LOSS_DELAY;
    }

    loss_p->period         = msg_rsp.period;
    loss_p->rx_oam_packets = msg_rsp.rx_oam_packets;
    loss_p->tx_oam_packets = msg_rsp.tx_oam_packets;
    loss_p->loss_nearend   = msg_rsp.loss_nearend;
    loss_p->loss_farend    = msg_rsp.loss_farend;
    loss_p->tx_nearend     = msg_rsp.tx_nearend;
    loss_p->rx_nearend     = msg_rsp.rx_nearend;
    loss_p->tx_farend      = msg_rsp.tx_farend;
    loss_p->rx_farend      = msg_rsp.rx_farend;


    /* Copy data from local SW copy */
    loss_p->int_pri = ep->lm_int_pri;
    loss_p->pkt_pri = ep->lm_exp;

    loss_p->lm_counter_size = ep->lm_ctr_size;
    for (i=0; i < ep->lm_ctr_size; i++) {
        loss_p->lm_counter_base_id[i] = ep->lm_ctr_base_id[i];
        loss_p->lm_counter_offset[i]  = ep->lm_ctr_offset[i];
        loss_p->lm_counter_action[i]  = ep->lm_ctr_action[i];
    }

    loss_p->pm_id = _MPLS_LM_DM_EP_ID_TO_SESS_ID(loss_p->id);;

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_loss_delete
 * Purpose:
 *     Deletes MPLS LM/DM Loss session
 * Parameters:
 *     unit    - (IN) BCM device number
 *     loss_p  - (IN) Loss params
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_loss_delete(int unit, bcm_oam_endpoint_t id)
{
    _bcm_fp_oam_control_t                  *oc = NULL;
    shr_mpls_lm_dm_msg_ctrl_loss_delete_t   msg;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t       *ep;
    uint8                                  *buffer = NULL;
    uint16                                  buffer_len, reply_len;
    uint16                                  idx;
    int                                     rv = BCM_E_NONE;
    int                                     i  = 0;

    /* Get OAM control pointer */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validation checks */
    if (!oc->mpls_lm_dm_ukernel_ready) {
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);
    if (!ep->in_use) {
        return BCM_E_PARAM;
    }

    /* Check if the LM is enabled for this EP */
    if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_LOSS_ADDED)) {
        return BCM_E_NOT_FOUND;
    }

    /* Initialize the Message structure */
    sal_memset(&msg, 0, sizeof(msg));
    msg.sess_id = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);

    /* Send loss delete message */
    buffer = oc->mpls_lm_dm_dma_buffer;
    buffer_len = shr_mpls_lm_dm_msg_ctrl_loss_delete_pack(buffer, &msg) - buffer;

    rv = _bcm_fp_oam_msg_send_receive(unit,
                       MOS_MSG_CLASS_MPLS_LM_DM,
                       MOS_MSG_SUBCLASS_MPLS_LM_DM_LOSS_MEASUREMENT_DELETE,
                       buffer_len, 0,
                       MOS_MSG_SUBCLASS_MPLS_LM_DM_LOSS_MEASUREMENT_DELETE_REPLY,
                       &reply_len);

    if ((BCM_SUCCESS(rv)) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Clear the loss data from EP data */
    ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DLM);
    ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM);
    ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_PKT_TYPE_ILM_DM);
    ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_LOSS_ADDED);

    if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED)) {
        ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED);
    }

    ep->lm_exp     = 0;
    ep->lm_int_pri = 0;
    for (i = 0; i < ep->lm_ctr_size; i++) {
        ep->lm_ctr_base_id[i] = 0;
        ep->lm_ctr_offset[i]  = 0;
        ep->lm_ctr_action[i]  = bcmOamCounterActionNone;
    }
    ep->lm_ctr_size = 0;

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_delay_add
 * Purpose:
 *     Create MPLS LM/DM Delay session
 * Parameters:
 *     unit    - (IN) BCM device number
 *     delay_p - (IN) Delay params
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_fp_oam_mpls_lm_dm_delay_add(uint8 unit, bcm_oam_delay_t *delay_p)
{
    _bcm_fp_oam_control_t              *oc = NULL;
    mpls_lm_dm_sdk_msg_ctrl_delay_add_t msg;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t   *ep;
    uint32                              ach = 0;
    uint32                              dm_int_flags = 0;
    bcm_module_t                        my_modid = 0;
    bcm_gport_t                         cpu_dglp;
    soc_olp_l2_hdr_t                    olp_l2_hdr;
    soc_olp_tx_hdr_t                    olp_oam_hdr;
    uint8                              *buffer, *cur_ptr;
    uint16                              buffer_len, reply_len;
    uint32                              ctr_pool;
    uint32                              ctr_mode;
    uint32                              base_index;
    bcm_stat_group_mode_t               ctr_group;
    bcm_stat_object_t                   ctr_object;
    uint16                              idx;
    int                                 rv = BCM_E_NONE;
    int                                 i;


    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validation checks */
    if (!oc->mpls_lm_dm_ukernel_ready) {
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(delay_p->id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);

    if (!ep->in_use) {
        return BCM_E_PARAM;
    }

    if (delay_p->dm_tx_update_lm_counter_size > BCM_OAM_LM_COUNTER_MAX) {
        return BCM_E_PARAM;
    }

    if (ep->flags & SHR_MPLS_LM_DM_FLAG_LOSS_ADDED) {
        if (delay_p->flags & BCM_OAM_COMBINED_LOSS_DELAY) {
            if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED)) {
                /* Loss created without COMBINED flag, reject delay add with
                 * COMBINED flag
                 */
                return BCM_E_PARAM;
            }
        } else {
            if (ep->flags & SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED) {
                /* Loss created with COMBINED flag, reject delay add without
                 * COMBINED flag
                 */
                return BCM_E_PARAM;
            }
        }
    }

    /* Validate period, for combined Loss/Delay period is taken from loss_add */
    if (delay_p->flags & BCM_OAM_COMBINED_LOSS_DELAY) {
        BCM_IF_ERROR_RETURN(
                        _bcm_fp_oam_mpls_lm_dm_period_validate(delay_p->period));
    }

    /* Set internal flags */
    if (delay_p->flags & BCM_OAM_COMBINED_LOSS_DELAY) {
        /* For combined loss, delay the common flags are derived from loss_add */
        dm_int_flags |= SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED;
    } else {
        ach = SHR_MPLS_LM_DM_ACH_TYPE_DM;

        dm_int_flags |= SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DM;

        /* Active or passive mode */
        if (!(delay_p->flags & BCM_OAM_DELAY_TX_ENABLE)) {
            dm_int_flags |= SHR_MPLS_LM_DM_PASSIVE_FLAG;
        }

        /* Single or dual ended mode */
        if (!(delay_p->flags & BCM_OAM_DELAY_ONE_WAY)) {
            dm_int_flags |= SHR_MPLS_LM_DM_SINGLE_ENDED_FLAG;
        }
    }


    /* Copy user given values */
    ep->dm_exp     = delay_p->pkt_pri;
    ep->dm_int_pri = delay_p->int_pri;

    for(i = 0; i < delay_p->dm_tx_update_lm_counter_size; i++) {
        ep->dm_ctr_base_id[i] = delay_p->dm_tx_update_lm_counter_base_id[i];
        ep->dm_ctr_offset[i]  = delay_p->dm_tx_update_lm_counter_offset[i];
    }
    ep->dm_ctr_size = delay_p->dm_tx_update_lm_counter_size;

    /* Construct the message */
    sal_memset(&msg, 0, sizeof(msg));
    msg.flags     = dm_int_flags;
    msg.sess_id   = _MPLS_LM_DM_EP_ID_TO_SESS_ID(delay_p->id);
    msg.dm_format = delay_p->timestamp_format;

    /* In case of combined loss,delay the rest of the parameters are taken from
     * loss_add
     */
    if (!(delay_p->flags & BCM_OAM_COMBINED_LOSS_DELAY)) {
        msg.period              = delay_p->period;
        msg.session_id          = ep->hw_base_session_id;
        msg.session_num_entries = ep->session_num_entries;

        /* Create packet encap */
        BCM_IF_ERROR_RETURN(
                      _bcm_fp_oam_mpls_lm_dm_encap_create(unit, ep,
                                                          msg.l2_encap_data,
                                                          &(msg.l2_encap_length),
                                                          ach, ep->dm_exp));
        /* Create OLP encap */
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        BCM_IF_ERROR_RETURN(_bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID,
                                                   my_modid, CMIC_PORT(unit),
                                                   &cpu_dglp));
        BCM_IF_ERROR_RETURN(_bcm_olp_l2_hdr_get(unit, cpu_dglp, 0, &olp_l2_hdr));

        cur_ptr = _bcm_fp_oam_olp_l2_header_pack(msg.olp_encap_data, &olp_l2_hdr);

        /* Construct OLP OAM header */
        sal_memset(&olp_oam_hdr, 0, sizeof(olp_oam_hdr));
        SOC_OLP_TX_PORT(&olp_oam_hdr) = BCM_GPORT_MODPORT_PORT_GET(ep->tx_gport);
        SOC_OLP_TX_MODID(&olp_oam_hdr)= BCM_GPORT_MODPORT_MODID_GET(ep->tx_gport);

        /* Pack counter information in OLP header */

        for (i=0; i< ep->dm_ctr_size; i++) {
            _bcm_esw_stat_get_counter_id_info(unit, ep->dm_ctr_base_id[i],
                                              &ctr_group, &ctr_object,
                                              &ctr_mode, &ctr_pool,
                                              &base_index);
            if (i == 0) {
                SOC_OLP_TX_CTR1_ID(&olp_oam_hdr) =
                    SOC_OLP_TX_CTR(ctr_pool, base_index + ep->dm_ctr_offset[i]);
                SOC_OLP_TX_CTR1_ACTION(&olp_oam_hdr)   = 1; /* Increment */
                SOC_OLP_TX_CTR1_LOCATION(&olp_oam_hdr) = 1; /* Egress */

            }
            if (i == 1) {
                SOC_OLP_TX_CTR2_ID_SET(&olp_oam_hdr,
                                       SOC_OLP_TX_CTR(ctr_pool,
                                                      base_index +
                                                      ep->dm_ctr_offset[i]));
                SOC_OLP_TX_CTR2_ACTION(&olp_oam_hdr)   = 1; /* Increment */
                SOC_OLP_TX_CTR2_LOCATION(&olp_oam_hdr) = 1; /* Egress */
            }
            if (i == 2) {
                SOC_OLP_TX_CTR3_ID_SET(&olp_oam_hdr,
                                       SOC_OLP_TX_CTR(ctr_pool,
                                                      base_index +
                                                      ep->dm_ctr_offset[i]));
                SOC_OLP_TX_CTR3_ACTION(&olp_oam_hdr)   = 1; /* Increment */
                SOC_OLP_TX_CTR3_LOCATION(&olp_oam_hdr) = 1; /* Egress */
            }
        }
        SOC_OLP_TX_PRI(&olp_oam_hdr) = ep->dm_int_pri;

        /* Set Sample timestamp action */
        SOC_OLP_TX_TIMESTAMP_ACTION(&olp_oam_hdr) =
                         ((delay_p->timestamp_format == bcmOAMTimestampFormatNTP)
                                                        ? 2:1);

        /* Timestamp replacement offset */
        SOC_OLP_TX_OAM_OFFSET(&olp_oam_hdr) =
            ((msg.l2_encap_length + _MPLS_LM_DM_OLP_REPL_OFFSET_DM - 14) / 2);

        /* Pack OLP OAM Encap */
        shr_olp_tx_header_pack(cur_ptr, &olp_oam_hdr);
    }

    /* Send Delay Add message */
    buffer = oc->mpls_lm_dm_dma_buffer;
    buffer_len = mpls_lm_dm_sdk_msg_ctrl_delay_add_pack(buffer, &msg) - buffer;

    rv = _bcm_fp_oam_msg_send_receive(unit,
                         MOS_MSG_CLASS_MPLS_LM_DM,
                         MOS_MSG_SUBCLASS_MPLS_LM_DM_DELAY_MEASUREMENT_ADD,
                         buffer_len, 0,
                         MOS_MSG_SUBCLASS_MPLS_LM_DM_DELAY_MEASUREMENT_ADD_REPLY,
                         &reply_len);

    if ((BCM_SUCCESS(rv)) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    ep->flags |= dm_int_flags;
    ep->flags |= SHR_MPLS_LM_DM_FLAG_DELAY_ADDED;

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_delay_get
 * Purpose:
 *     Get MPLS LM/DM Delay session
 * Parameters:
 *     unit    - (IN) BCM device number
 *     delay_p - (IN) Delay params
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_fp_oam_mpls_lm_dm_delay_get(uint8 unit, bcm_oam_delay_t *delay_p)
{
    _bcm_fp_oam_control_t               *oc;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t    *ep;
    uint8                               *buffer;
    uint16                               reply_len, req_len, rsp_len;
    shr_mpls_lm_dm_msg_ctrl_delay_get_t  msg_req;
    mpls_lm_dm_sdk_msg_ctrl_delay_data_t msg_rsp;
    uint16                               idx;
    int                                  rv = BCM_E_NONE;
    int                                  i;

    /* Get OAM control pointer */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validation checks */
    if (!oc->mpls_lm_dm_ukernel_ready) {
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(delay_p->id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);
    if (!ep->in_use) {
        return BCM_E_PARAM;
    }

    /* Check if DM is enabled for this EP */
    if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED)) {
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&msg_req, 0, sizeof(msg_req));
    msg_req.sess_id = _MPLS_LM_DM_EP_ID_TO_SESS_ID(delay_p->id);

    buffer = oc->mpls_lm_dm_dma_buffer;
    req_len = shr_mpls_lm_dm_msg_ctrl_delay_get_pack(buffer, &msg_req) - buffer;

    /* Send Delay get request */
    rv = _bcm_fp_oam_msg_send_receive(unit,
                         MOS_MSG_CLASS_MPLS_LM_DM,
                         MOS_MSG_SUBCLASS_MPLS_LM_DM_DELAY_MEASUREMENT_GET,
                         req_len, 0,
                         MOS_MSG_SUBCLASS_MPLS_LM_DM_DELAY_MEASUREMENT_GET_REPLY,
                         &reply_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                  (BSL_META_U(unit, "MPLS_LM_DM (unit %d) Error:"
                              " Delay get Msg failed EP:%d %s.\n"),
                   unit, delay_p->id, bcm_errmsg(rv)));
        return rv;
    }

    /* Initialize the receive Message structure */
    sal_memset(&msg_rsp, 0, sizeof(msg_rsp));

    /* Unpack loss get data   */
    buffer = oc->mpls_lm_dm_dma_buffer;
    rsp_len = mpls_lm_dm_sdk_msg_ctrl_delay_data_unpack(buffer, &msg_rsp) - buffer;

    if (reply_len != rsp_len) {
        return BCM_E_INTERNAL;
    }

    if (msg_rsp.flags & SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED) {
        delay_p->flags |= BCM_OAM_COMBINED_LOSS_DELAY;
    } else {
        if (!(msg_rsp.flags & SHR_MPLS_LM_DM_PASSIVE_FLAG)) {
            delay_p->flags |= BCM_OAM_DELAY_TX_ENABLE;
        }
        if (!(msg_rsp.flags & SHR_MPLS_LM_DM_SINGLE_ENDED_FLAG)) {
            delay_p->flags |= BCM_OAM_DELAY_ONE_WAY;
        }
    }

    delay_p->period = msg_rsp.period;
    delay_p->timestamp_format = msg_rsp.dm_format;
    rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->delay),
                                            msg_rsp.delay_seconds,
                                            msg_rsp.delay_nanoseconds);
    if (BCM_SUCCESS(rv)) {
        rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->txf),
                                                msg_rsp.txf_seconds,
                                                msg_rsp.txf_nanoseconds);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->rxf),
                                                msg_rsp.rxf_seconds,
                                                msg_rsp.rxf_nanoseconds);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->txb),
                                                msg_rsp.txb_seconds,
                                                msg_rsp.txb_nanoseconds);
    }
    if (BCM_SUCCESS(rv)) {
        rv = bcm_fp_oam_convert_ep_to_time_spec(&(delay_p->rxb),
                                                msg_rsp.rxb_seconds,
                                                msg_rsp.rxb_nanoseconds);
    }
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    delay_p->rx_oam_packets = msg_rsp.rx_oam_packets;
    delay_p->tx_oam_packets = msg_rsp.tx_oam_packets;

    /* Copy data from local SW copy */
    delay_p->int_pri = ep->dm_int_pri;
    delay_p->pkt_pri = ep->dm_exp;

    delay_p->dm_tx_update_lm_counter_size = ep->dm_ctr_size;
    for (i=0; i < ep->dm_ctr_size; i++) {
        delay_p->dm_tx_update_lm_counter_base_id[i] = ep->dm_ctr_base_id[i];
        delay_p->dm_tx_update_lm_counter_offset[i]  = ep->dm_ctr_offset[i];
    }

    delay_p->pm_id = _MPLS_LM_DM_EP_ID_TO_SESS_ID(delay_p->id);

    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_mpls_lm_dm_delay_delete
 * Purpose:
 *     Delete MPLS LM/DM Delay session
 * Parameters:
 *     unit    - (IN) BCM device number
 *     delay_p - (IN) Delay params
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_mpls_lm_dm_delay_delete(int unit, bcm_oam_endpoint_t id)
{
    _bcm_fp_oam_control_t                  *oc = NULL;
    shr_mpls_lm_dm_msg_ctrl_delay_delete_t  msg;
    _bcm_fp_oam_mpls_lm_dm_ep_data_t       *ep;
    uint8                                  *buffer = NULL;
    uint16                                  buffer_len, reply_len;
    uint16                                  idx;
    int                                     rv = BCM_E_NONE;
    int                                     i = 0;

    /* Get OAM control pointer */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validation checks */
    if (!oc->mpls_lm_dm_ukernel_ready) {
        return BCM_E_INIT;
    }

    idx = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);
    ep = &(oc->mpls_lm_dm_ep_data[idx]);
    if (!ep->in_use) {
        return BCM_E_PARAM;
    }

    /* Check if DM is enabled for this EP */
    if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_DELAY_ADDED)) {
        return BCM_E_NOT_FOUND;
    }

    /* Initialize the Message structure */
    sal_memset(&msg, 0, sizeof(msg));
    msg.sess_id = _MPLS_LM_DM_EP_ID_TO_SESS_ID(id);

    /* Send delay delete message */
    buffer = oc->mpls_lm_dm_dma_buffer;
    buffer_len = shr_mpls_lm_dm_msg_ctrl_delay_delete_pack(buffer, &msg) - buffer;

    rv = _bcm_fp_oam_msg_send_receive(unit,
                      MOS_MSG_CLASS_MPLS_LM_DM,
                      MOS_MSG_SUBCLASS_MPLS_LM_DM_DELAY_MEASUREMENT_DELETE,
                      buffer_len, 0,
                      MOS_MSG_SUBCLASS_MPLS_LM_DM_DELAY_MEASUREMENT_DELETE_REPLY,
                      &reply_len);

    if ((BCM_SUCCESS(rv)) && (reply_len != 0)) {
        rv =  BCM_E_INTERNAL;
    }
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Clear the delay data from EP data */
    ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_PKT_TYPE_DM);
    ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_DELAY_ADDED);

    if (!(ep->flags & SHR_MPLS_LM_DM_FLAG_LOSS_ADDED)) {
        ep->flags &= ~(SHR_MPLS_LM_DM_FLAG_PKT_TYPE_COMBINED);
    }

    ep->dm_int_pri = 0;
    for (i = 0; i < ep->dm_ctr_size; i++) {
        ep->dm_ctr_base_id[i] = 0;
        ep->dm_ctr_offset[i]  = 0;
    }
    ep->dm_ctr_size = 0;

    return rv;
}

/*
 * Function:
 *      _bcm_fp_oam_mpls_lm_dm_appl_callback
 * Purpose:
 *      Update FW BHH appl state
 * Parameters:
 *      unit  - (IN) Unit number.
 *      uC    - (IN) core number.
 *      stage - (IN) core reset stage.
 *      user_data - (IN) data pointer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int _bcm_fp_oam_mpls_lm_dm_appl_callback(int unit,
                                    int uC,
                                    soc_cmic_uc_shutdown_stage_t stage,
                                    void *user_data) {
    _bcm_fp_oam_control_t *oc;

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);
    oc->mpls_lm_dm_ukernel_ready = 0;

    _BCM_OAM_UNLOCK(oc);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fp_mpls_lm_dm_event_mask_set
 * Purpose:
 *      Set the MPLS_LM_DM Events mask.
 *      Events are set per BHH module.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_fp_mpls_lm_dm_event_mask_set(int unit)
{
    _bcm_fp_oam_control_t    *oc;
    _bcm_oam_event_handler_t *event_handler_p;
    uint16 reply_len;
    int rv = BCM_E_NONE;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    /* Get event mask from all callbacks */
    for (event_handler_p = oc->event_handler_list_p;
            event_handler_p != NULL;
            event_handler_p = event_handler_p->next_p) {

        /* Update MPLS_LM_DM event mask in uKernel */
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventMplsLmDmPmCounterRollover)) {
            /* Send MPLS_LM_DM Event Mask message to uC */
            rv = _bcm_fp_oam_pm_msg_send_receive(unit,
                            MOS_MSG_CLASS_MPLS_LM_DM,
                            MOS_MSG_SUBCLASS_MPLS_LM_DM_EVENT_MASK_SET,
                            0, MPLS_LM_DM_BTE_EVENT_PM_STATS_COUNTER_ROLLOVER,
                            MOS_MSG_SUBCLASS_MPLS_LM_DM_EVENT_MASK_SET_REPLY,
                            &reply_len);
            if(BCM_SUCCESS(rv) && (reply_len != 0)) {
                rv = BCM_E_INTERNAL;
            }
            _BCM_OAM_UNLOCK(oc);
            return (rv);
        }
    }
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

#endif /* INCLUDE_MPLS_LM_DM */
/*******************************************************************************/
/* MPLS LM/DM App specific routines end                                        */
/*******************************************************************************/

/*******************************************************************************/
/* Common routines for BHH & MPLS LM/DM App end                                */
/*******************************************************************************/

/*******************************************************************************/
/* Common routines for CCM, BHH & MPLS LM/DM App start */
/*******************************************************************************/
#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)

/*
 * Function:
 *     bcm_fp_oam_event_register
 * Purpose:
 *     Register a callback for handling OAM events
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM events for which the specified
 *                        callback should be called.
 *     cb          - (IN) A pointer to the callback function to call for
 *                        the specified OAM events
 *     user_data   - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_event_register(int unit, bcm_oam_event_types_t event_types,
                        bcm_oam_event_cb cb, void *user_data)
{
    _bcm_fp_oam_control_t    *oc;
    _bcm_oam_event_handler_t *event_h_p;
    _bcm_oam_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t     e_type;
    uint32                   event_bmp;
    int                      rv = 0;
#if defined (INCLUDE_CCM)
    int                      update_event_mask = 0;
#endif
#if defined (INCLUDE_BHH)
    int                      update_bhh_event_mask = 0;
#endif
#if defined(INCLUDE_MPLS_LM_DM)
    int                      update_mpls_lm_dm_event_mask = 0;
#endif
    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Check if an event is set for register in the events bitmap. */
    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, event_bmp);
    if (0 == event_bmp) {
        /* No events specified. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "No events specified for register.\n"), unit));
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {

        _BCM_OAM_ALLOC(event_h_p, _bcm_oam_event_handler_t,
             sizeof(_bcm_oam_event_handler_t), "OAM event handler");

        if (NULL == event_h_p) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
                "Event handler alloc failed - %s.\n"), unit,
                bcm_errmsg(BCM_E_MEMORY)));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_MEMORY);
        }

        event_h_p->next_p = NULL;
        event_h_p->cb = cb;
        event_h_p->user_data = user_data;

        SHR_BITCLR_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount);
        if (prev_p != NULL) {
            prev_p->next_p = event_h_p;
        } else {
            oc->event_handler_list_p = event_h_p;
        }
    }

    for (e_type = 0; e_type < bcmOAMEventCount; ++e_type) {
        if (SHR_BITGET(event_types.w, e_type)) {
#if defined(INCLUDE_BHH)
            if(soc_feature(unit, soc_feature_bhh)) {
                /*
                 * BHH events are generated by the uKernel
                 */
                if ((e_type == bcmOAMEventBHHLBTimeout) ||
                    (e_type == bcmOAMEventBHHLBDiscoveryUpdate) ||
                    (e_type == bcmOAMEventBHHCCMTimeout) ||
                    (e_type == bcmOAMEventBHHCCMTimeoutClear) ||
                    (e_type == bcmOAMEventBHHCCMState) ||
                    (e_type == bcmOAMEventBHHCCMRdi) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevel) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriod) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriority) ||
                    (e_type == bcmOAMEventBHHCCMRdiClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevelClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriodClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriorityClear) ||
                    (e_type == bcmOAMEventBhhPmCounterRollover) ||
                    (e_type == bcmOAMEventCsfLos) ||
                    (e_type == bcmOAMEventCsfFdi) ||
                    (e_type == bcmOAMEventCsfRdi) ||
                    (e_type == bcmOAMEventCsfDci)) {
                    SHR_BITSET(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] += 1;
                    update_bhh_event_mask = 1;
                    continue;
                }
            }
#endif

#if defined(INCLUDE_CCM)
            if(soc_feature(unit, soc_feature_uc_ccm)) {
                /* CCM events are generated by the uKernel */
                if ((e_type == bcmOAMEventEndpointPortDown) ||
                    (e_type == bcmOAMEventEndpointPortUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceDown) ||
                    (e_type == bcmOAMEventEndpointInterfaceUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceTesting) ||
                    (e_type == bcmOAMEventEndpointInterfaceUnkonwn) ||
                    (e_type == bcmOAMEventEndpointInterfaceDormant) ||
                    (e_type == bcmOAMEventEndpointInterfaceNotPresent) ||
                    (e_type == bcmOAMEventEndpointInterfaceLLDown) ||
                    (e_type == bcmOAMEventGroupCCMxcon) ||
                    (e_type == bcmOAMEventGroupCCMError) ||
                    (e_type == bcmOAMEventGroupRemote) ||
                    (e_type == bcmOAMEventGroupCCMTimeout) ||
                    (e_type == bcmOAMEventGroupMACStatus) ||
                    (e_type == bcmOAMEventGroupCCMxconClear) ||
                    (e_type == bcmOAMEventGroupCCMErrorClear) ||
                    (e_type == bcmOAMEventGroupRemoteClear) ||
                    (e_type == bcmOAMEventGroupCCMTimeoutClear) ||
                    (e_type == bcmOAMEventGroupMACStatusClear) ||
                    (e_type == bcmOAMEventEndpointCCMTimeout) ||
                    (e_type == bcmOAMEventEndpointCCMTimein) ||
                    (e_type == bcmOAMEventEndpointRemote) ||
                    (e_type == bcmOAMEventEndpointRemoteUp)) {

                    SHR_BITSET(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] += 1;
                    update_event_mask = 1;
                    continue;
                }
            }
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_MPLS_LM_DM)
            if (soc_feature(unit, soc_feature_mpls_lm_dm)) {
                if ((e_type == bcmOAMEventMplsLmDmPmCounterRollover)) {
                    /*
                     * MPLS_LM_DM events are generated by the uKernel
                     */
                    SHR_BITSET(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] += 1;
                    update_mpls_lm_dm_event_mask = 1;
                    continue;
                }
            }
#endif /* INCLUDE_MPLS_LM_DM */
        }
    }

#if defined(INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) && 
        update_bhh_event_mask) {
        /*
         * Update BHH Events mask
         */
        rv = _bcm_fp_oam_bhh_event_mask_set(unit);
    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_CCM)
    if ((soc_feature(unit, soc_feature_uc_ccm)) && (update_event_mask)) {
        /* Update CCM Events mask */
        rv = _bcm_fp_oam_ccm_event_mask_set(unit);
    }
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_mpls_lm_dm)) &&
        update_mpls_lm_dm_event_mask) {
        /*
         * Update MPLS_LM_DM Events mask
         */
        rv = _bcm_fp_mpls_lm_dm_event_mask_set(unit);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: failed to set MPLS LM DM event mask-"
                                " %s.\n"), unit, bcm_errmsg(rv)));
            return (rv);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_event_unregister
 * Purpose:
 *     Unregister event and its callback from the event handler list
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM events for which the specified
 *                        callback should not be called.
 *     cb          - (IN) A pointer to the callback function to unregister
 *                        from the specified OAM events
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_event_unregister(int unit, bcm_oam_event_types_t event_types,
                        bcm_oam_event_cb cb)
{
    _bcm_fp_oam_control_t    *oc;
    _bcm_oam_event_handler_t *event_h_p;
    _bcm_oam_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t     e_type;
    uint32                   event_bmp;
    int                      rv = 0;
#if defined (INCLUDE_CCM)
    int                      update_event_mask = 0;
#endif
#if defined (INCLUDE_BHH)
    int                      update_bhh_event_mask = 0;
#endif
#if defined(INCLUDE_MPLS_LM_DM)
    int                      update_mpls_lm_dm_event_mask = 0;
#endif
    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Check if an event is set for unregister in the events bitmap. */
    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, event_bmp);
    if (0 == event_bmp) {
        /* No events specified. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "No events specified for unregister.\n"), unit));
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {
        _BCM_OAM_UNLOCK(oc);
        return (BCM_E_NOT_FOUND);
    }

    for (e_type = 0; e_type < bcmOAMEventCount; ++e_type) {
        if (SHR_BITGET(event_types.w, e_type)) {
#if defined(INCLUDE_BHH)
            if(soc_feature(unit, soc_feature_bhh)) {
                /*
                 * BHH events are generated by the uKernel
                 */
                if ((e_type == bcmOAMEventBHHLBTimeout) ||
                    (e_type == bcmOAMEventBHHLBDiscoveryUpdate) ||
                    (e_type == bcmOAMEventBHHCCMTimeout) ||
                    (e_type == bcmOAMEventBHHCCMTimeoutClear) ||
                    (e_type == bcmOAMEventBHHCCMState) ||
                    (e_type == bcmOAMEventBHHCCMRdi) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevel) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepId) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriod) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriority) ||
                    (e_type == bcmOAMEventBHHCCMRdiClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegLevelClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMegIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownMepIdClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPeriodClear) ||
                    (e_type == bcmOAMEventBHHCCMUnknownPriorityClear) ||
                    (e_type == bcmOAMEventBhhPmCounterRollover)) {
                    SHR_BITCLR(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] -= 1;
                    update_bhh_event_mask = 1;
                    continue;
                }
            }
#endif

#if defined(INCLUDE_CCM)
            if(soc_feature(unit, soc_feature_uc_ccm)) {
                /* CCM events are generated by the uKernel */
                if ((e_type == bcmOAMEventEndpointPortDown) ||
                    (e_type == bcmOAMEventEndpointPortUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceDown) ||
                    (e_type == bcmOAMEventEndpointInterfaceUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceTestingToUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceUnknownToUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceDormantToUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceNotPresentToUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceLLDownToUp) ||
                    (e_type == bcmOAMEventEndpointInterfaceTesting) ||
                    (e_type == bcmOAMEventEndpointInterfaceUnkonwn) ||
                    (e_type == bcmOAMEventEndpointInterfaceDormant) ||
                    (e_type == bcmOAMEventEndpointInterfaceNotPresent) ||
                    (e_type == bcmOAMEventEndpointInterfaceLLDown) ||
                    (e_type == bcmOAMEventGroupCCMxcon) ||
                    (e_type == bcmOAMEventGroupCCMError) ||
                    (e_type == bcmOAMEventGroupRemote) ||
                    (e_type == bcmOAMEventGroupCCMTimeout) ||
                    (e_type == bcmOAMEventEndpointCCMTimeout) ||
                    (e_type == bcmOAMEventEndpointCCMTimein) ||
                    (e_type == bcmOAMEventEndpointRemote) ||
                    (e_type == bcmOAMEventEndpointRemoteUp)) {

                    SHR_BITCLR(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] -= 1;
                    update_event_mask = 1;
                    continue;
                }
            }
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_MPLS_LM_DM)
            if (soc_feature(unit, soc_feature_mpls_lm_dm)) {
                if ((e_type == bcmOAMEventMplsLmDmPmCounterRollover)) {
                    /*
                     * MPLS_LM_DM events are generated by the uKernel
                     */
                    SHR_BITCLR(event_h_p->event_types.w, e_type);
                    oc->event_handler_cnt[e_type] -= 1;
                    update_mpls_lm_dm_event_mask = 1;
                    continue;
                }
            }
#endif /* INCLUDE_MPLS_LM_DM */
        }
    }
    SHR_BITTEST_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount, event_bmp);

    if (0 == event_bmp) {

        if (NULL != prev_p) {

            prev_p->next_p = event_h_p->next_p;

        } else {

            oc->event_handler_list_p = event_h_p->next_p;

        }
        sal_free(event_h_p);
    }

#if defined(INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) && 
        update_bhh_event_mask) {
        /*
         * Update BHH Events mask
         */
        rv = _bcm_fp_oam_bhh_event_mask_set(unit);
    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_CCM)
    if ((soc_feature(unit, soc_feature_uc_ccm)) && (update_event_mask)) {
        /* Update CCM Events mask */
        rv = _bcm_fp_oam_ccm_event_mask_set(unit);
    }
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_mpls_lm_dm)) &&
        update_mpls_lm_dm_event_mask) {
        /*
         * Update MPLS_LM_DM Events mask
         */
        rv = _bcm_fp_mpls_lm_dm_event_mask_set(unit);
        if (BCM_FAILURE(rv)) {
            _BCM_OAM_UNLOCK(oc);
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: failed to set MPLS LM DM event mask-"
                                " %s.\n"), unit, bcm_errmsg(rv)));
            return (rv);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_resource_count_init
 * Purpose:
 *     Retrieves and initializes endpoint count information for this device.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to device OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_resource_count_init(int unit, _bcm_fp_oam_control_t *oc)
{
    /* Input parameter check. */
    if (NULL == oc) {
        return (BCM_E_PARAM);
    }

#if defined(INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm)) {
        /*
         * Get endpoint firmware table index count values and
         * initialize device OAM control structure members variables.
         */
        oc->mep_count = soc_property_get(unit, spn_OAM_CCM_MAX_MEPS, 256);
        oc->group_count = soc_property_get(unit, spn_OAM_CCM_MAX_GROUPS, 256);
    }
#endif /* INCLUDE_CCM */
#if defined(INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh)) {

    /* Get SOC properties for BHH */
    oc->bhh_endpoint_count = soc_property_get(unit, spn_BHH_NUM_SESSIONS, 128);
    oc->bhh_base_group_id  = soc_property_get(unit, spn_BHH_BASE_GROUP_ID, 256);
    oc->bhh_base_endpoint_id  = soc_property_get(unit, spn_BHH_BASE_ENDPOINT_ID,
                                                 512);
    }
#endif /* INCLUDE_BHH */

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_fp_oam_control_free
 * Purpose:
 *     Free OAM control structure resources allocated by this unit.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_control_free(int unit, _bcm_fp_oam_control_t *oc)
{
    _fp_oam_control[unit] = NULL;

#if defined(INCLUDE_BHH)
    int status = 0;
#endif

    if (NULL == oc) {
        /* Module already un-initialized */
        return (BCM_E_NONE);
    }

    /* Free protection mutex */
    if (NULL != oc->oc_lock) {
        sal_mutex_destroy(oc->oc_lock);
        oc->oc_lock = NULL;
    }

    /* Destory group indices list */
    if (NULL != oc->group_pool) {
       shr_idxres_list_destroy(oc->group_pool);
       oc->group_pool = NULL;
    }

    /* Free group memory. */
    if (NULL != oc->group_data) {
        sal_free(oc->group_data);
        oc->group_data = NULL;
    }

    /* Destroy LMEP indices list */
    if (NULL != oc->mep_pool) {
        shr_idxres_list_destroy(oc->mep_pool);
        oc->mep_pool = NULL;
    }

    /* Free LMEP memory. */
    if (NULL != oc->mep_data) {
        sal_free(oc->mep_data);
    }
#if 0
    /* Free hash data storage memory */
    if (NULL != oc->oam_hash_data) {
        sal_free(oc->oam_hash_data);
    }
#endif
#if defined(INCLUDE_BHH)
    /* Free BHH hash data storage memory */
    if (NULL != oc->bhh_ep_data) {
        sal_free(oc->bhh_ep_data);
    }
    /* Destory endpoint hash table. */
    if (NULL != oc->bhh_mep_htbl) {
        status = shr_htb_destroy(&oc->bhh_mep_htbl, NULL);
        if (BCM_FAILURE(status)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "Freeing bhh_mep_htbl failed\n")));
        }
    }
    /* Free BHH group memory. */
    if (NULL != oc->bhh_group_data) {
        sal_free(oc->bhh_group_data);
    }
    /* Destroy BHH EP indices list */
    if (NULL != oc->bhh_pool) {
        shr_idxres_list_destroy(oc->bhh_pool);
        oc->bhh_pool = NULL;
    }
    /* Free BHH DMA buffer */
    if (NULL != oc->bhh_dma_buffer) {
        soc_cm_sfree(oc->unit, oc->bhh_dma_buffer);
    }
    /* Free BHH reply DMA buffer */
    if (NULL != oc->bhh_dmabuf_reply) {
        soc_cm_sfree(oc->unit, oc->bhh_dmabuf_reply);
    }
    if (soc_feature(unit, soc_feature_oam_pm)) {
        if (_BCM_FP_OAM_PM_BHH_DATA_COLLECTION_MODE_ENABLED(oc)) {
            soc_cm_sfree(oc->unit, oc->pm_bhh_dma_buffer);
        }
    }
#endif

#if defined(INCLUDE_MPLS_LM_DM)
    if (NULL != oc->mpls_lm_dm_dma_buffer) {
        soc_cm_sfree(oc->unit, oc->mpls_lm_dm_dma_buffer);
        oc->mpls_lm_dm_dma_buffer = NULL;
    }

    if (NULL != oc->mpls_lm_dm_pool) {
        shr_idxres_list_destroy(oc->mpls_lm_dm_pool);
        oc->mpls_lm_dm_pool = NULL;
    }

    if (NULL != oc->mpls_lm_dm_ep_data) {
        sal_free(oc->mpls_lm_dm_ep_data);
        oc->mpls_lm_dm_ep_data = NULL;
    }
#endif /* INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    if (soc_feature(unit, soc_feature_oam_pm)) {
        if (pm_profile_control[unit]) {
            sal_free(pm_profile_control[unit]);
            pm_profile_control[unit] = NULL;
        }
    }
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

    /* Free OAM control structure memory. */
    sal_free(oc);
    oc = NULL;

    return (BCM_E_NONE);

}


/*
 * Function:
 *     bcm_fp_oam_detach
 * Purpose:
 *     Shut down OAM subsystem
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_detach(int unit)
{
    int rv = 0;
    _bcm_fp_oam_control_t    *oc = NULL;
#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH)
    uint16 reply_len;
#endif /* INCLUDE_CCM || INCLUDE_BHH */

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Stop CCM Event Thread */
#if defined(INCLUDE_CCM)
        if (soc_feature(unit, soc_feature_uc_ccm)) {
            if (oc->ccm_ukernel_ready) {
                /* Event Handler thread exit signal */
                sal_usecs_t timeout = sal_time_usecs() + 5000000;
                while (NULL !=  oc->event_thread_id) {
                    soc_cmic_uc_msg_receive_cancel(unit, oc->ccm_uc_num,
                                                   MOS_MSG_CLASS_CCM_EVENT);

                    if (sal_time_usecs() < timeout) {
                        /*give some time to already running CCM callback thread
                         * to schedule and exit */
                        sal_usleep(10000);
                    } else {
                        /*timeout*/
                        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                                  "CCM event thread did not exit.\n")));
                        _BCM_OAM_UNLOCK(oc);
                        return BCM_E_INTERNAL;
                    }
                }

                if (!SOC_WARM_BOOT(unit)) {
                    /*
                     * Send CCM Uninit message to uC
                     * Ignore error since that may indicate uKernel was reloaded.
                     */
                    rv = _bcm_fp_oam_msg_send_receive(unit,
                            MOS_MSG_CLASS_CCM,
                            MOS_MSG_SUBCLASS_CCM_UNINIT,
                            0, 0,
                            MOS_MSG_SUBCLASS_CCM_UNINIT_REPLY,
                            &reply_len);
                    if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                        if (NULL != oc->oc_lock) {
                            _BCM_OAM_UNLOCK(oc);
                        }
                        return BCM_E_INTERNAL;
                    } else {
                        if ((rv == BCM_E_TIMEOUT) || (rv == BCM_E_INIT)) {
                            LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM: "
                                "Uninit error - %s\n"), bcm_errmsg(rv)));
                            rv = BCM_E_NONE;
                        }
                    }
                }
            }
        }
#endif /* INCLUDE_CCM */


     /* Stop BHH Event Thread */
#if defined(INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh)) {
        int uc_status;
        if (oc->ukernel_not_ready == 0) {
            /*
             * Event Handler thread exit signal
             */
            sal_usecs_t timeout = sal_time_usecs() + 5000000;
            while (NULL !=  oc->bhh_event_thread_id) {
                soc_cmic_uc_msg_receive_cancel(unit, oc->bhh_uc_num,
                                               MOS_MSG_CLASS_BHH_EVENT);

                if (sal_time_usecs() < timeout) {
                    /*give some time to already running bhh callback thread
                     * to schedule and exit */
                    sal_usleep(10000);
                } else {
                    /*timeout*/
                    LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "BHH event thread did not exit.\n")));
                    return BCM_E_INTERNAL;
                }
            }
            if (!SOC_WARM_BOOT(unit)) {
                /*
                 * Send BHH Uninit message to uC
                 */
                SOC_IF_ERROR_RETURN(soc_uc_status(unit, oc->bhh_uc_num, &uc_status));
                if (uc_status) {
                    rv = _bcm_fp_oam_msg_send_receive(unit,
                               MOS_MSG_CLASS_BHH,
                               MOS_MSG_SUBCLASS_BHH_UNINIT,
                               0, 0,
                               MOS_MSG_SUBCLASS_BHH_UNINIT_REPLY,
                              &reply_len);
                    if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                        LOG_ERROR(BSL_LS_BCM_OAM,
                              (BSL_META_U(unit,
                                          "BHH UNINIT failed - error %s\n"),
                                          bcm_errmsg(rv)));
                        return BCM_E_INTERNAL;
                    }
                }
                 /* Ignore timeout/init error since that may indicate
                  * uKernel was reloaded/msging is not up.*/
                if ((rv == BCM_E_TIMEOUT) || (rv == BCM_E_INIT)) {
                    LOG_DEBUG(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "BHH UNINIT failed - %s\n"),
                             bcm_errmsg(rv)));
                    rv = BCM_E_NONE;
                }

            }
        }

    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    if (soc_feature(unit, soc_feature_mpls_lm_dm)) {
        rv = _bcm_fp_oam_mpls_lm_dm_detach(unit);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                      (BSL_META_U(unit,
                                  "MPLS LM/DM UNINIT failed - error %s\n"),
                       bcm_errmsg(rv)));
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */


    _bcm_fp_oam_control_free(unit, oc);
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_endpoint_create
 * Purpose:
 *     Create or replace an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_PARAM;

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) &&
        (endpoint_info->type  == bcmOAMEndpointTypeEthernet)) {
        rv = bcm_fp_oam_ccm_endpoint_create(unit, endpoint_info);
    }
#endif
  
#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) && 
             BHH_EP_TYPE (endpoint_info)){
        rv = _bcm_fp_oam_bhh_endpoint_create (unit, endpoint_info);
    }
#endif

#if defined(INCLUDE_MPLS_LM_DM)
    if (soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if (_MPLS_LM_DM_ENDPOINT_TYPE(endpoint_info->type)) {
            rv = _bcm_fp_oam_mpls_lm_dm_endpoint_create(unit, endpoint_info);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_endpoint_get
 * Purpose:
 *     Fetches an OAM endpoint object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     endpoint - (IN) Endpoint ID to get.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                         bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_PARAM; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);


#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((endpoint >= 0) &&  (endpoint < oc->mep_count))) {
        endpoint_info->id = endpoint;
        rv = bcm_fp_oam_ccm_endpoint_get(unit, endpoint_info);     
    }
#endif

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((endpoint >= oc->bhh_base_endpoint_id) &&  
        (endpoint < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        endpoint_info->id = endpoint;
        rv = _bcm_fp_oam_bhh_endpoint_get(unit, endpoint_info);
    }
#endif

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(endpoint)) {
            rv = _bcm_fp_oam_mpls_lm_dm_endpoint_get(unit, endpoint,
                                                     endpoint_info);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_endpoint_destroy
 * Purpose:
 *     Destroy an OAM endpoint object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     endpoint - (IN) Endpoint ID to destroy.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    int rv = BCM_E_PARAM; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);


#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((endpoint >= 0) &&  (endpoint < oc->mep_count))) {
        rv = bcm_fp_oam_ccm_endpoint_destroy(unit, endpoint);
    }
#endif

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((endpoint >= oc->bhh_base_endpoint_id) &&  
        (endpoint < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_endpoint_destroy(unit, endpoint);
    }
#endif

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(endpoint)) {
            rv = _bcm_fp_oam_mpls_lm_dm_endpoint_destroy(unit, endpoint);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*  
 * Function:
 *     bcm_fp_oam_endpoint_destroy_all
 * Purpose:
 *     Destroy all OAM endpoint objects associated with a group.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     group - (IN) The OAM group whose endpoints should be destroyed
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_destroy_all(int unit, bcm_oam_group_t group)
{
    int rv = BCM_E_PARAM; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((group >= oc->bhh_base_group_id) &&
        (group < (oc->bhh_base_group_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_endpoint_destroy_all(unit, group);
    }
#endif
    _BCM_OAM_UNLOCK(oc);

    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_endpoint_traverse
 * Purpose:
 *     Traverse the entire set of BHH endpoints associated with BHH group
 *     and calls specified callback for each one
 * Parameters:
 *     unit  - (IN) BCM device number
 *     cb        - (IN) Pointer to call back function.
 *     user_data - (IN) Pointer to user supplied data.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_traverse(int unit, bcm_oam_group_t group,
                             bcm_oam_endpoint_traverse_cb cb,
                             void *user_data)
{    int rv = BCM_E_PARAM; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);
#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm)
        && (group > 0 && group <= oc->group_count)) {
        if (!oc->ccm_ukernel_ready) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "CCM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }

        rv = _bcm_fp_oam_ccm_endpoint_traverse(unit, group, cb,user_data);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "CCM(unit %d) Error: Endpoint traverse "
                                    "routine : %s.\n"), unit, bcm_errmsg(rv)));
        }
    }
#endif

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
            ((group >= oc->bhh_base_group_id) &&  
             (group < (oc->bhh_base_group_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_endpoint_traverse (unit, group, cb, user_data);
    }
#endif
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_group_create
 * Purpose:
 *     Create or replace an OAM group object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to group information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    int rv = BCM_E_PARAM;

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) &&
        !(group_info->flags  & BCM_OAM_GROUP_TYPE_BHH)) {
        rv = bcm_fp_oam_ccm_group_create(unit, group_info);
    }
#endif
  
#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) && 
        (group_info->flags  & BCM_OAM_GROUP_TYPE_BHH)) {
        rv = _bcm_fp_oam_bhh_group_create (unit, group_info);
    }
#endif
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_group_get
 * Purpose:
 *     Fetches an OAM group object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group - (IN) Group ID to get.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_group_get(int unit,  bcm_oam_group_t group,
                         bcm_oam_group_info_t *group_info)
{
    int rv = BCM_E_PARAM; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);


#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((group >= 0) &&  (group < oc->group_count))) {
        group_info->id = group;
        rv = bcm_fp_oam_ccm_group_get(unit, group_info);     
    }
#endif

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((group >= oc->bhh_base_group_id) &&  
        (group < (oc->bhh_base_group_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        group_info->id = group;
        rv = _bcm_fp_oam_bhh_group_get(unit, group_info);
    }
#endif
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_group_destroy
 * Purpose:
 *     Destroy an OAM group object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group    - (IN) Group ID to destroy.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_group_destroy(int unit, bcm_oam_group_t group)
{
    int rv = BCM_E_PARAM; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);


#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((group >= 0) &&  (group < oc->group_count))) {
        rv = bcm_fp_oam_ccm_group_destroy(unit, group);
    }
#endif

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((group >= oc->bhh_base_group_id) &&  
        (group < (oc->bhh_base_group_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_group_destroy(unit, group);
    }
#endif
    _BCM_OAM_UNLOCK(oc);
    return (rv);
}

/*
 * Function:
 *     bcm_fp_oam_group_destroy_all
 * Purpose:
 *     Destroy all OAM groups
 * Parameters:
 *     unit  - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_group_destroy_all(int unit)
{
    int rv = BCM_E_UNAVAIL; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        (oc->ukernel_not_ready == 0)){
        rv = _bcm_fp_oam_bhh_group_destroy_all(unit);
    }
#endif
    _BCM_OAM_UNLOCK(oc);
    return (rv);

}

/*
 * Function:
 *     bcm_fp_oam_group_traverse
 * Purpose:
 *     Traverse the entire set of OAM groups, calling a specified
 *     callback for each one
 * Parameters:
 *     unit  - (IN) BCM device number
 *     cb        - (IN) Pointer to call back function.
 *     user_data - (IN) Pointer to user supplied data.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_group_traverse(int unit, bcm_oam_group_traverse_cb cb,
                           void *user_data)
{
    int rv = BCM_E_UNAVAIL; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    
    _BCM_OAM_LOCK(oc);
#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm)) {
        if (oc->ccm_ukernel_ready) {
            rv = _bcm_fp_oam_ccm_group_traverse(unit, cb, user_data);

            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "CCM(unit %d) Error: Group traverse "
                                        "routine : %s.\n"), unit, bcm_errmsg(rv)));
            }
        }
    }
#endif
#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        (oc->ukernel_not_ready == 0)){
        rv = _bcm_fp_oam_bhh_group_traverse (unit, cb, user_data);
    }
#endif
    _BCM_OAM_UNLOCK(oc);
    return (rv);

}


/*
 * Function:
 *     bcm_fp_oam_loss_add
 * Purpose:
 *        Loss Measurement add
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_loss_add(int unit, bcm_oam_loss_t *loss_p)
{
    int                    rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((loss_p->id >= oc->bhh_base_endpoint_id) &&
        (loss_p->id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_loss_add(unit, loss_p);
    }
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(loss_p->id)) {
            rv = _bcm_fp_oam_mpls_lm_dm_loss_add(unit, loss_p);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_loss_get
 * Purpose:
 *     Loss Measurement get
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_loss_get(int unit, bcm_oam_loss_t *loss_p)
{
    int rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((loss_p->id >= oc->bhh_base_endpoint_id) &&
        (loss_p->id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_loss_get(unit, loss_p);
    }
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(loss_p->id)) {
            rv = _bcm_fp_oam_mpls_lm_dm_loss_get(unit, loss_p);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_loss_delete
 * Purpose:
 *    Loss Measurement Delete
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_loss_delete(int unit, bcm_oam_loss_t *loss_p)
{
    int rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((loss_p->id >= oc->bhh_base_endpoint_id) &&
        (loss_p->id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_loss_delete(unit, loss_p);
    }
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(loss_p->id)) {
            rv =  _bcm_fp_oam_mpls_lm_dm_loss_delete(unit, loss_p->id);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_delay_add
 * Purpose:
 *     Delay Measurement add
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_delay_add(int unit, bcm_oam_delay_t *delay_p)
{
    int rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((delay_p->id >= oc->bhh_base_endpoint_id) &&
        (delay_p->id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_delay_add(unit, delay_p);
    }
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(delay_p->id)) {
            rv =  _bcm_fp_oam_mpls_lm_dm_delay_add(unit, delay_p);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_delay_get
 * Purpose:
 *     Delay Measurements get
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_delay_get(int unit, bcm_oam_delay_t *delay_p)
{
    int rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((delay_p->id >= oc->bhh_base_endpoint_id) &&
        (delay_p->id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_delay_get(unit, delay_p);
    }
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(delay_p->id)) {
            rv = _bcm_fp_oam_mpls_lm_dm_delay_get(unit, delay_p);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_delay_delete
 * Purpose:
 *     Delay Measurement Delete
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
int
bcm_fp_oam_delay_delete(int unit, bcm_oam_delay_t *delay_p)
{
    int rv = BCM_E_UNAVAIL;
#if defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh) &&
        ((delay_p->id >= oc->bhh_base_endpoint_id) &&
        (delay_p->id < (oc->bhh_base_endpoint_id + oc->bhh_endpoint_count)))) {
        if (oc->ukernel_not_ready == 1){
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "OAM(unit %d) Error: BTE(ukernel) "
                                "not ready.\n"), unit));
            _BCM_OAM_UNLOCK(oc);
            return (BCM_E_INIT);
        }
        rv = _bcm_fp_oam_bhh_delay_delete(unit, delay_p);
    }
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_mpls_lm_dm)) {
        if(_MPLS_LM_DM_EP_ID_RANGE(delay_p->id)) {
            rv = _bcm_fp_oam_mpls_lm_dm_delay_delete(unit, delay_p->id);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

    _BCM_OAM_UNLOCK(oc);
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
    return rv;
}

/*
 * Function:
 *     _bcm_fp_oam_sync
 * Purpose:
 *     Store OAM configuration to level two storage cache memory.
 * Parameters:
 *     unit - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_fp_oam_sync(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_WARM_BOOT_SUPPORT)
#if defined(INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh)) {
        BCM_IF_ERROR_RETURN(_bcm_fp_oam_bhh_sync(unit));
    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    if (soc_feature(unit, soc_feature_mpls_lm_dm)) {
        BCM_IF_ERROR_RETURN(_bcm_fp_oam_mpls_lm_dm_sync(unit));
    }
#endif /* INCLUDE_MPLS_LM_DM */
#endif /* BCM_WARM_BOOT_SUPPORT */

    return rv;
}


#endif /*INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */
/*******************************************************************************/
/* Common routines for CCM, BHH & MPLS LM/DM App end */
/*******************************************************************************/



/*
 * Function: bcm_td2p_oam_init
 *
 * Purpose:
 *     Initialize OAM module.
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - OAM not support on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     CM_E_XXX     - Error code from bcm_XX_oam_init()
 *     BCM_E_NONE    - Success
 */
int
bcm_td2p_oam_init(int unit)
{
    int rv;             /* Operation return value. */
#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH)
    uint32 size = 0;    /* Size of memory allocation. */
#endif /* INCLUDE_CCM || INCLUDE_BHH */

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    _bcm_fp_oam_control_t *oc = NULL;  /* OAM control structure. */
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined (INCLUDE_BHH)
    int uc = 0;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
#endif /* INCLUDE_BHH */

#if defined (INCLUDE_CCM) || defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    if(soc_feature(unit, soc_feature_uc_ccm) ||
       soc_feature(unit, soc_feature_bhh)    ||
       soc_feature(unit, soc_feature_mpls_lm_dm)) {

        /* Detach first if the module has been previously initialized. */
        if (NULL != _fp_oam_control[unit]) {
            _fp_oam_control[unit]->init = FALSE;
            rv = bcm_fp_oam_detach(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                                "Module deinit - %s.\n"), bcm_errmsg(rv)));
                return (rv);
            }
        }


        /* Allocate OAM control memory for this unit. */
        _BCM_OAM_ALLOC(oc, _bcm_fp_oam_control_t,
                sizeof(_bcm_fp_oam_control_t), "OAM control");
        if (NULL == oc) {
            return (BCM_E_MEMORY);
        }

        /* Create protection mutex. */
        oc->oc_lock = sal_mutex_create("oam_control.lock");
        if (NULL == oc->oc_lock) {
            _bcm_fp_oam_control_free(unit, oc);
            return (BCM_E_MEMORY);
        }

        /* Set up the unit OAM control structure. */
        _fp_oam_control[unit] = oc;

        /* Get number of endpoints and groups supported by this unit. */
        rv = _bcm_fp_oam_resource_count_init(unit, oc);
        if (BCM_FAILURE(rv)) {
            _bcm_fp_oam_control_free(unit, oc);
            return (rv);
        }
        oc->unit = unit;

    }
#endif /* INCLUDE_CCM || INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

    /* Initialize PM */
#if defined (INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
    if (soc_feature(unit, soc_feature_oam_pm)) {
        rv = _bcm_fp_oam_pm_init(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_fp_oam_control_free(unit, oc);
            return (rv);
        }
    }
#endif /* INCLUDE_BHH || INCLUDE_MPLS_LM_DM */

#if defined (INCLUDE_CCM)
    if(soc_feature(unit, soc_feature_uc_ccm)) {
        shr_ccm_msg_ctrl_init_t ccm_init_msg;
        uint8 *buffer_p1 = NULL;
        uint8 *buffer_p2 = NULL;
        uint16 s_len = 0;
        uint16 r_len = 0;

        
        /* Core #0 is used for CCM in Saber2*/
        if (soc_uc_in_reset(unit, _CCM_UC_NUM)) {

            LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM Warn:"
                      " uKernel Not Ready, CCM not started.\n If CCM"
                      " app is not not needed, compile SDK by excluding CCM"
                      " from FEATURE_LIST.\n")));

        } else {

            /* Initialize uController side
             * Start CCM application in BTE (Broadcom Task Engine) uController.
             * _CCM_UC_NUM runs the CCM Application.
             */
            rv = soc_cmic_uc_appl_init(unit, _CCM_UC_NUM, MOS_MSG_CLASS_CCM,
                    _UC_MSG_TIMEOUT_USECS, _CCM_SDK_VERSION,
                    _CCM_UC_MIN_VERSION, NULL, NULL);

            if (!(BCM_FAILURE(rv))) {

                /* Create Group list.*/
                rv = shr_idxres_list_create(&oc->group_pool, 0, 
                                        oc->group_count - 1,
                                        0, oc->group_count - 1, "group pool");
                if (BCM_FAILURE(rv)) {
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }
                
                /* Allocate group memory */
                size = sizeof(_bcm_fp_oam_group_data_t) * oc->group_count;
                _BCM_OAM_ALLOC(oc->group_data, _bcm_fp_oam_group_data_t, size,
                        "Group Info");
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM Error: "
                        "Group data Alloca failed - %s.\n"), bcm_errmsg(rv)));
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }

                /* Create Mep list. */
                rv = shr_idxres_list_create(&oc->mep_pool, 0, oc->mep_count - 1,
                                            0, oc->mep_count - 1, "mep pool");
                if (BCM_FAILURE(rv)) {
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }

                /* Allocate group memory */
                size = sizeof(_bcm_fp_oam_mep_data_t) * oc->mep_count;
                _BCM_OAM_ALLOC(oc->mep_data, _bcm_fp_oam_mep_data_t, size,
                        "MEP Info");
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM Error: "
                        "MEP data Alloca failed - %s.\n"), bcm_errmsg(rv)));
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }            

                /* Allocate DMA Memory */
                oc->dma_buffer_len = sizeof(shr_ccm_msg_ctrl_t);
                oc->dma_buffer = soc_cm_salloc(unit, oc->dma_buffer_len,
                                               "CCM DMA buffer");
                if (!oc->dma_buffer) {
                    _bcm_fp_oam_control_free(unit, oc);
                    return (BCM_E_MEMORY);
                }

                oc->ccm_ukernel_ready = 1;
                oc->ccm_uc_num = _CCM_UC_NUM;

                /* Send Initialize CCM App on UKERNEL */
                /* Set control message data */
                sal_memset(&ccm_init_msg, 0, sizeof(ccm_init_msg));
                ccm_init_msg.max_groups = oc->group_count;
                ccm_init_msg.max_meps = oc->mep_count;
                ccm_init_msg.rx_channel = _CCM_RX_CHANNEL_ID;

                /* Pack control message data into DMA buffer */
                buffer_p1 = oc->dma_buffer;
                buffer_p2 = shr_ccm_msg_ctrl_init_pack(buffer_p1, &ccm_init_msg);
                s_len = buffer_p2 - buffer_p1;

                /* Send CCM Init message to uC */
                rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
                       MOS_MSG_SUBCLASS_CCM_INIT, s_len, 0,
                       MOS_MSG_SUBCLASS_CCM_INIT_REPLY, &r_len);

                if (BCM_FAILURE(rv) || (r_len != 0)) {
                   /* Could not start MPLS_LM_DM appl */
                   LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                       "CCM Session alloc failed\n")));
                   BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                   return rv;
                }

                /* If we have Warmrebooted, recover S/w state from UKENREL */
#ifdef BCM_WARM_BOOT_SUPPORT
                if (SOC_WARM_BOOT(unit)) {
                    rv = _bcm_fp_oam_ccm_reinit(unit);
                    if (BCM_FAILURE(rv)) {
                        BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                        return (rv);
                    }
                }
#endif
                /* Start event message callback thread */
                if (oc->event_thread_id == NULL) {
                    oc->event_thread_id =  sal_thread_create("bcmCCM",
                        SAL_THREAD_STKSZ, _FP_OAM_CCM_EVENT_THREAD_PRIO,
                        _bcm_fp_oam_ccm_callback_thread, (void*)oc);
                    if (oc->event_thread_id == SAL_THREAD_ERROR) {
                         oc->event_thread_id = NULL;
                         BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                        return (BCM_E_MEMORY);
                    }
                }

            } else {
                /* Could not start CCM appl */
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                         "OAM WARN: CCM Application not available\n")));
                /* Most likely core 0 has a different Application image */
            }
        }
    } /* soc_feature_ccm */
#endif /* INCLUDE_CCM */

#if defined (INCLUDE_BHH)
    if (soc_feature(unit, soc_feature_bhh)) {

        uint8 carrier_code[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        char *carrier_code_str;
        uint32 node_id = 0;
        bhh_sdk_msg_ctrl_init_t msg_init;
        int priority;
        int ukernel_not_ready = 0;
        uint8 bhh_consolidate_final_event = 0;
        int uc_status;
 
        /*
         * Start BHH application in BTE (Broadcom Task Engine) uController,
         * if not already running (warm boot).
         * Determine which uController is running BHH  by choosing the first
         * uC that returns successfully.
         */
        for (uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
            rv = soc_uc_status(unit, uc, &uc_status);
            if ((rv == SOC_E_NONE) && (uc_status != 0)) {
                rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_BHH,
                        _BHH_UC_MSG_TIMEOUT_USECS,
                        BHH_SDK_VERSION,
                        BHH_UC_MIN_VERSION, _bcm_oam_bhh_appl_callback, NULL);
                if (SOC_E_NONE == rv) {
                    /* BHH started successfully */
                    oc->bhh_uc_num = uc;
                    break;
                }
            }
        }

        if (uc >= SOC_INFO(unit).num_ucs) {/* Could not find or start BHH appl */
            ukernel_not_ready = 1;
            LOG_ERROR(BSL_LS_BCM_OAM,
                    (BSL_META_U(unit,
                                "uKernel Not Ready, bhh not started\n")));
        }
        oc->ukernel_not_ready = ukernel_not_ready;

        if(ukernel_not_ready == 0){
            /* Create BHH Group list.*/
            /* BHH groups are equalent to BHH endpoints - 1 to 1 mapping*/
            rv = shr_idxres_list_create(&oc->bhh_group_pool, 0, 
                    oc->bhh_endpoint_count - 1,
                    0, oc->bhh_endpoint_count - 1, "BHH group pool");
            if (BCM_FAILURE(rv)) {
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                return (rv);
            }
            /* Allocate BHH group memory */
            size = sizeof(_bhh_fp_oam_group_data_t) * oc->bhh_endpoint_count;
            _BCM_OAM_ALLOC(oc->bhh_group_data, _bhh_fp_oam_group_data_t, size,
                    "BHH Group Info");

            if (NULL == oc->bhh_group_data) {
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                return (BCM_E_MEMORY);
            }

            /* Mem_1: Allocate BHH endpoints hash data memory */
            size = sizeof(_bhh_fp_oam_ep_data_t) * oc->bhh_endpoint_count;
            _BCM_OAM_ALLOC(oc->bhh_ep_data, _bhh_fp_oam_ep_data_t, 
                    size, "Hash data");
            if (NULL == oc->bhh_ep_data) {
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                return (BCM_E_MEMORY);
            }
            /*Calculating hash table size (which should be power of 2) 
              from the given number of BHH 
              endpoints to the nearest power of 2*/
            int hash_size = 1;
            if (oc->bhh_endpoint_count && !(oc->bhh_endpoint_count & 
                                           (oc->bhh_endpoint_count - 1))){
                hash_size = oc->bhh_endpoint_count;
            } else {
                while (hash_size < oc->bhh_endpoint_count) {
                    hash_size <<= 1;
                }
            }
            rv = shr_htb_create(&oc->bhh_mep_htbl, hash_size,
                    sizeof(_bhh_fp_oam_hash_key_t), "BHH MEP Hash");
            if (BCM_FAILURE(rv)) {
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                return (rv);
            }

            /* Create BHH endpoint indices list. */
            rv = shr_idxres_list_create(&oc->bhh_pool, 0, 
                    oc->bhh_endpoint_count - 1,
                    0, oc->bhh_endpoint_count - 1, "bhh pool");
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Creating BHH pool " 
                                    "failed \n"),
                         unit));
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                return (rv);
            }

            node_id = soc_property_get(unit, spn_BHH_NODE_ID, 0);
            bhh_consolidate_final_event = soc_property_get(unit, 
                                           spn_BHH_CONSOLIDATED_FINAL_EVENT, 0);

            carrier_code_str = soc_property_get_str(unit, spn_BHH_CARRIER_CODE);
            if (carrier_code_str != NULL) {
                /*
                 * Note that the carrier code is specified in colon separated
                 * MAC address format.
                 */
                if (_shr_parse_macaddr(carrier_code_str, carrier_code) < 0) {
                    BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                    return (BCM_E_INTERNAL);
                }
            }

            oc->bhh_max_encap_length =
                             soc_property_get(unit,
                                              spn_BHH_ENCAP_MAX_LENGTH,
                                              _BCM_OAM_BHH_DEFAULT_ENCAP_LENGTH);

            if(oc->bhh_max_encap_length > _BCM_OAM_BHH_MAX_ENCAP_LENGTH) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                          (BSL_META_U(unit,
                                      "Invalid max encap_length=%u\n"),
                                      oc->bhh_max_encap_length));
                return BCM_E_CONFIG;
            }

            if (soc_feature(unit, soc_feature_oam_pm)) {
                rv = _bcm_fp_oam_bhh_pm_init(unit);
                if (BCM_FAILURE(rv)) {
                    BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                    return (BCM_E_INTERNAL);
                }
            }

            /*
             * Allocate DMA buffers
             *
             * DMA buffer will be used to send and receive 'long' messages
             * between SDK Host and uController (BTE).
             */
            oc->bhh_dma_buffer_len = sizeof(bhh_sdk_msg_ctrl_t);
            oc->bhh_dma_buffer = soc_cm_salloc(unit, oc->bhh_dma_buffer_len,
                                           "BHH DMA buffer");
            if (!oc->bhh_dma_buffer) {
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Allocating BHH buffer" 
                                    " failed \n"),
                         unit));
                return (BCM_E_MEMORY);
            }
            sal_memset(oc->bhh_dma_buffer, 0, oc->bhh_dma_buffer_len);

            oc->bhh_dmabuf_reply = soc_cm_salloc(unit, sizeof(bhh_sdk_msg_ctrl_t),
                                             "BHH uC reply");
            if (!oc->bhh_dmabuf_reply) {
                LOG_ERROR(BSL_LS_BCM_OAM,
                        (BSL_META_U(unit,
                                    "OAM(unit %d) Error: Allocating BHH reply " 
                                    "buffer failed \n"),
                         unit));
                BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                return (BCM_E_MEMORY);
            }
            sal_memset(oc->bhh_dmabuf_reply, 0, sizeof(bhh_sdk_msg_ctrl_t));


            /* RX DMA channel (0..3) local to the uC */
            oc->rx_channel = BCM_KT_BHH_RX_CHANNEL;
#ifdef BCM_WARM_BOOT_SUPPORT
            if (!SOC_WARM_BOOT(unit)) {
                rv = _bcm_fp_oam_bhh_scache_alloc(unit);
                if ((BCM_E_NOT_FOUND != rv) && (BCM_FAILURE(rv))) {
                    BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                    return (rv);
                }
#endif
                /* Set control message data */
                sal_memset(&msg_init, 0, sizeof(msg_init));
                msg_init.num_sessions       = oc->bhh_endpoint_count;
                msg_init.rx_channel         = oc->rx_channel;
                msg_init.node_id            = node_id;
                sal_memcpy(msg_init.carrier_code, carrier_code,
                           SHR_BHH_CARRIER_CODE_LEN);
                if (soc_feature(unit, soc_feature_oam_pm)) {
                    msg_init.data_collection_mode = 
                                  oc->pm_bhh_lmdm_data_collection_mode;
                }

                msg_init.max_encap_length = oc->bhh_max_encap_length;
                if (bhh_consolidate_final_event) {
                    msg_init.flags |= BHH_SDK_MSG_CTRL_INIT_ONLY_FINAL_EVENT;
                }
                /* Pack control message data into DMA buffer */
                buffer     = oc->bhh_dma_buffer;
                buffer_ptr = bhh_sdk_msg_ctrl_init_pack(unit, buffer, &msg_init);
                buffer_len = buffer_ptr - buffer;

                /* Send BHH Init message to uC */
                rv = _bcm_fp_oam_msg_send_receive(unit,
                                                 MOS_MSG_CLASS_BHH, 
                                                 MOS_MSG_SUBCLASS_BHH_INIT,
                                                 buffer_len, 0,
                                                 MOS_MSG_SUBCLASS_BHH_INIT_REPLY,
                                                 &reply_len);

                if (BCM_FAILURE(rv) || (reply_len != 0)) {
                    BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                    LOG_ERROR(BSL_LS_BCM_OAM,
                            (BSL_META_U(unit,
                                        "OAM(unit %d) Error: BHH init " 
                                        "failed \n"),
                             unit));
                    return (BCM_E_INTERNAL);
                }

#ifdef BCM_WARM_BOOT_SUPPORT
            } else if (SOC_WARM_BOOT(unit)) {
                rv = _bcm_fp_oam_bhh_reinit(unit);
                if (BCM_FAILURE(rv)) {
                    BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                    return (rv);
                }
            }
#endif

            /*
             * Start event message callback thread
             */
            priority = soc_property_get(unit, spn_BHH_THREAD_PRI,
                                        BHH_THREAD_PRI_DFLT);

            if (oc->bhh_event_thread_id == NULL) {
                if ((oc->bhh_event_thread_id =
                    sal_thread_create("bcmBHH", SAL_THREAD_STKSZ,
                                      priority,
                                      _bcm_fp_oam_bhh_callback_thread,
                                      (void*)oc)) == SAL_THREAD_ERROR) {
                    oc->bhh_event_thread_id = NULL;
                    BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                    return (BCM_E_MEMORY);
                }
            }
        }
    }
#endif /* INCLUDE_BHH */

#if defined(INCLUDE_MPLS_LM_DM)
    if (soc_feature(unit, soc_feature_mpls_lm_dm)) {
        rv = _bcm_fp_oam_mpls_lm_dm_init(unit, oc);
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
            return rv;
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit))
#endif
    {
        /* Set default olp header type mapping */
        rv = _bcm_td2p_oam_olp_header_type_mapping_set(unit);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        /* Enable OLP on HG ports */
        BCM_IF_ERROR_RETURN(_bcm_td2p_oam_hg_olp_enable(unit));

        /* Set Magic port used for remote OLP over HG communication */
        BCM_IF_ERROR_RETURN(_bcm_td2p_oam_olp_magic_port_set(unit));

#if defined (BCM_APACHE_SUPPORT)
        if(SOC_IS_APACHE(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_apache_oam_default_hw_config_set(unit));
        }
#endif /* BCM_APACHE_SUPPORT */
    }
    return (BCM_E_NONE);
}

#if defined(INCLUDE_CCM)
void
_bcm_fp_oam_ccm_hash_data_dump(int unit, _bcm_fp_oam_mep_data_t *ep_p)
{
    LOG_CLI((BSL_META_U(unit,"Flags = 0x%x\r\n"),ep_p->mep_data.flags));
    LOG_CLI((BSL_META_U(unit,"EP ID = %d\r\n"),ep_p->mep_data.ep_id));
    LOG_CLI((BSL_META_U(unit,"Grp id = %d\r\n"),ep_p->mep_data.group_id));
    LOG_CLI((BSL_META_U(unit,"Src Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             ep_p->mep_data.sa[0], ep_p->mep_data.sa[1],
             ep_p->mep_data.sa[2], ep_p->mep_data.sa[3],
             ep_p->mep_data.sa[4], ep_p->mep_data.sa[5]));
    LOG_CLI((BSL_META_U(unit,"Dst Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             ep_p->mep_data.da[0], ep_p->mep_data.da[1],
             ep_p->mep_data.da[2], ep_p->mep_data.da[3],
             ep_p->mep_data.da[4], ep_p->mep_data.da[5]));
    LOG_CLI((BSL_META_U(unit,"Type = %d\r\n"),ep_p->mep_data.type));
    LOG_CLI((BSL_META_U(unit,"Level = %d\r\n"),ep_p->mep_data.level));
    LOG_CLI((BSL_META_U(unit,"Name = %d\r\n"),ep_p->mep_data.name));
    LOG_CLI((BSL_META_U(unit,"Period = %d\r\n"),ep_p->mep_data.period));
    LOG_CLI((BSL_META_U(unit,"Outer Vlan = %d\r\n"),ep_p->mep_data.vlan));
    LOG_CLI((BSL_META_U(unit,"Inner Vlan = %d\r\n"),ep_p->mep_data.inner_vlan));
    LOG_CLI((BSL_META_U(unit,"pkt pri = 0x%x\r\n"),ep_p->mep_data.pkt_pri));
    LOG_CLI((BSL_META_U(unit,"Inner Vlan Pri = %d\r\n"),ep_p->mep_data.inner_vlan_pri));
    LOG_CLI((BSL_META_U(unit,"Outer TPID = 0x%04X\r\n"),ep_p->mep_data.tpid));
    LOG_CLI((BSL_META_U(unit,"Inner TPID = 0x%04X\r\n"),ep_p->mep_data.inner_tpid));
    LOG_CLI((BSL_META_U(unit,"Gport = %d\r\n"),ep_p->mep_data.gport));
    LOG_CLI((BSL_META_U(unit,"SGLP = %d\r\n"),ep_p->mep_data.sglp));
    LOG_CLI((BSL_META_U(unit,"DGLP = %d\r\n"),ep_p->mep_data.dglp));
    LOG_CLI((BSL_META_U(unit,"Trunk Index = %d\r\n"),ep_p->mep_data.trunk_index));
    LOG_CLI((BSL_META_U(unit,"Faults = %d\r\n"),ep_p->mep_data.faults));
    LOG_CLI((BSL_META_U(unit,"persistent_faults = %d\r\n"),
                        ep_p->mep_data.persistent_faults));
    LOG_CLI((BSL_META_U(unit,"port_state = %d\r\n"),
                        ep_p->mep_data.port_state));
    LOG_CLI((BSL_META_U(unit,"intf_state = %d\r\n"),
                        ep_p->mep_data.intf_state));
    LOG_CLI((BSL_META_U(unit,"num_ctrs = %d\r\n\n"),
                        ep_p->mep_data.num_ctrs));
}
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_MPLS_LM_DM)
void
_bcm_fp_oam_mpls_lm_dm_hash_data_dump(int unit, _bcm_fp_oam_mpls_lm_dm_ep_data_t *ep_p)
{
    LOG_CLI((BSL_META_U(unit,"EP ID = %d\r\n"),ep_p->id));
    LOG_CLI((BSL_META_U(unit,"Flags = 0x%x\r\n"),ep_p->flags));
    LOG_CLI((BSL_META_U(unit,"Type = %d\r\n"),ep_p->type));
    LOG_CLI((BSL_META_U(unit,"VCCV_TYPE = %d\r\n"),ep_p->vccv_type));
    LOG_CLI((BSL_META_U(unit,"Gport = %d\r\n"),ep_p->gport));
    LOG_CLI((BSL_META_U(unit,"Tx_Gport = %d\r\n"),ep_p->tx_gport));
    LOG_CLI((BSL_META_U(unit,"Trunk Index = %d\r\n"),ep_p->trunk_index));
    LOG_CLI((BSL_META_U(unit,"Src Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             ep_p->src_mac[0], ep_p->src_mac[1],
             ep_p->src_mac[2], ep_p->src_mac[3],
             ep_p->src_mac[4], ep_p->src_mac[5]));
    LOG_CLI((BSL_META_U(unit,"Dst Mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n"),
             ep_p->dst_mac[0], ep_p->dst_mac[1],
             ep_p->dst_mac[2], ep_p->dst_mac[3],
             ep_p->dst_mac[4], ep_p->dst_mac[5]));
    LOG_CLI((BSL_META_U(unit,"Outer Vlan = %d\r\n"),ep_p->outer_vlan));
    LOG_CLI((BSL_META_U(unit,"Outer TPID = 0x%04X\r\n"),ep_p->outer_tpid));
    LOG_CLI((BSL_META_U(unit,"Outer pri = 0x%x\r\n"),ep_p->outer_pri));
    LOG_CLI((BSL_META_U(unit,"Inner Vlan = %d\r\n"),ep_p->inner_vlan));
    LOG_CLI((BSL_META_U(unit,"Inner TPID = 0x%04X\r\n"),ep_p->inner_tpid));
    LOG_CLI((BSL_META_U(unit,"Inner Vlan Pri = %d\r\n"),ep_p->inner_pri));

    LOG_CLI((BSL_META_U(unit,"Session Id = %d\r\n"),ep_p->session_id));
    LOG_CLI((BSL_META_U(unit,"Session num entries = %d\r\n"),
                ep_p->session_num_entries));
    LOG_CLI((BSL_META_U(unit,"HW Base Session Id = %d\r\n\n"),
                ep_p->hw_base_session_id));
}
#endif /* INCLUDE_MPLS_LM_DM */

#if defined(INCLUDE_CCM) || defined(INCLUDE_BHH) || defined(INCLUDE_MPLS_LM_DM)
/*
 * Function:
 *     _bcm_fp_oam_sw_dump
 * Purpose:
 *     Displays oam information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_fp_oam_sw_dump(int unit)
{
    int ep_id;
    int rv;
    _bcm_fp_oam_control_t *oc;

    LOG_CLI((BSL_META_U(unit,"OAM\n")));
    /* Get OAM control structure. */
    rv = _bcm_fp_oam_control_get(unit, &oc);
    if(BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                "OAM(unit %d) Error: Get oam control variable\n"),
                 unit));

        return;
    }

#if defined(INCLUDE_CCM)
    if ((soc_feature(unit, soc_feature_uc_ccm)) &&
        (oc->ccm_ukernel_ready)) {
        LOG_CLI((BSL_META_U(unit,"CCM EPs data :\n")));
        for(ep_id=0; ep_id < oc->mep_count; ep_id++) {
            if (oc->mep_data[ep_id].in_use) {
                _bcm_fp_oam_ccm_hash_data_dump(unit, &(oc->mep_data[ep_id]));
            }
        }
    }
#endif /* INCLUDE_CCM */

#if defined(INCLUDE_MPLS_LM_DM)
    if ((soc_feature(unit, soc_feature_mpls_lm_dm)) &&
        (oc->mpls_lm_dm_ukernel_ready)) {
        _bcm_fp_oam_mpls_lm_dm_ep_data_t  *ep_p;
        LOG_CLI((BSL_META_U(unit,"MPLS LM/DM EPs data :\n")));
        _MPLS_LM_DM_EP_ITER(ep_p, ep_id) {
            if (!ep_p->in_use) {
                continue;
            }
            _bcm_fp_oam_mpls_lm_dm_hash_data_dump(unit, ep_p);
        }
    }
#endif /* INCLUDE_MPLS_LM_DM */

#if defined (INCLUDE_BHH)
    if ((soc_feature(unit, soc_feature_bhh)) &&
        (oc->ukernel_not_ready == 0)) {
        LOG_CLI((BSL_META_U(unit,"BHH EPs data :\n")));
        for(ep_id=0; ep_id < oc->bhh_endpoint_count; ep_id++) {
            if (oc->bhh_ep_data[ep_id].in_use) {
                _bcm_fp_oam_bhh_hash_data_dump(unit,  &oc->bhh_ep_data[ep_id]);
            }
        }
    }
#endif
}

/*
 * Function:
 *     _bcm_fp_oam_port_trunk_update_msg_send
 * Purpose:
 *     Send port to trunk mapping update msg to UKERNEL
 * Parameters:
 *     unit          - (IN) BCM device number
 *     num_ports     - number of ports.
 *     trunk_id      - ports map to which trunk_id
 *     list_of_ports - list of ports
 *
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_fp_oam_port_trunk_update_msg_send(int unit, uint16 num_ports,
                            uint16 trunk_id, uint16 *list_of_ports)
{
    shr_ccm_msg_ctrl_port_trunk_upd_t upd_msg;
    _bcm_fp_oam_control_t *oc = NULL;
    uint8 *dma_buffer = NULL;  /* DMA buffer */
    uint8 *dma_buffer_ptr = NULL;  /* DMA buffer ptr after pack */
    uint16 s_msg_len = 0;       /* Send Message length */
    uint16  r_msg_len = 0; /* Msg response length */
    int rv = BCM_E_NONE, index;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Clear upd_msg structure */
    sal_memset(&upd_msg, 0, sizeof(upd_msg));


    /* Fill upd_msg structure */
    if (num_ports < MAX_PORTS) {
        upd_msg.num_ports = num_ports; 
    } else {
        return BCM_E_PARAM;
    }
   
    upd_msg.trunk_id = trunk_id;

    for (index = 0; index < num_ports; index++) {
        upd_msg.list_of_ports[index] = list_of_ports[index];
    }


    /* Pack Update data to buffer */
    dma_buffer = oc->dma_buffer;
    dma_buffer_ptr = shr_ccm_msg_ctrl_port_trunk_upd_pack(dma_buffer, &upd_msg);
    s_msg_len = dma_buffer_ptr - dma_buffer;
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_PORT_TRUNK_UPDATE, s_msg_len, 0,
            MOS_MSG_SUBCLASS_CCM_PORT_TRUNK_UPDATE_REPLY, &r_msg_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Port trunk update Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_fp_oam_port_trunk_map_get
 * Purpose:
 *     Get port to trunk mapping from UKERNEL
 * Parameters:
 *     unit          - (IN) BCM device number
 *     num_ports     - number of ports.
 *     trunk_id      - ports map to which trunk_id
 *     list_of_ports - list of ports
 *
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_fp_oam_port_trunk_map_get(int unit, uint16 *num_ports,
                            uint16 trunk_id, uint16 *list_of_ports)
{
    shr_ccm_msg_ctrl_port_trunk_upd_t get_msg;
    _bcm_fp_oam_control_t *oc = NULL;
    uint8 *dma_buffer = NULL;  /* DMA buffer */
    uint8 *dma_buffer_ptr = NULL;  /* DMA buffer ptr after pack */
    uint16  r_msg_len = 0; /* Msg response length */
    int rv = BCM_E_NONE, index;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Send trunk_id in the send length variable */
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_PORT_TRUNK_GET, trunk_id, 0, 
            MOS_MSG_SUBCLASS_CCM_PORT_TRUNK_GET_REPLY, &r_msg_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Port trunk map get Msg returned error %s.\n"), bcm_errmsg(rv)));
        return(BCM_E_INTERNAL);
    }

    /* Clear get_msg structure */
    sal_memset(&get_msg, 0, sizeof(get_msg));

    /* UnPack control message data in the DMA buffer */
    dma_buffer = oc->dma_buffer;
    dma_buffer_ptr = shr_ccm_msg_ctrl_port_trunk_upd_unpack(dma_buffer, &get_msg);

    if (r_msg_len != dma_buffer_ptr - dma_buffer) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Trunk map get Msg length check failed, trunk:%d.\n"), trunk_id));
        return (rv);
    }

    *num_ports = get_msg.num_ports;

    for (index = 0; index < *num_ports; index++) {
        list_of_ports[index] = get_msg.list_of_ports[index];
    }

    return (rv);
}

int
_bcm_fp_oam_trunk_ports_update(int unit, bcm_gport_t trunk_gport,
                               int max_ports, bcm_gport_t *port_arr,
                               uint8 delete)
{
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    uint16 *list_of_ports = NULL;
    int rv = BCM_E_NONE;
    bcm_module_t module_id;
    bcm_port_t   port_id;
    bcm_trunk_t port_trunk_id;
    int local_id, result, i;

    if (!BCM_GPORT_IS_TRUNK(trunk_gport)) {
        return BCM_E_PARAM;
    }

    if (max_ports >= MAX_PORTS) {
        return BCM_E_PARAM;
    }

    if (!port_arr) {
        return BCM_E_PARAM;
    }

    /* Get trunk_id from gport */
    trunk_id = BCM_GPORT_TRUNK_GET(trunk_gport);

    if (trunk_id == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    _BCM_OAM_ALLOC(list_of_ports, uint16, sizeof(uint16) * max_ports , "list of ports");

    for (i = 0; i < max_ports; i++) {
        module_id = -1;
        port_id = -1;
        port_trunk_id = BCM_TRUNK_INVALID;
        local_id = -1;
        result = FALSE;

        /* Get (Modid + Port) value from Gport */
        rv = _bcm_esw_gport_resolve(unit, port_arr[i],
                    &module_id, &port_id, &port_trunk_id,
                    &local_id);
        if ((BCM_FAILURE(rv)) || 
            (port_trunk_id != BCM_TRUNK_INVALID)) {
            goto cleanup;
        }
        rv = _bcm_esw_modid_is_local(unit, module_id, &result);

        if (BCM_FAILURE(rv)) {
            goto cleanup;
        } else if (!result) {
            /* We will handle only local ports */
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        
        list_of_ports[i] = port_id; 
    }

    if (delete) {
        rv = _bcm_fp_oam_port_trunk_update_msg_send(
                unit, max_ports, CCM_INVALID_TRUNK_NUM, list_of_ports);
    } else {
        rv = _bcm_fp_oam_port_trunk_update_msg_send(
                unit, max_ports, trunk_id, list_of_ports);
    }

    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Trunk map update " 
                            "failed \n"),
                 unit));
    }

cleanup:
    sal_free(list_of_ports);

    return rv;
}

int _bcm_fp_oam_trunk_ports_update_trunk_id(int unit, bcm_trunk_t trunk_id)
{
    bcm_port_t port_id_list[MAX_PORTS];
    uint16 *list_of_ports = NULL;
    int local_member_count = 0, i;
    int rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id, MAX_PORTS,
                port_id_list,
                &local_member_count));
    _BCM_OAM_ALLOC(list_of_ports, uint16, sizeof(uint16) * local_member_count, "list of ports");

    for(i = 0; i < local_member_count; i++) {
        list_of_ports[i] = port_id_list[i];
    }

    rv = _bcm_fp_oam_port_trunk_update_msg_send(unit, local_member_count,
                trunk_id, list_of_ports);


    sal_free(list_of_ports);
    return rv;

}

/*Add ports (specified by port_arr) to trunk (specified by trunk_gport) mapping 
  to port-trunk database in OAM  */
int 
bcm_fp_oam_trunk_ports_add (int unit, bcm_gport_t trunk_gport,
                            int max_ports, bcm_gport_t *port_arr)
{
    return _bcm_fp_oam_trunk_ports_update(unit, trunk_gport, max_ports, port_arr, 0);
}

/*Delete ports (specified by port_arr) to trunk (specified by trunk_gport) 
  mapping from port-trunk database in OAM  */
int
bcm_fp_oam_trunk_ports_delete (int unit, bcm_gport_t trunk_gport,
                               int max_ports, bcm_gport_t *port_arr)
{
    return _bcm_fp_oam_trunk_ports_update(unit, trunk_gport, max_ports, port_arr, 1);
}

/*Get ports (in port_arr) that are mapped to trunk (specified by trunk_gport)
  from port-trunk database in OAM  */
int
bcm_fp_oam_trunk_ports_get (int unit, bcm_gport_t trunk_gport,
                            int max_ports, bcm_gport_t *port_arr,
                            int *port_count)
{
    bcm_trunk_t trunk_id = 0;
    uint16 *list_of_ports = NULL;
    int rv = BCM_E_NONE, i;
    bcm_port_t port;
    bcm_gport_t gport;
    uint16 num_ports = 0;

    if (!BCM_GPORT_IS_TRUNK(trunk_gport)) {
        return BCM_E_PARAM;
    }

    if (max_ports >= MAX_PORTS) {
        return BCM_E_PARAM;
    }

    if (!port_arr) {
        return BCM_E_PARAM;
    }

    /* Get trunk_id from gport */
    trunk_id = BCM_GPORT_TRUNK_GET(trunk_gport);

    if (trunk_id == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    _BCM_OAM_ALLOC(list_of_ports, uint16, sizeof(uint16) * max_ports , "list of ports");


    rv = _bcm_fp_oam_port_trunk_map_get(
            unit, &num_ports, trunk_id, list_of_ports);


    if (BCM_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_OAM,
                (BSL_META_U(unit,
                            "OAM(unit %d) Error: Trunk map get" 
                            "failed \n"),
                 unit));
        goto cleanup;
    }

    *port_count = num_ports;

    /* If the size of array sent by user is smaller than
     * actual number of ports, then reduce the count to
     * array size.
     */
    if (*port_count > max_ports) {
        *port_count = max_ports;
    }

    for (i = 0; i < *port_count; i++) {
        gport = BCM_GPORT_INVALID;
        port = list_of_ports[i];
        rv = bcm_esw_port_gport_get(unit, port, &gport);
        if (BCM_FAILURE(rv) || gport == BCM_GPORT_INVALID) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
        port_arr[i] = gport;
    }


cleanup:
    sal_free(list_of_ports);

    return rv;
}

#endif
