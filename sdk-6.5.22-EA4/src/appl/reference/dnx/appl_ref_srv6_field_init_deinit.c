/** \file appl_ref_srv6_field_init_deinit.c
 * 
 *
 * configuring PMFs for SRV6 processing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

#define PRESEL_QUAL_FWD_LAYER_SIZE (5)

#define MIN_NOF_SIDS (1)
#define EXTENDED_TERMINATION_NOF_SIDS_PER_CYCLE (3) /** how many SIDs are removed in an extended termination cycle */
#define SRV6_TERMINATION_ARRAY_SIZE (24)
#define SRV6_UDH_BASE_VALUE (0x3)

#define FAI_BITS_VALUE_NOT_UPDATE_TTL (0x4)

/**
 * All Endpoint cases must be resolved at ingress PMF, by building the next DIP on UDH.
 * than use the following UDH types to indicate if Egress should perform SL decrement or not.
 */
#define ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT (0xAA)
#define ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT_SINGLE_UDH (0x2) /** single UDH Type of 2b, which construct 0xAA together */
#define ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT (0x55)
#define ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH (0x1) /** single UDH Type of 2b, which construct 0x55 together */

#define IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_VAL_GET(_qual_val_) \
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get(unit, DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST,\
                                                  DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0, \
                                                  &_qual_val_));\
    _qual_val_ = _qual_val_ << 2;\
    _qual_val_ |= 0x2;

#define IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_MASK_GET(_qual_mask_) \
    _qual_mask_ = 0x3E;
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
/** bcm_int */
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
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
    bcm_field_qualify_t pmf2_zero_container_q;
    bcm_field_action_t pmf2_container_zero_a;
    bcm_field_action_t container_void_action;
} field_srv6_endpoint_info_t;

typedef struct cint_field_srv6_endpoint_gsid_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf2_tcam_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual_prefix;
    bcm_field_qualify_t udh_qual_next_gsid;
    bcm_field_qualify_t udh_qual_next_si;
    bcm_field_qualify_t udh_qual_zero_1_pad;
    bcm_field_qualify_t udh_qual_zero_2_pad;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t parsing_start_offset_valid_qual;
    bcm_field_qualify_t lr_fwd_offset_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_qualify_t pmf2_zero_container_q;
    bcm_field_action_t pmf2_container_zero_a;
    bcm_field_action_t container_void_action;
} field_srv6_endpoint_gsid_info_t;

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
    bcm_field_group_t ipmf1_tcam_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_psp_start_type_is_psp_valid_qual;
    bcm_field_qualify_t parsing_psp_start_type_inner_valid_qual;
    bcm_field_qualify_t srv6_psp_layer_type_qual;
    bcm_field_qualify_t parsing_ep_start_type_is_psp_valid_qual;
    bcm_field_qualify_t parsing_ep_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t egress_parsing_idx_qual;
    bcm_field_qualify_t fwd_layer_idx_qual;
    bcm_field_qualify_t msb_bits_tm_compensate_size;
    bcm_field_qualify_t parsing_nof_sids_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_qualify_t is_psp_qual;
    bcm_field_qualify_t parsing_psp_tm_comp_valid_qual;
    bcm_field_action_t container_void_action;
    bcm_field_action_t tcam_void_action;
    bcm_field_action_t psp_parsing_start_void_action;
    bcm_field_action_t endpoint_parsing_start_void_action;
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

