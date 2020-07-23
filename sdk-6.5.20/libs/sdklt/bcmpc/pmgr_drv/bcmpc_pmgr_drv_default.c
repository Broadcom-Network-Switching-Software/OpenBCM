/*! \file bcmpc_pmgr_drv_default.c
 *
 * This file implements the default port manager driver.
 *
 * The port manager API will use port manager driver instances to achieve the
 * hardware configuration of PHY and PMAC.
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
#include <phymod/phymod.h>
#include <phymod/phymod_sim_control.h>

#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_drv_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_fdr.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PMGR

#define PMGR_CFG_UNDO_NUM   (10)
#define PM_CORE_INIT_PASS   (2)
#define NUM_PML             (2)

/*!
 * \brief Check if the port config is changed.
 *
 * \param [in] _force Forcedly treat as config changed.
 * \param [in] _pcfg1 The first pcfg variable.
 * \param [in] _pcfg2 The second pcfg variable.
 * \param [in] _f field name.
 *
 * retval 1 The port configuration is changed.
 * retval 0 The port configuration is same.
 */
#define PMGR_CFG_CHANGED(_force, _pcfg1, _pcfg2, _f) \
            ((_force) || (!BCMPC_FIELD_VAL_EQUAL(_pcfg1, _pcfg2, _f)))

/*!
 * \brief Check if the port config array is changed.
 *
 * \param [in] _force Forcedly treat as config changed.
 * \param [in] _pcfg1 The first pcfg variable.
 * \param [in] _pcfg2 The second pcfg variable.
 * \param [in] _f Field name (Need to be an array).
 * \param [in] _cnt Number of element to compare in the array.
 *
 * retval 1 The port configuration is changed.
 * retval 0 The port configuration is same.
 */
#define PMGR_CFG_ARR_CHANGED(_force, _pcfg1, _pcfg2, _f, _cnt) \
       ((_force) || \
        (!BCMPC_ARRAY_PARTIAL_EQUAL((_pcfg1)->_f, (_pcfg2)->_f, _cnt)))

/*! Port configuration handler. */
typedef int
(*pmgr_pcfg_attr_op_f)(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg);

/*! Phy operation handler function. */
typedef int
(*pmgr_phy_op_f)(int unit, bcmpc_pport_t pport, uint32_t param);

/*! This structure is used to define the operation for the PHY. */
typedef struct pmgr_phy_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    pmgr_phy_op_f func;

} pmgr_phy_op_hdl_t;


/*! Port control flags. */
#define PMGR_CTRL_F_MAC (1 << 0)
#define PMGR_CTRL_F_PHY (1 << 1)


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get BCMPC driver information.
 *
 * To get the PHY/PMAC driver information via BCMPC driver object, and fill the
 * information to \ref bcmpc_pmgr_drv_data_t structure.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] pcfg Port configuration.
 * \param [out] drv_data Pmgr driver data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL BCMPC driver object is not assigned.
 */
static int
port_drv_data_get(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg,
                  bcmpc_pmgr_drv_data_t *drv_data)
{
    bcmpc_drv_t *pc_drv;
    bcmpmac_access_t *pmac_acc;
    bcmpc_pm_lport_info_t lport_info;
    int blk_num, blk_port;

    SHR_FUNC_ENTER(unit);

    sal_memset(drv_data, 0, sizeof(*drv_data));
    bcmpc_pm_lport_info_t_init(&lport_info);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &drv_data->pm_id));
     SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_lport_info_get(unit, pport, pcfg, &lport_info));

    /* Save the device port number. */
    drv_data->pport = pport;

    /* Setup pmac_drv and pmac_acc. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_drv->pblk_get(unit, pport, &blk_num, &blk_port));
    drv_data->pmac_data.unit = unit;
    drv_data->pmac_data.blk = blk_num;
    drv_data->pmac_blkport = blk_port;
    drv_data->pmac_drv = pc_drv->pmac_drv_get(unit, pport);
    SHR_NULL_CHECK(drv_data->pmac_drv, SHR_E_INTERNAL);
    pmac_acc = &(drv_data->pmac_acc);
    PMAC_ACC_USER_ACC(pmac_acc) = &(drv_data->pmac_data);
    PMAC_ACC_BUS(pmac_acc) = pc_drv->pmac_bus_get(unit, pport);

    /* Setup PHY access user data. */
    BCMPC_PHY_ACCESS_DATA_SET(&drv_data->phy_data, unit, pport);

    /* Setup phy_core_acc. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_phymod_core_access_t_init(unit, pport, &drv_data->phy_data,
                                         &drv_data->phy_core_acc));

    /* Setup phy_acc. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, lport_info.lbmp,
                                        &drv_data->phy_data,
                                        &drv_data->phy_acc));
    /* Set tvco pll index */
    if (pc_drv->phy_pll_idx_get) {
        drv_data->phy_acc.access.tvco_pll_index =
                  pc_drv->phy_pll_idx_get(unit, pport, BCMPC_PLL_TYPE_TVCO);
    }

exit:
    SHR_FUNC_EXIT();
}

static inline int
pm_port_drv_data_get(int unit, bcmpc_pport_t pport, pm_access_t *pm_acc,
                     bcmpc_pmgr_drv_data_t *drv_data, bcmpc_pm_info_t *pm_info,
                     pm_info_t **pm_drv_info, bcmpc_pm_drv_t **pm_drv)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, NULL, drv_data));

    sal_memset(pm_acc, 0, sizeof(pm_access_t));

    pm_acc->phy_acc = drv_data->phy_acc;
    pm_acc->core_acc = drv_data->phy_core_acc;
    pm_acc->pmac_acc = drv_data->pmac_acc;
    pm_acc->pmac_blkport = drv_data->pmac_blkport;
    pm_acc->pport = pport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, drv_data->pm_id, pm_info));
    *pm_drv = (pm_info->prop.pm_drv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_drv_info_get(unit, drv_data->pm_id, pm_drv_info));
    (*pm_drv_info)->pm_data.pmacd = drv_data->pmac_drv;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Get the starting lane and the number of lanes for a given lane mask.
 *
 * This function only support at most 8 lanes' lane mask for now.
 *
 * \param [in] port_lane_mask Lane mask for a logical port.
 * \param [out] start_lane Starting lane of the port.
 * \param [out] num_lanes Number of lanes of the port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM The input is not valid.
 */
static int
port_lane_info_get(uint32_t port_lane_mask, int *start_lane, int *num_lanes)
{
    switch (port_lane_mask) {
        case 0x1:
            *start_lane = 0;
            *num_lanes = 1;
            break;
        case 0x2:
            *start_lane = 1;
            *num_lanes = 1;
            break;
        case 0x4:
            *start_lane = 2;
            *num_lanes = 1;
            break;
        case 0x8:
            *start_lane = 3;
            *num_lanes = 1;
            break;
        case 0x3:
            *start_lane = 0;
            *num_lanes = 2;
            break;
        case 0xc:
            *start_lane = 2;
            *num_lanes = 2;
            break;
        case 0x7:
            *start_lane = 0;
            *num_lanes = 3;
            break;
        case 0xf:
            *start_lane = 0;
            *num_lanes = 4;
            break;
        case 0x10:
            *start_lane = 4;
            *num_lanes = 1;
            break;
        case 0x20:
            *start_lane = 5;
            *num_lanes = 1;
            break;
        case 0x40:
            *start_lane = 6;
            *num_lanes = 1;
            break;
        case 0x80:
            *start_lane = 7;
            *num_lanes = 1;
            break;
        case 0x30:
            *start_lane = 4;
            *num_lanes = 2;
            break;
        case 0xc0:
            *start_lane = 6;
            *num_lanes = 2;
            break;
        case 0xf0:
            *start_lane = 4;
            *num_lanes = 4;
            break;
        case 0xff:
            *start_lane = 0;
            *num_lanes = 8;
            break;
        default:
            return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}

/*!
 * Get the lane mask for a specific lane in a port.
 *
 * This function only support at most 8 lanes' lane mask for now.
 *
 * \param [in] port_lane_mask Lane mask for a logical port.
 * \param [in] lane_index Lane index in this logical port.
 * \param [out] lane_mask Lane mask for that lane.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM This input combination is not valid.
 */
static int
phy_lane_mask_set(uint32_t port_lane_mask, int lane_index, uint32_t *lane_mask)
{
    int start_lane, num_of_lane;
    int rv = SHR_E_NONE;

    rv = port_lane_info_get(port_lane_mask, &start_lane, &num_of_lane);

    if (rv != SHR_E_NONE) {
        return rv;
    }

    if ((lane_index >= num_of_lane) || (lane_index < 0)) {
        return SHR_E_PARAM;
    }

    *lane_mask = 0x1 << (start_lane + lane_index);

    return SHR_E_NONE;
}

static int
tsc_power_control(int unit, bcmpc_pport_t pport, int power_on)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    bcmpc_drv_t *pc_drv;
    bool is_dual_pml_supported = false;
    bcmpc_pport_t pport_tmp;
    int pml_cnt, pml_idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv->is_dual_pml_supported) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->is_dual_pml_supported(unit, pport,
                                           &is_dual_pml_supported));
    }

    if (is_dual_pml_supported) {
        /*
         * If the PM consists of two PMLs. we need to power on/off both PMLs.
         */
        if (pc_drv->pm_base_pport_adjust == NULL) {
            SHR_EXIT();
        }
        pml_cnt = 2;
        for (pml_idx = 0; pml_idx < pml_cnt; pml_idx++) {
            SHR_IF_ERR_EXIT
                (pc_drv->pm_base_pport_adjust(unit, pport, pml_idx,
                                              &pport_tmp));

            SHR_IF_ERR_VERBOSE_EXIT
                (pm_port_drv_data_get(unit, pport_tmp, &pm_acc, &drv_data,
                                      &pm_info, &pm_drv_info, &pm_drv));
            if (pm_drv->pm_tsc_ctrl == NULL) {
                /* Return success if not supprted */
                SHR_EXIT();
            }

            PM_LOCK(pm_drv_info);
            rv = pm_drv->pm_tsc_ctrl(unit, &pm_acc, &op_st,
                                     pm_drv_info, power_on);
            PM_UNLOCK(pm_drv_info);

            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                                  &pm_drv_info, &pm_drv));
        if (pm_drv->pm_tsc_ctrl == NULL) {
            /* Return success if not supprted */
            SHR_EXIT();
        }

        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_tsc_ctrl(unit, &pm_acc, &op_st, pm_drv_info, power_on);
        PM_UNLOCK(pm_drv_info);

        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * PHY operation handlers
 */
/*!
 * \brief Set PHY enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device number.
 * \param [in] enable 1 to enable, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
phy_enable_set(int unit, bcmpc_pport_t pport, uint32_t enable)
{
    uint32_t flag = 0;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    flag = PM_PORT_ENABLE_PHY;

    rv = pm_drv->pm_port_enable_set(unit, &pm_acc, &op_st, pm_drv_info,
                                    flag, enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link up event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device number.
 * \param [in] enable Don't care. Common prototype for PHY operation handlers.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
link_up_event(int unit, bcmpc_pport_t pport, uint32_t enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_phy_link_up_event(unit, &pm_acc, &op_st, pm_drv_info);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link down event handler.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device number.
 * \param [in] enable Don't care. Common prototype for PHY operation handlers.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
link_down_event(int unit, bcmpc_pport_t pport, uint32_t enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_phy_link_down_event(unit, &pm_acc,
                                             &op_st, pm_drv_info);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Private functions for handling the fields of bcmpc_port_cfg_t
 */

