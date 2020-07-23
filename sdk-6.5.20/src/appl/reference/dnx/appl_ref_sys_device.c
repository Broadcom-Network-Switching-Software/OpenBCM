/** \file appl_ref_sys_device.c
 * 
 *
 * System device level application procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/property.h>
#include <bcm/types.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/port.h>
#include <bcm/fabric.h>
#include <bcm/instru.h>

#include <appl/diag/shell.h>
#include "appl_ref_sys_device.h"
#include "appl_ref_rx_init.h"
#include "appl_ref_l2_init.h"

#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <appl/reference/sand/appl_ref_sand.h>


/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

appl_dnx_sys_device_t *appl_dnx_sys_params = NULL;
int appl_dnx_sys_params_ref_counter = 0;

/*
 * MACROs
 * {
 */
/*
 * }
 */

 /*
  * Global and Static
  */
/*
 * }
 */

/** required Application parameters to describe the system */
/**
 * appl_param_nof_devices = 5                           :: default is 1 - there are X FAPs in the system
 * appl_param_device_type1 = 0x8690                    :: default is taken from bcm device info (bcm_info_get) - device index X is of type 0xYYYY
 * appl_param_device_1_modid_2_map = 0x100              :: map device index 1, 3rd base modid to be 0x100. - device index X is mapped to modid 0xYYY
 *
 * appl_param_cpu_agent_id = 0                          :: map cpu agent id for current cpu agent to 0 - for other CPU agents, this cpu agent ID has to be different
 * appl_param_cpu_agent_0_device_unit_map3 = 2         :: map unit 3 for cpu agent 0 to device index 2
 *
 * appl_param_multi_cpu_agents = 1                      :: there are more than a single CPU agent controlling the FAPs in the system
 * appl_param_sys_port5 = <dev>:<port>:<core>:<tm_port>:<rate>:<port_type>                 :: map system port to device, local port, core, tm port and rate
 *
 * appl_param_stand_alone = 1                           :: system is booted in stand_alone_configuration mode -
 *                                                      :: meaning each device in the system is init as if it is in its own TMD
 *
 * appl_param_modids_per_core_device1 = 2               :: set nof of modids per core for different value than default for SKU
 *                                                      :: set device 1 nof modids per core to be 2
 */
/** { */
/** temporary mechanism to access device info  */
typedef enum
{
    APPL_DNX_SYS_NOF_MODIDS_PER_CORE,
    APPL_DNX_SYS_NOF_CORES,
} device_param_type_t;

appl_dnx_device_info_t device_types[] = {
    /** J2 device */
    {.device_type = 0x8690,
     .nof_cores = 2,
     .nof_modids_per_core = 1},

    /** J2C devices */
    {.device_type = 0x8800,
     .nof_cores = 1,
     .nof_modids_per_core = 3},
    {.device_type = 0x8820,
     .nof_cores = 1,
     .nof_modids_per_core = 3},

    /** J2P devices */
    {.device_type = 0x8850,
     .nof_cores = 2,
     .nof_modids_per_core = 1},

    /** Q2A Devices */
    {.device_type = 0x8480,
     .nof_cores = 1,
     .nof_modids_per_core = 1},

    /** Q2U Devices */
    {.device_type = 0x8280,
     .nof_cores = 1,
     .nof_modids_per_core = 1},
};

/** struct to define input for verify port CB function */
typedef struct
{
    bcm_port_config_t port_config;
    int device_id;
} local_port_verify_input_t;

/**
 * \brief - match value to param type according to device type (can be overridden by advanced access to DNX data in the future)
 */
