/** \file tdm_ingress_context.c
 * 
 *
 * TDM ingress context procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TDM

/*
 * Include
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tdm.h>

#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/algo_tdm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tdm_access.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/common/multicast.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Defines.
 * {
 */
/**
 * \brief - supported flags for API bcm_dnx_fabric_dnx()
 */
#define DNX_TDM_INGRESS_CONTEXT_CREATE_SUPPORTED_FLAGS \
             (BCM_TDM_INGRESS_CONTEXT_SPECIFIC_FABRIC_LINKS | BCM_TDM_INGRESS_CONTEXT_WITH_ID | BCM_TDM_INGRESS_CONTEXT_REPLACE)
#define DNX_TDM_INGRESS_CONTEXT_DESTROY_SUPPORTED_FLAGS         (0)
#define DNX_TDM_INGRESS_CONTEXT_GET_SUPPORTED_FLAGS             (0)
#define DNX_TDM_STREAM_INGRESS_CONTEXT_SUPPORTED_FLAGS          (0)
#define DNX_TDM_INTERFACE_CONFIG_SUPPORTED_FLAGS                (0)
#define DNX_TDM_INGRESS_CONTEXT_FAILOVER_SUPPORTED_FLAGS        (0)

/*
 * In MC context PP_DSP should be set to 0xFF
 */
#define TDM_FTMH_PP_DSP_FOR_MC_CONTEXT                       0xFF
/*
 * IF there will be differences in number between devices - move to dnx data
 */
#define TDM_CONTEXT_DROP_REG_SIZE                        512
#define TDM_CONTEXT_DROP_REG_SIZE_WORDS                 _SHR_BITDCLSIZE(TDM_CONTEXT_DROP_REG_SIZE)

/*
 * Number of bits extracted from the packet when working in sid from packet mode
 * (sid is stamped from configured offset in the packet)
 */
#define DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE             (16)
/*
 * }
 */

/**
 * \brief - Fill/Update link mask entry with mask
 *
 * \param [in] unit - Relevant unit
 * \param [in] link_mask_id - Link Mask entry id to be written into the HW
 * \param [in] tdm_link_mask - Pointer to link mask to be written
 *
 * \return
 *   standard shr_error_e
 */
static shr_error_e
dnx_tdm_link_mask_write(
    int unit,
    int link_mask_id,
    dnx_tdm_link_mask_t * tdm_link_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_DIRECT_LINKS,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_LINK_MASK_ID, link_mask_id,
                                     GEN_DBAL_ARR32, DBAL_FIELD_LINK_MASK_DATA, INST_SINGLE,
                                     tdm_link_mask->link_mask_data, GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Allocate/Update link mask template and set HW for the 1st reference
 *
 * \param [in] unit - Relevant unit
 * \param [in] ingress_context - Ingress Context info from which links array to be configured is taken
 * \param [in] old_link_mask_id - Previous Link Mask entry id
 * \param [out] link_mask_id_p - Link Mask id to be returned for context configuration
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   dnx_tdm_link_mask_reset
 */
static shr_error_e
dnx_tdm_link_mask_update(
    int unit,
    bcm_tdm_ingress_context_t * ingress_context,
    int old_link_mask_id,
    int *link_mask_id_p)
{
    uint8 new_is_first, old_is_last;
    dnx_tdm_link_mask_t tdm_link_mask;
    int i_link, fabric_link_id;
    int new_link_mask_id;

    SHR_FUNC_INIT_VARS(unit);
    {
        /*
         * First nullify data array
         */
        sal_memset(&tdm_link_mask, 0, sizeof(dnx_tdm_link_mask_t));
        /*
         * Generate mask from links list
         */
        for (i_link = 0; i_link < ingress_context->fabric_ports_count; i_link++)
        {
            /*
             * Get fabric link ID from logical port.
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get
                            (unit, ingress_context->fabric_ports[i_link], &fabric_link_id));
            /*
             * Set the bit of relevant data
             */
            SHR_BITSET(tdm_link_mask.link_mask_data, fabric_link_id);
            /*
             * Move link_mask_data array32 to BITMAP
             */
        }
    }
    /*
     * Exchange profile
     */
    SHR_IF_ERR_EXIT(algo_tdm_db.link_mask.exchange(unit, 0, &tdm_link_mask, old_link_mask_id, NULL, &new_link_mask_id,
                                                   &new_is_first, &old_is_last));
    /*
     * IF it is first reference - initiate HW
     */
    if (new_is_first == 1)
    {
        SHR_IF_ERR_EXIT(dnx_tdm_link_mask_write(unit, new_link_mask_id, &tdm_link_mask));
    }
    if (old_is_last == TRUE)
    {
        /*
         * Nullify HW entry for consistency purposes
         */
        sal_memset(&tdm_link_mask, 0, sizeof(dnx_tdm_link_mask_t));

        SHR_IF_ERR_EXIT(dnx_tdm_link_mask_write(unit, old_link_mask_id, &tdm_link_mask));
    }
    /*
     * Return to user new d if it was requested
     */
    if (link_mask_id_p != NULL)
    {
        *link_mask_id_p = new_link_mask_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Obtain link mask
 *
 * \param [in] unit - Relevant unit
 * \param [in] link_mask_id - Link Mask id for which link mask will be fetched
 * \param [out] ingress_context - Ingress Context to be filled with link mask
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   dnx_tdm_link_mask_create/dnx_tdm_link_mask_destroy
 */
static shr_error_e
dnx_tdm_link_mask_get(
    int unit,
    int link_mask_id,
    bcm_tdm_ingress_context_t * ingress_context)
{
    dnx_tdm_link_mask_t tdm_link_mask;
    int fabric_link_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * First nullify data array
     */
    sal_memset(&tdm_link_mask, 0, sizeof(dnx_tdm_link_mask_t));
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_DIRECT_LINKS,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_LINK_MASK_ID, link_mask_id,
                                     GEN_DBAL_ARR32, DBAL_FIELD_LINK_MASK_DATA, INST_SINGLE,
                                     tdm_link_mask.link_mask_data, GEN_DBAL_FIELD_LAST_MARK));
    ingress_context->fabric_ports_count = 0;
    /*
     * Generate links list from mask
     */
    for (fabric_link_id = 0; fabric_link_id < dnx_data_fabric.links.nof_links_get(unit); fabric_link_id++)
    {
        if (SHR_BITGET(tdm_link_mask.link_mask_data, fabric_link_id))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, fabric_link_id,
                                                                     &ingress_context->fabric_ports
                                                                     [ingress_context->fabric_ports_count]));
            ingress_context->fabric_ports_count++;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_ingress_context_config_result_type(
    int unit,
    uint32 *result_type_p)
{
    uint32 use_optimized_ftmh;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tdm_system_ftmh_type_get(unit, &use_optimized_ftmh));
    if (use_optimized_ftmh == TRUE)
    {
        result_type = DBAL_RESULT_TYPE_TDM_CONTEXT_CONFIG_OPTIMIZED_FTMH;
    }
    else
    {
        int system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            result_type = DBAL_RESULT_TYPE_TDM_CONTEXT_CONFIG_STANDARD_FTMH;
        }
        else
        {
            result_type = DBAL_RESULT_TYPE_TDM_CONTEXT_CONFIG_STANDARD_FTMH_J1;
        }
    }
    *result_type_p = result_type;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_tdm_ingress_context_*
 */
