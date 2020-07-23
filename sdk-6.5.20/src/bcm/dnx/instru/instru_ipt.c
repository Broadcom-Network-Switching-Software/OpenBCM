/** \file instru_ipt.c
 * 
 *
 * IPT procedures for DNX in instrumentation module.
 *
 * This file contains functions for IPT (INT/Tail-Edit) configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
/*
 * }
 */

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/instru.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <shared/bitop.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */

#define DNX_INSTRU_IFA_INITIATOR_TRAP_CONTEXT_PROFILE DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_NOTHING_OR_BUILD_PTCH1

#define NOF_TM_ACTION_TYPE_IS_FWD 2
#define NOF_ASE_INT_PROFILE 4

 /*
  * }
  */

 /*
  * Internal functions.
  * {
  */

 /*
  * }
  */

/**
 * \brief - Setting all flags according type
 */
static void
dnx_instru_ipt_all_flags_set(
    int unit,
    bcm_instru_ipt_node_type_t type,
    uint32 *flags)
{

    if (type == bcmInstruIptNodeTypeIntOverVxlanLast)
    {
        *flags = 0;
    }
    else
    {
        *flags = BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD |
            BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT |
            BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_TOD |
            BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_SYS_PORT |
            BCM_INSTRU_IPT_METADATA_FLAG_SWITCH_ID | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO;

        if (dnx_data_instru.ipt.feature_get(unit, dnx_data_instru_ipt_advanced_ipt))
        {
            *flags |= BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_FG |
                BCM_INSTRU_IPT_METADATA_FLAG_LATENCY |
                BCM_INSTRU_IPT_METADATA_FLAG_PORT_RATE | BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_SIZE_CG;
        }
        else
        {
            *flags |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID |
                BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_DEV_ID | BCM_INSTRU_IPT_METADATA_FLAG_EGRESS_RESERVED;

            if (type == bcmInstruIptNodeTypeIntOverVxlanFirst || type == bcmInstruIptNodeTypeIntOverVxlanIntermediate)
            {
                *flags |= BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED;
            }
        }
    }
}

/**
 * \brief - verify procedure for bcm_dnx_instru_ipt_profile_set() API
 */
