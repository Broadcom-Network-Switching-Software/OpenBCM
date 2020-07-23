/** \file diag_dnx_port_autoneg.c
 * 
 * Diagnostics for port Auto-Negotiation
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_PORT

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb.h>
/** sal */
#include <sal/appl/sal.h>

/*
 * }
 */

#define DIAG_DNX_PORT_MAX_ABILITIES              70
#define DIAG_DNX_PORT_MAX_STR_LEN                16
#define DIAG_DNX_PORT_AN_ABILITY_PRT_COLUMN_ADD      \
        PRT_COLUMN_ADD("Port");\
        PRT_COLUMN_ADD("Ability No.");\
        PRT_COLUMN_ADD("Speed");\
        PRT_COLUMN_ADD("Lanes");\
        PRT_COLUMN_ADD("FEC Type");\
        PRT_COLUMN_ADD("Medium Type");\
        PRT_COLUMN_ADD("Pause");\
        PRT_COLUMN_ADD("Channel");\
        PRT_COLUMN_ADD("AN Mode");\

#define DIAG_DNX_PORT_FEC_STR \
        { \
            "NONE", \
            "BASE-R", \
            "RS-528", \
            "RS-544_1XN", \
            "RS-272_1XN", \
            "RS-206", \
            "RS-108", \
            "RS-545", \
            "RS-304", \
            "RS-544_2XN", \
            "RS_272_2XN" \
         }

#define DIAG_DNX_PORT_AN_MODE_STR \
        { \
            "INVALID", \
            "CL37", \
            "CL37_BAM", \
            "CL73", \
            "CL73_BAM", \
            "CL37_CL73", \
            "CL37_CL73_BAM", \
            "CL37_BAM_CL73", \
            "CL37_BAM_CL73_BAM", \
            "CL73_MSA", \
            "NONE" \
        }

#define DIAG_DNX_PORT_MEDIUM_TYPE_STR \
        { \
            "NONE", \
            "COPPER", \
            "FIBER", \
            "BACKPLANE", \
            "ALL", \
        }

#define DIAG_DNX_PORT_PAUSE_STR \
        { \
            "INVALID", \
            "NONE", \
            "RX", \
            "TX", \
            "SYMM", \
            "ALL", \
        }

#define DIAG_DNX_PORT_CHANNEL_STR \
        { \
            "INVALID", \
            "SHORT", \
            "LONG", \
            "ALL", \
        }

typedef enum
{
    DIAG_DNX_PORT_AUTONEG_OPTION_OFF = 0,
    DIAG_DNX_PORT_AUTONEG_OPTION_ON,
    DIAG_DNX_PORT_AUTONEG_OPTION_INFO
} diag_dnx_port_autoneg_option_e;

/**
 * \brief - Function to format AN Info table
 */
