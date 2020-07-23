/** \file init_cb_wrappers.c
 * DNX init sequence CB wrapper functions.
 *
 * The DNX init sequence uses CB functions for init and deinit steps. new functions are written according the required
 * definitions, old ones however are wrapped and placed in this file.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX
/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#define ERPP_MAPPING_INLIF_PROFILE_SIZE 256
#define ERPP_MAPPING_OUTLIF_PROFILE_SIZE 16
#define ERPP_FILTER_PER_FWD_CONTEXT_SIZE 64
#define ERPP_FILTER_PER_PORT_SIZE  256
#define FWD_CONTEXT_ETHERNET 6

#include "init_cb_wrappers.h"

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/pemladrv/pemladrv.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/common/family.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_hard_reset_access.h>
#include <appl/diag/system.h>
#include <sal/core/boot.h>
#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/sand/sand_aux_access.h>

#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/sand/sand_mbist.h>
#include <soc/dnxc/dnxc_verify.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/access/adapter_access.h>


#define FIELD_0_5_DEFAULT 61
#define FIELD_6_6_DEFAULT 1
#define FIELD_7_11_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_MPLS
#define FIELD_12_16_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_IPV4
#define FIELD_7_16_DEFAULT (FIELD_7_11_DEFAULT) | (FIELD_12_16_DEFAULT << 5)
#define FIELD_17_24_DEFAULT 0xFF
#define FIELD_25_27_DEFAULT 6
#define FIELD_28_49_DEFAULT 0
#define FIELD_50_51_DEFAULT 2
#define FIELD_52_69_DEFAULT 0
#define FIELD_70_72_DEFAULT 4
#define FIELD_73_80_DEFAULT 1
#define FIELD_81_82_DEFAULT 3
#define FIELD_83_85_DEFAULT 0
#define FIELD_86_88_DEFAULT 4
#define FIELD_89_92_DEFAULT 7
#define FIELD_93_94_DEFAULT 2
#define FIELD_95_97_DEFAULT 0
#define FIELD_98_99_DEFAULT 3
#define FIELD_100_102_DEFAULT 0
#define IPPF_REG_03A0_FIELD_17_17_DEFAULT 0
#define VTT_ST_TWO_LE_SHARED_PDS_STAGE_SELECTORS_DEFAULT 2
#define ITPP_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe
#define ITPPD_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe
/*
 * }
 */

/*
 * Enums
 * {
 */
/*
 * }
 */

shr_error_e
dnx_init_done_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(adapter_dnx_init_done(unit));
#endif
    /*
     * Notify DNX DATA -  init done
     */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

    /*
     * Notify DBAL -  init done
     */
    SHR_IF_ERR_EXIT(dbal_device_init_done(unit));
    {
        /*
         * Mark soc control as done
         */
        uint32 *soc_flags_p;

        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, SOC_FLAGS, (void **) &soc_flags_p));
        *soc_flags_p |= dnx_drv_soc_f_inited();
        *soc_flags_p |= dnx_drv_soc_f_all_modules_inited();
    }

    /*
     * Enable verification after init is done.
     */
    dnxc_verify_allowed_set(unit, 1);

    /*
     * init / deinit thread is cleared. No further validation of HA access.
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tid_clear(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_done_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * set init / deinit access validation thread
     */
    SHR_IF_ERR_EXIT(dnxc_ha_tid_set(unit));

    /*
     * Update DNXC DATA module about the state.
     */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_reset(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

    /*
     * declare that driver is detaching
     */
    dnx_drv_soc_detach(unit, 1);
    {
        /*
         * Mark soc control as not done
         */
        uint32 *soc_flags_p;

        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, SOC_FLAGS, (void **) &soc_flags_p));
        *soc_flags_p &= ~dnx_drv_soc_f_inited();
        *soc_flags_p &= ~dnx_drv_soc_f_all_modules_inited();
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_info_config_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_drv_soc_dnx_info_config(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_info_config_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NONE, "place holder for actual deinit code\n%s%s%s", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_feature_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** disable schan error checks for pcid */
#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        dnx_drv_soc_feature_clear(unit, soc_feature_schan_err_check);
    }
#endif /* PLISIM */

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_family_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_chip_family_set(unit, BCM_FAMILY_DNX));