static shr_error_e
dnx_tdm_ingress_context_create_verify(
    int unit,
    uint32 flags,
    int *ingress_context_id,
    bcm_tdm_ingress_context_t * ingress_context)
{
    int i_link;
    int user_defined_data_size;
    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_NULL_CHECK(ingress_context, _SHR_E_PARAM, "ingress_context");
    SHR_NULL_CHECK(ingress_context_id, _SHR_E_PARAM, "ingress_context_id");
    SHR_BOOL_VERIFY(ingress_context->is_mc, _SHR_E_PARAM, "ingress_context->is_mc");
    SHR_BOOL_VERIFY(ingress_context->stamp_mcid_with_sid, _SHR_E_PARAM, "ingress_context->stamp_mcid_with_sid");

    SHR_MASK_VERIFY(flags, DNX_TDM_INGRESS_CONTEXT_CREATE_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    if ((ingress_context->type != bcmTdmIngressEditingPrepend) &&
        (ingress_context->type != bcmTdmIngressEditingNoChange))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported bcmTdmIngressEditingType for ingress:%d", ingress_context->type);
    }
    /*
     * Verify that together with REPLACE flag valid context id is provided
     */
    if (flags & BCM_TDM_INGRESS_CONTEXT_REPLACE)
    {
        uint8 is_allocated;

        SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, *ingress_context_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "REPLACE flag allocated id, %d is not\n", *ingress_context_id);
        }
    }
    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist) &&
        (ingress_context->fabric_ports_count != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No fabric ports for device without fabric\n");
    }
    /*
     * Verify that all ports in the list are FABRIC ones
     */
    for (i_link = 0; i_link < ingress_context->fabric_ports_count; i_link++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_verify(unit, ingress_context->fabric_ports[i_link],
                                                  DNX_ALGO_PORT_TYPE_FABRIC));
    }
    /*
     * Verify that user data fits DBAL_FIELD_USER_DEFINED_DATA size
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_TDM_CONTEXT_CONFIG, DBAL_FIELD_USER_DEFINED_DATA,
                                               FALSE, 0, 0, &user_defined_data_size));
    if (ingress_context->user_data_count > user_defined_data_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Used Data size:%d is bigger than supported:%d\n",
                     ingress_context->user_data_count, user_defined_data_size);
    }

    if (ingress_context->is_mc == FALSE)
    {
        bcm_gport_t gport = ingress_context->destination_port;
        /*
         * MC ID stamping is valid only for multicast context
         */
        if (ingress_context->stamp_mcid_with_sid != FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "STAMP MC ID not relevant for UNICAST Context");
        }

        if (BCM_GPORT_IS_MODPORT(gport))
        {
            int ftmh_pp_dsp = BCM_GPORT_MODPORT_PORT_GET(gport);
            int modid = BCM_GPORT_MODPORT_MODID_GET(gport);

            SHR_RANGE_VERIFY(ftmh_pp_dsp, 0, dnx_data_port.general.nof_tm_ports_per_fap_id_get(unit) - 1, _SHR_E_PARAM,
                             "ftmh_pp_dsp %d in modport is out of range\n", ftmh_pp_dsp);
            SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                             "modid %d in modport is out of range\n", modid);
        }
        else if (BCM_GPORT_IS_LOCAL(gport))
        {
            bcm_port_t logical_port = BCM_GPORT_LOCAL_GET(gport);
            SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
        }
        else if (!BCM_GPORT_IS_SET(gport))
        {
                    /** assume it logical_port */
            bcm_port_t logical_port = gport;
            SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid gport type");
        }
    }
    else
    {
        /*
         * Multicast ID should stay 0 for stamping context
         */
        if ((ingress_context->stamp_mcid_with_sid == TRUE) && (ingress_context->multicast_id != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MC ID should be zero when sid stamping is enabled");
        }
        /*
         * No verification otherwise on MC_ID, because in TDM case group is 0
         */
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set tdm context configuration per stream
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc Flags
 * \param [in] ingress_context_id_p - ID of ingress context
 * \param [out] ingress_context - TDM ingress_context configuration
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tdm_ingress_context_create(
    int unit,
    uint32 flags,
    int *ingress_context_id_p,
    bcm_tdm_ingress_context_t * ingress_context)
{
    dbal_enum_value_result_type_tdm_context_config_e result_type;
    uint32 entry_handle_id;
    int ingress_context_id;
    int cur_link_mask_id, new_link_mask_id;
    bcm_tdm_ingress_context_t loc_ingress_context;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memcpy(&loc_ingress_context, ingress_context, sizeof(loc_ingress_context));
    SHR_INVOKE_VERIFY_DNX(dnx_tdm_ingress_context_create_verify
                          (unit, flags, ingress_context_id_p, &loc_ingress_context));
    /*
     * Replace means: Already active context whose contents need to be updated
     */
    if (!(flags & BCM_TDM_INGRESS_CONTEXT_REPLACE))
    {
        uint32 algo_flags;
        if (flags & BCM_TDM_INGRESS_CONTEXT_WITH_ID)
        {
            algo_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }
        else
        {
            algo_flags = 0;
        }
        SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.allocate_single(unit, algo_flags, NULL, ingress_context_id_p));
    }
    ingress_context_id = *ingress_context_id_p;
    /*
     * Figure out result type
     */
    SHR_IF_ERR_EXIT(dnx_tdm_ingress_context_config_result_type(unit, &result_type));
    /*
     * Now get the old link link mask id and replace is by new
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONTEXT_CONFIG, &entry_handle_id));
    /*
     * If there is fabric handle links_mask - direct links
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_CONTEXT_ID, ingress_context_id);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        /*
         * Fetch existing on uninitiated entries it should be default 0
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_LINK_MASK_ID, INST_SINGLE,
                                   (uint32 *) &cur_link_mask_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if ((flags & BCM_TDM_INGRESS_CONTEXT_SPECIFIC_FABRIC_LINKS) == 0)
        {
            /*
             * If BCM_TDM_INGRESS_CONTEXT_SPECIFIC_FABRIC_LINKS bit is not set then override input contents
             * of 'loc_ingress_context.fabric_ports[]' and 'loc_ingress_context.fabric_ports_count': Load
             * it with all available fabric links.
             */
            bcm_port_config_t all_ports_config;
            bcm_port_t fabric_logical_port;

            loc_ingress_context.fabric_ports_count = 0;
            SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &all_ports_config));
            BCM_PBMP_ITER(all_ports_config.sfi, fabric_logical_port)
            {
                loc_ingress_context.fabric_ports[loc_ingress_context.fabric_ports_count] = fabric_logical_port;
                loc_ingress_context.fabric_ports_count++;
            }
        }
        /*
         * Replace old link_mask by new one
         */
        SHR_IF_ERR_EXIT(dnx_tdm_link_mask_update(unit, &loc_ingress_context, cur_link_mask_id, &new_link_mask_id));
        /*
         * Now write the entry
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_TDM_CONTEXT_CONFIG, entry_handle_id));
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_CONTEXT_ID, ingress_context_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_LINK_MASK_ID, INST_SINGLE, new_link_mask_id);
    }
    /*
     * DO we need to prepend FTMH header or don't touch anything
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PREPEND_FTMH, INST_SINGLE,
                                 ((loc_ingress_context.type == bcmTdmIngressEditingPrepend) ? TRUE : FALSE));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAMP_SID,
                                 INST_SINGLE, loc_ingress_context.stamp_mcid_with_sid);
    if (result_type != DBAL_RESULT_TYPE_TDM_CONTEXT_CONFIG_OPTIMIZED_FTMH)
    {
        /*
         * For NON-optimized header we must set separately is_mc bit to 1. For 'optimized', DBAL takes care
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, INST_SINGLE, loc_ingress_context.is_mc);
        /*
         * For NON-optimized header, add user data. (Note that in 'optimized' header, there is no user defined data) ; 
         */
        dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_USER_DEFINED_DATA, INST_SINGLE,
                                        loc_ingress_context.user_data);
    }
    if (loc_ingress_context.is_mc)
    {
        uint32 mc_id;
        /*
         * Fetch mc_id from loc_ingress_context.multicast_id
         * We need multicast id only if there is no Stream ID stamping, but set MC_ID anyway to signal in optimized
         * that it is multicast context
         */
        mc_id = _BCM_MULTICAST_ID_GET(loc_ingress_context.multicast_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE, mc_id);
        if (result_type != DBAL_RESULT_TYPE_TDM_CONTEXT_CONFIG_OPTIMIZED_FTMH)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MOD_PORT, INST_SINGLE,
                                         TDM_FTMH_PP_DSP_FOR_MC_CONTEXT);
        }
    }
    else
    {
        int tm_port;
        bcm_port_t logical_port;
        bcm_core_t core;
        int modid;
        int ftmh_pp_dsp;
        uint32 modport = 0;
        bcm_gport_t gport = loc_ingress_context.destination_port;
        /*
         * Extract modid + FTMH PP DSP from gport
         */
        if (BCM_GPORT_IS_MODPORT(gport))
        {
            ftmh_pp_dsp = BCM_GPORT_MODPORT_PORT_GET(gport);
            modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        }
        else
        {
            /** local port gport or just a port */
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
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MOD_PORT, DBAL_FIELD_FTMH_PP_DSP,
                                                        (uint32 *) &ftmh_pp_dsp, &modport));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_MOD_PORT, DBAL_FIELD_FAP_ID,
                                                        (uint32 *) &modid, &modport));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MOD_PORT, INST_SINGLE, modport);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_tdm_ingress_context_*
 */
