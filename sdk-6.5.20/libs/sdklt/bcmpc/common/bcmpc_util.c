/*! \file bcmpc_util.c
 *
 * BCMPC internal utilites.
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
#include <bcmlrd/bcmlrd_table.h>

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <phymod/phymod.h>
#include <phymod/phymod_sim_control.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_DB

/* Convenient macro for setting phymod_access_t */
#define PHYMOD_ACCESS_T_SET(_acc, _u, _drv, _p, _m, _user_data) \
    do { \
        PHYMOD_ACC_USER_ACC(_acc) = (_user_data); \
        PHYMOD_ACC_LANE_MASK(_acc) = (_m); \
        if ((_drv)->phy_bus_get) { \
            PHYMOD_ACC_BUS(_acc) = (_drv)->phy_bus_get((_u), (_p)); \
        } \
        if ((_drv)->phy_addr_get) { \
            PHYMOD_ACC_ADDR(_acc) = (_drv)->phy_addr_get((_u), (_p)); \
        } \
        if ((_drv)->phy_pll_idx_get) { \
            PHYMOD_ACC_TVCOIDX(_acc) = (_drv)->phy_pll_idx_get((_u), (_p), BCMPC_PLL_TYPE_TVCO); \
        } \
    } while (0)

/*!
 * \brief Port macro mode entry structure.
 *
 * The key of the entry, i.e. \c dbe->key, is a PM ID.
 */
typedef struct bcmpc_pm_mode_stage_s {

    /*! DB entry. */
    bcmpc_db_entry_t dbe;

    /*! PM mode. */
    bcmpc_pm_mode_t pm_mode;

} bcmpc_pm_mode_stage_t;

/*! PC driver objects. */
static bcmpc_drv_t *bcmpc_drv[BCMPC_NUM_UNITS_MAX];

/*! PM mode stage database. */
static bcmpc_db_entry_t *pm_mode_stage_db[BCMPC_NUM_UNITS_MAX];


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the PHYMOD dispatch type.
 *
 * Get the PHYMOD driver for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pc_drv PC driver.
 * \param [in] pport Physical device port.
 *
 * \return The phymod driver for the port.
 */
static phymod_dispatch_type_t
port_phy_drv_get(int unit, bcmpc_drv_t *pc_drv, bcmpc_pport_t pport)
{
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        return pc_drv->phy_drv_get(unit, pport);
    }
    if (!bcmdrd_feature_is_real_hw(unit)) {
        return phymodDispatchTypeNull;
    }

    return pc_drv->phy_drv_get(unit, pport);
}

