/*! \file sbr.h
 *
 * Management of strength profile table and strength field for strength
 * based-resolution (SBR).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_SBR_H
#define BCMI_LTSW_SBR_H

#include <sal/sal_types.h>

/*!
 * \brief Strength profile table handle.
 *
 * The table handle is used to identify strength profile table.
 * New table handle should be added at the last.
 */
typedef enum bcmi_ltsw_sbr_profile_tbl_hdl_e {
    /*! DESTINATION_FP_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_DESTINATION_FP = 0,

    /*! EGR_DVP_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_DVP = 1,

    /*! EGR_IFA_ATTRIBUTES_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_IFA_ATTRIBUTES = 2,

    /*! EGR_L2_OIF_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_L2_OIF = 3,

    /*! EGR_L3_NEXT_HOP_1_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1 = 4,

    /*! EGR_L3_NEXT_HOP_2_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_2 = 5,

    /*! EGR_L3_OIF_1_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1 = 6,

    /*! EGR_L3_TNL_0_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_L3_TNL_0 = 7,

    /*! EGR_L3_TNL_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_L3_TNL = 8,

    /*! EGR_VXLT_2_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_VXLT_2 = 9,

    /*! EGR_VXLT_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_VXLT = 10,

    /*! ING_DVP_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_ING_DVP = 11,

    /*! ING_ECMP_MEMBER_1_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_ING_ECMP_MEMBER_1 = 12,

    /*! ING_IPMC_REMAP_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_ING_IPMC_REMAP = 13,

    /*! ING_L3_NEXT_HOP_1_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_1 = 14,

    /*! ING_L3_NEXT_HOP_2_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2 = 15,

    /*! ING_SYSTEM_DESTINATION_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION = 16,

    /*! L2_HOST_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_HOST_LOOKUP0_DEFAULT = 17,

    /*! L2_HOST_NARROW_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_HOST_NARROW = 18,

    /*! L2_HOST_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_HOST = 19,

    /*! L2_IPV4_MULTICAST_SG_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST_SG = 20,

    /*! L2_IPV4_MULTICAST_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST = 21,

    /*! L2_IPV6_MULTICAST_SG_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST_SG = 22,

    /*! L2_IPV6_MULTICAST_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST = 23,

    /*! L3_IIF_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IIF = 24,

    /*! L3_IPV4_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_ADDRESS_COMPRESSION = 25,

    /*! L3_IPV4_ALPM_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_ALPM = 26,

    /*! L3_IPV4_MULTICAST_1_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_1 = 27,

    /*! L3_IPV4_MULTICAST_2_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_2 = 28,

    /*! L3_IPV4_MULTICAST_DEFIP_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_DEFIP = 29,

    /*! L3_IPV4_UNICAST_DEFIP_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP = 30,

    /*! L3_IPV4_UNICAST_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST = 31,

    /*! L3_IPV6_ADDRESS_COMPRESSION_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_ADDRESS_COMPRESSION = 32,

    /*! L3_IPV6_ALPM_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_ALPM = 33,

    /*! L3_IPV6_MULTICAST_1_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_1 = 34,

    /*! L3_IPV6_MULTICAST_2_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_2 = 35,

    /*! L3_IPV6_MULTICAST_DEFIP_128_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_128 = 36,

    /*! L3_IPV6_MULTICAST_DEFIP_64_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_64 = 37,

    /*! L3_IPV6_UNICAST_DEFIP_128_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_128 = 38,

    /*! L3_IPV6_UNICAST_DEFIP_64_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_64 = 39,

    /*! L3_IPV6_UNICAST_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST = 40,

    /*! L3_IP_LOOKUP0_DEFAULT_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP0_DEFAULT = 41,

    /*! L3_IP_LOOKUP1_DEFAULT_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP1_DEFAULT = 42,

    /*! SYS_PORT_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_SYS_PORT = 43,

    /*! VFI_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_VFI = 44,

    /*! VLAN_XLATE_1_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_VLAN_XLATE_1 = 45,

    /*! VLAN_XLATE_2_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_VLAN_XLATE_2 = 46,

    /*! VLAN_XLATE_3_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_VLAN_XLATE_3 = 47,

    /*! EGR_MPLS_VPN_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_MPLS_VPN = 48,

    /*! L3_IPV4_UNICAST_NAPT_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_NAPT = 49,

    /*! PROTOCOL_PKT_FORWARD_STRENGTH_PROFILE. */
    BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD = 50,

    /*! EGR_VFI_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_VFI = 51,

    /*! EGR_VXLT_DW_STR_PROFILE. */
    BCMI_LTSW_SBR_PTH_EGR_VXLT_DW = 52,

    /*! Always the last. */
    BCMI_LTSW_SBR_PTH_COUNT
} bcmi_ltsw_sbr_profile_tbl_hdl_t;

