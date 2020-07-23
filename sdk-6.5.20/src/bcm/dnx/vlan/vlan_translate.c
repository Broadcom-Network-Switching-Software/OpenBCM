/** \file vlan_translate.c
 *
 *  VLAN port translate procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/switch/switch_tpid.h>
#include <bcm_int/dnx/port/port_tpid.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <shared/util.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * Conversions from BCM API to HW DBAL
 */

/**
 * \brief VLAN Translation VID action information in BCM API format
 * This structure contains VID actioninformation in a BCM API format.
 */
typedef struct
{
    /**
     *
     * * VID action for an Outer Tag
     */
    bcm_vlan_action_t outer_tag_action;
    /**
     *
     * * VID action for Inner Tag
     */
    bcm_vlan_action_t inner_tag_action;
} dnx_vlan_translate_vid_action_bcm_format_t;

/**
 *  \brief VLAN Translation enumeration for VID Sources. VLAN Translation enumeration for VID Sources. This
 * enumeration is later on mapped to the DBAL enumeration for IVE or EVE.
 */
typedef enum
{
    FIRST_DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_E = 0,
     /**
      *  No VLAN Edit action for the tag
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY = FIRST_DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_E,
     /**
      *  VID Source is the AC LIF Outer VID
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID,
     /**
      *  VID Source is the AC LIF Inner VID
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID,
    /**
      *  VID Source is the Incoming packet Outer VID
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID,
    /**
      *  VID Source is the Incoming packet Inner VID
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID,
     /**
      *  VID Source is the Forward domain (VSI)
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN,
     /**
      *  VID Source is the ARP+AC LIF Outer VID
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_OUTER_VID,
     /**
      *  VID Source is the ARP+AC LIF Inner VID
      */
    DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_INNER_VID,

    NUM_DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_E
} dnx_vlan_translate_vid_source_type_e;

/**
 *  \brief VLAN Translation enumeration for PCP-DEI Sources. VLAN Translation enumeration for PCP-DEI Sources. This
 * enumeration is later on mapped to the DBAL enumeration for IVE or EVE.
 */
typedef enum
{
    FIRST_DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_E = 0,
     /**
      * No PCP_DEI Source for the tag
      * Note: only valid for IVE.
      */
    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_EMPTY = FIRST_DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_E,
     /**
      *  PCP_DEI Source is the AC LIF Inner PCP-DEI mapping
      */
    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_INNER_PCP_DEI,
     /**
      *  PCP-DEI Source is the AC LIF Outer PCP-DEI mapping
      */
    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_OUTER_PCP_DEI,
     /**
      *  PCP_DEI Source is the Incoming packet Inner PCP-DEI
      */
    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_INNER_PCP_DEI,
     /**
      *  PCP_DEI Source is the Incoming packet Outer PCP-DEI
      */
    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_OUTER_PCP_DEI,

    NUM_DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_E
} dnx_vlan_translate_pcp_dei_source_type_e;

/**
 *  \brief VLAN Translation VID Action information in DNX HW format This structure contains VID action information in
 * a DNX HW format.
 */
typedef struct
{
    /**
     *
     * * Number of VLAN tags to remove during this VLAN Translation or -1 if invalid
     */
    int nof_tags_to_remove;
    /**
     *
     * * VID Source for added Outer Tag
     */
    dnx_vlan_translate_vid_source_type_e vid_source_outer;
    /**
     *
     * * VID Source for added Inner Tag
     */
    dnx_vlan_translate_vid_source_type_e vid_source_inner;
} dnx_vlan_translate_vid_action_dnx_format_t;

/**
 * \brief VLAN Translation information in DNX HW format
 * This structure contains VLAN Translation command information in a DNX HW format.
 * DNX HW format. It stores the number of tags to remove and the
 * It stores the number of tags to remove and the sources from which to build up to two new tags: Outer & inner.
 */
typedef struct
{
    /**
     *
     * * Number of VLAN tags to remove during this VLAN Translation or -1 if invalid
     */
    int nof_tags_to_remove;
    /**
     *
     * * VID Source for added Outer Tag
     */
    dnx_vlan_translate_vid_source_type_e vid_source_outer;
    /**
     *
     * * VID Source for added Inner Tag
     */
    dnx_vlan_translate_vid_source_type_e vid_source_inner;
    /**
     *
     * * PCP-DEI Source for added Outer Tag
     */
    dnx_vlan_translate_pcp_dei_source_type_e pcp_dei_source_outer;
    /**
     *
     * * PCP-DEI Source for added Inner Tag
     */
    dnx_vlan_translate_pcp_dei_source_type_e pcp_dei_source_inner;
    /**
     *
     * * Global TPID index for added Outer Tag
     */
    int outer_tpid_idx;
    /**
     *
     * * Global TPID index for added Inner Tag
     */
    int inner_tpid_idx;
} dnx_vlan_translate_action_dnx_format_t;

/**
 * \brief VLAN Translation conversion information.
 * This structure contains information that is required in order to convert a VLAN Translation command from
 * BCM API format to DNX HW format.
 * \see dnx_xxxxxx
 */
typedef struct
{
    /**
     *
     * * VLAN Translation action in BCM API format. Comprised of an action for each of the supported tags -
     *   Outer & Inner.
     */
    dnx_vlan_translate_vid_action_bcm_format_t vlan_action_bcm;
    /**
     *
     * * VLAN Translation action in DNX HW format. Comprised of the number of tags to remove and Sources from which to
     *   build the content of each of the tags - Outer & Inner.
     */
    dnx_vlan_translate_vid_action_dnx_format_t vlan_action_dnx;
    /**
     *
     * * Flag that determines if the BCM API format is valid for Ingress conversion.
     */
    uint8 is_valid_ingress_conversion;
    /**
     *
     * * Flag that determines if the BCM API format is valid for Egress conversion.
     */
    uint8 is_valid_egress_conversion;
} dnx_vlan_translate_vid_action_bcm_to_dnx_conversion_t;

/*
 * bcm-outer-vid, bcm-inner-vid,
 *      tags-to-remove, outer-vid-src, inner-vid-src, ingress-valid-conversion,egress-valid-conversion
 */
static dnx_vlan_translate_vid_action_bcm_to_dnx_conversion_t dnx_vlan_translation_conversion_info[] = {
/*
 {XXX, bcmVlanActionNone}:
*/
    {{bcmVlanActionNone, bcmVlanActionNone},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionReplace, bcmVlanActionNone},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionAdd, bcmVlanActionNone},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionDelete, bcmVlanActionNone},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionNone},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionNone},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionNone},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionOuterAdd, bcmVlanActionNone},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionInnerAdd, bcmVlanActionNone},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionNone},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
/*
 {XXX, bcmVlanActionReplace}:
*/
    {{bcmVlanActionNone, bcmVlanActionReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1,
     1},
    {{bcmVlanActionReplace, bcmVlanActionReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1, 1},
    {{bcmVlanActionAdd, bcmVlanActionReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionDelete, bcmVlanActionReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1,
     1},
    {{bcmVlanActionMappedReplace, bcmVlanActionReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionOuterAdd, bcmVlanActionReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionInnerAdd, bcmVlanActionReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionReplace},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionAdd}:
*/
    {{bcmVlanActionNone, bcmVlanActionAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionReplace, bcmVlanActionAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1, 1},
    {{bcmVlanActionAdd, bcmVlanActionAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1, 1},
    {{bcmVlanActionDelete, bcmVlanActionAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID}, 1,
     1},
    {{bcmVlanActionMappedReplace, bcmVlanActionAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 0, 1},
    {{bcmVlanActionOuterAdd, bcmVlanActionAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1,
     1},
    {{bcmVlanActionInnerAdd, bcmVlanActionAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1,
     1},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionAdd},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionDelete}:
*/
    {{bcmVlanActionNone, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionReplace, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionAdd, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1,
     1},
    {{bcmVlanActionDelete, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionDelete},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID},
     0, 1},
    {{bcmVlanActionOuterAdd, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1, 1},
    {{bcmVlanActionInnerAdd, bcmVlanActionDelete},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1, 1},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionDelete},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionCopy}:
*/
    {{bcmVlanActionNone, bcmVlanActionCopy},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1, 1},
    {{bcmVlanActionReplace, bcmVlanActionCopy},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1,
     1},
    {{bcmVlanActionAdd, bcmVlanActionCopy},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionDelete, bcmVlanActionCopy},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionCopy},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionCopy},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID},
     0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionCopy},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionOuterAdd, bcmVlanActionCopy},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionInnerAdd, bcmVlanActionCopy},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionCopy},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionMappedReplace}:
*/
    {{bcmVlanActionNone, bcmVlanActionMappedReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN},
     0, 1},
    {{bcmVlanActionReplace, bcmVlanActionMappedReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0, 1},
    {{bcmVlanActionAdd, bcmVlanActionMappedReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionDelete, bcmVlanActionMappedReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionCopy, bcmVlanActionMappedReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN},
     0, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionMappedReplace},
     {2, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0,
     1},
    {{bcmVlanActionMappedAdd, bcmVlanActionMappedReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionOuterAdd, bcmVlanActionMappedReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionInnerAdd, bcmVlanActionMappedReplace},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionMappedReplace},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionMappedAdd}:
*/
    {{bcmVlanActionNone, bcmVlanActionMappedAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionReplace, bcmVlanActionMappedAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 0, 1},
    {{bcmVlanActionAdd, bcmVlanActionMappedAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0, 1},
    {{bcmVlanActionDelete, bcmVlanActionMappedAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionCopy, bcmVlanActionMappedAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID},
     0, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionMappedAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0,
     1},
    {{bcmVlanActionMappedAdd, bcmVlanActionMappedAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0,
     1},
    {{bcmVlanActionOuterAdd, bcmVlanActionMappedAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN},
     0, 1},
    {{bcmVlanActionInnerAdd, bcmVlanActionMappedAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN},
     0, 1},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionMappedAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN}, 0,
     1},
/*
 {XXX, bcmVlanActionOuterAdd}:
*/
    {{bcmVlanActionNone, bcmVlanActionOuterAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionReplace, bcmVlanActionOuterAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1,
     1},
    {{bcmVlanActionAdd, bcmVlanActionOuterAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1,
     1},
    {{bcmVlanActionDelete, bcmVlanActionOuterAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionOuterAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID}, 1, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionOuterAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN},
     0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionOuterAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID},
     0, 1},
    {{bcmVlanActionOuterAdd, bcmVlanActionOuterAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1, 1},
    {{bcmVlanActionInnerAdd, bcmVlanActionOuterAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID}, 1, 1},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionOuterAdd},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionInnerAdd}:
*/
    {{bcmVlanActionNone, bcmVlanActionInnerAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionReplace, bcmVlanActionInnerAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID}, 1,
     1},
    {{bcmVlanActionAdd, bcmVlanActionInnerAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID}, 1,
     1},
    {{bcmVlanActionDelete, bcmVlanActionInnerAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 1, 1},
    {{bcmVlanActionCopy, bcmVlanActionInnerAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID}, 1, 1},
    {{bcmVlanActionMappedReplace, bcmVlanActionInnerAdd},
     {1, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN},
     0, 1},
    {{bcmVlanActionMappedAdd, bcmVlanActionInnerAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID},
     0, 1},
    {{bcmVlanActionOuterAdd, bcmVlanActionInnerAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID}, 1, 1},
    {{bcmVlanActionInnerAdd, bcmVlanActionInnerAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID,
      DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID}, 1, 1},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionInnerAdd},
     {-1, 0, 0}, 0, 0},
