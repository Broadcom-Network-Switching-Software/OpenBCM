/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/**
 * \file diag_dnx_visibility.c
 *
 *  Created on: Jan 13, 2018
 *      Author: mf954075
 *
 * Visibility management shell commands.
 */

#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_bitstream.h>
/*
 * soc
 */
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
/*
 * bcm
 */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm/trunk.h>
/*
 * appl
 */
/** allow drv.h include explicitly for system*/
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h> /** needed for BCM_PORT_NAME */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include "tm/diag_dnx_lag.h"
/*
 * }
 */

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t dnx_visibility_modes[] = {
    {"show", -1, "Used to represent get command instead of set"},
    {"None", bcmInstruVisModeNone, "No visibility mode - visibility is disabled"},
    {"Always", bcmInstruVisModeAlways,
     "Always mode - last packet is recorded, ignoring visibility configuration, AKA J1"},
    {"Select", bcmInstruVisModeSelective, "First packet macthing visibiity setting will be recorded"},
    {NULL}
};

/**
 * Keeping all the tokens form general shell to be compliant :)
 */
static sh_sand_enum_t dnx_vis_directions[] = {
    {"Ingress", BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS, "Controls ingress PP port based visibility"},
    {"Egress", BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS, "Controls egress TM port based visibility"},
    {"Both", BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
     "Relevant for ingress & egress part of PP"},
    {NULL}
};

typedef enum
{
    /*
     * Switch something on
     */
    DNX_COMMAND_OFF = FALSE,
    /*
     * Switch something off
     */
    DNX_COMMAND_ON = TRUE,
    /*
     * DO nothing - probably show
     */
    DNX_COMMAND_NONE,
} dnx_command_e;

/**
 * Enable style command enum
 */
static sh_sand_enum_t dnx_visibility_command[] = {
    {"On", DNX_COMMAND_ON, "Property is set"},
    {"Off", DNX_COMMAND_OFF, "Property is reset"},
    {"None", DNX_COMMAND_NONE, "Property is not modified"},
    {NULL}
};

static shr_error_e
visibility_device_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode;
    uint32 prt_qualifier;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("mode", mode);

    if (mode == -1)
    {
        /*
         * Show current state
         */
        bcm_instru_vis_mode_control_t get_mode;
        SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &get_mode, TRUE));
        LOG_CLI((BSL_META("Visibility Mode %s\n"), sh_sand_enum_str(dnx_visibility_modes, get_mode)));
        SHR_IF_ERR_EXIT(dnx_visibility_prt_qualifier_get(unit, &prt_qualifier));
        LOG_CLI((BSL_META("Visibility PRT Qualifier %d\n"), prt_qualifier));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_visibility_mode_set(unit, mode));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_port_show(
    int unit,
    uint32 direction,
    sh_sand_control_t * sand_control,
    bcm_pbmp_t * logical_ports_p)
{
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    bcm_port_interface_info_t port_interface_info;
    bcm_port_mapping_info_t port_mapping_info;
    uint32 flags;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(logical_ports_p, _SHR_E_PARAM, "logical_ports_p");

    PRT_TITLE_SET("Visibility Status Per Port");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "PP Port");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "TM Port");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Core");
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Ingress Enable");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Ingress Force");
    }
    if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Egress Enable");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Egress Force");
    }

    logical_ports = *logical_ports_p;
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        dnx_algo_port_info_s port_info;
        uint32 pp_port, tm_port;
        char port_info_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];

        flags = 0;
        sal_memset(&port_interface_info, 0x0, sizeof(bcm_port_interface_info_t));
        sal_memset(&port_mapping_info, 0x0, sizeof(bcm_port_mapping_info_t));

        SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &port_interface_info, &port_mapping_info));
        if ((flags & (BCM_PORT_ADD_KBP_PORT | BCM_PORT_ADD_STIF_PORT)) ||
            (port_interface_info.interface == BCM_PORT_IF_SFI))
        {
            continue;
        }

        pp_port = port_mapping_info.pp_port;
        tm_port = port_mapping_info.tm_port;

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, port_info.port_type, port_info_str));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s", BCM_PORT_NAME(unit, logical_port));

        if (pp_port == -1)
        {
            PRT_CELL_SET("N/A");
        }
        else
        {
            PRT_CELL_SET("%d", pp_port);
        }
        PRT_CELL_SET("%d", tm_port);
        PRT_CELL_SET("%s", port_info_str);
        PRT_CELL_SET("%d", port_mapping_info.core);

        if (pp_port == -1)
        {
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("N/A");
        }
        else
        {
            if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS)
            {
                int status;
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                                   logical_port, &status), "Port Enable get failed\n");
                PRT_CELL_SET("%d", status);
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                                  logical_port, &status), "Port Force get failed\n");
                PRT_CELL_SET("%d", status);
            }
        }
        if (direction & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS)
        {
            int status;
            SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                               logical_port, &status), "Port Enable get failed\n");
            PRT_CELL_SET("%d", status);
            SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_get(unit, BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS,
                                                              logical_port, &status), "Port Force get failed\n");
            PRT_CELL_SET("%d", status);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_port_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    dnx_command_e enable, force;
    uint32 direction;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("name", logical_ports);
    SH_SAND_GET_ENUM("enable", enable);
    SH_SAND_GET_ENUM("force", force);
    SH_SAND_GET_ENUM("direction", direction);

    if ((force == DNX_COMMAND_NONE) && (enable == DNX_COMMAND_NONE))
    {
        /*
         * Show port status
         */
        SHR_CLI_EXIT_IF_ERR(visibility_port_show(unit, direction, sand_control, &logical_ports), "vis show failed\n");
    }
    else
    {
        BCM_PBMP_ITER(logical_ports, port)
        {
            uint32 visib_flags = direction;
            dnx_algo_port_info_s port_info;
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (!DNX_ALGO_PORT_TYPE_IS_TM(unit, port_info))
            {
                visib_flags &= ~BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS;
            }
            if (!DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, FALSE))
            {
                visib_flags &= ~BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS;
            }
            if (visib_flags == 0)
            {
                continue;
            }
            if (force != DNX_COMMAND_NONE)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_force_set(unit, visib_flags, port, force),
                                    "Port Force failed\n");
            }
            if (enable != DNX_COMMAND_NONE)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_port_enable_set(unit, visib_flags, port, enable),
                                    "Port Enable failed\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file / man section */
