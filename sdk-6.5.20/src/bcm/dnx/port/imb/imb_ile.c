/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include "imb_ile_internal.h"
#include "imb_utils.h"
/** include CDU logic for ILKN FEC CDU power down */
#include "imb_cdu_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_NIF_QUEUE_LEVEL_FLOW_CTRL_NOF_CHANNELS  (16)

#define DNX_PORT_ADDR_IS_PAIRS_NATUNAL_MATCH(addr1, addr2)          (((addr2) == (addr1) + 1) && ((addr1) % 2 == 0))

#define DNX_PORT_ADDR_IS_PAIRS_MATCH(addr1, addr2)         (DNX_PORT_ADDR_IS_PAIRS_NATUNAL_MATCH(addr1, addr2) || \
                                                                DNX_PORT_ADDR_IS_PAIRS_NATUNAL_MATCH(addr2, addr1))

static int imb_ile_port_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

static int imb_ile_port_fec_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down);

/**
 * \brief - initialize the ILE required information.
 * allso create a new PM for the ILE unit
 *
 * \param [in] unit - chip unit ID
 * \param [in] imb_info - IMB info required for
 *        initalization.
 * \param [in] imb_specific_info - specific info required to
 *        init the ILE
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    portmod_pm_create_info_t pm_info;
    int pm;
    const dnx_data_nif_ilkn_ilkn_cores_t *ilkn_core_info;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get core info
     */
    ilkn_core_info = dnx_data_nif.ilkn.ilkn_cores_get(unit, imb_info->inst_id);

    SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));
    pm_info.type = portmodDispatchTypePmOsILKN_50G;
    /*
     * In ilkn interface the aggregated PM are added dynamically at port add stage
     */
    pm_info.pm_specific_info.os_ilkn.nof_aggregated_pms = 0;
    pm_info.pm_specific_info.os_ilkn.fec_disable_by_bypass = ilkn_core_info->fec_disable_by_bypass;
    BCM_PBMP_CLEAR(pm_info.phys);

    for (pm = 0; pm < dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit); ++pm)
    {
        pm_info.pm_specific_info.os_ilkn.wm_high[pm] = dnx_data_nif.ilkn.watermark_high_elk_get(unit);
        pm_info.pm_specific_info.os_ilkn.wm_low[pm] = dnx_data_nif.ilkn.watermark_low_elk_get(unit);
    }
    pm_info.pm_specific_info.os_ilkn.core_clock_khz = dnx_data_device.general.core_clock_khz_get(unit);
    /*
     * ILKN core lane map callbacks
     */
    pm_info.pm_specific_info.os_ilkn.ilkn_port_lane_map_get = imb_ile_port_ilkn_port_lane_map_get;
    pm_info.pm_specific_info.os_ilkn.ilkn_pm_lane_map_get = imb_ile_port_ilkn_pm_lane_map_get;
    pm_info.pm_specific_info.os_ilkn.ilkn_port_fec_units_set = imb_ile_port_ilkn_fec_units_set;

    pm_info.pm_specific_info.os_ilkn.ilkn_block_index = imb_info->inst_id;

    SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

    if (!sw_state_is_warm_boot(unit))
    {
        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ile_power_down_set(unit, imb_info->inst_id, TRUE));
        }

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - De-init the ILE.
 *
 * \param [in] unit - chip unit ID
 * \param [in] imb_info - IMB informatin required for de-init
 * \param [in] imb_specific_info - specific ILE information to
 *        de-init.
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Nothing to do
     */
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable FSRD blocks for fabric links
 *
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] enable - enable state. 1:enable, 0:disable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ile_port_fsrd_enable(
    int unit,
    bcm_port_t port,
    int enable)
{
    int fsrd_block = 0;
    int nof_fsrd = 0;
    int nof_links_in_fsrd = 0;
    int link = 0;
    bcm_port_t logical_port;
    int ilkn_phys_counter = 0;
    int fabric_ports_counter = 0;
    bcm_pbmp_t mask_fsrd_phy_bmp;
    bcm_pbmp_t mask_fsrd_fabric_bmp;
    bcm_pbmp_t phys, fabric_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));

    nof_fsrd = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    nof_links_in_fsrd = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_pbmp));

    for (fsrd_block = 0; fsrd_block < nof_fsrd; fsrd_block++)
    {
        BCM_PBMP_CLEAR(mask_fsrd_phy_bmp);
        BCM_PBMP_CLEAR(mask_fsrd_fabric_bmp);

        for (link = fsrd_block * nof_links_in_fsrd; link < fsrd_block * nof_links_in_fsrd + nof_links_in_fsrd; link++)
        {
            /*
             * Set FSRD phy bitmap
             */
            BCM_PBMP_PORT_ADD(mask_fsrd_phy_bmp, link);

            /*
             * Set FSRD fabric ports bitmap
             */
            logical_port = link + dnx_data_port.general.fabric_port_base_get(unit);
            BCM_PBMP_PORT_ADD(mask_fsrd_fabric_bmp, logical_port);
        }

        /*
         * Count number ILKN lanes on FSRD
         */
        BCM_PBMP_AND(mask_fsrd_phy_bmp, phys);
        BCM_PBMP_COUNT(mask_fsrd_phy_bmp, ilkn_phys_counter);

        /*
         * Count number of active fabric ports on FSRD
         */

        BCM_PBMP_AND(mask_fsrd_fabric_bmp, fabric_pbmp);
        BCM_PBMP_COUNT(mask_fsrd_fabric_bmp, fabric_ports_counter);

        /*
         * If no fabric ports on FSRD and ILKN lanes defined on FSRD --> change FSRD enable status
         */
        if ((fabric_ports_counter == 0) && (ilkn_phys_counter > 0))
        {
            SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_set(unit, fsrd_block, enable));
            SHR_IF_ERR_EXIT(mib_stat_fabric_fsrd_init(unit, fsrd_block));
        }
    }

    SHR_IF_ERR_EXIT(dnx_fabric_if_brdc_fsrd_blk_id_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable RX of ILE port
 *
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] enable - enable state. 1:enable, 0:disable
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_rx_data_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int nof_segments = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_get(unit, port, &nof_segments));
    nof_segments = enable ? nof_segments : 0;

    SHR_IF_ERR_EXIT(imb_ile_port_segments_enable_set(unit, port, nof_segments));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initalize the add_info structure before calling
 *        Portmod API
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] add_info - add_info information to init
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_portmod_add_info_config(
    int unit,
    bcm_port_t port,
    portmod_port_add_info_t * add_info)
{
    int ilkn_id;
    bcm_core_t core;
    bcm_pbmp_t phys;
    int phy;
    int portmod_phy_offset = 0;
    int ilkn_port_is_over_fabric;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /*
     * initalize the structure to invalid values
     */
    SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, add_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    add_info->init_config.lane_map_overwrite = 0;
    add_info->init_config.polarity_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.fw_load_method_overwrite = 0; /** same as in pm create, we don't need to overwrite*/
    add_info->init_config.ref_clk_overwrite = 0; /** same as in pm create, we don't need to overwrite*/

    PORTMOD_PORT_ADD_F_SKIP_SPEED_INIT_SET(add_info);
    PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_SET(add_info);
    PORTMOD_PORT_ADD_F_RX_SRIP_CRC_SET(add_info);

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
    {
        PORTMOD_PORT_ADD_F_ELK_SET(add_info);
    }

    if (dnx_data_port.static_add.nif_fw_crc_check_get(unit))
    {
        PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_CLR(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_BYPASS_FW_CRC_CHECK_SET(add_info);
    }

    if (dnx_data_port.static_add.nif_fw_load_verify_get(unit))
    {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(add_info);
    }
    else
    {
        PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(add_info);
    }

    /*
     * Interface config
     */
    add_info->interface_config.interface = SOC_PORT_IF_ILKN;
    add_info->interface_config.speed = 10312;
    add_info->interface_config.max_speed = 28125;
    add_info->interface_config.flags |= PHYMOD_INTF_F_DONT_TURN_OFF_PLL;

    /*
     * Get ILKN id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * ilkn_core_id is the ilkn port index within the ILKN core.
     */
    add_info->ilkn_core_id = ilkn_id % dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);;

    /*
     * Set over fabric indication
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &ilkn_port_is_over_fabric));
    add_info->ilkn_port_is_over_fabric = ilkn_port_is_over_fabric;

    if (add_info->ilkn_port_is_over_fabric)
    {
        /** Get phy offset for ilkn over fabric */
        portmod_phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
    }

    /** get port phys */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_ITER(phys, phy)
    {
        BCM_PBMP_PORT_ADD(add_info->phys, phy + portmod_phy_offset);
    }

    /** get ilkn lanes */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_lanes_get(unit, port, (bcm_pbmp_t *) & (add_info->ilkn_lanes)));

    /*
     * Set Burst configurations
     */
    add_info->ilkn_burst_max = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->burst_max;
    add_info->ilkn_burst_min = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->burst_min;
    add_info->ilkn_burst_short = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->burst_short;

    /*
     * Set nof segments
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_nof_segments_default_get(unit, port, &add_info->ilkn_nof_segments));

    /*
     * Set Metaframe period
     */
    add_info->ilkn_metaframe_period = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->metaframe_period;

    /*
     * No retransmit in Jr2
     */
    add_info->rx_retransmit = 0;
    add_info->tx_retransmit = 0;

    /*
     * Inband FC calender length
     */
    add_info->ilkn_inb_cal_len_tx =
        ((dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) >
          0) ? dnx_data_fc.inband.calender_length_get(unit, ilkn_id)->tx : 0);
    add_info->ilkn_inb_cal_len_rx =
        ((dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) >
          0) ? dnx_data_fc.inband.calender_length_get(unit, ilkn_id)->rx : 0);

    /*
     * Aggregated PMs
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_core_aggregated_pms_get
                    (unit, port, ilkn_port_is_over_fabric, &(add_info->nof_aggregated_pms), add_info->controlled_pms));

    /*
     * Set Bypass flag
     */
    PORTMOD_PORT_ADD_F_PCS_BYPASSED_SET(add_info);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - control enable/ disable ILE port credits - credits from SerDes Tx to ILKN core.
 *          this function is relevant only for ILKN over fabric
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable - enable / disable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_ile_port_credits_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(portmod_port_credits_enable_set(unit, port, enable));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - configure a new ILE port. includes calling Portmod
 *        API
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    portmod_port_add_info_t add_info;
    int ilkn_id, is_over_fabric, nof_segments, is_ilkn_elk = 0, is_master_port, master_port;
    dnx_algo_port_info_s port_info;
    portmod_pbmp_t ilkn_phys_tmp;
    dnx_algo_port_ilkn_access_info_t ilkn_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get wheather this port is master port or channelized
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_to_ilkn_master_get(unit, port, &master_port));
    is_master_port = (port == master_port);

    /*
     * Get ILKN id
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    /*
     * Get port type
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (is_master_port)
    {
        /*
         * ILKN core configuration
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_info));

        if (dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power up the unit */
            SHR_IF_ERR_EXIT(imb_ile_port_power_down_set(unit, port, FALSE));
        }

        if (ilkn_info.pm_imb_type == imbDispatchTypeImb_cdu &&
            dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down))
        {
            /** Power up ILKN logic in CDU */
            SHR_IF_ERR_EXIT(imb_ile_port_fec_power_down_set(unit, port, FALSE));
        }

        /*
         * Port main OOO
         */
        SHR_IF_ERR_EXIT(imb_ile_port_reset_set(unit, port, 1, IMB_COMMON_OUT_OF_RESET));

        /*
         * Nof Segment calculation
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_imb_nof_segments_default_get(unit, port, &nof_segments));
        SHR_IF_ERR_EXIT(imb_ile_port_nof_segments_set(unit, port, nof_segments));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_nof_segments_set(unit, port, nof_segments));

        /*
         * Disable RX data to avoid bad signals during alignment process
         */
        SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 0));

        /*
         * Set ILKN over Fabric indication
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_over_fabric_per_core_selector))
        {
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_over_fabric_set(unit, port, is_over_fabric));
        }

        /*
         * If ILKN credit selector is supported: select the credit source
         */
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_credits_selector))
        {
            /*
             * Set ilkn credit source (CDU or CLU)
             */
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_credits_source_set(unit, port));
        }

        /*
         * Set ELK/Data MUX
         */
        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_ilu_supported))
        {
            is_ilkn_elk = (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info)) ? TRUE : FALSE;
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_elk_data_mux_set(unit, port, is_ilkn_elk));
        }

        /*
         * Set ETH units (CDU/CLU) ilkn Data MUX or ILE fabric ELK MUX
         */
        if ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_cdu_ilkn_selectors))
            || (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_selectors))
            || (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_12lanes_mode)))
        {
            SHR_IF_ERR_EXIT(imb_ile_port_mux_selectors_set(unit, port));
        }

        /*
         * Enable ELK
         */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info))
        {
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_elk_set(unit, port, 1));
        }

        if (is_over_fabric)
        {
            /*
             *  Enable fabric SerDes for ILKN over fabric
             */
            SHR_IF_ERR_EXIT(imb_ile_port_fsrd_enable(unit, port, 1));
        }

        /*
         * FC should not be used when connecting to KBP and for any ELK use case.
         */
        if (dnx_data_fc.inband.nof_inband_intlkn_cal_instances_get(unit) > 0)
        {
            SHR_IF_ERR_EXIT(imb_ile_port_rx_llfc_enable_set(unit, port, 0));
        }
    }

    /*
     * get port properties before calling Portmod API
     */
    SHR_IF_ERR_EXIT(imb_portmod_add_info_config(unit, port, &add_info));

    if (is_master_port)
    {
        BCM_PBMP_ASSIGN(ilkn_phys_tmp, add_info.phys);
        SHR_IF_ERR_EXIT(portmod_ilkn_block_phys_set(unit, ilkn_info.ilkn_core, ilkn_phys_tmp, 1));
    }

    SHR_IF_ERR_EXIT(imb_er_portmod_port_add(unit, port, &add_info));

    if (is_master_port)
    {
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);

        /*
         * Enable credits from SerDes Tx to ILKN core (ILKN over fabric only), not expected to be disabled on detach
         */
        SHR_IF_ERR_EXIT(imb_ile_port_credits_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure a new ILE port. includes calling Portmod
 *        API
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_detach(
    int unit,
    bcm_port_t port)
{
    int is_over_fabric, nof_channels, ilkn_nof_ports, is_core_reset, phy;
    bcm_pbmp_t port_phys;
    portmod_pbmp_t ilkn_phys_tmp;
    dnx_algo_port_ilkn_access_info_t ilkn_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_info));
    /*
     * get number of channels
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
    /*
     * Get the relevant ports in this ilkn core
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_nof_ports_get(unit, port, &ilkn_nof_ports));

    /*
     * Last channel in port
     */
    if (nof_channels == 1)
    {
        /*
         * 1. disable port in portmod
         */
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, 0));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);

        /*
         * 2. Remove the PM mapping if port is master
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &port_phys));
        BCM_PBMP_CLEAR(ilkn_phys_tmp);
        BCM_PBMP_ITER(port_phys, phy)
        {
            BCM_PBMP_PORT_ADD(ilkn_phys_tmp, phy);
        }
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        SHR_IF_ERR_EXIT(portmod_ilkn_block_phys_set(unit, ilkn_info.ilkn_core, ilkn_phys_tmp, 0));
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
    }

    /*
     * 3. Remove port in portmod
     */
    SHR_IF_ERR_EXIT(imb_er_portmod_port_remove(unit, port));

    /*
     * Last channel in port
     */
    if (nof_channels == 1)
    {
        /*
         * Set ILKN over Fabric indication
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
        if (is_over_fabric)
        {
            /*
             *  4. Disable fabric SerDes for ILKN over fabric
             */
            SHR_IF_ERR_EXIT(imb_ile_port_fsrd_enable(unit, port, 0));
        }

        /*
         * 5. Reset CLU/CDU ilkn Data MUX
         */
        if ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_cdu_ilkn_selectors))
            || (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_clu_ilkn_selectors)))
        {
            SHR_IF_ERR_EXIT(imb_ile_port_mux_selectors_reset(unit, port, ilkn_info.pm_imb_type, ilkn_nof_ports));
        }
        /*
         * 6. Port main reset
         */
        is_core_reset = (ilkn_nof_ports == 1) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_ile_port_reset_set(unit, port, is_core_reset, IMB_COMMON_IN_RESET));

        if ((ilkn_nof_ports == 1) && dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_clock_power_down))
        {
            /** Power down the unit */
            SHR_IF_ERR_EXIT(imb_ile_port_power_down_set(unit, port, TRUE));
        }

        if (ilkn_info.pm_imb_type == imbDispatchTypeImb_cdu &&
            dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down))
        {
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_fec_units_set
                            (unit, port, TRUE, 0,
                             dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_fec_bypass_supported), 0));

            /** Power down FEC in  CDUs which have no other ILKN ports */
            SHR_IF_ERR_EXIT(imb_ile_port_fec_power_down_set(unit, port, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - control enable/ disable ILE port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] flags - flags
 * \param [in] enable - enable / disable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    uint8 is_xpmd_enabled, ilkn_aligner_supported;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_is_xpmd_enabled(unit, port, &is_xpmd_enabled));
    ilkn_aligner_supported = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_support_aligner);

    if (!enable)
    {
        /*
         * Disable port in wrapper
         */
        SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_set(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));

        if (is_xpmd_enabled && ilkn_aligner_supported)
        {
            SHR_IF_ERR_EXIT(imb_ile_port_xpmd_lane_enable(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));

            /*
             * Disable xpmd interface:  Put Async Fifo in reset and disable lane
             */

            SHR_IF_ERR_EXIT(imb_ile_port_async_fifo_reset(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));
        }
        else if (ilkn_aligner_supported)
        {
            /*
             * Disable bypass interface
             */
            SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set
                            (unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));

            /*
             * Put Async Fifo in reset
             */

            SHR_IF_ERR_EXIT(imb_ile_port_async_fifo_reset(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));

        }
        else if (!dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only))
        {
            /*
             * Disable bypass interface
             */
            SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set
                            (unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));
        }
    }

    if (!(flags & IMB_PORT_ENABLE_F_SKIP_PORTMOD))
    {
        /*
         * Enable / Disable port in Portmod
         */
        SHR_IF_ERR_EXIT(imb_er_portmod_port_enable_set(unit, port, 0, enable));

    }

    sal_usleep(1000);

    if (enable)
    {
        if (is_xpmd_enabled && ilkn_aligner_supported)
        {
            SHR_IF_ERR_EXIT(imb_ile_port_xpmd_lane_enable(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));

            /*
             * Enable xpmd interface : Take Async Fifo out of reset and enable lane
             */
            SHR_IF_ERR_EXIT(imb_ile_port_async_fifo_reset(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));

        }
        else if (ilkn_aligner_supported)
        {
            /*
             * Enable bypass interface
             */
            SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set
                            (unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));

            /*
             * Take Async Fifo out of reset
             */

            SHR_IF_ERR_EXIT(imb_ile_port_async_fifo_reset(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 0));

        }
        else if (!dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only))
        {
            /*
             * Enable bypass interface
             */
            SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set
                            (unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));
        }

        /*
         * Enable port in wrapper
         */
        SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_set(unit, port, IMB_ILE_DIRECTION_RX | IMB_ILE_DIRECTION_TX, 1));

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get enable status of the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - returned enable status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the indication from Portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set the ILE port in loopback.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback type:
 * 0 => no loopback.
 * 1 => MAC loopback (inside the ILKN core)
 * 2 => PHY loopback (TX->RX, inside the PM)
 * 3 => PHY Remote (RX->TX, inside the PM)
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    portmod_loopback_mode_t portmod_loopback;
    int curr_loopback, enable, is_symmetric;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Since the lane map for PM8x50 has been moved to PMD, PHY/REMOTE/MAC
     * loopback cannot be configured when lane map is not symmetric.
     */
    if (loopback == BCM_PORT_LOOPBACK_PHY || loopback == BCM_PORT_LOOPBACK_PHY_REMOTE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_is_symmetric_get(unit, port, &is_symmetric));
        if (!is_symmetric)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Lane Map for port %d is not symmetric. Configuring PHY/REMOTE loopback on non-symmetric lane swapped port is forbidden, please adjust the lane map",
                         port);
        }
    }

    SHR_IF_ERR_EXIT(imb_port_loopback_get(unit, port, &curr_loopback));
    if (curr_loopback == loopback)
    {
        /*
         * Nothing to do
         */
        SHR_EXIT();
    }

    /*
     * Get Enable status to retrieve after setting loopack
     */
    SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &enable));

    /*
     * Disable port before closing loopback
     */
    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port, 0, 0));

    sal_usleep(10000);

    if (curr_loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Open current loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, curr_loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 0));
    }
    if (loopback != BCM_PORT_LOOPBACK_NONE)
    {
        /*
         * Close requested loopback
         */
        SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, loopback, &portmod_loopback));
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, portmod_loopback, 1));
    }

    sal_usleep(10000);
    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port, 0, 1));
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get loopback type on the port.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] loopback - loopback type. see loopback_set for
 *        loopback types
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    int rv = 0, lb_enabled = 0;
    portmod_loopback_mode_t portmod_loopback;
    SHR_FUNC_INIT_VARS(unit);

    *loopback = 0;
    /*
     * Iterate over all portmod loopback types
     */
    for (portmod_loopback = portmodLoopbackMacOuter; portmod_loopback != portmodLoopbackCount; ++portmod_loopback)
    {
        rv = portmod_port_loopback_get(unit, port, portmod_loopback, &lb_enabled);
        if (rv == _SHR_E_UNAVAIL)
        {
            /*
             * Portmod loopback type is not supported for PM type
             */
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        if (lb_enabled)
        {
            SHR_IF_ERR_EXIT(imb_bcm_loopback_from_portmod_loopback_get(unit, port, portmod_loopback, loopback));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get link state indication. includes current link
 *        state and latch down indication (whether the port was
 *        down since last call to this API)
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] clear_status - whether to clear the latch down
 *        indication in DB.
 * \param [out] link_state - link state structure.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state)
{
    int db_latch_down, is_link_up;
    uint32 hw_latch_low_aligned;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * link up indication
     */
    SHR_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY, &is_link_up));

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_get(unit, port, &db_latch_down));

    if ((is_link_up) && !(db_latch_down))
    {
        SHR_IF_ERR_EXIT(imb_ile_port_latch_down_get(unit, port, &hw_latch_low_aligned));
        db_latch_down = hw_latch_low_aligned ? 0 : 1;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, db_latch_down));
    }

    if (clear_status)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_latch_down_set(unit, port, 0));
    }

    link_state->status = is_link_up;
    link_state->latch_down = db_latch_down;

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Measure the serdes rate for ILE and ILU ports. If the port is
 *    over fabric, use fabric measure mechanism. Otherwise, use
 *    NIF measure mechanism.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_rx - indicate the FIFO used to measure
 * \param [out] phy_measure - phy measure output
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_phy_measure_get(
    int unit,
    bcm_port_t port,
    int is_rx,
    soc_dnxc_port_phy_measure_t * phy_measure)
{
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));

    if (is_over_fabric)
    {
        SHR_IF_ERR_EXIT(imb_diag_fabric_phy_measure_get(unit, port, is_rx, phy_measure));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_diag_nif_phy_measure_get(unit, port, is_rx, phy_measure));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * imb_ile_port_mib_counter_get
 *
 * \brief Get ilkn counter
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  counter_type    - counter type
 * \param [out]  counter_val     - ilkn counter value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_mib_counter_get(
    int unit,
    bcm_port_t port,
    int counter_type,
    uint64 *counter_val)
{
    int channel;
    uint32 ctr_low, ctr_high;
    uint64 tmp_counter;
    int speed, port_has_speed;
    uint32 fec_unit;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, 0, &port_has_speed));
    if (port_has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
    }
    else
    {
        speed = 0;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

    switch (counter_type)
    {
        case dnx_mib_counter_ilkn_rx_pkt:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case dnx_mib_counter_ilkn_rx_bytes:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_BYTES_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_BYTES_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case dnx_mib_counter_ilkn_rx_err_pkt:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_rx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_ERR_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_RX_ERR_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case dnx_mib_counter_ilkn_tx_pkt:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case dnx_mib_counter_ilkn_tx_bytes:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_BYTES_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_BYTES_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case dnx_mib_counter_ilkn_tx_err_pkt:
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_tx_counter_get_prepared_by_channel(unit, port, channel));
            /** get the relevant counter*/
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_ERR_PACKETS_LOW, &ctr_low));
            SHR_IF_ERR_EXIT(imb_ile_port_ilkn_counter_dbal_get(unit, port, DBAL_FIELD_TX_ERR_PACKETS_HIGH, &ctr_high));

            COMPILER_64_SET(tmp_counter, ctr_high, ctr_low);
            COMPILER_64_ADD_64(*counter_val, tmp_counter);
            break;
        case dnx_mib_counter_nif_rx_fec_correctable_errors:
            if ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_fec_supported))
                && (DNXC_PORT_PHY_SPEED_IS_PAM4(speed)))
            {
                /*
                 * One FEC instance for every 2 adjacent lanes.
                 * There are total 6 FEC instance.
                 * The max lanenum is 12 for PAM4 ILKN.
                 */
                for (fec_unit = 0; fec_unit < dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit); ++fec_unit)
                {
                    SHR_IF_ERR_EXIT(imb_ile_port_ilkn_fec_counter_dbal_get
                                    (unit, port, fec_unit, DBAL_FIELD_RX_FEC_CORRECTED_CNT, &ctr_low));
                    COMPILER_64_SET(tmp_counter, 0, ctr_low);
                    COMPILER_64_ADD_64(*counter_val, tmp_counter);
                }
            }
            break;
        case dnx_mib_counter_nif_rx_fec_uncorrectable_errors:
            if ((dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_fec_supported))
                && (DNXC_PORT_PHY_SPEED_IS_PAM4(speed)))
            {
                /*
                 * One FEC instance for every 2 adjacent lanes.
                 * There are total 6 FEC instance.
                 * The max lanenum is 12 for PAM4 ILKN.
                 */
                for (fec_unit = 0; fec_unit < dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit); ++fec_unit)
                {
                    SHR_IF_ERR_EXIT(imb_ile_port_ilkn_fec_counter_dbal_get
                                    (unit, port, fec_unit, DBAL_FIELD_RX_FEC_UNCORRECTED_CNT, &ctr_low));
                    COMPILER_64_SET(tmp_counter, 0, ctr_low);
                    COMPILER_64_ADD_64(*counter_val, tmp_counter);
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is invalid", counter_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * imb_ile_port_mib_counter_clear
 *
 * \brief Clear ilkn counter
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_mib_counter_clear(
    int unit,
    bcm_port_t port)
{
    uint32 entry_handle_id_rx, entry_handle_id_tx;
    int channel;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_RX_STATS_ACC, &entry_handle_id_rx));
    dbal_entry_key_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_LOGICAL_PORT, port);
    dbal_entry_value_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_LOGICAL_PORT_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_CMD, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id_rx, DBAL_FIELD_ADDR, INST_SINGLE, channel);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_rx, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ILKN_TX_STATS_ACC, &entry_handle_id_tx));
    dbal_entry_key_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_LOGICAL_PORT, port);
    dbal_entry_value_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_LOGICAL_PORT_TYPE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_CMD, INST_SINGLE, 2);
    dbal_entry_value_field32_set(unit, entry_handle_id_tx, DBAL_FIELD_ADDR, INST_SINGLE, channel);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_tx, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get recommended default values of resource members for given port with speed
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used
 * \param [out] resource - each memeber other than speed with BCM_PORT_RESOURCE_DEFAULT_REQUEST
 *                         will be filled with default value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_resource_t
 *   * BCM_PORT_RESOURCE_DEFAULT_REQUEST
 */