static shr_error_e
dnx_tdm_ingress_context_destroy_verify(
    int unit,
    uint32 flags,
    int ingress_context_id)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_MASK_VERIFY(flags, DNX_TDM_INGRESS_CONTEXT_DESTROY_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, ingress_context_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Destroy of unallocated ingress context:%d\n", ingress_context_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy tdm context
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc Flags - currently unused
 * \param [in] ingress_context_id - ID of ingress context
 *
 * \remark
 *      No verification on whether this context is assigned to any stream or not
 * \return
 *   int
 * \see
 *   * bcm_dnx_tdm_ingress_context_create
 */
int
bcm_dnx_tdm_ingress_context_destroy(
    int unit,
    uint32 flags,
    int ingress_context_id)
{
    uint32 result_type;
    uint32 entry_handle_id;
    int link_mask_id;
    bcm_tdm_ingress_context_t ingress_context;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_tdm_ingress_context_destroy_verify(unit, flags, ingress_context_id));
    /*
     * Read link_mask_id from context
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONTEXT_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_CONTEXT_ID, ingress_context_id);
    /*
     * Set result type
     */
    SHR_IF_ERR_EXIT(dnx_tdm_ingress_context_config_result_type(unit, &result_type));
    /*
     * If there is fabric free links_mask profile
     */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM_LINK_MASK_ID, INST_SINGLE,
                                   (uint32 *) &link_mask_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Generate NULL context wih NULL Link_Mask to allow use of link_mask_update
         */
        bcm_tdm_ingress_editing_context_t_init(&ingress_context);
        /*
         * Replace existing link_mask_id by NULL one - done inside
         */
        SHR_IF_ERR_EXIT(dnx_tdm_link_mask_update(unit, &ingress_context, link_mask_id, NULL));
    }
    /** free context id in allocation manager */
    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.free_single(unit, ingress_context_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_TDM_CONTEXT_CONFIG, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_CONTEXT_ID, ingress_context_id);
    /** Clear hardware entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_tdm_ingress_context_*
 */