field_srv6_endpoint_gsid_info_t cint_field_srv6_gsid_endpoint_info = {
    -1, -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount,
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
    -1, -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_psp_info_t cint_field_srv6_endpoint_psp_ext_cut_info = {
    -1, -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_psp_info_t cint_field_srv6_endpoint_usp_ext_cut_info = {
    -1, -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
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
 *   \param [in] ipmf1_ctx_id -  the context of PMF1-2 to which to attach the Presel
 *   \param [in] pmf3_ctx_id -  the context of PMF3 to be triggerred
 *   \param [in] presel_id -  PMF3 presel id
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
field_srv6_pmf3_by_pmf1_context_presel_set(
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
 * \brief - create a GSID Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 End-Point GSID node
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id -  the allocated number of the Presel
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_gsid_pmf1_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id)
{
    int rv = 0;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_id.presel_id = presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = ctx_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the bit in the port profile is set */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyAppTypePredefined;
    presel_entry_data.qual_data[0].qual_value = bcmFieldAppTypeSrv6GsidEndpoint;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_presel_set\n");
        return rv;
    }

    return BCM_E_NONE;
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

    /**  in SRv6 Endpoint, after IPv6 termination, we set fwd layer be again IPv6, for hashing of IPv6 layer in FWD */
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
        /** enter mutual Endpoint and PSP, in any case that SL!=0 */
        ipmf_presel_entry_data.qual_data[1].qual_value = 0;
        ipmf_presel_entry_data.qual_data[1].qual_mask = 1;
    }
    else
    {
        /** enter mutual Endpoint and PSP Ext, in any case that SL!=0, and NOF SIDs > 4 */
        ipmf_presel_entry_data.qual_data[1].qual_value = (0 | (1 << 8));
        ipmf_presel_entry_data.qual_data[1].qual_mask = (1 | (1 << 8));
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
    uint32 ipv6_srv6_sl_0_qual_val;
    uint32 ipv6_srv6_sl_0_qual_mask;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = 2;

     /** set 1st qualifier to egress, check if FWD layer is SRv6Beyond, after IPv6 termination */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[0].qual_arg = -1;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    ipmf_presel_entry_data.qual_data[0].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_VAL_GET(ipv6_srv6_sl_0_qual_val);
    IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_MASK_GET(ipv6_srv6_sl_0_qual_mask);
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    ipmf_presel_entry_data.qual_data[1].qual_arg = -1;
    ipmf_presel_entry_data.qual_data[1].qual_value = ipv6_srv6_sl_0_qual_val;
    ipmf_presel_entry_data.qual_data[1].qual_mask = ipv6_srv6_sl_0_qual_mask;

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
    int presel_id,
    bcm_field_layer_type_t layer_type,
    int is_ext_cut)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;
    uint32 ipv6_srv6_sl_0_qual_val;
    uint32 ipv6_srv6_sl_0_qual_mask;

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
    ipmf_presel_entry_data.qual_data[0].qual_arg = -1;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    ipmf_presel_entry_data.qual_data[0].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    /** set 2nd qualifier to egress, check if FWD+1 layer is also SRv6Beyond, after IPv6 termination */
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[1].qual_arg = 0;
    ipmf_presel_entry_data.qual_data[1].qual_value = layer_type;
    ipmf_presel_entry_data.qual_data[1].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    /*
     * Egress would occur if: -2nd qualifier check layer_qualifier[0] of SRv6 if == 1 (SL==0) in USP mode
     */
    IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_VAL_GET(ipv6_srv6_sl_0_qual_val);
    IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_MASK_GET(ipv6_srv6_sl_0_qual_mask);
    ipmf_presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    ipmf_presel_entry_data.qual_data[2].qual_arg = -1;
    if (is_ext_cut)
    {
        ipv6_srv6_sl_0_qual_val |= (1 << 8);
        ipv6_srv6_sl_0_qual_mask |= (1 << 8);
    }
    else
    {
        ipv6_srv6_sl_0_qual_val |= (0 << 8);
        ipv6_srv6_sl_0_qual_mask |= (1 << 8);
    }
    ipmf_presel_entry_data.qual_data[2].qual_value = ipv6_srv6_sl_0_qual_val;
    ipmf_presel_entry_data.qual_data[2].qual_mask = ipv6_srv6_sl_0_qual_mask;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF1-2
 *          -It copies the 128bits IPv6 DIP into x4 UDH (32bits)
 *          -Counts NOF SIDs and passes in container to PMF3
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x3 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing 
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT)
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
    int unit,
    int appl_psp_enable)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_info_t tcam_fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_attach_info_t tcam_attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_info_t tcam_action_info;
    bcm_field_fem_action_info_t fem_action_info;
    bcm_field_qualifier_info_get_t qual_info_get;

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

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_is_psp_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.is_psp_qual));

    /*
     * -Create a void action (user defined) for nof_sids + is_psp bits on container (bits 0-7, 9)
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8 + 1;
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
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_psp_info.srh_last_entry_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_psp_info.is_psp_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_psp_info.udh_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_psp_info.udh_type_qual;

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
     * PSP TCAM (when PSP is not Global)
     *     The TCAM Supports any combination of Next-SID and/or MyDIP combination,
     *     at entry set, mask can choose which qual be taken into TCAM account.
     *
     * -TCAM 3 qualifiers
     *      1.SL==1
     *      2.Next-SID
     *      3.MyDIP LIF's CS Profile bits
     * -Result: is_psp 1b result
     */
    if (!appl_psp_enable)
    {

        int lyr_qualifier_offset;
        bcm_field_qualify_t tcam_sl_qual;
        bcm_field_qualify_t tcam_next_dip_qual;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        sal_strncpy_s((char *) (qual_info.name), "srv6_tcam_sl_q", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &tcam_sl_qual));

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 128;
        sal_strncpy_s((char *) (qual_info.name), "srv6_tcam_next_dip_q", sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &tcam_next_dip_qual));

        /*
         * Define TCAM (SL==1 + DIP is defined) (if not Global)
 */
        bcm_field_group_info_t_init(&tcam_fg_info);
        tcam_fg_info.fg_type = bcmFieldGroupTypeTcam;
        tcam_fg_info.stage = bcmFieldStageIngressPMF1; /** in PMF1 so that PMF2 can use in FEM as cascaded */

        /** 2 Quals - SL==1 from SRv6 layer Qualifier, next IPv6 DIP (SID) */
        tcam_fg_info.nof_quals = 3;
        tcam_fg_info.qual_types[0] = tcam_sl_qual; /** Qualifier SL==1 */
        tcam_fg_info.qual_types[1] = tcam_next_dip_qual; /** user qualifier of next DIP */
        tcam_fg_info.qual_types[2] = bcmFieldQualifyInVportClass0; /** LIF of MyDIP by CS Profile bits */

        /*
         * 1 Action - 1b result "is_psp"
         */

        /*
         * -Create a void action (user defined): for the 1 bit TCAM hit (if SL==1 + DIP is found in TCAM)
         *  Void means is that no HW is involved, performed by SW only
         * -The container action will be performed by a FEM.
         */
        bcm_field_action_info_t_init(&tcam_action_info);
        tcam_action_info.action_type = bcmFieldActionVoid;
        tcam_action_info.size = 1;
        tcam_action_info.stage = bcmFieldStageIngressPMF1; /** in PMF1 so that PMF2 can use in FEM as cascaded */
        sal_strncpy_s((char *) (tcam_action_info.name), "srv6_is_psp_tcam_void_act", sizeof(tcam_action_info.name));
        SHR_IF_ERR_EXIT(bcm_field_action_create
                        (unit, 0, &tcam_action_info, &cint_field_srv6_endpoint_psp_info.tcam_void_action));

        tcam_fg_info.nof_actions = 1;
        tcam_fg_info.action_types[0] = cint_field_srv6_endpoint_psp_info.tcam_void_action;
        tcam_fg_info.action_with_valid_bit[0] = FALSE;
        dest_char = &(tcam_fg_info.name[0]);
        sal_strncpy_s(dest_char, "SRv6_psp_tcam", sizeof(tcam_fg_info.name));

        /** Define TCAM Fg and get its Fg-Id */
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &tcam_fg_info,
                                            &cint_field_srv6_endpoint_psp_info.ipmf1_tcam_fg));

        /*
         * TCAM Fg attach to Context
         */

        bcm_field_group_attach_info_t_init(&tcam_attach_info);
        tcam_attach_info.key_info.nof_quals = tcam_fg_info.nof_quals;
        tcam_attach_info.key_info.qual_types[0] = tcam_fg_info.qual_types[0];
        tcam_attach_info.key_info.qual_types[1] = tcam_fg_info.qual_types[1];
        tcam_attach_info.key_info.qual_types[2] = tcam_fg_info.qual_types[2];
        tcam_attach_info.payload_info.nof_actions = tcam_fg_info.nof_actions;
        tcam_attach_info.payload_info.action_types[0] = tcam_fg_info.action_types[0];

        /*
         * 1st Qualifier type is LayerRecords, and to use Record of Layer Qualifier, get its offset + 1 (for bit [1])
         */
        tcam_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
            /** LayerRecord of SRv6 */ ;
        tcam_attach_info.key_info.qual_info[0].input_arg = 1; /** taking from SRv6 */
        /**Get the offset (within the layer record) of the layer record qualifier (within the header).*/
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF1, &qual_info_get));

        lyr_qualifier_offset = qual_info_get.offset;
        tcam_attach_info.key_info.qual_info[0].offset = lyr_qualifier_offset + 1;
                                                                                /** take layer_qualifiers[fwd + 1][1]*/

        /*
         * 2nd Qualifier type user defined, to represnet next IPv6 DIP
         * - take the last 128 bit from the packet, i.e metadata that's passed in the pipe
         * - In non Ecologic support this is 128b LSB, in EcoLogic support, right after SRH
         */
        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
        {
            /** Jump to start of SRH Base */
            tcam_attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
            /** Indicates the layer relative to FWD layer */
            tcam_attach_info.key_info.qual_info[1].input_arg = 1;
            /** Take from offset of SRH Base + SRH Base - and it'll take the SID (MSB->LSB) */
            tcam_attach_info.key_info.qual_info[1].offset = 64;
        }
        else
        {
            /** Jump to Medadata which is located right after the 144B of packet */
            tcam_attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
            /** Jump to Medadata which is located right after the 144B of packet */
            tcam_attach_info.key_info.qual_info[1].offset = 0;
        }

        /*
         * 3rd Qualifier of LIF CS Profile
         */
        tcam_attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
        tcam_attach_info.key_info.qual_info[2].input_arg = 0;
        tcam_attach_info.key_info.qual_info[2].offset = 0;

        /*
         * Attach TCAM Fg to both Endpoint-PSP/Endpoint-PSP Extended Contexts
         */
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf1_tcam_fg,
                                                       cint_field_srv6_endpoint_psp_info.ipmf1_context_id,
                                                       &tcam_attach_info));

        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf1_tcam_fg,
                                                       cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id,
                                                       &tcam_attach_info));

    }

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

    /*
     * Leave 1 bit space between the two fields, since we increment the first field by 1.
     */
    fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
    fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    ii++;
    /*
     * Set next 1 bit as valid bit for PMF3 PSP/Endpoint
     */
    fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
    /** take from qualifier of 1 bit of either SL==1 qualifier (PSP global) or TCAM result (none global) */
    fem_action_info.fem_extraction[0].output_bit[ii].offset = ii - 1;
    ii++;

    /** set the remaining bits up to 0 */
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
     * i.e from the FWD layer which is IPv6, so arg+1 to get to SRv6,
     * and this field is in 32bits offset from there
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 32;

    /*
     * Case of PSP not Global
     * (is_psp qualifier) - either eqaul to SL==1 or TCAM result
     */

    /*
     * If PSP is not global, then valid bit for PMF3 is TCAM result (combination of SL==1 && DIP is defined in TCAM)
     * If PSP is global, then valid bit is simply direct extraction from SL==1 SRv6 Layer Qualifier
     */
    if (!appl_psp_enable)
    {
        int tcam_action_in_fg_offset;

        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeCascaded;
        /** input arg is the FG of TCAM */
        attach_info.key_info.qual_info[1].input_arg = cint_field_srv6_endpoint_psp_info.ipmf1_tcam_fg;

        /** get the TCAM action (result hit bit result) offset, in the TCAM fg*/
        SHR_IF_ERR_EXIT(bcm_field_group_action_offset_get(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf1_tcam_fg,
                                                          cint_field_srv6_endpoint_psp_info.tcam_void_action,
                                                          &tcam_action_in_fg_offset));

        attach_info.key_info.qual_info[1].offset = tcam_action_in_fg_offset;
    }
    else
    {

        int lyr_qualifier_offset;
        /*
         *  Qualifier type is LayerRecords, and to use Record of Layer Qualifier, get its offset + 1 (for bit [1])
         */

         /**Get the offset (within the layer record) of the layer record qualifier (within the header).*/
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF1, &qual_info_get));

        lyr_qualifier_offset = qual_info_get.offset;

        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
        attach_info.key_info.qual_info[1].input_arg = 1;  /** taking from SRv6 */
        attach_info.key_info.qual_info[1].offset = lyr_qualifier_offset + 1;/** take layer_qualifiers[fwd + 1][1]*/
    }

    /*
     * take the last 128 bit from the packet, i.e metadata that's passed in the pipe
     * the 128 lsb is IPv6 DIP, that the start processing writes to the metadata, offset 0
     */

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        /** Jump to start of SRH Base */
        attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerFwd;
        /** Indicates the layer relative to FWD layer */
        attach_info.key_info.qual_info[2].input_arg = 1;
        /** Take from offset of SRH Base + SRH Base - and it'll take the SID (MSB->LSB) */
        attach_info.key_info.qual_info[2].offset = 64;
    }
    else
    {
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[2].offset = 0;
    }

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT;

    /*
     * Attach Direct Extract FG to both Endpoint-PSP/Endpoint-PSP Extended contexts
     */
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf2_fg,
                                                   cint_field_srv6_endpoint_psp_info.ipmf1_context_id, &attach_info));

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf2_fg,
                                                   cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id,
                                                   &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2
 *          -It builds the next DIP on UDH as: {usid prefix(dip[0:31]),next usid list(DIP[48,127], zero 16 bits}
 *          -It set USH type to ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT, to identify at egress that no need to perform SL--
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x2 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing 
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT)
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
    bcm_field_action_info_t action_info;

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

    /** Qualifier to set PMF2 container to PMF3 bits [0-9] to zero - bit[9] is 'is_psp' activator in PMF3 endpoint_psp context */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usid_zero_container_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_usid_endpoint_info.pmf2_zero_container_q));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_usid_endpoint_UDH", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 5;
    fg_info.qual_types[0] = udh_qual_0;
    fg_info.qual_types[1] = udh_qual_1;
    fg_info.qual_types[2] = udh_qual_2;
    fg_info.qual_types[3] = cint_field_srv6_usid_endpoint_info.udh_type_qual;
    fg_info.qual_types[4] = cint_field_srv6_usid_endpoint_info.pmf2_zero_container_q;

    /*
     * Set actions
     */
    fg_info.nof_actions = 9;

    /** user action to zero all PMF2 container, to not intervene with PMF3 endpoint_psp 'is_psp' bit[9] - using min size of qual */
    bcm_field_action_info_t_init(&action_info);
    action_info.size = 1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "PMF2_usid_container_zero_a", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_usid_endpoint_info.pmf2_container_zero_a));

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
    fg_info.action_types[8] = cint_field_srv6_usid_endpoint_info.pmf2_container_zero_a;
    fg_info.action_with_valid_bit[8] = FALSE;

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
     * We write a constant value, which is ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT;

    /*
     * set zeros on PMF2 to PMF3 container, so that it won't activate 'is_psp' bit[9] in PMF3
     */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[4].input_arg = 0x0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_usid_endpoint_info.ipmf2_fg,
                                                   cint_field_srv6_usid_endpoint_info.ipmf1_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF2
 *          -It copies creates the x4 UDH (4 x 32bits) as next IPv6 DIP in following way:
 *                 [prefix | usid n | usid n-1 |... usid 0] ---> [prefix | usid n-1 |... | 0000]
 *                 - Takes the IPv6 DIP, leaves the 32b Prefix
 *                 - Shifts left the 16b u-SIDs in rest of DIP bits
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x2 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by start processing 
 *                  2. 8bits of Const Type (value 0xAA)
 *
 *          -x8 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_gsid_end_point_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_entry_info_t tcam_ent_info;
    bcm_field_entry_t tcam_entry_id;
    bcm_field_action_info_t action_info;

    void *dest_char;
    int ii = 0;

    int gen_data_offset =
        dnx_data_field.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_0)->offset;
    int container3_40_offset = gen_data_offset + 160 + 96 + 40;
    int container5_24_offset = container3_40_offset + 40 + 24;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 48;
    sal_strncpy_s((char *) (qual_info.name), "srv6_gsid_prefix_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_gsid_endpoint_info.udh_qual_prefix));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 32;
    sal_strncpy_s((char *) (qual_info.name), "srv6_gsid_next_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_gsid_endpoint_info.udh_qual_next_gsid));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    sal_strncpy_s((char *) (qual_info.name), "srv6_gsid_nxtsi_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_gsid_endpoint_info.udh_qual_next_si));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 30;
    sal_strncpy_s((char *) (qual_info.name), "srv6_gsid_zero_1_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_gsid_endpoint_info.udh_qual_zero_1_pad));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 16;
    sal_strncpy_s((char *) (qual_info.name), "srv6_gsid_zero_2_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_gsid_endpoint_info.udh_qual_zero_2_pad));

    /** Qualifier to set PMF2 container to PMF3 bits [0-9] to zero - bit[9] is 'is_psp' activator in PMF3 endpoint_psp context */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_gsid_zero_container_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_gsid_endpoint_info.pmf2_zero_container_q));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_gsid_endpoint", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 6;
    fg_info.qual_types[0] = cint_field_srv6_gsid_endpoint_info.udh_qual_next_si;
    fg_info.qual_types[1] = cint_field_srv6_gsid_endpoint_info.udh_qual_zero_1_pad;
    fg_info.qual_types[2] = cint_field_srv6_gsid_endpoint_info.udh_qual_zero_2_pad;
    fg_info.qual_types[3] = cint_field_srv6_gsid_endpoint_info.udh_qual_next_gsid;
    fg_info.qual_types[4] = cint_field_srv6_gsid_endpoint_info.udh_qual_prefix;
    fg_info.qual_types[5] = cint_field_srv6_gsid_endpoint_info.pmf2_zero_container_q;

    /*
     * Set actions
     */
    fg_info.nof_actions = 5;

    /** user action to zero all PMF2 container, to not intervene with PMF3 endpoint_psp 'is_psp' bit[9] - using min size of qual */
    bcm_field_action_info_t_init(&action_info);
    action_info.size = 1;
    action_info.prefix_size = 31;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    action_info.action_type = bcmFieldActionContainer;
    dest_char = &(action_info.name[0]);
    sal_strncpy_s(dest_char, "PMF2_gsid_container_zero_a", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_gsid_endpoint_info.pmf2_container_zero_a));

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

    fg_info.action_types[4] = cint_field_srv6_gsid_endpoint_info.pmf2_container_zero_a;
    fg_info.action_with_valid_bit[4] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_gsid_endpoint_info.ipmf2_fg));

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
     * 2b next SI from container5_24b as qualifier source
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = container5_24_offset;

    /*
     * 30b zero padding for the zero pad qualifier source
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = 0x0;

    /*
     * 16b zero padding for the zero pad qualifier source
     */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 0x0;

    /*
     * 32b next GSID from container3_40b as qualifier source
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].offset = container3_40_offset;

    /*
     * 48b existing IPv6 GSDI Prefix from IPv6 DIP's 64b MSB, from header MSB 320-128b
     */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[4].input_arg = 1;
    attach_info.key_info.qual_info[4].offset = 192;

    /*
     * set zeros on PMF2 to PMF3 container, so that it won't activate 'is_psp' bit[9] in PMF3
     */
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[5].input_arg = 0x0;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_gsid_endpoint_info.ipmf2_fg,
                                                   cint_field_srv6_gsid_endpoint_info.ipmf1_context_id, &attach_info));

    /*
     * TCAM Field Group
     * - Create a FG with 1xQualifier (SI value) and corresponding 1xAction (UDH Base Type)
     * - Create 4 TCAM entries between SI to  UDH Base Type
     *      -SI == 0 --> UDH Base Type = 0xAA (update SL)
     *      -SI == {1,2,3} --> UDH Base Type = 0x55 (don't update SL)
     */

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_srv6_gsid_endpoint_info.udh_qual_next_si;
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionUDHBase3;
    fg_info.action_types[1] = bcmFieldActionUDHBase2;
    fg_info.action_types[2] = bcmFieldActionUDHBase1;
    fg_info.action_types[3] = bcmFieldActionUDHBase0;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_GSID_UDH_Base_TCAM", sizeof(fg_info.name));

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info,
                                        &cint_field_srv6_gsid_endpoint_info.ipmf2_tcam_fg));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    attach_info.payload_info.action_types[3] = fg_info.action_types[3];

    /*
     * 2b next SI from container5_24b as qualifier source
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = container5_24_offset;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_gsid_endpoint_info.ipmf2_tcam_fg,
                                                   cint_field_srv6_gsid_endpoint_info.ipmf1_context_id, &attach_info));

    /*
     * Add entry for next SI==0 (current SI==1), don't perform SL decrement (UDH Base = 0x55)
     */
    bcm_field_entry_info_t_init(&tcam_ent_info);
    tcam_ent_info.priority = 1;
    tcam_ent_info.nof_entry_quals = fg_info.nof_quals;
    tcam_ent_info.entry_qual[0].type = fg_info.qual_types[0];
    tcam_ent_info.entry_qual[0].value[0] = 0x0;
    tcam_ent_info.entry_qual[0].mask[0] = 0x3;

    tcam_ent_info.nof_entry_actions = fg_info.nof_actions;
    tcam_ent_info.entry_action[0].type = fg_info.action_types[0];
    tcam_ent_info.entry_action[1].type = fg_info.action_types[1];
    tcam_ent_info.entry_action[2].type = fg_info.action_types[2];
    tcam_ent_info.entry_action[3].type = fg_info.action_types[3];
    tcam_ent_info.entry_action[0].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[1].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[2].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[3].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;

    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_gsid_endpoint_info.ipmf2_tcam_fg, &tcam_ent_info, &tcam_entry_id));

    /*
     * Add entry for next SI==1 (current SI==2), don't perform SL decrement (UDH Base = 0x55)
     */
    tcam_ent_info.entry_qual[0].value[0] = 0x1;
    tcam_ent_info.entry_action[0].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[1].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[2].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[3].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;

    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_gsid_endpoint_info.ipmf2_tcam_fg, &tcam_ent_info, &tcam_entry_id));

    /*
     * Add entry for next SI==2, (current SI==3) don't perform SL decrement (UDH Base = 0x55)
     */
    tcam_ent_info.entry_qual[0].value[0] = 0x2;
    tcam_ent_info.entry_action[0].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[1].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[2].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[3].value[0] = ENDPOINT_UDH_TYPE_WITHOUT_SL_DECREMENT_SINGLE_UDH;

    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_gsid_endpoint_info.ipmf2_tcam_fg, &tcam_ent_info, &tcam_entry_id));

    /*
     * Add entry for next SI==3 (current SI==0), perform SL decrement (UDH Base = 0xAA)
     */
    tcam_ent_info.entry_qual[0].value[0] = 0x3;
    tcam_ent_info.entry_action[0].value[0] = ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[1].value[0] = ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[2].value[0] = ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT_SINGLE_UDH;
    tcam_ent_info.entry_action[3].value[0] = ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT_SINGLE_UDH;

    SHR_IF_ERR_EXIT(bcm_field_entry_add
                    (unit, 0, cint_field_srv6_gsid_endpoint_info.ipmf2_tcam_fg, &tcam_ent_info, &tcam_entry_id));

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
 *                  2. 8bits of Const Type (value ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT)
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
     * i.e from the FWD layer -1 which is IPv6, and this field is in 32bits offset, after skipping 320 bits to the SRH
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = -1;
    attach_info.key_info.qual_info[0].offset = 320 + 32;

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
     * We write a constand value, which is ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = ENDPOINT_UDH_TYPE_WITH_SL_DECREMENT;

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
    attach_info.key_info.qual_info[0].offset = 320 + 32;
    attach_info.key_info.qual_info[0].input_arg = -1;

    /*
     * Take the "Next Protocol" field from the SRH header (the FWD layer) * - this field is 0 bits offset in the SRH
     * first * - qualifer is of size 8bits, so they will be taken from offset 0 in SRH header
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = -1;
    attach_info.key_info.qual_info[1].offset = 320;

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
     * Contains 5 bits with value 10 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint_PSP) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x2A => 1(0|1010 = 10)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x2A; /** Contains 5 bits with value 10 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint_PSP) and 1 bit with value 1 (valid bit)*/

    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_egress_usp_info.ipmf3_context_id, &param_info));

    /*
     * -Parsing-Start-Type Qualifier for valid bit - will be set from the PMF2 container 1bit "is_psp"
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

    /** take from MetaData, where the Key_Gen_Val of 0x2A is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x2A is stored, 5 bits, Qual's size which is 20*/
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
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 10 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint_PSP) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x2A => 1(0|1010 = 10)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x2A + /** Contains 5 bits with value 10 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint_PSP) */
                                  /** + '1' valid bit for all cases that are mutual to Endpoint and PSP */
        (0x14 << 6);                   /** Contains 5 bits with value 20 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint) */

    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_endpoint_psp_info.ipmf3_context_id, &param_info));

    /*
     * Endpoint Only - Parsing Start Type, consisting of 1b Valid bit on 'is_psp' bit from PMF2 container ('0' polarity)
     */

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_ep_is_psp_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.parsing_ep_start_type_is_psp_valid_qual));

     /** Endpoint - inner valid bit */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_ep_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.parsing_ep_start_type_valid_qual));

    /** Endpoint Only - Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_ep_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.srv6_endpoint_layer_type_qual));

    /*
     * PSP Only - Parsing Start Type, consisting of 1b Valid bit on 'is_psp' bit from PMF2 container ('1' polarity)
     */

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_is_psp_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.parsing_psp_start_type_is_psp_valid_qual));

    /** PSP Parsing Start Type inner valid bit */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.parsing_psp_start_type_inner_valid_qual));

    /** PSP Only - Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.srv6_psp_layer_type_qual));

    /*
     * Mutual Endpoint and PSP -  Configure the fwd_layer_idx and egress_parsing_idx
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

    /*
     * PSP Only - TM Compensation valid bit - consisting of 1b Valid bit on 'is_psp' bit from PMF2 container ('1' polarity)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_tm_comp_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_info.parsing_psp_tm_comp_valid_qual));

    /** PSP Only - NOF SIDs Qualifier - to be used to choose the right TM compensate size entry per number of SIDs */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_nof_sids_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_info.parsing_nof_sids_qual));

    /** PSP Only 5 MSB bit for TM compensation size (which is 8bits) */
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
    fg_info.nof_quals = 11;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_psp_info.parsing_ep_start_type_is_psp_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_psp_info.parsing_ep_start_type_valid_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_psp_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_psp_info.parsing_psp_start_type_is_psp_valid_qual;
    fg_info.qual_types[4] = cint_field_srv6_endpoint_psp_info.parsing_psp_start_type_inner_valid_qual;
    fg_info.qual_types[5] = cint_field_srv6_endpoint_psp_info.srv6_psp_layer_type_qual;
    fg_info.qual_types[6] = cint_field_srv6_endpoint_psp_info.fwd_layer_idx_qual;
    fg_info.qual_types[7] = cint_field_srv6_endpoint_psp_info.egress_parsing_idx_qual;
    fg_info.qual_types[8] = cint_field_srv6_endpoint_psp_info.parsing_psp_tm_comp_valid_qual;
    fg_info.qual_types[9] = cint_field_srv6_endpoint_psp_info.parsing_nof_sids_qual;
    fg_info.qual_types[10] = cint_field_srv6_endpoint_psp_info.msb_bits_tm_compensate_size;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */
    fg_info.nof_actions = 5;

    /*
     * Endpoint Only - Parsing-Start-Type action (no Valid bit) - be done by default
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */

    /** create a new void action, to override above PSP case when EndPoint case (action types cannot be same) */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartType;
    action_info.size = 6; /** signal is 5bit + 1bit inner valid bit */
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_endpoint_psp_info.endpoint_parsing_start_void_action));

    fg_info.action_types[0] = cint_field_srv6_endpoint_psp_info.endpoint_parsing_start_void_action;
    fg_info.action_with_valid_bit[0] = TRUE;

    /*
     * PSP Only - Parsing-Start-Type action (with Valid bit)
     * Q[2] - action outer 'is_psp' valid bit (polarity '1')
     * Q[3] - action inner valid bit
     * Q[4] - the actual value (10)
     */

    /** create a new void action, to override above PSP case when EndPoint case (action types cannot be same) */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartType;
    action_info.size = 6; /** signal is 5bit + 1bit inner valid bit */
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_endpoint_psp_info.psp_parsing_start_void_action));

    fg_info.action_types[1] = cint_field_srv6_endpoint_psp_info.psp_parsing_start_void_action;
    fg_info.action_with_valid_bit[1] = TRUE;

    /*
     * Endpoint and PSP  - FWD Layer Index and Egress Forward Index
     * Q[4] - fwd_layer_idx (3bits)
     * Q[5] - egress_parsing_idx (3bits)
     */
    fg_info.action_types[2] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    fg_info.action_types[3] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[3] = FALSE;

    /*
     * PSP Only - TM Compensation Size (with Valid bit)
     * Q[6] - valid bit (1bit)
     * Q[7] - nof sids - represents the TM entry num  (3bits)
     * Q[8] - msb bits compensation size - '0' (5bits)
     */
    fg_info.action_types[4] = bcmFieldActionAppendPointerCompensation;
    fg_info.action_with_valid_bit[4] = TRUE;

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

    /*
     * Endpoint Only - Set Action for PSP with valid bit with polarity '0' on 'is_psp' bit - Action[1]
     */
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);
    attach_info.payload_info.action_info[0].valid_bit_polarity = 0;

    /** take from MetaData, where the container from PMF2 is holding the "is_psp" bit, bit[9]*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = container_offset + 9;

    /** take from MetaData, constant 1 - inner valid bit */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x14 is stored, 5 bits, Qual's size which is 20 Endpoint Parsing Start Type*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 6;

    /*
     * PSP Only - Set Action for PSP with valid bit with polarity '1' on 'is_psp' bit - Action[1]
     */
    attach_info.payload_info.action_info[1].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    attach_info.payload_info.action_info[1].valid_bit_polarity = 1;

    /** take from MetaData, where the container from PMF2 is holding the "is_psp" bit, bit[9]*/
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].offset = container_offset + 9;

    /** take from MetaData, constant 1 - inner valid bit */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0xA is stored, 5 bits, Qual's size which is 10 Endpoint PSP Parsing Start Type*/
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[5].offset = key_gen_var_offset + 0;

    /*
     * Endpoint and PSP - Set the value of fwd_layer_idx - Actions[2,3]
     */
    attach_info.key_info.qual_info[6].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[6].input_arg = 1;

    /** Endpoint and PSP - Set the value of egress_parsing_idx */
    attach_info.key_info.qual_info[7].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[7].input_arg = 1;

    /*
     * PSP Only - TM Compensation - Set Action for PSP with valid bit with polarity '1' on 'is_psp' bit
     * Action[4]
     */
    attach_info.payload_info.action_info[4].priority = BCM_FIELD_ACTION_POSITION(0, 2);
    attach_info.payload_info.action_info[4].valid_bit_polarity = 1;

    /** take from MetaData, where the container from PMF2 is holding the "is_psp" bit, bit[9]*/
    attach_info.key_info.qual_info[8].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[8].offset = container_offset + 9;

    /** 3bits LSB bits of NWK_HEADER_APPEND_SIZE - taking NOF SIDs from IPMF2 container cascading */
    attach_info.key_info.qual_info[9].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[9].offset = container_offset;

    /** 5bits MSB bits of the action which writes to NWK_HEADER_APPEND_SIZE pointer, which is 8bits */

    attach_info.key_info.qual_info[10].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[10].input_arg = 0x1E;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_info.ipmf3_fg,
                                                   cint_field_srv6_endpoint_psp_info.ipmf3_context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The function configures PMF3 for Endpoint / PSP Extended actions
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
    int container_offset;

    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 10 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint_PSP) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x2A => 1(0|1010 = 10)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x37 + /** Contains 5 bits with value 23 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint_PSP_Ext) */
                                  /** + '1' valid bit for all cases that are mutual to Endpoint and PSP */
        (0x14 << 6);              /** Contains 5 bits with value 20 (PARSING_START_TYPE_ENCODING_SRv6_Endpoint) */

    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                                cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id,
                                                &param_info));

    /*
     * Endpoint Only - Parsing Start Type, consisting of 1b Valid bit on 'is_psp' bit from PMF2 container ('0' polarity)
     */

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_ep_is_psp_ext_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_ep_start_type_is_psp_valid_qual));

     /** Endpoint - inner valid bit */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_ep_strtype_ext_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_ep_start_type_valid_qual));

    /** Endpoint Only - Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_ep_layer_ext_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.srv6_endpoint_layer_type_qual));

    /*
     * PSP Ext Only - Parsing Start Type, consisting of 1b Valid bit on 'is_psp' bit from PMF2 container ('1' polarity)
     */

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_ext_is_psp_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_psp_start_type_is_psp_valid_qual));

    /** PSP Parsing Start Type inner valid bit */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_ext_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_psp_start_type_inner_valid_qual));

    /** PSP Only - Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_ext_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.srv6_psp_layer_type_qual));

    /*
     * Mutual Endpoint and PSP -  Configure the fwd_layer_idx and egress_parsing_idx
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_fwd_psp_ext_layer_idx_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_ext_cut_info.fwd_layer_idx_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egress_psp_ext_parsing_idx_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_ext_cut_info.egress_parsing_idx_qual));

    /*
     * PSP Only - TM Compensation valid bit - consisting of 1b Valid bit on 'is_psp' bit from PMF2 container ('1' polarity)
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_ext_tm_comp_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_psp_tm_comp_valid_qual));

    /** PSP Only - NOF SIDs Qualifier - to be used to choose the right TM compensate size entry per number of SIDs */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_nof_sids_psp_ext_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_ext_cut_info.parsing_nof_sids_qual));

    /** PSP Only 5 MSB bit for TM compensation size (which is 8bits) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_msb_bit_tm_compensation_psp_ext_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_psp_ext_cut_info.msb_bits_tm_compensate_size));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_psp_ext_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 11;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_ep_start_type_is_psp_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_ep_start_type_valid_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_psp_ext_cut_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_psp_start_type_is_psp_valid_qual;
    fg_info.qual_types[4] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_psp_start_type_inner_valid_qual;
    fg_info.qual_types[5] = cint_field_srv6_endpoint_psp_ext_cut_info.srv6_psp_layer_type_qual;
    fg_info.qual_types[6] = cint_field_srv6_endpoint_psp_ext_cut_info.fwd_layer_idx_qual;
    fg_info.qual_types[7] = cint_field_srv6_endpoint_psp_ext_cut_info.egress_parsing_idx_qual;
    fg_info.qual_types[8] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_psp_tm_comp_valid_qual;
    fg_info.qual_types[9] = cint_field_srv6_endpoint_psp_ext_cut_info.parsing_nof_sids_qual;
    fg_info.qual_types[10] = cint_field_srv6_endpoint_psp_ext_cut_info.msb_bits_tm_compensate_size;

    /*
     * Set actions - Width of the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */
    fg_info.nof_actions = 5;

    /*
     * Endpoint Only - Parsing-Start-Type action (no Valid bit) - be done by default
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */

    /** create a new void action, to override above PSP case when EndPoint case (action types cannot be same) */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartType;
    action_info.size = 6; /** signal is 5bit + 1bit inner valid bit */
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info,
                     &cint_field_srv6_endpoint_psp_ext_cut_info.endpoint_parsing_start_void_action));

    fg_info.action_types[0] = cint_field_srv6_endpoint_psp_ext_cut_info.endpoint_parsing_start_void_action;
    fg_info.action_with_valid_bit[0] = TRUE;

    /*
     * PSP Only - Parsing-Start-Type action (with Valid bit)
     * Q[2] - action outer 'is_psp' valid bit (polarity '1')
     * Q[3] - action inner valid bit
     * Q[4] - the actual value (10)
     */

    /** create a new void action, to override above PSP case when EndPoint case (action types cannot be same) */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartType;
    action_info.size = 6; /** signal is 5bit + 1bit inner valid bit */
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    SHR_IF_ERR_EXIT(bcm_field_action_create
                    (unit, 0, &action_info, &cint_field_srv6_endpoint_psp_ext_cut_info.psp_parsing_start_void_action));

    fg_info.action_types[1] = cint_field_srv6_endpoint_psp_ext_cut_info.psp_parsing_start_void_action;
    fg_info.action_with_valid_bit[1] = TRUE;

    /*
     * Endpoint and PSP  - FWD Layer Index and Egress Forward Index
     * Q[4] - fwd_layer_idx (3bits)
     * Q[5] - egress_parsing_idx (3bits)
     */
    fg_info.action_types[2] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    fg_info.action_types[3] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[3] = FALSE;

    /*
     * PSP Only - TM Compensation Size (with Valid bit)
     * Q[6] - valid bit (1bit)
     * Q[7] - nof sids - represents the TM entry num  (3bits)
     * Q[8] - msb bits compensation size - '0' (5bits)
     */
    fg_info.action_types[4] = bcmFieldActionAppendPointerCompensation;
    fg_info.action_with_valid_bit[4] = TRUE;

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
    container_offset = qual_info_get.offset;

    /*
     * Endpoint Only - Set Action for PSP with valid bit with polarity '0' on 'is_psp' bit - Action[1]
     */
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);
    attach_info.payload_info.action_info[0].valid_bit_polarity = 0;

    /** take from MetaData, where the container from PMF2 is holding the "is_psp" bit, bit[9]*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = container_offset + 9;

    /** take from MetaData, constant 1 - inner valid bit */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x14 is stored, 5 bits, Qual's size which is 20 Endpoint Parsing Start Type*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 6;

    /*
     * PSP Only - Set Action for PSP with valid bit with polarity '1' on 'is_psp' bit - Action[1]
     */
    attach_info.payload_info.action_info[1].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    attach_info.payload_info.action_info[1].valid_bit_polarity = 1;

    /** take from MetaData, where the container from PMF2 is holding the "is_psp" bit, bit[9]*/
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].offset = container_offset + 9;

    /** take from MetaData, constant 1 - inner valid bit */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0xA is stored, 5 bits, Qual's size which is 10 Endpoint PSP Parsing Start Type*/
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[5].offset = key_gen_var_offset + 0;

    /*
     * Endpoint and PSP - Set the value of fwd_layer_idx - Actions[2,3]
     */
    attach_info.key_info.qual_info[6].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[6].input_arg = 1;

    /** Endpoint and PSP - Set the value of egress_parsing_idx */
    attach_info.key_info.qual_info[7].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[7].input_arg = 1;

    /*
     * PSP Only - TM Compensation - Set Action for PSP with valid bit with polarity '1' on 'is_psp' bit
     * Action[4]
     */
    attach_info.payload_info.action_info[4].priority = BCM_FIELD_ACTION_POSITION(0, 2);
    attach_info.payload_info.action_info[4].valid_bit_polarity = 1;

    /** take from MetaData, where the container from PMF2 is holding the "is_psp" bit, bit[9]*/
    attach_info.key_info.qual_info[8].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[8].offset = container_offset + 9;

    /** 3bits LSB bits of NWK_HEADER_APPEND_SIZE - in psp extended, compensating with always 3SIDs size, stored in TM Profile '0' */
    attach_info.key_info.qual_info[9].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[9].input_arg = 0;

    /** 5bits MSB bits of the action which writes to NWK_HEADER_APPEND_SIZE pointer, which is 8bits */
    attach_info.key_info.qual_info[10].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[10].input_arg = 0x1E;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_fg,
                                                   cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id,
                                                   &attach_info));

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
field_srv6_config_endpoint_usp_extended_cut_ipmf3_de(
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
                                                cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_context_id,
                                                &param_info));

    /*
     * -We build the number 20 which is hex 0x14 for parsing start type
     * -Parsing-Start-Type Qualifier for valid bit
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_ext_cut_strtype_val_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_usp_ext_cut_info.parsing_psp_start_type_inner_valid_qual));

    /**Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_usp_ext_cut_layer_type_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_usp_ext_cut_info.srv6_psp_layer_type_qual));

    /*
     * Configure the fwd_layer_idx and egress_parsing_idx.
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_fwd_layer_idx_usp_ext_cut_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_usp_ext_cut_info.fwd_layer_idx_qual));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egress_parsing_idx_usp_ext_cut_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info,
                                               &cint_field_srv6_endpoint_usp_ext_cut_info.egress_parsing_idx_qual));

    /** NOF SIDs Qualifier, 3b LSB in profile idx - to be used to choose the right TM compensate size entry per number of SIDs */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_nof_sids_usp_ext_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_usp_ext_cut_info.parsing_nof_sids_qual));

    /** 5 MSB bit for TM compensation size profile idx (which is 8bits) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_msb_bit_tm_compensation_usp_ext_q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create
                    (unit, 0, &qual_info, &cint_field_srv6_endpoint_usp_ext_cut_info.msb_bits_tm_compensate_size));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_usp_ext_cut_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 6;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_usp_ext_cut_info.parsing_psp_start_type_inner_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_usp_ext_cut_info.srv6_psp_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_usp_ext_cut_info.fwd_layer_idx_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_usp_ext_cut_info.egress_parsing_idx_qual;
    fg_info.qual_types[4] = cint_field_srv6_endpoint_usp_ext_cut_info.parsing_nof_sids_qual;
    fg_info.qual_types[5] = cint_field_srv6_endpoint_usp_ext_cut_info.msb_bits_tm_compensate_size;

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
     * Q[7] - nof sids - represents the TM entry num  (3bits)
     * Q[8] - msb bits compensation size  (5bits)
     */
    fg_info.action_types[3] = bcmFieldActionAppendPointerCompensation;
    fg_info.action_with_valid_bit[3] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_fg));

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

    /** 3bits LSB bits of NWK_HEADER_APPEND_SIZE - in psp extended, compensating with always 3SIDs size, stored in TM Profile '0' */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[4].input_arg = 0;

    /** 5bits MSB bits of the action which writes to NWK_HEADER_APPEND_SIZE pointer, which is 8bits */
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[5].input_arg = 0x1E;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_fg,
                                                   cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_context_id,
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
    attach_info.key_info.qual_info[1].input_arg = -1; /** Offset of FWD layer -1 is the ETH offset */
    attach_info.key_info.qual_info[1].offset = lr_offset_offset;
    /** Parsing-Start-Offset 7th bit (+64) Always set */
    fg_info.qual_types[2] = cint_field_srv6_egress_usd_info.large_parsing_start_offset_efes_valid_qual;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 1;
    /**
     * parsing start offset is ETH size + 64 (which is IPv6, SRH and first SID)
     * Rest of the SID will be compensate by the RCY outlif
 */
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
    attach_info.key_info.qual_info[0].input_arg = 0;

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
    SHR_IF_ERR_EXIT(dnx_dbal_fields_enum_value_get(unit, "PARSING_START_TYPE_ENCODING", "MPLS_DUMMY", &hw_value));

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
    uint32 ipv6_srv6_sl_0_qual_val;
    uint32 ipv6_srv6_sl_0_qual_mask;

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
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = -1;
    p_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    p_data.qual_data[0].qual_mask = 0x1F;

    IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_VAL_GET(ipv6_srv6_sl_0_qual_val);
    IPV6_ADDITIONAL_HEADER_SRV6_SL_0_QUAL_MASK_GET(ipv6_srv6_sl_0_qual_mask);
    p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    p_data.qual_data[1].qual_arg = -1;
    p_data.qual_data[1].qual_value = ipv6_srv6_sl_0_qual_val;
    p_data.qual_data[1].qual_mask = ipv6_srv6_sl_0_qual_mask;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &p_id, &p_data));

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
    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
                    (unit, cint_field_srv6_usid_endpoint_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_psp_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_usid_presel_id_ipmf3_get(unit)));

