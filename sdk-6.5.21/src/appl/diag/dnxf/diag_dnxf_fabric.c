/** \file diag_dnxf_fabric.c
 * 
 * Soecial diagnostic and printout functions for DNXF devices
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_fabric.h>
#include <appl/diag/dnxf/diag_dnxf_fabric.h>

/*bcm*/
#include <bcm/fabric.h>
/*soc*/
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_cgm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
/* Put your new common defines in this file*/
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/fabric.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

/*
 * LOCAL DEFINEs:
 * {
 */
#define DIAG_DNXF_FABRIC_PROFILE_INVALID -1

#define DIAG_DNXF_THRESHOLDS_CAST_CELL_PRINT(cast)                  \
                do \
                { \
                    if (cast == bcmCastUC) \
                    { \
                        PRT_CELL_SET("UC"); \
                    } \
                    else \
                    { \
                        PRT_CELL_SET("MC"); \
                    } \
                } while (0);


#define DIAG_DNXF_THRESHOLDS_CAST_ROW_SKIP_PRINT(cast)                  \
                do \
                { \
                    if (cast == 1) \
                    { \
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE); \
                    } \
                    else \
                    { \
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE); \
                    } \
                } while (0);
 /*
  * LOCAL DEFINEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */
/**
 * \brief Interface for link CGM  threshold profile ranges
 */
typedef struct
{
    /**
 */
    int range_first_link;
    /**
 */
    int range_last_link;
    /*
 */
    int range_profile;

} diag_dnxf_thresholds_profile_link_ranges_t;

/**
 * \brief Interface for CGM threshold iteration parameters
 */
typedef struct
{
    /**
 */
    int first_pipe;
    /**
 */
    int last_pipe;
    /**
 */
    int first_priority;
    /**
 */
    int last_priority;
    /**
 */
    int first_stage;
    /**
 */
    int last_stage;
} diag_dnxf_thresholds_iteration_params_t;

 /*
  * LOCAL TYPEs:
  * }
  */

/*
 * LOCAL FUNCTIONs:
 * {
 */

/*
* { DNXF DIAG printout function
*/



/*
 * Traffic Profile
 */

