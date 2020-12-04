/**
 *  \file bcm_int/dnx/field/field_context.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file is header file  for
 * Field Context procedures for DNX.
 *
 * Context id management and context info parameter
 */

#ifndef DNX_FIELD_CONTEXT_H_INCLUDED
/* { */
#define DNX_FIELD_CONTEXT_H_INCLUDED

/*
 * { Include files
 */

#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_key.h>
#include <include/bcm_int/dnx/field/field_group.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_context_types.h>

/*
 * } Include files
 */
/*
 * Typedefs
 * {
 */

typedef enum
{
    DNX_FIELD_CONTEXT_FLAG_WITH_ID = 0x1,
    DNX_FIELD_CONTEXT_FLAG_NOF
} dnx_field_context_flags_e;

typedef enum
{
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_INVALID = -1,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FIRST = 0,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH_UDH = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FIRST,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH = 1,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH = 2,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_EMPTY = 3,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_PPH = 4,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH = 5,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_UDH = 6,
    DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_NOF,
} dnx_field_context_sys_hdr_profile_e;

/**
 * This enum contains key ids values for the first and second compare keys
 */
typedef enum
{
    DNX_FIELD_CONTEXT_COMPARE_FIRST_KEY = 0,
    DNX_FIELD_CONTEXT_COMPARE_SECOND_KEY = 1,
} dnx_field_context_compare_key_id_e;

/**
 * Future use
 */
typedef enum
{

    DNX_FIELD_CONTEXT_HASH_FLAGS_FIRST = 0x0,
    DNX_FIELD_CONTEXT_HASH_FLAGS_NONE = DNX_FIELD_CONTEXT_HASH_FLAGS_FIRST,
    DNX_FIELD_CONTEXT_HASH_FLAGS_UPDATE = 0x1,
    DNX_FIELD_CONTEXT_HASH_FLAGS_NOF
} dnx_field_context_hash_flags_e;

/**
 * Future use
 */
typedef enum
{

    DNX_FIELD_CONTEXT_COMPARE_FLAGS_FIRST = 0x0,
    DNX_FIELD_CONTEXT_COMPARE_FLAGS_NONE = DNX_FIELD_CONTEXT_HASH_FLAGS_FIRST,
    DNX_FIELD_CONTEXT_COMPARE_FLAGS_NOF
} dnx_field_context_compare_flags_e;

/**
 * Compare pair indicator, indicates which pair of keys should be compared.
 */
typedef enum
{
    DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR = 0x1,
    DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR = 0x2,
} dnx_field_context_compare_pair_e;

/**
 * This struct represents iPMF1 mode parameters, it's used in context create to specify iPMF1 context mode
 */
typedef struct
{
    /** Specifies compare mode for the first pair */
    dnx_field_context_compare_mode_e compare_mode_1;
    /** Specifies compare mode for the second pair */
    dnx_field_context_compare_mode_e compare_mode_2;
    /** Specifies hashing mode */
    dnx_field_context_hash_mode_e hash_mode;
} dnx_field_context_ipmf1_mode_t;

/**
 * This struct represents iPMF2 mode parameters, it's used in context create to specify iPMF2 context mode
 */
typedef struct
{
    /** Specifies which context the current iPMF2 context is cascaded from */
    dnx_field_context_t cascaded_from;
} dnx_field_context_ipmf2_mode_t;

/**
 * This struct represents context mode parameters, it's used in context create to specify both iPMF1/iPMF2 context mode
 */
typedef struct
{
    /** Specifies mode for iPMF1 context */
    dnx_field_context_ipmf1_mode_t context_ipmf1_mode;
    /** Specifies mode for iPMF2 context */
    dnx_field_context_ipmf2_mode_t context_ipmf2_mode;
    /** Specifies state table feature mode */
    dnx_field_context_state_table_mode_e state_table_mode;
    /**
     * Name of the context.
     */
    char name[DBAL_MAX_STRING_LENGTH];
} dnx_field_context_mode_t;

/**
 * This struct represents the apptype info used for creating a dynamic apptype.
 */