/*
 {XXX, bcmVlanActionArpVlanTranslateAdd}:
*/
    {{bcmVlanActionNone, bcmVlanActionArpVlanTranslateAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY}, 0, 1},
    {{bcmVlanActionReplace, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionAdd, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionArpVlanTranslateAdd, bcmVlanActionArpVlanTranslateAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_OUTER_VID, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_INNER_VID},
     0, 1},
    {{bcmVlanActionDelete, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionCopy, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionMappedReplace, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionMappedAdd, bcmVlanActionArpVlanTranslateAdd},
     {0, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN, DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_INNER_VID}, 0,
     1},
    {{bcmVlanActionOuterAdd, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
    {{bcmVlanActionInnerAdd, bcmVlanActionArpVlanTranslateAdd},
     {-1, 0, 0}, 0, 0},
};

static dbal_enum_value_field_ive_outer_vid_src_e dnx_vlan_translate_ive_vid_action_mapping[] = {
     /**
      *  No VLAN Edit action for the tag -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY
      */
    DBAL_ENUM_FVAL_IVE_OUTER_VID_SRC_EMPTY,
     /**
      *  VID Source is the AC LIF Outer VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID
      */
    DBAL_ENUM_FVAL_IVE_OUTER_VID_SRC_AC_OUTER_VID,
     /**
      *  VID Source is the AC LIF Inner VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID
      */
    DBAL_ENUM_FVAL_IVE_OUTER_VID_SRC_AC_INNER_VID,
     /**
      *  VID Source is the Incoming packet Outer VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID
      */
    DBAL_ENUM_FVAL_IVE_OUTER_VID_SRC_INCOMING_OUTER_VID,
     /**
      *  VID Source is the Incoming packet Inner VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID
      */
    DBAL_ENUM_FVAL_IVE_OUTER_VID_SRC_INCOMING_INNER_VID
};

static dbal_enum_value_field_eve_outer_vid_src_e dnx_vlan_translate_eve_vid_action_mapping[] = {
     /**
      *  No VLAN Edit action for the tag -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY
      */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_EMPTY,
     /**
      *  VID Source is the AC LIF Outer VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_OUTER_VID
      */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_AC_OUTER_VID,
     /**
      *  VID Source is the AC LIF Inner VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_AC_INNER_VID
      */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_AC_INNER_VID,
     /**
      *  VID Source is the Incoming packet Outer VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID
      */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_INCOMING_OUTER_VID,
     /**
      *  VID Source is the Incoming packet Inner VID -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID
      */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_INCOMING_INNER_VID,
     /**
      *  VID Source is the Forward domain (VSI) -
      *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_FORWARD_DOMAIN
      */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_FORWARD_DOMAIN,
      /**
       *  VID Source is the ARP+AC LIF Outer VID -
       *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_OUTER_VID
       */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_ARP_AC_OUTER_VID,
      /**
       *  VID Source is the ARP+AC LIF Inner VID -
       *  DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_ARP_AC_INNER_VID
       */
    DBAL_ENUM_FVAL_EVE_OUTER_VID_SRC_ARP_AC_INNER_VID
};

static dbal_enum_value_field_ive_outer_pcp_dei_src_e dnx_vlan_translate_ive_pcp_dei_action_mapping[] = {
     /**
      *  PCP_DEI Source is EMPTY -
      *  DBAL_ENUM_FVAL_IVE_OUTER_PCP_DEI_SRC_EMPTY
      */
    DBAL_ENUM_FVAL_IVE_OUTER_PCP_DEI_SRC_EMPTY,

     /**
      *  PCP_DEI Source is the AC LIF Inner PCP-DEI mapping -
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_INNER_PCP_DEI
      */
    DBAL_ENUM_FVAL_IVE_OUTER_PCP_DEI_SRC_AC_INNER_PCP_DEI,
    /**
      *  PCP-DEI Source is the AC LIF Outer PCP-DEI mapping -
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_OUTER_PCP_DEI
      */
    DBAL_ENUM_FVAL_IVE_OUTER_PCP_DEI_SRC_AC_OUTER_PCP_DEI,
    /**
      *  PCP_DEI Source is the Incoming packet Inner PCP-DEI mapping
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_INNER_PCP_DEI
      */
    DBAL_ENUM_FVAL_IVE_OUTER_PCP_DEI_SRC_INCOMING_INNER_PCP_DEI,
     /**
      *  PCP_DEI Source is the Incoming packet Outer PCP-DEI mapping
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_OUTER_PCP_DEI
      */
    DBAL_ENUM_FVAL_IVE_OUTER_PCP_DEI_SRC_INCOMING_OUTER_PCP_DEI
};

const static dbal_enum_value_field_eve_pcp_dei_src_e dnx_vlan_translate_eve_pcp_dei_action_encap_mapping[] = {
     /**
      *  PCP_DEI Source is EMPTY -
      *  This source is not valid for Egress.
      *  We should not get here - thus can put any value.
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_0,
     /**
      *  PCP_DEI Source is the AC LIF Inner PCP-DEI mapping -
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_INNER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_0,
     /**
      *  PCP-DEI Source is the AC LIF Outer PCP-DEI mapping -
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_OUTER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_1,
     /**
      *  PCP_DEI Source is the Incoming packet Inner PCP-DEI
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_INNER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_2,
     /**
      *  PCP_DEI Source is the Incoming packet Outer PCP-DEI
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_OUTER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_3
};

const static dbal_enum_value_field_eve_pcp_dei_src_e dnx_vlan_translate_eve_pcp_dei_action_fwd_mapping[] = {
     /**
      *  PCP_DEI Source is EMPTY -
      *  This source is not valid for Egress.
      *  We should not get here - thus can put any value.
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_1,
     /**
      *  PCP_DEI Source is the AC LIF Inner PCP-DEI mapping -
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_INNER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_1,
    /**
      *  PCP-DEI Source is the AC LIF Outer PCP-DEI mapping -
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_OUTER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_0,
     /**
      *  PCP_DEI Source is the Incoming packet Inner PCP-DEI
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_INNER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_2,
     /**
      *  PCP_DEI Source is the Incoming packet Outer PCP-DEI
      *  DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_OUTER_PCP_DEI
      */
    DBAL_ENUM_FVAL_EVE_PCP_DEI_SRC_PCP_DEI_3
};

static int dnx_vlan_translate_action_class_set_egress_tx_enablers_set(
    int unit,
    uint32 entry_handle_id,
    uint32 remove_tx_tag,
    uint32 vlan_mirror,
    uint32 vlan_membership);

/**
 *  Verify function for verification of VLAN edit profile range
 */
static shr_error_e
dnx_vlan_translation_vlan_edit_profile_verify(
    int unit,
    uint32 flags,
    uint32 vlan_edit_profile_id)
{
    int is_ingress;
    uint32 nof_vlan_edit_profiles;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS);

    /*
     * Take the VLAN edit profile range from DNX Data:
     */
    if (is_ingress == TRUE)
    {
        nof_vlan_edit_profiles = dnx_data_vlan.vlan_translate.nof_ingress_vlan_edit_profiles_get(unit);
    }
    else
    {
        nof_vlan_edit_profiles = dnx_data_vlan.vlan_translate.nof_egress_vlan_edit_profiles_get(unit);
    }

    if (vlan_edit_profile_id >= nof_vlan_edit_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. vlan_edit_profile_id = %d. Maximum is %d. flags = 0x%08X, is_ingress = %d\n",
                     vlan_edit_profile_id, nof_vlan_edit_profiles, flags, is_ingress);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify function for verification of action_id range
 */
static shr_error_e
dnx_vlan_translation_action_id_verify(
    int unit,
    int vlan_translation_action_id,
    int is_ingress)
{

    uint32 nof_vlan_edit_action_ids;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Take the VLAN edit action IDs range from DNX Data:
     */
    if (is_ingress)
    {
        nof_vlan_edit_action_ids = dnx_data_vlan.vlan_translate.nof_ingress_vlan_edit_action_ids_get(unit);
    }
    else
    {
        nof_vlan_edit_action_ids = dnx_data_vlan.vlan_translate.nof_egress_vlan_edit_action_ids_get(unit);
    }

    if ((vlan_translation_action_id < 0) || (vlan_translation_action_id >= nof_vlan_edit_action_ids))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error, action_id = %d is out of range [0:%d]\n",
                     vlan_translation_action_id, nof_vlan_edit_action_ids - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Verify function for verification of tag_format_id
 *  range
 */
static shr_error_e
dnx_vlan_translation_tag_format_verify(
    int unit,
    uint32 flags,
    bcm_port_tag_format_class_t tag_format_id)
{
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;

    /*
     * Verify the the TAG-Struct was created:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_is_created_verify(unit, FALSE, is_ingress, tag_format_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Verify function for checking 'flags' as follows:
 *  - One of ingres/egress MUST be set.
 *  - Only one of ingres/egress is allowed to be set.
 * \see
 * dnx_vlan_port_translation_set_verify
 * dnx_vlan_port_translation_get_verify
 * dnx_vlan_translate_action_id_set_verify
 * dnx_vlan_translate_action_id_get_verify
 * dnx_vlan_translate_action_class_set_verify
 * dnx_vlan_translate_action_id_get_verify
 */
static shr_error_e
dnx_vlan_translate_flags_verify(
    int unit,
    uint32 flags)
{
    int is_ingress, is_egress;
    SHR_FUNC_INIT_VARS(unit);

    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    is_egress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_EGRESS) ? TRUE : FALSE;

    if ((is_ingress == FALSE) && (is_egress == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Neither INGRESS nor EGRESS are set!!! flags = 0x%08X\n",
                     flags);
    }

    if ((is_ingress == TRUE) && (is_egress == TRUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS and EGRESS are set!!! flags = 0x%08X\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for checking that action_id is already
 * allocated by resource management.
 * \see bcm_dnx_vlan_translate_action_id_create.
 */
static shr_error_e
dnx_vlan_translate_action_id_is_allocated_verify(
    int unit,
    uint32 flags,
    int action_id)
{
    int is_ingress;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    is_allocated = FALSE;

    if (is_ingress == TRUE)
    {
        SHR_IF_ERR_EXIT(vlan_db.action_id_ingress.is_allocated(unit, action_id, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ACTION_ID_INGRESS %d doesn't exist\n", action_id);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(vlan_db.action_id_egress.is_allocated(unit, action_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ACTION_ID_EGRESS %d doesn't exist\n", action_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Internal verify function for
 *        bcm_dnx_vlan_port_translation_set/get
 */
static shr_error_e
dnx_vlan_port_translation_set_get_gport_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that the gport type is VLAN PORT or a Virtual gport - Missing validation that virtual gport
     * is defined only for Egress configuration. MPLS Port is used temporarely.
     */
    if (BCM_GPORT_IS_VLAN_PORT(gport))
    {
        uint32 gport_subtype;

        gport_subtype = BCM_GPORT_SUB_TYPE_GET(gport);

        switch (gport_subtype)
        {
                /*
                 * Allowed sub types
                 */
            case BCM_GPORT_SUB_TYPE_LIF:

                break;

            case BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE:

                if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Wrong setting. gport = 0x%08X is Ingress BCM_VLAN_ACTION_SET_INGRESS (=0x%08X) is not set! flags = 0x%08X\n",
                                 gport, BCM_VLAN_ACTION_SET_INGRESS, flags);
                }

                break;

            case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH:
            case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT:
            case BCM_GPORT_SUB_TYPE_VLAN_TRANSLATION:

                if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_EGRESS) == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Wrong setting. gport = 0x%08X is Egress BCM_VLAN_ACTION_SET_EGRESS (=0x%08X) is not set! flags = 0x%08X\n",
                                 gport, BCM_VLAN_ACTION_SET_EGRESS, flags);
                }

                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport = 0x%08X is a VLAN-Port from a sub type that is not"
                             " supported for translation, gport_subtype = %d.", gport, gport_subtype);
        }
    }
    else if (BCM_GPORT_IS_EXTENDER_PORT(gport))
    {
        uint32 gport_subtype;

        gport_subtype = BCM_GPORT_SUB_TYPE_GET(gport);

        switch (gport_subtype)
        {
            case BCM_GPORT_SUB_TYPE_LIF:
                /*
                 * Allowed sub types
                 */
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Wrong setting. gport = 0x%08X is a EXTENDER Port from a sub type that is not"
                             " supported for translation, gport_subtype = %d.", gport, gport_subtype);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport = 0x%08X is not a VLAN or EXTENDER Port.", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_port_translation_set
 */
static shr_error_e
dnx_vlan_port_translation_set_verify(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the vlan_port_translation pointer is not NULL:
     */
    SHR_NULL_CHECK(vlan_port_translation, _SHR_E_PARAM, "vlan_port_translation");

    /*
     *  Verify vlan_port_translation->flags;
     *    - One of Ingress/Egress is set.
     *    - Only one of Ingress/Egress is set.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, vlan_port_translation->flags));

    /*
     * Verify vlan_port_translation->new_outer_vlan, new_inner_vlan;
     *      - the new VLAN values should be in range
     */
    BCM_DNX_VLAN_CHK_ID(unit, vlan_port_translation->new_outer_vlan);
    BCM_DNX_VLAN_CHK_ID(unit, vlan_port_translation->new_inner_vlan);

    /*
     * Verification that 'vlan_port_translation->gport' is within range is done by DNX Algo Gport Managment.
     * See dnx_algo_gpm_gport_to_hw_resources.
     */

    /*
     * Validate the VLAN edit profile, verify vlan_port_translation->vlan_edit_class_id
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_vlan_edit_profile_verify
                    (unit, vlan_port_translation->flags, vlan_port_translation->vlan_edit_class_id));

    /*
     * Verify that the gport type is VLAN PORT or a Virtual gport
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_get_gport_verify
                    (unit, vlan_port_translation->gport, vlan_port_translation->flags));

    /*
     * Egress ESEM default specific validations
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        int default_dual_homing_val;

        /*
         * Verify that the configuration isn't requested for a fixed entry default
         */
        default_dual_homing_val = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);
        if (BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port_translation->gport) == default_dual_homing_val)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. gport = 0x%08X represents a fixed value VIRTUAL EGERSS DEFUALT entry!\n",
                         vlan_port_translation->gport);
        }
    }

    /*
     * Egress vlan translation
     */
    if (!_SHR_IS_FLAG_SET(vlan_port_translation->flags, BCM_VLAN_ACTION_SET_INGRESS))
    {
        uint8 is_2tag_valid = TRUE;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

        /*
         * Get local In-LIF using DNX Algo Gport Management:
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, vlan_port_translation->gport,
                                                           DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                                                           &gport_hw_resources));
        
        if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_ARP)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                            (unit, _SHR_CORE_ALL, gport_hw_resources.local_out_lif,
                             gport_hw_resources.outlif_dbal_table_id, FALSE, DBAL_FIELD_VLAN_EDIT_VID_2,
                             &is_2tag_valid));
        }
        else
        {
            if ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE) ||
                ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB) &&
                 ((gport_hw_resources.outlif_dbal_result_type ==
                   DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC)
                  || (BCM_DNX_ESEM_IS_RESULT_TYPE_NAMESPACE_VSI(gport_hw_resources.outlif_dbal_result_type))))
                || (gport_hw_resources.outlif_dbal_table_id ==
                    DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB
                    && gport_hw_resources.outlif_dbal_result_type ==
                    DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC))
            {
                is_2tag_valid = FALSE;
            }

        }
        /*
         * Not all entries support 2 tag vlan editing, if required but not supported return error
         */
        if ((is_2tag_valid == FALSE) && (vlan_port_translation->new_inner_vlan != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! The specified gport = 0x%08X does not support 2 tag vlan editing, new_inner_vlan = %d must be 0!\n",
                         vlan_port_translation->gport, vlan_port_translation->new_inner_vlan);
        }

        /*
         * Verify VLAN_EDIT_PROFILE:
         *
         * The ARP_AC result type that doesn't support 2 tag VLAN editing also supports a
         * reduced VLAN-Edit-Profile range. Validate the reduced range isn't exceeded.
         *
         * Note:
         * NAMESPACE_VSI with result-type ETPS_L2_FODO does not contains VIDs and VLAN_EDIT_PROFILE!
         */
        if (is_2tag_valid == FALSE)
        {
            int vlan_edit_profile_size, vlan_edit_profile_max = 0;
            uint32 candidate_resultType;

            if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB &&
                gport_hw_resources.outlif_dbal_result_type ==
                DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
            {
                /** the result type is going to be changed to ETPS_L2_FODO_VLANS */
                candidate_resultType = DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS;
            }
            else
            {
                candidate_resultType = gport_hw_resources.outlif_dbal_result_type;
            }

            SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                            (unit, gport_hw_resources.outlif_dbal_table_id, DBAL_FIELD_VLAN_EDIT_PROFILE, FALSE,
                             candidate_resultType, INST_SINGLE, &vlan_edit_profile_size));
            vlan_edit_profile_max = SHR_BITSET(&vlan_edit_profile_max, vlan_edit_profile_size) - 1;

            if (vlan_port_translation->vlan_edit_class_id > vlan_edit_profile_max)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Error! The specified gport = 0x%08x, vlan_edit_class_id = %d exceeds the max value = %d\n",
                             vlan_port_translation->gport, vlan_port_translation->vlan_edit_class_id,
                             vlan_edit_profile_max);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_port_translation_get
 */
