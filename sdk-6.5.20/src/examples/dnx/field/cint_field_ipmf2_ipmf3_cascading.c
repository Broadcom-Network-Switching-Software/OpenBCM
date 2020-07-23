/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * Configuration example start:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint ../../../src/examples/dnx/field/cint_field_ipmf2_ipmf3_cascading.c
 * cint;
 * int unit = 0;
 * bcm_port_t in_port = 201;
 * cint_field_cascaded_main(unit,in_port);
 *
 * Configuration example end
 *
 * Configuring field processor to perform IPMF2-IPMF3 cascading.
 * Configure 2 field groups:
 *     IPMF2 FG:
 *         Qualify by SrcPort and perform Container action with following
 *         value 0xFFFFFFFF.
 *
 *     IPMF3 FG:
 *         Qualify by Container, expecting to receive the result of the
 *         IPMF2 FG action resolution (0xFFFFFFFF). In case of match
 *         PrioIntNew action will be performed with value 0x4.
 *
 *     The Container qualifier and action, are used to parse information between PMFs.
 */

/* *INDENT-OFF* */

int NOF_FIELD_GROUPS            = 2;
bcm_field_group_t IPMF2_FG_ID   = -1;
bcm_field_group_t IPMF3_FG_ID   = -1;
bcm_field_entry_t IPMF2_ENTRY_HANDLE       = 0;
bcm_field_entry_t IPMF3_ENTRY_HANDLE       = 0;

bcm_field_presel_t cint_field_ipmf2_ipmf3_cascading_presel_id = 50;

struct cint_field_cascaded_group_and_attach_info_t {
    int nof_quals;
    int nof_actions;
    field_util_basic_tcam_fg_t tcam_fg_info;
};

/** Array which contains all need information for IPMF2-IPMF3 cascading. */
cint_field_cascaded_group_and_attach_info_t cascaded_fg_info[NOF_FIELD_GROUPS] = {
    /* IPMF2 cascaded info */
    {
        1, /* Number of qualifiers */
        1, /* Number of actions */
        {
            IPMF2_FG_ID, /* Field Group ID */
            BCM_FIELD_CONTEXT_ID_DEFAULT, /* Context Id */
            IPMF2_ENTRY_HANDLE, /* Entry Handle Id */
            bcmFieldStageIngressPMF2, /* Field Stage */
            {
                /* Array of qualifiers */
                bcmFieldQualifySrcPort
            },
            {
                /* Array of qualifier info (input_type, input_arg, offset) */
                {bcmFieldInputTypeMetaData, 0, 0}
            },
            {
                /* Array of qualifier values */
                {{0x6c0000c8}}
            },
            {
                /* Array of actions */
                bcmFieldActionContainer
            },
            {
                /* Array of action values */
                {{0xFFFFFFFF}}
            }
        }
    },
    /* IPMF3 cascaded info */
    {
        1, /* Number of qualifiers */
        2, /* Number of actions */
        {
            IPMF3_FG_ID, /* Field Group ID */
            BCM_FIELD_CONTEXT_ID_DEFAULT, /* Context Id */
            IPMF3_ENTRY_HANDLE, /* Entry Handle Id */
            bcmFieldStageIngressPMF3, /* Field Stage */
            {
                /* Array of qualifiers */
                bcmFieldQualifyContainer
            },
            {
                /* Array of qualifier info (input_type, input_arg, offset) */
                {bcmFieldInputTypeMetaData, 0, 0}
            },
            {
                /* Array of qualifier values */
                {{0xFFFFFFFF}}
            },
            {
                /* Array of actions */
                bcmFieldActionPrioIntNew,
                bcmFieldActionForward
            },
            {
                /* Array of action values */
                {{0x4}},
                {{0xc9}}
            }
        }
    }
};

/* *INDENT-ON* */

