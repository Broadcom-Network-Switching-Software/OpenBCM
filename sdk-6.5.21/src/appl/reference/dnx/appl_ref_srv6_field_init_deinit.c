/** \file appl_ref_srv6_field_init_deinit.c
 * 
 *
 * configuring PMFs for SRV6 processing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

#define NOF_QUALIFIERS (2)

#define PRESEL_QUAL_FWD_LAYER_SIZE (5)

#define MIN_NOF_SIDS (1)
#define SRV6_TERMINATION_ARRAY_SIZE (24)
#define SRV6_UDH_BASE_VALUE (0x3)

#define FAI_BITS_VALUE_NOT_UPDATE_TTL (0x4)

/**
 * All Endpoint cases must be resolved at ingress PMF, by building the next DIP on UDH.
 * than use the following UDH types to indicate if Egress should perform SL decrement or not.
 */
#define ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT (0xAA)
#define ENDPOINT_UDH_TUPE_WITHOUT_SL_DECREMENT (0x55)
 /*
  * Include files.
 */
/** soc */
#include <soc/schanmsg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_field_types_defs.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "appl_ref_srv6_field_init_deinit.h"

/** bcm */
#include <bcm/field.h>
#include <bcm/switch.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/** dbal */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <include/bcm/cosq.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

/*
 * DEFINEs
 */

typedef struct cint_field_srv6_endpoint_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t parsing_start_offset_valid_qual;
    bcm_field_qualify_t lr_fwd_offset_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_qualify_t parsing_index_qual;
    bcm_field_action_t container_void_action;
} field_srv6_endpoint_info_t;

typedef struct cint_field_srv6_egress_usd_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_direct_fg;
    bcm_field_group_t ipmf3_tcam_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_action_t container_void_action;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t parsing_start_offset;
    bcm_field_qualify_t srv6_beyond_layer_type_qual;
    bcm_field_qualify_t parsing_start_offset_valid;
    bcm_field_qualify_t parsing_start_offset_srh_extended_qual;
    bcm_field_qualify_t parsing_index_qual;
    bcm_field_qualify_t bytes_to_remove_header_qual;
    bcm_field_qualify_t bytes_to_remove_fix_qual;
    bcm_field_qualify_t large_parsing_start_offset_efes_valid_qual;
    bcm_field_qualify_t large_parsing_start_offset_valid_and_srh_base_qual;
    bcm_field_qualify_t large_parsing_start_offset_srh_extended_qual;
    bcm_field_qualify_t large_parsing_start_offset_add_64_qual;
    bcm_field_qualify_t large_bytes_to_remove_efes_valid_qual;
    bcm_field_qualify_t large_bytes_to_remove_qual;
    bcm_field_action_t large_parsing_start_offset_action;
    bcm_field_action_t large_bytes_to_remove_action;
    bcm_field_qualify_t srh_next_header_pmf2_qual;
    bcm_field_qualify_t srh_next_header_pmf3_qual;
    bcm_field_qualify_t pmf2_esi_1st_pass_udh3_size;
    bcm_field_qualify_t pmf2_esi_1st_pass_udh3_data;
    bcm_field_action_t pmf2_udh3_esi_user_action;

    /** Ecologic usage additions */
    bcm_field_qualify_t parsing_start_offset_zero_qual;
    bcm_field_qualify_t large_parsing_start_offset_sids_qual;

} field_srv6_egress_usd_info_t;

typedef struct cint_field_srv6_egress_usp_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t egress_parsing_idx_qual;
    bcm_field_qualify_t fwd_layer_idx_qual;
    bcm_field_qualify_t msb_bits_tm_compensate_size;
    bcm_field_qualify_t parsing_nof_sids_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_qualify_t qual_id_fai_3_0;
    bcm_field_action_t container_void_action;
} field_srv6_egress_usp_info_t;

typedef struct cint_field_srv6_endpoint_psp_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t egress_parsing_idx_qual;
    bcm_field_qualify_t fwd_layer_idx_qual;
    bcm_field_qualify_t msb_bits_tm_compensate_size;
    bcm_field_qualify_t parsing_nof_sids_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_action_t container_void_action;
} field_srv6_endpoint_psp_info_t;

/*
 * MACROs
 */

 /*
  * Global and Static
  */

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_info_t cint_field_srv6_endpoint_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_info_t cint_field_srv6_usid_endpoint_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_psp_info_t cint_field_srv6_endpoint_psp_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_psp_info_t cint_field_srv6_endpoint_psp_ext_cut_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_egress_usd_info_t cint_field_srv6_egress_usd_info = {
    -1, -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldActionCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldActionCount, bcmFieldActionCount, bcmFieldQualifyCount, bcmFieldActionCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldActionCount
};

field_srv6_egress_usp_info_t cint_field_srv6_egress_usp_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/*
 * Functions
 */

/**
 * \brief - create a context in IPMF[1..3] and return its id
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_name - the name of the context
 *   \param [in] flags - context creation flags
 *   \param [out] ctx_id -  the returned ctx_id that was created
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_context_set(
    int unit,
    bcm_field_stage_t stage,
    char *ctx_name,
    uint32 flags,
    bcm_field_context_t * ctx_id)
{
    bcm_field_context_info_t context_info;
    void *dest_char;
    SHR_FUNC_INIT_VARS(unit);

    /** init the structure which holds the parameters of context */
    bcm_field_context_info_t_init(&context_info);

    /** set the user name of the context into the context structure */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, ctx_name, sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, flags, stage, &context_info, ctx_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - always follows same presels as PMF1-2
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id -  the allocated number of the Presel
 *   \param [in] is_egress -  0 for End-Point node and 1 for Egress node
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf3_presel_set(
    int unit,
    bcm_field_context_t ipmf1_ctx_id,
    bcm_field_context_t pmf3_ctx_id,
    int presel_id)
{
    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF3;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.nof_qualifiers = 1;
    ipmf_presel_entry_data.context_id = pmf3_ctx_id;

     /** set 1st qualifier to both end-point and egress, check if FWD layer is SRv6Beyond */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    ipmf_presel_entry_data.qual_data[0].qual_value = ipmf1_ctx_id;
    ipmf_presel_entry_data.qual_data[0].qual_mask = 0x3f;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create a Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 uSID End-Point
 *          presel is set based on FWD2 context, in higher priority than the classical end-point context
 *
 *   \param [in] unit - device id
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id - the preselector id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf1_usid_endpoint_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = 1;

    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldAppTypeSrv6UsidEndpoint;
    ipmf_presel_entry_data.qual_data[0].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create a Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 Classic End-Point
 *          presel is set based on FWD2 context, in lower priority than the uSID end-point context
 *
 *   \param [in] unit - device id
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id - the preselector id
 *   \param [in] is_psp - PSP mode is set
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf1_classic_endpoint_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id,
    uint8 is_psp)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;

    int srv6_relative_to_fwd_layer = 1;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = 2;

    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldAppTypeSrv6Endpoint;
    ipmf_presel_entry_data.qual_data[0].qual_mask = 0x1F;

    /*
     * Endpoint would occur if: -2nd qualifier check layer_qualifier[0] of SRv6 if == 0 (SL!=0) in USP mode -or
     * layer_qualifier[1] if == 0 (SL!=1) in PSP mode 
     */
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    ipmf_presel_entry_data.qual_data[1].qual_arg = srv6_relative_to_fwd_layer;
    ipmf_presel_entry_data.qual_data[1].qual_value = 0 << is_psp;
    ipmf_presel_entry_data.qual_data[1].qual_mask = 0x1 << is_psp;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create a Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 Classic End-Point
 *          presel is set based on FWD2 context, in lower priority than the uSID end-point context
 *
 *   \param [in] unit - device id
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id - the preselector id
 *   \param [in] is_ext_cut - boolean indication for extended_cut flow or not
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf1_endpoint_psp_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id,
    uint8 is_ext_cut)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;

    /**  in SRv6 Endpoint, after IPv6 termination, we set in VTT5 fwd layer be again IPv6, for hashing of IPv6 layer in FWD */
    int srv6_relative_to_fwd_layer = 1;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = 2;

    /** set qualifier on FWD2 context be SRv6___Private_UC */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldAppTypeSrv6Endpoint;
    ipmf_presel_entry_data.qual_data[0].qual_mask = 0x1F;

    /*
     * Endpoint PSP would occur if: layer_qualifier[1] if == 1 (SL==1) 
     */
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    ipmf_presel_entry_data.qual_data[1].qual_arg = srv6_relative_to_fwd_layer;
    if (!is_ext_cut)
    {
        ipmf_presel_entry_data.qual_data[1].qual_value = 1 << 1;
        ipmf_presel_entry_data.qual_data[1].qual_mask = 1 << 1;
    }
    else
    {
        ipmf_presel_entry_data.qual_data[1].qual_value = ((1 << 1) | (1 << 3));
        ipmf_presel_entry_data.qual_data[1].qual_mask = ((1 << 1) | (1 << 3));
    }

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create a Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 and Egress node
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id -  the allocated number of the Presel
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf1_egress_usd_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;

    int srv6_relative_to_fwd_layer = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = NOF_QUALIFIERS;

     /** set 1st qualifier to egress, check if FWD layer is SRv6Beyond, after IPv6 termination */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[0].qual_arg = srv6_relative_to_fwd_layer;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf_presel_entry_data.qual_data[0].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    /*
     * Egress would occur if: -2nd qualifier check layer_qualifier[0] of SRv6 if == 1 (SL==0) in USP mode -or
     * layer_qualifier[1] if == 1 (SL==1) in PSP mode 
     */
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    ipmf_presel_entry_data.qual_data[1].qual_arg = srv6_relative_to_fwd_layer;
    ipmf_presel_entry_data.qual_data[1].qual_value = 1;
    ipmf_presel_entry_data.qual_data[1].qual_mask = 0x1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create a Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 and Egress USP node
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id -  the allocated number of the Presel
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf1_egress_usp_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;

    int srv6_relative_to_fwd_layer = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = 3;

     /** set 1st qualifier to egress, check if FWD layer is SRv6Beyond, after IPv6 termination */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[0].qual_arg = 0;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf_presel_entry_data.qual_data[0].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    /** set 2nd qualifier to egress, check if FWD+1 layer is also SRv6Beyond, after IPv6 termination */
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[1].qual_arg = 1;
    ipmf_presel_entry_data.qual_data[1].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf_presel_entry_data.qual_data[1].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    /*
     * Egress would occur if: -2nd qualifier check layer_qualifier[0] of SRv6 if == 1 (SL==0) in USP mode
     */
    ipmf_presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    ipmf_presel_entry_data.qual_data[2].qual_arg = srv6_relative_to_fwd_layer;
    ipmf_presel_entry_data.qual_data[2].qual_value = 1;
    ipmf_presel_entry_data.qual_data[2].qual_mask = 0x1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2
 *          -It copies the 128bits IPv6 DIP into x4 UDH (32bits)
 *          -Counts NOF SIDs and passes in container to PMF3
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x3 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing 
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT)
 *                  3. SRH's Segment Left
 *
 *          -x9 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *                  x1 FEM adding to the Segment Left + 1 to get NOF SIDs
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok..
 */