static shr_error_e
dnx_vlan_port_translation_get_verify(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the vlan_port_translation pointer is not NULL:
     */
    SHR_NULL_CHECK(vlan_port_translation, _SHR_E_PARAM, "vlan_port_translation");

    /*
     *  Verify vlan_port_translation->flags;
     *    - One of Ingress/Egress is set.
     *    - Only one of Ingress/Egress is set.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, vlan_port_translation->flags));

    /*
     * Verification that 'vlan_port_translation->gport' is within range is done by DNX Algo Gport Managment.
     * See dnx_algo_gpm_gport_to_hw_resources.
     */

    /*
     * Verify that the gport type is VLAN PORT or special virtual port:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_get_gport_verify
                    (unit, vlan_port_translation->gport, vlan_port_translation->flags));

    /*
     * Egress ESEM default specific validations
     */
    if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(vlan_port_translation->gport))
    {
        int default_dual_homing_val;

        /*
         * Verify that the data retrieval isn't requested for an entry with no VLAN-Translation info
         */
        default_dual_homing_val = dnx_data_esem.default_result_profile.default_dual_homing_get(unit);
        if (BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(vlan_port_translation->gport) == default_dual_homing_val)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting. gport = 0x%08X represents a VIRTUAL EGERSS DEFUALT entry with no VLAN-Translation info!\n",
                         vlan_port_translation->gport);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_translate_action_id_create
 */
static shr_error_e
dnx_vlan_translate_action_id_create_verify(
    int unit,
    uint32 flags,
    int *action_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the action_id pointer is not NULL:
     */
    SHR_NULL_CHECK(action_id, _SHR_E_PARAM, "action_id");

    /*
     *  Verify flags;
     *      - One of ingress/egress is set.
     *      - Only one of ingress/egress is set.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, flags));

    /*
     * For WITH_ID:
     *  - verify the action id is in range.
     *  - verify that the action is not already allocated.
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_WITH_ID))
    {
        uint8 is_allocated;
        int is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS);

        SHR_IF_ERR_EXIT(dnx_vlan_translation_action_id_verify(unit, *action_id, is_ingress));

        if (is_ingress)
        {
            SHR_IF_ERR_EXIT(vlan_db.action_id_ingress.is_allocated(unit, *action_id, &is_allocated));

            if (is_allocated == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Error, WITH_ID (0x%08X) is set, ACTION_ID_INGRESS %d already exist, can't create it! flags = 0x%08X\n",
                             BCM_VLAN_ACTION_SET_WITH_ID, *action_id, flags);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(vlan_db.action_id_egress.is_allocated(unit, *action_id, &is_allocated));

            if (is_allocated == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_EXISTS,
                             "Error, WITH_ID (0x%08X) is set, ACTION_ID_EGRESS %d already exist, can't create it! flags = 0x%08X\n",
                             BCM_VLAN_ACTION_SET_WITH_ID, *action_id, flags);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_translate_action_id_set
 */
static shr_error_e
dnx_vlan_translate_action_id_set_verify(
    int unit,
    uint32 flags,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    int is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the action pointer is not NULL:
     */
    SHR_NULL_CHECK(action, _SHR_E_PARAM, "action");

    /*
     *  Verify flags;
     *      - One of ingress/egress is set.
     *      - Only one of ingress/egress is set.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, flags));

    /*
     * Verify action_id range
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_action_id_verify(unit, action_id, is_ingress));

    /*
     * Check whether the action_id was allocated:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_is_allocated_verify(unit, flags, action_id));

    /*
     * Verify action->dt_outer:
     *              - the outer tag action value should be in range
     */
    switch (action->dt_outer)
    {
        case bcmVlanActionNone:
        case bcmVlanActionAdd:
        case bcmVlanActionReplace:
        case bcmVlanActionDelete:
        case bcmVlanActionCopy:
        case bcmVlanActionOuterAdd:
        case bcmVlanActionInnerAdd:

            break;

        case bcmVlanActionMappedAdd:
        case bcmVlanActionMappedReplace:
        case bcmVlanActionArpVlanTranslateAdd:

            if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_EGRESS) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. dt_outer = %d which is applicable only for Egress only but EGRESS flag is not set! flags = 0x%08X\n",
                             action->dt_outer, flags);
            }

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported/Unknown dt_outer action. dt_outer = %d\n", action->dt_outer);
            break;

    }

    /*
     * Verify action->dt_inner:
     *              - the inner tag action value should be in range
     */

    switch (action->dt_inner)
    {
        case bcmVlanActionNone:
        case bcmVlanActionAdd:
        case bcmVlanActionReplace:
        case bcmVlanActionDelete:
        case bcmVlanActionCopy:
        case bcmVlanActionOuterAdd:
        case bcmVlanActionInnerAdd:

            break;

        case bcmVlanActionMappedAdd:
        case bcmVlanActionMappedReplace:
        case bcmVlanActionArpVlanTranslateAdd:

            if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_EGRESS) == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. dt_inner = %d which is applicable for Egress only but EGRESS flag is not set! flags = 0x%08X\n",
                             action->dt_inner, flags);
            }

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported/Unknown dt_inner action. dt_inner = %d\n", action->dt_inner);
            break;

    }

    /*
     * Verify action->dt_outer_pkt_prio:
     *              - the Outer packet priority action value should be in range
     */
    switch (action->dt_outer_pkt_prio)
    {
        case bcmVlanActionNone:
        case bcmVlanActionAdd:
        case bcmVlanActionReplace:
        case bcmVlanActionCopy:
        case bcmVlanActionOuterAdd:
        case bcmVlanActionInnerAdd:

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported/Unknown dt_outer_pkt_prio action. dt_outer_pkt_prio = %d\n",
                         action->dt_outer_pkt_prio);
            break;

    }

    /*
     * Verify action->dt_inner_pkt_prio:
     *              - the Inner packet priority action value should be in range
     */
    switch (action->dt_inner_pkt_prio)
    {
        case bcmVlanActionNone:
        case bcmVlanActionAdd:
        case bcmVlanActionReplace:
        case bcmVlanActionCopy:
        case bcmVlanActionOuterAdd:
        case bcmVlanActionInnerAdd:

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown/Unsupported dt_inner_pkt_prio action. dt_inner_pkt_prio = %d\n",
                         action->dt_inner_pkt_prio);
            break;

    }

    /*
     * Verify action->outer_tpid_action:
     *              - the outer tpid action value should be in range
     */
    switch (action->outer_tpid_action)
    {
        case bcmVlanTpidActionNone:
        case bcmVlanTpidActionModify:

            break;

        case bcmVlanTpidActionInner:
        case bcmVlanTpidActionOuter:

            /*
             * The "use packet's TPID" option is only valid for EVE
             */
            if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! outer_tpid_action = %d is only supported for Egress! flags = 0x%08X\n",
                             action->outer_tpid_action, flags);
            }
            /*
             * For "use packet's TPID" verify that TPID value is invalid
             */
            if (action->outer_tpid != BCM_SWITCH_TPID_VALUE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! outer_tpid_action = %d - use packet's outer/inner TPID  - thus outer_tpid (=0x%04X) needs to be set to invalid (=0x%04X)!\n",
                             action->outer_tpid_action, action->outer_tpid, BCM_SWITCH_TPID_VALUE_INVALID);
            }

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported/Unknown outer_tpid_action action. outer_tpid_action = %d\n",
                         action->outer_tpid_action);
            break;

    }

    /*
     * Verify action->inner_tpid_action:
     *              - the inner tpid action value should be in range
     */
    switch (action->inner_tpid_action)
    {
        case bcmVlanTpidActionNone:
        case bcmVlanTpidActionModify:

            break;

        case bcmVlanTpidActionInner:
        case bcmVlanTpidActionOuter:

            /*
             * The "use packet's TPID" option is only valid for EVE
             */
            if (_SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! inner_tpid_action = %d is only supported for Egress! flags = 0x%08X\n",
                             action->inner_tpid_action, flags);
            }

            /*
             * For "use packet's TPID" verify that TPID value is invalid
             */
            if (action->inner_tpid != BCM_SWITCH_TPID_VALUE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! inner_tpid_action = %d - use packet's outer/inner TPID  - thus inner_tpid (=0x%04X) needs to be set to invalid (=0x%04X)!\n",
                             action->inner_tpid_action, action->inner_tpid, BCM_SWITCH_TPID_VALUE_INVALID);
            }

            break;

        default:

            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported/Unknown inner_tpid_action action. inner_tpid_action = %d\n",
                         action->inner_tpid_action);
            break;

    }

    /*
     * Verify unsupport configuration:
     */
    if (action->new_inner_pkt_prio ||
        action->new_outer_cfi ||
        action->new_inner_cfi ||
        action->ingress_if ||
        action->priority ||
        action->dt_outer_prio ||
        action->dt_outer_cfi ||
        action->dt_inner_prio ||
        action->dt_inner_cfi ||
        action->ot_outer_prio ||
        action->ot_inner ||
        action->ot_inner_pkt_prio ||
        action->ot_inner_cfi ||
        action->it_outer ||
        action->it_outer_pkt_prio ||
        action->it_outer ||
        action->it_outer_pkt_prio ||
        action->it_outer_cfi ||
        action->it_inner_prio ||
        action->it_inner_pkt_prio ||
        action->it_inner_cfi ||
        action->ut_outer ||
        action->ut_outer_pkt_prio ||
        action->ut_outer_cfi ||
        action->ut_inner ||
        action->ut_inner_pkt_prio ||
        action->ut_inner_cfi ||
        action->outer_pcp ||
        action->inner_pcp ||
        action->policer_id ||
        action->action_id ||
        action->class_id ||
        action->taf_gate_primap ||
        action->outer_tag ||
        action->inner_tag ||
        action->forwarding_domain ||
        action->inner_qos_map_id != (-1) || action->outer_qos_map_id != (-1) || action->ot_outer || action->it_inner)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error, No support for new_inner_pkt_prio, new_outer_cfi, new_inner_cfi, ingress_if, priority, dt_outer_prio, dt_outer_cfi, dt_inner_prio, dt_inner_cfi, ot_outer_prio, ot_inner, ot_inner_pkt_prio, ot_inner_cfi, it_outer, it_outer_pkt_prio, it_outer, it_outer_pkt_prio, it_outer_cfi, it_inner_prio, it_inner_pkt_prio, it_inner_cfi, ut_outer, ut_outer_pkt_prio, ut_outer_cfi, ut_inner, ut_inner_pkt_prio, ut_inner_cfi, outer_pcp, inner_pcp, policer_id, action_id, class_id, taf_gate_primap, outer_tag, inner_tag, forwarding_domain, inner_qos_map_id, outer_qos_map_id, ot_outer, it_inner");
    }

    /*
     * Verify action->outer_tpid:
     * The new outer tag tpid value can be one of these:
     * - Invalid value (i.e 0x0000).
     * - One of the global TPIDs value (checking this case is done while converting the TPIDs from BCM API format to DNX HW, see dnx_vlan_translate_tpid_action_to_dnx).
     */

    /*
     * Verify action->inner_tpid:
     * The new inner tag tpid value can be one of these:
     * - Invalid value (i.e 0x0000).
     * - One of the global TPIDs value (checking this case is done while converting the TPIDs from BCM API format to DNX HW, see dnx_vlan_translate_tpid_action_to_dnx).
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_translate_action_id_get
 */