static shr_error_e
dnx_tdm_ingress_context_get_verify(
    int unit,
    uint32 flags,
    int ingress_context_id,
    bcm_tdm_ingress_context_t * ingress_context)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_NULL_CHECK(ingress_context, _SHR_E_PARAM, "ingress_context");

    SHR_MASK_VERIFY(flags, DNX_TDM_INGRESS_CONTEXT_GET_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, ingress_context_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Get unallocated ingress context:%d\n", ingress_context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get tdm context configuration per stream
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc Flags - currently unused
 * \param [in] ingress_context_id - ID of ingress context
 * \param [out] ingress_context - TDM ingress_context configuration
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tdm_ingress_context_get(
    int unit,
    uint32 flags,
    int ingress_context_id,
    bcm_tdm_ingress_context_t * ingress_context)
{
    uint32 result_type;
    uint32 entry_handle_id;
    int link_mask_id;
    uint32 prepend_ftmh;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_tdm_ingress_context_get_verify(unit, flags, ingress_context_id, ingress_context));

    bcm_tdm_ingress_editing_context_t_init(ingress_context);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONTEXT_CONFIG, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_CONTEXT_ID, ingress_context_id);
    /*
     * Set result type
     */
    SHR_IF_ERR_EXIT(dnx_tdm_ingress_context_config_result_type(unit, &result_type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_PREPEND_FTMH, INST_SINGLE,
                                                        &prepend_ftmh));
    ingress_context->type = (prepend_ftmh == TRUE) ? bcmTdmIngressEditingPrepend : bcmTdmIngressEditingNoChange;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_STAMP_SID, INST_SINGLE,
                                                        (uint32 *) &ingress_context->stamp_mcid_with_sid));
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TDM_LINK_MASK_ID,
                                                            INST_SINGLE, (uint32 *) &link_mask_id));

        SHR_IF_ERR_EXIT(dnx_tdm_link_mask_get(unit, link_mask_id, ingress_context));
    }
    if (result_type == DBAL_RESULT_TYPE_TDM_CONTEXT_CONFIG_OPTIMIZED_FTMH)
    {
        /*
         * Define tdm_destination & tdm_field_value as an arrays to avoid coverity complain
         */
        uint32 tdm_destination[1], tdm_field_value[1];
        dbal_fields_e field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_TDM_DESTINATION,
                                                            INST_SINGLE, tdm_destination));
        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, DBAL_FIELD_TDM_DESTINATION, tdm_destination,
                                                        &field_id, tdm_field_value));
        if (field_id == DBAL_FIELD_MOD_PORT)
        {
            ingress_context->is_mc = FALSE;
        }
        else
        {
            ingress_context->is_mc = TRUE;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_IS_MC, INST_SINGLE,
                                                            (uint32 *) &ingress_context->is_mc));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_USER_DEFINED_DATA,
                                                               INST_SINGLE, ingress_context->user_data));
    }
    if (ingress_context->is_mc == FALSE)
    {
        uint32 modport;
        int modid;
        int ftmh_pp_dsp;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MOD_PORT, INST_SINGLE,
                                                            (uint32 *) &modport));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_MOD_PORT, DBAL_FIELD_FTMH_PP_DSP,
                                                        (uint32 *) &ftmh_pp_dsp, &modport));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_MOD_PORT, DBAL_FIELD_FAP_ID,
                                                        (uint32 *) &modid, &modport));
        /*
         * Only MODPORT will be returned
         */
        BCM_GPORT_MODPORT_SET(ingress_context->destination_port, modid, ftmh_pp_dsp);
    }
    else
    {
        uint32 mc_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MC_ID, INST_SINGLE,
                                                            (uint32 *) &mc_id));
        _BCM_MULTICAST_GROUP_SET(ingress_context->multicast_id, 0, mc_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_tdm_stream_ingress_context_set
 */
static shr_error_e
dnx_tdm_stream_ingress_context_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int ingress_context_id)
{
    int stream_id;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_MASK_VERIFY(flags, DNX_TDM_STREAM_INGRESS_CONTEXT_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");
    if (!BCM_GPORT_IS_TDM_STREAM(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GPORT:0x%08x is not a TDM STREAM\n", gport);
    }
    stream_id = BCM_GPORT_TDM_STREAM_ID_GET(gport);
    /*
     * Verify stream_id validity
     */
    if ((stream_id < 0) || (stream_id >= dnx_data_tdm.params.nof_stream_ids_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID:%d is out of valid range:0-%d\n",
                     stream_id, dnx_data_tdm.params.nof_stream_ids_get(unit) - 1);
    }
    /*
     * Verify that ingress context was set
     */
    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, ingress_context_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress Context ID:%d was not created\n", ingress_context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Assign ingress context per TDM Bypass stream
 *
 * \param [in] unit               - Relevant unit
 * \param [in] flags              - Misc. flags (see DNX_TDM_STREAM_INGRESS_CONTEXT_SUPPORTED_FLAGS)
 * \param [in] gport              - Currently only Stream ID supported, CPU will be next
 * \param [in] ingress_context_id - Ingress Context ID, previously created by ingress_context_create
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   bcm_dnx_tdm_stream_ingress_context_get
 */
int
bcm_dnx_tdm_stream_ingress_context_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int ingress_context_id)
{
    int stream_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_tdm_stream_ingress_context_set_verify(unit, flags, gport, ingress_context_id));
    
    stream_id = BCM_GPORT_TDM_STREAM_ID_GET(gport);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_CONTEXT_MAP,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_STREAM_ID, stream_id,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_CONTEXT_ID, INST_SINGLE, ingress_context_id,
                                     GEN_DBAL_FIELD_LAST_MARK));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_tdm_stream_ingress_context_set
 */