/*!
 * \brief Get the PHY and MAC enable state of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] flag The target of the operation, PMGR_CTRL_F_XXX.
 * \param [out] enable 1 for enabled, 0 for disabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_enable_get(int unit, bcmpc_pport_t pport, uint32_t flag, uint32_t *enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bool release_lock = FALSE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    if (!flag) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "flag is not specified\n")));
        *enable = 0;
        SHR_EXIT();
    }

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    release_lock = TRUE;

    if (flag & PMGR_CTRL_F_PHY) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_enable_get(unit, &pm_acc, &op_st, pm_drv_info,
                                        flag, enable));
        if (*enable == 0) {
            SHR_EXIT();
        }
    }

    if (flag & PMGR_CTRL_F_MAC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_drv->pm_port_enable_get(unit, &pm_acc, &op_st, pm_drv_info,
                                        flag, enable));
    }

exit:
    if (release_lock) {
        PM_UNLOCK(pm_drv_info);
    }
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the PHY and MAC enable state of a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] flag The target of the operation, PMGR_CTRL_F_XXX.
 * \param [in] enable 1 to enable, 0 to disable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_enable_set(int unit, bcmpc_pport_t pport, uint32_t flag, uint32_t enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_enable_set(unit, &pm_acc, &op_st, pm_drv_info,
                                    flag, enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update administrative enable.
 *
 * A port must be enabled in order to forward packets. If a port is not enabled,
 * the driver will attempt to bring down any active link on the port, usually by
 * powering down the PHY. If no PHY is associated with the port, the MAC will be
 * disabled. An administratively disabled port may be brought partially up in
 * order to allow packet loopback on the port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_admin_enable_update(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    uint32_t flag = 0;

    SHR_FUNC_ENTER(unit);

    flag |= PM_PORT_ENABLE_PHY;

    SHR_ERR_EXIT
        (port_enable_set(unit, pport, flag, pcfg->enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get loopback configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] lpbk_mode Loopback mode.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_lpbk_get(int unit, bcmpc_pport_t pport, bcmpc_port_lpbk_t *lpbk_mode)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpc_port_lpbk_t lpbk = BCMPC_PORT_LPBK_NONE;
    int enable = 0;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    for (lpbk = BCMPC_PORT_LPBK_MAC; lpbk < BCMPC_PORT_LPBK_COUNT; lpbk++) {
         rv = pm_drv->pm_port_loopback_get(unit, &pm_acc, &op_st, pm_drv_info,
                                           lpbk, &enable);
        if (SHR_FAILURE(rv)) {
            PM_UNLOCK(pm_drv_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        if (enable) {
            break;
        }
    }

    PM_UNLOCK(pm_drv_info);

    if (lpbk == BCMPC_PORT_LPBK_COUNT) {
        lpbk = BCMPC_PORT_LPBK_NONE;
    }

    *lpbk_mode = lpbk;

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set loopback configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_lpbk_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    bcmpc_port_lpbk_t cur_lpbk_mode = BCMPC_PORT_LPBK_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    /* Retrieve current loopback type. */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_lpbk_get(unit, pport, &cur_lpbk_mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

     /* Disable current loopback mode. */
    if (cur_lpbk_mode != BCMPC_PORT_LPBK_NONE) {
        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_port_loopback_set(unit, &pm_acc, &op_st, pm_drv_info,
                                          cur_lpbk_mode, 0);
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Set requested loopback mode. */
    if (pcfg->lpbk_mode != BCMPC_PORT_LPBK_NONE) {
        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_port_loopback_set(unit, &pm_acc, &op_st, pm_drv_info,
                                          pcfg->lpbk_mode, 1);
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate speed configuration for a given logical port.
 *
 * This function is called when auto-negotiation (AN) is disabled.
 * Before we restore the force speed for the port, we need to validate
 * the force speed configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_speed_validate(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_speed_config_t spd_config;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    spd_config.speed = pcfg->speed;
    spd_config.num_lanes = pcfg->num_lanes;
    spd_config.fec = pcfg->fec_mode;
    if (pm_drv->speed_config_validate != NULL) {
        PM_LOCK(pm_drv_info);
        rv = pm_drv->speed_config_validate(unit, &pm_acc,&op_st, pm_drv_info,
                                           &spd_config);
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (bcmdrd_feature_is_real_hw(unit) ||
        bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        if (pm_drv->pm_port_config_validate != NULL) {
            PM_LOCK(pm_drv_info);
            rv = pm_drv->pm_port_config_validate(unit, &pm_acc, &op_st,
                                                 pm_drv_info, &spd_config, 0);
            PM_UNLOCK(pm_drv_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*
 * This function is called by the speed config set to derive the PMD
 * lane configuration that should be set in the hardware.
 *
 * Steps to determine the PMD firmware lane configuration
 * 1. First lookup the IMM DB to get the current PC_PMD_FIMRWARE
 *    configuration.
 * 2. get the default PMD firmware lane configuration from the
 *    driver based on the signalling (speed and number of lanes).
 * 3. Look at the individual LT fields to determine if it should
 *    be AUTO configured.
 * 4. If a n LT field is AUTO, copy the default value retrieved
 *    from the driver for the field.
 * 5. If the LT field is NOT AUTO (AUTO = 0), then use the value
 *    from PC_PMD_FIRMWARE IMM DB lookup. It is the user's
 *    responsibility to ensure that the LT field is set to a
 *    meaningful value for the port configuration, before turning
 *    OFF the AUTO field.
 */

static int
port_default_pmd_config_check(int unit, bcmpc_pport_t pport,
                              bcmpc_lport_t lport,
                              pm_speed_config_t *speed_config,
                              bcmpc_pmd_firmware_config_t *pmd_config)
{
    int rv = SHR_E_NONE, idx;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_oper_status_t op_st;
    phymod_firmware_lane_config_t pfm_cfg;
    bcmpc_pmd_firmware_config_t pmd_cfg;
    bcmpc_pmd_firmware_config_t def_pmd_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&pmd_cfg, 0, sizeof(bcmpc_pmd_firmware_config_t));
    sal_memset(&def_pmd_cfg, 0, sizeof(bcmpc_pmd_firmware_config_t));
    phymod_firmware_lane_config_t_init(&pfm_cfg);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));


    rv = (bcmpc_db_imm_entry_lookup(unit, PC_PMD_FIRMWAREt, P(&lport),
                                    P(&pmd_cfg)));

    /*
     * When the PC_PMD_FIRMWARE LT is not present, it could mean the logical
     * port is deleted and is being added now, set all the PMD lane config
     * to auto to set the default values in the hardware.
     */
    if (rv == SHR_E_NOT_FOUND) {
        pmd_cfg.dfe_auto = 1;
        pmd_cfg.lp_dfe_auto = 1;
        pmd_cfg.medium_auto = 1;
        pmd_cfg.scrambling_en_auto = 1;
        pmd_cfg.cl72_res_to_auto = 1;
        pmd_cfg.lp_tx_precoder_on_auto = 1;
        pmd_cfg.unreliable_los_auto = 1;
        pmd_cfg.norm_reach_pam4_auto = 1;
        pmd_cfg.ext_reach_pam4_auto = 1;
    }

    PM_LOCK(pm_drv_info);


    rv = pm_drv->pm_default_pmd_lane_config_get(unit, &pm_acc, &op_st,
                                                pm_drv_info,
                                                speed_config, &pfm_cfg);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_lane_config_from_phy(unit, &pfm_cfg, &def_pmd_cfg));

    /*
     * Default PMD lane configuration is per-lane based.
     * Need to set normal reach and extended reach bitmask for all the lanes
     * based on the default value.
     */
    for (idx = 0; idx < speed_config->num_lanes; idx++) {
        def_pmd_cfg.norm_reach_pam4_bmask |= pfm_cfg.ForceNormalReach << idx;
        def_pmd_cfg.ext_reach_pam4_bmask |= pfm_cfg.ForceExtenedReach << idx;
    }

    /*
     * If DFE auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.dfe_auto) {
        pmd_config->dfe = def_pmd_cfg.dfe;
    } else {
        pmd_config->dfe = pmd_cfg.dfe;
    }

    /*
     * If LP DFE auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.lp_dfe_auto) {
        pmd_config->lp_dfe = def_pmd_cfg.lp_dfe;
    } else {
        pmd_config->lp_dfe = pmd_cfg.lp_dfe;
    }

    /*
     * If port MEDIUM TYPE auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.medium_auto) {
        pmd_config->medium = def_pmd_cfg.medium;
    } else {
        pmd_config->medium = pmd_cfg.medium;
    }

    /*
     * If SCRAMBLING_EN auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.scrambling_en_auto) {
        pmd_config->scrambling_en = def_pmd_cfg.scrambling_en;
    } else {
        pmd_config->scrambling_en = pmd_cfg.scrambling_en;
    }

    /*
     * If CL72_RESTART_TIMEOUT_EN auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.cl72_res_to_auto) {
        pmd_config->cl72_res_to = def_pmd_cfg.cl72_res_to;
    } else {
        pmd_config->cl72_res_to = pmd_cfg.cl72_res_to;
    }

    /*
     * If LP_TX_PRECODER_ON auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.lp_tx_precoder_on_auto) {
        pmd_config->lp_tx_precoder_on = def_pmd_cfg.lp_tx_precoder_on;
    } else {
        pmd_config->lp_tx_precoder_on = pmd_cfg.lp_tx_precoder_on;
    }

    /*
     * If UNRELIABLE_LOS auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.unreliable_los_auto) {
        pmd_config->unreliable_los = def_pmd_cfg.unreliable_los;
    } else {
        pmd_config->unreliable_los = pmd_cfg.unreliable_los;
    }

    /*
     * If NORMAL_REACH_PAM4 auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.norm_reach_pam4_auto) {
        pmd_config->norm_reach_pam4_bmask = def_pmd_cfg.norm_reach_pam4_bmask;
    } else {
        pmd_config->norm_reach_pam4_bmask = pmd_cfg.norm_reach_pam4_bmask;
    }

    /*
     * If EXTENDED_REACH_PAM4 auto is set,
     *       use the default value from the driver.
     * else
     *      use the value from the DB lookup.
     */
    if (pmd_cfg.ext_reach_pam4_auto) {
        pmd_config->ext_reach_pam4_bmask = def_pmd_cfg.ext_reach_pam4_bmask;
    } else {
        pmd_config->ext_reach_pam4_bmask = pmd_cfg.ext_reach_pam4_bmask;
    }


exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*
 * This function is called by the port attach to derive the
 * TX TAPs configuration that should be set in the hardware.
 *
 * Steps to determine the TX TAPs configuration
 * 1. First lookup the IMM DB to get the current PC_TX_TAPSt
 *    configuration.
 * 2. get the default TX TAPs configuration from the
 *    driver based on the signalling (speed and number of lanes).
 * 3. Look at the individual LT fields to determine if it should
 *    be AUTO configured.
 * 4. If a n LT field is AUTO, copy the default value retrieved
 *    from the driver for the field.
 * 5. If the LT field is NOT AUTO (AUTO = 0), then use the value
 *    from PC_TX_TAPSt IMM DB lookup. It is the user's
 *    responsibility to ensure that the LT field is set to a
 *    meaningful value for the port configuration, before turning
 *    OFF the AUTO field.
 */

static int
port_default_tx_taps_config_check(int unit, bcmpc_pport_t pport,
                                  bcmpc_port_lane_t port_lane,
                                  pm_speed_config_t *speed_config,
                                  bcmpc_tx_taps_config_t  *tx_taps_config)
{
    int rv = SHR_E_NONE, idx;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_oper_status_t op_st;
    phymod_tx_t default_phymod_tx_taps;
    bcmpc_tx_taps_config_t default_tx_taps;
    bcmpc_tx_taps_config_t tx_taps_lookup[PHYMOD_MAX_LANES_PER_CORE];
    int num_lanes;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&default_tx_taps, 0, sizeof(bcmpc_tx_taps_config_t));
    phymod_tx_t_init(&default_phymod_tx_taps);
    for (idx = 0; idx < PHYMOD_MAX_LANES_PER_CORE; idx++) {
        sal_memset(&tx_taps_lookup[idx], 0, sizeof(bcmpc_tx_taps_config_t));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));
    /* get default TX Taps config */
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_default_tx_taps_config_get(unit, &pm_acc, &op_st,
                                           pm_drv_info,
                                           speed_config, &default_phymod_tx_taps);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* convert PHYMOD TX TAPs config to PC TX TAPs config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_tx_lane_status_from_phy(unit,
                                      &default_phymod_tx_taps, &default_tx_taps));

    /* Check if PC_TX_TAPS table exists */
    num_lanes = shr_util_popcount(pm_acc.phy_acc.access.lane_mask);
    for (idx = 0; idx < num_lanes; idx++) {
        port_lane.lane_index = idx;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_lt_entry_get(unit, PC_TX_TAPSt, P(&port_lane),
                                P(&tx_taps_lookup[idx])));

        /*
         * If TX FIR pre cursor auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].pre_auto) {
            tx_taps_config[idx].pre = default_tx_taps.pre;
            tx_taps_config[idx].pre_sign = default_tx_taps.pre_sign;
        } else {
            tx_taps_config[idx].pre = tx_taps_lookup[idx].pre;
            tx_taps_config[idx].pre_sign = tx_taps_lookup[idx].pre_sign;
        }

        /*
         * If TX FIR pre2 cursor auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].pre2_auto) {
            tx_taps_config[idx].pre2 = default_tx_taps.pre2;
            tx_taps_config[idx].pre2_sign = default_tx_taps.pre2_sign;
        } else {
            tx_taps_config[idx].pre2 = tx_taps_lookup[idx].pre2;
            tx_taps_config[idx].pre2_sign = tx_taps_lookup[idx].pre2_sign;
        }

        /*
         * If TX FIR main cursor auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].main_auto) {
            tx_taps_config[idx].main = default_tx_taps.main;
            tx_taps_config[idx].main_sign = default_tx_taps.main_sign;
        } else {
            tx_taps_config[idx].main = tx_taps_lookup[idx].main;
            tx_taps_config[idx].main_sign = tx_taps_lookup[idx].main_sign;
        }

        /*
         * If TX FIR post cursor auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].post_auto) {
            tx_taps_config[idx].post = default_tx_taps.post;
            tx_taps_config[idx].post_sign = default_tx_taps.post_sign;
        } else {
            tx_taps_config[idx].post = tx_taps_lookup[idx].post;
            tx_taps_config[idx].post_sign = tx_taps_lookup[idx].post_sign;
        }

        /*
         * If TX FIR post2 cursor auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].post2_auto) {
            tx_taps_config[idx].post2 = default_tx_taps.post2;
            tx_taps_config[idx].post2_sign = default_tx_taps.post2_sign;
        } else {
            tx_taps_config[idx].post2 = tx_taps_lookup[idx].post2;
            tx_taps_config[idx].post2_sign = tx_taps_lookup[idx].post2_sign;
        }

        /*
         * If TX FIR post3 cursor auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].post3_auto) {
            tx_taps_config[idx].post3 = default_tx_taps.post3;
            tx_taps_config[idx].post3_sign = default_tx_taps.post3_sign;
        } else {
            tx_taps_config[idx].post3 = tx_taps_lookup[idx].post3;
            tx_taps_config[idx].post3_sign = tx_taps_lookup[idx].post3_sign;
        }

        /*
         * If TX FIR shunt value auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].rpara_auto) {
            tx_taps_config[idx].rpara = default_tx_taps.rpara;
            tx_taps_config[idx].rpara_sign = default_tx_taps.rpara_sign;
        } else {
            tx_taps_config[idx].rpara = tx_taps_lookup[idx].rpara;
            tx_taps_config[idx].rpara_sign = tx_taps_lookup[idx].rpara_sign;
        }

        /*
         * If TX amplitude auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].amp_auto) {
            tx_taps_config[idx].amp = default_tx_taps.amp;
            tx_taps_config[idx].amp_sign = default_tx_taps.amp_sign;
        } else {
            tx_taps_config[idx].amp = tx_taps_lookup[idx].amp;
            tx_taps_config[idx].amp_sign = tx_taps_lookup[idx].amp_sign;
        }

        /*
         * If TX driver mode auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].drv_mode_auto) {
            tx_taps_config[idx].drv_mode = default_tx_taps.drv_mode;
            tx_taps_config[idx].drv_mode_sign = default_tx_taps.drv_mode_sign;
        } else {
            tx_taps_config[idx].drv_mode = tx_taps_lookup[idx].drv_mode;
            tx_taps_config[idx].drv_mode_sign = tx_taps_lookup[idx].drv_mode_sign;
        }

        /*
         * If TX signalling mode auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].sig_mode_auto) {
            tx_taps_config[idx].sig_mode = default_tx_taps.sig_mode;
        } else {
            tx_taps_config[idx].sig_mode = tx_taps_lookup[idx].sig_mode;
        }

        /*
         * If TXFIR mode auto is set,
         *     use the default value from the driver.
         * else
         *     use the value from the DB lookup.
         */
        if (tx_taps_lookup[idx].txfir_tap_mode_auto) {
            tx_taps_config[idx].txfir_tap_mode = default_tx_taps.txfir_tap_mode;
        } else {
            tx_taps_config[idx].txfir_tap_mode = tx_taps_lookup[idx].txfir_tap_mode;
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore TX TAPs configurations
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_tx_taps_restore(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_oper_status_t op_st;
    pm_speed_config_t speed_config;
    bcmpc_port_lane_t port_lane;
    phymod_tx_t phymod_tx_taps;
    bcmpc_tx_taps_config_t tx_taps_config[PHYMOD_MAX_LANES_PER_CORE];
    int start_lane, num_lanes, idx = 0, rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* update TX TAPs */
    pm_speed_config_t_init(unit, &speed_config);

    for (idx = 0; idx < PHYMOD_MAX_LANES_PER_CORE; idx++) {
        sal_memset(&tx_taps_config[idx], 0, sizeof(bcmpc_tx_taps_config_t));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_speed_config_to_pm_cfg(unit, pcfg, &speed_config));
    /* derive the TX TAPs configurations that should be set in HW */
    port_lane.lport = pcfg->lport;
    port_lane.lane_index = 0;

    SHR_IF_ERR_VERBOSE_EXIT
    (port_default_tx_taps_config_check(unit, pcfg->pport, port_lane,
                                       &speed_config, tx_taps_config));

    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));
    for (idx = 0; idx < num_lanes; idx++) {
        /* convert to phymod tx taps */
        phymod_tx_t_init(&phymod_tx_taps);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_tx_taps_cfg_to_phy(unit, &tx_taps_config[idx], &phymod_tx_taps));
        /* set tx taps */
        pm_acc.phy_acc.access.lane_mask = 1 << (start_lane + idx);
        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_port_tx_set(unit, &pm_acc, &op_st, pm_drv_info, &phymod_tx_taps);

        PM_UNLOCK(pm_drv_info);
        if (rv != SHR_E_NONE) {
            /* ignore tx_set return value no matter tx taps is valid or not */
            rv = SHR_E_NONE;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Restore PMD FW lane configures.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pmd_firmware_lane_config_restore(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    pm_speed_config_t speed_config;
    int start_lane, num_lanes, idx = 0;
    phymod_firmware_lane_config_t phymod_lane_config;
    bcmpc_pmd_firmware_config_t pmd_config;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));
    /* update PMD_FIRMWARE lane config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_speed_config_to_pm_cfg(unit, pcfg, &speed_config));
    /* derive the PMD_FIRMWARE lane configurations that should be set in HW */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_default_pmd_config_check(unit, pport, pcfg->lport,
                                       &speed_config,
                                       &pmd_config));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_lane_config_to_phy(unit, &pmd_config,
                                           &phymod_lane_config));

    if (pcfg->link_training) {
        phymod_lane_config.ForceExtenedReach = 0;
        phymod_lane_config.ForceNormalReach = 0;
        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_port_pmd_lane_cfg_set(unit, &pm_acc, &op_st,
                                              pm_drv_info, &phymod_lane_config);
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        for (idx = 0; idx < num_lanes; idx++) {
            pm_acc.phy_acc.access.lane_mask = 1 << (start_lane + idx);
            phymod_lane_config.ForceExtenedReach =
                (pmd_config.ext_reach_pam4_bmask >> idx) & 0x1;
            phymod_lane_config.ForceNormalReach =
                (pmd_config.norm_reach_pam4_bmask >> idx) & 0x1;
            PM_LOCK(pm_drv_info);
            rv = pm_drv->pm_port_pmd_lane_cfg_set(unit, &pm_acc, &op_st,
                                              pm_drv_info, &phymod_lane_config);
            PM_UNLOCK(pm_drv_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set operating mode configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_speed_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_speed_config_t speed_config;
    pm_oper_status_t op_st;
    bcmpc_pmd_firmware_config_t pmd_config;
    phymod_firmware_lane_config_t phymod_lane_config;
    int start_lane = 0, num_lanes = 0;
    uint32_t temp_bmask = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    pm_speed_config_t_init(unit, &speed_config);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));
    /* get starting lane and num of lanes */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_speed_config_to_pm_cfg(unit, pcfg, &speed_config));

    /* derive the PMD_FIRMWARE lane configurations that should be set in HW */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_default_pmd_config_check(unit, pport, pcfg->lport,
                                       &speed_config,
                                       &pmd_config));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_lane_config_to_phy(unit, &pmd_config,
                                           &phymod_lane_config));

    /* _speed_config_set() will set same PMD lane config for all the lanes,
     * therefore, speed_config.lane_config takes the value of starting lane.
     */
    phymod_lane_config.ForceExtenedReach &= 0x1;
    phymod_lane_config.ForceNormalReach &= 0x1;
    sal_memcpy(&speed_config.lane_config,
        &phymod_lane_config, sizeof(phymod_firmware_lane_config_t));


    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_speed_config_set(unit, &pm_acc, &op_st, pm_drv_info,
                                          &speed_config);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* after speed_config_set is done,
     * check if ForceExtenedReach and ForceNormalReach need to be set
     * differently for each lane, PMD_FIRMWARE lane config will be skipped when
     * - link_training is enabled;
     * - force ER/NR is set to 0x0 for all lanes;
     * - force ER/NR is set to 0x1 for all lanes;
     */
    if (!speed_config.link_training) {
        /* bmask value if all lanes are set to 0x1 */
        temp_bmask = (0x1 << num_lanes) - 1;
        if (((pmd_config.norm_reach_pam4_bmask != 0) &&
             (pmd_config.norm_reach_pam4_bmask != temp_bmask)) ||
            ((pmd_config.ext_reach_pam4_bmask != 0) &&
             (pmd_config.ext_reach_pam4_bmask != temp_bmask))) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_pmd_firmware_lane_config_restore(unit, pport, pcfg));
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port speed.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [out] speed_config Port speed configuration information.
 * \param [out] is_an 1 is auto-negotiation enabled, 0 is disabled.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_speed_get(int unit, bcmpc_pport_t pport,
               pm_speed_config_t *speed_config,
               pm_autoneg_status_t *autoneg_status)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpc_pm_lport_info_t lport_info;
    phymod_autoneg_status_t an_status;
    bool release_lock = FALSE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    bcmpc_pm_lport_info_t_init(&lport_info);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    release_lock = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->pm_port_speed_config_get(unit, &pm_acc, &op_st, pm_drv_info,
                                          speed_config));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_drv->pm_port_autoneg_status_get(unit, &pm_acc, &op_st, pm_drv_info,
                                            &an_status));

    PM_UNLOCK(pm_drv_info);
    release_lock = FALSE;

    autoneg_status->an_enabled = an_status.enabled? 1: 0;
    autoneg_status->an_done = an_status.locked? 1: 0;

    if (an_status.enabled && an_status.locked) {
        speed_config->num_lanes = an_status.resolved_num_lane;
    }

exit:
    if (SHR_FUNC_ERR() && release_lock) {
        PM_UNLOCK(pm_drv_info);
    }
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set auto-negotiation advertisement.
 *
 * This function will lookup IMM database to get AUTONEG_PROFILEs used
 * by the logical port. Then translate the auto-negotiation abilities
 * to PM data type and pass to PM driver.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_autoneg_advert_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    unsigned int size = 0;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int num_ability = 0, index = 0, rv = SHR_E_NONE;
    bcmpc_autoneg_prof_t an_cfg;
    pm_port_ability_t *an_ability = NULL;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    /* If number of profile is 0, just return. */
    if (pcfg->num_an_profs == 0) {
        return SHR_E_NONE;
    }
    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* Allocate an_ability*/
    size = sizeof(pm_port_ability_t) * NUM_AUTONEG_PROFILES_MAX;
    SHR_ALLOC(an_ability, size, "bcmpcAnAbility");
    SHR_NULL_CHECK(an_ability, SHR_E_MEMORY);
    sal_memset(an_ability, 0, size);

    num_ability = pcfg->num_an_profs;
    for (index = 0; index < num_ability; index++) {
        rv = bcmpc_db_imm_entry_lookup(unit,
                                       PC_AUTONEG_PROFILEt,
                                       P(&pcfg->autoneg_profile_ids[index]),
                                       P(&an_cfg));
        if (rv == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(rv);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_ability_to_pm_ability(unit,
                                                   &an_cfg,
                                                   &(an_ability[index])));

    }

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_ability_advert_set(unit, &pm_acc, &op_st, pm_drv_info,
                                            num_ability, an_ability);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if (an_ability !=NULL) {
        SHR_FREE(an_ability);
    }
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set auto-negotiation control.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_autoneg_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    phymod_autoneg_control_t an_control;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&an_control, 0, sizeof(phymod_autoneg_control_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    an_control.enable = pcfg->autoneg ? 1: 0;

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_autoneg_set(unit, &pm_acc, &op_st,
                                     pm_drv_info, &an_control);

    PM_UNLOCK(pm_drv_info);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get encap configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [out] encap_mode Encapsulation mode.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_encap_get(int unit, bcmpc_pport_t pport, bcmpc_port_encap_t *encap_mode)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_encap_t encap;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_encap_get(unit, &pm_acc, &op_st, pm_drv_info, &encap);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_encap_mode_from_pmac(encap, encap_mode));

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set encap configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_encap_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_encap_t pm_encap;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_encap_mode_to_pmac(pcfg->encap_mode, &pm_encap));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_encap_set(unit, &pm_acc, &op_st,
                                   pm_drv_info, pm_encap);

    /* Reconfigure other blocks: PGW/MMU/TDM/IP/EP. */

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pause configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [in] mac_cfg MAC configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_set(int unit, bcmpc_pport_t pport, bcmpc_mac_control_t *mac_cfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    bcmpmac_pause_ctrl_t pause_ctrl;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    pause_ctrl.tx_enable = mac_cfg->pause_tx;
    pause_ctrl.rx_enable = mac_cfg->pause_rx;

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_pause_ctrl_set(unit, &pm_acc, &op_st, pm_drv_info,
                                        &pause_ctrl);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pause MAC address configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] mac_cfg MAC configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_addr_set(int unit, bcmpc_pport_t pport, bcmpc_mac_control_t *mac_cfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_pause_addr_set(unit, &pm_acc, &op_st, pm_drv_info,
                                        mac_cfg->pause_addr);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set max frame size configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_frame_max_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_frame_max_set(unit, &pm_acc, &op_st, pm_drv_info,
                                       pcfg->max_frame_size);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable link training.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_link_training_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_link_training_set(unit, &pm_acc, &op_st, pm_drv_info,
                                           pcfg->link_training);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get link training set status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] enable 1 for enabled, otherwise disabled.
 * \param [out] done Link training is locked or not.
 * \retval SHR_E_NONE No errors.
 */