int
cint_field_cascaded_group_config(
    int unit,
    bcm_port_t in_port,
    cint_field_cascaded_group_and_attach_info_t * cascading_fg_info)
{
    int rv = BCM_E_NONE;
    int qual_index, action_index;
    field_util_basic_tcam_fg_t tcam_fg_util;
    bcm_field_stage_t stage;

    stage = (cascading_fg_info->tcam_fg_info.stage == bcmFieldStageIngressPMF2) ? bcmFieldStageIngressPMF1 : cascading_fg_info->tcam_fg_info.stage;

    rv = field_presel_fwd_layer_main(unit, cint_field_ipmf2_ipmf3_cascading_presel_id, stage,
                                     bcmFieldLayerTypeEth, &cascading_fg_info->tcam_fg_info.context, "ipmf2_3_cascade");
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_presel_fwd_layer_main \n", rv);
        return rv;
    }

    field_util_basic_tcam_fg_t_init(unit, &tcam_fg_util);

    tcam_fg_util.stage = cascading_fg_info->tcam_fg_info.stage;
    tcam_fg_util.fg_id = cascading_fg_info->tcam_fg_info.fg_id;
    tcam_fg_util.context = cascading_fg_info->tcam_fg_info.context;
    tcam_fg_util.entry_handle = cascading_fg_info->tcam_fg_info.entry_handle;
    for (qual_index = 0; qual_index < cascading_fg_info->nof_quals; qual_index++)
    {
        if (cascading_fg_info->tcam_fg_info.bcm_qual[qual_index] == bcmFieldQualifySrcPort)
        {
            bcm_gport_t gport_in;
            BCM_GPORT_LOCAL_SET(gport_in, in_port);
            cascading_fg_info->tcam_fg_info.qual_values[qual_index].value[0] = gport_in;
        }
        tcam_fg_util.bcm_qual[qual_index] = cascading_fg_info->tcam_fg_info.bcm_qual[qual_index];
        tcam_fg_util.qual_values[qual_index].value[0] =
            cascading_fg_info->tcam_fg_info.qual_values[qual_index].value[0];
        tcam_fg_util.qual_info[qual_index].input_type =
            cascading_fg_info->tcam_fg_info.qual_info[qual_index].input_type;
        tcam_fg_util.qual_info[qual_index].input_arg = cascading_fg_info->tcam_fg_info.qual_info[qual_index].input_arg;
        tcam_fg_util.qual_info[qual_index].offset = cascading_fg_info->tcam_fg_info.qual_info[qual_index].offset;
    }

    for (action_index = 0; action_index < cascading_fg_info->nof_actions; action_index++)
    {
        tcam_fg_util.bcm_actions[action_index] = cascading_fg_info->tcam_fg_info.bcm_actions[action_index];
        tcam_fg_util.action_values[action_index].value[0] =
            cascading_fg_info->tcam_fg_info.action_values[action_index].value[0];
    }

    rv = field_util_basic_tcam_fg_full(unit, &tcam_fg_util);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_util_basic_tcam_fg_full\n", rv);
        return rv;
    }

    /*
     * Return the created entry_handle to be destroyed in the destroy function. 
     */
    cascading_fg_info->tcam_fg_info.fg_id = tcam_fg_util.fg_id;
    cascading_fg_info->tcam_fg_info.entry_handle = tcam_fg_util.entry_handle;

    return rv;
}

/** Main function to configure IPMF2-IPMF3 cascading. */
int
cint_field_cascaded_main(
    int unit,
    bcm_port_t in_port)
{
    int rv = BCM_E_NONE;
    int ii;

    for (ii = 0; ii < NOF_FIELD_GROUPS; ii++)
    {
        rv = cint_field_cascaded_group_config(unit, in_port, cascaded_fg_info[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in field_group_config\n", rv);
            return rv;
        }
    }

    return rv;
}

/** Destroys all allocated data for the IPMF2-IPMF3 cascading. */
int
cint_field_cascaded_destroy(
    int unit)
{
    int rv = BCM_E_NONE;
    int ii;
    bcm_field_stage_t stage;

    for (ii = 0; ii < NOF_FIELD_GROUPS; ii++)
    {
        rv = bcm_field_entry_delete(unit, cascaded_fg_info[ii].tcam_fg_info.fg_id, NULL,
                                        cascaded_fg_info[ii].tcam_fg_info.entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_dnx_field_entry_delete\n", rv);
            return rv;
        }

        rv = bcm_field_group_context_detach(unit, cascaded_fg_info[ii].tcam_fg_info.fg_id,
                                                cascaded_fg_info[ii].tcam_fg_info.context);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_dnx_field_group_context_detach\n", rv);
            return rv;
        }

        rv = bcm_field_group_delete(unit, cascaded_fg_info[ii].tcam_fg_info.fg_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_dnx_field_group_delete\n", rv);
            return rv;
        }

        stage = (cascaded_fg_info[ii].tcam_fg_info.stage == bcmFieldStageIngressPMF2) ? bcmFieldStageIngressPMF1 : cascaded_fg_info[ii].tcam_fg_info.stage;

        rv = field_presel_fwd_layer_destroy(unit, cint_field_ipmf2_ipmf3_cascading_presel_id, stage,
                                            cascaded_fg_info[ii].tcam_fg_info.context);
        if (rv != BCM_E_NONE)
        {
            printf("Error in field_presel_fwd_layer_destroy. \n");
            return rv;
        }
    }

    return rv;
}
