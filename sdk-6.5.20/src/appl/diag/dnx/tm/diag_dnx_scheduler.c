/** \file diag_dnx_scheduler.c
 *
 * diagnostics for e2e scheduler
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/stack.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/algo/sch/sch_alloc_mngr.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
/** sw state */
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_sch_alloc_mngr_access.h>
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnx_scheduler.h"
#include <appl/diag/dnx/diag_dnx_sch.h>

#define DIAG_DNX_GPORT_COUNT_CORE_DEPENDENT_PRINT(unit, gport_type_str, data_array) \
            do  \
            { \
                PRT_ROW_ADD(PRT_ROW_SEP_NONE); \
                PRT_CELL_SET("%s", gport_type_str); \
                DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)      \
                { \
                    PRT_CELL_SET("%d", data_array[core]); \
                } \
                PRT_CELL_SKIP(1); \
            } while (0);

#define DIAG_DNX_GPORT_COUNT_NOT_CORE_DEPENDENT_PRINT(unit, gport_type_str, data) \
            do  \
            { \
                PRT_ROW_ADD(PRT_ROW_SEP_NONE); \
                PRT_CELL_SET("%s", gport_type_str); \
                DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)      \
                { \
                    PRT_CELL_SKIP(1); \
                } \
                PRT_CELL_SET("%d", data); \
            } while (0);

/**
 * \brief -
 * Function to check if given gport is a shared shaper
 */
static shr_error_e
sh_dnx_scheduler_gport_is_shared_shaper_get(
    int unit,
    bcm_gport_t gport,
    int *is_shared_shaper,
    bcm_cosq_scheduler_gport_mode_t * mode)
{
    bcm_cosq_scheduler_gport_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    *is_shared_shaper = FALSE;
    *mode = bcmCosqSchedulerGportModeSimple;

    if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        SHR_IF_ERR_EXIT(bcm_cosq_scheduler_gport_get(unit, 0, gport, &params));
        if (params.mode == bcmCosqSchedulerGportModeSharedDual ||
            params.mode == bcmCosqSchedulerGportModeSharedQuad || params.mode == bcmCosqSchedulerGportModeSharedOcta)
        {
            *is_shared_shaper = TRUE;
            *mode = params.mode;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to create gport for scheduling elements and
 *        connectors from flow_id and core
 */
static shr_error_e
sh_dnx_scheduler_flow_gport_get(
    int unit,
    int core,
    int flow_id,
    bcm_gport_t * flow_gport)
{
    dnx_sch_element_se_type_e se_type;
    int is_shared_shaper;
    bcm_cosq_scheduler_gport_mode_t scheduler_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));

    if (se_type == DNX_SCH_ELEMENT_SE_TYPE_NONE)
    {
        /** If none flow is VOQ connector */
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(*flow_gport, flow_id, core);
    }
    else
    {
        /** Else it is HR,FQ, CL or Shared Shaper */
        BCM_GPORT_SCHEDULER_CORE_SET(*flow_gport, flow_id, core);
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_gport_is_shared_shaper_get
                        (unit, *flow_gport, &is_shared_shaper, &scheduler_mode));
        if (is_shared_shaper)
        {
            BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_SET(*flow_gport, flow_id, core);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Function to remap the enum elements  from
 * bcm_cosq_scheduler_gport_mode_t to strings
 */
const char *
diag_dnx_scheduler_se_mode_string_get(
    bcm_cosq_scheduler_gport_mode_t key)
{
    char *se_mode_string[] = {
        "Simple",
        "Composite",
        "Enhanced, Low Prio FQ",
        "Enhanced, High Prio FQ",
        "Shared Dual",
        "Shared Quad",
        "Shared Octa"
    };

    return se_mode_string[key];
}

/**
 * \brief -
 * Function to get the parent of the parent if possible.This is
 * needed in order to skip the printing of stand alone Port HR
 * Scheduling Elements
 */
static shr_error_e
sh_dnx_scheduler_flow_is_port_hr_get(
    int unit,
    int core,
    bcm_gport_t child_gport,
    bcm_gport_t * parent_gport,
    int *is_port_hr)
{

    SHR_FUNC_INIT_VARS(unit);
    *is_port_hr = FALSE;
    *parent_gport = BCM_GPORT_INVALID;
    /** Get flow parent */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_parent_get(unit, child_gport, 0, parent_gport));

    if (BCM_COSQ_GPORT_IS_E2E_PORT(*parent_gport) || BCM_COSQ_GPORT_IS_FMQ_CLASS(*parent_gport))
    {
        *is_port_hr = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * --------------------------------------------------------------------------
 * dnx e2e scheduler connection info
 * --------------------------------------------------------------------------
 */
/**
 * \brief Interface for scheduler connection diagnostic
 */
typedef struct
{
    /**
     * passes filter per core_id
     */
    int core_id;
    /**
     * passes type of print requested
     * if 1 ingress, 0 - egress
     */
    int is_ingress;
    /**
     * passes shell framework print control
     */
    void *prt_ctr;

} diag_dnx_tm_scheduler_connection_parameters_t;

/**
 * see .h
 */
shr_error_e
dnx_sch_connection_voq_row_print(
    int unit,
    bcm_gport_t gport,
    int numq,
    int core_id_filter,
    prt_control_t * prt_ctr)
{
    int core;
    bcm_cosq_gport_connection_t gport_connect;
    bcm_gport_t voq_id, voq_connector_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Go over all the cores selected and proveded by the diag */
    DNXCMN_CORES_ITER(unit, core_id_filter, core)
    {

        /** THE VOQ Gport provided from the gport traverse is with
         *  core BCM_CORE_ALL because Ingress side always
         *  symetrical. Still The bcm_cosq_gport_connection_get API
         *  will provide different VOQ connector for the two cores.
         *  In order to obtain them we manually set the VOQ core */

        /** Assume that the provided Gport is a VOQ */
        gport_connect.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;

        /**
         * Only MC and UC queue gports can be passed into this function.
         */
        /*
         * Input gport contains BCM_CORE_ALL and bcm_cosq_gport_connection_get should be called with specific core.
         */
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
        {
            voq_id = BCM_GPORT_MCAST_QUEUE_GROUP_QUEUE_GET(gport);
            gport_connect.voq = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(gport, core, voq_id);
        }
        else
        {
            voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
            gport_connect.voq = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport, core, voq_id);
        }

        SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_get(unit, &gport_connect));

        /** Get the relevant VOQ connector for the current VOQ on the
         *  given core */
        voq_connector_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect.voq_connector);

        /*
         * INGRESS Print
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", voq_id);
        PRT_CELL_SET("%d", numq);
        PRT_CELL_SET("%d", core);
        PRT_CELL_SET("--->");
        if (voq_connector_id)
        {
            PRT_CELL_SET("%d", voq_connector_id);
        }
        else
        {
            PRT_CELL_SET("-");
        }
        PRT_CELL_SET("%d", gport_connect.remote_modid);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see .h
 */
shr_error_e
dnx_sch_connection_voq_connector_row_print(
    int unit,
    bcm_gport_t gport,
    int numq,
    int core_id_filter,
    prt_control_t * prt_ctr)
{
    int core, core_filter, remote_core_idx;
    bcm_cosq_gport_connection_t gport_connect;
    bcm_gport_t voq_id, voq_connector_id, voq_connector_i;
    int apply_core_filter = TRUE;
    int nof_remote_cores;

    SHR_FUNC_INIT_VARS(unit);

    /** Assume that the provided Gport is a VOQ connector */
    voq_connector_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport);
    core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport);

    /** Get relevant VOQ for current VOQ connector */
    gport_connect.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    gport_connect.voq_connector = gport;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_get(unit, &gport_connect));
    voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_connect.voq);

    /** Check if diag should print for given core */
    DNXCMN_CORES_ITER(unit, core_id_filter, core_filter)
    {
        if (core == core_filter)
        {
            apply_core_filter = FALSE;
            break;
        }
    }
    /** Don't print if VOQ connector is on different core than the
     *  specified core filter*/
    if (apply_core_filter)
    {
        return BCM_E_NONE;
    }

    /** Get number of remote Cores */
    SHR_IF_ERR_EXIT(dnx_sch_voq_connector_nof_remote_cores_get(unit, core, voq_connector_id, &nof_remote_cores));
    for (remote_core_idx = 0; remote_core_idx < nof_remote_cores; remote_core_idx++)
    {
        voq_connector_i = DNX_SCH_FLOW_ID_FROM_REMOTE_CORE(voq_connector_id, nof_remote_cores, remote_core_idx);

        /*
         * EGRESS Print
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", voq_connector_i);
        PRT_CELL_SET("%d", numq);
        PRT_CELL_SET("%d", core);
        PRT_CELL_SET("--->");
        PRT_CELL_SET("%d", voq_id);
        PRT_CELL_SET("%d", (gport_connect.remote_modid + remote_core_idx));
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_cosq_connection_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    diag_dnx_tm_scheduler_connection_parameters_t *diag_params;
    prt_control_t *prt_ctr;

    SHR_FUNC_INIT_VARS(unit);
    diag_params = (diag_dnx_tm_scheduler_connection_parameters_t *) user_data;

    prt_ctr = (prt_control_t *) diag_params->prt_ctr;

    /*
     * Choose Ingress or Egress scheduler connection info to display
     */
    if (diag_params->is_ingress && (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)))
    {
        SHR_IF_ERR_EXIT(dnx_sch_connection_voq_row_print(unit, gport, numq, diag_params->core_id, prt_ctr));
    }
    else if (!(diag_params->is_ingress) && BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        SHR_IF_ERR_EXIT(dnx_sch_connection_voq_connector_row_print(unit, gport, numq, diag_params->core_id, prt_ctr));
    }
    else
    {
        /*
         * This function is called from bcm_cosq_gport_traverse function which itarates over all cosq gports. Skip
         * this cosq gport if not VOQ or VOQ connector
         */
        return BCM_E_NONE;
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t sh_dnx_scheduler_connection_options[] = {
/* *INDENT-OFF* */
    /*name          type                    desc                                default   ext*/
    {"ingress",     SAL_FIELD_TYPE_BOOL,    "Ingress scheduler info option",    "false"},
    {"egress",      SAL_FIELD_TYPE_BOOL,    "Egress scheduler info option",     "false"},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_connection_man = {
    .brief = "Print e2e scheduler connection info",
    .full = "Diagnostic for the E2E Scheduler connections. \n" "Mandatory input is the stage - INGress or EGress.",
    .synopsis = " <INGress/EGress> [core=<core_id>]",
    .examples = "INGress core=0\n" "EGress"
};

/**
 * \brief - Print E2E scheduler connection info.
 */
shr_error_e
sh_dnx_scheduler_connection_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 is_ingress, is_egress;
    int core_id;
    diag_dnx_tm_scheduler_connection_parameters_t pass_params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** get parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_BOOL("ingress", is_ingress);
    SH_SAND_GET_BOOL("egress", is_egress);

    if (!is_ingress && !is_egress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Please choose if INGress or EGress scheduler connection info is requested. See 'tm scheduler connection usage' for more info \n");
    }

    if (is_ingress && is_egress)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Passing 'ingress' togerther with 'egress' as parameters is not allowed. See 'tm scheduler connection usage' for more info \n");
    }

    if (is_ingress)
    {
        /*
         * INGRESS Display
         */
        PRT_TITLE_SET("E2E Scheduler connections - Ingress");
        PRT_COLUMN_ADD("VOQ ID");
        PRT_COLUMN_ADD("NOF VOQs");
        PRT_COLUMN_ADD("Core");
        PRT_COLUMN_ADD(" ");
        PRT_COLUMN_ADD("Remote VOQ Connector");
        PRT_COLUMN_ADD("Remote Modid");

        pass_params.is_ingress = 1;
    }
    else
    {
        /*
         * EGRESS Display
         */
        PRT_TITLE_SET("E2E Scheduler connections - Egress");
        PRT_COLUMN_ADD("VOQ Connector");
        PRT_COLUMN_ADD("NOF VOQ Connectors");
        PRT_COLUMN_ADD("Core");
        PRT_COLUMN_ADD("#");
        PRT_COLUMN_ADD("Remote VOQ");
        PRT_COLUMN_ADD("Remote Modid");

        pass_params.is_ingress = 0;
    }
    pass_params.core_id = core_id;
    /*
     * The structure of the table is in this function
     * But the rows of the table are populated in the
     * cosq_gport_traverse CB function.
     * In order for this to be possible some shell framework
     * controlls should be passed on also to the CB function
     * like pointer of the printing control
     */
    pass_params.prt_ctr = prt_ctr;

    SHR_IF_ERR_EXIT(bcm_cosq_gport_traverse(unit, dnx_cosq_connection_cb, &pass_params));

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * --------------------------------------------------------------------------
 * dnx scheduler gport diag
 * --------------------------------------------------------------------------
 */
/**
 * \brief - Structure for counting all scheduler elements and VOQs
 */
typedef struct
{
    /**
     * Count of VOQ connectors per core
     */
    int nof_conn[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    /**
    * Count Unicast queue bundles
    */
    int nof_uc_qs;
    /**
    * Count Multicast queue bundles
    */
    int nof_mc_qs;
    /**
     * Count of CL Scheduler Elementss per core
     */
    int nof_cl_ses[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    /**
     * Count of FQ Scheduler Elementss per core
     */
    int nof_fq_ses[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    /**
     * Count of HR Scheduler Elementss per core
     */
    int nof_hr_ses[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
} diag_dnx_gport_count_t;

/**
 * \brief - Structure for passing additional parameter for the
 *        Show all Flows print function
 */
typedef struct
{
    /**
     * Pointer to the prt control for the diag shell framework
     */
    prt_control_t *prt_ctr;
    /**
     * Flag indicating is 'verbose' provided to the diag
     */
    int verbose_is_provided;
} diag_dnx_gport_show_flows_params_t;

/**
* \brief
*   Function for retriving a a key enum for getting string
*   describing the given gport.
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    Relevant GPORT
*  \param [out] key -
*     Enum Key for diag_dnx_scheduler_gport_type_string_get
*     giving string descriping the gport type
*  \return
*    shr_error_e -
*      Error return value
 */
static shr_error_e
diag_dnx_scheduler_gport_type_string_key_get(
    int unit,
    bcm_gport_t gport,
    diag_dnx_sch_gport_type_string_e * key)
{
    int flow_id, core;
    dnx_sch_element_se_type_e se_type;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        *key = DIAG_DNX_SCH_GPORT_TYPE_STRING_UCAST_GROUP;
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        *key = DIAG_DNX_SCH_GPORT_TYPE_STRING_MCAST_GROUP;
    }
    else if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        flow_id = BCM_GPORT_SCHEDULER_GET(gport);
        core = BCM_GPORT_SCHEDULER_CORE_GET(gport);
        SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));

        switch (se_type)
        {
            case DNX_SCH_ELEMENT_SE_TYPE_HR:
                *key = DIAG_DNX_SCH_GPORT_TYPE_STRING_SE_HR;
                break;
            case DNX_SCH_ELEMENT_SE_TYPE_FQ:
                *key = DIAG_DNX_SCH_GPORT_TYPE_STRING_SE_FQ;
                break;
            case DNX_SCH_ELEMENT_SE_TYPE_CL:
                *key = DIAG_DNX_SCH_GPORT_TYPE_STRING_SE_CL;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flow id %d", flow_id);
        }
    }
    else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        *key = DIAG_DNX_SCH_GPORT_TYPE_STRING_VOQ_CONN;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Gport type: 0x%x \n", gport);
    }
exit:
    SHR_FUNC_EXIT;
}
const char *
diag_dnx_scheduler_gport_type_string_get(
    diag_dnx_sch_gport_type_string_e key)
{
    char *gport_string[] = {
        "Unicast Queue Group",
        "Multicast Queue Group",
        "Scheduler HR",
        "Scheduler FQ",
        "Scheduler CL",
        "Scheduler VOQ Connector"
    };

    return gport_string[key];
}

