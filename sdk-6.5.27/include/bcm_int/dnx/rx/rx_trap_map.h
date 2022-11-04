/** 
 *  \file bcm_int/dnx/rx/rx_trap_map.h
 * 
 * 
 * Internal DNX RX APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef RX_TRAP_MAP_H_INCLUDED
/* { */
#define RX_TRAP_MAP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <include/bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>

#define DNX_RX_TRAP_MAP_MAX_NOF_DEFAULT_TRAPS 3
/**
 * Taken and renamed from src/bcm/dnx/oam/oam_punt_and_protection.c -> DNX_OAMP_PUNT_MSB.
 */
#define DNX_RX_OAMP_TRAPS_LOWER_RANGE        0x400

/* 
 * Types 
 * { 
 */

/**
 * \brief struct containing mapping info for trap_type.
 */
typedef struct
{
    int valid;
    uint32 trap_id;
    dnx_rx_trap_block_e trap_block;
    dnx_rx_trap_class_e trap_class;
    char trap_name[DBAL_MAX_STRING_LENGTH];
    uint32 flags;
    uint8 is_strengthless;
} dnx_rx_trap_map_type_t;

/**
 * \brief struct containing user-defined traps information.
 */
typedef struct
{
    /** User-defined trap ids range is [lower_bound_trap_id, upper_bound_trap_id] */
    int lower_bound_trap_id;
    int upper_bound_trap_id;
    char block_name[DBAL_MAX_STRING_LENGTH];
    int default_trap_id_list[DNX_RX_TRAP_MAP_MAX_NOF_DEFAULT_TRAPS];
} dnx_rx_trap_map_ud_t;

/**
 * \brief struct containing Ingress user-defined traps mapping.
 */
typedef struct
{
    int valid;
    int alloc_index;
} dnx_rx_trap_map_ingress_ud_t;

/**
 * \brief struct containing ETPP profile dbal fields.
 */
typedef struct
{
    /** recycle packet priority field */
    dbal_fields_e cfg_rcy_prio;

    /** recycle packet crop field */
    dbal_fields_e cfg_rcy_crop;

    /** recycle packet append orifinal FTMH field */
    dbal_fields_e cfg_rcy_append_orig;

    /** recycle command field */
    dbal_fields_e cfg_rcy_cmd;

    /** CPU trap code field */
    dbal_fields_e cfg_cpu_trap_code;

    /** PP drop reason field */
    dbal_fields_e cfg_pp_drop_reason;

    /** Drop forward copy field */
    dbal_fields_e cfg_drop_fwd_copy;

    /** Generate recycle copy field */
    dbal_fields_e cfg_gen_recycle_copy;

    /** Keep system headers */
    dbal_fields_e cfg_keep_sys_hdr;

    /** Keep terminated headers */
    dbal_fields_e cfg_keep_term_hdr;

    /** Candidate header enable */
    dbal_fields_e cfg_candidate_hdr_en;

    /** Kill mirror copy */
    dbal_fields_e cfg_kill_mirror_copy;

    /** Kill snoop copy */
    dbal_fields_e cfg_kill_snoop_copy;
} dnx_rx_trap_map_etpp_profile_dbal_fields_t;

/**
 * \brief struct containing statistical object overwrite dbal fields.
 */
typedef struct
{
    /** overwrite field */
    dbal_fields_e overwrite;

    /** index to overwrite field */
    dbal_fields_e index;

    /** value field */
    dbal_fields_e value;

    /** offset by qualifier field */
    dbal_fields_e offset_by_qual;

    /** type field */
    dbal_fields_e type;

    /** is meter field */
    dbal_fields_e is_meter;
} dnx_rx_trap_map_stat_obj_dbal_fields_t;

typedef struct
{
    /** Trap-ID range per block, for verify */
    int lower_bound_trap_id;
    int upper_bound_trap_id;
} dnx_rx_trap_map_ranges_t;

/*
* } Types
*/

/*
 * Globals
 * {
 */

/*
* } Globals
*/

/*
 * Procedure prototypes.
 * {
 */

shr_error_e dnx_rx_trap_is_user_defined(
    int unit,
    dnx_rx_trap_block_e trap_block,
    int trap_id,
    uint8 *is_user_defined_p);

shr_error_e dnx_rx_trap_type_map_info_get(
    int unit,
    bcm_rx_trap_t trap_type,
    dnx_rx_trap_map_type_t * trap_type_info_p);