static shr_error_e
visibility_lag_show(
    int unit,
    uint32 in_pool,
    uint32 in_group,
    sh_sand_control_t * sand_control)
{
    int pool;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Print table header */
    PRT_TITLE_SET("LAGs");
    PRT_COLUMN_ADD("Pool");
    PRT_COLUMN_ADD("Group");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("PP Port");
    PRT_COLUMN_ADD("Enable");
    PRT_COLUMN_ADD("Force");
    for (pool = 0; pool < dnx_data_trunk.parameters.nof_pools_get(unit); ++pool)
    {
        bcm_trunk_t trunk_id;
        bcm_trunk_pp_port_allocation_info_t allocation_info;
        int core;
        int max_nof_groups_in_pool = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_groups_in_pool;
        int group;
        for (group = 0; group < max_nof_groups_in_pool; ++group)
        {
            int trunk_group_is_used;
                /** Get info */
            SHR_CLI_EXIT_IF_ERR(dnx_trunk_is_used_get(unit, pool, group, &trunk_group_is_used), "");
            if (!trunk_group_is_used)
                continue;

            if ((in_pool != -1) && (in_pool != pool))
                continue;

            if ((in_group != -1) && (in_group != group))
                continue;

            BCM_TRUNK_ID_SET(trunk_id, pool, group);
            sal_memset(&allocation_info, 0, sizeof(bcm_trunk_pp_port_allocation_info_t));
            SHR_CLI_EXIT_IF_ERR(bcm_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info), "");

            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                int status;
                int pp_port = allocation_info.pp_port_per_core_array[core];
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                if (core == 0)
                {
                    PRT_CELL_SET("%d", pool);
                    PRT_CELL_SET("%d", group);
                }
                else
                {
                    PRT_CELL_SKIP(2);
                }
                PRT_CELL_SET("%d", core);
                PRT_CELL_SET("%d", pp_port);
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_enable_get(unit, core, pp_port, &status),
                                    "Port Enable get failed\n");
                PRT_CELL_SET("%d", status);
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_force_get(unit, core, pp_port, &status),
                                    "Port Force get failed\n");
                PRT_CELL_SET("%d", status);
            }
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_lag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_command_e enable, force;
    uint32 pool, group;
    int in_core;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("pool", pool);
    SH_SAND_GET_INT32("group", group);
    SH_SAND_GET_INT32("core", in_core);
    SH_SAND_GET_ENUM("enable", enable);
    SH_SAND_GET_ENUM("force", force);

    if ((force == DNX_COMMAND_NONE) && (enable == DNX_COMMAND_NONE))
    {
        /*
         * Show port status
         */
        SHR_CLI_EXIT_IF_ERR(visibility_lag_show(unit, pool, group, sand_control), "");
    }
    else
    {
        bcm_trunk_t trunk_id;
        bcm_trunk_pp_port_allocation_info_t allocation_info;
        int core;

        if ((pool == -1) || (group == -1))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Lag pool and group need to be provided for this operation\n");
        }
        BCM_TRUNK_ID_SET(trunk_id, pool, group);
        sal_memset(&allocation_info, 0, sizeof(bcm_trunk_pp_port_allocation_info_t));
        SHR_CLI_EXIT_IF_ERR(bcm_trunk_pp_port_allocation_get(unit, trunk_id, 0, &allocation_info), "");

        if (force != DNX_COMMAND_NONE)
        {
            DNXCMN_CORES_ITER(unit, in_core, core)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_force_set(unit, core,
                                                                     allocation_info.pp_port_per_core_array[core],
                                                                     force), "Port Force Set failed\n");
            }
        }
        if (enable != DNX_COMMAND_NONE)
        {
            DNXCMN_CORES_ITER(unit, in_core, core)
            {
                SHR_CLI_EXIT_IF_ERR(dnx_visibility_pp_port_enable_set(unit, core,
                                                                      allocation_info.pp_port_per_core_array[core],
                                                                      enable), "Port Enable Set failed\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
visibility_resume_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core;
    int tmp_value;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_ENUM("direction", flags);

    SH_SAND_GET_BOOL("zero", tmp_value);
    if (tmp_value == TRUE)
        flags |= DNX_VISIBILITY_RESUME_ZERO;

    SHR_IF_ERR_EXIT(dnx_visibility_resume(unit, core, flags));

exit:
    SHR_FUNC_EXIT;
}

static char *
visibility_ready_str(
    shr_error_e status)
{
    char *status_str;
    switch (status)
    {
        case _SHR_E_NONE:
            status_str = "Ready";
            break;
        case _SHR_E_EMPTY:
            status_str = "Not Ready";
            break;
        default:
            status_str = "Error";
            break;
    }
    return status_str;
}

static shr_error_e
visibility_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id, core_idx;
    int full_flag;
    char *asic_block_filter;
    bcm_instru_vis_mode_control_t get_mode;
    dbal_enum_value_field_pp_asic_block_e asic_block;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_BOOL("full", full_flag);
    SH_SAND_GET_STR("block", asic_block_filter);

    PRT_TITLE_SET("Debug Memory Status");

    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("Block");
    PRT_COLUMN_ADD("NoF");
    PRT_COLUMN_ADD("Status");
    if (full_flag == TRUE)
    {
        PRT_COLUMN_ADD("Cache");
    }

    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &get_mode, TRUE));
    PRT_INFO_ADD("  Visibility Mode - \"%s\"", sh_sand_enum_str(dnx_visibility_modes, get_mode));

    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        for (asic_block = DBAL_ENUM_FVAL_PP_ASIC_BLOCK_IPPA; asic_block <= DBAL_ENUM_FVAL_PP_ASIC_BLOCK_ETPPC;
             asic_block++)
        {
            int size, cache_status;

            if (!ISEMPTY(asic_block_filter) &&
                sal_strcasestr(dnx_debug_mem_name(unit, asic_block), asic_block_filter) == NULL)
            {
                continue;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", core_idx);
            PRT_CELL_SET("%s(%d)", dnx_debug_mem_name(unit, asic_block), asic_block);
            SHR_IF_ERR_EXIT(dnx_debug_mem_lines(unit, asic_block, &size));
            PRT_CELL_SET("%d", size);
            PRT_CELL_SET("%s", visibility_ready_str(dnx_debug_mem_is_ready(unit, core_idx, asic_block)));
            if (full_flag == TRUE)
            {
                int i_line;
                for (i_line = 0; i_line < size; i_line++)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                    PRT_CELL_SET_SHIFT(1, "%d", i_line);
                    PRT_CELL_SKIP(1);
                    PRT_CELL_SET("%s", (dnx_debug_mem_line_is_ready(unit, core_idx, asic_block, i_line) == _SHR_E_NONE)
                                 ? "Ready" : "Not Ready");

                    SHR_IF_ERR_EXIT(dnx_debug_mem_cache(unit, core_idx, asic_block, i_line, &cache_status));
                    PRT_CELL_SET("%s", (cache_status == DNX_VISIBILITY_SIG_CACHE_STATUS_VALID) ? "Yes" : "No");
                }
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief - Man page for 'visibility mode' shell command
 */
static sh_sand_man_t visibility_device_man = {
    .brief = "Manage Device visibility abilities",
    .synopsis = NULL,   /* NULL means autogenerated */
    .examples = "mode=always\n" "mode=select\n"
};
/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_device_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"mode",      SAL_FIELD_TYPE_ENUM, "Visibility Mode Type", "show", dnx_visibility_modes},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief - Man page for 'visibility port' shell command
 */
static sh_sand_man_t visibility_port_man = {
    .brief = "Manage port visibility properties",
    .full = "Manage port visibility properties, use without arguments to present current state",
    .synopsis = "[name=]<port_name> [enable=<on|off]>] [force=<on|off>]",
    .examples = "\n" "eth13 enable=on\n" "force=on name=eth14\n" "enable=off direction=ingress name=all\n"
            "enable=on\n" "cell=\"type:ETH\""
};
/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_port_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"direction", SAL_FIELD_TYPE_ENUM, "Visibility Direction", "both", dnx_vis_directions},
    {"enable",    SAL_FIELD_TYPE_ENUM, "Enable/Disable port visibility ", "none", dnx_visibility_command},
    {"force",     SAL_FIELD_TYPE_ENUM, "Force/Unforce port visibility", "none", dnx_visibility_command},
    {"name",      SAL_FIELD_TYPE_PORT, "Port Name or type", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}      /* End of options list - must be last. */
};


