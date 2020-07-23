/*
 ** \file port_esem.c $Id$ Per PORT ESEM command mgmt procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bslenum.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/algo/template_mngr/smart_template.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/switch/switch_svtag.h>
#include <bcm_int/dnx/field/field.h>
/*
 * }
 */

/** Simplify the definitions: dbal_enum_value_field_esem_app_db_id_e */
#define ESEM_APP_DB_ID(KEY_TYPE)     DBAL_ENUM_FVAL_ESEM_APP_DB_ID_##KEY_TYPE
/** Simplify the definitions: dbal_enum_value_field_esem_offset_e */
#define ESEM_ENTRY_OFFSET(OFFSET)    DBAL_ENUM_FVAL_ESEM_OFFSET_##OFFSET

#define ESEM_DEFAULT_RESULT_TYPE_AC         (128)
#define ESEM_DEFAULT_RESULT_TYPE_NATIVE_AC  (ESEM_DEFAULT_RESULT_TYPE_AC + 1)
#define ESEM_DEFAULT_RESULT_TYPE_DUAL_HOME  (ESEM_DEFAULT_RESULT_TYPE_AC + 2)
#define ESEM_DEFAULT_RESULT_TYPE_SFLOW      (ESEM_DEFAULT_RESULT_TYPE_AC + 3)

#define ESEM_DEFAULT_RESULT_PROFILE_NULL    (dnx_data_esem.default_result_profile.default_dual_homing_get(unit))

#define DNX_PORT_ESEM_CMD_IS_DEFAULT(esem_cmd) \
    ((esem_cmd == dnx_data_esem.access_cmd.default_ac_get(unit)) ||  \
     (esem_cmd == dnx_data_esem.access_cmd.default_native_get(unit)) || \
     (esem_cmd == dnx_data_esem.access_cmd.no_action_get(unit)))

typedef struct dnx_port_esem_cmds_basic_s
{
    /*
     * ESEM command access type
     */
    dnx_esem_access_type_t access_type;
    /*
     * ESEM command data
     */
    dnx_esem_cmd_data_t esem_cmd_data;
} dnx_port_esem_cmds_basic_t;


/**INDENT-OFF* */
/*
 * Global and Static
 * {
 */
static const dnx_port_esem_cmds_basic_t  dnx_port_esem_cmds_basic[] = {
    /** Access type*/
    {ESEM_ACCESS_TYPE_NONE,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 0,    0,                                    0,                                   0},
        { 0,    0,                                    0,                                   0},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_ETH_AC,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 1,    ESEM_APP_DB_ID(FODO_NAMESPACE_CTAG),  DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE,    ESEM_DEFAULT_RESULT_TYPE_AC},
        { 0,    0,                                    0,                                   0},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_PORT_SA,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 1,    ESEM_APP_DB_ID(SSP_NAMESPACE),        DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE,    DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID},
        { 0,    0,                                    0,                                   0},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_ETH_NATIVE_AC,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 0,    0,                                    0,                                   0},
        { 1,    ESEM_APP_DB_ID(FODO_OUTLIF),          DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE,    ESEM_DEFAULT_RESULT_TYPE_NATIVE_AC},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_DUAL_HOME,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 1,    ESEM_APP_DB_ID(SSP_NAMESPACE),        DBAL_ENUM_FVAL_ESEM_OFFSET_BELOW,    ESEM_DEFAULT_RESULT_TYPE_DUAL_HOME},
        { 0,    0,                                    0,                                   0},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_EVPN,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 1,    ESEM_APP_DB_ID(SSP_NAMESPACE),        DBAL_ENUM_FVAL_ESEM_OFFSET_BELOW,    ESEM_DEFAULT_RESULT_TYPE_DUAL_HOME},
        { 1,    ESEM_APP_DB_ID(FODO_OUTLIF),          DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE,    ESEM_DEFAULT_RESULT_TYPE_NATIVE_AC},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_VXLAN_VNI,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 0,    0,                                    0,                                   0},
        { 1,    ESEM_APP_DB_ID(FODO_NAMESPACE_CTAG),  DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE,    DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID},
        { 0,    0,                                    0,                                   0}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_SVTAG,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 0,    0,                                    0,                                   0},
        { 0,    0,                                    0,                                   0},
        { 1,    ESEM_APP_DB_ID(OUT_LIF),              DBAL_ENUM_FVAL_ESEM_OFFSET_LAST,     DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_VXLAN_SVTAG,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 0,    0,                                    0,                                   0},
        { 1,    ESEM_APP_DB_ID(FODO_NAMESPACE_CTAG),  DBAL_ENUM_FVAL_ESEM_OFFSET_ABOVE,    DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID},
        { 1,    ESEM_APP_DB_ID(OUT_LIF),              DBAL_ENUM_FVAL_ESEM_OFFSET_LAST,     DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID}},  0}
    },
    /** Access type*/
    {ESEM_ACCESS_TYPE_SFLOW_SAME_INTERFACE,
     /** valid, app_db_id,                            offsset,                             default_result_profile */
      {{{ 1,    ESEM_APP_DB_ID(PEM_KEY1),             DBAL_ENUM_FVAL_ESEM_OFFSET_BELOW,    ESEM_DEFAULT_RESULT_TYPE_SFLOW},
        { 1,    ESEM_APP_DB_ID(PEM_KEY2),             DBAL_ENUM_FVAL_ESEM_OFFSET_BELOW,    ESEM_DEFAULT_RESULT_TYPE_SFLOW},
        { 0,    0,                                    0,                                   0}},  0}
    }
};