static int
bcmpc_phymod_access_t_fw_pmd_info_init(int unit, bcmpc_pport_t pport,
                                       phymod_access_t *phy_acc)
{
    int pm_id;
    pm_info_t *pm_drv_info;

    SHR_FUNC_ENTER(unit);

    if (phy_acc == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_drv_info_get(unit, pm_id, &pm_drv_info));

    if (pm_drv_info->pm_data.pmd_fw_info != NULL) {
        phy_acc->pmd_info_ptr = pm_drv_info->pm_data.pmd_fw_info;
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Internal public functions
 */

int
bcmpc_drv_set(int unit, bcmpc_drv_t *drv)
{
    bcmpc_drv[unit] = drv;

    return SHR_E_NONE;
}

int
bcmpc_drv_get(int unit, bcmpc_drv_t **drv)
{
    *drv = bcmpc_drv[unit];

    return SHR_E_NONE;
}

void
bcmpc_pm_info_t_init(bcmpc_pm_info_t *pm_info)
{
    pm_info->base_pport = BCMPC_INVALID_PPORT;
    bcmpc_topo_type_t_init(&pm_info->prop);
}

int
bcmpc_pm_info_get(int unit, int pm_id, bcmpc_pm_info_t *pm_info)
{
    bcmpc_topo_t topo;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &topo));
    pm_info->base_pport = topo.base_pport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_type_get(unit, topo.tid, &pm_info->prop));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_pmd_common_clk_get(int unit, bcmpc_pport_t pport, int *ref_clk)
{
    int rv, pm_id;
    bcmpc_pm_info_t pm_info;
    bcmpc_pm_core_t pm_core;
    bcmpc_pm_core_cfg_t ccfg;

    SHR_FUNC_ENTER(unit);

    *ref_clk = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    pm_core.pm_id = pm_id;
    pm_core.core_idx = 0;
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PM_COREt, P(&pm_core), P(&ccfg));
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    if (SHR_SUCCESS(rv)) {
        *ref_clk = ccfg.pmd_com_clk;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_stage_set(int unit, int pm_id, bcmpc_pm_mode_t *pm_mode)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_pm_mode_stage_t *entry = NULL;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(pm_mode_stage_db[unit], 1, &key, NULL);
    if (!dbe) {
        /* Allocate a new entry */
        SHR_ALLOC(entry, sizeof(*entry), "bcmpcPmModeStage");
        SHR_NULL_CHECK(entry, SHR_E_MEMORY);
        sal_memset(entry, 0, sizeof(*entry));
        entry->dbe.keys[0] = key;
        bcmpc_db_entry_add(&pm_mode_stage_db[unit], &entry->dbe);
    } else {
        entry = BCMPC_DB_ENTRY(dbe, bcmpc_pm_mode_stage_t, dbe);
    }

    sal_memcpy(&(entry->pm_mode), pm_mode, sizeof(bcmpc_pm_mode_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_stage_get(int unit, int pm_id, bcmpc_pm_mode_t **pm_mode)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_pm_mode_stage_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_search(pm_mode_stage_db[unit], 1, &key, NULL);
    if (!dbe) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_pm_mode_stage_t, dbe);
    *pm_mode = &entry->pm_mode;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_mode_stage_delete(int unit, int pm_id)
{
    bcmpc_db_entry_t *dbe;
    bcmpc_pm_mode_stage_t *entry;
    uint32_t key = pm_id;

    SHR_FUNC_ENTER(unit);

    dbe = bcmpc_db_entry_del(&pm_mode_stage_db[unit], 1, &key);
    if (!dbe) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    entry = BCMPC_DB_ENTRY(dbe, bcmpc_pm_mode_stage_t, dbe);
    SHR_FREE(entry);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_linkscan_mode_get(int unit, bcmpc_lport_t lport,
                        bcmpc_lm_mode_t *lm_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_lookup(unit, LM_PORT_CONTROLt,
                                   P(&lport), P(lm_mode)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_port_phys_map_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t *pport)
{
    bcmpc_port_cfg_t l2p_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORT_PHYS_MAPt,
                                   P(&lport), P(&l2p_cfg)));

exit:
    *pport = SHR_FUNC_ERR() ? BCMPC_INVALID_PPORT : l2p_cfg.pport;
    SHR_FUNC_EXIT();
}

int
bcmpc_phymod_phy_access_t_init(int unit, bcmpc_pport_t pport,
                               uint32_t ovrr_lane_mask,
                               bcmpc_phy_access_data_t *phy_data,
                               phymod_phy_access_t *pa)
{
    bcmpc_drv_t *pc_drv;
    uint32_t lane_mask = 0;
    phymod_dispatch_type_t drv_type;

    SHR_FUNC_ENTER(unit);

    if (phy_data == NULL || pa == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    drv_type = port_phy_drv_get(unit, pc_drv, pport);
    if (drv_type == phymodDispatchTypeCount) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (ovrr_lane_mask != 0) {
        lane_mask = ovrr_lane_mask;
    } else {
        bcmpc_pm_lport_info_t lport_info;

        bcmpc_pm_lport_info_t_init(&lport_info);
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info),
                                     SHR_E_NOT_FOUND);
        lane_mask = lport_info.lbmp;
    }

    phymod_phy_access_t_init(pa);
    pa->type = drv_type;
    PHYMOD_ACCESS_T_SET(&pa->access, unit, pc_drv, pport, lane_mask, phy_data);
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        PHYMOD_ACC_F_PHYSIM_SET(&pa->access);
        PHYMOD_ACC_F_CLAUSE45_SET(&pa->access);
    }

    bcmpc_phymod_access_t_fw_pmd_info_init(unit, pport, &pa->access);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phymod_core_access_t_init(int unit, bcmpc_pport_t pport,
                                bcmpc_phy_access_data_t *phy_data,
                                phymod_core_access_t *ca)
{
    bcmpc_drv_t *pc_drv;
    phymod_dispatch_type_t drv_type;
    bool is_internal = false;

    SHR_FUNC_ENTER(unit);

    if (phy_data == NULL || ca == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT
        (pc_drv->is_internal_port(unit, pport, &is_internal));
    if (is_internal == true) {
        SHR_EXIT();
    }

    drv_type = port_phy_drv_get(unit, pc_drv, pport);
    if (drv_type == phymodDispatchTypeCount) {
        SHR_ERR_MSG_EXIT(SHR_E_PORT,
                         (BSL_META_U(unit,
                                     "No PHY driver for port %d\n"), pport));
    }

    phymod_core_access_t_init(ca);
    ca->type = drv_type;
    PHYMOD_ACCESS_T_SET(&ca->access, unit, pc_drv, pport, 0, phy_data);
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_PHYMOD_SIM)) {
        PHYMOD_ACC_F_PHYSIM_SET(&ca->access);
        PHYMOD_ACC_F_CLAUSE45_SET(&ca->access);
    }
    bcmpc_phymod_access_t_fw_pmd_info_init(unit, pport, &ca->access);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_valid_block_pbmp_get(const bcmdrd_chip_info_t *ci, int blktype,
                           bcmdrd_pbmp_t *pbmp)
{
    int port;
    bcmdrd_pblk_t pblk;
    bcmdrd_pbmp_t pbmp_all;

    BCMDRD_PBMP_CLEAR(*pbmp);

    if (bcmdrd_chip_blktype_pbmp(ci, blktype, &pbmp_all) < 0) {
        return SHR_E_FAIL;
    }

    BCMDRD_PBMP_ITER(pbmp_all, port) {
        if (bcmdrd_chip_port_block(ci, port, blktype, &pblk) < 0) {
            return SHR_E_FAIL;
        }
        if (pblk.lane == 0) {
            BCMDRD_PBMP_PORT_ADD(*pbmp, port);
        }
    }

    return SHR_E_NONE;
}

int
bcmpc_block_pbmp_get(int unit, const bcmdrd_chip_info_t *ci, int blktype,
                     bcmdrd_pbmp_t *pbmp, bcmdrd_pbmp_t *pbmp_inactive)
{
    int port;
    const bcmdrd_block_t *blkp;
    bcmdrd_pbmp_t pbmp_valid;
    bcmdrd_pbmp_t pbmp_temp;

    SHR_FUNC_ENTER(unit);

    BCMDRD_PBMP_CLEAR(*pbmp);
    BCMDRD_PBMP_CLEAR(*pbmp_inactive);
    BCMDRD_PBMP_CLEAR(pbmp_valid);
    BCMDRD_PBMP_CLEAR(pbmp_temp);

    /* Get port bitmap for the blktype. */
    SHR_IF_ERR_EXIT
        (bcmdrd_dev_blktype_pbmp(unit, blktype, &pbmp_valid));
    for (blkp = ci->blocks; blkp < ci->blocks + ci->nblocks; blkp++) {
        if (blkp->type != blktype || BCMDRD_PBMP_IS_NULL(blkp->pbmps)) {
            continue;
        }

        /* Copy valid port bitmap on the device. */
        BCMDRD_PBMP_OR(pbmp_temp, pbmp_valid);
        for (port = 0; port < BCMDRD_CONFIG_MAX_PORTS; port++) {
            /* Get the first physical port number of the block. */
            if (BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
                /* Save the first port number in the output port bitmap. */
                BCMDRD_PBMP_PORT_ADD(*pbmp, port);
                /* Check if there are valid physical ports on the block,
                 * otherwise, set the port bitmap as inactive.
                 */
                BCMDRD_PBMP_AND(pbmp_temp, blkp->pbmps);
                if (BCMDRD_PBMP_IS_NULL(pbmp_temp)) {
                    BCMDRD_PBMP_PORT_ADD(*pbmp_inactive, port);
                }
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_pm_lport_info_t_init(bcmpc_pm_lport_info_t *lp_info)
{
    sal_memset(lp_info, 0, sizeof(bcmpc_pm_lport_info_t));
}

int
bcmpc_pm_lport_info_get(int unit, bcmpc_pport_t pport,
                         bcmpc_port_cfg_t *pcfg,
                        bcmpc_pm_lport_info_t *lp_info)
{
    bcmpc_drv_t *pc_drv;
    int pm_id, pm_phys_port, num_lanes, i;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_t pm_inst_info;

    SHR_FUNC_ENTER(unit);

    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }
    bcmpc_pm_lport_info_t_init(lp_info);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_inst_info));

    /* Physical PM port number */
    pm_phys_port = pport - pm_info.base_pport;

    /* Optionally override the default value. */
    if (pc_drv->pm_pport_get) {
        pc_drv->pm_pport_get(unit, pport, &pm_phys_port);
    }
    if (pcfg) {
        num_lanes = pcfg->num_lanes;
    } else {
        num_lanes = pm_inst_info.port_num_lanes[pm_phys_port];
    }
    for (i = 0; i < num_lanes; i++) {
        lp_info->lbmp |= (1 << i);
    }

    if (pc_drv->port_lbmp_get) {
        pc_drv->port_lbmp_get(unit, pport, pm_phys_port, &lp_info->lbmp);
    } else {
        lp_info->lbmp <<= pm_phys_port;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_topo_inst_info_update(int unit, bcmpc_port_cfg_t *pcfg)
{
    bcmpc_topo_t pm_inst_info;
    bcmpc_drv_t *pc_drv;
    int pm_id, pm_pport;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
         (bcmpc_drv_get(unit, &pc_drv));

    /* Get PM type based on physical port number. */
    SHR_IF_ERR_EXIT
         (bcmpc_topo_id_get(unit, pcfg->pport, &pm_id));
    SHR_IF_ERR_EXIT
         (bcmpc_topo_get(unit, pm_id, &pm_inst_info));

    pm_pport = pcfg->pport - pm_inst_info.base_pport;
    if (pc_drv->pm_pport_get) {
        pc_drv->pm_pport_get(unit, pcfg->pport, &pm_pport);
    }

    /*! Update PM instance port number of lanes */
    pm_inst_info.port_num_lanes[pm_pport] = pcfg->num_lanes;

    /*! Update PM instance info */
    SHR_IF_ERR_EXIT
         (bcmpc_topo_update(unit, pm_id, &pm_inst_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_type_params_set(int unit, int tid, pm_info_t *pm_drv_info)
{
    bcmpc_topo_type_t tinfo;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_type_get(unit, tid, &tinfo));

    if (sal_strcmp(tinfo.name, "PC_PM_TYPE_PM4X10") == 0) {
        pm_drv_info->type = PM_TYPE_PM4X10;
    } else if (sal_strcmp(tinfo.name, "PC_PM_TYPE_PM8X50") == 0) {
        pm_drv_info->type = PM_TYPE_PM8X50;
    } else if (sal_strcmp(tinfo.name, "PC_PM_TYPE_PM8X50_GEN2") == 0) {
        pm_drv_info->type = PM_TYPE_PM8X50_GEN2;
    } else if (sal_strcmp(tinfo.name, "PC_PM_TYPE_PM8X50_GEN3") == 0) {
        pm_drv_info->type = PM_TYPE_PM8X50_GEN3;
    } else if (sal_strcmp(tinfo.name, "PC_PM_TYPE_PM8X100") == 0) {
        pm_drv_info->type = PM_TYPE_PM8X100;
    } else if (sal_strcmp(tinfo.name, "PC_PM_TYPE_PM8X100_GEN2") == 0) {
        pm_drv_info->type = PM_TYPE_PM8X100_GEN2;
    } else {
        pm_drv_info->type = PM_TYPE_INVALID;
    }

    pm_drv_info->pm_data.ref_clk = tinfo.ref_clk;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pm_drv_info_get(int unit, int pm_id, pm_info_t **pm_info)
{
    bcmpc_topo_t topo;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &topo));

    *pm_info = topo.pm_info;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_interrupt_enable(int unit,
                       bcmpc_pport_t pport,
                       int intr_num,
                       bcmbd_intr_f intr_func,
                       bcmpc_intr_enable_t enable)
{
    int intr_inst;
    SHR_FUNC_ENTER(unit);

    intr_inst = intr_num;
    PORT_INTR_INST_SET(intr_inst, pport);
    switch (enable) {
        case INTR_ENABLE:
            /*
             * Enable the interrupt and register the
             * interrupt callback function.
             */
            SHR_IF_ERR_EXIT
                (bcmbd_port_intr_func_set(unit, intr_num, intr_func, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_port_intr_enable(unit, intr_inst));
            break;

        case INTR_DISABLE:
            /* Disable the interrupti, clear the interrupt before disable. */
            SHR_IF_ERR_EXIT
                (bcmbd_port_intr_clear(unit, intr_inst));
            SHR_IF_ERR_EXIT
                (bcmbd_port_intr_disable(unit, intr_inst));
            break;

        case INTR_CLEAR:
            /* Clear the interrupt. */
            SHR_IF_ERR_EXIT
                (bcmbd_port_intr_clear(unit, intr_inst));
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_interrupt_process(int unit, bcmpc_lport_t lport,
                        bcmpc_intr_type_t intr_type,
                        uint32_t *is_handled)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_interrupt_process(unit, lport, intr_type,
                                      is_handled));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_lt_entry_get(int unit, bcmltd_sid_t sid,
                   size_t key_size, void *key,
                   size_t data_size, void *data)
{
    int rv = SHR_E_NONE;
    bcmpc_port_lane_t *port_lane = NULL;
    bcmpc_tx_taps_config_t *tx_taps = NULL;

    SHR_FUNC_ENTER(unit);
    /*
     * Get the logical table name to retrieve the key.
     * If the table is TX_TAPS or PHY_CONTROL, the key is
     * logical port and lane index
     * else
     * key is logical port.
     */
    switch (sid) {
        case PC_TX_TAPSt:
            /* If key_size is not same a port_lane size, return error. */
            if ((key_size != sizeof(bcmpc_port_lane_t)) ||
                (data_size != sizeof(bcmpc_tx_taps_config_t))) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            } else {
                port_lane = (bcmpc_port_lane_t *) key;
                tx_taps = (bcmpc_tx_taps_config_t *) data;
            }
            rv = bcmpc_db_imm_entry_lookup(unit, PC_TX_TAPSt, P(port_lane),
                                           P(tx_taps));

            if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
                SHR_ERR_EXIT(rv);
            }

            SHR_IF_ERR_EXIT
                (bcmpc_lt_tx_taps_default_values_init(unit, port_lane,
                                                      tx_taps));
            break;
        case PC_MAC_CONTROLt:
        case PC_PFCt:
        case PC_PORT_TIMESYNCt:
        case PC_PMD_FIRMWAREt:
        case PC_PHY_CONTROLt:
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize values of all the fields of TX_TAPS lt entry
 * with user provisioned fields and the default values for
 * the fields not provisioned by user application.
 * The caller of this function must have already performed
 * a lookup on the LT. This function C\checks the field bitmap
 * and populates only the fields which are not provisioned
 * (inicated by the bit not being set) with default value from
 * the LT map file.
 *
 * \param [in] unit Unit number.
 * \param [in] unit Unit number.
 * \param [in/out] entry PC_TX_TAPs LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
int
bcmpc_lt_tx_taps_default_values_init(int unit,
                                     bcmpc_port_lane_t *port_lane,
                                     bcmpc_tx_taps_config_t *entry)
{
    uint64_t def_val = 0;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    /*
     * Check the field bitmap and populate the entry with
     * default value if field is not provisioned in the LT
     */

    /*
     * Check if PRE_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_PRE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_PRE_AUTOf,
                                     1, &def_val, &num));
        entry->pre_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_PRE is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_PRE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_PREf,
                                     1, &def_val, &num));
    }

    /*
     * Check if TX_PRE2_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_PRE2_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_PRE2_AUTOf,
                                     1, &def_val, &num));
        entry->pre2_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_PRE2 is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_PRE2)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_PRE2f,
                                     1, &def_val, &num));
        entry->pre2 = (uint16_t)def_val;
    }

    /*
     * Check if TX_MAIN_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_MAIN_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_MAIN_AUTOf,
                                     1, &def_val, &num));
        entry->main_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_MAIN is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_MAIN)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_MAINf,
                                     1, &def_val, &num));
            entry->main = (uint16_t)def_val;
    }

    /*
     * Check if TX_POST_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_POST_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_POST_AUTOf,
                                     1, &def_val, &num));
        entry->post_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_POST is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_POST)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_POSTf,
                                     1, &def_val, &num));
            entry->post = (uint16_t)def_val;
    }

    /*
     * Check if TX_POST2_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_POST2_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_POST2_AUTOf,
                                     1, &def_val, &num));
        entry->post2_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_POST2 is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_POST2)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_POST2f,
                                     1, &def_val, &num));
        entry->post2 = (uint16_t)def_val;
    }

    /*
     * Check if TX_POST3_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_POST3_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_POST3_AUTOf,
                                     1, &def_val, &num));
        entry->post3_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_POST3 is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_POST3)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_POST3f,
                                     1, &def_val, &num));
        entry->post3 = (uint16_t)def_val;
    }

    /*
     * Check if TX_RPARA_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_RPARA_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_RPARA_AUTOf,
                                     1, &def_val, &num));
        entry->rpara_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_RPARA is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_RPARA)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_RPARAf,
                                     1, &def_val, &num));
        entry->rpara = (uint16_t)def_val;
    }

    /*
     * Check if TX_AMP_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_AMP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_AMP_AUTOf,
                                     1, &def_val, &num));
        entry->amp_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_AMP is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_AMP)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_AMPf,
                                     1, &def_val, &num));
        entry->amp = (uint16_t)def_val;
    }

    /*
     * Check if TX_DRV_MODE_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_DRV_MODE_AUTOf,
                                     1, &def_val, &num));
        entry->drv_mode_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_DRV_MODE is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_DRV_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_DRV_MODEf,
                                     1, &def_val, &num));
        entry->drv_mode = (uint16_t)def_val;
    }

    /*
     * Check if TXFIR_TAP_MODE_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TXFIR_TAP_MODE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TXFIR_TAP_MODE_AUTOf,
                                     1, &def_val, &num));
        entry->txfir_tap_mode_auto = (uint8_t)def_val;
    }

    /*
     * Check if TXFIR_TAP_MODE is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TXFIR_TAP_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TXFIR_TAP_MODEf,
                                     1, &def_val, &num));
        entry->txfir_tap_mode = (uint16_t)def_val;
    }

    /*
     * Check if TX_SIG_MODE_AUTO is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_SIG_MODE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_SIG_MODE_AUTOf,
                                     1, &def_val, &num));
        entry->sig_mode_auto = (uint8_t)def_val;
    }

    /*
     * Check if TX_SIG_MODE is configured by the user,
     * Set to default value if not configured.
     */
    if (!BCMPC_LT_FIELD_GET(entry->fbmp,
                            BCMPC_TX_TAPS_LT_FLD_TX_SIG_MODE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_TX_TAPSt,
                                     PC_TX_TAPSt_TX_SIG_MODEf,
                                     1, &def_val, &num));
        entry->sig_mode = (uint16_t)def_val;
    }
exit:
    SHR_FUNC_EXIT();
}

int
port_oper_status_table_insert(int unit, size_t key_size, void *key,
                              size_t data_size, void *data, void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_pport_t pport = *(bcmpc_pport_t *)data;
    bcmpc_port_oper_status_t op_st;
    bcmpc_port_oper_status_entry_t *pentry =
                             (bcmpc_port_oper_status_entry_t *) cookie;
    int rv = SHR_E_NONE;


    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(bcmpc_port_oper_status_t));

    /*
     * Get the logical port from physical port for which the
     * operation failed.
     */
    if (pport == pentry->pport) {
        rv = (bcmpc_db_imm_entry_lookup(unit, PC_PORT_INFOt,
                                        sizeof(bcmpc_lport_t), &lport,
                                        sizeof(bcmpc_port_oper_status_t),
                                        &op_st));

        op_st.status = pentry->oper_status.status;
        op_st.num_port = pentry->oper_status.num_port;
        sal_memcpy(&op_st.port_list, pentry->oper_status.port_list,
                   sizeof(op_st.port_list));

        if (rv == SHR_E_NOT_FOUND) {
            rv = (bcmpc_db_imm_entry_insert(unit, PC_PORT_INFOt,
                                            sizeof(bcmpc_lport_t), &lport,
                                            sizeof(bcmpc_port_oper_status_t),
                                            &op_st));
        } else {
            rv = (bcmpc_db_imm_entry_update(unit, PC_PORT_INFOt,
                                            sizeof(bcmpc_lport_t), &lport,
                                            sizeof(bcmpc_port_oper_status_t),
                                            &op_st));
        }
            SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

int
port_oper_status_table_populate(int unit, bcmpc_pport_t pport,
                                pm_oper_status_t *op_st)
{
    bcmpc_port_oper_status_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmpc_port_oper_status_entry_t));

    entry.pport = pport;

    sal_memcpy(&entry.oper_status, op_st, sizeof(bcmpc_port_oper_status_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_traverse(unit, PC_PORT_PHYS_MAPt,
                                     sizeof(bcmpc_lport_t),
                                     sizeof(bcmpc_pport_t),
                                     port_oper_status_table_insert,
                                     &entry));
exit:
    SHR_FUNC_EXIT();
}