exit:
    SHR_FUNC_EXIT;

}

/*
 * gSID End point handling
 */
static shr_error_e
appl_ref_srv6_endpoint_gsid(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * End-Point GSID  - PMF1-2 configurations
     * -Higher Priority then Egress PMF1-2 context because Presel ID is lower
     * -Presel Qualifiers:
     *      -FWD layer is SRv6
     *      -In-LIF Profile == 1 (GSID) and since with SRv6 layer as FWD, we're in SRv6 CS namespace
     *
     * -Actions:
     *         - saves IPv6 DIP in x4 UDH Containers  (used for End-Point USP/PSP, Egress PSP)
     *         - DIP constructed from:
     *                      -64b Prefix from IPv6 original DIP
     *                      -32b next-gsid from container3_40b
     *                      -2b  next-si from container2_24b
     *                      -30b padding with zeros
     */

    /** GSID End-Point - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_gsid_udh_pmf1", 0,
                     &cint_field_srv6_gsid_endpoint_info.ipmf1_context_id));

    /** GSID End-Point - create the Presel and attach it to the above context - Higher Priority then Egress Context */
    SHR_IF_ERR_EXIT(field_srv6_gsid_pmf1_presel_set(unit, cint_field_srv6_gsid_endpoint_info.ipmf1_context_id,
                                                    dnx_data_field.
                                                    preselector.srv6_endpoint_gsid_presel_id_ipmf1_get(unit)));

    /** GSID End-Point- FG Action: Save IPv6 DIP in x4 UDH Containers in PMF2 */
    SHR_IF_ERR_EXIT(field_srv6_config_gsid_end_point_ipmf2_udh(unit));

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
    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
                    (unit, cint_field_srv6_gsid_endpoint_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_psp_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_gsid_presel_id_ipmf3_get(unit)));

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
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usd_presel_set(unit, cint_field_srv6_egress_usd_info.ipmf1_context_id,
                                                          dnx_data_field.
                                                          preselector.srv6_egress_usd_presel_id_ipmf1_get(unit)));

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
    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
                    (unit, cint_field_srv6_egress_usd_info.ipmf1_context_id,
                     cint_field_srv6_egress_usd_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_egress_usd_presel_id_ipmf3_get(unit)));

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
    /** IPv6 size + offset to Last-entry field in SRH */
    attach_info.key_info.qual_info[0].offset = 320 + 32;

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
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usp_presel_set(unit, cint_field_srv6_egress_usp_info.ipmf1_context_id,
                                                          dnx_data_field.
                                                          preselector.srv6_egress_usp_presel_id_ipmf1_get(unit),
                                                          bcmFieldLayerTypeSrv6Beyond, 0));
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usp_presel_set
                    (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_egress_usp_udp_presel_id_ipmf1_get(unit), bcmFieldLayerTypeUdp,
                     0));

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
    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
                    (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id,
                     cint_field_srv6_egress_usp_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_egress_usp_presel_id_ipmf3_get(unit)));

    /** configure FG in PMF3 - Parsing-Start-Type, Fwd-layer-idx, TCAM Mapping AC_LIF (IN_LIF1) to RCH Entry */
    SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;

}