static shr_error_e
field_srv6_config_endpoint_psp_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;

    void *dest_char;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 128;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_psp_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.udh_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_size_psp_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.udh_type_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_last_ent_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.srh_last_entry_qual));

    /*
     * -Create a void action (user defined).
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) (action_info.name), "srv6_psp_cont_void_a", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_endpoint_psp_info.container_void_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_psp", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_psp_info.srh_last_entry_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_psp_info.udh_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_psp_info.udh_type_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 9;

    fg_info.action_types[0] = cint_field_srv6_endpoint_psp_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Write the 128 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-3] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[1] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[4] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[5] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[7] = FALSE;
    fg_info.action_types[8] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[8] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_psp_info.ipmf2_fg));

    /*
     * Configure FEM to increment the last segment by one to get the number of segments
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** this is the offset, on input 'result' (action), of the 32bits 'chunk' in 16bits resolution */
    fem_action_info.fem_input.input_offset = 0;
    /** this is MS bit on the 32bits 'chunk' of the 4 bits that are used as 'condition'*/
    fem_action_info.condition_msb = 3;
    /** all 16 possible conditions initialized to be activated, and choose action 0 (of 0..3 possible)*/
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }

    /*
     * -fem_extraction = array of 4 'action' descriptors, we use only [0]
     *
     * -This action will be used as Container in IPMF2, to parse the action buffer,
     *  when performing cascading between IPMF2 and IPMF3
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;

    /*
     * - output_bit = array of 24 'bit descriptor' indication of how to construct action value.
     *   for each bit, indicate whether to take it from the 32bits 'chunk' (bcmFieldFemExtractionOutputSourceTypeKey)
     *   or from value written on this input (bcmFieldFemExtractionOutputSourceTypeForce)
     *
     * - we read first 8bits bit by bit from 'chunk' and rest 16bits set to '0'
     */
    for (ii = 0; ii < 8; ii++)
    {
        /*
         * Use a bit from the input key
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }

    /** set the remaining bits up to 24bits of the action to 0 */
    for (; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }

    /** increment = <value to add> so we add here number +1 to Last Element */
    fem_action_info.fem_extraction[0].increment = 1;

    /*
     * BCM_FIELD_ACTION_POSITION() - Encoded position of the FEM to add to this FG.
     */
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add
                    (unit, 0, cint_field_srv6_endpoint_psp_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(3, 2),
                     &fem_action_info));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * Take the "last entry" or "last segment" field from the SRH header,
     * i.e from the FWD layer which is SRv6, and this field is in 32bits offset
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 32;

    /*
     * take the last 128 bit from the packet, i.e metadata that's passed in the pipe
     * the 128 lsb is IPv6 DIP, that the start processing writes to the metadata, offset 0
     */

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        /** Jump to start of SRH Base */
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
        /** Indicates the layer relative to FWD layer */
        attach_info.key_info.qual_info[1].input_arg = 1;
        /** Take from offset of SRH Base + SRH Base - and it'll take the SID (MSB->LSB) */
        attach_info.key_info.qual_info[1].offset = 64;
    }
    else
    {
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[1].offset = 0;
    }

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf2_fg,
                                                   cint_field_srv6_endpoint_psp_info.ipmf1_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2
 *          -It builds the next DIP on UDH as: {usid prefix(dip[0:31]),next usid list(DIP[48,127], zero 16 bits}
 *          -It set USH type to ENDPOINT_UDH_TUPE_WITHOUT_SL_DECREMENT, to identify at egress that no need to perform SL--
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x2 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing 
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT)
 *
 *          -x8 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_usid_end_point_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t udh_qual_0;
    bcm_field_qualify_t udh_qual_1;
    bcm_field_qualify_t udh_qual_2;

    void *dest_char;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usid_endp_16b_lsb_0", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udh_qual_0));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 80;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usid_endp_x5_lsb_usids", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udh_qual_1));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 32;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usid_endp_prefix", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udh_qual_2));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "qual_usid_endpoint_udh_size_name", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_usid_endpoint_info.udh_type_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_usid_endpoint_UDH", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = udh_qual_0;
    fg_info.qual_types[1] = udh_qual_1;
    fg_info.qual_types[2] = udh_qual_2;
    fg_info.qual_types[3] = cint_field_srv6_usid_endpoint_info.udh_type_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 8;

    /*
     * Write the 128 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-3] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[0] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[3] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[4] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[7] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_usid_endpoint_info.ipmf2_fg));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /** 16bits LSB which are 0 for shift left */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[0].input_arg = 0;

    /** Jump to start of IPv6 DIP from msb side to take x5 LSB usids */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    /** Indicates the layer relative to FWD layer - in Endpoint we set FWD layer back to IPv6 for FWD hash */
    attach_info.key_info.qual_info[1].input_arg = 0;
    /** Take from IPv6 DIP the last x5 LSB usids for left shift by one sid in next DIP */
    attach_info.key_info.qual_info[1].offset = 320 - 80;

    /** Jump to start of IPv6 DIP from msb side to take 32b usid Prefix*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
    /** Indicates the layer relative to FWD layer - in Endpoint we set FWD layer back to IPv6 for FWD hash */
    attach_info.key_info.qual_info[2].input_arg = 0;
    /** Take from offset of IPv6 DIP the usid Prefix */
    attach_info.key_info.qual_info[2].offset = 320 - 128;

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constant value, which is ENDPOINT_UDH_TUPE_WITHOUT_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = ENDPOINT_UDH_TUPE_WITHOUT_SL_DECREMENT;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_usid_endpoint_info.ipmf2_fg,
                                                   cint_field_srv6_usid_endpoint_info.ipmf1_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2
 *          -It copies the 128bits IPv6 DIP into x4 UDH (32bits)
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x2 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing 
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT)
 *
 *          -x8 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_end_point_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;

    void *dest_char;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 128;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.udh_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "qual_udh_size_name", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.udh_type_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_UDH", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_info.udh_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_info.udh_type_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 8;

    /*
     * Write the 128 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-3] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[0] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[3] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[4] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[7] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_info.ipmf2_fg));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * take the last 128 bit from the packet, i.e metadata that's passed in the pipe
     * the 128 lsb is IPv6 DIP, that the start processing writes to the metadata, offset 0
     */

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        /** Jump to start of SRH Base */
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
        /** Indicates the layer relative to FWD layer - in Endpoint we stay at IPv6 layer, so take from fwd+1 */
        attach_info.key_info.qual_info[0].input_arg = 1;
        /** Take from offset of SRH Base + SRH Base - and it'll take the SID (MSB->LSB) */
        attach_info.key_info.qual_info[0].offset = 64;
    }
    else
    {
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[0].offset = 0;
    }

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_info.ipmf2_fg,
                                                   cint_field_srv6_endpoint_info.ipmf1_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2
 *          -It copies the 128bits IPv6 DIP into x4 UDH (32bits)
 *          -Counts NOF SIDs and passes in container to PMF3
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x3 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT)
 *                  3. SRH's Segment Left
 *
 *          -x9 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *                  x1 FEM adding to the Segment Left + 1 to get NOF SIDs
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_egress_usp_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;

    void *dest_char;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 128;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_usp_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.udh_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_size_usp_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.udh_type_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_last_ent_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.srh_last_entry_qual));

    /*
     * Create user define qualifier with CONST for FAI[3:0], Value 0x4 = 0100b
     * to set FAI[2] to 1, meaning no TTL update on the Egress
     */

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 4;
    sal_strncpy_s((char *) (qual_info.name), "FAI_BITS_3_0", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.qual_id_fai_3_0));

    /*
     * -Create a void action (user defined).
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;

    sal_strncpy_s((char *) (action_info.name), "srv6_usp_cont_void_a", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_egress_usp_info.container_void_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_usp", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_srv6_egress_usp_info.srh_last_entry_qual;
    fg_info.qual_types[1] = cint_field_srv6_egress_usp_info.udh_qual;
    fg_info.qual_types[2] = cint_field_srv6_egress_usp_info.udh_type_qual;
    fg_info.qual_types[3] = cint_field_srv6_egress_usp_info.qual_id_fai_3_0;

    /*
     * Set actions
     */
    fg_info.nof_actions = 10;

    fg_info.action_types[0] = cint_field_srv6_egress_usp_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Write the 128 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-3] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[1] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[4] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[5] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[7] = FALSE;
    fg_info.action_types[8] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[8] = FALSE;

    /*
     * Update the FAI bits [3..0]
     */
    fg_info.action_types[9] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_with_valid_bit[9] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usp_info.ipmf2_fg));

    /*
     * Configure FEM to increment the last segment by one to get the number of segments
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** this is the offset, on input 'result' (action), of the 32bits 'chunk' in 16bits resolution */
    fem_action_info.fem_input.input_offset = 0;
    /** this is MS bit on the 32bits 'chunk' of the 4 bits that are used as 'condition'*/
    fem_action_info.condition_msb = 3;
    /** all 16 possible conditions initialized to be activated, and choose action 0 (of 0..3 possible)*/
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }

    /*
     * -fem_extraction = array of 4 'action' descriptors, we use only [0]
     *
     * -This action will be used as Container in IPMF2, to parse the action buffer,
     *  when performing cascading between IPMF2 and IPMF3
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;

    /*
     * - output_bit = array of 24 'bit descriptor' indication of how to construct action value.
     *   for each bit, indicate whether to take it from the 32bits 'chunk' (bcmFieldFemExtractionOutputSourceTypeKey)
     *   or from value written on this input (bcmFieldFemExtractionOutputSourceTypeForce)
     *
     * - we read first 8bits bit by bit from 'chunk' and rest 16bits set to '0'
     */
    for (ii = 0; ii < 8; ii++)
    {
        /*
         * Use a bit from the input key
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }

    /** set the remaining bits up to 24bits of the action to 0 */
    for (; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }

    /** increment = <value to add> so we add here number +1 to Last Element */
    fem_action_info.fem_extraction[0].increment = 1;

    /*
     * BCM_FIELD_ACTION_POSITION() - Encoded position of the FEM to add to this FG.
     */
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add
                    (unit, 0, cint_field_srv6_egress_usp_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(3, 2),
                     &fem_action_info));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * Take the "last entry" or "last segment" field from the SRH header,
     * i.e from the FWD layer which is SRv6, and this field is in 32bits offset
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 32;

    /*
     * take the last 128 bit from the packet, i.e metadata that's passed in the pipe
     * the 128 lsb is IPv6 DIP, that the start processing writes to the metadata, offset 0
     */

    /** Jump to start of IPv6 */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    /** Indicates the layer relative to FWD layer */
    attach_info.key_info.qual_info[1].input_arg = -1;
    /** Take from IPv6 layer from DIP location from MSB of the layer and it'll take the DIP (MSB->LSB) */
    attach_info.key_info.qual_info[1].offset = 192;

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = ENDPOINT_UDH_TUPE_WITH_SL_DECREMENT;

    /*
     * Set FAI[3..0] so that FAI[2] is set, meaning not TTL update on Egress
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = FAI_BITS_VALUE_NOT_UPDATE_TTL; /** Set FAI[2]*/

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usp_info.ipmf2_fg,
                                                   cint_field_srv6_egress_usp_info.ipmf1_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2 for Egress node
 *          -it gets the number of SIDs in the SRv6 Header, by adding + 1 to
 *           Last Element field in the SRH header
 *          -reads SRH Next-Header to get into Egress with correct Layer Type
 *          -in case of ESI LIF, copy to UDH3 the ESI 16b value from IPv6 DIP 16 LSB
 *
 *          -x4 Qualifier:
 *                  1. 8bits to store the Last Element field from SRH header
 *                  2. 8bits to store the Next-Header field from SRH header
 *                  3. 2bits UDH type of UDH3 container (optional if ESI)
 *                  4. 16bits ESI value for UDH3 data   (optional if ESI)
 *          -x3 Void Action (SW only to use for FEM)
 *                  1. Action 16bits to use by FEM for above qualifiers
 *                  2. Action for UDH type write        (optional if ESI)
 *                  3. Action for UDH data write        (optional if ESI)
 *
 *
 *   Implementation:
 *          - Qualifiers = Last Element (8bits) + Next Header (8bits) --> (DirectExctract) = Action  (16bits VoidType)
 *          - FEM (17bits to container write)
 *               - get Last Element from SRH and add +1 to get NOF sids
 *               - container [0..7]  - 8bits write NOF sids
 *               - container [8]     - 1bit spacing '0'
 *               - container [9..16] - 8bits copy the Next-Header field
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_egress_usd_ipmf2(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;
    void *dest_char;
    int ii = 0;
    int size_last_entry_field = 8;
    int size_next_header_field = 8;
    SHR_FUNC_INIT_VARS(unit);

    /** create a user-defined qualifier in order to take manually as 8bits of Last-Entry field from SRH (FWD Layer) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = size_last_entry_field;
    sal_strncpy_s((char *) (qual_info.name), "srv6_srh_last_ent_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usd_info.srh_last_entry_qual));

    /** create a user-defined qualifier in order to take manually as 8bits of Next-Header field from SRH (FWD Layer) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = size_next_header_field;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usd_nxt_hdr_q2", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usd_info.srh_next_header_pmf2_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    sal_strncpy_s((char *) (qual_info.name), "srv6_esi_udh_size", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usd_info.pmf2_esi_1st_pass_udh3_size));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16;
    sal_strncpy_s((char *) (qual_info.name), "srv6_esi_udh_data", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usd_info.pmf2_esi_1st_pass_udh3_data));

    /*
     * -Create a void action (user defined):
     *    -for 8 bits of SRH Last Entry copy +
     *    -1 bit spacer +
     *    -for 8 bits of SRH Next-Header copy
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = size_last_entry_field + size_next_header_field;
    action_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) (action_info.name), "srv6_container_void_a", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_egress_usd_info.container_void_action));

    /*
     * Create UDA for a 16-bit ESI. UDA is required as the default size of the UDHData3 is 32bit. 
     */
    bcm_field_action_info_t_init(&action_info);
    /**
    * Fill the structure needed for bcm_field_qualifier_create API
    */
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionUDHData3;
    action_info.size = 16;
    action_info.prefix_size = 16;
    action_info.prefix_value = 0;
    sal_strncpy_s((char *) (action_info.name), "srv6_esi_uhd_16_uda", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_egress_usd_info.pmf2_udh3_esi_user_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_last_entry", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_srv6_egress_usd_info.srh_last_entry_qual;
    fg_info.qual_types[1] = cint_field_srv6_egress_usd_info.srh_next_header_pmf2_qual;
    fg_info.qual_types[2] = cint_field_srv6_egress_usd_info.pmf2_esi_1st_pass_udh3_size;
    fg_info.qual_types[3] = cint_field_srv6_egress_usd_info.pmf2_esi_1st_pass_udh3_data;

    /*
     * Set actions
     */
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = cint_field_srv6_egress_usd_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    fg_info.action_types[1] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[1] = FALSE;

    fg_info.action_types[2] = cint_field_srv6_egress_usd_info.pmf2_udh3_esi_user_action;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usd_info.ipmf2_fg));

    /*
     * Configure FEM to
     * - increment the last segment by one to get the number of segments
     * - create a container of 17bits = (8msb bits = Next-Header; 1bit = 0 (spacer); 8lsb bits = Last Element + 1)
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** this is the offset, on input 'result' of the 32bits 'chunk'*/
    fem_action_info.fem_input.input_offset = 0;
    /** this is MS bit on the 32bits 'chunk' of the 4 bits that are used as 'condition'*/
    fem_action_info.condition_msb = 3;
    /** all 16 possible conditions initialized to be activated, and choose action 0 (of 0..3 possible)*/
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }

    /*
     * -fem_extraction = array of 4 'action' descriptors, we use only [0]
     *
     * -This action will be used as Container in IPMF2, to parse the action buffer,
     *  when performing cascading between IPMF2 and IPMF3
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;

    /*
     * - output_bit = array of 24 'bit descriptor' indication of how to construct action value.
     *   for each bit, indicate whether to take it from the 32bits 'chunk' (bcmFieldFemExtractionOutputSourceTypeKey)
     *   or from value written on this input (bcmFieldFemExtractionOutputSourceTypeForce)
     *
     * - we read first 8bits bit by bit from 'chunk', that is the last entry field of the SRH
     */
    for (ii = 0; ii < size_last_entry_field; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }
    /*
     * Leave 1 bit space between the two fields, since we increment the first field by 1.
     */
    fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
    fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    ii++;
    /*
     * Read 8 bits of next header field
     */
    for (; ii < (size_last_entry_field + size_next_header_field + 1); ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii - 1;
    }

    /** set the remaining bits up to 24bits of the action to 0 */
    for (; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }

    /** increment = <value to add> so we add here number +1 to Last Element field - 8bits LSB in container */
    fem_action_info.fem_extraction[0].increment = 1;

    /*
     * BCM_FIELD_ACTION_POSITION() - Encoded position of the FEM to add to this FG.
     */
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add
                    (unit, 0, cint_field_srv6_egress_usd_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(3, 2),
                     &fem_action_info));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * Take the "last entry" or "last segment" field from the SRH header,
     * i.e from the FWD layer which is SRv6, and this field is in 32bits offset
     * -In case of Ecologic where the FWD layer is IPv4,
     *  take this field from FWD-1 layer, otherwise the FWD layer is SRv6
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].offset = 32;
    attach_info.key_info.qual_info[0].input_arg = 0;

    /*
     * Take the "Next Protocol" field from the SRH header (the FWD layer) * - this field is 0 bits offset in the SRH
     * first * - qualifer is of size 8bits, so they will be taken from offset 0 in SRH header
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    /** Use type 2 to set as UDH3 size as 32b */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 0x2;

    /** Take the ESI from IPv6 DIP 16b LSB - header idx 1, and offset from MSB 320-16b */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[3].input_arg = 1;
    attach_info.key_info.qual_info[3].offset = 304;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usd_info.ipmf2_fg,
                                                   cint_field_srv6_egress_usd_info.ipmf1_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Creates user action for ParsingStartOffset action type.
 *
 *   \param [in] unit - Device id
 *   \param [in] field_stage - The field stage of ParsingStartOffset action
 *   \param [in] extracted_action_size - The size of the value, which was extracted by
 *    qualifiers, to be used as action size. According to it the prefix_size will be calculated
 *    as well.
 *   \param [in] action_name - The name to be assigned to the action.
 *   \param [out] parsing_start_offset_action_id_p - The ID of the created action.
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_parsing_start_offset_action_create(
    int unit,
    bcm_field_stage_t field_stage,
    int extracted_action_size,
    char *action_name,
    bcm_field_action_t * parsing_start_offset_action_id_p)
{
    bcm_field_action_info_t action_info;
    int pars_offset_act_size;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, bcmFieldActionParsingStartOffset, field_stage, &action_info));
    pars_offset_act_size = action_info.size;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartOffset;
    sal_strncpy_s((char *) action_info.name, action_name, sizeof(action_info.name));
    action_info.size = extracted_action_size;
    action_info.prefix_size = (pars_offset_act_size - extracted_action_size);
    action_info.stage = field_stage;
    action_info.prefix_value = 0;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, parsing_start_offset_action_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF3 for End-Point
 *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
 *          -sets the Parsing-Start-Offset to keep the IPv6 header, by deleting only the ETH
 *
 *          -x4 Qualifiers:
 *                  1. 1bit Qual Valid for Parsing-Start-Type Qual
 *                  2. 5bits for Parsing-Start-Type of const value 20 - which is 'SRv6_endpoint'
 *                  3. 1bit Qual Valid for Parsing-Start-Offset Qual (bytes to skip in Egress)
 *                  3. 8bits Qual for Parsing-Start-Offset
 *          -x2 Actions:
 *                  1. Action to set the Parsing-Start-Type 
 *                  2. Action to set the Parsing-Start-Offset 
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_endpoint_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int ii = 0;
    int key_gen_var_offset;
    int lr_offset_offset;

    bcm_field_qualifier_info_get_t qual_info_get;

    int parsing_start_offset_extracted_value = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x34 => 1(1|0100 = 20)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x34;
    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF3, cint_field_srv6_endpoint_info.ipmf3_context_id, &param_info));

    /**We build the number 20 which is hex 0x14 for parsing start type.*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_srep_start_type_val_q", sizeof(qual_info.name));
    /**Qualifier for valid bit*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_info.parsing_start_type_valid_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_endpoint_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual));

    /**Need to copy the offset of forwarding layer to parsing offset action*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_start_offset_val_q", sizeof(qual_info.name));
    /**Qualifier for valid bit*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual));

    parsing_start_offset_extracted_value += qual_info.size;
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_lr_fwd_offset_q", sizeof(qual_info.name));
    /**Qualifier for de on fwd offset*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.lr_fwd_offset_qual));

    parsing_start_offset_extracted_value += qual_info.size;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_ipmf3", sizeof(fg_info.name));

    /*
     * Configure the fwd_layer_idx and egress_parsing_idx.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_index_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.parsing_index_qual));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 5;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_info.lr_fwd_offset_qual;
    fg_info.qual_types[4] = cint_field_srv6_endpoint_info.parsing_index_qual;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */

    /*
     * Parsing-Start-Type action
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Parsing-Start-Offset action
     * Q[2] - action inner valid bit
     * Q[3] - value is set to - IPv6 layer offset, meaning to cut the ETH layer in Egress
     */
    SHR_IF_ERR_EXIT(field_srv6_parsing_start_offset_action_create(unit, bcmFieldStageIngressPMF3,
                                                                  parsing_start_offset_extracted_value,
                                                                  "srv6_endpoint_ipmf3_ParsingStartOffset_a",
                                                                  &fg_info.action_types[1]));

    fg_info.action_with_valid_bit[1] = FALSE;

    /** Egress_Parsing_Index */
    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_info.ipmf3_fg));

    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of key_gen_var on the PBUS.*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get));

    key_gen_var_offset = qual_info_get.offset;
    /**Get the offset (within the layer record) of the layer offset (within the header).*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageIngressPMF3, &qual_info_get));

    lr_offset_offset = qual_info_get.offset;

    /**Get the offset (within the layer record) of the layer record qualifier (within the header).*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF3,
                                                 &qual_info_get));

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 5 bits, Qual's size which is 20*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 5;

    /*
     * take the layer_offset of IPv6 (which is the FWD layer, because we stay at IPv6 for FWD to hash IPv6),
     * Cutting in Egress the ETH, which means ETH is passed and not cut in Ingress 
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[3].input_arg = 0;
    /**This is the offset of protocol type inside layer records*/
    attach_info.key_info.qual_info[3].offset = lr_offset_offset;

    /*
     * Egress layer index
     */

    /** Set the value of Egress layer index*/
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[4].input_arg = 1;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_info.ipmf3_fg,
                                                   cint_field_srv6_endpoint_info.ipmf3_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF3 for Egress USP
 *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
 *           cause PSP is simply a Qualifier in SRv6_endpoint Layer in Egress
 *          -sets the egress parsing idx and fwd_layer_idx
 *          -updates nwk_header_append_size with IPv6 + SRv6 length to compensate
 *           the Egress deletion of whole IPv6 + Srv6
 *
 *          -x4 Qualifiers:
 *                  1. 1bit Qual Valid for Parsing-Start-Type Qual
 *                  2. 5bits for Parsing-Start-Type of const value 20 - which is 'SRv6_endpoint'
 *          -x2 Actions:
 *                  1. Action to set the Parsing-Start-Type system header
 *                  2. Action to set the egress_parsing_idx and fwd_layer_idx
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_egress_usp_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int ii = 0;
    int key_gen_var_offset;
    int container_offset;

    bcm_field_qualifier_info_get_t qual_info_get;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x34 => 1(1|0100 = 20)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x34; /** Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit)*/

    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_egress_usp_info.ipmf3_context_id, &param_info));

    /*
     * -We build the number 20 which is hex 0x14 for parsing start type
     * -Parsing-Start-Type Qualifier for valid bit
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_egress_usp_info.parsing_start_type_valid_qual));

    /**Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_egress_usp_info.srv6_endpoint_layer_type_qual));

    /*
     * Configure the fwd_layer_idx and egress_parsing_idx.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_fwdlyr_idx_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.fwd_layer_idx_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_egr_prs_idx_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.egress_parsing_idx_qual));

    /**NOF SIDs Qualifier - to be used to choose the right TM compensate size entry per number of SIDs */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_nof_sids_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.parsing_nof_sids_qual));

    /** 5 MSB bit for TM compensation size (which is 8bits) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_msb_bit_tm_compens_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.msb_bits_tm_compensate_size));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_usp_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 6;
    fg_info.qual_types[0] = cint_field_srv6_egress_usp_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_egress_usp_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_egress_usp_info.fwd_layer_idx_qual;
    fg_info.qual_types[3] = cint_field_srv6_egress_usp_info.egress_parsing_idx_qual;
    fg_info.qual_types[4] = cint_field_srv6_egress_usp_info.parsing_nof_sids_qual;
    fg_info.qual_types[5] = cint_field_srv6_egress_usp_info.msb_bits_tm_compensate_size;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */

    /*
     * Parsing-Start-Type action
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Bytes_to_Remove
     * Q[2] - fwd_layer_idx (3bits)
     * Q[3] - egress_parsing_idx (3bits)
     */
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[1] = FALSE;

    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    /*
     * TM Compensation Size
     * Q[4] - nof sids - represents the TM entry num  (3bits)
     * Q[5] - msb bits compensation size - '0' (5bits)
     */
    fg_info.action_types[3] = bcmFieldActionAppendPointerCompensation;
    fg_info.action_with_valid_bit[3] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usp_info.ipmf3_fg));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of key_gen_var on the PBUS.*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get));
    key_gen_var_offset = qual_info_get.offset;

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get));

    container_offset = qual_info_get.offset;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 5 bits, Qual's size which is 20*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /** Set the value of fwd_layer_idx */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 1;

    /** Set the value of egress_parsing_idx */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 1;

    /** 3bits LSB bits of NWK_HEADER_APPEND_SIZE - taking NOF SIDs from IPMF2 container cascading */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = container_offset;

    /** 5bits MSB bits of the action which writes to NWK_HEADER_APPEND_SIZE pointer, which is 8bits */
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[5].input_arg = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usp_info.ipmf3_fg,
                                                   cint_field_srv6_egress_usp_info.ipmf3_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF3 for Egress PSP
 *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
 *           cause PSP is simply a Qualifier in SRv6_endpoint Layer in Egress
 *          -sets the egress parsing idx and fwd_layer_idx
 *          -updates nwk_header_append_size with IPv6 + SRv6 length to compensate
 *           the Egress deletion of whole IPv6 + Srv6
 *
 *          -x4 Qualifiers:
 *                  1. 1bit Qual Valid for Parsing-Start-Type Qual
 *                  2. 5bits for Parsing-Start-Type of const value 20 - which is 'SRv6_endpoint'
 *          -x2 Actions:
 *                  1. Action to set the Parsing-Start-Type system header
 *                  2. Action to set the egress_parsing_idx and fwd_layer_idx
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_endpoint_psp_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int ii = 0;
    int key_gen_var_offset;
    int container_offset;

    bcm_field_qualifier_info_get_t qual_info_get;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x34 => 1(1|0100 = 20)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x34; /** Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit)*/

    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_endpoint_psp_info.ipmf3_context_id, &param_info));

    /*
     * -We build the number 20 which is hex 0x14 for parsing start type
     * -Parsing-Start-Type Qualifier for valid bit
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.parsing_start_type_valid_qual));

    /**Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.srv6_endpoint_layer_type_qual));

    /*
     * Configure the fwd_layer_idx and egress_parsing_idx.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_fwd_layer_idx_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.fwd_layer_idx_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egress_parsing_idx_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.egress_parsing_idx_qual));

    /**NOF SIDs Qualifier - to be used to choose the right TM compensate size entry per number of SIDs */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_nof_sids_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.parsing_nof_sids_qual));

    /** 5 MSB bit for TM compensation size (which is 8bits) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_msb_bit_tm_compensation_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.msb_bits_tm_compensate_size));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_psp_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 6;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_psp_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_psp_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_psp_info.fwd_layer_idx_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_psp_info.egress_parsing_idx_qual;
    fg_info.qual_types[4] = cint_field_srv6_endpoint_psp_info.parsing_nof_sids_qual;
    fg_info.qual_types[5] = cint_field_srv6_endpoint_psp_info.msb_bits_tm_compensate_size;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */

    /*
     * Parsing-Start-Type action
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Bytes_to_Remove
     * Q[2] - fwd_layer_idx (3bits)
     * Q[3] - egress_parsing_idx (3bits)
     */
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[1] = FALSE;

    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    /*
     * TM Compensation Size
     * Q[4] - nof sids - represents the TM entry num  (3bits)
     * Q[5] - msb bits compensation size - '0' (5bits)
     */
    fg_info.action_types[3] = bcmFieldActionAppendPointerCompensation;
    fg_info.action_with_valid_bit[3] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_psp_info.ipmf3_fg));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of key_gen_var on the PBUS.*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get));
    key_gen_var_offset = qual_info_get.offset;

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get));
    container_offset = qual_info_get.offset;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 5 bits, Qual's size which is 20*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /** Set the value of fwd_layer_idx */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 1;

    /** Set the value of egress_parsing_idx */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 1;

    /** 3bits LSB bits of NWK_HEADER_APPEND_SIZE - taking NOF SIDs from IPMF2 container cascading */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = container_offset;

    /** 5bits MSB bits of the action which writes to NWK_HEADER_APPEND_SIZE pointer, which is 8bits */
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[5].input_arg = 0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf3_fg,
                                                   cint_field_srv6_endpoint_psp_info.ipmf3_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF3 for Egress PSP extended cut
 *
 * - set parsing_Start_type to PSP_EXTENDED
 * - set pph_fwd_layer to 0
 * - set parsing_start_offset to layer_offset[1] (ETH)+40(IPv6))+8(SRH)+16(SID) = offset[1]+64
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_endpoint_psp_extended_cut_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int ii = 0;
    int key_gen_var_offset;

    bcm_field_qualifier_info_get_t qual_info_get;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x34 => 1(1|0100 = 20)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x37; /** Contains 5 bits with value dec23 (hex17) (PSP_EXT) and 1 bit with value 1 (valid bit)*/

    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id,
                                                &param_info));

    /*
     * -We build the number 20 which is hex 0x14 for parsing start type
     * -Parsing-Start-Type Qualifier for valid bit
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_ext_cut_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_start_type_valid_qual));

    /**Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_ext_cut_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.srv6_endpoint_layer_type_qual));

    /*
     * Configure the fwd_layer_idx and egress_parsing_idx.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_fwd_layer_idx_psp_ext_cut_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_ext_cut_info.fwd_layer_idx_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egress_parsing_idx_psp_ext_cut_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.egress_parsing_idx_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_psp_ext_cut_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_psp_ext_cut_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_psp_ext_cut_info.fwd_layer_idx_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_psp_ext_cut_info.egress_parsing_idx_qual;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */

    /*
     * Parsing-Start-Type action
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Bytes_to_Remove
     * Q[2] - fwd_layer_idx (3bits)
     * Q[3] - egress_parsing_idx (3bits)
     */
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[1] = FALSE;

    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_fg));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of key_gen_var on the PBUS.*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get));

    key_gen_var_offset = qual_info_get.offset;

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get));

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 5 bits, Qual's size which is 20*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /** Set the value of fwd_layer_idx */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 1;

    /** Set the value of egress_parsing_idx */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 1;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_fg,
                                                   cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id,
                                                   &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF3 for Egress USD
 *          -it sets the Parsing-Start-Type to 'IPv4/IPv6/ETH' in accordance to SRH Next-Header field
 *          -updates Bytes_to_Remove and Parsing_Start_Offset in following way,
 *              (if NOF SIDs <= 7)
 *                 * Cut at Ingress ETH + IPv6 (set Bytes_to_Remove=2, which means cut up to FWD layer which is SRv6)
 *                 * Cut at Egress SRH + SIDs (Parsing_Start_Offset to SRH + 128b * NOF_SIDs)
  *              (if NOF SIDs > 7)
 *                 * Cut at Ingress ETH + IPv6 + 64B (set Bytes_to_Remove=2, Bytes_to_Remove_Fix = 64)
 *                 * Cut at Egress SRH + SIDs -64B (Parsing_Start_Offset to SRH + 128b * NOF_SIDs - 64B)
 *          -sets Egress_Parsing_Index to SRv6 layer
 *
 *
 *   Field Group 1 -  Direct Extraction:
 *          -x11 Qualifiers, which accommodate below actions
 *
 *          -x5 Actions:
 *                  1. set Parsing_Start_Offset (when NOF SIDs <= 7)
 *                  2. set Egress_Parsing_Index
 *                  3. set Bytes_to_Remove (when NOF SIDs <= 7)
 *                  4. set Parsing_Start_Offset (when NOF SIDs > 7)
 *                  5. set Bytes_to_Remove (when NOF SIDs > 7)
 *
 *   Field Group 2 -  TCAM:
 *          -1 Qualifier, of SRH Next-Header field, received from PMF2 container
 *          -1 Action sets Parsing-Start-Type according to Next-Header field
 *          -In TCAM 3 entries of Next-Header to Parsing-Start-Type exist
 *                  -IANA IPv4 (4)  --> Layer_Types_IPv4
 *                  -IANA IPv6 (41) --> Layer_Types_IPv6
 *                  -IANA ETH  (143) --> Layer_Types_ETHERNET
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_config_egress_usd_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_entry_info_t tcam_ent_info;
    bcm_field_entry_t tcam_entry_id;
    void *dest_char;
    int ii = 0;
    int key_gen_var_offset;
    int container_offset;
    int lr_offset_offset;
    int hw_value;
    uint32 bcm_action_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS] = { 0 };
    uint32 hw_action_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS] = { 0 };

    bcm_field_qualifier_info_get_t qual_info_get;

    int parsing_start_offset_extracted_value = 0;

    /** In case NOF SIDs > 7, 64B addition to Bytes_to_Remove in Ingress */
    uint32 large_bytes_to_remove_fix = 64;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context. */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x1 +          /** 1 bit with value 1 to be used as qualifier valid bits for actions (FES inner valid bit) */
        (large_bytes_to_remove_fix << 3); /** 7 bits contain bytes_to_remove_fix for the case of more than 7 segments */
    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_egress_usd_info.ipmf3_context_id, &param_info));

    /**Parsing-Start-Offset valid bit from FWD layer - 1bit action's inner valid bit  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_valid_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usd_info.parsing_start_offset_valid));

    parsing_start_offset_extracted_value += qual_info.size;

    /**Parsing-Start-Offset up to FWD layer ETH + IPv6 (up to 62b) 6bits */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 6;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_offset_q", sizeof(qual_info.name));
    /**Qualifier for de on fwd offset*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_egress_usd_info.parsing_start_offset));

    parsing_start_offset_extracted_value += qual_info.size;

    /** Qualifier to indicate 3rd bit of NOF SIDs, i.e if NOF SIDs >= 8 */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_efes_val_q", sizeof(qual_info.name));
    /**Both EFES valid bit according to the fourth bit of the number of entries..*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_egress_usd_info.large_parsing_start_offset_efes_valid_qual));

    parsing_start_offset_extracted_value += qual_info.size;

    /*
     * Get MetaData Offsets
     */

    /**Get the offset of key_gen_var on the PBUS - get the Key Gen Value */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get));

    key_gen_var_offset = qual_info_get.offset;

    /**Get the offset (within the layer record) of the layer offset (within the header).*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageIngressPMF3, &qual_info_get));

    lr_offset_offset = qual_info_get.offset;

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get));

    container_offset = qual_info_get.offset;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    /*
     * Set quals
     */
    fg_info.nof_quals = 3;

    /*
     * Parsing-Start-Offset + 6bit value (ETH (14-22)b + IPv6 (40)b - 54-62b max
     * -take first 6bits the layer_offset of SRv6_Beyond, which is the FWD layer
     * -take 7th bit (+64) if NOF SIDS >= 8
     */

    /** Parsing-Start-Offset-valid-bit (1bit valid bit) - take from the Key Gen Value*/
    fg_info.qual_types[0] = cint_field_srv6_egress_usd_info.parsing_start_offset_valid;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset;
    /** Parsing-Start-Offset (6 first bits) */
    fg_info.qual_types[1] = cint_field_srv6_egress_usd_info.parsing_start_offset;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 0; /** from FWD layer stay at 0 in regards to FWD layer */
    attach_info.key_info.qual_info[1].offset = lr_offset_offset;
    /** Parsing-Start-Offset 7th bit (+64)  taking NOF SIDs 3rd bit (add 64 in case NOF SIDs >= 8) */
    fg_info.qual_types[2] = cint_field_srv6_egress_usd_info.large_parsing_start_offset_efes_valid_qual;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = container_offset + 3;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;

    /*
     * Action for Parsing_Start_Offset - from Qualifier 2 -Q3(3b) - NOF SIDs (7th bit - 128b x
     * NOF_SIDs) -Q2(4b+1b) - original Pasrsing_Start_Offset (SRH size without SIDs) + 1b inner action validate bit
     */
    SHR_IF_ERR_EXIT(field_srv6_parsing_start_offset_action_create(unit, bcmFieldStageIngressPMF3,
                                                                  parsing_start_offset_extracted_value,
                                                                  "srv6_egress_usd_ipmf3_ParsingStartOffset_a",
                                                                  &fg_info.action_types[0]));

    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Create the FG
     */
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usd_info.ipmf3_direct_fg));

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usd_info.ipmf3_direct_fg,
                                                   cint_field_srv6_egress_usd_info.ipmf3_context_id, &attach_info));

    /*
     * TCAM Field Group
     * - Create a FG with 1xQualifier (SRH Next-Header) and corresponding 1xAction (Parsing Start Context)
     * - Create 3 TCAM entries between SRH Next-Header to  Egress Parsing Start Type and write
     * - IPv4 (4)  --> PARSING_START_TYPE_IPV4 (2)
     * - IPv6 (41) --> PARSING_START_TYPE_IPV6 (3)
     * - ETH  (143) --> PARSING_START_TYPE_DUMMY_ETHERNET (11)
     *                  Dummy ETH assumes a fixed ETH layer size, just so that Egress Parser would recognize ETH
     *                  as 1st layer, for sakes for RCH mapping correctly with next protocols as ETH
     */

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionParsingStartTypeRaw;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_Egress_TCAM", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info,
                                        &cint_field_srv6_egress_usd_info.ipmf3_tcam_fg));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    /*
     * 8bits - taking Next-Header from IPMF2 container cascading - 8lsb bits (0..7) - NOF SIDs (Last Element +1
     * calculated in PMF2 FEM) - bit 8 - space bit - 8msb bits (9..16) - Next-Header passed from PMF2
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usd_info.ipmf3_tcam_fg,
                                                   cint_field_srv6_egress_usd_info.ipmf3_context_id, &attach_info));

    bcm_field_entry_info_t_init(&tcam_ent_info);
    tcam_ent_info.priority = 1;
    tcam_ent_info.nof_entry_quals = fg_info.nof_quals;
    tcam_ent_info.entry_qual[0].type = fg_info.qual_types[0];
    tcam_ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp4;
    tcam_ent_info.entry_qual[0].mask[0] = 0x1ff;

    tcam_ent_info.nof_entry_actions = fg_info.nof_actions;
    tcam_ent_info.entry_action[0].type = fg_info.action_types[0];
    bcm_action_value[0] = bcmFieldLayerTypeIp4;
    SHR_IF_ERR_EXIT(bcm_field_action_value_map(unit, fg_info.stage, bcmFieldActionParsingStartType, bcm_action_value,
                                               hw_action_value));

    tcam_ent_info.entry_action[0].value[0] = hw_action_value[0];
    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_egress_usd_info.ipmf3_tcam_fg, &tcam_ent_info, &tcam_entry_id));

    tcam_ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeIp6;
    bcm_action_value[0] = bcmFieldLayerTypeIp6;
    SHR_IF_ERR_EXIT(bcm_field_action_value_map(unit, fg_info.stage, bcmFieldActionParsingStartType, bcm_action_value,
                                               hw_action_value));

    tcam_ent_info.entry_action[0].value[0] = hw_action_value[0];
    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_egress_usd_info.ipmf3_tcam_fg, &tcam_ent_info, &tcam_entry_id));

    tcam_ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeEth;
    SHR_IF_ERR_EXIT(dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "DUMMY_ETHERNET", &hw_value));

    tcam_ent_info.entry_action[0].value[0] = hw_value << 1 | 1; /* The valid bit value is the LSB, and it's set to 1 */
    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_egress_usd_info.ipmf3_tcam_fg, &tcam_ent_info, &tcam_entry_id));

    tcam_ent_info.entry_qual[0].value[0] = bcmFieldLayerTypeMpls;
    SHR_IF_ERR_EXIT(dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "MPLS", &hw_value));

    tcam_ent_info.entry_action[0].value[0] = hw_value << 1 | 1; /* The valid bit value is the LSB, and it's set to 1 */
    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_egress_usd_info.ipmf3_tcam_fg, &tcam_ent_info, &tcam_entry_id));