/**
 * \brief - Count the gport numbers for each gport type
 */
static shr_error_e
dnx_cosq_gport_count_all_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    int flow_id, core, core_id;
    dnx_sch_element_se_type_e se_type;
    diag_dnx_gport_count_t *gport_count = (diag_dnx_gport_count_t *) user_data;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport);
        DNXCMN_CORE_VALIDATE(unit, core, FALSE);
        gport_count->nof_conn[core]++;
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        gport_count->nof_uc_qs++;
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        gport_count->nof_mc_qs++;
    }
    else if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        flow_id = BCM_GPORT_SCHEDULER_GET(gport);
        core_id = BCM_GPORT_SCHEDULER_CORE_GET(gport);
        DNXCMN_CORES_ITER(unit, core_id, core)
        {
            SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));
            switch (se_type)
            {
                case DNX_SCH_ELEMENT_SE_TYPE_CL:
                    gport_count->nof_cl_ses[core]++;
                    break;
                case DNX_SCH_ELEMENT_SE_TYPE_FQ:
                    gport_count->nof_fq_ses[core]++;
                    break;
                case DNX_SCH_ELEMENT_SE_TYPE_HR:
                    gport_count->nof_hr_ses[core]++;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flow id %d", flow_id);
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Gport type: 0x%x \n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print all cosq VOQ gports info.
 */
int
dnx_cosq_gport_show_all_voqs_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    int num_cos_levels;
    int modid = BCM_MODID_INVALID;
    int modport = BCM_PORT_INVALID;
    bcm_gport_t mod_port = 0;
    uint32 flags2 = 0;
    diag_dnx_sch_gport_type_string_e gport_str_key;

    prt_control_t *prt_ctr = (prt_control_t *) user_data;

    SHR_FUNC_INIT_VARS(unit);

    /** Skip all gports that are not UC or MC VOQs */
    if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) && !BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        return _SHR_E_NONE;
    }

    SHR_IF_ERR_EXIT(bcm_cosq_gport_get(unit, gport, &mod_port, &num_cos_levels, &flags2));

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("0x%x", gport);
    PRT_CELL_SET("%d", num_cos_levels);

    SHR_IF_ERR_EXIT(diag_dnx_scheduler_gport_type_string_key_get(unit, gport, &gport_str_key));
    PRT_CELL_SET("%s", diag_dnx_scheduler_gport_type_string_get(gport_str_key));

    /*
     * Print Modid and port only for UC queues
     */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        modid = BCM_GPORT_MODPORT_MODID_GET(mod_port);
        modport = BCM_GPORT_MODPORT_PORT_GET(mod_port);

        PRT_CELL_SET("%d", modid);
        PRT_CELL_SET("%d", modport);
        /** Print base queue */
        PRT_CELL_SET("%d", BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport));
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        /** Skip port and modid for MC VOQs */
        PRT_CELL_SKIP(2);
        /** Print base queue */
        PRT_CELL_SET("%d", BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported Gport Type 0x%x \n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print all cosq Flow gports info.
 */
int
dnx_cosq_gport_show_all_flows_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    int num_cos_levels, mode = BCM_COSQ_SP0, weight, max_burst;
    int flow_id, core, flow_is_port_hr = FALSE, se_id, tc, is_shared_shaper = FALSE, fmq_class;
    bcm_gport_t mod_port = 0, local_gport, parent_gport = BCM_GPORT_INVALID;
    bcm_port_t logical_port;
    bcm_cos_queue_t cos;
    uint32 flags2 = 0;
    uint32 kbits_sec_min, kbits_sec_max;
    diag_dnx_sch_gport_type_string_e gport_str_key;
    prt_control_t *prt_ctr;
    bcm_cosq_gport_info_t gport_info;
    diag_dnx_gport_show_flows_params_t *diag_pass_params = (diag_dnx_gport_show_flows_params_t *) user_data;
    char mode_str[DIAG_DNX_SCH_GPORT_MODE_STR_LENGTH];
    dnx_sch_element_se_type_e se_type;
    bcm_cosq_scheduler_gport_mode_t shared_shaper_mode;

    SHR_FUNC_INIT_VARS(unit);

    prt_ctr = diag_pass_params->prt_ctr;

    /** Print only for Flows */
    if (!BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport) && !BCM_GPORT_IS_SCHEDULER(gport))
    {
        return _SHR_E_NONE;
    }

    /** Find gport Flow id and core*/
    core =
        BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport) ? BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport) :
        BCM_GPORT_SCHEDULER_CORE_GET(gport);
    flow_id =
        BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport) ? BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport) :
        BCM_GPORT_SCHEDULER_GET(gport);

    /** Get scheduling Element type */
    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));
    /**
     * Get NUM COS LEVELS
     * The switch is mainly to avoid accessing bcm_cosq_gport_get with shared shaper gport
     */
    if (se_type == DNX_SCH_ELEMENT_SE_TYPE_NONE)
    {
       /** VOQ connector handle */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_get(unit, gport, &mod_port, &num_cos_levels, &flags));
    }
    else
    {
       /** SE handle */
        num_cos_levels = 1;
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("0x%x", gport);
    PRT_CELL_SET("%d", num_cos_levels);

    /**
     * Check if gport is shared shaper
     */
    SHR_IF_ERR_EXIT(sh_dnx_scheduler_gport_is_shared_shaper_get(unit, gport, &is_shared_shaper, &shared_shaper_mode));
    /**
     * Get the type of the SE
     */
    if (is_shared_shaper)
    {
        PRT_CELL_SET("%s", diag_dnx_scheduler_se_mode_string_get(shared_shaper_mode));
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_dnx_scheduler_gport_type_string_key_get(unit, gport, &gport_str_key));
        PRT_CELL_SET("%s", diag_dnx_scheduler_gport_type_string_get(gport_str_key));
    }

    PRT_CELL_SET("%d", core);
    PRT_CELL_SET("%d", flow_id);

    if (diag_pass_params->verbose_is_provided)
    {
        for (cos = 0; cos < num_cos_levels; cos++)
        {

            if (num_cos_levels == 1)
            {
                /** Conitnue printing on same line */
            }
            else if (cos < num_cos_levels - 1)
            {
                /** Last row before next gport to be with underscore
                 *  separator */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(5);
            }
            else
            {
                /** All new lines per COS of same gport won't have separator
                 *  in between */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SKIP(5);
            }

            PRT_CELL_SET("%d", cos);

            /** Check if flow is Port HR. If so overwrite the gport
             *  variable and use as input to bcm_cosq_gport_bandwidth_get
             *  and bcm_cosq_control_get a E2E TC port */
            if (is_shared_shaper)
            {
                /**
                 * For Shared shaper Mode, Weight, BandWidth, Burst won't be printed
                 */
                PRT_CELL_SET("N/A");
                PRT_CELL_SET("N/A");
                PRT_CELL_SET("N/A");
                PRT_CELL_SET("N/A");
            }
            else
            {
                /**
                 * If Gport is shared shaper for sure it won't be Port HR
                 * Also If accessing parent get with raw shared_shaper gport and error will be triggered
                 */
                SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_is_port_hr_get
                                (unit, core, gport, &parent_gport, &flow_is_port_hr));

                if (BCM_COSQ_GPORT_IS_FMQ_CLASS(parent_gport))
                {
                    SHR_IF_ERR_EXIT(dnx_scheduler_fmq_hr_gport_fmq_class_get(unit, parent_gport, &fmq_class));
                    PRT_CELL_SET("N/A (FMQ Class %d)", fmq_class);
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    if (flow_is_port_hr)
                    {
                        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, flow_id, &se_id));
                        SHR_IF_ERR_EXIT(dnx_sch_convert_se_id_to_port_tc(unit, core, se_id, &logical_port, &tc));
                        /** Get current TC gport */
                        BCM_GPORT_LOCAL_SET(local_gport, logical_port);
                        gport_info.in_gport = local_gport;
                        gport_info.cosq = tc;
                        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &gport_info));

                        /** Return the correct E2E TC gport */
                        gport = gport_info.out_gport;

                        /** Don't print Mode and Weight for port HR */
                        PRT_CELL_SET("N/A");
                        PRT_CELL_SET("N/A");
                    }
                    else
                    {
                        /** If NOT port HR and SHARED SHAPER
                         *  Print Mode and Weight Also  */
                        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, gport, cos, &mode, &weight));
                        sal_snprintf(mode_str, 15, "BCM_COSQ_SP_%d", mode - BCM_COSQ_SP0);
                        PRT_CELL_SET("%s", mode_str);
                        PRT_CELL_SET("%d", weight);
                    }

                    /**
                     * Print Max BW and MAX Burst
                     */
                    /** Get Max burst and Max BandWidth for SEs and connectors*/
                    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get
                                    (unit, gport, cos, &kbits_sec_min, &kbits_sec_max, &flags2));
                    SHR_IF_ERR_EXIT(bcm_cosq_control_get
                                    (unit, gport, cos, bcmCosqControlBandwidthBurstMax, &max_burst));

                    if (kbits_sec_max == -1)
                    {
                        PRT_CELL_SET("No Limit");
                    }
                    else
                    {
                        PRT_CELL_SET("%u", kbits_sec_max);
                    }
                    PRT_CELL_SET("%d", max_burst);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t sh_dnx_scheduler_gport_options[] = {
/* *INDENT-OFF* */
    /*name          type                    desc                                            default   ext*/
    {"count",       SAL_FIELD_TYPE_BOOL,    "Show counter summary for all gport types",     "false"},
    {"verbose",     SAL_FIELD_TYPE_BOOL,    "Switch to print more detailed info",           "false"},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_gport_man = {
    .brief = "Print all gport info",
    .full = "Diagnostic for displaying information\n"
        "about all scheduler gports. \n"
        "Options:\n"
        "  CouNT - A summary counter information will be provided showing number of:\n"
        "    connectors\n"
        "    unicast queues\n"
        "    multicast queues\n"
        "    cl elements\n"
        "    fq elements\n"
        "    hr elements\n" "VERBose - displays additional information\n" "    per COS for each FLOW\n",
    .synopsis = "[count] [verbose]",
    .examples = "cnt\n" "verb\n"
};

/**
 * \brief - Print Scheduler Gports info.
 */
shr_error_e
sh_dnx_scheduler_gport_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 count_is_provided, verbose_is_provided;
    diag_dnx_gport_count_t gport_count;
    int core;
    diag_dnx_gport_show_flows_params_t show_flows_pass_params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&gport_count, 0, sizeof(diag_dnx_gport_count_t));

    /** get parameters */
    SH_SAND_GET_BOOL("count", count_is_provided);
    SH_SAND_GET_BOOL("verbose", verbose_is_provided);

    if (verbose_is_provided && count_is_provided)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Passing 'count' together with 'verbose' is not allowed!");
    }

    if (count_is_provided)
    {
        /*
         * Gport count display
         */
        PRT_TITLE_SET("Gports count");
        PRT_COLUMN_ADD("Type");
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Core %d count", core);
        }
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Core All count");

        SHR_IF_ERR_EXIT(bcm_cosq_gport_traverse(unit, dnx_cosq_gport_count_all_cb, &gport_count));

        DIAG_DNX_GPORT_COUNT_CORE_DEPENDENT_PRINT(unit, "VOQ Connector bundles", gport_count.nof_conn);

        DIAG_DNX_GPORT_COUNT_NOT_CORE_DEPENDENT_PRINT(unit, "UC queue bundles", gport_count.nof_uc_qs);

        DIAG_DNX_GPORT_COUNT_NOT_CORE_DEPENDENT_PRINT(unit, "MC queue bundles", gport_count.nof_mc_qs);

        DIAG_DNX_GPORT_COUNT_CORE_DEPENDENT_PRINT(unit, "CL elements ", gport_count.nof_cl_ses);

        DIAG_DNX_GPORT_COUNT_CORE_DEPENDENT_PRINT(unit, "FQ elements", gport_count.nof_fq_ses);

        DIAG_DNX_GPORT_COUNT_CORE_DEPENDENT_PRINT(unit, "HR elements", gport_count.nof_hr_ses);

        PRT_COMMITX;
    }
    else
    {
        /*
         * Gport show general info display
         */
        PRT_TITLE_SET("VOQs gport info");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "TM port");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "NOF COS");
        PRT_COLUMN_ADD("Type");
        PRT_COLUMN_ADD("Modid");
        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base VOQ");

        /*
         * The structure of the table is in this function
         * But the rows of the table are populated in the
         * cosq_gport_traverse CB function.
         * In order for this to be possible some shell framework
         * controlls should be passed on also to the CB function
         * like pointer of the printing control
         */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_traverse(unit, dnx_cosq_gport_show_all_voqs_cb, prt_ctr));

        PRT_COMMITX;

        PRT_TITLE_SET("Flows gport info");
        PRT_COLUMN_ADD("Gport");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "NOF COS");
        PRT_COLUMN_ADD("Type");
        PRT_COLUMN_ADD("Core");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Flow ID");
        if (verbose_is_provided)
        {
            PRT_COLUMN_ADD("COS");
            PRT_COLUMN_ADD("Mode");
            PRT_COLUMN_ADD("Weight");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MAX bandwidth[Kbps]");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MAX burst");
        }

        show_flows_pass_params.prt_ctr = prt_ctr;
        show_flows_pass_params.verbose_is_provided = verbose_is_provided;

        SHR_IF_ERR_EXIT(bcm_cosq_gport_traverse(unit, dnx_cosq_gport_show_all_flows_cb, &show_flows_pass_params));

        PRT_COMMITX;

    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * --------------------------------------------------------------------------
 * dnx print flow and up diag
 * --------------------------------------------------------------------------
 */

