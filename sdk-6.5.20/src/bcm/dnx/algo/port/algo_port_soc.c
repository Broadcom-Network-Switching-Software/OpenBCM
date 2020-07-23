/** \file algo_port_soc.c
 *
 *  PORT manager, Port utilities, and Port verifiers.
 *
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/chip.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_port_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#include <bcm/types.h>

#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "algo_port_internal.h"

/*
 * }
 */
/*
 * Macros
 * {
 */

/*
 * }
 */

/*
 * Typedefs
 * {
 */
/*
 * Struct for SOC info error recovery
 */
typedef struct dnx_algo_port_er_soc_info_rollback_s
{
    bcm_port_t logical_port;
    int is_master_port;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t phy_ports;
} dnx_algo_port_er_soc_info_rollback_t;

/*
 * }
 */

/*
 * Local functions (documantation in function implementation)
 * {
 */
static shr_error_e dnx_algo_port_soc_info_port_name_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info);

static shr_error_e dnx_algo_port_soc_info_port_restore(
    int unit,
    bcm_port_t logical_port);

static shr_error_e dnx_algo_port_soc_info_port_custom_bitmap_add(
    int unit,
    bcm_port_t logical_port);

static shr_error_e dnx_algo_port_soc_info_port_custom_bitmap_remove(
    int unit,
    bcm_port_t logical_port);

static shr_error_e dnx_algo_port_er_soc_info_port_access_entry_insert(
    int unit,
    int is_add,
    dnx_algo_port_er_soc_info_rollback_t * rollback_entry);