static shr_error_e
dnx_tdm_stream_ingress_context_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *ingress_context_id)
{
    int stream_id;
    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_MASK_VERIFY(flags, DNX_TDM_STREAM_INGRESS_CONTEXT_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");
    if (!BCM_GPORT_IS_TDM_STREAM(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GPORT:0x%08x is not a TDM STREAM\n", gport);
    }
    stream_id = BCM_GPORT_TDM_STREAM_ID_GET(gport);
    /*
     * Verify stream_id validity
     */
    if ((stream_id < 0) || (stream_id >= dnx_data_tdm.params.nof_stream_ids_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID:%d is out of valid range:0-%d\n",
                     stream_id, dnx_data_tdm.params.nof_stream_ids_get(unit) - 1);
    }

    SHR_NULL_CHECK(ingress_context_id, _SHR_E_PARAM, "ingress_context_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Obtain ingress context, assigned to TDM Bypass stream
 *
 * \param [in] unit                - Relevant unit
 * \param [in] flags               - Misc. flags (see DNX_TDM_STREAM_INGRESS_CONTEXT_SUPPORTED_FLAGS)
 * \param [in] gport               - Currently only Stream ID supported, CPU to be next
 * \param [out] ingress_context_id - Pointer Ingress Context ID, to be assign by this routine
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   bcm_dnx_tdm_stream_ingress_context_set
 */
int
bcm_dnx_tdm_stream_ingress_context_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *ingress_context_id)
{
    uint8 is_allocated;
    int stream_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_tdm_stream_ingress_context_get_verify(unit, flags, gport, ingress_context_id));
    
    stream_id = BCM_GPORT_TDM_STREAM_ID_GET(gport);
    SHR_IF_ERR_EXIT(dnx_dbal_gen_get(unit, DBAL_TABLE_TDM_CONTEXT_MAP,
                                     1, 1,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_STREAM_ID, stream_id,
                                     GEN_DBAL_FIELD32, DBAL_FIELD_TDM_CONTEXT_ID, INST_SINGLE,
                                     (uint32 *) ingress_context_id, GEN_DBAL_FIELD_LAST_MARK));
    /*
     * Verify that ingress context was set
     */
    SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, *ingress_context_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress Context ID:%d set for Stream ID:%d is not valid\n",
                     *ingress_context_id, stream_id);
    }
    
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_tdm_ingress_context_failover_verify(
    int unit,
    uint32 flags,
    int nof_contexts,
    int *context_ids,
    int *context_en)
{
    int max_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_NULL_CHECK(context_ids, _SHR_E_PARAM, "context_ids");
    SHR_NULL_CHECK(context_en, _SHR_E_PARAM, "context_en");

    max_nof_contexts = dnx_data_tdm.params.nof_contexts_get(unit);

    if (nof_contexts > max_nof_contexts)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of contexts:%d is over max:%d\n", nof_contexts, max_nof_contexts);
    }

    SHR_MASK_VERIFY(flags, DNX_TDM_INGRESS_CONTEXT_FAILOVER_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable Drop per ingress context
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc. flags (see DNX_TDM_INGRESS_CONTEXT_FAILOVER_SUPPORTED_FLAGS)
 * \param [in] nof_contexts - Number of contexts, which drop state should be modified
 *                              array size for context_ids/context_en
 * \param [in] context_ids - Array of context ids
 * \param [in] context_en - Array of drop enable/disable per context id
 *
 * \return
 *   standard shr_error_e
 *
 * \remark
 *      Only when all contexts have either below the TDM_CONTEXT_DROP_REG_SIZE id or above it,
 *      atomic operation is possible
 *
 * \see
 *   bcm_dnx_tdm_ingress_context_failover_get
 */
int
bcm_dnx_tdm_ingress_context_failover_set(
    int unit,
    uint32 flags,
    int nof_contexts,
    int *context_ids,
    int *context_en)
{
    uint32 entry_handle_id;
    int i_cxt;
    uint32 enables_lsb[TDM_CONTEXT_DROP_REG_SIZE_WORDS];
    uint32 enables_msb[TDM_CONTEXT_DROP_REG_SIZE_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_tdm_ingress_context_failover_verify(unit, flags, nof_contexts, context_ids, context_en));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONTEXT_DROP, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_CONTEXT_DROP_LSB,
                                                            INST_SINGLE, enables_lsb));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_CONTEXT_DROP_MSB,
                                                            INST_SINGLE, enables_msb));
    for (i_cxt = 0; i_cxt < nof_contexts; i_cxt++)
    {
        uint8 is_allocated;
        int cur_index;
        uint32 *enables;

        SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, context_ids[i_cxt], &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Ingress Context ID:%d was not created\n", context_ids[i_cxt]);
        }
        else if (context_ids[i_cxt] < TDM_CONTEXT_DROP_REG_SIZE)
        {
            cur_index = context_ids[i_cxt];
            enables = enables_lsb;
        }
        else
        {
            cur_index = context_ids[i_cxt] - TDM_CONTEXT_DROP_REG_SIZE;
            enables = enables_msb;
        }
        SHR_BITWRITE(enables, cur_index, context_en[i_cxt]);
    }
    
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_TDM_CONTEXT_DROP, entry_handle_id));
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_DROP_LSB, INST_SINGLE, enables_lsb);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_DROP_MSB, INST_SINGLE, enables_msb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Enable/Disable Drop per ingress context
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc. flags (see DNX_TDM_INGRESS_CONTEXT_FAILOVER_SUPPORTED_FLAGS)
 * \param [in] nof_contexts - Number of contexts, which drop state should be fetched
 *                              array size for context_ids/context_en
 * \param [in] context_ids - Array of context ids
 * \param [out] context_en - Array of drop enable/disable per context id  (allocated by caller with nof_contexts size)
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   bcm_dnx_tdm_ingress_context_failover_set
 */