exit:
    SHR_FUNC_EXIT;
}

 /*
  * \return
  *   \retval shr_error_e - negative in case of an error, zero in case all ok.
  */
static shr_error_e
appl_dnx_field_srv6_beyond_minus_fwd_layer_ipmf1_context_set(
    int unit,
    bcm_field_context_t * context_id)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;

    int presel_id = dnx_data_field.preselector.srv6_beyond_minus_fwd_layer_presel_id_ipmf1_get(unit);
    void *dest_char;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * IPMF1 Srv6 beyond fwd layer - 1 Context Create + Presel
     */

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Srv6_beyond_fwd_layer_minus_ipmf1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 1;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = -1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    p_data.qual_data[0].qual_mask = 0x1F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}
/*
* \return
*   \retval shr_error_e - negative in case of an error, zero in case all ok.
*/
static shr_error_e
appl_dnx_field_srv6_beyond_minus_2_with_mpls_fwd_layer_ipmf1_presel_set(
    int unit,
    bcm_field_context_t * context_id)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;

    int presel_id = dnx_data_field.preselector.srv6_beyond_minus_2_with_mpls_fwd_layer_presel_id_ipmf1_get(unit);
    void *dest_char;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * IPMF1 Srv6 beyond fwd layer - 1 Context Create + Presel
     */

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "Srv6_beyond_fwd_layer_minus_2_with_mpls_ipmf1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, context_id));

    /*
     * IPMF1 Srv6 beyond fwd layer - 2, and MPLS in Fwd-1 Context Create + Presel
     */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageIngressPMF1;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = -2;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    p_data.qual_data[0].qual_mask = 0x1F;

    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = -1;
    p_data.qual_data[1].qual_value = bcmFieldLayerTypeMpls;
    p_data.qual_data[1].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