/* *INDENT-OFF* */
sh_sand_option_t dnxf_fabric_traffic_options[] = {
    {"destination",      SAL_FIELD_TYPE_INT32,  "Destination ID",  "-1"},
    {"source",           SAL_FIELD_TYPE_INT32,  "Source ID",       "-1"},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
sh_sand_man_t dnxf_fabric_traffic_man = {
    .brief =    "Displays if traffic of a certain cast and priority is passing through the fabric",
    .full  =    "Displays if a certain traffic is currently passing through the fabric. \n"
                "Filters: \n"
                "\t DeSTination - shows the traffic information TO a certain module id \n"
                "\t SouRCe - shows the traffic information FROM a certain module id \n",
    .synopsis = "[destination=<destnation_id>] [source=<source_id>]",
    .examples = "\n"
                "destination=40 \n"
                "dst=40 src=50",
};
/* *INDENT-ON* */

/**
 * \brief - display if there are cells with given cast
 *        and priority sent through the fabric
 */
shr_error_e
cmd_dnxf_fabric_traffic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    soc_dnxf_diag_fabric_traffic_profile_t traffic_profile;
    int destination, source, priority;
    char dest_id_buf[10], source_id_buf[10];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("destination", destination);
    SH_SAND_GET_INT32("source", source);

    /*
     * Check the validity of the inputs if they are passed
     */
    if (source != DIAG_DNXC_OPTION_ALL)
    {
        if (source < 0 || source > DNXF_MAX_MODULES)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid source parameter specified! MODID should be between 0 and %d \n",
                         DNXF_MAX_MODULES);
        }
        sal_snprintf(source_id_buf, 10, "%d", source);
    }

    if (destination != DIAG_DNXC_OPTION_ALL)
    {
        if (destination < 0 || destination > DNXF_MAX_MODULES)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Invalid destination parameter specified! MODID should be between 0 and %d \n",
                         DNXF_MAX_MODULES);
        }
        sal_snprintf(dest_id_buf, 10, "%d", destination);
    }

    /*
     * Get traffic info
     */
    SHR_IF_ERR_EXIT(soc_dnxf_diag_fabric_traffic_profile_get(unit, source, destination, &traffic_profile));

    /*
     * Print traffic info
     */
    PRT_TITLE_SET("Traffic profile information");

    PRT_INFO_ADD("Checked for cells with source_id = %s and dest_id = %s",
                 source != -1 ? source_id_buf : "any", destination != -1 ? dest_id_buf : "any");

    PRT_COLUMN_ADD("Priority");
    PRT_COLUMN_ADD("UC");
    PRT_COLUMN_ADD("MC");
    for (priority = 0; priority < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); priority++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", priority);
        /*
         * Unicast printout
         */
        PRT_CELL_SET("%s", traffic_profile.unicast[priority] ? "Yes" : "No");
        /*
         * Multicast printout
         */
        PRT_CELL_SET("%s", traffic_profile.multicast[priority] ? "Yes" : "No");
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
static shr_error_e
diag_dnxf_thresholds_iteration_parameters_get(
    int unit,
    int pipe_selected,
    int priority_selected,
    int th_stage_selected,
    diag_dnxf_thresholds_iteration_params_t * iteration_parameters)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Set default iteration parameters
     */
    iteration_parameters->first_pipe = 0;
    iteration_parameters->last_pipe = dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1;
    iteration_parameters->first_priority = 0;
    iteration_parameters->last_priority = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit) - 1;
    iteration_parameters->first_stage = 0;
    iteration_parameters->last_stage = soc_dnxf_threshold_stage_nof;

    /*
     * If we have passed an argument we make sure that we will iterate only once over the selected parameter
     */
    if (pipe_selected != DIAG_DNXC_OPTION_ALL)
    {
        if (pipe_selected < 0 || pipe_selected > dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid pipe type parameter specified! Please see 'fabric threshold usage' \n");
        }
        iteration_parameters->first_pipe = pipe_selected;
        iteration_parameters->last_pipe = pipe_selected;
    }

    if (priority_selected != DIAG_DNXC_OPTION_ALL)
    {
        if (priority_selected < 0
            || priority_selected > dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit) - 1)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Invalid priority type parameter specified! Please see 'fabric threshold usage' \n");
        }
        iteration_parameters->first_priority = priority_selected;
        iteration_parameters->last_priority = priority_selected;
    }

    /*
     * Choose the stage to be displayed if such is provided
     */
    if (th_stage_selected != DIAG_DNXC_OPTION_ALL)
    {
        iteration_parameters->first_stage = th_stage_selected;
        iteration_parameters->last_stage = th_stage_selected;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Function used to split the links into ranges
 *  on the bases of their cgm profile
 */
static shr_error_e
diag_dnxf_thresholds_profile_link_ranges_get(
    int unit,
    bcm_pbmp_t sfi_pbmp,
    bcm_pbmp_t profile_0_links_pbmp,
    diag_dnxf_thresholds_profile_link_ranges_t * profiles_link_ranges,
    int * nof_link_ranges)
{
    bcm_port_t link = 0, last_enabled_link = 0;
    int link_inner_index = 0;
    int current_link_profile = 0;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Find first enabled link and last enabled link
     */
    _SHR_PBMP_LAST(sfi_pbmp, last_enabled_link);

    /**
     * Set default value for nof_link_ranges
     */
    *nof_link_ranges = 0;

    for (link = 0; link < dnxf_data_port.general.nof_links_get(unit);)
    {
        if (!BCM_PBMP_MEMBER(sfi_pbmp, link))
        {
            /**
             * If link is disabled skip it and go to the next one
             */
            link++;
            continue;
        }
        /**
         * Find first link from the range
         */
        /**
         * Get first link profile
         */
        profiles_link_ranges[(*nof_link_ranges)].range_first_link = link;
        if (BCM_PBMP_MEMBER(profile_0_links_pbmp, link))
        {
            current_link_profile = 0;
        }
        else
        {
            current_link_profile = 1;
        }
        profiles_link_ranges[(*nof_link_ranges)].range_profile = current_link_profile;

        /**
         * Find last link from the range
         */
        for (link_inner_index = link + 1; link_inner_index < dnxf_data_port.general.nof_links_get(unit); link_inner_index++)
        {
            if (!BCM_PBMP_MEMBER(sfi_pbmp, link_inner_index))
            {
                /**
                 * If disabled link is found end the range and shift to next one
                 */
                profiles_link_ranges[(*nof_link_ranges)].range_last_link = link_inner_index - 1;
                break;
            }

            if (BCM_PBMP_MEMBER(profile_0_links_pbmp, link_inner_index))
            {
                current_link_profile = 0;
            }
            else
            {
                current_link_profile = 1;
            }
            /**
             * If next link is with different profile OR on the last link on the system END the range
             */
            if ((current_link_profile != profiles_link_ranges[(*nof_link_ranges)].range_profile) || (link_inner_index == last_enabled_link))
            {

                if ((current_link_profile != profiles_link_ranges[(*nof_link_ranges)].range_profile) && (link_inner_index == last_enabled_link))
                {
                    /**
                     * Special case for when last enabled link is with different profile
                     * End current range and create one more for the last enabled link
                     */
                    profiles_link_ranges[(*nof_link_ranges)].range_last_link = link_inner_index - 1;
                    /**
                     * Create one more range for last enabled link
                     */
                    (*nof_link_ranges)++;
                    profiles_link_ranges[(*nof_link_ranges)].range_profile = current_link_profile;
                    profiles_link_ranges[(*nof_link_ranges)].range_first_link = last_enabled_link;
                    profiles_link_ranges[(*nof_link_ranges)].range_last_link = last_enabled_link;
                }
                else
                {
                    /**
                     * Check if it is not the last link on the FE. If so assume that no more ranges will follow and assign the last link on the last range with it.
                     */
                    profiles_link_ranges[(*nof_link_ranges)].range_last_link = (link_inner_index == last_enabled_link) ? link_inner_index : link_inner_index - 1;
                }
                break;
            }

        }
        /**
         * Continue Outside loop with range last link + 1 IF not the with the last link on the system so we exit the loop
         */
        link = (link_inner_index == last_enabled_link) ? dnxf_data_port.general.nof_links_get(unit) : link_inner_index;
        /**
         * Shift to next range element
         */
        (*nof_link_ranges)++;
    }

    if (*nof_link_ranges == 0)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "No valid profile link ranges were found! \n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * Function to check if a given threshold is marked as two_pipes_only
 */
static shr_error_e
diag_dnxf_threshold_is_with_two_pipes_get(
    int unit,
    int threshold_idx,
    int pipe_selected,
    uint32 th_validity_bmp,
    int * th_with_two_pipes)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Check if threshold has values only for pipe 0 and pipe 1. If so raise a flag
     */
    if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_ONLY_TWO_PIPES)
    {
        if (pipe_selected == 2)
        {
            /*
             * Print the name and skip it
             */
            LOG_CLI(("The threshold %s doesn't have values for pipe 2.\n",
                     dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str));
        }
        if (pipe_selected == DIAG_DNXC_OPTION_ALL)
        {
            LOG_CLI(("The following threshold doesn't have values for pipe 2.\n"));
        }
        *th_with_two_pipes = TRUE;
    }
    else
    {
        *th_with_two_pipes = FALSE;
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxf_pipe_type_threshold_print(
    int unit,
    int threshold_idx,
    uint32 th_validity_bmp,
    bcm_port_t link_selected,
    int pipe_selected,
    bcm_pbmp_t profile_0_links_pbmp,
    diag_dnxf_thresholds_iteration_params_t iteration_parameters,
    diag_dnxf_thresholds_profile_link_ranges_t * profiles_link_ranges,
    int nof_link_ranges,
    sh_sand_control_t * sand_control)
{
    uint32 flags = 0;
    int th_with_two_pipes = FALSE;
    int pipe, threshold_value, current_link_profile, range_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if threshold has values only for pipe 0 and pipe 1. If so raise a flag
     */
    SHR_IF_ERR_EXIT(diag_dnxf_threshold_is_with_two_pipes_get(unit, threshold_idx, pipe_selected, th_validity_bmp, &th_with_two_pipes));

    /**
     * If pipe selected is 2 and threshold type is with values for only 2 pipes SKIP the print
     */
    if(th_with_two_pipes && pipe_selected == 2) {
        SHR_EXIT();
    }

    PRT_TITLE_SET("%s", dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

    PRT_COLUMN_ADD("Links");

    for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
    {
        PRT_COLUMN_ADD("Pipe %d", pipe);
    }

    /*
     * Profile non dependent thresholds will printout values for all links
     */
    if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        if (link_selected != DIAG_DNXC_OPTION_ALL)
        {
            PRT_CELL_SET("%d", link_selected);
        }
        else
        {
            PRT_CELL_SET("ALL");
        }
        for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
        {
            if (th_with_two_pipes && pipe == 2)
            {
                PRT_CELL_SET("N/A");
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                (unit, 0, (bcm_fabric_threshold_id_t) pipe,
                                 (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                PRT_CELL_SET("%d", threshold_value);
            }
        }
    }
    else
    {
        /*
         * Two cases handled below 1. If specific link selected: only one iteration - print link and its
         * threshold values 2. If requested info is for all links: group links with same profile and print
         * them in consecutive manner
         */
        if (link_selected != DIAG_DNXC_OPTION_ALL)
        {
            /*
             * See what is the profile of the link
             */
            current_link_profile = BCM_PBMP_MEMBER(profile_0_links_pbmp, link_selected) ? 0 : 1;

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", link_selected);

            for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
            {
                if (th_with_two_pipes && pipe == 2)
                {
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                    (unit, current_link_profile, (bcm_fabric_threshold_id_t) pipe,
                                     (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                    PRT_CELL_SET("%d", threshold_value);
                }
            }
        }
        else
        {
            for (range_index = 0; range_index < nof_link_ranges; range_index++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                if (profiles_link_ranges[range_index].range_first_link != profiles_link_ranges[range_index].range_last_link)
                {
                    PRT_CELL_SET("%d - %d", profiles_link_ranges[range_index].range_first_link, profiles_link_ranges[range_index].range_last_link);
                }
                else
                {
                    PRT_CELL_SET("%d", profiles_link_ranges[range_index].range_first_link);
                }

                /*
                 * Print next group of links associated to same threshold profile
                 */
                for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
                {
                    if (th_with_two_pipes && pipe == 2)
                    {
                        PRT_CELL_SET("N/A");
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                        (unit, profiles_link_ranges[range_index].range_profile, (bcm_fabric_threshold_id_t) pipe,
                                         (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                        PRT_CELL_SET("%d", threshold_value);
                    }
                }

            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxf_pipe_level_type_threshold_print(
    int unit,
    int threshold_idx,
    uint32 th_validity_bmp,
    bcm_port_t link_selected,
    int pipe_selected,
    bcm_pbmp_t profile_0_links_pbmp,
    diag_dnxf_thresholds_iteration_params_t iteration_parameters,
    diag_dnxf_thresholds_profile_link_ranges_t * profiles_link_ranges,
    int nof_link_ranges,
    sh_sand_control_t * sand_control)
{
    uint32 flags = 0;
    int th_with_two_pipes = FALSE;
    int pipe, level, threshold_value, current_link_profile, range_index,threshold_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if threshold has values only for pipe 0 and pipe 1. If so raise a flag
     */
    SHR_IF_ERR_EXIT(diag_dnxf_threshold_is_with_two_pipes_get(unit, threshold_idx, pipe_selected, th_validity_bmp, &th_with_two_pipes));

    PRT_TITLE_SET("%s", dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

    PRT_COLUMN_ADD("Links");
    PRT_COLUMN_ADD("Level");

    for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
    {
        PRT_COLUMN_ADD("Pipe %d", pipe);
    }

    /*
     * Profile non dependent thresholds will printout values for all links
     */
    if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
    {
        for (level = 0; level < dnxf_data_fabric.congestion.nof_threshold_levels_get(unit); level++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            if (link_selected != DIAG_DNXC_OPTION_ALL)
            {
                PRT_CELL_SET("%d", link_selected);
            }
            else
            {
                PRT_CELL_SET("ALL");
            }
            PRT_CELL_SET("%d", level);
            for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
            {
                threshold_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level);
                if (th_with_two_pipes && pipe == 2)
                {
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                    (unit, 0, (bcm_fabric_threshold_id_t) threshold_id,
                                     (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                    PRT_CELL_SET("%d", threshold_value);
                }
            }
        }
    }
    else
    {
        /*
         * Two cases handled below 1. If specific link selected: only one iteration - print link and its
         * threshold values 2. If requested info is for all links: group links with same profile and print
         * them in consecutive manner
         */
        if (link_selected != DIAG_DNXC_OPTION_ALL)
        {
            /*
             * See what is the profile of the link
             */
            current_link_profile = BCM_PBMP_MEMBER(profile_0_links_pbmp, link_selected) ? 0 : 1;

            for (level = 0; level < dnxf_data_fabric.congestion.nof_threshold_levels_get(unit); level++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", link_selected);
                PRT_CELL_SET("%d", level);
                for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
                {
                    threshold_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level);
                    if (th_with_two_pipes && pipe == 2)
                    {
                        PRT_CELL_SET("N/A");
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                        (unit, current_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                         (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                        PRT_CELL_SET("%d", threshold_value);
                    }
                }
            }
        }
        else
        {
            for (range_index = 0; range_index < nof_link_ranges; range_index++)
            {

                for (level = 0; level < dnxf_data_fabric.congestion.nof_threshold_levels_get(unit); level++)
                {
                    /**
                     * Print dash separator only for the first element from the range
                     */
                    if (level == dnxf_data_fabric.congestion.nof_threshold_levels_get(unit) - 1)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    }

                    if (profiles_link_ranges[range_index].range_first_link != profiles_link_ranges[range_index].range_last_link)
                    {
                        PRT_CELL_SET("%d - %d", profiles_link_ranges[range_index].range_first_link, profiles_link_ranges[range_index].range_last_link);
                    }
                    else
                    {
                        PRT_CELL_SET("%d", profiles_link_ranges[range_index].range_first_link);
                    }
                    PRT_CELL_SET("%d", level);
                    /*
                     * Print next group of links associated to same threshold profile
                     */
                    for (pipe = iteration_parameters.first_pipe; pipe <= iteration_parameters.last_pipe; pipe++)
                    {
                        threshold_id = BCM_FABRIC_TH_INDEX_PIPE_LEVEL_SET(pipe, level);
                        if (th_with_two_pipes && pipe == 2)
                        {
                            PRT_CELL_SET("N/A");
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                            (unit, profiles_link_ranges[range_index].range_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                             (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                            PRT_CELL_SET("%d", threshold_value);
                        }
                    }
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxf_priority_type_threshold_print(
    int unit,
    int threshold_idx,
    uint32 th_validity_bmp,
    bcm_port_t link_selected,
    bcm_pbmp_t profile_0_links_pbmp,
    diag_dnxf_thresholds_iteration_params_t iteration_parameters,
    diag_dnxf_thresholds_profile_link_ranges_t * profiles_link_ranges,
    int nof_link_ranges,
    sh_sand_control_t * sand_control)
{
    uint32 flags = 0;
    int priority, threshold_value, current_link_profile, range_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

    PRT_COLUMN_ADD("Links");

    for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
    {
        PRT_COLUMN_ADD("Priority %d", priority);
    }

    /*
     * Profile non dependent thresholds will printout values for all links
     */
    if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        if (link_selected != DIAG_DNXC_OPTION_ALL)
        {
            PRT_CELL_SET("%d", link_selected);
        }
        else
        {
            PRT_CELL_SET("ALL");
        }
        for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
        {
            SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                            (unit, 0, (bcm_fabric_threshold_id_t) priority,
                             (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
            PRT_CELL_SET("%d", threshold_value);
        }
    }
    else
    {
        /*
         * Two cases handled below 1. If specific link selected: only one iteration - print link and its
         * threshold values 2. If requested info is for all links: group links with same profile and print
         * them in consecutive manner
         */
        if (link_selected != DIAG_DNXC_OPTION_ALL)
        {
            /*
             * See what is the profile of the link
             */
            current_link_profile = BCM_PBMP_MEMBER(profile_0_links_pbmp, link_selected) ? 0 : 1;

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", link_selected);

            for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_pipe; priority++)
            {

                SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                (unit, current_link_profile, (bcm_fabric_threshold_id_t) priority,
                                 (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                PRT_CELL_SET("%d", threshold_value);
            }
        }
        else
        {
            for (range_index = 0; range_index < nof_link_ranges; range_index++)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                if (profiles_link_ranges[range_index].range_first_link != profiles_link_ranges[range_index].range_last_link)
                {
                    PRT_CELL_SET("%d - %d", profiles_link_ranges[range_index].range_first_link, profiles_link_ranges[range_index].range_last_link);
                }
                else
                {
                    PRT_CELL_SET("%d", profiles_link_ranges[range_index].range_first_link);
                }

                /*
                 * Print next group of links associated to same threshold profile
                 */
                for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
                {
                    SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                    (unit, profiles_link_ranges[range_index].range_profile, (bcm_fabric_threshold_id_t) priority,
                                     (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                    PRT_CELL_SET("%d", threshold_value);
                }

            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxf_cast_priority_type_threshold_print(
    int unit,
    int threshold_idx,
    uint32 th_validity_bmp,
    bcm_port_t link_selected,
    bcm_pbmp_t profile_0_links_pbmp,
    diag_dnxf_thresholds_iteration_params_t iteration_parameters,
    diag_dnxf_thresholds_profile_link_ranges_t * profiles_link_ranges,
    int nof_link_ranges,
    sh_sand_control_t * sand_control)
{
    uint32 flags = 0;
    bcm_cast_t cast = bcmCastAll;
    int priority, threshold_value, current_link_profile, range_index,threshold_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_str);

    PRT_COLUMN_ADD("Links");
    PRT_COLUMN_ADD("Cast");

    for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
    {
        PRT_COLUMN_ADD("Priority %d", priority);
    }

    /*
     * Profile non dependent thresholds will printout values for all links
     */
    if (th_validity_bmp & _SHR_FABRIC_CGM_VALIDITY_NOT_PROFILE_DEPENDENT)
    {
        for (cast = 0; cast < 2; cast++)
        {
            DIAG_DNXF_THRESHOLDS_CAST_ROW_SKIP_PRINT(cast);
            if (link_selected != DIAG_DNXC_OPTION_ALL)
            {
                PRT_CELL_SET("%d", link_selected);
            }
            else
            {
                PRT_CELL_SET("ALL");
            }

            DIAG_DNXF_THRESHOLDS_CAST_CELL_PRINT(cast);

            for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
            {
                threshold_id = BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, priority);
                SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                (unit, 0, (bcm_fabric_threshold_id_t) threshold_id,
                                 (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                PRT_CELL_SET("%d", threshold_value);
            }
        }
    }
    else
    {
        /*
         * Two cases handled below 1. If specific link selected: only one iteration - print link and its
         * threshold values 2. If requested info is for all links: group links with same profile and print
         * them in consecutive manner
         */
        if (link_selected != DIAG_DNXC_OPTION_ALL)
        {
            /*
             * See what is the profile of the link
             */
            current_link_profile = BCM_PBMP_MEMBER(profile_0_links_pbmp, link_selected) ? 0 : 1;

            for (cast = 0; cast < 2; cast++)
            {
                DIAG_DNXF_THRESHOLDS_CAST_ROW_SKIP_PRINT(cast);
                PRT_CELL_SET("%d", link_selected);

                DIAG_DNXF_THRESHOLDS_CAST_CELL_PRINT(cast);

                for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
                {
                    threshold_id = BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, priority);
                    SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                    (unit, current_link_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                     (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                    PRT_CELL_SET("%d", threshold_value);
                }
            }
        }
        else
        {
            for (range_index = 0; range_index < nof_link_ranges; range_index++)
            {
                for (cast = 0; cast < 2; cast++)
                {
                    /**
                     * Print dash separator only for the first element from the range
                     */
                    DIAG_DNXF_THRESHOLDS_CAST_ROW_SKIP_PRINT(cast);

                    if (profiles_link_ranges[range_index].range_first_link != profiles_link_ranges[range_index].range_last_link)
                    {
                        PRT_CELL_SET("%d - %d", profiles_link_ranges[range_index].range_first_link, profiles_link_ranges[range_index].range_last_link);
                    }
                    else
                    {
                        PRT_CELL_SET("%d", profiles_link_ranges[range_index].range_first_link);
                    }
                    DIAG_DNXF_THRESHOLDS_CAST_CELL_PRINT(cast);
                    /*
                     * Print next group of links associated to same threshold profile
                     */
                    for (priority = iteration_parameters.first_priority; priority <= iteration_parameters.last_priority; priority++)
                    {
                        threshold_id = BCM_FABRIC_TH_INDEX_CAST_PRIO_SET(cast, priority);
                        SHR_IF_ERR_EXIT(bcm_fabric_profile_threshold_get
                                        (unit, profiles_link_ranges[range_index].range_profile, (bcm_fabric_threshold_id_t) threshold_id,
                                         (bcm_fabric_threshold_type_t) threshold_idx, flags, &threshold_value));
                        PRT_CELL_SET("%d", threshold_value);
                    }
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}



/*
 * LOCAL Thresholds STRUCTUREs:
 * {
 */

static sh_sand_enum_t dnxf_fabric_thresholds_group_enum_table[] = {
    {"GCI", soc_dnxf_threshold_group_gci_option, "GCI threshold"},
    {"RCI", soc_dnxf_threshold_group_rci_option, "RCI threshold"},
    {"FC", soc_dnxf_threshold_group_fc_option, "FC threshold"},
    {"DROP", soc_dnxf_threshold_group_drop_option, "DROP threshold"},
    {"SIZE", soc_dnxf_threshold_group_size_option, "SIZE threshold"},
    {NULL}
};

/* Link the values in the two enums*/
static sh_sand_enum_t dnxf_fabric_stage_enum_table[] = {
    {"TX", soc_dnxf_threshold_stage_tx_option, "TX STAGE"},
    {"MIDDLE", soc_dnxf_threshold_stage_middle_option, "MIDDLE STAGE"},
    {"RX", soc_dnxf_threshold_stage_rx_option, "RX STAGE"},
    {"SHARED", soc_dnxf_threshold_stage_shared_option, "SHARED STAGE"},
    {"ALL", DIAG_DNXC_OPTION_ALL, "ALL STAGE"},
    {NULL}
};

/*
 * LOCAL Threshold STRUCTUREs:
 * }
 */

/* *INDENT-OFF* */
sh_sand_option_t dnxf_fabric_thresholds_options[] = {
    {"group",      SAL_FIELD_TYPE_ENUM,    "Threshold Group (GCI/RCI/FC/DROP/SIZE)",  NULL, (void *) dnxf_fabric_thresholds_group_enum_table, NULL, SH_SAND_ARG_FREE},
    {"pipe",       SAL_FIELD_TYPE_INT32,   "Pipe ID (0/1/2)",                       "-1"},
    {"priority",   SAL_FIELD_TYPE_INT32,   "Priority (0/1/2/3)",                    "-1"},
    {"stage",      SAL_FIELD_TYPE_ENUM,    "TX/MIDDLE/RX/SHARED",                   "ALL",   (void *) dnxf_fabric_stage_enum_table},
    {"link",       SAL_FIELD_TYPE_INT32,   "link ID",                               "-1"},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
sh_sand_man_t dnxf_fabric_thresholds_man = {
    .brief =    "Display the configured FIFO thresholds",
    .full  =    "Displays all FIFO threshold information per threshold type \n"
                "One of the following threshold groups must be selected: \n"
                "\t RCI     - thresholds for local congestion manager \n"
                "\t GCI     - thresholds for global congestion manager \n"
                "\t DROP    - admission/drop thresholds \n"
                "\t FC      - FlowControl thresholds \n"
                "\t SIZE    - FIFO sizes \n"
                "Filters: \n"
                "\t STAGE       - shows thresholds only for specific stage - RX, MIDDLE, TX or SHARED \n"
                "\t PIPE        - shows thresholds only for specific pipe \n"
                "\t LINK        - shows thresholds only for specific link \n"
                "\t PRIOrity    - shows thresholds only for specific priority",
    .synopsis = "<RCI/GCI/FC/DROP/SIZE> \n"
                "[stage=TX/MIDDLE/RX/SHARED/ALL] [pipe=<0/1/2>] [priority=<0/1/2/3>] [link=<link_id>]",
    .examples = "RCI \n"
                "RCI stage=TX\n"
                "DROP stage=MIDDLE priority=3\n"
                "RCI priority=2 link=46 \n"
                "DROP stage=tx pipe=0"
};
/* *INDENT-ON* */

static shr_error_e
diag_dnxf_thresholds_diag_print(
    int unit,
    int th_group_selected,
    int th_stage_selected,
    int pipe_selected,
    int priority_selected,
    int link_selected,
    sh_sand_control_t * sand_control)
{
    bcm_port_config_t config;
    int threshold_idx;
    uint32 flags = 0x0, th_validity_bmp;
    bcm_pbmp_t profile_0_links_pbmp;
    int profile_0_links_arr[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS], profile_0_link_idx;
    uint32 profile_0_links_count;
    soc_dnxf_threshold_stage_options_t stage;
    int nof_link_ranges = 0;
    diag_dnxf_thresholds_iteration_params_t iteration_parameters;

    diag_dnxf_thresholds_profile_link_ranges_t profiles_link_ranges[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &config));
    if (link_selected != DIAG_DNXC_OPTION_ALL && !BCM_PBMP_MEMBER(config.sfi, link_selected))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Specified link is either not valid or not enabled! \n");
    }

    /**
     * Get parameters to iterate over
     */
    SHR_IF_ERR_EXIT(diag_dnxf_thresholds_iteration_parameters_get(unit, pipe_selected, priority_selected, th_stage_selected, &iteration_parameters));

    /*
     * Get all links associated to threshold profile 0
     */
    SHR_IF_ERR_EXIT(bcm_fabric_link_profile_get
                    (unit, 0, flags, dnxf_data_port.general.nof_links_get(unit), &profile_0_links_count,
                     profile_0_links_arr));
    /*
     * Because only 2 profiles are available there is no need to to get also the links for profile 1
     *  to construct the logic
     */

    /*
     * Convert the link array to port bmps for simplicity when creating the ranges
     */
    BCM_PBMP_CLEAR(profile_0_links_pbmp);
    for (profile_0_link_idx = 0; profile_0_link_idx < profile_0_links_count; profile_0_link_idx++)
    {
        BCM_PBMP_PORT_ADD(profile_0_links_pbmp, profile_0_links_arr[profile_0_link_idx]);
    }

    /**
     * Get the link ranges only if user haven't provided a specific link
     */
    if (link_selected != DIAG_DNXC_OPTION_ALL) {
        sal_memset(profiles_link_ranges, 0, sizeof(diag_dnxf_thresholds_profile_link_ranges_t) * DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS);
    } else {
        SHR_IF_ERR_EXIT(diag_dnxf_thresholds_profile_link_ranges_get(unit, config.sfi, profile_0_links_pbmp, profiles_link_ranges, &nof_link_ranges));
    }
    /*
     * Iterate over the selected stages
     */
    for (stage = iteration_parameters.first_stage; stage < iteration_parameters.last_stage; stage++)
    {
        /*
         * Iterate over all thresholds and decide which one to display
         */
        for (threshold_idx = 0; threshold_idx < dnxf_data_fabric.congestion.nof_thresholds_get(unit); threshold_idx++)
        {
            /*
             * Check if threshold should be displayed or not
             */
            if (!(dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->is_to_be_displayed))
            {
                continue;
            }
            /*
             * Check if currently displayed stage matches the one of the threshold
             */
            if (stage != dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_stage)
            {
                continue;
            }

            /*
             * Filter only thresholds for selected Threshold Type (GCI/RCI/FC/DROP/SIZE)
             */
            if (th_group_selected != dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->threshold_group)
            {
                continue;
            }

            /*
             * Get the features validity bit map for the current threshold from the main info table using the key
             */
            th_validity_bmp = dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->th_validity_bmp;


            /*
             * Four handler switches for the four threshold indexes types 1._shr_dnxf_cgm_index_type_pipe
             * 2._shr_dnxf_cgm_index_type_pipe_level 3._shr_dnxf_cgm_index_type_priority
             * 4._shr_dnxf_cgm_index_type_cast_prio 
             */
            /*
             * PIPE type threshold
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                    _shr_dnxf_cgm_index_type_pipe)
            {
                SHR_IF_ERR_EXIT(diag_dnxf_pipe_type_threshold_print(unit, threshold_idx, th_validity_bmp, link_selected, pipe_selected, profile_0_links_pbmp,
                         iteration_parameters, profiles_link_ranges, nof_link_ranges, sand_control));
            }

            /*
             * PIPE LEVEL type threshold
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                    _shr_dnxf_cgm_index_type_pipe_level)
            {
                SHR_IF_ERR_EXIT(diag_dnxf_pipe_level_type_threshold_print(unit, threshold_idx, th_validity_bmp, link_selected, pipe_selected, profile_0_links_pbmp,
                         iteration_parameters, profiles_link_ranges, nof_link_ranges, sand_control));
            }

            /*
             * PRIORITY type threshold
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                    _shr_dnxf_cgm_index_type_priority)
            {
                SHR_IF_ERR_EXIT(diag_dnxf_priority_type_threshold_print(unit, threshold_idx, th_validity_bmp, link_selected, profile_0_links_pbmp,
                         iteration_parameters, profiles_link_ranges, nof_link_ranges, sand_control));
            }

            /*
             * CAST PRIORITY type threshold
             */
            if (dnxf_data_fabric.congestion.thresholds_info_get(unit, threshold_idx)->index_type ==
                    _shr_dnxf_cgm_index_type_cast_prio)
            {
                SHR_IF_ERR_EXIT(diag_dnxf_cast_priority_type_threshold_print(unit, threshold_idx, th_validity_bmp, link_selected, profile_0_links_pbmp,
                         iteration_parameters, profiles_link_ranges, nof_link_ranges, sand_control));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - display the configured FIFO thresholds
 */
shr_error_e
cmd_dnxf_fabric_thresholds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int th_group_selected;
    int pipe_selected, priority_selected, link_selected;
    int th_stage_selected;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the command inputs
     */
    SH_SAND_GET_ENUM("group", th_group_selected);
    SH_SAND_GET_ENUM("stage", th_stage_selected);
    SH_SAND_GET_INT32("pipe", pipe_selected);
    SH_SAND_GET_INT32("priority", priority_selected);
    SH_SAND_GET_INT32("link", link_selected);

    /*
     * Print in format for DNXF devices
     */
    SHR_IF_ERR_EXIT(diag_dnxf_thresholds_diag_print
                    (unit, th_group_selected, th_stage_selected, pipe_selected, priority_selected, link_selected,
                     sand_control));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Queues diagnostic
 */

/* Queues filters per block*/
static sh_sand_enum_t dnxf_fabric_queues_blocks_enum_table[] = {
    {"DCH", soc_dnxf_dch_stage_option, "DCH block"},
    {"DTM", soc_dnxf_dtm_stage_option, "DTM block"},
    {"DTL", soc_dnxf_dtl_stage_option, "DTL block"},
    {"DFL", soc_dnxf_dfl_stage_option, "DFL block"},
    {"ALL", DIAG_DNXC_OPTION_ALL, "ALL block"},
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_option_t dnxf_fabric_queues_options[] = {
    {"block", SAL_FIELD_TYPE_ENUM, "FE Stage (DCH/DTM/DTL/DFL)", "ALL", (void *) dnxf_fabric_queues_blocks_enum_table, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

sh_sand_man_t dnxf_fabric_queues_man = {
    .brief    = "Display queues status",
    .full     = "Show maximum queues occupancy per block. \n"
                "Filters: \n"
                "\t DCH - Show most occupied link for each DCH link group and the maximum number of cells in its FIFO \n"
                "\t DTM - Shows the maximum cells in each DCH FIFO of each DTM block \n"
                "\t DTL - Show most occupied link of each DTL and the maximum number of cells in its FIFO \n"
                "\t DFL - Shows the most occupied DFL Bank and its most occupied Sub-Bank and the maximum number of cells in this Sub-Bank \n",
    .synopsis = "<DCH/DTM/DTL/DFL>",
    .examples = "dch\n"
                "dtm\n"
                "dtl\n"
                "dfl\n"
                "all",
};

/* *INDENT-ON* */

/**
 * \brief - display queues status
 */
shr_error_e
cmd_dnxf_fabric_queues(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_pipes;
    int queue_stage_selected;
    int pipe_idx, block_idx, fifo_idx, dfl_bank_idx, ifm_block_idx;
    int sub_bank_idx = -1, most_occupied_bank = -1;
    uint32 dtl_link_group_first_link = 0, dtl_link_group_last_link = 0;
    uint32 max_nof_sub_bank_entries = 0, min_nof_bank_free_entries = 0;
    uint32 current_dfl_sub_bank_0_value, current_dfl_sub_bank_1_value;
    uint32 current_fifo_max_occupancy;
    soc_dnxf_queues_info_t fe_queues_info;
    char sub_bank_idx_buf[5], most_occupied_bank_buf[5];
    soc_dnxf_queues_dch_link_group_t *current_dch_link_group;
    soc_dnxf_queues_dtl_info_t *current_dtl_block;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get filter input if used
     */
    SH_SAND_GET_ENUM("block", queue_stage_selected);

    LOG_CLI(("QUEUES status\n"));

    /*
     * Init the diag info structure which will be used to store the information
     */
    soc_dnxf_queues_info_init(unit, &fe_queues_info);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_queues_get_info, (unit, &fe_queues_info)));
    nof_pipes = fe_queues_info.nof_pipes;

    /*
     * DCH print 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == soc_dnxf_dch_stage_option)
    {
        PRT_TITLE_SET("DCH QUEUES");

        PRT_COLUMN_ADD("DCH#");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("Link Group");
        PRT_COLUMN_ADD("Link");
        PRT_COLUMN_ADD("Max Occupancy level");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dch_get(unit); block_idx++)
            {
                /*
                 * Each DCH is divided into 3 big IFM FIFO-s each responsible for 8 links from the overall 24
                 */
                for (ifm_block_idx = 0; ifm_block_idx < dnxf_data_device.blocks.nof_dch_link_groups_get(unit);
                     ifm_block_idx++)
                {
                    /*
                     * Display vallues only if there is any traffic through the link group
                     */
                    current_dch_link_group =
                        &fe_queues_info.dch_queues_info[pipe_idx].nof_dch[block_idx].dch_link_group[ifm_block_idx];
                    if (current_dch_link_group->max_occupancy_value)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("DCH%d", block_idx);
                        PRT_CELL_SET("%d", pipe_idx);
                        PRT_CELL_SET("%d - %d", current_dch_link_group->link_group_first_link,
                                     current_dch_link_group->link_group_last_link);
                        PRT_CELL_SET("%u", current_dch_link_group->most_occupied_link);
                        PRT_CELL_SET("%u", current_dch_link_group->max_occupancy_value);
                    }
                }
            }
        }
        PRT_COMMITX;
    }
    /*
     * DTM Printing { 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == soc_dnxf_dtm_stage_option)
    {
        PRT_TITLE_SET("DTM QUEUES");

        PRT_COLUMN_ADD("DTM#");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("DCH FIFO");
        PRT_COLUMN_ADD("MAX occupancy value");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
            {
                for (fifo_idx = 0; fifo_idx < dnxf_data_device.blocks.nof_dtm_fifos_get(unit); fifo_idx++)
                {
                    current_fifo_max_occupancy =
                        fe_queues_info.dtm_queues_info[pipe_idx].nof_dtm[block_idx].fifo_max_occupancy_value[fifo_idx];
                    /*
                     * Print only if value different than 0
                     */
                    if (current_fifo_max_occupancy)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("DTM%d", block_idx);
                        PRT_CELL_SET("%d", pipe_idx);
                        PRT_CELL_SET("%d", fifo_idx);
                        PRT_CELL_SET("%u", current_fifo_max_occupancy);
                    }
                }
            }
        }
        PRT_COMMITX;
        /*
         * DTM Printing } 
         */
    }

    /*
     * DTL print { 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == soc_dnxf_dtl_stage_option)
    {

        PRT_TITLE_SET("DTL QUEUES");
        if (SOC_DNXF_IS_FE13(unit))
        {
            PRT_INFO_ADD("NON-Local route");
        }

        PRT_COLUMN_ADD("DTL #");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("Link Group");
        PRT_COLUMN_ADD("Link");
        PRT_COLUMN_ADD("Max Occupancy level");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
            {
                current_dtl_block = &fe_queues_info.dtl_queues_info_nlr[pipe_idx].nof_dtl[block_idx];

                /*
                 * Print only if value different than 0
                 */
                if (current_dtl_block->max_occupancy_value)
                {
                    dtl_link_group_first_link = block_idx * dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
                    dtl_link_group_last_link =
                        dtl_link_group_first_link + dnxf_data_device.blocks.nof_links_in_dcq_get(unit) - 1;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    PRT_CELL_SET("DTL%d", block_idx);
                    PRT_CELL_SET("%d", pipe_idx);

                    PRT_CELL_SET("%d - %d", dtl_link_group_first_link, dtl_link_group_last_link);

                    PRT_CELL_SET("%u", current_dtl_block->most_occupied_link);
                    PRT_CELL_SET("%u", current_dtl_block->max_occupancy_value);
                }
            }
        }
        PRT_COMMITX;
    }
    /*
     * If FE13 Print Local-route table also
     */
    if (SOC_DNXF_IS_FE13(unit))
    {
        PRT_TITLE_SET("DTL QUEUES");
        PRT_INFO_ADD("Local route");

        PRT_COLUMN_ADD("DTL #");
        PRT_COLUMN_ADD("Pipe");
        PRT_COLUMN_ADD("Link Group");
        PRT_COLUMN_ADD("Link");
        PRT_COLUMN_ADD("Max Occupancy level");

        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
            {
                current_dtl_block = &fe_queues_info.dtl_queues_info_lr[pipe_idx].nof_dtl[block_idx];
                /*
                 * Print only if value different than 0
                 */
                if (current_dtl_block->max_occupancy_value)
                {
                    dtl_link_group_first_link = block_idx * dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
                    dtl_link_group_last_link =
                        dtl_link_group_first_link + dnxf_data_device.blocks.nof_links_in_dcq_get(unit) - 1;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    PRT_CELL_SET("DTL%d", block_idx);
                    PRT_CELL_SET("%d", pipe_idx);

                    PRT_CELL_SET("%d - %d", dtl_link_group_first_link, dtl_link_group_last_link);
                    PRT_CELL_SET("%u", current_dtl_block->most_occupied_link);
                    PRT_CELL_SET("%u", current_dtl_block->max_occupancy_value);
                }
            }
        }
        PRT_COMMITX;
    }
    /*
     * DTL print } 
     */

    /*
     * DFL print { 
     */
    if (queue_stage_selected == DIAG_DNXC_OPTION_ALL || queue_stage_selected == soc_dnxf_dfl_stage_option)
    {
        PRT_TITLE_SET("DFL STATUS");

        PRT_COLUMN_ADD("DFL#");
        PRT_COLUMN_ADD("BANK");
        PRT_COLUMN_ADD("Sub-BANK");
        PRT_COLUMN_ADD("MAX entries");

        /*
         * MAX NOF sub-bank entries is 1800 = max NOF DFL bank entries (3600) / NOF sub-banks(2) 
         */
        max_nof_sub_bank_entries =
            dnxf_data_fabric.congestion.nof_dfl_bank_entries_get(unit) /
            dnxf_data_fabric.congestion.nof_dfl_sub_banks_get(unit);

        for (block_idx = 0; block_idx < dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
        {
            /*
             * Reset values for next DCML block iteration
             */
            most_occupied_bank = -1;
            sub_bank_idx = -1;
            min_nof_bank_free_entries = max_nof_sub_bank_entries;

            for (dfl_bank_idx = 0; dfl_bank_idx < dnxf_data_fabric.congestion.nof_dfl_banks_get(unit); dfl_bank_idx++)
            {
                current_dfl_sub_bank_0_value =
                    fe_queues_info.dfl_queues_info[block_idx].nof_dfl[dfl_bank_idx].sub_bank_min_free_entries_value[0];
                current_dfl_sub_bank_1_value =
                    fe_queues_info.dfl_queues_info[block_idx].nof_dfl[dfl_bank_idx].sub_bank_min_free_entries_value[1];

                /*
                 * Check if current bank MIN free entries is smaller than current MIN value (Reset value is 1800)
                 */
                if (min_nof_bank_free_entries > current_dfl_sub_bank_0_value
                    || min_nof_bank_free_entries > current_dfl_sub_bank_1_value)
                {
                    most_occupied_bank = dfl_bank_idx;
                    /*
                     * See which sub-bank has smaller value for minimum free entries
                     */
                    if (current_dfl_sub_bank_0_value < current_dfl_sub_bank_1_value)
                    {
                        min_nof_bank_free_entries = current_dfl_sub_bank_0_value;
                        sub_bank_idx = 0;
                    }
                    else
                    {
                        min_nof_bank_free_entries = current_dfl_sub_bank_1_value;
                        sub_bank_idx = 1;
                    }
                }
            }
            sal_snprintf(most_occupied_bank_buf, 5, "%d", most_occupied_bank);
            sal_snprintf(sub_bank_idx_buf, 5, "%d", sub_bank_idx);

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("DFL%d", block_idx);
            PRT_CELL_SET("%s", most_occupied_bank != -1 ? most_occupied_bank_buf : "No occupancy");
            PRT_CELL_SET("%s", sub_bank_idx != -1 ? sub_bank_idx_buf : "No occupancy");
            /*
             * Keep the tendency from previous tables and print maximum BANK occupancy For that Show the max nof of
             * entries that occupies the BANK by substracting From the MAX entries value(1800) the minimum value of
             * free entries
             */
            PRT_CELL_SET("%u", max_nof_sub_bank_entries - min_nof_bank_free_entries);
        }

        PRT_COMMITX;
    }
    /*
     * DFL print } 
     */
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*Diag counters graphycal DNXF printout function*/
shr_error_e
diag_dnxf_counters_graphical_print(
    int unit,
    sh_sand_control_t * sand_control)
{
    uint32 nof_pipes, max_cells_in_fifos;
    uint64 temp_counter_data, dropped_cells, unreachable_discarded_cells_cnt;
    uint32 pipe_idx, prio_idx;
    soc_dnxf_counters_info_t fe_counters_info;
    char *printout_buffer_1[50];
    char *printout_buffer_2[50];
    char buf_val[32];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get counters Info
     */
    soc_dnxf_counters_info_init(&fe_counters_info);
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_counters_get_info, (unit, &fe_counters_info)));
    nof_pipes = fe_counters_info.nof_pipes;

    if (SOC_DNXF_IS_FE2(unit))
    {
        /*
         * Print diag counter information for FE2 { 
         */
        switch (nof_pipes)
        {
            case 1:
                *printout_buffer_1 = "                 ";
                break;
            case 2:
                *printout_buffer_1 = "                          ";
                break;
            case 3:
                *printout_buffer_1 = "                                   ";
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of pipes received!");
        }
        /*
         * DCH Print * { 
         */
        LOG_CLI(("%s    ||%s\n", *printout_buffer_1, *printout_buffer_1));
        LOG_CLI(("%s    \\/%s\n", *printout_buffer_1, *printout_buffer_1));
        PRT_TITLE_SET("%s  FE2", *printout_buffer_1);
        PRT_INFO_ADD("%sDCH(0-7)", *printout_buffer_1);
        PRT_COLUMN_ADD("  Counter name            ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_in +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_in, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Fifo Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_fifo_discard +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_fifo_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /*
         * Unreachable disacard is not per pipe but for all pipes.
         */
        /** Reset to 0 */
        unreachable_discarded_cells_cnt = 0;
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            unreachable_discarded_cells_cnt += fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_unreach_discard;
            unreachable_discarded_cells_cnt += fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_unreach_discard;
        }
        format_uint64_decimal(buf_val, unreachable_discarded_cells_cnt, ',');
        PRT_CELL_SET("Unreach Discard: %s", buf_val);
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("N/A");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            max_cells_in_fifos =
                UTILEX_MAX(fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_max_fifo_cells,
                           fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_max_fifo_cells);
            PRT_CELL_SET("%d", max_cells_in_fifos);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_out +
                                  fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_COMMITX;
        /*
         * DCH Print * } 
         */
        LOG_CLI(("%s    ||%s\n", *printout_buffer_1, *printout_buffer_1));
        LOG_CLI(("%s    \\/%s\n", *printout_buffer_1, *printout_buffer_1));
        /*
         * DTM Print { 
         */
        PRT_TITLE_SET("%sDTM(0-7)", *printout_buffer_1);

        PRT_COLUMN_ADD("  Counter name            ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        /*
         * DTM Total incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total incomming cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            temp_counter_data = fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_in_lr +
                fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_in_nlr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_lr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_nlr;
            format_uint64_decimal(buf_val, temp_counter_data, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM MAX Cells in FIFO
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            max_cells_in_fifos = UTILEX_MAX(fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_max_fifo_cells_nlr,
                                            UTILEX_MAX(fe_counters_info.
                                                       dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_nlr,
                                                       fe_counters_info.
                                                       dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_lr));
            PRT_CELL_SET("%d", max_cells_in_fifos);
        }

        /*
         * DTM Total outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            temp_counter_data = fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_out_nlr +
                fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_out_lr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_nlr +
                fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_lr;
            format_uint64_decimal(buf_val, temp_counter_data, ',');
            PRT_CELL_SET("%s", buf_val);

        }

        PRT_COMMITX;
        /*
         * DTM Print } 
         */
        LOG_CLI(("%s    ||%s\n", *printout_buffer_1, *printout_buffer_1));
        LOG_CLI(("%s    \\/%s\n", *printout_buffer_1, *printout_buffer_1));
        /*
         * DTL Print { 
         */
        PRT_TITLE_SET("%sDTL(0-7)", *printout_buffer_1);

        PRT_COLUMN_ADD("  Counter name            ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        /*
         * DTL Total incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incomming cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_in_nlr +
                                  fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL Dropped Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Dropped Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
                dropped_cells += fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL MAX Cells in FIFOs
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            max_cells_in_fifos =
                UTILEX_MAX(fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_max_fifo_cells_nlr,
                           fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_max_fifo_cells_nlr);
            PRT_CELL_SET("%d", max_cells_in_fifos);
        }

        /*
         * DTL Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val,
                                  fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_out +
                                  fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL Print } 
         */

        PRT_COMMITX;

        /*
         * Print diag counter information for FE2 } 
         */
    }
    else
    {

        switch (nof_pipes)
        {
            case 1:
                *printout_buffer_1 = "                          ";
                *printout_buffer_2 = "               ";
                break;
            case 2:
                *printout_buffer_1 = "                                ";
                *printout_buffer_2 = "                          ";
                break;
            case 3:
                *printout_buffer_1 = "                                       ";
                *printout_buffer_2 = "                                     ";
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid number of pipes received!");
        }

        LOG_CLI(("%s||%s         %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s||%sFAP LINKS%s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s\\/%s         %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%sFE1%s        %sFE3\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));

        /*
         * Title { 
         */
        PRT_TITLE_SET("");

        PRT_COLUMN_ADD("   Counter name   ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }

        PRT_COLUMN_ADD("");

        PRT_COLUMN_ADD("   Counter name   ");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_COLUMN_ADD("    Pipe %d    ", pipe_idx);
        }
        /*
         * Title } 
         */
        /*
         * First Stage { 
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DCH(0-3) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }
        PRT_CELL_SKIP(2);
        PRT_CELL_SET("   DTL(0-3) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }

        /*
         * DCH(0-3) Total INCOMING DTL(0-3) Total OUTGOING
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_in, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) FIFO discard DTL(0-3) LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("FIFO Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_fifo_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("LOCAL ROUTE");
        PRT_CELL_SKIP(nof_pipes);

        /*
         * DCH(0-3) Unreach discard DTL(0-3) LOCAL ROUTE Dropped Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /*
         * Unreachable disacard is not per pipe but for all pipes.
         */
        /** Reset to 0 */
        unreachable_discarded_cells_cnt = 0;
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            unreachable_discarded_cells_cnt += fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_unreach_discard;
        }
        format_uint64_decimal(buf_val, unreachable_discarded_cells_cnt, ',');

        PRT_CELL_SET("Unreach Discard: %s", buf_val);
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("N/A");
        }

        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  Dropped LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_dropped_lr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) MAX FIFO Cells DTL(0-3) LOCAL ROUTE MAX LR Cells in FIFOs
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_max_fifo_cells);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  MAX LR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_max_fifo_cells_lr);
        }

        /*
         * DCH(0-3) Total Outgoing Cells DTL(0-3) LOCAL ROUTE Total Incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe1_counters_info[pipe_idx].dch_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET("---");
        PRT_CELL_SET("  Total Incoming LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_in_lr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("NON-LOCAL ROUTE");
        PRT_CELL_SKIP(3);

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE Dropped Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  Dropped NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE MAX LR Cells in FIFOs
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  MAX NLR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_max_fifo_cells_nlr);
        }

        /*
         * DCH(0-3) - DTL(0-3) NON-LOCAL ROUTE Total incoming
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  Total incoming NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe3_counters_info[pipe_idx].dtl_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        /*
         * First Stage } 
         */

        /*
         * Second Stage { 
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DTM(4-7) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }
        PRT_CELL_SET("   | ");
        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DTM(0-3)");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }

        /*
         * DTM(4-7) Total Incoming Cells DTM(0-3) LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET("   |");
        PRT_CELL_SET("LOCAL ROUTE:");
        PRT_CELL_SKIP(nof_pipes);

        /*
         * DTM(4-7) MAX Cells in FIFOs DTM(0-3) LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_max_fifo_cells_nlr);
        }
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  Total Outgoing LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_lr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM(4-7) Total Outgoing Cells DTM(0-3) LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe1_counters_info[pipe_idx].dtm_total_out_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET("   |");
        PRT_CELL_SET("  MAX LR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_lr);
        }

        /*
         * DTM(4-7) - DTM(0-3) LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET("    -->");
        PRT_CELL_SET("  Total Incoming LR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_lr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM(4-7) - DTM(0-3) NON-LOCAL ROUTE PRINT START
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("NON-LOCAL ROUTE");
        PRT_CELL_SKIP(nof_pipes);

        /*
         * DTM(4-7) MAX Cells in FIFOs DTM(0-3) NON-LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  Total Outgoing NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_out_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTM(4-7) MAX Cells in FIFOs DTM(0-3) NON-LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  MAX NLR Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_max_fifo_cells_nlr);
        }

        /*
         * DTM(4-7) - DTM(0-3) NON-LOCAL ROUTE Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("  Total Incoming NLR Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtm_fe3_counters_info[pipe_idx].dtm_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        /*
         * Second Stage } 
         */

        /*
         * Third Stage { 
         */
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SKIP(1);
        PRT_CELL_SET("   DTL(4-7) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }
        PRT_CELL_SKIP(2);
        PRT_CELL_SET("   DCH(4-7) ");
        if (nof_pipes > 1)
        {
            PRT_CELL_SKIP(nof_pipes - 1);
        }

        /*
         * DTL(4-7) Total Incoming Cells DCH(4-7) Total Outgoing Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_in_nlr, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL(4-7) Dropped Cells DCH(4-7) FIFO Discard
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Dropped Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            dropped_cells = 0;
            for (prio_idx = 0; prio_idx < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); prio_idx++)
            {
                dropped_cells += fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_dropped_nlr_cells[prio_idx];
            }
            format_uint64_decimal(buf_val, dropped_cells, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("FIFO Discard:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_fifo_discard, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * DTL(4-7) MAX Cells in FIFOs DCH(4-7) Unreach Discard
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_max_fifo_cells_nlr);
        }
        PRT_CELL_SET_SHIFT(3, "");
        /** Reset to 0 */
        unreachable_discarded_cells_cnt = 0;
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            unreachable_discarded_cells_cnt += fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_unreach_discard;
        }
        format_uint64_decimal(buf_val, unreachable_discarded_cells_cnt, ',');

        PRT_CELL_SET("Unreach Discard: %s", buf_val);
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("N/A");
        }

        /*
         * DTL(4-7) Total Outgoing Cells DCH(4-7) MAX Cells in FIFO
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Total Outgoing Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dtl_fe1_counters_info[pipe_idx].dtl_total_out, ',');
            PRT_CELL_SET("%s", buf_val);
        }
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("MAX Cells in FIFOs:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            PRT_CELL_SET("%d", fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_max_fifo_cells);
        }

        /*
         * DTL(4-7) - DCH(4-7) Total Incoming Cells
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SKIP(nof_pipes + 1);
        PRT_CELL_SET_SHIFT(3, "");
        PRT_CELL_SET("Total Incoming Cells:");
        for (pipe_idx = 0; pipe_idx < nof_pipes; pipe_idx++)
        {
            format_uint64_decimal(buf_val, fe_counters_info.dch_fe3_counters_info[pipe_idx].dch_total_in, ',');
            PRT_CELL_SET("%s", buf_val);
        }

        /*
         * Third Stage } 
         */
        PRT_COMMITX;

        LOG_CLI(("%s||%s         %s/\\ \n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s||%s   FE2   %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
        LOG_CLI(("%s\\/%s         %s||\n", *printout_buffer_1, *printout_buffer_2, *printout_buffer_2));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
* } DNXF DIAG printout functions
*/

/*
 * LOCAL FUNCTIONs:
 * }
 */