/*
 * }
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_port_bitmaps_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** first, clear logical port from all bitmaps*/
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, XE_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, XL_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, IL_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, CE_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, SFI_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, LE_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, CC_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, CDE_PTYPE, logical_port));
    {
        pbmp_t *cmic_bitmap_p;

        SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &cmic_bitmap_p));
        BCM_PBMP_PORT_REMOVE(*cmic_bitmap_p, logical_port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, RCY_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, TDM_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, PORT_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, ALL_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, FLEXE_CLIENT_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, FLEXE_MAC_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, FLEXE_SAR_PTYPE, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, FLEXE_PHY_PTYPE, logical_port));

    /** set appropriate bitmaps */
    if (port_info.port_type == DNX_ALGO_PORT_TYPE_INVALID)
    {
        /** do nothing */
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, ALL_PTYPE, logical_port));
        if (DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, PORT_PTYPE, logical_port));
        }
        if (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, TDM_PTYPE, logical_port));
        }

        if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, SFI_PTYPE, logical_port));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_info))
        {
            pbmp_t *cmic_bitmap_p;

            SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, PBMP_CMIC_BITMAP, &cmic_bitmap_p));
            BCM_PBMP_PORT_ADD(*cmic_bitmap_p, logical_port);
        }
        else if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, RCY_PTYPE, logical_port));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 1, 1)
                 || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            int port_has_tx_speed;
            int port_has_rx_speed;
            /**
             * set the speed bitmaps only if the port has both tx and rx speed and they are equal.
             * in case of asymmetric port speed - which shouldn't happen in this context - skip setting the bitmaps
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &port_has_tx_speed));
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &port_has_rx_speed));
            if (port_has_tx_speed && port_has_rx_speed)
            {
                int rx_speed;
                int tx_speed;
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
                SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &tx_speed));
                if (rx_speed == tx_speed)
                {
                    int nof_lanes;
                    int speed = rx_speed;
                    /** figure out the port type by number of lanes */
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, logical_port, &nof_lanes));

                    switch (speed)
                    {
                        case 400000:
                            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, CDE_PTYPE, logical_port));
                            break;

                        case 200000:
                            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, CC_PTYPE, logical_port));
                            break;

                        case 100000:
                            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, CE_PTYPE, logical_port));
                            break;

                        case 50000:
                        case 40000:
                            if (nof_lanes == 1)
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, LE_PTYPE, logical_port));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, XL_PTYPE, logical_port));
                            }
                            break;

                        default:
                        /** speed range 10000 - 25000 */
                            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, XE_PTYPE, logical_port));
                    }
                }
            }
            if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, FLEXE_PHY_PTYPE, logical_port));
            }
        }
        else if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, IL_PTYPE, logical_port));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, FLEXE_MAC_PTYPE, logical_port));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, FLEXE_SAR_PTYPE, logical_port));
        }
        else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, FLEXE_CLIENT_PTYPE, logical_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register access per port is used for NIF ports and CPU ports.
 *          The SW configuration located in SOC INFO
 *          The function configure / removes it
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] is_master_port - see function dnx_algo_port_master_get for more information
 *   \param [in] port_info - port type of the added port
 *   \param [in] phy_ports - relevant just for nif ports - physical port bitmaps (one based)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO members which relevant for port access
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_soc_info_port_access_add(
    int unit,
    bcm_port_t logical_port,
    int is_master_port,
    dnx_algo_port_info_s port_info,
    bcm_pbmp_t phy_ports)
{
    int first_phy_port;
    int port_block, port_block_type;
    int lane_num;
    bcm_port_t phy_port, phy_port_i;
    int is_ilkn_over_fabric = 0;
    dnx_algo_port_er_soc_info_rollback_t soc_info_rollback;
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 1, 1, 1, 1))
    {

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        {
            is_ilkn_over_fabric = 0;
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, logical_port, &is_ilkn_over_fabric));
        }

        if (!is_ilkn_over_fabric)
        {
            /** get first phy port */
            _SHR_PBMP_FIRST(phy_ports, first_phy_port);

            /** get physical block */
            port_block = dnx_drv_soc_port_idx_info_blk(unit, first_phy_port, 0);
            if (port_block == -1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d  first_phy_port %d is not supported\n", logical_port,
                             first_phy_port);
            }

            /** add logical_port to block bitmap */
            {
                pbmp_t *block_bitmap_p;

                SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, BLOCK_BITMAP, &block_bitmap_p));
                block_bitmap_p = &block_bitmap_p[port_block];
                /*
                 * add logical_port to block bitmap
                 */
                SOC_PBMP_PORT_ADD(*block_bitmap_p, logical_port);
            }

            /** if master port set, set first port in bitmap as master block port - this is done to keep WB data consistent */
            if (is_master_port)
            {
                int *block_port_p;
                pbmp_t *block_bitmap_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, BLOCK_PORT_INT, &block_port_p));
                SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, BLOCK_BITMAP, &block_bitmap_p));
                block_port_p = &block_port_p[port_block];
                block_bitmap_p = &block_bitmap_p[port_block];
                /** set first port in bitmap as master block port */
                _SHR_PBMP_FIRST(*block_bitmap_p, *block_port_p);
            }
            {
                /*
                 * Set port block type
                 */
                int *soc_block_info_p;

                SHR_IF_ERR_EXIT(dnx_drv_soc_block_info_port_block_address_get
                                (unit, TYPE, port_block, &soc_block_info_p));
                port_block_type = *soc_block_info_p;
            }
            {
                int *port_info_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
                port_info_p = &port_info_p[logical_port];
                *port_info_p = port_block_type;
            }
            {
                /*
                 * set logical-to-physical
                 */
                int *int_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &int_p));
                int_p = &(int_p[logical_port]);
                *int_p = first_phy_port;
            }
            /** set port lane num*/
            BCM_PBMP_COUNT(phy_ports, lane_num);
            {
                int *int_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_NUM_LANES_INT, &int_p));
                int_p = &(int_p[logical_port]);
                *int_p = lane_num;
            }
            /** set physical-to-logical */
            if (is_master_port)
            {
                BCM_PBMP_ITER(phy_ports, phy_port_i)
                {
                    int *int_p;

                    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
                    int_p = &(int_p[phy_port_i]);
                    *int_p = logical_port;
                }
            }
        }
        else
        {
            /*
             * ILKN over fabric case
             */
            _SHR_PBMP_FIRST(phy_ports, first_phy_port);
            {
                int *port_info_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
                port_info_p = &port_info_p[logical_port];
                *port_info_p = SOC_BLK_FSRD;
            }
            /** set logical-to-physical*/
            phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + first_phy_port;
            {
                int *int_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &int_p));
                int_p = &(int_p[logical_port]);
                *int_p = phy_port;
            }
            /** set port lane num*/
            BCM_PBMP_COUNT(phy_ports, lane_num);
            {
                int *int_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_NUM_LANES_INT, &int_p));
                int_p = &(int_p[logical_port]);
                *int_p = lane_num;
            }
            /** set physical-to-logical */
            if (is_master_port)
            {
                BCM_PBMP_ITER(phy_ports, phy_port_i)
                {
                    int *int_p;

                    phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + phy_port_i;
                    SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
                    int_p = &(int_p[phy_port]);
                    *int_p = logical_port;
                }
            }
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        int fabric_link = -1;

        /*
         * Take info from logical_port to fabric_link mapping instead of
         * phy_ports bitmap, because getting the first port in the bitmap
         * is not efficient.
         */

        /*
         * Set logical-to-physical.
         * We give fabric phisical ports an offest, so they will be
         * separated from NIF ports.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &fabric_link));
        phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + fabric_link;
        {
            /** set logical-to-physical*/
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = phy_port;
        }
        {
            /** set physical-to-logical */
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
            int_p = &(int_p[phy_port]);
            *int_p = logical_port;
        }
        {
            /** set port lane num*/
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_NUM_LANES_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = 1;
        }
        {
            int *port_info_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
            port_info_p = &port_info_p[logical_port];
            *port_info_p = SOC_BLK_FSRD;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, TRUE, FALSE))
    {
        int *port_info_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
        port_info_p = &port_info_p[logical_port];
        *port_info_p = SOC_BLK_FEU;
    }
    else
    {
        int *port_info_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
        port_info_p = &port_info_p[logical_port];
        *port_info_p = SOC_BLK_ECI;
    }
    soc_info_rollback.logical_port = logical_port;
    soc_info_rollback.is_master_port = is_master_port;
    soc_info_rollback.port_info = port_info;
    soc_info_rollback.phy_ports = phy_ports;
    SHR_IF_ERR_EXIT(dnx_algo_port_er_soc_info_port_access_entry_insert(unit, 1, &soc_info_rollback));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Register access per port is used for NIF ports and CPU ports.
 *          The SW configuration located in SOC INFO
 *          The function configure / removes it
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] port_info - port type of the removed port
 *   \param [in] new_master_port - logical port of interface master port (after port remove), -1 if no master port
 *   \param [in] phy_ports - relevant just for nif ports - physical port bitmaps (one based)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO members which relevant for port access
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_soc_info_port_access_remove(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    bcm_port_t new_master_port,
    bcm_pbmp_t phy_ports)
{
    int first_phy_port;
    int port_block;
    int *soc_block_info_p;
    bcm_port_t phy_port, phy_port_i;
    int is_ilkn_over_fabric = 0;
    dnx_algo_port_er_soc_info_rollback_t soc_info_rollback;
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 1, 1, 1, 1))
    {

        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        {
            is_ilkn_over_fabric = 0;
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, logical_port, &is_ilkn_over_fabric));
        }

        if (!is_ilkn_over_fabric)
        {
            /** reset physical to logical */
            BCM_PBMP_ITER(phy_ports, phy_port_i)
            {
                int *int_p;

                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
                int_p = &(int_p[phy_port_i]);
                *int_p = new_master_port;
            }

            /** clear to block bitmap */
            _SHR_PBMP_FIRST(phy_ports, first_phy_port);
            port_block = dnx_drv_soc_port_idx_info_blk(unit, first_phy_port, 0);
            {
                pbmp_t *block_bitmap_p;
                bcm_port_t new_neighbor_port;
                int *block_port_p;

                SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, BLOCK_BITMAP, &block_bitmap_p));
                block_bitmap_p = &block_bitmap_p[port_block];
                BCM_PBMP_PORT_REMOVE(*block_bitmap_p, logical_port);

                /** reset block port */
                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, BLOCK_PORT_INT, &block_port_p));
                block_port_p = &block_port_p[port_block];

                /*
                 * Get port block instance number
                 */
                SHR_IF_ERR_EXIT(dnx_drv_soc_block_info_port_block_address_get
                                (unit, NUMBER, port_block, &soc_block_info_p));

                /*
                 * Use another port from the same block, or block instance number if this is the last port in the block
                 */
                _SHR_PBMP_FIRST(*block_bitmap_p, new_neighbor_port);
                *block_port_p =
                    (new_neighbor_port != -1 ? new_neighbor_port : (*soc_block_info_p | SOC_REG_ADDR_INSTANCE_MASK));
            }
        }
        else
        {
            /** reset physical to logical */
            BCM_PBMP_ITER(phy_ports, phy_port_i)
            {
                int *int_p;

                phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + phy_port_i;
                SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
                int_p = &(int_p[phy_port]);
                *int_p = new_master_port;
            }
        }
        {
            /** remove port block type */
            int *port_info_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
            port_info_p = &port_info_p[logical_port];
            *port_info_p = SOC_BLK_NONE;

            /*
             * Was: SOC_INFO(unit).port_info[logical_port] = SOC_BLK_NONE;
             */
        }
        {
            /** clear logical-to-physical */
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = -1;

            /*
             * Was: SOC_INFO(unit).port_l2p_mapping[logical_port] = -1;
             */
        }
        {
            /** Clear port lane num*/
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_NUM_LANES_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = 0;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        int fabric_link = -1;

        /*
         * Take info from logical_port to fabric_link mapping instead of
         * phy_ports bitmap, because getting the first port in the bitmap
         * is not efficient.
         */

        /*
         * Set logical-to-physical.
         * We give fabric phisical ports an offest, so they will be
         * separated from NIF ports.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &fabric_link));
        phy_port = dnx_data_port.general.fabric_phys_offset_get(unit) + fabric_link;
        {
            /** clear logical-to-physical*/
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = -1;
        }
        {
            /** clear physical-to-logical */
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
            int_p = &(int_p[phy_port]);
            *int_p = -1;
        }
        {
            /** clear port lane num*/
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_NUM_LANES_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = 0;
        }
        {
            int *port_info_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
            port_info_p = &port_info_p[logical_port];
            *port_info_p = SOC_BLK_NONE;
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, TRUE, FALSE))
    {
        int *port_info_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
        port_info_p = &port_info_p[logical_port];
        *port_info_p = SOC_BLK_NONE;
    }
    else
    {
        int *port_info_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
        port_info_p = &port_info_p[logical_port];
        *port_info_p = SOC_BLK_NONE;
    }
    soc_info_rollback.logical_port = logical_port;
    soc_info_rollback.is_master_port = (logical_port == new_master_port);
    soc_info_rollback.port_info = port_info;
    soc_info_rollback.phy_ports = phy_ports;
    SHR_IF_ERR_EXIT(dnx_algo_port_er_soc_info_port_access_entry_insert(unit, 0, &soc_info_rollback));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for port access in soc info
 */
