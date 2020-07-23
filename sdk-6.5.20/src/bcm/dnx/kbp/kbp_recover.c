/** \file kbp_recover.c
 *
 * Functions for handling Jericho2 KBP recover.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

/*************
 * INCLUDES  *
 */
/*
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/port.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/util.h>
#include <sal/core/boot.h>
#include <sal/core/sync.h>
#include <sal/core/time.h>
#include <soc/dnxc/dnxc_port.h>

#if defined(INCLUDE_KBP)

#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

#define DNX_KBP_PORT_MAX_LANES           12

/*
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */
/**
 * \brief
 *   Reset the KBP TX PCS, then restore the
 *   original configuration.
 * \param [in] unit - The unit number.
 * \param [in] mdio_id - The kbp mdio ID.
 * \param [in] kbp_port_id - kbp port ID.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_tx_pcs_reset_toggle(
    int unit,
    uint32 mdio_id,
    int kbp_port_id)
{
    uint16 data_16;
    uint32 kbp_reg_addr;

    SHR_FUNC_INIT_VARS(unit);

    kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PCS_EN_REG);
    SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_read(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & data_16));
    data_16 &= ~(1 << kbp_port_id);
    SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & data_16));

    sal_usleep(200000);

    data_16 |= (1 << kbp_port_id);
    SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & data_16));

    sal_usleep(200000);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset the KBP TX phase matching fifo.
 * \param [in] unit - The unit number.
 * \param [in] mdio_id - The kbp mdio ID.
 * \param [in] kbp_port_id - kbp port ID.
 * \param [in] reset - 0: out of reset, 1: in reset.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_phase_matching_fifo_reset(
    int unit,
    uint32 mdio_id,
    int kbp_port_id,
    int reset)
{
    uint16 reg_val;
    uint32 kbp_reg_addr;
    SHR_FUNC_INIT_VARS(unit);

    reg_val = reset ? DNX_KBP_TX_PHASE_MATCHING_FIFO_IN_RESET_VAL : DNX_KBP_TX_PHASE_MATCHING_FIFO_OUT_OF_RESET_VAL;

    if (kbp_port_id)
    {
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD4_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD5_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD6_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD7_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD8_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
    }
    else
    {
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD0_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD1_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD2_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(2, DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD3_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write(Kbp_app_data[unit]->xpt_p[0], 16, kbp_reg_addr, (uint8_t *) & reg_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get KBP record counter.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] port - kbp port.
 * \param [in] mdio_id - The kbp mdio ID.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_recover_txpcs_soft_reset(
    int unit,
    int core,
    bcm_port_t port,
    uint32 mdio_id)
{
    bcm_port_link_state_t link_state;
    int kbp_port_id = 0, i;

    SHR_FUNC_INIT_VARS(unit);

    kbp_port_id =
        (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
         core) ? kbp_port_id : kbp_port_id + 1;

    SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, mdio_id, kbp_port_id, 1));
    sal_usleep(200000);
    SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, mdio_id, kbp_port_id, 0));

    /*
     * Loop 1s until the link up
     */
    for (i = 0; i < DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES; i++)
    {
        sal_usleep(10000);
        SHR_IF_ERR_EXIT(bcm_port_link_state_get(unit, port, 0, &link_state));
        if (link_state.status == BCM_PORT_LINK_STATUS_UP)
        {
            break;
        }
    }
    if (i == DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "LINK NOT UP AFTER 1 sec for port %d\n"), port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   OP link statbility check.
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] mdio_id - The kbp mdio ID.
 * \param [in] retries - tried times.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_op_link_stability_check(
    int unit,
    uint32 core,
    uint32 kbp_mdio_id,
    uint32 retries)
{
    bcm_port_t kbp_port;
    int i, j, lane, ls_interval;
    int ilkn_rate, ilkn_metaframe, kbp_nof_lanes = 0;
    uint32 eye_margin;
    uint32 hz_l[DNX_KBP_PORT_MAX_LANES] = { 0 }, hz_r[DNX_KBP_PORT_MAX_LANES] =
    {
    0};
    uint32 vt_u[DNX_KBP_PORT_MAX_LANES] = { 0 }, vt_d[DNX_KBP_PORT_MAX_LANES] =
    {
    0};
    uint32 hz_l_avg, hz_r_avg, vt_u_avg, vt_d_avg;
    uint8 toggle_txpcs = 0, dead_lane = 0;
    bcm_port_link_state_t link_state;
    int kbp_port_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_ilkn_interface_param_get
                    (unit, core, &kbp_port, &kbp_nof_lanes, &ilkn_rate, &ilkn_metaframe));
    kbp_port_id =
        (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
         core) ? kbp_port_id : kbp_port_id + 1;

    SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &ls_interval));
    if (!ls_interval)
    {
        /*
         * If linkscan is not enabled, toggle pcs to avoid dead lane.
         */
        SHR_IF_ERR_EXIT(dnx_kbp_tx_pcs_reset_toggle(unit, kbp_mdio_id, kbp_port_id));
    }

    for (i = 0; i < retries; ++i)
    {
        /*
         * reset eye margin averages and toggle_txpcs
         * indicator for each retry
         */
        hz_l_avg = 0;
        hz_r_avg = 0;
        vt_u_avg = 0;
        vt_d_avg = 0;
        toggle_txpcs = 0;
        dead_lane = 0;

        for (lane = 0; lane < kbp_nof_lanes; lane++)
        {
            /*
             * horizontal left
             */
            /*
             * coverity[overrun-call]
             */
            SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_get(unit,
                                                          kbp_port,
                                                          -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_HZ_L, &eye_margin));
            hz_l[lane] = eye_margin;
            hz_l_avg += eye_margin;
            /*
             * horizontal right
             */
            /*
             * coverity[overrun-call]
             */
            SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_get(unit,
                                                          kbp_port,
                                                          -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_HZ_R, &eye_margin));
            hz_r[lane] = eye_margin;
            hz_r_avg += eye_margin;
            /*
             * vertical up
             */
            /*
             * coverity[overrun-call]
             */
            SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_get(unit,
                                                          kbp_port,
                                                          -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_VT_U, &eye_margin));
            vt_u[lane] = eye_margin;
            vt_u_avg += eye_margin;
            /*
             * vertical down
             */
            /*
             * coverity[overrun-call]
             */
            SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_get(unit,
                                                          kbp_port,
                                                          -1, lane, 0, SOC_PHY_CONTROL_EYE_VAL_VT_D, &eye_margin));
            vt_d[lane] = eye_margin;
            vt_d_avg += eye_margin;

            if (!hz_l[lane] || !hz_r[lane] || !vt_u[lane] || !vt_d[lane])
            {
                toggle_txpcs = 1;
                dead_lane = 1;
                break;
            }
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "dnx_kbp_op_link_stability_check: port %d, lane %d, HZ_L %d, HZ_R %d, VT_U %d, VT_D %d, nof_lanes %d\n"),
                       kbp_port, lane, hz_l[lane], hz_r[lane], vt_u[lane], vt_d[lane], kbp_nof_lanes));
        }

        if (!dead_lane)
        {
            /*
             * will test each lane's margin to 80% of average of the lanes
             */
            hz_l_avg = (hz_l_avg / kbp_nof_lanes) * 4 / 5;
            hz_r_avg = (hz_r_avg / kbp_nof_lanes) * 4 / 5;
            vt_u_avg = (vt_u_avg / kbp_nof_lanes) * 4 / 5;
            vt_d_avg = (vt_d_avg / kbp_nof_lanes) * 4 / 5;
            /*
             * if any lane's eye margin is less than 80% of average, need to toggle txpcs
             */
            for (lane = 0; lane < kbp_nof_lanes; lane++)
            {
                if ((hz_l[lane] < hz_l_avg) || (hz_r[lane] < hz_r_avg) ||
                    (vt_u[lane] < vt_u_avg) || (vt_d[lane] < vt_d_avg))
                {
                    toggle_txpcs = 1;
                }
            }
        }

        if (i < retries - 1)
        {
            /*
             * If any lane is a "dead_lane" i.e. any eye margin parameter
             * was zero, then toggle OP PHMFIFO then toggle txpcs
             */
            if (dead_lane)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, kbp_mdio_id, kbp_port_id, 1));
                sal_usleep(200000);
                SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, kbp_mdio_id, kbp_port_id, 0));
                sal_usleep(400000);
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "dnx_kbp_op_link_stability_check link recovery iteration %d/%d failed for dead lane, trying again.\n"),
                          i + 1, retries));
            }
            /*
             * toggle TxPCS if any lane has small eye compared to its peers
             */
            if (toggle_txpcs)
            {
                SHR_IF_ERR_EXIT(dnx_kbp_tx_pcs_reset_toggle(unit, kbp_mdio_id, kbp_port_id));
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "dnx_kbp_op_link_stability_check link recovery iteration %d/%d failed for small eye, trying again.\n"),
                          i + 1, retries));
            }
            else
            {
                /*
                 * Loop 1s until the link up
                 */
                for (j = 0; j < DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES; j++)
                {
                    sal_usleep(10000);
                    SHR_IF_ERR_EXIT(bcm_port_link_state_get(unit, kbp_port, 0, &link_state));
                    if (link_state.status == BCM_PORT_LINK_STATUS_UP)
                    {
                        SHR_EXIT();
                    }
                    else
                    {
                        LOG_INFO(BSL_LOG_MODULE,
                                 (BSL_META_U(unit,
                                             "dnx_kbp_op_link_stability_check link is NOT up for port %d, trying again.\n"),
                                  kbp_port));
                    }
                }
                if (j == DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES)
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_tx_pcs_reset_toggle(unit, kbp_mdio_id, kbp_port_id));
                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "dnx_kbp_op_link_stability_check link not up after 1 sec for port %d.\n"),
                              kbp_port));
                }
            }
        }
        else
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Eyescan Margin is too small for KBP port %d, ROP transmission may fail!\n"),
                      kbp_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   run recover sequence
 * \param [in] unit - The unit number.
 * \param [in] core - The core number.
 * \param [in] mdio_id - The kbp mdio ID.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_recover_run_recovery_sequence(
    int unit,
    uint32 core,
    uint32 mdio_id)
{
    int i;
    bcm_port_t ilkn_port;
    dnx_kbp_record_reply_t reccord_reply;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_ilkn_interface_param_get(unit, core, &ilkn_port, NULL, NULL, NULL));
    SHR_IF_ERR_EXIT(dnx_kbp_recover_txpcs_soft_reset(unit, core, ilkn_port, mdio_id));
    /*
     * Loop until the valid indicator is cleared
     */
    for (i = 0; i < DNX_KBP_RECORD_REPLY_VALID_CHECK_TIMES; i++)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_cpu_lookup_reply(unit, core, &reccord_reply));
        if (reccord_reply.reply_valid == 0)
        {
            break;
        }
    }
    if (i == DNX_KBP_RECORD_REPLY_VALID_CHECK_TIMES)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Record reply valid indicator cannot be cleared\n")));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Toggle link on OP2 for PAM4
 * \param [in] unit - The unit number.
 * \param [in] kbp_inst - kbp instance
 * \param [in] kbp_port_id - kbp port id
 * \param [in] lane_num -  ilkn num lane
 * \param [in] is_last -  is last kbp port
 * \param [in] enable - enable or disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_op2_pam4_link_interface_reset(
    int unit,
    int kbp_inst,
    int kbp_port_id,
    int ilkn_num_lanes,
    int is_last,
    uint32 enable)
{
    uint16 data_16;
    uint32 kbp_reg_addr;
    int devad;
    soc_timeout_t to;

    SHR_FUNC_INIT_VARS(unit);

    devad = (kbp_port_id % DNX_KBP_VALID_KBP_PORTS_NUM) ? 2 : 1;

    if (enable)
    {
        /*
         * Reset PHMF FIFO
         */
        data_16 = 0x6262;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_PHMF_FIFO_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));
        data_16 = 0x6260;
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));
        /*
         * Enable TX PCS
         */
        data_16 = 0x10;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_RX_TX_PCS_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        sal_usleep(SECOND_USEC);
        /*
         * Enable RX and TX PCS
         */
        data_16 = 0x11;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_RX_TX_PCS_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        /*
         * Waiting max up to 10s to check CDR lock
         */
        soc_timeout_init(&to, 10 * SECOND_USEC, 0);
        do
        {
            sal_usleep(SECOND_USEC);
            kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_CDR_STATUS_REG);
            SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_read
                            (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

            if (data_16 == ((0x1 << ilkn_num_lanes) - 1))
                break;

        }
        while (!soc_timeout_check(&to));

        if (soc_timeout_check(&to))
        {
            SHR_ERR_EXIT(BCM_E_TIMEOUT, "KBP Port Id %d : Check CDR lock on OP2 timeout.\n", kbp_port_id);
        }
        /*
         * Enable FEC
         */
        data_16 = 0xf00;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_FEC_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        /*
         * Waiting max up to 10s to check Link up
         */
        soc_timeout_init(&to, 10 * SECOND_USEC, 0);
        do
        {
            sal_usleep(SECOND_USEC);
            kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_LINKUP_STATUS_REG);
            SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_read
                            (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

            if ((data_16 & 0x1) == 1)
                break;

        }
        while (!soc_timeout_check(&to));

        if (soc_timeout_check(&to))
        {
            SHR_ERR_EXIT(BCM_E_TIMEOUT, "KBP Port Id %d : Check Link up on OP2 timeout.\n", kbp_port_id);
        }
        /*
         * clear error
         */
        data_16 = 0xffff;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_CLEAR_ERR_0_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        data_16 = 0x1fff;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_CLEAR_ERR_0_1_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        data_16 = 0xffff;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_CLEAR_ERR_1_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        data_16 = 0x7;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_CLEAR_ERR_2_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));

        if (is_last)
        {
            uint64 data_64;
            COMPILER_64_SET(data_64, 0xffffffff, 0xffffffff);
            kbp_reg_addr = DNX_KBP_CLEAR_ERR_3_CTRL_REG;
            SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                            (Kbp_app_data[unit]->xpt_p[kbp_inst], 64, kbp_reg_addr, (uint8_t *) & data_64));
        }
    }
    else
    {
        /*
         * Disable FEC
         */
        data_16 = 0x0;
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_FEC_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));
        /*
         * Disable RX and TX PCS
         */
        kbp_reg_addr = DNX_KBP_CLAUSE45_ADDR(devad, DNX_KBP_RX_TX_PCS_CTRL_REG);
        SHR_IF_ERR_EXIT(kbp_pcie_sat_bus_write
                        (Kbp_app_data[unit]->xpt_p[kbp_inst], 16, kbp_reg_addr, (uint8_t *) & data_16));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Toggle link both on OP2 and JR2 side
 * \param [in] unit - The unit number.
 * \param [in] enable - Disable or enable link on both side.
 * \param [out] ifc_status - ifc_status
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_device_interface_enable_link(
    int unit,
    uint32 enable)
{
    bcm_port_t elk_port, last_port;
    bcm_pbmp_t port_pbmp, port_pbmp_inst;
    int ilkn_id, kbp_port_id = 0, core_id;
    int ilkn_num_lanes;
    int ilkn_rate;
    int is_last_port;
    int kbp_inst;
    SHR_FUNC_INIT_VARS(unit);

    if (Kbp_app_data[unit] == NULL || Kbp_app_data[unit]->device_type != KBP_DEVICE_OP2)
    {
        /*
         * For non OP2, we don't need toggle link.
         */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &port_pbmp));

    DNX_KBP_INST_ITER(unit, kbp_inst)
    {
        /*
         * Find elk port pbmp for each kbp instance.
         */
        BCM_PBMP_CLEAR(port_pbmp_inst);
        BCM_PBMP_ITER(port_pbmp, elk_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, elk_port, &ilkn_id));
            if (DNX_KBP_IS_SECOND_KBP_EXIST)
            {
                if ((kbp_inst == 0 && ilkn_id % 2 == 0) || (kbp_inst == 1 && ilkn_id % 2 == 1))
                {
                    BCM_PBMP_PORT_ADD(port_pbmp_inst, elk_port);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                BCM_PBMP_PORT_ADD(port_pbmp_inst, elk_port);
            }
        }

        _SHR_PBMP_LAST(port_pbmp_inst, last_port);

        BCM_PBMP_ITER(port_pbmp_inst, elk_port)
        {
            /** reset everything include the serdes to clear ILKN core state */
            SHR_IF_ERR_EXIT(imb_port_enable_set(unit, elk_port, 0, enable));

            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, elk_port, 0, &ilkn_rate));
            if (DNXC_PORT_PHY_SPEED_IS_PAM4(ilkn_rate))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, elk_port, &core_id));
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, elk_port, &ilkn_num_lanes));
                kbp_port_id = kbp_inst * DNX_KBP_VALID_KBP_PORTS_NUM;
                kbp_port_id =
                    (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
                     core_id) ? kbp_port_id : kbp_port_id + 1;

                kbp_port_id = kbp_port_id % DNX_KBP_VALID_KBP_PORTS_NUM;
                is_last_port = (elk_port == last_port);

                SHR_IF_ERR_EXIT(dnx_kbp_op2_pam4_link_interface_reset
                                (unit, kbp_inst, kbp_port_id, ilkn_num_lanes, is_last_port, enable));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */

#undef BSL_LOG_MODULE
