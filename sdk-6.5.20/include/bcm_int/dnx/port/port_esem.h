/** \file port_esem.h
 * 
 *
 * Internal DNX per Port ESEM management APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PORT_ESEM_H_INCLUDED
/* { */
#define PORT_ESEM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>

/*
 * Defines {
 */
#define DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID     (255)

#define DNX_PORT_ESEM_NOF_ACCESS                         (DNX_DATA_MAX_ESEM_ACCESS_CMD_NOF_ESEM_ACCESSES)

/*
 * Define flags for esem access command exchanging.
 */

/** Allocate an ESEM cmd base on the inputs */
#define DNX_PORT_ESEM_CMD_ALLOCATE_WITH_ID       (SAL_BIT(0))

/** Update the existing cmd, maybe add new access or update the default result profile.*/
#define DNX_PORT_ESEM_CMD_UPDATE                 (SAL_BIT(1))

/** Remove requested ESEM cmd from the exists group of ESEM commands (Must have the DNX_PORT_ESEM_CMD_UPDATE set with it) .*/
#define DNX_PORT_ESEM_CMD_ACCESS_REMOVE          (SAL_BIT(2))

/** Returns the actual size of the suffix given value of type dnx_esem_cmd_suffix_size_t */
#define GET_ESEM_CMD_SIZE_FROM_ENUM(_expr) (((_expr)==(DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX)) ? (dnx_data_esem.access_cmd.nof_cmds_size_in_bits_get(unit)) : (_expr))

/** Returns dnx_esem_cmd_suffix_size_t given value of actual size of the suffix */
#define GET_ENUM_FROM_ESEM_CMD_SIZE(_expr) (((_expr)==(dnx_data_esem.access_cmd.nof_cmds_size_in_bits_get(unit))) ? (DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX) : (_expr))

/** }*/

/** For ESEM CMD, suffix size of ESEM CMD (in bit) */
typedef enum dnx_esem_cmd_suffix_size_e
{
    /** Maximum suffix size */
    DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_MAX = 0,

    /** Suffix size of size 2 */
    DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_2 = 2,

    /** Suffix size of size 3 */
    DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_3 = 3,

    /** Suffix size of size 4 */
    DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_4 = 4,

} dnx_esem_cmd_suffix_size_t;

/** Define possible ESEM access types */
typedef enum dnx_esem_access_type_e
{
    /** NULL ESEM access*/
    ESEM_ACCESS_TYPE_NONE = 0,

    /** ESEM access for outer AC*/
    ESEM_ACCESS_TYPE_ETH_AC,

    /** ESEM access for native AC*/
    ESEM_ACCESS_TYPE_ETH_NATIVE_AC,

    /** ESEM access for forward domain network name, such as VNI/GRE-key/ISID, etc*/
    ESEM_ACCESS_TYPE_VXLAN_VNI,

    /** ESEM access for dual-homing information, such as EVPN-ESI label, IPv4 tunnel SIP, etc*/
    ESEM_ACCESS_TYPE_DUAL_HOME,

    /** ESEM access for EVPN*/
    ESEM_ACCESS_TYPE_EVPN,

    /** ESEM access for port based source address, such as B-SA in Mac-in-Mac, etc*/
    ESEM_ACCESS_TYPE_PORT_SA,

    /** ESEM access for SVTAG*/
    ESEM_ACCESS_TYPE_SVTAG,

    /** ESEM access for VXLAN_SVTAG*/
    ESEM_ACCESS_TYPE_VXLAN_SVTAG,

    /** ESEM access for SFLOW*/
    ESEM_ACCESS_TYPE_SFLOW_SAME_INTERFACE,

    ESEM_ACCESS_TYPE_COUNT
} dnx_esem_access_type_t;

/*
 * }
 */
/*
 * Structures {
 */

/*
 * Holds all the fields of an ESEM access
 */