/*
 * }
 */

/**INDENT-ON* */

/**
 * \brief -
 *  Set ESEM access command data to hardware table.
 *  See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_data_set(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t esem_cmd_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to esem-cmd table with the given esem_cmd
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_ACCESS_CMD_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, esem_cmd);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_1_VALID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].valid);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_1_APP_DB_ID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].app_db_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_1_DESIGNATED_OFFSET, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].designated_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_1_DEFAULT_RESULT_PROFILE, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_1].default_result_profile);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_2_VALID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].valid);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_2_APP_DB_ID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].app_db_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_2_DESIGNATED_OFFSET, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].designated_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_2_DEFAULT_RESULT_PROFILE, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_2].default_result_profile);

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_3_VALID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].valid);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_3_APP_DB_ID, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].app_db_id);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_3_DESIGNATED_OFFSET, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].designated_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_ESEM_3_DEFAULT_RESULT_PROFILE, INST_SINGLE,
                                esem_cmd_data.esem[ESEM_ACCESS_IF_3].default_result_profile);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Clear the ESEM access command data from hardware table.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_data_clear(
    int unit,
    int esem_cmd)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Remove the attributes from esem-cmd table in the cmd-id*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_ACCESS_CMD_TABLE, &entry_handle_id));
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, esem_cmd);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM access command data according to the given command ID.
    See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_data_sw_get(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    int *ref_count)
{
    int ref_count_i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for esem access cmd data is needed!\n");

    sal_memset(esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));

    /** Check the esem cmd is in range.*/
    if (esem_cmd < 0 || esem_cmd >= dnx_data_esem.access_cmd.nof_cmds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "esem access cmd (%d) is out of range (< %d)\n",
                     esem_cmd, dnx_data_esem.access_cmd.nof_cmds_get(unit));
    }

    /** Rerieve the cmd data from profile data base.*/
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.profile_data_get(unit, esem_cmd, &ref_count_i, esem_cmd_data));

    if (ref_count != NULL)
    {
        *ref_count = ref_count_i;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM access type according to ESEM default result profile.
 *  See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_access_type_get(
    int unit,
    int esem_default_result_profile,
    dnx_esem_access_type_t * access_type)
{
    uint32 entry_handle_id;
    uint32 result_type = 0;
    uint8 last_layer = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(access_type, _SHR_E_INTERNAL, "A valid pointer for access_type is needed!\n");

    /** Check the default result profile is in range*/
    if (esem_default_result_profile >= dnx_data_esem.default_result_profile.nof_profiles_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "default_esem_result_profile (%d) is out of range [0, %d)!\n",
                     esem_default_result_profile, dnx_data_esem.default_result_profile.nof_profiles_get(unit));
    }

    /** Get the access type*/
    if (esem_default_result_profile == DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID)
    {
        *access_type = ESEM_ACCESS_TYPE_NONE;
    }
    else if (esem_default_result_profile == dnx_data_esem.default_result_profile.default_ac_get(unit))
    {
        *access_type = ESEM_ACCESS_TYPE_ETH_AC;
    }
    else if (esem_default_result_profile == dnx_data_esem.default_result_profile.default_native_get(unit))
    {
        *access_type = ESEM_ACCESS_TYPE_ETH_NATIVE_AC;
    }
    else if (esem_default_result_profile == dnx_data_esem.default_result_profile.default_dual_homing_get(unit))
    {
        *access_type = ESEM_ACCESS_TYPE_DUAL_HOME;
    }
    else
    {
        /** Get the access type from hw table if the default result profile is not predefined. */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                                   esem_default_result_profile);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id,
                                                            DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
        if (result_type == DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_AC)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get(unit, entry_handle_id,
                                                               DBAL_FIELD_EGRESS_LAST_LAYER, INST_SINGLE, &last_layer));
            if (last_layer == TRUE)
            {
                *access_type = ESEM_ACCESS_TYPE_ETH_AC;
            }
            else
            {
                *access_type = ESEM_ACCESS_TYPE_ETH_NATIVE_AC;
            }
        }
        else if (result_type == DBAL_ENUM_FVAL_ETPP_ETPS_FORMATS_ETPS_DUAL_HOMING)
        {
            *access_type = ESEM_ACCESS_TYPE_DUAL_HOME;
        }
        else
        {
            *access_type = ESEM_ACCESS_TYPE_COUNT;
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid result type(%s) in ESEM default result table with index %d!\n",
                         dbal_result_type_to_string(unit, DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, result_type),
                         esem_default_result_profile);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get the ESEM access information for the given access type.
 *  See port_esem.h for details.
 */