int
bcm_dnx_tdm_ingress_context_failover_get(
    int unit,
    uint32 flags,
    int nof_contexts,
    int *context_ids,
    int *context_en)
{
    uint32 entry_handle_id;
    int i_cxt;
    int max_nof_contexts;
    uint32 enables_lsb[TDM_CONTEXT_DROP_REG_SIZE_WORDS];
    uint32 enables_msb[TDM_CONTEXT_DROP_REG_SIZE_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_tdm_ingress_context_failover_verify(unit, flags, nof_contexts, context_ids, context_en));

    max_nof_contexts = dnx_data_tdm.params.nof_contexts_get(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_CONTEXT_DROP, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_CONTEXT_DROP_LSB,
                                                            INST_SINGLE, enables_lsb));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_CONTEXT_DROP_MSB,
                                                            INST_SINGLE, enables_msb));

    for (i_cxt = 0; i_cxt < nof_contexts; i_cxt++)
    {
        int cur_index;
        uint32 *enables;
        if ((context_ids[i_cxt] < 0) || (context_ids[i_cxt] >= max_nof_contexts))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID:%d is out of valid range:0-%d\n",
                         context_ids[i_cxt], max_nof_contexts);
        }
        else if (context_ids[i_cxt] < TDM_CONTEXT_DROP_REG_SIZE)
        {
            cur_index = context_ids[i_cxt];
            enables = enables_lsb;
        }
        else
        {
            cur_index = context_ids[i_cxt] - TDM_CONTEXT_DROP_REG_SIZE;
            enables = enables_msb;
        }
        context_en[i_cxt] = (SHR_BITGET(enables, cur_index) != FALSE) ? 1 : 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dnx_tdm_interface_config_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_interface_config_t * interface_config)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;

    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_NULL_CHECK(interface_config, _SHR_E_PARAM, "interface_config");

    SHR_MASK_VERIFY(flags, DNX_TDM_INTERFACE_CONFIG_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");
    SHR_BOOL_VERIFY(interface_config->stream_id_enable, _SHR_E_PARAM, "interface_config->stream_id_enable");
    /*
     * If Stream ID distinction is disabled all parameters should be default
     */
    if (interface_config->stream_id_enable == FALSE)
    {
        if (interface_config->stream_id_key_offset != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "NON-Zero key offset:%d while stream classification is disabled\n",
                         interface_config->stream_id_key_offset);
        }
        if ((interface_config->stream_id_key_size < dnx_data_tdm.params.sid_min_size_get(unit)) ||
            (interface_config->stream_id_key_size > dnx_data_tdm.params.sid_size_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID key size:%d is out of valid range %d-%d\n",
                         interface_config->stream_id_key_size, dnx_data_tdm.params.sid_min_size_get(unit),
                         dnx_data_tdm.params.sid_size_get(unit));
        }
    }
    else
    {
        /*
         * Verify key size in bits - key size cannot be zero
         */
        if ((interface_config->stream_id_key_size < dnx_data_tdm.params.sid_min_size_get(unit)) ||
            (interface_config->stream_id_key_size > dnx_data_tdm.params.sid_size_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID key size:%d is out of valid range 1-%d\n",
                         interface_config->stream_id_key_size, dnx_data_tdm.params.sid_size_get(unit));
        }
        /*
         * Verify offset in bytes (translate from bits to bytes)
         */
        if ((interface_config->stream_id_key_offset < 0) ||
            (UTILEX_TRANSLATE_BITS_TO_BYTES(interface_config->stream_id_key_offset) >=
             dnx_data_tdm.params.sid_offset_nof_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID key offset:%d is out of valid range 0-%d\n",
                         interface_config->stream_id_key_offset, dnx_data_tdm.params.sid_offset_nof_get(unit) - 1);
        }

        /**
         * verify offset and size combination can be achieved, the rule is:
         * (offset + size) <= (offset/8)*8 + DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE.
         * Explanation - HW always parses 16 bits (DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE) from the given offset,
         * but sometimes the desired field size (key_size) spans on more than 16 bits - in such cases we must
         * return an error
         */
        if ((interface_config->stream_id_key_offset + interface_config->stream_id_key_size) >
            (UTILEX_ALIGN_DOWN(interface_config->stream_id_key_offset, 8) + DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID key offset:%d and Key size=%d combination can't be achieved\n",
                         interface_config->stream_id_key_offset, interface_config->stream_id_key_size);
        }
    }
    /*
     * Verify that base is inside the Base Range
     */
    if ((interface_config->stream_id_base < 0) ||
        (interface_config->stream_id_base >= dnx_data_tdm.params.context_map_base_nof_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stream ID base: %d is out of valid range: 0-%d\n",
                     interface_config->stream_id_base, dnx_data_tdm.params.context_map_base_nof_get(unit) - 1);
    }
    /*
     * Verify that base plus key_size based offset are inside the Stream Range
     */
    if ((interface_config->stream_id_base < 0) ||
        ((interface_config->stream_id_base + SAL_BIT(interface_config->stream_id_key_size)) >=
         dnx_data_tdm.params.nof_stream_ids_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stream Range with base: %d key_size: %d is out of valid range: 0-%d\n",
                     interface_config->stream_id_base, interface_config->stream_id_key_size,
                     dnx_data_tdm.params.nof_stream_ids_get(unit) - 1);
    }
    if (BCM_GPORT_IS_LOCAL(gport))
    {
        port = BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        port = gport;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, FALSE, FALSE, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Logical Port:%d is not NIF. ONLY NIF is eligible for TDM Stream Configuration\n",
                     port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
    if (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Logical Port:%d Interface is not TDM one\n", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set TDM stream configuration
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc. flags
 * \param [in] gport - NIF to IRE Interface gport
 * \param [out] interface_config - Pointer to the structure where Stream Configuration parameters will be assigned
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   dnx_port_tdm_stream_get
 */
int
bcm_dnx_tdm_interface_config_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_interface_config_t * interface_config)
{
    int nif_interface_id = 0, core_id;
    uint32 shift_size;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_dnx_tdm_interface_config_set_verify(unit, flags, gport, interface_config));
    if (BCM_GPORT_IS_LOCAL(gport))
    {
        port = BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        port = gport;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core_id, &nif_interface_id));

    /*
     * since design always reads DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE from the given order we need to calculate how
     * many bits to shift till we reach the desired offset
     */
    shift_size = DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE -
        (interface_config->stream_id_key_offset - UTILEX_ALIGN_DOWN(interface_config->stream_id_key_offset, 8) +
         interface_config->stream_id_key_size);