static shr_error_e
dnx_vlan_translate_action_id_get_verify(
    int unit,
    uint32 flags,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    int is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the action pointer is not NULL:
     */
    SHR_NULL_CHECK(action, _SHR_E_PARAM, "action");

    /*
     * Verify flags;
     *  - One of ingress/egress is set.
     *  - Only one of ingress/egress is set.
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, flags));

    /*
     * Verify action_id range
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_action_id_verify(unit, action_id, is_ingress));

    /*
     * Check whether the action_id was allocated:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_is_allocated_verify(unit, flags, action_id));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_translate_action_class_set
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit-ID
 *   \param [in] action_class - Pointer to
 *       bcm_vlan_translate_action_class_t
 *   As input - Points to the action_class identifying the
 *   action_id and its properties.
 * \par INDIRECT INPUT:
 *    action_class
 *    See 'action_class' in DIRECT INPUT above
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
     action_class
 * \remark
     None
 * \see
     See .h file
 */
static shr_error_e
dnx_vlan_translate_action_class_set_verify(
    int unit,
    bcm_vlan_translate_action_class_t * action_class)
{
    int is_ingress = _SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_INGRESS);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the vlan_port_translation pointer is not NULL:
     */
    SHR_NULL_CHECK(action_class, _SHR_E_PARAM, "action_class");

    /*
     *  Verify action_class->flags
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, action_class->flags));

    /*
     * Verify Post-EVE flags
     */
    if (_SHR_IS_FLAG_SET(action_class->flags,
                         (BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP |
                          BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP |
                          BCM_VLAN_ACTION_SET_EGRESS_DISABLE_OUTER_TAG_REMOVAL |
                          BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING |
                          BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING)))
    {
        /*
         * Check egress-only flag:
         */
        if (!_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X. Post-EVE flags are set without BCM_VLAN_ACTION_SET_EGRESS (0x%08X) flag!\n",
                         action_class->flags, BCM_VLAN_ACTION_SET_EGRESS);
        }

        /*
         * Check VLAN-membership flags:
         */
        if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP) &&
            _SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X. Both Post-EVE VLAN-membership flags are set!"
                         "BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP (0x%08X)"
                         " and BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP (0x%08X)\n",
                         action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP,
                         BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP);
        }

        /*
         * Check VLAN-mirroring flags:
         */
        if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING) &&
            _SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error, flags = 0x%08X. Both Post-EVE VLAN-mirroing flags are set!"
                         "BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING (0x%08X)"
                         " and BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING (0x%08X)\n",
                         action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING,
                         BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING);
        }
    }

    /*
     * Verify vlan_translation_action_id
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_action_id_verify(unit, action_class->vlan_translation_action_id, is_ingress));

    /*
     * Validate the tag format
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_tag_format_verify
                    (unit, action_class->flags, action_class->tag_format_class_id));

    /*
     * Validate the VLAN edit profile
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_vlan_edit_profile_verify
                    (unit, action_class->flags, action_class->vlan_edit_class_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for
 *        bcm_dnx_vlan_translate_action_class_get
 */
static shr_error_e
dnx_vlan_translate_action_class_get_verify(
    int unit,
    bcm_vlan_translate_action_class_t * action_class)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Checking that the vlan_port_translation pointer is not NULL:
     */
    SHR_NULL_CHECK(action_class, _SHR_E_PARAM, "action_class");

    /*
     *  Verify action_class->flags
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_flags_verify(unit, action_class->flags));

    /*
     * Validate the tag format
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_tag_format_verify
                    (unit, action_class->flags, action_class->tag_format_class_id));

    /*
     * Validate the VLAN edit profile
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_vlan_edit_profile_verify
                    (unit, action_class->flags, action_class->vlan_edit_class_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Conversion function from a combination of VID
 *        actions (Outer & Inner), to a HW representation that
 *        is comprised of the tags to remove and the VID Source
 *        for each tag.
 *        The conversion is performed by looking up for a
 *        matching entry of VID actions in a dedicated static
 *        table.
 */
static shr_error_e
dnx_vlan_translate_vid_action_to_dnx(
    int unit,
    uint32 is_ingress,
    bcm_vlan_action_set_t * bcm_action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{
    uint32 tbl_idx, is_valid_conversion, nof_conversion_tbl_entries;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Lookup for the BCM API VID-Action combination in a conversion table
     */
    nof_conversion_tbl_entries =
        sizeof(dnx_vlan_translation_conversion_info) / sizeof(dnx_vlan_translation_conversion_info[0]);
    for (tbl_idx = 0; tbl_idx < nof_conversion_tbl_entries; tbl_idx++)
    {
        if ((dnx_vlan_translation_conversion_info[tbl_idx].vlan_action_bcm.outer_tag_action == bcm_action->dt_outer)
            && (dnx_vlan_translation_conversion_info[tbl_idx].vlan_action_bcm.inner_tag_action == bcm_action->dt_inner))
        {
            is_valid_conversion =
                (is_ingress) ? dnx_vlan_translation_conversion_info[tbl_idx].is_valid_ingress_conversion :
                dnx_vlan_translation_conversion_info[tbl_idx].is_valid_egress_conversion;

            if (is_valid_conversion)
            {
                dnx_vlan_translation_command->nof_tags_to_remove =
                    dnx_vlan_translation_conversion_info[tbl_idx].vlan_action_dnx.nof_tags_to_remove;
                dnx_vlan_translation_command->vid_source_outer =
                    dnx_vlan_translation_conversion_info[tbl_idx].vlan_action_dnx.vid_source_outer;
                dnx_vlan_translation_command->vid_source_inner =
                    dnx_vlan_translation_conversion_info[tbl_idx].vlan_action_dnx.vid_source_inner;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid VLAN Translation configuration with actions %d,%d at %s",
                             bcm_action->dt_outer, bcm_action->dt_inner, (is_ingress) ? "Ingress" : "Egress");
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Missing entry in VLAN Translation conversion table for actions %d,%d",
                 bcm_action->dt_outer, bcm_action->dt_inner);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Modify additional optional tags if the TPID action is set for them (that is it's TPID action is not "NONE").
 * Internal function that applies logic to determine whether the default
 * VID action configuration should be set to the Ingress/Egress HW, or
 * modifies the HW configuration, if PCP/TPID modifications
 * requires it.
 * The referred tag for modification (outer/inner) is according
 * to the packet after the user VLAN translation is applied. The
 * VID value to recover is according to its position in the
 * original packet (outer/inner).
 * \param [in] unit -
 *  Relevant unit.
 * \param [in] is_ingress -
 *  TRUE if INGRESS, FALSE if EGRESS
 * \param [in] bcm_action -
 *  pointer to bcm_vlan_action_set typedef structure containing
 *  VLAN tag action set
 * \param [in,out] dnx_vlan_translation_command -
 *  pointer to VLAN Translation information in DNX HW format.
 *  \b As \b input - \n
 *  vid_source_inner - VID Source for added Inner Tag
 *  vid_source_outer -  VID Source for added Outer Tag
 *  nof_tags_to_remove - Number of VLAN tags to remove during this VLAN Translation
 *  \b As \b output - \n
 *  If TPID modification is required, the following parameters may be changed:
 *  vid_source_inner - VID Source for added Inner Tag
 *  vid_source_outer -  VID Source for added Outer Tag
 *  nof_tags_to_remove - Number of VLAN tags to remove during this VLAN Translation
 * \return
 *     Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 *   bcm_dnx_vlan_translate_action_id_set
 */
static shr_error_e
dnx_vlan_translate_modify_optional_tags_action_to_dnx(
    int unit,
    uint32 is_ingress,
    bcm_vlan_action_set_t * bcm_action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{
    int nof_tags_to_construct;

    SHR_FUNC_INIT_VARS(unit);

    nof_tags_to_construct = 0;

    /*
     * Calculate the number of tags to be constructed.
     * Note: tag is constructed if it's VID source is not "EMPTY".
     */
    if (dnx_vlan_translation_command->vid_source_outer != DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY)
    {
        nof_tags_to_construct++;
        if (dnx_vlan_translation_command->vid_source_inner != DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY)
        {
            nof_tags_to_construct++;
        }
    }

    switch (dnx_vlan_translation_command->nof_tags_to_remove)
    {
        case 2:
            /*
             * Two tags has been removed.
             * New tags are constructed based on the user actions
             * bcm_action->dt_outer and bcm_action->dt_inner.
             * Nothing to do.
             */
            break;

        case 1:
            /*
             * One tag has been removed.
             * Need to decide based on the actual tags that being constructed.
             */
            switch (nof_tags_to_construct)
            {
                case 0:
                    /*
                     * One tag has been removed and no tag is being constructed.
                     * We are left with only one tag (originally the inner).
                     * Only the outer (the one tag that is left) can be edited based on the tpid action.
                     */
                    if (bcm_action->outer_tpid_action != bcmVlanTpidActionNone)
                    {
                        dnx_vlan_translation_command->nof_tags_to_remove++;
                        dnx_vlan_translation_command->vid_source_outer =
                            DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID;
                    }
                    break;

                case 1:
                    /*
                     * One tag has been removed and one tag is being constructed.
                     * The outer tag is edited based on the user action.
                     * Only the inner tag can be edit based on the tpid action.
                     */
                    if (bcm_action->inner_tpid_action != bcmVlanTpidActionNone)
                    {
                        dnx_vlan_translation_command->nof_tags_to_remove++;
                        dnx_vlan_translation_command->vid_source_inner =
                            DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID;
                    }
                    break;

                case 2:
                    /*
                     * Two new tags are constructed based on the user actions.
                     * Nothing to do.
                     */
                    break;
            }
            break;

        case 0:
            /*
             * No tag has been removed.
             * Need to decide based on the actual tags that are being constructed.
             */
            switch (nof_tags_to_construct)
            {
                case 0:
                    /*
                     * No tag has been removed or added (that is {none,none} option).
                     * The two tags can be edit based on the tpid action.
                     */
                    if (bcm_action->outer_tpid_action != bcmVlanTpidActionNone)
                    {
                        dnx_vlan_translation_command->nof_tags_to_remove++;
                        dnx_vlan_translation_command->vid_source_outer =
                            DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID;

                        if (bcm_action->inner_tpid_action != bcmVlanTpidActionNone)
                        {
                            dnx_vlan_translation_command->nof_tags_to_remove++;
                            dnx_vlan_translation_command->vid_source_inner =
                                DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_INNER_VID;
                        }
                    }
                    break;

                case 1:
                    /*
                     * No tag has been removed, one tag is added.
                     * The new tag is edited based on the user action.
                     * The inner tag (originally the outer tag) can be edited based on the tpid action.
                     */
                    if (bcm_action->inner_tpid_action != bcmVlanTpidActionNone)
                    {
                        dnx_vlan_translation_command->nof_tags_to_remove++;
                        dnx_vlan_translation_command->vid_source_inner =
                            DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_INCOMING_OUTER_VID;
                    }
                    break;

                case 2:
                    /*
                     * Two new tags are constructed based on the user actions.
                     * Nothing to do.
                     */
                    break;
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error!!! nof_tags_to_remove = %d, can only be 0,1, 2. nof_tags_to_construct=%d, vid_source_outer=%d, vid_source_inner=%d\n",
                         dnx_vlan_translation_command->nof_tags_to_remove, nof_tags_to_construct,
                         dnx_vlan_translation_command->vid_source_outer,
                         dnx_vlan_translation_command->vid_source_inner);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Conversion function for VLAN Translation PCP-DEI
 *        action in BCM API format to a DNX HW command value.
 *        The conversion is performed by
 */
static shr_error_e
dnx_vlan_translate_pcp_dei_action_to_dnx(
    int unit,
    bcm_vlan_action_set_t * bcm_action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{

    /*
     * Handle the PCP-DEI action for the Outer tag.
     * The conversion is relative to the tag position and therefore is different from Inner Tag conversion.
     */
    if (dnx_vlan_translation_command->vid_source_outer != DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY)
    {
        switch (bcm_action->dt_outer_pkt_prio)
        {
            case bcmVlanActionNone:
            case bcmVlanActionOuterAdd:
                dnx_vlan_translation_command->pcp_dei_source_outer =
                    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_OUTER_PCP_DEI;
                break;
            case bcmVlanActionAdd:
            case bcmVlanActionReplace:
                dnx_vlan_translation_command->pcp_dei_source_outer =
                    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_OUTER_PCP_DEI;
                break;
            case bcmVlanActionCopy:
            case bcmVlanActionInnerAdd:
            default:
                dnx_vlan_translation_command->pcp_dei_source_outer =
                    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_INNER_PCP_DEI;
                break;
        }
    }
    else
    {
        /*
         * Any valid value can be set when the tag isn't created as it isn't applicable/
         */
        dnx_vlan_translation_command->pcp_dei_source_outer = DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_EMPTY;
    }

    /*
     * Handle the PCP-DEI action for the Inner tag.
     * The conversion is relative to the tag position and therefore is different from Outer Tag conversion.
     */
    if (dnx_vlan_translation_command->vid_source_inner != DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY)
    {
        switch (bcm_action->dt_inner_pkt_prio)
        {
            case bcmVlanActionNone:
            case bcmVlanActionInnerAdd:
                dnx_vlan_translation_command->pcp_dei_source_inner =
                    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_INNER_PCP_DEI;
                break;
            case bcmVlanActionAdd:
            case bcmVlanActionReplace:
                dnx_vlan_translation_command->pcp_dei_source_inner =
                    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_AC_INNER_PCP_DEI;
                break;
            case bcmVlanActionCopy:
            case bcmVlanActionOuterAdd:
            default:
                dnx_vlan_translation_command->pcp_dei_source_inner =
                    DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_INCOMING_OUTER_PCP_DEI;
                break;
        }
    }
    else
    {
        /*
         * Any valid value can be set when the tag isn't created as it isn't applicable
         */
        dnx_vlan_translation_command->pcp_dei_source_inner = DNX_VLAN_TRANSLATION_PCP_DEI_SOURCE_TYPE_EMPTY;
    }

    return _SHR_E_NONE;
}

/**
 * \brief - Conversion function for VLAN Translation TPID value
 *        as in BCM API format to a DNX HW command value - TPID
 *        index.
 */
static shr_error_e
dnx_vlan_translate_tpid_action_to_dnx(
    int unit,
    bcm_vlan_action_set_t * bcm_action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the Global TPID-Index for a given TPID value, both for the Outer & Inner tags
     */
    if (bcm_action->outer_tpid == BCM_SWITCH_TPID_VALUE_INVALID)
    {
        /*
         * Take the TPID from Outer/Inner packet value:
         */
        if (bcm_action->outer_tpid_action == bcmVlanTpidActionOuter)
        {
            dnx_vlan_translation_command->outer_tpid_idx =
                dnx_data_vlan.vlan_translate.egress_tpid_outer_packet_index_get(unit);
        }
        else if (bcm_action->outer_tpid_action == bcmVlanTpidActionInner)
        {
            dnx_vlan_translation_command->outer_tpid_idx =
                dnx_data_vlan.vlan_translate.egress_tpid_inner_packet_index_get(unit);
        }
        else
        {

            if (dnx_vlan_translation_command->vid_source_outer != DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY)
            {
                /*
                 * The outer_tpid is relevant, it's value cannot be INVALID for the command!
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! Outer-TPID should be a valid global TPID for this action!!! - outer_tpid = 0x%04x action = {%d,%d}, vid_source_outer = %d\n",
                             bcm_action->outer_tpid, bcm_action->dt_outer, bcm_action->dt_inner,
                             dnx_vlan_translation_command->vid_source_outer);
            }

            /*
             * Since the tpid_index is irrelevant, set it to any valid global tpid index [0:6]
             */
            dnx_vlan_translation_command->outer_tpid_idx = 0;
        }
    }
    else
    {
        /*
         * Take the TPID from the command:
         */
        SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get
                        (unit, bcm_action->outer_tpid, &(dnx_vlan_translation_command->outer_tpid_idx)));

        if (dnx_vlan_translation_command->outer_tpid_idx == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
        {
            /*
             * The outer_tpid is not one of the global tpid
             */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! The specified Outer TPID isn't configured for the device - 0x%04x, action = {%d,%d}, vid_source_outer = %d\n",
                         bcm_action->outer_tpid, bcm_action->dt_outer, bcm_action->dt_inner,
                         dnx_vlan_translation_command->vid_source_outer);
        }
    }

    if (bcm_action->inner_tpid == BCM_SWITCH_TPID_VALUE_INVALID)
    {
        /*
         * Take the TPID from Outer/Inner packet value:
         */
        if (bcm_action->inner_tpid_action == bcmVlanTpidActionOuter)
        {
            dnx_vlan_translation_command->inner_tpid_idx =
                dnx_data_vlan.vlan_translate.egress_tpid_outer_packet_index_get(unit);
        }
        else if (bcm_action->inner_tpid_action == bcmVlanTpidActionInner)
        {
            dnx_vlan_translation_command->inner_tpid_idx =
                dnx_data_vlan.vlan_translate.egress_tpid_inner_packet_index_get(unit);
        }
        else
        {

            if (dnx_vlan_translation_command->vid_source_inner != DNX_VLAN_TRANSLATION_VID_SOURCE_TYPE_EMPTY)
            {
                /*
                 * The inner_tpid is relevant, it's value cannot be INVALID for the command!
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error! Inner-TPID should be a valid global TPID for this action!!! - inner_tpid = 0x%04x action = {%d,%d}, vid_source_inner = %d\n",
                             bcm_action->inner_tpid, bcm_action->dt_outer, bcm_action->dt_inner,
                             dnx_vlan_translation_command->vid_source_inner);
            }

            /*
             * Since the tpid_index is irrelevant, set it to any valid global tpid index [0:6]
             */
            dnx_vlan_translation_command->inner_tpid_idx = 0;
        }
    }
    else
    {
        /*
         * Take the TPID from the command:
         */
        SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get
                        (unit, bcm_action->inner_tpid, &(dnx_vlan_translation_command->inner_tpid_idx)));

        if (dnx_vlan_translation_command->inner_tpid_idx == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
        {
            /*
             * The inner_tpid is not one of the global tpid
             */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! The specified Inner TPID isn't configured for the device - 0x%04x, action = {%d,%d}, vid_source_inner = %d\n",
                         bcm_action->inner_tpid, bcm_action->dt_outer, bcm_action->dt_inner,
                         dnx_vlan_translation_command->vid_source_inner);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - conversion function from a BCM API VLAN Translation
 * action to a DNX HW VLAN Translation command info.
 * The function first converts the VID action. If that's
 * valid, it proceedes to determine the PCP-DEI and TPID-Index
 * for each added tag.
 */
static shr_error_e
dnx_vlan_translate_action_to_dnx_command(
    int unit,
    uint32 is_ingress,
    bcm_vlan_action_set_t * action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Convert the VID actions from BCM API format to DNX HW
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_vid_action_to_dnx(unit, is_ingress, action, dnx_vlan_translation_command));

    /*
     * Modify additional optional tags if the PCP/TPID is set for them
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_modify_optional_tags_action_to_dnx
                    (unit, is_ingress, action, dnx_vlan_translation_command));

    /*
     * Convert the PCP-DEI actions from BCM API format to DNX HW
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_pcp_dei_action_to_dnx(unit, action, dnx_vlan_translation_command));

    /*
     * Convert the TPIDs from BCM API format to DNX HW
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_tpid_action_to_dnx(unit, action, dnx_vlan_translation_command));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to configure INGRESS VLAN Transation
 * parameters per gport - In-LIF.
 * \see
 * bcm_dnx_vlan_port_translation_set
 */
static shr_error_e
dnx_vlan_port_translation_set_ingress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    /*
     * Get local In-LIF using DNX Algo Gport Management:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, vlan_port_translation->gport,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                       &gport_hw_resources));
    /*
     * Set the VLAN Editing values to the local In-LIF HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                 vlan_port_translation->vlan_edit_class_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                                 vlan_port_translation->new_outer_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE,
                                 vlan_port_translation->new_inner_vlan);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to configure EGRESS VLAN Translation
 * parameters in the given ESEM entry.
 * \see
 * bcm_dnx_vlan_port_translation_set
 */
static shr_error_e
dnx_vlan_port_translation_set_egress_data(
    int unit,
    uint32 entry_handle_id,
    bcm_vlan_port_translation_t * vlan_port_translation,
    uint8 set_two_vlan_tags)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                                 vlan_port_translation->new_outer_vlan);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                 vlan_port_translation->vlan_edit_class_id);

    /** Not all entries support 2 tag vlan editing */
    if (set_two_vlan_tags)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE,
                                     vlan_port_translation->new_inner_vlan);
    }

    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - ESEM NAMESPACE_VSI SW-STATE - update result type
 * Called by dnx_vlan_port_translation_set_egress_esem_namespace_vsi
 */
static shr_error_e
dnx_egress_esem_namespace_vsi_sw_state_result_type_update(
    int unit,
    bcm_gport_t gport,
    uint32 resultType)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Take table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &entry_handle_id));
    /*
     * Set key:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI);
    /*
     * Set result-type:
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, resultType);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to configure EGRESS VLAN Translation
 * parameters for a virtual ESEM Namespace-vsi LIF.
 * \see
 * bcm_dnx_vlan_port_translation_set
 */
static shr_error_e
dnx_vlan_port_translation_set_egress_esem_namespace_vsi(
    int unit,
    uint32 esem_handle_id,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 resultType;
    int vlan_translate_values_are_zero;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((vlan_port_translation->new_outer_vlan == 0) && (vlan_port_translation->new_inner_vlan == 0) &&
        (vlan_port_translation->vlan_edit_class_id == 0))
    {
        vlan_translate_values_are_zero = TRUE;
    }
    else
    {
        vlan_translate_values_are_zero = FALSE;
    }

    /*
     * Get the result-type:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, esem_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, esem_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        &resultType));

    if (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
    {
        if (vlan_translate_values_are_zero == FALSE)
        {
            /*
             * The entry currently does not contains vlan-translation values!
             * Change result-type to have vlan-translation fields (ETPS_L2_FODO_VLANS).
             */
            SHR_IF_ERR_EXIT(dbal_entry_result_type_update(unit,
                                                          esem_handle_id,
                                                          DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS,
                                                          DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT));

            dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                                         vlan_port_translation->new_outer_vlan);
            dbal_entry_value_field32_set(unit, esem_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                                         vlan_port_translation->vlan_edit_class_id);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, esem_handle_id, DBAL_COMMIT_UPDATE));

            /*
             * Result type was change, update SW STATE DB!
             */
            SHR_IF_ERR_EXIT(dnx_egress_esem_namespace_vsi_sw_state_result_type_update
                            (unit, vlan_port_translation->gport,
                             DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS));
        }
        else
        {
            /*
             * The entry currently does not contains vlan-translation values!
             * But, all the vlan-translation values are zero, thus nothing to do!
             */
        }
    }
    else if (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS)
    {

        if (vlan_translate_values_are_zero == FALSE)
        {
            /*
             * The entry contains vlan-translation values.
             * Update vlan-translate fields
             */
            SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_egress_data
                            (unit, esem_handle_id, vlan_port_translation, FALSE));

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, esem_handle_id, DBAL_COMMIT_UPDATE));

        }
        else
        {
            /*
             * The entry contains vlan-translation values.
             * But, all the vlan-translation values are zero, thus change the result-type and upadte the relevant fields.
             */

            /*
             * Change result-type to have vlan-translation fields (ETPS_L2_FODO).
             */
            SHR_IF_ERR_EXIT(dbal_entry_result_type_update(unit,
                                                          esem_handle_id,
                                                          DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO,
                                                          DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT));

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, esem_handle_id, DBAL_COMMIT_UPDATE));

            /*
             * Result type was change, update SW STATE DB!
             */
            SHR_IF_ERR_EXIT(dnx_egress_esem_namespace_vsi_sw_state_result_type_update
                            (unit, vlan_port_translation->gport,
                             DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO));
        }
    }
    else
    {

        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error!, accessing ESM Namespace-vsi DB table (%d) with the key generated from gport = 0x%08X but"
                     " resultType = %d is not supported.\n",
                     DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, vlan_port_translation->gport, resultType);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to configure EGRESS VLAN Transation
 * parameters per gport Out-LIF
 * \see
 * bcm_dnx_vlan_port_translation_set
 */
