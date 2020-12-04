/** \file appl_ref_vlan_init.c
 * 
 *
 * VLAN application procedures for DNX.
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
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/switch.h>
#include <bcm/vlan.h>
#include "appl_ref_vlan_init.h"
#include "appl_ref_sys_device.h"
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame (IEEE 802.1Q)*/
#define DNX_TPID_VALUE_C_8100_TAG       (0x8100)
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1Q)*/
#define DNX_TPID_VALUE_S_9100_TAG       (0x9100)
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1ad, a.k.a QinQ)*/
#define DNX_TPID_VALUE_S_88A8_TAG       (0x88A8)
/* Tag Protocol Identifier (TPID) value for VLAN-tagged frame with double tagging (IEEE 802.1ah, a.k.a MAC-IN-MAC)*/
#define DNX_TPID_VALUE_MAC_IN_MAC_TAG   (0x88E7)

/**
 * \brief
 *  Default tag format values.
 *  Note:
 *  The below enumeration values are based on JR1 values.
*/
typedef enum dnx_pp_vlan_tag_format_default_e
{
    /**
     * Frame has no VLAN TAGs
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE = 0,
    /**
     * Frame has C-C-VLAN TAG
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG = 2,
    /**
     * Frame has S-VLAN TAG
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG = 4,
    /**
     * Frame has C-VLAN TAG
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG = 8,
    /**
     * Frame has S-C-VLAN TAGs
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG = 16,
    /**
     * Frame has S-Priority-C-VLAN TAGs (Dtag S+C and OUTER_VID=0)
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG = 24,
    /**
     * Frame has S-S-VLAN TAG
     */
    DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG = 28,

} dnx_pp_vlan_tag_format_default_t;
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
  * Global and Static
  */
/*
 * }
 */

/**
 * \brief -
 * This function initializes global TPID values. The following
 * global TPID values are set:
 *  - 0x8100 (C-TAG)
 *  - 0x9100 (S-TAG)
 *  - 0x88A8 (S-TAG)
 *  - 0x88E8 (Mac-In-Mac)
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 * \see
 * None.
 */