typedef struct
{
    /** Specifies the static apptype that the dynamic apptype cascades from. */
    bcm_field_AppType_t base_static_apptype;
    /**
     * Name of the context.
     */
    char name[DBAL_MAX_STRING_LENGTH];
} dnx_field_context_apptype_info_t;

/**
 * This structure represents a single context_key element.
 * The context key element is a collection of quals/quals_info, that by itself can construct a key in PMF (key is
 * defined to be a collection of FFCs).
 * This is an abstract struct used only by other structs as a uniformed way to define 'context_keys'
 */
typedef struct
{
       /**
        * The qualifier order, type and number must be the same as was passed to dnx_field_group_add API
        * Number of meaningful elements on this array: All elements until
        * DNX_FIELD_QUAL_TYPE_INVALID is encountered.
        */
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
       /**
        * Indicate how to read the relevant qualifier from PBUS
        * The information is aligned to the qualifier types array above
        * Number of elements set by dnx_quals array
        */
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];

} dnx_field_context_key_qual_info_t;

/*
 * This structure holds detailed info about hash configuration, needed
 * for bcm_field_context_hash_info_t struct.
 * See dnx_field_context_hash_create().
 */
typedef struct
{
    /**
     * One of the various lb-keys which are iPMF1 output.
     */
    dbal_enum_value_field_field_hash_lb_key_e action_key;
    /**
     * Hashing Action which selects weather to keep the input lb-key or to replace it.
     */
    dbal_enum_value_field_hash_action_e hash_action;
    /**
     * CRC Action which selects CRC Polynomial to apply on hash function ouput (in case hash action is augment).
     */
    dbal_enum_value_field_crc_select_e crc_select;

} dnx_field_context_hash_config_t;

/**
 * This structure holds the information for the hash key used for hashing mechanism.
 * See dnx_field_context_hash_create().
 */
typedef struct
{

    /** The key information */
    dnx_field_context_key_qual_info_t key_info;
    /** Enable/Disable the compression data */
    uint8 compression;
    /** Enable/Disable the order of data */
    uint8 order;
    /** Hashing configuration params, describing which hash action is performed and which signals */
    dnx_field_context_hash_config_t hash_config[DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS];
    /** Hashing polynomial for hashing mechanism. */
    dbal_enum_value_field_context_hash_function_e hash_function;

} dnx_field_context_hash_info_t;

typedef struct
{
    /** The information for the first key*/
    dnx_field_context_key_qual_info_t first_key_info;
    /** The information for the second key*/
    dnx_field_context_key_qual_info_t second_key_info;

} dnx_field_key_compare_pair_info_t;

/*
 * Typedefs
 * }
 */

