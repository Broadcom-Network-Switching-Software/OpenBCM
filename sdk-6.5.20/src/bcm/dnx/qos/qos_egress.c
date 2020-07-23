/** \file qos_egress.c
 *
 *  Qos REMARKING procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include "qos_internal.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>
#include <bcm_int/dnx/lif/out_lif_profile.h>

/*
 * }
 */

/** ECN field in IPv4.TOS/IPv6.TC*/
#define DNX_QOS_ECN_CNI_OFFSET 0x1

#define FWD_PLUS_1_REMARK_TYPE_EXPLICIT        DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_EXPLICIT
#define FWD_PLUS_1_REMARK_TYPE_REMARK_ALL      DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_ALL
#define FWD_PLUS_1_REMARK_TYPE_REMARK_NON_ECN  DBAL_ENUM_FVAL_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_REMARK_TYPE_INHERIT_REMARK_NON_ECN
#define FWD_PLUS_1_REMARK_TYPE_NOF_TYPES       DBAL_NOF_ENUM_QOS_FORWARD_P1_EXPLICIT_REMARK_PROFILE_VALUES

#define FWD_PLUS_1_REMARK_INGRESS_DISABLE      DBAL_ENUM_FVAL_QOS_FORWARD_P1_REMARK_PROFILE_INGRESS_REMARK_PROFILE_0

typedef struct
{
    /*
     * Key fields - Forwarding layer additional info
     */
    uint32 forwarding_layer_additional_info;
    /*
     * Table result fields
     */
    uint8 ttl_dec_disable;
} etpcc_ttl_decrement_config_t;

 /*
  * Initialization table for dnx_qos_egress_ttl_decrement_init, which configures EGRESS_TTL_DECREMENT_CONFIGURATION Table.
  * In case lsb is set the TTL is pipe.
  */
static const etpcc_ttl_decrement_config_t jr2_ttl_decrement_config_map[] = {
   /** Forwarding Layer Additional Info,     TTL Dec Disable   */
    {0, 0},
    {1, 0},
    {2, 0},
    {3, 0},
    {4, 1},
    {5, 1},
    {6, 1},
    {7, 1},
    {8, 0},
    {9, 0},
    {10, 0},
    {11, 0},
    {12, 1},
    {13, 1},
    {14, 1},
    {15, 1},

};

/*
 * Initiate the EGRESS_TTL_DECREMENT_CONFIGURATION Table,It's per Fwd-code configuration to disable the TTL
 * TTL decrentment.
 * By default, IP and MPLS service should not set this flag.
 */
static const etpcc_ttl_decrement_config_t jr_ttl_decrement_config_map[] = {
   /** Forwarding Layer Additional Info,     TTL Dec Disable   */
    {0, 0},     /* Bridge */
    {1, 0},     /* IPv4 UC */
    {2, 0},     /* IPv4 MC */
    {3, 0},     /* IPv6 UC */
    {4, 0},     /* IPv6 MC */
    {5, 0},     /* MPLS */
    {6, 1},
    {7, 1},
    {8, 1},
    {9, 1},
    {10, 1},
    {11, 1},
    {12, 1},
    {13, 1},
    {14, 1},
    {15, 1},

};

/* *INDENT-OFF* */

CONST qos_model_mapping_table_t qos_model_mapping_table_init[DBAL_NOF_ENUM_ENCAP_QOS_MODEL_VALUES] = {
 /*****************************************************************************************************************************
  *      DP Select type             *      EGRESS_IS_ECN_ELIGIABLE  *  DP PIPE       *  VAR_PIPE   *FWD_PLUS_1_EXPLICIT_PROFILE
 */
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_UNIFORM, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O1,
     FWD_PLUS_1_REMARK_TYPE_REMARK_NON_ECN},
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O1,
     FWD_PLUS_1_REMARK_TYPE_EXPLICIT},
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O2,
     FWD_PLUS_1_REMARK_TYPE_EXPLICIT},
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_INITIAL, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_NOT_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O1,
     FWD_PLUS_1_REMARK_TYPE_REMARK_NON_ECN},
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_UNIFORM, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O1,
     FWD_PLUS_1_REMARK_TYPE_REMARK_ALL},
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_PIPE_DP, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O1,
     FWD_PLUS_1_REMARK_TYPE_EXPLICIT},
    {DBAL_ENUM_FVAL_QOS_DP_SELECT_INITIAL, DBAL_ENUM_FVAL_ECN_IS_ELIGIBLE_ELIGIBLE, QOS_DP_PIPE_O1, NETWORK_QOS_PIPE_O1,
     FWD_PLUS_1_REMARK_TYPE_REMARK_NON_ECN}
};
/* *INDENT-ON* */

/**
 * \brief
 * Initialize DBAL Table:
 *  DBAL_TABLE_EGRESS_QOS_MODEL_MAPPING_TABLE. for each
 * QOS_MODEL an entry is set with all needed parameters.
 * use qos_model_mapping_table_init for input values for table
 * init
 * Write to QOS Hardware: ETPPB_QOS_MODEL_MAPPING_TABLE
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_remark_qos_model_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_encap_qos_model_e qos_model_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MODEL_MAPPING_TABLE, &entry_handle_id));

    for (qos_model_index = 0; qos_model_index < DBAL_NOF_ENUM_ENCAP_QOS_MODEL_VALUES; qos_model_index++)
    {
        CONST qos_model_mapping_table_t *qos_model_map_init_ptr = &qos_model_mapping_table_init[qos_model_index];

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENCAP_QOS_MODEL, qos_model_index);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_DP_SELECT, INST_SINGLE,
                                     qos_model_map_init_ptr->dp_select);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_IS_ELIGIBLE, INST_SINGLE,
                                     qos_model_map_init_ptr->is_ecn_eligiable);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_IS_PRESERVE, INST_SINGLE,
                                     qos_model_map_init_ptr->dp_is_preserve);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_IS_PRESERVE, INST_SINGLE,
                                     qos_model_map_init_ptr->nwk_qos_is_preserve);

        /** For forwarding plus one header remarking */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_EXPLICIT_PROFILE, INST_SINGLE,
                                     qos_model_map_init_ptr->fwd_plus_1_explicit_profile);

        /** For PHP */
        if (qos_model_index == DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_MODEL_TYPE, INST_SINGLE,
                                         DBAL_ENUM_FVAL_MODEL_TYPE_PIPE);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     inint mapping to convert JR1 FHEI.model (1bit) to JR2 FHEI model (2bits)
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_fhei_model_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    int model_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_JR1_FHEI_MODEL_MAP, &entry_handle_id));

    for (model_type = DBAL_ENUM_FVAL_MODEL_TYPE_UNIFORM; model_type < DBAL_NOF_ENUM_MODEL_TYPE_VALUES; model_type++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_MODEL, model_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_QOS_MODEL, INST_SINGLE, model_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FHEI_TTL_MODEL, INST_SINGLE, model_type);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table: QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE.
 * set the appropriate qos var type for each forwarding code and
 * ecn eligiable combination.
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_fwd_plus_1_remark_table_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_qos_lr_forward_p1_type_index_e layer_protocol_index;
    dbal_enum_value_field_qos_forward_p1_remark_profile_e remark_ingress_profile;
    dbal_enum_value_field_qos_forward_p1_explicit_remark_profile_e remark_explicit_profile;
    dbal_enum_value_field_ftmh_ecn_eligible_e ecn_enable;

    dbal_enum_value_field_qos_typefwdplus1_type_e qos_map_type;
    dbal_enum_value_field_remark_or_preserve_e remark_or_keep;
    uint32 is_explicit;
    uint32 ecn_is_eligible;
    uint32 nwk_qos_remark_bitmap;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_FWD_PLUS_1_REMAKR_TABLE, &entry_handle_id));

    for (layer_protocol_index = 0; layer_protocol_index < DBAL_NOF_ENUM_QOS_LR_FORWARD_P1_TYPE_INDEX_VALUES;
         layer_protocol_index++)
    {
        for (remark_ingress_profile = 0; remark_ingress_profile < DBAL_NOF_ENUM_QOS_FORWARD_P1_REMARK_PROFILE_VALUES;
             remark_ingress_profile++)
        {
            for (remark_explicit_profile = 0; remark_explicit_profile < FWD_PLUS_1_REMARK_TYPE_NOF_TYPES;
                 remark_explicit_profile++)
            {
                for (ecn_enable = 0; ecn_enable < DBAL_NOF_ENUM_FTMH_ECN_ELIGIBLE_VALUES; ecn_enable++)
                {
                    /** Decide ECN eligible for fwd plus one header.*/
                    if ((ecn_enable == DBAL_ENUM_FVAL_FTMH_ECN_ELIGIBLE_ENABLE) &&
                        (remark_explicit_profile == FWD_PLUS_1_REMARK_TYPE_REMARK_ALL)
                        && ((layer_protocol_index == DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV4)
                            || (layer_protocol_index == DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV6)))
                    {
                        ecn_is_eligible = TRUE;
                    }
                    else
                    {
                        ecn_is_eligible = FALSE;
                    }

                    /** Select qos map type for fwd plus one header. */
                    /** Decide NWK QOS updates for fwd plus one header. Currently only DSCP/TC is updated.*/
                    if (layer_protocol_index == DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV4)
                    {
                        qos_map_type = DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV4;

                        /** nwk_qos_remark_bitmap[i] == 1 means do not update the bit.*/
                        nwk_qos_remark_bitmap = (ecn_is_eligible ? 0 : DNX_QOS_ECN_MASK);
                    }
                    else if (layer_protocol_index == DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV6)
                    {
                        qos_map_type = DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV6;

                        /** Update the entire TC field for IPv6 currently.*/
                        nwk_qos_remark_bitmap = 0;
                    }
                    else
                    {
                        qos_map_type = DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_OTHER;

                        /** Do not update any bits.*/
                        nwk_qos_remark_bitmap = DNX_QOS_TOS_MASK;
                    }

                    /** Select remark_or_keep for fwd plus one header. */

                    /*
                     * FIXME SDK-176952, fwd plus one remark is not enabled in FAI.
                     * Currently we suppose it is always enabled in FAI now.
                     *
                     if (remark_ingress_profile == FWD_PLUS_1_REMARK_INGRESS_DISABLE)
                     {
                     remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP;
                     }
                     else
                     {
                     remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;
                     }
 */
                    remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;

                    /** Decide explict or inherit for fwd plus one header.*/
                    if (remark_explicit_profile == FWD_PLUS_1_REMARK_TYPE_EXPLICIT)
                    {
                        is_explicit = TRUE;
                    }
                    else
                    {
                        is_explicit = FALSE;
                    }

                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_LAYER_PROTOCOL_INDEX,
                                               layer_protocol_index);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_REMARK_INGRESS_PROFILE,
                                               remark_ingress_profile);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_ECN_ELIGIBLE, ecn_enable);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_EXPLICIT_PROFILE,
                                               remark_explicit_profile);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_QOS_VAR_TYPE, INST_SINGLE,
                                                 qos_map_type);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                                 remark_or_keep);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_IS_ELIGIBLE, INST_SINGLE,
                                                 ecn_is_eligible);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_REMARK_EXPLICIT,
                                                 INST_SINGLE, is_explicit);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_NWK_QOS_REMARK_BMP,
                                                 INST_SINGLE, nwk_qos_remark_bitmap);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Init the mapping table from forward code remark enabler and qos profile
 *  remark enabler to the final enabler for forward header qos remarking.
 *
 * \param [in] unit - Relevant unit.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \remark
 *  Call it in case qos var type map is enabled.
 */