static shr_error_e
appl_dnx_global_tpid_init(
    int unit)
{
    int tpid_index;
    int nof_tpid_default_values;
    bcm_switch_tpid_info_t tpid_info;
    static const uint16 tpid_values_default[] = {
        /*
         * tpid_values:
         */
        DNX_TPID_VALUE_C_8100_TAG,
        DNX_TPID_VALUE_S_9100_TAG,
        DNX_TPID_VALUE_S_88A8_TAG,
        DNX_TPID_VALUE_MAC_IN_MAC_TAG
    };

    SHR_FUNC_INIT_VARS(unit);

    nof_tpid_default_values = sizeof(tpid_values_default) / sizeof(uint16);

    /*
     * Set Global TPID default values:
     */
    SHR_IF_ERR_EXIT(bcm_switch_tpid_delete_all(unit));

    sal_memset(&tpid_info, 0, sizeof(tpid_info));

    for (tpid_index = 0; tpid_index < nof_tpid_default_values; tpid_index++)
    {
        tpid_info.tpid_value = tpid_values_default[tpid_index];

        SHR_IF_ERR_EXIT(bcm_switch_tpid_add(unit, 0, &tpid_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief -
 * This function initializes Ingress LLVP classification table
 * (INGRESS_LLVP_CLASSIFICATION)  with default values:
 *  - untag packet: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE
 *  - c_tag 0x8100: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG
 *  - s_tag 0x9100: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG
 *  - s_tag 0x88A8: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG
 *  - dtag_s_c {0x9100,0x8100}: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG
 *  - dtag_s_c {0x88A8,0x8100}: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG
 *  - dtag_s_c {0x9100,0x8100} with outer priority: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
 *  - dtag_s_c {0x88A8,0x8100} with outer priority: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 *  1. Global TPIDs must be set before setting LLVP!
 *  2. The tag format values are based on JR1 values.
 *  3. In order to optimize and reduce the number of HW writes,
 *      building the LLVP profile is done as follows:
 *      - loop over all the ports and update first LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" to this profile.
 *      - loop over all the ports and update second LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" on this profile.
 *      - loop over all the ports and update third LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" on this profile.
 *     etc...
  * \see
 * appl_dnx_global_tpid_init.
 */
static bcm_error_t
appl_dnx_vlan_llvp_init_ingress(
    int unit)
{
    bcm_port_tpid_class_t tpid_class;
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp_eth;
    uint32 is_ext_vcp_enable;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Take the ETH port mask
     */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_CLEAR(pbmp_eth);
    BCM_PBMP_OR(pbmp_eth, port_config.e);
    BCM_PBMP_REMOVE(pbmp_eth, port_config.sat);
    bcm_port_tpid_class_t_init(&tpid_class);

    is_ext_vcp_enable = dnx_data_vlan.llvp.ext_vcp_en_get(unit);

    if (is_ext_vcp_enable)
    {
        flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    else
    {
        flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    }

    /*
     * Set mapping:
     * Frame has no VLAN TAGs
     */
    tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has C-VLAN TAG
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has C-VLAN TAG with priority (VID=0) - set to UNTAG!
     */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

        BCM_PBMP_ITER(pbmp_eth, logical_port)
        {
            tpid_class.port = logical_port;
            SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
        }
        /*
         * Update Ingress Native AC:
         */
        tpid_class.port = 0;
        tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-VLAN TAG
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-VLAN TAG with priority (VID=0) - set to UNTAG!
     */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

        BCM_PBMP_ITER(pbmp_eth, logical_port)
        {
            tpid_class.port = logical_port;
            SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
        }
        /*
         * Update Ingress Native AC:
         */
        tpid_class.port = 0;
        tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-VLAN TAG
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-VLAN TAG with priority (VID=0) - set to UNTAG!
     */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
        tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

        BCM_PBMP_ITER(pbmp_eth, logical_port)
        {
            tpid_class.port = logical_port;
            SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
        }
        /*
         * Update Ingress Native AC:
         */
        tpid_class.port = 0;
        tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-Priority-C-VLAN TAGs
     */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
        tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

        BCM_PBMP_ITER(pbmp_eth, logical_port)
        {
            tpid_class.port = logical_port;
            SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
        }
        /*
         * Update Ingress Native AC:
         */
        tpid_class.port = 0;
        tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-Priority-C-VLAN TAGs
     */
    if (!is_ext_vcp_enable)
    {
        tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
        tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
        tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
        tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

        BCM_PBMP_ITER(pbmp_eth, logical_port)
        {
            tpid_class.port = logical_port;
            SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
        }
        /*
         * Update Ingress Native AC:
         */
        tpid_class.port = 0;
        tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-C-VLAN TAGs
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-C-VLAN TAGs
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has C-C-VLAN TAG
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-S-VLAN TAG
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_9100_TAG;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Ingress Native AC:
     */
    tpid_class.port = 0;
    tpid_class.flags |= BCM_PORT_TPID_CLASS_NATIVE_IVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-S-VLAN TAG
     */
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_88A8_TAG;

    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief -
 * This function initializes Egress LLVP classification table
 * (EGRESS_LLVP_CLASSIFICATION) with default values:
 *  - untag packet: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE
 *  - c_tag 0x8100: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG
 *  - s_tag 0x9100: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG
 *  - s_tag 0x88A8: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG
 *  - dtag_s_c {0x9100,0x8100}: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG
 *  - dtag_s_c {0x88A8,0x8100}: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG
 *  - dtag_s_c {0x9100,0x8100} with outer priority: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
 *  - dtag_s_c {0x88A8,0x8100} with outer priority: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 *  1. Global TPIDs must be set before setting LLVP!
 *  2. The tag format values are based on JR1 values.
 *  3. In order to optimize and reduce the number of HW writes,
 *      building the LLVP profile is done as follows:
 *      - loop over all the ports and update first LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" to this profile.
 *      - loop over all the ports and update second LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" on this profile.
 *      - loop over all the ports and update third LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" on this profile.
 *     etc...
  * \see
 * appl_dnx_global_tpid_init.
 */
static bcm_error_t
appl_dnx_vlan_llvp_init_egress(
    int unit)
{
    bcm_port_tpid_class_t tpid_class;
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp_pp_eth_candidates;
    bcm_pbmp_t pbmp_pp_eth_egress;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Take all the PP ports
     */
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
    BCM_PBMP_ASSIGN(pbmp_pp_eth_candidates, port_config.all);
    BCM_PBMP_REMOVE(pbmp_pp_eth_candidates, port_config.sfi);
    BCM_PBMP_REMOVE(pbmp_pp_eth_candidates, port_config.sat);

    /*
     * Loop over all the PP Ports candidates and take only the Egress "ETH"
     */
    BCM_PBMP_CLEAR(pbmp_pp_eth_egress);

    key.index = 2;      /* 1 is in, 2 is out */
    key.type = bcmSwitchPortHeaderType;

    BCM_PBMP_ITER(pbmp_pp_eth_candidates, logical_port)
    {
        flags = 0;
        sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
        sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));
        if (flags & APPL_DNX_FILTERED_PORTS_FLAGS)
        {
            continue;
        }
        if ((port_interface_info.interface == BCM_PORT_IF_ERP)
            || (port_interface_info.interface == BCM_PORT_IF_SCH)
            || (port_interface_info.interface == BCM_PORT_IF_RCY_MIRROR)
            || (port_interface_info.interface == BCM_PORT_IF_CRPS))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_get(unit, logical_port, key, &value));

        if (value.value == BCM_SWITCH_PORT_HEADER_TYPE_ETH)
        {
            BCM_PBMP_PORT_ADD(pbmp_pp_eth_egress, logical_port);
        }

    }

    bcm_port_tpid_class_t_init(&tpid_class);
    /*
     * Set mapping:
     * Frame has no VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has C-VLAN TAG
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-VLAN TAG
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-VLAN TAG
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-Priority-C-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-Priority-C-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_IS_PRIO;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-C-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = flags;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has S-C-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }
    /*
     * Update Egress Native AC LLVP Profile with the same above settings:
     */
    tpid_class.port = 0;
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    /*
     * Update Egress IP Tunnel LLVP Profile with the same above settings:
     */
    tpid_class.flags = flags | BCM_PORT_TPID_CLASS_EGRESS_IP_TUNNEL;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));

    /*
     * Set mapping:
     * Frame has C-C-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_C_8100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_C_8100_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-S-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_9100_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_9100_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

    /*
     * Set mapping:
     * Frame has S-S-VLAN TAGs
     */
    flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    tpid_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    tpid_class.tpid1 = DNX_TPID_VALUE_S_88A8_TAG;
    tpid_class.tpid2 = DNX_TPID_VALUE_S_88A8_TAG;

    BCM_PBMP_ITER(pbmp_pp_eth_egress, logical_port)
    {
        /*
         * Update port LLVP Profile with the above settings:
         */
        tpid_class.port = logical_port;
        SHR_IF_ERR_EXIT(bcm_port_tpid_class_set(unit, &tpid_class));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *
 * \brief -
 * This function initializes Ingress and Egress LLVP
 * classification tables (INGRESS_LLVP_CLASSIFICATION,
 * EGRESS_LLVP_CLASSIFICATION) with default values.
 *  - untag packet: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE
 *  - c_tag 0x8100: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG
 *  - s_tag 0x9100: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG
 *  - s_tag 0x88A8: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG
 *  - dtag_s_c {0x9100,0x8100}: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG
 *  - dtag_s_c {0x88A8,0x8100}: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG
 *  - dtag_s_c {0x9100,0x8100} with outer priority: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
 *  - dtag_s_c {0x88A8,0x8100} with outer priority: tag format DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG
 * \param [in] unit - Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 *  1. Global TPIDs must be set before setting LLVP!
 *  2. The tag format values are based on JR1 values.
 *  3. In order to optimize and reduce the number of HW writes,
 *      building the LLVP profile is done as follows:
 *      - loop over all the ports and update first LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" to this profile.
 *      - loop over all the ports and update second LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" on this profile.
 *      - loop over all the ports and update third LLVP mapping, thus only one LLVP HW update and all ports are updates to "points" on this profile.
 *     etc...
  * \see
 * appl_dnx_global_tpid_init.
 */
static bcm_error_t
appl_dnx_vlan_llvp_init(
    int unit)
{
    bcm_port_tag_format_class_t tag_format_class_id;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create Tag-Struct for UNTAG
     * Note: this is doen by SDK Init - no need to create Tag-Struct 0 for untag!
     */

    /*
     * Create Tag-Struct for S-TAG
     */
    tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create
                    (unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeSTag, &tag_format_class_id));

    /*
     * Create Tag-Struct for C-TAG
     */
    tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create
                    (unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeCTag, &tag_format_class_id));

    /*
     * Create Tag-Struct for S_C-TAG
     */
    tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create
                    (unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeSCTag, &tag_format_class_id));

    /*
     * Create Tag-Struct for S_PRIO_C-TAG
     */
    if (dnx_data_vlan.llvp.ext_vcp_en_get(unit))
    {
        flags = BCM_PORT_TPID_CLASS_WITH_ID | BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else
    {
        flags = BCM_PORT_TPID_CLASS_WITH_ID;
    }
    tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create(unit, flags, bcmTagStructTypeSPrioCTag, &tag_format_class_id));

    /*
     * Create Tag-Struct for C_C-TAG
     */
    tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create
                    (unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeCCTag, &tag_format_class_id));

    /*
     * Create Tag-Struct for S_S-TAG
     */
    tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG;
    SHR_IF_ERR_EXIT(bcm_port_tpid_class_create
                    (unit, BCM_PORT_TPID_CLASS_WITH_ID, bcmTagStructTypeSSTag, &tag_format_class_id));

    SHR_IF_ERR_EXIT(appl_dnx_vlan_llvp_init_ingress(unit));

    SHR_IF_ERR_EXIT(appl_dnx_vlan_llvp_init_egress(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Application initialization function.
 * Initialize BCM VLAN Translate module. It configures the
 * reserved IVE & EVE Command IDs, FWD state:
 *  - IVEC 0 - NOP
 *  - EVEC 0 - Add VSI (for untagged)
 *  - EVEC 1 - NOP (for tagged)
 * and performs mapping:
 *  - IVEC Untag - 0 - NOP.
 *  - EVEC Untag - 0 - Add VSI.
 *  - EVEC all others - 1- NOP.
 * And configures the reserved EVE 0 Command ID to add VSI,
 * Egress ENCAP state.
 *
 * \param [in] unit -
 *     Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 *   The function is used on init time
 * \see
 * None.
 */
static bcm_error_t
appl_dnx_vlan_translate_init(
    int unit)
{
    uint32 flags;
    bcm_vlan_action_set_t vlan_action;
    int action_id;
    bcm_vlan_translate_action_class_t action_class;
    int vlan_edit_profile, tag_format_index;
    int tag_format_array[] = { DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE, DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG,
        DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG, DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG,
        DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG,
        DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_PRIORITY_C_TAG, DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG
    };
    int nof_tag_format_created = sizeof(tag_format_array) / sizeof(int);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate IVE & EVE Reserved Command IDs
     */
    flags = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    action_id = 0;
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_id_create(unit, flags, &action_id));
    flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    action_id = 0;
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_id_create(unit, flags, &action_id));
    flags = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    action_id = 1;
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_id_create(unit, flags, &action_id));
    /*
     * Configure the reserved IVE & EVE Command IDs
     * IVEC 0 - NOP
     * EVEC 0 - Add VSI (for untagged)
     * EVEC 1 - NOP (for tagged)
     */
    bcm_vlan_action_set_t_init(&vlan_action);
    vlan_action.dt_outer = bcmVlanActionNone;
    vlan_action.dt_inner = bcmVlanActionNone;
    vlan_action.dt_outer_pkt_prio = bcmVlanActionNone;
    vlan_action.dt_inner_pkt_prio = bcmVlanActionNone;
    vlan_action.outer_tpid = BCM_SWITCH_TPID_VALUE_INVALID;
    vlan_action.inner_tpid = BCM_SWITCH_TPID_VALUE_INVALID;
    /*
     * First set the NOP Command IDs
     */
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, 0, &vlan_action));
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 1, &vlan_action));
    /*
     * Set the EVEC 0 entry with added VSI
     */
    vlan_action.dt_outer = bcmVlanActionMappedAdd;
    vlan_action.dt_inner = bcmVlanActionNone;
    vlan_action.dt_outer_pkt_prio = bcmVlanActionAdd;
    vlan_action.dt_inner_pkt_prio = bcmVlanActionNone;
    vlan_action.outer_tpid = DNX_TPID_VALUE_C_8100_TAG;
    vlan_action.inner_tpid = BCM_SWITCH_TPID_VALUE_INVALID;
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, 0, &vlan_action));
    /*
     * Perform IVEC mapping:
     *  - IVEC Untag - 0 - NOP: since all default values are 0, nothing to do here.
     */

    /*
     * Perform EVEC mapping:
     *  - EVEC Untag - 0 - Add VSI: since all default values are 0, nothing to do here.
     *  - EVEC all others - 1 - NOP: need to map all entries but entry 0 to action ID 1.
     */
    bcm_vlan_translate_action_class_t_init(&action_class);

    /*
     * First, sets all the entries to action ID 1 (NOP):
     * vlan_edit_profile is 6 bits
     * tag_format is 5 bits - take only the created tag_struct
     */
    action_class.vlan_translation_action_id = 1;

    for (vlan_edit_profile = 0; vlan_edit_profile < (1 << 6); vlan_edit_profile++)
    {
        action_class.vlan_edit_class_id = vlan_edit_profile;

        /*
         * take only the created tag_struct
         */
        for (tag_format_index = 0; tag_format_index < nof_tag_format_created; tag_format_index++)
        {

            action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
            action_class.tag_format_class_id = tag_format_array[tag_format_index];

            /*
             * If the packet is untagged packet - disable all POST-EVE proccessing since it is VID based
             */
            if (action_class.tag_format_class_id == DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE)
            {
                action_class.flags |=
                    BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MEMBERSHIP | BCM_VLAN_ACTION_SET_EGRESS_DISABLE_OUTER_TAG_REMOVAL
                    | BCM_VLAN_ACTION_SET_EGRESS_DISABLE_MIRRORING;
            }

            SHR_IF_ERR_EXIT(bcm_vlan_translate_action_class_set(unit, &action_class));
        }
    }

    /*
     * Second, set back entry 0 (UNTAG) to action ID 0 (Add VSI):
     */
    action_class.vlan_translation_action_id = 0;
    action_class.vlan_edit_class_id = 0;
    action_class.tag_format_class_id = DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    SHR_IF_ERR_EXIT(bcm_vlan_translate_action_class_set(unit, &action_class));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Application initialization function.
 * Initialize default VLAN flooding. It configures
 *   - Creation of ingress multicast group 1
 *   - Add all "E" pbmp members to it.
 * \param [in] unit -
 *     Relevant unit.
 * \return
 *   shr_error_e
 *
 * \remark
 *   The function is used on init time
 * \see
 * None.
 */