static int
port_link_training_get(int unit, bcmpc_pport_t pport,
                       uint32_t *enable, uint32_t *done)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_link_training_get(unit, &pm_acc, &op_st,
                                           pm_drv_info, enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (*enable) {
        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_port_link_training_status_get(unit, &pm_acc, &op_st,
                                                      pm_drv_info, done);
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    } else {
        *done = 0;
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Undo port configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] backlog List of attribute config functions to execute.
 * \param [in] lport Logical port number.
 * \param [in] pm_drv PM driver information.
 * \param [in] pm_drv_info PM internal state information.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_cfg_undo(int unit, pmgr_pcfg_attr_op_f *backlog,
              bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    pmgr_pcfg_attr_op_f *attr_cfg = backlog;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_CONT
        (port_enable_set(unit, pport, PM_PORT_ENABLE_MAC, 0));

    while (*attr_cfg) {
        SHR_IF_ERR_CONT
            ((*attr_cfg)(unit, pport, pcfg));
        attr_cfg++;
    }

    SHR_IF_ERR_CONT
        (port_admin_enable_update(unit, pport, pcfg));

    SHR_FUNC_EXIT();
}



/*!
 * \brief Get mac tx enable.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] tx_enable Mac tx enable .
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_tx_enable_set(int unit, bcmpc_pport_t pport, uint32_t tx_enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_tx_enable_set(unit, &pm_acc, &op_st,
                                      pm_drv_info, tx_enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mac tx enable.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] rx_enable Mac rx enable .
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_rx_enable_set(int unit, bcmpc_pport_t pport, uint32_t rx_enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_rx_enable_set(unit, &pm_acc, &op_st,
                                      pm_drv_info, rx_enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable mac local, remote fault .
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] dis Mac fault disable data structure.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_fault_disable_set(int unit, bcmpc_pport_t pport, uint32_t local_fault,
                      uint32_t rmt_fault)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    bcmpmac_fault_disable_t flt_dis;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    sal_memset(&flt_dis, 0, sizeof(bcmpmac_fault_disable_t));
    flt_dis.local_fault_disable = local_fault;
    flt_dis.remote_fault_disable = rmt_fault;

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_fault_disable_set(unit, &pm_acc, &op_st,
                                          pm_drv_info, &flt_dis);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mac fault status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] st Mac fault data structure.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_fault_status_get(int unit, bcmpc_pport_t pport, bcmpmac_fault_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_fault_status_get(unit, &pm_acc, &op_st,
                                         pm_drv_info, st);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set MAC inter packet gap size.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ipg_size Inter packet gap size.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_avg_ipg_set(int unit, bcmpc_pport_t pport, uint8_t ipg_size)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_avg_ipg_set(unit, &pm_acc, &op_st, pm_drv_info, ipg_size);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set MAC MIB oversize value.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] size MIB oversize value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_mib_oversize_set(int unit, bcmpc_pport_t pport, uint32_t size)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mib_oversize_set(unit, &pm_acc, &op_st, pm_drv_info, size);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pause frames handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable 1 to pass pause frames to system side,
 *                    otherwise drop in MAC.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_frames_pass_set(int unit, bcmpc_pport_t pport, uint32_t enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pause_frames_pass_set(unit, &pm_acc, &op_st,
                                       pm_drv_info, enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set control frames handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable 1 to pass control frames to system side,
 *                    otherwise drop in MAC.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_control_frames_pass_set(int unit, bcmpc_pport_t pport, uint32_t enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->control_frames_pass_set(unit, &pm_acc, &op_st,
                                         pm_drv_info, enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get control frames handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] enable 1 - pass control frames are passed to system side,
 *                     otherwise dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_control_frames_pass_get(int unit, bcmpc_pport_t pport, uint32_t *enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->control_frames_pass_get(unit, &pm_acc, &op_st,
                                         pm_drv_info, enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set Runt threshold on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] value Size of packet below which it is marked as runt.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_runt_threshold_set(int unit, bcmpc_pport_t pport, uint32_t value)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->runt_threshold_set(unit, &pm_acc, &op_st, pm_drv_info, value);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Runt threshold on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] value Size of packet below which it is marked as runt.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_runt_threshold_get(int unit, bcmpc_pport_t pport, uint32_t *value)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->runt_threshold_get(unit, &pm_acc, &op_st, pm_drv_info, value);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable RLM mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] pcfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_rlm_config_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_port_rlm_config_t rlm_config;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&rlm_config, 0, sizeof(pm_port_rlm_config_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    rlm_config.is_initiator = pcfg->initiator;
    rlm_config.active_lane_bit_map = pcfg->active_lane_mask;
    rlm_config.rlm_enable = pcfg->rlm_mode;

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_rlm_config_set(unit, &pm_acc, &op_st, pm_drv_info,
                                          &rlm_config);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mac enabled status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] tx_enabled 1 for tx is enabled, otherwise disabled.
 * \param [out] rx_enabled 1 for rx is enabled, otherwise disabled.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_enable_status_get(int unit, bcmpc_pport_t pport,
                      uint32_t *tx_enabled, uint32_t *rx_enabled)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_enable_get(unit, &pm_acc, &op_st, pm_drv_info,
                                   tx_enabled, rx_enabled);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mac enabled status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] mac_state MAC TX and RX state.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_status_get(int unit, bcmpc_pport_t pport,
               bcmpc_port_oper_state_t *mac_state)
{
    uint32_t rx_enable = 0, tx_enable = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(mac_enable_status_get(unit, pport, &tx_enable, &rx_enable));

    if (rx_enable == tx_enable) {
        *mac_state = (rx_enable == 1)? BCMPC_MAC_TX_RX_ON: BCMPC_MAC_TX_RX_OFF;
    } else {
        *mac_state = (rx_enable == 1)? BCMPC_MAC_RX_ON: BCMPC_MAC_TX_ON;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pause configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [out] pause_tx Pause TX configuration.
 * \param [out] pause_rx Pause RX configuration.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_get(int unit, bcmpc_pport_t pport, uint32_t *pause_tx,
               uint32_t *pause_rx)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    bcmpmac_pause_ctrl_t pause_ctrl;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    sal_memset(&pause_ctrl, 0, sizeof(bcmpmac_pause_ctrl_t));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_pause_ctrl_get(unit, &pm_acc, &op_st, pm_drv_info,
                                        &pause_ctrl);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    *pause_tx = pause_ctrl.tx_enable;
    *pause_rx = pause_ctrl.rx_enable;

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pause MAC address configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port.
 * \param [out] pause_addr Pause MAC address.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_addr_get(int unit, bcmpc_pport_t pport, bcmpc_mac_t pause_addr)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_pause_addr_get(unit, &pm_acc, &op_st, pm_drv_info,
                                        pause_addr);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get max frame size configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] max_frame_size Max frame size.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_frame_max_get(int unit, bcmpc_pport_t pport, uint32_t *max_frame_size)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_frame_max_get(unit, &pm_acc, &op_st, pm_drv_info,
                                       max_frame_size);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get LAG failover state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] lag_failover_en Lag failover enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_failover_enable_get(int unit, bcmpc_pport_t pport,
                         uint32_t *lag_failover_en)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_failover_enable_get(unit, &pm_acc, &op_st,
                                             pm_drv_info, lag_failover_en);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mac fault disable status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] st Mac fault disable data structure.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
mac_fault_disable_get(int unit, bcmpc_pport_t pport,
                      bcmpmac_fault_disable_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_fault_disable_get(unit, &pm_acc, &op_st,
                                          pm_drv_info, st);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get MAC inter packet gap size.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] ipg_size Inter packet gap size.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_avg_ipg_get(int unit, bcmpc_pport_t pport, uint8_t *ipg_size)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mac_avg_ipg_get(unit, &pm_acc, &op_st, pm_drv_info,
                                    ipg_size);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get MAC MIB oversize value.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] size MIB oversize value.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_mib_oversize_get(int unit, bcmpc_pport_t pport, uint32_t *size)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_mib_oversize_get(unit, &pm_acc, &op_st, pm_drv_info, size);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get pause frames handling in the switch.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] enable 1 - pass pause frames are passed to system side,
 *                     otherwise dropped in MAC.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_pause_frames_pass_get(int unit, bcmpc_pport_t pport, uint32_t *enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pause_frames_pass_get(unit, &pm_acc, &op_st, pm_drv_info,
                                       enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port TX stall setting.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] enable 1 - Packet transmission stalled,
 *                         otherwise enabled.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_tx_stall_enable_get(int unit, bcmpc_pport_t pport, bool *enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_stall_tx_get(unit, &pm_acc, &op_st, pm_drv_info,
                                      enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port TX stall setting.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable 1 - Packet transmission stalled,
 *                        otherwise enabled.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
port_tx_stall_enable_set(int unit, bcmpc_pport_t pport, bool enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_stall_tx_set(unit, &pm_acc, &op_st, pm_drv_info,
                                      enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}
/*************************************************************************
 * Private functions for PC_MAC_CONTROL LT
 */
static int
port_rsv_mask_get(int unit, bcmpc_pport_t pport,
                  bcmpc_mac_control_t *mac_status)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->port_rsv_mask_get(unit, &pm_acc, &op_st, pm_drv_info, 0,
                                   &mac_status->mac_rsv_ctrl);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
port_rsv_mask_set(int unit, bcmpc_pport_t pport,
                  bcmpc_mac_control_t *mac_control)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpc_mac_control_t mac_status;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    uint32_t rsv_flags = 0, rsv_mask = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&mac_status, 0, sizeof(bcmpc_mac_control_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (port_rsv_mask_get(unit, pport, &mac_status));


    sal_memcpy(&rsv_flags, mac_status.rsv_fbmp, sizeof(rsv_flags));
    rsv_mask = mac_status.mac_rsv_ctrl;
    if (pc_drv->port_to_mac_rsv) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->port_to_mac_rsv(unit, pport, *mac_control->rsv_fbmp,
                                     mac_control->mac_rsv_ctrl,
                                     &rsv_flags, &rsv_mask));
    } else {
        SHR_EXIT();
    }

    PM_LOCK(pm_drv_info);

    rv = pm_drv->port_rsv_mask_set(unit, &pm_acc, &op_st, pm_drv_info,
                                   rsv_flags, rsv_mask);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Pmgr driver functions
 */
static int
default_pm_port_enable(int unit, bcmpc_pport_t pport, int enable)
{
    int pmac_blkport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpc_drv_t *pc_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, NULL, &drv_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    pmac_blkport = drv_data.pmac_blkport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    /* Per-port MAC initialization. */
    SHR_IF_ERR_VERBOSE_EXIT
       (pmacd->port_init(pmac_acc, pmac_blkport, enable));

    /* Chip specific initial coniguration settings. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    PM_LOCK(pm_drv_info);

    /* Disable 16B credit granularity, if PM is MACSEC capable.  */
    if (pc_drv->is_pm_sec_enabled) {
        if (pc_drv->is_pm_sec_enabled(unit, drv_data.pm_id)) {
            if (pmacd->credit_granularity_set) {
                SHR_IF_ERR_EXIT
                    (pmacd->credit_granularity_set(pmac_acc,
                                                   pmac_blkport, 0));
            }
        }
    }

    PM_UNLOCK(pm_drv_info);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_cfg_set(int unit, bcmpc_pport_t pport, bcmpc_lport_t lport,
                     bcmpc_port_cfg_t *pcfg, bool is_new)
{
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpc_port_cfg_t old_pcfg;
    int rv, lcnt = 0, force_cfg = 0;
    pmgr_pcfg_attr_op_f backlog[PMGR_CFG_UNDO_NUM] = { 0 };
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    bool an_ability_configured = 0, is_speed_set_called = 0;
    bool tx_taps_restore_required = 0, pmd_lane_config_restore_required = 0;
    bcmpc_mac_control_t mctrl;
    bcmpc_phy_sig_mode_t cfg_sigmode, old_cfg_sigmode;

    SHR_FUNC_ENTER(unit);
    sal_memset(&old_pcfg, 0, sizeof(bcmpc_port_cfg_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, pcfg, &drv_data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, drv_data.pm_id, &pm_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_drv_info_get(unit, drv_data.pm_id, &pm_drv_info));
    pm_drv_info->pm_data.pmacd = drv_data.pmac_drv;

    /*
     * The PC initialization first populates all the LT's in the pre-config
     * stage and applies the config to the hardware during the run stage.
     */
    if (is_new) {
        /* New logical port, force the configuration to the hardware. */
        force_cfg = 1;
    } else {
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&old_pcfg));
        if (rv == SHR_E_NOT_FOUND) {
             /*
             * SHR_E_NOT_FOUND means there is no previous configuration.
             * set attributes of PC_PORTt.
             */
            force_cfg = 1;
        }
    }
    pcfg->lport = lport;

    if (PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, max_frame_size)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (port_frame_max_set(unit, pport, pcfg));
        backlog[lcnt++] = port_frame_max_set;
    }

    if (PMGR_CFG_CHANGED(force_cfg, pcfg, &old_pcfg, encap_mode)) {
        /*
         * If ENCAP mode is Higig3 and the MAC_CONTROLt.IFG_AUTO is set
         * then fforce the IFG to 8 and then set the encapsulation to
         * Higig3.
         * Perform a MAC_CONTROL IMM DB lookup to get the IFG_AUTO
         * setting.
         */
        sal_memset(&mctrl, 0, sizeof(bcmpc_mac_control_t));
        rv = bcmpc_db_imm_entry_lookup(unit, PC_MAC_CONTROLt, P(&lport),
                                       P(&mctrl));

        /*
         * Force IFG, if PC_MAC_CONTROL LT is not configured or
         * if IFG_AUTO is set in PC_MAC_CONTROL LT.
         */
        if (rv == SHR_E_NOT_FOUND || mctrl.ifg_auto) {
            /*
             * SHR_E_NOT_FOUND means there is no previous configuration.
             * Force the IFG to 8.
             */
            force_cfg = TRUE;
        }

        /*
         * If force cfg is adjust the IFG based on the encapsulation type.
         */
        if (force_cfg) {
            uint32_t ifg = 0;
            if (mctrl.ifg_auto) {
                /* IFG auto configured based on encapsulation type. */
                if (pcfg->encap_mode == BCMPC_PORT_ENCAP_HG3) {
                    ifg = 8;
                } else if (pcfg->encap_mode == BCMPC_PORT_ENCAP_IEEE) {
                    ifg = 12;
                }
            } else {
                /* User controlled IFG value. */
                ifg = mctrl.ifg;
            }
            if (ifg) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (port_avg_ipg_set(unit, pport, ifg));
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (port_encap_set(unit, pport, pcfg));
        backlog[lcnt++] = port_encap_set;
    }

    if (PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, speed) ||
        PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, fec_mode)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (port_speed_set(unit, pport, pcfg));
        backlog[lcnt++] = port_speed_set;
        is_speed_set_called = 1;

        /*
         * TX TAPs restore is required if,
         * - signaling mode is changed when link training has been disabled, or
         * - link training is changed from enabled to disable
         */
        if ((!pcfg->link_training) && (!is_new)) {
            cfg_sigmode = (pcfg->speed/pcfg->num_lanes) > 25000 ? \
                BCMPC_PHY_SIG_MODE_PAM4 : BCMPC_PHY_SIG_MODE_NRZ;
            old_cfg_sigmode = (old_pcfg.speed/old_pcfg.num_lanes) > 25000 ? \
                BCMPC_PHY_SIG_MODE_PAM4 : BCMPC_PHY_SIG_MODE_NRZ;
            if ((cfg_sigmode != old_cfg_sigmode) ||
                (old_pcfg.link_training == 1)) {
                tx_taps_restore_required = 1;
            }
        }
    }

    if (PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, num_an_profs) ||
        PMGR_CFG_ARR_CHANGED(0, pcfg, &old_pcfg,
                             autoneg_profile_ids, pcfg->num_an_profs)) {
        if (pcfg->num_an_profs != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_autoneg_advert_set(unit, pport, pcfg));
            an_ability_configured = 1;
        }
        backlog[lcnt++] = port_autoneg_advert_set;
    }

    if (PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, autoneg)) {
        /*
         * Auto-negotiation (AN) control is enabled.
         * Number of AN profiles is unchagned and non-zero.
         */
        if (pcfg->autoneg && (!an_ability_configured)) {
            /* Set AN advertisment when AN is enabled. */
            SHR_IF_ERR_VERBOSE_EXIT
                (port_autoneg_advert_set(unit, pport, pcfg));
            backlog[lcnt++] = port_autoneg_advert_set;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (port_autoneg_set(unit, pport, pcfg));
        backlog[lcnt++] = port_autoneg_set;
        if (!pcfg->autoneg) {
            /* Restore force speed configuration when AN is disabled. */
            SHR_IF_ERR_VERBOSE_EXIT
                (port_speed_validate(unit, pport, pcfg));
            SHR_IF_ERR_VERBOSE_EXIT
                (port_speed_set(unit, pport, pcfg));
            backlog[lcnt++] = port_speed_set;
            /* TX TAPs need to be restored */
            tx_taps_restore_required = 1;
        }
    }

    /*
     * link training could be set inside port_speed_set(), therefore
     * skip the following call if port_speed_set() is already called
     */
    if ((PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, link_training)) &&
        (!is_speed_set_called)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (port_link_training_set(unit, pport, pcfg));
        backlog[lcnt++] = port_link_training_set;
        if (!pcfg->link_training) {
            tx_taps_restore_required = 1;
            pmd_lane_config_restore_required = 1;
        }
    }

    /*
     * Do RLM configuration when:
     * 1. User changes pcfg.rlm_mode value to enable/disable RLM.
     * 2. The port has pcfg.rlm_mode as '1' and
     *    a. pcfg.initiator value is changed.
     *    b. pcfg.active_lane_mask is changed.
     *    c. User sets pcfg.rlm_mode to '1' in the command line to restart RLM
     *       state machine.
     */
    if (PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, rlm_mode) ||
        ((PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, initiator) ||
          PMGR_CFG_CHANGED(0, pcfg, &old_pcfg, active_lane_mask) ||
          SHR_BITGET(pcfg->fbmp, BCMPC_PORT_LT_FLD_RLM)) &&
         pcfg->rlm_mode)) {
        /* RLM config can only be invoked on real HW */
        if (bcmdrd_feature_is_real_hw(unit) &&
            !bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
            /* need to check if RLM is supported */
            if (SHR_BITGET(pm_info.prop.pm_feature, BCMPC_FT_RLM)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (port_rlm_config_set(unit, pport, pcfg));
                backlog[lcnt++] = port_rlm_config_set;
                /*
                 * When a port is in Reduced Lane Mode (RLM), some
                 * of the lanes are disabled, When disabling the RLM
                 * feature on the port, the driver reconfigures all
                 * the lanes and reconfigures the actual speed configured
                 * on the port.
                 * Since the port is now using all the lanes configured,
                 * it should not be assumed that the PMD firmware settings
                 * are same as before enabling RLM.
                 * Restore the PMD firmware configuration on the
                 * all the lanes.
                 */
                if (!pcfg->rlm_mode) {
                    pmd_lane_config_restore_required = 1;
                }
            }
        }
    }

    /* restore TX TAPs */
    if (tx_taps_restore_required) {
        /* update TX TAPs */
        SHR_IF_ERR_VERBOSE_EXIT
            (port_tx_taps_restore(unit, pport, pcfg));
        backlog[lcnt++] = port_tx_taps_restore;
    }
    /* restore PMD lane configs */
    if (pmd_lane_config_restore_required) {
        /* update PMD lane config */
        SHR_IF_ERR_VERBOSE_EXIT
            (port_pmd_firmware_lane_config_restore(unit, pport, pcfg));
        backlog[lcnt++] = port_pmd_firmware_lane_config_restore;
    }

exit:
    /* Undo the change when something failed. */
    if (SHR_FUNC_ERR() && lcnt > 0) {
        port_cfg_undo(unit, backlog, pport, &old_pcfg);
    }

    SHR_FUNC_EXIT();
}