exit:
    SHR_FUNC_EXIT;
}
/*
* \return
*   \retval shr_error_e - negative in case of an error, zero in case all ok.
*/
static shr_error_e
appl_dnx_field_srv6_partial_fwd_layer_set(
    int unit,
    bcm_field_context_t context_id_ipmf1)
{
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    bcm_field_context_t etpp_fwd_term_context_id_ipmf3_v6;
    bcm_field_group_t etpp_fwd_term_tcam_fg_id;
    bcm_field_entry_t etpp_fwd_term_ent_3_ipv6_id;
    bcm_field_entry_t etpp_fwd_term_ent_def_id;

    int etpp_fwd_term_presel_id_v6 = dnx_data_field.preselector.srv6_partial_fwd_layer_presel_id_ipmf3_get(unit);
    void *dest_char;
    int hw_value;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_ipmf3_v6", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, 0, bcmFieldStageIngressPMF3, &context_info, &etpp_fwd_term_context_id_ipmf3_v6));

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = etpp_fwd_term_presel_id_v6;
    p_id.stage = bcmFieldStageIngressPMF3;
    p_data.entry_valid = TRUE;
    p_data.context_id = etpp_fwd_term_context_id_ipmf3_v6;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = context_id_ipmf1;
    p_data.qual_data[0].qual_mask = 0x3F;
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = bcmFieldLayerTypeIp6;
    p_data.qual_data[1].qual_mask = 0x1F;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /*
     * IPMF3 FG Create + Attach 
     */
    /*
     * Create and attach TCAM group in iPMF3
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF3;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyForwardingLayerIndex;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionParsingStartTypeRaw;
    fg_info.action_types[1] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[2] = bcmFieldActionStartPacketStrip;
    fg_info.action_types[3] = bcmFieldActionInvalidNext;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "nof_rebuild_tcam_fg_appl_init", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &etpp_fwd_term_tcam_fg_id));

    /*
     * Attach the FG to contexts
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    attach_info.payload_info.action_types[3] = fg_info.action_types[3];
    attach_info.payload_info.action_info[3].priority = BCM_FIELD_ACTION_POSITION(0, 0);

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, etpp_fwd_term_tcam_fg_id, etpp_fwd_term_context_id_ipmf3_v6,
                                                   &attach_info));

    /*
     * Add TCAM entries
     */
    bcm_field_entry_info_t_init(&ent_info);

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];

    ent_info.entry_qual[0].value[0] = 3;        /* fwd_layer_index = 3 */
    ent_info.entry_qual[0].mask[0] = 0x7;

    ent_info.nof_entry_actions = 3;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "IPV6_8B", &hw_value);  /* DBAL_ENUM_FVAL_PARSING_START_TYPE_ENCODING_IPV6_8B 
                                                                                                 */
    hw_value = hw_value << 1 | 1;       /* The valid bit value is the LSB, and it's set to 1 */
    ent_info.entry_action[0].value[0] = hw_value;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 3;      /* Egress FWD Index should be equal to fwd_layer_index */
    ent_info.entry_action[2].type = fg_info.action_types[2];
    ent_info.entry_action[2].value[0] = bcmFieldStartToConfigurableStrip;
    ent_info.entry_action[2].value[1] = 0;      /* nof_bytes_to_remove = 0 */

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_3_ipv6_id));

    /*
     * Add default entry that enables the Invalid-Next to ensure no offset gets changed by DE when none of the above
     * entries gets hit 
     */
    ent_info.nof_entry_quals = 0;       /* No QUALS: Default entry */
    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = fg_info.action_types[2];
    ent_info.entry_action[0].value[0] = 1;

    SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, etpp_fwd_term_tcam_fg_id, &ent_info, &etpp_fwd_term_ent_def_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * uSID End point handling
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_endpoint_usid(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * End-Point (PSP/USP) - PMF1-2 configurations
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL >= 1/2 (case of USP/PSP)
     * -Actions:
     *         - saves IPv6 DIP in x4 UDH Containers  (used for End-Point USP/PSP, Egress PSP)
     *
     * Will be peformed for following cases:
     * -In case of End-Point PSP, will be performed cause SL >= 2 
     * -In case of End-Point USP, will be performed cause SL >= 1
     */

    /** End-Point - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_srep_usid_udh_pmf1", 0,
                     &cint_field_srv6_usid_endpoint_info.ipmf1_context_id));

    /** End-Point - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_usid_endpoint_presel_set
                    (unit, cint_field_srv6_usid_endpoint_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_usid_presel_id_ipmf1_get(unit)));

    /** End-Point- FG Action: Save IPv6 DIP in x4 UDH Containers in PMF2 */
    SHR_IF_ERR_EXIT(field_srv6_config_usid_end_point_ipmf2_udh(unit));

    /*
     * PMF3 configurations - End-Point (PSP/USP)
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL > 1/2 (case of USP/PSP)
     * -Actions:
     *          -sets the Parsing-Start-Type to 'SRv6_endpoint' 
     *          -sets the Parsing-Start-Offset to keep the IPv6 header, by deleting only the ETH in Egress
     *
     * Will be peformed for following cases:
     * -In case of End-Point PSP, will be performed cause SL > 2 
     * -In case of End-Point USP, will be performed cause SL > 1
     */

    /** End-Point in PMF3 Qualifies according to PMF1 context (same conditions for these contexts in both PMFs) */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_usid_endpoint_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_usid_presel_id_ipmf3_get(unit)));