static shr_error_e
dnx_qos_egress_qos_var_type_map_init(
    int unit)
{
    uint32 qos_var_type;
    uint32 new_qos_var_type;
    int ecn_eligible;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ENC_QOS_VAR_TYPE_MAPPING_TABLE, &entry_handle_id));

    for (qos_var_type = 0; qos_var_type < DBAL_NOF_ENUM_ETPP_QOS_VAR_TYPE_VALUES; qos_var_type++)
    {
        for (ecn_eligible = FALSE; ecn_eligible <= TRUE; ecn_eligible++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_VAR_TYPE_CURRENT, qos_var_type);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_IS_ECN_ELIGIBLE, ecn_eligible);
            new_qos_var_type = qos_var_type;
            /**in case ECN eligible and IPV4/6, upadte new qos var type */
            if (ecn_eligible)
            {
                switch (qos_var_type)
                {
                    case DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS:
                        new_qos_var_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS_ECN;
                        break;
                    case DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC:
                        new_qos_var_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC_ECN;
                        break;
                    default:
                        break;
                }
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_QOS_VAR_TYPE_NEW, INST_SINGLE,
                                         new_qos_var_type);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Init the mapping table from qos var type and layer is ECN eligible
 *  to the new qos var type.
 *
 * \param [in] unit - Relevant unit.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \remark
 *  Call it in case of the egress forward header remarking enabler is mapped,
 *  such as in Jerico2B(In Jerico2A, it was a calculated result.).
 *  By defult, the remarking is enabled when one of forward-code-enabler and
 *  qos-profile-enabler is set.
 */