static shr_error_e
dnx_algo_port_er_soc_info_port_access_rollback(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *is_add = NULL;
    bcm_port_t master_port = DNX_ALGO_PORT_INVALID;
    dnx_algo_port_er_soc_info_rollback_t *rollback_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    is_add = (int *) metadata;
    rollback_entry = (dnx_algo_port_er_soc_info_rollback_t *) payload;

    if (*is_add)
    {
        if (!rollback_entry->is_master_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, rollback_entry->logical_port, 0, &master_port));
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_access_remove
                        (unit, rollback_entry->logical_port, rollback_entry->port_info, master_port,
                         rollback_entry->phy_ports));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_access_add
                        (unit, rollback_entry->logical_port, rollback_entry->is_master_port, rollback_entry->port_info,
                         rollback_entry->phy_ports));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for port name in soc info
 */
static shr_error_e
dnx_algo_port_er_soc_info_port_name_rollback(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    dnx_algo_port_info_s port_info;
    dnx_algo_port_er_soc_info_rollback_t *rollback_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    rollback_entry = (dnx_algo_port_er_soc_info_rollback_t *) payload;

    if (rollback_entry->port_info.port_type != DNX_ALGO_PORT_TYPE_INVALID)
    {
        dnx_algo_port_info_s port_info_invalid;
        sal_memset(&port_info_invalid, 0, sizeof(dnx_algo_port_info_s));
        port_info_invalid.port_type = DNX_ALGO_PORT_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_name_set(unit, rollback_entry->logical_port, port_info_invalid));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, rollback_entry->logical_port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_name_set(unit, rollback_entry->logical_port, port_info));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Rollback for custom pbmp in soc info
 */
