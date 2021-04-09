 /** \file flow_def.h
 *
 * All dnx flow internal structs, callbacks structures etc.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _DNX_FLOW_DEF_INCLUDED__
/*
 * {
 */
#define _DNX_FLOW_DEF_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>

#define FLOW_STR_MAX 100
#define FLOW_ENABLER_INVALID 0
#define FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS 32
#define FLOW_APP_FIRST_IDX 1
#define FLOW_NO_COMMON_FIELDS 0
#define FLOW_HANDLE_FLAGS_MAX   (16)
#define FLOW_FIELD_MAX_SIZE_UINTS (8) /** corresponds to the definition in bcm_flow_special_field_t (shr_var_uint32_arr)*/

/** MASK FIELD definitions */

/**  Special fields that user can add Mask for them */
#define FLOW_FIELD_IS_MASKABLE(field_id)  \
        ((field_id == DBAL_FIELD_IPV4_SIP) || (field_id == DBAL_FIELD_IPV4_DIP) || (field_id == DBAL_FIELD_IPV6_SIP) || (field_id == DBAL_FIELD_IPV6_DIP))

/** Is the field represent mask */
#define FLOW_FIELD_IS_MASK(field_id)  \
        ((field_id == DBAL_FIELD_SRC_IPV4_MASK) || (field_id == DBAL_FIELD_DST_IPV4_MASK) || (field_id == DBAL_FIELD_SRC_IPV6_MASK) || (field_id == DBAL_FIELD_DST_IPV6_MASK))

/** Each of the above fields should be included here in pairs */
#define FLOW_FIELD_IS_MASK_MATCHING(maskable_field_id, mask_field_id)                                       \
        (((maskable_field_id == DBAL_FIELD_IPV4_SIP) && (mask_field_id == DBAL_FIELD_SRC_IPV4_MASK)) ||     \
        ((maskable_field_id == DBAL_FIELD_IPV4_DIP) && (mask_field_id == DBAL_FIELD_DST_IPV4_MASK)) ||      \
        ((maskable_field_id == DBAL_FIELD_IPV6_SIP) && (mask_field_id == DBAL_FIELD_SRC_IPV6_MASK)) ||      \
        ((maskable_field_id == DBAL_FIELD_IPV6_DIP) && (mask_field_id == DBAL_FIELD_DST_IPV6_MASK)))

/**  Special fields that related to src profile used with FLOW_APP_INIT_IND_SRC_ADDR_PROFILE */
#define FLOW_FIELD_IS_SRC_ADDR_PROFILE_RELATED(field_id)  \
        ((field_id == DBAL_FIELD_IPV4_SIP) || (field_id == DBAL_FIELD_IPV6_SIP) || (field_id == DBAL_FIELD_SOURCE_ADDRESS))

/** Each applicationcan hold up to 4 mask fields (this is a SW limitation, can be changed) */
#define FLOW_MAX_NOF_MASK_FIELDS_PER_APPLICATION    4

/** special field type is uint32 arr */
#define FLOW_SPECIAL_FIELD_TYPE_IS_UINT32_ARR(field_id) \
        (field_id == DBAL_FIELD_BITSTR)

/** number of app indications - up to 32 bits (indication per bit) */
#define FLOW_MAX_NOF_APP_INDICATIONS       32

/** Enum for initiator flow app definition, convention: FLOW_APP_INIT_IND_XXX in order to check the indication use
 *  dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_XXX), when adding new indication need also to
 *  update flow_app_init_indications_string with the related string */
typedef enum
{
    FLOW_APP_INIT_IND_SRC_ADDR_PROFILE, /** \brief IPVX SIP /SRC MAC profile indication, sip profile is supported by application. If this indication is set - instead of writing directly to the lif table, the SIP will allocate profile and write to the profile table DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPVX */
    FLOW_NOF_APP_INIT_IND /** should be last */
}
flow_app_init_indications_e;

/** Enum for termination flow app definition, convention: FLOW_APP_TERM_IND_XXX   */
typedef enum
{
    FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE,     /** \brief qos phb default value indication. If used - default value will be set to pipe. Otherwise - short pipe */
    FLOW_APP_TERM_IND_IGNORE_L2_LEARN_INFO_SET, /** \brief L2 learn info indication to setat application cb instead of the field set */
    FLOW_NOF_APP_TERM_IND /** should be last */
}
flow_app_term_indications_e;

/** Enum for match flow app definition, convention: FLOW_APP_MATCH_IND_XXX   */
typedef enum
{
    FLOW_APP_MATCH_IND_DUMMY, /** \brief Dummy indication, should be removed when the first real indication exists */
    FLOW_NOF_APP_MATCH_IND /** should be last */
}
flow_app_match_indications_e;

/** indicate that application doesn't have any inidications */
#define FLOW_APP_INDICATIONS_NONE    0