exit:
    SHR_FUNC_EXIT;

}

/*
 * Classic End point handling
 *  *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_endpoint(
    int unit,
    int appl_psp_enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * End-Point (PSP/USP) - PMF1-2 configurations
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL >= 1/2 (case of USP/PSP)
     * -Actions:
     *         - saves IPv6 DIP in x4 UDH Containers  (used for End-Point USP/PSP, Egress PSP)
     *
     * Will be peformed for following cases:
     * -In case of End-Point PSP, will be performed cause SL >= 2
     * -In case of End-Point USP, will be performed cause SL >= 1
     */

    /** End-Point - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_srep_udh_pmf1", 0,
                     &cint_field_srv6_endpoint_info.ipmf1_context_id));

    /** End-Point - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_classic_endpoint_presel_set
                    (unit, cint_field_srv6_endpoint_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_classic_endpoint_presel_id_ipmf1_get(unit),
                     (uint8) appl_psp_enable));

    /** End-Point- FG Action: Save IPv6 DIP in x4 UDH Containers in PMF2 */
    SHR_IF_ERR_EXIT(field_srv6_config_end_point_ipmf2_udh(unit));

    /*
     * PMF3 configurations - End-Point (PSP/USP)
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL > 1/2 (case of USP/PSP)
     * -Actions:
     *          -sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
     *          -sets the Parsing-Start-Offset to keep the IPv6 header, by deleting only the ETH in Egress
     *
     * Will be peformed for following cases:
     * -In case of End-Point PSP, will be performed cause SL > 2
     * -In case of End-Point USP, will be performed cause SL > 1
     */

    /** End-Point in PMF3 Context */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_srep_pmf3", 0,
                     &cint_field_srv6_endpoint_info.ipmf3_context_id));

    /** End-Point in PMF3 Qualifies according to PMF1 context (same conditiions for these contexts in both PMFs) */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_endpoint_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_classic_endpoint_presel_id_ipmf3_get(unit)));

    /*
     * End-Point in PMF3 FG - Parsting-Start-Type, Parsing-Start-Offset
     */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;

}