sh_sand_option_t sh_dnx_scheduler_print_flow_and_up_options[] = {
/* *INDENT-OFF* */
    /*name          type                    desc                            default   ext      range,    flags*/
    {"id",          SAL_FIELD_TYPE_INT32,   "Flow_id or System port",        "-1"},
    {"is_flow",     SAL_FIELD_TYPE_BOOL,    "Indicates if ID is flow",      "false"},
    {"print_status",SAL_FIELD_TYPE_BOOL,    "Prints Flow rate",             "false"},
    /** dest is used to support legacy command format */
    {"destination", SAL_FIELD_TYPE_INT32,   "Flow_id or System port, legacy", "-1",   NULL,    NULL,     SH_SAND_ARG_QUIET},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_print_flow_and_up_man = {
    .brief = "Print scheduling scheme for flow or system port",
    .full = "Diagnostic to describe the scheduling\n"
        "scheme of a provided flow or system port.\n"
        "The diag first print info for the flow or \n"
        "system port provided.\n"
        "Than goes up the scheduling schem by \n"
        "connecting each flow to its parents\n"
        "Filters and Options:"
        "\t IS_FLOW - specifies if given ID is \n"
        "flow or system port \n"
        "\t PRinT_Status - gives additional info \n"
        "for actual credit rate and port flow control status.\n"
        "These statistics are counted for the period of 1 second. \n" "Also provides current flow slow status",
    .synopsis = "[core=<core_id>] [id=<dest_id>] [is_flow] [print_status]",
    .examples = "id=32 is_flow \n" "id=145 is_flow core=0 \n" "id=15 print_status"
};

/**
 * \brief
 * Structure to collect the parents info
 */
typedef struct
{
    /**
     * Nof of parents of the current flow
     */
    int nof_parents;
    /**
     * Parent flow id. If flow has more than one parent all parents
     * can be accessed by the array index
     */
    int parent_flow_id[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
    /**
     * Parent gport created by parent_flow_id and the core . If flow
     * has more than one parent all parents info can be accessed by
     * the array index
     */
    bcm_gport_t parent_gport[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
    /**
     * Parent gport type - VOQ_CON, FQ, CL or HR. If flow has more
     * than one parent all parents info can be accessed by the array
     * index
     */
    dnx_sch_element_se_type_e parent_gport_type[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
    /**
     * Child subflow of current parent index
     */
    int child_subflow[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
    /**
     * Child subflow gport of current parent index. Gport is created
     * with current child_subflow and core.
     */
    bcm_gport_t child_subflow_gport[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
} diag_dnx_tm_scheduler_flow_parents_info_t;

/**
 * \brief
 * Function to remap the enum elements  from
 * dnx_sch_element_se_type_e to strings
 */
const char *
diag_dnx_scheduler_type_string_get(
    dnx_sch_element_se_type_e key)
{
    char *se_types_string[] = {
        "VOQ Connector",
        "HR",
        "CL",
        "FQ"
    };

    return se_types_string[key];
}
/**
 * \brief
 * Function to remap the enum elements  from
 * bcm_cosq_scheduler_gport_type_t to strings
 */
const char *
diag_dnx_scheduler_se_type_string_get(
    bcm_cosq_scheduler_gport_type_t key)
{
    char *se_type_string[] = {
        "4SP",  /* CL mode 1 -- 4 strict priorities */
        "3SP, last WFQ",        /* CL mode 2 -- 3 strict priorities, last WFQ(2) */
        "2SP, first WFQ",       /* CL mode 3 -- 2 strict priorities, first WFQ(3) or WFQ(1:63) */
        "2SP, last WFQ",        /* CL mode 4 -- 2 strict priorities, last WFQ(3) or WFQ(1:63) */
        "1SP, WFQ",     /* CL mode 5 -- 1 strict priority WFQ(4) or WFQ(1:253) */
        "Single WFQ",   /* HR with single WFQ */
        "Dual WFQ",     /* HR with Dual WFQ */
        "Enhanced priority WFQ",        /* Enhanced HR */
        "FQ"
    };

    return se_type_string[key];
}

/**
 * \brief
 * Function to remap the enum elements  from
 * bcm_cosq_scheduler_gport_cl_wfq_mode_t to strings
 */
const char *
diag_dnx_scheduler_cl_wfq_mode_string_get(
    bcm_cosq_scheduler_gport_cl_wfq_mode_t key)
{
    char *cl_wfq_mode_string[] = {
        "None",
        "Discrete",
        "Class",
        "Independant",
        "Independent Proportional"
    };

    return cl_wfq_mode_string[key];
}

/**
 * \brief
 * Function to remap the registry value received from
 * dnx_sch_flow_slow_status_get to strings
 */
const char *
diag_dnx_scheduler_slow_status_string_get(
    dbal_enum_value_field_flow_slow_status_e slow_status_index)
{
    char *slow_status_string[] = {
        "OFF",
        "SLOW",
        "NORMAL"
    };

    return slow_status_string[slow_status_index];
}

/**
 * \brief - Return the actual credit rate of the current flow
 */
static shr_error_e
sh_dnx_scheduler_flow_actual_credit_rate_get(
    int unit,
    int core,
    int flow_id,
    uint32 *credit_rate_kbps)
{
    uint32 credit_count;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;

    SHR_FUNC_INIT_VARS(unit);

    /** Select Flow id which should be tracked */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_counter_configuration_set(unit, core, flow_id));
    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_SCH, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_SCH, core));

    /** Get actual Credit count after 1 sec */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_counter_info_get(unit, core, &credit_count));

    /** Convert to Kbps */
    /** In order to avoid overflow the order of the calculation must be kept*/
    *credit_rate_kbps = credit_count * (UTILEX_NOF_BITS_IN_BYTE * dnx_data_iqs.credit.worth_get(unit) / 1000);

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_SCH, core));

    /** Disable the credit counter filters */
    SHR_IF_ERR_EXIT(dnx_sch_flow_credit_counter_configuration_reset(unit, core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
sh_dnx_scheduler_port_actual_credit_rate_get(
    int unit,
    int core,
    int port_hr,
    uint32 *credit_rate_kbps)
{
    uint32 credit_count;
    uint32 nof_clock_cycles;
    dnxcmn_time_t gtimer_time;

    SHR_FUNC_INIT_VARS(unit);

    /** Select port HR to be tracked */
    SHR_IF_ERR_EXIT(dnx_sch_port_credit_counter_configuration_set(unit, core, port_hr));
    /** Get number of cycles in a second and use it in gtimer.*/
    gtimer_time.time_units = DNXCMN_TIME_UNIT_SEC;
    COMPILER_64_SET(gtimer_time.time, 0, 1);
    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &gtimer_time, &nof_clock_cycles));

    SHR_IF_ERR_EXIT(dnx_gtimer_set(unit, nof_clock_cycles, SOC_BLK_SCH, core));
    SHR_IF_ERR_EXIT(dnx_gtimer_wait(unit, SOC_BLK_SCH, core));

    /** Get actual Credit count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_sch_port_credit_counter_info_get(unit, core, &credit_count));

    /** Convert to Kbps */
    /** In order to avoid overflow the order of the calculation must be kept*/
    *credit_rate_kbps = credit_count * (UTILEX_NOF_BITS_IN_BYTE * dnx_data_iqs.credit.worth_get(unit) / 1000);

    /** Disable the gtimer for the given block*/
    SHR_IF_ERR_EXIT(dnx_gtimer_clear(unit, SOC_BLK_SCH, core));

    /** Disable the credit counter filters */
    SHR_IF_ERR_EXIT(dnx_sch_port_credit_counter_configuration_reset(unit, core));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
sh_dnx_scheduler_port_current_fc_info_get(
    int unit,
    int core,
    int port_hr,
    uint32 *fc_count,
    uint32 *fc_percent)
{
    dnxcmn_time_t timer_info;
    uint32 nof_clock_cycles;
    SHR_FUNC_INIT_VARS(unit);

    /** Select port HR to be tracked */
    SHR_IF_ERR_EXIT(dnx_sch_port_fc_counter_configuration_set(unit, core, port_hr));

    /** Clear counters */
    SHR_IF_ERR_EXIT(dnx_sch_port_fc_counter_info_get(unit, core, fc_count));

    sal_usleep(1000); /** 1000 usec -- 0.001 sec */

    /** Get actual FC count for 1 sec */
    SHR_IF_ERR_EXIT(dnx_sch_port_fc_counter_info_get(unit, core, fc_count));

    *fc_percent = 0;
    if (*fc_count)
    {
        /** get number of cycles in 1000 micro second */
        timer_info.time_units = DNXCMN_TIME_UNIT_USEC;
        COMPILER_64_SET(timer_info.time, 0, 1000);
        SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &timer_info, &nof_clock_cycles));

        *fc_percent = UTILEX_DIV_ROUND_UP((*fc_count * 100), nof_clock_cycles);
        *fc_percent = UTILEX_RANGE_BOUND(*fc_percent, 0, 100);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return the slow status of the current flow
 */
static shr_error_e
sh_dnx_scheduler_slow_slow_status_get(
    int unit,
    int core,
    int flow_id,
    dbal_enum_value_field_flow_slow_status_e * slow_status)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Get slow status of the flow */
    SHR_IF_ERR_EXIT(dnx_sch_flow_slow_status_get(unit, core, flow_id, slow_status));

    if (*slow_status >= DBAL_NOF_ENUM_FLOW_SLOW_STATUS_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "slow_status %d is bigger than MAX allowed value\n", *slow_status);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief Function to return if a scheduler element is
 *        composite, the nof of its subflows and their IDs.
 *
 * \param unit
 * \param core
 * \param flow_gport
 * \param is_composite
 * \param flow
 *
 * \return shr_error_e
 */
static shr_error_e
dnx_diag_scheduler_element_subflows_get(
    int unit,
    int core,
    bcm_gport_t flow_gport,
    int *is_composite,
    int flow[2])
{
    dnx_scheduler_region_odd_even_mode_t odd_even_mode;
    int flow_region, base_flow_id, num_cos_levels, dummy_core;
    dnx_sch_region_type_e region_type;
    bcm_gport_t mod_port;
    uint32 flags = 0;
    dnx_sch_element_se_type_e se_type;
    bcm_cosq_scheduler_gport_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    /** Find gport Flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, flow_gport, 0, &dummy_core, &base_flow_id));

    /** Get scheduling Element type */
    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, base_flow_id, &se_type));

    if (se_type == DNX_SCH_ELEMENT_SE_TYPE_NONE)
    {
       /** VOQ connector handle */
       /** Get gport flags to determine if flow is COMPOSITE */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_get(unit, flow_gport, &mod_port, &num_cos_levels, &flags));
        *is_composite = flags & BCM_COSQ_GPORT_COMPOSITE ? TRUE : FALSE;
    }
    else
    {
       /** SE handle */
        SHR_IF_ERR_EXIT(bcm_cosq_scheduler_gport_get(unit, flags, flow_gport, &params));
        *is_composite = (params.mode == bcmCosqSchedulerGportModeComposite) ? TRUE : FALSE;
    }

    flow[0] = base_flow_id;

    if (*is_composite)
    {
        /** Get flow region and check if region is in odd or in even
         *  mode */
        flow_region = DNX_SCH_REGION_INDEX_FROM_FLOW_INDEX_GET(base_flow_id);
        region_type = dnx_data_sch.flow.region_type_get(unit, core, flow_region)->type;
        odd_even_mode = dnx_data_sch.sch_alloc.region_get(unit, region_type)->odd_even_mode;

        if (odd_even_mode == DNX_SCHEDULER_REGION_ODD)
        {
            flow[1] = base_flow_id + 1;
        }
        else
        {
            flow[1] = base_flow_id + 2;
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Flow and up voq connector info print
 */
static shr_error_e
sh_dnx_scheduler_voq_con_info_print(
    int unit,
    int core,
    bcm_gport_t gport,
    dnx_sch_element_se_type_e se_type,
    sh_sand_control_t * sand_control)
{
    int is_slow_enabled, is_enabled, dummy_core;
    int cos = 0, flow_id;
    int sub_flows[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW], is_composite;
    bcm_cosq_voq_connector_gport_t connector_config;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Find gport Flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, 0, &dummy_core, &flow_id));

    /** Checks if flow is valid for this Core */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id, &is_enabled));
    if (!is_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow %d is not allocated \n", flow_id);
    }

    /** determine if flow is composite */
    SHR_IF_ERR_EXIT(dnx_diag_scheduler_element_subflows_get(unit, core, gport, &is_composite, sub_flows));

    /** Check if Slow is enabled */
    SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, gport, cos, bcmCosqControlFlowSlowRate, &is_slow_enabled));

    SHR_IF_ERR_EXIT(bcm_cosq_voq_connector_gport_get(unit, gport, &connector_config));

    PRT_TITLE_SET("Flow[%d] Info", flow_id);
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Slow enabled");
    PRT_COLUMN_ADD("Low rate");

    PRT_COLUMN_ADD("Composite");
    if (is_composite)
    {
        PRT_COLUMN_ADD("SubFlows");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", diag_dnx_scheduler_type_string_get(se_type));
    PRT_CELL_SET("%s", is_slow_enabled ? "TRUE" : "FALSE");
    PRT_CELL_SET("%s", (connector_config.flags & BCM_COSQ_VOQ_CONNECTOR_GPORT_F_LOW_RATE) ? "TRUE" : "FALSE");
    PRT_CELL_SET("%s", is_composite ? "TRUE" : "FALSE");
    if (is_composite)
    {
        PRT_CELL_SET("%d, %d", sub_flows[0], sub_flows[1]);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Flow and up scheduling element info print
 */
static shr_error_e
sh_dnx_scheduler_se_info_print(
    int unit,
    int core,
    bcm_gport_t gport,
    dnx_sch_element_se_type_e se_type,
    sh_sand_control_t * sand_control)
{
    int is_enabled = 0, is_composite = FALSE, dummy_core, flow_id;
    uint32 flags = 0;
    int sub_flows[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
    bcm_cosq_scheduler_gport_params_t params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Find gport Flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, 0, &dummy_core, &flow_id));

    /** Checks if flow is valid for this Core */
    SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id, &is_enabled));
    if (!is_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flow %d is not allocated \n", flow_id);
    }

    /** determine if flow is composite */
    SHR_IF_ERR_EXIT(dnx_diag_scheduler_element_subflows_get(unit, core, gport, &is_composite, sub_flows));

    /** Get gport params */
    SHR_IF_ERR_EXIT(bcm_cosq_scheduler_gport_get(unit, flags, gport, &params));

    PRT_TITLE_SET("Flow[%d] Info", flow_id);
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Mode");
    if (se_type == DNX_SCH_ELEMENT_SE_TYPE_CL || se_type == DNX_SCH_ELEMENT_SE_TYPE_HR)
    {
        PRT_COLUMN_ADD("SubType");
        if (se_type == DNX_SCH_ELEMENT_SE_TYPE_CL)
        {
            PRT_COLUMN_ADD("WFQ mode");
        }
    }

    PRT_COLUMN_ADD("Composite");
    if (is_composite)
    {
        PRT_COLUMN_ADD("SubFlows");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", diag_dnx_scheduler_type_string_get(se_type));
    PRT_CELL_SET("%s", diag_dnx_scheduler_se_mode_string_get(params.mode));
    if (se_type == DNX_SCH_ELEMENT_SE_TYPE_CL || se_type == DNX_SCH_ELEMENT_SE_TYPE_HR)
    {
        PRT_CELL_SET("%s", diag_dnx_scheduler_se_type_string_get(params.type));
        if (se_type == DNX_SCH_ELEMENT_SE_TYPE_CL)
        {
            PRT_CELL_SET("%s", diag_dnx_scheduler_cl_wfq_mode_string_get(params.cl_wfq_mode));
        }
    }
    PRT_CELL_SET("%s", is_composite ? "TRUE" : "FALSE");
    if (is_composite)
    {
        PRT_CELL_SET("%d, %d", sub_flows[0], sub_flows[1]);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print port HR info. Last stage of the flow_and_up
 *        diag print.
 */
static shr_error_e
sh_dnx_scheduler_port_info_print(
    int unit,
    int core,
    bcm_gport_t gport,
    int print_status_provided,
    sh_sand_control_t * sand_control)
{
    int hr_idx = 0, base_hr, num_priorities;
    int cos = 0, max_burst, group;
    bcm_port_t port;
    bcm_gport_t e2e_tc_gport;
    bcm_cosq_gport_info_t gport_info;
    uint32 flags = 0;
    uint32 fc_count = 0, fc_percent = 0, credit_rate_kbps = 0;
    uint32 kbits_sec_min, kbits_sec_max;
    bcm_cosq_scheduler_gport_params_t params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    port = BCM_COSQ_GPORT_E2E_PORT_GET(gport);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, port, &num_priorities));

    PRT_TITLE_SET("Port[%d] HRs", port);
    PRT_COLUMN_ADD("HR");
    PRT_COLUMN_ADD("HR Flow ID");
    PRT_COLUMN_ADD("Mode");
    PRT_COLUMN_ADD("Rate[Kbps]");
    PRT_COLUMN_ADD("Max Burst");
    PRT_COLUMN_ADD("Color Group");
    if (print_status_provided)
    {
        PRT_COLUMN_ADD("Actual Credit Rate[Kbps]");
        PRT_COLUMN_ADD("Flow Control Status");
    }

    BCM_COSQ_GPORT_E2E_PORT_TC_SET(e2e_tc_gport, port);
    for (hr_idx = 0; hr_idx < num_priorities; hr_idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", base_hr + hr_idx);

        gport_info.in_gport = e2e_tc_gport;
        gport_info.cosq = hr_idx;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info));
        PRT_CELL_SET("%d", BCM_GPORT_SCHEDULER_GET(gport_info.out_gport));

        SHR_IF_ERR_EXIT(bcm_cosq_scheduler_gport_get(unit, flags, gport_info.out_gport, &params));
        PRT_CELL_SET("%s", diag_dnx_scheduler_se_type_string_get(params.type));

        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get
                        (unit, e2e_tc_gport, hr_idx, &kbits_sec_min, &kbits_sec_max, &flags));
        SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, e2e_tc_gport, cos, bcmCosqControlBandwidthBurstMax, &max_burst));
        PRT_CELL_SET("%u", kbits_sec_max);
        PRT_CELL_SET("%d", max_burst);
        SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, e2e_tc_gport, cos, bcmCosqControlGroup, &group));
        PRT_CELL_SET("%d", group);

        if (print_status_provided)
        {
            SHR_IF_ERR_EXIT(sh_dnx_scheduler_port_actual_credit_rate_get
                            (unit, core, base_hr + hr_idx, &credit_rate_kbps));
            SHR_IF_ERR_EXIT(sh_dnx_scheduler_port_current_fc_info_get
                            (unit, core, base_hr + hr_idx, &fc_count, &fc_percent));
            PRT_CELL_SET("%u", credit_rate_kbps);
            PRT_CELL_SET("%u FC events. (Under FC for %d%% of the time)", fc_count, fc_percent);
        }
    }

    PRT_COMMITX;

    PRT_TITLE_SET("Port Info");
    PRT_COLUMN_ADD("Scheduler Rate[Kbps]");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    /** Get E2E gport max rate */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, gport, cos, &kbits_sec_min, &kbits_sec_max, &flags));
    PRT_CELL_SET("%u", kbits_sec_max);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print FMQ HR info. Last stage of the flow_and_up
 *        diag print.
 */
