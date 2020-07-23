/** \file oam_classifier_utils.c
 * 
 *
 * OAM classifier OEM tables lookup procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/*
 *  * Include files. * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
/*
 *  }
 */

/*
 *  DEFINEs {
 */
/** Prefixes (3 bits) to be used in accessing the relevant tables */
#define OAM_LIF_DB_PREF     0
#define OAM_ACC_MEP_DB_PREF 0
#define BFD_ACC_MEP_DB_PREF 1
/*
 *  }
 */

/*
 *  MACROs {
 */
/** Make sure all bits in a flags argument are within the legal set of flag bits */
#define CHECK_LEGAL_FLAGS(checked_flags, legal_flags) ((checked_flags) & ~(legal_flags))
/*
 *  }
 */

/*
 *  * Global and Static
 */
/*
 *  }
 */

/*
 * Argument verification that is the same for dnx_oam_lif_get and dnx_oam_lif_add
 */
static shr_error_e
dnx_oam_lif_verify(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key)
{
    uint32 legal_flags, max_lif_bits, max_lif_id;
    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(oam_lif_entry_key, _SHR_E_PARAM, "oam_lif_entry_key");

    /** Currenly, only one flag is defined */
    legal_flags = DNX_OAM_CLASSIFIER_INGRESS;

    /** Apply an inverted mask to find illegal flags */
    if (CHECK_LEGAL_FLAGS(oam_lif_entry_key->flags, legal_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination 0x%08x\n", oam_lif_entry_key->flags);
    }

    /** Get number of cores for this device */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);

    if ((oam_lif_entry_key->core_id >= num_of_cores) ||
        ((oam_lif_entry_key->core_id < 0) && (oam_lif_entry_key->core_id != _SHR_CORE_ALL)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for core_id %d, total number of cores %d\n",
                     oam_lif_entry_key->core_id, num_of_cores);
    }

    /** Get maximum allowed ID of inlifs/outlifs for this device */
    if (oam_lif_entry_key->flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        max_lif_bits = dnx_data_oam.general.oam_nof_bits_oam_lif_db_key_base_ingress_get(unit);
    }
    else
    {
        max_lif_bits = dnx_data_oam.general.oam_nof_bits_oam_lif_db_key_base_egress_get(unit);
    }
    max_lif_id = SAL_UPTO_BIT(max_lif_bits);

    /** Use that number to verify the parameter   */
    if (oam_lif_entry_key->oam_lif_id > max_lif_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for oam_lif_id 0x%08x, must be between 0 and 0x%08x\n",
                     oam_lif_entry_key->oam_lif_id, max_lif_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification unique to dnx_oam_lif_get
 */
static shr_error_e
dnx_oam_lif_get_verify(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    const dnx_oam_lif_entry_value_t * oam_lif_entry_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_lif_verify(unit, oam_lif_entry_key));
    SHR_NULL_CHECK(oam_lif_entry_value, _SHR_E_PARAM, "oam_lif_entry_value");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_lif_get(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    dnx_oam_lif_entry_value_t * oam_lif_entry_value)
{
    int index;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_lif_get_verify(unit, oam_lif_entry_key, oam_lif_entry_value));

    /** Get table */
    if (oam_lif_entry_key->flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_LIF_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_LIF_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, oam_lif_entry_key->oam_key_prefix);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, oam_lif_entry_key->oam_lif_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, oam_lif_entry_key->core_id);

    /** Value fields */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        /** Loop over all possible levels defined in standard: 0-7 */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE,
                                   index, &oam_lif_entry_value->mdl_mp_type[index]);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE,
                               INST_SINGLE, &oam_lif_entry_value->mp_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE,
                               INST_SINGLE, &oam_lif_entry_value->counter_base);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE,
                              INST_SINGLE, &oam_lif_entry_value->counter_interface);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
 */
shr_error_e
dnx_oam_lif_modify_mp_profile(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_db_key,
    int mp_profile)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Select Ing/Eg OAM_LIF_DB table */
    if (oam_lif_db_key->flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_LIF_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_LIF_DB;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, oam_lif_db_key->oam_lif_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, oam_lif_db_key->oam_key_prefix);

    /** Set value mp_profile field only */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, INST_SINGLE, mp_profile);

    /** Access the table - Update entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_mp_type_get(
    int unit,
    uint32 is_ingress,
    uint32 nof_mep_above_mdl,
    uint32 mdl_mp_type,
    uint32 nof_mep_below_mdl,
    uint32 packet_is_bfd,
    uint32 mip_above_mdl,
    uint32 *mp_type)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    if (is_ingress)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_MP_TYPE_MAP;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_MP_TYPE_MAP;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MIP_ABOVE_MDL, mip_above_mdl);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_MEP_BELOW_MDL, nof_mep_below_mdl);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_BFD, packet_is_bfd);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE, mdl_mp_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NOF_MEP_ABOVE_MDL, nof_mep_above_mdl);

    /** Value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, INST_SINGLE, mp_type);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description in oam_internal.h
 */
shr_error_e
dnx_oam_opcode_map_get(
    int unit,
    uint32 is_ingress,
    uint8 opcode,
    dbal_enum_value_field_oam_internal_opcode_e * internal_opcode)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table */
    if (is_ingress)
    {
        table_id = DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, opcode);

    /** Value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE, internal_opcode);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Argument verification unique to dnx_oam_acc_mep_add
 */
