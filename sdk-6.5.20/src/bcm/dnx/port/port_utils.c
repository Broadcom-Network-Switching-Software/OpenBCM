/**
 * \file port_utils.c $Id$ PORT Utility procedures for DNX. 
 *       this file is meant for internal functions, not
 *       dispatched from BCM APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include "port_utils.h"

/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
/*
 * }
 */
/*
 * Include files.
 * {
 */

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */
#define DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS   (BCM_PORT_ATTR_PFM_MASK           |  \
                                                 BCM_PORT_ATTR_PHY_MASTER_MASK    |  \
                                                 BCM_PORT_ATTR_RATE_MCAST_MASK    |  \
                                                 BCM_PORT_ATTR_RATE_BCAST_MASK    |  \
                                                 BCM_PORT_ATTR_RATE_DLFBC_MASK    |  \
                                                 BCM_PORT_ATTR_INTERFACE_MASK     |  \
                                                 BCM_PORT_ATTR_SPEED_MAX_MASK     |  \
                                                 BCM_PORT_ATTR_ABILITY_MASK       |  \
                                                 BCM_PORT_ATTR_MDIX_STATUS_MASK   |  \
                                                 BCM_PORT_ATTR_MEDIUM_MASK        |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK  |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK |  \
                                                 BCM_PORT_ATTR_DISCARD_MASK       |  \
                                                 BCM_PORT_ATTR_UNTAG_PRI_MASK     |  \
                                                 BCM_PORT_ATTR_ENCAP_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_MAC_MASK     |  \
                                                 BCM_PORT_ATTR_MDIX_MASK)

#define DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS     (DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS |  \
                                                 BCM_PORT_ATTR_FRAME_MAX_MASK          |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_FAULT_MASK)

#define DNX_PORT_NIF_ILKN_ELK_UNSUPPORTED_ATTRS (DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS   |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_MASK              |  \
                                                 BCM_PORT_ATTR_STP_STATE_MASK)

#define DNX_PORT_STIF_UNSUPPORTED_ATTRS         (DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_TX_MASK           |  \
                                                 BCM_PORT_ATTR_PAUSE_RX_MASK           |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_FAULT_MASK              |  \
                                                 BCM_PORT_ATTR_LINKSCAN_MASK)

#define DNX_PORT_FLEXE_PHY_UNSUPPORTED_ATTRS    (DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_PAUSE_TX_MASK           |  \
                                                 BCM_PORT_ATTR_PAUSE_RX_MASK           |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_FAULT_MASK)

#define DNX_PORT_NIF_L1_UNSUPPORTED_ATTRS       (DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS |  \
                                                  BCM_PORT_ATTR_LEARN_MASK             |  \
                                                  BCM_PORT_ATTR_VLANFILTER_MASK        |  \
                                                  BCM_PORT_ATTR_UNTAG_VLAN_MASK        |  \
                                                  BCM_PORT_ATTR_PAUSE_TX_MASK          |  \
                                                  BCM_PORT_ATTR_PAUSE_RX_MASK          |  \
                                                  BCM_PORT_ATTR_STP_STATE_MASK)

#define DNX_PORT_NIF_ILKN_L1_UNSUPPORTED_ATTRS  (DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS   |  \
                                                 DNX_PORT_NIF_L1_UNSUPPORTED_ATTRS)

#define DNX_PORT_FABRIC_UNSUPPORTED_ATTRS       (BCM_PORT_ATTR_PAUSE_TX_MASK           |  \
                                                 BCM_PORT_ATTR_PAUSE_RX_MASK           |  \
                                                 BCM_PORT_ATTR_LEARN_MASK              |  \
                                                 BCM_PORT_ATTR_DISCARD_MASK            |  \
                                                 BCM_PORT_ATTR_VLANFILTER_MASK         |  \
                                                 BCM_PORT_ATTR_UNTAG_PRI_MASK          |  \
                                                 BCM_PORT_ATTR_UNTAG_VLAN_MASK         |  \
                                                 BCM_PORT_ATTR_STP_STATE_MASK          |  \
                                                 BCM_PORT_ATTR_INTERFACE_MASK          |  \
                                                 BCM_PORT_ATTR_FRAME_MAX_MASK          |  \
                                                 BCM_PORT_ATTR_AUTONEG_MASK            |  \
                                                 BCM_PORT_ATTR_LOCAL_ADVERT_MASK       |  \
                                                 BCM_PORT_ATTR_REMOTE_ADVERT_MASK      |  \
                                                 BCM_PORT_ATTR_ENCAP_MASK              |  \
                                                 BCM_PORT_ATTR_PFM_MASK                |  \
                                                 BCM_PORT_ATTR_PHY_MASTER_MASK         |  \
                                                 BCM_PORT_ATTR_RATE_MCAST_MASK         |  \
                                                 BCM_PORT_ATTR_RATE_BCAST_MASK         |  \
                                                 BCM_PORT_ATTR_ABILITY_MASK            |  \
                                                 BCM_PORT_ATTR_MDIX_STATUS_MASK        |  \
                                                 BCM_PORT_ATTR_PAUSE_MAC_MASK          |  \
                                                 BCM_PORT_ATTR_RATE_DLFBC_MASK         |  \
                                                 BCM_PORT_ATTR_MEDIUM_MASK)

