/*! \file bcmpc_pm_drv_pm8x50_common.c
 *
 * PM common driver for PM8X50.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmdrd/bcmdrd_feature.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include "bcmpc_pm_drv_pm8x50_common.h"
#include <bcmpc/bcmpc_pm_drv_utils.h>

/******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/* Number of PMD lanes per core in PM8x50. */
#define PM8X50_COMMON_NUM_LANES_PER_CORE 8

#define PM8X50_COMMON_NRZ_LANE_SPEED_MAX 25000
#define PM8X50_COMMON_PHY_SIGNALLING_MODE_GET(speed, num_lanes) \
    (speed/num_lanes) > PM8X50_COMMON_NRZ_LANE_SPEED_MAX ? phymodSignallingMethodPAM4 : \
                                                phymodSignallingMethodNRZ
/*******************************************************************************
 * PM driver function vector
 */

int
pm8x50_common_pm_destroy(int unit, pm_info_t *pm_info)
{

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pm_info, SHR_E_FAIL);

    if (pm_info->pm_data.pmd_fw_info != NULL) {
        SHR_FREE(pm_info->pm_data.pmd_fw_info);
        pm_info->pm_data.pmd_fw_info = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_speed_config_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    pm_speed_config_t *speed_config)
{
    phymod_phy_speed_config_t phy_speed_config;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_speed_config_t_init(&phy_speed_config));

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_speed_config_get(phy_acc, &phy_speed_config));

    speed_config->speed = phy_speed_config.data_rate;
    speed_config->fec = phy_speed_config.fec_type;
    speed_config->link_training = phy_speed_config.linkTraining;
    speed_config->num_lanes = shr_util_popcount(phy_acc->access.lane_mask);
    /* need to check if RLM speed, if so adjust the num lane */
    if (speed_config->speed == 350000) {
        speed_config->num_lanes = 7;
    } else if (speed_config->speed == 300000) {
        speed_config->num_lanes = 6;
    } else if (speed_config->speed == 150000) {
        speed_config->num_lanes = 3;
    }

    /* Get the PMD lane config from the hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm8x50_common_port_pmd_lane_cfg_get(unit, pm_acc, op_st,
                                         pm_info,
                                         &phy_speed_config.pmd_lane_config));

    sal_memcpy(&(speed_config->lane_config), &phy_speed_config.pmd_lane_config,
               sizeof(phymod_firmware_lane_config_t));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_loopback_set(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_loopback_mode_t loopback_type, int enable)
{
    int rv = PHYMOD_E_NONE;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    pm_oper_status_type_t status = PM_OPER_SUCCESS;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    switch (loopback_type) {
        case PM_PORT_LPBK_MAC:
            rv = pmacd->lpbk_set(&pmac_acc, pm_acc->pmac_blkport, enable);
           if (SHR_FAILURE(rv)) {
               status = PM_MAC_OPER_ERROR;
               SHR_ERR_EXIT(SHR_E_FAIL);
           }
            break;
        case PM_PORT_LPBK_PHY_PMD:
            rv = phymod_phy_loopback_set(phy_acc, phymodLoopbackGlobal,
                                         enable);
           if (SHR_FAILURE(rv)) {
               status = PM_PHY_OPER_ERROR;
               SHR_ERR_EXIT(SHR_E_FAIL);
           }
           break;
        case PM_PORT_LPBK_REMOTE_PMD:
            rv = phymod_phy_loopback_set(phy_acc, phymodLoopbackRemotePMD,
                                         enable);
           if (SHR_FAILURE(rv)) {
               status = PM_PHY_OPER_ERROR;
               SHR_ERR_EXIT(SHR_E_FAIL);
           }
           break;
        case PM_PORT_LPBK_PHY_PCS:
        case PM_PORT_LPBK_REMOTE_PCS:
        default:
           status = PM_LOOPBACK_TYPE_INVALID;
           SHR_ERR_EXIT(SHR_E_FAIL);
           break;
    }
exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_loopback_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                pm_loopback_mode_t loopback_type, int *enable)
{
    uint32_t lpbk_en = 0;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    switch (loopback_type) {
        case PM_PORT_LPBK_MAC:
            SHR_IF_ERR_EXIT
                 (pmacd->lpbk_get(&pmac_acc, pm_acc->pmac_blkport, &lpbk_en));
            break;
        case PM_PORT_LPBK_PHY_PMD:
            SHR_IF_ERR_EXIT
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackGlobal,
                                         &lpbk_en));
            break;
        case PM_PORT_LPBK_REMOTE_PMD:
            SHR_IF_ERR_EXIT
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackRemotePMD,
                                         &lpbk_en));
            break;
        case PM_PORT_LPBK_REMOTE_PCS:
        default:
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackRemotePCS,
                                         &lpbk_en), SHR_E_UNAVAIL);
            break;
    }

    *enable = lpbk_en;

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_encap_get(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_encap_t *encap)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->encap_get(&pmac_acc, pm_acc->pmac_blkport, encap));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_encap_set(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_encap_t encap)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    pm_encap_t current_encap;
    uint8_t ipg_size;
    const phymod_phy_access_t *phy_access;
    int port_num_lanes;
    phymod_phy_codec_mode_t codec_type;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    phy_access = &(pm_acc->phy_acc);

    status = PM_MAC_OPER_ERROR;
    SHR_IF_ERR_VERBOSE_EXIT(pm8x50_common_port_encap_get(unit, pm_acc, op_st,
                                                     pm_info, &current_encap));

    if (encap == BCMPMAC_ENCAP_HG3) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pm_drv_pm8x50.pm_mac_avg_ipg_get(unit, pm_acc, op_st,
                                                    pm_info, &ipg_size));

        if (ipg_size != 8) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    /*
     * Programming sequecnce for HiGig3
     * Set SPEED_ID_TABLE.CODEC_MODE[1] = 1 (Reduced IPG Codec)
     * Load TX_LKUP_1588_MEM_MPP0 or TX_LKUP_1588_MEM_MPP1 with
     * a special TX 1588 lookup table (In reduced IPG mode, SOP is
     *  used to capture the timestamp).
     * Set CDMAC_TX_CTRL.AVERAGE_IPG = 8.
     * Set CDMAC_MODE.HDR_MODE = 0x6
     *               (Encoding: IEEE_4BYTES - K.SOP + 2 byte Preamble + SFD).
     */

    if (encap != current_encap) {
        if (encap == BCMPMAC_ENCAP_HG3) {
            codec_type = phymodCodecReducedIPG;
        } else {
            SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask), port_num_lanes,
                               0, PM8X50_COMMON_NUM_LANES_PER_CORE);
            if (port_num_lanes == 1) {
                codec_type = phymodCodecCL49;
            } else {
                codec_type = phymodCodecCL82;
            }
        }
        status = PM_PHY_OPER_ERROR;
        SHR_IF_ERR_VERBOSE_EXIT
            (phymod_phy_codec_mode_set(phy_access, codec_type));

        status = PM_MAC_OPER_ERROR;
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->encap_set(&pmac_acc, pm_acc->pmac_blkport,
                              encap));
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_pause_ctrl_set(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_pause_ctrl_t *pause_ctrl)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_EXIT
        (pmacd->pause_set(&pmac_acc, pm_acc->pmac_blkport, pause_ctrl));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_pause_ctrl_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_pause_ctrl_t *pause_ctrl)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pause_get(&pmac_acc, pm_acc->pmac_blkport, pause_ctrl));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_pause_addr_set(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_mac_t pause_addr)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_EXIT
        (pmacd->pause_addr_set(&pmac_acc, pm_acc->pmac_blkport, pause_addr));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_pause_addr_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_mac_t pause_addr)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pause_addr_get(&pmac_acc, pm_acc->pmac_blkport, pause_addr));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_frame_max_set(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 uint32_t max_frame_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_EXIT
        (pmacd->frame_max_set(&pmac_acc, pm_acc->pmac_blkport, max_frame_size));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_frame_max_get(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 uint32_t *max_frame_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->frame_max_get(&pmac_acc, pm_acc->pmac_blkport, max_frame_size));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_enable_set(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flag,  uint32_t enable)
{

    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    uint32_t failover_en, mac_en = 0;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    if (flag & PM_PORT_ENABLE_MAC) {
        status = PM_MAC_OPER_ERROR;

        /* If the LAG failover is enabled, always keep the MAC enabled. */
        SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->failover_enable_get(&pmac_acc, pm_acc->pmac_blkport,
                                            &failover_en));
        mac_en = failover_en ? 1 : enable;
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->rx_enable_set(&pmac_acc, pm_acc->pmac_blkport, mac_en));
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->tx_enable_set(&pmac_acc, pm_acc->pmac_blkport, mac_en));
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->mac_reset_set(&pmac_acc, pm_acc->pmac_blkport,
                                  (mac_en == 1) ? 0 : 1));
    }

    /* for PM8x05 GEN2, will use new phymod_port_enable function to do phy enable */
    if (flag & PM_PORT_ENABLE_PHY) {
        status = PM_PHY_OPER_ERROR;
        if ((pm_info->type != PM_TYPE_PM8X50) && bcmdrd_feature_is_real_hw(unit))  {
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_port_enable_set(phy_acc, enable));
        } else {
            if (enable) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_tx_lane_control_set(phy_acc, phymodTxSquelchOff));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (phymod_phy_rx_lane_control_set(phy_acc, phymodRxSquelchOff));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_tx_lane_control_set(phy_acc, phymodTxSquelchOn));
                SHR_IF_ERR_VERBOSE_EXIT
                    (phymod_phy_rx_lane_control_set(phy_acc, phymodRxSquelchOn));
            }
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_enable_get(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flag, uint32_t *enable)
{
    uint32_t mac_reset = 0;
    phymod_phy_tx_lane_control_t tx_ctrl;
    phymod_phy_rx_lane_control_t rx_ctrl;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    if (flag & PM_PORT_ENABLE_PHY) {
        if ((pm_info->type == PM_TYPE_PM8X50_GEN2) && bcmdrd_feature_is_real_hw(unit)) {
            PHY_IF_ERR_EXIT
                (phymod_port_enable_get(phy_acc, enable));
        } else {
            PHY_IF_ERR_EXIT
                (phymod_phy_tx_lane_control_get(phy_acc, &tx_ctrl));
            PHY_IF_ERR_EXIT
                (phymod_phy_rx_lane_control_get(phy_acc, &rx_ctrl));

            if (tx_ctrl == phymodTxSquelchOn && rx_ctrl == phymodRxSquelchOn) {
                *enable = 0;
            } else {
                *enable = 1;
            }
        }
    }
    if (flag & PM_PORT_ENABLE_MAC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->mac_reset_get(&pmac_acc, pm_acc->pmac_blkport, &mac_reset));
        if (mac_reset) {
            *enable = 0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->rx_enable_get(&pmac_acc, pm_acc->pmac_blkport, enable));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_lag_failover_loopback_get(int unit,
                                             const pm_access_t *pm_acc,
                                             pm_oper_status_t *op_st,
                                             pm_info_t *pm_info,
                                             uint32_t *enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->failover_loopback_get(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();

}

int
pm8x50_common_port_tx_set(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          phymod_tx_t *tx_cfg)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

     rv = phymod_phy_tx_set(phy_acc, tx_cfg);

    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (SHR_FAILURE(rv) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_PHY_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_tx_get(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          phymod_tx_t *tx_cfg)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_get(phy_acc, tx_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_pmd_lane_cfg_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    phymod_firmware_lane_config_t *fw_lane_cfg)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_firmware_lane_config_get(phy_acc, fw_lane_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_failover_enable_set(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info,
                                       uint32_t enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->failover_enable_set(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_failover_enable_get(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st,
                                       pm_info_t *pm_info,
                                       uint32_t *enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->failover_enable_get(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_link_training_set(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info,
                                     uint32_t enable)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    uint32_t status = 0;

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_cl72_get(phy_acc, &status));

    if (enable != status) {
        PHY_IF_ERR_EXIT
            (phymod_phy_cl72_set(phy_acc, enable));
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_PHY_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_link_training_get(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info,
                                     uint32_t *enable)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_cl72_get(phy_acc, enable));
exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_phy_status_get(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_phy_status_t *phy_st)
{
    uint32_t linear_enable = 0, jp03b_enable = 0;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    phymod_tx_override_t local_tx_override;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));
    phymod_rx_t_init(&phy_st->rx_afe);

    /* tx_squelch.  */
    PHY_IF_ERR_EXIT
        (phymod_phy_tx_lane_control_get(phy_acc, &phy_st->tx_control));

    /* rx_squelch. */
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_lane_control_get(phy_acc, &phy_st->rx_control));

    /*!
     * Get vga, peaking filter, low_fre_peaking filter, hi_fre_peaking_filter,
     * dfe_taps.
     */
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_get(phy_acc, &phy_st->rx_afe));

   /* rx signal detect. */
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_signal_detect_get(phy_acc, &phy_st->rx_signal_detect));

   /* pam4 precoder enable state. */
    PHY_IF_ERR_EXIT
        (phymod_phy_tx_pam4_precoder_enable_get(phy_acc,
                                            &phy_st->pam4_tx_precoder_enabled));

   /* pam4 pattern enable state. */
    PHY_IF_ERR_EXIT
        (phymod_phy_PAM4_tx_pattern_enable_get(phy_acc,
                                    phymod_PAM4TxPattern_JP03B, &jp03b_enable));
    PHY_IF_ERR_EXIT
        (phymod_phy_PAM4_tx_pattern_enable_get(phy_acc,
                                  phymod_PAM4TxPattern_Linear, &linear_enable));
    if (jp03b_enable) {
        phy_st->pam4_tx_pat = phymod_PAM4TxPattern_JP03B;
    } else if (linear_enable) {
        phy_st->pam4_tx_pat = phymod_PAM4TxPattern_Linear;
    } else {
        phy_st->pam4_tx_pat = phymod_Count;
    }

   /* ECC interrupt. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->interrupt_enable_get(&pmac_acc, pm_acc->pmac_blkport,
                                     &(phy_st->phy_ecc_intr_enabled)));

   /* get the tx pi override value */
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_tx_override_get(phy_acc, &local_tx_override));

    phy_st->tx_pi = local_tx_override.phase_interpolator.value;

    SHR_IF_ERR_EXIT
        (phymod_phy_fec_bypass_indication_get(phy_acc,
                                              &phy_st->phy_fec_bypass_ind));
exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_phy_control_set(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t flags, pm_phy_control_t *pctrl)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    phymod_tx_override_t local_tx_override;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    if (flags & PM_PHY_CTRL_F_TX_SQUELCH) {
        PHY_IF_ERR_EXIT
            (phymod_phy_tx_lane_control_set(phy_acc, pctrl->tx_control));
    }
    if (flags & PM_PHY_CTRL_F_RX_SQUELCH) {
        PHY_IF_ERR_EXIT
            (phymod_phy_rx_lane_control_set(phy_acc, pctrl->rx_control));
    }
    if (flags & PM_PHY_CTRL_F_RX_ADPT_RESUME) {
        PHY_IF_ERR_EXIT
            (phymod_phy_rx_adaptation_resume(phy_acc));
    }
    if (flags & (PM_PHY_CTRL_F_RX_AFE_VGA |
                 PM_PHY_CTRL_F_RX_AFE_PK_FLT |
                 PM_PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT |
                 PM_PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT |
                 PM_PHY_CTRL_F_RX_AFE_DFE_TAP)) {
        PHY_IF_ERR_EXIT
            (phymod_phy_rx_set(phy_acc, &(pctrl->rx_afe)));
    }
    if (flags & PM_PHY_CTRL_F_PAM4_TX_PATTERN) {
        PHY_IF_ERR_EXIT
            (phymod_phy_PAM4_tx_pattern_enable_set(phy_acc,
                                                   pctrl->pam4_tx_pat,
                                                   pctrl->pam4_tx_pat_enable));
    }
    if (flags & PM_PHY_CTRL_F_PAM4_TX_PRECODER) {
        PHY_IF_ERR_EXIT
            (phymod_phy_tx_pam4_precoder_enable_set(phy_acc,
                                             pctrl->pam4_tx_precoder_enable));
    }
    if (flags & PM_PHY_CTRL_F_ECC_INTR_ENABLE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->interrupt_enable_set(&pmac_acc, pm_acc->pmac_blkport,
                                         pctrl->phy_ecc_intr_enable));
    }

    if (flags & PM_PHY_CTRL_F_TX_PI_OVERRIDE) {
        local_tx_override.phase_interpolator.enable = 1;
        local_tx_override.phase_interpolator.value = pctrl->tx_pi;
        PHY_IF_ERR_EXIT
            (phymod_phy_tx_override_set(phy_acc, &local_tx_override));
    }

    if (flags & PM_PHY_CTRL_F_PMD_DEBUG_LANE_EVENT_LOG_LEVEL) {
        PHY_IF_ERR_EXIT
            (phymod_phy_pmd_lane_diag_debug_level_set(phy_acc, pctrl->pmd_debug_lane_event_log_level));
    }

    if (flags & PM_PHY_CTRL_F_FEC_BYPASS_IND) {
        PHY_IF_ERR_EXIT
            (phymod_phy_fec_bypass_indication_set(phy_acc, pctrl->phy_fec_bypass_ind));
    }

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_avg_ipg_get(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          uint8_t *ipg_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->avg_ipg_get(&pmac_acc, pm_acc->pmac_blkport, ipg_size));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_avg_ipg_set(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          uint8_t ipg_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));


    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->avg_ipg_set(&pmac_acc, pm_acc->pmac_blkport, ipg_size));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_synce_clk_ctrl_set(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      pm_port_synce_clk_ctrl_t *cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
    phy_synce_cfg.stg0_mode = cfg->stg0_mode;
    phy_synce_cfg.stg1_mode = cfg->stg1_mode;
    phy_synce_cfg.sdm_val = cfg->sdm_val;

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_synce_clk_ctrl_set(phy_acc, phy_synce_cfg));
exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_PHY_OPER_ERROR);
    }

    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_synce_clk_ctrl_get(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st,
                                      pm_info_t *pm_info,
                                      pm_port_synce_clk_ctrl_t *cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_synce_clk_ctrl_get(phy_acc, &phy_synce_cfg));

    cfg->stg0_mode = phy_synce_cfg.stg0_mode;
    cfg->stg1_mode = phy_synce_cfg.stg1_mode;
    cfg->sdm_val = phy_synce_cfg.sdm_val;

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_link_training_status_get(int unit, const pm_access_t *pm_acc,
                                            pm_oper_status_t *op_st,
                                            pm_info_t *pm_info,
                                            uint32_t *locked)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    phymod_cl72_status_t status;

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_cl72_status_get(phy_acc, &status));

    *locked = status.locked? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_default_tx_taps_config_get(int unit, const pm_access_t *pm_acc,
                                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                                              pm_speed_config_t *speed_config,
                                              phymod_tx_t *tx_taps_config)
{
    phymod_phy_signalling_method_t sig_mode = phymodSignallingMethodNRZ;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    sig_mode = PM8X50_COMMON_PHY_SIGNALLING_MODE_GET(speed_config->speed,
                                          speed_config->num_lanes);
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_tx_taps_default_get(phy_acc, sig_mode,
                                        tx_taps_config));
exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_phy_polarity_get(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               phymod_polarity_t *polarity)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_polarity_get(phy_acc, polarity));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_core_lane_map_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                phymod_lane_map_t *lane_map)
{
    const phymod_core_access_t *core_acc = &(pm_acc->core_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_lane_map_get(core_acc, lane_map));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_port_tx_timestamp_info_get(int unit, const pm_access_t *pm_acc,
                                         pm_oper_status_t *op_st,
                                         pm_info_t *pm_info,
                                         pm_tx_timestamp_info_t *timestamp_info)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    phymod_ts_fifo_status_t phy_timestamp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_ts_fifo_status_t_init(&phy_timestamp));

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_timesync_tx_info_get(phy_acc, &phy_timestamp));

    timestamp_info->timestamp_lo = phy_timestamp.ts_in_fifo_lo;
    timestamp_info->timestamp_hi = phy_timestamp.ts_in_fifo_hi;
    timestamp_info->sequence_id = phy_timestamp.ts_seq_id;
    timestamp_info->timestamp_sub_nanosec = phy_timestamp.ts_sub_nanosec;

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_phy_rsfec_uncorrectable_counter_get(int unit,
                                                  const pm_access_t *pm_acc,
                                                  pm_info_t *pm_info,
                                                  uint64_t *count)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);
    *count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_fec_cl91_uncorrectable_counter_get(phy_acc, (uint32_t *)count));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_phy_rsfec_correctable_counter_get(int unit,
                                                const pm_access_t *pm_acc,
                                                pm_info_t *pm_info,
                                                uint64_t *count)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);
    *count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_fec_cl91_correctable_counter_get(phy_acc, (uint32_t *)count));

exit:
    SHR_FUNC_EXIT();
}

int
pm8x50_common_phy_rsfec_symbol_error_counter_get(int unit,
                                                 const pm_access_t *pm_acc,
                                                 pm_info_t *pm_info,
                                                 uint64_t *count)
{
    uint32_t err_count[PM8X50_COMMON_NUM_LANES_PER_CORE];
    int i, actual_count;

    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);
    *count = 0;

    sal_memset(err_count, 0, sizeof(err_count));
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_rsfec_symbol_error_counter_get(phy_acc,
                                                   PM8X50_COMMON_NUM_LANES_PER_CORE,
                                                   &actual_count,
                                                   err_count));

    for (i = 0; i < PM8X50_COMMON_NUM_LANES_PER_CORE; i++) {
        if (i < actual_count) {
            *count += err_count[i];
        } else {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