static shr_error_e
dnx_oam_lif_add_verify(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    const dnx_oam_lif_entry_value_t * oam_lif_entry_value)
{
    int index;
    uint32 nof_bits_counter_base, max_counter_base;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_lif_verify(unit, oam_lif_entry_key));
    SHR_NULL_CHECK(oam_lif_entry_value, _SHR_E_PARAM, "oam_lif_entry_value");

    /** Get value of DNX Data */
    nof_bits_counter_base = dnx_data_oam.general.oam_nof_bits_counter_base_get(unit);
    /** Get the maximum allowed Counter Base for this device */
    max_counter_base = SAL_UPTO_BIT(nof_bits_counter_base);

    /** Array has an MP type (none, MIP, active/passive MEP for
     *  each possible MDL (0-7) and each value is verified */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        /** Each MP type must be a value from this enum   */
        if (oam_lif_entry_value->mdl_mp_type[index] >= DBAL_NOF_ENUM_MDL_MP_TYPE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for mdl_mp_type[%d] 0x%04x, must be between 0 and 0x%04x\n",
                         index, oam_lif_entry_value->mdl_mp_type[index], DBAL_NOF_ENUM_MDL_MP_TYPE_VALUES - 1);
        }
    }
    if (oam_lif_entry_value->mp_profile > dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for acc_mp_profile %d, must be between 0 and %d\n",
                     oam_lif_entry_value->mp_profile, dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit));
    }
    if (oam_lif_entry_value->counter_base > max_counter_base)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for counter_base 0x%04x, must be between 0 and 0x%04x\n",
                     oam_lif_entry_value->counter_base, max_counter_base);
    }
    /*
     * Invalid Counter_base is represented as 3
     */
    if (oam_lif_entry_value->counter_interface > DBAL_NOF_ENUM_OAM_LM_COUNTER_READ_IDX_SEL_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for counter_interface %d, must be between 0 and %d\n",
                     oam_lif_entry_value->counter_interface, DBAL_NOF_ENUM_OAM_LM_COUNTER_READ_IDX_SEL_VALUES - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_oam_lif_modify_mdl_mp_type(
    int unit,
    int level,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    const dnx_oam_lif_entry_value_t * oam_lif_entry_value)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
   /** Get table */
    if (oam_lif_entry_key->flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_LIF_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_LIF_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

   /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, oam_lif_entry_key->oam_key_prefix);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, oam_lif_entry_key->oam_lif_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, oam_lif_entry_key->core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE,
                                 level, oam_lif_entry_value->mdl_mp_type[level]);

   /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_oam_lif_add(
    int unit,
    dbal_entry_action_flags_e dbal_oper_flags,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    const dnx_oam_lif_entry_value_t * oam_lif_entry_value)
{
    int index;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_lif_add_verify(unit, oam_lif_entry_key, oam_lif_entry_value));

    /** Get table */
    if (oam_lif_entry_key->flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_LIF_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_LIF_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, oam_lif_entry_key->oam_key_prefix);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, oam_lif_entry_key->oam_lif_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, oam_lif_entry_key->core_id);

    /** Value fields */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        /** Set each value in the array */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE,
                                     index, oam_lif_entry_value->mdl_mp_type[index]);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE,
                                 INST_SINGLE, oam_lif_entry_value->mp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE,
                                 INST_SINGLE, oam_lif_entry_value->counter_base);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE,
                                 INST_SINGLE, oam_lif_entry_value->counter_interface);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_oper_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See oam_internal.h for information */
shr_error_e
dnx_oam_lif_delete(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_lif_verify(unit, oam_lif_entry_key));

    /** Get table */
    if (oam_lif_entry_key->flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_LIF_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_LIF_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_PREFIX, oam_lif_entry_key->oam_key_prefix);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_KEY_BASE, oam_lif_entry_key->oam_lif_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, oam_lif_entry_key->core_id);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Argument verification that is the same for dnx_oam_acc_mep_get and dnx_oam_acc_mep_add
 */
static shr_error_e
dnx_acc_mep_verify(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl)
{
    uint32 legal_flags, max_lif_bits, max_lif_id;
    int num_of_cores;
    SHR_FUNC_INIT_VARS(unit);

    /** Currenly, only one flag is defined */
    legal_flags = DNX_OAM_CLASSIFIER_INGRESS | DNX_OAM_CLASSIFIER_UPDATE | DNX_OAM_CLASSIFIER_BFD;

    /** Get number of cores for this device */
    num_of_cores = dnx_data_device.general.nof_cores_get(unit);

    /** Get maximum allowed ID of inlifs/outlifs for this device */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        max_lif_bits = dnx_data_oam.general.oam_nof_bits_oam_lif_db_key_base_ingress_get(unit);
    }
    else
    {
        max_lif_bits = dnx_data_oam.general.oam_nof_bits_oam_lif_db_key_base_egress_get(unit);
    }
    max_lif_id = SAL_UPTO_BIT(max_lif_bits);

    if ((core_id >= num_of_cores) || ((core_id < 0) && (core_id != _SHR_CORE_ALL)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for core_id %d, total number of cores %d\n", core_id, num_of_cores);
    }

    if ((oam_lif < 0) || (oam_lif > max_lif_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for oam_lif 0x%08x, must be between 0 and 0x%08x\n",
                     oam_lif, max_lif_id);
    }

    if (flags & ~legal_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: unsupported flag combination\n");
    }
    if ((mdl < 0) || (mdl > DNX_OAM_MAX_MDL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for mdl %d, must be between 0 and %d\n", mdl, DNX_OAM_MAX_MDL);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Argument verification unique to dnx_oam_acc_mep_get
 */
static shr_error_e
dnx_oam_acc_mep_get_verify(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl,
    uint32 *mep_id,
    uint32 *acc_mp_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_acc_mep_verify(unit, core_id, flags, oam_lif, mdl));
    SHR_NULL_CHECK(mep_id, _SHR_E_PARAM, "mep_id");
    SHR_NULL_CHECK(acc_mp_profile, _SHR_E_PARAM, "acc_mp_profile");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_oam_acc_mep_get(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl,
    uint32 *mep_id,
    uint32 *acc_mp_profile)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_acc_mep_get_verify(unit, core_id, flags, oam_lif, mdl, mep_id, acc_mp_profile));

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_ACC_MEP_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_ACC_MEP_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    if (_SHR_IS_FLAG_SET(flags, DNX_OAM_CLASSIFIER_BFD))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_PREFIX, BFD_ACC_MEP_DB_PREF);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_PREFIX, OAM_ACC_MEP_DB_PREF);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF, oam_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, mdl);
    /** In DPC table, core needs to be well defined in order to perform a get operation */
    if (core_id == DBAL_CORE_ALL)
    {
        core_id = DBAL_CORE_DEFAULT;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACC_MP_PROFILE, INST_SINGLE, acc_mp_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, mep_id);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Argument verification unique to dnx_oam_acc_mep_add
 */