static int
pmm_port_status_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                    bcmpc_pmgr_port_status_t *pst)
{
    uint32_t link_training = 0, training_done = 0;
    pm_speed_config_t speed_cfg;
    bcmpmac_fault_status_t pmac_st;
    pm_autoneg_status_t an_status;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_lpbk_get(unit, pport, &pst->lpbk_mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_encap_get(unit, pport, &pst->encap_mode));

    if (!bcmdrd_feature_is_real_hw(unit) &&
        !bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        /*
         * Get speed/fec/num_lanes from IMM database in simulation.
         */
        int rv;
        bcmpc_port_cfg_t pcfg;
        rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
        if (rv == SHR_E_NOT_FOUND) {
            return rv;
        }
        pst->speed = pcfg.speed;
        pst->fec_mode = pcfg.fec_mode;
        pst->num_lanes = pcfg.num_lanes;
        pst->autoneg = pcfg.autoneg;
        /* AN will not be resolved in SIM. */
        pst->an_done = 0;
    } else {
        sal_memset(&an_status, 0, sizeof(an_status));
        SHR_IF_ERR_VERBOSE_EXIT
            (pm_speed_config_t_init(unit, &speed_cfg));
        SHR_IF_ERR_VERBOSE_EXIT
            (port_speed_get(unit, pport, &speed_cfg, &an_status));
        pst->speed = speed_cfg.speed;
        pst->fec_mode = speed_cfg.fec;
        pst->num_lanes = speed_cfg.num_lanes;
        pst->autoneg = an_status.an_enabled;
        pst->an_done = an_status.an_done;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (port_link_training_get(unit, pport, &link_training, &training_done));
    pst->link_training = link_training? 1 : 0;
    pst->link_training_done = training_done? 1 : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (port_enable_get(unit, pport, PMGR_CTRL_F_MAC, &pst->mac_enabled));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_enable_get(unit, pport, PMGR_CTRL_F_PHY, &pst->phy_enabled));

    SHR_IF_ERR_VERBOSE_EXIT
        (mac_status_get(unit, pport, &pst->oper_state));

    sal_memset(&pmac_st, 0, sizeof(bcmpmac_fault_status_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mac_fault_status_get(unit, pport, &pmac_st));

    pst->local_fault = pmac_st.local_fault;
    pst->remote_fault = pmac_st.remote_fault;

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_phy_link_state_get(int unit, bcmpc_pport_t pport,
                           bcmpc_pmgr_link_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bool release_lock = FALSE;
    uint32_t linked = 0;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    release_lock = TRUE;

    PHY_IF_ERR_EXIT
        (pm_drv->pm_port_link_status_get(unit, &pm_acc, &op_st,
                                         pm_drv_info, &linked));

    if (linked) {
        BCMPC_BIT_SET(*st, BCMPC_PMGR_LINK_EVENT_LINK);
    }

exit:
    if (release_lock) {
        PM_UNLOCK(pm_drv_info);
    }
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
default_fault_state_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pmgr_link_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;
    int pmac_blkport;
    bcmpmac_fault_status_t fault_st = { 0 };

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, NULL, &drv_data));

    pmac_blkport = drv_data.pmac_blkport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    /* Get MAC fault state. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->fault_status_get(pmac_acc, pmac_blkport, &fault_st));

    if (fault_st.local_fault) {
        BCMPC_BIT_SET(*st, BCMPC_PMGR_LINK_EVENT_LOCAL_FAULT);
    }

    if (fault_st.remote_fault) {
        BCMPC_BIT_SET(*st, BCMPC_PMGR_LINK_EVENT_REMOTE_FAULT);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
default_phy_link_info_get(int unit, bcmpc_pport_t pport,
                          bcmpc_pmgr_link_status_t *st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    phymod_phy_access_t *phy_acc;
    uint32_t pmd_locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, NULL, &drv_data));

    phy_acc = &drv_data.phy_acc;

    PHY_IF_ERR_EXIT
        (phymod_phy_rx_pmd_locked_get(phy_acc, &pmd_locked));

    if (pmd_locked) {
        BCMPC_BIT_SET(*st, BCMPC_PMGR_LINK_EVENT_PMD_RX_LOCK);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
default_pmac_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    int pmac_blkport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, NULL, &drv_data));

    pmac_blkport = drv_data.pmac_blkport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->op_exec(pmac_acc, pmac_blkport, op->name, op->param));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * PHY Operations
 */

static pmgr_phy_op_hdl_t pmgr_phy_ops[] = {
    { "lane_enable", phy_enable_set },
    { "link_up_event", link_up_event},
    { "link_down_event", link_down_event},
};

static int
default_phy_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    pmgr_phy_op_hdl_t *op_hdl;
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < COUNTOF(pmgr_phy_ops); i++) {
        op_hdl = &pmgr_phy_ops[i];

        if (sal_strcmp(op->name, op_hdl->name) != 0) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (op_hdl->func(unit, pport, op->param));

        SHR_EXIT();
    }


    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_failover_enable_set(int unit, bcmpc_pport_t pport, uint32_t enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_failover_enable_set(unit, &pm_acc, &op_st, pm_drv_info,
                                             enable);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_failover_enable_get(int unit, bcmpc_pport_t pport, uint32_t *enable)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_failover_enable_get(unit, &pm_acc, &op_st, pm_drv_info,
                                             enable);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_failover_loopback_get(int unit, bcmpc_pport_t pport,
                          uint32_t *failover_loopback)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_lag_failover_lpbk_get(unit, &pm_acc, &op_st,
                                               pm_drv_info,
                                               failover_loopback);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
default_failover_loopback_remove(int unit, bcmpc_pport_t pport)
{
    int pmac_blkport;
    bcmpc_pmgr_drv_data_t drv_data;
    bcmpmac_drv_t *pmacd;
    bcmpmac_access_t *pmac_acc;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pport, NULL, &drv_data));

    pmac_blkport = drv_data.pmac_blkport;
    pmacd = drv_data.pmac_drv;
    pmac_acc = &drv_data.pmac_acc;

    SHR_IF_ERR_VERBOSE_EXIT
        (pmacd->failover_loopback_remove(pmac_acc, pmac_blkport));

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_pfc_get(int unit, bcmpc_lport_t lport,
            bcmpc_pport_t pport, bcmpc_pfc_control_t *pctrl)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpmac_pfc_t pfc_cfg;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_pfc_get(unit, &pm_acc, &op_st, pm_drv_info, &pfc_cfg);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmpc_pmgr_pm_pfc_cfg_from_pm_cfg(unit, &pfc_cfg, pctrl));

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*!
 * This function is used to set the PFC configuration of a logical port.
 * The callers are expected to set the field bitmap in the
 * input param bcmpc_pfc_control_t structure, to indicate which fields
 * are to be configured.
 * The other fields are retrieved from the existing configuration and
 * updated to the hardware.
 *
 * This function is called during run-time for PC_PFC updates and
 * following are steps to set the hardware.
 * 1. Check the field bitmask to determine the fields to be updated
 *    to the hardware.
 * 2. Get the current IMM DB configuration by performing a lookup
 *    on the PC_PFC LT.
 * 3. Get the current hardware configured values.
 * 4. Compare if the field value changed from the current DB value.
 * 5. Update the hardware with the updated values.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pmm_pfc_set(int unit, bcmpc_lport_t lport,
            bcmpc_pport_t pport,  bcmpc_pfc_control_t *pctrl)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpmac_pfc_t pfc_cfg;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    bcmpc_pfc_control_t cur_pfc;
    bcmpc_pfc_control_t old_db_pfc;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&cur_pfc, 0, sizeof(bcmpc_pfc_control_t));
    sal_memset(&old_db_pfc, 0, sizeof(bcmpc_pfc_control_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* Get the current hardware pfc settings for the port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pmm_pfc_get(unit, lport, pport, &cur_pfc));

    /* Get the old IMM DB PFC settings. */
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PFCt,
                                   P(&lport), P(&old_db_pfc));

    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            sal_memcpy(&old_db_pfc, &cur_pfc, sizeof(bcmpc_pfc_control_t));
        }
    }

    /* If ENABLE_RX flag set, update rx_enable if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_ENABLE_RX)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, rx_enable)) {
            cur_pfc.rx_enable = pctrl->rx_enable;
        }
    }

    /* If ENABLE_TX flag set, update tx_enable if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_ENABLE_TX)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, tx_enable)) {
            cur_pfc.tx_enable = pctrl->tx_enable;
        }
    }

    /* If ENABLE_STATS flag set, update stats_enable if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_ENABLE_STATS)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, stats_enable)) {
            cur_pfc.stats_enable = pctrl->stats_enable;
        }
    }

    /* If REFRESH_TIMER flag set, update refresh_timer if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_REFRESH_TIMER)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, refresh_timer)) {
            cur_pfc.refresh_timer = pctrl->refresh_timer;
        }
    }

    /* If DEST_ADDR flag set, update pfc dest MAC addr if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_DEST_ADDR)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, da)) {
            sal_memcpy(&cur_pfc.da, pctrl->da, sizeof(cur_pfc.da));
        }
    }

    /* If ETH_TYPE flag set, update eth_type if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_ETH_TYPE)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, eth_type)) {
            cur_pfc.eth_type = pctrl->eth_type;
        }
    }

    /* If OPCODE flag set, update opcode if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_OPCODE)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, opcode)) {
            cur_pfc.opcode = pctrl->opcode;
        }
    }

    /* If XOFF_TIMER flag set, update xoff_timer if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_XOFF_TIMER)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, xoff_timer)) {
            cur_pfc.xoff_timer = pctrl->xoff_timer;
        }
    }

    /* If PFC_PASS flag set, update pfc_pass if db value changed. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_PFC_LT_FLD_PFC_PASS)) {
        if (PMGR_CFG_CHANGED(0, pctrl, &old_db_pfc, pfc_pass)) {
            cur_pfc.pfc_pass = pctrl->pfc_pass;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmpc_pmgr_pm_pfc_cfg_to_pm_cfg(unit, &cur_pfc, &pfc_cfg));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_pfc_set(unit, &pm_acc, &op_st, pm_drv_info, &pfc_cfg);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
port_ecc_intr_enable_set(int unit,
                         bcmpc_pm_drv_t  *pm_drv,
                         pm_access_t *pm_acc, pm_oper_status_t *op_st,
                         pm_info_t *pm_drv_info)
{
    int rv = SHR_E_NONE;
    bcmpc_intr_type_t intr_type;

    for (intr_type = 0; intr_type < BCMPC_INTR_COUNT; intr_type++) {
        switch (intr_type) {
            /*! One bit ECC errors */
            case BCMPC_INTR_ECC_1B_ERR_RSFEC_RBUF_MPP0:
            case BCMPC_INTR_ECC_1B_ERR_RSFEC_RBUF_MPP1:
            case BCMPC_INTR_ECC_1B_ERR_RSFEC_MPP0:
            case BCMPC_INTR_ECC_1B_ERR_RSFEC_MPP1:
            case BCMPC_INTR_ECC_1B_ERR_DESKEW:
            case BCMPC_INTR_ECC_1B_ERR_SPEED:
            case BCMPC_INTR_ECC_1B_ERR_AM:
            case BCMPC_INTR_ECC_1B_ERR_UM:
            case BCMPC_INTR_ECC_1B_ERR_TX_1588:
            case BCMPC_INTR_ECC_1B_ERR_RX_1588_MPP0:
            case BCMPC_INTR_ECC_1B_ERR_RX_1588_MPP1:

                /*! Two bit ECC errors */
            case BCMPC_INTR_ECC_2B_ERR_RSFEC_RBUF_MPP0:
            case BCMPC_INTR_ECC_2B_ERR_RSFEC_RBUF_MPP1:
            case BCMPC_INTR_ECC_2B_ERR_RSFEC_MPP0:
            case BCMPC_INTR_ECC_2B_ERR_RSFEC_MPP1:
            case BCMPC_INTR_ECC_2B_ERR_DESKEW:
            case BCMPC_INTR_ECC_2B_ERR_SPEED:
            case BCMPC_INTR_ECC_2B_ERR_AM:
            case BCMPC_INTR_ECC_2B_ERR_UM:
            case BCMPC_INTR_ECC_2B_ERR_TX_1588:
            case BCMPC_INTR_ECC_2B_ERR_RX_1588_MPP0:
            case BCMPC_INTR_ECC_2B_ERR_RX_1588_MPP1:

                rv = pm_drv->port_intr_enable_set(unit, pm_acc, op_st, pm_drv_info, intr_type, 1);
                if (SHR_E_NONE != rv) {
                    return rv;
                }
                break;

            default:
                continue;
        }
    }

    return rv;
}

static int
pmm_port_attach(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
               bcmpc_pm_core_cfg_t *ccfg, int enable)
{
    pm_port_add_info_t add_info;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t  *pm_drv = NULL;
    int rv = SHR_E_NONE, idx = 0;
    pm_oper_status_t op_st;
    bcmpc_tx_taps_config_t tx_taps_config[PHYMOD_MAX_LANES_PER_CORE];
    bcmpc_port_lane_t port_lane;
    bcmpc_drv_t *pc_drv;
    int num_lanes;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pcfg->pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmpc_pmgr_pm_core_cfg_to_pm_cfg(unit, ccfg, pm_info.prop.num_lanes,
                                         &add_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_cfg_to_pm_cfg(unit, pcfg, &add_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pcfg->pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    for (idx = 0; idx < PHYMOD_MAX_LANES_PER_CORE; idx++) {
        sal_memset(&tx_taps_config[idx], 0, sizeof(bcmpc_tx_taps_config_t));
    }

    /* derive the TX TAPs configurations that should be set in HW */
    port_lane.lport = lport;
    port_lane.lane_index = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (port_default_tx_taps_config_check(unit, pcfg->pport, port_lane,
                                           &add_info.speed_config,
                                           tx_taps_config));

    num_lanes = shr_util_popcount(pm_acc.phy_acc.access.lane_mask);
    for (idx = 0; idx < num_lanes; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_tx_taps_cfg_to_phy(unit, &tx_taps_config[idx],
                                              &add_info.init_config.tx_params[idx]));
    }

    PM_LOCK(pm_drv_info);
    rv = pm_drv->port_add(unit, &pm_acc, &op_st, pm_drv_info, &add_info);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    /* Enable port ecc interrupts at high level (e.g. cmic, cdport)*/
    if (pc_drv->interrupt_set) {
        SHR_IF_ERR_EXIT
            (pc_drv->interrupt_set(unit, pcfg->pport, enable));
    }

    /* Enable ecc interrupts at port or port-macro level */
    rv = port_ecc_intr_enable_set(unit, pm_drv, &pm_acc, &op_st, pm_drv_info);
    if (SHR_E_UNAVAIL == rv) {
        /*
         * For management port, interrupt handling is not available.
         * Hence ignore SHR_E_UNAVAIL
         */
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pcfg->pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_port_speed_get(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_speed_config_t speed_config;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&speed_config, 0, sizeof(pm_speed_config_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pcfg->pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_speed_config_get(unit, &pm_acc, &op_st, pm_drv_info,
                                          &speed_config);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_pm_cfg_to_speed_config(unit, &speed_config, pcfg));

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pcfg->pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_pm_port_config_validate(int unit, bcmpc_port_cfg_t *pcfg, int is_first,
                            int is_new)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_speed_config_t spd_config;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&spd_config, 0, sizeof(pm_speed_config_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_speed_config_to_pm_cfg(unit, pcfg, &spd_config));
    /*
     * When port is new, phymod access is not available before port being
     * attached. Here assigning lane_mask in order to validate FEC type on MPP.
     */
    if (is_new) {
        sal_memset(&pm_acc, 0, sizeof(pm_access_t));

        SHR_IF_ERR_VERBOSE_EXIT
           (port_drv_data_get(unit, pcfg->pport, pcfg, &drv_data));

        pm_acc.phy_acc = drv_data.phy_acc;
        pm_acc.core_acc = drv_data.phy_core_acc;
        pm_acc.pmac_acc = drv_data.pmac_acc;
        pm_acc.pmac_blkport = drv_data.pmac_blkport;
        pm_acc.pport = pcfg->pport;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pm_info_get(unit, drv_data.pm_id, &pm_info));
        pm_drv = (pm_info.prop.pm_drv);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pm_drv_info_get(unit, drv_data.pm_id, &pm_drv_info));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
           (pm_port_drv_data_get(unit, pcfg->pport, &pm_acc, &drv_data,
                                 &pm_info, &pm_drv_info, &pm_drv));
    }

    spd_config.speed = pcfg->speed;
    spd_config.num_lanes = pcfg->num_lanes;
    spd_config.fec = pcfg->fec_mode;

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_config_validate(unit, &pm_acc, &op_st, pm_drv_info,
                                         &spd_config, is_first);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pcfg->pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_port_autoneg_advert_set(int unit,
                            bcmpc_lport_t lport,
                            bcmpc_autoneg_prof_t *an_cfg,
                            bcmpc_port_cfg_t *pcfg)
{
    unsigned int size = 0;
    int num_ability = 0, index;
    pm_port_ability_t *an_ability = NULL;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    /* Allocate an_ability*/
    size = sizeof(pm_port_ability_t) * NUM_AUTONEG_PROFILES_MAX;
    SHR_ALLOC(an_ability, size, "bcmpcAnAbility");
    SHR_NULL_CHECK(an_ability, SHR_E_MEMORY);
    sal_memset(an_ability, 0, size);

    num_ability = pcfg->num_an_profs;

    for (index = 0; index < num_ability; index++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_ability_to_pm_ability(unit,
                                                   &an_cfg[index],
                                                   &an_ability[index]));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (port_drv_data_get(unit, pcfg->pport, pcfg, &drv_data));

    sal_memset(&pm_acc, 0, sizeof(pm_access_t));

    pm_acc.phy_acc = drv_data.phy_acc;
    pm_acc.core_acc = drv_data.phy_core_acc;
    pm_acc.pmac_acc = drv_data.pmac_acc;
    pm_acc.pmac_blkport = drv_data.pmac_blkport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, drv_data.pm_id, &pm_info));
    pm_drv = pm_info.prop.pm_drv;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_drv_info_get(unit, drv_data.pm_id, &pm_drv_info));
    pm_drv_info->pm_data.pmacd = drv_data.pmac_drv;

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_ability_advert_set(unit, &pm_acc, &op_st, pm_drv_info,
                                            num_ability, an_ability);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