/**
* \brief
*  Init the Sw state of Field Context module
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_sw_state_init(
    int unit);

/**
* \brief
*  Init the Sw state of Field Apptype module
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_apptype_sw_state_init(
    int unit);

/**
* \brief
*  Inits a dnx_field_mode_t struct.
*  Sets compare_modes to NONE, hashing to disabled as iPMF1 context mode, and sets iPMF2 context mode
*  to not cascaded from any valid context.
* \param [in] unit            - Device ID
* \param [out] context_mode_p - The struct to init / Needs to be allocated by user
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
shr_error_e dnx_field_context_mode_t_init(
    int unit,
    dnx_field_context_mode_t * context_mode_p);

/**
* \brief
*  Inits a dnx_field_context_apptype_info_t struct.
* \param [in] unit            - Device ID
* \param [out] apptype_info_p - The struct to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
shr_error_e dnx_field_context_apptype_info_t_init(
    int unit,
    dnx_field_context_apptype_info_t * apptype_info_p);

/**
* \brief
*  Allocates Context Id in the allowed range per Stage
* \param [in] unit                - Device ID
* \param [in] flags               - Flags for context creation (WITH_ID)
* \param [in] stage               - Field Stage to create context for
* \param [in] context_mode_p      - Context properties struct (see dnx_field_context_types.h)
*                                   Will only take into consideration the substructure related to the field stage.
*                                   At the moment only iPMF1 and iPMF2 have their own substructures.
* \param [in,out] field_context_id_p  - Pointer to Context ID
*                                          As in - Context id to allocate in case WITH ID is set
*                                          As out - Allocated context id by algo
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_create(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_mode_t * context_mode_p,
    dnx_field_context_t * field_context_id_p);

/**
* \brief
*  Allocates Context Id in the allowed range per Stage
* \param [in] unit                - Device ID
* \param [in] flags               - Flags for context creation (WITH_ID)
* \param [in] apptype_info_p      - The structure containing the name of the new apptype and the static apptype
*                                   it cascades from.
* \param [in,out] apptype_p       - Pointer to Apptype.
*                                          As in - Apptype to allocate in case WITH ID is set
*                                          As out - Allocated Apptype by algo
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_apptype_create(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_context_apptype_info_t * apptype_info_p,
    bcm_field_AppType_t * apptype_p);

/**
 * \brief
*  Get the cascaded static apptype of a dynamic apptype.
* \param [in] unit       - Device ID
* \param [in] dynamic_apptype     - The cascading dynamic apptype.
* \param [out] base_static_apptype_p  - The cascaded static apptype.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_apptype_cascaded_get(
    int unit,
    bcm_field_AppType_t dynamic_apptype,
    bcm_field_AppType_t * base_static_apptype_p);

/**
* \brief
*  Get the context apptype info.
* \param [in] unit       - Device ID
* \param [in] apptype     - The apptype for which info will be returned.
* \param [out] apptype_info_p  - Pointer to the structure containing the
*                                name of the apptype and the static
*                                apptype it cascades from.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_apptype_info_get(
    int unit,
    bcm_field_AppType_t apptype,
    dnx_field_context_apptype_info_t * apptype_info_p);

/**
* \brief
*  Get the list of user defined Apptypes that cascade from a predefined Apptype.
* \param [in] unit       - Device ID
* \param [in] predef_app_type  - The parent predefiend Apptype.
* \param [out] nof_child_apptypes_p  - The number of user defiend Apptypes that cascade from the predefine Apptype.
* \param [out] user_app_types  - The user defiend Apptypes that cascade from the predefine Apptype.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_apptype_to_child_apptypes(
    int unit,
    bcm_field_AppType_t predef_app_type,
    int *nof_child_apptypes_p,
    bcm_field_AppType_t user_app_types[DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF]);

/**
* \brief
*  Get the predefined Apptype upon which a user defined apptype is based.
* \param [in] unit       - Device ID
* \param [in] user_app_type       - The user defiend Apptypes that cascade from the predefine Apptype.
* \param [out] predef_app_type_p  - The parent predefiend Apptype.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_user_apptype_to_predef_apptype(
    int unit,
    bcm_field_AppType_t user_app_type,
    bcm_field_AppType_t * predef_app_type_p);

/**
 * \brief
*  Get context compare info of a Field context
* \param [in] unit       - Device ID
* \param [in] context_id     - Field Context ID
* \param [out] context_compare_info_p  - Field Context compare info_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * The mode can be one of the following: Hashing/Compare
* \see
*   * None
*/
shr_error_e dnx_field_context_cmp_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_compare_info_t * context_compare_info_p);

/**
* \brief
*  Init the struct of Field Context Compare info
* \param [in] unit  - Device id
* \param [in] compare_info_p  - context compare struct (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_compare_info_t_init(
    int unit,
    dnx_field_context_compare_info_t * compare_info_p);

/**
* \brief
*  Init the struct of Field Context Hashing info
* \param [in] unit  - Device id
* \param [in] hash_info_p  - context hashing struct (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_hashing_info_t_init(
    int unit,
    dnx_field_context_hashing_info_t * hash_info_p);

/**
* \brief
*  Init the struct of Field Context state table info
* \param [in] unit  - Device id
* \param [in] state_table_info_p  - context state table info struct (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_state_table_info_t_init(
    int unit,
    dnx_field_context_state_table_info_t * state_table_info_p);

/**
* \brief
*  Set context State Table information of a Field context
* \param [in] unit       - Device ID
* \param [in] context_id - Field Context ID
* \param [in] state_table_info_p  - Field State Table info (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * The mode can be one of the following: Hashing/Compare
* \see
*   * None
*/
shr_error_e dnx_field_context_state_table_info_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_state_table_info_t * state_table_info_p);

