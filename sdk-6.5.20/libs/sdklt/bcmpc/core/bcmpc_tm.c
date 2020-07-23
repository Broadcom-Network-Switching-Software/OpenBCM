/*! \file bcmpc_tm.c
 *
 * BCMPC TM APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmpc/bcmpc_sec.h>
#include <bcmpc/bcmpc_tm_internal.h>
#include <bcmpc/bcmpc_sec_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DEV

/*! Non-zero speed value. */
#define PC_SPEED_NON_ZERO ((uint32_t)-1)

/*! TM update mode (manual/auto) for each unit. */
static bool tm_manual_update[BCMPC_NUM_UNITS_MAX];

/*! TM port-based update mode state for each unit. */
static bool tm_port_based_update[BCMPC_NUM_UNITS_MAX];

/*! TM handler for each unit. */
static bcmpc_tm_handler_t tm_handler[BCMPC_NUM_UNITS_MAX];

/*! PC -> TM Mutex for each unit. */
static sal_mutex_t pc_tm_mutex[BCMPC_NUM_UNITS_MAX];

/*! The TM configuration. */
typedef struct tm_cfg_s {

    /* The elements in the buffer \c mmu_pcfg_array.*/
    size_t num_ports;

    /* The buffer to save the MMU configuration.*/
    bcmpc_mmu_port_cfg_t *mmu_pcfg_array;

    /* The oversub port bit map.*/
    bcmpc_pbmp_t oversub_pbmp;

} tm_cfg_t;


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the number of physical ports.
 *
 * \param [in] unit Unit number.
 *
 * \retrun The number of physical ports in the device.
 */
static int
pc_num_pports_get(int unit)
{
    bcmdrd_pbmp_t pbmp;
    int port;

    if (SHR_FAILURE(bcmdrd_dev_valid_ports_get(unit, &pbmp))) {
        return 0;
    }

    for (port = BCMDRD_CONFIG_MAX_PORTS - 1; port >= 0; port--) {
        if (BCMDRD_PBMP_MEMBER(pbmp, port)) {
            return port + 1;
        }
    }

    return 0;
}

/*!
 * \brief Get the oversub port bitmap for the given unit.
 *
 * \param [in] unit Unit number.
 * \param [out] oversub_pbmp The oversub port bit map.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_oversub_pbmp_get(int unit, bcmpc_pbmp_t *oversub_pbmp)
{
    bcmpc_drv_t *pc_drv = NULL;

    sal_memset(oversub_pbmp, 0, sizeof(*oversub_pbmp));

    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return SHR_E_FAIL;
    }

    if (pc_drv && pc_drv->oversub_pbmp_get) {
        return pc_drv->oversub_pbmp_get(unit, oversub_pbmp);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Initialize the port config array.
 *
 * \param [in] num_ports Number of physical ports in configuration array.
 * \param [out] cfg Port configuration array.
 */
static void
pc_mmu_port_cfg_init(size_t num_ports, bcmpc_mmu_port_cfg_t *cfg)
{
    uint32_t i;

    sal_memset(cfg, 0, (num_ports * sizeof(bcmpc_mmu_port_cfg_t)));
    for (i = 0; i < num_ports; i++) {
        cfg[i].lport = BCMPC_INVALID_LPORT;
        cfg[i].pport = BCMPC_INVALID_LPORT;
    }
}

