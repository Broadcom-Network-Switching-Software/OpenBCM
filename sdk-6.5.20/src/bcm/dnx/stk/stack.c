/** \file stk/stack.c
 *
 * Stacking procedures for DNX.
 *
 * Implementation of BCM module stk APIs (bcm_dnx_stk_xxx()).
 * Relevant logical modules:
 * - Stacking
 * - Switch System
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STK
/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/stack.h>
#include <bcm/fabric.h>

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/stk/stk_domain.h>
#include <bcm_int/dnx/stk/stk_trunk.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stack.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_stk_sys_access.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>

/*
 * }
 */

/**
 * \brief 
 * Getting the system port from logical port.
 * Warning: Logical port might be mapped to more then one system port.
 *          The function will return just one system port.
 *          Therefore, this function should be use only for debug and testing.
 * 
 * \param [in] unit - Unit # 
 * \param [in] gport - logical_port. 
 * \param [out] sysport - system port gport 
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_gport_sysport_get(
    int unit,
    bcm_gport_t gport,
    bcm_gport_t * sysport)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_NULL_CHECK(sysport, _SHR_E_PARAM, "sysport");

    /*
     * Get system port from gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
    /*
     * Getting system port from modport is not supported.
     * Make sure that indeed gport management managed to retrieve the system port
     */
    if (gport_info.sys_port == -1)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "API is not supported for non-local ports. Getting port attributes should use system port explicitly.\n");
    }

    /*
     * Print a warning (API should be used for debug and testing only)
     */
    LOG_WARN(BSL_LOG_MODULE,
             (BSL_META_U(unit, "warning: gport might be mapped to several system ports - returning one of them.\n")));

    /** Create the required gport */
    BCM_GPORT_SYSTEM_PORT_ID_SET(*sysport, gport_info.sys_port);

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief - Verify procedure for bcm_dnx_stk_module_enable API
 */