static shr_error_e
dnx_vlan_port_translation_set_egress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint8 is_2tag_valid = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local Out-LIF using DNX Algo Gport Managment:
     */
    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, vlan_port_translation->gport,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * Allocate handle and set its keys
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set
                    (unit, entry_handle_id, vlan_port_translation->gport, &gport_hw_resources));

    /*
     * Handle ESEM Namespace-vsi
     */
    if ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB) &&
        (BCM_DNX_ESEM_IS_RESULT_TYPE_NAMESPACE_VSI(gport_hw_resources.outlif_dbal_result_type)))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_egress_esem_namespace_vsi
                        (unit, entry_handle_id, vlan_port_translation));
    }
    else
    {
        /*
         * Handle one VID entires:
         *  - ESEM-Default
         *  - ESEM non-native: ETPS_AC
         *  - ESEM native: ETPS_AC
         */
        if ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE) ||
            (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB &&
             gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC) ||
            (gport_hw_resources.outlif_dbal_table_id ==
             DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB
             && gport_hw_resources.outlif_dbal_result_type ==
             DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC))
        {
            is_2tag_valid = FALSE;
        }
        
        else if (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_ARP)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                            (unit, _SHR_CORE_ALL, gport_hw_resources.local_out_lif,
                             gport_hw_resources.outlif_dbal_table_id, FALSE, DBAL_FIELD_VLAN_EDIT_VID_2,
                             &is_2tag_valid));
        }

        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_egress_data
                        (unit, entry_handle_id, vlan_port_translation, is_2tag_valid));

        if ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_OUT_AC) ||
            (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_EEDB_ARP) ||
            (gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE))
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to get the configuration of INGRESS VLAN
 * Translation parameters per gport In-LIF:
 * - vlan_edit_class_id.
 * - new_outer_vlan.
 * - new_inner_vlan.
 * \see
 * bcm_dnx_vlan_port_translation_get
 */
static shr_error_e
dnx_vlan_port_translation_get_ingress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local In-LIF using DNX Algo Gport Management:
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, vlan_port_translation->gport,
                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS,
                                                       &gport_hw_resources));
    /*
     * Get the VLAN Editing values:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                     &vlan_port_translation->vlan_edit_class_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                     &vlan_port_translation->new_outer_vlan));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, INST_SINGLE,
                     &vlan_port_translation->new_inner_vlan));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to get the configuration of EGRESS VLAN
 * Translation parameters in the given entry:
 * - vlan_edit_class_id.
 * - new_outer_vlan.
 * - new_inner_vlan.
 * \see
 * bcm_dnx_vlan_port_translation_get
 */
static shr_error_e
dnx_vlan_port_translation_get_egress_data(
    int unit,
    uint32 entry_handle_id,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint8 is_2tag_valid;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                     &vlan_port_translation->vlan_edit_class_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                                                        &vlan_port_translation->new_outer_vlan));

    
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_uint32_field_on_dbal_handle_get
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_VID_2, &is_2tag_valid,
                     &vlan_port_translation->new_inner_vlan));

    /*
     * Not all entries support 2 tag vlan editing, if irrelevant - set to 0
     */
    if (is_2tag_valid == FALSE)
    {
        vlan_port_translation->new_inner_vlan = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to get the configuration of EGRESS VLAN
 * Translation parameters in the given ESEM Namespace-vsi entry.
 * \see
 * bcm_dnx_vlan_port_translation_set
 */
static shr_error_e
dnx_vlan_port_translation_get_egress_esem_namespace_vsi(
    int unit,
    uint32 esem_handle_id,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 resultType;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, esem_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                        &resultType));
    /*
     * ESEM Namespace-vsi has only one VID, set inner to zero:
     */
    vlan_port_translation->new_inner_vlan = 0;

    if (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
    {
        /*
         * No vlan-translation info, set all returned values to zero:
         */
        vlan_port_translation->new_outer_vlan = 0;
        vlan_port_translation->vlan_edit_class_id = 0;
    }
    else if (resultType == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO_VLANS)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, esem_handle_id, DBAL_FIELD_VLAN_EDIT_VID_1, INST_SINGLE,
                         &vlan_port_translation->new_outer_vlan));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, esem_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, INST_SINGLE,
                         &vlan_port_translation->vlan_edit_class_id));
    }
    else
    {

        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error!, accessing ESM Namespace-vsi DB table (%d) with the key generated from gport = 0x%08X but"
                     " resultType = %d is not supported.\n",
                     DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, vlan_port_translation->gport, resultType);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function to get the configuration of EGRESS VLAN
 * Translation parameters per gport (Out-LIF):
 * - vlan_edit_class_id.
 * - new_outer_vlan.
 * - new_inner_vlan.
 * \see
 * bcm_dnx_vlan_port_translation_get
 */