/**
* \brief
*  Get context hashing information  of a Field context
* \param [in] unit       - Device ID
* \param [in] context_id     - Field Context ID
* \param [out] context_hashing_info_p  - Field Hashing info (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * The mode can be one of the following: Hashing/Compare
* \see
*   * None
*/
shr_error_e dnx_field_context_hashing_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_hashing_info_t * context_hashing_info_p);

/**
* \brief
*  Get context state table information  of a Field context
* \param [in] unit       - Device ID
* \param [in] context_id     - Field Context ID
* \param [out] state_table_info_p  - Field State Table info (see dnx_field_context_types.h)
* \return
*   shr_error_e - Error Type
* \remark
* \see
*   * None
*/
shr_error_e dnx_field_context_state_table_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_state_table_info_t * state_table_info_p);

/**
 * \brief
 *  Get context cascaded from of a Field context.
 * \param [in] unit       - Device ID
 * \param [in] stage      - DNX PMF Stage. Only iPMF2 can use this function.
 * \param [in] context_id - Field Context ID
 * \param [out] cascaded_from_context_id_p  - The iPMF1 context that the iPMF2 context cascades from.
 *                                          Note that all iPMF2 context must have a valid iPMF1 context to cascade from.
 * \return
 *   shr_error_e -
 *     * Error Type
 *     * If 'context_id' has not been allocated for specified stage then
 *       returned error code is '_SHR_E_NOT_FOUND'
 * \remark
 *   *
 * \see
 *   * None
 */
shr_error_e dnx_field_context_cascaded_from_context_id_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t * cascaded_from_context_id_p);

/**
 * \brief
 *   Given a 'main_context_id', which is KNOWN to be allocated and to be on IPMF1,
 *   find a 'cascaded_context_id', on IPMF2, which is cascaded from 'main context_id'
 *   and which is higher, in number, than specified input 'cascaded_context_id'.
 *   If input 'cascaded_context_id' is DNX_FIELD_CONTEXT_ID_INVALID then look for
 *   the first occurrence.
 *   If no such 'cascaded_context_id' is found then the value of
 *   DNX_FIELD_CONTEXT_ID_INVALID is returned.
 * \param [in] unit              -
 *   Device id
 * \param [in] main_field_context_id  -
 *   Context id, assumed to be on IPMF1, to get the list of 'siblings' for (on
 *   IPMF2), one by one.
 * \param [in] include_main_field_context_id  -
 *   In the search, include 'main_field_context_id' itself if it is its own
 *   'sibling'.
 * \param [in,out] cascaded_field_context_id_p  -
 *   \b As \b input:
 *     This is a pointer to 'context_id':
 *     This is the context id to start searching at (look for higher numbers). If set
 *     to DNX_FIELD_CONTEXT_ID_INVALID then look for the first.
 *   \b As \b output:
 *     This procedure loads pointed memory by 'context_id':
 *     This is the 'next' context id. See description above. If no 'next' context id is
 *     found then a value of DNX_FIELD_CONTEXT_ID_INVALID is loaded.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_field_group_fems_context_attach()
 */
shr_error_e dnx_field_context_next_cascaded_find(
    int unit,
    dnx_field_context_t main_field_context_id,
    unsigned int include_main_field_context_id,
    dnx_field_context_t * cascaded_field_context_id_p);