static shr_error_e
dnx_oam_acc_mep_add_verify(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl,
    int mep_id,
    int acc_mp_profile)
{
    uint32 max_mep_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_acc_mep_verify(unit, core_id, flags, oam_lif, mdl));

    max_mep_id = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_endpoint_entries_get(unit)) - 1;
    if ((mep_id < 0) || (mep_id > max_mep_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for mep_id 0x%04x, must be between 0 and 0x%04x\n",
                     mep_id, max_mep_id);
    }
    if ((acc_mp_profile < 0) || (acc_mp_profile > dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal value for acc_mp_profile %d, must be between 0 and %d\n",
                     acc_mp_profile, dnx_data_oam.general.oam_nof_acc_action_profiles_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_oam_acc_mep_add(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl,
    int mep_id,
    int acc_mp_profile)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_entry_action_flags_e dbal_action;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_acc_mep_add_verify(unit, core_id, flags, oam_lif, mdl, mep_id, acc_mp_profile));

    /** Get table */
    if (_SHR_IS_FLAG_SET(flags, DNX_OAM_CLASSIFIER_INGRESS))
    {
        table_id = DBAL_TABLE_INGRESS_OAM_ACC_MEP_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_ACC_MEP_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    if (_SHR_IS_FLAG_SET(flags, DNX_OAM_CLASSIFIER_BFD))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_PREFIX, BFD_ACC_MEP_DB_PREF);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_PREFIX, OAM_ACC_MEP_DB_PREF);
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF, oam_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, mdl);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACC_MP_PROFILE, INST_SINGLE, acc_mp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, mep_id);

    /** New or update? */
    if (_SHR_IS_FLAG_SET(flags, DNX_OAM_CLASSIFIER_UPDATE))
    {
        dbal_action = DBAL_COMMIT_UPDATE;
    }
    else
    {
        dbal_action = DBAL_COMMIT;
    }

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_action));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See oam_internal.h for information */
shr_error_e
dnx_oam_acc_mep_delete(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_acc_mep_verify(unit, core_id, flags, oam_lif, mdl));

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_ACC_MEP_DB;
    }
    else
    {
        table_id = DBAL_TABLE_EGRESS_OAM_ACC_MEP_DB;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    if (_SHR_IS_FLAG_SET(flags, DNX_OAM_CLASSIFIER_BFD))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_PREFIX, BFD_ACC_MEP_DB_PREF);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_PREFIX, OAM_ACC_MEP_DB_PREF);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF, oam_lif);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL, mdl);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Key verification for oam lif action and accelerated
 *          MEP action. To be used by: dnx_oam_action_set() and
 *          dnx_oam_action_get()
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently three in use:\n
 *        DNX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *        (flag), or egress DB lookup (no flag)\n
 *        DNX_OAM_CLASSIFIER_NO_LIF_ACTION: Add or read
 *        entry in the LIF action table (OAMA)\n
 *        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION: Add or read entry
 *        in the accelerated MEP action table (OAMB)
 * \param [in] action_key - Structure that holds the key of the
 *        entry/entries. See internal_oam.h
 *
 * \return shr_error_e - Error indication
 */