shr_error_e dnx_rx_trap_ingress_ud_map_info_get(
    int unit,
    int trap_id,
    dnx_rx_trap_map_ingress_ud_t * ingress_ud_info_p);

shr_error_e dnx_rx_trap_map_ingress_ud_alloc_get(
    int unit,
    int alloc_index,
    int *trap_id_p);

shr_error_e dnx_rx_trap_id_boundry_ranges_map_info_get(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_ranges_t * trap_id_range_info_p);

shr_error_e dnx_rx_trap_etpp_lif_profile_map_to_dbal_trap_id(
    int unit,
    dnx_rx_trap_etpp_lif_profiles_e lif_trap_profile,
    dbal_enum_value_field_etpp_trap_id_e * dbal_trap_id_p);

shr_error_e dnx_rx_trap_etpp_fwd_act_profile_map_to_oam_trap_type(
    int unit,
    dbal_enum_value_field_oam_sub_type_e oam_sub_type,
    int fwd_act_profile,
    dnx_rx_trap_etpp_oam_types_e * oam_trap_type_p);

shr_error_e dnx_rx_trap_appl_trap_map_info_get(
    int unit,
    bcm_rx_trap_t trap_type,
    dnx_rx_trap_map_type_t * appl_trap_info_p);

shr_error_e dnx_rx_trap_user_defined_block_map_info_get(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_ud_t * ud_block_info_p);

shr_error_e dnx_rx_trap_stat_obj_map_info_get(
    int unit,
    uint32 stat_obj_profile,
    dnx_rx_trap_map_stat_obj_dbal_fields_t * stat_obj_info_p);

shr_error_e dnx_rx_trap_type_description_map_info_get(
    int unit,
    bcm_rx_trap_t trap_type,
    char **description);

shr_error_e dnx_rx_trap_description_init_check(
    int unit);

/*
 * }
 */

/**
* \brief
*  Retrun list of trap map info for supported per block and class
* \param [in] unit                  - Device ID
* \param [in] trap_block            - Stage to get trap ID for
* \param [in] trap_class            -  Trap class to get Id of
* \param [out] trap_descriptions    - List of trap descriptions
* \param [out] trap_map_list_p      - List of trap map info
* \param [out] nof_traps_p          - number of traps in the list
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_map_list(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_class_e trap_class,
    char **trap_descriptions,
    dnx_rx_trap_map_type_t ** trap_map_list_p,
    int *nof_traps_p);

/**
* \brief
*  Retrun list of application trap map info for supported per block
* \param [in] unit            - Device ID
* \param [in] trap_block      - Stage to get trap ID for
* \param [out] trap_descriptions    - List of trap descriptions
* \param [out] trap_map_list_p  - List of trap map info
* \param [out] nof_traps_p   - number of traps in the list
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_app_map_list(
    int unit,
    dnx_rx_trap_block_e trap_block,
    char **trap_descriptions,
    dnx_rx_trap_map_type_t ** trap_map_list_p,
    int *nof_traps_p);

/**
* \brief
*  Retrun trap type of application trap
* \param [in] unit            - Device ID
* \param [in] trap_block      - trap block
* \param [in] trap_id  - trap_id
* \param [out] trap_type_p   - bcmRxTrap type
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_app_type_from_id_get(
    int unit,
    dnx_rx_trap_block_e trap_block,
    uint32 trap_id,
    bcm_rx_trap_t * trap_type_p);

/**
* \brief
*  Retrun the UserDefined trap information for the requested fwd action profile.
* \param [in] unit                    - Device ID
* \param [in] fwd_action_profile      - ETPP Action Profile
* \param [out] ud_profile_info_p      - Action Profile info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_etpp_ud_profile_map_info_get(
    int unit,
    dnx_rx_trap_etpp_fwd_act_profiles_e fwd_action_profile,
    dnx_rx_trap_map_etpp_profile_dbal_fields_t * ud_profile_info_p);

/**
* \brief
*  Retrun the OAM trap information for the requested fwd action profile.
* \param [in] unit                    - Device ID
* \param [in] oam_action_profile      - ETPP OAM Action Profile
* \param [out] oam_profile_info_p     - OAM Action Profile info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_etpp_oam_profile_map_info_get(
    int unit,
    dnx_rx_trap_etpp_oam_types_e oam_action_profile,
    dnx_rx_trap_map_etpp_profile_dbal_fields_t * oam_profile_info_p);

#endif/*_RX_TRAP_MAP_API_INCLUDED__*/