static int
dnx_stk_module_enable_verify(
    int unit,
    bcm_module_t modid,
    int nports,
    int enable)
{
    bcm_switch_dram_power_down_callback_t callback;
    bcm_port_t port;
    bcm_pbmp_t port_bm;
    int nof_fap_ids;
    int module_enable_done;
    bcm_core_t core;
    uint32 tm_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify fap ID(s) are defined */
    SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.get(unit, &nof_fap_ids));

    if (nof_fap_ids == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No modid define. First, define modid(s) by calling bcm_stk_modid_config_add\n");
    }

     /** check if ptr for dram shut down exist. If not, return error before enable traffic */
    if (enable == TRUE)
    {
        uint32 dram_bitmap[1];
        /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
        dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
        /** power down callback is required only in architecture with HBM */
        if ((TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported)) && (dram_bitmap[0] != 0))
        {
            SHR_IF_ERR_EXIT(dnx_dram_db.power_down_callback.callback.get(unit, &callback));
            if (callback == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "DRAM Power down callback function does not exist. cannot enable traffic \n");
            }
        }

        SHR_IF_ERR_EXIT(dnx_stk_sys_db.module_enable_done.get(unit, &module_enable_done));
        if (!module_enable_done)
        {
            /** first time module_enable is called */
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM, 0,
                                                       &port_bm));

            /** first time after init need to make sure tm_port < 256 * nof_fap_ids */
            BCM_PBMP_ITER(port_bm, port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_allowed_verify(unit, core, tm_port, nof_fap_ids));
            }
            SHR_IF_ERR_EXIT(dnx_stk_sys_db.module_enable_done.set(unit, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Enable \ Disable traffic. 
 * This function should be used to configure modid (the function will throw an error in a case device modid != 'modid)  
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - Unused
 * \param [in] nports - ignored 
 * \param [in] enable - Enable or Disable traffic 
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_module_enable(
    int unit,
    bcm_module_t modid,
    int nports,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_module_enable_verify(unit, modid, nports, enable));

    /** Enable/disable Traffic */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_set(unit, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Get device modid (FAPID of core 0). 
 * core 1 is configure to 'modid' + 1 
 * 
 * \param [in] unit - Unit # 
 * \param [out] modid - core 0 modid.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_get(
    int unit,
    int *modid)
{
    bcm_stk_modid_config_t config;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Verify
     */
    SHR_NULL_CHECK(modid, _SHR_E_PARAM, "modid");

    /** redirect*/
    bcm_stk_modid_config_t_init(&config);
    SHR_IF_ERR_EXIT(bcm_dnx_stk_modid_config_get(unit, &config));
    *modid = config.modid;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Set device modid (FAPID of core 0) and enable control cells.
 * core 1 is configure to 'modid' + 1. 
 * Note: It is recommnded to set device modid and enable control cells in seperate steps: 
 *  1. Set modid - using 'bcm_stk_modid_config_set()'
 *  2. Run user application
 *  3. Enable control cells - using 'bcm_fabric_control_set(unit, bcmFabricControlCellsEnable, 1);
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - core 0 modid.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_set(
    int unit,
    int modid)
{
    bcm_stk_modid_config_t config;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Set modid */
    bcm_stk_modid_config_t_init(&config);
    config.modid = modid;
    SHR_IF_ERR_EXIT(bcm_dnx_stk_modid_config_set(unit, &config));

    /** Enable Control Cells */
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_set(unit, bcmFabricControlCellsEnable, 1));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief 
 * * See 'bcm_dnx_stk_modid_get();
 */
int
bcm_dnx_stk_my_modid_get(
    int unit,
    int *my_modid)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** redirect */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_modid_get(unit, my_modid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * See 'bcm_dnx_stk_modid_set();
 */
int
bcm_dnx_stk_my_modid_set(
    int unit,
    int modid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** redirect */
    SHR_IF_ERR_EXIT(bcm_dnx_stk_modid_set(unit, modid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API bcm_dnx_stk_modid_config_set().
 *          For details about API parameters refer to bcm_dnx_stk_modid_config_set() descrption.
 */
static int
dnx_stk_modid_nof_verify(
    int unit,
    int max_nof_fap_ids)
{
    int nof_fap_ids;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify current number of fap IDs */
    SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.get(unit, &nof_fap_ids));
    if (nof_fap_ids > max_nof_fap_ids)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "This API is available only when up to %d FAP IDs are defined\n", max_nof_fap_ids);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Verify parameters of bcm_dnx_stk_modid_config_get_all
 */
static int
dnx_stk_modid_config_get_all_verify(
    int unit,
    int modid_max,
    bcm_stk_modid_config_t * modid_array,
    int *modid_count)
{
    int nof_fap_ids_per_core, actual_nof_modid;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_NULL_CHECK(modid_array, _SHR_E_PARAM, "modid_array");
    SHR_NULL_CHECK(modid_count, _SHR_E_PARAM, "modid_count");

    SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.get(unit, &nof_fap_ids_per_core));
    actual_nof_modid = dnx_data_device.general.nof_cores_get(unit) * nof_fap_ids_per_core;

    if (modid_max < actual_nof_modid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "modid_max (%d) is too small to contain all %d modid(s)\n",
                     modid_max, actual_nof_modid);
    }
    if (actual_nof_modid == 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Can not get modid before adding modid\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Get all device modid(s)
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid_max - maximal number of returned modid(s)
 * \param [out] modid_array - array of modid(s) 
 * \param [out] modid_count - number of returned modid(s)
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_config_get_all(
    int unit,
    int modid_max,
    bcm_stk_modid_config_t * modid_array,
    int *modid_count)
{
    int nof_fap_ids_per_core;
    bcm_core_t core;
    uint32 *modid = NULL;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_config_get_all_verify(unit, modid_max, modid_array, modid_count));

    *modid_count = 0;

    modid = sal_alloc(sizeof(uint32) * dnx_data_device.general.max_nof_fap_ids_per_core_get(unit), "modid");

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get_all(unit, core, modid, &nof_fap_ids_per_core));

        for (i = 0; i < nof_fap_ids_per_core; i++)
        {
            modid_array[*modid_count].modid = modid[i];
            (*modid_count)++;
        }
    }

exit:
    SHR_FREE(modid);
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Get device modid (FAPID of core 0). 
 * core 1 is configure to 'modid' + 1.
 * 
 * \param [in] unit - Unit # 
 * \param [out] modid - (field modid) core 0 modid.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_config_get(
    int unit,
    bcm_stk_modid_config_t * modid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_NULL_CHECK(modid, _SHR_E_PARAM, "modid");

    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_nof_verify(unit, 1));

    /** Get modid for Core 0, local index 0 */
    SHR_IF_ERR_EXIT(dnx_stk_sys_modid_get(unit, 0, 0, (uint32 *) &(modid->modid)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API bcm_dnx_stk_modid_config_set().
 *          For details about API parameters refer to bcm_dnx_stk_modid_config_set() descrption.
 */
static int
dnx_stk_modid_config_add_verify(
    int unit,
    bcm_stk_modid_config_t * modid,
    int fap_id_index)
{
    int device_enabled;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(modid, _SHR_E_PARAM, "modid");

    /** modid verify */
    if ((modid->modid < 0) || (modid->modid >= dnx_data_device.general.nof_faps_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "modid is invalid %d\n", modid->modid);
    }

    if (fap_id_index >= dnx_data_device.general.max_nof_fap_ids_per_core_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Cannot add more than %d fap IDs\n",
                     dnx_data_device.general.max_nof_fap_ids_per_core_get(unit));
    }

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /** device isolated verify */
        SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &device_enabled));
        if (device_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Setting device id is not allowed after control cells enabled operation");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Add device modid (FAP ID).
 * The APIs adds 'modid' on core 0 and 'modid' + 1 on core 1.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - (field modid) modid to be configured.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_config_add(
    int unit,
    bcm_stk_modid_config_t * modid)
{
    bcm_core_t core;
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);
    int fap_id_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.get(unit, &fap_id_index));

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_config_add_verify(unit, modid, fap_id_index));

    /** Set sequential modids for each core */
    for (core = 0; core < nof_cores; core++)
    {
        /** set my_modid to HW */
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_add(unit, core, fap_id_index, modid->modid + core));

        if (fap_id_index == 0)
        {
            /** set fmq dedicated system port for first modid only*/
            SHR_IF_ERR_EXIT(dnx_stk_sys_fmq_sysport_set(unit, core, modid->modid + core));
        }

        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            /** In MESH mode need to set the modids also to fabric HW */
            if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_mesh_device_id_local_set(unit, core, fap_id_index, modid->modid + core));
            }
        }
    }

    /** increment number of FAP ID(s) */
    SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.inc(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Set device modid (FAP ID).
 * The APIs configure core 0 modid to 'modid' and core 1 modid to 'modid' + 1.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid - (field modid) modid to be configured.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_config_set(
    int unit,
    bcm_stk_modid_config_t * modid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify no modid is added yet */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_nof_verify(unit, 0));

    SHR_IF_ERR_EXIT(bcm_dnx_stk_modid_config_add(unit, modid));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Get number of local modids (number of cores).
 * 
 * \param [in] unit - Unit # 
 * \param [out] num_modid - number of local modids.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_count(
    int unit,
    int *num_modid)
{
    int nof_fap_ids_per_core;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_NULL_CHECK(num_modid, _SHR_E_PARAM, "num_modid");

    SHR_IF_ERR_EXIT(dnx_stk_sys_db.nof_fap_ids.get(unit, &nof_fap_ids_per_core));

    /** Get number of cores */
    *num_modid = dnx_data_device.general.nof_cores_get(unit) * nof_fap_ids_per_core;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function of bcm_dnx_stk_sysport_gport_set()
 */
static int
dnx_stk_sysport_gport_set_verify(
    int unit,
    bcm_gport_t sysport,
    bcm_port_t gport)
{
    uint32 sysport_id;
    int modid, ftmh_pp_dsp;
    bcm_port_t logical_port;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * system port verify
     */
    if (!BCM_GPORT_IS_SYSTEM_PORT(sysport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "sysport must be gport of type 'system port'");
    }
    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
    if (sysport_id > dnx_data_device.general.invalid_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port (%u) is out of range, range is 0 - %d", sysport_id,
                     dnx_data_device.general.invalid_system_port_get(unit));
    }
    if (sysport_id == dnx_data_device.general.invalid_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port (%u) is reserved as invalid", sysport_id);
    }
    if (sysport_id == dnx_data_device.general.fmq_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port (%u) is reserved for FMQs", sysport_id);
    }

    /*
     * gport verify
     */
    if (BCM_GPORT_IS_MODPORT(gport))
    {
        ftmh_pp_dsp = BCM_GPORT_MODPORT_PORT_GET(gport);
        SHR_RANGE_VERIFY(ftmh_pp_dsp, 0, dnx_data_port.general.nof_tm_ports_per_fap_id_get(unit) - 1, _SHR_E_PARAM,
                         "ftmh_pp_dsp %d in modport is out of range\n", ftmh_pp_dsp);

        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                         "modid %d in modport is out of range\n", modid);
    }
    else if (BCM_GPORT_IS_LOCAL(gport))
    {
        logical_port = BCM_GPORT_LOCAL_GET(gport);
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));

    }
    else if (!BCM_GPORT_IS_SET(gport))
    {
        if (gport != -1)
        {
            /** assume it logical_port */
            logical_port = gport;
            SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
        }
    }
    else if (BCM_COSQ_GPORT_IS_FMQ_CLASS(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FMQ CLASS gport is not supported, instead they are handled internally");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport type");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief 
 * Map system port to module port.
 * This API configures mapping relevant just for local ports and mapping relevant for remote ports. 
 * This API configures both TM and PP attributes. 
 * 
 * \param [in] unit - Unit # 
 * \param [in] sysport - system port to be created by 'BCM_GPORT_SYSTEM_PORT_ID_SET' 
 * \param [in] gport - module port to be created by 'BCM_GPORT_MODPORT_SET' or local port or -1 to remove mapping.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_sysport_gport_set(
    int unit,
    bcm_gport_t sysport,
    bcm_port_t gport)
{
    uint32 sysport_id, prev_sysport_id;
    bcm_port_t logical_port;
    bcm_core_t core;
    int modid;
    int tm_port;
    int ftmh_pp_dsp;
    int is_local;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_sysport_gport_set_verify(unit, sysport, gport));

    /*
     * Parse gports
     */
    /*
     * Extract system port
     */
    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);

    /*
     * Extract modid + FTMH PP DSP from gport 
     */
    if (BCM_GPORT_IS_MODPORT(gport))
    {
        ftmh_pp_dsp = BCM_GPORT_MODPORT_PORT_GET(gport);
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    }
    else if (gport == -1)
    {
        ftmh_pp_dsp = dnx_data_port.general.nof_tm_ports_per_fap_id_get(unit) - 1;
        modid = dnx_data_device.general.invalid_fap_id_get(unit);
    }
    else
    {
        /** local port gport ot just a port */
        if (BCM_GPORT_IS_LOCAL(gport))
        {
            logical_port = BCM_GPORT_LOCAL_GET(gport);
        }
        else
        {
            logical_port = gport;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, logical_port, &core, (uint32 *) &tm_port));
        SHR_IF_ERR_EXIT(dnx_stk_sys_tm_port_to_modport_convert(unit, core, tm_port, &modid, &ftmh_pp_dsp));
    }

    /*
     * Map sysport to modport
     */
    SHR_IF_ERR_EXIT(dnx_stk_sys_sysport_modport_map_set(unit, _SHR_CORE_ALL, sysport_id, modid, ftmh_pp_dsp));

    /*
     * Delete existing port - sys_port mapping - if exist
     */
    /** skip on emulation to save time */
    if (!soc_sand_is_emulation_system(unit))
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_sysport_map_clear(unit, sysport_id));
    }

    /*
     * Set port - sys_port mapping - if not already exist
     */
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, modid, &is_local));
        if (is_local) /** local modid */
        {
            /*
             * map only the first sysport that mapped to destination, i.e. the master system port 
             */
            SHR_IF_ERR_EXIT(dnx_stk_sys_modport_to_tm_port_convert(unit, modid, ftmh_pp_dsp, &core, &tm_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
            SHR_IF_ERR_EXIT(dnx_stk_sys_logical_port_to_sysport_map_get(unit, logical_port, &prev_sysport_id));
            if (prev_sysport_id == dnx_data_device.general.invalid_system_port_get(unit))
            {
                SHR_IF_ERR_EXIT(dnx_stk_sys_logical_port_to_sysport_map_set(unit, logical_port, sysport_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function of bcm_dnx_stk_sysport_gport_get()
 */
static int
dnx_stk_sysport_gport_get_verify(
    int unit,
    bcm_gport_t sysport,
    bcm_port_t * gport)
{
    uint32 sysport_id;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * system port verify
     */
    if (!BCM_GPORT_IS_SYSTEM_PORT(sysport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "sysport must be gport of type 'system port'");
    }
    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
    if (sysport_id > dnx_data_device.general.max_nof_system_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "system port (%u) is out of range, range is 0 - %d", sysport_id,
                     dnx_data_device.general.invalid_system_port_get(unit));
    }

    /** verify gport */
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief 
 * Get module port given system port.
 * 
 * \param [in] unit - Unit # 
 * \param [in] sysport - system port to be created by 'BCM_GPORT_SYSTEM_PORT_ID_SET' 
 * \param [out] gport - module port to be created by 'BCM_GPORT_MODPORT_SET'.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_sysport_gport_get(
    int unit,
    bcm_gport_t sysport,
    bcm_gport_t * gport)
{

    uint32 modid;
    uint32 ftmh_pp_dsp;
    uint32 sysport_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_sysport_gport_get_verify(unit, sysport, gport));

    sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
    SHR_IF_ERR_EXIT(dnx_stk_sys_sysport_modport_map_get(unit, sysport_id, &modid, &ftmh_pp_dsp));

    /** Create modport gport */
    BCM_GPORT_MODPORT_SET(*gport, modid, ftmh_pp_dsp);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API bcm_dnx_stk_modid_domain_add/delete().
 *          For details about API parameters refer to bcm_dnx_stk_modid_domain_add/delete() description.
 */
static int
dnx_stk_modid_domain_verify(
    int unit,
    int modid,
    int tm_domain)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify modid */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_modid_verify(unit, modid));

    /** Verify tm_domain */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_tm_domain_verify(unit, tm_domain));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Map module id to domain id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid -  modle id
 * \param [in] tm_domain - tm domain id
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_domain_add(
    int unit,
    int modid,
    int tm_domain)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_domain_verify(unit, modid, tm_domain));

    /*
     * Set  module id to tm domain SW table 
     */
    SHR_IF_ERR_EXIT(dnx_stk_domain_modid_to_domain_set(unit, modid, tm_domain, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Unmap module id to domain id.
 * 
 * \param [in] unit - Unit # 
 * \param [in] modid -  modle id
 * \param [in] tm_domain - tm domain id
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_domain_delete(
    int unit,
    int modid,
    int tm_domain)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_domain_verify(unit, modid, tm_domain));

    /*
     * Clear  module id to tm domain SW table 
     */
    SHR_IF_ERR_EXIT(dnx_stk_domain_modid_to_domain_set(unit, modid, tm_domain, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API bcm_dnx_stk_modid_domain_get().
 *          For details about API parameters refer to bcm_dnx_stk_modid_domain_get() descrption.
 */
static int
dnx_stk_modid_domain_get_verify(
    int unit,
    int tm_domain,
    int mod_max,
    int *mod_array,
    int *mod_count)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm_domain */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_tm_domain_verify(unit, tm_domain));

    /*
     * Input parameters verify. 
     */
    SHR_NULL_CHECK(mod_array, _SHR_E_PARAM, "mod_array");
    SHR_NULL_CHECK(mod_count, _SHR_E_PARAM, "mod_count");
    if (mod_max <= 0)
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "mod_max is invalid");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Get array of module ids given a tm domain.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domain id
 * \param [in] mod_max - max amount of modules in supplied array.
 * \param [out] mod_array - returned found modules.
 * \param [out] mod_count - amount of modules in returned array.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_modid_domain_get(
    int unit,
    int tm_domain,
    int mod_max,
    int *mod_array,
    int *mod_count)
{
    uint32 modid = 0;
    uint32 modid_count = 0;
    uint32 is_exist = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_domain_get_verify(unit, tm_domain, mod_max, mod_array, mod_count));

    for (modid = 0; modid < dnx_data_device.general.nof_faps_get(unit); modid++)
    {
        SHR_IF_ERR_EXIT(dnx_stk_domain_modid_to_domain_get(unit, modid, tm_domain, &is_exist));
        if (is_exist == 0x1)
        {
            mod_array[modid_count++] = modid;
        }
        is_exist = 0;

        if (modid_count == mod_max)
        {
            break;
        }
    }

    *mod_count = modid_count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify for API bcm_dnx_stk_domain_stk_trunk_add/delete().
 *          For details about API parameters refer to bcm_dnx_stk_domain_stk_trunk_add/delete() description.
 */
static int
dnx_stk_domain_stk_trunk_verify(
    int unit,
    int tm_domain,
    bcm_trunk_t stk_trunk)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify tm_domain */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_tm_domain_verify(unit, tm_domain));

    /** Verify stk_trunk */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_trunk_trunk_id_verify(unit, stk_trunk));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Define which TM-domain can be reachable by a stacking trunk.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domian id
 * \param [in] stk_trunk - stacking trunk id to be created by Macro BCM_TRUNK_STACKING_TID_SET.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_domain_stk_trunk_add(
    int unit,
    int tm_domain,
    bcm_trunk_t stk_trunk)
{
    uint32 stk_tid = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_stk_trunk_verify(unit, tm_domain, stk_trunk));

    /*
     * Set  Domain to Stack trunk SW table
     */
    stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk);
    SHR_IF_ERR_EXIT(dnx_stk_domain_stk_domain_to_trunk_set(unit, tm_domain, stk_tid, 1));

    /*
     * Update Stack FEC resolve HW table
     */
    SHR_IF_ERR_EXIT(dnx_stk_trunk_domain_fec_map_update(unit, tm_domain /* peer domain */ ));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Delete a path from stacking trunk to TM-domain.
 * 
 * \param [in] unit - Unit # 
 * \param [in] tm_domain - tm domian id
 * \param [in] stk_trunk - stacking trunk id to be created by Macro BCM_TRUNK_STACKING_TID_SET.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_domain_stk_trunk_delete(
    int unit,
    int tm_domain,
    bcm_trunk_t stk_trunk)
{
    uint32 stk_tid = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_domain_stk_trunk_verify(unit, tm_domain, stk_trunk));

    /*
     * Clear  Domain to Stack trunk SW table 
     */
    stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk);
    SHR_IF_ERR_EXIT(dnx_stk_domain_stk_domain_to_trunk_set(unit, tm_domain, stk_tid, 0));

    /*
     * Update Stack FEC resolve HW table
     */
    SHR_IF_ERR_EXIT(dnx_stk_trunk_domain_fec_map_update(unit, tm_domain));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * Get array of domains reached by a given stacking trunk.
 * 
 * \param [in] unit - Unit # 
 * \param [in] stk_trunk - stack trunk id
 * \param [in] domain_max - max amount of domains in supplied array.
 * \param [out] domain_array - returned found domains.
 * \param [out] domain_count - amount of domains in returned array.
 * 
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_domain_stk_trunk_get(
    int unit,
    bcm_trunk_t stk_trunk,
    int domain_max,
    int *domain_array,
    int *domain_count)
{
    uint32 tm_domain = 0;
    uint32 tm_domain_count = 0;
    uint32 stk_tid = 0;
    uint32 is_exist = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify */
    SHR_INVOKE_VERIFY_DNX(dnx_stk_modid_domain_get_verify(unit, tm_domain, domain_max, domain_array, domain_count));

    for (tm_domain = 0; tm_domain < dnx_data_stack.general.nof_tm_domains_max_get(unit); tm_domain++)
    {
        SHR_IF_ERR_EXIT(dnx_stk_domain_stk_domain_to_trunk_get(unit, tm_domain, stk_tid, &is_exist));

        if (is_exist == 0x1)
        {
            domain_array[tm_domain_count++] = tm_domain;
        }
        is_exist = 0;

        if (tm_domain_count == domain_max)
        {
            break;
        }
    }

    *domain_count = tm_domain_count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate bcm_stk_module_max_set/get params
 */
static shr_error_e
dnx_stk_module_max_verify(
    int unit,
    uint32 flags)
{
    uint32 mask = BCM_STK_MODULE_MAX_ALL_REACHABLE;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "bcm_dnx_stk_module_max_set/get API is not supported for devices without fabric links\n");
    }

    SHR_MASK_VERIFY(flags, mask, _SHR_E_PARAM, "Unrecognized flags");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *   Set maximum allowed module id in the system.
 * 
 * \param [in] unit -
 *   Unit # 
 * \param [in] flags -
 *   Denotes what to configure.
 *   Can be one of the followings:
 *     0 - set max modid.
 *     BCM_STK_MODULE_MAX_ALL_REACHABLE - set max modid for multicast traffic.
 * \param [in] max_module -
 *   max module id to configure.
 * 
 * \return
 *     see \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_module_max_set(
    int unit,
    uint32 flags,
    bcm_module_t max_module)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_module_max_verify(unit, flags));

    SHR_RANGE_VERIFY(max_module, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                     "max_module is out of range\n");

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_all_reachable_set(unit, max_module));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_set(unit, max_module));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *   Get maximum allowed module id in the system.
 * 
 * \param [in] unit -
 *   Unit # 
 * \param [in] flags -
 *   Denotes what configuration to get.
 *   Can be one of the followings:
 *     0 - set max modid.
 *     BCM_STK_MODULE_MAX_ALL_REACHABLE - get max modid for multicast traffic.
 * \param [out] max_module -
 *   The configured max module id.
 * 
 * \return
 *     see \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stk_module_max_get(
    int unit,
    uint32 flags,
    bcm_module_t * max_module)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_module_max_verify(unit, flags));

    SHR_NULL_CHECK(max_module, _SHR_E_PARAM, "max_module");

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE)
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_all_reachable_get(unit, max_module));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stk_sys_module_max_get(unit, max_module));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate bcm_stk_module_control_set/get params
 */