typedef struct dnx_port_esem_access_info_s
{
    /*
     * The access type of the esem command.
     */
    dnx_esem_access_type_t access_type;
    /*
     * ESEM command APP ID(key ID)
     */
    dbal_enum_value_field_esem_app_db_id_e esem_app_db;
    /*
     * The esem_default_result_profile for the command.
     */
    int esem_default_result_profile;
} dnx_port_esem_access_info_t;

/*
 * Holds all the fields of an ESEM command
 */
typedef struct dnx_port_esem_cmd_info_s
{
    /*
     * The nof esem access types for cmd allocation
     */
    int nof_accesses;

    /*
     * The requested ESEM commands to add information.
     */
    dnx_port_esem_access_info_t accesses[DNX_PORT_ESEM_NOF_ACCESS];

    /*
     * The nof esem access types to remove from the cmd(valid only with DNX_PORT_ESEM_CMD_ACCESS_REMOVE)
     */
    int nof_accesses_remove;

    /*
     * List of access types to remove
     */
    dnx_esem_access_type_t remove_accesses[DNX_PORT_ESEM_NOF_ACCESS];
} dnx_port_esem_cmd_info_t;

/*
 * }
 */

/*
 * Declarations {
 */
/**
 * \brief -
 *  Allocate and initialize esem default commands native and outer ac.
 *
 * \param [in] unit - relevant unit
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Esem cmds and default result profiles for default usage are predefined in
 *  dnx-data. Default cmds are allocated here. The corresponding tables are
 *  configured with default properties.
 *
 * \see
 *  dnx_pp_port_init
 */
shr_error_e dnx_port_esem_default_resource_init(
    int unit);