static shr_error_e
dnx_qos_egress_forward_header_remark_enabler_init(
    int unit)
{
    uint32 entry_handle_id;
    int dscp_preserve_mode;
    int system_headers_mode;
    int egress_inlif_profile_start_bit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dscp_preserve_mode = dnx_data_qos.qos.routing_dscp_preserve_mode_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /*
     * If dscp preserve is done per inlif & outlif
     *    Init the final remark enabler = fwd_code_remark | outlif_remark.
     * else
     *    Init the final remark enabler = fwd_code_remark & outlif_remark.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_REMARK_OR_PRESERVE,
                               DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE_REMARK_OR_PRESERVE,
                               DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dscp_preserve_mode == ROUTING_DSCP_PRESERVE_MODE_INLIF_OUTLIF)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_REMARK_OR_PRESERVE,
                                   DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE_REMARK_OR_PRESERVE,
                                   DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_FWD_HEADER_REMARK_ENABLER, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE_REMARK_OR_PRESERVE,
                                   DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_PROFILE_REMARK_OR_PRESERVE,
                                   DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            if (dnx_data_lif.feature.feature_get(unit, dnx_data_lif_feature_iop_mode_orientation_selection))
            {
                egress_inlif_profile_start_bit =
                    IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_JR_MODE;
            }
            else
            {
                egress_inlif_profile_start_bit =
                    IN_LIF_PROFILE_PMF_RESERVED_START_BIT - IN_LIF_PROFILE_EGRESS_NOF_BITS_DSCP_MODE;
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_QOS, entry_handle_id));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTING_DSCP_PRESERVE_INLIF_OUTLIF_MODE,
                                         INST_SINGLE, TRUE);
            /** in-lif-profile bit4:5 is for egress usage.*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROUTING_DSCP_PRESERVE_INLIF_ENABLE_OFFSET,
                                         INST_SINGLE, egress_inlif_profile_start_bit);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Initialize DBAL Table: ETPPA_CFG_INGRESS_NWK_QOS_TYPE
 * & DBAL_TABLE_QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING
 * When working with Jericho1 devices there is a need to handle with the ingress_nwq_qos,
 * and there are cases that this parameter is not passed to the egress.
 * the first table maps fwd_code to nwk_qos type,
 * if nwk_qos type is IPv4 uc/mc, or IPv6 uc/mc, or mpls,
 * and there is in-dscp-exp in system header from ingress,
 * The second table supply mapping JER 1 ingress in-dscp-exp to nwq_qos.
 * Write to QOS Hardware:
 * ETPPA_JERICHO_INGRESS_NWK_QOS_TABLE
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_remark_jer_ingress_nwk_qos_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 nwk_qos_type;
    uint32 in_dscp_exp;
    uint32 fwd_code;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** map fwd-code to nwk_qos_type*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_INGRESS_NWK_QOS_TYPE_SELECT, &entry_handle_id));

    for (fwd_code = 0; fwd_code < DBAL_NOF_ENUM_EGRESS_FWD_CODE_VALUES; fwd_code++)
    {
        switch (fwd_code)
        {
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0:
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1:
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0:
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1:
                nwk_qos_type = DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV4;
                break;
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0:
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1:
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0:
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1:
                nwk_qos_type = DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV6;
                break;
            case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS:
                nwk_qos_type = DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_MPLS;
                break;
            default:
                nwk_qos_type = DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER;
                break;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, fwd_code);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_NWK_QOS_TYPE, INST_SINGLE, nwk_qos_type);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** map in-dscp-exp to nwk_qos if nwk_qos_type is IPv4 or IPv6 or MPLS*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_JER_INGRESS_NWK_QOS_MAPPING, entry_handle_id));
    for (nwk_qos_type = 0; nwk_qos_type < DBAL_NOF_ENUM_INGRESS_NWK_QOS_TYPE_VALUES; nwk_qos_type++)
    {
        switch (nwk_qos_type)
        {
            case DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV4:
            case DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_IPV6:
            case DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_MPLS:
                for (in_dscp_exp = 0; in_dscp_exp <= DNX_DATA_MAX_QOS_QOS_PACKET_MAX_DSCP; in_dscp_exp++)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_NWK_QOS_TYPE, nwk_qos_type);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_DSCP_EXP, in_dscp_exp);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, INST_SINGLE,
                                                 in_dscp_exp);

                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
                break;
            case DBAL_ENUM_FVAL_INGRESS_NWK_QOS_TYPE_OTHER:
            default:
                break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table: DBAL_TABLE_QOS_EGRESS_TC_NWK_QOS_MAPPING
 * When working with Jericho1 devices, there is  a need to take a look with the ingress_nwq_qos,
 * and there are cases that this parameter is not passed to the egress.
 * when packet forward header is L2,
 * This table supply map TC to nwq_qos.
 * Write to QOS Hardware:
 * ETPPA_CFG_TC_2_NWK_QOS_MAPPING
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_remark_tc_nwk_qos_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 ingress_tc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** initial mapping tc to network-qos*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_TC_NWK_QOS_MAPPING, &entry_handle_id));

    for (ingress_tc = 0; ingress_tc <= DNX_DATA_MAX_QOS_QOS_PACKET_MAX_TC; ingress_tc++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TC, ingress_tc);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_NWK_QOS, INST_SINGLE, ingress_tc);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table: DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT
 * When working only with Jericho 2 devices this table only
 * takes the ingress_nwq_qos. When working with Jericho1 devices
 * there is a need to look with the ingress_nwq_qos, and there are cases
 * that this parameter is not passed to the egress.
 * This table should supply a solution for those cases.
 * if ingress-nwd-qos is valid, select INGRESS_NETWORK_QOS
 * else if pakcet forward type is IPv4/IPv6/MPLS (ingress-nwd-qos is invalid)
 * then select FORWARD_NETWORK_QOS
 * else if packet forward tyep is L2  (ingress-nwd-qos is invalid)
 * then select TC_NETWORK_QOS
 * Write to QOS Hardware:
 * ETPPA_INITIAL_NWK_QOS_SELECT,
 * ETPPA_INITIAL_NWK_QOS_SELECT_PLUS_1
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_remark_nwk_qos_select_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 protocol;
    uint32 layer_protocol;
    uint32 fwd_type;
    uint32 fwd_plus1_type;
    uint32 fwd_plus1_layer_protocol_index;
    int max_fwd_type;
    int max_fwd_plus1_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** map layer type to forward type*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_FWD_LAYER_TYPE_SELECT, &entry_handle_id));

    for (protocol = 0; protocol < DBAL_NOF_ENUM_LAYER_TYPES_VALUES; protocol++)
    {
        switch (protocol)
        {
            case DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET:
                fwd_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI;
                layer_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_ETH;
                fwd_plus1_layer_protocol_index = DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_OTHER;
                break;
            case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
                fwd_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS;
                layer_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV4;
                fwd_plus1_layer_protocol_index = DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV4;
                break;
            case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
                fwd_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC;
                layer_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV6;
                fwd_plus1_layer_protocol_index = DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_IPV6;
                break;
            case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
                fwd_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC;
                layer_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_MPLS;
                fwd_plus1_layer_protocol_index = DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_OTHER;
                break;
            default:
                fwd_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI;
                layer_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_INIT;
                fwd_plus1_layer_protocol_index = DBAL_ENUM_FVAL_QOS_LR_FORWARD_P1_TYPE_INDEX_OTHER;
                break;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_PROTOCOL, protocol);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_TYPE, INST_SINGLE, fwd_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_LAYER_PROTOCOL, INST_SINGLE, layer_protocol);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_LAYER_PROTOCOL_INDEX, INST_SINGLE,
                                     fwd_plus1_layer_protocol_index);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT, entry_handle_id));

    /*
     * when ingress network qos is valid, then both fwd_select and fwd_plus1_select are ingress network qos
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_NETWORK_VALID, TRUE);
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT,
                                                    DBAL_FIELD_FORWARD_QOS_MAP_TYPE, TRUE, 0, 0, &max_fwd_type));
    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_INITIAL_SELECT,
                                                    DBAL_FIELD_FORWARD_QOS_MAP_TYPE_PLUS1, TRUE, 0, 0,
                                                    &max_fwd_plus1_type));

    for (fwd_type = 0; fwd_type < max_fwd_type; fwd_type++)
    {
        for (fwd_plus1_type = 0; fwd_plus1_type < max_fwd_plus1_type; fwd_plus1_type++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MAP_TYPE_PLUS1, fwd_plus1_type);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MAP_TYPE, fwd_type);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT, INST_SINGLE,
                                         DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_INGRESS_NETWORK_QOS);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT_PLUS_1,
                                         INST_SINGLE, DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_INGRESS_NETWORK_QOS);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /*
     * when ingress network qos is valid, then both fwd_select and fwd_plus1_select are ingress network qos
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_QOS_NETWORK_VALID, FALSE);
    for (fwd_type = 0; fwd_type < DBAL_NOF_ENUM_ETPP_QOS_VAR_TYPE_VALUES; fwd_type++)
    {
        switch (fwd_type)
        {
            case DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS:
            case DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC:
            case DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC:
                for (fwd_plus1_type = 0; fwd_plus1_type < max_fwd_plus1_type; fwd_plus1_type++)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MAP_TYPE, fwd_type);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MAP_TYPE_PLUS1,
                                               fwd_plus1_type);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT,
                                                 INST_SINGLE,
                                                 DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_FORWARD_NETWORK_QOS);

                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
                break;
            case DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI:
                for (fwd_plus1_type = 0; fwd_plus1_type < max_fwd_plus1_type; fwd_plus1_type++)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MAP_TYPE, fwd_type);
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_QOS_MAP_TYPE_PLUS1,
                                               fwd_plus1_type);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT,
                                                 INST_SINGLE, DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS);
                    dbal_entry_value_field32_set(unit, entry_handle_id,
                                                 DBAL_FIELD_NETWORK_QOS_INITIAL_SELECT_PLUS_1, INST_SINGLE,
                                                 DBAL_ENUM_FVAL_NETWORK_QOS_INITIAL_SELECT_TC_NETWORK_QOS);
                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                }
                break;
            default:
                break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table: DBAL_TABLE_QOS_DP_TO_TYPE_FWD.
 * set the appropriate qos var type for each forwarding code and
 * ecn eligiable combination.
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_remark_qos_map_type_select_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 qos_map_type;
    uint32 remark_or_keep;
    dbal_enum_value_field_egress_fwd_code_e egress_fwd_code;
    dbal_enum_value_field_ftmh_ecn_eligible_e ecn_enable;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Currently, EGRESS_FWD_CODE == *_R0 (FAI[3]==0) is mapped to REMAKR.
     *            EGRESS_FWD_CODE == *_R1 (FAI[3]==1) is mapped to PRESERVE.
     * Note: Just exchange the two fwd-code after updating ucode with FAI[3]==1 mapping to REMARK.
     */
    remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, &entry_handle_id));

    for (egress_fwd_code = 0; egress_fwd_code < DBAL_NOF_ENUM_EGRESS_FWD_CODE_VALUES; egress_fwd_code++)
    {
        for (ecn_enable = 0; ecn_enable < DBAL_NOF_ENUM_FTMH_ECN_ELIGIBLE_VALUES; ecn_enable++)
        {
            /*
             * we have 32 forwarding code types and only 8 qos var type map
             * each of the forwarding code type to it's appropeiate qos var
             * type. In case ECN enable, new opcode for IPv4&IPv6, one ECN opcode represent 4 opcode
             */
            switch (egress_fwd_code)
            {
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_ETHERNET:
                    qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI;
                    break;
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS:
                    qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC;
                    break;
                /** All IPv4 cases use the same "break". */
                     /* coverity[unterminated_case:FALSE]  */
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R1:
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R1:
                    if (dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_egress_remark_or_keep_enabler))
                    {
                        remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP;
                    }
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_UC_R0:
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV4_MC_R0:
                    if (ecn_enable)
                    {
                        qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS_ECN;
                    }
                    else
                    {
                        qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS;
                    }
                    break;
                /** All IPv6 cases use the same "break". */
                     /* coverity[unterminated_case:FALSE]  */
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R1:
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R1:
                    if (dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_egress_remark_or_keep_enabler))
                    {
                        remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP;
                    }
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_UC_R0:
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_IPV6_MC_R0:
                    if (ecn_enable)
                    {
                        qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC_ECN;
                    }
                    else
                    {
                        qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC;
                    }
                    break;
                case DBAL_ENUM_FVAL_EGRESS_FWD_CODE_BIER_MPLS:
                    qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC;
                    break;
                default:
                    qos_map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI;
                    break;
            }

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, egress_fwd_code);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_ECN_ELIGIBLE, ecn_enable);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_QOS_VAR_TYPE, INST_SINGLE,
                                         qos_map_type);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                         remark_or_keep);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECN_IS_ELIGIBLE, INST_SINGLE, ecn_enable);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table:
 * DBAL_TABLE_EGRESS_CFG_MAP_REMARK_PROFILE_QOS.
 * 1:1 mapping of remark profile to mapped remark profile for
 * both FWD and ENC.
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_remark_profile_qos_init(
    int unit)
{
    uint32 entry_handle_id;
    uint8 remark_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_REMARK_PROFILE_INFO, &entry_handle_id));

    for (remark_profile = 0; remark_profile < dnx_data_qos.qos.nof_egress_profiles_get(unit); remark_profile++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, remark_profile);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAPPED_REMARK_PROFILE, INST_SINGLE,
                                     remark_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table:
 * DBAL_TABLE_EGRESS_CFG_MAP_REMARK_PROFILE_QOS.
 * 1:1 mapping of remark profile to mapped remark profile for
 * both FWD and ENC.
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_layer_qos_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    int qos_var;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** profile 0 mapping opcode to 0 for all protocols*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));
    dbal_entry_key_field8_range_set(unit, entry_handle_id, DBAL_FIELD_QOS_LAYER_PROTOCOL, DBAL_RANGE_ALL,
                                    DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, INST_SINGLE, 0);

    /*
     * init 1:1 mapping for opcode 0 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS, &entry_handle_id));

    for (qos_var = 0; qos_var <= dnx_data_qos.qos.packet_max_dscp_get(unit); qos_var++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, 0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_QOS, qos_var);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS, INST_SINGLE, qos_var);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * init DBAL Table: QOS_EGRESS_TLL_MODEL_CONFIGURATION This
 * table group set ttl_mode(PIPE/Uniform)
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_ttl_model_configuration_init(
    int unit)
{
    uint32 in_ttl_mode;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_TTL_MODEL_CONFIGURATION, &entry_handle_id));

    for (in_ttl_mode = 0; in_ttl_mode < DBAL_NOF_ENUM_TTL_MODE_VALUES; in_ttl_mode++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_TTL_MODE, in_ttl_mode);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_TTL_DECREMENT_DISABLE, DBAL_RANGE_ALL,
                                         DBAL_RANGE_ALL);
        if (in_ttl_mode == DBAL_ENUM_FVAL_TTL_MODE_PIPE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_INHERITANCE, INST_SINGLE, FALSE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_MAP, INST_SINGLE, TRUE);
        }
        else
        {
            /**DBAL_ENUM_FVAL_TTL_MODE_UNIFORM:*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_INHERITANCE, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_MAP, INST_SINGLE, FALSE);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Set TTL DECREMENT EN/DIS
 * Indicates whether the TTL decrement should occur or not.
 * update DBAL table: EGRESS_TTL_DECREMENT_CONFIGURATION
 *    \param [in] unit - Relevant unit.
 *    \param [in] layer_type - relevant layer type
 *    \param [in] fwd_additional_info - Relevant
 *                      pph_forwarding_layer_additional_info.
 *    \param [in] ttl_dec_en - 1 - decrement TTL by 1
 *                             0 - do not decrement.
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_ttl_decrement_set(
    int unit,
    dbal_enum_value_field_layer_types_e layer_type,
    uint32 fwd_additional_info,
    uint8 ttl_dec_en)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_TTL_DECREMENT_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARDING_ADDITIONAL_INFO, fwd_additional_info);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LAYER_TYPES, layer_type);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_TTL_DECREMENT_DISABLE, INST_SINGLE, ttl_dec_en);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table: EGRESS_TTL_DECREMENT_CONFIGURATION
 * Indicates whether the TTL decrement should occur or not.
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_ttl_decrement_init(
    int unit)
{
    dbal_enum_value_field_layer_types_e layer_type;
    etpcc_ttl_decrement_config_t *entry;
    int system_headers_mode;

    uint32 fwd_additional_info;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        entry = (etpcc_ttl_decrement_config_t *) jr2_ttl_decrement_config_map;
    }
    else
    {
        entry = (etpcc_ttl_decrement_config_t *) jr_ttl_decrement_config_map;
    }

    for (layer_type = 0; layer_type < DBAL_NOF_ENUM_LAYER_TYPES_VALUES; layer_type++)
    {
        for (fwd_additional_info = 0; fwd_additional_info < 16; fwd_additional_info++)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_decrement_set(unit, layer_type, fwd_additional_info,
                                                             entry[fwd_additional_info].ttl_dec_disable));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize DBAL Table: QOS_EGRESS_TTL_PIPE_MAPPING with the reserved ttl profile.
 *
 * \param [in] unit - Relevant unit.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_ttl_pipe_profile_init(
    int unit)
{
    uint32 ttl_profile;

    SHR_FUNC_INIT_VARS(unit);

    ttl_profile = dnx_data_qos.qos.eei_mpls_ttl_profile_get(unit);
    SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_profile_hw_set(unit, ttl_profile, 255));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize qos egress tables
 */
shr_error_e
dnx_qos_egress_tables_init(
    int unit)
{
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    SHR_IF_ERR_EXIT(dnx_qos_egress_remark_qos_model_mapping_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_remark_nwk_qos_select_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_remark_tc_nwk_qos_mapping_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_remark_qos_map_type_select_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_remark_profile_qos_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_layer_qos_mapping_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_decrement_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_model_configuration_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_ttl_pipe_profile_init(unit));
    SHR_IF_ERR_EXIT(dnx_qos_egress_fwd_plus_1_remark_table_init(unit));

    if (system_headers_mode != DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_remark_jer_ingress_nwk_qos_mapping_init(unit));
        SHR_IF_ERR_EXIT(dnx_qos_egress_fhei_model_mapping_init(unit));
    }

    if (dnx_data_qos.feature.feature_get(unit, dnx_data_qos_feature_egress_remark_or_keep_enabler))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_forward_header_remark_enabler_init(unit));
    }
    else if (dnx_data_qos.qos.routing_dscp_preserve_mode_get(unit) == ROUTING_DSCP_PRESERVE_MODE_INLIF_OUTLIF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "routing DSCP preserve mode (%d) is not available on current device\n",
                     ROUTING_DSCP_PRESERVE_MODE_INLIF_OUTLIF);
    }

    if (dnx_data_qos.qos.feature_get(unit, dnx_data_qos_qos_qos_var_type_map_enable))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_qos_var_type_map_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   allocated profile 0 as default profile
 */
