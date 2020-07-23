/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT
#include <shared/bsl.h>

#include <shared/shrextend/shrextend_error.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/cmic.h> 
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/register.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#ifdef PHYMOD_TSCBH_SUPPORT
#include <libs/phymod/chip/tscbh/tier1/tbhmod.h>
#endif
#ifdef PHYMOD_BLACKHAWK_SUPPORT
#include <libs/phymod/chip/blackhawk/tier1/public/blackhawk_tsc_fields_public.h>
#include <libs/phymod/chip/blackhawk/tier1/blackhawk_tsc_field_access.h>
#endif

#ifdef PHYMOD_TSCBH_SUPPORT


shr_error_e
dnx_phy_nif_bh_power_up(
    int unit)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    int pm;
    int i;

    uint32 reg_val;
    int port, blk;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&params, 0, sizeof(params));
    params.lane = -1;
    params.apply_lane_mask = 0;
    params.sys_side = 1;
    params.lane = 0;

    if (dnx_data_nif.simulator.cdu_type_get(unit) != phymodDispatchTypeTscbh)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Heat Up is not supported for this device.\n");

    }
    
    for (pm = 0; pm < dnx_data_nif.eth.nof_cdu_pms_get(unit); pm++)
    {
        blk = dnx_drv_cdport_block(unit, pm);
        port = dnx_drv_soc_block_port(unit, blk);
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDPORT_XGXS0_CTRL_REGr, port, 0, &reg_val));

        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_CLK_SELf, 1);
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_PWRDWNf, 1);
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_RSTBf, 0);

        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDPORT_XGXS0_CTRL_REGr, port, 0, reg_val));
        sal_usleep(10);

        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_PWRDWNf, 0);
        soc_reg_field_set(unit, CDPORT_XGXS0_CTRL_REGr, &reg_val, TSC_RSTBf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDPORT_XGXS0_CTRL_REGr, port, 0, reg_val));

        sal_usleep(10);
    }

    for (pm = 0; pm < dnx_data_nif.eth.nof_cdu_pms_get(unit); pm++)
    {
        portmod_pm_phy_lane_access_get(unit, pm, &params, 1, &phy_access, &nof_phys, NULL);
        phy_access.type = phymodDispatchTypeTscbh;
        SHR_IF_ERR_EXIT(tbhmod_pmd_reset_seq(&phy_access.access, 0));

        phy_access.access.lane_mask = 1 << 0;

        for (i = 0; i < 2; i++)
        {
            phy_access.access.pll_idx = i;
            SHR_IF_ERR_EXIT(wrc_blackhawk_tsc_ams_pll_pwrdn(&phy_access.access, 0x0));
        }
        for (i = 0; i < 2; i++)
        {
            phy_access.access.pll_idx = i;
            SHR_IF_ERR_EXIT(wrc_blackhawk_tsc_core_dp_s_rstb(&phy_access.access, 0x1));
        }
    }
exit:
    SHR_FUNC_EXIT;

}
#endif

#ifdef PHYMOD_BLACKHAWK_SUPPORT

shr_error_e
dnx_phy_fabric_bh_power_up(
    int unit)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    int pm_id, fabric_indx;
    int i;

    soc_reg_above_64_val_t reg_above_64_val;
    uint32 reg_val;

    int nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&params, 0, sizeof(params));
    params.lane = -1;
    params.apply_lane_mask = 0;
    params.sys_side = 1;
    params.lane = 0;

    for (fabric_indx = 0; fabric_indx < nof_fabric_pms; fabric_indx++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_LN_RX_H_PWRDNf, 0);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_LN_TX_H_PWRDNf, 0);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_IDDQf, 0);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));
        sal_usleep(10);

        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_POR_H_RSTBf, 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_LN_RX_H_RSTBf, 0xff);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_LN_TX_H_RSTBf, 0xff);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, FSRD_SRD_PLL_0_CTRLr, fabric_indx, 0, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_PLL_0_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_0_DP_H_RSTBf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, FSRD_SRD_PLL_0_CTRLr, fabric_indx, 0, reg_val));

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, FSRD_SRD_PLL_1_CTRLr, fabric_indx, 0, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_PLL_1_CTRLr, &reg_val, SRD_N_PMD_CORE_PLL_1_DP_H_RSTBf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, FSRD_SRD_PLL_1_CTRLr, fabric_indx, 0, reg_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_LN_RX_DP_H_RSTBf, 0xFF);
        soc_reg_above_64_field32_set(unit, FSRD_SRD_CTRLr, reg_above_64_val, SRD_N_PMD_LN_TX_DP_H_RSTBf, 0xFF);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSRD_SRD_CTRLr, fabric_indx, 0, reg_above_64_val));
    }

    for (fabric_indx = 0; fabric_indx < nof_fabric_pms; fabric_indx++)
    {
        pm_id = fabric_indx + dnx_data_nif.eth.nof_cdu_pms_get(unit) + dnx_data_nif.eth.nof_clu_pms_get(unit);

        portmod_pm_phy_lane_access_get(unit, pm_id, &params, 1, &phy_access, &nof_phys, NULL);
        phy_access.type = phymodDispatchTypeBlackhawk;

        phy_access.access.lane_mask = 1 << 0;

        for (i = 0; i < 2; i++)
        {
            phy_access.access.pll_idx = i;
            SHR_IF_ERR_EXIT(wrc_blackhawk_tsc_ams_pll_pwrdn(&phy_access.access, 0x0));
        }

        for (i = 0; i < 2; i++)
        {
            phy_access.access.pll_idx = i;
            SHR_IF_ERR_EXIT(wrc_blackhawk_tsc_core_dp_s_rstb(&phy_access.access, 0x1));
        }

        
        
    }

exit:
    SHR_FUNC_EXIT;
}
#endif