int
imb_ile_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0",
                     port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: there is no given default value for speed. speed is mandatory as input to this API",
                     port);
    }

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed))
        {
            resource->fec_type = bcmPortPhyFecRs544;
        }
        else
        {
            resource->fec_type = bcmPortPhyFecNone;
        }
    }

    /*
     * in case user didn't configure CL72 - by default it is enabled
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /*
     * get default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->phy_lane_config = 0;
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                     SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(resource->phy_lane_config);

        /*
         * FORCE_ES, FORCE_NS must be clear for {NRZ} and for {PAM4 with link training}
         */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);

        if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed) && (resource->link_training == 0))
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
        }

        /*
         * internal fields that will be set in resource_set before calling portmod. Not exposed to user.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);
        /*
         * this field should always be 0 in normal operation, can be changed only for debug.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);

    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check the PHY lane swap (PMD address)
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] phys - port lane bitmap
 * \param [in] lane2serdes - lane2serdes map
 * \param [in] map_size - lane2serdes array size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
static int
imb_ile_port_fec_pmd_laneswap_check(
    int unit,
    int port,
    bcm_pbmp_t * phys,
    soc_dnxc_lane_map_db_map_t * lane2serdes,
    int map_size)
{
    int pre_lane_id = -1, lane_id;
    int first_lane_id, last_lane_id;
    int inner_index;
    int nof_lanes;
    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_LAST(*phys, last_lane_id);
    SHR_MAX_VERIFY(last_lane_id, map_size, _SHR_E_PARAM, "map_size(%d) must be higher than last_lane_id(%d)", map_size,
                   last_lane_id);

    _SHR_PBMP_FIRST(*phys, first_lane_id);
    if (first_lane_id % 2 != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: The combination of FEC type bcmPortPhyFecRs544, first_lane_id %d must be starting from even num! \n ",
                     port, first_lane_id);
    }
    BCM_PBMP_COUNT(*phys, nof_lanes);
    if (nof_lanes % 2 != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: The combination of FEC type bcmPortPhyFecRs544, lane numbers %d is not supported for odd nof_lanes! \n ",
                     port, nof_lanes);
    }

    inner_index = -1;
    BCM_PBMP_ITER(*phys, lane_id)
    {
        inner_index++;
        if (inner_index % 2 == 0)
        {
            pre_lane_id = lane_id;
            continue;
        }
        /*
         * Check the PHY lane swap (PMD address)
         */
        if (lane_id != pre_lane_id + 1 ||
            !DNX_PORT_ADDR_IS_PAIRS_NATUNAL_MATCH(lane2serdes[pre_lane_id].tx_id, lane2serdes[lane_id].tx_id) ||
            !DNX_PORT_ADDR_IS_PAIRS_MATCH(lane2serdes[pre_lane_id].rx_id, lane2serdes[lane_id].rx_id))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: on the PHY lane swap, the two adjacent physical addr should be in pairs!\n", port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check ILKN core level mapping
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] lane_order - lane order
 * \param [in] lane_order_size - lane_order array size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