shr_error_e
dnx_qos_egress_profiles_init(
    int unit)
{
    uint32 flags;
    int map_id;

    SHR_FUNC_INIT_VARS(unit);

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    SHR_IF_ERR_EXIT(dnx_qos_egress_map_create(unit, flags, &map_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert bcm flags to map_type (DBAL/HW) table identifiers
 */
static shr_error_e
dnx_qos_egress_map_type_get(
    int unit,
    uint32 flags,
    uint32 *map_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L3_L2)
    {
        /** get map type for fwd plus one header*/
        if (flags & BCM_QOS_MAP_IPV6)
        {
            *map_type = DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV6;
        }
        else if (flags & BCM_QOS_MAP_IPV4)
        {
            *map_type = DBAL_ENUM_FVAL_QOS_TYPEFWDPLUS1_TYPE_IPV4;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal protocol flags(%x)for forwarding plus one header remarking! "
                         "Only BCM_QOS_MAP_IPV6 and BCM_QOS_MAP_IPV4 are supported currently.\n", flags);
        }
    }
    /** get map type for fwd header*/
    else if (flags & BCM_QOS_MAP_L2)
    {
        *map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_PCP_DEI;
    }
    else if (flags & BCM_QOS_MAP_IPV6)
    {
        if (flags & BCM_QOS_MAP_ECN)
        {
            *map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC_ECN;
        }
        else
        {
            *map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV6_TC;
        }
    }
    else if (flags & BCM_QOS_MAP_IPV4)
    {
        if (flags & BCM_QOS_MAP_ECN)
        {
            *map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS_ECN;
        }
        else
        {
            *map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_IPV4_TOS;
        }
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        *map_type = DBAL_ENUM_FVAL_ETPP_QOS_VAR_TYPE_MPLS_TC;
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        /** map_type not relevant for POLICER_TYPE */
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags %x\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * convert bcm flags to map_protocol (DBAL/HW) table identifiers
 */
static shr_error_e
dnx_qos_egress_map_protocol_get(
    int unit,
    uint32 flags,
    dbal_enum_value_field_qos_layer_protocol_e * map_protocol)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2)
    {
        *map_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_ETH;
    }
    else if (flags & BCM_QOS_MAP_IPV6)
    {
        *map_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV6;
    }
    else if (flags & BCM_QOS_MAP_IPV4)
    {
        *map_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_IPV4;
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        *map_protocol = DBAL_ENUM_FVAL_QOS_LAYER_PROTOCOL_MPLS;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags %x\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * build map var.
 * for each profile & app & map type we get a unique var
 * structure for example for ETH it is PCP/DEI, for IPV6 it is
 * TOS for IPV4 it is DSCP.
 * \param [in] unit - Relevant unit.
 * \param [in] flags - relevant flags
 * \param [in] map_entry - Relevant map entry.
 * \param [in] map_var - Relevant map var.
 * \param [in] old_nwk_qos - old_nwk_qos.
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_build_map_var(
    int unit,
    uint32 flags,
    bcm_qos_map_t * map_entry,
    uint16 *map_var,
    uint32 old_nwk_qos)
{
    uint8 pcp_dei;
    uint8 inner_pcp_dei;
    uint32 pcp_dei_mask;
    uint32 exp_mask;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2)
    {
        /**
         * both outer pcp dei and inner pcp dei are in same nwk_qos
         * outer pcp dei uses qos_variable[7:4], inner pcp dei qos_variable[3:0]
         * see dbal_enum_value_field_eve_pcp_dei_src_e
         */
        if (flags & BCM_QOS_MAP_L2_TWO_TAGS)
        {
            inner_pcp_dei =
                ((map_entry->inner_pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->inner_pkt_cfi &
                                                                                         DNX_QOS_CFI_MASK);
            pcp_dei =
                ((map_entry->
                  pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->pkt_cfi & DNX_QOS_CFI_MASK);
            *map_var = inner_pcp_dei | (pcp_dei << DNX_QOS_PCP_DEI_OFFSET);
        }
        else if (flags & BCM_QOS_MAP_L2_INNER_TAG)
        {
            pcp_dei =
                ((map_entry->inner_pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->inner_pkt_cfi &
                                                                                         DNX_QOS_CFI_MASK);
            pcp_dei_mask = DNX_QOS_PCP_DEI_MASK;
            *map_var = (old_nwk_qos & (~pcp_dei_mask)) | pcp_dei;
        }
        else
        {
            pcp_dei =
                ((map_entry->
                  pkt_pri & DNX_QOS_PRI_MASK) << DNX_QOS_PRI_OFFSET) | (map_entry->pkt_cfi & DNX_QOS_CFI_MASK);
            pcp_dei_mask = DNX_QOS_PCP_DEI_MASK << DNX_QOS_PCP_DEI_OFFSET;
            *map_var = (old_nwk_qos & (~pcp_dei_mask)) | (pcp_dei << DNX_QOS_PCP_DEI_OFFSET);
        }
    }
    else if (flags & BCM_QOS_MAP_IPV6)
    {
        *map_var = map_entry->dscp;
    }
    else if (flags & BCM_QOS_MAP_IPV4)
    {
        *map_var = map_entry->dscp;
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        /*
         * mpls_0_1[exp] uses qos_variable[2:0], mpls_0_2[exp] uses qos_variable[5:3]
         */
        if (flags & BCM_QOS_MAP_MPLS_SECOND)
        {
            exp_mask = DNX_QOS_EXP_MASK << DNX_QOS_SECOND_EXP_OFFSET;
            *map_var = (old_nwk_qos & (~exp_mask)) | (map_entry->exp << DNX_QOS_SECOND_EXP_OFFSET);
        }
        else
        {
            exp_mask = DNX_QOS_EXP_MASK;
            *map_var = (old_nwk_qos & (~exp_mask)) | (map_entry->exp);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags %x\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set qos map structure.
 * for different flags, set different member. for example:
 * set pkt_pri & pkt_cfi for BCM_QOS_MAP_L2, set dscp for BCM_QOS_MAP_L3
 * set expfor BCM_QOS_MAP_MPLS .
 * \param [in] unit - Relevant unit.
 * \param [in] flags - relevant flags
 * \param [in] map_entry - Relevant map entry.
 * \param [in] map_var - Relevant map var.
 * \return
 *    \retval Negative in case of an error.
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_map_var_set_map(
    int unit,
    uint32 flags,
    bcm_qos_map_t * map_entry,
    uint32 map_var)
{
    uint8 pcp_dei;
    uint32 pcp_dei_mask;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_L2)
    {
        /**
         * both outer pcp dei and inner pcp dei are in same nwk_qos
         * outer pcp dei uses pcp_dei_0, inner pcp dei uses pcp_dei_1
         */
        if (flags & BCM_QOS_MAP_L2_INNER_TAG)
        {
            pcp_dei = map_var & DNX_QOS_PCP_DEI_MASK;
            map_entry->inner_pkt_pri = (pcp_dei >> DNX_QOS_PRI_OFFSET) & DNX_QOS_PRI_MASK;
            map_entry->inner_pkt_cfi = pcp_dei & DNX_QOS_CFI_MASK;
        }
        else
        {
            pcp_dei_mask = DNX_QOS_PCP_DEI_MASK << DNX_QOS_PCP_DEI_OFFSET;
            pcp_dei = (map_var & pcp_dei_mask) >> DNX_QOS_PCP_DEI_OFFSET;
            map_entry->pkt_pri = (pcp_dei >> DNX_QOS_PRI_OFFSET) & DNX_QOS_PRI_MASK;
            map_entry->pkt_cfi = pcp_dei & DNX_QOS_CFI_MASK;
        }
    }
    else if (flags & BCM_QOS_MAP_L3)
    {
        map_entry->dscp = map_var & DNX_QOS_TOS_MASK;
    }
    else if (flags & BCM_QOS_MAP_MPLS)
    {
        /*
         * mpls_0_1[exp] uses qos_variable[2:0], mpls_0_2[exp] uses qos_variable[5:3]
         */
        if (flags & BCM_QOS_MAP_MPLS_SECOND)
        {
            map_entry->exp = (map_var >> DNX_QOS_SECOND_EXP_OFFSET) & DNX_QOS_EXP_MASK;
        }
        else
        {
            map_entry->exp = map_var & DNX_QOS_EXP_MASK;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for flags %x\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  create the egress sw qos profile according to the flags
 */
shr_error_e
dnx_qos_egress_map_create(
    int unit,
    uint32 flags,
    int *map_id)
{
    uint32 resource_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_WITH_ID)
    {
        /** Get the input map_id.*/
        resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        *map_id = DNX_QOS_MAP_PROFILE_GET(*map_id);
    }

    if (flags & BCM_QOS_MAP_EGRESS)
    {
        if (flags & BCM_QOS_MAP_POLICER)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_policer.allocate_single(unit, resource_flags, NULL, map_id));
            /** set map policer */
            DNX_QOS_POLICER_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.allocate_single(unit, resource_flags, NULL, map_id));

            /** set map remark */
            DNX_QOS_REMARK_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_mpls_php.allocate_single(unit, resource_flags, NULL, map_id));

            /** set map remark */
            DNX_QOS_MPLS_PHP_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_OAM_PCP)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_oam_pcp_map.allocate_single(unit, resource_flags, NULL, map_id));

           /** set oam pcp */
            DNX_QOS_OAM_PCP_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_cos_profile.allocate_single(unit, resource_flags, NULL, map_id));

           /** set encap cos */
            DNX_QOS_ENCAP_COS_SET(*map_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for EGRESS QOS profile [%x]\n", flags);
        }
        /** set map direction  */
        DNX_QOS_EGRESS_MAP_SET(*map_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  create the egress sw qos opcode according to the flags
 */
shr_error_e
dnx_qos_egress_opcode_create(
    int unit,
    uint32 flags,
    int *map_opcode)
{
    uint32 resource_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_WITH_ID)
    {
        /** Get the input value.*/
        resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        *map_opcode = DNX_QOS_MAP_PROFILE_GET(*map_opcode);
    }

    if (flags & BCM_QOS_MAP_EGRESS)
    {
        if (flags & BCM_QOS_MAP_REMARK)
        {
            if (flags & BCM_QOS_MAP_ECN)
            {
                int start_opcode = 0, opcode, max_opcode;
                uint8 is_allocated = FALSE;

                /*
                 * burn 4 opcodes to represent one ECN opcode 
                 * check if there is free opcode
                 */
                max_opcode = dnx_data_qos.qos.nof_egress_opcode_profiles_get(unit) - DNX_QOS_ECN_BURN_OPCODE_NUM;
                if (flags & BCM_QOS_MAP_WITH_ID)
                {
                    start_opcode = DNX_QOS_MAP_PROFILE_GET(*map_opcode);
                    if (0 != (start_opcode % DNX_QOS_ECN_BURN_OPCODE_NUM))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value profile index %d for ecn, must be multiples of 4\n",
                                     *map_opcode);
                    }
                    for (opcode = start_opcode; opcode < start_opcode + DNX_QOS_ECN_BURN_OPCODE_NUM; opcode++)
                    {
                        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.is_allocated(unit, opcode, &is_allocated));
                        if (is_allocated == TRUE)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    for (start_opcode = 0; start_opcode < max_opcode; (start_opcode += DNX_QOS_ECN_BURN_OPCODE_NUM))
                    {
                        is_allocated = FALSE;
                        for (opcode = start_opcode; opcode < start_opcode + DNX_QOS_ECN_BURN_OPCODE_NUM; opcode++)
                        {
                            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.is_allocated(unit, opcode, &is_allocated));
                            if (is_allocated == TRUE)
                            {
                                break;
                            }
                        }
                        if (is_allocated == FALSE)
                        {
                            break;
                        }
                    }
                }
                /** allocate all 4 opcodes*/
                if ((is_allocated == FALSE) && (start_opcode < max_opcode))
                {
                    resource_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
                    SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.allocate_several(unit, resource_flags,
                                                                                   DNX_QOS_ECN_BURN_OPCODE_NUM, NULL,
                                                                                   &start_opcode));
                    *map_opcode = start_opcode;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "no resource for ECN opcode\n");
                }
                DNX_QOS_ECN_MAP_SET(*map_opcode);
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.allocate_single(unit, resource_flags, NULL, map_opcode));
            }
            /** set map remark */
            DNX_QOS_REMARK_MAP_SET(*map_opcode);
        }
        else if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_php_opcode.allocate_single(unit, resource_flags, NULL, map_opcode));
            /** set map remark */
            DNX_QOS_MPLS_PHP_MAP_SET(*map_opcode);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for EGRESS QOS profile [%x]\n", flags);
        }
        /** set map opcode */
        DNX_QOS_OPCODE_MAP_SET(*map_opcode);
        /** set map direction  */
        DNX_QOS_EGRESS_MAP_SET(*map_opcode);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dnx_qos_egress_map_opcode_set
 * set qos map opcode connect egress remark profile
 *     \param [in] unit - Relevant unit.
 *     \param [in] map_id - ID of the EGRESS MAP.
 *     \param [in] flags - Relevant flags.
 *     \param [in] opcode - encoded ID for internal map.
 *
 *  \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 *
 *  \remark
 *   opcode may be BCM_QOS_OPCODE_PRESERVE for DSCP preserve.
 *   If DSCP preserve is per inlif and outlif, user should map the remark-profile
 *   to a valid opcode first, and then map it to BCM_QOS_OPCODE_PRESERVE again.
 */