static shr_error_e
dnx_algo_port_er_soc_info_custom_pbmp_rollback(
    int unit,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *is_add = NULL;
    bcm_port_t *logical_port = NULL;
    _shr_pbmp_t *custom_port_pbmp_p;

    SHR_FUNC_INIT_VARS(unit);

    is_add = (int *) metadata;
    logical_port = (bcm_port_t *) payload;

    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, CUSTOM_PORT_PBMP, &custom_port_pbmp_p));

    if (*is_add)
    {
        BCM_PBMP_PORT_REMOVE(*custom_port_pbmp_p, *logical_port);
    }
    else
    {
        BCM_PBMP_PORT_ADD(*custom_port_pbmp_p, *logical_port);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Insert port access info to generic ER database
 */
static shr_error_e
dnx_algo_port_er_soc_info_port_access_entry_insert(
    int unit,
    int is_add,
    dnx_algo_port_er_soc_info_rollback_t * rollback_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new
                    (unit, (uint8 *) &is_add, sizeof(int),
                     (uint8 *) rollback_entry, sizeof(dnx_algo_port_er_soc_info_rollback_t),
                     &dnx_algo_port_er_soc_info_port_access_rollback));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Insert port name info to generic ER database
 */
static shr_error_e
dnx_algo_port_er_soc_info_port_name_entry_insert(
    int unit,
    dnx_algo_port_er_soc_info_rollback_t * rollback_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new
                    (unit, (uint8 *) rollback_entry, sizeof(dnx_algo_port_er_soc_info_rollback_t),
                     (uint8 *) rollback_entry, sizeof(dnx_algo_port_er_soc_info_rollback_t),
                     &dnx_algo_port_er_soc_info_port_name_rollback));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Insert custom pbmp info to generic ER database
 */
static shr_error_e
dnx_algo_port_er_soc_info_custom_pbmp_entry_insert(
    int unit,
    int is_add,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_GENERIC_STATE_JOURNAL_EXIT_IF_OFF(unit);

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_entry_new
                    (unit, (uint8 *) &is_add, sizeof(int),
                     (uint8 *) &logical_port, sizeof(bcm_port_t), &dnx_algo_port_er_soc_info_custom_pbmp_rollback));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_add(
    int unit,
    bcm_port_t logical_port,
    int is_master_port,
    dnx_algo_port_info_s port_info,
    bcm_pbmp_t phy_ports,
    dnx_algo_port_tdm_mode_e tdm_mode)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Add port access configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_access_add(unit, logical_port, is_master_port, port_info, phy_ports));
    /*
     * Add to port bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set(unit, logical_port, port_info, tdm_mode));
    /*
     * Set port name
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_name_set(unit, logical_port, port_info));

    /** update ilkn pbmp for reg access */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_custom_bitmap_add(unit, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_remove(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info,
    bcm_port_t new_master_port,
    bcm_pbmp_t phy_ports)
{
    dnx_algo_port_info_s port_info_invalid;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Remove port access configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_access_remove
                    (unit, logical_port, port_info, new_master_port, phy_ports));

    /*
     * Remove port from soc info bitmaps
     */
    sal_memset(&port_info_invalid, 0, sizeof(dnx_algo_port_info_s));
    port_info_invalid.port_type = DNX_ALGO_PORT_TYPE_INVALID;
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_bitmaps_set
                    (unit, logical_port, port_info_invalid, DNX_ALGO_PORT_TDM_MODE_NONE));

    /*
     * Remove port name
     */
    if (!DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_name_set(unit, logical_port, port_info_invalid));
    }

    /** update ilkn pbmp for reg access */
    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_custom_bitmap_remove(unit, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_init(
    int unit)
{
    bcm_port_t logical_port;
    bcm_port_t phy_port;
    SHR_FUNC_INIT_VARS(unit);

    /** Init physical port info */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++)
    {
        int *int_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_P2L_MAPPING_INT, &int_p));
        int_p = &(int_p[phy_port]);
        *int_p = -1;

        /*
         * WAS: SOC_INFO(unit).port_p2l_mapping[phy_port] = -1;
         */
    }

    /** Init logical port info */
    for (logical_port = 0; logical_port < SOC_MAX_NUM_PORTS; logical_port++)
    {
        {
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_L2P_MAPPING_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = -1;

            /*
             * WAS: SOC_INFO(unit).port_l2p_mapping[logical_port] = -1;
             */
        }
        {
            int *port_info_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_TYPE_INT, &port_info_p));
            port_info_p = &port_info_p[logical_port];
            *port_info_p = SOC_BLK_NONE;

            /*
             * WAS:SOC_INFO(unit).port_info[logical_port] = SOC_BLK_NONE;
             */
        }
        {
            int *int_p;

            SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, PORT_NUM_LANES_INT, &int_p));
            int_p = &(int_p[logical_port]);
            *int_p = 0;
        }
    }
    {
        /** Init fabric logical port base info */
        int *int_p;

        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FABRIC_LOGICAL_PORT_BASE_INT, &int_p));
        *int_p = dnx_data_port.general.fabric_port_base_get(unit);

        /*
         * WAS: SOC_INFO(unit).fabric_logical_port_base = dnx_data_port.general.fabric_port_base_get(unit);
         */
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set port name and alternative port name - used mostly be soc property read.
 * The name set according to port type and port interface type (read from dnx algo port DB)
 * This function will be called after port add and remove to update port name.
 *
 * to remove set port_info to DNX_ALGO_PORT_TYPE_INVALID
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - required logical port.
 *   \param [in] port_info - port type of the added /removed port
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * SOC INFO port name
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_port_soc_info_port_name_set(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_info_s port_info)
{
    char *port_name;
    char *port_name_alter;
    int port_name_alter_valid;
    bcm_port_t port_name_alter_sufix;
    dnx_algo_port_er_soc_info_rollback_t soc_info_rollback;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port name - according to port type
     */
    /** set default to without alternative name */
    port_name_alter = "";
    port_name_alter_valid = 0;
    port_name_alter_sufix = logical_port;

    switch (port_info.port_type)
    {
        case DNX_ALGO_PORT_TYPE_INVALID:
        {
            port_name = "";
            port_name_alter = "";
            port_name_alter_valid = 0;
            break;
        }
        case DNX_ALGO_PORT_TYPE_FABRIC:
        {
            port_name = "sfi";
            port_name_alter = "fabric";
            port_name_alter_valid = 1;
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, (int *) &port_name_alter_sufix));
            break;
        }
        case DNX_ALGO_PORT_TYPE_CPU:
        {
            port_name = "cpu";
            break;
        }
        case DNX_ALGO_PORT_TYPE_OAMP:
        {
            port_name = "oamp";
            break;
        }
        case DNX_ALGO_PORT_TYPE_OLP:
        {
            port_name = "olp";
            break;
        }
        case DNX_ALGO_PORT_TYPE_SAT:
        {
            port_name = "sat";
            break;
        }
        case DNX_ALGO_PORT_TYPE_EVENTOR:
        {
            port_name = "eventor";
            break;
        }
        case DNX_ALGO_PORT_TYPE_ERP:
        {
            port_name = "erp";
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY:
        {
            port_name = "rcy";
            break;
        }
        case DNX_ALGO_PORT_TYPE_RCY_MIRROR:
        {
            port_name = "rcy_mirror";
            break;
        }
        case DNX_ALGO_PORT_TYPE_NIF_ILKN:
            port_name = "il";
            break;
        case DNX_ALGO_PORT_TYPE_NIF_ETH:
            port_name = "eth";
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_PHY:
            port_name = "eth";
            port_name_alter = "flexe_phy";
            port_name_alter_valid = 1;
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_MAC:
            port_name = "flexe_mac";
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_SAR:
            port_name = "flexe_sar";
            break;
        case DNX_ALGO_PORT_TYPE_FLEXE_CLIENT:
            port_name = "flexe_client";
            break;

        case DNX_ALGO_PORT_TYPE_SCH_ONLY:
        {
            port_name = "sch";
            break;
        }
        case DNX_ALGO_PORT_TYPE_CRPS:
        {
            port_name = "crps";
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported port type for port %d (%d).\n", logical_port,
                         port_info.port_type);
            break;
        }
    }
    {
        /*
         * Set port name
         */
        /** Set port name in SOC CONTROL*/
        {
            char *port_name_p;
            int port_name_size;

            SHR_IF_ERR_EXIT(soc_info_char_2dimensions_address_size_get
                            (unit, PORT_NAME, logical_port, &port_name_p, &port_name_size));

            /*** When port is removed, its type is changed to invalid. At this point port_name needs to be set to an empty string.*/
            if (port_info.port_type == DNX_ALGO_PORT_TYPE_INVALID)
            {
                sal_snprintf(port_name_p, port_name_size, "%s", port_name);
            }
            else
            {
                sal_snprintf(port_name_p, port_name_size, "%s%d", port_name, logical_port);
            }

            /*
             * WAS: sal_snprintf(SOC_INFO(unit).port_name[logical_port],
             * sizeof(SOC_INFO(unit).port_name[logical_port]), "%s%d", port_name, logical_port);
             */
        }
        {
            char *port_name_p;
            int port_name_size;

            SHR_IF_ERR_EXIT(soc_info_char_2dimensions_address_size_get
                            (unit, PORT_NAME_ALTER, logical_port, &port_name_p, &port_name_size));

            /*** When port is removed, its type is changed to invalid. At this point port_name_alter needs to be set to an empty string.*/
            if (port_info.port_type == DNX_ALGO_PORT_TYPE_INVALID)
            {
                sal_snprintf(port_name_p, port_name_size, "%s", port_name_alter);
            }
            else
            {
                sal_snprintf(port_name_p, port_name_size, "%s%d", port_name_alter, port_name_alter_sufix);
            }

            /*
             * WAS: sal_snprintf(SOC_INFO(unit).port_name_alter[logical_port],
             * sizeof(SOC_INFO(unit).port_name_alter[logical_port]), "%s%d", port_name_alter, port_name_alter_sufix);
             */
        }
        {
            uint8 *port_name_alter_valid_p;

            SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, PORT_NAME_ALTER_VALID, &port_name_alter_valid_p));
            port_name_alter_valid_p = &(port_name_alter_valid_p[logical_port]);
            *port_name_alter_valid_p = port_name_alter_valid;

            /*
             * WAS: SOC_INFO(unit).port_name_alter_valid[logical_port] = port_name_alter_valid;
             */
        }
    }
    soc_info_rollback.logical_port = logical_port;
    soc_info_rollback.port_info = port_info;
    SHR_IF_ERR_EXIT(dnx_algo_port_er_soc_info_port_name_entry_insert(unit, &soc_info_rollback));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Add custom bitmap in soc info
 */