static shr_error_e
sh_dnx_scheduler_fmq_info_print(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{

    int fmq_class, fmq_flow_id, se_id, hr_id;
    bcm_gport_t fmq_hr_gport;
    bcm_cosq_scheduler_gport_params_t params;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FMQ HRS");
    PRT_COLUMN_ADD("HR");
    PRT_COLUMN_ADD("HR Flow ID");
    PRT_COLUMN_ADD("Mode");
    PRT_COLUMN_ADD("FMQ Class");

    for (fmq_class = 0; fmq_class < dnx_data_sch.general.nof_fmq_class_get(unit); fmq_class++)
    {
        /**
         * Get FMQ HR id for the given class and core
         */
        SHR_IF_ERR_EXIT(dnx_scheduler_db.fmq.fmq_flow_id.get(unit, core, fmq_class, &fmq_flow_id));
        BCM_GPORT_SCHEDULER_CORE_SET(fmq_hr_gport, fmq_flow_id, core);

        SHR_IF_ERR_EXIT(dnx_sch_convert_flow_to_se_id(unit, fmq_flow_id, &se_id));
        hr_id = DNX_SCHEDULER_ELEMENT_HR_ID_GET(unit, se_id);

        SHR_IF_ERR_EXIT(bcm_cosq_scheduler_gport_get(unit, 0, fmq_hr_gport, &params));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", hr_id);
        /**
         * FMQ CLASSes are 1-based
         */
        PRT_CELL_SET("%d", fmq_flow_id);
        PRT_CELL_SET("%s", diag_dnx_scheduler_se_type_string_get(params.type));
        PRT_CELL_SET("%d", fmq_class);

    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dispatch function for printing flow info
 */
static shr_error_e
sh_dnx_scheduler_flow_and_up_flow_info_print(
    int unit,
    int core,
    bcm_gport_t gport,
    sh_sand_control_t * sand_control)
{
    dnx_sch_element_se_type_e se_type;
    int dummy_core = 0, flow_id;
    SHR_FUNC_INIT_VARS(unit);

    /** Find gport Flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, gport, 0, &dummy_core, &flow_id));

    /** Get scheduling Element type */
    SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get(unit, core, flow_id, &se_type));

    if (se_type == DNX_SCH_ELEMENT_SE_TYPE_NONE)
    {
        /** VOQ connector print */
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_voq_con_info_print(unit, core, gport, se_type, sand_control));
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_se_info_print(unit, core, gport, se_type, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to retrieve tha parent info information of
 *        a certaing flow provided and populate the parent info
 *        structure
 */
static shr_error_e
sh_dnx_scheduler_flow_and_up_flow_parents_get(
    int unit,
    int core,
    bcm_gport_t gport,
    diag_dnx_tm_scheduler_flow_parents_info_t * parents_info)
{
    bcm_gport_t subflow_gport;
    int subflow_idx, is_composite = FALSE, nof_subflows;
    bcm_cos_queue_t cos = 0;
    int sub_flows[DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW];
    int dummy_core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_diag_scheduler_element_subflows_get(unit, core, gport, &is_composite, sub_flows));

    nof_subflows = is_composite ? 2 : 1;
    for (subflow_idx = 0; subflow_idx < nof_subflows; subflow_idx++)
    {
        /** Increment nof of parents */
        parents_info->nof_parents++;

        /** Save the child subflow ID */
        parents_info->child_subflow[subflow_idx] = sub_flows[subflow_idx];

        /** Create gport from the sub flow_id */
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_gport_get(unit, core, sub_flows[subflow_idx], &subflow_gport));

        /** Save also the child subflow gport*/
        parents_info->child_subflow_gport[subflow_idx] = subflow_gport;

        /** Find subflow parent gport */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_parent_get(unit, subflow_gport, cos, &parents_info->parent_gport[subflow_idx]));

        /** Find parent Flow id */
        SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get
                        (unit, parents_info->parent_gport[subflow_idx], 0,
                         &dummy_core, &parents_info->parent_flow_id[subflow_idx]));

        /** Find what SE type is the parent */
        SHR_IF_ERR_EXIT(dnx_sch_flow_se_type_get
                        (unit, core, parents_info->parent_flow_id[subflow_idx],
                         &parents_info->parent_gport_type[subflow_idx]));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to print the info for each subflow and its
 *        related credit source
 */
static shr_error_e
sh_dnx_scheduler_flow_and_up_flow_credit_srcs_print(
    int unit,
    int core,
    bcm_gport_t child_gport,
    int print_status_provided,
    diag_dnx_tm_scheduler_flow_parents_info_t parents_info,
    int *nof_flows_to_print_next_level,
    sh_sand_control_t * sand_control)
{
    int credit_src_idx;
    int child_flow_id;
    int mode, weight, max_burst, group, dummy_core;
    int is_fabric, is_high_prio;
    bcm_cos_queue_t cos = 0;
    uint32 flags = 0, credit_rate_kbps = 0;
    uint32 kbits_sec_min = 0, kbits_sec_max = 0, slow_status = 0;
    char mode_str[DIAG_DNX_SCH_GPORT_MODE_STR_LENGTH];
    char group_str[DIAG_DNX_SCH_GROUP_STR_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Find gport Flow id */
    SHR_IF_ERR_EXIT(dnx_scheduler_gport_core_flow_id_get(unit, child_gport, 0, &dummy_core, &child_flow_id));

    PRT_TITLE_SET("Flow[%d] Properties", child_flow_id);
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("SubFlow");
    /** Add extra columns for Connector gport */
    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(child_gport))
    {
        /** If child subflow is a VOQ connector add more info per
         *  subflow */
        PRT_COLUMN_ADD("Is Fabric");
        PRT_COLUMN_ADD("Priority");
    }
    PRT_COLUMN_ADD("Rate[Kbps]");
    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(child_gport) && print_status_provided)
    {
        /** Slow status printed only for connectors */
        PRT_COLUMN_ADD("Slow status");
        PRT_COLUMN_ADD("Actual Credit Rate[Kbps]");
    }
    PRT_COLUMN_ADD("Burst[Byte]");
    PRT_COLUMN_ADD("Credit Src[Flow ID]");
    PRT_COLUMN_ADD("Mode");
    PRT_COLUMN_ADD("Weight");
    PRT_COLUMN_ADD("Color Group");

    for (credit_src_idx = 0; credit_src_idx < parents_info.nof_parents; credit_src_idx++)
    {
        /** Get child general info to be printed */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get
                        (unit, parents_info.child_subflow_gport[credit_src_idx], cos, &mode, &weight));
        if (parents_info.parent_gport_type[credit_src_idx] == DNX_SCH_ELEMENT_SE_TYPE_CL)
        {
            bcm_cosq_scheduler_gport_params_t params;

            /** Get gport params */
            SHR_IF_ERR_EXIT(bcm_cosq_scheduler_gport_get(unit, 0, parents_info.parent_gport[credit_src_idx], &params));
            if ((params.cl_wfq_mode == bcmCosqSchedulerGportClWfqModeClass ||
                 params.cl_wfq_mode == bcmCosqSchedulerGportClWfqModeDiscrete) &&
                weight >= BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL3 && weight <= BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0)
            {
                /*
                 * The actual weight of discrete CL is configured on CL profile
                 */
                SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, parents_info.parent_gport[credit_src_idx], 0,
                                                     bcmCosqControlDiscreteWeightLevel0 +
                                                     (BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0 - weight), &weight));
            }
        }
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get
                        (unit, parents_info.child_subflow_gport[credit_src_idx], cos, &kbits_sec_min, &kbits_sec_max,
                         &flags));
        SHR_IF_ERR_EXIT(bcm_cosq_control_get
                        (unit, parents_info.child_subflow_gport[credit_src_idx], cos, bcmCosqControlBandwidthBurstMax,
                         &max_burst));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", (*nof_flows_to_print_next_level) + credit_src_idx);
        PRT_CELL_SET("%d", parents_info.child_subflow[credit_src_idx]);
        if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(parents_info.child_subflow_gport[credit_src_idx]))
        {
            /** Check if fabric or local */
            SHR_IF_ERR_EXIT(bcm_cosq_control_get
                            (unit, parents_info.child_subflow_gport[credit_src_idx], cos, bcmCosqControlIsFabric,
                             &is_fabric));
            PRT_CELL_SET("%s", is_fabric ? "TRUE" : "FALSE");

            /** Check if High or Low priority */
            SHR_IF_ERR_EXIT(bcm_cosq_control_get
                            (unit, parents_info.child_subflow_gport[credit_src_idx], cos, bcmCosqControlIsHighPriority,
                             &is_high_prio));
            PRT_CELL_SET("%s", is_high_prio ? "HIGH" : "LOW");
        }

        if (kbits_sec_max == -1)
        {
            PRT_CELL_SET("No Limit");
        }
        else
        {
            PRT_CELL_SET("%d", kbits_sec_max);
        }

        if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(child_gport) && print_status_provided)
        {

            /** Slow status printed only for connectors */
            SHR_IF_ERR_EXIT(sh_dnx_scheduler_slow_slow_status_get
                            (unit, core, parents_info.child_subflow[credit_src_idx], &slow_status));
            PRT_CELL_SET("%s", diag_dnx_scheduler_slow_status_string_get(slow_status));
            SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_actual_credit_rate_get
                            (unit, core, parents_info.child_subflow[credit_src_idx], &credit_rate_kbps));
            PRT_CELL_SET("%u", credit_rate_kbps);

        }

        PRT_CELL_SET("%d", max_burst);
        PRT_CELL_SET("%s[%d]",
                     diag_dnx_scheduler_type_string_get(parents_info.parent_gport_type[credit_src_idx]),
                     parents_info.parent_flow_id[credit_src_idx]);

        sal_snprintf(mode_str, DIAG_DNX_SCH_GPORT_MODE_STR_LENGTH, "BCM_COSQ_SP_%d", mode - BCM_COSQ_SP0);
        PRT_CELL_SET("%s", mode_str);
        PRT_CELL_SET("%d", weight);

        if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(parents_info.child_subflow_gport[credit_src_idx]))
        {
            sal_snprintf(group_str, DIAG_DNX_SCH_GROUP_STR_LENGTH, "N/A");
        }
        else
        {
            /** get color group for Element */
            SHR_IF_ERR_EXIT(bcm_cosq_control_get
                            (unit, parents_info.child_subflow_gport[credit_src_idx], cos, bcmCosqControlGroup, &group));
            sal_snprintf(group_str, DIAG_DNX_SCH_GROUP_STR_LENGTH, "%d", group);
        }
        PRT_CELL_SET("%s", group_str);
    }

    *nof_flows_to_print_next_level += parents_info.nof_parents;

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Function to print the flow info tables and to get
 *        its parents
 */