#define DNX_PORT_UNSUPPORTED_COMMON_ATTRS2       BCM_PORT_ATTR2_PORT_ABILITY

#define DNX_PORT_NO_PP_UNSUPPORTED_ATTRS        (   BCM_PORT_ATTR_LEARN_MASK            | \
                                                    BCM_PORT_ATTR_VLANFILTER_MASK       | \
                                                    BCM_PORT_ATTR_UNTAG_VLAN_MASK       | \
                                                    BCM_PORT_ATTR_STP_STATE_MASK)

#define DNX_PORT_NON_ETHER_UNSUPPORTED_ATTRS     BCM_PORT_ATTR_STP_STATE_MASK
/*
 * }
 */

/**
 * \brief - internal function to the bcm_port_add API. 
 * this function is used to retrieve the added port and call the IMB layer 
 * port_add API to open the port path in the IMB.
 *  
 * see .h file 
 */
int
dnx_port_imb_path_port_add_handle(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get added port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, TRUE))
    {
        /*
         * only call IMB open path for NIF and Fabric
         */
        SHR_IF_ERR_EXIT(imb_port_add(unit, port, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - internal function to the bcm_port_remove API. 
 * this function is used to retrieve the removed port and call 
 * the IMB layer port_remove API to close the port path in the 
 * IMB. 
 * 
 * see .h file
 */
int
dnx_port_imb_path_port_remove_handle(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * get removed port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, TRUE))
    {
        /*
         * only call IMB close path for NIF and Fabric
         */
        SHR_IF_ERR_EXIT(imb_port_remove(unit, port));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get support action mask for port info (bcm_port_info_t)
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] mask - support mask
 * \param [out] mask2 - support mask2
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_port_info_support_action_mask_get(
    int unit,
    bcm_port_t port,
    uint32 *mask,
    uint32 *mask2)
{
    dnx_algo_port_info_s port_info;
    int port_in_lag;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    *mask = BCM_PORT_ATTR_ALL_MASK;

    if (!dnx_drv_is_e_port(unit, port))
    {
        *mask &= ~DNX_PORT_NON_ETHER_UNSUPPORTED_ATTRS;
    }
    /*
     * Currently BCM_PORT_ATTR2_PORT_ABILITY is the only attr2 flag 
     */
    *mask2 = BCM_PORT_ATTR2_PORT_ABILITY;
    /*
     * Get rid of the unspported attr for mask1 and mask2
     */
    *mask2 &= ~DNX_PORT_UNSUPPORTED_COMMON_ATTRS2;

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        *mask &= ~DNX_PORT_FABRIC_UNSUPPORTED_ATTRS;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
    {
        *mask &= ~DNX_PORT_NIF_ILKN_ELK_UNSUPPORTED_ATTRS;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 0, 1))
    {
        if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
        {
            *mask &= ~DNX_PORT_NIF_ILKN_L1_UNSUPPORTED_ATTRS;
        }
        else
        {
            *mask &= ~DNX_PORT_NIF_ILKN_UNSUPPORTED_ATTRS;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_info))
    {
        *mask &= ~DNX_PORT_STIF_UNSUPPORTED_ATTRS;
    }
    else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0, 1))
    {
        if (DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info))
        {
            *mask &= ~DNX_PORT_NIF_L1_UNSUPPORTED_ATTRS;
        }
        else
        {
            *mask &= ~DNX_PORT_NIF_UNSUPPORTED_COMMON_ATTRS;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        *mask &= ~DNX_PORT_FLEXE_PHY_UNSUPPORTED_ATTRS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Port type is not supported.\r\n");
    }

    /*
     * check if port has a valid pp port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));
    if (!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
    {
        *mask &= ~DNX_PORT_NO_PP_UNSUPPORTED_ATTRS;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *
 * see .h file
 */
int
dnx_port_pre_add_validate(
    int unit,
    int port,
    bcm_pbmp_t phys)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_port_pre_add_validate_wrap(unit, port, phys));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