/* *INDENT-OFF* */
    /*
     * First set the window and direction
     */
    SHR_IF_ERR_EXIT(dnx_dbal_gen_set(unit, DBAL_TABLE_TDM_STREAM_CONFIGURATION,
                            1, 5,
                            GEN_DBAL_FIELD32, DBAL_FIELD_INGRESS_REASSEMBLY_INTERFACE, nif_interface_id,
                            GEN_DBAL_FIELD32, DBAL_FIELD_MAP_BASE, INST_SINGLE,interface_config->stream_id_base,
                            GEN_DBAL_FIELD32, DBAL_FIELD_SID_ENABLE, INST_SINGLE, interface_config->stream_id_enable,
                            GEN_DBAL_FIELD32, DBAL_FIELD_SID_BITS_SIZE, INST_SINGLE, interface_config->stream_id_key_size,
                            GEN_DBAL_FIELD32, DBAL_FIELD_SID_BYTES_OFFSET, INST_SINGLE, UTILEX_TRANSLATE_BITS_TO_BYTES(interface_config->stream_id_key_offset),
                            GEN_DBAL_FIELD32, DBAL_FIELD_SID_BITS_SHIFT, INST_SINGLE, shift_size,
                            GEN_DBAL_FIELD_LAST_MARK));
/* *INDENT-ON* */

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_dnx_tdm_interface_config_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_interface_config_t * interface_config)
{
    bcm_port_t port;
    dnx_algo_port_info_s port_info;
    dnx_algo_port_if_tdm_mode_e if_tdm_mode;

    SHR_FUNC_INIT_VARS(unit);

    DNX_TDM_BYPASS_MODE_VERIFY(unit);

    SHR_NULL_CHECK(interface_config, _SHR_E_PARAM, "interface_config");

    SHR_MASK_VERIFY(flags, DNX_TDM_INTERFACE_CONFIG_SUPPORTED_FLAGS, _SHR_E_PARAM, "unexpected flags.\n");

    if (BCM_GPORT_IS_LOCAL(gport))
    {
        port = BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        port = gport;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (!DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, FALSE, FALSE, FALSE, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Logical Port:%d is not NIF. ONLY NIF is eligible for TDM Stream Configuration\n",
                     port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_get(unit, port, &if_tdm_mode));
    if (if_tdm_mode == DNX_ALGO_PORT_IF_NO_TDM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Logical Port:%d Interface is not TDM one\n", port);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ingress context configuration per out port or stream ID
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - Misc. flags
 * \param [in] gport - NIF to IRE Interface gport
 * \param [out] interface_config - Pointer to the structure where Stream Configuration parameters will be assigned
 *
 * \return
 *   standard shr_error_e
 *
 * \see
 *   dnx_port_tdm_stream_set
 */
int
bcm_dnx_tdm_interface_config_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_interface_config_t * interface_config)
{
    uint32 entry_handle_id;
    int nif_interface_id = 0, core_id;
    bcm_port_t port;
    uint32 bytes_offset, shift_size;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_dnx_tdm_interface_config_get_verify(unit, flags, gport, interface_config));

    sal_memset(interface_config, 0, sizeof(bcm_tdm_interface_config_t));
    if (BCM_GPORT_IS_LOCAL(gport))
    {
        port = BCM_GPORT_LOCAL_GET(gport);
    }
    else
    {
        port = gport;
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_interface_id_get(unit, port, 0, &core_id, &nif_interface_id));
    /*
     * First set the window and direction
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TDM_STREAM_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_REASSEMBLY_INTERFACE, nif_interface_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MAP_BASE, INST_SINGLE,
                                                        (uint32 *) &interface_config->stream_id_base));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SID_ENABLE, INST_SINGLE,
                                                        (uint32 *) &interface_config->stream_id_enable));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_SID_BITS_SIZE,
                                                        INST_SINGLE, (uint32 *) &interface_config->stream_id_key_size));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SID_BYTES_OFFSET, INST_SINGLE, &bytes_offset));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_SID_BITS_SHIFT, INST_SINGLE, &shift_size));
    /*
     * We are interested in 'stream_id_key_offset' only if this key is enabled
     */
    if (interface_config->stream_id_enable == TRUE)
    {
        shift_size = DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE -
            interface_config->stream_id_key_offset + bytes_offset * 8 - interface_config->stream_id_key_size;
        interface_config->stream_id_key_offset = DNX_TDM_SID_STAMP_PARSED_FIELD_SIZE + bytes_offset * 8 -
            (shift_size + interface_config->stream_id_key_size);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