static int
imb_ile_port_fec_pcs_laneorder_check(
    int unit,
    int port,
    const int *lane_order,
    int lane_order_size)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);

    if (lane_order_size % 2 != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: lane_order size must be even number for FEC type bcmPortPhyFecRs544!\n ", port);
    }
    for (i = 0; i < lane_order_size; i++)
    {
        if (i % 2 == 0)
        {
            continue;
        }
        /*
         * Check the ILKN core level mapping
         */
        if (!DNX_PORT_ADDR_IS_PAIRS_NATUNAL_MATCH(lane_order[i - 1], lane_order[i]))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: on the ILKN core level mapping, the lanes should be in pairs!\n",
                         port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the FEC some limitations for NIF ILKN ports
 *   1. on the PHY lane swap (PMD address), only mapping between the pairs is allowed
 *       On BCM88690 TX -
 *          Outgoing logic lanes [2n] and [2n+1] must use two adjacent physical TX SerDes [2j, 2j+1].
 *          Using [2j+1, 2j] is not supported.
 *       On BCM88690 RX -
 *          Incoming logic lanes [2m] and [2m+1] can use two adjacent physical RX SerDes [2i, 2i+1] or [2i+1, 2i].
 *   2. on the ILKN core level mapping
 *      - the lanes should be in pairs (ILKN ports with FEC must have even number of lanes).
 *        Every even lane must be mapped to an even lane-id, and the following lane should
 *        be mapped to the consecutive lane-id. For example: lane0 can be mapped to lane-id4,
 *        but then lane1 must be mapped to lane-id5.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fec_type - fec type
 * \param [in] speed - speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ile_port_fec_speed_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_fec_t fec_type,
    int speed)
{
    int is_over_fabric = 0, nof_lanes, nof_phys_per_core, is_valid;
    int map_size, lane_order_size, phy_id, phy_in_core, idx;
    bcm_pbmp_t phys;
    dnx_algo_lane_map_type_e lane_map_type;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    int lane_order[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    dnx_algo_port_ilkn_access_info_t ilkn_info;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    nof_phys_per_core =
        (is_over_fabric) ? dnx_data_fabric.links.nof_links_per_core_get(unit) : dnx_data_nif.
        phys.nof_phys_per_core_get(unit);
    BCM_PBMP_COUNT(phys, nof_lanes);

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_type_to_pm_type_get
                    (unit, ilkn_info.pm_imb_type, is_over_fabric, (int *) &pm_type));
    is_valid = FALSE;
    /**
     * Go over all supported NIF interfaces for the device
     * and mark the port configuration as valid only if it passes all the checks
     */
    for (idx = 0; idx < dnx_data_nif.ilkn.supported_interfaces_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_nif.ilkn.supported_interfaces_get(unit, idx)->is_valid)
        {
            if (pm_type == dnx_data_nif.ilkn.supported_interfaces_get(unit, idx)->pm_dispatch_type)
            {
                if (speed == dnx_data_nif.ilkn.supported_interfaces_get(unit, idx)->speed)
                {
                    /**
                     * Fec type is protected by soc_dnxc_port_resource_validate when in set API
                     * The fec type validity skip can be applied only in port resource default request API
                     * where only the speed should be validate
                     */
                    if (fec_type == dnx_data_nif.ilkn.supported_interfaces_get(unit, idx)->fec_type
                        || fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
                    {
                        is_valid = TRUE;
                        break;
                    }
                }
            }
        }
    }

    if (!is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: The combination of speed %d and fec type %d is not supported! \n ",
                     port, speed, fec_type);
    }

    /**
     * Do additional checks if FEC is used
     */
    if (fec_type != bcmPortPhyFecNone)
    {

        /**
         * Check for lane swap if FEC is used
         */
        BCM_PBMP_ITER(phys, phy_id)
        {
            phy_in_core = phy_id % nof_phys_per_core;
            if (phy_in_core >=
                dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) * dnx_data_nif.ilkn.nof_lanes_per_fec_unit_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: unsupported phy %d for using FEC!\n ", port, phy_id);
            }
        }

        lane_map_type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;
        map_size = is_over_fabric ? dnx_data_fabric.links.nof_links_get(unit) : dnx_data_nif.phys.nof_phys_get(unit);

        SHR_ALLOC(lane2serdes, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes mapping struct",
                  "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit, lane_map_type, map_size, lane2serdes));

        /*
         * Check the PHY lane swap (PMD address)
         */
        SHR_IF_ERR_EXIT(imb_ile_port_fec_pmd_laneswap_check(unit, port, &phys, lane2serdes, map_size));
        /*
         * Check the ILKN core level mapping
         */
        SHR_IF_ERR_EXIT(imb_port_logical_lane_order_get(unit, port, nof_lanes, lane_order, &lane_order_size));
        SHR_IF_ERR_EXIT(imb_ile_port_fec_pcs_laneorder_check(unit, port, lane_order, lane_order_size));

        /*
         * Check if number of lanes is even.
         * If FEC is used the number of lanes should be an even number
         */
        if (nof_lanes % 2 != 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Number of lanes for ILKN port %d is %d. Number should be even if FEC is used! \n ", port,
                         nof_lanes);
        }

        /**
         * Check if interface has more than allowed NOF lanes if it uses FEC
         */
        if (nof_lanes > dnx_data_nif.ilkn.lanes_max_nof_using_fec_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Number of lanes for ILKN port %d is %d. Number should be less than maximum %d for cases where FEC is used! \n ",
                         port, nof_lanes, dnx_data_nif.ilkn.lanes_max_nof_using_fec_get(unit));
        }
    }