static shr_error_e
dnx_vlan_port_translation_get_egress(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local Out-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, vlan_port_translation->gport,
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * Allocate handle and set its keys
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set
                    (unit, entry_handle_id, vlan_port_translation->gport, &gport_hw_resources));

    /*
     * Get all fields:
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Get the vlan-translation data:
     */

    /*
     * Handle ESEM Namespace-vsi
     */
    if ((gport_hw_resources.outlif_dbal_table_id == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB) &&
        (BCM_DNX_ESEM_IS_RESULT_TYPE_NAMESPACE_VSI(gport_hw_resources.outlif_dbal_result_type)))
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_get_egress_esem_namespace_vsi
                        (unit, entry_handle_id, vlan_port_translation));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_get_egress_data(unit, entry_handle_id, vlan_port_translation));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, set a VLAN translation ID instance with
 * tag actions for INGRESS - writes to HW table
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 */
static shr_error_e
dnx_vlan_translate_action_id_set_ingress_hw(
    int unit,
    int action_id,
    bcm_vlan_action_set_t * action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set the modified VLAN Translation command to the HW - Ingress
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE,
                                 dnx_vlan_translation_command->outer_tpid_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_VID_SRC, INST_SINGLE,
                                 dnx_vlan_translate_ive_vid_action_mapping
                                 [dnx_vlan_translation_command->vid_source_outer]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IVE_OUTER_PCP_DEI_SRC, INST_SINGLE,
                                 dnx_vlan_translate_ive_pcp_dei_action_mapping
                                 [dnx_vlan_translation_command->pcp_dei_source_outer]);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE,
                                 dnx_vlan_translation_command->inner_tpid_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IVE_INNER_VID_SRC, INST_SINGLE,
                                 dnx_vlan_translate_ive_vid_action_mapping
                                 [dnx_vlan_translation_command->vid_source_inner]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IVE_INNER_PCP_DEI_SRC, INST_SINGLE,
                                 dnx_vlan_translate_ive_pcp_dei_action_mapping
                                 [dnx_vlan_translation_command->pcp_dei_source_inner]);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE,
                                 dnx_vlan_translation_command->nof_tags_to_remove);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, set a VLAN translation ID instance with
 * tag actions for INGRESS - writes to SW state table.
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 */
static shr_error_e
dnx_vlan_translate_action_id_set_ingress_sw_state(
    int unit,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Save the INGRESS VLAN Translation command in the SW state table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IVEC_ACTION_ID_SW_INFO, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER, INST_SINGLE,
                                 action->dt_outer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER, INST_SINGLE,
                                 action->dt_inner);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER_PKT_PRIO, INST_SINGLE,
                                 action->dt_outer_pkt_prio);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER_PKT_PRIO, INST_SINGLE,
                                 action->dt_inner_pkt_prio);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID, INST_SINGLE,
                                 action->outer_tpid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID, INST_SINGLE,
                                 action->inner_tpid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID_ACTION, INST_SINGLE,
                                 action->outer_tpid_action);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID_ACTION, INST_SINGLE,
                                 action->inner_tpid_action);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, set a VLAN translation ID instance with
 * tag actions for EGRESS - writes to FWD/ENCAP HW table
 * (DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD/DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP).
 *
 * \param [in] unit - relevant unit.
 * \param [in] table_id -
 *          The relevant table - DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD or DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP.
 * \param [in] table_key - can be one of the two:
 *              action_id - the VLAN translation command ID - for DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD.
 *              vlan_edit_profile - the vlan edit profile ID - for DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP.
 * \param [in] dnx_vlan_translation_command - pointer
 * to structure that contains VLAN Translation command
 * information in a DNX HW format.
 * \param [in] post_eve_remove_tx_tag - Post-EVE enabler - remove TX tag.
 * \param [in] post_eve_vlan_mirror - Post-EVE enabler - VLAN-mirror.
 * \param [in] post_eve_vlan_membership - Post-EVE enabler - VLAN-membership.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 * dnx_vlan_translate_encap_egress_init
 */
static shr_error_e
dnx_vlan_translate_action_id_set_egress_hw_fwd_encap(
    int unit,
    dbal_tables_e table_id,
    int table_key,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command,
    uint32 post_eve_remove_tx_tag,
    uint32 post_eve_vlan_mirror,
    uint32 post_eve_vlan_membership)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((table_id != DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD)
        && (table_id != DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Error! table_id =  %d !!! Only %d (EVE FWD Stage) and %d (EVE ENCAP Stage) tables IDs are valid!!!\n",
                     table_id, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP);
    }

    /*
     * Set the modified VLAN Translation command to the HW - Egress
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (table_id == DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, table_key);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, table_key);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TPID_INDEX, INST_SINGLE,
                                 dnx_vlan_translation_command->outer_tpid_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_TPID_INDEX, INST_SINGLE,
                                 dnx_vlan_translation_command->inner_tpid_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_VID_SRC, INST_SINGLE,
                                 dnx_vlan_translate_eve_vid_action_mapping
                                 [dnx_vlan_translation_command->vid_source_outer]);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVE_INNER_VID_SRC, INST_SINGLE,
                                 dnx_vlan_translate_eve_vid_action_mapping
                                 [dnx_vlan_translation_command->vid_source_inner]);

    if ((dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_fwd_eve_pcp_diff_encap)) &&
        (table_id == DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, INST_SINGLE,
                                     dnx_vlan_translate_eve_pcp_dei_action_fwd_mapping
                                     [dnx_vlan_translation_command->pcp_dei_source_outer]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, INST_SINGLE,
                                     dnx_vlan_translate_eve_pcp_dei_action_fwd_mapping
                                     [dnx_vlan_translation_command->pcp_dei_source_inner]);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVE_OUTER_PCP_DEI_SRC, INST_SINGLE,
                                     dnx_vlan_translate_eve_pcp_dei_action_encap_mapping
                                     [dnx_vlan_translation_command->pcp_dei_source_outer]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVE_INNER_PCP_DEI_SRC, INST_SINGLE,
                                     dnx_vlan_translate_eve_pcp_dei_action_encap_mapping
                                     [dnx_vlan_translation_command->pcp_dei_source_inner]);
    }

    if (table_id == DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TAGS_TO_REMOVE, INST_SINGLE,
                                     dnx_vlan_translation_command->nof_tags_to_remove);
    }

    if (table_id == DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP)
    {
        /*
         * Set Post-EVE enablers:
         *  - REMOVE_TX_TAG
         *  - VLAN_MIRROR
         *  - VLAN_MEMBERSHIP
         */
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_class_set_egress_tx_enablers_set
                        (unit, entry_handle_id, post_eve_remove_tx_tag, post_eve_vlan_mirror,
                         post_eve_vlan_membership));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, set a VLAN translation ID instance with
 * tag actions for EGRESS - writes to HW table.
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 */
static shr_error_e
dnx_vlan_translate_action_id_set_egress_hw(
    int unit,
    int action_id,
    bcm_vlan_action_set_t * action,
    dnx_vlan_translate_action_dnx_format_t * dnx_vlan_translation_command)
{
    uint32 entry_handle_id;
    uint32 vlan_edit_profile;
    uint32 incoming_tag_struct, vlan_edit_cmd_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set the modified VLAN Translation command to the HW - FWD Egress
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_egress_hw_fwd_encap
                    (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD, action_id, dnx_vlan_translation_command, 0, 0, 0));

    /*
     * Handle Encap stage EVEC:
     * In case this command is pointed by tag_format = 0 (for UNTAG packet), needs to update also Encap stage EVEC table.
     * Go over all the EVEC mapping table for tag_format = 0 and check if mapped to this action_id.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * incoming_tag_struct key equals 0 (untag)
     * vlan_edit_cmd_id field equels the given action_id
     */
    incoming_tag_struct = TAG_FORMAT_NONE;
    vlan_edit_cmd_id = action_id;

    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, DBAL_CONDITION_EQUAL_TO,
                     &incoming_tag_struct, NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                     &vlan_edit_cmd_id, NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 post_eve_remove_tx_tag, post_eve_vlan_mirror, post_eve_vlan_membership;

        /*
         * This action id is pointed by UNTAG tag format, need to update also ENCAP table with this commad
         * Take the vlan_edit_profile key and update ENCAP table with this commad:
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, &vlan_edit_profile));

        /*
         * Take the Post-EVE enablers values:
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_POST_EVE_REMOVE_TX_TAG_ENABLE, INST_SINGLE,
                         &post_eve_remove_tx_tag));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MIRROR, INST_SINGLE, &post_eve_vlan_mirror));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MEMBERSHIP, INST_SINGLE,
                         &post_eve_vlan_membership));

        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_egress_hw_fwd_encap
                        (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP, vlan_edit_profile,
                         dnx_vlan_translation_command, post_eve_remove_tx_tag, post_eve_vlan_mirror,
                         post_eve_vlan_membership));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, set a VLAN translation ID instance with
 * tag actions for EGRESS - writes to SW state tables.
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 * bcm_dnx_vlan_translate_action_id_get
 */
static shr_error_e
dnx_vlan_translate_action_id_set_egress_sw_state(
    int unit,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Save the EGRESS VLAN Translation command in the SW state table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVEC_FRWRD_STAGE_ACTION_ID_INFO_SW, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER, INST_SINGLE,
                                 action->dt_outer);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER, INST_SINGLE,
                                 action->dt_inner);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER_PKT_PRIO, INST_SINGLE,
                                 action->dt_outer_pkt_prio);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER_PKT_PRIO, INST_SINGLE,
                                 action->dt_inner_pkt_prio);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID, INST_SINGLE,
                                 action->outer_tpid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID, INST_SINGLE,
                                 action->inner_tpid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID_ACTION, INST_SINGLE,
                                 action->outer_tpid_action);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID_ACTION, INST_SINGLE,
                                 action->inner_tpid_action);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Internal function, get a VLAN translation ID instance with
 * tag actions for INGRESS - reads the SW state table
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 * bcm_dnx_vlan_translate_action_id_get
 */
static shr_error_e
dnx_vlan_translate_action_id_get_ingress(
    int unit,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    uint32 entry_handle_id;
    uint32 tmp32bitVal;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the INGRESS VLAN Translation command from the SW state table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IVEC_ACTION_ID_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER, INST_SINGLE, &tmp32bitVal));
    action->dt_outer = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER, INST_SINGLE, &tmp32bitVal));
    action->dt_inner = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER_PKT_PRIO, INST_SINGLE, &tmp32bitVal));
    action->dt_outer_pkt_prio = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER_PKT_PRIO, INST_SINGLE, &tmp32bitVal));
    action->dt_inner_pkt_prio = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID, INST_SINGLE, &tmp32bitVal));
    action->outer_tpid = (uint16) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID, INST_SINGLE, &tmp32bitVal));
    action->inner_tpid = (uint16) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID_ACTION, INST_SINGLE, &tmp32bitVal));
    action->outer_tpid_action = (bcm_vlan_tpid_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID_ACTION, INST_SINGLE, &tmp32bitVal));
    action->inner_tpid_action = (bcm_vlan_tpid_action_t) tmp32bitVal;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, get a VLAN translation ID instance with
 * tag actions for EGRESS - reads the SW state table.
 * \see
 * bcm_dnx_vlan_translate_action_id_set
 * bcm_dnx_vlan_translate_action_id_get
 */