static shr_error_e
sh_dnx_scheduler_flow_and_up_flow_print(
    int unit,
    int core,
    int print_status_provided,
    bcm_gport_t gport,
    diag_dnx_tm_scheduler_flow_parents_info_t * parents_info,
    int *nof_flows_to_print_next_level,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Print flow parent info */
    SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_and_up_flow_info_print(unit, core, gport, sand_control));

    /** Get flow credit sources info */
    SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_and_up_flow_parents_get(unit, core, gport, parents_info));

    /** Print general info for the flow credit sources */
    SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_and_up_flow_credit_srcs_print(unit, core, gport, print_status_provided,
                                                                        *parents_info, nof_flows_to_print_next_level,
                                                                        sand_control));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Print flow and up.
 */
shr_error_e
sh_dnx_scheduler_print_flow_and_up_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_id_present, is_dest_present, dest_id, is_flow, print_status_provided;
    int flow_id;
    int core, core_id, core_id_filter, scheduler_level, voq_id;
    int flow_idx, parent_flow_idx;
    diag_dnx_tm_scheduler_flow_parents_info_t *parents_info_array = NULL;
    diag_dnx_tm_scheduler_flow_parents_info_t *previous_level_parents_info_array = NULL;
    int nof_flows_previous_level, nof_parents_to_print;
    int is_allocated, nof_flows_to_print_next_level = 0, nof_flows_this_level;
    bcm_gport_t parent_gport, gport;
    int sys_port, modid, flow_is_port_hr, local_core;
    bcm_gport_t sys_gport, mod_gport;
    bcm_gport_t voq_gport;
    bcm_cosq_gport_connection_t gport_connect;
    int queue_id;
    int fap_id_index;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("id", is_id_present);
    SH_SAND_IS_PRESENT("destination", is_dest_present);
    if (is_id_present)
    {
        /** get DNX destination id */
        SH_SAND_GET_INT32("id", dest_id);
    }
    else if (is_dest_present)
    {
        /** get destination for legacy command support */
        SH_SAND_GET_INT32("destination", dest_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Presence of option:'ID' is requested\n");
    }

    /** get parameters */
    SH_SAND_GET_INT32("core", core_id);
    SH_SAND_GET_BOOL("is_flow", is_flow);
    SH_SAND_GET_BOOL("print_status", print_status_provided);

    /** If system port is provided MAP the system port to a VOQ
     *  connector and than continue */
    if (!is_flow)
    {
        sys_port = dest_id;
        /** Create sysport gport */
        BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, sys_port);

        /** Check if system port is Local */
        SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_get(unit, sys_gport, &mod_gport));
        modid = BCM_GPORT_MODPORT_MODID_GET(mod_gport);
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_local_core_get(unit, modid, &local_core, &fap_id_index));
        if (local_core == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Provided system port %d should be local! \n", sys_port);
        }

        /** Get relevant VOQ gport for the provided sys port */
        SHR_IF_ERR_EXIT(bcm_cosq_sysport_ingress_queue_map_get(unit, 0, sys_gport, &voq_gport));

        /** set VOQ gport to contain the proper core */
        queue_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport);
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, local_core, queue_id);

        /** Get the VOQ connector connected to this VOQ so the
         *  scheduler scheme can start with it */
        gport_connect.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(voq_gport);
        gport_connect.voq = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, local_core, voq_id);

        /** Set VOQ_connector gport core to be local core before calling
         *  to bcm_cosq_gport_connection_get. */ ;
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(gport_connect.voq_connector, 0, local_core);
        SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_get(unit, &gport_connect));
        flow_id = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect.voq_connector);
        core_id_filter = local_core;
    }
    else
    {
        flow_id = dest_id;
        core_id_filter = core_id;
    }

    DNXCMN_CORES_ITER(unit, core_id_filter, core)
    {

        cli_out("\n");
        cli_out("***********************\n");
        cli_out("***      Core %d     ***\n", core);
        cli_out("***********************\n");

        /** Checks if flow is valid for this Core */
        SHR_IF_ERR_EXIT(dnx_scheduler_flow_is_allocated(unit, core, flow_id, &is_allocated));
        if (!is_allocated)
        {
            cli_out("Flow %d is not allocated on core %d! \n", flow_id, core);
            continue;
        }

        /*
         * Encode FLOW to GPORT
         */
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_gport_get(unit, core, flow_id, &gport));

        /** If Flow provided is a Port HR print directly last stage
         *  and EXIT */
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_is_port_hr_get(unit, core, gport, &parent_gport, &flow_is_port_hr));
        if (flow_is_port_hr)
        {
            /** Last stage of the Flow and UP diag printout */
            if (BCM_COSQ_GPORT_IS_FMQ_CLASS(parent_gport))
            {
                SHR_IF_ERR_EXIT(sh_dnx_scheduler_fmq_info_print(unit, core, sand_control));
            }
            else
            {
                SHR_IF_ERR_EXIT(sh_dnx_scheduler_port_info_print(unit, core, parent_gport, print_status_provided,
                                                                 sand_control));
            }
            continue;
        }

        /** ALLOC and Set to zero the Parents Info structure for the
         *  flow ID provided */
        SHR_ALLOC_SET_ZERO(parents_info_array,
                           sizeof(diag_dnx_tm_scheduler_flow_parents_info_t),
                           "sh_dnx_scheduler_print_flow_and_up_cmd.diag_dnx_tm_scheduler_flow_parents_info_t",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /** Print Flow info
         *  and
         *  Get Flow Credit sources Info */
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_and_up_flow_print(unit, core, print_status_provided, gport,
                                                                parents_info_array, &nof_flows_to_print_next_level,
                                                                sand_control));

        /*
         * Prepare the structures for the UP printing
         */
        /** Giving that the user provides only 1 flow and iteration is
         *  per CORE on LEVEL0 NOF flows printed are ALWAYS 1  */
        nof_flows_previous_level = 1;

        /** Copy the PARENTs info structures */
        SHR_ALLOC_SET_ZERO(previous_level_parents_info_array,
                           sizeof(diag_dnx_tm_scheduler_flow_parents_info_t) * nof_flows_previous_level,
                           "sh_dnx_scheduler_print_flow_and_up_cmd.diag_dnx_tm_scheduler_flow_parents_info_t",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_memcpy(previous_level_parents_info_array, parents_info_array,
                   nof_flows_previous_level * sizeof(diag_dnx_tm_scheduler_flow_parents_info_t));

        /** Allocate and array for the next lavel flows parents info */
        SHR_FREE(parents_info_array);
        SHR_ALLOC_SET_ZERO(parents_info_array,
                           sizeof(diag_dnx_tm_scheduler_flow_parents_info_t) *
                           DIAG_DNX_SCH_FLOW_AND_UP_MAX_NOF_PARENTS_PER_FLOW,
                           "sh_dnx_scheduler_print_flow_and_up_cmd.diag_dnx_tm_scheduler_flow_parents_info_t",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        /** Next Scheduler level is LEVEL 1 */
        scheduler_level = 1;
        do
        {
            cli_out("\n");
            cli_out("Scheduler Level %d\n", scheduler_level);
            cli_out("________________________\n");
            cli_out("\n");

            /** Reset Counters */
            nof_flows_to_print_next_level = 0;
            nof_flows_this_level = 0;
            /** Go over the parent flows of the previous level */
            for (flow_idx = 0; flow_idx < nof_flows_previous_level; flow_idx++)
            {
                nof_parents_to_print = previous_level_parents_info_array[flow_idx].nof_parents;

                for (parent_flow_idx = 0; parent_flow_idx < nof_parents_to_print; parent_flow_idx++)
                {

                    cli_out("Credit source index %d\n", nof_flows_this_level);

                    /**  Look one step ahead and see what will be the parent of this
                     *  flow. If it is an E2E gport this means that current flow to
                     *  be printed is PORT HR and it should be skipped. Instead
                     *  print the whole PORT  */
                    SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_is_port_hr_get
                                    (unit, core,
                                     previous_level_parents_info_array[flow_idx].parent_gport[parent_flow_idx],
                                     &parent_gport, &flow_is_port_hr));

                    /** IF Parent gport is port HR don't print stand alone HR
                     *  element but instead print the whole port with all its
                     *  configured Port HRs */
                    if (flow_is_port_hr)
                    {
                        /** Last stage of the Flow and UP diag printout */

                        if (BCM_COSQ_GPORT_IS_FMQ_CLASS(parent_gport))
                        {
                            SHR_IF_ERR_EXIT(sh_dnx_scheduler_fmq_info_print(unit, core, sand_control));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(sh_dnx_scheduler_port_info_print
                                            (unit, core, parent_gport, print_status_provided, sand_control));
                        }

                        parents_info_array[nof_flows_this_level].nof_parents = 0;
                    }
                    else
                    {

                        /** Print Flow info
                         *  and
                         *  Get Flow Credit sources Info */
                        SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_and_up_flow_print
                                        (unit, core, print_status_provided,
                                         previous_level_parents_info_array[flow_idx].parent_gport[parent_flow_idx],
                                         &parents_info_array[nof_flows_this_level], &nof_flows_to_print_next_level,
                                         sand_control));

                    }
                    /** Increment the NOF flows printed in this Scheduler level */
                    nof_flows_this_level++;
                }
            }

            /*
             * NEXT SCHEDULER LEVEL PREPARATIONS
             * {
             */
            nof_flows_previous_level = nof_flows_this_level;
            /** Copy current level flows parents info because they will
             *  be next level Flows */
            SHR_FREE(previous_level_parents_info_array);
            SHR_ALLOC_SET_ZERO(previous_level_parents_info_array,
                               sizeof(diag_dnx_tm_scheduler_flow_parents_info_t) * nof_flows_previous_level,
                               "sh_dnx_scheduler_print_flow_and_up_cmd.diag_dnx_tm_scheduler_flow_parents_info_t",
                               "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            sal_memcpy(previous_level_parents_info_array, parents_info_array,
                       nof_flows_previous_level * sizeof(diag_dnx_tm_scheduler_flow_parents_info_t));

            /** Create an array of type
             *  diag_dnx_tm_scheduler_flow_parents_info_t for the MAX
             *  NUMBER OF PARENTS for next level.   */
            SHR_FREE(parents_info_array);
            SHR_ALLOC_SET_ZERO(parents_info_array,
                               sizeof(diag_dnx_tm_scheduler_flow_parents_info_t) * nof_flows_to_print_next_level,
                               "sh_dnx_scheduler_print_flow_and_up_cmd.diag_dnx_tm_scheduler_flow_parents_info_t",
                               "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            scheduler_level++;
            /*
             * }
             */

        }
        while (nof_flows_to_print_next_level);

        SHR_FREE(parents_info_array);
        SHR_FREE(previous_level_parents_info_array);
    }