exit:
    SHR_FREE(lane2serdes);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ilkn logical lane order
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] lane_order - lane_order
 * \param [in] lane_order_size - lane_order array size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ile_port_logical_lane_order_set(
    int unit,
    int port,
    const int *lane_order,
    int lane_order_size)
{
    portmod_speed_config_t port_speed_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));

    if (DNXC_PORT_PHY_SPEED_IS_PAM4(port_speed_config.speed) && port_speed_config.fec == _SHR_PORT_PHY_FEC_RS_544)
    {
        SHR_IF_ERR_EXIT(imb_ile_port_fec_pcs_laneorder_check(unit, port, lane_order, lane_order_size));
    }

    SHR_IF_ERR_EXIT(portmod_port_logical_lane_order_set(unit, port, lane_order, lane_order_size));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get ilkn logical lane order
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] lane_order_max_size - max lane order size
 * \param [out] lane_order - lane_order
 * \param [out] lane_order_actual_size - lane_order array size
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ile_port_logical_lane_order_get(
    int unit,
    bcm_port_t port,
    int lane_order_max_size,
    int *lane_order,
    int *lane_order_actual_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_logical_lane_order_get
                    (unit, port, lane_order_max_size, lane_order, lane_order_actual_size));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set ilkn port speed configuration
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed_config - port speed configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ile_port_speed_config_set(
    int unit,
    bcm_port_t port,
    const portmod_speed_config_t * speed_config)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    portmod_speed_config_t new_speed_config;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set flags in speed config struct according to device features
     * Since 'speed_config' is const we need to copy it to a new struct.
     */
    sal_memcpy(&new_speed_config, speed_config, sizeof(portmod_speed_config_t));

    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    /*
     * if device / IMB type doesn't support FEC skip FEC configuration in portmod
     */
    if ((!dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_fec_supported))
        || (ilkn_access_info.pm_imb_type == imbDispatchTypeImb_clu))
    {
        PORTMOD_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_SET(&new_speed_config);
    }

    SHR_IF_ERR_EXIT(portmod_port_speed_config_set(unit, port, &new_speed_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ilkn port speed configuration
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] speed_config - port speed configuration
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_ile_port_speed_config_get(
    int unit,
    bcm_port_t port,
    portmod_speed_config_t * speed_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, speed_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if phy control is supported for current PM.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] type - Phy Control type
 * \param [in] is_set - if this fuction is called by set function
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_ile_port_phy_control_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    int is_set)
{
    portmod_dispatch_type_t pm_type;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ilkn_access_info_t ilkn_info;
    int is_over_fabric = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    /*
     * We need to filter phy controls based on phy types. For this to happen we are using
     * PM dispatch types. In this case here we are interested to filter some phy controls that are
     * not valid for portmodDispatchTypePm4x25.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_imb_type_to_pm_type_get
                    (unit, ilkn_info.pm_imb_type, is_over_fabric, (int *) &pm_type));

    switch (type)
    {
            /*
             * Sepcial handle for PRBs function, as port control PRBs function needs to
             * call the soc layer PRBs function to deal with Per Lane Gport.
             * Therefore we must ensure the PRBs is supported by soc layer common API.
             */
        case BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "PHY control type %d is not supported, please use bcm_port_control_set/get instead.\r\n",
                         type);
            break;
        }
        case BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        case BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        case BCM_PORT_PHY_CONTROL_RX_HIGH_FREQ_PEAK_FILTER:
        case BCM_PORT_PHY_CONTROL_RX_TAP6:
        case BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME:
        case BCM_PORT_PHY_CONTROL_FEC_CORRUPTION_PERIOD:
        case BCM_PORT_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        {
            if (pm_type == portmodDispatchTypePm4x25)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "PHY control type %d is not supported on Falcon PHY.\r\n", type);
            }
            break;
        }
        case BCM_PORT_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST:
        {
            if (pm_type == portmodDispatchTypePm8x50 || pm_type == portmodDispatchTypePm8x50_fabric)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "PHY control type %d is not supported on Blackhawk PHY.\r\n", type);
            }
            break;
        }
        /** The soc layer will verify the rest types */
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Link status change event handler
 *          If Alignment is stable enable RX data, otherwise disable RX data
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
/*Action on link status change*/
int
imb_ile_port_link_status_change_event(
    int unit,
    bcm_port_t port)
{
    uint32 live_status;
    uint32 achieved_cnt;
    uint32 lost_cnt;
    uint32 is_alignment_stable;
    uint32 pmd_lock_counter, pmd_lock_counter_prev;
    bcm_port_resource_t resource;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Clear Alignment achieved / lost counters
     */
    SHR_IF_ERR_EXIT(imb_ile_port_alignment_counters_get(unit, port, &achieved_cnt, &lost_cnt));

    /*
     * Get alignment live status
     */
    SHR_IF_ERR_EXIT(imb_ile_port_alignment_live_status_get(unit, port, &live_status));
    if (live_status == 0)
    {
        /*
         * If live status is down disable RX data
         */
        SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 0));
        SHR_EXIT();
    }

    /*
     * Wait 1 usec to check alignment stability
     */
    sal_usleep(1);

    /*
     * Get alignment achieved / lost counter
     */
    SHR_IF_ERR_EXIT(imb_ile_port_alignment_counters_get(unit, port, &achieved_cnt, &lost_cnt));

    /*
     * Alignment is defined to be stable if [live status is up] AND [link status was not changed from last read]
     */
    is_alignment_stable = (achieved_cnt == 0) && (lost_cnt == 0);

    /*
     * If Alignment is stable enable RX data, otherwise disable RX data
     */
    if (is_alignment_stable)
    {
        /*
         * Get port type
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        /*
         * Get ILKN access info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_reset_core_after_link_training) &&
            DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_info)
            && ilkn_access_info.pm_imb_type == imbDispatchTypeImb_clu)
        {
            SHR_IF_ERR_EXIT(imb_common_port_resource_get(unit, port, &resource));
            if (resource.link_training)
            {
                SHR_IF_ERR_EXIT(portmod_port_pmd_lock_counter_get(unit, port, &pmd_lock_counter));
                SHR_IF_ERR_EXIT(imbm.pmd_lock_counter.get(unit, port, &pmd_lock_counter_prev));
                if (pmd_lock_counter != pmd_lock_counter_prev)
                {
                    /** disable data into FIFO */
                    SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 0));

                    SHR_IF_ERR_EXIT(imbm.pmd_lock_counter.set(unit, port, pmd_lock_counter));
                    /** reset everything but the serdes to clear ILKN core state */
                    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port, IMB_PORT_ENABLE_F_SKIP_PORTMOD, 0));
                    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, port, IMB_PORT_ENABLE_F_SKIP_PORTMOD, 1));

                }

            }
        }
        SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 1));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_ile_port_rx_data_enable_set(unit, port, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable Prbs
 *
 *  see .h file
 */
int
imb_ile_port_prbs_enable_set(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int enable)
{
    uint32 bypass_flags = 0;
    uint8 is_xpmd_enabled, ilkn_aligner_supported;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_ilu_is_xpmd_enabled(unit, port, &is_xpmd_enabled));
    ilkn_aligner_supported = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_support_aligner);

    bypass_flags = (type == bcmPortControlPrbsRxEnable ? IMB_ILE_DIRECTION_RX : IMB_ILE_DIRECTION_TX);

    if (ilkn_aligner_supported && !is_xpmd_enabled && enable)
    {
        /** disable PCS bypass for q2a falcon */
        SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_set(unit, port, bypass_flags, 0));
        SHR_IF_ERR_EXIT(imb_ile_port_async_fifo_reset(unit, port, bypass_flags, 1));
        SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set(unit, port, bypass_flags, FALSE));
    }

    SHR_IF_ERR_EXIT(imb_common_port_prbs_enable_set(unit, port, type, prbs_mode, enable));
    if (ilkn_aligner_supported && !is_xpmd_enabled && !enable)
    {
        /** enable back PCS bypass */
        SHR_IF_ERR_EXIT(imb_ile_port_bypass_if_enable_set(unit, port, bypass_flags, TRUE));
        SHR_IF_ERR_EXIT(imb_ile_port_async_fifo_reset(unit, port, bypass_flags, 0));
        /*
         * Enable port in wrapper
         */
        SHR_IF_ERR_EXIT(imb_ile_port_wrapper_enable_set(unit, port, bypass_flags, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure power down of ILE
 */
static int
imb_ile_port_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    SHR_IF_ERR_EXIT(imb_ile_power_down_set(unit, ilkn_access_info.ilkn_core, power_down));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure power down FEC in CDUs of this port
 */
static int
imb_ile_port_fec_power_down_set(
    int unit,
    bcm_port_t port,
    int power_down)
{
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    bcm_pbmp_t ilkn_phys;
    bcm_pbmp_t cdu_bmp;
    bcm_pbmp_t all_ilkn_ports;
    bcm_pbmp_t other_ilkn_phys;
    bcm_pbmp_t phys;
    bcm_port_t ilkn_port;
    int iter_phy;
    int cdu_id, ethu_id;
    int pm_index;
    int cdu_port_count;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get ILKN access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, port, &ilkn_access_info));

    if (ilkn_access_info.pm_imb_type == imbDispatchTypeImb_cdu)
    {
        /*
         * get port information
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &ilkn_phys));

        BCM_PBMP_CLEAR(cdu_bmp);
        _SHR_PBMP_ITER(ilkn_phys, iter_phy)
        {
            /** Obtain global ETHU id -- not per core */
            SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, iter_phy, &ethu_id));

            BCM_PBMP_PORT_ADD(cdu_bmp, ethu_id);
        }

        if (power_down == TRUE)
        {
            /*
             * get all nif ILKN logical ports for our specific device core
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN,
                                                       DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &all_ilkn_ports));

            BCM_PBMP_CLEAR(other_ilkn_phys);
            _SHR_PBMP_ITER(all_ilkn_ports, ilkn_port)
            {
                if (ilkn_port != port)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
                    BCM_PBMP_OR(other_ilkn_phys, phys);
                }
            }
        }

        _SHR_PBMP_ITER(cdu_bmp, ethu_id)
        {
            if (power_down == TRUE)
            {
                /*
                 * want to power down the unit
                 * check if there are other ports on the same unit
                 */
                /** CDU contains a single PM */
                pm_index = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->pms[0];
                phys = dnx_data_nif.eth.pm_properties_get(unit, pm_index)->phys;

                BCM_PBMP_AND(phys, other_ilkn_phys);
                BCM_PBMP_COUNT(phys, cdu_port_count);
                if (cdu_port_count != 0)
                {
                    /** there are ports on this CDU - cannot be power down */
                    continue;
                }

            }

            cdu_id = ethu_id % dnx_data_nif.eth.ethu_nof_per_core_get(unit);
            SHR_IF_ERR_EXIT(imb_cdu_ilkn_logic_power_down_set(unit, core, cdu_id, power_down));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