static shr_error_e
dnx_vlan_translate_action_id_get_egress(
    int unit,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    uint32 entry_handle_id;
    uint32 tmp32bitVal;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the EGRESS VLAN Translation command from the SW state table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVEC_FRWRD_STAGE_ACTION_ID_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER, INST_SINGLE, &tmp32bitVal));
    action->dt_outer = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER, INST_SINGLE, &tmp32bitVal));
    action->dt_inner = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_OUTER_PKT_PRIO, INST_SINGLE, &tmp32bitVal));
    action->dt_outer_pkt_prio = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_DT_INNER_PKT_PRIO, INST_SINGLE, &tmp32bitVal));
    action->dt_inner_pkt_prio = (bcm_vlan_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID, INST_SINGLE, &tmp32bitVal));
    action->outer_tpid = (uint16) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID, INST_SINGLE, &tmp32bitVal));
    action->inner_tpid = (uint16) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_OUTER_TPID_ACTION, INST_SINGLE, &tmp32bitVal));
    action->outer_tpid_action = (bcm_vlan_tpid_action_t) tmp32bitVal;

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_BCM_API_VLAN_EDIT_INNER_TPID_ACTION, INST_SINGLE, &tmp32bitVal));
    action->inner_tpid_action = (bcm_vlan_tpid_action_t) tmp32bitVal;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, clears an INGRESS VLAN translation ID instance -
 * clears the HW table entry.
 * \see
 * bcm_dnx_vlan_translate_action_id_destroy
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
static shr_error_e
dnx_vlan_translate_action_id_clear_ingress_hw(
    int unit,
    int action_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear VLAN Translation ID HW - Ingress
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, clear an INGRESS VLAN translation ID instance
 * - clears the SW state table.
 * \see
 * bcm_dnx_vlan_translate_action_id_destroy
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
static shr_error_e
dnx_vlan_translate_action_id_clear_ingress_sw_state(
    int unit,
    int action_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Clear INGRESS VLAN Translation command SW state table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IVEC_ACTION_ID_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, clears an EGRESS VLAN translation ID
 * - clears the FWD/ENCAP HW table
 * (DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD/DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP).
 * \see
 * bcm_dnx_vlan_translate_action_id_destroy
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
static shr_error_e
dnx_vlan_translate_action_id_clear_egress_hw_fwd_encap(
    int unit,
    dbal_tables_e table_id,
    int table_key)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((table_id != DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD)
        && (table_id != DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Error! table_id =  %d !!! Only %d (EVE FWD Stage) and %d (EVE ENCAP Stage) tables IDs are valid!!!\n",
                     table_id, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP);
    }

    /*
     * Clear VLAN Translation command HW - Egress
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (table_id == DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, table_key);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, table_key);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, clear an EGRESS VLAN translation ID
 * - clears the HW table.
 * \see
 * bcm_dnx_vlan_translate_action_id_destroy
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
static shr_error_e
dnx_vlan_translate_action_id_clear_egress_hw(
    int unit,
    int action_id)
{
    uint32 entry_handle_id;
    int vlan_edit_profile;
    uint32 vlan_edit_profile_min, vlan_edit_profile_max;
    uint32 evec_mapping_action_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear VLAN Translation command HW - FWD Egress
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_clear_egress_hw_fwd_encap
                    (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD, action_id));

    /*
     * Handle Encap stage EVEC:
     * In case this command is pointed by tag_format = 0 (for UNTAG packet), needs to clear also Encap stage EVEC table.
     * Go over all the EVEC mapping table for tag_format = 0 and check if mapped to this action_id.
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, DBAL_FIELD_VLAN_EDIT_PROFILE, TRUE, 0, 0,
                     DBAL_PREDEF_VAL_MIN_VALUE, &vlan_edit_profile_min));
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, DBAL_FIELD_VLAN_EDIT_PROFILE, TRUE, 0, 0,
                     DBAL_PREDEF_VAL_MAX_VALUE, &vlan_edit_profile_max));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));

    for (vlan_edit_profile = vlan_edit_profile_min; vlan_edit_profile <= vlan_edit_profile_max; vlan_edit_profile++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, TAG_FORMAT_NONE);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, vlan_edit_profile);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE,
                                   &evec_mapping_action_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (evec_mapping_action_id == action_id)
        {
            /*
             * This action id is pointed by UNTAG tag format, need to clear also ENCAP table
             */
            SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_clear_egress_hw_fwd_encap
                            (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP, vlan_edit_profile));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function, clear an EGRESS VLAN translation ID
 * - clearss the SW state table.
 * \see
 * bcm_dnx_vlan_translate_action_id_destroy
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
static shr_error_e
dnx_vlan_translate_action_id_clear_egress_sw_state(
    int unit,
    int action_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Clear EGRESS VLAN Translation command in the SW state table:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EVEC_FRWRD_STAGE_ACTION_ID_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, action_id);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function. It maps a
 * combination of tag-format and VLAN-Edit-Profile to a Ingress
 * Command-ID.
 *
 * \param [in] unit -
 *     Relevant unit.
 * \param [in] action_class -
 *     A pointer to a structure that holds the mapping
 *     parameters: tag-format, VLAN-Edit-Profile and the result
 *     Command-ID.
 *
 * \return
 *   int - Negative in case of an error, Zero in case of NO
 *   ERROR
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vlan_translate_action_class_set
 */
static int
dnx_vlan_translate_action_class_set_ingress(
    int unit,
    bcm_vlan_translate_action_class_t * action_class)
{
    uint32 entry_handle_id;
    int tag_struct_hw_id_ingress;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get HW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                    (unit, action_class->tag_format_class_id, &tag_struct_hw_id_ingress, NULL));

    /*
     * Perform IVEC-Mapping configuration
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, tag_struct_hw_id_ingress);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, action_class->vlan_edit_class_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE,
                                 action_class->vlan_translation_action_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function. Sets EVE tx-enablers.
 * In FWD stage, these enablers are per EVE command mapping.
 * In ENCAP stage, these enablers are per EVE command.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] entry_handle_id -the DBAL table handle to update.
 * \param [in] remove_tx_tag -remove TX tag enabler:
 *  0 - do not remove Tx-TAG
 *  1 - remove Outer VLAN for Tx-TAG
 * \param [in] vlan_mirror -vlan-mirroing enabler:
 *  0 - do not do mirror
 *  1 - use Outer VLAN
 *  2 - use Inner VLAN
 * \param [in] vlan_membership -vlan-membership enabler:
 *  0 - do not do check VLAN membership
 *  1 - use Outer VLAN
 *  2 - use Inner VLAN
  * \return
 *   int - Negative in case of an error, Zero in case of NO
 *   ERROR
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vlan_translate_action_class_set
 */
static int
dnx_vlan_translate_action_class_set_egress_tx_enablers_set(
    int unit,
    uint32 entry_handle_id,
    uint32 remove_tx_tag,
    uint32 vlan_mirror,
    uint32 vlan_membership)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_EVE_REMOVE_TX_TAG_ENABLE, INST_SINGLE,
                                 remove_tx_tag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MIRROR, INST_SINGLE, vlan_mirror);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MEMBERSHIP, INST_SINGLE,
                                 vlan_membership);

    SHR_EXIT();

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Internal function. It maps a combination of tag-format and
 * VLAN-Edit-Profile to a Egress Command-ID. The mapping is used
 * by the HW to index into the Forwarding stage EVEC table, but
 * not to the Encap stage EVEC table.
 * In case the tag_format is 0 (refer to UNTAG packet) ENCAP
 * EVEC table is also updated with the same command as in the
 * FWD EVEC table.
 * \param [in] unit -
 *      Relevant unit.
 * \param [in] action_class -
 *     A pointer to a structure that holds the mapping
 *     parameters: tag-format, VLAN-Edit-Profile and the result
 *     Command-ID.
 *
 * \return
 *   int - Negative in case of an error, Zero in case of NO
 *   ERROR
 *
 * \remark
 *   * None
 * \see
 *  bcm_dnx_vlan_translate_action_class_set
 */
static int
dnx_vlan_translate_action_class_set_egress(
    int unit,
    bcm_vlan_translate_action_class_t * action_class)
{
    uint32 entry_handle_id;
    int tag_struct_hw_id_egress;
    uint32 post_eve_remove_tx_tag, post_eve_vlan_mirror, post_eve_vlan_membership;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get HW tag-structure value:
     */
    SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                    (unit, action_class->tag_format_class_id, NULL, &tag_struct_hw_id_egress));

    /*
     * Perform EVEC-Mapping configuration
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, tag_struct_hw_id_egress);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE, action_class->vlan_edit_class_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE,
                                 action_class->vlan_translation_action_id);
    /*
     * Set Post-EVE enablers:
     *  - REMOVE_TX_TAG - default value is ENABLE
     *  - VLAN_MIRROR - default value is "use outer VLAN"
     *  - VLAN_MEMBERSHIP - default value is "use outer VLAN"
     */
    post_eve_remove_tx_tag = 1;
    if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_OUTER_TAG_REMOVAL))
    {
        post_eve_remove_tx_tag = 0;
    }

    post_eve_vlan_mirror = DBAL_ENUM_FVAL_POST_EVE_VLAN_MIRROR_USE_OUTER_VLAN;
    if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING))
    {
        post_eve_vlan_mirror = DBAL_ENUM_FVAL_POST_EVE_VLAN_MIRROR_DISABLED;
    }
    else if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING))
    {
        post_eve_vlan_mirror = DBAL_ENUM_FVAL_POST_EVE_VLAN_MIRROR_USE_INNER_VLAN;
    }

    post_eve_vlan_membership = DBAL_ENUM_FVAL_POST_EVE_VLAN_MEMBERSHIP_USE_OUTER_VLAN;
    if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP))
    {
        post_eve_vlan_membership = DBAL_ENUM_FVAL_POST_EVE_VLAN_MEMBERSHIP_DISABLED;
    }
    else if (_SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP))
    {
        post_eve_vlan_membership = DBAL_ENUM_FVAL_POST_EVE_VLAN_MEMBERSHIP_USE_INNER_VLAN;
    }

    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_class_set_egress_tx_enablers_set
                    (unit, entry_handle_id, post_eve_remove_tx_tag, post_eve_vlan_mirror, post_eve_vlan_membership));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Handle ENCAP EVEC
     */
    if (action_class->tag_format_class_id == TAG_FORMAT_NONE)
    {

        /*
         * First, check that the EVE action is allocated:
         */
        uint8 is_allocated;
        SHR_IF_ERR_EXIT(vlan_db.action_id_egress.is_allocated(unit,
                                                              action_class->vlan_translation_action_id, &is_allocated));
        if (is_allocated == TRUE)
        {
            /*
             * Since mapping UNTAG packet to EVEC command, need to sync ENCAP EVEC table:
             *  1. Read this EVEC command from the EVEC FWD table.
             *  2. Update ENCAP table
             */
            bcm_vlan_action_set_t evec_fwd_action;
            dnx_vlan_translate_action_dnx_format_t dnx_vlan_translation_command;

            /*
             *  1. Read this EVEC command from the EVEC FWD table:
             */
            SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_get_egress
                            (unit, action_class->vlan_translation_action_id, &evec_fwd_action));

            /*
             *  2. Update ENCAP table:
             */

            /*
             * Convert the VLAN Translation action from BCM API format to DNX HW info
             */
            SHR_IF_ERR_EXIT(dnx_vlan_translate_action_to_dnx_command
                            (unit, FALSE, &evec_fwd_action, &dnx_vlan_translation_command));

            /*
             * Set the modified VLAN Translation command to the Egress ENCAP HW
             */
            SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_egress_hw_fwd_encap
                            (unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_ENCAP, action_class->vlan_edit_class_id,
                             &dnx_vlan_translation_command, post_eve_remove_tx_tag, post_eve_vlan_mirror,
                             post_eve_vlan_membership));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API configures VLAN Transation parameters per gport -
 * In-LIF, Out-LIF, ESEM or Egress Default.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] vlan_port_translation -
 *     A pointer to structure that holds the VLAN Translation parameters.
 *    The fields flags and gport are input.
 * \par INDIRECT INPUT:
 *    vlan_port_translation->flags -indicates whether it refers to INGRESS or EGRESS (see vlan_port_translation above).
 *    vlan_port_translation->gport - the gport ID (see vlan_port_translation above).
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: invalid vid.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Update HW according to the supplied gport - In-LIF,
 *     Out-LIF, ESEM or Egress Default.
 */
/*
 * Set translation information for a vlan port
 */