static shr_error_e
dnx_algo_port_soc_info_port_custom_bitmap_add(
    int unit,
    bcm_port_t logical_port)
{
    _shr_pbmp_t *custom_port_pbmp_p;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, CUSTOM_PORT_PBMP, &custom_port_pbmp_p));
    BCM_PBMP_PORT_ADD(*custom_port_pbmp_p, logical_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_er_soc_info_custom_pbmp_entry_insert(unit, 1, logical_port));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Remove custom bitmap in soc info
 */
static shr_error_e
dnx_algo_port_soc_info_port_custom_bitmap_remove(
    int unit,
    bcm_port_t logical_port)
{
    _shr_pbmp_t *custom_port_pbmp_p;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_info_pbmp_address_get(unit, CUSTOM_PORT_PBMP, &custom_port_pbmp_p));
    BCM_PBMP_PORT_REMOVE(*custom_port_pbmp_p, logical_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_er_soc_info_custom_pbmp_entry_insert(unit, 0, logical_port));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_port_eth_bitmap_set(
    int unit,
    bcm_port_t local_port,
    dnx_algo_port_header_mode_e header_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (header_mode)
    {
        case DNX_ALGO_PORT_HEADER_MODE_ETH:
        case DNX_ALGO_PORT_HEADER_MODE_MPLS_RAW:
        case DNX_ALGO_PORT_HEADER_MODE_INJECTED_2_PP:
        {

            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, ETHER_PTYPE, local_port));
            break;
        }
        case DNX_ALGO_PORT_HEADER_MODE_NON_PP:
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, ETHER_PTYPE, local_port));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Header_mode %d isn't supported (local_port %d)\n", header_mode, local_port);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Restore information stored in soc info per logical port.
 * This API should called only upon warm reboot
 */