shr_error_e
dnx_port_esem_cmd_access_info_get(
    int unit,
    uint32 flags,
    dnx_esem_access_type_t access_type,
    dnx_esem_access_if_t * esem_if,
    dbal_enum_value_field_esem_offset_e * esem_offset,
    int *base_cmd_idx)
{
    int cmd_idx, nof_base_cmds, access_idx;
    int access_cnt = 0;
    const dnx_esem_cmd_if_data_t *esem_cmd_if_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (esem_if == NULL && esem_offset == NULL && base_cmd_idx == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "All pointers for output are NULL!\n");
    }
    if (esem_if != NULL)
    {
        *esem_if = ESEM_ACCESS_IF_COUNT;
    }
    if (esem_offset != NULL)
    {
        *esem_offset = DBAL_NOF_ENUM_ESEM_OFFSET_VALUES;
    }
    if (base_cmd_idx != NULL)
    {
        *base_cmd_idx = -1;
    }

    nof_base_cmds = sizeof(dnx_port_esem_cmds_basic) / sizeof(dnx_port_esem_cmds_basic_t);
    for (cmd_idx = 0; cmd_idx < nof_base_cmds; cmd_idx++)
    {
        if (dnx_port_esem_cmds_basic[cmd_idx].access_type != access_type)
        {
            continue;
        }
        for (access_idx = 0; access_idx < DNX_PORT_ESEM_NOF_ACCESS; access_idx++)
        {
            esem_cmd_if_data = dnx_port_esem_cmds_basic[cmd_idx].esem_cmd_data.esem + access_idx;
            if (esem_cmd_if_data->valid)
            {
                if (esem_if != NULL)
                {
                    esem_if[access_cnt] = access_idx;
                }
                if (esem_offset != NULL)
                {
                    esem_offset[access_cnt] = esem_cmd_if_data->designated_offset;
                }
                access_cnt++;
            }
        }

        break;
    }

    if (cmd_idx == nof_base_cmds)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The given access type (%d) is not defined!\n", access_type);
    }

    if (base_cmd_idx != NULL)
    {
        *base_cmd_idx = cmd_idx;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Verify the inputs to dnx_port_esem_cmd_algo_exchange.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - flags, DNX_ALGO_TEMPLATE_ALLOCATE_*.
 * \param [in] prefix - the prefix assigned to the port or lif
 * \param [in] esem_cmd_data - ESEM access command data.
 * \param [in] esem_cmd_size_bit - Tags used for ESEM command allocation.
 * \param [out] new_esem_cmd - pointer to the ESEM command allocated.
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
static shr_error_e
dnx_port_esem_cmd_algo_exchange_verify(
    int unit,
    uint32 flags,
    uint32 prefix,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint32 esem_cmd_size_bit,
    int *new_esem_cmd,
    uint8 *is_first,
    uint8 *is_last)
{
    uint32 range_start, range_end;
    SHR_FUNC_INIT_VARS(unit);

    /** The given pointer should not be NULL*/
    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for esem access cmd data is needed");
    SHR_NULL_CHECK(new_esem_cmd, _SHR_E_INTERNAL, "A valid pointer for newly allocated esem access cmd is needed");
    SHR_NULL_CHECK(is_first, _SHR_E_INTERNAL, "A valid pointer for is_first is needed");
    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_last is needed");

    /** Validate bit size dedicated to esem cmd index.*/
    if (esem_cmd_size_bit > utilex_log2_round_up(dnx_data_esem.access_cmd.nof_cmds_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Given ESEM CMD size in bits (%d) is out of range ( < %d).",
                     esem_cmd_size_bit, dnx_data_esem.access_cmd.nof_cmds_get(unit));
    }
    /** Verify prefix */
    if (prefix >= dnx_data_esem.access_cmd.nof_cmds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Given prefix (%d) is out of range ( >= %d).",
                     prefix, dnx_data_esem.access_cmd.nof_cmds_get(unit));
    }

    /** Define the range */
    range_start = prefix;
    range_end = UTILEX_MIN(range_start + (1 << esem_cmd_size_bit), dnx_data_esem.access_cmd.nof_cmds_get(unit));

    /** verify that the esem cmd prefix combined with the esem cmd size doesn't exceed the range */
    if (flags & DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID)
    {
        /** Validate the profile in range */
        SHR_RANGE_VERIFY(*new_esem_cmd, range_start, range_end - 1, _SHR_E_INTERNAL,
                         "Given ESEM access cmd (%d) is out of range (should be between %d and %d).", *new_esem_cmd,
                         range_start, range_end);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 *  Get the ESEM command suffix size and prefix according to the given port.
 */
shr_error_e
dnx_port_esem_cmd_size_and_prefix_get(
    int unit,
    bcm_gport_t port,
    dnx_esem_cmd_suffix_size_t * esem_cmd_size,
    uint32 *prefix)
{
    uint8 is_phy_port;

    SHR_FUNC_INIT_VARS(unit);

    /** Save maximum suffix size possible for physical port */
    *esem_cmd_size = DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX;
    /** Save minimal prefix for physical port */
    *prefix = 0;

    /** Get the right esem cmd pool according to lif type */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_is_physical(unit, port, &is_phy_port));
    if (is_phy_port == FALSE)
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        dbal_table_field_info_t table_field_info;

        /** Get the local-out-lif information.*/
        SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                     &gport_hw_resources), _SHR_E_NOT_FOUND, _SHR_E_PARAM);

        /** Consider to get the tags by analyzing the field size and prefix in the next step.*/
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, gport_hw_resources.outlif_dbal_table_id,
                                                   DBAL_FIELD_ESEM_COMMAND, FALSE,
                                                   gport_hw_resources.outlif_dbal_result_type, 0, &table_field_info));

        *esem_cmd_size = GET_ENUM_FROM_ESEM_CMD_SIZE(table_field_info.field_nof_bits);
        *prefix = table_field_info.arr_prefix;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange out an ESEM access cmd with the cmd data.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - flags, DNX_ALGO_TEMPLATE_ALLOCATE_*.
 * \param [in] prefix - The prefix of the ESEM command.
 * \param [in,out] esem_cmd_data - ESEM access command data.
 * \param [in] esem_cmd_size_bit - an enum of format dnx_esem_cmd_suffix_size_t that represents
 *                                 the size in bits of the esem cmd
 * \param [in] old_esem_cmd - The esem command used now by the port or LIF.
 * \param [in,out] new_esem_cmd - pointer to the esem command allocated.
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  We have 64 esem commands which are marked by 16 different tags per ARR prefix.
 *  There are 4 commands in each tag. Tables can apply for esem command with some
 *  or all tags per the prefix in current entry.
 *  The allocation is trying to allocate an esem command for the applicant in the
 *  tag with least overlaps. The overlaps are calculated by counting the number of
 *  different applications that can use this tag.
 *
 *  Note: DON'T expose it to other files because we expect all allocated esem commands
 *  match with one of the basic commands in dnx_port_esem_cmds_basic. Using
 *  dnx_port_esem_cmd_exchange/dnx_port_esem_cmd_exchange_to_default/dnx_port_esem_cmd_free
 *  if required.
 *
 * \see
 *  None
 */