shr_error_e
bcm_dnx_vlan_port_translation_set(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint8 is_ingress;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_translation_set_verify(unit, vlan_port_translation));

    is_ingress = _SHR_IS_FLAG_SET(vlan_port_translation->flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    /*
     * Handle the Ingress scenario
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_ingress(unit, vlan_port_translation));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_set_egress(unit, vlan_port_translation));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API get the configuration of VLAN Transation parameters
 * per gport (In-LIF, Out-LIF, ESEM or Egress Default):
 * - vlan_edit_class_id.
 * - new_outer_vlan.
 * - new_inner_vlan.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] vlan_port_translation -
 *     A pointer to structure that holds the VLAN Translation parameters.
 *    The fields flags and gport are input.
 * \par INDIRECT INPUT:
 *    vlan_port_translation->flags -indicates whether it refers to INGRESS or EGRESS (see vlan_port_translation above).
 *    vlan_port_translation->gport - the gport ID (see vlan_port_translation above).
  * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Update vlan_port_translation with the values of:
 *      - vlan_edit_class_id.
 *      - new_outer_vlan.
 *      - new_inner_vlan.
 */
/*
 * Get translation information from a vlan port
 */
shr_error_e
bcm_dnx_vlan_port_translation_get(
    int unit,
    bcm_vlan_port_translation_t * vlan_port_translation)
{
    uint8 is_ingress;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_port_translation_get_verify(unit, vlan_port_translation));
    is_ingress = _SHR_IS_FLAG_SET(vlan_port_translation->flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    /*
     * Handle the Ingress scenario
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_get_ingress(unit, vlan_port_translation));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_port_translation_get_egress(unit, vlan_port_translation));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API allocates an Ingress VLAN Editing(IVE) or Egress
 * VLAN Editing (EVE) Command ID.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] flags -
 *     Flags that are used for controlling the API like WITH_ID
 *     and to determine Ingress or Egress opertaion.
 *   \param [in] action_id -
 *     Pointer to a VLAN Translation Command ID.
 *     \b As \b output - \n
 *     Pointer to an SDK allocated VLAN Translation Command ID.
 *     \b As \b input - \n
 *     Matched with a WITH_ID flag, the pointed value contains a
 *     user supplied VLAN Translation Command ID for allocation.
 * \par INDIRECT INPUT:
 *   * \b *action_id \n
 *     See 'action_id' in DIRECT INPUT above
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: invalid vid.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * SW Allocation of a VLAN Translation Command ID, either
 *     for Ingress or Egress.
 *   * \b *action_id \n
 *     See 'action_id' in DIRECT INPUT above
 */
int
bcm_dnx_vlan_translate_action_id_create(
    int unit,
    uint32 flags,
    int *action_id)
{

    int is_ingress;
    uint32 algo_flags;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_id_create_verify(unit, flags, action_id));
    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    algo_flags = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_WITH_ID) ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    if (is_ingress)
    {
        /*
         * DNX SW Algorithm, allocate ACTION_ID_INGRESS.
         * dnx_algo_res_allocate() will return an error if resource for specified ACTION_ID_INGRESS has already been allocated.
         */
        SHR_IF_ERR_EXIT(vlan_db.action_id_ingress.allocate_single(unit, algo_flags, NULL, action_id));
    }
    else
    {
        /*
         * DNX SW Algorithm, allocate ACTION_ID_EGRESS.
         * dnx_algo_res_allocate() will return an error if resource for specified ACTION_ID_EGRESS has already been allocated.
         */
        SHR_IF_ERR_EXIT(vlan_db.action_id_egress.allocate_single(unit, algo_flags, NULL, action_id));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify action_id and flags parameters for BCM-API:
 *   bcm_dnx_vlan_destroy()
 */
static shr_error_e
dnx_vlan_translate_action_id_destroy_verify(
    int unit,
    uint32 flags,
    int action_id)
{
    int is_ingress, is_egress;

    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Verify flags;
     *    - One of Ingrees/Egrees is set.
     *    - Only one of Ingrees/Egrees is set.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS);
    is_egress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_EGRESS);
    if ((!is_ingress) && (!is_egress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong flags setting. Neither INGRESS nor EGRESS are set!!! flags = 0x%08X\n", flags);
    }

    if ((is_ingress) && (is_egress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS and EGRESS are set!!! flags = 0x%08X\n", flags);
    }

    /*
     * Verify action_id boundry
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translation_action_id_verify(unit, action_id, is_ingress));

    /*
     * Check that the ACTION_ID was allocated:
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_is_allocated_verify(unit, flags, action_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - internal function for destroying a VLAN translation ID instance:
 * DNX SW Algorithm, de-allocate ACTION_ID
 * Clear HW
 * Clear SW State
 */
static shr_error_e
dnx_vlan_translate_action_id_destroy(
    int unit,
    int is_ingress,
    int action_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        /*
         * DNX SW Algorithm, de-allocate ACTION_ID
         */
        SHR_IF_ERR_EXIT(vlan_db.action_id_ingress.free_single(unit, action_id));

        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_clear_ingress_hw(unit, action_id));

        /*
         * Clear the SW State:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_clear_ingress_sw_state(unit, action_id));
    }
    else
    {
        /*
         * DNX SW Algorithm, de-allocate ACTION_ID
         */
        SHR_IF_ERR_EXIT(vlan_db.action_id_egress.free_single(unit, action_id));

        /*
         * Clear the HW:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_clear_egress_hw(unit, action_id));

        /*
         * Clear the SW State:
         */
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_clear_egress_sw_state(unit, action_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy a VLAN translation ID instance
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   Relevant unit
 *   \param [in] flags -
 *   Flags that are used for controlling the API
 *     and to determine Ingress or Egress opertaion.
 *   \param [in] action_id -
 *   Pointer to a VLAN Translation Command ID.
 * \par INDIRECT INPUT:
     None
 * \par DIRECT OUTPUT:
 *    \retval Negative in case of an error. See shr_error_e, for
 *            example: action_id wasn't allocated.
 *    \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   SW de-allocation of a VLAN Translation command ID, either
 *   for Ingress or Egress
 * \remark
     None
 * \see
     None
 */
shr_error_e
bcm_dnx_vlan_translate_action_id_destroy(
    int unit,
    uint32 flags,
    int action_id)
{
    int is_ingress;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_id_destroy_verify(unit, flags, action_id));
    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;

    /*
     * DNX SW Algorithm, de-allocate ACTION_ID
     * Clear HW
     * Clear SW State
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_destroy(unit, is_ingress, action_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify flags parameter for BCM-API:
 *   bcm_dnx_vlan_destroy_all()
 */
static shr_error_e
dnx_vlan_translate_action_id_destroy_all_verify(
    int unit,
    uint32 flags)
{
    int is_ingress, is_egress;
    SHR_FUNC_INIT_VARS(unit);
    /*
     *  Verify flags;
     *    - One of Ingrees/Egrees is set.
     *    - Only one of Ingrees/Egrees is set.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS);
    is_egress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_EGRESS);
    if ((!is_ingress) && (!is_egress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Neither INGRESS nor EGRESS are set!!! flags = 0x%0d\n", flags);
    }

    if ((is_ingress) && (is_egress))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong flags setting. Both INGRESS and EGRESS are set!!! flags = 0x%0d\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy all VLAN translation ID instances
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *   Relevant unit.
 *   \param [in] flags -
 *   Flags that are used for controlling the API
 *     and to determine Ingress or Egress opertaion.
 * \par INDIRECT INPUT:
 *   None
 * \par DIRECT OUTPUT:
 *    \retval Negative in case of an error. See shr_error_e.
 *    \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   SW de-allocation of a VLAN Translation command IDs, either
 *   for Ingress or Egress
 * \remark
     None
 * \see
     None
 */
shr_error_e
bcm_dnx_vlan_translate_action_id_destroy_all(
    int unit,
    uint32 flags)
{
    int is_ingress;
    int action_id, last_action_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_id_destroy_all_verify(unit, flags));

    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;

    /*
     * DNX SW Algorithm, de-allocate all relevant action IDs in Ingress / Egress.
     * Traverse all the relevant entries
     */
    if (is_ingress)
    {
        last_action_id = dnx_data_vlan.vlan_translate.nof_ingress_vlan_edit_action_ids_get(unit) - 1;
    }
    else
    {
        last_action_id = dnx_data_vlan.vlan_translate.nof_egress_vlan_edit_action_ids_get(unit) - 1;
    }

    for (action_id = 0; action_id <= last_action_id; action_id++)
    {
        uint8 is_allocated;

        if (is_ingress == TRUE)
        {
            SHR_IF_ERR_EXIT(vlan_db.action_id_ingress.is_allocated(unit, action_id, &is_allocated));
        }
        else
        {
            SHR_IF_ERR_EXIT(vlan_db.action_id_egress.is_allocated(unit, action_id, &is_allocated));
        }

        if (is_allocated == TRUE)
        {
            /*
             * DNX SW Algorithm, de-allocate ACTION_ID
             * Clear HW
             * Clear SW State
             */
            SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_destroy(unit, is_ingress, action_id));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set a VLAN translation ID instance with tag actions
 * \par DIRECT_INPUT:
 *  \param [in] unit -
 *   Relevant unit.
 *  \param [in] flags -
 *     Flags that are used for controlling the API, to determine
 *     Ingress or Egress opertaion (BCM_VLAN_ACTION_SET_INGRESS
 *     or BCM_VLAN_ACTION_SET_EGRESS).
 *  \param [in] action_id - VLAN translation command ID.
 *  \param [in] action - pointer to bcm_vlan_action_set typedef
 *         structure
 *  \par INDIRECT INPUT:
 *  dt_outer - Outer-tag action. See action in DIRECT_INPUT
 *  above.
 *  dt_inner - Inner-tag action .See action in DIRECT_INPUT
 *  above.
 *  dt_outer_pkt_prio - Outer packet priority action. See action
 *  in DIRECT_INPUT above.
 *  dt_inner_pkt_prio - Inner packet priority action. See action
 *  in DIRECT_INPUT above.
 *  outer_tpid - New outer-tag's tpid field action. See action
 *  in DIRECT_INPUT above.
 *  inner_tpid - New inner-tag's tpid field action. See action
 *  in DIRECT_INPUT above.
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *        example: invalid vid.
 *   \retval Zero in case of NO ERROR
 *\par INDIRECT OUTPUT:
 *  For ingress:
 *      DBAL_TABLE_IVEC_ACTION_ID_SW_INFO. DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND
 *  For egress:
 *      DBAL_TABLE_EVEC_FRWRD_STAGE_ACTION_ID_INFO_SW DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_FWD
 * \remark
 * None.
 *
 * \see
 * bcm_dnx_vlan_translate_action_id_create,
 * bcm_dnx_vlan_translate_action_id_get,
 * bcm_dnx_vlan_translate_action_id_destroy,
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
int
bcm_dnx_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    int is_ingress;
    dnx_vlan_translate_action_dnx_format_t dnx_vlan_translation_command;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_id_set_verify(unit, flags, action_id, action));
    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    /*
     * Convert the VLAN Translation action from BCM API format to DNX HW info
     */
    SHR_IF_ERR_EXIT(dnx_vlan_translate_action_to_dnx_command(unit, is_ingress, action, &dnx_vlan_translation_command));
    /*
     * Set the modified VLAN Translation command to the HW - Ingress or Egress
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_ingress_hw
                        (unit, action_id, action, &dnx_vlan_translation_command));
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_ingress_sw_state(unit, action_id, action));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_egress_hw
                        (unit, action_id, action, &dnx_vlan_translation_command));
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_set_egress_sw_state(unit, action_id, action));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get tag actions from a VLAN translation ID instance
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *    Relevant unit.
 *   \param [in] flags -
 *     Flags that are used for controlling the API, to determine
 *     Ingress or Egress opertaion (BCM_VLAN_ACTION_SET_INGRESS
 *     or BCM_VLAN_ACTION_SET_EGRESS).
 *   \param [in] action_id - VLAN translation command ID.
 *   \param [in] action - pointer to bcm_vlan_action_set typedef
 *        structure
 * \par INDIRECT INPUT:
     *action
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *        example: invalid vid.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *  dt_outer - Outer-tag action. See action in DIRECT_INPUT
 *  above.
 *  dt_inner - Inner-tag action .See action in DIRECT_INPUT
 *  above.
 *  dt_outer_pkt_prio - Outer packet priority action. See action
 *  in DIRECT_INPUT above.
 *  dt_inner_pkt_prio Inner packet priority action. See action
 *  in DIRECT_INPUT above.
 *  outer_tpid - New outer-tag's tpid field action. See action
 *  in DIRECT_INPUT above.
 *  inner_tpid - New inner-tag's tpid field action. See action
 *  in DIRECT_INPUT above.
 * \remark
 *  None
 * \see
 * bcm_dnx_vlan_translate_action_id_create,
 * bcm_dnx_vlan_translate_action_id_set,
 * bcm_dnx_vlan_translate_action_id_destroy,
 * bcm_dnx_vlan_translate_action_id_destroy_all
 */
int
bcm_dnx_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_vlan_action_set_t * action)
{
    uint8 is_ingress;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** init default first*/
    SHR_NULL_CHECK(action, _SHR_E_PARAM, "action");
    bcm_vlan_action_set_t_init(action);

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_id_get_verify(unit, flags, action_id, action));
    is_ingress = _SHR_IS_FLAG_SET(flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    /*
     * Get the modified VLAN Translation command - Ingress or Egress
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_get_ingress(unit, action_id, action));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_id_get_egress(unit, action_id, action));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API sets a VLAN translation mapping. It maps a
 * combination of tag-format and VLAN-Edit-Profile to a
 * Command-ID, either for Ingress or Egress.
 * In the Egress case, the mapping is used by the HW to index
 * into the Forwarding stage EVEC table, but not to the Encap
 * stage EVEC table.
 *
 * \par DIRECT INPUT
 *    \param [in] unit -
 *     Relevant unit.
 *   \param [in] action_class -
 *     A pointer to a structure that holds the mapping
 *     parameters: tag-format, VLAN-Edit-Profile and the result
 *     Command-ID.
 *     It also contains flags to determine Ingress or Egress
 *     operation.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for example: invalid vid.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * HW IVEC-Mapping or EVEC-Mappings are modified with the
 *     supplied mapping.
 */
int
bcm_dnx_vlan_translate_action_class_set(
    int unit,
    bcm_vlan_translate_action_class_t * action_class)
{
    uint8 is_ingress;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_class_set_verify(unit, action_class));

    is_ingress = _SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;
    /*
     * Perform IVEC/EVEC-Mapping configuration according to the selected flag
     */
    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_class_set_ingress(unit, action_class));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_vlan_translate_action_class_set_egress(unit, action_class));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the action ID that is configured for a specified combination of
 * packet format ID and VLAN edit profile
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [in] action_class - A pointer to a structure that holds the mapping
 *     parameters: tag-format, VLAN-Edit-Profile and the result
 *     Command-ID.
 *     It also contains flags to determine Ingress or Egress
 *     operation.
 * \par INDIRECT INPUT:
     None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e.
 *   \retval Zero in case of NO ERROR.
 * \par INDIRECT OUTPUT:
 *     HW IVEC-Mapping or EVEC-Mappings are modified with the
 *     supplied mapping.
 * \remark
     None
 * \see
     None
 */
int
bcm_dnx_vlan_translate_action_class_get(
    int unit,
    bcm_vlan_translate_action_class_t * action_class)
{
    uint32 entry_handle_id;
    uint8 is_ingress;
    int tag_struct_hw_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(action_class, _SHR_E_PARAM, "action_class");

    SHR_INVOKE_VERIFY_DNX(dnx_vlan_translate_action_class_get_verify(unit, action_class));

    is_ingress = _SHR_IS_FLAG_SET(action_class->flags, BCM_VLAN_ACTION_SET_INGRESS) ? TRUE : FALSE;

    /*
     * Perform IVEC/EVEC-Mapping retrieval according to the selected flag
     */
    if (is_ingress)
    {

        /*
         * Get HW tag-structure value:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                        (unit, action_class->tag_format_class_id, &tag_struct_hw_id, NULL));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, tag_struct_hw_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE,
                                   action_class->vlan_edit_class_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE,
                                   (uint32 *) &action_class->vlan_translation_action_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        uint32 remove_tx_tag, vlan_mirror, vlan_membership;

        /*
         * Get HW tag-structure value:
         */
        SHR_IF_ERR_EXIT(dnx_port_tpid_class_sw_id_get_info
                        (unit, action_class->tag_format_class_id, NULL, &tag_struct_hw_id));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VLAN_EDIT_COMMAND_MAPPING, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INCOMING_TAG_STRUCTURE, tag_struct_hw_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_PROFILE,
                                   action_class->vlan_edit_class_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VLAN_EDIT_CMD_INDEX, INST_SINGLE,
                                   (uint32 *) &action_class->vlan_translation_action_id);
        /*
         * Read Post-EVE fields and set flags
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POST_EVE_REMOVE_TX_TAG_ENABLE, INST_SINGLE,
                                   &remove_tx_tag);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MIRROR, INST_SINGLE, &vlan_mirror);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POST_EVE_VLAN_MEMBERSHIP, INST_SINGLE,
                                   &vlan_membership);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Set Post-EVE enablers flags according to enablers values:
         */
        if (remove_tx_tag == 0)
        {
            action_class->flags |= BCM_VLAN_ACTION_SET_EGRESS_DISABLE_OUTER_TAG_REMOVAL;
        }

        if (vlan_mirror == DBAL_ENUM_FVAL_POST_EVE_VLAN_MIRROR_DISABLED)
        {
            action_class->flags |= BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING;
        }
        else if (vlan_mirror == DBAL_ENUM_FVAL_POST_EVE_VLAN_MIRROR_USE_INNER_VLAN)
        {
            action_class->flags |= BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MIRRORING;
        }

        if (vlan_membership == DBAL_ENUM_FVAL_POST_EVE_VLAN_MEMBERSHIP_DISABLED)
        {
            action_class->flags |= BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP;
        }
        else if (vlan_membership == DBAL_ENUM_FVAL_POST_EVE_VLAN_MEMBERSHIP_USE_INNER_VLAN)
        {
            action_class->flags |= BCM_VLAN_ACTION_SET_EGRESS_ENABLE_INNER_MEMBERSHIP;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