exit:
    if (an_ability != NULL) {
        SHR_FREE(an_ability);
    }
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pcfg->pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_mac_control_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                    bcmpc_mac_control_t *mctrl,
                    bcmpc_mac_control_t default_mctrl)
{
    int rv = 0;
    uint32_t size = 0;
    bcmpc_mac_control_t old_mctrl;
    bcmpc_mac_control_t mac_oper;
    bcmpc_drv_t *pc_drv;
    bool tx = FALSE, rx = FALSE;
    bool is_supported = false;
    bool runt_threshold_auto_field = FALSE, runt_threshold_field = FALSE;
    bool runt_threshold_set = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&old_mctrl, 0, sizeof(old_mctrl));

    rv = bcmpc_db_imm_entry_lookup(unit, PC_MAC_CONTROLt, P(&lport),
                                   P(&old_mctrl));

    if (rv == SHR_E_NOT_FOUND) {
        /*
         * SHR_E_NOT_FOUND means there is no previous configuration.
         * Not set all attributes of mac control.
         */
        rv = SHR_E_NONE;
    }

    /* Get current MAC hardware settings for the port. */
    sal_memset(&mac_oper, 0, sizeof(mac_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_mac_control_get(unit, lport, pport, &mac_oper));

    /*
     * The following block of code handles the MAC TX enable setting.
     * when either the tx_enable_auto or the tx_enable LT field is
     * updated by the user. This information is present in the
     * fbmp (field bitmap).
     * Generally the MAC TX setting is only touched if tx_enable_auto
     * is FALSE or transitioned to FALSE from TRUE.
     * If tx_enable_auto == FALSE or transioned to FALSE from TRUE
     * and user also update the field tx_enable then user provisioned
     * value is applied to the hardware.
     *
     * If tx_enable_auto == TRUE
     * and user also update the field tx_enable then user provisioned
     * value is ignored and it a no-operation.
     *
     * If tx_enable_auto transioned to TRUE from FALSE
     * and user also update the field tx_enable then user provisioned
     * value is ignored and tx_enable is set to TRUE.
     */
    if ((SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE_AUTO)) ||
        (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE))) {
        bool set_tx = FALSE;

        /*
         * User is updating the TX enable setting, so get the tx_enable_auto
         * from the DB.
         */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE_AUTO)) {
            mctrl->tx_enable_auto = old_mctrl.tx_enable_auto;
        }

        /*
         * User is updating the TX enable auto setting, so get the tx_enable
         * from the DB.
         */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE)) {
            mctrl->tx_enable = old_mctrl.tx_enable;
        }

        if (mctrl->tx_enable_auto != old_mctrl.tx_enable_auto) {
            tx = (mctrl->tx_enable_auto == TRUE)? TRUE: mctrl->tx_enable;
            set_tx = TRUE;
        } else if (!mctrl->tx_enable_auto) {
            if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE)) {
                tx = mctrl->tx_enable;
                set_tx = TRUE;
            }
        } else {
            set_tx = FALSE;
        }

        if (set_tx == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mac_tx_enable_set(unit, pport, tx));
        }
    }

    /*
     * The following block of code handles the MAC RX enable setting.
     * when either the rx_enable_auto or the rx_enable LT field is
     * updated by the user. This information is present in the
     * fbmp (field bitmap).
     * Generally the MAC RX setting is only touched if rx_enable_auto
     * is FALSE or transitioned to FALSE from TRUE.
     * If rx_enable_auto == FALSE or transioned to FALSE from TRUE
     * and user also update the field rx_enable then user provisioned
     * value is applied to the hardware.
     *
     * If rx_enable_auto == TRUE
     * and user also update the field rx_enable then user provisioned
     * value is ignored and it a no-operation.
     *
     * If rx_enable_auto transioned to TRUE from FALSE
     * and user also update the field rx_enable then user provisioned
     * value is ignored and rx_enable is set to FALSE.
     */
    if ((SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE_AUTO)) ||
        (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE))) {
        bool set_rx = FALSE;

        /*
         * User is updating the RX enable setting, so get the rx_enable_auto
         * from the DB.
         */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE_AUTO)) {
            mctrl->rx_enable_auto = old_mctrl.rx_enable_auto;
        }

        /*
         * User is updating the RX enable auto setting, so get the rx_enable
         * from the DB.
         */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE)) {
            mctrl->rx_enable = old_mctrl.rx_enable;
        }

        if (mctrl->rx_enable_auto != old_mctrl.rx_enable_auto) {
            rx =  (mctrl->rx_enable_auto == TRUE)? FALSE: mctrl->rx_enable;
            set_rx = TRUE;
        } else if (!mctrl->rx_enable_auto) {
            if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE)) {
                rx = mctrl->rx_enable;
                set_rx = TRUE;
            }
        } else {
            set_rx = FALSE;
        }

        if (set_rx == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mac_rx_enable_set(unit, pport, rx));
        }
    }

    /* Check if the PAUSE settings are being updated. */
    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_REMOTE_FAULT_DISABLE) ||
        SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_LOCAL_FAULT_DISABLE)) {

        /* If Remote fault is not being updated, set it to to current value. */
        if (!SHR_BITGET(mctrl->fbmp,
                        BCMPC_MAC_CTRL_LT_FLD_REMOTE_FAULT_DISABLE)) {
            mctrl->rmt_fault_dis = old_mctrl.rmt_fault_dis;
        }

        /* If Local fault is not being updated, set it to to current value. */
        if (!SHR_BITGET(mctrl->fbmp,
                        BCMPC_MAC_CTRL_LT_FLD_LOCAL_FAULT_DISABLE)) {
            mctrl->local_fault_dis = old_mctrl.local_fault_dis;
        }

        if (PMGR_CFG_CHANGED(0, mctrl, &old_mctrl, local_fault_dis) ||
           (PMGR_CFG_CHANGED(0, mctrl, &old_mctrl, rmt_fault_dis))) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mac_fault_disable_set(unit, pport, mctrl->local_fault_dis,
                                       mctrl->rmt_fault_dis));
        }
    }

    if ((SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP)) ||
        (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP_AUTO))) {
        bcmpc_port_encap_t encap = BCMPC_PORT_ENCAP_COUNT;
        bool set_ifg = FALSE;
        uint8_t ifg = 0;

        /*
         * User is updating the IFG enable setting, so get the ifg_auto
         * from the DB.
         */
        if (!SHR_BITGET(mctrl->fbmp,
                        BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP_AUTO)) {
            mctrl->ifg_auto = old_mctrl.ifg_auto;
        }

        /*
         * User is updating the IFG auto setting, so get the IFG
         * from the DB.
         */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP)) {
            mctrl->ifg = old_mctrl.ifg;
        }

        if (mctrl->ifg_auto != old_mctrl.ifg_auto) {
            if (mctrl->ifg_auto == TRUE) {
                /* Get the port encapsulation type. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (port_encap_get(unit, pport, &encap));
                if (encap == BCMPC_PORT_ENCAP_IEEE) {
                    ifg = 0xc;
                } else if (encap == BCMPC_PORT_ENCAP_HG3) {
                    ifg = 0x8;
                }
            } else {
                ifg = mctrl->ifg;
            }
            set_ifg = TRUE;
        } else if (!mctrl->ifg_auto) {
            if (SHR_BITGET(mctrl->fbmp,
                BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP)) {
                ifg = mctrl->ifg;
                set_ifg = TRUE;
            }
        } else {
            set_ifg = FALSE;
        }

        if (set_ifg == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_avg_ipg_set(unit, pport, ifg));
        }
    }

    /* Check if OVERSIZE frame setting being updated. */
    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_OVERSIZE_PKT)) {
        if (PMGR_CFG_CHANGED(0, mctrl, &mac_oper, oversize_pkt)) {
            size = mctrl->oversize_pkt;
            SHR_IF_ERR_VERBOSE_EXIT(port_mib_oversize_set(unit, pport, size));
        }
    }

    /* Check if the PAUSE settings are being updated. */
    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_TX) ||
        SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_RX)) {

        /* If Pause TX is not being updated, set it to to current value. */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_TX)) {
            mctrl->pause_tx = old_mctrl.pause_tx;
        }

        /* If Pause RX is not being updated, set it to to current value. */
        if (!SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_RX)) {
            mctrl->pause_rx = old_mctrl.pause_rx;
        }

        if (PMGR_CFG_CHANGED(0, mctrl, &mac_oper, pause_tx) ||
            PMGR_CFG_CHANGED(0, mctrl, &mac_oper, pause_rx)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_pause_set(unit, pport, mctrl));
        }
    }

    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_PASS)) {
        if (PMGR_CFG_CHANGED(0, mctrl, &mac_oper, pause_pass)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_pause_frames_pass_set(unit, pport, mctrl->pause_pass));
        }
    }

    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_CONTROL_PASS)) {
        if (PMGR_CFG_CHANGED(0, mctrl, &mac_oper, control_pass)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_control_frames_pass_set(unit, pport,
                                              mctrl->control_pass));
        }
    }

    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_ADDR)) {
        if (!BCMPC_ARRAY_EQUAL(mctrl->pause_addr, &mac_oper.pause_addr)) {
            SHR_IF_ERR_VERBOSE_EXIT (port_pause_addr_set(unit, pport, mctrl));
        }
    }

    if (SHR_BITGET(mctrl->fbmp,
                   BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD_AUTO)) {
        runt_threshold_auto_field = TRUE;
    }
    if (SHR_BITGET(mctrl->fbmp,
                   BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD)) {
        runt_threshold_field = TRUE;
    }

    /*
     * RT_AUTO_FIELD = runt_threshold_auto_field
     * RT_AUTO = mctrl->runt_threshold_auto
     * OLD_RT_AUTO = old_mctrl.runt_threshold_auto
     * RT_FIELD = runt_threshold_field
     *
     * +---------------+---------+-------------+----------+------------+
     * | RT_AUTO_FIELD | RT_AUTO | OLD_RT_AUTO | RT_FIELD |   Action   |
     * +---------------+---------+-------------+----------+------------+
     * |      1        |   1     |      0      |    X     | Set Default|
     * |      1        |   0     |      1      |    0     | Set Old RT |
     * |      1        |   0     |      X      |    1     | Set New RT |
     * |      0        |   X     |      0      |    1     | Set New RT |
     * +---------------+---------+-------------+----------+------------+
     * RT = Runt Threshold
     *
     * All other combinations do not need any 'set' action for runt threshold
 */

    if (runt_threshold_auto_field) {
        if (mctrl->runt_threshold_auto && !old_mctrl.runt_threshold_auto) {
            /* Set default runt threshold */
            mctrl->runt_threshold = default_mctrl.runt_threshold;
            runt_threshold_set = TRUE;
        } else if (!runt_threshold_field && !mctrl->runt_threshold_auto) {
            /* Set old runt threshold */
            mctrl->runt_threshold = old_mctrl.runt_threshold;
            runt_threshold_set = TRUE;
        }
    }

    if (runt_threshold_field) {
        /* Set new runt threshold */
        if ((runt_threshold_auto_field && !mctrl->runt_threshold_auto) ||
            (!runt_threshold_auto_field && !old_mctrl.runt_threshold_auto)) {
            runt_threshold_set = TRUE;
        }
    }

    if (runt_threshold_set) {
            SHR_IF_ERR_VERBOSE_EXIT
                (port_runt_threshold_set(unit, pport, mctrl->runt_threshold));
    }

    /* Check if TX_STALL setting is being updated. */
    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_STALL_TX)) {
        if (PMGR_CFG_CHANGED(0, mctrl, &old_mctrl, stall_tx)) {
            /* Check if the feature is supported on the port. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_drv_get(unit, &pc_drv));

            if (pc_drv->is_stall_tx_supported) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (pc_drv->is_stall_tx_supported(unit, pport, &is_supported));
            }

            if (is_supported) {
                /*
                 * On a device, there could be ports on which this feature is
                 * not supported and return unavailable. Ignore the unavailable.
                 */
                SHR_IF_ERR_VERBOSE_EXIT
                    (port_tx_stall_enable_set(unit, pport, mctrl->stall_tx));
            }
        }
    }

    /* Check if RSV fields are  being updated. */
    if (SHR_BITGET(mctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_MAC_RSV_CONTROL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (port_rsv_mask_set(unit, pport, mctrl));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_mac_control_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                    bcmpc_mac_control_t *mst)
{
    bcmpmac_fault_status_t pmac_st;
    bcmpmac_fault_disable_t pmac_dis;
    bcmpc_drv_t *pc_drv;
    uint32_t rx_enable = 0, tx_enable = 0;
    uint32_t max_frame_size = 0;
    uint32_t lag_failover_enabled = 0;
    bool is_supported = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mac_enable_status_get(unit, pport, &tx_enable, &rx_enable));
    mst->rx_enable = (rx_enable)? true: false;
    mst->tx_enable = (tx_enable)? true: false;

    SHR_IF_ERR_VERBOSE_EXIT
        (port_pause_addr_get(unit, pport, mst->pause_addr));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_frame_max_get(unit, pport, &max_frame_size));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_failover_enable_get(unit, pport, &lag_failover_enabled));
    SHR_IF_ERR_VERBOSE_EXIT
        (port_avg_ipg_get(unit, pport, &mst->ifg));
    SHR_IF_ERR_VERBOSE_EXIT
        (mac_fault_status_get(unit, pport, &pmac_st));

    sal_memset(&pmac_dis, 0, sizeof(bcmpmac_fault_disable_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (mac_fault_disable_get(unit, pport, &pmac_dis));

    mst->local_fault_dis = pmac_dis.local_fault_disable? true: false;
    mst->rmt_fault_dis = pmac_dis.remote_fault_disable? true: false;

    SHR_IF_ERR_VERBOSE_EXIT
        (port_mib_oversize_get(unit, pport, &mst->oversize_pkt));

    SHR_IF_ERR_VERBOSE_EXIT(port_pause_get(unit, pport, (uint32_t *) &mst->pause_tx,
                                   (uint32_t *) &mst->pause_rx));

    SHR_IF_ERR_VERBOSE_EXIT(port_pause_frames_pass_get(unit, pport,
                                         (uint32_t *) &mst->pause_pass));

    SHR_IF_ERR_VERBOSE_EXIT(port_control_frames_pass_get(unit, pport,
                                      (uint32_t *) &mst->control_pass));

    SHR_IF_ERR_VERBOSE_EXIT
        (port_runt_threshold_get(unit, pport,
                                 (uint32_t *) &mst->runt_threshold));

    /* Check if the feature is supported on the port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv->is_stall_tx_supported) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->is_stall_tx_supported(unit, pport, &is_supported));
    }

    if (is_supported) {
        /*
         * On a device, there could be ports on which this feature is
         * not supported and return unavailable. Ignore the unavailable.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (port_tx_stall_enable_get(unit, pport, &mst->stall_tx));
        mst->stall_tx_status = mst->stall_tx? BCMPC_STALL_TX_ENABLE:
                                              BCMPC_STALL_TX_DISABLE;
    } else {
        mst->stall_tx_status = BCMPC_STALL_TX_NO_SUPPORT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (port_rsv_mask_get(unit, pport, mst));

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_phy_control_set(int unit, bcmpc_port_lane_t *port_lane, bcmpc_pport_t pport,
                    bcmpc_phy_control_t *pctrl)
{
    uint32_t phy_ctrl_flags = 0;
    int force_cfg = 0;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpc_phy_control_t old_pctrl;
    bcmpc_phy_control_t phy_ctrl;
    pm_phy_control_t  pm_pctrl;
    pm_phy_status_t  phy_stat;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    uint32_t phy_lane_mask = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    sal_memset(&old_pctrl, 0, sizeof(old_pctrl));
    sal_memset(&phy_ctrl, 0, sizeof(phy_ctrl));

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PHY_CONTROLt, P(port_lane),
                                   P(&old_pctrl));
    if (rv == SHR_E_NOT_FOUND) {
        /*
         * SHR_E_NOT_FOUND means there is no previous configuration.
         * To set attributes of phy_ctrl
         */
        force_cfg = 0;
        old_pctrl.tx_squelch_auto = 1;
        old_pctrl.rx_squelch_auto = 1;
        old_pctrl.rx_adpt_resume_auto = 1;
        old_pctrl.rx_afe_vga_auto = 1;
        old_pctrl.rx_afe_pk_flt_auto = 1;
        old_pctrl.rx_afe_low_fre_pk_flt_auto = 1;
        old_pctrl.rx_afe_hi_fre_pk_flt_auto = 1;
        old_pctrl.rx_afe_dfe_tap_auto = 1;
        old_pctrl.pam4_tx_pat_auto = 1;
        old_pctrl.pam4_tx_precoder_auto = 1;
        old_pctrl.tx_pi_ovride_auto = 1;
        old_pctrl.phy_ecc_intr_auto = 1;
        old_pctrl.phy_fec_bypass_ind_auto = 1;
    }

    /*
     * Copy the old IMM DB values to the local phy control data structure
     * This structure is updated by the below logic and some of the
     * information is present in the IMM DB.
     */
    sal_memcpy(&phy_ctrl, &old_pctrl, sizeof(phy_ctrl));

    /*
     * Check if any of the auto LT fields are set in the field bitmap,
     * indicating that the LT field is being configured by the user.
     * If field bitmap is set, use the value configured by user. If the
     * value is 0, need to honor the value given by user.
     * If not set, use old values, from the imm DB, of the auto fields.
     */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_TX_SQUELCH_AUTO)) {
        phy_ctrl.tx_squelch_auto = pctrl->tx_squelch_auto;
        if ((!phy_ctrl.tx_squelch_auto) &&
            (phy_ctrl.tx_squelch_auto != old_pctrl.tx_squelch_auto)) {
            phy_ctrl_flags |= PM_PHY_CTRL_F_TX_SQUELCH;
        }
    } else {
        phy_ctrl.tx_squelch_auto = old_pctrl.tx_squelch_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_SQUELCH_AUTO)) {
        phy_ctrl.rx_squelch_auto = pctrl->rx_squelch_auto;
        if ((!phy_ctrl.rx_squelch_auto) &&
            (phy_ctrl.rx_squelch_auto != old_pctrl.rx_squelch_auto)) {
            phy_ctrl_flags |= PM_PHY_CTRL_F_RX_SQUELCH;
        }
    } else {
        phy_ctrl.rx_squelch_auto = old_pctrl.rx_squelch_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_ADAPTION_RESUME_AUTO)) {
        phy_ctrl.rx_adpt_resume_auto = pctrl->rx_adpt_resume_auto;
        if ((!phy_ctrl.rx_adpt_resume_auto) &&
            (phy_ctrl.rx_adpt_resume_auto != old_pctrl.rx_adpt_resume_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_RX_ADPT_RESUME;
        }
    } else {
        phy_ctrl.rx_adpt_resume_auto = old_pctrl.rx_adpt_resume_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_VGA_AUTO)) {
        phy_ctrl.rx_afe_vga_auto = pctrl->rx_afe_vga_auto;
        if ((!phy_ctrl.rx_afe_vga_auto) &&
            (phy_ctrl.rx_afe_vga_auto != old_pctrl.rx_afe_vga_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_VGA;
        }
    } else {
        phy_ctrl.rx_afe_vga_auto = old_pctrl.rx_afe_vga_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_PEAKING_FILTER_AUTO)) {
        phy_ctrl.rx_afe_pk_flt_auto = pctrl->rx_afe_pk_flt_auto;
        if ((!phy_ctrl.rx_afe_pk_flt_auto) &&
            (phy_ctrl.rx_afe_pk_flt_auto != old_pctrl.rx_afe_pk_flt_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_PK_FLT;
        }
    } else {
        phy_ctrl.rx_afe_pk_flt_auto = old_pctrl.rx_afe_pk_flt_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER_AUTO)) {
        phy_ctrl.rx_afe_low_fre_pk_flt_auto = pctrl->rx_afe_low_fre_pk_flt_auto;
        if ((!phy_ctrl.rx_afe_low_fre_pk_flt_auto) &&
            (phy_ctrl.rx_afe_low_fre_pk_flt_auto !=
                            old_pctrl.rx_afe_low_fre_pk_flt_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT;
        }
    } else {
        phy_ctrl.rx_afe_low_fre_pk_flt_auto =
                                         old_pctrl.rx_afe_low_fre_pk_flt_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER_AUTO)) {
        phy_ctrl.rx_afe_hi_fre_pk_flt_auto = pctrl->rx_afe_hi_fre_pk_flt_auto;
        if (!phy_ctrl.rx_afe_hi_fre_pk_flt_auto) {
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT;
        }
    } else {
        phy_ctrl.rx_afe_hi_fre_pk_flt_auto =
                                         old_pctrl.rx_afe_hi_fre_pk_flt_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_DFE_TAP_AUTO)) {
        phy_ctrl.rx_afe_dfe_tap_auto = pctrl->rx_afe_dfe_tap_auto;
        if ((!phy_ctrl.rx_afe_dfe_tap_auto) &&
            (phy_ctrl.rx_afe_dfe_tap_auto != old_pctrl.rx_afe_dfe_tap_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_DFE_TAP;
        }
    } else {
        phy_ctrl.rx_afe_dfe_tap_auto = old_pctrl.rx_afe_dfe_tap_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_PAM4_TX_PATTERN_AUTO)) {
        phy_ctrl.pam4_tx_pat_auto = pctrl->pam4_tx_pat_auto;
        if ((!phy_ctrl.pam4_tx_pat_auto) &&
            (phy_ctrl.pam4_tx_pat_auto != old_pctrl.pam4_tx_pat_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_PAM4_TX_PATTERN;
        }
    } else {
        phy_ctrl.pam4_tx_pat_auto = old_pctrl.pam4_tx_pat_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_PAM4_TX_PRECODER_AUTO)) {
        phy_ctrl.pam4_tx_precoder_auto = pctrl->pam4_tx_precoder_auto;
        if ((!phy_ctrl.pam4_tx_precoder_auto) &&
            (phy_ctrl.pam4_tx_precoder_auto !=
                                 old_pctrl.pam4_tx_precoder_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_PAM4_TX_PRECODER;
        }
    } else {
        phy_ctrl.pam4_tx_precoder_auto = old_pctrl.pam4_tx_precoder_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_PHY_ECC_INTR_EN_AUTO)) {
        phy_ctrl.phy_ecc_intr_auto = pctrl->phy_ecc_intr_auto;
        if ((!phy_ctrl.phy_ecc_intr_auto) &&
            (phy_ctrl.phy_ecc_intr_auto != old_pctrl.phy_ecc_intr_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_ECC_INTR_ENABLE;
        }
    } else {
        phy_ctrl.phy_ecc_intr_auto = old_pctrl.phy_ecc_intr_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_TX_PI_FREQ_OVERRIDE_AUTO)) {
        phy_ctrl.tx_pi_ovride_auto = pctrl->tx_pi_ovride_auto;
        if ((!phy_ctrl.tx_pi_ovride_auto) &&
            (phy_ctrl.tx_pi_ovride_auto != old_pctrl.tx_pi_ovride_auto)) {
            phy_ctrl_flags |= PHY_CTRL_F_TX_PI_OVERRIDE;
        }
    } else {
        phy_ctrl.tx_pi_ovride_auto = old_pctrl.tx_pi_ovride_auto;
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_FEC_BYPASS_INDICATION_AUTO)) {
        phy_ctrl.phy_fec_bypass_ind_auto = pctrl->phy_fec_bypass_ind_auto;
        if ((!phy_ctrl.phy_fec_bypass_ind_auto) &&
            (phy_ctrl.phy_fec_bypass_ind_auto !=
                                    old_pctrl.phy_fec_bypass_ind_auto)) {
            phy_ctrl_flags |= PM_PHY_CTRL_F_FEC_BYPASS_IND;
        }
    } else {
        phy_ctrl.phy_fec_bypass_ind_auto = old_pctrl.phy_fec_bypass_ind_auto;
    }

    /*
     * The code below does the following.
     * 1. Is field bitmask set indicating user configured LT field.
     * 2. Check if the value changed from the previous value in DB.
     * 3. If changed, apply to hardware if associated auto field is 0.
     *    else ignore.
     * 4. Check if correspoding PHY_FLAG is set, indicating that the
     *    user just initiated auto to 0, apply the value to hardware.
     */

    /* Check TX squelch auto or TX squelch is set in the bitmap. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_TX_SQUELCH) &&
        (!phy_ctrl.tx_squelch_auto)) {
         if (pctrl->tx_squelch != old_pctrl.tx_squelch) {
             phy_ctrl.tx_squelch = pctrl->tx_squelch;
             phy_ctrl_flags |= PHY_CTRL_F_TX_SQUELCH;
         }
    }

    /* Check RX squelch auto or RX squelch is set in the bitmap. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_SQUELCH) &&
        (!phy_ctrl.rx_squelch_auto)) {
        if (pctrl->rx_squelch != old_pctrl.rx_squelch) {
            phy_ctrl.rx_squelch = pctrl->rx_squelch;
            phy_ctrl_flags |= PHY_CTRL_F_RX_SQUELCH;
        }
    }

    /* Check RX Adaptation resume auto or TX squelch is set in the bitmap. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_ADAPTION_RESUME) &&
        (!phy_ctrl.rx_adpt_resume_auto)) {
        if (pctrl->rx_adpt_resume != old_pctrl.rx_adpt_resume) {
            phy_ctrl.rx_adpt_resume = pctrl->rx_adpt_resume;
            phy_ctrl_flags |= PHY_CTRL_F_RX_ADPT_RESUME;
        }
    }

    /* Check RX AFE VGA auto or TX squelch is set in the bitmap. */
    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_VGA) &&
        (!phy_ctrl.rx_afe_vga_auto)) {
        if (pctrl->rx_afe_vga != old_pctrl.rx_afe_vga) {
            phy_ctrl.rx_afe_vga = pctrl->rx_afe_vga;
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_VGA;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_PEAKING_FILTER) &&
        (!phy_ctrl.rx_afe_pk_flt_auto)) {
        if (pctrl->rx_afe_pk_flt != old_pctrl.rx_afe_pk_flt) {
            phy_ctrl.rx_afe_pk_flt = pctrl->rx_afe_pk_flt;
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_PK_FLT;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_LO_FREQ_PEAKING_FILTER) &&
        (!phy_ctrl.rx_afe_low_fre_pk_flt_auto)) {
        if (pctrl->rx_afe_low_fre_pk_flt != old_pctrl.rx_afe_low_fre_pk_flt) {
            phy_ctrl.rx_afe_low_fre_pk_flt = pctrl->rx_afe_low_fre_pk_flt;
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_LOW_FRE_PK_PLT;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_HI_FREQ_PEAKING_FILTER) &&
        (!phy_ctrl.rx_afe_hi_fre_pk_flt_auto)) {
        if (pctrl->rx_afe_hi_fre_pk_flt != old_pctrl.rx_afe_hi_fre_pk_flt) {
            phy_ctrl.rx_afe_hi_fre_pk_flt = pctrl->rx_afe_hi_fre_pk_flt;
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_HI_FRE_PK_PLT;
        }
    }

    if ((SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_DFE_TAP) ||
        SHR_BITGET(pctrl->fbmp, BCMPC_FLD_RX_AFE_DFE_TAP_SIGN)) &&
        (!phy_ctrl.rx_afe_dfe_tap_auto)) {
        if (PMGR_CFG_ARR_CHANGED(0, pctrl, &old_pctrl, rx_afe_dfe_tap,
                                  BCMPC_NUM_RX_DFE_TAPS_MAX)) {
            sal_memcpy(&phy_ctrl.rx_afe_dfe_tap, pctrl->rx_afe_dfe_tap,
                        sizeof(phy_ctrl.rx_afe_dfe_tap));
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_DFE_TAP;
        }

        if (PMGR_CFG_ARR_CHANGED(0, pctrl, &old_pctrl, rx_afe_dfe_tap_sign,
                         BCMPC_NUM_RX_DFE_TAPS_MAX)) {
            sal_memcpy(&phy_ctrl.rx_afe_dfe_tap_sign,
                       pctrl->rx_afe_dfe_tap_sign,
                       sizeof(phy_ctrl.rx_afe_dfe_tap_sign));
            phy_ctrl_flags |= PHY_CTRL_F_RX_AFE_DFE_TAP;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_PAM4_TX_PATTERN) &&
        (!phy_ctrl.pam4_tx_pat_auto)) {
        if (pctrl->pam4_tx_pat != old_pctrl.pam4_tx_pat) {
            phy_ctrl.pam4_tx_pat = pctrl->pam4_tx_pat;
            phy_ctrl_flags |= PHY_CTRL_F_PAM4_TX_PATTERN;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_PAM4_TX_PRECODER) &&
        (!phy_ctrl.pam4_tx_precoder_auto)) {
        if (pctrl->pam4_tx_precoder != old_pctrl.pam4_tx_precoder) {
            phy_ctrl.pam4_tx_precoder = pctrl->pam4_tx_precoder;
            phy_ctrl_flags |= PHY_CTRL_F_PAM4_TX_PRECODER;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_PHY_ECC_INTR_EN) &&
        (!phy_ctrl.phy_ecc_intr_auto)) {
        if (pctrl->phy_ecc_intr != old_pctrl.phy_ecc_intr) {
            phy_ctrl.phy_ecc_intr = pctrl->phy_ecc_intr;
            phy_ctrl_flags |= PHY_CTRL_F_ECC_INTR_ENABLE;
        }
    }

    if ((SHR_BITGET(pctrl->fbmp, BCMPC_FLD_TX_PI_FREQ_OVERRIDE) ||
        SHR_BITGET(pctrl->fbmp, BCMPC_FLD_TX_PI_FREQ_OVERRIDE_SIGN)) &&
        (!phy_ctrl.tx_pi_ovride_auto)) {
        /* Check if TX PI override value changed from old value. */
        if (pctrl->tx_pi_ovride != old_pctrl.tx_pi_ovride) {
            phy_ctrl.tx_pi_ovride = pctrl->tx_pi_ovride;
            phy_ctrl_flags |= PHY_CTRL_F_TX_PI_OVERRIDE;
        }

        /* Check if TX PI override value sign changed from old value. */
        if (pctrl->tx_pi_ovride_sign != old_pctrl.tx_pi_ovride_sign) {
            phy_ctrl.tx_pi_ovride_sign = pctrl->tx_pi_ovride_sign;
            phy_ctrl_flags |= PHY_CTRL_F_TX_PI_OVERRIDE;
        }
    }

    if (SHR_BITGET(pctrl->fbmp, BCMPC_FLD_FEC_BYPASS_INDICATION) &&
        (!phy_ctrl.phy_fec_bypass_ind_auto)) {
        if (pctrl->phy_fec_bypass_ind != old_pctrl.phy_fec_bypass_ind) {
            phy_ctrl.phy_fec_bypass_ind = pctrl->phy_fec_bypass_ind;
            phy_ctrl_flags |= PM_PHY_CTRL_F_FEC_BYPASS_IND;
        }
    }

    if (PMGR_CFG_CHANGED(force_cfg, pctrl, &old_pctrl,
                         pmd_debug_lane_event_log_level)) {
        if (bcmdrd_feature_is_real_hw(unit) &&
            !bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
                phy_ctrl.pmd_debug_lane_event_log_level = pctrl->pmd_debug_lane_event_log_level;
                phy_ctrl_flags |= PM_PHY_CTRL_F_PMD_DEBUG_LANE_EVENT_LOG_LEVEL;
        }
    }

    if (!phy_ctrl_flags) {
        return SHR_E_NONE;
    }
    bcmpc_pmgr_phy_ctrl_to_pm_phy_ctrl(unit, &phy_ctrl, &pm_pctrl);

    SHR_IF_ERR_VERBOSE_EXIT
        (phy_lane_mask_set(pm_acc.phy_acc.access.lane_mask,
                           port_lane->lane_index,
                           &phy_lane_mask));

    pm_acc.phy_acc.access.lane_mask = phy_lane_mask;

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_phy_status_get(unit, &pm_acc, &op_st,
                                   pm_drv_info, &phy_stat);

    if (SHR_FAILURE(rv)) {
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    /* Get current PAM4 pattern before disable. */
    if (phy_ctrl.pam4_tx_pat == BCMPC_PAM4_PATTERN_NONE) {
        rv = pm_drv->pm_phy_status_get(unit, &pm_acc, &op_st,
                                       pm_drv_info, &phy_stat);
        if (SHR_FAILURE(rv)) {
            PM_UNLOCK(pm_drv_info);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        pm_pctrl.pam4_tx_pat = phy_stat.pam4_tx_pat;
    }
    rv = pm_drv->pm_phy_control_set(unit, &pm_acc, &op_st, pm_drv_info,
                                    phy_ctrl_flags, &pm_pctrl);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_phy_status_get(int unit, bcmpc_port_lane_t *port_lane, bcmpc_pport_t pport,
                   bcmpc_pmgr_phy_status_t *pst)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_phy_status_t  phy_stat;
    int rv = SHR_E_NONE;
    uint32_t phy_lane_mask = 0;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
      (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (phy_lane_mask_set(pm_acc.phy_acc.access.lane_mask,
                           port_lane->lane_index,
                           &phy_lane_mask));

    pm_acc.phy_acc.access.lane_mask = phy_lane_mask;
    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_phy_status_get(unit, &pm_acc, &op_st, pm_drv_info, &phy_stat);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    bcmpc_pmgr_phy_status_from_pm_phy_status(unit, &phy_stat, pst);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_tx_taps_config_set(int unit, bcmpc_port_lane_t *port_lane,
                       bcmpc_pport_t pport,
                       bcmpc_tx_taps_config_t *taps_cfg)
{
    int lane_idx = 0, force_cfg = 0;
    uint8_t tx_taps_cfg_flags = 0;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    phymod_tx_t phymod_tx;
    bcmpc_tx_taps_config_t old_taps_cfg;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    phymod_tx_t default_phymod_tx_taps;
    bcmpc_tx_taps_config_t default_tx_taps;
    pm_speed_config_t speed_config;
    int start_lane = 0, num_lanes = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* get default TX Taps config */
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_speed_config_get(unit, &pm_acc, &op_st, pm_drv_info,
                                          &speed_config);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_default_tx_taps_config_get(unit, &pm_acc, &op_st,
                                           pm_drv_info,
                                           &speed_config, &default_phymod_tx_taps);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* convert PHYMOD TX TAPs config to PC TX TAPs config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_tx_lane_status_from_phy(unit,
            &default_phymod_tx_taps, &default_tx_taps));

    sal_memset(&old_taps_cfg, 0, sizeof(old_taps_cfg));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_lt_entry_get(unit, PC_TX_TAPSt, P(port_lane),
                            P(&old_taps_cfg)));

    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, pre_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, pre) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, pre_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, pre2_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, pre2) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, pre2_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, main_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, main) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, main_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post2_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post2) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post2_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post3_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post3) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, post3_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, rpara_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, rpara) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, rpara_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, amp_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, amp) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, amp_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, drv_mode_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, drv_mode) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, drv_mode_sign)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, sig_mode_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, sig_mode)) {
        tx_taps_cfg_flags++;
    }
    if (PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, txfir_tap_mode_auto) ||
        PMGR_CFG_CHANGED(force_cfg, taps_cfg, &old_taps_cfg, txfir_tap_mode)) {
        tx_taps_cfg_flags++;
    }
    if (!tx_taps_cfg_flags) {
        return SHR_E_NONE;
    } else {
        if (taps_cfg->pre_auto) {
            taps_cfg->pre = default_tx_taps.pre;
            taps_cfg->pre_sign = default_tx_taps.pre_sign;
        }
        if (taps_cfg->pre2_auto) {
            taps_cfg->pre2 = default_tx_taps.pre2;
            taps_cfg->pre2_sign = default_tx_taps.pre2_sign;
        }
        if (taps_cfg->main_auto) {
            taps_cfg->main = default_tx_taps.main;
            taps_cfg->main_sign = default_tx_taps.main_sign;
        }
        if (taps_cfg->post_auto) {
            taps_cfg->post = default_tx_taps.post;
            taps_cfg->post_sign = default_tx_taps.post_sign;
        }
        if (taps_cfg->post2_auto) {
            taps_cfg->post2 = default_tx_taps.post2;
            taps_cfg->post2_sign = default_tx_taps.post2_sign;
        }
        if (taps_cfg->post3_auto) {
            taps_cfg->post3 = default_tx_taps.post3;
            taps_cfg->post3_sign = default_tx_taps.post3_sign;
        }
        if (taps_cfg->rpara_auto) {
            taps_cfg->rpara = default_tx_taps.rpara;
            taps_cfg->rpara_sign = default_tx_taps.rpara_sign;
        }
        if (taps_cfg->amp_auto) {
            taps_cfg->amp = default_tx_taps.amp;
            taps_cfg->amp_sign = default_tx_taps.amp_sign;
        }
        if (taps_cfg->drv_mode_auto) {
            taps_cfg->drv_mode = default_tx_taps.drv_mode;
            taps_cfg->drv_mode_sign = default_tx_taps.drv_mode_sign;
        }
        if (taps_cfg->sig_mode_auto) {
            taps_cfg->sig_mode = default_tx_taps.sig_mode;
        }
        if (taps_cfg->txfir_tap_mode_auto) {
            taps_cfg->txfir_tap_mode = default_tx_taps.txfir_tap_mode;
        }
    }

    /* Setup phymod_phy_access_t. */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));
    lane_idx = port_lane->lane_index + start_lane;

    pm_acc.phy_acc.access.lane_mask = 1 << lane_idx;

    phymod_tx_t_init(&phymod_tx);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_tx_taps_cfg_to_phy(unit, taps_cfg, &phymod_tx));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_tx_set(unit, &pm_acc, &op_st, pm_drv_info, &phymod_tx);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }

    SHR_FUNC_EXIT();
}