static shr_error_e
appl_dnx_sys_device_param_get(
    int unit,
    uint32 device_type,
    device_param_type_t param_type,
    int *param_value)
{
    static int NOT_FOUND = -1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(param_value, _SHR_E_PARAM, "param_value");
    *param_value = NOT_FOUND;
    for (int device_type_index = 0; device_type_index < sizeof(device_types) / sizeof(device_types[0]);
         ++device_type_index)
    {
        /** ignore last digit when parsing device type - this digit is reserved for SKUs */
        if ((device_type & 0xFFF0) != device_types[device_type_index].device_type)
        {
            continue;
        }
        switch (param_type)
        {
            case APPL_DNX_SYS_NOF_MODIDS_PER_CORE:
            {
                *param_value = device_types[device_type_index].nof_modids_per_core;
                break;
            }
            case APPL_DNX_SYS_NOF_CORES:
            {
                *param_value = device_types[device_type_index].nof_cores;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "device param was not found, unknown param type = %d\n", param_type);
                break;
            }
        }
        break;
    }
    if (*param_value == NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "device param was not found, unknown device type = 0x%x\n", device_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/** } */

/**
 * \brief - parse device level system parameters (nof devices, base modid, etc...)
 * These attributes are later used by following applications
 */
static shr_error_e
appl_dnx_sys_device_params_parse(
    int unit,
    appl_dnx_sys_device_t * sys_params)
{
    int cpu_agent;
    int multi_cpu_agents_support = 0;
    char cpu_agent_prop[SOC_PROPERTY_NAME_MAX];
    char modid_map_prop[SOC_PROPERTY_NAME_MAX];

    SHR_FUNC_INIT_VARS(unit);

    /** number of device in the system */
    sys_params->nof_devices = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "nof_devices", 1);
    LOG_VERBOSE_EX(BSL_LOG_MODULE, "   nof devices = %d.\n%s%s%s", sys_params->nof_devices, EMPTY, EMPTY, EMPTY);

    /** is stand alone mode */
    sys_params->stand_alone_configuration =
        soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "stand_alone", 0);
    LOG_VERBOSE_EX(BSL_LOG_MODULE, "   stand alone configuration = %d.\n%s%s%s", sys_params->stand_alone_configuration,
                   EMPTY, EMPTY, EMPTY);

    /** allocate devices structs */
    SHR_ALLOC_SET_ZERO(sys_params->devices_array, sizeof(*(sys_params->devices_array)) * sys_params->nof_devices,
                       "devices_array", "%s%s%s", EMPTY, EMPTY, EMPTY);
    /** populate devices struct */
    for (int device_index = 0, total_nof_modids = 0; device_index < sys_params->nof_devices; ++device_index)
    {
        bcm_info_t info;
        SHR_IF_ERR_EXIT(bcm_info_get(unit, &info));
        /** get device type, use default as current unit's type */
        sys_params->devices_array[device_index].device_type =
            soc_property_suffix_num_get_only_suffix(unit, device_index, spn_APPL_PARAM, "device_type", info.device);

        /** fill the rest of the parameters according to device type */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_param_get
                        (unit, sys_params->devices_array[device_index].device_type, APPL_DNX_SYS_NOF_CORES,
                         &sys_params->devices_array[device_index].nof_cores));
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_param_get
                        (unit, sys_params->devices_array[device_index].device_type, APPL_DNX_SYS_NOF_MODIDS_PER_CORE,
                         &sys_params->devices_array[device_index].nof_modids_per_core));

        /** override nof_modids_per_core if indicated by dedicated appl property */
        sys_params->devices_array[device_index].nof_modids_per_core =
            soc_property_suffix_num_get_only_suffix(unit, device_index, spn_APPL_PARAM, "modids_per_core_device",
                                                    sys_params->devices_array[device_index].nof_modids_per_core);

        /** calculate overall modids for device */
        sys_params->devices_array[device_index].nof_modids =
            sys_params->devices_array[device_index].nof_cores *
            sys_params->devices_array[device_index].nof_modids_per_core;

        /** loop on modids per core */
        for (int modid_index = 0; modid_index < sys_params->devices_array[device_index].nof_modids_per_core;
             ++modid_index)
        {
            /** build modid soc property to read :: appl_param_device_<0-2048>_modid_<0-3>_map = 0x100 */
            if (sal_snprintf(modid_map_prop, SOC_PROPERTY_NAME_MAX, "device_%d_modid_%d_map", device_index, modid_index)
                >= SOC_PROPERTY_NAME_MAX)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Max soc property name length (%d) exceeded\n", SOC_PROPERTY_NAME_MAX);
            }

            /** read base modid for this index, default is a running index cross all devices */
            sys_params->devices_array[device_index].base_modids[modid_index] =
                soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, modid_map_prop,
                                                        total_nof_modids +
                                                        (modid_index *
                                                         sys_params->devices_array[device_index].nof_cores));

            /** Check if base modid is too big - need to leave place for consecutive modids for other cores */
            if (sys_params->devices_array[device_index].base_modids[modid_index] >=
                APPL_DNX_NOF_MODIDS - sys_params->devices_array[device_index].nof_cores)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid modid (%d) - too high\n",
                             sys_params->devices_array[device_index].base_modids[modid_index]);
            }
        }

        /** sum modids for next iteration */
        total_nof_modids += sys_params->devices_array[device_index].nof_modids;

        /** fill modid to index array */
        for (int modid_index = 0; modid_index < sys_params->devices_array[device_index].nof_modids_per_core;
             ++modid_index)
        {
            for (int core_index = 0; core_index < sys_params->devices_array[device_index].nof_cores; ++core_index)
            {
                sys_params->modid_to_index[sys_params->devices_array[device_index].base_modids[modid_index] +
                                           core_index].device_index = device_index;
                sys_params->modid_to_index[sys_params->devices_array[device_index].base_modids[modid_index] +
                                           core_index].base_modid_index = modid_index;
            }
        }
    }

    /** build CPU agent soc property for this CPU agent */
    cpu_agent = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "cpu_agent_id", 0);
    if (sal_snprintf(cpu_agent_prop, SOC_PROPERTY_NAME_MAX, "cpu_agent_%d_device_unit_map", cpu_agent) >=
        SOC_PROPERTY_NAME_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max soc property name length (%d) exceeded\n", SOC_PROPERTY_NAME_MAX);
    }

    /** fill unit to device mapping array */
    for (int unit_index = 0; unit_index < SOC_MAX_NUM_DEVICES; ++unit_index)
    {
        /** usually in MGMT systems the FE is unit 0, and the FAPs are units 1,2.
         * the following is a solution to assign proper default even if user didn't specify the relevant application properties */
        int default_device_index = unit_index == 0 ? unit_index : unit_index - 1;
        sys_params->unit_to_device_index[unit_index] =
            soc_property_suffix_num_get_only_suffix(unit, unit_index, spn_APPL_PARAM, cpu_agent_prop,
                                                    default_device_index);
    }

    /** get system port info */
    multi_cpu_agents_support = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "multi_cpu_agents", 0);
    if (multi_cpu_agents_support)
    {
        /** for multiple CPU agents need to get system ports using a dedicated appl soc property */
        SHR_IF_ERR_EXIT(appl_dnx_system_port_db_init(unit, sys_params->system_port_db, sys_params->nof_devices));
    }
    else
    {
        
        /** system ports can be calculated from ucode port or in the future taken directly from each relevant unit */
        /** for now ASSUME SYMMETRIC CONFIGURATION!!! asymmetric configuration for MGMT devices will be delivered later */
        int local_port;
        bcm_pbmp_t pbmp;
        bcm_port_config_t port_config;

        /** get port config info to get olp ports */
        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
        /** get port bitmap - since configuration is assumed symmetric in this case we can
         * assume local unit has the same configuration as all other units */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_ports_get(unit, &pbmp));
        BCM_PBMP_ITER(pbmp, local_port)
        {
            uint32 flags;
            bcm_port_interface_info_t interface_info;
            bcm_port_mapping_info_t mapping_info;
            bcm_port_resource_t port_resource;
            system_port_mapping_t sys_port_map;

            /** get port info */
            SHR_IF_ERR_EXIT(bcm_port_get(unit, local_port, &flags, &interface_info, &mapping_info));
            /** get port speed */
            SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, local_port, &port_resource));

            sys_port_map.local_port = local_port;
            sys_port_map.core = mapping_info.core;
            sys_port_map.tm_port = mapping_info.tm_port;
            sys_port_map.rate = port_resource.speed;
            if (flags & BCM_PORT_ADD_TDM_PORT)
            {
                sys_port_map.port_type = APPL_PORT_TYPE_TDM;
            }
            else if (BCM_PBMP_MEMBER(port_config.olp, local_port))
            {
                sys_port_map.port_type = APPL_PORT_TYPE_OLP;
            }
            else
            {
                sys_port_map.port_type = APPL_PORT_TYPE_NONE;
            }
            /** iterate over all existing devices and define system port for each according to this configuration */
            for (int device_index = 0; device_index < sys_params->nof_devices; ++device_index)
            {
                int system_port_id;
                SHR_IF_ERR_EXIT(appl_sand_device_to_sysport_convert(unit, device_index,
                                                                    APPL_DNX_NOF_SYSPORTS_PER_DEVICE,
                                                                    local_port, &system_port_id));
                sys_port_map.device_index = device_index;
                SHR_IF_ERR_EXIT(appl_dnx_system_port_db_add
                                (unit, sys_params->system_port_db, system_port_id, &sys_port_map));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify parsed system parameters
 */
static shr_error_e
appl_dnx_sys_device_params_verify(
    int unit,
    appl_dnx_sys_device_t * sys_params)
{
    SHR_FUNC_INIT_VARS(unit);

    for (int modid = 0; modid < APPL_DNX_NOF_MODIDS; ++modid)
    {
        int device_index = sys_params->modid_to_index[modid].device_index;
        int base_modid_index = sys_params->modid_to_index[modid].base_modid_index;
        if (device_index == 0xffffffff)
        {
            /** Invalid device index - modid not used */
            continue;
        }

        /** check that modid is in valid range */
        if ((modid < sys_params->devices_array[device_index].base_modids[base_modid_index]) ||
            (modid >=
             sys_params->devices_array[device_index].base_modids[base_modid_index] +
             sys_params->devices_array[device_index].nof_cores))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "modid (%d) is not in correct range (%d - %d)\n", modid,
                         sys_params->devices_array[device_index].base_modids[base_modid_index],
                         sys_params->devices_array[device_index].base_modids[base_modid_index] +
                         sys_params->devices_array[device_index].nof_cores - 1);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FAP index + TM port to
 * modid + ftmh_pp_dsp
 */
shr_error_e
appl_dnx_fap_and_tm_port_to_modport_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid,
    int *ftmh_pp_dsp)
{
    appl_dnx_sys_device_t *sys_params;
    const appl_dnx_device_info_t *device_info;
    int local_index;
    int base_modid;
    SHR_FUNC_INIT_VARS(unit);

    /** override core_id with core 0 in case or BCM_CORE_ALL */
    core_id = (core_id == BCM_CORE_ALL) ? 0 : core_id;

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));
    /** Get pointer to device info */
    device_info = &sys_params->devices_array[fap_index];
    /** Get local index - part of offset from base modid */
    local_index = tm_port / APPL_DNX_NOF_PORTS_PER_MODID;
    /** Get base modid */
    base_modid = device_info->base_modids[local_index];
    /** Get FTMH pp dsp (currently only 8b) */
    *ftmh_pp_dsp = tm_port % APPL_DNX_NOF_PORTS_PER_MODID;
    /** Calculate modid */
    *modid = base_modid + core_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FAP index + TM port to
 * modid
 */