static shr_error_e
sh_dnx_port_autoneg_status_format(
    int unit,
    prt_control_t * prt_ctr,
    bcm_port_t port,
    bcm_port_speed_ability_t * abilities,
    int ability_num)
{
    int i, count;
    char speed_str[DIAG_DNX_PORT_MAX_STR_LEN];
    char *fec_str[] = DIAG_DNX_PORT_FEC_STR;
    char *an_mode_str[] = DIAG_DNX_PORT_AN_MODE_STR;
    char *medium_type_str[] = DIAG_DNX_PORT_MEDIUM_TYPE_STR;
    char *pause_str[] = DIAG_DNX_PORT_PAUSE_STR;
    char *channel_str[] = DIAG_DNX_PORT_CHANNEL_STR;

    SHR_FUNC_INIT_VARS(unit);

    count = 0;
    for (i = 0; i < ability_num; ++i)
    {
        /*
         * Since we only print AN abilities, filter the FS abilities here
         */
        if (abilities[i].an_mode == bcmPortAnModeNone)
        {
            continue;
        }

        if (count == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%d", port);
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
        }
        PRT_CELL_SET("%d", count);
        PRT_CELL_SET("%s", if_fmt_speed(speed_str, abilities[i].speed));
        PRT_CELL_SET("%d", abilities[i].resolved_num_lanes);
        PRT_CELL_SET("%s", fec_str[abilities[i].fec_type - 1]);
        PRT_CELL_SET("%s", medium_type_str[abilities[i].medium]);
        PRT_CELL_SET("%s", pause_str[abilities[i].pause]);
        PRT_CELL_SET("%s", channel_str[abilities[i].channel]);
        PRT_CELL_SET("%s", an_mode_str[abilities[i].an_mode]);
        count++;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to dump AN info
 */
static shr_error_e
sh_dnx_port_autoneg_status_dump(
    int unit,
    sh_sand_control_t * sand_control,
    bcm_pbmp_t * pbmp)
{
    int actual_ability_num;
    phymod_autoneg_status_t an_status;
    bcm_port_t port;
    bcm_port_speed_ability_t abilities[DIAG_DNX_PORT_MAX_ABILITIES];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print local supported AN abilities.
     */
    PRT_TITLE_SET("AutoNeg Local Supported Abilities");
    DIAG_DNX_PORT_AN_ABILITY_PRT_COLUMN_ADD;

    BCM_PBMP_ITER(*pbmp, port)
    {
        sal_memset(abilities, 0, DIAG_DNX_PORT_MAX_ABILITIES * sizeof(bcm_port_speed_ability_t));
        SHR_IF_ERR_EXIT(bcm_port_speed_ability_local_get
                        (unit, port, DIAG_DNX_PORT_MAX_ABILITIES, abilities, &actual_ability_num));
        SHR_IF_ERR_EXIT(sh_dnx_port_autoneg_status_format(unit, prt_ctr, port, abilities, actual_ability_num));
    }
    PRT_COMMITX;

    /*
     * Print local advertised AN abilities.
     */
    PRT_TITLE_SET("AutoNeg Local Advertised Abilities");
    DIAG_DNX_PORT_AN_ABILITY_PRT_COLUMN_ADD;

    BCM_PBMP_ITER(*pbmp, port)
    {
        sal_memset(abilities, 0, DIAG_DNX_PORT_MAX_ABILITIES * sizeof(bcm_port_speed_ability_t));
        SHR_IF_ERR_EXIT(bcm_port_autoneg_ability_advert_get
                        (unit, port, DIAG_DNX_PORT_MAX_ABILITIES, abilities, &actual_ability_num));
        SHR_IF_ERR_EXIT(sh_dnx_port_autoneg_status_format(unit, prt_ctr, port, abilities, actual_ability_num));
    }
    PRT_COMMITX;

    /*
     * Print Remote advertised AN abilities.
     */
    PRT_TITLE_SET("AutoNeg Received Abilities From Partner");
    DIAG_DNX_PORT_AN_ABILITY_PRT_COLUMN_ADD;

    BCM_PBMP_ITER(*pbmp, port)
    {
        SHR_IF_ERR_EXIT(portmod_port_autoneg_status_get(unit, port, &an_status));
        /*
         * Remote abiltiy is only valid when AN is enabled and locked.
         */
        if (an_status.enabled && an_status.locked)
        {
            sal_memset(abilities, 0, DIAG_DNX_PORT_MAX_ABILITIES * sizeof(bcm_port_speed_ability_t));
            SHR_IF_ERR_EXIT(bcm_port_autoneg_ability_remote_get
                            (unit, port, DIAG_DNX_PORT_MAX_ABILITIES, abilities, &actual_ability_num));
            SHR_IF_ERR_EXIT(sh_dnx_port_autoneg_status_format(unit, prt_ctr, port, abilities, actual_ability_num));
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Turn on/off auto-negotiation or dump
 *    AN info
 */
shr_error_e
sh_dnx_port_autoneg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t port;
    bcm_pbmp_t pbmp;
    dnx_algo_port_info_s port_info;
    diag_dnx_port_autoneg_option_e option;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_ENUM("mode", option);

    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, TRUE, TRUE))
        {
            BCM_PBMP_PORT_REMOVE(pbmp, port);
        }
    }

    if ((option == DIAG_DNX_PORT_AUTONEG_OPTION_ON) || (option == DIAG_DNX_PORT_AUTONEG_OPTION_OFF))
    {
        BCM_PBMP_ITER(pbmp, port)
        {
            SHR_IF_ERR_EXIT(bcm_port_autoneg_set(unit, port, option));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_port_autoneg_status_dump(unit, sand_control, &pbmp));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/* *INDENT-OFF* */
/** port auto-negotiation */
static sh_sand_enum_t dnx_port_autoneg_option_enum_table[] = {
    {"on",     DIAG_DNX_PORT_AUTONEG_OPTION_ON, "Enable Autoneg"},
    {"off",    DIAG_DNX_PORT_AUTONEG_OPTION_OFF, "Disable Autoneg"},
    {"info",   DIAG_DNX_PORT_AUTONEG_OPTION_INFO, "Show Autoneg info"},
    {NULL}
};

sh_sand_man_t sh_dnx_port_autoneg_man = {
    .brief    = "Diagnostic for Auto-Negotiation",
    .full     = "Enable, Disable or Show AN info",
    .synopsis = "<pbmp> [<on|off|info>]",
    .examples = "eth info \n"
                "xe13 \n"
                "14 off"
};

sh_sand_option_t sh_dnx_port_autoneg_options[] = {
    {"pbmp", SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {"mode", SAL_FIELD_TYPE_ENUM, "on, off or info", "info", (void *)dnx_port_autoneg_option_enum_table, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
