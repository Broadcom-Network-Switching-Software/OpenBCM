/** \file appl_ref_nat_init.c
 * 
 *
 * Application for NAT
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include <soc/schanmsg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "appl_ref_field_nat_init.h"
#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm/field.h>
#include <bcm/switch.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/*
 * }
 */

/*
 * DEFINEs
 */

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

static shr_error_e
appl_dnx_field_nat_presel_set(
    int unit,
    uint32 acl_ctx_profile)
{
    uint32 entry_handle_id = 0;
    uint32 context_hw_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * NAT upstream
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_FWD2_CONTEXT_ID, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM,
                     &context_hw_value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_hw_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, acl_ctx_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * NAT downstream
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_FWD2_CONTEXT_ID, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM,
                     &context_hw_value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_hw_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, acl_ctx_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Tunneled NAT upstream
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_FWD2_CONTEXT_ID, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___TUNNELED_SRC_NAT,
                     &context_hw_value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_hw_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, acl_ctx_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Tunneled NAT downstream
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                    (unit, DBAL_FIELD_FWD2_CONTEXT_ID, DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___TUNNELED_DST_NAT,
                     &context_hw_value));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_hw_value);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, acl_ctx_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_epmf_tunneled_nat_cfg(
    int unit,
    uint32 qual_value)
{
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_group_info_t epmf_fg_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    bcm_field_action_info_t action_info, action_info_get;
    uint32 ace_entry_handle;
    bcm_field_action_t action_ace_entry_id;
    bcm_field_group_t epmf_fg_id;
    bcm_field_group_attach_info_t epmf_attach_info;
    bcm_field_context_t epmf_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    /** overide IOAMP_JP value*/
    uint32 val_in_field = DBAL_ENUM_FVAL_ETPP_TERM_ACE_CTXT_VALUE_IOAM_JP_OR_NAT;
    uint32 ace_context_value = 0;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "Tunneled_NAT", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &epmf_context_id));
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_nat_presel_id_epmf_get(unit);
    presel_entry_id.stage = bcmFieldStageEgress;
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 5;
    presel_entry_data.context_id = epmf_context_id;
    presel_entry_data.entry_valid = TRUE;
    /** Check that the bit in the port profile is set - 0xAA and fwd_layer_type is IPv4*/
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyUDHBase0;
    presel_entry_data.qual_data[0].qual_value = 2;
    presel_entry_data.qual_data[0].qual_mask = 0xFF;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyUDHBase1;
    presel_entry_data.qual_data[1].qual_value = 2;
    presel_entry_data.qual_data[1].qual_mask = 0xFF;
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyUDHBase2;
    presel_entry_data.qual_data[2].qual_value = 2;
    presel_entry_data.qual_data[2].qual_mask = 0xFF;
    presel_entry_data.qual_data[3].qual_type = bcmFieldQualifyUDHBase3;
    presel_entry_data.qual_data[3].qual_value = 2;
    presel_entry_data.qual_data[3].qual_mask = 0xFF;
    presel_entry_data.qual_data[4].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[4].qual_arg = 0;
    presel_entry_data.qual_data[4].qual_value = bcmFieldLayerTypeIp4;
    presel_entry_data.qual_data[4].qual_mask = 0x1F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*
     * Configure the ACE format.
     */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValueRaw;
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id));

    SHR_IF_ERR_EXIT(dbal_fields_field_type_struct_field_encode
                    (unit, DBAL_FIELD_TYPE_DEF_ACE_CONTEXT_VALUE, DBAL_FIELD_ETPP_TERM_ACE_CTXT_VALUE, &val_in_field,
                     &ace_context_value));

    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
    ace_entry_info.entry_action[0].type = ace_format_info.action_types[0];
    ace_entry_info.entry_action[0].value[0] = ace_context_value;
    SHR_IF_ERR_EXIT(bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle));

     /**
      * Fill action info
      */
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, bcmFieldActionAceEntryId, bcmFieldStageEgress, &action_info_get));
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionAceEntryId;
    sal_strncpy_s((char *) (action_info.name), "uda_ace_entry", sizeof(action_info.name));
    action_info.stage = bcmFieldStageEgress;
    action_info.size = 0;
    action_info.prefix_size = action_info_get.size;
    action_info.prefix_value = ace_entry_handle;
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_ace_entry_id));

    /*
     * Create a field group in ePMF that uses the ACE entry.
     */
    bcm_field_group_info_t_init(&epmf_fg_info);
    epmf_fg_info.fg_type = bcmFieldGroupTypeConst;
    epmf_fg_info.nof_quals = 0;
    epmf_fg_info.stage = bcmFieldStageEgress;
    epmf_fg_info.nof_actions = 1;
    /*
     * The first action decides the ACE entry that will happen. 
     */
    epmf_fg_info.action_types[0] = action_ace_entry_id;
    epmf_fg_info.action_with_valid_bit[0] = FALSE;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &epmf_fg_info, &epmf_fg_id));

    bcm_field_group_attach_info_t_init(&epmf_attach_info);
    epmf_attach_info.key_info.nof_quals = epmf_fg_info.nof_quals;
    epmf_attach_info.payload_info.nof_actions = epmf_fg_info.nof_actions;
    for (ii = 0; ii < epmf_fg_info.nof_quals; ii++)
    {
        epmf_attach_info.key_info.qual_types[ii] = epmf_fg_info.qual_types[ii];
    }
    for (ii = 0; ii < epmf_fg_info.nof_actions; ii++)
    {
        epmf_attach_info.payload_info.action_types[ii] = epmf_fg_info.action_types[ii];
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, epmf_fg_id, epmf_context_id, &epmf_attach_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_ipmf_nat_cfg(
    int unit,
    uint32 qual_value)
{
    bcm_field_context_t context_id = dnx_data_field.context.default_nat_context_get(unit);
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_qualify_t nat_base_udq;
    bcm_field_group_t fg_id;
    int gen_data_offset;
    int qual_idx = 0;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create a context and use the bit we set in the port profile as a qualifier for its preselector.
     */
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "NAT", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id));
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_nat_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    /** Check that the bit in the port profile is set */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyAppTypeRaw;
    presel_entry_data.qual_data[0].qual_value = qual_value;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*
     * Create the field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 1 /* srcNAT */  + 1 /* dstNAT */  + 1 /* udh_Base */ ;
    gen_data_offset =
        dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_QUAL_GENERAL_DATA_0)->offset;

    /*
     * src NAT info is on contaier1 bits 48-95
     * dst NAT info is on contaier1 bits 0-47
     */
    {
        bcm_field_qualify_t nat_src_udq;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 48;
        sal_snprintf((char *) qual_info.name, sizeof(qual_info.name), "%s", "NAT_SRC_Q");
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &nat_src_udq));

        fg_info.qual_types[qual_idx] = nat_src_udq;
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[qual_idx].offset = gen_data_offset + 160 /* container0 */  + 48  /* src NAT
                                                                                                         * offset */ ;
        qual_idx++;
    }

    {
        bcm_field_qualify_t nat_dst_udq;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 48;
        sal_snprintf((char *) qual_info.name, sizeof(qual_info.name), "%s", "NAT_DST_Q");
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &nat_dst_udq));

        fg_info.qual_types[qual_idx] = nat_dst_udq;
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
        attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[qual_idx].offset = gen_data_offset + 160 /* container0 */  + 0   /* dst NAT
                                                                                                         * offset */ ;
        qual_idx++;
    }

    /*
     * Configure UDH base creation
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) qual_info.name, "NAT_BASE_Q", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &nat_base_udq));

    fg_info.qual_types[qual_idx] = nat_base_udq;
    attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
    attach_info.key_info.qual_info[qual_idx].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[qual_idx].input_arg = 0xAA;

    /*
     *
     */
    fg_info.nof_actions = 7;
    fg_info.action_types[0] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[0] = FALSE;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    fg_info.action_types[1] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[1] = FALSE;
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    fg_info.action_types[2] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[2] = FALSE;
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    fg_info.action_types[3] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[3] = FALSE;
    attach_info.payload_info.action_types[3] = fg_info.action_types[3];
    fg_info.action_types[4] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[4] = FALSE;
    attach_info.payload_info.action_types[4] = fg_info.action_types[4];
    fg_info.action_types[5] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[5] = FALSE;
    attach_info.payload_info.action_types[5] = fg_info.action_types[5];
    fg_info.action_types[6] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[6] = FALSE;
    attach_info.payload_info.action_types[6] = fg_info.action_types[6];

    sal_strncpy_s((char *) fg_info.name, "NAT_UDH_LOAD", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function Configures a context and field group for NAT header application
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_field_nat_init(
    int unit)
{
    uint32 qual_value = 60;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_ipmf_nat_cfg(unit, qual_value));

    SHR_IF_ERR_EXIT(appl_dnx_field_nat_presel_set(unit, qual_value));

    /*
     * Configure ACE_value for the tunneled NAT application 
     */
    SHR_IF_ERR_EXIT(appl_dnx_epmf_tunneled_nat_cfg(unit, qual_value));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