shr_error_e
appl_dnx_fap_and_tm_port_to_modid_convert(
    int unit,
    int fap_index,
    int core_id,
    int tm_port,
    int *modid)
{
    int ftmh_pp_dsp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_fap_and_tm_port_to_modport_convert
                    (unit, fap_index, core_id, tm_port, modid, &ftmh_pp_dsp));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FAP index + TM port to
 * modid
 */
shr_error_e
appl_dnx_modid_to_fap_and_core_convert(
    int unit,
    int modid,
    int *fap_index,
    int *core_id)
{
    appl_dnx_sys_device_t *sys_params;
    int base_modid;
    int base_modid_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** convert modid to device index */
    *fap_index = sys_params->modid_to_index[modid].device_index;
    base_modid_index = sys_params->modid_to_index[modid].base_modid_index;
    base_modid = sys_params->devices_array[*fap_index].base_modids[base_modid_index];

    /** convert modid to core id */
    *core_id = modid - base_modid;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h files
 */
shr_error_e
appl_dnx_sys_device_init(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;
    bcm_stk_modid_config_t modid_config;
    int device_index;

    SHR_FUNC_INIT_VARS(unit);


    /** Get a pointer to the system parameters */
    SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

    /** Configure my modid */
    bcm_stk_modid_config_t_init(&modid_config);

    device_index = sys_params->unit_to_device_index[unit];
    for (int modid_index = 0; modid_index < sys_params->devices_array[device_index].nof_modids_per_core; modid_index++)
    {
        modid_config.modid = sys_params->devices_array[device_index].base_modids[modid_index];
        SHR_IF_ERR_EXIT(bcm_stk_modid_config_add(unit, &modid_config));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_device_params_get(
    int unit,
    appl_dnx_sys_device_t ** sys_params)
{
    SHR_FUNC_INIT_VARS(unit);

    *sys_params = appl_dnx_sys_params;

    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate sys device struct and init values to invalid
 */
static shr_error_e
appl_dnx_sys_device_alloc(
    int unit,
    appl_dnx_sys_device_t ** sys_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /** allocate sys_params and init to 0 */
    SHR_ALLOC_SET_ZERO(appl_dnx_sys_params, sizeof(appl_dnx_sys_device_t), "sys_params", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Create system port DB */
    SHR_IF_ERR_EXIT(appl_dnx_system_port_db_create(unit, &(appl_dnx_sys_params->system_port_db)));

    sal_memset(appl_dnx_sys_params->modid_to_index, 0xff, sizeof(appl_dnx_sys_params->modid_to_index));
    /** return pointer */
    *sys_params = appl_dnx_sys_params;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CB function for sys_db iterator to verify mapping of system ports,
 *          This function
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Note that this step is skipped in WB in order to not free the already allocated sys_params strcuture.
 * \see
 *   * None
 */
static shr_error_e
appl_dnx_sys_local_ports_verify_cb(
    int unit,
    int system_port_id,
    const system_port_mapping_t * sys_port_map_info,
    void *local_port_verify_void_ptr)
{
    local_port_verify_input_t *local_port_verify_input;
    int local_port;
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_resource_t port_resource;

    SHR_FUNC_INIT_VARS(unit);

    /** cast void* to int* */
    local_port_verify_input = local_port_verify_void_ptr;

    /** if system port is not local to this device exit */
    if (local_port_verify_input->device_id != sys_port_map_info->device_index)
    {
        SHR_EXIT();
    }

    /** get port info */
    local_port = sys_port_map_info->local_port;
    SHR_IF_ERR_EXIT_WITH_LOG(bcm_port_get(unit, local_port, &flags, &interface_info, &mapping_info),
                             "Local port %d is not found on unit %d, need to remove it from system mapping\n%s",
                             local_port, unit, EMPTY);
    /** get port speed */
    SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, local_port, &port_resource));

    /** Check TM, core and speed mapping */
    if ((sys_port_map_info->tm_port != mapping_info.tm_port) ||
        (sys_port_map_info->core != mapping_info.core) || (sys_port_map_info->rate != port_resource.speed))
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("system mapping for system port %d: local_port=%d, core=%d, tm_port=%d, rate=%d\n"),
                   system_port_id, sys_port_map_info->local_port, sys_port_map_info->core, sys_port_map_info->tm_port,
                   sys_port_map_info->rate));
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("local mapping for local port %d :                core=%d, tm_port=%d, rate=%d\n"),
                   local_port, mapping_info.core, mapping_info.tm_port, port_resource.speed));
        SHR_ERR_EXIT(_SHR_E_PARAM, "system mapping doesn't match device local mapping\n");
    }

    /** Check TDM indication */
    if (! !(sys_port_map_info->port_type == APPL_PORT_TYPE_TDM) != ! !(flags & BCM_PORT_ADD_TDM_PORT))
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("system mapping for system port %d is TDM=%d\n"), system_port_id,
                   ! !(sys_port_map_info->port_type == APPL_PORT_TYPE_TDM)));
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("matching local port is TDM=%d\n"), ! !(flags & BCM_PORT_ADD_TDM_PORT)));
        SHR_ERR_EXIT(_SHR_E_PARAM, "system mapping doesn't match device local mapping (port type)\n");
    }

    /** Check OLP indication */
    if (! !(sys_port_map_info->port_type == APPL_PORT_TYPE_OLP) !=
        ! !(BCM_PBMP_MEMBER(local_port_verify_input->port_config.olp, local_port)))
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("system mapping for system port %d is OLP=%d\n"), system_port_id,
                   ! !(sys_port_map_info->port_type == APPL_PORT_TYPE_OLP)));
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("matching local port is OLP=%d\n"),
                   ! !(BCM_PBMP_MEMBER(local_port_verify_input->port_config.olp, local_port))));
        SHR_ERR_EXIT(_SHR_E_PARAM, "system mapping doesn't match device local mapping (port type)\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_params_parse(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;
    local_port_verify_input_t local_port_verify_input;
    uint8 mutex_is_mine = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Init the dnxc_verify_allowed flag. */
    dnxc_verify_allowed_init(unit);

    /** take appl global mutex */
    APPL_GLOBAL_LOCK;
    {
        mutex_is_mine = 1;

        /** for first unit in this CPU agent - need to parse system params */
        if (appl_dnx_sys_params == NULL)
        {
            /** allocate sys params for this CPU agent */
            SHR_IF_ERR_EXIT(appl_dnx_sys_device_alloc(unit, &sys_params));

            /** Parse, fill and allocate the parameters */
            SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_parse(unit, sys_params));

            /** Verify parsed system parameters */
            SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_verify(unit, sys_params));
        }
        else
        {
            /** get sys params */
            SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));
        }

        /** update ref counter */
        ++appl_dnx_sys_params_ref_counter;
    }
    APPL_GLOBAL_UNLOCK;
    mutex_is_mine = 0;

    /** Verify input for local unit only! each unit verify its own ports, eventually all ports are verified */
    local_port_verify_input.device_id = sys_params->unit_to_device_index[unit];
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &local_port_verify_input.port_config));
    SHR_IF_ERR_EXIT(appl_dnx_system_port_db_iterate
                    (unit, sys_params->system_port_db, appl_dnx_sys_local_ports_verify_cb, &local_port_verify_input));

