/*! \file bcmpc_phy.c
 *
 * PHY related functions in Port Control.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_phy.h>
#include <bcmpc/bcmpc_pm_internal.h>

/* Log source for this component */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PHYMOD

/* TSC_ADDr[24] TSC register MPP0 access is enabled. */
#define BCMPC_TSC_REG_ACC_MPP0      0x1
/* TSC_ADDr[25] TSC register MPP1 access is enabled. */
#define BCMPC_TSC_REG_ACC_MPP1      0x2

/* TSCO one-copy register check */
#define BCMPC_TSCO_ONE_COPY_REG(reg_addr) \
        (reg_addr < 0xc000)

/* TSCO four-copy register check */
#define BCMPC_TSCO_FOUR_COPY_REG(reg_addr) \
        (((0xc010 <= reg_addr) && (reg_addr <= 0xc018)) || \
         ((0xc150 <= reg_addr) && (reg_addr <= 0xc15b)) || \
         ((0xc170 <= reg_addr) && (reg_addr <= 0xc17d)) || \
         ((0xc1a0 <= reg_addr) && (reg_addr <= 0xc1ab)) || \
         ((0xc210 <= reg_addr) && (reg_addr <= 0xc212)))

static int
phy_acc_validate(int unit, bcmpc_topo_t *pm_info, bcmpc_phy_acc_ovrr_t *ovrr)
{
    SHR_FUNC_ENTER(unit);

    if (pm_info->pm_info->type == PM_TYPE_PM4X10) {
        if (ovrr->lane_index > 3 && ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Lane index is out of range(0-3).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (ovrr->pll_index !=0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "PLL index is out of range(0).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if ((pm_info->pm_info->type == PM_TYPE_PM8X50) ||
               (pm_info->pm_info->type == PM_TYPE_PM8X50_GEN2)) {
        if (ovrr->lane_index > 7 && ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Lane index is out of range(0-7).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if ((ovrr->pll_index > 1)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "PLL index is out of range(0-1).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (pm_info->pm_info->type == PM_TYPE_PM8X50_GEN3) {
        if (ovrr->lane_index > 7 && ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Lane index is out of range(0-7).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if ((ovrr->pll_index != 0)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "PLL index is out of range(0).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (pm_info->pm_info->type == PM_TYPE_PM8X100) {
        if (ovrr->lane_index > 15 && ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Lane index is out of range(0-15).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if ((ovrr->pll_index != 0)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "PLL index is out of range(0).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (pm_info->pm_info->type == PM_TYPE_PM8X100_GEN2) {
        if (ovrr->lane_index > 15 && ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Lane index is out of range(0-15).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if ((ovrr->pll_index > 1)) {
            LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "PLL index is out of range(0-1).\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "PM ID does not match PM type.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static void
phy_acc_ovrr_update(phymod_access_t *acc, bcmpc_phy_acc_ovrr_t *ovrr)
{
    uint32_t lane_mask = 0;

    
    if (ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
        lane_mask = 1 << ovrr->lane_index;
        PHYMOD_ACC_LANE_MASK(acc) = lane_mask;
    }
    if (ovrr->pll_index >= 0) {
        PHYMOD_ACC_PLLIDX(acc) = ovrr->pll_index;
    }
}

int
bcmpc_phy_acc_ovrr_t_init(bcmpc_phy_acc_ovrr_t *ovrr)
{
    if (ovrr == NULL) {
        return SHR_E_PARAM;
    }
    sal_memset(ovrr, 0, sizeof(*ovrr));

    return SHR_E_NONE;
}

int
bcmpc_phy_reg_read(int unit, bcmpc_pport_t pport,
                   uint32_t reg_addr, uint32_t *val,
                   bcmpc_phy_acc_ovrr_t *ovrr)
{
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;
    bcmpc_topo_t pm_info;
    int pm_id = 0;

    SHR_FUNC_ENTER(unit);

    if (val == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    if (ovrr != NULL) {
        SHR_IF_ERR_EXIT
            (phy_acc_validate(unit, &pm_info, ovrr));
        phy_acc_ovrr_update(&pa->access, ovrr);
    }

    PHY_IF_ERR_EXIT
        (phymod_phy_reg_read(pa, reg_addr, val));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_reg_write(int unit, bcmpc_pport_t pport,
                    uint32_t reg_addr, uint32_t val,
                    bcmpc_phy_acc_ovrr_t *ovrr)
{
    phymod_phy_access_t phy_access, *pa = &phy_access;
    bcmpc_phy_access_data_t acc_data;
    bcmpc_topo_t pm_info;
    int pm_id = 0;

    SHR_FUNC_ENTER(unit);

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    if (ovrr != NULL) {
        SHR_IF_ERR_EXIT
            (phy_acc_validate(unit, &pm_info, ovrr));
        phy_acc_ovrr_update(&pa->access, ovrr);
    }

    PHY_IF_ERR_EXIT
        (phymod_phy_reg_write(pa, reg_addr, val));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_info_get(int unit, bcmpc_pport_t pport, bcmpc_phy_info_t *phy_info)
{
    int rv, pm_id = 0;
    phymod_core_access_t core_access, *ca = &core_access;
    phymod_core_info_t core_info;
    bcmpc_phy_access_data_t acc_data;
    bcmpc_topo_t pm_info;
    bcmpc_topo_type_t tinfo;
    bcmpc_pm_lport_info_t lport_info;
    bool is_internal = false;
    bcmpc_drv_t *pc_drv;

    SHR_FUNC_ENTER(unit);

    if (phy_info == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sal_memset(phy_info, 0, sizeof(*phy_info));

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT
        (pc_drv->is_internal_port(unit, pport, &is_internal));
    if (is_internal == true) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_phymod_core_access_t_init(unit, pport, &acc_data, ca));

    phymod_core_info_t_init(&core_info);
    rv = phymod_core_info_get(ca, &core_info);
    if (rv != PHYMOD_E_NONE) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    SHR_IF_ERR_EXIT
         (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_EXIT
         (bcmpc_topo_get(unit, pm_id, &pm_info));
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, pm_info.tid, &tinfo));
    SHR_IF_ERR_EXIT
         (bcmpc_pm_lport_info_get(unit, pport, NULL, &lport_info));

    phy_info->addr = ca->access.addr;
    sal_strlcpy(phy_info->name, core_info.name, BCMPC_PHY_NAME_MAX);
    phy_info->pm_id = pm_id;
    phy_info->num_lanes = tinfo.num_lanes;
    phy_info->pbmp = lport_info.lbmp;

exit:
    SHR_FUNC_EXIT();
}

static void
phy_raw_reg_addr_access_update(bcmpc_topo_t *pm_info, uint32_t *reg_addr,
                               bcmpc_phy_acc_ovrr_t *ovrr, uint8_t *pml_sel)
{
    uint32_t aer = 0, devad = 0;
    uint32_t pll_index = 0, lane = 0;
    uint8_t mpp_sel = 0;

    *pml_sel = 0;
    if (ovrr != NULL) {
        if (ovrr->lane_index != NO_LN_IDX_SPECIFIED) {
            lane = ovrr->lane_index;
        }
        if (ovrr->pll_index >= 0) {
             pll_index = ovrr->pll_index;
        }
    }

    devad = (*reg_addr >> 16) & 0xf;
    /* Mask raw register value. */
    *reg_addr &= 0xffff;
    if (pm_info->pm_info->type == PM_TYPE_PM4X10) {
        aer = lane % 4;
        if (devad) {
            aer |= (devad << 11) | (pll_index << 8);
        }
    } else if ((pm_info->pm_info->type == PM_TYPE_PM8X100 && !devad) ||
               (pm_info->pm_info->type == PM_TYPE_PM8X100_GEN2 && !devad)) {
        if (BCMPC_TSCO_ONE_COPY_REG(*reg_addr)) {
            /* One-copy register */
            *pml_sel = (lane >> 2) & 0x1;
            mpp_sel = (lane >> 1) & 0x1 ? BCMPC_TSC_REG_ACC_MPP1 :
                                          BCMPC_TSC_REG_ACC_MPP0;
            lane = 0;
        } else if (BCMPC_TSCO_FOUR_COPY_REG(*reg_addr)) {
            /* Four-copy register */
            *pml_sel = (lane >> 3) & 0x1;
            mpp_sel = (lane >> 2) & 0x1 ? BCMPC_TSC_REG_ACC_MPP1 :
                                          BCMPC_TSC_REG_ACC_MPP0;
            lane = lane % 4;
        } else {
            /* Two-copy register */
            *pml_sel = (lane >> 2) & 0x1;
            mpp_sel = (lane >> 1) & 0x1 ? BCMPC_TSC_REG_ACC_MPP1 :
                                          BCMPC_TSC_REG_ACC_MPP0;
            lane = lane % 2;
        }
        aer = lane | (mpp_sel << 8);
    } else {
        if (devad) {
            aer = lane | (devad << 11) | (pll_index << 8);
        } else {
            /* TSC_Addr[25:24] */
            mpp_sel = (lane > 3) ? BCMPC_TSC_REG_ACC_MPP1 :
                                   BCMPC_TSC_REG_ACC_MPP0;
            aer = (lane % 4) | (mpp_sel << 8);
        }
    }
    *reg_addr |= (aer << 16);
}

int
bcmpc_phy_raw_reg_read(int unit, int pm_id, uint32_t reg_addr, uint32_t *val,
                       bcmpc_phy_acc_ovrr_t *ovrr)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_topo_t pm_info;
    uint32_t reg_addr_update = reg_addr;
    uint8_t pml_sel = 0;
    bcmpc_pport_t base_pport_adjust;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (phy_acc_validate(unit, &pm_info, ovrr));

    phy_raw_reg_addr_access_update(&pm_info, &reg_addr_update, ovrr, &pml_sel);

    if (pc_drv->pm_base_pport_adjust) {
        SHR_IF_ERR_EXIT
            (pc_drv->pm_base_pport_adjust(unit, pm_info.base_pport, pml_sel,
                                          &base_pport_adjust));
    } else {
        base_pport_adjust = pm_info.base_pport;
    }

    SHR_IF_ERR_EXIT
        (pc_drv->pm_phy_raw_read(unit, base_pport_adjust, reg_addr_update,
                                 val));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_phy_raw_reg_write(int unit, int pm_id, uint32_t reg_addr, uint32_t val,
                         bcmpc_phy_acc_ovrr_t *ovrr)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_topo_t pm_info;
    bcmpc_phy_info_t phy_info;
    uint32_t reg_addr_update = reg_addr;
    int i, num_copy;
    uint8_t pml_sel = 0;
    bcmpc_pport_t base_pport_adjust;
    uint16_t reg_addr_tmp;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_get(unit, pm_id, &pm_info));

    SHR_IF_ERR_EXIT
        (phy_acc_validate(unit, &pm_info, ovrr));

    SHR_IF_ERR_EXIT
        (bcmpc_phy_info_get(unit, pm_info.base_pport, &phy_info));

    if ( ovrr->lane_index != NO_LN_IDX_SPECIFIED ) {
        phy_raw_reg_addr_access_update(&pm_info, &reg_addr_update,
                                       ovrr, &pml_sel);
        if (pc_drv->pm_base_pport_adjust) {
            SHR_IF_ERR_EXIT
                (pc_drv->pm_base_pport_adjust(unit, pm_info.base_pport, pml_sel,
                                              &base_pport_adjust));
        } else {
            base_pport_adjust = pm_info.base_pport;
        }
        SHR_IF_ERR_EXIT
            (pc_drv->pm_phy_raw_write(unit, base_pport_adjust, reg_addr_update,
                                      val));
    } else {
        reg_addr_tmp = reg_addr & 0xffff;
        if ((pm_info.pm_info->type == PM_TYPE_PM8X100) &&
            BCMPC_TSCO_FOUR_COPY_REG(reg_addr_tmp)) {
            num_copy = 16;
        } else {
            num_copy = phy_info.num_lanes;
        }
        for (i = 0; i < num_copy; i++) {
            reg_addr_update = reg_addr;
            ovrr->lane_index = i;
            phy_raw_reg_addr_access_update(&pm_info, &reg_addr_update,
                                           ovrr, &pml_sel);
            if (pc_drv->pm_base_pport_adjust) {
                SHR_IF_ERR_EXIT
                    (pc_drv->pm_base_pport_adjust(unit, pm_info.base_pport,
                                                  pml_sel, &base_pport_adjust));
            } else {
                base_pport_adjust = pm_info.base_pport;
            }
            SHR_IF_ERR_EXIT
                (pc_drv->pm_phy_raw_write(unit, base_pport_adjust,
                                              reg_addr_update, val));
        }
    }

exit:
    SHR_FUNC_EXIT();
}