/** special field type is uint8 arr */
#define FLOW_SPECIAL_FIELD_TYPE_IS_UINT8_ARR(field_id) \
        ((field_id == DBAL_FIELD_IPV6_DIP) || (field_id == DBAL_FIELD_IPV6_SIP) || (field_id == DBAL_FIELD_DST_IPV6_MASK) || (field_id == DBAL_FIELD_SRC_IPV6_MASK) || (field_id == DBAL_FIELD_SOURCE_ADDRESS))

/** Enum for termination, initiator, match */
typedef enum
{
    /** application related to LIF termination */
    FLOW_APP_TYPE_TERM,

    /** application related to LIF initiator */
    FLOW_APP_TYPE_INIT,

    /** application related to MATCH LIF */
    FLOW_APP_TYPE_MATCH,

    FLOW_APP_TYPE_NOF_TYPES
} dnx_flow_app_type_e;

typedef enum
{
    FIRST_FLOW_FIELD_TYPE = 0,
  /**
   * Field that is part of the Flow common fields.
   */
    FLOW_FIELD_TYPE_COMMON = FIRST_FLOW_FIELD_TYPE,
  /**
   * Field that is defined as a special field.
   */
    FLOW_FIELD_TYPE_SPECIAL,
    NOF_FLOW_FIELD_TYPES
} dnx_flow_field_type_e;

/*
 * Flow command control info (dnx_flow_cmd_control_info_t)
 * dnx_flow_cb_type_e - Defines the type of main FLOW CB that was called 
 * dnx_flow_command_e - Enables to control the command's sequence 
 */
typedef enum
{
    FIRST_FLOW_CB_TYPE = 0,

    FLOW_CB_TYPE_SET = FIRST_FLOW_CB_TYPE,
    FLOW_CB_TYPE_GET,
    FLOW_CB_TYPE_DELETE,

    NOF_FLOW_CB_TYPES
} dnx_flow_cb_type_e;

typedef enum
{
    FIRST_FLOW_COMMAND = 0,

    /*
     * Default command to continue the cb steps as planned
     */
    FLOW_COMMAND_CONTINUE = FIRST_FLOW_COMMAND,
    /*
     * A command to skip the cb steps from this point.
     * Currently supported only from the call to app_specific_operations_cb 
     */
    FLOW_COMMAND_SKIP_TO_END,

    NOF_FLOW_COMMANDS
} dnx_flow_command_e;

typedef struct
{
    dnx_flow_cb_type_e flow_cb_type;
    dnx_flow_command_e flow_command;
} dnx_flow_cmd_control_info_t;

/** Field description cb */
typedef const char *(
    *dnx_flow_field_desc_cb) (
    int unit);

/** Generic callback for various operations per aaplication: set hard coded fields etc */
typedef int (
    *dnx_flow_app_specific_operations_cb) (
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_mgmt_info,
    uint32 *selectable_result_types);

/* 
 *  Generic callback, used for result type selection.
 *  If the function limits the selectable result types, their bitmap is returned in selectable_result_types,
 *  otherwise a zero value means that all the result types are selectable via the result type selection callback.
 */
typedef int (
    *dnx_flow_result_type_select_cb) (
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    void *lif_mgmt_info,
    uint32 *selectable_result_types);

/* 
 * Field value printing function type
 * The function needs to format a string for printing the
 * field's values
 */
typedef int (
    *dnx_flow_field_print_cb) (
    int unit,
    void *val,
    char print_string[FLOW_STR_MAX]);

/** specific application verify cb, flow_info should be casted to the specific applicatopn type - should be
 *  implemented only of application needs dedicated verifications. */
typedef shr_error_e(
    *dnx_flow_verify_cb) (
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields);

/** dnx flow app configuration */
typedef struct
{
    /*
     * Application name
     */
    char app_name[FLOW_STR_MAX];

    /** related DBAL table */
    dbal_tables_e flow_table;

    /*
     * Flow app type(term, init, match)
     */
    dnx_flow_app_type_e flow_app_type;

    /** Bitwise for supported common fields according to BCM_FLOW_INITIATOR_XXX_VALID  */
    uint32 valid_common_fields_bitmap;

    /*
     * specific table special fields
     */
    dbal_fields_e special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];

    /*
     * Bitmap for supported flow handler flags of type BCM_FLOW_HANDLE_INFO_XXX
     */
    uint32 valid_flow_flags;

    /** User can change the default behavior of the application. to see which inidication options see
     *  FLOW_APP_INIT_IND_XXX, FLOW_APP_TERM_IND_XXX, FLOW_APP_MATCH_IND_XXX. in order to set the require indications user
     *  should use SAL_BIT(FLOW_APP_MATCH_IND_XXX) | SAL_BIT(FLOW_APP_MATCH_IND_YYY) */
    uint32 app_indications_bm;

    /*
     * verify callback, if exists will perform application specific verify (if not needed can be set as NULL)
     */
    dnx_flow_verify_cb verify_cb;

    /*
     * Generic application callback, can be used for setting hard coded values, result type picking (outside of result_type_select_cb) etc 
     * For Match add, the cb doesn't receive app_entry_data & lif_mgmt_info 
     * For Match get/delete, the cb doesn't receive app_entry_data, lif_mgmt_info & gport_hw_resources
     * For Terminator/Initiator destroy, the cb doesn't receive special_fields & lif_mgmt_info 
     * Only Set commands receive selectable_result_types
     * NOTE: It's advised to start the function by exiting for unrequired command types: Get/Set/Delete.
     */
    dnx_flow_app_specific_operations_cb app_specific_operations_cb;

    /*
     * Generic callback for marking selectable result types prior to applying the result type 
     * selection algorithm. 
     * The selectable result types are set in the bitmap selectable_result_types
     */
    dnx_flow_result_type_select_cb result_type_select_cb;
} dnx_flow_app_config_t;

