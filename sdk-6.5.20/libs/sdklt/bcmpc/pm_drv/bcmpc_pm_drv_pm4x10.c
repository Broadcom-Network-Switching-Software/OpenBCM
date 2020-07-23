/*! \file bcmpc_pm_drv_pm4x10.c
 *
 * PM driver for PM4x10.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <phymod/phymod.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>
#include <bcmpc/bcmpc_pm_drv_utils.h>

#include "bcmpc_pm_drv_pm4x10.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

/*! The number of lanes in PM4x10. */
#define PM4X10_LANES 4

#define PM4X10_MAX_AN_ABILITY 15
#define PM4X10_MAX_SPEED_ABILITY 50

#define PM4X10_PMD_FW_INFO_SIZE 128

/*! \brief PM4x10 lane map. */
typedef struct pm_lane_layout_s {
    int lane_map[PM4X10_LANES];
} pm_lane_layout_t;


/*******************************************************************************
 * PM driver private functions.
 */

/*******************************************************************************
 * PM driver function vector
 */

static int
pm4x10_pm_port_lane_map_validate(int unit, pm_oper_status_t *op_st,
                                 pm_info_t *pm_info, int lane_map)
{

    int valid = 0;
    int  idx;
    int lmap_lane[] = { 0x1, 0x2, 0x4, 0x8, 0x3, 0xc, 0xf};

    SHR_FUNC_ENTER(unit);

    /* Check port lane map. */
    for (idx = 0; idx < COUNTOF(lmap_lane); idx++) {
         if (lane_map == lmap_lane[idx]) {
             valid = 1;
              break;
         }
    }

    if (!valid) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_LANE_MASK_INVALID);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_speed_to_vco_get(int unit, pm_oper_status_t *op_st, pm_info_t *pm_info,
                        pm_speed_config_t *speed_cfg, pm_vco_t *vco)
{
    int idx;
    const pm4x10_force_speed_ability_table_entry_t *abil = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < COUNTOF(pm4x10_fs_ability_table); idx++) {
        abil = &pm4x10_fs_ability_table[idx];
        if (abil->speed == speed_cfg->speed &&
            abil->num_lanes == speed_cfg->num_lanes &&
            abil->fec == speed_cfg->fec) {
            if (vco) {
                *vco = abil->vco;
            }
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_PARAM);
exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_SPEED_INVALID);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_speed_config_validate(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_speed_config_t *speed_cfg)
{
    int idx;
    const pm4x10_force_speed_ability_table_entry_t *abil = NULL;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < COUNTOF(pm4x10_fs_ability_table); idx++) {
        abil = &(pm4x10_fs_ability_table)[idx];
        if ((abil->speed == speed_cfg->speed) &&
            (abil->num_lanes == speed_cfg->num_lanes)) {
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_SPEED_INVALID);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_pm_core_probe(int unit, const pm_access_t *pm_acc,
                     pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    uint32_t is_core_probed = 0;
    const phymod_core_access_t *core_acc = &(pm_acc->core_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_identify(core_acc, 0, &is_core_probed));

    if (!is_core_probed) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_pm_serdes_core_init(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           pm_port_add_info_t *add_info)
{
    int rv = 0;
    phymod_core_init_config_t core_config;
    phymod_core_status_t core_status;
    const phymod_core_access_t *core_acc = &(pm_acc->core_acc);
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_init_config_t_init(&core_config));
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_status_t_init(&core_status));
    core_status.pmd_active = 0;

    /* Probe serdes core */
    if(PM_PORT_ADD_F_INIT_PROBE_GET(add_info)) {
        rv = pm4x10_pm_core_probe(unit, pm_acc, op_st, pm_info);
    }
    if (SHR_FAILURE(rv)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                     "The PM serdes core is not probed"
                     "for physical port %"PRIu32".\n"), pm_acc->pport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Add lane map config */
    core_config.lane_map = add_info->init_config.lane_map;

    core_config.firmware_load_method = add_info->init_config.fw_load_method;
    core_config.firmware_loader = add_info->init_config.fw_loader;

    if (!PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_BYPASS_CRC_CHECK_SET(&core_config);
    }
    if (PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_config);
    }

    /* SW WAR to disable DCC on rev A0. */
    if (PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0_GET(add_info)) {
        PHYMOD_CORE_INIT_F_TOP_DEV_REV_ID_A0_SET(&core_config);
    }

    if (PM_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_config);
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_CLR(&core_config);
    }

    if (PM_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_CLR(&core_config);
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_config);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_core_init(core_acc, &core_config, &core_status));

    if (PM_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        pm_ha_info->firmware_downloaded = 1;
    }
    if (PM_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        pm_ha_info->is_pm_initialized = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_pm_init(int unit, const pm_access_t *pm_acc, pm_info_t *pm_info)
{
    pm_access_t pm_acc_copy;

    SHR_FUNC_ENTER(unit);
    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));

    /* check the pmd_fw info_ptr */
    if (pm_info->pm_data.pmd_fw_info == NULL) {
        /* Allocate a new entry */
        SHR_ALLOC(pm_info->pm_data.pmd_fw_info,
                  (sizeof(char) * PM4X10_PMD_FW_INFO_SIZE), "bcmpcPmdInfo");
        SHR_NULL_CHECK(pm_info->pm_data.pmd_fw_info, SHR_E_MEMORY);
        sal_memset(pm_info->pm_data.pmd_fw_info, 0,
                   (sizeof(char) * PM4X10_PMD_FW_INFO_SIZE));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_core_attach(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   pm_port_add_info_t *add_info)
{
    pm_access_t pm_acc_copy;

    SHR_FUNC_ENTER(unit);
    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_pm_init(unit, pm_acc, pm_info));

    if (pm_acc_copy.core_acc.access.pmd_info_ptr == NULL) {
        pm_acc_copy.core_acc.access.pmd_info_ptr = pm_info->pm_data.pmd_fw_info;
        pm_acc_copy.phy_acc.access.pmd_info_ptr = pm_info->pm_data.pmd_fw_info;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_pm_serdes_core_init(unit, &pm_acc_copy, op_st,
                                    pm_info, add_info));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_mode_update(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    uint32_t lane_mask;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    lane_mask = pm_acc->phy_acc.access.lane_mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->port_mode_set(&pmac_acc, pm_acc->pmac_blkport, 0, lane_mask));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_add(int unit, const pm_access_t *pm_acc,
                pm_oper_status_t *op_st, pm_info_t *pm_info,
                pm_port_add_info_t *add_info)
{
    int idx, num_lane;
    phymod_phy_init_config_t init_config;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (phymod_phy_init_config_t_init(&init_config));

    /* get tx taps setting from caller function */
    num_lane = add_info->speed_config.num_lanes;
    for (idx = 0; idx < num_lane; idx++) {
        sal_memcpy(&init_config.tx[idx],
            &add_info->init_config.tx_params[idx], sizeof(phymod_tx_t));
    }

    /*
     * Get polarity settings for the port.
     * This is different from PM8x50 due to different
     * underlying phymod code.
    */
    num_lane = 0;
    for (idx = 0; idx < PM4X10_LANES; idx++) {
        if (SHR_BITGET(&phy_acc->access.lane_mask, idx)) {
            if (SHR_BITGET(&add_info->init_config.polarity.rx_polarity, idx)) {
                SHR_BITSET(&init_config.polarity.rx_polarity, num_lane);
            }
            if (SHR_BITGET(&add_info->init_config.polarity.tx_polarity, idx)) {
                SHR_BITSET(&init_config.polarity.tx_polarity, num_lane);
            }
            num_lane++;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_init(phy_acc, &init_config));

    SHR_IF_ERR_VERBOSE_EXIT
       (pm4x10_port_mode_update(unit, pm_acc, op_st, pm_info));

    /*
     * In pm4x10, only one PLL is supported.
     * Use tvco_active_lane_bitmap to to track configured port bit map.
     */
    pm_ha_info->tvco_active_lane_bmap |= pm_acc->phy_acc.access.lane_mask;

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_loopback_set(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         pm_loopback_mode_t loopback_type, int enable)
{

    bcmpmac_drv_t *pmacd;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    switch (loopback_type) {
        case PM_PORT_LPBK_MAC:
            SHR_IF_ERR_VERBOSE_EXIT
                 (pmacd->lpbk_set(&pmac_acc, pm_acc->pmac_blkport, enable));
            break;
        case PM_PORT_LPBK_PHY_PCS:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_set(phy_acc, phymodLoopbackGlobal,
                                         enable));
            break;
        case PM_PORT_LPBK_PHY_PMD:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_set(phy_acc, phymodLoopbackGlobalPMD,
                                         enable));
            break;
        case PM_PORT_LPBK_REMOTE_PMD:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_set(phy_acc, phymodLoopbackRemotePMD,
                                         enable));
            break;
        case PM_PORT_LPBK_REMOTE_PCS:
        default:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_set(phy_acc, phymodLoopbackRemotePCS,
                                         enable));
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_loopback_get(int unit, const pm_access_t *pm_acc,
                         pm_oper_status_t *op_st, pm_info_t *pm_info,
                         pm_loopback_mode_t loopback_type, int *enable)
{
    uint32_t lpbk_en = 0;
    bcmpmac_drv_t *pmacd;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    switch (loopback_type) {
        case PM_PORT_LPBK_MAC:
            SHR_IF_ERR_VERBOSE_EXIT
                 (pmacd->lpbk_get(&pmac_acc, pm_acc->pmac_blkport, &lpbk_en));
            break;
        case PM_PORT_LPBK_PHY_PCS:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackGlobal,
                                         &lpbk_en));
            break;
        case PM_PORT_LPBK_PHY_PMD:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackGlobalPMD,
                                         &lpbk_en));
            break;
        case PM_PORT_LPBK_REMOTE_PMD:
            SHR_IF_ERR_VERBOSE_EXIT
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackRemotePMD,
                                         &lpbk_en));
            break;
        default:
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (phymod_phy_loopback_get(phy_acc, phymodLoopbackRemotePCS,
                                         &lpbk_en), PHYMOD_E_UNAVAIL);
            break;
    }
    *enable = lpbk_en;

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_enable_set(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       uint32_t flag,  uint32_t enable)
{
    uint32_t failover_en, mac_en = 0;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
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
            (pmacd->tx_enable_set(&pmac_acc, pm_acc->pmac_blkport, 1));
        SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->mac_reset_set(&pmac_acc, pm_acc->pmac_blkport,
                                  mac_en == 1 ? 0 : 1));
    }

    if (flag & PM_PORT_ENABLE_PHY) {
        status = PM_PHY_OPER_ERROR;
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

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_enable_get(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       uint32_t flag,  uint32_t *enable)
{
    phymod_phy_tx_lane_control_t tx_ctrl;
    phymod_phy_rx_lane_control_t rx_ctrl;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_lane_control_get(phy_acc, &tx_ctrl));
    PHY_IF_ERR_EXIT
        (phymod_phy_rx_lane_control_get(phy_acc, &rx_ctrl));

    if (tx_ctrl == phymodTxSquelchOn && rx_ctrl == phymodRxSquelchOn) {
        *enable = 0;
    } else {
        *enable = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_detach(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
            (pmacd->port_init(&pmac_acc, pm_acc->pmac_blkport, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_port_enable_set(unit, pm_acc, op_st, pm_info,
                                PM_PORT_ENABLE_PHY | PM_PORT_ENABLE_MAC, 0));

    pm_ha_info->tvco_active_lane_bmap &= ~(pm_acc->phy_acc.access.lane_mask);

    /* Power off PM if no port is configured on t. */
    if (!pm_ha_info->tvco_active_lane_bmap) {
        if (!pm_ha_info->is_master_pm) {
            /* Power off slave PM. */
            SHR_IF_ERR_VERBOSE_EXIT
                (pmacd->tsc_ctrl_set(&pmac_acc, pm_acc->pmac_blkport, 0));

            /* Update PM init State. */
            pm_ha_info->is_pm_initialized = 0;
            /* Update firmware download state. */
            pm_ha_info->firmware_downloaded = 0;
            if (pm_info->pm_data.pmd_fw_info != NULL) {
                SHR_FREE(pm_info->pm_data.pmd_fw_info);
                pm_info->pm_data.pmd_fw_info = NULL;
            }
        }
   }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_pm_destroy(int unit, pm_info_t *pm_info)
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

static int
pm4x10_port_link_status_get(int unit, const pm_access_t *pm_acc,
                            pm_oper_status_t *op_st, pm_info_t *pm_info,
                            uint32_t *link_status)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_link_status_get(phy_acc, link_status));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_encap_set(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      pm_encap_t encap)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->encap_set(&pmac_acc, pm_acc->pmac_blkport, encap));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_encap_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_port_pause_ctrl_set(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           pm_pause_ctrl_t *pause_ctrl)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pause_set(&pmac_acc, pm_acc->pmac_blkport, pause_ctrl));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_pause_ctrl_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_port_pause_addr_set(int unit, const pm_access_t *pm_acc,
                           pm_oper_status_t *op_st, pm_info_t *pm_info,
                           pm_mac_t pause_addr)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->pause_addr_set(&pmac_acc, pm_acc->pmac_blkport, pause_addr));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_pause_addr_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_port_frame_max_set(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          uint32_t max_frame_size)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->frame_max_set(&pmac_acc, pm_acc->pmac_blkport, max_frame_size));

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, PM_MAC_OPER_ERROR);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_frame_max_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_port_lag_failover_loopback_get(int unit, const pm_access_t *pm_acc,
                                      pm_oper_status_t *op_st, pm_info_t *pm_info,
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

/*
 * This function will set speed, num_lane, fec_type.
 * Lane_config and link_training will be set in different functions.
 */
static int
pm4x10_port_speed_config_set(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_speed_config_t *speed_config)
{

    phymod_phy_inf_config_t intf_cfg;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    uint32_t enable;

    SHR_FUNC_ENTER(unit);

    phymod_phy_inf_config_t_init(&intf_cfg);

    intf_cfg.data_rate = speed_config->speed;
    intf_cfg.ref_clock = pm_info->pm_data.ref_clk;
    pm_drv_port_interface_type_get(unit, pm_info, speed_config,
                                   &intf_cfg.interface_type);

    SHR_IF_ERR_VERBOSE_EXIT(
        phymod_phy_interface_config_set(phy_acc, 0, &intf_cfg));

    enable = (speed_config->fec == BCMPC_FEC_BASE_R) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_fec_enable_set(phy_acc, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_speed_config_get(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             pm_speed_config_t *speed_config)
{
    phymod_phy_inf_config_t intf_cfg;
    phymod_firmware_lane_config_t fw_cfg;
    const phymod_phy_access_t *phy_acc = (&(pm_acc->phy_acc));
    uint32_t status, enable;

    SHR_FUNC_ENTER(unit);


    phymod_phy_inf_config_t_init(&intf_cfg);
    phymod_firmware_lane_config_t_init(&fw_cfg);

    /* Get the the port medium type. */
    SHR_IF_ERR_VERBOSE_EXIT(
        phymod_phy_firmware_lane_config_get(phy_acc, &fw_cfg));

    /* Get the the port speed. */
    SHR_IF_ERR_VERBOSE_EXIT(
        phymod_phy_interface_config_get(phy_acc, 0, pm_info->pm_data.ref_clk,
                                        &intf_cfg));

    speed_config->speed = intf_cfg.data_rate;
    speed_config->num_lanes = shr_util_popcount(phy_acc->access.lane_mask);

    switch (fw_cfg.MediaType) {
        case phymodFirmwareMediaTypePcbTraceBackPlane:
            speed_config->medium_type = PM_PORT_MEDIUM_BACKPLANE;
            break;
        case phymodFirmwareMediaTypeCopperCable:
            speed_config->medium_type = PM_PORT_MEDIUM_COPPER;
            break;
        case phymodFirmwareMediaTypeOptics:
            speed_config->medium_type = PM_PORT_MEDIUM_FIBER;
            break;
        default:
            speed_config->medium_type = PM_PORT_MEDIUM_COPPER;
            break;
    }

    /* Get number of lanes of the port. */
    switch (phy_acc->access.lane_mask) {
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
            speed_config->num_lanes = 1;
            break;
        case 0x3:
        case 0xc:
            speed_config->num_lanes = 2;
            break;
        case 0xf:
            speed_config->num_lanes = 4;
            break;
    }

    /* Get link training status. */
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_cl72_get(phy_acc, &status));

    speed_config->link_training = status? 1 : 0;

    /* Get FEC type. */
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_fec_enable_get(phy_acc, &enable));

    speed_config->fec = enable? BCMPC_FEC_BASE_R : BCMPC_FEC_NONE;

exit:
    SHR_FUNC_EXIT();
}

static void
pm4x10_port_index_get(const pm_access_t *pm_acc, pm_info_t *pm_info, int *index)
{
    const phymod_phy_access_t *phy_access;
    int idx;

    phy_access = &(pm_acc->phy_acc);
    for (idx = 0; idx < PM4X10_LANES; idx++) {
        if ((phy_access->access.lane_mask >> idx) & 0x1) {
            break;
        }
    }
    *index = idx;
}

static int
pm4x10_pll_to_vco_get(int unit, phymod_ref_clk_t ref_clk, uint32_t pll,
                      pm_vco_t *vco)
{
    SHR_FUNC_ENTER(unit);

    if (ref_clk == phymodRefClk156Mhz) {
        switch (pll) {
            case phymod_TSCE_PLL_DIV66:
                *vco = PM_VCO_10P3125G;
                 break;
            case phymod_TSCE_PLL_DIV80:
                *vco = PM_VCO_12P5G;
                 break;
            default:
                 SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_pm_vco_rate_get(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       pm_vco_t *vco_rate)
{
    uint32_t pll_div;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    if (phy_acc->type == phymodDispatchTypeNull) {
        return SHR_E_NONE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_multiplier_get(phy_acc, &pll_div));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_pll_to_vco_get(unit, pm_info->pm_data.ref_clk,
                               pll_div, vco_rate));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_autoneg_ability_to_vco_get(int unit,
                                  pm_port_ability_t ability,
                                  pm_vco_t *vco)
{
    int idx;
    const pm4x10_autoneg_ability_table_entry_t *abil = NULL;

    SHR_FUNC_ENTER(unit);

    *vco = PM_VCO_INVALID;
    for (idx = 0; idx < COUNTOF(pm4x10_autoneg_ability_table); idx++) {
        abil = &pm4x10_autoneg_ability_table[idx];
        if (abil->speed == ability.speed &&
            abil->num_lanes == ability.num_lanes &&
            abil->fec == ability.fec_type &&
            abil->an_mode == ability.an_mode &&
            abil->medium == ability.medium) {
            *vco = abil->vco;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_FAIL);

exit:
    SHR_FUNC_EXIT();

}

static int
pm4x10_pm_to_phy_ability_get(int unit,
                             int num_abilities,
                             pm_port_ability_t *abilities,
                             phymod_autoneg_ability_t *an_ability)
{
    int idx;
    uint32_t an_tech_ability   = 0;
    uint32_t an_bam37_ability  = 0;
    phymod_cl37_sgmii_speed_t sgmii_speed = 0;

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT(phymod_autoneg_ability_t_init(an_ability));

    for (idx = 0; idx < num_abilities; idx++) {
        /* Pause */
        switch (abilities[idx].pause) {
            case PM_PORT_PAUSE_NONE:
                break;
            case PM_PORT_PAUSE_TX:
                PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability);
                break;
            case PM_PORT_PAUSE_RX:
                PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability);
                PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability);
                break;
            case PM_PORT_PAUSE_SYMM:
                PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        switch (abilities[idx].an_mode) {
            case PM_PORT_AUTONEG_MODE_CL73:
                if (abilities[idx].num_lanes == 4) {
                    if (abilities[idx].speed == 40000) {
                        if (abilities[idx].medium == PM_PORT_MEDIUM_BACKPLANE) {
                            PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
                        } else {
                            PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
                        }
                        if (abilities[idx].fec_type == PM_PORT_FEC_BASE_R) {
                            PHYMOD_AN_FEC_CL74_SET(an_ability->an_fec);
                        }
                    } else if (abilities[idx].speed == 10000) {
                        PHYMOD_AN_CAP_10G_KX4_SET(an_tech_ability);
                    }
                } else if (abilities[idx].num_lanes == 1) {
                    if (abilities[idx].speed == 10000) {
                        PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
                        if (abilities[idx].fec_type == PM_PORT_FEC_BASE_R) {
                            PHYMOD_AN_FEC_CL74_SET(an_ability->an_fec);
                        } else {
                            PHYMOD_AN_FEC_OFF_SET(an_ability->an_fec);
                        }
                    } else if (abilities[idx].speed == 1000) {
                        PHYMOD_AN_CAP_1G_KX_SET(an_tech_ability);
                    }
                }
                break;
            case PM_PORT_AUTONEG_MODE_CL37_BAM:
                if (abilities[idx].num_lanes == 4) {
                    PHYMOD_BAM_CL37_CAP_10G_CX4_SET(an_bam37_ability);
                } else if (abilities[idx].num_lanes == 2) {
                    PHYMOD_BAM_CL37_CAP_10G_X2_CX4_SET(an_bam37_ability);
                    PHYMOD_BAM_CL37_CAP_10G_DXGXS_SET(an_bam37_ability);
                } else if (abilities[idx].num_lanes == 1) {
                    PHYMOD_BAM_CL37_CAP_2P5G_SET(an_bam37_ability);
                }
                break;
            case PM_PORT_AUTONEG_MODE_SGMII:
                if (abilities[idx].speed == 1000) {
                    sgmii_speed = phymod_CL37_SGMII_1000M;
                } else if (abilities[idx].speed == 100) {
                    sgmii_speed = phymod_CL37_SGMII_100M;
                } else if (abilities[idx].speed == 10) {
                    sgmii_speed = phymod_CL37_SGMII_10M;
                }
                break;
            default:
                /* PM_PORT_AUTONEG_MODE_CL37 */
                break;
        }

    }
    if (!PHYMOD_AN_FEC_CL74_GET(an_ability->an_fec)) {
        PHYMOD_AN_FEC_OFF_SET(an_ability->an_fec);
    }
    an_ability->an_cap = an_tech_ability;
    an_ability->cl37bam_cap = an_bam37_ability;
    an_ability->sgmii_speed = sgmii_speed;
    an_ability->an_cl72 = 1;

exit:
    SHR_FUNC_EXIT();

}

static int
pm4x10_phy_to_pm_ability_get(int unit,
                             phymod_autoneg_ability_t *an_ability,
                             uint32_t max_num_abilities,
                             uint32_t *num_abilities,
                             pm_port_ability_t *abilities)

{
    uint32_t index = 0;
    pm_port_pause_t pause = PM_PORT_PAUSE_NONE;
    pm_port_ability_t temp_abilities[PM4X10_MAX_AN_ABILITY];
    SHR_FUNC_ENTER(unit);

    *num_abilities = 0;
    sal_memset(abilities, 0, max_num_abilities * sizeof(pm_port_ability_t));
    sal_memset(&temp_abilities, 0,
               PM4X10_MAX_AN_ABILITY * sizeof(pm_port_ability_t));

    if (an_ability->capabilities & PHYMOD_AN_CAP_ASYM_PAUSE) {
        if (an_ability->capabilities & PHYMOD_AN_CAP_SYMM_PAUSE) {
            pause = PM_PORT_PAUSE_RX;
        } else {
            pause = PM_PORT_PAUSE_TX;
        }
    } else if (an_ability->capabilities & PHYMOD_AN_CAP_SYMM_PAUSE) {
        pause = PM_PORT_PAUSE_SYMM;
    }

    if (PHYMOD_AN_CAP_40G_KR4_GET(an_ability->an_cap)) {
        temp_abilities[index].speed = 40000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL73;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 4;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        if (PHYMOD_AN_FEC_OFF_GET(an_ability->an_fec)) {
            temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        } else {
            temp_abilities[index].fec_type = PM_PORT_FEC_BASE_R;
        }
        index++;
    }

    if (PHYMOD_AN_CAP_40G_CR4_GET(an_ability->an_cap)) {
        temp_abilities[index].speed = 40000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL73;
        temp_abilities[index].medium = PM_PORT_MEDIUM_COPPER;
        temp_abilities[index].num_lanes = 4;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        if (PHYMOD_AN_FEC_OFF_GET(an_ability->an_fec)) {
            temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        } else {
            temp_abilities[index].fec_type = PM_PORT_FEC_BASE_R;
        }
        index++;
    }

    if (PHYMOD_AN_CAP_10G_KR_GET(an_ability->an_cap)) {
        temp_abilities[index].speed = 10000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL73;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 1;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        if (PHYMOD_AN_FEC_OFF_GET(an_ability->an_fec)) {
            temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        } else {
            temp_abilities[index].fec_type = PM_PORT_FEC_BASE_R;
        }
        index++;
    }

    if (PHYMOD_AN_CAP_10G_KX4_GET(an_ability->an_cap)) {
        temp_abilities[index].speed = 10000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL73;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 4;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        index++;
    }

    if (PHYMOD_AN_CAP_1G_KX_GET(an_ability->an_cap)) {
        temp_abilities[index].speed = 1000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL73;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 1;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        index++;
    }

    if (PHYMOD_BAM_CL37_CAP_10G_X2_CX4_GET(an_ability->cl37bam_cap) ||
        PHYMOD_BAM_CL37_CAP_10G_DXGXS_GET(an_ability->cl37bam_cap)) {
        temp_abilities[index].speed = 10000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL37_BAM;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 2;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        index++;
    }

    if (PHYMOD_BAM_CL37_CAP_10G_CX4_GET(an_ability->cl37bam_cap)) {
        temp_abilities[index].speed = 10000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL37_BAM;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 4;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        index++;
    }

    if (PHYMOD_BAM_CL37_CAP_2P5G_GET(an_ability->cl37bam_cap)) {
        temp_abilities[index].speed = 2500;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL37_BAM;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 1;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        index++;
    }

    if (PHYMOD_AN_CAP_CL37_GET(an_ability)) {
        temp_abilities[index].speed = 1000;
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_CL37;
        temp_abilities[index].medium = PM_PORT_MEDIUM_FIBER;
        temp_abilities[index].num_lanes = 1;
        temp_abilities[index].pause = pause;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        index++;
    }

    if (PHYMOD_AN_CAP_SGMII_GET(an_ability)){
        temp_abilities[index].an_mode = PM_PORT_AUTONEG_MODE_SGMII;
        temp_abilities[index].medium = PM_PORT_MEDIUM_BACKPLANE;
        temp_abilities[index].num_lanes = 1;
        temp_abilities[index].pause = pause;
        temp_abilities[index].fec_type = PM_PORT_FEC_NONE;
        temp_abilities[index].channel = PM_PORT_CHANNEL_LONG;
        switch (an_ability->sgmii_speed){
            case phymod_CL37_SGMII_10M:
                temp_abilities[index].speed = 10;
                index++;
                break;
            case phymod_CL37_SGMII_100M:
                temp_abilities[index].speed = 100;
                index++;
                break;
            case phymod_CL37_SGMII_1000M:
                temp_abilities[index].speed = 1000;
                index++;
                break;
            default:
                break;
        }
    }

    if (index > max_num_abilities) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    } else {
        sal_memcpy(abilities,
                   temp_abilities,
                   index * sizeof(pm_port_ability_t));
        *num_abilities = index;
    }

exit:
    SHR_FUNC_EXIT();

}

static int
pm4x10_port_ability_advert_set(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info, int num_abilities,
                               pm_port_ability_t *ability)
{
    int idx, port_index, port_num_lanes, rv;
    bool cl73_req = 0, cl37_req = 0, sgmii_req = 0, cl37bam_req = 0;
    pm_port_fec_t cl73_fec_req = PM_PORT_FEC_COUNT;
    pm_vco_t vco, current_vco;
    pm_port_medium_t current_media_type;
    pm_port_pause_t pause = PM_PORT_PAUSE_COUNT;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    phymod_autoneg_ability_t    an_ability;
    const phymod_phy_access_t *phy_access = &(pm_acc->phy_acc);
    phymod_firmware_lane_config_t fw_lane_config;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;

    SHR_FUNC_ENTER(unit);

    current_media_type = phymodFirmwareMediaTypeCount;
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_firmware_lane_config_get(phy_access, &fw_lane_config));

    if (fw_lane_config.MediaType == phymodFirmwareMediaTypePcbTraceBackPlane) {
        current_media_type = PM_PORT_MEDIUM_BACKPLANE;
    } else if (fw_lane_config.MediaType == phymodFirmwareMediaTypeCopperCable) {
        current_media_type = PM_PORT_MEDIUM_COPPER;
    } else if (fw_lane_config.MediaType == phymodFirmwareMediaTypeOptics) {
        current_media_type = PM_PORT_MEDIUM_FIBER;
    }

    SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask),
                       port_num_lanes,
                       0,
                       PM4X10_LANES);

    SHR_IF_ERR_EXIT
        (pm4x10_pm_vco_rate_get(unit, pm_acc, op_st, pm_info, &current_vco));

    /* 1. Validate abilities. */
    for (idx = 0; idx < num_abilities; idx++) {
        /* 1.1 Ability is supported by PM4x10. */
        rv = pm4x10_autoneg_ability_to_vco_get(unit,
                                               ability[idx],
                                               &vco);
        if (SHR_FAILURE(rv)) {
            status = PM_ABILITY_PORT_CFG_INVALID;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        /* 1.2 Media type is the same as current HW setting. */
        if (current_media_type != ability[idx].medium) {
            status = PM_ABILITY_MEDIA_TYPE_CONFLICT;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /* 1.3 Number of lanes is the same as port's configuration. */
        if ((uint32_t)port_num_lanes != ability[idx].num_lanes) {
            status = PM_ABILITY_PORT_CFG_INVALID;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /* 1.4 Pause requests are identical among abilities. */
        if (pause == PM_PORT_PAUSE_COUNT) {
            pause = ability[idx].pause;
        } else if (pause !=  ability[idx].pause) {
            status = PM_ABILITY_PAUSE_CONFLICT;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        /*!
         * 1.5 For non-signle port mode, VCO request can be satisfied by
         * current VCO setting.
         */
        if ((port_num_lanes != PM4X10_LANES) && (vco != current_vco)) {
            status = PM_VCO_UNAVAIL;
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

        if (ability[idx].an_mode == PM_PORT_AUTONEG_MODE_CL73) {
            cl73_req = 1;
            /* 1.6 CL73 FEC requests are identical. */
            if (cl73_fec_req == PM_PORT_FEC_COUNT) {
                cl73_fec_req = ability[idx].fec_type;
            } else if (cl73_fec_req != ability[idx].fec_type) {
                status = PM_ABILITY_FEC_CONFLICT;
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        } else if (ability[idx].an_mode == PM_PORT_AUTONEG_MODE_SGMII) {
            sgmii_req = 1;
        } else if (ability[idx].an_mode == PM_PORT_AUTONEG_MODE_CL37_BAM) {
            cl37bam_req = 1;
        } else if (ability[idx].an_mode == PM_PORT_AUTONEG_MODE_CL37) {
            cl37_req = 1;
        }
    }

    /* 1.7 AN mode combination is supported. */
    if (cl73_req && (cl37_req || cl37bam_req || sgmii_req)) {
        status = PM_ABILITY_AN_CONFLICT_CFG;
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* 2. Configure hardware. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_pm_to_phy_ability_get(unit,
                                      num_abilities,
                                      ability,
                                      &an_ability));

    PHY_IF_ERR_EXIT
        (phymod_phy_autoneg_ability_set(phy_access, &an_ability));

    /* 3. Update an_mode. */
    pm4x10_port_index_get(pm_acc, pm_info, &port_index);
    if (cl73_req) {
        pm_ha_info->an_mode[port_index] = PM_PORT_AUTONEG_MODE_CL73;
    } else if (sgmii_req) {
        pm_ha_info->an_mode[port_index] = PM_PORT_AUTONEG_MODE_SGMII;
    } else if (cl37bam_req) {
        pm_ha_info->an_mode[port_index] = PM_PORT_AUTONEG_MODE_CL37_BAM;
    } else if (cl37_req) {
        pm_ha_info->an_mode[port_index] = PM_PORT_AUTONEG_MODE_CL37;
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_ability_advert_get(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info, uint32_t max_num_abilities,
                               uint32_t *actual_num_abilities,
                               pm_port_ability_t *ability)
{
    phymod_autoneg_ability_t         an_ability;
    const phymod_phy_access_t *phy_access = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_autoneg_ability_get(phy_access, &an_ability));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_phy_to_pm_ability_get(unit,
                                      &an_ability,
                                      max_num_abilities,
                                      actual_num_abilities,
                                      ability));
exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_ability_remote_get(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info, uint32_t max_num_abilities,
                               uint32_t *actual_num_abilities,
                               pm_port_ability_t *ability)
{
    phymod_autoneg_ability_t         an_ability;
    const phymod_phy_access_t *phy_access = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_autoneg_remote_ability_get(phy_access, &an_ability));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_phy_to_pm_ability_get(unit,
                                      &an_ability,
                                      max_num_abilities,
                                      actual_num_abilities,
                                      ability));
exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_autoneg_set(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        phymod_autoneg_control_t *an)
{
    phymod_autoneg_control_t an_control;
    const phymod_phy_access_t *phy_access;
    pm_port_autoneg_mode_t an_mode;
    int port_num_lanes, port_index;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;
    pm_access_t pm_acc_copy;

    SHR_FUNC_ENTER(unit);

    an_control = *an;

    sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));
    phy_access = &(pm_acc->phy_acc);
    SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask),
                       port_num_lanes,
                       0,
                       PM4X10_LANES);
    an_control.num_lane_adv = port_num_lanes;

    pm4x10_port_index_get(pm_acc, pm_info, &port_index);
    an_mode = pm_ha_info->an_mode[port_index];
    if (an_mode == PM_PORT_AUTONEG_MODE_CL73) {
        an_control.an_mode = phymod_AN_MODE_CL73;
    } else if (an_mode == PM_PORT_AUTONEG_MODE_CL37_BAM) {
        an_control.an_mode = phymod_AN_MODE_CL37BAM;
    } else if (an_mode == PM_PORT_AUTONEG_MODE_CL37) {
        an_control.an_mode = phymod_AN_MODE_CL37;
    } else if (an_mode == PM_PORT_AUTONEG_MODE_SGMII) {
        an_control.an_mode = phymod_AN_MODE_SGMII;
    } else {
        if (!an_control.enable) {
            an_control.an_mode = phymod_AN_MODE_CL73;
        } else {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    if (an_control.enable) {
        /* Update port mode in MAC. */
        pm_acc_copy.phy_acc.access.lane_mask = 1 << port_index;
        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_mode_update(unit, &pm_acc_copy, op_st, pm_info));
    }

    /* Enable AN in Serdes. */
    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_autoneg_set(phy_access,
                                &an_control));
exit:
    SHR_FUNC_EXIT();

}

static int
pm4x10_port_autoneg_get(int unit, const pm_access_t *pm_acc,
                        pm_oper_status_t *op_st, pm_info_t *pm_info,
                        phymod_autoneg_control_t *an)
{
    const phymod_phy_access_t *phy_access;
    uint32_t an_done;

    SHR_FUNC_ENTER(unit);

    phy_access = &(pm_acc->phy_acc);

    SHR_IF_ERR_VERBOSE_EXIT(phymod_phy_autoneg_get(phy_access, an, &an_done));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_autoneg_status_get(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st,
                               pm_info_t *pm_info,
                               phymod_autoneg_status_t *an_status)
{
    const phymod_phy_access_t *phy_access = &(pm_acc->phy_acc);
    uint32_t an_done;
    int port_num_lanes = 0;
    phymod_autoneg_control_t an;

    SHR_FUNC_ENTER(unit);

    SHR_BITCOUNT_RANGE(&(phy_access->access.lane_mask),
                       port_num_lanes,
                       0,
                       PM4X10_LANES);
    SHR_IF_ERR_VERBOSE_EXIT(phymod_phy_autoneg_get(phy_access, &an, &an_done));
    an_status->enabled = an.enable;
    an_status->locked = an_done;

    /* PM4x10 auto-negotiation does not dynamically change number of lanes. */
    if (an.enable && an_done) {
        an_status->resolved_num_lane = port_num_lanes;
    } else {
        an_status->resolved_num_lane = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_failover_enable_set(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
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

static int
pm4x10_port_failover_enable_get(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st, pm_info_t *pm_info,
                                uint32_t *enable)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    pmacd = pm_info->pm_data.pmacd;

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->failover_enable_get(&pmac_acc, pm_acc->pmac_blkport, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_link_training_set(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
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
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_link_training_get(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t *enable)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_cl72_get(phy_acc, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_tx_set(int unit, const pm_access_t *pm_acc,
                   pm_oper_status_t *op_st, pm_info_t *pm_info,
                   phymod_tx_t *tx_cfg)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    PHY_IF_ERR_EXIT
        (phymod_phy_tx_set(phy_acc, tx_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_tx_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_port_pmd_lane_cfg_set(int unit, const pm_access_t *pm_acc,
                             pm_oper_status_t *op_st, pm_info_t *pm_info,
                             phymod_firmware_lane_config_t *fw_lane_cfg)
{
    phymod_phy_access_t phy_acc;
    phymod_firmware_lane_config_t firmware_lane_cfg;

    SHR_FUNC_ENTER(unit);
    phymod_firmware_lane_config_t_init(&firmware_lane_cfg);
    PHYMOD_MEMCPY(&phy_acc, &(pm_acc->phy_acc), sizeof(phy_acc));
    PHYMOD_MEMCPY(&firmware_lane_cfg, fw_lane_cfg, sizeof(firmware_lane_cfg));

    PHY_IF_ERR_EXIT
        (phymod_phy_firmware_lane_config_set(&phy_acc, firmware_lane_cfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_pmd_lane_cfg_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_phy_status_get(int unit, const pm_access_t *pm_acc,
                      pm_oper_status_t *op_st, pm_info_t *pm_info,
                      pm_phy_status_t *phy_st)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    int rv = SHR_E_FAIL;

    SHR_FUNC_ENTER(unit);

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
    rv = phymod_phy_rx_get(phy_acc, &phy_st->rx_afe);
    if ((rv == PHYMOD_E_UNAVAIL) || (rv == PHYMOD_E_NONE)) {
        rv = SHR_E_NONE;
    } else {
        rv = SHR_E_FAIL;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* rx signal detect. */
    rv = phymod_phy_rx_signal_detect_get(phy_acc, &phy_st->rx_signal_detect);
    if ((rv == PHYMOD_E_UNAVAIL) || (rv == PHYMOD_E_NONE)) {
        rv = SHR_E_NONE;
    } else {
        rv = SHR_E_FAIL;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_phy_control_set(int unit, const pm_access_t *pm_acc,
                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                       uint32_t flags, pm_phy_control_t *pctrl)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    int rv = SHR_E_FAIL;

    SHR_FUNC_ENTER(unit);

    if (flags & PM_PHY_CTRL_F_TX_SQUELCH) {
        PHY_IF_ERR_EXIT
            (phymod_phy_tx_lane_control_set(phy_acc, pctrl->tx_control));
    }
    if (flags & PM_PHY_CTRL_F_RX_SQUELCH) {
        PHY_IF_ERR_EXIT
            (phymod_phy_rx_lane_control_set(phy_acc, pctrl->rx_control));
    }
    if (flags & PM_PHY_CTRL_F_RX_ADPT_RESUME) {
        rv = phymod_phy_rx_adaptation_resume(phy_acc);
        if ((rv == PHYMOD_E_UNAVAIL) || (rv == PHYMOD_E_NONE)) {
            rv = SHR_E_NONE;
        } else {
            rv = SHR_E_FAIL;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (flags & (PM_PHY_CTRL_F_RX_AFE_VGA |
                 PM_PHY_CTRL_F_RX_AFE_PK_FLT |
                 PM_PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT |
                 PM_PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT |
                 PM_PHY_CTRL_F_RX_AFE_DFE_TAP)) {
        rv = phymod_phy_rx_set(phy_acc, &(pctrl->rx_afe));
        if ((rv == PHYMOD_E_UNAVAIL) || (rv == PHYMOD_E_NONE)) {
            rv = SHR_E_NONE;
        } else {
            rv = SHR_E_FAIL;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (flags & PM_PHY_CTRL_F_PAM4_TX_PATTERN) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PM4x10 does not support PAM4 TX PATTERN\n")));
    }
    if (flags & PM_PHY_CTRL_F_PAM4_TX_PRECODER) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PM4x10 does not support PAM4 TX PRECODER\n")));
    }
    if (flags & PM_PHY_CTRL_F_ECC_INTR_ENABLE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PM4x10 does not support ECC interrupt\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_ability_local_get(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info,
                              uint32_t max_num_abilities,
                              uint32_t *actual_num_abilities,
                              pm_port_ability_t *abilities)
{
    uint8_t idx = 0;
    int port_num_lanes = 0;
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);
    const pm4x10_force_speed_ability_table_entry_t *abil = NULL;
    const pm4x10_autoneg_ability_table_entry_t *an_abil = NULL;

    SHR_BITCOUNT_RANGE(&(phy_acc->access.lane_mask), port_num_lanes,
                       0, PM4X10_LANES);

    for (idx = 0; idx < COUNTOF(pm4x10_fs_ability_table); idx++) {
        abil = &pm4x10_fs_ability_table[idx];
        if (abil->num_lanes == (uint32_t)port_num_lanes) {
            if (*actual_num_abilities < max_num_abilities) {
                abilities[*actual_num_abilities].speed = abil->speed;
                abilities[*actual_num_abilities].num_lanes = abil->num_lanes;
                abilities[*actual_num_abilities].fec_type = abil->fec;
                abilities[*actual_num_abilities].medium = abil->medium;
                abilities[*actual_num_abilities].pause = PM_PORT_PAUSE_ALL;
                abilities[*actual_num_abilities].channel = PM_PORT_CHANNEL_ALL;
                abilities[*actual_num_abilities].an_mode =
                                                      PM_PORT_AUTONEG_MODE_NONE;
                (*actual_num_abilities) += 1;
            } else {
                break;
            }
        }
    }

    for (idx = 0; idx < COUNTOF(pm4x10_autoneg_ability_table); idx++) {
        an_abil = &pm4x10_autoneg_ability_table[idx];
        if (an_abil->num_lanes == (uint32_t)port_num_lanes) {
            if (*actual_num_abilities < max_num_abilities) {
                abilities[*actual_num_abilities].speed = an_abil->speed;
                abilities[*actual_num_abilities].num_lanes = an_abil->num_lanes;
                abilities[*actual_num_abilities].fec_type = an_abil->fec;
                abilities[*actual_num_abilities].medium = an_abil->medium;
                abilities[*actual_num_abilities].pause = PM_PORT_PAUSE_ALL;
                abilities[*actual_num_abilities].channel = PM_PORT_CHANNEL_ALL;
                abilities[*actual_num_abilities].an_mode = an_abil->an_mode;
                (*actual_num_abilities) += 1;
            } else {
                break;
            }
        }
    }

    return SHR_E_NONE;
}

static int
pm4x10_port_abilities_get(int unit, const pm_access_t *pm_acc,
                          pm_oper_status_t *op_st, pm_info_t *pm_info,
                          pm_port_ability_type_t ability_type,
                          uint32_t *actual_num_abilities,
                          pm_port_ability_t *pabilities)
{
    SHR_FUNC_ENTER(unit);

    if (ability_type == PM_PORT_ABILITY_LOCAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_ability_local_get(unit, pm_acc, op_st, pm_info,
                                           PM4X10_MAX_SPEED_ABILITY,
                                           actual_num_abilities, pabilities));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_ability_advert_get(unit, pm_acc, op_st, pm_info,
                                           PM4X10_MAX_AN_ABILITY,
                                           actual_num_abilities, pabilities));
   }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * PM4x10 link up event:
 * 1. AN ports: Update resolved port mode.
 *              Confiugre resolved pause settings.
 */
static int
pm4x10_port_phy_link_up_event(int unit, const pm_access_t *pm_acc,
                              pm_oper_status_t *op_st, pm_info_t *pm_info)
{
    pm_access_t pm_acc_copy;
    int port_index, rx_pause, tx_pause;
    phymod_autoneg_status_t an_status;
    uint32_t idx, num_advert, num_remote;
    pm_port_ability_t advert_ability[PM4X10_MAX_AN_ABILITY];
    pm_port_ability_t remote_ability[PM4X10_MAX_AN_ABILITY];
    pm_port_pause_t pause_local, pause_remote;
    pm_pause_ctrl_t pause_ctrl;

    SHR_FUNC_ENTER(unit);

    pm4x10_port_index_get(pm_acc, pm_info, &port_index);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_port_autoneg_status_get(unit, pm_acc, op_st, pm_info,
                                        &an_status));
    if ((an_status.enabled && an_status.locked)) {
        /* 1.1 AN ports: Port mode update. */
        sal_memcpy(&pm_acc_copy, pm_acc, sizeof(pm_acc_copy));
        pm_acc_copy.phy_acc.access.lane_mask = 0;
        /* Update lane mask based on resovled number of lanes. */
        for (idx = 0; idx < an_status.resolved_num_lane; idx++) {
            pm_acc_copy.phy_acc.access.lane_mask |= 0x1 << (port_index + idx);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_mode_update(unit, &pm_acc_copy, op_st, pm_info));

        /* 1.2 AN ports: Pause update. */
        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_ability_advert_get(unit, pm_acc, op_st, pm_info,
                                            PM4X10_MAX_AN_ABILITY,
                                            &num_advert, advert_ability));
        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_ability_remote_get(unit, pm_acc, op_st, pm_info,
                                            PM4X10_MAX_AN_ABILITY,
                                            &num_remote, remote_ability));
        if (num_advert && num_remote) {
            pause_local = advert_ability[0].pause;
            pause_remote = remote_ability[0].pause;
            if (
                ((pause_local == PM_PORT_PAUSE_RX) &&
                 (pause_remote == PM_PORT_PAUSE_TX)) ||
                ((pause_local == PM_PORT_PAUSE_RX ||
                  pause_local == PM_PORT_PAUSE_SYMM) &&
                 (pause_remote == PM_PORT_PAUSE_RX ||
                  pause_remote == PM_PORT_PAUSE_SYMM))
               ) {
                rx_pause = 1;
            } else {
                rx_pause = 0;
            }
            if (
                ((pause_local == PM_PORT_PAUSE_RX ||
                  pause_local == PM_PORT_PAUSE_SYMM) &&
                 (pause_remote == PM_PORT_PAUSE_RX ||
                  pause_remote == PM_PORT_PAUSE_SYMM)) ||
                ((pause_local == PM_PORT_PAUSE_TX) &&
                 (pause_remote == PM_PORT_PAUSE_RX))
               ) {
                tx_pause = 1;
            } else {
                tx_pause = 0;
            }
        } else {
            tx_pause = 0;
            rx_pause = 0;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (pm4x10_port_pause_ctrl_get(unit, pm_acc, op_st,
                                        pm_info, &pause_ctrl));
        if ((pause_ctrl.rx_enable != rx_pause) ||
            (pause_ctrl.tx_enable != tx_pause)) {
            pause_ctrl.rx_enable = rx_pause;
            pause_ctrl.tx_enable = tx_pause;
            SHR_IF_ERR_VERBOSE_EXIT
                (pm4x10_port_pause_ctrl_set(unit, pm_acc, op_st, pm_info,
                                            &pause_ctrl));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_phy_link_down_event(int unit, const pm_access_t *pm_acc,
                                pm_oper_status_t *op_st,
                                pm_info_t *pm_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

static int
pm4x10_pm_port_config_validate(int unit, const pm_access_t *pm_acc,
                               pm_oper_status_t *op_st, pm_info_t *pm_info,
                               pm_speed_config_t *spd_cfg, int is_first)
{
    uint32_t vco_pll_div = 0;
    pm_vco_t cur_vco = PM_VCO_INVALID, req_vco;
    phymod_phy_access_t phy_copy;
    int rv = SHR_E_NONE;
    pm_oper_status_type_t status = PM_OPER_SUCCESS;
    pm_ha_info_t *pm_ha_info = pm_info->pm_data.pm_ha_info;

    SHR_FUNC_ENTER(unit);

    sal_memcpy(&phy_copy, &(pm_acc->phy_acc), sizeof(phy_copy));

    /* First, check lane_map. */
    rv = pm4x10_pm_port_lane_map_validate(unit, op_st, pm_info,
                                          pm_acc->phy_acc.access.lane_mask);
    if (SHR_FAILURE(rv)) {
        status = op_st->status;
        SHR_ERR_EXIT(rv);
    }

    /* Second, validate force speed ability. */
    rv = pm4x10_speed_config_validate(unit, pm_acc, op_st, pm_info, spd_cfg);
    if (SHR_FAILURE(rv)) {
        status = op_st->status;
        SHR_ERR_EXIT(rv);
    }

    /* Third, check requested VCO is valid on the PM. */
    /* If the port is the first port on the PM, just return. */
    if (is_first) {
        return SHR_E_NONE;
    }

    /* if the port is the only port on the PM, then just return */
    if (pm_ha_info->tvco_active_lane_bmap == pm_acc->phy_acc.access.lane_mask) {
        return SHR_E_NONE;
    }

    /* Use the first lane. */
    phy_copy.access.lane_mask = 0x1;

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_pll_multiplier_get(&phy_copy, &vco_pll_div));
    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_pll_to_vco_get(unit, 0, vco_pll_div, &cur_vco));

    /* Compare requested VCO with current VCO */
    rv = pm4x10_speed_to_vco_get(unit, op_st, pm_info, spd_cfg, &req_vco);
    if (SHR_FAILURE(rv)) {
        status = op_st->status;
        SHR_ERR_EXIT(rv);
    }

    if (req_vco != PM_VCO_INVALID) {
        if (req_vco != cur_vco) {
            /* req_vco is other than current VCO */
            status = PM_VCO_UNAVAIL;
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Requested VCO %"PRIu32" for speed "
                                  "%"PRIu32" cannot be configured on current "
                                  "settings VCO %"PRIu32"\n"),
                       req_vco, spd_cfg->speed, cur_vco));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        /* req_vco is invalid */
        status = PM_VCO_UNAVAIL;
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "The reqested VCO is invalid on "
                             "logical port.\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st != NULL)) {
        pm_port_oper_status_set(op_st, status);
    }
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_link_training_status_get(int unit, const pm_access_t *pm_acc,
                                     pm_oper_status_t *op_st,
                                     pm_info_t *pm_info, uint32_t *locked)
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

static int
pm4x10_pm_vco_calculate(int unit, pm_oper_status_t *op_st,
                        pm_info_t *pm_info, pm_vco_setting_t *vco_select)
{
    pm_vco_t cur_vco = PM_VCO_INVALID, req_vco = PM_VCO_INVALID;
    pm_vco_t vco;
    int i;

    SHR_FUNC_ENTER(unit);

    if (vco_select == NULL){
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < vco_select->num_speeds; i++) {
        vco = PM_VCO_INVALID;
        pm4x10_speed_to_vco_get(unit, op_st, pm_info,
                                &(vco_select->speed_config_list[i]), &vco);

        if ((cur_vco == PM_VCO_INVALID) && (vco != PM_VCO_INVALID)) {
            cur_vco = vco;
        }

        if (vco != PM_VCO_INVALID) {
            if (vco != cur_vco) {
                /* req_vco is other than current VCO */
                LOG_INFO(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Requested VCO %"PRIu32" for speed "
                                    "%"PRIu32" cannot be configured on current "
                                    "settings VCO %"PRIu32"\n"), req_vco,
                                     vco_select->speed_config_list[i].speed,
                                    cur_vco));
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
        } else {
            /* req_vco is invalid */
            LOG_INFO(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "The reqested VCO is invalid on "
                                 "logical port.\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }

    }


exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_default_lane_config_get(int unit, const pm_access_t *pm_acc,
                                    pm_oper_status_t *op_st, pm_info_t *pm_info,
                                    pm_speed_config_t *speed_config,
                                    phymod_firmware_lane_config_t *lane_config)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm4x10_port_pmd_lane_cfg_get(unit, pm_acc, op_st,
                                      pm_info, lane_config));

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_port_default_tx_taps_config_get(int unit, const pm_access_t *pm_acc,
                                       pm_oper_status_t *op_st, pm_info_t *pm_info,
                                       pm_speed_config_t *speed_config,
                                       phymod_tx_t *tx_taps_config)
{
    const phymod_phy_access_t *phy_acc = &(pm_acc->phy_acc);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (phymod_phy_media_type_tx_get(phy_acc, phymodMediaTypeChipToChip,
                                      tx_taps_config));
exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_phy_polarity_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_core_lane_map_get(int unit, const pm_access_t *pm_acc,
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

static int
pm4x10_port_tx_timestamp_info_get(int unit, const pm_access_t *pm_acc,
                                  pm_oper_status_t *op_st, pm_info_t *pm_info,
                                  pm_tx_timestamp_info_t *timestamp_info)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    bcmpmac_tx_timestamp_info_t timestamp;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    sal_memset(&timestamp, 0, sizeof(timestamp));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->tx_timestamp_info_get(&pmac_acc, pm_acc->pmac_blkport,
                                        &timestamp));
    timestamp_info->timestamp_lo = timestamp.timestamp_lo;
    timestamp_info->timestamp_hi = timestamp.timestamp_hi;
    timestamp_info->sequence_id = timestamp.sequence_id;

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_egress_timestamp_mode_get(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 pm_egr_timestamp_mode_t *timestamp_mode)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    bcmpmac_egr_timestamp_mode_t egr_timestamp_mode;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->egress_timestamp_mode_get(&pmac_acc, pm_acc->pmac_blkport,
                                          &egr_timestamp_mode));
    *timestamp_mode = egr_timestamp_mode;

exit:
    SHR_FUNC_EXIT();
}

static int
pm4x10_egress_timestamp_mode_set(int unit, const pm_access_t *pm_acc,
                                 pm_oper_status_t *op_st, pm_info_t *pm_info,
                                 pm_egr_timestamp_mode_t timestamp_mode)
{
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t pmac_acc;
    bcmpmac_egr_timestamp_mode_t egr_timestamp_mode;

    SHR_FUNC_ENTER(unit);

    pmacd = pm_info->pm_data.pmacd;
    sal_memcpy(&pmac_acc, &(pm_acc->pmac_acc), sizeof(bcmpmac_access_t));

    egr_timestamp_mode = timestamp_mode;
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->egress_timestamp_mode_set(&pmac_acc, pm_acc->pmac_blkport,
                                          egr_timestamp_mode));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

bcmpc_pm_drv_t bcmpc_pm_drv_pm4x10 = {
    .speed_config_validate = pm4x10_speed_config_validate,
    .port_lane_map_validate = pm4x10_pm_port_lane_map_validate,
    .core_attach = pm4x10_core_attach,
    .port_detach = pm4x10_port_detach,
    .pm_init = pm4x10_pm_init,
    .pm_destroy = pm4x10_pm_destroy,
    .port_add = pm4x10_port_add,
    .pm_port_loopback_set = pm4x10_port_loopback_set,
    .pm_port_loopback_get = pm4x10_port_loopback_get,
    .pm_port_encap_set = pm4x10_port_encap_set,
    .pm_port_encap_get = pm4x10_port_encap_get,
    .pm_port_pause_ctrl_set = pm4x10_port_pause_ctrl_set,
    .pm_port_pause_ctrl_get = pm4x10_port_pause_ctrl_get,
    .pm_port_pause_addr_set = pm4x10_port_pause_addr_set,
    .pm_port_pause_addr_get = pm4x10_port_pause_addr_get,
    .pm_port_frame_max_set = pm4x10_port_frame_max_set,
    .pm_port_frame_max_get = pm4x10_port_frame_max_get,
    .pm_port_enable_set = pm4x10_port_enable_set,
    .pm_port_enable_get = pm4x10_port_enable_get,
    .pm_port_link_status_get = pm4x10_port_link_status_get,
    .pm_port_lag_failover_lpbk_get = pm4x10_port_lag_failover_loopback_get,
    .pm_port_speed_config_set = pm4x10_port_speed_config_set,
    .pm_port_speed_config_get = pm4x10_port_speed_config_get,
    .pm_port_ability_advert_set = pm4x10_port_ability_advert_set,
    .pm_port_ability_advert_get = pm4x10_port_ability_advert_get,
    .pm_port_ability_remote_get = pm4x10_port_ability_remote_get,
    .pm_port_autoneg_set = pm4x10_port_autoneg_set,
    .pm_port_autoneg_get = pm4x10_port_autoneg_get,
    .pm_port_autoneg_status_get = pm4x10_port_autoneg_status_get,
    .pm_port_failover_enable_set = pm4x10_port_failover_enable_set,
    .pm_port_failover_enable_get = pm4x10_port_failover_enable_get,
    .pm_port_link_training_set = pm4x10_port_link_training_set,
    .pm_port_link_training_get = pm4x10_port_link_training_get,
    .pm_port_tx_set = pm4x10_port_tx_set,
    .pm_port_tx_get = pm4x10_port_tx_get,
    .pm_port_pmd_lane_cfg_set = pm4x10_port_pmd_lane_cfg_set,
    .pm_port_pmd_lane_cfg_get = pm4x10_port_pmd_lane_cfg_get,
    .pm_phy_status_get = pm4x10_phy_status_get,
    .pm_phy_control_set = pm4x10_phy_control_set,
    .pm_port_abilities_get = pm4x10_port_abilities_get,
    .pm_vco_get = pm4x10_pm_vco_rate_get,
    .pm_port_phy_link_up_event = pm4x10_port_phy_link_up_event,
    .pm_port_phy_link_down_event = pm4x10_port_phy_link_down_event,
    .pm_port_config_validate = pm4x10_pm_port_config_validate,
    .pm_port_link_training_status_get = pm4x10_port_link_training_status_get,
    .pm_vco_rate_calculate = pm4x10_pm_vco_calculate,
    .pm_default_pmd_lane_config_get = pm4x10_port_default_lane_config_get,
    .pm_default_tx_taps_config_get = pm4x10_port_default_tx_taps_config_get,
    .pm_phy_polarity_get = pm4x10_phy_polarity_get,
    .pm_core_lane_map_get = pm4x10_core_lane_map_get,
    .port_tx_timestamp_info_get = pm4x10_port_tx_timestamp_info_get,
    .pm_egress_timestamp_mode_get = pm4x10_egress_timestamp_mode_get,
    .pm_egress_timestamp_mode_set = pm4x10_egress_timestamp_mode_set
};