exit:
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE
shr_error_e
dnx_init_adapter_reg_access_init(
    int unit)
{
    sal_thread_t *mem_scan_pid_p;
    sal_usecs_t **mem_scan_interval_p;
    int sub_unit_id;
    SHR_FUNC_INIT_VARS(unit);

    /** lock the adapter init step - because socket connection use the same address for all units and can't do it in parallel */
    if (dnx_adapter_init_lock != NULL)
    {
        /** take lock */
        if (0 != sal_mutex_take(dnx_adapter_init_lock, 2000000))
        {
            SHR_ERR_EXIT(BCM_E_TIMEOUT, "unable to take dnx_adapter_init_lock mutex");
        }
    }

    adapter_context_db.init(unit);
    adapter_context_db.params.alloc(unit);
    for (sub_unit_id = 0; sub_unit_id < dnx_data_device.general.nof_sub_units_get(unit); sub_unit_id++)
    {
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, MEM_SCAN_PID, (void **) &mem_scan_pid_p));
        SHR_IF_ERR_EXIT(soc_control_element_address_get(unit, MEM_SCAN_INTERVAL, (void **) &mem_scan_interval_p));
        /*
         * do not enable memscan task, otherwise tr 7 will segmentation fail
         */
        *mem_scan_pid_p = SAL_THREAD_ERROR;
        *mem_scan_interval_p = 0;

        SHR_IF_ERR_EXIT(adapter_reg_access_init(unit, sub_unit_id));
    }
exit:
    if (dnx_adapter_init_lock != NULL)
    {
        if (0 != sal_mutex_give(dnx_adapter_init_lock))
        {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "unable to give dnx_adapter_init_lock mutex \n")));
        }
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_adapter_reg_access_deinit(
    int unit)
{
    int sub_unit_id;
    SHR_FUNC_INIT_VARS(unit);
    for (sub_unit_id = 0; sub_unit_id < dnx_data_device.general.nof_sub_units_get(unit); sub_unit_id++)
    {
        SHR_IF_ERR_EXIT(adapter_reg_access_deinit(unit, sub_unit_id));
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dnx_init_hard_reset_init(
    int unit)
{
    uint8 is_init;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_drv_soc_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_hard_reset_db.is_init(unit, &is_init));
        if (is_init == FALSE)
        {
            /** init hard reset swstate database */
            SHR_IF_ERR_EXIT(dnx_hard_reset_db.init(unit));
        }

        SHR_IF_ERR_EXIT(dnx_drv_soc_dnx_hard_reset(unit));
    }

    SHR_IF_ERR_EXIT(sand_init_cpu2tap(unit));   /* init CPU2TAP software mutexes */

exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief
 * reads core clock from hw and verify that the SOC PROPERTY value is
 * compatible with it
 * 1. read enable bit (ECI_RESERVED_21 FIELD_0_0) and verify that it's disabled.
 * 2. read ref clock from dnxdata (need to be set to 25Mhz)
 * 3. read N & M from  ECI_POWERUP_CONFIG
 * 4. core clock = ref clock * N / M
 */
shr_error_e
dnx_init_core_clock_verify(
    int unit)
{
    uint32 cpu_override;
    uint32 ndiv;
    uint32 mdiv;
    uint32 entry_handle_id;
    uint32 ideal_core_clock_value_khz = 0;
    uint32 core_clock_soc_property_khz = 0;
    uint32 ref_core_clock_mhz = 0;
    uint32 deviation = 1;
    uint32 min_permitted_range = 100 - deviation;
    uint32 max_permitted_range = 100 + deviation;
    uint32 core_clock_ratio = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * skip the emulation boot
     */
    if (!(SAL_BOOT_PLISIM || soc_sand_is_emulation_system(unit)))
    {

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_CORE_CLOCK_KHZ, &entry_handle_id));
        /** Setting pointers value to receive the fields */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NDIV, INST_SINGLE, &ndiv);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MDIV, INST_SINGLE, &mdiv);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CPU_OVERRIDE, INST_SINGLE, &cpu_override);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * get the soc property value from dnx data
         */
        core_clock_soc_property_khz = dnx_data_device.general.core_clock_khz_get(unit);
        ref_core_clock_mhz = dnx_data_device.general.ref_core_clock_mhz_get(unit);

        /*
         * cpu override is not supported 
         */
        if (!cpu_override)
        {
            if (mdiv == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "  mdiv equal zero ");
            }
            ideal_core_clock_value_khz = 1000 * ndiv / mdiv * ref_core_clock_mhz;
            core_clock_ratio = 100 * core_clock_soc_property_khz / ideal_core_clock_value_khz;
            if (core_clock_ratio < min_permitted_range || core_clock_ratio > max_permitted_range)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             " user has set core clock to be %d. core clock should be in the range of %d percentage from %d , ndiv is %d and mdiv is %d",
                             core_clock_soc_property_khz, deviation, ideal_core_clock_value_khz, ndiv, mdiv);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_dbal_mdb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_access_method_init(unit, DBAL_ACCESS_METHOD_MDB));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_pemla_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_access_method_init(unit, DBAL_ACCESS_METHOD_PEMLA));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_sw_access_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_access_method_init(unit, DBAL_ACCESS_METHOD_SW_STATE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_mdb_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_access_method_deinit(unit, DBAL_ACCESS_METHOD_MDB));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_pemla_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_access_method_deinit(unit, DBAL_ACCESS_METHOD_PEMLA));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_sw_access_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_access_method_deinit(unit, DBAL_ACCESS_METHOD_SW_STATE));

exit:
    SHR_FUNC_EXIT;
}