/*
 * Egress USP/USD handling
 *  *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_egress_usd(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /************************/
    /** Egress USD PMF1-2 **/
    /**********************/

    /*
     * PMF1-2 configurations - Egress (USD)
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL value == 1
     * -Actions:
     *        - get the NOF SIDs and store for PMF3
     *
     * Will be peformed for following cases:
     *        -In case of Egress USD, will be performed cause SL == 0
     */

    /** create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_egrs_usd_pmf1", 0,
                     &cint_field_srv6_egress_usd_info.ipmf1_context_id));

    /** create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usd_presel_set(unit, cint_field_srv6_egress_usd_info.ipmf1_context_id, 16));

    /** Egress - Configure FG in PMF2, get the NOF SIDs + Next-Header and store in container for PMF3 */
    SHR_IF_ERR_EXIT(field_srv6_config_egress_usd_ipmf2(unit));

    /**********************/
    /** Egress USD PMF3 **/
    /*********************/

    /*
     * PMF3 configurations - Egress (USD)
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL value == 0
     * -Actions:
     *         -it sets the Parsing-Start-Type to 'SRv6_Beyond' for parser context
     *         -updates Bytes_to_Remove and Parsing_Start_Offset to terminate SRv6 and IPv6
     *
     * Will be peformed for following cases:
     *        -In case of Egress USD, will be performed cause SL == 0
     */

    /** create the context in PMF3 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_egrs_usd_pmf3", 0,
                     &cint_field_srv6_egress_usd_info.ipmf3_context_id));

    /** create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_egress_usd_info.ipmf1_context_id,
                     cint_field_srv6_egress_usd_info.ipmf3_context_id, 16));

    /** configure FG in PMF3 */
    SHR_IF_ERR_EXIT(field_srv6_config_egress_usd_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - estimate byte to remove in case of srv6 extended termination.
 *    Add to epmf tcam entries, to estimate the btr per SRH-"last entry" value, which is written on UDH-DATA
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_extended_termination_btr_epmf(
    int unit)
{
    void *dest_char;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_t ent_id;
    /** Size of egress additional termination in bytes */
    int termination_size[SRV6_TERMINATION_ARRAY_SIZE] =
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 150, 150, 150, 150 };
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "srv6TermUdhEbtrCtx", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id));

    /***
     * create the context selection based on UDH base=0x3
 */
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = dnx_data_field.preselector.srv6_ext_termination_btr_epmf_get(unit);
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 4;

    p_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase3;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = SRV6_UDH_BASE_VALUE;
    p_data.qual_data[0].qual_mask = 0x3;
    p_data.qual_data[1].qual_type = bcmFieldQualifyUDHBase2;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = 0;
    p_data.qual_data[1].qual_mask = 0x3;
    p_data.qual_data[2].qual_type = bcmFieldQualifyUDHBase1;
    p_data.qual_data[2].qual_arg = 0;
    p_data.qual_data[2].qual_value = 0;
    p_data.qual_data[2].qual_mask = 0x3;
    p_data.qual_data[3].qual_type = bcmFieldQualifyUDHBase0;
    p_data.qual_data[3].qual_arg = 0;
    p_data.qual_data[3].qual_value = 0;
    p_data.qual_data[3].qual_mask = 0x3;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

    /** Create and attach group */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_term_ebtr_epmf", sizeof(fg_info.name));

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyUDHData3;
    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionEstimatedStartPacketStrip;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    for (int last_entry = 0; last_entry < SRV6_TERMINATION_ARRAY_SIZE; last_entry++)
    {
        /*
         * Add entry 
         */
        bcm_field_entry_info_t_init(&ent_info);
        ent_info.priority = 1;

        ent_info.nof_entry_quals = 1;
        ent_info.entry_qual[0].type = fg_info.qual_types[0];
        ent_info.entry_qual[0].value[0] = last_entry;
        ent_info.entry_qual[0].mask[0] = 0x1F;

        ent_info.nof_entry_actions = 1;
        ent_info.entry_action[0].type = fg_info.action_types[0];
        ent_info.entry_action[0].value[0] = termination_size[last_entry];

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id, &ent_info, &ent_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - estimate byte to remove in case of srv6 extended termination.
 *    First, Ipmf, will add to UDH-DATA-0 the "last_entry" field from the SRH header.
 *    Then, epmf, will add tcam entries, to estimate the btr per last entry value
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_extended_termination_btr(
    int unit,
    int relative_layer,
    bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_action_info_t action_info;
    bcm_field_action_t action_udh;
    bcm_field_group_t fg_udh;
    int ii = 0;
    int char_count;
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    char_count =
        sal_snprintf((char *) qual_info.name, sizeof(qual_info.name), "srv6_extended_termination_btr_%d",
                     relative_layer);

    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udh_qual));
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    char_count =
        sal_snprintf((char *) qual_info.name, sizeof(qual_info.name), "srv6_extended_termination_btr_type_%d",
                     relative_layer);
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &udh_type_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    char_count =
        sal_snprintf((char *) fg_info.name, sizeof(fg_info.name), "Srv6_extended_termination_btr_%d", relative_layer);

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = udh_qual;
    fg_info.qual_types[1] = udh_type_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 2;

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionUDHData3;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    char_count = sal_snprintf((char *) action_info.name, sizeof(action_info.name), "srv6 udh btr %d", relative_layer);
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_udh));

    if (char_count > (sizeof(action_info.name) - 1))
    {
        /** Just to avoid compilation error*/
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "too long string");
    }

    fg_info.action_types[0] = action_udh;
    fg_info.action_types[1] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_with_valid_bit[1] = FALSE;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_udh));

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /** Jump to start of SRH Base */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    /** Indicates the layer relative to FWD layer */
    attach_info.key_info.qual_info[0].input_arg = relative_layer;
    /** Take SRH "last entry" field */
    attach_info.key_info.qual_info[0].offset = 32;

    /*
     * Use type 3 to indicate Srv6
     * We write a constant value, which is 0x3, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = SRV6_UDH_BASE_VALUE;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_udh, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function Configures a new epmf rule as follow:
 *    if  (fwd_layer=srv6_endpoint && fwd_layer_qual[bit 0]=1) ==> epmf_bta=fwd_layer_qual[bit 1..9]
 *    fwd_layer_qualbits 1..9 holds the size of the Srv6 terminated bytes that will be made at the egress pipe.
 *    Note: fwd_layer_qual[bit 0]=1 is used as context preselection in this example,
 *          but it can also be used as action_with_valid_bit.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_srv6_config_endpoint_psp_epmf_bta(
    int unit)
{
    void *dest_char;
    bcm_field_context_t context_id;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_t fg_id;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info, action_info_get;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_egress_estimated_bta_btr_config))
    {
        /*
         * create a direct Extraction Field Group in ePMF
         */
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "Srv6EndPointPspEbtrCtx", sizeof(context_info.name));
        SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id));

        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageEgress, &qual_info_get));

        /***
         * create the context selection based on Fwd_layer=Eth
 */
        bcm_field_presel_entry_id_info_init(&p_id);
        bcm_field_presel_entry_data_info_init(&p_data);

        p_id.presel_id = dnx_data_field.preselector.srv6_endpoint_psp_bta_presel_id_epmf_get(unit);
        p_id.stage = bcmFieldStageEgress;
        p_data.entry_valid = TRUE;
        p_data.context_id = context_id;
        p_data.nof_qualifiers = 2;

        p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
        p_data.qual_data[0].qual_arg = 0;
        p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Endpoint;
        p_data.qual_data[0].qual_mask = 0x1F;
        p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
        p_data.qual_data[1].qual_arg = 0;
        p_data.qual_data[1].qual_value = 0x1;
        p_data.qual_data[1].qual_mask = 0x1 << qual_info_get.offset;

        SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

        bcm_field_group_info_t_init(&fg_info);
        bcm_field_group_attach_info_t_init(&attach_info);

        fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
        fg_info.stage = bcmFieldStageEgress;
        dest_char = &(fg_info.name[0]);
        sal_strncpy_s(dest_char, "Srv6_EP_Psp_Ebtr_epmf", sizeof(fg_info.name));
        fg_info.nof_quals = 1;
        fg_info.nof_actions = 1;
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 9;
        sal_strncpy_s((char *) (qual_info.name), "Srv6_EP_Psp_Ebtr_qual", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual));

        bcm_field_action_info_t_init(&action_info);
        SHR_IF_ERR_EXIT(bcm_field_action_info_get
                        (unit, bcmFieldActionEstimatedStartPacketStrip, bcmFieldStageEgress, &action_info_get));
        action_info.action_type = bcmFieldActionEstimatedStartPacketStrip;
        action_info.size = 9;
        action_info.prefix_size = action_info_get.size - action_info.size;
        action_info.prefix_value = 0;
        action_info.stage = bcmFieldStageEgress;
        sal_strncpy_s((char *) (action_info.name), "Srv6_EP_Psp_Ebtr_action", sizeof(action_info.name));
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_id));

        fg_info.nof_quals = 1;
        fg_info.qual_types[0] = qual;
        fg_info.action_types[0] = action_id;
        fg_info.action_with_valid_bit[0] = FALSE;
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];
        attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = qual_info_get.offset + 1;

        /** Create the field group. */
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

        /** Attach the created field group to the context. */
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Egress USP handling
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_egress_usp(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /************************/
    /** Egress USP PMF1-2 **/
    /**********************/

    /*
     * PMF1-2 configurations - Egress (USP)
     * -Presel Qualifiers: FWD layer is SRv6 + (FWD layer + 1) is also SRv6 + expected SL value == 0
     * -Actions:
     *        - get the NOF SIDs and store for PMF3
     *
     * Will be peformed for following cases:
     *        -In case of Egress USD, will be performed cause SL == 0
     */

    cint_field_srv6_egress_usp_info.ipmf1_context_id = 8;
    /** create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_egrs_usp_pmf1", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_egress_usp_info.ipmf1_context_id));

    /** create the Presel and attach it to the above context - Presel in higher priority then USD */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usp_presel_set(unit, cint_field_srv6_egress_usp_info.ipmf1_context_id, 12));

    /** FG Action: Save the same current IPv6 DIP in x4 UDH Containers in PMF2 + Nof_SIDs to Container for PMF3 */
    SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf2_udh(unit));

    /**********************/
    /** Egress USP PMF3 **/
    /*********************/

    /*
     * PMF3 configurations - Egress (USP)
     * -Presel Qualifiers: FWD layer is SRv6 + (FWD layer + 1) is also SRv6 + expected SL value == 0 (follows PMF1-2 context)
     * -Actions:
     *         -it sets the Parsing-Start-Type to 'SRv6_Beyond' for parser context
     *         -updates Bytes_to_Remove and Parsing_Start_Offset to terminate SRv6 and IPv6
     *
     * Will be peformed for following cases:
     *        -In case of Egress USD, will be performed cause SL == 0
     */

    cint_field_srv6_egress_usp_info.ipmf3_context_id = 8;
    /** create the context in PMF3 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_egrs_usp_pmf3", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_egress_usp_info.ipmf3_context_id));

    /** create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id,
                     cint_field_srv6_egress_usp_info.ipmf3_context_id, 12));

    /** configure FG in PMF3 - Parsing-Start-Type, Fwd-layer-idx, TCAM Mapping AC_LIF (IN_LIF1) to RCH Entry */
    SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;

}

