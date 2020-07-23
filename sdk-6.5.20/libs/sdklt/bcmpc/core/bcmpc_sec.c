/*! \file bcmpc_sec.c
 *
 * BCMPC SEC APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_sec.h>
#include <bcmpc/bcmpc_sec_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*! PC -> SEC Mutex for each unit. */
static sal_mutex_t pc_sec_mutex[BCMPC_NUM_UNITS_MAX];

/*! SEC handler for each unit. */
static bcmpc_sec_handler_t sec_handler[BCMPC_NUM_UNITS_MAX];

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Internal Public functions
 */
void
pc_sec_port_cfg_init(size_t num_ports, bcmpc_sec_port_cfg_t *cfg)
{
    uint32_t i;

    sal_memset(cfg, 0, (num_ports * sizeof(bcmpc_sec_port_cfg_t)));
    for (i = 0; i < num_ports; i++) {
        cfg[i].lport = BCMPC_INVALID_LPORT;
        cfg[i].pport = BCMPC_INVALID_LPORT;
        cfg[i].speed_cur = -1;
        cfg[i].num_lanes = -1;
    }
}

/*!
 * \brief Update SEC configuration.
 *
 * Update SEC port configuration via the callbacks of bcmpc_sec_handler_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] old_cfg Current port configuration.
 * \param [in] new_cfg New/requested port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
pc_sec_update(int unit, int num_ports,
              bcmpc_sec_port_cfg_t *sec_pcfg_old,
              bcmpc_sec_port_cfg_t *sec_pcfg_new)
{
    bcmpc_sec_update_f func;

    SHR_FUNC_ENTER(unit);

    sal_mutex_take(pc_sec_mutex[unit], SAL_MUTEX_FOREVER);
    func = sec_handler[unit].sec_validate;
    if (func) {
        SHR_IF_ERR_EXIT
            (func(unit, num_ports, sec_pcfg_old, sec_pcfg_new));
    }

    func = sec_handler[unit].sec_configure;
    if (func) {
        SHR_IF_ERR_EXIT
            (func(unit, num_ports, sec_pcfg_old, sec_pcfg_new));
    }

exit:
    sal_mutex_give(pc_sec_mutex[unit]);
    SHR_FUNC_EXIT();
}


int
bcmpc_sec_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_sec_port_op_exec_f func;

    SHR_FUNC_ENTER(unit);

    func = sec_handler[unit].port_op_exec;
    SHR_NULL_CHECK
        (func, SHR_E_NONE);
    SHR_IF_ERR_EXIT
        (func(unit, pport, op->name, op->param));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_sec_valid_get(int unit)
{
    bcmpc_drv_t *pc_drv = NULL;
    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return 0;
    }
    if (pc_drv && pc_drv->is_sec_supported) {
        return pc_drv->is_sec_supported(unit);
    }
    return 0;
}

int
bcmpc_pm_sec_enabled(int unit, int pm_id)
{
    bcmpc_drv_t *pc_drv = NULL;
    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return 0;
    }
    if (pc_drv && pc_drv->is_pm_sec_enabled) {
        return pc_drv->is_pm_sec_enabled(unit, pm_id);
    }
    return 0;
}

/*******************************************************************************
 * Public functions
 */

int
bcmpc_sec_handler_register(int unit, bcmpc_sec_handler_t *hdl)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    sal_memcpy(&sec_handler[unit], hdl, sizeof(*hdl));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_sec_handler_get(int unit, bcmpc_sec_handler_t **hdl)
{
    *hdl = &sec_handler[unit];
    return;
}

int
bcmpc_pc_sec_mutex_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (pc_sec_mutex[unit]) {
        SHR_EXIT();
    }

    pc_sec_mutex[unit] = sal_mutex_create("bcmpcPcSec");
    SHR_NULL_CHECK(pc_sec_mutex[unit], SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pc_sec_mutex_cleanup(int unit)
{
    if (!pc_sec_mutex[unit]) {
        return;
    }

    sal_mutex_destroy(pc_sec_mutex[unit]);
    pc_sec_mutex[unit] = NULL;
}

void
tm_sec_port_cfg_init(size_t num_ports, bcmpc_sec_port_cfg_t *cfg,
                     bcmpc_mmu_port_cfg_t *mmu_pcfg)
{
    uint32_t i;

    for (i = 0; i < num_ports; i++) {
        cfg[i].lport = mmu_pcfg[i].lport;
        cfg[i].pport = mmu_pcfg[i].pport;
        cfg[i].speed_cur = mmu_pcfg[i].speed_cur;
        cfg[i].num_lanes = mmu_pcfg[i].num_lanes;
    }
}