#define BCMI_LTSW_SBR_PTH_STR \
{ \
    "BCMI_LTSW_SBR_PTH_DESTINATION_FP", \
    "BCMI_LTSW_SBR_PTH_EGR_DVP", \
    "BCMI_LTSW_SBR_PTH_EGR_IFA_ATTRIBUTES", \
    "BCMI_LTSW_SBR_PTH_EGR_L2_OIF", \
    "BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_1", \
    "BCMI_LTSW_SBR_PTH_EGR_L3_NEXT_HOP_2", \
    "BCMI_LTSW_SBR_PTH_EGR_L3_OIF_1", \
    "BCMI_LTSW_SBR_PTH_EGR_L3_TNL_0", \
    "BCMI_LTSW_SBR_PTH_EGR_L3_TNL", \
    "BCMI_LTSW_SBR_PTH_EGR_VXLT_2", \
    "BCMI_LTSW_SBR_PTH_EGR_VXLT", \
    "BCMI_LTSW_SBR_PTH_ING_DVP", \
    "BCMI_LTSW_SBR_PTH_ING_ECMP_MEMBER_1", \
    "BCMI_LTSW_SBR_PTH_ING_IPMC_REMAP", \
    "BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_1", \
    "BCMI_LTSW_SBR_PTH_ING_L3_NEXT_HOP_2", \
    "BCMI_LTSW_SBR_PTH_ING_SYSTEM_DESTINATION", \
    "BCMI_LTSW_SBR_PTH_L2_HOST_LOOKUP0_DEFAULT", \
    "BCMI_LTSW_SBR_PTH_L2_HOST_NARROW", \
    "BCMI_LTSW_SBR_PTH_L2_HOST", \
    "BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST_SG", \
    "BCMI_LTSW_SBR_PTH_L2_IPV4_MULTICAST", \
    "BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST_SG", \
    "BCMI_LTSW_SBR_PTH_L2_IPV6_MULTICAST", \
    "BCMI_LTSW_SBR_PTH_L3_IIF", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_ADDRESS_COMPRESSION", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_ALPM", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_1", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_2", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_MULTICAST_DEFIP", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_DEFIP", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_ADDRESS_COMPRESSION", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_ALPM", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_1", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_2", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_128", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_MULTICAST_DEFIP_64", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_128", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST_DEFIP_64", \
    "BCMI_LTSW_SBR_PTH_L3_IPV6_UNICAST", \
    "BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP0_DEFAULT", \
    "BCMI_LTSW_SBR_PTH_L3_IP_LOOKUP1_DEFAULT", \
    "BCMI_LTSW_SBR_PTH_SYS_PORT", \
    "BCMI_LTSW_SBR_PTH_VFI", \
    "BCMI_LTSW_SBR_PTH_VLAN_XLATE_1", \
    "BCMI_LTSW_SBR_PTH_VLAN_XLATE_2", \
    "BCMI_LTSW_SBR_PTH_VLAN_XLATE_3", \
    "BCMI_LTSW_SBR_PTH_EGR_MPLS_VPN", \
    "BCMI_LTSW_SBR_PTH_L3_IPV4_UNICAST_NAPT", \
    "BCMI_LTSW_SBR_PTH_PROTOCOL_PKT_FORWARD", \
    "BCMI_LTSW_SBR_PTH_EGR_VFI", \
    "BCMI_LTSW_SBR_PTH_EGR_VXLT_DW" \
}

/*!
 * \brief Strength Profile Entry Type.
 *
 * The entry type is used to address the strength profile entry. The entry type
 * is unique in a profile table, but can be shared across different tables.
 */