/**
 * \brief - Man page for 'visibility port' shell command
 */
static sh_sand_man_t visibility_lag_man = {
    .brief = "Manage lag visibility properties",
    .init_cb = sh_dnx_lag_init_cb,
    .synopsis = "[enable=<on|off]>] [force=<on|off>] [pool=<pool id>] [group=<group id>]",
    .examples = "\n enable=off pool=0 group=21\n force=on pool=0 group=22"
};

static sh_sand_option_t visibility_lag_args[] = {
    /*name        type                  desc                 default ext                             valid_tange */
    {"pool",      SAL_FIELD_TYPE_INT32, "lag pool",          "-1",   (void*)sh_enum_table_lag_pool,  "0-max"},
    {"group",     SAL_FIELD_TYPE_INT32, "lag group in pool", "-1"},
    {"enable",    SAL_FIELD_TYPE_ENUM,  "Enable/Disable port visibility ", "none", dnx_visibility_command},
    {"force",     SAL_FIELD_TYPE_ENUM,  "Force/Unforce port visibility",   "none", dnx_visibility_command},
    {NULL}
};

/**
 * \brief - Man page for 'visibility port' shell command
 */
static sh_sand_man_t visibility_resume_man = {
    .brief = "Clear debug memory and allow for next packet to be recorded",
    .examples = "direction=ingress\n" "core=0\n" "zero\n"
};
/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_resume_args[] = {
   /* Name      | Type                | Description          | Default     */
    {"direction", SAL_FIELD_TYPE_ENUM, "Visibility Direction", "both", dnx_vis_directions},
    {"zero",      SAL_FIELD_TYPE_BOOL, "NUllify the cache in addition to invalidation", "no"},
    {NULL}      /* End of options list - must be last. */
};

