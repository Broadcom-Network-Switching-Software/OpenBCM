/** \file diag_dnx_pp_termi.c
 *
 * Main diagnostics for PP applications are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*************
 * INCLUDES  *
 */
#include "diag_dnx_pp.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <src/bcm/dnx/init/init_pp.h>
/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX

/*************
 *  MACROS  *
 */

/*************
 * TYPEDEFS  *
 */
#define DNX_PP_TERMI_INGRESS_STAGES 6
#define DNX_PP_TERMI_NOF_LAYERS 8
#define DNX_PP_TERMI_MAX_NOF_CORES (dnx_data_device.general.nof_cores_get(unit))
#define DNX_PP_TERMI_IS_LAYER_PROTOCOL_MPLS(stage_idx, fwd_layer_idx)  \
    ((stage_info[stage_idx].layer_protocols[fwd_layer_idx] == (dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS))) \
     || (stage_info[stage_idx].layer_protocols[fwd_layer_idx] == (dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM))))

/*************
 * GLOBALS   *
 */
char dnx_pp_termi_ingress_stages_str[DNX_PP_TERMI_INGRESS_STAGES][RHNAME_MAX_SIZE] =
    { "VTT1", "VTT2", "VTT3", "VTT4", "VTT5", "FWD1" };
/*************
 * FUNCTIONS *
 */
typedef struct
{
    int core_id;
} diag_pp_termi_input_t;

typedef struct
{
    char packet_structure_str[DSIG_MAX_SIZE_STR];
    char terminated_headers[DNX_PP_TERMI_INGRESS_STAGES][DBAL_MAX_PRINTABLE_BUFFER_SIZE];
} diag_pp_termi_output_t;

typedef struct
{
    char name[RHNAME_MAX_SIZE];
    uint32 layer_offsets[DNX_PP_TERMI_NOF_LAYERS];
    uint32 layer_protocols[DNX_PP_TERMI_NOF_LAYERS];
    uint32 fwd_layer_index;
} diag_pp_termi_stage_info_t;

/**
 * \brief
 * initialize 'diag_pp_termi_output_t'
 *
 *   \param [in] unit - unit id
 *   \param [out] outputs - initialized object
 */
static void
diag_pp_termi_output_t_init(
    int unit,
    diag_pp_termi_output_t * outputs)
{
    int stage_idx, core_idx;

    /** initialize outputs */
    for (core_idx = 0; core_idx < DNX_PP_TERMI_MAX_NOF_CORES; core_idx++)
    {
        sal_strncpy(outputs[core_idx].packet_structure_str, "", RHNAME_MAX_SIZE);

        for (stage_idx = 0; stage_idx < DNX_PP_TERMI_INGRESS_STAGES; stage_idx++)
        {
            sal_strncpy(outputs[core_idx].terminated_headers[stage_idx], "", RHNAME_MAX_SIZE);
        }
    }
}

/**
 * \brief
 * Check if there is any difference between the 2 provided layer offset arrays.
 *
 *   \param [in] unit - unit id
 *   \param [in] layer_offsets1 - layer offsets array
 *   \param [in] layer_offsets2 - layer offsets array
 *
 * \return
 *   1 - layer_offsets arrays are not equal, 0 - otherwise
 */
static int
sh_dnx_pp_termi_layer_offsets_changed(
    int unit,
    uint32 layer_offsets1[DNX_PP_TERMI_NOF_LAYERS],
    uint32 layer_offsets2[DNX_PP_TERMI_NOF_LAYERS])
{
    int rv = 0;
    int ii;

    if ((layer_offsets1[0] == -1) || (layer_offsets2[0] == -1))
    {
        /*
         * skip in case any of the offsets is unset
         */
        return rv;
    }
    for (ii = 0; ii < DNX_PP_TERMI_NOF_LAYERS; ii++)
    {
        if (layer_offsets1[ii] != layer_offsets2[ii])
        {
            /*
             * in case speculative parsing was incorrect layer_offsets[4] is set to 0, then ignore this
             */
            if (layer_offsets1[4] != 0)
            {
                rv = 1;
                break;
            }
        }
    }

    return rv;
}

