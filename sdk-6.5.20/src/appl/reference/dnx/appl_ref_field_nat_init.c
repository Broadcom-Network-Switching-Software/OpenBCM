/** \file appl_ref_nat_init.c
 * 
 *
 * Application for NAT
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

 /*
  * Include files.
  * {
  */
#include <soc/schanmsg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
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
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * NAT upstream
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_UPSTREAM);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, acl_ctx_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * NAT downstream
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID,
                               DBAL_ENUM_FVAL_FWD2_CONTEXT_ID_IPV4___NAT_DOWNSTREAM);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, DBAL_ENUM_FVAL_FIELD_PMF_STAGE_IPMF1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_CONTEXT_PROFILE, INST_SINGLE, acl_ctx_profile);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
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
    presel_entry_data.qual_data[0].qual_value = 60;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    SHR_IF_ERR_EXIT(appl_dnx_field_nat_presel_set(unit, presel_entry_data.qual_data[0].qual_value));

    /*
     * Create the field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.nof_quals = 1 /* srcNAT */  + 1 /* dstNAT */  + 1 /* udh_Base */ ;
    gen_data_offset =
        dnx_data_field.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_IPMF1_QUAL_GENERAL_DATA_0)->offset;

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

/*
 * }
 */