exit:
    SHR_FREE(parents_info_array);
    SHR_FREE(previous_level_parents_info_array);
    SHR_FUNC_EXIT;
}

/*
 * --------------------------------------------------------------------------
 * dnx scheduler port scheme diag
 * --------------------------------------------------------------------------
 */

sh_sand_option_t sh_dnx_scheduler_port_info_options[] = {
/* *INDENT-OFF* */
    /*name        type                    desc                    default*/
    {"port",      SAL_FIELD_TYPE_INT32,   "logical port",         NULL},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_port_info_man = {
    .brief = "Print port scheme",
    .full = "Diagnostic to print the scheduler \n"
        "scheme of a logical port.\n" "Two tables are printed:\n" "\t - Per Traffic Class Group\n",
    "\t - Per Traffic Class\n" "First table is printed only \n" "for ports with 4 or 8 priorities\n ",
    .synopsis = "port=<logical_port>",
    .examples = "port=13"
};
/**
 * \brief - Print scheduler port scheme info.
 */
shr_error_e
sh_dnx_scheduler_port_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t logical_port;
    int base_hr, tc_idx, tcg_idx, num_priorities;
    bcm_cosq_gport_info_t gport_info;
    bcm_gport_t local_gport, interface_gport, e2e_gport;
    uint32 min_kbps, max_kbps;
    uint32 flags;
    int eir_weight_enabled, weight, tcg, max_burst;
    int tc_to_tcg[DNX_DATA_MAX_SCH_PS_MAX_NOF_TCG] = { 0 };
    char tcg_to_tc_str_buf[DIAG_DNX_SCH_PORT_SCHEME_TCG_TO_TC_STR_LENGTH];
    char tcg_to_tc_str[DIAG_DNX_SCH_PORT_SCHEME_TCG_TO_TC_STR_LENGTH];
    int priority_propagation_enable;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", logical_port);

    /*
     * Port INFO
     * {
     */

    PRT_TITLE_SET("Port %d info", logical_port);

    PRT_COLUMN_ADD("Overall Interface Rate[Kbps]");
    PRT_COLUMN_ADD("Overall Port Rate[Kbps]");
    PRT_COLUMN_ADD("Priority Propagation Enable");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    BCM_COSQ_GPORT_E2E_INTERFACE_SET(interface_gport, logical_port);
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, interface_gport, 0, &min_kbps, &max_kbps, &flags));
    PRT_CELL_SET("%u", max_kbps);

    /** Map to the LOCAL gport and collect EGQ info   */
    BCM_GPORT_LOCAL_SET(local_gport, logical_port);
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_gport, logical_port);
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, e2e_gport, 0, &min_kbps, &max_kbps, &flags));

    PRT_CELL_SET("%u", max_kbps);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priority_propagation_enable_get(unit, logical_port,
                                                                      &priority_propagation_enable));

    PRT_CELL_SET("%s", priority_propagation_enable ? "TRUE" : "FALSE");
    PRT_COMMITX;

    /*
     * }
     */
    gport_info.in_gport = local_gport;

    /** Get port base HR and NOF of priorities/HRs */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &num_priorities));

    /*
     * TCG INFO
     * {
     */
    /** For ports with 4 and 8 priorities print also TCG to TC
     *  mapping */
    if (priority_propagation_enable == FALSE && num_priorities >= dnx_data_sch.ps.min_priority_for_tcg_get(unit))
    {
        for (tc_idx = 0; tc_idx < num_priorities; tc_idx++)
        {
            /** Get current TC gport */
            SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &gport_info));

            /** Find to which TCG the current TC is assigned to */
            SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, gport_info.out_gport, tc_idx, &tcg, &weight));
            tc_to_tcg[tc_idx] = tcg - BCM_COSQ_SP0;
        }

        PRT_TITLE_SET("Port %d scheduling scheme", logical_port);
        PRT_INFO_ADD("Port %d - TCG Info", logical_port);

        PRT_COLUMN_ADD("TCG");
        PRT_COLUMN_ADD("CIR Rate[Kbps]");
        PRT_COLUMN_ADD("Max Burst[Bytes]");
        PRT_COLUMN_ADD("EIR Weight");
        PRT_COLUMN_ADD("TCs");

        for (tcg_idx = 0; tcg_idx < num_priorities; tcg_idx++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", tcg_idx);

            SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTCG, &gport_info));

            /** Print current TCG CIR bandwidth in kbps */
            SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get
                            (unit, gport_info.out_gport, tcg_idx, &min_kbps, &max_kbps, &flags));
            PRT_CELL_SET("%u", max_kbps);

            SHR_IF_ERR_EXIT(bcm_cosq_control_get
                            (unit, gport_info.out_gport, 0, bcmCosqControlBandwidthBurstMax, &max_burst));
            PRT_CELL_SET("%d", max_burst);

            /** Print current TCG EIR wight */
            SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get
                            (unit, gport_info.out_gport, tcg_idx, &eir_weight_enabled, &weight));
            if (eir_weight_enabled)
            {
                PRT_CELL_SET("%d", weight);

            }
            else
            {
                PRT_CELL_SET("Disabled");
            }
            /** Find out which priorities are mapped to this TCG and
             *  create a string out of them to be printed in the end */
            sal_memset(tcg_to_tc_str, '\0', sizeof(tcg_to_tc_str));
            for (tc_idx = 0; tc_idx < num_priorities; tc_idx++)
            {
                if (tc_to_tcg[tc_idx] == tcg_idx)
                {
                    sal_snprintf(tcg_to_tc_str_buf, 3, "%d ", tc_idx);
                    sal_strncat(tcg_to_tc_str, tcg_to_tc_str_buf, 3);
                }
            }
            PRT_CELL_SET("%s", tcg_to_tc_str);
        }
        PRT_COMMITX;
    }
    /*
     * }
     */

    /*
     * TC INFO
     * {
     */
    PRT_TITLE_SET("Port %d scheduling scheme", logical_port);
    PRT_INFO_ADD("Port %d - TC Info", logical_port);

    PRT_COLUMN_ADD("TC");
    PRT_COLUMN_ADD("TC Rate[Kbps]");
    PRT_COLUMN_ADD("Max Burst[Bytes]");

    for (tc_idx = 0; tc_idx < num_priorities; tc_idx++)
    {
        /** Get current TC gport */
        gport_info.cosq = tc_idx;
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &gport_info));

        /** Get current TC gport bandwidth */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_get(unit, gport_info.out_gport, tc_idx, &min_kbps, &max_kbps, &flags));

        SHR_IF_ERR_EXIT(bcm_cosq_control_get
                        (unit, gport_info.out_gport, 0, bcmCosqControlBandwidthBurstMax, &max_burst));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%d", tc_idx);
        PRT_CELL_SET("%u", max_kbps);
        PRT_CELL_SET("%d", max_burst);

    }

    PRT_COMMITX;
    /*
     * }
     */
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * --------------------------------------------------------------------------
 * dnx scheduler rate calculation diag
 * --------------------------------------------------------------------------
 */