/** Generic field cb, for set,get,delete */
typedef int (
    *dnx_flow_field_cb) (
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data);

/** Generic field cb, for replace */
typedef int (
    *dnx_flow_field_replace_cb) (
    int unit,
    uint32 set_entry_handle_id,
    uint32 get_entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data);

/** set,get,delete field cbs */
typedef struct
{
    /** set callback */
    dnx_flow_field_cb field_set_cb;
    /** get callback */
    dnx_flow_field_cb field_get_cb;
    /*
     * replace callback (optional)
     * need to be implemented and used when replace need to perform different actions than the set callback
     * the set callback will be executed when the replace is not implemented
     */
    dnx_flow_field_replace_cb field_replace_cb;
    /** delete callback */
    dnx_flow_field_cb field_delete_cb;
} dnx_flow_field_cbs_t;

/** Special field descriptor */
typedef struct
{
    char field_name[FLOW_STR_MAX];      /* Field name in case we want to overrun the DBAL name */
    dnx_flow_field_desc_cb desc_cb;     /* Field description */
    uint32 field_mapping;       /* default Mapping to dbal field */
    dnx_flow_field_cbs_t field_cbs;     /* Set, Get, Delete callbacks */
} dnx_flow_special_field_desc_t;

/** Common field descriptor */
typedef struct
{
    char field_name[FLOW_STR_MAX];      /* Field name */
    uint32 term_field_enabler;  /* Field matching term enabler from flow.h (i.e - BCM_FLOW_TERMINATOR_ELEMENT_...) */
    uint32 init_field_enabler;  /* Field matching init enabler from flow.h (i.e - BCM_FLOW_INITIATOR_ELEMENT_...) */
    uint8 is_profile;           /* Indicate if field is a profile. Used by diagnostics In case the field is a profile -
                                 * replace cb is usually defined, and the hw writes are done to other tables as well */
    dnx_flow_field_desc_cb desc_cb;     /* Field description callback. If a cb is defined - will be used in diagnostic */
    dnx_flow_field_cbs_t field_cbs;     /* Set, Get, Delete callbacks */
    dnx_flow_field_print_cb print_cb;   /* An optional call back function for printing the field value. If no function
                                         * is supplied - the field will be printed as an integer. */
} dnx_flow_common_field_desc_t;

int dnx_flow_app_is_ind_set(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 ind);

/** Perform get if field is on handle */
shr_error_e dnx_flow_field_value32_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val);

/** Perform get if field is on handle */
shr_error_e dnx_flow_field_value8_get(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *field_val);

const dnx_flow_app_config_t *dnx_flow_app_info_get(
    int unit,
    bcm_flow_handle_t flow_handle_id);

int dnx_flow_number_of_apps(
    void);

int dnx_flow_number_of_special_fields(
    void);

shr_error_e dnx_flow_number_of_common_fields(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int *nof_fields);

shr_error_e dnx_flow_common_fields_desc_by_app_type_get(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int field_idx,
    const dnx_flow_common_field_desc_t ** common_field_desc);

const char *dnx_flow_handle_to_string(
    int unit,
    bcm_flow_handle_t flow_handle_id);

const char *flow_app_type_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type);

const char *flow_app_valid_flag_to_string(
    int unit,
    uint32 flow_handler_flag);

const char *dnx_flow_app_indications_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    uint32 app_inidication);

const char *dnx_flow_common_field_enabler_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    uint32 common_field_enabler);

const char *dnx_flow_special_field_to_string(
    int unit,
    dbal_fields_e field_id);

shr_error_e flow_field_encap_access_print_cb(
    int unit,
    void *val,
    char *print_string);

shr_error_e dnx_flow_field_encap_access_get(
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    dnx_flow_app_config_t * flow_app_info,
    void *field_data);

shr_error_e dnx_flow_common_fields_index_by_enabler_get(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int enabler,
    int *index);

shr_error_e dnx_flow_terminator_info_get_lif_table_manager_info(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    uint32 *entry_handle_id,
    lif_table_mngr_inlif_info_t * inlif_info);

uint32 dnx_flow_gport_to_flow_id(
    int unit,
    uint32 gport);

#endif  /*_DNX_FLOW_DEF_INCLUDED__*/