exit:
    /** give appl global mutex */
    if (mutex_is_mine)
    {
        APPL_GLOBAL_UNLOCK;
    }
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
appl_dnx_sys_params_free(
    int unit)
{
    int mutex_is_mine = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** take appl global mutex */
    APPL_GLOBAL_LOCK;
    mutex_is_mine = 1;
    {

        /** decrease ref count */
        --appl_dnx_sys_params_ref_counter;

        if (appl_dnx_sys_params_ref_counter == 0)
        {
            /** Destroy system port DB */
            SHR_IF_ERR_EXIT(appl_dnx_system_port_db_destroy(unit, &(appl_dnx_sys_params->system_port_db)));

            /** free resources */
            SHR_FREE(appl_dnx_sys_params->devices_array);
            SHR_FREE(appl_dnx_sys_params);
        }
    }
    /** give appl global mutex */
    APPL_GLOBAL_UNLOCK;
    mutex_is_mine = 0;

exit:
    if (mutex_is_mine != 0)
    {
        APPL_GLOBAL_UNLOCK;
    }
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_traffic_enable(
    int unit)
{
    appl_dnx_sys_device_t *sys_params;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        /** Get a pointer to the system parameters */
        SHR_IF_ERR_EXIT(appl_dnx_sys_device_params_get(unit, &sys_params));

        if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        {
            /** Enable fabric control cells */
            SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricControlCellsEnable, 1));
        }

        /** Enable traffic */
        SHR_IF_ERR_EXIT(bcm_stk_module_enable(unit, 0, -1, 1));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_vis_port_enable(
    int unit,
    bcm_port_t logical_port)
{
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;
    uint32 instru_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    flags = 0;
    sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
    sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));

    /** Indicate egress visibility */
    if (!(flags & APPL_DNX_FILTERED_PORTS_FLAGS))
    {
        instru_flags |= BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS;
    }

    /** Indicate ingress visibility */
    if (port_mapping_info.pp_port != -1)
    {
        instru_flags |= BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS;
    }

    /** set visibility */
    if (instru_flags != 0)
    {
        SHR_IF_ERR_EXIT(bcm_instru_gport_control_set(unit, logical_port,
                                                     instru_flags, bcmInstruGportControlVisForce, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_vis_enable(
    int unit)
{
    bcm_port_config_t port_config;
    bcm_gport_t logical_port;
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_instru_control_set(unit, 0, bcmInstruControlVisMode, bcmInstruVisModeSelective));

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

        BCM_PBMP_ASSIGN(pbmp, port_config.all);
        BCM_PBMP_REMOVE(pbmp, port_config.sfi);
        BCM_PBMP_REMOVE(pbmp, port_config.olp);

        BCM_PBMP_ITER(pbmp, logical_port)
        {
            SHR_IF_ERR_EXIT(appl_dnx_sys_vis_port_enable(unit, logical_port));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_init_done_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Notify Error Recovery -  init done
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_appl_init_state_change(unit, TRUE));

#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_init(unit));
#endif /* DNX_ROLLBACK_JOURNAL_DIAGNOSTICS */

    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_application_init_is_done(unit, TRUE)));

    /*
     * Enable verification after init is done.
     */
    dnxc_verify_allowed_set(unit, 1);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_init_done_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_application_init_is_done(unit, FALSE)));

    /*
     * Notify Error Recovery -  init done
     */