/*!
 * \brief Set the MMU port configuration.
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pentry Port configuration.
 * \param [in] oversub_pbmp Oversub port bitmap.
 * \param [out] mmu_pcfg Buffer for MMU port config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_mmu_pcfg_set(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                bcmpc_pbmp_t *oversub_pbmp, bcmpc_mmu_port_cfg_t *mmu_pcfg)
{
    bcmpc_pport_t pport;

    pport = pcfg->pport;
    mmu_pcfg->pport = pport;
    mmu_pcfg->lport = lport;
    mmu_pcfg->speed_max = pcfg->speed;
    mmu_pcfg->speed_cur = pcfg->speed;
    mmu_pcfg->num_lanes = pcfg->num_lanes;
    mmu_pcfg->flags = 0;;

    if (SHR_BITGET((uint32_t*)(oversub_pbmp), pport)) {
        mmu_pcfg->flags |= BCMPC_MMU_F_OVSUB;
    }

    if (pcfg->encap_mode == BCMPC_PORT_ENCAP_HIGIG) {
        mmu_pcfg->flags |= BCMPC_MMU_F_HG2;
        mmu_pcfg->flags  &= ~(BCMPC_MMU_F_HG3 | BCMPC_MMU_F_IEEE);
    } else if (pcfg->encap_mode == BCMPC_PORT_ENCAP_IEEE) {
        mmu_pcfg->flags |= BCMPC_MMU_F_IEEE;
        mmu_pcfg->flags  &= ~(BCMPC_MMU_F_HG2 | BCMPC_MMU_F_HG3);
    } else if (pcfg->encap_mode == BCMPC_PORT_ENCAP_HG3) {
        mmu_pcfg->flags |= BCMPC_MMU_F_HG3;
        mmu_pcfg->flags  &= ~(BCMPC_MMU_F_HG2 | BCMPC_MMU_F_IEEE);
    } else if (pcfg->encap_mode == BCMPC_PORT_ENCAP_CELL) {
        mmu_pcfg->flags |= BCMPC_MMU_F_CELL;
        mmu_pcfg->flags &= ~(BCMPC_MMU_F_HG2 | BCMPC_MMU_F_IEEE |
                              BCMPC_PORT_ENCAP_HG3);
    }

    return SHR_E_NONE;
}

/*!
 * \brief Traverse function for setting the MMU port configuration.
 *
 * This function is called for each logical port by traversing the logical port
 * table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie TM configuration, tm_cfg_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_mmu_pcfg_traverse(int unit, size_t key_size, void *key,
                     size_t data_size, void *data,
                     void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_port_cfg_t *pcfg = data;
    tm_cfg_t *tm_cfg = cookie;
    bcmpc_mmu_port_cfg_t *mmu_pcfg = NULL;
    bcmpc_pmgr_port_status_t pst;

    SHR_FUNC_ENTER(unit);

    /* Do not publish invalid ports to TM. */
    if (bcmpc_lport_to_pport(unit, lport) == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmpc_port_phys_map_get(unit, lport, &pcfg->pport));

    if (pcfg->pport >= tm_cfg->num_ports) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Buffer is not enough for phys port[%d].\n"),
              pcfg->pport));
    }

    if (pcfg->autoneg) {
        /*
         * For AN ports, if AN is resolved, the resolved speed and num_lanes
         * should come from current port status.
         */
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_port_status_get(unit, pcfg->pport, &pst));
        if (pst.an_done) {
            pcfg->speed = pst.speed;
            pcfg->num_lanes = pst.num_lanes;
        }
    }

    mmu_pcfg = &(tm_cfg->mmu_pcfg_array[pcfg->pport]);
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_set(unit, lport, pcfg, &tm_cfg->oversub_pbmp, mmu_pcfg));

exit:
    SHR_FUNC_EXIT();
}
static int
pc_mmu_pcfg_list_alloc(int unit, int num_ports, bcmpc_mmu_port_cfg_t **plist);