static int
pmm_tx_taps_status_get(int unit, bcmpc_port_lane_t *port_lane,
                       bcmpc_pport_t pport,
                       bcmpc_pmgr_tx_taps_status_t *taps_st)
{
    int lane_idx = 0;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    phymod_tx_t phymod_tx;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    int start_lane = 0, num_lanes = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* Setup phymod_phy_access_t. */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));
    lane_idx = port_lane->lane_index + start_lane;
    pm_acc.phy_acc.access.lane_mask = 1 << lane_idx;

    phymod_tx_t_init(&phymod_tx);

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_tx_get(unit, &pm_acc, &op_st, pm_drv_info, &phymod_tx);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_tx_lane_status_from_phy(unit, &phymod_tx, taps_st));

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_pmd_firmware_status_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                            bcmpc_pmgr_pmd_firmware_status_t *fm_st)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    phymod_firmware_lane_config_t pfm_cfg;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    int start_lane = 0;
    int i = 0, num_lanes = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* Setup phymod_phy_access_t. */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));

    phymod_firmware_lane_config_t_init(&pfm_cfg);

    PM_LOCK(pm_drv_info);

    for (i = 0; i < num_lanes; i++) {
        phymod_firmware_lane_config_t local;

        /*
         * Currently PMD lane config get API is lane based
         * The Normal Reach and the Extended Reach parameters
         * are lane based, to get these properties for a
         * logical port, the PMD lane config should be read
         * for all lanes. All the properties except the
         * Normal and Extended Reach will remain same.
         * So having if/else condition where 1st ieration
         * reads all the properties and for the subsequent
         * reads only the Noram and Extended Reach properties
         * are retrieved.
         */
        pm_acc.phy_acc.access.lane_mask = 1 << (start_lane + i);
        if (i == 0) {
            rv |= pm_drv->pm_port_pmd_lane_cfg_get(unit, &pm_acc, &op_st,
                                                   pm_drv_info, &pfm_cfg);
        } else {
            phymod_firmware_lane_config_t_init(&local);
            rv |= pm_drv->pm_port_pmd_lane_cfg_get(unit, &pm_acc, &op_st,
                                                   pm_drv_info, &local);
            pfm_cfg.ForceNormalReach |= local.ForceNormalReach << i;
            pfm_cfg.ForceExtenedReach |= local.ForceExtenedReach << i;
        }
    }
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_lane_config_from_phy(unit, &pfm_cfg, fm_st));
exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

/*
 * PMD Firmware configuration setting rules.
 * This function is only invoked during the run-time for
 * PMD firmware configuration updates, based on this
 * following are the steps to to update these settings
 * to hardware.
 * 1. Perform a DB lookup on PMD_FIRWARE LT to get the
 *    existing (previous settings).
 * 2. Perform a hardware read to get the current PMD firmware
 *    settings from the hardware, these will be used to populate
 *    settings marked as auto-configure.
 * 3. From the DB data, identify the settings which are
 *    auto-configure and settings which are manually
 *    updated by the user by checking the auto-configure
 *    settings associated with each setting.
 * 4. If a field is auto-configure, do not update the
 *    the corresponding field in the data structure with
 *    the user supplied value, instead get the value from
 *    the hardware, configured by the PortMacro driver.
 * 5. If a field is not auto-configure, update he data
 *    structure with the user supplied value.
 * 6. Invalid configuration is rejected and the PC_PORT_INFO
 *    LT is updated with the status.
 */