static shr_error_e
dnx_stk_module_control_verify(
    int unit,
    uint32 flags,
    bcm_module_t module)
{
    uint32 mask = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "bcm_dnx_stk_module_max_set/get API is not supported for devices without fabric links\n");
    }

    SHR_MASK_VERIFY(flags, mask, BCM_E_PARAM, "Unrecognized flags");

    SHR_RANGE_VERIFY(module, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                     "module is out of range.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stk_module_control_set(
    int unit,
    uint32 flags,
    bcm_module_t module,
    bcm_stk_module_control_t control,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_module_control_verify(unit, flags, module));

    switch (control)
    {
            /*
             * Exclude/Include specific module id from all-reachable vector.
             */
        case bcmStkModuleAllReachableIgnore:
        {
            SHR_IF_ERR_EXIT(dnx_stk_sys_module_all_reachable_ignore_id_set(unit, module, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Invalid control %d", control);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stk_module_control_get(
    int unit,
    uint32 flags,
    bcm_module_t module,
    bcm_stk_module_control_t control,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stk_module_control_verify(unit, flags, module));

    SHR_NULL_CHECK(arg, BCM_E_PARAM, "arg");

    switch (control)
    {
            /*
             * Return whether specific module id is excluded/included in
             * all-reachable vector.
             */
        case bcmStkModuleAllReachableIgnore:
        {
            SHR_IF_ERR_EXIT(dnx_stk_sys_module_all_reachable_ignore_id_get(unit, module, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "Invalid control %d", control);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