/*!
 * \brief Build the port config array for TM.
 *
 * This function constructs the list of the port configuration which will be
 * passed to the TM component to reconfigure the MMU resources.
 *
 * The configuration arrays \c cfg are indexed by the physical port number.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration array.
 * \param [out] cfg Port configuration array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_mmu_pcfg_array_build(int unit, size_t num_ports,
                        bcmpc_mmu_port_cfg_t *mmu_pcfg)
{
    tm_cfg_t tm_cfg;
    bcmpc_drv_t *pc_drv = NULL;

    SHR_FUNC_ENTER(unit);

    pc_mmu_port_cfg_init(num_ports, mmu_pcfg);

    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return SHR_E_FAIL;
    }

    if (pc_drv && pc_drv->mmu_pcfg_array) {
        SHR_IF_ERR_EXIT
            (pc_drv->mmu_pcfg_array(unit, num_ports, &mmu_pcfg));
    }

    /* Setup the cookie for the traverse. */
    sal_memset(&tm_cfg, 0, sizeof(tm_cfg));
    tm_cfg.num_ports = num_ports;
    tm_cfg.mmu_pcfg_array = mmu_pcfg;
    SHR_IF_ERR_EXIT
        (pc_oversub_pbmp_get(unit, &tm_cfg.oversub_pbmp));

    /* Traverse the existing ports to construct the MMU port config array. */
    bcmpc_db_imm_entry_traverse(unit,
                                PC_PORTt,
                                sizeof(bcmpc_lport_t),
                                sizeof(bcmpc_port_cfg_t),
                                pc_mmu_pcfg_traverse,
                                &tm_cfg);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update TM configuration.
 *
 * Update TM port configuration via the callbacks of bcmpc_tm_handler_t.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] old_cfg Current port configuration.
 * \param [in] new_cfg New/requested port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_tm_update(int unit, int num_ports,
             bcmpc_mmu_port_cfg_t *mmu_pcfg_old,
             bcmpc_mmu_port_cfg_t *mmu_pcfg_new)
{
    bcmpc_mmu_update_f func;

    SHR_FUNC_ENTER(unit);

    sal_mutex_take(pc_tm_mutex[unit], SAL_MUTEX_FOREVER);
    func = tm_handler[unit].tm_validate;
    if (func) {
        SHR_IF_ERR_EXIT
            (func(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));
    }

    func = tm_handler[unit].tm_configure;
    if (func) {
        SHR_IF_ERR_EXIT
            (func(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));
    }

exit:
    sal_mutex_give(pc_tm_mutex[unit]);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the necessary for port-based TM update.
 *
 * The funnction will check if we need to update TM for each port
 * addition/deletion.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Need to update TM.
 * \retval false No need to update TM.
 */
static int
pc_tm_port_update_check(int unit)
{
    bool enable = 0, manual= 0;

    bcmpc_tm_port_based_update_get(unit, &enable);
    bcmpc_tm_manual_update_mode_get(unit, &manual);

    return (enable && !manual);
}

/*!
 * \brief Allocate an bcmpc_mmu_port_cfg_t array.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports to allocate.
 * \param [out] plist Pointer to the allocated bcmpc_mmu_port_cfg_t array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Failed to allocate the memory.
 */
static int
pc_mmu_pcfg_list_alloc(int unit, int num_ports, bcmpc_mmu_port_cfg_t **plist)
{
    int mem_size;

    SHR_FUNC_ENTER(unit);

    mem_size = num_ports * sizeof(bcmpc_mmu_port_cfg_t);
    SHR_ALLOC(*plist, mem_size, "bcmpcPlist");
    SHR_NULL_CHECK(*plist, SHR_E_MEMORY);
    pc_mmu_port_cfg_init(num_ports, *plist);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate an bcmpc_sec_port_cfg_t array.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports to allocate.
 * \param [out] plist Pointer to the allocated bcmpc_sec_port_cfg_t array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Failed to allocate the memory.
 */
static int
pc_sec_pcfg_list_alloc(int unit, int num_ports, bcmpc_sec_port_cfg_t **plist)
{
    int mem_size;

    SHR_FUNC_ENTER(unit);

    mem_size = num_ports * sizeof(bcmpc_sec_port_cfg_t);
    SHR_ALLOC(*plist, mem_size, "bcmpcPlist");
    SHR_NULL_CHECK(*plist, SHR_E_MEMORY);
    pc_sec_port_cfg_init(num_ports, *plist);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Internal Public functions
 */

int
bcmpc_tm_port_based_update_set(int unit, bool enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    tm_port_based_update[unit] = enable;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_based_update_get(int unit, bool *enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    *enable = tm_port_based_update[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_add(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    int num_ports;
    bcmpc_pbmp_t oversub_pbmp;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;
    bcmpc_mmu_port_cfg_t *mmu_pcfg;
    bcmpc_sec_port_cfg_t *sec_pcfg_old = NULL, *sec_pcfg_new = NULL;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_EXIT();
    }

    num_ports = pc_num_pports_get(unit);
    if (num_ports <= 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_old));

    sal_memcpy(mmu_pcfg_new, mmu_pcfg_old, sizeof(*mmu_pcfg_new) *  num_ports);
    mmu_pcfg = &mmu_pcfg_new[pcfg->pport];

    SHR_IF_ERR_EXIT
        (pc_oversub_pbmp_get(unit, &oversub_pbmp));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_set(unit, lport, pcfg, &oversub_pbmp, mmu_pcfg));

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

    if (bcmpc_sec_valid_get(unit)) {
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_old));
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_new));

        /* Selectively copy required parameters to sec_pcfg from mmu_pcfg. */
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_old, mmu_pcfg_old);
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_new, mmu_pcfg_new);
        SHR_IF_ERR_EXIT
            (pc_sec_update(unit, num_ports, sec_pcfg_old, sec_pcfg_new));
    }

exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);
    SHR_FREE(sec_pcfg_old);
    SHR_FREE(sec_pcfg_new);

    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_delete(int unit, bcmpc_lport_t lport)
{
    int num_ports;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;
    bcmpc_mmu_port_cfg_t *mmu_pcfg;
    bcmpc_sec_port_cfg_t *sec_pcfg_old = NULL, *sec_pcfg_new = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_EXIT();
    }

    num_ports = pc_num_pports_get(unit);
    if (num_ports <= 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_old));

    sal_memcpy(mmu_pcfg_new, mmu_pcfg_old, sizeof(*mmu_pcfg_new) *  num_ports);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);
    if (pport >= num_ports) {
        SHR_IF_ERR_MSG_EXIT
            (SHR_E_FAIL,
             (BSL_META_U(unit, "Buffer is not enough for phys port[%d].\n"),
              pport));
    }
    mmu_pcfg = &mmu_pcfg_new[pport];

    sal_memset(mmu_pcfg, 0, sizeof(*mmu_pcfg));
    mmu_pcfg->lport = BCMPC_INVALID_LPORT;
    mmu_pcfg->pport = pport;

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

    if (bcmpc_sec_valid_get(unit)) {
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_old));
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_new));

        /* Selectively copy required parameters to sec_pcfg from mmu_pcfg. */
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_old, mmu_pcfg_old);
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_new, mmu_pcfg_new);
        SHR_IF_ERR_EXIT
            (pc_sec_update(unit, num_ports, sec_pcfg_old, sec_pcfg_new));
    }


exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);
    SHR_FREE(sec_pcfg_old);
    SHR_FREE(sec_pcfg_new);

    SHR_FUNC_EXIT();
}