static shr_error_e
dnx_instru_ipt_profile_set_verify(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    const dnx_data_instru_ipt_profile_info_t *profile_info;

    SHR_FUNC_INIT_VARS(unit);

    /** null check */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    /** get ipt profile info */
    profile_info = dnx_data_instru.ipt.profile_info_get(unit, ipt_profile);

    /** check that the provided ipt profile fits the provided node type */
    if (profile_info->type != config->node_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The provided profile id (%d) and type (%d) doesn't match", ipt_profile,
                     config->node_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for bcm_dnx_instru_ipt_profile_get() API
 */
static shr_error_e
dnx_instru_ipt_profile_get_verify(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    const dnx_data_instru_ipt_profile_info_t *profile_info;
    const dnxc_data_table_info_t *profile_table_info;

    SHR_FUNC_INIT_VARS(unit);

    /** null check */
    SHR_NULL_CHECK(config, _SHR_E_PARAM, "config");

    /** verify profile id range*/
    profile_table_info = dnx_data_instru.ipt.profile_info_info_get(unit);
    if ((ipt_profile < 0) || (ipt_profile >= profile_table_info->key_size[0]))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The provided profile id (%d) is invalid", ipt_profile);
    }

    /** check that the provided ipt profile is valid */
    profile_info = dnx_data_instru.ipt.profile_info_get(unit, ipt_profile);
    if (profile_info->type == -1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The provided profile id (%d) is invalid", ipt_profile);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for dnx_instru_ipt_tail_edit_profile_set()
 */
static shr_error_e
dnx_instru_ipt_tail_edit_profile_set_verify(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{

    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** verify flags */
    dnx_instru_ipt_all_flags_set(unit, config->node_type, &supported_flags);

    SHR_MASK_VERIFY(config->metadata_flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    if (dnx_data_instru.ipt.feature_get(unit, dnx_data_instru_ipt_advanced_ipt) != 1)
    {
        /** If Queue info is provided, verify that the data before queue info is also set */
        if (_SHR_IS_FLAG_SET(config->metadata_flags, BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO))
        {
            if ((_SHR_IS_FLAG_SET(config->metadata_flags, BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD) == FALSE) ||
                (_SHR_IS_FLAG_SET(config->metadata_flags, BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT) == FALSE) ||
                (_SHR_IS_FLAG_SET(config->metadata_flags, BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID) == FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "When queue info is required in Tal-Edit profile, then the following flags must "
                             "be also provided: BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_TOD, "
                             "BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_SYS_PORT, BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_DEV_ID");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify procedure for dnx_instru_ipt_int_profile_set()
 */
static shr_error_e
dnx_instru_ipt_int_profile_set_verify(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    uint32 supported_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** verify flags */
    dnx_instru_ipt_all_flags_set(unit, config->node_type, &supported_flags);

    SHR_MASK_VERIFY(config->metadata_flags, supported_flags, _SHR_E_PARAM, "non supported flags specified.\n");

    /** Queue info must be set for INT profile */
    if (_SHR_IS_FLAG_SET(config->metadata_flags, BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO flag must be set");
    }

    /**  first node - verify first node header (iOAM SHIM header) */
    if (config->node_type == bcmInstruIptNodeTypeIntOverVxlanFirst)
    {
        if ((config->first_node_header).length != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "INT First node header length must be se to 0");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ipt profile info to HW
 */
static shr_error_e
dnx_instru_ipt_profile_hw_set(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    uint32 metadata_size = 0;
    uint32 metadata_bitmap = 0;
    int member;
    uint32 metadata_flags[1];
    const dnx_data_instru_ipt_metadata_t *metadata_info;
    uint8 advanced = 0;
    uint8 ase_int_profile = 0;
    uint8 tm_action_type_is_fwd = 0;
    uint32 ipt_profile_key;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    metadata_flags[0] = config->metadata_flags;
    advanced = dnx_data_instru.ipt.feature_get(unit, dnx_data_instru_ipt_advanced_ipt);

    if (advanced != 1)
    {
        /** Workarounds for INT */
        if ((config->node_type == bcmInstruIptNodeTypeIntOverVxlanFirst) ||
            (config->node_type == bcmInstruIptNodeTypeIntOverVxlanIntermediate) ||
            (config->node_type == bcmInstruIptNodeTypeIntOverVxlanLast))
        {
            /*
             * special case for INT - queue info is always added by PEMLLA for INT packets.
             * There is no control bit and size shouldn't be updated.
             */
            metadata_flags[0] &= ~BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO;
        }
    }

    /** construct size and bitmap HW fields */
    SHR_BIT_ITER(metadata_flags, SHR_BITWID, member)
    {
        /** get metadata info */
        metadata_info = dnx_data_instru.ipt.metadata_get(unit, member);

        /** add field size */
        metadata_size += metadata_info->size;

        /** add bit to metadata bitmap */
        SHR_BITSET(&metadata_bitmap, metadata_info->bit);
    }

    for (ase_int_profile = 0; ase_int_profile < NOF_ASE_INT_PROFILE; ase_int_profile++)
    {
        for (tm_action_type_is_fwd = 0; tm_action_type_is_fwd < NOF_TM_ACTION_TYPE_IS_FWD; tm_action_type_is_fwd++)
        {
            if (advanced == 1)
            {
          /** Build key for J2P and up */
                ipt_profile_key = (ase_int_profile << 4) + (ipt_profile << 1) + tm_action_type_is_fwd;
            }
            else
            {
                ipt_profile_key = ipt_profile;
          /** No need to iterate */
                ase_int_profile = NOF_ASE_INT_PROFILE;
                tm_action_type_is_fwd = NOF_TM_ACTION_TYPE_IS_FWD;
            }
    /** commit to HW */
            {
                uint32 entry_handle_id;

        /** create handle */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INSTRU_IPT_PROFILE, &entry_handle_id));

        /** set key - group id*/
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE, ipt_profile_key);

        /** set value - metadata bitmap */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METADATA_BITMAP, INST_SINGLE,
                                             metadata_bitmap);

        /** set value - metadata size */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_METADATA_SIZE, INST_SINGLE,
                                             metadata_size);

                if (advanced == 1)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APPEND_DATA_ENABLE, INST_SINGLE, 1);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_DATA_SIZE_RESOLUTION, INST_SINGLE,
                                                 0);

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRUNCATE_DATA_ENABLE, INST_SINGLE,
                                                 0);
                }

        /** commit */
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get ipt profile info from HW
 */
static shr_error_e
dnx_instru_ipt_profile_hw_get(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{

    uint32 metadata_bitmap[1] = { 0 };
    uint32 metadata_flags[1] = { 0 };
    int member, flag_id;
    const dnx_data_instru_ipt_metadata_t *metadata_info;
    const dnx_data_instru_ipt_profile_info_t *profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get ipt profile info from dnx data */
    profile_info = dnx_data_instru.ipt.profile_info_get(unit, ipt_profile);
    config->node_type = profile_info->type;

    /** Get metadata bitmap from HW */
    {
        uint32 entry_handle_id;

        /** create handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INSTRU_IPT_PROFILE, &entry_handle_id));

        /** set key - group id*/
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE, ipt_profile);

        /** request metadata bitmap */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_METADATA_BITMAP, INST_SINGLE, metadata_bitmap);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * construct API flags according to the given metadata bitmap.
     */
    SHR_BIT_ITER(metadata_bitmap, SHR_BITWID, member)
    {
        /*
         * Iterate over all possible API flags, if the given API flag is mapped to the current member of metadata,
         * then add this flag metadata_flags and break.
         */
        for (flag_id = 0; flag_id < SHR_BITWID; flag_id++)
        {
            /** for tail-edit, BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED flag is invalid */
            if (profile_info->type == bcmInstruIptNodeTypeTail)
            {
                if (BCM_INSTRU_IPT_METADATA_FLAG_INGRESS_RESERVED & (1 << flag_id))
                {
                    continue;
                }
            }

            /** for INT, BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO flag is always valid (it's not controlled by HW) */
            if (profile_info->type == bcmInstruIptNodeTypeIntOverVxlanFirst ||
                profile_info->type == bcmInstruIptNodeTypeIntOverVxlanIntermediate)
            {
                if (BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO & (1 << flag_id))
                {
                    continue;
                }
            }

            /** get metadata info */
            metadata_info = dnx_data_instru.ipt.metadata_get(unit, flag_id);

            /** Check if the flag is mapped to the member */
            if (metadata_info->bit == member)
            {
                /** flag found, add it to metadata_flags and break */
                SHR_BITSET(metadata_flags, flag_id);
                break;
            }
        }
    }

    config->metadata_flags = metadata_flags[0];

    /** for INT, BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO flag is always valid (it's not controlled by HW) */
    if (profile_info->type == bcmInstruIptNodeTypeIntOverVxlanFirst ||
        profile_info->type == bcmInstruIptNodeTypeIntOverVxlanIntermediate)
    {
        config->metadata_flags |= BCM_INSTRU_IPT_METADATA_FLAG_QUEUE_INFO;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set tail-edit profile
 */
static shr_error_e
dnx_instru_ipt_tail_edit_profile_set(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify Tail-Edit ipt profile */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_ipt_tail_edit_profile_set_verify(unit, flags, ipt_profile, config));

    /** set profile */
    SHR_IF_ERR_EXIT(dnx_instru_ipt_profile_hw_set(unit, flags, ipt_profile, config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get INT first node header (iOAM SHIM header)
 */
static shr_error_e
dnx_instru_ipt_profile_first_node_header_hw_get(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    uint32 entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_INT, &entry_handle_id));

    /** Request instruction bitmap (options) */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_INSTRUCTION_BITMAP, INST_SINGLE,
                               &((config->first_node_header).options));

    /** Request type */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_TYPE, INST_SINGLE,
                               &((config->first_node_header).type));
#if 0
    /** Request length */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_LENGTH, INST_SINGLE,
                               &((config->first_node_header).length));
#endif

    
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        /*
         * Get
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set INT first node header (iOAM SHIM header)
 */
static shr_error_e
dnx_instru_ipt_profile_first_node_header_hw_set(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    uint32 entry_handle_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_INT, &entry_handle_id));

    /** set instruction bitmap (options) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_INSTRUCTION_BITMAP, INST_SINGLE,
                                 (config->first_node_header).options);

    /** set type */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_TYPE, INST_SINGLE,
                                 (config->first_node_header).type);
#if 0
    /** set length */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_LENGTH, INST_SINGLE,
                                 (config->first_node_header).length);

    /** set reserved to 0 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_RESERVED, INST_SINGLE, 0);
#endif

    
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        /*
         * Commit
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set INT profile
 */
static shr_error_e
dnx_instru_ipt_int_profile_set(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify INT ipt profile */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_ipt_int_profile_set_verify(unit, flags, ipt_profile, config));

    /** set profile */
    SHR_IF_ERR_EXIT(dnx_instru_ipt_profile_hw_set(unit, flags, ipt_profile, config));

    /** set first node header */
    if (config->node_type == bcmInstruIptNodeTypeIntOverVxlanFirst)
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipt_profile_first_node_header_hw_set(unit, flags, ipt_profile, config));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The API sets the IPT profile attributes
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - flags
 * \param [in] ipt_profile - IPT profile ID
 * \param [in] config - IPT profile attributes
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_ipt_profile_set(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify ipt profile */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_ipt_profile_set_verify(unit, flags, ipt_profile, config));

    /** dispatch according to ipt profile type */
    switch (config->node_type)
    {
        case (bcmInstruIptNodeTypeTail):
            SHR_IF_ERR_EXIT(dnx_instru_ipt_tail_edit_profile_set(unit, flags, ipt_profile, config));
            break;

        case (bcmInstruIptNodeTypeIntOverVxlanFirst):
        case (bcmInstruIptNodeTypeIntOverVxlanIntermediate):
            SHR_IF_ERR_EXIT(dnx_instru_ipt_int_profile_set(unit, flags, ipt_profile, config));
            break;

        case (bcmInstruIptNodeTypeIntOverVxlanLast):
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "IPT profile (%d) can't be configured since it's last node", ipt_profile);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type not supported %d", config->node_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - The API gets the IPT profile attributes
 *
 * \param [in] unit - Relevant unit
 * \param [in] flags - flags
 * \param [in] ipt_profile - IPT profile ID
 * \param [out] config - IPT profile attributes
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_instru_ipt_profile_get(
    int unit,
    uint32 flags,
    int ipt_profile,
    bcm_instru_ipt_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** verify ipt profile */
    SHR_INVOKE_VERIFY_DNX(dnx_instru_ipt_profile_get_verify(unit, flags, ipt_profile, config));

    /** init */
    bcm_instru_ipt_t_init(config);

    /** get IPT profile info from HW */
    SHR_IF_ERR_EXIT(dnx_instru_ipt_profile_hw_get(unit, flags, ipt_profile, config));

    /** Get first node header from HW */
    if (config->node_type == bcmInstruIptNodeTypeIntOverVxlanFirst)
    {
        SHR_IF_ERR_EXIT(dnx_instru_ipt_profile_first_node_header_hw_get(unit, flags, ipt_profile, config));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ipt_node_id_get(
    int unit,
    int *node_id)
{
    uint32 entry_handle_id, field32;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** null check */
    SHR_NULL_CHECK(node_id, _SHR_E_PARAM, "node_id");

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INSTRU_IPT_GENERAL_CONFIG, &entry_handle_id));

    /** request Switch ID */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NODE_ID, INST_SINGLE, &field32);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *node_id = field32;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ipt_node_id_set(
    int unit,
    int node_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INSTRU_IPT_GENERAL_CONFIG, &entry_handle_id));

    /** set value - switch ID */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NODE_ID, INST_SINGLE, node_id);

    /** commit */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ipt_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** By default, set all IPT profiles to include all possible metadata */
    {
        const dnx_data_instru_ipt_profile_info_t *profile_info;
        const dnxc_data_table_info_t *profile_table_info;
        uint32 ipt_profile;
        bcm_instru_ipt_t config;
        uint32 flags;

        profile_table_info = dnx_data_instru.ipt.profile_info_info_get(unit);

        /** iterate over all ipt profiles and configure them to default values */
        for (ipt_profile = 0; ipt_profile < profile_table_info->key_size[0]; ipt_profile++)
        {
            bcm_instru_ipt_t_init(&config);

            profile_info = dnx_data_instru.ipt.profile_info_get(unit, ipt_profile);
            /**  tail-edit */

            if (profile_info->type == bcmInstruIptNodeTypeTail ||
                profile_info->type == bcmInstruIptNodeTypeIntOverVxlanFirst ||
                profile_info->type == bcmInstruIptNodeTypeIntOverVxlanIntermediate)
            {
                dnx_instru_ipt_all_flags_set(unit, profile_info->type, &flags);
                config.metadata_flags = flags;

                /** set tail-edit profile */
                config.node_type = profile_info->type;
                SHR_IF_ERR_EXIT(bcm_dnx_instru_ipt_profile_set(unit, 0, ipt_profile, &config));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ifa_initiator_rcy_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int arg)
{
    uint32 pp_port_index, entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dbal_enum_value_field_etpp_trap_context_port_profile_e port_trap_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** iterate over pp ports */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

        /** set pp port and core */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        /** set trap profile */
        if (arg)
        {
            port_trap_profile = DNX_INSTRU_IFA_INITIATOR_TRAP_CONTEXT_PROFILE;
        }
        else
        {
            port_trap_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL;
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                     port_trap_profile);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ifa_initiator_rcy_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *arg)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dbal_enum_value_field_etpp_trap_context_port_profile_e port_trap_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** null check */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /** get gport info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

        /** set pp port and core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                               &port_trap_profile);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (port_trap_profile == DNX_INSTRU_IFA_INITIATOR_TRAP_CONTEXT_PROFILE)
    {
        *arg = TRUE;
    }
    else
    {
        *arg = FALSE;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ipt_trap_to_rcy_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int arg)
{
    uint32 pp_port_index, entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dbal_enum_value_field_etpp_trap_context_port_profile_e port_trap_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get pp port */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** iterate over pp ports */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

        /** set pp port and core */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        /** set trap profile */
        if (arg)
        {
            port_trap_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_INT_EGRESS_PORT;
        }
        else
        {
            port_trap_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL;
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                     port_trap_profile);

        /** commit */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_instru_ipt_trap_to_rcy_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    int *arg)
{
    uint32 pp_port_index, entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dbal_enum_value_field_etpp_trap_context_port_profile_e port_trap_profile;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** null check */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /** get gport info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    /** iterate over pp ports */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

        /** set pp port and core */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                   &port_trap_profile);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (port_trap_profile == DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_INT_EGRESS_PORT)
        {
            *arg = TRUE;
        }
        else
        {
            *arg = FALSE;
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