static shr_error_e
dnx_qos_egress_map_opcode_set(
    int unit,
    int map_id,
    uint32 flags,
    uint32 opcode)
{
    uint32 entry_handle_id;
    int profile_ndx;
    int opcode_ndx;
    uint32 qos_remark_or_keep;
    uint32 map_type;
    dbal_enum_value_field_qos_layer_protocol_e map_protocol;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(map_id);
    opcode_ndx = DNX_QOS_MAP_PROFILE_GET(opcode);

    if (flags & BCM_QOS_MAP_REMARK)
    {
        if (opcode == BCM_QOS_OPCODE_PRESERVE)
        {
            qos_remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP;
        }
        else
        {
            qos_remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;
        }

        SHR_IF_ERR_EXIT(dnx_qos_egress_map_type_get(unit, flags, &map_type));

        if (!_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_L3_L2))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_QOS_VAR_TYPE, map_type);
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_QOS_VAR_TYPE, map_type);
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile_ndx);
        if (qos_remark_or_keep == DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK)
        {
            /** If it is KEEP, the opcode is unchanged for the possible original qos mapping.*/
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, opcode_ndx);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                     qos_remark_or_keep);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
    else if (flags & BCM_QOS_MAP_MPLS_PHP)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_protocol_get(unit, flags, &map_protocol));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile_ndx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_LAYER_PROTOCOL, map_protocol);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, INST_SINGLE,
                                     opcode_ndx);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * dnx_qos_egress_remark_map_to_internal_clear
 * clear opcode id connected to egress remark profile
 *     \param [in] unit - Relevant unit.
 *     \param [in] map_id - ID of the EGRESS MAP.
 *     \param [in] flags - Relevant flags
 *  \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_map_opcode_clear(
    int unit,
    int map_id,
    uint32 flags)
{
    uint32 entry_handle_id;
    int profile_ndx;
    uint32 map_type;
    dbal_enum_value_field_qos_layer_protocol_e map_protocol;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(map_id);
    if (flags & BCM_QOS_MAP_REMARK)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_type_get(unit, flags, &map_type));

        if (!_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_L3_L2))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_QOS_VAR_TYPE, map_type);
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_QOS_VAR_TYPE, map_type);
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile_ndx);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    }
    else if (flags & BCM_QOS_MAP_MPLS_PHP)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_protocol_get(unit, flags, &map_protocol));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile_ndx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_LAYER_PROTOCOL, map_protocol);

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API returns the opcode id connected to qos_profile and
 * map type.
 *     \param [in] unit - Relevant unit.
 *     \param [in] map_id - ID of the EGRESS MAP.
 *     \param [in] flags - Relevant flags
 *     \param [out] opcode - ID of internal map.
 *  \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_egress_map_opcode_get(
    int unit,
    int map_id,
    uint32 flags,
    uint32 *opcode)
{
    uint32 map_type;
    dbal_enum_value_field_qos_layer_protocol_e map_protocol;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 qos_remark_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;
    int profile_ndx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(map_id);

    if (flags & BCM_QOS_MAP_REMARK)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_type_get(unit, flags, &map_type));

        if (_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_L3_L2))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_FWD_PLUS_1_INTERNAL_MAP_ID, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD_PLUS_1_QOS_VAR_TYPE, map_type);
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_QOS_EGRESS_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_QOS_VAR_TYPE, map_type);
        }

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile_ndx);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, INST_SINGLE, opcode);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE,
                                   &qos_remark_or_keep);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        DNX_QOS_REMARK_MAP_SET(*opcode);
    }
    else if (flags & BCM_QOS_MAP_MPLS_PHP)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_protocol_get(unit, flags, &map_protocol));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_QOS_EGRESS_PROTOCOL_PROFILE_TO_INTERNAL_MAP_ID, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile_ndx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_LAYER_PROTOCOL, map_protocol);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, INST_SINGLE,
                                   opcode);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        DNX_QOS_MPLS_PHP_MAP_SET(*opcode);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
    }

    /** Encoding the opcode*/
    if (qos_remark_or_keep == DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP)
    {
        *opcode = BCM_QOS_OPCODE_PRESERVE;
    }
    else
    {
        DNX_QOS_EGRESS_MAP_SET(*opcode);
        DNX_QOS_OPCODE_MAP_SET(*opcode);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos EGRESS MARK map
 */
shr_error_e
dnx_qos_egress_mark_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint16 map_var = 0;
    uint32 nwk_qos;
    int dp;
    uint32 entry_handle_id;
    int opcode_ndx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx);

    /** get old nwk_qos value */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS, INST_SINGLE, &nwk_qos);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * this entry represents how to map the nwk_qos to the packet parameter
     */
    SHR_IF_ERR_EXIT(dnx_qos_egress_build_map_var(unit, flags, map_entry, &map_var, nwk_qos));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS, INST_SINGLE, map_var);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos EGRESS MARK for IPv4/6, update ECN field with CNI and ECN Eligible.
 * In single call, will add one mapping entry for each of 4 consecutive opcodes 
 *     \param [in] unit - Relevant unit.
 *     \param [in] flags - Relevant flags.
 *     \param [in] map_entry - relevant map entry.
 *     \param [in] map_opcode - encoded map opcode of egress map for current layer marking
 *  
 *  \return
 *     \retval Negative in case of an error
 *     \retval Zero in case of NO ERROR
 */
shr_error_e static
dnx_qos_egress_mark_map_with_ecn_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint16 map_var = 0;
    uint32 nwk_qos;
    int dp;
    uint32 entry_handle_id;
    int opcode_ndx;
    uint8 ecn_eligible, ecn_congestion;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, &entry_handle_id));

    /*
     * 2 Lsb of input ECN opcode should be zero
     * udpate opcode with CNI and Eligible, 
     * CNI and Eligible are part of opcode actually
     * add mapping for each opcode of total 4 
     */
    for (ecn_congestion = FALSE; ecn_congestion <= TRUE; ecn_congestion++)
    {
        for (ecn_eligible = FALSE; ecn_eligible <= TRUE; ecn_eligible++)
        {
            opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);
            opcode_ndx |= ecn_eligible;
            opcode_ndx |= ecn_congestion << DNX_QOS_ECN_CNI_OFFSET;
            /**add mapping entry*/
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, map_entry->int_pri);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx);

            /** get old nwk_qos value */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS, INST_SINGLE,
                                       &nwk_qos);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(dnx_qos_egress_build_map_var(unit, flags, map_entry, &map_var, nwk_qos));
            map_var &= ~DNX_QOS_ECN_MASK;
            map_var |= ecn_eligible;
            if (ecn_eligible)
            {
                map_var |= ecn_congestion << DNX_QOS_ECN_CNI_OFFSET;
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS, INST_SINGLE,
                                         map_var);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos EGRESS REMARK map
 */
shr_error_e
dnx_qos_egress_remark_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    int dp, remark_dp;
    uint32 entry_handle_id;
    int profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->remark_color, &remark_dp));
    profile = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_QOS, INST_SINGLE,
                                 map_entry->remark_int_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_DP, INST_SINGLE, remark_dp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos EGRESS cos map
 */