shr_error_e
dnx_port_esem_cmd_algo_exchange(
    int unit,
    uint32 flags,
    uint32 prefix,
    dnx_esem_cmd_data_t * esem_cmd_data,
    dnx_esem_cmd_suffix_size_t esem_cmd_size_bit,
    int old_esem_cmd,
    int *new_esem_cmd,
    uint8 *is_first,
    uint8 *is_last)
{
    int rv, tmp_new_esem_cmd;
    smart_template_alloc_info_t extra_alloc_info;
    int esem_cmd_actual_size;
    SHR_FUNC_INIT_VARS(unit);

    esem_cmd_actual_size = GET_ESEM_CMD_SIZE_FROM_ENUM(esem_cmd_size_bit);
    SHR_INVOKE_VERIFY_DNX(dnx_port_esem_cmd_algo_exchange_verify
                          (unit, flags, prefix, esem_cmd_data, esem_cmd_actual_size, new_esem_cmd, is_first, is_last));

    sal_memset(&extra_alloc_info, 0, sizeof(smart_template_alloc_info_t));

    /** Define range using the prefix and the ESEM CMD dedicated bit size */
    extra_alloc_info.resource_flags = RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    extra_alloc_info.resource_alloc_info.range_start = prefix;
    extra_alloc_info.resource_alloc_info.range_end =
        UTILEX_MIN(extra_alloc_info.resource_alloc_info.range_start + (1 << esem_cmd_actual_size),
                   dnx_data_esem.access_cmd.nof_cmds_get(unit));

    /** If the given profile data matches with data that is already allocated make sure that the profile is in range */
    rv = algo_port_pp_db.esem.access_cmd.profile_get(unit, esem_cmd_data, &tmp_new_esem_cmd);
    esem_cmd_data->range_unit = 0;

    while (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
        if ((tmp_new_esem_cmd >= extra_alloc_info.resource_alloc_info.range_start) &&
            (tmp_new_esem_cmd < extra_alloc_info.resource_alloc_info.range_end))
        {
            SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.exchange
                            (unit, flags, esem_cmd_data, old_esem_cmd, (void *) &extra_alloc_info, new_esem_cmd,
                             is_first, is_last));
            SHR_EXIT();
        }
        else
        {
            /** if the profile doesn't match the range try increment the range unit */
            esem_cmd_data->range_unit++;
            rv = algo_port_pp_db.esem.access_cmd.profile_get(unit, esem_cmd_data, &tmp_new_esem_cmd);
        }
    }

    /** Allocating new profile */
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.exchange
                    (unit, flags, esem_cmd_data, old_esem_cmd, (void *) &extra_alloc_info, new_esem_cmd, is_first,
                     is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Initialize ESEM default commands. The default command should be allocated
 *  Before the Initialization.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  This procedure allocate the predefined default ESEM access commands by
 *  dnx-data for outer AC and native AC. If the ESEM access commands are allocated
 *  already, it update the commands by increasing reference count. If the commands
 *  are allocated for the first time, write the command information to hardware.
 *
 *  This procedure should be called to update the default ESEM command when they
 *  are quoted.
 *
 * \see
 *  dnx_pp_port_init
 */
static shr_error_e
dnx_port_esem_cmd_default_init(
    int unit)
{
    uint32 flags;
    uint8 is_first, is_last;
    int esem_cmd_default, esem_cmd_alloc;
    dnx_esem_cmd_data_t esem_cmd_data;
    uint8 esem_default_profile;
    dnx_port_esem_cmd_info_t esem_cmd_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    esem_cmd_default = dnx_data_esem.access_cmd.no_action_get(unit);
    flags = DNX_PORT_ESEM_CMD_ALLOCATE_WITH_ID;

    /*
     * Init the esem cmd for no actions
     */
    esem_cmd_info.nof_accesses = 0;
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add(unit, 0, ESEM_ACCESS_TYPE_NONE,
                                                 DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES,
                                                 DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID, &esem_cmd_info));
    esem_cmd_alloc = dnx_data_esem.access_cmd.no_action_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_2, 0,
                                               &esem_cmd_info, esem_cmd_default, &esem_cmd_alloc,
                                               &esem_cmd_data, &is_first, &is_last));
    /** Write to esem-cmd table */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));
    }

    /*
     * Init the esem cmd for ip tunnel no actions
     */
    esem_cmd_info.nof_accesses = 0;
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add(unit, 0, ESEM_ACCESS_TYPE_NONE,
                                                 DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES,
                                                 DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID, &esem_cmd_info));
    esem_cmd_alloc = dnx_data_esem.access_cmd.ip_tunnel_no_action_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_2,
                                               dnx_data_esem.
                                               access_cmd.esem_cmd_arr_prefix_ip_tunnel_no_action_get(unit),
                                               &esem_cmd_info, esem_cmd_default, &esem_cmd_alloc, &esem_cmd_data,
                                               &is_first, &is_last));
    /** Write to esem-cmd table */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));
    }

    /*
     * Init the esem cmd for default native
     */
    esem_cmd_info.nof_accesses = 0;
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add(unit, 0, ESEM_ACCESS_TYPE_ETH_NATIVE_AC,
                                                 DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES,
                                                 DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID, &esem_cmd_info));
    esem_cmd_alloc = dnx_data_esem.access_cmd.default_native_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_3, 0,
                                               &esem_cmd_info, esem_cmd_default, &esem_cmd_alloc,
                                               &esem_cmd_data, &is_first, &is_last));
    /** Write to esem-cmd table */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));
    }

    /*
     * Init the esem cmd for default ac
     */
    esem_cmd_info.nof_accesses = 0;
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add(unit, 0, ESEM_ACCESS_TYPE_ETH_AC,
                                                 DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES,
                                                 dnx_data_esem.default_result_profile.default_ac_get(unit),
                                                 &esem_cmd_info));
    esem_cmd_alloc = dnx_data_esem.access_cmd.default_ac_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX, 0,
                                               &esem_cmd_info, esem_cmd_default, &esem_cmd_alloc,
                                               &esem_cmd_data, &is_first, &is_last));
    /** Write to esem-cmd table */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));
    }

    /*
     * Create ESEM command for SFLOW physical-port to interface
     */
    esem_cmd_info.nof_accesses = 0;
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_add(unit, 0, ESEM_ACCESS_TYPE_SFLOW_SAME_INTERFACE,
                                                 DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES,
                                                 dnx_data_esem.default_result_profile.sflow_sample_interface_get(unit),
                                                 &esem_cmd_info));
    esem_cmd_alloc = dnx_data_esem.access_cmd.sflow_sample_interface_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX, 0,
                                               &esem_cmd_info, esem_cmd_default, &esem_cmd_alloc,
                                               &esem_cmd_data, &is_first, &is_last));
    /** Write to esem-cmd table */
    if (is_first)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_set(unit, esem_cmd_alloc, esem_cmd_data));
    }

    
    {
        uint32 entry_handle_id;
        esem_default_profile = dnx_data_esem.default_result_profile.default_native_get(unit);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEM_ESEM_DEFAULT_RESULT_PROFILE, &entry_handle_id));

        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PEM_ESEM_DEFAULT_RESULT_PROFILE_1, INST_SINGLE,
                                    esem_default_profile);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_PEM_ESEM_DEFAULT_RESULT_PROFILE_2, INST_SINGLE,
                                    esem_default_profile);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Initialize the default esem cmds and profiles for native and outer AC.
 *  See port_esem.h for information.
 */
