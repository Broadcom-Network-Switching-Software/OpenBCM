/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC 
#include <shared/bsl.h>
#include <shared/utilex/utilex_u64.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/dnxf/ramon/ramon_fabric_system_upgrade.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

#define SOC_RAMON_SYSTEM_UPGRADE_PHASE1_FIRST_ITERATION     0x1
#define SOC_RAMON_SYSTEM_UPGRADE_PHASE1_REPEAT_ITERATION    0x2


shr_error_e
soc_ramon_system_upgrade_in_progress_get(
    int unit,
    int *in_progress)
{
    uint64 reg_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E4r(unit, &reg_64_val));
    *in_progress = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_20_20f);

exit:
    SHR_FUNC_EXIT;
}


static void
soc_ramon_system_upgrade_target_translate(
    int unit,
    uint32 target,
    uint32 *intg,
    uint32 *frac)
{
    uint32 device_core_clock_khz;
    UTILEX_U64 frac_0, frac_x, frac_x2;

    device_core_clock_khz = dnxf_data_device.general.core_clock_khz_get(unit);
    *intg = target / device_core_clock_khz;
    utilex_u64_clear(&frac_0);
    utilex_u64_clear(&frac_x);
    utilex_u64_multiply_longs(target, (1<<19), &frac_x);
    utilex_u64_devide_u64_long(&frac_x, device_core_clock_khz, &frac_0);
    if (device_core_clock_khz <= target)
    {
        utilex_u64_multiply_longs(*intg, (1<<19), &frac_x2);
        utilex_u64_subtract_u64(&frac_0, &frac_x2);
    }
    *frac = frac_0.arr[0];
}


shr_error_e
soc_ramon_system_upgrade_enable_set(
    int unit,
    int enable)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_110_110f, (enable!=0));
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_system_upgrade_phase1_init(
    int unit,
    int master_id)
{
    uint32 intg, frac;
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01E2r, reg_above_64_val, FIELD_0_31f,
                                dnxf_data_fabric.system_upgrade.param_1_get(unit));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01E2r, reg_above_64_val, FIELD_48_63f,
                                dnxf_data_fabric.system_upgrade.param_2_get(unit));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01E2r, reg_above_64_val, FIELD_32_47f,
                                dnxf_data_fabric.system_upgrade.param_7_get(unit));
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_78_109f,
                                dnxf_data_fabric.system_upgrade.param_0_get(unit));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_46_77f,
                                dnxf_data_fabric.system_upgrade.param_3_get(unit));
    soc_ramon_system_upgrade_target_translate(unit, dnxf_data_fabric.system_upgrade.param_4_get(unit), &intg, &frac);
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_23_41f,
                                frac);
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_42_45f,
                                intg);
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_111_121f,
                                master_id);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_system_upgrade_target_set(
    int unit,
    uint32 target)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 intg, frac;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));
    soc_ramon_system_upgrade_target_translate(unit, target, &intg, &frac);
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_0_18f,
                                frac);
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_19_22f,
                                intg);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_system_upgrade_master_id_get(
    int unit,
    int *master_id)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));
    *master_id = soc_reg_above_64_field32_get(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_111_121f);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_system_upgrade_enable_get(
    int unit,
    int *enable)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));
    *enable = soc_reg_above_64_field32_get(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_110_110f);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_system_upgrade_trig(
    int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01E2r, reg_above_64_val, FIELD_64_64f, 0);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));
    soc_reg_above_64_field32_set(unit, MESH_TOPOLOGY_REG_01E2r, reg_above_64_val, FIELD_64_64f, 1);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));

    sal_usleep(10000);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_system_upgrade_trig_get(
    int unit,
    int *is_triggered)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E2r(unit, reg_above_64_val));
    *is_triggered = soc_reg_above_64_field32_get(unit, MESH_TOPOLOGY_REG_01E2r, reg_above_64_val, FIELD_64_64f);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_system_upgrade_done_get(
    int unit,
    int *done)
{
    uint32 state_5, state_6;
    uint64 reg_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E4r(unit, &reg_64_val));
    state_5 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_20_20f);
    state_6 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_21_21f);

    *done = !state_5 && state_6;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_system_upgrade_ok_get(
    int unit,
    int *ok)
{
    uint64 reg_64_val;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 state_7, state_8;
    uint32 param_8, param_9;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E4r(unit, &reg_64_val));
    state_7 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_22_40f);
    state_8 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_41_44f);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01DEr(unit, reg_above_64_val));
    param_8 = soc_reg_above_64_field32_get(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_0_18f);
    param_9 = soc_reg_above_64_field32_get(unit, MESH_TOPOLOGY_REG_01DEr, reg_above_64_val, FIELD_19_22f);

    *ok = (state_7 == param_8) && (state_8 == param_9);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_system_upgrade_apply(
    int unit)
{
    uint64 reg_64_val;
    uint32 reg_val;
    uint32 state_7, state_8;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_01E4r(unit, &reg_64_val));
    state_7 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_22_40f);
    state_8 = soc_reg64_field32_get(unit, MESH_TOPOLOGY_REG_01E4r, reg_64_val, FIELD_41_44f);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r(unit, &reg_val));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r, &reg_val, REG_107_CONFIG_1f, state_7);
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r, &reg_val, REG_107_CONFIG_2f, state_8);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

