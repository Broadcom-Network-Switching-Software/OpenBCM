

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PVT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pvt.h>



extern shr_error_e jr2_a0_data_pvt_attach(
    int unit);
extern shr_error_e j2c_a0_data_pvt_attach(
    int unit);
extern shr_error_e q2a_a0_data_pvt_attach(
    int unit);
extern shr_error_e j2p_a0_data_pvt_attach(
    int unit);



static shr_error_e
dnx_data_pvt_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "PVT general attributes";
    
    submodule_data->nof_features = _dnx_data_pvt_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pvt general features");

    submodule_data->features[dnx_data_pvt_general_pvt_temp_monitors_hw_support].name = "pvt_temp_monitors_hw_support";
    submodule_data->features[dnx_data_pvt_general_pvt_temp_monitors_hw_support].doc = "Indicate if PVT temperature monitors exist";
    submodule_data->features[dnx_data_pvt_general_pvt_temp_monitors_hw_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pvt_general_pvt_peak_clear_support].name = "pvt_peak_clear_support";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_clear_support].doc = "Indicate if support PVT peak clear";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_clear_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pvt_general_pvt_peak_faulty].name = "pvt_peak_faulty";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_faulty].doc = "Indicate if PVT peak have faulty";
    submodule_data->features[dnx_data_pvt_general_pvt_peak_faulty].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pvt_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pvt general defines");

    submodule_data->defines[dnx_data_pvt_general_define_nof_pvt_monitors].name = "nof_pvt_monitors";
    submodule_data->defines[dnx_data_pvt_general_define_nof_pvt_monitors].doc = "Number of device PVT monitors";
    
    submodule_data->defines[dnx_data_pvt_general_define_nof_pvt_monitors].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_pvt_base].name = "pvt_base";
    submodule_data->defines[dnx_data_pvt_general_define_pvt_base].doc = "PVT base. Used for correct calculation of the monitor readings";
    
    submodule_data->defines[dnx_data_pvt_general_define_pvt_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_pvt_factor].name = "pvt_factor";
    submodule_data->defines[dnx_data_pvt_general_define_pvt_factor].doc = "PVT factor Used for correct calculation of the monitor readings";
    
    submodule_data->defines[dnx_data_pvt_general_define_pvt_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits].name = "pvt_mon_control_sel_nof_bits";
    submodule_data->defines[dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits].doc = "Number of bits in the field PVT_MON_CONTROL_SEL";
    
    submodule_data->defines[dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_thermal_data_nof_bits].name = "thermal_data_nof_bits";
    submodule_data->defines[dnx_data_pvt_general_define_thermal_data_nof_bits].doc = "Number of bits in the field THERMAL_DATA";
    
    submodule_data->defines[dnx_data_pvt_general_define_thermal_data_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_sel].name = "vol_sel";
    submodule_data->defines[dnx_data_pvt_general_define_vol_sel].doc = "select voltage value";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_sel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_factor].name = "vol_factor";
    submodule_data->defines[dnx_data_pvt_general_define_vol_factor].doc = "voltage calculation base value";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_factor].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_multiple].name = "vol_multiple";
    submodule_data->defines[dnx_data_pvt_general_define_vol_multiple].doc = "voltage calculation multiple value";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_multiple].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pvt_general_define_vol_divisor].name = "vol_divisor";
    submodule_data->defines[dnx_data_pvt_general_define_vol_divisor].doc = "voltage calculation divisor";
    
    submodule_data->defines[dnx_data_pvt_general_define_vol_divisor].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_pvt_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pvt general tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pvt_general_feature_get(
    int unit,
    dnx_data_pvt_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, feature);
}


uint32
dnx_data_pvt_general_nof_pvt_monitors_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_nof_pvt_monitors);
}

uint32
dnx_data_pvt_general_pvt_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_pvt_base);
}

uint32
dnx_data_pvt_general_pvt_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_pvt_factor);
}

uint32
dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits);
}

uint32
dnx_data_pvt_general_thermal_data_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_thermal_data_nof_bits);
}

uint32
dnx_data_pvt_general_vol_sel_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_sel);
}

uint32
dnx_data_pvt_general_vol_factor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_factor);
}

uint32
dnx_data_pvt_general_vol_multiple_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_multiple);
}

uint32
dnx_data_pvt_general_vol_divisor_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pvt, dnx_data_pvt_submodule_general, dnx_data_pvt_general_define_vol_divisor);
}







shr_error_e
dnx_data_pvt_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "pvt";
    module_data->nof_submodules = _dnx_data_pvt_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data pvt submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_pvt_general_init(unit, &module_data->submodules[dnx_data_pvt_submodule_general]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pvt_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pvt_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pvt_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