static shr_error_e
dnx_oam_action_key_verify(
    int unit,
    int flags,
    const oam_action_key_t * action_key)
{
    SHR_FUNC_INIT_VARS(unit);

    /** There is also an illegal flags combination */
    if ((flags & (DNX_OAM_CLASSIFIER_ACC_MEP_ACTION | DNX_OAM_CLASSIFIER_LIF_ACTION)) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal flags combination.  Either DNX_OAM_CLASSIFIER_LIF_ACTION or "
                     "DNX_OAM_CLASSIFIER_ACC_MEP_ACTION must be on, otherwise No table is accessed.\n");
    }
    if ((flags & (DNX_OAM_CLASSIFIER_ACC_MEP_ACTION | DNX_OAM_CLASSIFIER_LIF_ACTION)) ==
        (DNX_OAM_CLASSIFIER_ACC_MEP_ACTION | DNX_OAM_CLASSIFIER_LIF_ACTION))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal flags combination, using both DNX_OAM_CLASSIFIER_LIF_ACTION and"
                     "DNX_OAM_CLASSIFIER_ACC_MEP_ACTION.  Only one table may be accessed.\n");
    }

    if (action_key->is_inject > MAX_BOOL_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal is_inject value %d. Should be boolean.\n", action_key->is_inject);
    }
    if (action_key->oam_internal_opcode >= DBAL_NOF_ENUM_OAM_INTERNAL_OPCODE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal oam_internal_opcode value %d. Max allowed value: %d\n",
                     action_key->oam_internal_opcode, DBAL_NOF_ENUM_OAM_INTERNAL_OPCODE_VALUES - 1);
    }
    if (action_key->is_my_mac > MAX_BOOL_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal my_mac value %d. Should be boolean.\n", action_key->is_my_mac);
    }
    if (action_key->mp_type >= DBAL_NOF_ENUM_MP_TYPE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal mp_type value %d.Max allowed value: %d\n", action_key->mp_type,
                     DBAL_NOF_ENUM_MP_TYPE_VALUES - 1);
    }
    if (action_key->mp_profile > dnx_data_oam.general.oam_nof_non_acc_action_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal mp_profile value %d. Max allowed value: %d\n", action_key->mp_profile,
                     dnx_data_oam.general.oam_nof_non_acc_action_profiles_get(unit));
    }
    if (action_key->da_is_mc > MAX_BOOL_VALUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal da_is_mc value %d. Should be boolean.\n", action_key->da_is_mc);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Argument verification for oam lif action add function:
 *          dnx_oam_action_set()
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently three in use:\n
 *        DNX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *        (flag), or egress DB lookup (no flag)\n
 *        DNX_OAM_CLASSIFIER_LIF_ACTION: Add an entry
 *        to the LIF action table (OAMA)\n
 *        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION: Add an entry to an
 *        the accelerated MEP action table (OAMB)
 * \param [in] action_content - Structure that holds the action
 *        parameters of the entry. See internal_oam.h
 *
 * \return shr_error_e - Error indication
 */
static shr_error_e
dnx_oam_action_content_verify(
    int unit,
    int flags,
    const oam_action_content_t * action_content)
{
    SHR_FUNC_INIT_VARS(unit);

    if (action_content->forwarding_strength > DNX_OAM_MAX_FORWARDING_STRENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal forwarding strength value %d. Max allowed value: %d\n",
                     action_content->forwarding_strength, DNX_OAM_MAX_FORWARDING_STRENGTH);
    }
    if (action_content->snoop_strength > DNX_OAM_MAX_SNOOP_STRENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal snoop strength value %d. Max allowed value: %d\n",
                     action_content->snoop_strength, DNX_OAM_MAX_SNOOP_STRENGTH);
    }
    if (action_content->oam_sub_type >= DBAL_NOF_ENUM_OAM_SUB_TYPE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal sub-type value %d. Max allowed value: %d\n",
                     action_content->oam_sub_type, DBAL_NOF_ENUM_OAM_SUB_TYPE_VALUES - 1);
    }
    if (((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0) &&
        (action_content->oama_counter_disable_access_0 > MAX_BOOL_VALUE ||
         action_content->oama_counter_disable_access_1 > MAX_BOOL_VALUE ||
         action_content->oama_counter_disable_access_2 > MAX_BOOL_VALUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal oama counter disable value/s. Must be 0 or 1\n");
    }
    if (((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) == 0) && (action_content->oama_counter_disable_access_0 != 0 ||
                                                           action_content->oama_counter_disable_access_1 != 0 ||
                                                           action_content->oama_counter_disable_access_2 != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal oama counter disable value/s. OAMA not accessed. Must be 0.\n");
    }
    if (((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) != 0) && (action_content->oamb_counter_disable > MAX_BOOL_VALUE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal oamb counter disable value %d. Should be boolean.\n",
                     action_content->oamb_counter_disable);
    }
    if (((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) == 0) && (action_content->oamb_counter_disable != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal oamb counter disable value %d. OAMB not accessed. Must be 0.\n",
                     action_content->oamb_counter_disable);
    }
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        if (action_content->table_specific_fields.ingress_only_fields.ingress_trap_code >=
            DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal trap_code value %d. Max allowed value: %d\n",
                         action_content->table_specific_fields.ingress_only_fields.ingress_trap_code,
                         DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES - 1);
        }
        if (action_content->table_specific_fields.ingress_only_fields.is_up_mep > MAX_BOOL_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal is_up_mep value %d. Should be boolean.\n",
                         action_content->table_specific_fields.ingress_only_fields.is_up_mep);
        }
        if (action_content->table_specific_fields.ingress_only_fields.meter_disable > MAX_BOOL_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal meter disable value %d. Should be boolean.\n",
                         action_content->table_specific_fields.ingress_only_fields.meter_disable);
        }
    }
    else
    {
        if (action_content->table_specific_fields.egress_only_fields.egress_action_profile >
            DNX_OAM_MAX_EGRESS_ACTION_PROFILE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal trap_code value %d. Max allowed value: %d\n",
                         action_content->table_specific_fields.egress_only_fields.egress_action_profile,
                         DNX_OAM_MAX_EGRESS_ACTION_PROFILE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See oam_internal.h for information */
shr_error_e
dnx_oam_action_set(
    int unit,
    uint32 flags,
    const oam_action_key_t * action_key,
    const oam_action_content_t * action_content)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_action_key_verify(unit, flags, action_key));
    SHR_INVOKE_VERIFY_DNX(dnx_oam_action_content_verify(unit, flags, action_content));

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        if ((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0)
        {
            table_id = DBAL_TABLE_INGRESS_OAM_LIF_ACTION_DB;
        }
        else
        {
            table_id = DBAL_TABLE_INGRESS_OAM_ACC_MEP_ACTION_DB;
        }
    }
    else
    {
        if ((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0)
        {
            table_id = DBAL_TABLE_EGRESS_OAM_LIF_ACTION_DB;
        }
        else
        {
            table_id = DBAL_TABLE_EGRESS_OAM_ACC_MEP_ACTION_DB;
        }
    }

    /** Get handle to table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, action_key->is_inject);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, action_key->oam_internal_opcode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MY_CFM_MAC, action_key->is_my_mac);
    if ((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) != 0)
    {
        if (flags & DNX_OAM_CLASSIFIER_INGRESS)
        {
            /**
             *  This key element only exists for ingress, no BFD in
             *  egress!
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_BFD,
                                       (DBAL_ENUM_FVAL_MP_TYPE_BFD == action_key->mp_type) ? TRUE : FALSE);
        }
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, action_key->mp_type);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, action_key->mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_DA_IS_MC, action_key->da_is_mc);

    /** Value fields */

    /** Same values for ingress and egress */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE,
                                 action_content->forwarding_strength);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE,
                                 action_content->snoop_strength);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE, INST_SINGLE,
                                 action_content->oam_sub_type);

    /**
     *  Different for LIF and ACC_MEP, but not for ingress and
     *  egress
     */
    if ((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) != 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE,
                                     INST_SINGLE, action_content->oamb_counter_disable);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_0,
                                     INST_SINGLE, action_content->oama_counter_disable_access_0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_1,
                                     INST_SINGLE, action_content->oama_counter_disable_access_1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_2,
                                     INST_SINGLE, action_content->oama_counter_disable_access_2);
    }

    /** Fields that are ingress only, or egress only   */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, INST_SINGLE,
                                     action_content->table_specific_fields.ingress_only_fields.ingress_trap_code);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INGRESS_UP_MEP, INST_SINGLE,
                                     action_content->table_specific_fields.ingress_only_fields.is_up_mep);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INGRESS_METER_DISABLE, INST_SINGLE,
                                     action_content->table_specific_fields.ingress_only_fields.meter_disable);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_TRAP_ACTION_PROFILE, INST_SINGLE,
                                     action_content->table_specific_fields.egress_only_fields.egress_action_profile);
    }

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See oam_internal.h for information */
shr_error_e
dnx_oam_action_get(
    int unit,
    uint32 flags,
    const oam_action_key_t * action_key,
    oam_action_content_t * action_content)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_action_key_verify(unit, flags, action_key));

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        if ((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0)
        {
            table_id = DBAL_TABLE_INGRESS_OAM_LIF_ACTION_DB;
        }
        else
        {
            table_id = DBAL_TABLE_INGRESS_OAM_ACC_MEP_ACTION_DB;
        }
    }
    else
    {
        if ((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0)
        {
            table_id = DBAL_TABLE_EGRESS_OAM_LIF_ACTION_DB;
        }
        else
        {
            table_id = DBAL_TABLE_EGRESS_OAM_ACC_MEP_ACTION_DB;
        }
    }

    /** Get handle to table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, action_key->is_inject);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, action_key->oam_internal_opcode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MY_CFM_MAC, action_key->is_my_mac);
    if ((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) != 0)
    {
        if (flags & DNX_OAM_CLASSIFIER_INGRESS)
        {
            /**
             *  This key element only exists for ingress, no BFD in
             *  egress!
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_BFD,
                                       (DBAL_ENUM_FVAL_MP_TYPE_BFD == action_key->mp_type) ? TRUE : FALSE);
        }
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, action_key->mp_type);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, action_key->mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_DA_IS_MC, action_key->da_is_mc);

    /** Value fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_FWD_STRENGTH, INST_SINGLE,
                               &(action_content->forwarding_strength));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_SNP_STRENGTH, INST_SINGLE,
                               &(action_content->snoop_strength));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE, INST_SINGLE,
                               &(action_content->oam_sub_type));

    /** DIfferent for ingress and egress   */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, INST_SINGLE,
                                   &(action_content->table_specific_fields.ingress_only_fields.ingress_trap_code));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_INGRESS_UP_MEP, INST_SINGLE,
                                   &(action_content->table_specific_fields.ingress_only_fields.is_up_mep));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_INGRESS_METER_DISABLE, INST_SINGLE,
                                   &(action_content->table_specific_fields.ingress_only_fields.meter_disable));
    }
    else
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETPP_TRAP_ACTION_PROFILE, INST_SINGLE,
                                   &(action_content->table_specific_fields.egress_only_fields.egress_action_profile));
    }

    /**
     *  Different for LIF and ACC_MEP, but not for ingress and
     *  egress
     */
    if ((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) != 0)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE,
                                   INST_SINGLE, &(action_content->oamb_counter_disable));
    }
    else
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_0, INST_SINGLE,
                                   &(action_content->oama_counter_disable_access_0));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_1, INST_SINGLE,
                                   &(action_content->oama_counter_disable_access_1));
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_DISABLE_ACCESS_2, INST_SINGLE,
                                   &(action_content->oama_counter_disable_access_2));
    }

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See oam_internal.h for information */
shr_error_e
dnx_oam_action_clear(
    int unit,
    uint32 flags,
    const oam_action_key_t * action_key)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_action_key_verify(unit, flags, action_key));

    /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        if ((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0)
        {
            table_id = DBAL_TABLE_INGRESS_OAM_LIF_ACTION_DB;
        }
        else
        {
            table_id = DBAL_TABLE_INGRESS_OAM_ACC_MEP_ACTION_DB;
        }
    }
    else
    {
        if ((flags & DNX_OAM_CLASSIFIER_LIF_ACTION) != 0)
        {
            table_id = DBAL_TABLE_EGRESS_OAM_LIF_ACTION_DB;
        }
        else
        {
            table_id = DBAL_TABLE_EGRESS_OAM_ACC_MEP_ACTION_DB;
        }
    }

    /** Get handle to table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INJECT, action_key->is_inject);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, action_key->oam_internal_opcode);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MY_CFM_MAC, action_key->is_my_mac);
    if ((flags & DNX_OAM_CLASSIFIER_ACC_MEP_ACTION) != 0)
    {
        if (flags & DNX_OAM_CLASSIFIER_INGRESS)
        {
            /**
             *  This key element only exists for ingress, no BFD in
             *  egress!
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_IS_BFD,
                                       (DBAL_ENUM_FVAL_MP_TYPE_BFD == action_key->mp_type) ? TRUE : FALSE);
        }
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_TYPE, action_key->mp_type);
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, action_key->mp_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_DA_IS_MC, action_key->da_is_mc);

    /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get information about the configured lifs.
 *  Information gathered:
 *   - mp_profile[ingress]
 *   - mp_profile[egress]
 *   - counter_base[level] (same for both ingress and egress)
 *   - counter_if[level] (same for both ingress and egress)
 *   - mep_dir[level] : may be up, down, both, or mip
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] lif_resolution - Structure of lif resolution
 * \param [out] configured_lif - Structure of configured lif
 *        parameters
 *
 * \retval Error indication.
 */