shr_error_e
dnx_qos_egress_cos_map_add(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int cos_profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    int dp = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COS_PROFILE, cos_profile);
    if (((uint32) map_entry->int_pri) <= BCM_COS_MAX)
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TC_VALID, INST_SINGLE, TRUE);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_TC, INST_SINGLE, map_entry->int_pri);
    }
    else
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TC_VALID, INST_SINGLE, FALSE);
    }
    if (((uint32) map_entry->color) < bcmColorPreserve)
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DP_VALID, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DP, INST_SINGLE, dp);
    }
    else
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DP_VALID, INST_SINGLE, FALSE);
    }
    if (((uint32) map_entry->dscp) <= dnx_data_qos.qos.packet_max_dscp_get(unit))
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_VALID, INST_SINGLE, TRUE);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_NWK_QOS, INST_SINGLE, map_entry->dscp);
    }
    else
    {
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_VALID, INST_SINGLE, FALSE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos EGRESS REMARK map entry

 * Details: see qos_internal.h
 */
shr_error_e
dnx_qos_egress_mark_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int opcode_ndx, dp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, &entry_handle_id));
    /*
     * clear qos mapping : internal map id + internal priority.
     */
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));
    opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos EGRESS REMARK map which was updated with ECN
 * In single call, will delete one mapping entry from each of 4 consecutive opcodes
 *   \param [in] unit - Relevant unit.
 *   \param [in] flags - Relevant flags
 *   \param [in] map_entry - relevant map entry.
 *   \param [in] map_opcode -encoded map opcode of egress map for current layer marking 
 *   
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e static
dnx_qos_egress_mark_map_with_ecn_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int opcode_ndx;
    int dp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, &entry_handle_id));
    /*
     * clear qos mapping : internal map id + internal priority.
     */
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));
    opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);
    /**one ECN opcode represent 4 opcodes*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID, opcode_ndx,
                                     (opcode_ndx + DNX_QOS_ECN_BURN_OPCODE_NUM - 1));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos EGRESS REMARK map entry

 * Details: see qos_internal.h
 */
shr_error_e
dnx_qos_egress_remark_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int profile, dp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * clear next layer nwk_qos.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER, &entry_handle_id));

    profile = DNX_QOS_MAP_PROFILE_GET(map_id);
    SHR_IF_ERR_EXIT(dnx_qos_color_encode(unit, map_entry->color, &dp));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, map_entry->color);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, profile);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * add Qos EGRESS mpls php map
 */
shr_error_e
dnx_qos_egress_php_map_add(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, opcode_ndx);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS, INST_SINGLE,
                                 map_entry->remark_int_pri);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos EGRESS mpls php map entry

 * Details: see qos_internal.h
 */
shr_error_e
dnx_qos_egress_php_map_delete(
    int unit,
    uint32 flags,
    int map_opcode,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int opcode_ndx = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS, &entry_handle_id));
    /*
     * clear current layer qos mapping : internal map id + internal priority.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_QOS, map_entry->int_pri);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, opcode_ndx);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set to HW the policer_offset based on the Qos profile and tc.
 *   Used by meter_ptr as follow: {Meter_Ptr = Meter_ID + policer_offset}
 */
shr_error_e
dnx_qos_egress_policer_map_set(
    int unit,
    int map_id,
    int int_pri,
    int policer_offset)
{
    uint32 entry_handle_id;
    int profile_ndx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_POLICER_OFFSET_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_POLICER_PROFILE_ID, profile_ndx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, int_pri);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POLICER_OFFSET, INST_SINGLE, policer_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete Qos EGRESS cos map entry
 */
shr_error_e
dnx_qos_egress_cos_map_delete(
    int unit,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    uint32 entry_handle_id;
    int cos_profile = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COS_PROFILE, cos_profile);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the egress oam priority map
 *   Used to map TC to PCP for all priority map profiles
 */
static shr_error_e
dnx_qos_egress_oam_priority_map_set(
    int unit,
    int map_id,
    int int_pri,
    int pkt_pri)
{
    uint32 entry_handle_id;
    int profile_ndx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_PRIORITY_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PRIORITY_MAP_PROFILE, profile_ndx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, pkt_pri);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PCP, INST_SINGLE, int_pri);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get from HW the policer_offset based on the Qos profile and tc.
 */
static shr_error_e
dnx_qos_egress_policer_map_get(
    int unit,
    int map_id,
    int int_pri,
    int *policer_offset)
{
    uint32 entry_handle_id;
    int profile_ndx;
    uint32 offset;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(map_id);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_POLICER_OFFSET_MAP, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QOS_POLICER_PROFILE_ID, profile_ndx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, int_pri);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_POLICER_OFFSET, INST_SINGLE, &offset);

    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    *policer_offset = (int) offset;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get from HW the policer_offset based on the Qos profile and tc.
 */
static shr_error_e
dnx_qos_egress_cos_map_get(
    int unit,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    uint32 entry_handle_id;
    int cos_profile;
    uint8 tc_valid = FALSE, dp_valid = FALSE, nwk_qos_valid = FALSE;
    uint8 tc, dp, nwk_qos;
    bcm_qos_map_t *map;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *array_count = 1;

    if (array_size == 0)
    {
        /** querying the size of map */
        SHR_EXIT();
    }

    cos_profile = DNX_QOS_MAP_PROFILE_GET(map_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_COS_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_COS_PROFILE, cos_profile);
    /** Setting pointers value to receive the fields */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TC_VALID, INST_SINGLE, &tc_valid);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_TC, INST_SINGLE, &tc);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DP_VALID, INST_SINGLE, &dp_valid);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_DP, INST_SINGLE, &dp);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_NWK_QOS_VALID, INST_SINGLE, &nwk_qos_valid);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EGRESS_NWK_QOS, INST_SINGLE, &nwk_qos);
    /** Preforming the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    map = &array[0];
    bcm_qos_map_t_init(map);
    if (tc_valid)
    {
        map->int_pri = tc;
    }
    else
    {
        map->int_pri = BCM_COS_INVALID;
    }
    if (dp_valid)
    {
        SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, dp, &map->color));
    }
    else
    {
        map->color = bcmColorPreserve;
    }
    if (nwk_qos_valid)
    {
        map->dscp = nwk_qos;
    }
    else
    {
        map->dscp = BCM_COS_INVALID;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the egress remark mapping entries
 *    return mapping entry array, and number of mapping.
 *    if array_size is zero, means only to get number of entries.
 */
static int
dnx_qos_egress_remark_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    int idx = 0;
    uint32 opcode;
    uint32 data1;
    bcm_qos_map_t *map;
    uint32 int_pri, dp, remark_dp;
    uint32 entry_handle_id;
    uint32 profile_mask[1];
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_mask[0] = DNX_QOS_MAP_PROFILE_MASK;
    /** 
     * if map id is opcode, then it is for current layer, 
     * otherwise it is for next layer
     */
    if (DNX_QOS_MAP_IS_OPCODE(map_id))
    {
        opcode = DNX_QOS_MAP_PROFILE_GET(map_id);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_PACKET, &entry_handle_id));

        dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT);

        /** Add KEY rule */
        dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_QOS_INT_MAP_ID,
                                               DBAL_CONDITION_EQUAL_TO, &opcode, profile_mask);
        dbal_iterator_get_next(unit, entry_handle_id, &is_end);
        while (!is_end)
        {
            /** process the entry */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, &int_pri));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DP, &dp));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_CURRENT_LAYER_NETWORK_QOS, INST_SINGLE, &data1));

            map = &array[idx++];
            bcm_qos_map_t_init(map);

            map->int_pri = int_pri;
            SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, dp, &map->color));
            SHR_IF_ERR_EXIT(dnx_qos_egress_map_var_set_map(unit, flags, map, data1));

            if (idx >= array_size)
            {
                break;
            }
            dbal_iterator_get_next(unit, entry_handle_id, &is_end);
        }
    }
    else
    {
        uint32 profile = DNX_QOS_MAP_PROFILE_GET(map_id);
        idx = 0;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_MAPPING_NEXT_LAYER, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

        /** Add KEY rule */
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE,
                                                               DBAL_CONDITION_EQUAL_TO, &profile, profile_mask));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            /** process the entry */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS, &int_pri));
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DP, &dp));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_NETWORK_QOS, INST_SINGLE, &data1));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_NEXT_LAYER_DP, INST_SINGLE, &remark_dp));

            map = &array[idx++];
            /*
             * only update nextlayer priority and dp
             */
            bcm_qos_map_t_init(map);

            map->int_pri = int_pri;
            SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, dp, &map->color));
            map->remark_int_pri = data1;
            SHR_IF_ERR_EXIT(dnx_qos_color_decode(unit, remark_dp, &map->remark_color));

            if (idx >= array_size)
            {
                break;
            }
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }

    *array_count = idx;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the egress mpls php mapping entries
 *    return mapping entry array, and number of mapping.
 *    if array_size is zero, means only to get number of entries.
 */
static int
dnx_qos_egress_mpls_php_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    int idx = 0;
    uint32 map_opcode, opcode;
    uint32 data1;
    bcm_qos_map_t *map;
    uint32 int_pri;
    uint32 entry_handle_id;
    uint32 profile_mask[1];
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    profile_mask[0] = DNX_QOS_MAP_PROFILE_MASK;

    if (DNX_QOS_MAP_IS_OPCODE(map_id))
    {
        opcode = DNX_QOS_MAP_PROFILE_GET(map_id);
        map_opcode = map_id;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_opcode_get(unit, map_id, flags, &map_opcode));
        opcode = DNX_QOS_MAP_PROFILE_GET(map_opcode);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAPPING_INITIAL_NETWORK_QOS, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS_INT_MAP_ID, DBAL_CONDITION_EQUAL_TO, &opcode,
                     profile_mask));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        /** process the entry */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PACKET_QOS, &int_pri));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_INITIAL_NETWORK_QOS, INST_SINGLE, &data1));

        map = &array[idx++];
        bcm_qos_map_t_init(map);

        map->int_pri = int_pri;
        map->opcode = map_opcode;
        map->remark_int_pri = data1;

        if (idx >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    *array_count = idx;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    to get the egress mapping entries
 *    return mapping entry array, and number of mapping.
 *    if array_size is zero, means only to get number of entries.
 *    in case flag, 
 *       BCM_QOS_MAP_POLICER get policer mapping of map id
 *       BCM_QOS_MAP_OPCODE get mapped opcode of map id 
 *       BCM_QOS_MAP_REMARK get all egress remark mapping of map id
 *       BCM_QOS_MAP_MPLS_PHP get all mpls php mapping of map id
 */
int
dnx_qos_egress_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    int idx = 0, num_entries = 0;
    uint32 map_opcode;
    bcm_qos_map_t *map;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_QOS_MAP_POLICER)
    {
        /** get from HW the policer_offset, per dp. Break the loop when reacing the array size or last tc */
        for (idx = 0; ((idx < array_size) && (idx <= dnx_data_qos.qos.packet_max_tc_get(unit))); idx++)
        {
            array[idx].int_pri = idx;
            SHR_IF_ERR_EXIT(dnx_qos_egress_policer_map_get
                            (unit, map_id, array[idx].int_pri, &array[idx].policer_offset));
            *array_count = idx;
        }
    }
    else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_cos_map_get(unit, map_id, array_size, array, array_count));
    }
    else
    {
        if ((flags & BCM_QOS_MAP_OPCODE) || (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR))
        {
            num_entries = 1;
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            int max_var = dnx_data_qos.qos.max_egress_var_get(unit) + 1;
            int max_dp = dnx_data_qos.qos.packet_max_dp_get(unit) + 1;
            num_entries = max_var * max_dp;
        }
        else if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            num_entries = dnx_data_qos.qos.packet_max_exp_get(unit) + 1;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " [%x]: layer flags was not support yet\n", flags);
        }

        if (array_size == 0)
        {
            /*
             * querying the size of map 
             */
            *array_count = num_entries;
            SHR_EXIT();
        }

        SHR_NULL_CHECK(array, _SHR_E_PARAM, "array Is NULL\n");

        if (flags & BCM_QOS_MAP_OPCODE)
        {
            /*
             * In case BCM_QOS_MAP_OPCODE flag is set, only get internal map id
             */
            SHR_IF_ERR_EXIT(dnx_qos_egress_map_opcode_get(unit, map_id, flags, &map_opcode));
            map = &array[0];
            bcm_qos_map_t_init(map);

            map->opcode = map_opcode;
            *array_count = 1;

            SHR_EXIT();
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_remark_map_multi_get(unit, flags, map_id, array_size, array, array_count));
        }
        else if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_mpls_php_map_multi_get(unit, flags, map_id, array_size, array, array_count));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] not supported yet\n", flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   add egress map entries to an allocated QOS profile
 */
