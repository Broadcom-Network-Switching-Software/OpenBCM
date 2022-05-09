/** \file bfd_init.c
 * 
 *
 * BFD init procedures for DNX.
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_BFD

 /*
  * Include files.
  * {
  */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <include/bcm_int/dnx/algo/oam/algo_oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <src/bcm/dnx/oam/bfd_oamp.h>
/* { */
#include <src/bcm/dnx/oam/oamp_v1/bfd_oamp_v1.h>
/* } */
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

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_PEMLA_OAM, entry_handle_id));
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_BFDV6_CHECKSUM_ERROR_TRAP_CODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6_UDP_CHECKSUM);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CFG_BFDV6_TRAP_TO_OAMP_TRAP_CODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6);
    }

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

    /** Set the default channel type value for MPLS TP VCCV type BFD */
    SHR_IF_ERR_EXIT(dnx_bfd_oamp_vccv_channel_type_set(unit));

    if (DNX_OAMP_IS_V1(unit))
    {
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_v1_init_flags_profile(unit));
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_v1_init_diag_profile(unit));
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_v1_init_router_alert(unit));
        SHR_IF_ERR_EXIT(dnx_bfd_oamp_v1_init_tx_rate_profile(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