/**
 * \brief
 * Collect all the needed signals for all stages and save the data in 'stage_info'.
 *
 *   \param [in] unit - unit id
 *   \param [in] core_id - related core
 *   \param [out] stage_info - all collected information per stage for VTT1-FWD1 stages
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_termi_convert_signals_to_data(
    int unit,
    int core_id,
    diag_pp_termi_stage_info_t * stage_info)
{
    signal_output_t *field_list_element;
    int i = 0, stage_idx;
    signal_output_t *layer_offsets_sig = NULL;
    signal_output_t *layer_protocols_sig = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (stage_idx = 0; stage_idx < DNX_PP_TERMI_INGRESS_STAGES - 1; stage_idx++)
    {
        sal_strncpy(stage_info[stage_idx].name, dnx_pp_termi_ingress_stages_str[stage_idx], RHNAME_MAX_SIZE - 1);
        stage_info[stage_idx].name[RHNAME_MAX_SIZE - 1] = '\0';

        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0,
                                                    "IRPP", stage_info[stage_idx].name, "", "layer_offsets",
                                                    &layer_offsets_sig), "");
        i = 0;
        RHITERATOR(field_list_element, layer_offsets_sig->field_list)
        {
            stage_info[stage_idx].layer_offsets[i] = field_list_element->value[0];
            i++;
        }

        SHR_CLI_EXIT_IF_ERR(sand_signal_output_find(unit, core_id, 0,
                                                    "IRPP", stage_info[stage_idx].name, "", "layer_protocols",
                                                    &layer_protocols_sig), "");
        i = 0;
        RHITERATOR(field_list_element, layer_protocols_sig->field_list)
        {
            stage_info[stage_idx].layer_protocols[i] = field_list_element->value[0];
            i++;
        }

        SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP", "", stage_info[stage_idx].name,
                                          "fwd_layer_index", &stage_info[stage_idx].fwd_layer_index, 1), "");
    }

    /*
     * update FWD1 stage seperately as layer_offsets and layer_protocols signals are not exposed for chip
     */
    sal_strncpy(stage_info[DNX_PP_TERMI_INGRESS_STAGES - 1].name,
                dnx_pp_termi_ingress_stages_str[DNX_PP_TERMI_INGRESS_STAGES - 1], RHNAME_MAX_SIZE - 1);
    SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP", "", stage_info[DNX_PP_TERMI_INGRESS_STAGES - 1].name,
                                      "fwd_layer_index", &stage_info[DNX_PP_TERMI_INGRESS_STAGES - 1].fwd_layer_index,
                                      1), "");
    for (i = 0; i < DNX_PP_TERMI_NOF_LAYERS; i++)
    {
        stage_info[DNX_PP_TERMI_INGRESS_STAGES - 1].layer_offsets[i] = -1;
        stage_info[DNX_PP_TERMI_INGRESS_STAGES - 1].layer_protocols[i] = -1;
    }
    stage_info[stage_idx].name[RHNAME_MAX_SIZE - 1] = '\0';