typedef enum bcmi_ltsw_sbr_profile_ent_type_e {
    BCMI_LTSW_SBR_PET_DEF = 0,

    BCMI_LTSW_SBR_PET_NONE = 1,

    BCMI_LTSW_SBR_PET_NO_CLASSID = 2,

    BCMI_LTSW_SBR_PET_NO_CLASS_ID = 3,

    BCMI_LTSW_SBR_PET_NO_CNG_NO_DESTINATION_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID = 4,

    BCMI_LTSW_SBR_PET_NO_CNG_NO_IFP_OPAQUE_CTRL_ID = 5,

    BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_IFP_OPAQUE_CTRL_ID_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID = 6,

    BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID = 7,

    BCMI_LTSW_SBR_PET_NO_DOT1P_PTR = 8,

    BCMI_LTSW_SBR_PET_NO_DVP = 9,

    BCMI_LTSW_SBR_PET_NO_DVP_NO_MEMBERSHIP_STG = 10,

    BCMI_LTSW_SBR_PET_NO_FORWARDING_TYPE_NO_SYSTEM_OPCODE = 11,

    BCMI_LTSW_SBR_PET_NO_IP_DIP_NO_SEQ_NUM = 12,

    BCMI_LTSW_SBR_PET_NO_L2_IIF = 13,

    BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF = 14,

    BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF_NO_SVP = 15,

    BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP = 16,

    BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL = 17,

    BCMI_LTSW_SBR_PET_NO_L3_VPN_INDEX_NO_TUNNEL_IDX_NO_VFI = 18,

    BCMI_LTSW_SBR_PET_NO_MAC_DA = 19,

    BCMI_LTSW_SBR_PET_NO_MAC_SA = 20,

    BCMI_LTSW_SBR_PET_NO_NO_CUT_THRU = 21,

    BCMI_LTSW_SBR_PET_NO_SEQ_NUM_NO_TNL_WORD_2 = 22,

    BCMI_LTSW_SBR_PET_NO_TAG = 23,

    BCMI_LTSW_SBR_PET_NO_TNL_WORD_2 = 24,

    BCMI_LTSW_SBR_PET_NO_VRF = 25,

    /*! Always the last. */
    BCMI_LTSW_SBR_PET_COUNT
} bcmi_ltsw_sbr_profile_ent_type_t;

#define BCMI_LTSW_SBR_PET_STR \
{ \
    "BCMI_LTSW_SBR_PET_DEF", \
    "BCMI_LTSW_SBR_PET_NONE", \
    "BCMI_LTSW_SBR_PET_NO_CLASSID", \
    "BCMI_LTSW_SBR_PET_NO_CLASS_ID", \
    "BCMI_LTSW_SBR_PET_NO_CNG_NO_DESTINATION_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID", \
    "BCMI_LTSW_SBR_PET_NO_CNG_NO_IFP_OPAQUE_CTRL_ID", \
    "BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_IFP_OPAQUE_CTRL_ID_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID", \
    "BCMI_LTSW_SBR_PET_NO_DESTINATION_NO_L2MC_L3MC_L2_OIF_SYS_DST_VALID", \
    "BCMI_LTSW_SBR_PET_NO_DOT1P_PTR", \
    "BCMI_LTSW_SBR_PET_NO_DVP", \
    "BCMI_LTSW_SBR_PET_NO_DVP_NO_MEMBERSHIP_STG", \
    "BCMI_LTSW_SBR_PET_NO_FORWARDING_TYPE_NO_SYSTEM_OPCODE", \
    "BCMI_LTSW_SBR_PET_NO_IP_DIP_NO_SEQ_NUM", \
    "BCMI_LTSW_SBR_PET_NO_L2_IIF", \
    "BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF", \
    "BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF_NO_SVP", \
    "BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP", \
    "BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL", \
    "BCMI_LTSW_SBR_PET_NO_L3_VPN_INDEX_NO_TUNNEL_IDX_NO_VFI", \
    "BCMI_LTSW_SBR_PET_NO_MAC_DA", \
    "BCMI_LTSW_SBR_PET_NO_MAC_SA", \
    "BCMI_LTSW_SBR_PET_NO_NO_CUT_THRU", \
    "BCMI_LTSW_SBR_PET_NO_SEQ_NUM_NO_TNL_WORD_2", \
    "BCMI_LTSW_SBR_PET_NO_TAG", \
    "BCMI_LTSW_SBR_PET_NO_TNL_WORD_2", \
    "BCMI_LTSW_SBR_PET_NO_VRF" \
}

/*!
 * \brief Get entry index per strength profile table handle and entry type.
 *
 * This function is used to get strength profile entry index for specified
 * strength table handle and entry type. This function can be called in case
 * that the LT table points to a strength profile.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to strength profile table.
 * \param [in] ent_type Strength profile entry type.
 * \param [out] index   Strength profile entry index.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_sbr_profile_ent_index_get(int unit,
                                    bcmi_ltsw_sbr_profile_tbl_hdl_t tbl_hdl,
                                    bcmi_ltsw_sbr_profile_ent_type_t ent_type,
                                    int *index);


/*!
 * \brief Get entry type per strength profile table handle and entry index.
 *
 * This function is used to get strength profile entry type for specified
 * strength table handle and entry index. This function can be called in case
 * that the LT table points to a strength profile.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to strength profile table.
 * \param [in] index    Strength profile entry index.
 * \param [out]ent_type Strength profile entry type.

 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_sbr_profile_ent_type_get(int unit,
                                   bcmi_ltsw_sbr_profile_tbl_hdl_t tbl_hdl,
                                   int index,
                                   bcmi_ltsw_sbr_profile_ent_type_t *ent_type);

/*!
 * \brief Handle to the table which has strength field.
 *
 * The table handle is used to identify the table which has strength field.
 * New table handle should be added at the last.
 */