static int
pmm_pmd_firmware_config_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                            bcmpc_pmd_firmware_config_t *fm_cfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    phymod_firmware_lane_config_t pfm_cfg;
    bcmpc_pmd_firmware_config_t pmd_db_lookup;
    bcmpc_pmd_firmware_config_t pmd_cfg;
    bcmpc_pmgr_pmd_firmware_status_t pmd_driver_default;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    bcmpc_port_cfg_t pcfg;
    int start_lane = 0, num_lanes = 0, i = 0;
    bool cfg_apply_hw = FALSE;
    pm_speed_config_t speed_config;
    phymod_firmware_lane_config_t default_pmd_lane_config;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&pmd_cfg, 0, sizeof(bcmpc_pmd_firmware_config_t));
    sal_memset(&pmd_db_lookup, 0, sizeof(bcmpc_pmd_firmware_config_t));
    sal_memset(&pmd_driver_default, 0, sizeof(bcmpc_pmgr_pmd_firmware_status_t));
    phymod_firmware_lane_config_t_init(&default_pmd_lane_config);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (port_lane_info_get(pm_acc.phy_acc.access.lane_mask,
                            &start_lane, &num_lanes));

    sal_memset(&pcfg, 0, sizeof(bcmpc_port_cfg_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PMD_FIRMWAREt, P(&lport),
                                   P(&pmd_db_lookup)));


    /* get default PMD FIRMWARE Config */
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_speed_config_get(unit, &pm_acc, &op_st, pm_drv_info,
                                          &speed_config);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_default_pmd_lane_config_get(unit, &pm_acc, &op_st,
                                           pm_drv_info,
                                           &speed_config, &default_pmd_lane_config);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    /* convert PHYMOD PMD FIRMWARE lane config to PC PMD FIRMWARE lane config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_lane_config_from_phy(unit,
            &default_pmd_lane_config, &pmd_driver_default));
    /*
     * Default PMD lane configuration is per-lane based.
     * Need to set normal reach and extended reach bitmask for all the lanes
     * based on the default value.
     */
    for (i = 0; i < num_lanes; i++) {
        pmd_driver_default.norm_reach_pam4_bmask |=
                                default_pmd_lane_config.ForceNormalReach << i;
        pmd_driver_default.ext_reach_pam4_bmask |=
                                default_pmd_lane_config.ForceExtenedReach << i;
    }

    /* Check the PMD_FIRMWARE configuration mode of individual fields. */

    /* Is DFE auto or DFE field is being updated by the user. */
    if ((SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_DFE_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_DFE))) {

        /*
         * User is updating the DFE setting, so get the dfe_auto setting
         * from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_DFE_AUTO)) {
            fm_cfg->dfe_auto = pmd_db_lookup.dfe_auto;
        }

        /*
         * User is updating the DFE auto setting, so get the DFE
         * from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_DFE)) {
            fm_cfg->dfe = pmd_db_lookup.dfe;
        }

        if (fm_cfg->dfe_auto != pmd_db_lookup.dfe_auto) {
            /* If DFE setting is auto configure, use the configured hw value. */
            pmd_cfg.dfe = (fm_cfg->dfe_auto == TRUE)?
                                             pmd_driver_default.dfe: fm_cfg->dfe;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->dfe_auto) {
            /* DFE setting is user controlled. */
            if (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_DFE)) {
                pmd_cfg.dfe = fm_cfg->dfe;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* DFE auto or DFE field is not updated by the user */
        /*
         * if dfe_auto is 1,
         *    use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.dfe_auto) {
            pmd_cfg.dfe = pmd_driver_default.dfe;
        } else {
            pmd_cfg.dfe = pmd_db_lookup.dfe;
        }
    }

    /* Is LP_DFE auto or LP_DFE field is being updated by the user. */
    if ((SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_DFE_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_DFE))) {
        /*
         * User is updating the LP_DFE setting, so get the lp_dfe_auto setting
         * from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_DFE_AUTO)) {
            fm_cfg->lp_dfe_auto = pmd_db_lookup.lp_dfe_auto;
        }

        /*
         * User is updating the LP_DFE auto setting, so get the LP_DFE
         * from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_DFE)) {
            fm_cfg->lp_dfe = pmd_db_lookup.lp_dfe;
        }

        if (fm_cfg->lp_dfe_auto != pmd_db_lookup.lp_dfe_auto) {
            /*
             * If LP_DFE setting is auto configure, use the
             * configured hw value.
             */
            pmd_cfg.lp_dfe = (fm_cfg->lp_dfe_auto == TRUE)?
                                        pmd_driver_default.lp_dfe: fm_cfg->lp_dfe;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->lp_dfe_auto) {
            if (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_DFE)) {
                pmd_cfg.lp_dfe = fm_cfg->lp_dfe;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* LP_DFE auto or LP_DFE field is not updated by the user */
        /*
         * if lp_dfe_auto is 1,
         *    use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.lp_dfe_auto) {
            pmd_cfg.lp_dfe = pmd_driver_default.lp_dfe;
        } else {
            pmd_cfg.lp_dfe = pmd_db_lookup.lp_dfe;
        }
    }

    /* Is MEDIUM_TYPE auto or MEDIUM_TYPE field is being updated by the user. */
    if ((SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE))) {
        /*
         * User is updating the MEDIUM_TYPE setting, so get the
         * medium auto type setting from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE_AUTO)) {
            fm_cfg->medium_auto = pmd_db_lookup.medium_auto;
        }

        /*
         * User is updating the MEDIUM_TYPE auto setting, so get the MEDIUM_TYPE
         * from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE)) {
            fm_cfg->medium = pmd_db_lookup.medium;
        }

        if (fm_cfg->medium_auto != pmd_db_lookup.medium_auto) {
            /*
             * If MEDIUM type setting is auto configure, use the
             * configured hw value.
             */
            pmd_cfg.medium = (fm_cfg->medium_auto == TRUE)?
                                   pmd_driver_default.medium: fm_cfg->medium;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->medium_auto) {
            if (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_MEDIUM_TYPE)) {
                pmd_cfg.medium = fm_cfg->medium;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* MEDIUM_TYPE auto or MEDIUM_TYPE field is not updated by the user */
        /*
         * if medium_auto is 1,
         *     use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.medium_auto) {
            pmd_cfg.medium = pmd_driver_default.medium;
        } else {
            pmd_cfg.medium = pmd_db_lookup.medium;
        }
    }

    /*
     * Is SCRAMBLING_ENABLE auto or SCRAMBLING_ENABLE field is
     * being updated by the user.
     */
    if ((SHR_BITGET(fm_cfg->fbmp,
                    BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE))) {
        /*
         * User is updating the SCRAMBLING_ENABLE setting, so get the
         * scrambling_en setting from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE_AUTO)) {
            fm_cfg->scrambling_en_auto = pmd_db_lookup.scrambling_en_auto;
        }

        /*
         * User is updating the SCRAMBLING_ENABLE auto setting,
         * so get the SCRAMBLING_ENABLE from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE)) {
            fm_cfg->scrambling_en = pmd_db_lookup.scrambling_en;
        }

        if (fm_cfg->scrambling_en_auto != pmd_db_lookup.scrambling_en_auto) {
            /*
             * If SCRAMBLING_EN setting is auto configure, use the
             * configured hw value.
             */
            pmd_cfg.scrambling_en = (fm_cfg->scrambling_en_auto == TRUE)?
                                                pmd_driver_default.scrambling_en:
                                                fm_cfg->scrambling_en;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->scrambling_en_auto) {
            if (SHR_BITGET(fm_cfg->fbmp,
                           BCMPC_PMD_FW_LT_FLD_SCRAMBLING_ENABLE)) {
                pmd_cfg.scrambling_en = fm_cfg->scrambling_en;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* SCRAMBLING_ENABLE or _auto field is not updated by the user. */
        /*
         * if scrambling_en_auto is 1,
         *    use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.scrambling_en_auto) {
            pmd_cfg.scrambling_en = pmd_driver_default.scrambling_en;
        } else {
            pmd_cfg.scrambling_en = pmd_db_lookup.scrambling_en;
        }
    }

    /*
     * Is CL72_RESTART_TIMEOUT_EN auto or CL72_RESTART_TIMEOUT_EN field is
     * being updated by the user.
     */
    if ((SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN))) {
        /*
         * User is updating the CL72_RESTART_TIMEOUT_EN setting, so get the
         * cl72_res_to_auto setting from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN_AUTO)) {
            fm_cfg->cl72_res_to_auto = pmd_db_lookup.cl72_res_to_auto;
        }

        /*
         * User is updating the CL72_RSTRT_TO_EN auto setting,
         * so get the CL72_RSTRT_TO_EN from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN)) {
            fm_cfg->cl72_res_to = pmd_db_lookup.cl72_res_to;
        }

        if (fm_cfg->cl72_res_to_auto != pmd_db_lookup.cl72_res_to_auto) {
            /*
             * If CL72_RSTRT_TO_EN setting is auto configure, use the
             * configured hw value.
             */
            pmd_cfg.cl72_res_to = (fm_cfg->cl72_res_to_auto == TRUE)?
                                                pmd_driver_default.cl72_res_to:
                                                fm_cfg->cl72_res_to;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->cl72_res_to_auto) {
            if (SHR_BITGET(fm_cfg->fbmp,
                           BCMPC_PMD_FW_LT_FLD_CL72_RSTRT_TO_EN)) {
                pmd_cfg.cl72_res_to = fm_cfg->cl72_res_to;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* CL72_RESTART_TIMEOUT_EN or _auto field is not updated by the user.*/
        /*
         * if cl72_res_to_en_auto is 1,
         *    use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.cl72_res_to_auto) {
            pmd_cfg.cl72_res_to = pmd_driver_default.cl72_res_to;
        } else {
            pmd_cfg.cl72_res_to = pmd_db_lookup.cl72_res_to;
        }
    }

    /*
     * Is LP_TX_PRECODER_ON auto or LP_TX_PRECODER_ON field is
     * being updated by the user.
     */
    if ((SHR_BITGET(fm_cfg->fbmp,
                     BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON))) {
        /*
         * User is updating the LP_TX_PRECODER_ON setting, so get the
         * lp_tx_precoder_on_auto setting from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON_AUTO)) {
            fm_cfg->lp_tx_precoder_on_auto = pmd_db_lookup.lp_tx_precoder_on_auto;
        }

        /*
         * User is updating the LP_TX_PRECODER_ON auto setting,
         * so get the LP_TX_PRECODER_ON from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON)) {
            fm_cfg->lp_tx_precoder_on = pmd_db_lookup.lp_tx_precoder_on;
        }

        if (fm_cfg->lp_tx_precoder_on_auto !=
                                 pmd_db_lookup.lp_tx_precoder_on_auto) {
            /*
             * If LP_TX_PRECODER_ON setting is auto configure, use the
             * configured hw value.
             */
            pmd_cfg.lp_tx_precoder_on =
                                     (fm_cfg->lp_tx_precoder_on_auto == TRUE)?
                                                pmd_driver_default.lp_tx_precoder_on:
                                                fm_cfg->lp_tx_precoder_on;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->lp_tx_precoder_on_auto) {
            if (SHR_BITGET(fm_cfg->fbmp,
                           BCMPC_PMD_FW_LT_FLD_LP_TX_PRECODER_ON)) {
                pmd_cfg.lp_tx_precoder_on = fm_cfg->lp_tx_precoder_on;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* LP_TX_PRECODER_ON or _auto field is not updated by the user. */
        /*
         * if lp_tx_precoder_on_auto is 1,
         *    use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.lp_tx_precoder_on_auto) {
            pmd_cfg.lp_tx_precoder_on = pmd_driver_default.lp_tx_precoder_on;
        } else {
            pmd_cfg.lp_tx_precoder_on = pmd_db_lookup.lp_tx_precoder_on;
        }
    }

    /*
     * Is UNRELIABLE_LOS auto or UNRELIABLE_LOS field is
     * being updated by the user.
     */
    if ((SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS_AUTO)) ||
        (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS))) {
        /*
         * User is updating the UNRELIABLE_LOS setting, so get the
         * unreliable_los_auto setting from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS_AUTO)) {
            fm_cfg->unreliable_los_auto = pmd_db_lookup.unreliable_los_auto;
        }

        /*
         * User is updating the UNRELIABLE_LOS auto setting,
         * so get the UNRELIABLE_LOS from the DB.
         */
        if (!SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS)) {
            fm_cfg->unreliable_los = pmd_db_lookup.unreliable_los;
        }

        if (fm_cfg->unreliable_los_auto != pmd_db_lookup.unreliable_los_auto) {
            /*
             * If UNRELIABLE_LOS setting is auto configure, use the
             * configured hw value.
             */
            pmd_cfg.unreliable_los = (fm_cfg->unreliable_los_auto == TRUE)?
                                                pmd_driver_default.unreliable_los:
                                                fm_cfg->unreliable_los;
            cfg_apply_hw = TRUE;
        } else if (!fm_cfg->unreliable_los_auto) {
            if (SHR_BITGET(fm_cfg->fbmp,
                           BCMPC_PMD_FW_LT_FLD_UNRELIABLE_LOS)) {
                pmd_cfg.unreliable_los = fm_cfg->unreliable_los;
            }
            cfg_apply_hw = TRUE;
        }
    } else {
        /* UNRELIABLE_LOS or _auto field is not updated by the user. */
        /*
         * if unreliable_los_auto is 1,
         *    use the default value from the driver
         * else
         *    use the value from the DB lookup.
         */
        if (pmd_db_lookup.unreliable_los_auto) {
            pmd_cfg.unreliable_los = pmd_driver_default.unreliable_los;
        } else {
            pmd_cfg.unreliable_los = pmd_db_lookup.unreliable_los;
        }
    }

    /*
     * Normal reach and extended reach configuration can only be updated by
     * users when link training is not enabled.
     */
    if (!speed_config.link_training) {
        /*
         * Is NORMAL_REACH_PAM4 auto or NORMAL_REACH_PAM4 field is
         * being updated by the user.
         */
        if ((SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4_AUTO)) ||
            (SHR_BITGET(fm_cfg->fbmp, BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4))) {
            /*
             * User is updating the NORMAL_REACH_PAM4 setting, so get the
             * norm_reach_pam4_auto setting from the DB.
             */
            if (!SHR_BITGET(fm_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4_AUTO)) {
                fm_cfg->norm_reach_pam4_auto =
                                             pmd_db_lookup.norm_reach_pam4_auto;
            }

            /*
             * User is updating the NORMAL_REACH_PAM4 auto setting,
             * so get the NORMAL_REACH_PAM4 from the DB.
             */
            if (!SHR_BITGET(fm_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4)) {
                fm_cfg->norm_reach_pam4_bmask =
                                            pmd_db_lookup.norm_reach_pam4_bmask;
            }

            if (fm_cfg->norm_reach_pam4_auto !=
                                           pmd_db_lookup.norm_reach_pam4_auto) {
                /*
                 * If NORMAL_REACH_PAM4 setting is auto configure, use the
                 * configured hw value.
                 */
                pmd_cfg.norm_reach_pam4_bmask =
                                       (fm_cfg->norm_reach_pam4_auto == TRUE)?
                                       pmd_driver_default.norm_reach_pam4_bmask:
                                       fm_cfg->norm_reach_pam4_bmask;
                cfg_apply_hw = TRUE;
            } else if (!fm_cfg->norm_reach_pam4_auto) {
                if (SHR_BITGET(fm_cfg->fbmp,
                               BCMPC_PMD_FW_LT_FLD_NORMAL_REACH_PAM4)) {
                    pmd_cfg.norm_reach_pam4_bmask =
                                                  fm_cfg->norm_reach_pam4_bmask;
                }
                cfg_apply_hw = TRUE;
            }
        } else {
            /* NORMAL_REACH_PAM4 or _auto field is not updated by the user.*/
            /*
             * if norm_reach_pam4_auto is 1,
             *    use the default value from the driver
             * else
             *    use the value from the DB lookup.
             */
            if (pmd_db_lookup.norm_reach_pam4_auto) {
                pmd_cfg.norm_reach_pam4_bmask =
                                       pmd_driver_default.norm_reach_pam4_bmask;
            } else {
                pmd_cfg.norm_reach_pam4_bmask =
                                            pmd_db_lookup.norm_reach_pam4_bmask;
            }
        }

        /*
         * Is EXTENDED_REACH_PAM4 auto or EXTENDED_REACH_PAM4 field is
         * being updated by the user.
         */
        if ((SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4_AUTO)) ||
            (SHR_BITGET(fm_cfg->fbmp,
                        BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4))) {
            /*
             * User is updating the EXTENDED_REACH_PAM4 setting, so get the
             * ext_reach_pam4_auto setting from the DB.
             */
            if (!SHR_BITGET(fm_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4_AUTO)) {
                fm_cfg->ext_reach_pam4_auto = pmd_db_lookup.ext_reach_pam4_auto;
            }

            /*
             * User is updating the EXTENDED_REACH_PAM4 auto setting,
             * so get the EXTENDED_REACH_PAM4 from the DB.
             */
            if (!SHR_BITGET(fm_cfg->fbmp,
                            BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4)) {
                fm_cfg->ext_reach_pam4_bmask =
                                             pmd_db_lookup.ext_reach_pam4_bmask;
            }

            if (fm_cfg->ext_reach_pam4_auto !=
                                           pmd_db_lookup.ext_reach_pam4_auto) {
                /*
                 * If EXTENDED_REACH_PAM4 setting is auto configure, use the
                 * configured hw value.
                 */
                pmd_cfg.ext_reach_pam4_bmask =
                                        (fm_cfg->ext_reach_pam4_auto == TRUE)?
                                        pmd_driver_default.ext_reach_pam4_bmask:
                                        fm_cfg->ext_reach_pam4_bmask;
                cfg_apply_hw = TRUE;
            } else if (!fm_cfg->ext_reach_pam4_auto) {
                if (SHR_BITGET(fm_cfg->fbmp,
                               BCMPC_PMD_FW_LT_FLD_EXTENDED_REACH_PAM4)) {
                    pmd_cfg.ext_reach_pam4_bmask = fm_cfg->ext_reach_pam4_bmask;
                }
                cfg_apply_hw = TRUE;
            }
        } else {
            /* EXTENDED_REACH_PAM4 or _auto is not updated by the user. */
            /*
             * if ext_reach_pam4_auto is 1,
             *    use the default value from the driver
             * else
             *    use the value from the DB lookup.
             */
            if (pmd_db_lookup.ext_reach_pam4_auto) {
                pmd_cfg.ext_reach_pam4_bmask =
                                        pmd_driver_default.ext_reach_pam4_bmask;
            } else {
                pmd_cfg.ext_reach_pam4_bmask =
                                             pmd_db_lookup.ext_reach_pam4_bmask;
            }
        }
    }

    /*
     * Return if no changes in the PMD config properties, else apply to the
     * hardware.
     */
    if (!cfg_apply_hw) {
        SHR_EXIT();
    }

    phymod_firmware_lane_config_t_init(&pfm_cfg);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pmd_lane_config_to_phy(unit, &pmd_cfg, &pfm_cfg));

    PM_LOCK(pm_drv_info);

    for (i = 0; i < pcfg.num_lanes; i++) {
        phymod_firmware_lane_config_t local;

        pm_acc.phy_acc.access.lane_mask = 1 << (start_lane + i);
        phymod_firmware_lane_config_t_init(&local);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_pmd_lane_config_to_phy(unit, &pmd_cfg, &local));

        pfm_cfg.ForceNormalReach = (local.ForceNormalReach & (1 << i)) ? 1: 0;
        pfm_cfg.ForceExtenedReach = (local.ForceExtenedReach & (1 << i))? 1: 0;
        rv |= pm_drv->pm_port_pmd_lane_cfg_set(unit, &pm_acc, &op_st,
                                               pm_drv_info, &pfm_cfg);
    }
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_port_abilities_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                       pm_port_ability_type_t ability_type,
                       bcmpc_pmgr_port_abilities_t *pabilities)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_port_ability_t *port_abilities = pabilities->port_abilities;
    uint32_t num_abilities = 0;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_abilities_get(unit, &pm_acc, &op_st, pm_drv_info,
                                       ability_type, &num_abilities,
                                       port_abilities);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    pm_port_ability_to_bcmpc_pmgr_ability(unit, num_abilities,
                                          port_abilities, pabilities);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();

}