shr_error_e
dnx_oam_lif_configured_lif_get(
    int unit,
    const oam_lif_resolution_t * lif_resolution,
    oam_lif_configured_lif_t * configured_lif)
{
    int index;
    int side;
    dnx_oam_endpoint_info_t dnx_oam_endpoint_info;
    dnx_oam_lif_info_t lif_info[DNX_OAM_CLASSIFIER_BOTH_SIDES];

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Get OAM lif info
     *  Need the SW information from both sides.
 */

    SHR_IF_ERR_EXIT(dnx_oam_lif_info_t_init(unit, &lif_info[DNX_OAM_CLASSIFIER_INGRESS]));
    SHR_IF_ERR_EXIT(dnx_oam_lif_info_t_init(unit, &lif_info[DNX_OAM_CLASSIFIER_EGRESS]));

    if (lif_resolution->local_out_lif != DNX_OAM_INVALID_LIF)
    {
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_oam_sw_state_oam_lif_info_get
                                  (unit, lif_resolution->local_out_lif, DNX_OAM_CLASSIFIER_EGRESS,
                                   &lif_info[DNX_OAM_CLASSIFIER_EGRESS]), _SHR_E_NOT_FOUND);
    }

    if (lif_resolution->global_in_lif != DNX_OAM_INVALID_LIF)
    {
        SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_oam_sw_state_oam_lif_info_get
                                  (unit, lif_resolution->global_in_lif, DNX_OAM_CLASSIFIER_INGRESS,
                                   &lif_info[DNX_OAM_CLASSIFIER_INGRESS]), _SHR_E_NOT_FOUND);
    }

    configured_lif->mp_profile[DNX_OAM_CLASSIFIER_EGRESS] = lif_info[DNX_OAM_CLASSIFIER_EGRESS].mp_profile;
    configured_lif->mp_profile[DNX_OAM_CLASSIFIER_INGRESS] = lif_info[DNX_OAM_CLASSIFIER_INGRESS].mp_profile;

    /*
     *  Get current endpoints info (before API call) for all MDL levels.
     */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        /*
         * Default values for levels without any MEP
         */
        configured_lif->counter_base[index] = 0;
        configured_lif->counter_if[index] = 0;
        configured_lif->mep_dir[index] = NO_MEP;
        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] = DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] = DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;

        for (side = DNX_OAM_CLASSIFIER_EGRESS; side <= DNX_OAM_CLASSIFIER_INGRESS; side++)
        {
            if (lif_info[side].endpoint_id[index] != BCM_OAM_ENDPOINT_INVALID)
            {
                SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get
                                (unit, lif_info[side].endpoint_id[index], &dnx_oam_endpoint_info));

                configured_lif->counter_base[index] = dnx_oam_endpoint_info.lm_counter_idx;
                configured_lif->counter_if[index] = dnx_oam_endpoint_info.lm_counter_if;

                if (dnx_oam_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_UP_FACING)
                {
                    /*
                     * UpMEP => the type is Ethernet.
                     */
                    if (configured_lif->mep_dir[index] == MEP_DIRECTION_IS_DOWN)
                    {
                        configured_lif->mep_dir[index] = MEP_DIRECTION_IS_BOTH_DIRECTIONS;
                        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] =
                            DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] =
                            DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                    }
                    else
                    {
                        configured_lif->mep_dir[index] = MEP_DIRECTION_IS_UP;
                        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] =
                            DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP;
                        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] =
                            DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                    }
                }
                else
                {
                    if (dnx_oam_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_INTERMEDIATE)
                    {
                        configured_lif->mep_dir[index] = MEP_IS_MIP;
                        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] = DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP;
                        configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] = DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP;
                    }
                    else
                    {
                        if (configured_lif->mep_dir[index] == MEP_DIRECTION_IS_UP)
                        {
                            configured_lif->mep_dir[index] = MEP_DIRECTION_IS_BOTH_DIRECTIONS;
                            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] =
                                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                            configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] =
                                DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                        }
                        else
                        {
                            configured_lif->mep_dir[index] = MEP_DIRECTION_IS_DOWN;
                            if (dnx_oam_endpoint_info.type != bcmOAMEndpointTypeEthernet
                                && dnx_oam_endpoint_info.gport == BCM_GPORT_INVALID)
                            {
                                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] =
                                    DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
                            }
                            else
                            {
                                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][index] =
                                    DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP;
                            }

                            if (dnx_oam_endpoint_info.type != bcmOAMEndpointTypeEthernet
                                && dnx_oam_endpoint_info.mpls_out_gport == BCM_GPORT_INVALID)
                            {
                                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] =
                                    DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP;
                            }
                            else
                            {
                                configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][index] =
                                    DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP;
                            }
                        }
                    }
                }
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read default endpoint from hw
 *          tables
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Allowed flags:
 *     DNX_OAM_CLASSIFIER_INGRESS
 *     DNX_OAM_CLASSIFIER_EGRESS
 *     DNX_OAM_NO_FLAGS
 *     DNX_OAM_EXCEPT_NOT_FOUND
 *
 * \param [in] oam_default_profile - Default Profile ID
 * \param [out] default_mep_info - Default MEP info to be
 *        returned
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_default_endpoint_get(
    int unit,
    int flags,
    const uint8 oam_default_profile,
    oam_default_mep_info_t * default_mep_info)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    int index;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

   /** Get table */
    if ((flags & DNX_OAM_CLASSIFIER_INGRESS) == DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_DEFAULT_PAYLOAD;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Egress default endpoint is not supported ");
    }

   /** Get handle to table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PROFILE_ID, oam_default_profile);

   /** Value fields */

   /** Same values for ingress and egress */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
        /** Loop over all possible levels defined in standard: 0-7 */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE,
                                   index, &default_mep_info->mdl_mp_type[index]);
    }

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, INST_SINGLE,
                               &default_mep_info->mp_profile);
   /** Access the table */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if ((flags & DNX_OAM_EXCEPT_NOT_FOUND) == DNX_OAM_EXCEPT_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See oam_internal.h
 */
shr_error_e
dnx_oam_default_mep_profile_get(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_default_mep_info_t * default_mep_info)
{

    uint8 oam_default_profile;
    uint32 flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (endpoint_info->id)
    {
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS0:
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS0:
            oam_default_profile = DNX_OAM_DEFAULT_PROFILE_0;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS1:
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS1:
            oam_default_profile = DNX_OAM_DEFAULT_PROFILE_1;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS2:
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS2:
            oam_default_profile = DNX_OAM_DEFAULT_PROFILE_2;
            break;
        case BCM_OAM_ENDPOINT_DEFAULT_INGRESS3:
        case BCM_OAM_ENDPOINT_DEFAULT_EGRESS3:
            oam_default_profile = DNX_OAM_DEFAULT_PROFILE_3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Unsupported Default MEP Profile ID 0x%x\n", endpoint_info->id);
    }
    default_mep_info->oam_default_mep = oam_default_profile;
    default_mep_info->is_upmep = DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info);
    flags = DNX_OAM_EXCEPT_NOT_FOUND;
    if (default_mep_info->is_upmep)
    {
        flags |= DNX_OAM_CLASSIFIER_EGRESS;
    }
    else
    {
        flags |= DNX_OAM_CLASSIFIER_INGRESS;
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_oam_default_endpoint_get
                              (unit, flags, oam_default_profile, default_mep_info), _SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add all required entries to oam default profile hw
 *          table
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - Number of hardware unit used.
 * \param [in] oam_default_profile - Default oam profile ID
 *        (0-3)
 * \param [in] mp_profile - mp profile to be set for the given
 *        oam_default_profile
 * \param [out] mdl_mp_type - mdl_mp_type to be set for the
 *        given oam_default_profile
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_default_endpoint_update(
    int unit,
    int flags,
    const uint8 oam_default_profile,
    const uint8 mp_profile,
    const dbal_enum_value_field_mdl_mp_type_e * mdl_mp_type)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    int index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

   /** Get table */
    if (flags & DNX_OAM_CLASSIFIER_INGRESS)
    {
        table_id = DBAL_TABLE_INGRESS_OAM_DEFAULT_PAYLOAD;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Egress default endpoint is not supported ");
    }

   /** Get handle to table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PROFILE_ID, oam_default_profile);

   /** Value fields */
    for (index = 0; index <= DNX_OAM_MAX_MDL; index++)
    {
       /** Set each value in the array */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDL_MP_TYPE, index, mdl_mp_type[index]);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MP_PROFILE, INST_SINGLE, mp_profile);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_BASE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_INTERFACE, INST_SINGLE, 0);
   /** Access the table */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Analyze the configured_lif structure to prepare it for
 * hlm_by_mdl verification and oam_lif_db (hw) management.
 *
 *  Following information is resolved for later use in endpoint_create:
 *    - configured_lif including the new MEP:
 *      - mep_dir[level] (new endpoint added)
 *      - counter base[level] (new endpoint added)
 *      - meps_by_level[index] - array of the mep-levels on lif, in ascending order
 *      - meps_level_is_lm[index] - is the mep lm-mep ?
 *      - lm_mep_levels[index] - array of the lm_mep-levels on
 *        lif, in ascending order
 *      - nof_meps
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif.
 * \param [out] lif_analysis - results of analysis performed on
 *        the lifs
 *
 * \retval Error indication.
 */