int
bcmpc_tm_op_exec(int unit, bcmpc_pport_t pport, bcmpc_operation_t *op)
{
    bcmpc_tm_port_op_exec_f func;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_EXIT();
    }

    func = tm_handler[unit].port_op_exec;
    if (!func) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (func(unit, pport, op->name, op->param));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_port_update(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *old_pcfg,
                     bcmpc_port_cfg_t *pcfg)
{
    int num_ports;
    bcmpc_pbmp_t oversub_pbmp;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;
    bcmpc_sec_port_cfg_t *sec_pcfg_old = NULL, *sec_pcfg_new = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;

    SHR_FUNC_ENTER(unit);

    if (pc_tm_port_update_check(unit) == 0) {
        SHR_EXIT();
    }

    /* Do not notify TM invalid port. */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);
    if ((pport != pcfg->pport) || (pport != old_pcfg->pport)) {
        SHR_EXIT();
    }

    /* Notify TM only about this logical port. */
    num_ports = 1;

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_oversub_pbmp_get(unit, &oversub_pbmp));

    /* Populate the old configuration for the logical port to notify TM. */
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_set(unit, lport, old_pcfg, &oversub_pbmp, mmu_pcfg_old));

    /* Populate the new configuration for the logical port to notify TM. */
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_set(unit, lport, pcfg, &oversub_pbmp, mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

    if (bcmpc_sec_valid_get(unit)) {
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_old));
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_new));

        /* Selectively copy required parameters to sec_pcfg from mmu_pcfg. */
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_old, mmu_pcfg_old);
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_new, mmu_pcfg_new);
        SHR_IF_ERR_EXIT
            (pc_sec_update(unit, num_ports, sec_pcfg_old, sec_pcfg_new));
    }


exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);
    SHR_FREE(sec_pcfg_old);
    SHR_FREE(sec_pcfg_new);

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int
bcmpc_tm_handler_register(int unit, bcmpc_tm_handler_t *hdl)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    sal_memcpy(&tm_handler[unit], hdl, sizeof(*hdl));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_manual_update_mode_set(int unit, bool manual)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PC->TM callback mode: %s\n"),
                 manual ? "manual" : "auto"));

    tm_manual_update[unit] = manual;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_manual_update_mode_get(int unit, bool *manual)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PC->TM callback mode: %s\n"),
                 manual ? "manual" : "auto"));

    *manual = tm_manual_update[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_tm_update_now(int unit)
{
    int num_ports;
    bcmdrd_dev_type_t dev_type;
    bcmpc_mmu_port_cfg_t *mmu_pcfg_old = NULL, *mmu_pcfg_new = NULL;
    bcmpc_sec_port_cfg_t *sec_pcfg_old = NULL, *sec_pcfg_new = NULL;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    num_ports = pc_num_pports_get(unit);
    if (num_ports <= 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_old));
    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_list_alloc(unit, num_ports, &mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_mmu_pcfg_array_build(unit, num_ports, mmu_pcfg_new));

    SHR_IF_ERR_EXIT
        (pc_tm_update(unit, num_ports, mmu_pcfg_old, mmu_pcfg_new));

    if (bcmpc_sec_valid_get(unit)) {
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_old));
        SHR_IF_ERR_EXIT
            (pc_sec_pcfg_list_alloc(unit, num_ports, &sec_pcfg_new));

        /* Selectively copy required parameters to sec_pcfg from mmu_pcfg. */
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_old, mmu_pcfg_old);
        (void)tm_sec_port_cfg_init(num_ports, sec_pcfg_new, mmu_pcfg_new);
        SHR_IF_ERR_EXIT
            (pc_sec_update(unit, num_ports, sec_pcfg_old, sec_pcfg_new));
    }


exit:
    SHR_FREE(mmu_pcfg_old);
    SHR_FREE(mmu_pcfg_new);
    SHR_FREE(sec_pcfg_old);
    SHR_FREE(sec_pcfg_new);

    SHR_FUNC_EXIT();
}

int
bcmpc_pc_tm_mutex_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (pc_tm_mutex[unit]) {
        SHR_EXIT();
    }

    pc_tm_mutex[unit] = sal_mutex_create("bcmpcPcTm");
    SHR_NULL_CHECK(pc_tm_mutex[unit], SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pc_tm_mutex_cleanup(int unit)
{
    if (!pc_tm_mutex[unit]) {
        return;
    }

    sal_mutex_destroy(pc_tm_mutex[unit]);
    pc_tm_mutex[unit] = NULL;
}