#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_deinit(unit));
#endif /* DNX_ROLLBACK_JOURNAL_DIAGNOSTICS */

    SHR_IF_ERR_EXIT(dnx_rollback_journal_appl_init_state_change(unit, FALSE));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
appl_dnx_sys_device_ports_get(
    int unit,
    bcm_pbmp_t * pbmp)
{
    bcm_port_config_t port_config;
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    bcm_port_t logical_port;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    /** Create a bitmap of all ports */
    BCM_PBMP_CLEAR(*pbmp);
    BCM_PBMP_OR(*pbmp, port_config.nif);
    BCM_PBMP_OR(*pbmp, port_config.cpu);
    BCM_PBMP_OR(*pbmp, port_config.rcy);
    BCM_PBMP_OR(*pbmp, port_config.sat);
    BCM_PBMP_OR(*pbmp, port_config.ipsec);
    BCM_PBMP_OR(*pbmp, port_config.erp);
    BCM_PBMP_OR(*pbmp, port_config.oamp);
    BCM_PBMP_OR(*pbmp, port_config.olp);
    BCM_PBMP_OR(*pbmp, port_config.eventor);

    BCM_PBMP_ITER(*pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));

        /** filter KBP, STIF ports, FlexE phy and L1 ports */
        if (flags & APPL_DNX_FILTERED_PORTS_FLAGS)
        {
            BCM_PBMP_PORT_REMOVE(*pbmp, logical_port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