/**
 * \brief -
 *  Get the ESEM command suffix size and prefix according to the given port.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] port - the given port
 * \param [out] esem_cmd_size - Available ESEM access command size for the port or lif.
 * \param [out] prefix - Prefix as defined for the port or lif.
 *
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
shr_error_e dnx_port_esem_cmd_size_and_prefix_get(
    int unit,
    bcm_gport_t port,
    dnx_esem_cmd_suffix_size_t * esem_cmd_size,
    uint32 *prefix);

/**
 * \brief -
 *  Exchange an ESEM access command from sw algo.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - ESEM access command allocation flags.
 *                     See DNX_PORT_ESEM_CMD_*
 * \param [in] esem_cmd_size - an enum of format dnx_esem_cmd_suffix_size_t that represents
 *                             the size in bits of the esem cmd
 * \param [in] esem_cmd_prefix - In case an ARR prefix is required for the ESEM command.
 * \param [in] esem_cmd_info - The ESEM commands add/remove information.
 * \param [in] old_esem_cmd - The esem command used previously.
 * \param [out] new_esem_cmd - pointer to the esem command allocated.
 * \param [out] esem_cmd_data - pointer to esem command info used to allocate the new command.
 * \param [out] is_first - Indicate that if new_esem_cmd is used for the first time.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  This exchange won't update hw.
 *  If flags DNX_PORT_ESEM_CMD_UPDATE is used, the exchange will
 *  inherit the old esem_cmd_data from old_esem_cmd and update it with the inputs;
 *  Else re-construct the esem_cmd_data per the inputs.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_exchange(
    int unit,
    uint32 flags,
    int esem_cmd_size,
    int esem_cmd_prefix,
    dnx_port_esem_cmd_info_t * esem_cmd_info,
    int old_esem_cmd,
    int *new_esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 *is_first,
    uint8 *is_last);

/**
 * \brief -
 *  Exchange an ESEM access command to default.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] old_esem_cmd - The esem command used previously.
 * \param [in] default_esem_cmd - pointer to the esem command allocated.
 * \param [out] is_last - Indicate that if old_esem_cmd is not in use now.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  The given default_esem_cmd must be an actual default command.
 *  'default profiles' are the profiles allocated during init stage.
 *  is_first is not addressed because we use an allocated profile.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_exchange_to_default(
    int unit,
    int old_esem_cmd,
    int default_esem_cmd,
    uint8 *is_last);

/**
 * \brief -
 *  Set ESEM access command data to hardware table.
 *
 * \param [in] unit - relevant unit
 * \param [in] esem_cmd - The esem cmd id to update
 * \param [in] esem_cmd_data - ESEM access command data.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_data_set(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t esem_cmd_data);

/**
 * \brief -
 *  Clear the ESEM access command data from hardware table.
 *
 * \param [in] unit - relevant unit
 * \param [in] esem_cmd - The esem cmd id to update
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_data_clear(
    int unit,
    int esem_cmd);

/**
 * \brief -
 *  Free an ESEM access command from sw algo.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_cmd - The given ESEM access command.
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
shr_error_e dnx_port_esem_cmd_free(
    int unit,
    int esem_cmd,
    uint8 *is_last);

/**
 * \brief -
 *  Get the ESEM access command data according to the given command ID.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_cmd - The given ESEM access command data.
 * \param [out] esem_cmd_data - ESEM access command data.
 * \param [out] ref_count - Pointer for number of reference to the cmd, can be NULL.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  None.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_data_sw_get(
    int unit,
    int esem_cmd,
    dnx_esem_cmd_data_t * esem_cmd_data,
    int *ref_count);

/**
 * \brief -
 *  Get the ESEM access type according to ESEM default result profile.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] esem_default_result_profile - esem default result profile
 *             Valid range is [0,15].
 * \param [out] access_type - Access type to esem table.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  Get the access type to ESEM per the defaut result entry.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_access_type_get(
    int unit,
    int esem_default_result_profile,
    dnx_esem_access_type_t * access_type);

/**
 * \brief -
 *  Get the ESEM access interface.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - Not used now.
 * \param [in] access_type - esem access type.
 *             See dnx_esem_access_type_t for the available types.
 * \param [out] esem_if - esem access interface.
 *              See dnx_esem_access_if_t for the possible interfaces.
 * \param [out] esem_offset - esem entry offset relative to its EES entry.
 *              See dbal_enum_value_field_esem_offset_e for the possible values.
 * \param [out] base_cmd_idx - The index of the access type is defined in basic command array.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  There are two accesses to ESEM and must be used properly in order to yield
 *  two results. We define access_type to manage the applications that need accesses
 *  to ESEM. Access type should be determined according to applications and their
 *  match criterion. see dnx_esem_access_type_t for the available esem access types.
 *
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_access_info_get(
    int unit,
    uint32 flags,
    dnx_esem_access_type_t access_type,
    dnx_esem_access_if_t * esem_if,
    dbal_enum_value_field_esem_offset_e * esem_offset,
    int *base_cmd_idx);

/**
 * \brief -
 *  ADds a new type to the "commands info" structure.
 *
 * \param [in] unit - the relevant unit.
 * \param [in] flags - flags of DNX_PORT_ESEM_CMD_*.
 * \param [in] access_type - ESEM access type.
 * \param [in] esem_app_db - The ESEM command access APP DB.
 *             If it's DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES, the default value is used.
 * \param [in] esem_default_result_profile -The ESEM command default result profile.
 *             If it's DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID,
 *             the original value(if valid) or default value is used.
 * \param [out] esem_cmd_info - The ESEM commands accesses structure.
 *
 * \return
 *  shr_error_e
 *
 * \remark
 *  If the access-type includes more than one access-IFs, then the app_db_id and
 *  default_result_profile for each IF can be the default as predefined by setting
 *  esem_app_db = DBAL_NOF_ENUM_ESEM_APP_DB_ID_VALUES
 *  esem_default_result_profile = DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID
 *  or the same for each IF with inputs. If different values for each IF are needed,
 *  You should add these access-IFs with separate access-type one by one.
 *  
 * \see
 *  None
 */
shr_error_e dnx_port_esem_cmd_access_add(
    int unit,
    uint32 flags,
    dnx_esem_access_type_t access_type,
    dbal_enum_value_field_esem_app_db_id_e esem_app_db,
    int esem_default_result_profile,
    dnx_port_esem_cmd_info_t * esem_cmd_info);

/*
 * }
 */
#endif /* PORT_ESEM_H_INCLUDED */
