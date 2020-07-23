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
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
#include <libs/phymod/chip/falcon16_v1l4p1/tier1/include/public/falcon16_v1l4p1_fields_public.h>
#include <libs/phymod/chip/falcon16_v1l4p1/tier1/include/falcon16_v1l4p1_field_access.h>
#endif

#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
int tefmod16_gen3_pmd_reset_seq(
    const phymod_access_t * pc,
    int pmd_touched);


shr_error_e
dnx_phy_nif_falcon_power_up(
    int unit)
{
    int clu_pm;
    int blk;
    int port;
    uint32 reg_val;
    int pm_id;

    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&params, 0, sizeof(params));
    params.lane = -1;
    params.apply_lane_mask = 0;
    params.sys_side = 1;
    params.lane = 0;

    if (dnx_data_nif.simulator.clu_type_get(unit) != phymodDispatchTypeTscf16_gen3)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Heat Up is not supported for this device.\n");

    }
    
    for (clu_pm = 0; clu_pm < dnx_data_nif.eth.nof_clu_pms_get(unit); clu_pm++)
    {
        blk = dnx_drv_clport_block(unit, clu_pm);
        port = dnx_drv_soc_block_port(unit, blk);
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, &reg_val));

        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 1);
        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 1);
        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 0);

        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, reg_val));
        sal_usleep(10);

        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 0);
        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 0);
        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, reg_val));

        sal_usleep(10);
    }

    for (clu_pm = 0; clu_pm < dnx_data_nif.eth.nof_clu_pms_get(unit); clu_pm++)
    {
        pm_id = dnx_data_nif.eth.nof_cdu_pms_get(unit) + clu_pm;
        portmod_pm_phy_lane_access_get(unit, pm_id, &params, 1, &phy_access, &nof_phys, NULL);
        phy_access.type = phymodDispatchTypeTscf16_gen3;

        SHR_IF_ERR_EXIT(tefmod16_gen3_pmd_reset_seq(&phy_access.access, 0));

        SHR_IF_ERR_EXIT(wrc_falcon16_v1l4p1_ams_pll_iqp(&phy_access.access, 0x5));

        SHR_IF_ERR_EXIT(wrc_falcon16_v1l4p1_ams_pll_pwrdn(&phy_access.access, 0));

        SHR_IF_ERR_EXIT(wrc_falcon16_v1l4p1_core_dp_s_rstb(&phy_access.access, 1));
    }
exit:
    SHR_FUNC_EXIT;
}
#endif