shr_error_e
dnx_port_esem_default_resource_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate and Init default esem-access-cmd
     */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_default_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Copy the esem access data and update it with the proper default result profile.
 *
 * \param [in] unit - relevant unit
 * \param [in] src_cmd_if_data - source of esem access data
 * \param [out] dst_cmd_if_data - dest of esem access data.

 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
static shr_error_e
dnx_port_esem_cmd_access_data_copy(
    int unit,
    const dnx_esem_cmd_if_data_t * src_cmd_if_data,
    dnx_esem_cmd_if_data_t * dst_cmd_if_data)
{
    dnx_esem_cmd_if_data_t *esem_cmd_if_data = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** Copy the access data first*/
    sal_memcpy(dst_cmd_if_data, src_cmd_if_data, sizeof(dnx_esem_cmd_if_data_t));

    /** Update the access data with default*/
    esem_cmd_if_data = dst_cmd_if_data;
    if (esem_cmd_if_data->valid)
    {
        if (esem_cmd_if_data->default_result_profile == ESEM_DEFAULT_RESULT_TYPE_AC)
        {
            esem_cmd_if_data->default_result_profile = dnx_data_esem.default_result_profile.default_ac_get(unit);
        }
        else if (esem_cmd_if_data->default_result_profile == ESEM_DEFAULT_RESULT_TYPE_NATIVE_AC)
        {
            esem_cmd_if_data->default_result_profile = dnx_data_esem.default_result_profile.default_native_get(unit);
        }
        else if (esem_cmd_if_data->default_result_profile == ESEM_DEFAULT_RESULT_TYPE_DUAL_HOME)
        {
            esem_cmd_if_data->default_result_profile =
                dnx_data_esem.default_result_profile.default_dual_homing_get(unit);
        }
        else if (esem_cmd_if_data->default_result_profile == ESEM_DEFAULT_RESULT_TYPE_SFLOW)
        {
            esem_cmd_if_data->default_result_profile =
                dnx_data_esem.default_result_profile.sflow_sample_interface_get(unit);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The esem access is not valid!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Validate that if a proper esem command request data is constructed.
 *
 * \param [in] unit - relevant unit
 * \param [in] flags - flags
 * \param [in] esem_cmd_info - Esem command information used to allocate a command.
 * \param [in] esem_cmd_data - Esem command data translated from esem_cmd_info.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
static shr_error_e
dnx_port_esem_cmd_data_validation(
    int unit,
    uint32 flags,
    dnx_port_esem_cmd_info_t esem_cmd_info,
    dnx_esem_cmd_data_t * esem_cmd_data)
{
    int cmd_idx, nof_base_cmds, access_idx;
    int esem_if_idx, actual_accesses, expect_accesses;
    dnx_esem_access_type_t access_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Check if the final nof accesses in cmd match with requested. */
    if (!_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_UPDATE))
    {
        /** Calculate the actual accesses in cmd-data*/
        actual_accesses = 0;
        for (esem_if_idx = 0; esem_if_idx < DNX_PORT_ESEM_NOF_ACCESS; esem_if_idx++)
        {
            if (esem_cmd_data->esem[esem_if_idx].valid)
            {
                actual_accesses++;
            }
        }

        /** Calculate the requested accesses in cmd-info*/
        nof_base_cmds = sizeof(dnx_port_esem_cmds_basic) / sizeof(dnx_port_esem_cmds_basic_t);
        expect_accesses = 0;
        for (access_idx = 0; access_idx < esem_cmd_info.nof_accesses; access_idx++)
        {
            access_type = esem_cmd_info.accesses[access_idx].access_type;
            for (cmd_idx = 0; cmd_idx < nof_base_cmds; cmd_idx++)
            {
                if (access_type == dnx_port_esem_cmds_basic[cmd_idx].access_type)
                {
                    for (esem_if_idx = 0; esem_if_idx < DNX_PORT_ESEM_NOF_ACCESS; esem_if_idx++)
                    {
                        if (dnx_port_esem_cmds_basic[cmd_idx].esem_cmd_data.esem[esem_if_idx].valid)
                        {
                            expect_accesses++;
                        }
                    }
                    break;
                }
            }
        }

        if (expect_accesses != actual_accesses)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "The esem cmd data is not prepared properly, expected access %d, but %d!\n",
                         expect_accesses, actual_accesses);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Translate the esem command information to command data that is used to allocate
 *  a command. The original command is also used for the translation in case of flag
 *  _UPDATE is set.
 *
 * \param [in] unit - relevant unit
 * \param [in] flags - flags
 * \param [in] esem_cmd_info - Esem command information used to allocate a command.
 * \param [in] old_esem_cmd - the original esem command. 
 * \param [out] esem_cmd_data - Esem command data translated from esem_cmd_info.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None.
 */
static shr_error_e
dnx_port_esem_cmd_data_collect(
    int unit,
    uint32 flags,
    dnx_port_esem_cmd_info_t esem_cmd_info,
    int old_esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data)
{
    int access_idx;
    int base_cmd_idx, if_idx;
    int old_ref_cnt = 0;
    uint8 is_update;
    dnx_esem_access_if_t access_if[DNX_PORT_ESEM_NOF_ACCESS] =
        { ESEM_ACCESS_IF_COUNT, ESEM_ACCESS_IF_COUNT, ESEM_ACCESS_IF_COUNT };
    const dnx_esem_cmd_if_data_t *base_cmd_if_data = NULL;
    dnx_esem_cmd_data_t old_esem_cmd_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, old_esem_cmd, &old_esem_cmd_data, &old_ref_cnt));

    is_update = _SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_UPDATE);
    if (is_update)
    {
        sal_memcpy(esem_cmd_data, &old_esem_cmd_data, sizeof(dnx_esem_cmd_data_t));
    }
    else
    {
        sal_memset(esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    }

    /** For removing accesses from existing cmd.*/
    if (is_update && _SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_ACCESS_REMOVE))
    {
        for (access_idx = 0; access_idx < esem_cmd_info.nof_accesses_remove; access_idx++)
        {
            if (esem_cmd_info.remove_accesses[access_idx] != ESEM_ACCESS_TYPE_NONE)
            {
                SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get(unit, 0, esem_cmd_info.remove_accesses[access_idx],
                                                                  access_if, NULL, &base_cmd_idx));
                for (if_idx = 0; if_idx < DNX_PORT_ESEM_NOF_ACCESS; if_idx++)
                {
                    if (access_if[if_idx] != ESEM_ACCESS_IF_COUNT)
                    {
                        if (!esem_cmd_data->esem[access_if[if_idx]].valid)
                        {
                            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                         "The removing access IF (%d) is invalid in current command (%d)!\n",
                                         access_if[if_idx], old_esem_cmd);
                        }
                        sal_memset(&(esem_cmd_data->esem[access_if[if_idx]]), 0, sizeof(dnx_esem_cmd_if_data_t));
                    }
                }
            }
        }
    }

    /** For newly allocating cmd or adding accesses to existing cmd.*/
    for (access_idx = 0; access_idx < esem_cmd_info.nof_accesses; access_idx++)
    {
        if (esem_cmd_info.accesses[access_idx].access_type != ESEM_ACCESS_TYPE_NONE)
        {
            SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_info_get(unit, 0, esem_cmd_info.accesses[access_idx].access_type,
                                                              access_if, NULL, &base_cmd_idx));
            base_cmd_if_data = dnx_port_esem_cmds_basic[base_cmd_idx].esem_cmd_data.esem;
            for (if_idx = 0; if_idx < DNX_PORT_ESEM_NOF_ACCESS; if_idx++)
            {
                if (access_if[if_idx] != ESEM_ACCESS_IF_COUNT)
                {
                    /** If not _UPDATE and orignal IF is in-use, error!; else copy the base cmd. */
                    if (esem_cmd_data->esem[access_if[if_idx]].valid && !is_update)
                    {
                        SHR_ERR_EXIT(_SHR_E_BUSY, "The adding access IF (%d) is in-use in current command (%d)!\n",
                                     access_if[if_idx], old_esem_cmd);
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_access_data_copy
                                        (unit, &(base_cmd_if_data[access_if[if_idx]]),
                                         &(esem_cmd_data->esem[access_if[if_idx]])));
                    }

                    if (esem_cmd_info.accesses[access_idx].esem_app_db != DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES)
                    {
                        esem_cmd_data->esem[access_if[if_idx]].app_db_id =
                            esem_cmd_info.accesses[access_idx].esem_app_db;
                    }

                    /** Set the default result profile for the access.*/
                    if (esem_cmd_info.accesses[access_idx].esem_default_result_profile ==
                        DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID)
                    {
                        if (old_esem_cmd_data.esem[access_if[if_idx]].valid)
                        {
                            esem_cmd_data->esem[access_if[if_idx]].default_result_profile =
                                old_esem_cmd_data.esem[access_if[if_idx]].default_result_profile;
                        }
                        else if (esem_cmd_data->esem[access_if[if_idx]].default_result_profile ==
                                 DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID)
                        {
                            esem_cmd_data->esem[access_if[if_idx]].default_result_profile =
                                ESEM_DEFAULT_RESULT_PROFILE_NULL;
                        }
                    }
                    else
                    {
                        esem_cmd_data->esem[access_if[if_idx]].default_result_profile =
                            esem_cmd_info.accesses[access_idx].esem_default_result_profile;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    /** Check if the cmd data matches with one of the predefined cmds.*/
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_validation(unit, flags, esem_cmd_info, esem_cmd_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange an ESEM access command from sw algo.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_exchange(
    int unit,
    uint32 flags,
    int esem_cmd_size,
    int esem_cmd_prefix,
    dnx_port_esem_cmd_info_t * esem_cmd_info,
    int old_esem_cmd,
    int *new_esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *is_first,
    uint8 *is_last)
{
    uint32 algo_flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_first, _SHR_E_INTERNAL, "A valid pointer for is_first is needed!\n");
    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_last is needed!\n");
    SHR_NULL_CHECK(new_esem_cmd, _SHR_E_INTERNAL, "A valid pointer for new_esem_cmd is needed!\n");
    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_INTERNAL, "A valid pointer for receiving esem_cmd_data is needed!\n");

    /*
     * Collect the right esem cmd information for the allocation.
     */
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_collect(unit, flags, *esem_cmd_info, old_esem_cmd, esem_cmd_data));

    /*
     * Exchange for an esem access cmd
     */
    algo_flags = _SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_ALLOCATE_WITH_ID) ? DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID : 0;
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange
                    (unit, algo_flags, esem_cmd_prefix, esem_cmd_data, esem_cmd_size, old_esem_cmd, new_esem_cmd,
                     is_first, is_last));

    esem_cmd_info->nof_accesses = 0;
    esem_cmd_info->nof_accesses_remove = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Exchange an ESEM access command to default.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_exchange_to_default(
    int unit,
    int old_esem_cmd,
    int default_esem_cmd,
    uint8 *is_last)
{
    dnx_esem_cmd_data_t esem_cmd_data;
    uint8 is_first;
    int tmp_esem_cmd, ref_count;
    SHR_FUNC_INIT_VARS(unit);

    if (!DNX_PORT_ESEM_CMD_IS_DEFAULT(default_esem_cmd))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The given default esem command(%d) is not actually default!\n",
                     default_esem_cmd);
    }

    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, default_esem_cmd, &esem_cmd_data, &ref_count));
    SHR_IF_ERR_EXIT(dnx_port_esem_cmd_algo_exchange(unit, 0, 0, &esem_cmd_data, DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX,
                                                    old_esem_cmd, &tmp_esem_cmd, &is_first, is_last));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Free an ESEM access command from sw algo.
 *  See port_esem.h for more information.
 */