exit:
    sand_signal_output_free(layer_offsets_sig);
    sand_signal_output_free(layer_protocols_sig);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Construct the terminated and forward headers based on per stage information
 *
 *   \param [in] unit - unit id
 *   \param [in] next_protocol_array - array of packet headers
 *   \param [in] stage_info - all collected information per stage for VTT1-FWD1 stages
 *   \param [in] mpls_label_array - array of strings containing the MPLS labels relared to 'next_protocol_array'
 *   \param [out] outputs - data to be outputed
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_termi_construct_outputs(
    int unit,
    char next_protocol_array[SH_DNX_PP_MAX_LAYERS_NUMBER][RHNAME_MAX_SIZE],
    char mpls_label_array[SH_DNX_PP_MAX_LAYERS_NUMBER][DSIG_MAX_SIZE_UINT32],
    diag_pp_termi_stage_info_t * stage_info,
    diag_pp_termi_output_t * outputs)
{
    int stage_idx = 0;
    uint32 fwd_layer_idx = 0;
    int next_protocol_idx = 0;
    int mpls_label_idx = 0;
    int nof_mpls_term = 0;
    int mpls_idx;

    SHR_FUNC_INIT_VARS(unit);

    if (sal_strcmp(next_protocol_array[next_protocol_idx], "PTCH_2") == 0)
    {
        next_protocol_idx++;
    }

    /*
     * handle VTT1
     */
    if ((sal_strstr(next_protocol_array[next_protocol_idx], "ETH")) &&
        (stage_info[0].fwd_layer_index == 0) && (stage_info[1].fwd_layer_index != 0))
    {
        fwd_layer_idx = stage_info[0].fwd_layer_index;
        if (((stage_info[stage_idx].layer_protocols[fwd_layer_idx] !=
              (dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS)))
             && (stage_info[stage_idx].layer_protocols[fwd_layer_idx] !=
                 (dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UNTERM))))
            || (stage_info[stage_idx].layer_offsets[fwd_layer_idx] ==
                stage_info[stage_idx].layer_offsets[fwd_layer_idx + 1]))
        {
            /*
             * ETH termination - simple case
             */
            sal_strncpy(outputs->terminated_headers[0], next_protocol_array[next_protocol_idx], RHNAME_MAX_SIZE);
            next_protocol_idx++;
        }
        else
        {
            /*
             * ETH+MPLS termination - FRR
             */
            sal_snprintf(outputs->terminated_headers[0], RHNAME_MAX_SIZE, "%so%s(0x%s)",
                         next_protocol_array[next_protocol_idx], next_protocol_array[next_protocol_idx + 1],
                         mpls_label_array[mpls_label_idx]);
            next_protocol_idx += 2;
            mpls_label_idx++;
        }
    }

    /*
     * handle VTT2-5
     */
    for (stage_idx = 1; stage_idx < DNX_PP_TERMI_INGRESS_STAGES - 1; stage_idx++)
    {
        int prev_fwd_layer_idx;

        fwd_layer_idx = stage_info[stage_idx].fwd_layer_index;
        prev_fwd_layer_idx = stage_info[stage_idx - 1].fwd_layer_index;
        nof_mpls_term =
            (stage_info[stage_idx].layer_offsets[fwd_layer_idx + 1] -
             stage_info[stage_idx - 1].layer_offsets[prev_fwd_layer_idx + 1]) / 4;

        if ((stage_info[stage_idx].layer_protocols[fwd_layer_idx] ==
             (dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS))) && (nof_mpls_term > 0))
        {
            for (mpls_idx = 0; mpls_idx < nof_mpls_term; mpls_idx++)
            {
                /** previous string without current layer*/
                char previous_str[DSIG_MAX_SIZE_STR];

                if (sal_strcmp(next_protocol_array[next_protocol_idx], "MPLS") != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expecting MPLS header but got %s",
                                 next_protocol_array[next_protocol_idx]);
                }

                sal_strncpy(previous_str, outputs->terminated_headers[stage_idx], DSIG_MAX_SIZE_STR - 1);

                /** append the current layer at the beginning of the previous layer stack */
                sal_snprintf(outputs->terminated_headers[stage_idx], RHNAME_MAX_SIZE, "%s(0x%s)",
                             next_protocol_array[next_protocol_idx], mpls_label_array[mpls_label_idx]);

                if (mpls_idx != 0)
                {
                    sal_strncat(outputs->terminated_headers[stage_idx], "o", DSIG_MAX_SIZE_STR - 1);
                    sal_strncat(outputs->terminated_headers[stage_idx], previous_str, DSIG_MAX_SIZE_STR - 1);
                }

                next_protocol_idx++;
                mpls_label_idx++;
            }
        }
        else if ((stage_info[stage_idx].layer_protocols[fwd_layer_idx] !=
                  (dnx_init_pp_layer_types_value_get(unit, DBAL_ENUM_FVAL_LAYER_TYPES_MPLS)))
                 && ((fwd_layer_idx < stage_info[stage_idx + 1].fwd_layer_index)
                     || sh_dnx_pp_termi_layer_offsets_changed(unit, stage_info[stage_idx].layer_offsets,
                                                              stage_info[stage_idx + 1].layer_offsets)))
        {
            sal_strncpy(outputs->terminated_headers[stage_idx], next_protocol_array[next_protocol_idx],
                        RHNAME_MAX_SIZE);
            next_protocol_idx++;
        }
    }

    /*
     * handle FWD
     */
    if (sal_strcmp(next_protocol_array[next_protocol_idx], "MPLS") == 0)
    {
        sal_snprintf(outputs->terminated_headers[DNX_PP_TERMI_INGRESS_STAGES - 1], RHNAME_MAX_SIZE, "%s(0x%s)",
                     next_protocol_array[next_protocol_idx], mpls_label_array[mpls_label_idx]);
    }
    else
    {
        sal_strncpy(outputs->terminated_headers[DNX_PP_TERMI_INGRESS_STAGES - 1],
                    next_protocol_array[next_protocol_idx], RHNAME_MAX_SIZE);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * collect and calculate the outputed data
 *
 *   \param [in] unit - unit id
 *   \param [in] inputs - user inputs
 *   \param [out] outputs - data to be outputed
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_termi_results(
    int unit,
    diag_pp_termi_input_t inputs,
    diag_pp_termi_output_t * outputs)
{
    int core_id;
    char mpls_label_array[SH_DNX_PP_MAX_LAYERS_NUMBER][DSIG_MAX_SIZE_UINT32];
    signal_output_t *sig_list_element = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_DNX_CORES_ITER(unit, inputs.core_id, core_id)
    {
        char next_protocol_array[SH_DNX_PP_MAX_LAYERS_NUMBER][RHNAME_MAX_SIZE] = { "" };
        diag_pp_termi_stage_info_t stage_info[DNX_PP_TERMI_INGRESS_STAGES];

        SHR_IF_ERR_EXIT(dnx_pp_ingress_header_sig_get(unit, core_id, &sig_list_element));

        if (sig_list_element)
        {
            SHR_IF_ERR_EXIT(construct_packet_structure
                            (unit, sig_list_element->field_list, outputs[core_id].packet_structure_str,
                             next_protocol_array, mpls_label_array));

            /** get all per stage signals and fill 'stage_info' */
            SHR_IF_ERR_EXIT(sh_dnx_pp_termi_convert_signals_to_data(unit, core_id, stage_info));

            /** calculate 'outputs' based on signals */
            SHR_IF_ERR_EXIT(sh_dnx_pp_termi_construct_outputs
                            (unit, next_protocol_array, mpls_label_array, stage_info, &outputs[core_id]));
        }
    }

exit:
    sand_signal_output_free(sig_list_element);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Display the collected information for both cores
 *
 *   \param [in] unit - unit id
 *   \param [in] sand_control - according to diag mechanism definition
 *   \param [in] inputs - user inputs
 *   \param [in] outputs - data to be outputed
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
sh_dnx_pp_termi_display(
    int unit,
    sh_sand_control_t * sand_control,
    diag_pp_termi_input_t inputs,
    diag_pp_termi_output_t * outputs)
{
    int stage_idx, core_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_DNX_CORES_ITER(unit, inputs.core_id, core_id)
    {
        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, TRUE);

        PRT_TITLE_SET("Ingress Handled Headers");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_NONE, "Handled Headers");
        PRT_INFO_ADD("Core %d: Packet header is: %s", core_id, outputs[core_id].packet_structure_str);

        for (stage_idx = 0; stage_idx < DNX_PP_TERMI_INGRESS_STAGES - 1; stage_idx++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", dnx_pp_termi_ingress_stages_str[stage_idx]);
            PRT_CELL_SET("%s", outputs[core_id].terminated_headers[stage_idx]);
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", dnx_pp_termi_ingress_stages_str[DNX_PP_TERMI_INGRESS_STAGES - 1]);
        PRT_CELL_SET("%s", outputs[core_id].terminated_headers[DNX_PP_TERMI_INGRESS_STAGES - 1]);
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Provides the trace (changes) of the forwarding decision for the last packet in the following stages in the Ingress-PP
 *
 *   \param [in] unit - unit id
 *   \param [in] args - according to diag mechanism definition
 *   \param [in] sand_control - according to diag mechanism definition
 *
 * \return
 *   shr_error_e
 */
shr_error_e
sh_dnx_pp_termi_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_pp_termi_input_t inputs;
    diag_pp_termi_output_t *outputs_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(outputs_p, sizeof(*outputs_p) * DNX_PP_TERMI_MAX_NOF_CORES, "outputs_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     *  Get inputs to diag
     */
    SH_SAND_GET_INT32("core", inputs.core_id);

    diag_pp_termi_output_t_init(unit, outputs_p);

    /*
     *  Get requested information
     */
    SHR_IF_ERR_EXIT(sh_dnx_pp_termi_results(unit, inputs, outputs_p));

    /*
     *  Display requested information
     */
    SHR_IF_ERR_EXIT(sh_dnx_pp_termi_display(unit, sand_control, inputs, outputs_p));
exit:
    SHR_FREE(outputs_p);
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_termi_man = {
    .brief = "PP Termination Option",
    .full =
        "Provides the trace (changes) of the termination and forwarding decisions for the last packet in the following stages in the Ingress-PP.\n"
        "Core ID is supported.\n",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "core=0\n" "SHORT",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};

sh_sand_option_t dnx_pp_termi_options[] = {
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"}
    ,
    {NULL}
};