static int
pmm_port_detach(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    bcmpc_drv_t *pc_drv;
    bool is_dual_pml_supported = false;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->port_detach(unit, &pm_acc, &op_st, pm_drv_info);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);


    if (pc_drv->is_dual_pml_supported) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->is_dual_pml_supported(unit, pport,
                                           &is_dual_pml_supported));
    }

    if (is_dual_pml_supported &&
        (!pm_drv_info->pm_data.pm_ha_info->is_pm_initialized)) {
        /*
         * If the PM consists of two PMLs,
         * after port deletion, if the PM becomes uninitialized,
         * we need to power down both PMLs.
         * For other single PML core, PM_DRV will power down the PM
         * as part of port deletion.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (tsc_power_control(unit, pport, 0));
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_tsc_power_on(int unit, bcmpc_pport_t pport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tsc_power_control(unit, pport, 1));

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_vco_rate_get(int unit, bcmpc_pport_t pport, bcmpc_vco_type_t *vco_rate)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_vco_t pm_vcos[BCMPC_NUM_PLLS_PER_PM_MAX] = {PM_VCO_INVALID, PM_VCO_INVALID};
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));
    PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_vco_get(unit, &pm_acc, &op_st, pm_drv_info, pm_vcos);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    pm_vco_rate_to_bcmpc_vco_rate(unit, pm_vcos, vco_rate);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_timesync_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                 bcmpc_port_timesync_t *config)
{
    bcmpc_port_timesync_t old_config;
    int rv = SHR_E_NONE;
    int config_1588 = 0;
    pm_port_synce_clk_ctrl_t synce_ctrl, synce_ctrl_current;
    pm_phy_timesync_config_t ts_config;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    pm_oper_status_t op_st;
    bool cfg_apply_hw = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    sal_memset(&old_config, 0, sizeof(old_config));

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORT_TIMESYNCt, P(&lport),
                                   P(&old_config));

    if (rv == SHR_E_NOT_FOUND) {
        /*
         * SHR_E_NOT_FOUND means there is no previous timesync configuration.
         */
        rv = SHR_E_NONE;
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /*
     * If IEEE_1588 flag set, update ieee_1588 if db value changed.
     * else get the ieee_1588 value from the old_db to check if already
     * enabled.
     */
    if (SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_IEEE_1588_ENABLE)) {
        if (PMGR_CFG_CHANGED(0, config, &old_config, ieee_1588)) {
            config_1588++;
        }
    } else {
        if (old_config.ieee_1588) {
            config_1588++;
        }
    }

    if (SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_ONE_STEP_TIMESTAMP)) {
        if (PMGR_CFG_CHANGED(0, config, &old_config, one_step_timestamp)) {
            cfg_apply_hw = 1;
        }
    } else {
            config->one_step_timestamp = old_config.one_step_timestamp;
    }

    if (SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_IS_SOP)) {
        if (PMGR_CFG_CHANGED(0, config, &old_config, is_sop)) {
            cfg_apply_hw = 1;
        }
    } else {
            config->is_sop = old_config.is_sop;
    }

    if (SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_TS_COMP_MODE)) {
        if (PMGR_CFG_CHANGED(0, config, &old_config, ts_comp_mode)) {
            cfg_apply_hw = 1;
        }
    } else {
            config->ts_comp_mode = old_config.ts_comp_mode;
    }

    if (config_1588 || cfg_apply_hw) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_timesync_to_pm_ts_config(unit, config, &ts_config));

        PM_LOCK(pm_drv_info);

        rv = pm_drv->pm_port_timesync_config_set(unit, &pm_acc, &op_st,
                                                 pm_drv_info, &ts_config);
        PM_UNLOCK(pm_drv_info);

        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_timesync_to_pm_synce(unit, config, &synce_ctrl));
    /*
     * For SyncE configuration, we have two non-zero default values.
     * So we need to keep the hardware default value if user does not
     * give an input for these fields.
     */
    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_synce_clk_ctrl_get(unit, &pm_acc, &op_st, pm_drv_info,
                                            &synce_ctrl_current);

    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (!SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_STG0_MODE)) {
        synce_ctrl.stg0_mode = synce_ctrl_current.stg0_mode;
    }
    if (!SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_STG1_MODE)) {
        synce_ctrl.stg1_mode = synce_ctrl_current.stg1_mode;
    }
    if (!SHR_BITGET(config->fbmp, BCMPC_TIMESYNC_LT_FLD_SDM_VAL)) {
        synce_ctrl.sdm_val = synce_ctrl_current.sdm_val;
    }

    if (sal_memcmp(&synce_ctrl, &synce_ctrl_current, sizeof(synce_ctrl))) {
        PM_LOCK(pm_drv_info);
        rv = pm_drv->pm_port_synce_clk_ctrl_set(unit, &pm_acc, &op_st,
                                                pm_drv_info,
                                                &synce_ctrl);
        PM_UNLOCK(pm_drv_info);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_port_sw_state_update(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_port_sw_state_update(unit, &pm_acc, &op_st, pm_drv_info);

    PM_UNLOCK(pm_drv_info);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_pm_init(int unit, bcmpc_pport_t pport)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_init(unit, &pm_acc, pm_drv_info);

    PM_UNLOCK(pm_drv_info);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_rlm_status_get(int unit, bcmpc_pport_t pport,
                        bcmpc_pmgr_port_rlm_status_t *status)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    pm_port_rlm_status_t rlm_status;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);

    rv = pm_drv->pm_port_rlm_status_get(unit, &pm_acc, &op_st,
                                        pm_drv_info, &rlm_status);

    PM_UNLOCK(pm_drv_info);
    if (rv == SHR_E_UNAVAIL) {
        /* If RLM API is not supported, return status as DISABLED. */
        *status = BCMPC_PMGR_PORT_RLM_STATUS_DISABLED;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        SHR_IF_ERR_VERBOSE_EXIT
            (pm_port_rlm_status_to_bcmpc_pmgr_port_rlm_status(unit, &rlm_status,
                                                              status));
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_vco_rate_calculate(int unit, bcmpc_pport_t pport,
                       pm_vco_setting_t *vco_select)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_vco_rate_calculate(unit, &op_st, pm_drv_info, vco_select);

    PM_UNLOCK(pm_drv_info);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_core_attach(int unit, bcmpc_pport_t pport, pm_vco_setting_t *vco_cfg,
                bcmpc_port_cfg_t *pcfg, bcmpc_pm_core_cfg_t *ccfg)
{
    int init_pass;
    pm_port_add_info_t add_info;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    bcmpc_drv_t *pc_drv;
    uint16_t core_mdio_addr;
    phymod_sim_drv_t *sim_drv = NULL;
    phymod_dispatch_type_t dispatch_type;
    phymod_core_init_config_t core_config;
    phymod_firmware_load_method_t fw_ld_method = phymodFirmwareLoadMethodNone;
    /* It is used to indicate firmware will be downloaded. */
    uint32_t fw_to_load = 0;
    int rv = SHR_E_NONE;
    pm_oper_status_t op_st;
    bool is_dual_pml_supported = FALSE;
    bool core_init_pass1_pport_adjust = FALSE;
    bcmpc_pport_t original_pport = 0;
    bool rev_id_a0 = FALSE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    sal_memset(&add_info, 0, sizeof(pm_port_add_info_t));
    phymod_core_init_config_t_init(&core_config);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmpc_pmgr_pm_core_cfg_to_pm_cfg(unit, ccfg, pm_info.prop.num_lanes,
                                         &add_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (ccfg->pm_fw_cfg.fw_crc_verify) {
        PM_PORT_ADD_F_INIT_FIRMWARE_CRC_VERIFY_SET(&add_info);
    }
    if (ccfg->pm_fw_cfg.fw_load_verify) {
        PM_PORT_ADD_F_INIT_FIRMWARE_LOAD_VERIFY_SET(&add_info);
    }

    /*
     * If VCO setting is NULL, it means the VCO rates have to be
     * determined by the driver based on the speed of the port
     * added to the device.
     * If VCO setting is not NULL, means the VCO rate is already
     * determined by the upper layer based on the ports configured
     * on a Port Macro.
     */
    if (vco_cfg != NULL) {
        /* Check if VCO rates for the SERDES core is determined. */
        if (vco_cfg->tvco != PM_VCO_INVALID) {
            add_info.tvco = vco_cfg->tvco;
        }

        /* Check if OVCO rates for the SERDES core is determined. */
        if (vco_cfg->ovco != PM_VCO_INVALID) {
            add_info.ovco = vco_cfg->ovco;
        }
    } else {
        add_info.speed_config.speed = pcfg->speed;
        add_info.speed_config.num_lanes = pcfg->num_lanes;
        add_info.speed_config.fec = pcfg->fec_mode;
    }

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        if (pc_drv->pm_physim_add) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_drv->pm_physim_add(unit, drv_data.pport));
        } else {
            core_mdio_addr=(uint16_t)pc_drv->phy_addr_get(unit, drv_data.pport);
            dispatch_type=drv_data.phy_acc.type;
            phymod_control_physim_drv_get(unit, dispatch_type, &sim_drv);
            if (sim_drv == NULL) {
                SHR_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            phymod_control_physim_add(unit, core_mdio_addr, sim_drv);
        }

    }

    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        add_info.init_config.fw_load_method = phymodFirmwareLoadMethodNone;
    } else if (pc_drv->pm_firmware_loader != NULL) {
        /* Get firmware download method and loader. */
        if (ccfg->pm_fw_cfg.fw_load == PM_SERDES_FW_LOAD_FAST) {
            add_info.init_config.fw_load_method =
                                           phymodFirmwareLoadMethodExternal;
            fw_to_load = 1;
            /* Get firmware loader. */
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_drv->pm_firmware_loader(unit, pport, fw_to_load,
                                            &core_config.firmware_loader));
            add_info.init_config.fw_loader = core_config.firmware_loader;
         } else {
            add_info.init_config.fw_load_method =
                                           phymodFirmwareLoadMethodInternal;
         }
         fw_ld_method = add_info.init_config.fw_load_method;
    }

    /*
     * Firmware is downloaded during LT booting.
     * In order to avoid double firmware download, pass firmware_load_method
     * phymodFirmwareLoadMethodNone to PHYMOD PASS1 core init
     */
    if ((pm_drv_info->pm_data.pm_ha_info)->firmware_downloaded) {
        add_info.init_config.fw_load_method = phymodFirmwareLoadMethodNone;
    }

    for (init_pass = 0; init_pass < PM_CORE_INIT_PASS; init_pass++) {
         if (init_pass == 0 ){
            /* PASS1 */
            PM_PORT_ADD_F_INIT_PROBE_SET(&add_info);
            PM_PORT_ADD_F_INIT_PASS1_SET(&add_info);
            /* need to check if the pport needs to be adjusted */
            if (pc_drv->phymod_core_init_pass1_pport_adjust) {
                /* fist save the original pport */
                original_pport = pm_acc.pport;
                /* next check the value */
                SHR_IF_ERR_EXIT
                    (pc_drv->phymod_core_init_pass1_pport_adjust(unit,
                                                                 &pm_acc,
                                          &core_init_pass1_pport_adjust));
            }
            PM_PORT_ADD_F_INIT_PASS2_CLR(&add_info);
         } else {
            /* PASS2 */
            PM_PORT_ADD_F_INIT_PROBE_CLR(&add_info);
            PM_PORT_ADD_F_INIT_PASS1_CLR(&add_info);
            PM_PORT_ADD_F_INIT_PASS2_SET(&add_info);
            if (!bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM) &&
             (pm_drv_info->pm_data.pm_ha_info)->firmware_downloaded) {
             add_info.init_config.fw_load_method = fw_ld_method;
            }
            /* need to check if the PM consists of two PMLs. */
            if (pc_drv->is_dual_pml_supported) {
                SHR_IF_ERR_EXIT
                    (pc_drv->is_dual_pml_supported(unit, pport,
                                                   &is_dual_pml_supported));
                /*
                 *  If the PM consists of two PMLs, it means
                 *  core_init_pass2 needs to be run twice.
                 *  Here get the U1 port.
                 */
                if (is_dual_pml_supported) {
                    /* first save the original pport */
                    original_pport = pm_acc.pport;
                    SHR_IF_ERR_EXIT
                        (pc_drv->phymod_core_init_pass2_u1_adjust(unit, &pm_acc));
                    /* set the bypass PMD config for the phymod _core_init_pass2 flag
                     * just do U1 pcs only core init */
                    PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_SET(&add_info);
                    PM_LOCK(pm_drv_info);
                    rv = pm_drv->core_attach(unit, &pm_acc, &op_st,
                                            pm_drv_info, &add_info);
                    PM_UNLOCK(pm_drv_info);
                    SHR_IF_ERR_EXIT(rv);
                }
            }
        }
        /* needs to restore some of the info if is_dual_pml_supported is TRUE */
        if (PM_PORT_ADD_F_INIT_PASS2_GET(&add_info) && is_dual_pml_supported) {
            SHR_IF_ERR_EXIT
                (pc_drv->phymod_core_init_pass2_u0_adjust(unit, &pm_acc));
            /* clear the BYPASS PMD config flag */
            PM_PORT_ADD_F_INIT_PASS2_BYPASS_PMD_CONFIG_CLEAR(&add_info);
        }

        /* SW WAR to disable DCC on device revision A0. */
       if (pc_drv->dev_revision_a0_get) {
            SHR_IF_ERR_EXIT
                (pc_drv->dev_revision_a0_get(unit, &rev_id_a0));
            if (PM_PORT_ADD_F_INIT_PASS2_GET(&add_info) && rev_id_a0 == true) {
                 PM_PORT_ADD_F_INIT_TOP_DEV_REV_ID_A0_SET(&add_info);
            }
        }
        PM_LOCK(pm_drv_info);
        rv = pm_drv->core_attach(unit, &pm_acc, &op_st,
                              pm_drv_info, &add_info);

        PM_UNLOCK(pm_drv_info);
        /* need to restore the PM_ACC pport afterinit_pass1 or  init_pass2 is done */
        if (is_dual_pml_supported || core_init_pass1_pport_adjust) {
            pm_acc.pport = original_pport;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Clear cell mode if applied. */
    if (PM_PORT_ADD_F_INIT_PASS2_GET(&add_info) &&
        drv_data.pmac_drv->cell_mode_set) {
        int pmac_blkport = 0, pml_idx;
        bcmpmac_drv_t *pmacd = NULL;
        bcmpmac_access_t *pmac_acc = NULL;

        bcmpc_pport_t pport_tmp = BCMPC_INVALID_PPORT;
        for (pml_idx = 0; pml_idx < NUM_PML; pml_idx++) {
            SHR_IF_ERR_EXIT
                (pc_drv->pm_base_pport_adjust(unit, pport, pml_idx,
                                                   &pport_tmp));
                SHR_IF_ERR_VERBOSE_EXIT
                    (pm_port_drv_data_get(unit, pport_tmp, &pm_acc,
                                               &drv_data, &pm_info,
                                               &pm_drv_info, &pm_drv));
                pmac_blkport = drv_data.pmac_blkport;
                pmacd = drv_data.pmac_drv;
                pmac_acc = &drv_data.pmac_acc;
                SHR_IF_ERR_VERBOSE_EXIT
                    (pmacd->cell_mode_set(pmac_acc, pmac_blkport, 0));
        }
    }

exit:
    if ((SHR_FUNC_ERR()) && (op_st.valid)) {
        port_oper_status_table_populate(unit, pport, &op_st);
    }
    SHR_FUNC_EXIT();
}

static int
pmm_port_loopback_set(int unit, bcmpc_pport_t pport, bcmpc_port_cfg_t *pcfg)
{
    SHR_FUNC_ENTER(unit)

    SHR_IF_ERR_VERBOSE_EXIT
        (port_lpbk_set(unit, pport, pcfg));

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_enable_set(int unit, bcmpc_pport_t pport, uint32_t flags,
                    bcmpc_port_cfg_t *pcfg)
{
    SHR_FUNC_ENTER(unit)

    SHR_IF_ERR_VERBOSE_EXIT
        (port_enable_set(unit, pport, flags, pcfg->enable));

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_core_config_get(int unit, bcmpc_pport_t pport, bcmpc_pm_core_cfg_t *ccfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    phymod_polarity_t polarity;
    phymod_lane_map_t lane_map;
    int rv = SHR_E_NONE;
    uint32_t lane_index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    /* Get TX/RX polarity flip settings. */
    phymod_polarity_t_init(&polarity);
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_phy_polarity_get(unit, &pm_acc, &op_st, pm_drv_info,
                                     &polarity);
    PM_UNLOCK(pm_drv_info);

    ccfg->rx_polarity_flip_oper = (uint16_t) polarity.rx_polarity;
    ccfg->tx_polarity_flip_oper = (uint16_t) polarity.tx_polarity;

    /* Get the PM core TX/RX lane map settings. */
    phymod_lane_map_t_init(&lane_map);
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_core_lane_map_get(unit, &pm_acc, &op_st, pm_drv_info,
                                     &lane_map);
    PM_UNLOCK(pm_drv_info);

    for (lane_index = 0; lane_index < lane_map.num_of_lanes; lane_index++) {
        ccfg->rx_lane_map_oper |=
                ((lane_map.lane_map_rx[lane_index] & 0xf) << (lane_index * 4));
        ccfg->tx_lane_map_oper |=
                ((lane_map.lane_map_tx[lane_index] & 0xf) << (lane_index * 4));
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_tx_timestamp_info_get(int unit, bcmpc_lport_t lport,
                               bcmpc_pport_t pport,
                               bcmpc_pmgr_port_timestamp_t *timestamp_info)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_tx_timestamp_info_t timestamp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    sal_memset(&timestamp, 0, sizeof(timestamp));
    PM_LOCK(pm_drv_info);
    rv = pm_drv->port_tx_timestamp_info_get(unit, &pm_acc, &op_st, pm_drv_info,
                                            &timestamp);
    PM_UNLOCK(pm_drv_info);

    timestamp_info->timestamp_lo = timestamp.timestamp_lo;
    timestamp_info->timestamp_hi = timestamp.timestamp_hi;
    timestamp_info->sequence_id = timestamp.sequence_id;
    timestamp_info->timestamp_sub_nanosec = timestamp.timestamp_sub_nanosec;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_egress_timestamp_mode_get(int unit, bcmpc_pport_t pport,
                              bcmpc_pmgr_egr_timestamp_mode_t *timestamp_mode)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_egr_timestamp_mode_t egr_timestamp_mode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_egress_timestamp_mode_get(unit, &pm_acc, &op_st,
                                              pm_drv_info,
                                              &egr_timestamp_mode);
    PM_UNLOCK(pm_drv_info);

    *timestamp_mode = egr_timestamp_mode;
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_egress_timestamp_mode_set(int unit, bcmpc_pport_t pport,
                              bcmpc_pmgr_egr_timestamp_mode_t timestamp_mode)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    pm_egr_timestamp_mode_t egr_timestamp_mode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    egr_timestamp_mode = timestamp_mode;
    PM_LOCK(pm_drv_info);
    rv = pm_drv->pm_egress_timestamp_mode_set(unit, &pm_acc, &op_st,
                                              pm_drv_info,
                                              egr_timestamp_mode);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_fdr_control_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                         bcmpc_fdr_port_control_t *fdr_cfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    bcmpc_drv_t *pc_drv;
    uint32_t fdr_intr = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv->fdr_control_get) {
        SHR_IF_ERR_EXIT
            (pc_drv->fdr_control_get(unit, pport, fdr_cfg));

        /* Interrupt valid only on the real hardware. */
        if (bcmdrd_feature_is_real_hw(unit)) {
            PM_LOCK(pm_drv_info);
            rv = pm_drv->port_intr_enable_get(unit, &pm_acc, &op_st,
                                              pm_drv_info, BCMPC_INTR_FDR,
                                              &fdr_intr);
            PM_UNLOCK(pm_drv_info);
            SHR_IF_ERR_EXIT(rv);

            fdr_cfg->interrupt_enable &= (fdr_intr? 1: 0);
        }
    } else {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_fdr_control_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                         bcmpc_fdr_port_control_t *fdr_cfg)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;
    int cfg_hw_apply = 0;
    bcmpc_drv_t *pc_drv;
    bcmpc_fdr_port_control_t fdr_cur_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    sal_memset(&fdr_cur_cfg, 0, sizeof(bcmpc_fdr_port_control_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (pmm_port_fdr_control_get(unit, lport, pport, &fdr_cur_cfg));

    if (!SHR_BITGET(fdr_cfg->fbmp, BCMPC_FDR_CTRL_LT_FLD_ENABLE)) {
        fdr_cfg->enable = fdr_cur_cfg.enable;
    }

    if (!SHR_BITGET(fdr_cfg->fbmp, BCMPC_FDR_CTRL_LT_FLD_SYMERR_WINDOW_SIZE)) {
        fdr_cfg->window_size = fdr_cur_cfg.window_size;
    }

    if (!SHR_BITGET(fdr_cfg->fbmp, BCMPC_FDR_CTRL_LT_FLD_SYMERR_THRESHOLD)) {
        fdr_cfg->symbol_error_threshold = fdr_cur_cfg.symbol_error_threshold;
    }

    if (!SHR_BITGET(fdr_cfg->fbmp, BCMPC_FDR_CTRL_LT_FLD_SYMERR_START_VALUE)) {
        fdr_cfg->symbol_error_start_value =
                                fdr_cur_cfg.symbol_error_start_value;
    }

    if (!SHR_BITGET(fdr_cfg->fbmp, BCMPC_FDR_CTRL_LT_FLD_FDR_INTR_ENABLE)) {
        fdr_cfg->interrupt_enable = fdr_cur_cfg.interrupt_enable;
    }

    /* Check if user configured fields are present. */
    SHR_BITCOUNT_RANGE(fdr_cfg->fbmp, cfg_hw_apply, 0,
                       PC_FDR_CTRL_FIELD_ID_MAX);

    /* Return if no user configured fields. */
    if (!cfg_hw_apply) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv->fdr_control_set) {
        SHR_IF_ERR_EXIT
            (pc_drv->fdr_control_set(unit, pport, *fdr_cfg));

        /* Interrupt valid only on the real hardware. */
        if (bcmdrd_feature_is_real_hw(unit)) {
            PM_LOCK(pm_drv_info);
            rv = pm_drv->port_intr_enable_set(unit, &pm_acc, &op_st,
                                              pm_drv_info, BCMPC_INTR_FDR,
                                              fdr_cfg->interrupt_enable);
            PM_UNLOCK(pm_drv_info);
            SHR_IF_ERR_EXIT(rv);
        }
    } else {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_fdr_stats_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                       bcmpc_fdr_port_stats_t *fdr_stats)
{
    bcmpc_drv_t *pc_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv->fdr_stats_get) {
        SHR_IF_ERR_EXIT
            (pc_drv->fdr_stats_get(unit, pport, fdr_stats));
    } else {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pmm_port_diag_stats_get(int unit, bcmpc_pport_t pport,
                       bcmpc_pmgr_port_diag_stats_t *diag_stats)
{
    uint64_t fec_uncorrected_cws, fec_corrected_cws, fec_symbol_errors;
    bool release_lock = FALSE;
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    release_lock = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT(
            pm_drv->pm_phy_rsfec_uncorrectable_counter_get(unit,
                &pm_acc, pm_drv_info, &fec_uncorrected_cws));
    SHR_IF_ERR_VERBOSE_EXIT(
            pm_drv->pm_phy_rsfec_correctable_counter_get(unit,
                &pm_acc, pm_drv_info, &fec_corrected_cws));
    SHR_IF_ERR_VERBOSE_EXIT(
            pm_drv->pm_phy_rsfec_symbol_error_counter_get(unit,
                &pm_acc, pm_drv_info, &fec_symbol_errors));

    PM_UNLOCK(pm_drv_info);
    release_lock = FALSE;

    diag_stats->fec_uncorrected_cws = fec_uncorrected_cws;
    diag_stats->fec_corrected_cws = fec_corrected_cws;
    diag_stats->fec_symbol_errors = fec_symbol_errors;

exit:
    if (release_lock == TRUE) {
        PM_UNLOCK(pm_drv_info);
    }
    SHR_FUNC_EXIT();
}


static int
pmm_port_interrupt_process(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport,
                           bcmpc_intr_type_t intr_type,
                           uint32_t *is_handled)
{
    bcmpc_pmgr_drv_data_t drv_data;
    pm_info_t *pm_drv_info = NULL;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    pm_access_t pm_acc;
    bcmpc_pm_drv_t *pm_drv = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (pm_port_drv_data_get(unit, pport, &pm_acc, &drv_data, &pm_info,
                              &pm_drv_info, &pm_drv));

    PM_LOCK(pm_drv_info);
    rv = pm_drv->port_interrupt_process(unit, &pm_acc, &op_st,
                                        pm_drv_info, intr_type, is_handled);
    PM_UNLOCK(pm_drv_info);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

bcmpc_pmgr_drv_t bcmpc_pmgr_drv_default = {
    .pm_port_enable = default_pm_port_enable,
    .port_cfg_set = pmm_port_cfg_set,
    .port_status_get = pmm_port_status_get,
    .port_loopback_set = pmm_port_loopback_set,
    .port_enable_set = pmm_port_enable_set,
    .phy_link_info_get = default_phy_link_info_get,
    .phy_link_state_get = pmm_phy_link_state_get,
    .fault_state_get = default_fault_state_get,
    .core_mode_set = NULL,
    .pmac_op_exec = default_pmac_op_exec,
    .phy_op_exec = default_phy_op_exec,
    .failover_enable_set = pmm_failover_enable_set,
    .failover_enable_get = pmm_failover_enable_get,
    .failover_loopback_get = pmm_failover_loopback_get,
    .failover_loopback_remove = default_failover_loopback_remove,
    .pm_pfc_set = pmm_pfc_set,
    .pm_pfc_get = pmm_pfc_get,
    .port_attach = pmm_port_attach,
    .port_speed_get = pmm_port_speed_get,
    .pm_port_config_validate = pmm_pm_port_config_validate,
    .port_autoneg_advert_set = pmm_port_autoneg_advert_set,
    .pm_mac_control_set = pmm_mac_control_set,
    .pm_mac_control_get = pmm_mac_control_get,
    .pm_phy_control_set = pmm_phy_control_set,
    .pm_phy_status_get = pmm_phy_status_get,
    .pm_tx_taps_config_set = pmm_tx_taps_config_set,
    .pm_tx_taps_status_get = pmm_tx_taps_status_get,
    .pm_pmd_firmware_config_set = pmm_pmd_firmware_config_set,
    .pm_pmd_firmware_status_get = pmm_pmd_firmware_status_get,
    .pm_port_abilities_get = pmm_port_abilities_get,
    .pm_port_detach = pmm_port_detach,
    .pm_tsc_power_on = pmm_tsc_power_on,
    .pm_vco_rate_get = pmm_vco_rate_get,
    .pm_timesync_set = pmm_timesync_set,
    .pm_port_sw_state_update = pmm_port_sw_state_update,
    .port_info_table_update = port_oper_status_table_populate,
    .pm_init = pmm_pm_init,
    .port_rlm_status_get = pmm_port_rlm_status_get,
    .pm_core_attach = pmm_core_attach,
    .pm_vco_rate_calculate = pmm_vco_rate_calculate,
    .pm_core_config_get = pmm_core_config_get,
    .port_tx_timestamp_info_get = pmm_port_tx_timestamp_info_get,
    .pm_egress_timestamp_mode_get = pmm_egress_timestamp_mode_get,
    .pm_egress_timestamp_mode_set = pmm_egress_timestamp_mode_set,
    .pm_port_fdr_control_get = pmm_port_fdr_control_get,
    .pm_port_fdr_control_set = pmm_port_fdr_control_set,
    .pm_port_fdr_stats_get = pmm_port_fdr_stats_get,
    .pm_port_diag_stats_get = pmm_port_diag_stats_get,
    .pm_port_interrupt_process = pmm_port_interrupt_process
};