/**
* \brief
*  Destoy created context
* \param [in] unit              - Device id
* \param [in] stage             - field stage
* \param [in] field_context_id  - Context id to destroy
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_destroy(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t field_context_id);

/**
* \brief
*  Get all SW-state context info of a Field context
* \param [in] unit       - Device ID
* \param [in] stage      - field stage
* \param [in] context_id - Field Context ID
* \param [out] context_mode_p  - Field Context info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_mode_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t * context_mode_p);

/**
* \brief
*  Init the struct of  Context compare pair info
* \param [in] unit  - Device id
* \param [in] pair_info_p  - context compare pair info struct (see field_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_compare_pair_info_t_init(
    int unit,
    dnx_field_key_compare_pair_info_t * pair_info_p);

/**
* \brief
*  Init the struct of  Context Hash info
* \param [in] unit  - Device id
* \param [in] hash_info_p  - context Hash info struct (see field_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_hash_info_t_init(
    int unit,
    dnx_field_context_hash_info_t * hash_info_p);

/**
* \brief
*  Configure HASH key which is used to create CRC
* \param [in] unit       - Device ID
* \param [in] flags      - flags (can be UPDATE)
* \param [in] stage      - Field Stage. Only iPMF1 can use this function.
* \param [in] context_id - Field Context ID
* \param [in] hash_info_p  - structure for hash key info (see filed_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * Can be used only for context of iPMF1 stage. Sum of qualifiers bits in hash key must be up to 320 bits.
* \see
*   * None
*/
shr_error_e dnx_field_context_hash_create(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p);

/**
* \brief
*  Get HASH key information which is used to create CRC
* \param [in] unit       - Device ID
* \param [in] stage      - Field Stage. Only iPMF1 can use this function.
* \param [in] context_id - Field Context ID
* \param [in] hash_info_p  - structure for hash key info (see filed_context.h)
* \return
*   shr_error_e - Error Type
* \remark
*   * Can be used only for context of iPMF1 stage. Sum of qualifiers bits in hash key must be up to 320 bits.
*   * If the hash_info_p->hash_config.action_key is specified, the information regarding this action_key
*   * will be stored in hash_config, otherwise, the first action key will be stored.
*   * The information of the rest lb-keys will be stored in additional_hash_config.
* \see
*   * None
*/
shr_error_e dnx_field_context_hash_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_hash_info_t * hash_info_p);

shr_error_e dnx_field_context_state_table_attach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id);

shr_error_e dnx_field_context_state_table_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id);

/**
* \brief
*  destroy HASH key which is used to create CRC, deallocate all resources allocated by dnx_field_context_hash_set.
* \param [in] unit       - Device ID
* \param [in] flags      - flags (future use)
* \param [in] stage      - Field Stage. Only iPMF1 can use this function.
* \param [in] context_id - Field Context ID
* \return
*   shr_error_e - Error Type
* \remark
*   * Can be used only for context of iPMF1 stage.
* \see
*   * None
*/
shr_error_e dnx_field_context_hash_destroy(
    int unit,
    dnx_field_context_hash_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id);

/**
* \brief
*  Creates the compare keys for the given context.
*  There are two pairs of comparison keys in which the user can allocate one of these pairs or both according to his
*  needs, but user needs to make sure beforehand to set the context mode accordingly for keys comparison usage.
*  Context Mode is configured in dnx_field_context_create().
*  This function can be called separately for each pair.
*
* \param [in] unit          - Device ID
* \param [in] flags         - flags (future use)
* \param [in] stage         - Field Stage. Only iPMF1 can use this function.
* \param [in] context_id    - Field Context ID to set the compare-keys for.
* \param [in] pair_id       - Identifier of the pair ID(Can be either 1 or 2, see dnx_field_context_compare_pair_e).
* \param [in] cmp_pair_p    - The pair of keys information. Each key information is composed of
*                             DNX quals(types/info).
*/
shr_error_e dnx_field_context_compare_create(
    int unit,
    dnx_field_context_compare_flags_e flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_key_compare_pair_info_t * cmp_pair_p);

/**
* \brief
*  Retrieves information about the compare keys for the given context.
*  There are two pairs of comparison keys for which information can be retrieve.
*  This function can be called separately for each pair.
*
* \param [in] unit          - Device ID
* \param [in] stage         - Field Stage. Only iPMF1 can use this function.
* \param [in] context_id    - Field Context ID to get the compare-keys for.
* \param [in] pair_id       - Identifier of the pair ID(Can be either 1 or 2, see dnx_field_context_compare_pair_e).
* \param [in] cmp_pair_p    - The pair of keys information. Each key information is composed of
*                             DNX quals(types/info).
*/
shr_error_e dnx_field_context_compare_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_key_compare_pair_info_t * cmp_pair_p);