shr_error_e
dnx_port_esem_cmd_free(
    int unit,
    int esem_cmd,
    uint8 *is_last)
{
    uint8 free_needed, is_first;
    int tmp_esem_cmd;
    dnx_esem_cmd_data_t esem_cmd_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_last, _SHR_E_INTERNAL, "A valid pointer for is_last is needed\n!");

    /*
     * Check if the cmd is one of the default. If TRUE, no need to free it.
     */
    free_needed = TRUE;
    if ((esem_cmd == dnx_data_esem.access_cmd.default_ac_get(unit)) ||
        (esem_cmd == dnx_data_esem.access_cmd.default_native_get(unit)) ||
        (esem_cmd == dnx_data_esem.access_cmd.no_action_get(unit)))
    {
        free_needed = FALSE;
    }

    /*
     * Free the given esem_cmd by exchanging it to default.
     */
    *is_last = FALSE;
    if (free_needed == TRUE)
    {
        tmp_esem_cmd = dnx_data_esem.access_cmd.no_action_get(unit);

        sal_memset(&esem_cmd_data, 0, sizeof(esem_cmd_data));
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_data_sw_get(unit, tmp_esem_cmd, &esem_cmd_data, NULL));

        SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.exchange
                        (unit, 0, &esem_cmd_data, esem_cmd, NULL, &tmp_esem_cmd, &is_first, is_last));
        if (tmp_esem_cmd != dnx_data_esem.access_cmd.no_action_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "esem_cmd(%d) is not released correctly. Expect exchanging to (%d) but (%d)",
                         esem_cmd, dnx_data_esem.access_cmd.no_action_get(unit), tmp_esem_cmd);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see port_esem.h file
 */
shr_error_e
dnx_port_esem_cmd_access_add(
    int unit,
    uint32 flags,
    dnx_esem_access_type_t access_type,
    dbal_enum_value_field_esem_app_db_id_e esem_app_db,
    int esem_default_result_profile,
    dnx_port_esem_cmd_info_t * esem_cmd_info)
{
    int *nof_access = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, DNX_PORT_ESEM_CMD_ACCESS_REMOVE))
    {
        nof_access = &(esem_cmd_info->nof_accesses_remove);
        esem_cmd_info->remove_accesses[*nof_access] = access_type;
    }
    else
    {
        nof_access = &(esem_cmd_info->nof_accesses);
        esem_cmd_info->accesses[*nof_access].access_type = access_type;
        esem_cmd_info->accesses[*nof_access].esem_app_db = esem_app_db;
        esem_cmd_info->accesses[*nof_access].esem_default_result_profile = esem_default_result_profile;
    }

    (*nof_access)++;

    if (*nof_access >= DNX_PORT_ESEM_NOF_ACCESS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Less than %d ESEM access IFs are supported!\n", DNX_PORT_ESEM_NOF_ACCESS);
    }

exit:
    SHR_FUNC_EXIT;
}