shr_error_e
dnx_oam_analyze_configured_lif(
    int unit,
    const oam_lif_configured_lif_t * configured_lif,
    oam_lif_analysis_results_t * lif_analysis)
{
    int level;
    int index;
    int prefix;
    int highest_level_counter_prefix;
    int total_meps;
    int total_lm_meps;
    int side;
    int ingress_occupied_prefixes;
    int egress_occupied_prefixes;
    uint8 mep_levels[DNX_OAM_CLASSIFIER_BOTH_SIDES][DNX_OAM_MAX_MDL + 1];

    /*
     *  Scan the MEPs on the LIF
     */
    total_lm_meps = 0;
    total_meps = 0;
    lif_analysis->nof_levels_with_mip = 0;
    ingress_occupied_prefixes = 0;
    egress_occupied_prefixes = 0;
    lif_analysis->lif_db.occupied_prefixes[DNX_OAM_CLASSIFIER_INGRESS] = 0;
    lif_analysis->lif_db.occupied_prefixes[DNX_OAM_CLASSIFIER_EGRESS] = 0;
    for (level = 0; level <= DNX_OAM_MAX_MDL; level++)
    {
        if (configured_lif->mep_dir[level] != NO_MEP)
        {
            lif_analysis->meps_by_level[total_meps] = level;
            if (configured_lif->mep_dir[level] == MEP_IS_MIP)
            {
                lif_analysis->nof_levels_with_mip++;
            }
            if (configured_lif->counter_base[level] > 0)
            {
                lif_analysis->lm_meps_by_level[total_lm_meps++] = level;
                lif_analysis->level_is_lm[total_meps] = 1;
            }
            else
            {
                lif_analysis->level_is_lm[total_meps] = 0;
            }
            if ((configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][level] ==
                 DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP)
                || (configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_INGRESS][level] ==
                    DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP))
            {
                mep_levels[DNX_OAM_CLASSIFIER_INGRESS][ingress_occupied_prefixes] = level;
                ingress_occupied_prefixes++;
            }
            if ((configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][level] == DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP)
                || (configured_lif->mdl_mp_type[DNX_OAM_CLASSIFIER_EGRESS][level] ==
                    DBAL_ENUM_FVAL_MDL_MP_TYPE_PASSIVE_MEP))
            {
                mep_levels[DNX_OAM_CLASSIFIER_EGRESS][egress_occupied_prefixes] = level;
                egress_occupied_prefixes++;
            }
            total_meps++;
        }
    }
    lif_analysis->nof_levels_with_mep = total_meps;
    lif_analysis->nof_levels_with_lm_mep = total_lm_meps;
    lif_analysis->lif_db.occupied_prefixes[DNX_OAM_CLASSIFIER_INGRESS] = ingress_occupied_prefixes;
    lif_analysis->lif_db.occupied_prefixes[DNX_OAM_CLASSIFIER_EGRESS] = egress_occupied_prefixes;

    /*
     * lif_analysis->lif_db.occupied_prefixes[side] is the number of meps to be represented by
     * the 3 accesses of ingress/egress lif-db.
     * MIPs will not be represented in lif-db unless there are only MIPs (without MEPs).
     * In that case a single entry should be added to lif-db.
     */
    for (side = DNX_OAM_CLASSIFIER_EGRESS; side <= DNX_OAM_CLASSIFIER_INGRESS; side++)
    {
        if (lif_analysis->nof_levels_with_mep == lif_analysis->nof_levels_with_mip)
        {
            if (lif_analysis->nof_levels_with_mep > 0)
            {
                /*
                 * MIPs only - Single entry is needed
                 */
                lif_analysis->lif_db.counter_base_in_prefix[side][PREFIX_HIGH] = 0;
                lif_analysis->lif_db.counter_if_in_prefix[side][PREFIX_HIGH] = 0;
                lif_analysis->lif_db.occupied_prefixes[side] = 1;
            }
            else
            {
                /*
                 * No meps on lif
                 */
                lif_analysis->lif_db.occupied_prefixes[side] = 0;
            }
        }
        else
        {
            /*
             * Fill counters in counter_base_in_prefix[side][i], counter_if_in_prefix[side][i]
             */
            if (lif_analysis->lif_db.occupied_prefixes[side] < 4)
            {
                prefix = 0;
                for (index = lif_analysis->lif_db.occupied_prefixes[side] - 1; index >= 0; index--)
                {
                    lif_analysis->lif_db.counter_base_in_prefix[side][prefix] =
                        configured_lif->counter_base[mep_levels[side][index]];
                    lif_analysis->lif_db.counter_if_in_prefix[side][prefix] =
                        configured_lif->counter_if[mep_levels[side][index]];
                    prefix++;
                }
            }
            else
            {
                /*
                 * Highest level and 2 lowest level MEPs should be in lif-db
                 */
                lif_analysis->lif_db.counter_base_in_prefix[side][0] =
                    configured_lif->counter_base[mep_levels[side][lif_analysis->lif_db.occupied_prefixes[side] - 1]];
                lif_analysis->lif_db.counter_if_in_prefix[side][0] =
                    configured_lif->counter_if[mep_levels[side][lif_analysis->lif_db.occupied_prefixes[side] - 1]];
                lif_analysis->lif_db.counter_base_in_prefix[side][1] =
                    configured_lif->counter_base[mep_levels[side][1]];
                lif_analysis->lif_db.counter_if_in_prefix[side][1] = configured_lif->counter_if[mep_levels[side][1]];
                lif_analysis->lif_db.counter_base_in_prefix[side][2] =
                    configured_lif->counter_base[mep_levels[side][0]];
                lif_analysis->lif_db.counter_if_in_prefix[side][2] = configured_lif->counter_if[mep_levels[side][0]];

                if (lif_analysis->lif_db.occupied_prefixes[side] > NOF_OAM_PREFIXES)
                {
                    lif_analysis->lif_db.occupied_prefixes[side] = NOF_OAM_PREFIXES;
                }
            }

            /*
             * HLM by LIF support
             * ------------------
             * PREFIX_HIGH should have the counter of the highest level
             * MEP. Therefore highest_level_counter is duplicated.
             *
             * Copy counters to higher levels for hlm by lif.
             * ( HLM by LIF requires that if counter is allocated on
             * any level on lif it must be copied also to PREFIX_HIGH.
             * This requirement doesn't compromise HLM by MDL operation)
             */
            highest_level_counter_prefix = 0;
            while ((highest_level_counter_prefix < NOF_OAM_PREFIXES) &&
                   (lif_analysis->lif_db.counter_base_in_prefix[side][highest_level_counter_prefix] == 0))
            {
                highest_level_counter_prefix++;
            }
            if (highest_level_counter_prefix < NOF_OAM_PREFIXES)
            {
                for (index = 0; index < highest_level_counter_prefix; index++)
                {
                    lif_analysis->lif_db.counter_base_in_prefix[side][index] =
                        lif_analysis->lif_db.counter_base_in_prefix[side][highest_level_counter_prefix];
                    lif_analysis->lif_db.counter_if_in_prefix[side][index] =
                        lif_analysis->lif_db.counter_if_in_prefix[side][highest_level_counter_prefix];
                }
            }
        }
    }

    return BCM_E_NONE;
}