/**
* \brief
*  Destroys the compare keys in the given context, for both pairs.
*
* \param [in] unit          - Device ID
* \param [in] flags         - TBD
* \param [in] stage         - Field Stage. Only iPMF1 can use this function.
* \param [in] context_id    - Field Context ID
* \param [in] pair_id       - The pair ID
*/
shr_error_e dnx_field_context_compare_destroy(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id);

/**
* \brief
*  Retrieve a context ID from a name.
*
* \param [in] unit          - Device ID
* \param [in] stage         - Field Stage.
* \param [in] name          - Name of the context. Number of elements DBAL_MAX_STRING_LENGTH.
* \param [out] context_id_p - Field Context ID. DNX_FIELD_CONTEXT_ID_INVALID if not found.
*/
shr_error_e dnx_field_context_name_to_context(
    int unit,
    dnx_field_stage_e stage,
    char name[DBAL_MAX_STRING_LENGTH],
    dnx_field_context_t * context_id_p);

/**
* \brief
*  Retrieve an Apptype ID from a name.
*
* \param [in] unit          - Device ID
* \param [in] name          - Name of the Apptype. Number of elements DBAL_MAX_STRING_LENGTH.
* \param [out] apptype_p    - Apptype ID. DNX_FIELD_APPTYPE_INVALID if not found.
*/
shr_error_e dnx_field_context_name_to_apptype(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    bcm_field_AppType_t * apptype_p);

/**
* \brief
*  Init context profile mapping based on ACL context coming from FWD.
* \param [in] unit  - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_profile_map_kbp_context_init(
    int unit);

/**
* \brief
*  run init for Context module, such as default context creation.
* \param [in] unit       - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_init(
    int unit);

/**
* \brief
*  run deinit for Context module.
* \param [in] unit       - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_deinit(
    int unit);

/**
 * \brief
 *  This function initiates the ACL resources.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This function is called from the dnx_general_pp_init() because we need
 *   the PEMLA to finish it's initializations for the key/ffc resource recognition.
 *   EMLA finishes it's initialization after Field module, thus we call the acl_init from
 *   dnx_general_pp_init().
 * \see
 *   * None
 */
shr_error_e dnx_field_context_ifwd2_kbp_acl_init(
    int unit);

/**
* \brief
*  Trigger the NEVER_ADD_LEARN_EXT in the default system header profile
*  in case enable = TRUE , NEVER_ADD_LEARN_EXT=1
* \param [in] unit  - Device id
* \param [in] enable  - enable = TRUE , NEVER_ADD_LEARN_EXT=1
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_default_sys_header_learn_ext(
    int unit,
    uint32 enable);

/**
* \brief
*  Deallocate the Context ID for Field module per Stage
* \param [in] unit        - Device id
* \param [in] stage        - Field Stage the context is in
* \param [in] context_id  - Context id to check
* \param [out] is_allocated_p  - TRUE if context was allocated, false otherwise
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_id_is_allocated(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    uint8 *is_allocated_p);

/**
* \brief
*  Init ACL CPU trap code profile table.
* \param [in] unit  - Device ID
* \param [in] trap_type  - The ingress trap code.
* \param [in] trap_profile  - Trap profile value to be set.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_acl_cpu_trap_code_profile_update(
    int unit,
    int trap_type,
    uint32 trap_profile);

/**
* \brief
*  Get ACL CPU trap code profile of the given trap_type.
* \param [in] unit  - Device ID
* \param [in] trap_type  - The ingress trap code.
* \param [out] trap_profile_p  - Returned trap profile.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_context_acl_cpu_trap_code_profile_get(
    int unit,
    int trap_type,
    uint32 *trap_profile_p);
#endif /* DNX_FIELD_CONTEXT_H_INCLUDED */