sh_sand_option_t sh_dnx_scheduler_port_rate_options[] = {
/* *INDENT-OFF* */
    /*name        type                    desc                    default*/
    {"port",      SAL_FIELD_TYPE_INT32,   "logical port",         NULL},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_port_rate_man = {
    .brief = "Calculate and display scheduler port rate",
    .full = "Calculate and display scheduler port rate \n"
        "scheme of a logical port.\n" "credit rate and FC count are displayed:\n"
        "\t - Per HR and tatol on this port\n",
    .synopsis = "port=<logical_port>",
    .examples = "port=13"
};
/**
 * \brief - Calculate and display scheduler port rate.
 */
shr_error_e
sh_dnx_scheduler_port_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    bcm_port_t logical_port;
    int hr_idx, base_hr, num_priorities;
    uint32 fc_count = 0, fc_percent = 0, credit_rate_kbps = 0, credit_rate_total = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", logical_port);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &num_priorities));

    PRT_TITLE_SET("Port %d Rate", logical_port);

    PRT_COLUMN_ADD("HR_ID");
    PRT_COLUMN_ADD("Rate[Kbps]");
    PRT_COLUMN_ADD("FlowControl");

    for (hr_idx = 0; hr_idx < num_priorities; hr_idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_port_actual_credit_rate_get(unit, core, base_hr + hr_idx, &credit_rate_kbps));
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_port_current_fc_info_get
                        (unit, core, base_hr + hr_idx, &fc_count, &fc_percent));
        PRT_CELL_SET("%d", base_hr + hr_idx);
        PRT_CELL_SET("%u", credit_rate_kbps);
        PRT_CELL_SET("%u, Under FC for %d%% of the time", fc_count, fc_percent);
        credit_rate_total += credit_rate_kbps;
    }
    PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
    PRT_CELL_SET("TOTAL: ");
    PRT_CELL_SET("%u", credit_rate_total);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_option_t sh_dnx_scheduler_flow_rate_options[] = {
/* *INDENT-OFF* */
    /*name        type                    desc                    default*/
    {"FlowId",    SAL_FIELD_TYPE_INT32,   "Flow ID",              NULL},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_flow_rate_man = {
    .brief = "Calculate and display scheduler flow rates",
    .full = "Calculate and display scheduler flow rates \n"
        "scheme of a flow.\n" "credit rate are displayed:\n" "\t - flow rate on all cores\n",
    .synopsis = "FlowId=<Flow ID>",
    .examples = "FlowId=512"
};

/**
 * \brief - Calculate and display scheduler flow rate.
 */
shr_error_e
sh_dnx_scheduler_flow_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    int flow_id;
    uint32 credit_rate_kbps = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("FlowId", flow_id);

    PRT_TITLE_SET("Flow %d rate", flow_id);

    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("FlowId");
    PRT_COLUMN_ADD("Rate[Kbps]");

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        SHR_IF_ERR_EXIT(sh_dnx_scheduler_flow_actual_credit_rate_get(unit, core, flow_id, &credit_rate_kbps));
        PRT_CELL_SET("%d", core);
        PRT_CELL_SET("%d", flow_id);
        PRT_CELL_SET("%u", credit_rate_kbps);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* --------------------------------------------------------------------------
 * dnx scheduler fsm config command
 * --------------------------------------------------------------------------
 */

static sh_sand_enum_t sand_diag_fsm_cnt_type_enum[] = {
/* *INDENT-OFF* */
    /**String    Value*/
    {"FABRIC_OFF",                  DBAL_ENUM_FVAL_COUNT_TYPE_FABRIC_OFF,       "Count type fabric off"},
    {"FABRIC_SLOW",                 DBAL_ENUM_FVAL_COUNT_TYPE_FABRIC_SLOW,      "Count type fabric slow"},
    {"FABRIC_FAST",                 DBAL_ENUM_FVAL_COUNT_TYPE_FABRIC_FAST,      "Count type fabric fast"},
    {"FABRIC_ALL",                  DBAL_NOF_ENUM_COUNT_TYPE_VALUES,            "Default count type"},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_option_t sh_dnx_scheduler_fsm_config_options[] = {
/* *INDENT-OFF* */
    /*name              type                    desc                                           default                   extension*/
    {"ID",              SAL_FIELD_TYPE_INT32,   "count msg from this flow",                    "-1",                     NULL},
    {"TYpe",            SAL_FIELD_TYPE_ENUM,    "msg to count",                                "FABRIC_ALL",             (void *) sand_diag_fsm_cnt_type_enum},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_scheduler_fsm_config_man = {
    .brief = "Config fms counter fabric type and flow ID",
    .full = "Config the fms counter fabric type and flow ID and if it uses filter",
    .synopsis = "[TYpe=fabric_type] [id=flow_id]",
    .examples = "type=FABRIC_OFF\n" "id=0\n"
};

shr_error_e
sh_dnx_scheduler_fsm_config_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    bcm_core_t usr_core;
    uint32 fabric_type;
    int flow_id, id_provided, type_provided;
    int filter_by_flow_id = 0;
    int filter_by_type = 0;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("core", usr_core);
    SH_SAND_GET_ENUM("TYpe", fabric_type);
    SH_SAND_GET_INT32("ID", flow_id);
    SH_SAND_IS_PRESENT("TYpe", type_provided);
    SH_SAND_IS_PRESENT("ID", id_provided);
    /*
     * Check if user set FLOW ID or FABRIC TYPE to enable the filters
     */
    if (id_provided)
    {
        filter_by_flow_id = 1;
    }
    else
    {
        flow_id = 0;
    }

    if (type_provided && !(fabric_type == DBAL_NOF_ENUM_COUNT_TYPE_VALUES))
    {
        filter_by_type = 1;
    }
    else
    {
        fabric_type = 0;
    }

    DNXCMN_CORES_ITER(unit, usr_core, core)
    {
        SHR_IF_ERR_EXIT(dnx_sch_fsm_counter_configuration_fabric_type_set(unit, core, flow_id, fabric_type,
                                                                          filter_by_type, filter_by_flow_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/* --------------------------------------------------------------------------
 * dnx scheduler fsm counter read command
 * --------------------------------------------------------------------------
 */
const char *
diag_dnx_scheduler_fsm_cnt_type_string_get(
    dbal_enum_value_field_count_type_e cnt_type_index)
{
    char *slow_status_string[] = {
        "FABRIC_OFF",
        "FABRIC_SLOW",
        "FABRIC_FAST"
    };

    return slow_status_string[cnt_type_index];
}

sh_sand_man_t sh_dnx_scheduler_fsm_read_man = {
    .brief = "Read fms counter fabric type and flow ID",
    .full = "Read the fms counter fabric type and flow ID and if it uses filter",
    .synopsis = "",
    .examples = ""
};

shr_error_e
sh_dnx_scheduler_fsm_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_core_t core;
    bcm_core_t usr_core;
    uint32 fabric_type, count;
    uint32 flow_id, filter_by_type, filter_by_flow_id, overflow;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("core", usr_core);

    PRT_TITLE_SET("Flow Status Message counter");

    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD("FlowId");
    PRT_COLUMN_ADD("Fabric type");
    PRT_COLUMN_ADD("Count");

    DNXCMN_CORES_ITER(unit, usr_core, core)
    {
        SHR_IF_ERR_EXIT(dnx_sch_fsm_counter_configuration_fabric_type_get(unit, core, &flow_id, &fabric_type,
                                                                          &filter_by_type, &filter_by_flow_id));
        SHR_IF_ERR_EXIT(dnx_sch_fsm_counter_info_get(unit, core, &count, &overflow));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", core);
        if (filter_by_flow_id)
        {
            PRT_CELL_SET("%d", flow_id);
        }
        else
        {
            PRT_CELL_SET("ALL");
        }
        if (filter_by_type)
        {
            PRT_CELL_SET("%s", diag_dnx_scheduler_fsm_cnt_type_string_get(fabric_type));
        }
        else
        {
            PRT_CELL_SET("FABRIC_ALL");
        }
        if (overflow)
        {
            PRT_CELL_SET("%u(ovf)", count);
        }
        else
        {
            PRT_CELL_SET("%u", count);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