/*
 * Endpoint PSP handling
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_endpoint_psp(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * PMF1-2 configurations - Egress PSP
     * -Presel Qualifiers: (FWD layer is SRv6 && expected SL == 1 && is_psp soc property)
     * -Actions:
     *         - saves IPv6 DIP in x4 UDH Containers  (used for End-Point USD/PSP, Egress PSP)
     *
     * Will be peformed for following cases:
     *         -In case of Egres PSP, will be performed cause SL == 1
     */

     /** Egress PSP - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_psp_udh_pmf1", 0,
                     &cint_field_srv6_endpoint_psp_info.ipmf1_context_id));

     /** Egress PSP - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_endpoint_psp_presel_set
                    (unit, cint_field_srv6_endpoint_psp_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_presel_id_ipmf1_get(unit), 0));

     /** Egress PSP - FG Action - Save IPv6 DIP in x4 UDH Containers in PMF2 */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_psp_ipmf2_udh(unit));

    /*
     * PMF3 configurations - Egress (PSP)
     * -Presel Qualifiers: (FWD layer is SRv6 + expected SL value == 1 && is_psp soc property)
     * -Actions:
     *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
     *           cause PSP is simply a Qualifier in SRv6_endpoint Layer in Egress
     *          -sets the Bytes-to-Remove-Fix to keep the IPv6 header, but delete the ETH in Ingress
     *          -updates TM nwk_header_append_size with IPv6 + SRv6 length to compensate
     *           the Egress deletion of whole IPv6 + Srv6
     *
     * Will be peformed for following cases:
     *         -In case of Egres PSP, will be performed cause SL == 1
     */

    /** Egress - create the context in PMF3 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_psp_ipmf3", 0,
                     &cint_field_srv6_endpoint_psp_info.ipmf3_context_id));

    /** Egress - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_endpoint_psp_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_psp_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_presel_id_ipmf3_get(unit)));

    /** Egress - Configure FG in PMF3 */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_psp_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Endpoint PSP handling
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_endpoint_psp_extended_cut(
    int unit)
{
    /*
     * Case of Endpoint PSP, with more than 4 SIDs, so need to do and extended pass just for cutting 4SIDs
     */
    SHR_FUNC_INIT_VARS(unit);

    cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id = 37;
     /** Egress PSP - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_psp_ext_cut_pmf1", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id));

     /** Egress PSP - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_endpoint_psp_presel_set
                    (unit, cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get(unit), 1));

    cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id = 37;
    /** Egress - create the context in PMF3 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_psp_ext_cut_pmf3", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id));

    /** Egress - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get(unit)));

    /** Egress - Configure FG in PMF3 */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_psp_extended_cut_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
shr_error_e
appl_dnx_field_srv6_init(
    int unit)
{
    int appl_psp_enable;
    int appl_reuced_enable;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
         dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)))
    {
        SHR_EXIT();
    }

    if ((dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_network_header_termination)) ||
        (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_egress_estimated_bta_btr_config)))
    {
        bcm_field_context_t context_id;
        bcm_field_context_t context_id_srv6_mpls;
        SHR_IF_ERR_EXIT(appl_dnx_field_srv6_beyond_minus_fwd_layer_ipmf1_context_set(unit, &context_id));
        SHR_IF_ERR_EXIT(appl_dnx_field_srv6_beyond_minus_2_with_mpls_fwd_layer_ipmf1_presel_set
                        (unit, &context_id_srv6_mpls));
        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_network_header_termination))
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_srv6_partial_fwd_layer_set(unit, context_id));
        }
        /** estimate Btr for srv6 extended termination - 1st round*/
        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_egress_estimated_bta_btr_config))
        {
            SHR_IF_ERR_EXIT(appl_ref_srv6_extended_termination_btr(unit, -1, context_id));
            SHR_IF_ERR_EXIT(appl_ref_srv6_extended_termination_btr(unit, -2, context_id_srv6_mpls));
            SHR_IF_ERR_EXIT(appl_ref_srv6_extended_termination_btr_epmf(unit));
        }
    }

    /** get the psp and reduced mode parameter from the config file */
    appl_psp_enable = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "srv6_psp_enable", 0);
    appl_reuced_enable = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "srv6_reduce_enable", 0);

    if (appl_psp_enable)
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchSRV6EgressPSPEnable, 1));
    }
    if (appl_reuced_enable)
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchSRV6ReducedModeEnable, 1));
    }

    /*
     * Classic End point handling
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint(unit, appl_psp_enable));

    /*
     * uSID End point handling - handle the uSID middle case, when SL is not decreased
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_usid(unit));

    /*
     * Egress USD handling
     */
    if (!dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        SHR_IF_ERR_EXIT(appl_ref_srv6_egress_usd(unit));
    }

    /*
     * Egress USP handling
     */

    SHR_IF_ERR_EXIT(appl_ref_srv6_egress_usp(unit));

    /*
     * Endpoint PSP handling
     */
    if (appl_psp_enable)
    {
        SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_psp(unit));
        SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_psp_extended_cut(unit));
    }

    /** EPMF - update BTA for PSP (PSP mode is always on) and relevant for USP as well  */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_psp_epmf_bta(unit));

exit:
    SHR_FUNC_EXIT;
}