static shr_error_e
dnx_algo_port_soc_info_port_restore(
    int unit,
    bcm_port_t logical_port)
{
    bcm_pbmp_t phy_ports;
    dnx_algo_port_info_s port_info;
    int is_master_port;
    dnx_algo_port_tdm_mode_e tdm_mode;
    dnx_algo_port_header_mode_e header_mode;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int is_lag = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Collect data to restore soc info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, logical_port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
    BCM_PBMP_CLEAR(phy_ports);
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phy_ports));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        BCM_PBMP_CLEAR(phy_ports);
        /** Set logical_port in bitmap, even though we currently don't use it */
        BCM_PBMP_PORT_ADD(phy_ports, logical_port);
    }
    /** Until TDM mode will be set assume the port is not TDM */
    tdm_mode = DNX_ALGO_PORT_TDM_MODE_NONE;
    if (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, logical_port, &tdm_mode));
    }

    /*
     * Config SoC Info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_add(unit, logical_port, is_master_port, port_info, phy_ports, tdm_mode));

    /*
     * Restoring Headers and Ethernet bitmap
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, logical_port, &is_lag));
    if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, is_lag))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, logical_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

        SHR_IF_ERR_EXIT(dnx_algo_port_db.pp.header_mode.get(unit, gport_info.internal_port_pp_info.core_id[0],
                                                            gport_info.internal_port_pp_info.pp_port[0], &header_mode));

        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_eth_bitmap_set(unit, logical_port, header_mode));
    }
    else if (is_lag)
    {
        bcm_trunk_t trunk_id;
        uint32 in_header_type;
        uint32 dummy_out_header_type;

        /** get the trunk in which the port is found */
        SHR_IF_ERR_EXIT(dnx_algo_port_lag_id_get(unit, logical_port, &trunk_id));

        /** get the trunk IN Header type. the out Header type is not relevant here */
        SHR_IF_ERR_EXIT(dnx_trunk_header_types_get(unit, trunk_id, &in_header_type, &dummy_out_header_type));

        /** Translate BCM Header type to DNX Header mode */
        SHR_IF_ERR_EXIT(dnx_switch_bcm_to_dnx_header_type_convert(unit, in_header_type, &header_mode));

        /** set port Eth bitmap for logical port (that is member in a trunk) */
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_eth_bitmap_set(unit, logical_port, header_mode));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_restore(
    int unit)
{
    bcm_pbmp_t valid_logical_ports;
    bcm_port_t logical_port;

    uint32 fabric_port_base = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Warm boot restore sequence.
     * Sync soc_control port related data with the relevant data in sw state
     */
    /** Init SOC INFO */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_init(unit));

    /** restore each valid port soc info */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_VALID, 0, &valid_logical_ports));
    BCM_PBMP_ITER(valid_logical_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_restore(unit, logical_port));
    }

    /*
     * Get the relevant dnx data.
     */
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);

    /*
     * Special handling!
     * Set port name -
     * Required to restore sfi and fabric port names.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_fabric_default_port_names_set(unit, fabric_port_base));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_ref_clk_get(
    int unit,
    bcm_port_t logical_port,
    int *ref_clk)
{
    int pll_index = -1;
    dnx_algo_port_info_s port_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        int link_id;
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));

        pll_index = link_id / (dnx_data_fabric.links.nof_links_get(unit) / 2);
        *ref_clk = dnx_data_pll.general.fabric_pll_get(unit, pll_index)->out_freq;
        if (*ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
        {
            *ref_clk = dnx_data_pll.general.fabric_pll_get(unit, pll_index)->in_freq;
        }
    }
    else
    {
        dnx_algo_port_ethu_access_info_t cdu_info;
        SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, logical_port, &cdu_info));

        pll_index = cdu_info.ethu_id >= dnx_data_nif.eth.cdu_nof_per_core_get(unit) ? 1 : 0;
        *ref_clk = dnx_data_pll.general.nif_pll_get(unit, pll_index)->out_freq;
        if (*ref_clk == DNX_SERDES_REF_CLOCK_BYPASS)
        {
            *ref_clk = dnx_data_pll.general.nif_pll_get(unit, pll_index)->in_freq;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_port_soc_info_fabric_default_port_names_set(
    int unit,
    uint32 fabric_port_base)
{
    bcm_port_t fabric_port;

    SHR_FUNC_INIT_VARS(unit);

    for (fabric_port = fabric_port_base; fabric_port < fabric_port_base + dnx_data_fabric.links.nof_links_get(unit);
         fabric_port++)
    {
        {
            char *port_name_p;
            int port_name_size;

            SHR_IF_ERR_EXIT(soc_info_char_2dimensions_address_size_get
                            (unit, PORT_NAME, fabric_port, &port_name_p, &port_name_size));
            sal_snprintf(port_name_p, port_name_size, "sfi%d", fabric_port);

            /*
             * WAS: sal_snprintf(SOC_INFO(unit).port_name[fabric_port], sizeof(SOC_INFO(unit).port_name[fabric_port]),
             * "sfi%d", fabric_port);
             */
        }
        {
            char *port_name_p;
            int port_name_size;

            SHR_IF_ERR_EXIT(soc_info_char_2dimensions_address_size_get
                            (unit, PORT_NAME_ALTER, fabric_port, &port_name_p, &port_name_size));
            sal_snprintf(port_name_p, port_name_size, "fabric%d", fabric_port - fabric_port_base);

            /*
             * WAS: sal_snprintf(SOC_INFO(unit).port_name_alter[fabric_port],
             * sizeof(SOC_INFO(unit).port_name_alter[fabric_port]), "fabric%d", fabric_port - fabric_port_base);
             */
        }
        {
            uint8 *port_name_alter_valid_p;

            SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, PORT_NAME_ALTER_VALID, &port_name_alter_valid_p));
            port_name_alter_valid_p = &(port_name_alter_valid_p[fabric_port]);
            *port_name_alter_valid_p = 1;

            /*
             * WAS: SOC_INFO(unit).port_name_alter_valid[fabric_port] = 1;
             */
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