typedef enum bcmi_ltsw_sbr_tbl_hdl_e {
    /*! EGR_L2_OIF. */
    BCMI_LTSW_SBR_TH_EGR_L2_OIF = 0,

    /*! EGR_L3_NEXT_HOP_1. */
    BCMI_LTSW_SBR_TH_EGR_L3_NEXT_HOP_1 = 1,

    /*! ING_SYSTEM_DESTINATION_TABLE. */
    BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE = 2,

    /*! ING_VLAN_XLATE_1_TABLE. */
    BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_1_TABLE = 3,

    /*! ING_VLAN_XLATE_2_TABLE. */
    BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_2_TABLE = 4,

    /*! ING_VLAN_XLATE_3_TABLE. */
    BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_3_TABLE = 5,

    /*! L3_IPV4_TUNNEL_TABLE. */
    BCMI_LTSW_SBR_TH_L3_IPV4_TUNNEL_TABLE = 6,

    /*! L3_IPV6_TUNNEL_TABLE. */
    BCMI_LTSW_SBR_TH_L3_IPV6_TUNNEL_TABLE = 7,

    /*! R_TUNNEL_L3_IIF_STRENGTH. */
    BCMI_LTSW_SBR_TH_R_TUNNEL_L3_IIF_STRENGTH = 8,

    /*! Always the last. */
    BCMI_LTSW_SBR_TH_COUNT
} bcmi_ltsw_sbr_tbl_hdl_t;

/*!
 * \brief Strength Field Type.
 *
 * The field type is used to address the strength field. The field type
 * is unique in a profile table, but can be shared across different tables.
 */
typedef enum bcmi_ltsw_sbr_fld_type_e {
    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_1_TABLE
     * BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_2_TABLE
     * BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_3_TABLE
     * BCMI_LTSW_SBR_TH_L3_IPV4_TUNNEL_TABLE
     * BCMI_LTSW_SBR_TH_L3_IPV6_TUNNEL_TABLE
     */
    BCMI_LTSW_SBR_FT_L3_IIF = 0,

    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_1_TABLE
     * BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_2_TABLE
     * BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_3_TABLE
     */
    BCMI_LTSW_SBR_FT_VFI = 1,

    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE
     */
    BCMI_LTSW_SBR_FT_L2_OIF = 2,

    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_ING_SYSTEM_DESTINATION_TABLE
     */
    BCMI_LTSW_SBR_FT_L2MC_L3MC_L2_OIF_SYS_DST_VALID = 3,

    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_EGR_L3_NEXT_HOP_1
     */
    BCMI_LTSW_SBR_FT_TOS_PTR = 4,

    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_EGR_L2_OIF
     */
    BCMI_LTSW_SBR_FT_DOT1P_PTR = 5,

    /*!
     * Available table handles -
     * R_TUNNEL_L3_IIF_STRENGTH
     */
    BCMI_LTSW_SBR_FT_VALUE = 6,

    /*!
     * Available table handles -
     * BCMI_LTSW_SBR_TH_EGR_L2_OIF
     */
    BCMI_LTSW_SBR_FT_DOT1P_PTR_HIGH = 7,

    /*! Always the last. */
    BCMI_LTSW_SBR_FT_COUNT
} bcmi_ltsw_sbr_fld_type_t;

/*!
 * \brief Get strength field value per table handle and field type.
 *
 * This function is used to get strength field value per table handle and
 * field type. This function can be called on the configuration of LT table
 * which has strength field.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to table which has strength field.
 * \param [in] fld_type Field type.
 * \param [out] value   Strength value.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_sbr_fld_value_get(int unit,
                            bcmi_ltsw_sbr_tbl_hdl_t tbl_hdl,
                            bcmi_ltsw_sbr_fld_type_t fld_type,
                            int *value);

/*!
 * \brief Get strength field type per table handle and field value.
 *
 * This function is used to get strength field type per table handle and
 * field index. This function can be called on the configuration of LT table
 * which has strength field.
 *
 * \param [in] unit     Unit Number.
 * \param [in] tbl_hdl  Handle to table which has strength field.
 * \param [in] value    Strength value.
 * \param [out] fld_type Field type.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_sbr_fld_type_get(int unit,
                           bcmi_ltsw_sbr_tbl_hdl_t tbl_hdl,
                           int value,
                           bcmi_ltsw_sbr_fld_type_t *fld_type);

/*!
 * \brief De-initialize the resource of SBR module.
 *
 * \param [in] unit     Unit Number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_sbr_detach(int unit);

/*!
 * \brief Initialize the resource of SBR module.
 *
 * \param [in] unit     Unit Number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_sbr_init(int unit);

/*!
 * \brief Display SBR software structure information.
 *
 * \retval None
 */
extern void
bcmi_ltsw_sbr_sw_dump(int unit);

#endif /* BCMI_LTSW_SBR_H */