shr_error_e
dnx_qos_egress_map_add(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case BCM_QOS_MAP_OPCODE flag is set we only set the internal map ID
     */
    if ((flags & BCM_QOS_MAP_OPCODE) && ((flags & BCM_QOS_MAP_REMARK) || (flags & BCM_QOS_MAP_MPLS_PHP)))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_opcode_set(unit, map_id, flags, map_entry->opcode));
    }
    else if (flags & BCM_QOS_MAP_REMARK)
    {
        /** 
         * if map id is opcode, then it is for current layer marking, 
         * otherwise it is for next layer
         */
        if (DNX_QOS_MAP_IS_OPCODE(map_id))
        {
            if (flags & BCM_QOS_MAP_ECN)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_mark_map_with_ecn_add(unit, flags, map_id, map_entry));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_mark_map_add(unit, flags, map_id, map_entry));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_remark_map_add(unit, flags, map_id, map_entry));
        }
    }
    else if (flags & BCM_QOS_MAP_MPLS_PHP)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_php_map_add(unit, flags, map_id, map_entry));
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_policer_map_set(unit, map_id, map_entry->int_pri, map_entry->policer_offset));
    }
    else if (flags & BCM_QOS_MAP_OAM_PCP)
    {
        /*
         * Add EGRESS_OAM_PRIORITY_MAP entry
         */
        SHR_IF_ERR_EXIT(dnx_qos_egress_oam_priority_map_set(unit, map_id, map_entry->int_pri, map_entry->pkt_pri));
    }
    else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_cos_map_add(unit, map_id, map_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags[%x] not supported yet\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * ref qos_internal.h
 */
shr_error_e
dnx_qos_egress_map_delete(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case BCM_QOS_MAP_OPCODE flag is set we only reset the internal map ID to 0
     */
    if ((flags & BCM_QOS_MAP_OPCODE) && ((flags & BCM_QOS_MAP_REMARK) || (flags & BCM_QOS_MAP_MPLS_PHP)))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_opcode_clear(unit, map_id, flags));
    }
    if (flags & BCM_QOS_MAP_REMARK)
    {
        /** 
         * if map id is opcode, then it is for current layer marking, 
         * otherwise it is for next layer
         */
        if (DNX_QOS_MAP_IS_OPCODE(map_id))
        {
            if (flags & BCM_QOS_MAP_ECN)
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_mark_map_with_ecn_delete(unit, flags, map_id, map_entry));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_egress_mark_map_delete(unit, flags, map_id, map_entry));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_remark_map_delete(unit, flags, map_id, map_entry));
        }
    }
    else if (flags & BCM_QOS_MAP_MPLS_PHP)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_php_map_delete(unit, flags, map_id, map_entry));
    }
    else if (flags & BCM_QOS_MAP_POLICER)
    {
        /** for policer, delete means set the offset=0 (default value) for the given map_id+TC */
        SHR_IF_ERR_EXIT(dnx_qos_egress_policer_map_set(unit, map_id, map_entry->int_pri, 0));
    }
    else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_cos_map_delete(unit, map_id, map_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags[%x] not supported yet\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

 /**
  *  ref  qos_internal.h
  */

shr_error_e
dnx_qos_egress_port_map_set(
    int unit,
    bcm_gport_t port,
    int egr_map)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources = { 0 };
    uint32 entry_handle_id;
    uint32 profile_ndx;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int old_out_lif_profile, new_out_lif_profile;
    uint32 old_out_lif_profile_get;
    uint8 is_last = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Extract profile index from EGRESS map
     */
    profile_ndx = DNX_QOS_MAP_PROFILE_GET(egr_map);

    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set(unit, entry_handle_id, port, &gport_hw_resources));

    if (BCM_GPORT_IS_VLAN_PORT(port) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(port))
    {
        if (gport_hw_resources.outlif_dbal_result_type == DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_L2_FODO)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "this port (ETPS_L2_FODO entry) does not support qos map set\n");
        }
        /*
         * Update entry of virtual port (ESEM)
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, profile_ndx);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

    }
    else if (BCM_GPORT_IS_VLAN_PORT(port) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(port))
    {
        /** Check qos profile*/
        if (profile_ndx > DNX_QOS_EGR_AC_MAX_PROFILE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "trying to set too big QOS map ID for AC table, the limit is 5 bits[0x%x]\n",
                         profile_ndx);
        }
        /*
         * Update entry
         */
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, profile_ndx);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        /*
         * Handle EEDB
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        if (DNX_QOS_MAP_IS_OAM_PCP(egr_map))
        {
          /** read outlif profile for the given LIF */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &old_out_lif_profile_get));
            old_out_lif_profile = (int) old_out_lif_profile_get;
            /** Get the allocated outlif profile info based on the outlif profile*/
            etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
            SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, old_out_lif_profile, &out_lif_profile_info));
            out_lif_profile_info.oam_lif_profile = profile_ndx;

            /** Exchange the outlif profile info according to the input parameters*/
            SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_exchange
                            (unit, gport_hw_resources.outlif_dbal_table_id, 0, &out_lif_profile_info,
                             old_out_lif_profile, &new_out_lif_profile, &is_last));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE,
                                         new_out_lif_profile);
        }
        else if (DNX_QOS_MAP_IS_REMARK(egr_map))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, profile_ndx);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  ref  qos_internal.h
 */
shr_error_e
dnx_qos_egress_port_map_get(
    int unit,
    bcm_gport_t port,
    int *egr_map,
    uint32 flags)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint8 is_physical_port;
    uint32 entry_handle_id;
    uint32 profile_ndx = BCM_DNX_QOS_MAP_INVALID_ID;
    uint32 out_lif_profile_get;
    etpp_out_lif_profile_info_t out_lif_profile_info;
    int out_lif_profile;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *egr_map = BCM_DNX_QOS_MAP_INVALID_ID;

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_physical_port));
    if (is_physical_port)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal port 0x%x\n", port);
    }
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL,
                                 &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.outlif_dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set(unit, entry_handle_id, port, &gport_hw_resources));

    /*
     * Handle virtual port (ESEM)
     */
    if (BCM_GPORT_IS_VLAN_PORT(port) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(port))
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE,
                                            &profile_ndx);
        *egr_map = profile_ndx;

        /** set map remark  */
        DNX_QOS_REMARK_MAP_SET(*egr_map);

        /** set map direction */
        DNX_QOS_EGRESS_MAP_SET(*egr_map);
    }
    else
    {
        is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        if (flags & BCM_QOS_MAP_OAM_PCP)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_OUT_LIF_PROFILE, INST_SINGLE, &out_lif_profile_get));
            out_lif_profile = (int) out_lif_profile_get;
            /** Get the allocated outlif profile info based on the outlif profile*/
            etpp_out_lif_profile_info_t_init(unit, &out_lif_profile_info);
            SHR_IF_ERR_EXIT(dnx_etpp_out_lif_profile_get_data(unit, out_lif_profile, &out_lif_profile_info));
            profile_ndx = out_lif_profile_info.oam_lif_profile;
            if (profile_ndx != BCM_DNX_QOS_MAP_INVALID_ID)
            {
                SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_oam_pcp_map.is_allocated(unit, profile_ndx, &is_allocated));
                if (is_allocated)
                {
                    *egr_map = profile_ndx;
                    /** set map remark  */
                    DNX_QOS_OAM_PCP_MAP_SET(*egr_map);
                    /** set map direction */
                    DNX_QOS_EGRESS_MAP_SET(*egr_map);
                }
            }
        }
        else
        {
            /** read entry's profile id */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_REMARK_PROFILE, INST_SINGLE, &profile_ndx));
            if (profile_ndx != BCM_DNX_QOS_MAP_INVALID_ID)
            {
                SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.is_allocated(unit, profile_ndx, &is_allocated));
                if (is_allocated)
                {
                    *egr_map = profile_ndx;
                    /** set map remark  */
                    DNX_QOS_REMARK_MAP_SET(*egr_map);
                    /** set map direction */
                    DNX_QOS_EGRESS_MAP_SET(*egr_map);
                }
            }
        }
        if (!is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Valid egress map ID is not found\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * update TTL_PIPE_PROFILE to TTL-value.
 * BCM APIs of IPv4, IPv6 Tunnels will use it.
 */
shr_error_e
dnx_qos_egress_ttl_profile_hw_set(
    int unit,
    uint32 ttl_pipe_profile,
    uint32 ttl_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, ttl_pipe_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * clear TTL_PIPE_PROFILE to TTL-value.
 * BCM APIs of IPv4, IPv6 , MPLS Tunnels will use it.
 */
shr_error_e
dnx_qos_egress_ttl_profile_hw_clear(
    int unit,
    uint32 ttl_pipe_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_TTL_PIPE_MAPPING, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL_PIPE_PROFILE, ttl_pipe_profile);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * convert egress BCM qos model type to DBAL qos model. 
 * see qos.h
 */
shr_error_e
dnx_qos_egress_model_bcm_to_dbal(
    int unit,
    bcm_qos_egress_model_t * qos_model,
    dbal_enum_value_field_encap_qos_model_e * dbal_qos_model)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qos_model, _SHR_E_PARAM, "qos_model Is NULL\n");
    SHR_NULL_CHECK(dbal_qos_model, _SHR_E_PARAM, "qos_model Is NULL\n");

    switch (qos_model->egress_qos)
    {
        case bcmQosEgressModelUniform:
            if (qos_model->egress_ecn == bcmQosEgressEcnModelEligible)
            {
                *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM_ECN;
            }
            else
            {
                *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM;
            }
            break;
        case bcmQosEgressModelPipeNextNameSpace:
            if (qos_model->egress_ecn == bcmQosEgressEcnModelEligible)
            {
                *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN;
            }
            else
            {
                *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE;
            }
            break;
        case bcmQosEgressModelPipeMyNameSpace:
            *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE;
            break;
        case bcmQosEgressModelInitial:
            if (qos_model->egress_ecn == bcmQosEgressEcnModelEligible)
            {
                *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INIT_ECN;
            }
            else
            {
                *dbal_qos_model = DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INITIAL;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos model type %d is not expected", qos_model->egress_qos);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * convert egress DBAL qos model to BCM model. 
 * see qos.h
 */
shr_error_e
dnx_qos_egress_model_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_encap_qos_model_e dbal_qos_model,
    bcm_qos_egress_model_t * qos_model)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qos_model, _SHR_E_PARAM, "qos_model_type Is NULL\n");

    switch (dbal_qos_model)
    {
        case DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM:
            qos_model->egress_qos = bcmQosEgressModelUniform;
            qos_model->egress_ecn = bcmQosEgressEcnModelInvalid;
            break;
        case DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_NEXT_SPACE:
            qos_model->egress_qos = bcmQosEgressModelPipeNextNameSpace;
            qos_model->egress_ecn = bcmQosEgressEcnModelInvalid;
            break;
        case DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_MY_SPACE:
            qos_model->egress_qos = bcmQosEgressModelPipeMyNameSpace;
            qos_model->egress_ecn = bcmQosEgressEcnModelInvalid;
            break;
        case DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_INITIAL:
            qos_model->egress_qos = bcmQosEgressModelInitial;
            qos_model->egress_ecn = bcmQosEgressEcnModelInvalid;
            break;
        case DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_UNIFORM_ECN:
            qos_model->egress_qos = bcmQosEgressModelUniform;
            qos_model->egress_ecn = bcmQosEgressEcnModelEligible;
            break;
        case DBAL_ENUM_FVAL_ENCAP_QOS_MODEL_PIPE_ECN:
            qos_model->egress_qos = bcmQosEgressModelPipeNextNameSpace;
            qos_model->egress_ecn = bcmQosEgressEcnModelEligible;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "qos model %d is not expected", dbal_qos_model);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set network_qos profile attributes
 */
shr_error_e
dnx_qos_egress_network_qos_pipe_profile_set(
    int unit,
    uint8 network_qos_pipe_profile,
    uint32 network_qos_pipe,
    uint32 network_qos_pipe_mapped,
    uint32 dp_pipe_mapped)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_PIPE_PROFILE, network_qos_pipe_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_PIPE, INST_SINGLE, network_qos_pipe);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_PIPE_MAPPED, INST_SINGLE,
                                 network_qos_pipe_mapped);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP_PIPE_MAPPED, INST_SINGLE, dp_pipe_mapped);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * clear network_qos profile attributes
 */