/*
 * Configure TM profiles per, used in PSP like flows (PSP, USP),
 * where large cut is performed in Egress, much larger than bytes to add.
 *
 * -Since in Egress PSP we cut all Bytes in Egress, we must update TM with size of Bytes we are to cut.
 * -We do this by using PMF to write into network-header-append-size-ptr which is 8bits and usually is
 *  set by 8 MSB bits of the Outlif. These bits are key to TM table which sets per entry the compensated value.
 * -Since we want to use same Outlif for all cases of NOF SIDs, we want to override with PMF the key to represent
 *  the SIDs number - entries 1,2,3,4,5,6 and have the adequate compensation.
 *
 * \param [in] unit - The unit number.
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_tm_compensation_profile_configure(
    int unit)
{
    int sid_compensation_size_entry;
    bcm_cosq_pkt_size_adjust_info_t adjust_info;
    int egress_basic_compensation_size = 8; /** compensation in Bytes without SIDs, just SRH (8B)*/

    int tm_profile_start_range = 0xF << 4;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create TM Profiles per each NOF SIDs case
     * -Using entries 2-6 to represent the number of available SIDs in the packet
     */
    adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;        /** compensation type*/
    adjust_info.flags = 0;
    adjust_info.cosq = 0;
    adjust_info.source_info.source_id = 0;        /** irrelevant */

    for (sid_compensation_size_entry = tm_profile_start_range;
         sid_compensation_size_entry <=
         tm_profile_start_range + dnx_data_srv6.termination.max_nof_terminated_sids_psp_get(unit);
         sid_compensation_size_entry++)
    {
        /*
         * -In 1-Cycle PSP/USP, compensation size is SRH + NOF_SIDS * 16B,
         * -in extended PSP/USP, compensation size is Constant NOF_SIDs * 16B, stored in TM profile, idx '0'
         * - setting as negative value for compensation
         */

        int compensation_size;

        if (sid_compensation_size_entry > tm_profile_start_range)
        {
            compensation_size =
                0 - (egress_basic_compensation_size + (sid_compensation_size_entry - tm_profile_start_range) * 16);
        }
        else
        {
            /** in location '0', store compensation for an extended termination flow */
            compensation_size = 0 - EXTENDED_TERMINATION_NOF_SIDS_PER_CYCLE * 16;
        }

        BCM_GPORT_PROFILE_SET(adjust_info.gport, sid_compensation_size_entry);

        /*
         * updating egress compensation size to include each time the SID numbers x 16B
         * the TM compensation field is limited to 127, so not passing it
         */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info,
                                                           (compensation_size < -127 ? -127 : compensation_size)));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Endpoint and PSP handling
 * -Contains mutual PMF2 configuration for Endpoint and PSP, and mutual PMF3 configuration.
 * -PMF2:
 *      Endpoint + PSP Mutual:
 *      -Both Endpoint and PSP do next DIP UDH Buildup.
 *
 *      For PSP: PSP may be activated by Global configuration or by next DIP lookup in PMF2 TCAM.
 *           PMF2 passes to PMF3 container on which :
 *              -8b NOF SIDs (for TM compensation)
 *              -1b Valid bit to choose PMF3 between Endpoint or PSP actions.
 *
 *          When PSP global configuration is set:
 *              -1b Valid passes the SRv6 layer qualifier of SL==1 (and when set PMF3 would do PSP)
 *
 *          When PSP global configuration not set:
 *              -PMF2 would also contain TCAM FG with entries:
 *                  -x2 qualifiers: (SRv6 layer qualifier SL==1) + (DIP is PSP)
 *                  -result 1b if current packet hits above qualifiers
 *              -1b Valid bit is according to above TCAM hit (so that PMF3 would do PSP)
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_endpoint_and_psp(
    int unit,
    int appl_psp_enable)
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

    /*
     * Endpoint + 1Pass PSP Context + Preselector
     */

     /** Egress PSP - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_psp_udh_pmf1", 0,
                     &cint_field_srv6_endpoint_psp_info.ipmf1_context_id));

     /** Egress PSP - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_endpoint_psp_presel_set
                    (unit, cint_field_srv6_endpoint_psp_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_presel_id_ipmf1_get(unit), 0));

    /*
     * Endpoint + Extended PSP Context + Preselector
     */
    cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id = 38;

     /** Endpoint + PSP Ext - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_ep_psp_ext_pmf1", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id));

     /** Endpoint + PSP Ext - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_endpoint_psp_presel_set
                    (unit, cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf1_get(unit), 1));

    /*
     * Mutual Contexts Function - which attaches all FGs to both Endpoint+PSP/Endpoint + PSP Extended contexts - both
     * contexts do the same, with same FGs (same TCAM), just called different contexts, with different presels 
     */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_psp_ipmf2_udh(unit, appl_psp_enable));

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
    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
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
appl_ref_srv6_endpoint_usp_extended_cut(
    int unit)
{
    /*
     * Case of Endpoint PSP, with more than 4 SIDs, so need to do and extended pass just for cutting 4SIDs
     */
    SHR_FUNC_INIT_VARS(unit);

    cint_field_srv6_endpoint_usp_ext_cut_info.ipmf1_context_id = 37;
     /** Egress PSP - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_psp_ext_cut_pmf1", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_endpoint_usp_ext_cut_info.ipmf1_context_id));

     /** Egress USP - create the Presel and attach it to the above context */
    /** create the Presel and attach it to the above context - Presel in higher priority then USD */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usp_presel_set
                    (unit, cint_field_srv6_endpoint_usp_ext_cut_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_usp_extended_cut_presel_id_ipmf1_get(unit),
                     bcmFieldLayerTypeSrv6Beyond, 1));
    SHR_IF_ERR_EXIT(field_srv6_pmf1_egress_usp_presel_set
                    (unit, cint_field_srv6_endpoint_usp_ext_cut_info.ipmf1_context_id,
                     dnx_data_field.preselector.srv6_endpoint_usp_udp_extended_cut_presel_id_ipmf1_get(unit),
                     bcmFieldLayerTypeUdp, 1));

    cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_context_id = 37;
    /** Egress - create the context in PMF3 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_usp_ext_cut_pmf3", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_context_id));

    /** Egress -  attach PSP Ext PMF3 context to PMF1 USP EXT context */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
                    (unit, cint_field_srv6_endpoint_usp_ext_cut_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_usp_ext_cut_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_usp_extended_cut_presel_id_ipmf3_get(unit)));

    /** Egress - Configure FG in PMF3 */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_usp_extended_cut_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Endpoint PSP handling
 * Relevant field configuration already set by extended USP, now just need to update PREsel
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static shr_error_e
appl_ref_srv6_endpoint_psp_extended_cut(
    int unit,
    int appl_psp_enable)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * PMF3 Context similar to Endpoint and Regular PSP, but different function
     */

    cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id = 38;
    /** Egress - create the context in PMF3 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_psp_ext_cut_pmf3", BCM_FIELD_FLAG_WITH_ID,
                     &cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id));

    SHR_IF_ERR_EXIT(field_srv6_pmf3_by_pmf1_context_presel_set
                    (unit, cint_field_srv6_endpoint_psp_ext_cut_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_psp_ext_cut_info.ipmf3_context_id,
                     dnx_data_field.preselector.srv6_endpoint_psp_extended_cut_presel_id_ipmf3_get(unit)));

    /** Mutual Context to Endpoint and PSP Extended Configure FG in PMF3 */
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

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_egress_estimated_bta_btr_config))
    {
        bcm_field_context_t context_id;
        SHR_IF_ERR_EXIT(appl_dnx_field_srv6_beyond_minus_fwd_layer_ipmf1_context_set(unit, &context_id));

        /** estimate Btr for srv6 extended termination - 1st round*/
        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_egress_estimated_bta_btr_config))
        {
            SHR_IF_ERR_EXIT(appl_ref_srv6_extended_termination_btr(unit, -1, context_id));
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
     * To allow Maximum SIDs termination for L2VPN Egress nodes, need to enable IVEC null feature
     */
    SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchIngressVlanEditClassNull, 1));

    /*
     * Configure TM compensation profiles for PSP like flows with large bytes to remove
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_tm_compensation_profile_configure(unit));

    /*
     * Classic End point and PSP handling
     *  -Defining PMF1 FGs to both Endpoint-PSP and Endpoint-PSP Extended contexts
     *  -PMF3 only for Endpoint-PSP
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_and_psp(unit, appl_psp_enable));

    /*
     * Classic EndPoint-PSP Extended handling
     * -Only PMF3, as PMF1 was defined in Endpoint-PSP function
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_psp_extended_cut(unit, appl_psp_enable));

    /*
     * uSID End point handling - handle the uSID middle case, when SL is not decreased
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_usid(unit));

    /*
     * GSID End point handling - handle the GSID middle case, when SL is not decreased
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_gsid(unit));

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
     * USP EXT cut handling
     * -specific USP Ext PMF1-2 void context, just be used as qualifier for PMF3 PSP Ext context
     * -do same PMF3 PSP context
     */
    SHR_IF_ERR_EXIT(appl_ref_srv6_endpoint_usp_extended_cut(unit));

    /** EPMF - update BTA for PSP (PSP mode is always on) and relevant for USP as well  */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_psp_epmf_bta(unit));

exit:
    SHR_FUNC_EXIT;
}