static sh_sand_man_t visibility_status_man = {
    .brief = "Show readiness status for debug memories",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "core=0\n" "full\n" "block=IPP full\n"
};

/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t visibility_status_args[] = {
   /* Name      | Type                | Description                | Default     */
    {"full",      SAL_FIELD_TYPE_BOOL, "Present details per block and cache status", "no"},
    {"block",     SAL_FIELD_TYPE_STR, "Present details only for blocks matching this substring", ""},
    {NULL}      /* End of options list - must be last. */
};
sh_sand_man_t sh_dnx_visibility_man = {
    .brief = "Visibility Control commands",
};

sh_sand_cmd_t sh_dnx_visibility_cmds[] = {
  /* Name       | Leaf Action         | Node | Options for Leaf     | Usage                */
    {"global",    visibility_device_cmd, NULL,  visibility_device_args, &visibility_device_man},
    {"port",      visibility_port_cmd,   NULL,  visibility_port_args,   &visibility_port_man},
    {"lag",       visibility_lag_cmd,    NULL,  visibility_lag_args,    &visibility_lag_man},
    {"resume",    visibility_resume_cmd, NULL,  visibility_resume_args, &visibility_resume_man},
    {"status",    visibility_status_cmd, NULL,  visibility_status_args, &visibility_status_man},
    {NULL}
};
/* *INDENT-ON* */