shr_error_e
dnx_qos_egress_network_qos_pipe_profile_hw_clear(
    int unit,
    uint8 network_qos_pipe_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_NETWORK_QOS_PIPE_PROFILE_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NETWORK_QOS_PIPE_PROFILE, network_qos_pipe_profile);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * allocate qos egress network_qos profile for update
 * outlif.NWK_QOS_IDX, the profile is allocated according to the
 * requested network_qos value for current and next layer and
 * the next layer DP value. In case this is the first time this
 * network_qos value is requested a new profile is allocated and
 * the hw is set accordingly with the propare value. In case
 * this network_qos value already exist the profile is returned
 * and the reference counter increment (this is done using the
 * template manager).
 */
shr_error_e
dnx_qos_egress_network_qos_pipe_profile_allocate(
    int unit,
    uint8 network_qos_pipe,
    uint8 network_qos_pipe_mapped,
    uint8 dp_pipe_mapped,
    int *network_qos_pipe_profile,
    uint8 *first_reference)
{
    qos_pipe_profile_template_t network_qos_pipe_profile_data;
    int new_network_qos_pipe_profile;
    SHR_FUNC_INIT_VARS(unit);

    *first_reference = FALSE;

    network_qos_pipe_profile_data.network_qos_pipe = network_qos_pipe;
    network_qos_pipe_profile_data.network_qos_pipe_mapped = network_qos_pipe_mapped;
    network_qos_pipe_profile_data.dp_pipe_mapped = dp_pipe_mapped;

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.allocate_single
                    (unit, 0, &network_qos_pipe_profile_data, NULL, &new_network_qos_pipe_profile, first_reference));

    *network_qos_pipe_profile = new_network_qos_pipe_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get qos network_qos pipe profile attributes for requested
 * network_qos pipe profile ID.
 */
shr_error_e
dnx_qos_egress_network_qos_pipe_profile_attr_get(
    int unit,
    int network_qos_pipe_profile,
    uint8 *network_qos_pipe,
    uint8 *network_qos_pipe_mapped,
    uint8 *dp_pipe_mapped)
{
    int reference_counter;
    qos_pipe_profile_template_t network_qos_profile_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                    (unit, network_qos_pipe_profile, &reference_counter, &network_qos_profile_data));
    *network_qos_pipe = network_qos_profile_data.network_qos_pipe;
    *network_qos_pipe_mapped = network_qos_profile_data.network_qos_pipe_mapped;
    *dp_pipe_mapped = network_qos_profile_data.dp_pipe_mapped;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get qos egress network_qos profile with requested network_qos value
 * it will be nwk_qos_idx of OUTLIFs
 */

shr_error_e
dnx_qos_egress_network_qos_pipe_profile_get(
    int unit,
    uint8 network_qos_pipe,
    uint8 network_qos_pipe_mapped,
    uint8 dp_pipe_mapped,
    int *network_qos_pipe_profile)
{
    qos_pipe_profile_template_t network_qos_pipe_profile_data;
    int new_network_qos_pipe_profile;

    SHR_FUNC_INIT_VARS(unit);

    network_qos_pipe_profile_data.network_qos_pipe = network_qos_pipe;
    network_qos_pipe_profile_data.network_qos_pipe_mapped = network_qos_pipe_mapped;
    network_qos_pipe_profile_data.dp_pipe_mapped = dp_pipe_mapped;

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_get
                    (unit, &network_qos_pipe_profile_data, &new_network_qos_pipe_profile));

    *network_qos_pipe_profile = new_network_qos_pipe_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * update existing profile with new network_qos value. In case
 * the current network_qos profile support the requested
 * network_qos value it will remain unchanged, otherwise a new
 * qos network_qos profile is allocated.
 */
shr_error_e
dnx_qos_egress_network_qos_pipe_profile_update(
    int unit,
    uint16 network_qos_pipe,
    uint16 network_qos_pipe_mapped,
    uint16 dp_pipe_mapped,
    int *network_qos_pipe_profile,
    uint8 *first_reference,
    uint8 *last_reference)
{
    qos_pipe_profile_template_t network_qos_pipe_profile_data;
    qos_pipe_profile_template_t old_network_qos_pipe_profile_data;
    int reference_counter;
    int new_network_qos_pipe_profile;

    SHR_FUNC_INIT_VARS(unit);

    *first_reference = FALSE;
    *last_reference = FALSE;

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.profile_data_get
                    (unit, *network_qos_pipe_profile, &reference_counter, &network_qos_pipe_profile_data));
    /**
     * keep current profile and profile data
     */
    sal_memcpy(&old_network_qos_pipe_profile_data, &network_qos_pipe_profile_data, sizeof(qos_pipe_profile_template_t));

    /**
     * update profile data
     */
    if (network_qos_pipe != DNX_QOS_NETWORK_QOS_PROFILE_KEEP_VALUE)
    {
        network_qos_pipe_profile_data.network_qos_pipe = network_qos_pipe;
    }

    if (network_qos_pipe_mapped != DNX_QOS_NETWORK_QOS_PROFILE_KEEP_VALUE)
    {
        network_qos_pipe_profile_data.network_qos_pipe_mapped = network_qos_pipe_mapped;
    }

    if (dp_pipe_mapped != DNX_QOS_NETWORK_QOS_PROFILE_KEEP_VALUE)
    {
        network_qos_pipe_profile_data.dp_pipe_mapped = dp_pipe_mapped;
    }

     /** allocate TTL PIPE profile */
    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.exchange
                    (unit, 0, &network_qos_pipe_profile_data, *network_qos_pipe_profile,
                     NULL, &new_network_qos_pipe_profile, first_reference, last_reference));

    *network_qos_pipe_profile = new_network_qos_pipe_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * free qos egress network_qos pipe profile reference. the
 * reference counter decrement (this is done using the template
 * manager).
 */
shr_error_e
dnx_qos_egress_network_qos_pipe_profile_free(
    int unit,
    int network_qos_pipe_profile,
    uint8 *last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    *last_reference = FALSE;

    SHR_IF_ERR_EXIT(algo_qos_db.qos_egress_network_qos_profile.free_single
                    (unit, network_qos_pipe_profile, last_reference));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set the exp preserve in MPLS egress swap.
 */
shr_error_e
dnx_qos_egress_control_mpls_fwd_exp_preserve_set(
    int unit,
    uint32 flags,
    int arg)
{
    uint32 entry_handle_id;
    uint32 forward_code;
    uint32 preserve_or_keep;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    preserve_or_keep = arg ? DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP : DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;
    forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS;

    /** set forwarding code for preserve or un-preserve*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FTMH_ECN_ELIGIBLE, FALSE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, forward_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE, preserve_or_keep);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get if exp preserve in MPLS egress swap.
 */
shr_error_e
dnx_qos_egress_control_mpls_fwd_exp_preserve_get(
    int unit,
    uint32 flags,
    int *arg)
{
    uint32 entry_handle_id;
    uint32 forward_code;
    uint32 preserve_or_keep;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *arg = FALSE;
    forward_code = DBAL_ENUM_FVAL_EGRESS_FWD_CODE_MPLS;
    preserve_or_keep = DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_REMARK;

    /** get forwarding code*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_QOS_EGRESS_MAP_TYPE_SELECT, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_FTMH_ECN_ELIGIBLE, FALSE, TRUE);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FORWARD_CODE, forward_code);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_REMARK_OR_PRESERVE, INST_SINGLE, &preserve_or_keep);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *arg = (preserve_or_keep == DBAL_ENUM_FVAL_REMARK_OR_PRESERVE_KEEP) ? TRUE : FALSE;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