static bcm_error_t
appl_dnx_vlan_default_flooding_init(
    int unit)
{
    int flags;
    bcm_multicast_t ingress_mcid;
    bcm_multicast_replication_t *rep_array = NULL;
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp_eth;
    int nof_port;
    int port_idx;
    bcm_vlan_t def_vlan_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /*
     * Take the ETH port
     */
    BCM_PBMP_CLEAR(pbmp_eth);
    BCM_PBMP_OR(pbmp_eth, port_config.e);
    BCM_PBMP_REMOVE(pbmp_eth, port_config.rcy_mirror);
    BCM_PBMP_REMOVE(pbmp_eth, port_config.rcy);

#ifdef ADAPTER_SERVER_MODE
    /*
     * Only add Ethernet and CPU ports 
     */
    BCM_PBMP_AND(pbmp_eth, port_config.cpu);
#endif

    /*
     * Create default ingress MC-ID
     */
    SHR_IF_ERR_EXIT(bcm_vlan_default_get(unit, &def_vlan_id));
    ingress_mcid = def_vlan_id;
    flags = (BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP);
    SHR_IF_ERR_EXIT(bcm_multicast_create(unit, flags, &ingress_mcid));

    /*
     * Add eth port to MC
     */
    nof_port = 0;
    BCM_PBMP_ITER(pbmp_eth, logical_port)
    {
        if (BCM_PBMP_MEMBER(port_config.oamp, logical_port))
            continue;
        nof_port++;
    }

    if (nof_port)
    {
        SHR_ALLOC_SET_ZERO(rep_array, sizeof(bcm_multicast_replication_t) * nof_port, "dnx default_flooding_init",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        port_idx = 0;
        BCM_PBMP_ITER(pbmp_eth, logical_port)
        {
            if (BCM_PBMP_MEMBER(port_config.oamp, logical_port))
                continue;
            rep_array[port_idx].port = logical_port;
            port_idx++;
        }

        flags = BCM_MULTICAST_INGRESS_GROUP;
        SHR_IF_ERR_EXIT(bcm_multicast_add(unit, ingress_mcid, flags, nof_port, rep_array));
    }

exit:
    SHR_FREE(rep_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Application initialization function.
 * Initialize the VLAN membership filtering for ingress.
 * \param [in] unit -
 *     Relevant unit.
 * \return
 *   shr_error_e
 * \remark
 *   The function is used on init time
 * \see
 * None.
 */
static bcm_error_t
appl_dnx_vlan_trap_init(
    int unit)
{
    bcm_rx_trap_config_t config;
    int trap_id;
    SHR_FUNC_INIT_VARS(unit);
    /** Initialize trap for vlan membership filtering */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapPortNotVlanMember, &trap_id));
    bcm_rx_trap_config_t_init(&config);
    config.trap_strength = dnx_data_trap.strength.default_trap_strength_get(unit);
    config.dest_port = BCM_GPORT_BLACK_HOLE;
    config.flags |= BCM_RX_TRAP_UPDATE_DEST;
    config.snoop_strength = 0;
    config.snoop_cmnd = 0;
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
appl_dnx_vlan_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_global_tpid_init(unit));

    SHR_IF_ERR_EXIT(appl_dnx_vlan_llvp_init(unit));

    SHR_IF_ERR_EXIT(appl_dnx_vlan_translate_init(unit));

    SHR_IF_ERR_EXIT(appl_dnx_vlan_default_flooding_init(unit));

    SHR_IF_ERR_EXIT(appl_dnx_vlan_trap_init(unit));

exit:
    SHR_FUNC_EXIT;
}
