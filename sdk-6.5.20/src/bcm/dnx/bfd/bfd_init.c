/** \file bfd_init.c
 * 
 *
 * BFD init procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** 
 *  Value to use if "your discriminator" field is 0 - same as
 *  Jericho 1.\n
 *  This value is used by the hardware as the LIF when parsing a
 *  BFD packet that has a 0 in the "your discriminator" field.\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: BFD_YOUR_DISC_LIF_NULL\n
 *  physical register: IPPB_OAM_BFD_YOUR_DISCR_TO_LIF_CFG\n
 *  physical field: BFD_NULL_YOUR_DISCR_LIF
 */
#define BFD_NUL_YR_DISC_LIF        0

/** Conversion of a 4 bit profile to a 6 bit flags value   */
#define BFD_FLAGS_PROFILE_TO_VAL(x) ((UTILEX_GET_BITS_RANGE(x, 3, 1) << 3) | (UTILEX_GET_BIT(x, 0) << 1))

/** Numbers of entries in the profile tables   */
#define NOF_FLAGS_PROFILES 16
#define NOF_DIAG_PROFILES  16

/** Router alert header */
#define MPLS_LABEL_ROUTER_ALERT      1
#define MPLS_LABEL_ROUTER_ALERT_TTL  64
#define MPLS_LABEL_ROUTER_ALERT_EXP  0
#define MPLS_LABEL_ROUTER_ALERT_BOS  0

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
 * Global and Static
 * {
 */


/* *INDENT-OFF* */

/* *INDENT-ON* */
/*
 * }
 */

/**
 * \brief - This function initializes a few registers and 
 *        memories that are necessary for BFD functionality.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * Registers and memories initialized in this function:
 *  
 *   The following registers are grouped together in a dbal
 *   virtual table, BFD_GENERAL_CONFIGURATION:\n
 *       a. IPPB_OAM_IDENTIFICATION_ENABLE\n
 *       b. IPPB_OAM_BFD_YOUR_DISCR_TO_LIF_CFG\n
 *       c. IPPB_IS_LAYER_PROTOCOL_BFD_ONE_HOP_VAL\n
 *       d. IPPB_IS_LAYER_PROTOCOL_BFD_M_HOP_VAL\n
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_init_general_configuration(
    int unit)
{
    uint32 entry_handle_id, max_lif_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Access dbal virtual table BFD_GENERAL_CONFIGURATION */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BFD_GENERAL_CONFIGURATION, &entry_handle_id));

    /** Set initial values   */

    /** 
     *  If the "your discriminator" field in the BFD packet is
     *  invalid, use the maximum LIF ID for the resulting LIF
     */
    max_lif_id = dnx_data_lif.global_lif.nof_global_in_lifs_get(unit) - 1;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_YOUR_DISC_LIF_INVALID, INST_SINGLE, max_lif_id);

    /** 
     *  If the "your discriminator" field in the BFD packet is
     *  NULL, use this value for the LIF
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_YOUR_DISC_LIF_NULL,
                                 INST_SINGLE, BFD_NUL_YR_DISC_LIF);

    /** Access table   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_OAM, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_BFDV6_CHECKSUM_ERROR_TRAP_CODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6_UDP_CHECKSUM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_BFDV6_TRAP_TO_OAMP_TRAP_CODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6);
    /*
     * Used in the "do not edit" forward context ucode. IP TTL will be taken from OAMP, if the next pointer is MPLS as
     * opposed to L2, its TTL will be taken from this guy 
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_BFD_OVER_IP_INJECTION_TTL_VALUE_FOR_INHERITANCE,
                                 INST_SINGLE, 255);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes the BFD flags profile 
 *        table.  Since there are enough profiles to contain all
 *        possible legitimate values, there is no need to write
 *        to this table after it is initialized.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_oamp_init_flags_profile(
    int unit)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Access dbal virtual table OAMP_BFD_FLAGS_PROFILE */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_FLAGS_PROFILE, &entry_handle_id));

    for (index = 0; index < NOF_FLAGS_PROFILES; index++)
    {
        /** Set key   */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, index);

        /** Set value   */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLAGS_VALUE,
                                     INST_SINGLE, BFD_FLAGS_PROFILE_TO_VAL(index));

        /** Access table   */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes the BFD diag profile 
 *        table.  Since there are enough profiles to contain all
 *        possible legitimate values, there is no need to write
 *        to this table after it is initialized.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_oamp_init_diag_profile(
    int unit)
{
    uint32 entry_handle_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Access dbal virtual table OAMP_BFD_DIAG_PROFILE */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_BFD_DIAG_PROFILE, &entry_handle_id));

    for (index = 0; index < NOF_DIAG_PROFILES; index++)
    {
        /** Set key   */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, index);

        /** Set value   */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DIAG_VALUE, INST_SINGLE, index);

        /** Access table   */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - TX rate must be 0 when unused.
 * Otherwise, if creating an endpoint with period 0 and using a new BFD-TX-RATE profile
 * there is a small chance that a packet will be incorrectly transmitted by the OAMP.
 * This is because the tx-rate value is "cached" by the OAMP for 1.67ms.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_bfd_oamp_init_tx_rate_profile(
    int unit)
{
    int index, nof_profiles = dnx_data_bfd.general.nof_profiles_for_tx_rate_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (index = 0; index < nof_profiles; index++)
    {
        SHR_IF_ERR_EXIT(dnx_bfd_endpoint_db_write_tx_rate_to_oamp_hw(unit, index, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_bfd_oamp_init_router_alert(
    int unit)
{
    uint32 entry_handle_id;
    uint32 mpls_label, ttl, exp, bos;
    uint32 router_alert_header = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Access dbal virtual table OAMP_INIT_BFD_CFG */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_BFD_CFG, &entry_handle_id));

    /** Build router alert header */
    mpls_label = MPLS_LABEL_ROUTER_ALERT;
    ttl = MPLS_LABEL_ROUTER_ALERT_TTL;
    exp = MPLS_LABEL_ROUTER_ALERT_EXP;
    bos = MPLS_LABEL_ROUTER_ALERT_BOS;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PWE_ROUTER_ALERT, DBAL_FIELD_BOS, &bos, &router_alert_header));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PWE_ROUTER_ALERT, DBAL_FIELD_MPLS_LABEL, &mpls_label, &router_alert_header));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PWE_ROUTER_ALERT, DBAL_FIELD_EXP, &exp, &router_alert_header));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_PWE_ROUTER_ALERT, DBAL_FIELD_TTL, &ttl, &router_alert_header));

    /** Set value   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PWE_ROUTER_ALERT, INST_SINGLE, router_alert_header);

    /** Access table   */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_bfd_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize various BFD relevant settings */
    SHR_IF_ERR_EXIT(dnx_bfd_init_general_configuration(unit));
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_init_flags_profile(unit));
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_init_diag_profile(unit));
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_init_router_alert(unit));
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_init_tx_rate_profile(unit));
    /*
     * set the default channel type value for MPLS TP VCCV type BFD
     */
    SHR_IF_ERR_EXIT(dnx_bfd_vccv_channel_type_set(unit));

exit:
    SHR_FUNC_EXIT;
}
