 /** \file flow_def.h
 *
 * All dnx flow internal structs, callbacks structures etc.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _DNX_FLOW_DEF_INCLUDED__
/*
 * {
 */
#define _DNX_FLOW_DEF_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/types.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/flow.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/swstate/auto_generated/types/flow_types.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>

/** general definitions */
#define FLOW_STR_MAX                        (100)
#define FLOW_ENABLER_INVALID                (0)
#define FLOW_MAX_NOF_POSSIBLE_COMMON_FIELDS (32)
#define FLOW_APP_FIRST_IDX                  (1)
#define FLOW_APP_INVALID_IDX                (-1)
#define FLOW_NO_COMMON_FIELDS               (0)
#define FLOW_HANDLE_FLAGS_MAX               (16)
#define FLOW_FIELD_MAX_SIZE_UINTS           (8) /** corresponds to the definition in bcm_flow_special_field_t (shr_var_uint32_arr)*/
#define FLOW_ENUM_MAX_VALUE                 (64)
#define FLOW_MAX_NOF_MATCH_PAYLOAD_APPS     (4)
#define FLOW_NO_MATCH_PAYLOAD_APPS          {"", "", "", ""}
#define FLOW_MAX_NOF_MATCH_DBAL_TABLES      (5)
#define FLOW_LIF_DBAL_TABLE_POS             (0)
#define FLOW_PROFILE_INVALID                (-1)

#define FLOW_LIF_DBAL_TABLE_GET(flow_app)       flow_app->flow_table[FLOW_LIF_DBAL_TABLE_POS]
#define FLOW_LIF_DBAL_TABLE_SET(dbal_table)     {dbal_table, DBAL_TABLE_EMPTY, DBAL_TABLE_EMPTY, DBAL_TABLE_EMPTY, DBAL_TABLE_EMPTY}
#define FLOW_MATCH_DBAL_TABLE_SET(dbal_table)   {dbal_table, DBAL_TABLE_EMPTY, DBAL_TABLE_EMPTY, DBAL_TABLE_EMPTY, DBAL_TABLE_EMPTY}

#define FLOW_APP_IS_NOT_VALID(_flow_app_info)               (_flow_app_info->is_valid_cb && !_flow_app_info->is_valid_cb(unit))

/** number of app indications - up to 32 bits (indication per bit) */
#define FLOW_MAX_NOF_APP_INDICATIONS       (32)

#define FLOW_S_FIELD_UNMAPPED                     DBAL_FIELD_EMPTY

#define FLOW_ENTRY_HANDLE_ID_INVALID              (DBAL_SW_NOF_ENTRY_HANDLES + 1)

/** Match applications MACRO */
#define FLOW_APP_IS_MATCH(flow_app_type) \
        ((flow_app_type == FLOW_APP_TYPE_TERM_MATCH) || (flow_app_type == FLOW_APP_TYPE_INIT_MATCH))

/** Lif applications MACRO */
#define FLOW_APP_IS_LIF(flow_app_type) \
        ((flow_app_type == FLOW_APP_TYPE_TERM) || (flow_app_type == FLOW_APP_TYPE_INIT))

/** in FLOW when getting the field value from handle, not found is a legal return value, all other errors should be
 *  dumped */
#define FLOW_FIELD_GET_ERR_EXIT(rv)        SHR_IF_ERR_EXIT_WITH_MSG_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

/** Enum for initiator flow app definition, convention: FLOW_APP_INIT_IND_XXX in order to check the indication use
 *  dnx_flow_app_is_ind_set(unit, flow_app_info, FLOW_APP_INIT_IND_XXX), when adding new indication need also to
 *  update flow_app_init_indications_string with the related string */
typedef enum
{
    /** \brief IPVX SIP /SRC MAC profile indication, supported only for outlif.
     *  If this indication is set - instead of writing directly to the lif table, the SRC address special field will allocate
     *  profile and write to the profile table DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPVX instead of directly to the lif table */
    FLOW_APP_INIT_IND_SRC_ADDR_PROFILE,
    /** \brief L2_GLOBAL_LIF indication limit the range of the global lif */
    FLOW_APP_INIT_IND_L2_GLOBAL_LIF,
    /** \brief Indication that the application uses no global-LIF, instead a Virtual gport represents the local entry */
    FLOW_APP_INIT_IND_VIRTUAL_LIF_ONLY,
    /** \brief Indication that the application can use Global-LIF or non Global-LIF (Virtual) entries */
    FLOW_APP_INIT_IND_VIRTUAL_LIF_SUPPORTED,
    /** \brief Indication that the application key isn't a local In-LIF/Out-LIF, but a combination of match keys */
    FLOW_APP_INIT_IND_NO_LOCAL_LIF,
    /** \brief Indication means that common field l3_intf must be set */
    FLOW_APP_INIT_IND_MANDATORY_L3_INTF,
    /** \brief Indication means that common field l3_intf will be ignored in the fields cb operation and its configurations should be handled in the app specific cb */
    FLOW_APP_INIT_IND_IGNORE_L3_INTF,
    /** \brief Indication that the source address profile LSB should be masked before writing into the lif table */
    FLOW_APP_INIT_IND_SRC_ADDR_PROFILE_LSB,
    /** \brief Indication that the phy DB management is shared between flow and other applications, so when user performs
     *         create, the entry may already exist*/
    FLOW_APP_INIT_IND_SHARED_PHY_DB_MANAGEMENT,
    /** \brief Indication that the application has ESEM command profile specific logic for MPLS Port*/
    FLOW_APP_INIT_IND_ESEM_CMD_PROFILE_MPLS_PORT,
    /** \brief Indication that the application has ESEM command profile specific logic for MPLS EVPN*/
    FLOW_APP_INIT_IND_ESEM_CMD_PROFILE_EVPN,
    FLOW_NOF_APP_INIT_IND /** should be last */
}
flow_app_init_indications_e;

/** Enum for termination flow app definition, convention: FLOW_APP_TERM_IND_XXX  update
 *  flow_app_term_indications_string with the related string  */
typedef enum
{
    FLOW_APP_TERM_IND_QOS_PHB_DEFAULT_PIPE,     /** \brief qos phb default value indication. If used - default value will be set to pipe. Otherwise - short pipe */
    FLOW_APP_TERM_IND_QOS_RM_DEFAULT_PIPE,      /** \brief qos remark default value indication. If used - default value will be set to pipe. Otherwise - short pipe */
    FLOW_APP_TERM_IND_QOS_TTL_DEFAULT_PIPE,     /** \brief qos TTL default value indication. If used - default value will be set to pipe. Otherwise - short pipe */
    FLOW_APP_TERM_IND_VIRTUAL_LIF_ONLY,         /** \brief Indication that the application uses no global-LIF, instead a Virtual gport represents the local entry */
    FLOW_APP_TERM_IND_VIRTUAL_LIF_SUPPORTED,    /** \brief Indication that the application can use Global-LIF or non Global-LIF entries */
    FLOW_APP_TERM_IND_SUPPORT_EXTENDED_TERM,    /** \brief Indication that the application can support extended termination */
    FLOW_APP_TERM_IND_L2_GLOBAL_LIF,            /** \brief L2_GLOBAL_LIF indication limit the range of the global lif */
    FLOW_APP_TERM_IND_SUPPORT_MPLS_PROFILE,     /** \brief in case of used, the dedicated logic will be executed*/
    FLOW_APP_TERM_IND_L2_LEARN_OPTIMIZATION,    /** \brief L2 learn info indication to enable Optimization learning (In case the learn destination is a physical port
                                                 *         that matches the src-sys-port, it is possible to use optimization learning of the src-sys-port instead of
                                                 *         specifying the port in the learn-info). Optimization enables learning of multiple ports using the same LIF. */
    FLOW_NOF_APP_TERM_IND /** should be last */
}
flow_app_term_indications_e;

/** Enum for Term Match flow app definition, convention: FLOW_APP_TERM_MATCH_IND_XXX */
typedef enum
{
     /** \brief using this indication to allow that many applications will relate to the same DBAL table. */
    FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE,

    /** \brief using this indication allow to define multiple DBAL tables that will be related to the app, note that in
     *         this case all the tables must be with the same type (TCAM/ EM/ direct) */
    FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES,

    /** \brief using this indication means that the application can add entries to a specific core */
    FLOW_APP_TERM_MATCH_IND_PER_CORE,

    /** \brief using this indication means that the application uses key field to identify BUD entries (unlike dbal table)
     *         when using this indication no need to define the second_pass table in the app definitions. if this ind is
     *         used, user should handle the related key field in the specific_app_cb.
     */
    FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD,

    FLOW_NOF_APP_TERM_MATCH_IND     /** should be last */
}
flow_app_term_match_indications_e;

/** Enum for Init Match flow app definition, convention: FLOW_APP_INIT_MATCH_IND_XXX */
typedef enum
{
    FLOW_APP_INIT_MATCH_IND_DUMMY,  /** \brief Dummy indication, should be removed when the first real indication is introduced */
    FLOW_NOF_APP_INIT_MATCH_IND     /** should be last */
}
flow_app_init_match_indications_e;

/** indicate that application doesn't have any indications */
#define FLOW_APP_INDICATIONS_NONE    0

/** Enum for termination, initiator, match application type */
typedef enum
{
    /** An Application that represents a LIF Terminator */
    FLOW_APP_TYPE_TERM,

    /** An Application that represents a LIF Initiator */
    FLOW_APP_TYPE_INIT,

    /** An Application that represents a Terminator LIF Match */
    FLOW_APP_TYPE_TERM_MATCH,

    /** An Application that represents an Initiator LIF Match */
    FLOW_APP_TYPE_INIT_MATCH,

    FLOW_APP_TYPE_NOF_TYPES
} dnx_flow_app_type_e;

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
    FLOW_CB_TYPE_TRAVERSE,

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
     * A command to skip most of the steps that follow the cb call, except for the finalization steps. 
     * Suitable in cases where the cb replaces some steps, but the entry is valid some additional finalization steps are required. 
     * In a match traverse - If the entry is handled and valid, there's still a need to call the user cb
     * Currently supported only from the call to app_specific_operations_cb 
     */
    FLOW_COMMAND_FINALIZE,
    /*
     * A command to skip the steps that follow the cb call.
     * Currently supported only from the call to app_specific_operations_cb 
     */
    FLOW_COMMAND_SKIP_TO_END,

    NOF_FLOW_COMMANDS
} dnx_flow_command_e;

/** Special Fields enums */

/** special fields enum - for each special field should be an enum value and also a struct definition located in dnx_flow_spcial_fields in the same order*/
typedef enum
{
    FLOW_S_F_EMPTY = 0,
    FLOW_S_F_IPV4_DIP,
    FLOW_S_F_IPV4_DIP_MASK,
    FLOW_S_F_IPV4_SIP,
    FLOW_S_F_IPV4_SIP_MASK,
    FLOW_S_F_IPV6_DIP,
    FLOW_S_F_IPV6_DIP_MASK,
    FLOW_S_F_IPV6_SIP,
    FLOW_S_F_IPV6_SIP_MASK,
    FLOW_S_F_IPV6,
    FLOW_S_F_IPV6_MASK,
    FLOW_S_F_SRC_MAC_ADDRESS,
    FLOW_S_F_DST_MAC_ADDRESS,

    FLOW_S_F_TUNNEL_ENDPOINT_IDENTIFIER,
    FLOW_S_F_NEXT_LAYER_NETWORK_DOMAIN,
    FLOW_S_F_MPLS_PHP,
    FLOW_S_F_MPLS_LABEL,
    FLOW_S_F_MPLS_LABEL_2,
    FLOW_S_F_MPLS_LABEL_3,
    FLOW_S_F_MPLS_LABEL_4,
    FLOW_S_F_MPLS_LABEL_5,
    FLOW_S_F_MPLS_LABEL_6,
    FLOW_S_F_MPLS_LABEL_7,
    FLOW_S_F_MPLS_LABEL_8,
    FLOW_S_F_VRF,

    FLOW_S_F_SRC_UDP_PORT,
    FLOW_S_F_DST_UDP_PORT,
    FLOW_S_F_IPV4_HEADER_DF,
    FLOW_S_F_IPVX_INIT_GRE_KEY_USE_LB,
    FLOW_S_F_IPVX_INIT_GRE_WITH_SN,

    FLOW_S_F_SVTAG_TXSCI,

    FLOW_S_F_VLAN_EDIT_PROFILE,
    FLOW_S_F_VLAN_EDIT_PCP_DEI_PROFILE,
    FLOW_S_F_VLAN_EDIT_VID_1,
    FLOW_S_F_VLAN_EDIT_VID_2,
    FLOW_S_F_VSI_ASSIGNMENT_MODE,
    FLOW_S_F_PORT,
    FLOW_S_F_IN_PORT,
    FLOW_S_F_MAPPED_PORT,
    FLOW_S_F_VLAN_DOMAIN,
    FLOW_S_F_GLOB_IN_LIF,
    FLOW_S_F_VLAN_PORT_SVTAG_PORT,

    FLOW_S_F_VID,
    FLOW_S_F_S_VID,
    FLOW_S_F_C_VID,
    FLOW_S_F_S_VID_1,
    FLOW_S_F_S_VID_2,
    FLOW_S_F_C_VID_1,
    FLOW_S_F_C_VID_2,
    FLOW_S_F_VSI,
    FLOW_S_F_UNKNOWN_UNICAST_DEFAULT_DESTINATION,
    FLOW_S_F_UNKNOWN_MULTICAST_DEFAULT_DESTINATION,
    FLOW_S_F_BROADCAST_DEFAULT_DESTINATION,

    FLOW_S_F_VID_OUTER_VLAN,
    FLOW_S_F_VID_INNER_VLAN,
    FLOW_S_F_PCP_DEI_OUTER_VLAN,
    FLOW_S_F_TUNNEL_CLASS,

    FLOW_S_F_QOS_DSCP,
    FLOW_S_F_QOS_EXP,
    FLOW_S_F_QOS_EXP_1,
    FLOW_S_F_QOS_PRI,
    FLOW_S_F_QOS_CFI,
    FLOW_S_F_QOS_PCP,

    FLOW_S_F_BFR_BIT_STR,
    FLOW_S_F_BIER_BIFT_ID,
    FLOW_S_F_BIER_BSL,
    FLOW_S_F_BIER_RSV,
    FLOW_S_F_BIER_OAM,
    FLOW_S_F_BIER_BFIR_ID,

    /** the FLOW_S_F_IPV4_TUNNEL_TYPE is relevant only for match applications. for lif applications, need to set app specific operation */
    FLOW_S_F_IPV4_TUNNEL_TYPE,
    FLOW_S_F_IPV6_TUNNEL_TYPE,
    FLOW_S_F_IPVX_TUNNEL_TYPE,
    FLOW_S_F_LAYER_TYPE,

    FLOW_S_F_MPLS_ENTROPY_LABEL,
    FLOW_S_F_MPLS_ENTROPY_LABEL_INDICATION,
    FLOW_S_F_MPLS_CONTROL_WORD_ENABLE,
    FLOW_S_F_MPLS_PLATFORM_NAMESPACE,
    FLOW_S_F_MPLS_INCLUSIVE_MULTICAST_LABEL,
    FLOW_S_F_MPLS_NOF_SERVICE_TAGS,

    FLOW_S_F_MPLS_VCCV_TYPE,
    FLOW_S_F_MPLS_EXPECT_BOS,
    FLOW_S_F_MPLS_EXPECT_NON_BOS,
    FLOW_S_F_MPLS_TRAP_TTL_0,
    FLOW_S_F_MPLS_TRAP_TTL_1,
    FLOW_S_F_MATCH_LABEL,

    FLOW_S_F_VSI_MATCH,
    FLOW_S_F_CLASS_ID_MATCH,
    FLOW_S_F_NAME_SPACE_MATCH,
    FLOW_S_F_OUT_LIF_MATCH,
    FLOW_S_F_VID_MATCH,
    FLOW_S_F_SYSTEM_PORT_MATCH,

    FLOW_S_F_RCH_VRF,
    FLOW_S_F_RCH_OUTLIF_0_PUSHED_TO_STACK,
    FLOW_S_F_RCH_P2P,
    FLOW_S_F_RCH_DESTINATION,
    FLOW_S_F_RCH_ADDITIONAL_BYTES_TO_STRIP,
    FLOW_S_F_RCH_CONTROL_VLAN_PORT,

    FLOW_S_F_REFLECTOR_PROCESS_TYPE,
    FLOW_S_F_REFLECTOR_SRC_MAC,
    FLOW_S_F_REFLECTOR_EGRESS_LAST_LAYER,

    FLOW_S_F_GTP_EXT_HEADER_TYPE,
    FLOW_S_F_GTP_EXT_SSN_TYPE,
    FLOW_S_F_GTP_EXT_SSN_RQI,

    FLOW_S_F_NEXT_OUTLIF_POINTER,
    FLOW_S_F_NATIVE_INDEX_MODE,
    FLOW_S_F_OAM_REFLECTOR_RAW_DATA,
    FLOW_S_F_VNI,
    NOF_FLOW_SPECIAL_FIELDS
} flow_special_fields_e;

typedef enum
{
    FLOW_PAYLOAD_TYPE_UINT32,
    FLOW_PAYLOAD_TYPE_UINT32_ARR,
    FLOW_PAYLOAD_TYPE_UINT8_ARR,

    /** the value is mapped to a enum created inside the SDK (enum itseld is not exposed to customers). field description cb
     *  & print cb must exists. Field print cb should reflect all possible values. Note
     *  that the max enum value is FLOW_ENUM_MAX_VALUE */
    FLOW_PAYLOAD_TYPE_ENUM,

    /** the value is mapped to an enum from the BCM level, the value used by this field is the int symbol - when using this
     *  type for a field the mapped 1. description callback must be defined example - "Legal values are taken from
     *  bcm_encap_access_t" 2. Print callback Must be defined in the field description and all legal values should be
     *  mapped to a value. Note that the max enum value is FLOW_ENUM_MAX_VALUE. */
    FLOW_PAYLOAD_TYPE_BCM_ENUM,

    /** this type requires not value and uses for flag fields. If exists the flag is set, and the not value is used.
     *  if the user wants to remove it, he need to set its is_clear bit in a replace operation
     *  thus, from get point of view, it either exists if there is value or not if the value is zeroed
     *  for enablers which are also mandatory, the field can be set in replace but still cannot be cleared.
     */
    FLOW_PAYLOAD_TYPE_ENABLER,

    FLOW_NOF_PAYLOAD_TYPES
} flow_payload_type_e;

/** Enum for special_field indications, when adding new indication need also to
 *  update dnx_flow_special_fields_indications_to_string with the related string */
typedef enum
{
    /** when adding this field a mask can be added as well, this fields has a corresponding mask field to indicate the
     *  mask value */
    FLOW_SPECIAL_FIELD_IND_MASK_SUPPORTED,

    /** when adding this field a mask can be added as well, this fields has a corresponding mask field to indicate the
     *  mask value */
    FLOW_SPECIAL_FIELD_IND_IS_MASK,

    /** when set, the field will not perform any operation when doing set/get/delete/replace - the operation is done
     *  using another field. this is relevant for values that are constructed from multiple inputs and creating one
     *  profile. The indication is only relevant for LIF applications*/
    FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION,

    /** Works similar to FLOW_SPECIAL_FIELD_IND_LIF_IGNORE_OPERATION, but relevant only for match applications instead */
    FLOW_SPECIAL_FIELD_IND_MATCH_IGNORE_OPERATION,

    /** when set, the field is able to perform special logic for setting the field in the src profile */
    FLOW_SPECIAL_FIELD_IND_SRC_ADDR_PROFILE_SUPPORTED,

    /** when set, the field is mandatory for lif applications must be set in create, and cannot be cleared in replace
     *  for enabler fields, where the field has no value, the field can be set not in create, but once set - it cannot be cleared
     */
    FLOW_SPECIAL_FIELD_IND_LIF_MANDATORY,

    /** when set, the field is mandatory for match applications must be set in create, and cannot be cleared in replace */
    FLOW_SPECIAL_FIELD_IND_MATCH_MANDATORY,

    /** when set, for lif applications, once set, this field cannot be changed or cleared in replace */
    FLOW_SPECIAL_FIELD_IND_LIF_UNCHANGEABLE,

    /** when set, the field is part of the key, relevant for non-match applications */
    FLOW_SPECIAL_FIELD_IND_IS_KEY,

    FLOW_NOF_SPECIAL_FIELDS_IND /** should be last */
} flow_special_fields_indications_e;

typedef struct
{
    dnx_flow_cb_type_e flow_cb_type;
    dnx_flow_command_e flow_command;
} dnx_flow_cmd_control_info_t;

 /*
  * Generic callback for various operations per application: set hard coded fields etc. current_entry_handle_id is the
  * handle_entry which is handled currently (relevant for create,set,get,traverse, etc..) For replace cases, also given 
  * prev_entry_handle_id which is the handle to the prev entry that should be replaced 
  */
typedef int (
    *dnx_flow_app_specific_operations_cb) (
    int unit,
    uint32 current_entry_handle_id,
    uint32 prev_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *selectable_result_types);

typedef int (
    *dnx_flow_is_valid_cb) (
    int unit);

typedef int (
    *dnx_flow_is_entry_related_cb) (
    int unit,
    uint32 entry_handle_id,
    int *is_entry_related);

typedef enum
{
    FLOW_MAP_TYPE_NONE,

    /** used for operations such as set, get & delete when the key is known and the dbal table should be decided using the key */
    FLOW_MAP_TYPE_KEY_FIELD_TO_TABLE,

    /** used for traverse operations, when the dbal table is known and it is required to retrieve the key field value */
    FLOW_MAP_TYPE_TABLE_TO_KEY_FIELD,

    FLOW_NOF_MAP_TYPES
} flow_key_field_to_table_map_type_e;

/** callback that map between dbal table and key field. must be defined when flag
 *  FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES is set, should map between dbal table and key fields note:
 *  entry_handle_id is valid only when map_type==FLOW_MAP_TYPE_TABLE_TO_KEY_FIELD */
typedef int (
    *dnx_flow_match_dbal_table_to_key_field_map_cb) (
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    flow_key_field_to_table_map_type_e map_type,
    bcm_flow_special_fields_t * key_special_fields,
    dbal_tables_e * selected_dbal_table);

/* 
 *  callback used for result type selection.
 *  If the function limits the selectable result types, their bitmap is returned in selectable_result_types,
 *  otherwise a zero value means that all the result types are selectable via the result type selection callback.
 *  get_entry_handle_id - valid only in replace, otherwise equal to -1, it should be used in replace to understand the best result type including the existing fields.
 */
typedef int (
    *dnx_flow_result_type_select_cb) (
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    void *app_entry_data,
    bcm_flow_special_fields_t * special_fields,
    uint32 *selectable_result_types);

/** specific application verify cb, flow_info should be casted to the specific applicatopn type - should be
 *  implemented only of application needs dedicated verifications. Only in relace operation
 *  (BCM_FLOW_HANDLE_INFO_REPLACE) get_entry_handle is valid and represents the entry that should be replaced.. (user
 *  can validate againts existing values) */
typedef shr_error_e(
    *dnx_flow_verify_cb) (
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_cmd_control_info_t * flow_cmd_control,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields,
    uint32 get_entry_handle);

/** dnx flow app configuration */
typedef struct
{
    /*
     * Application name, in order to expose all the application names easily a define should be add to bcm_int/dnx/flow/flow.h
     */
    char app_name[FLOW_STR_MAX];

    /** related DBAL tables, for LIF applications only one DBAL table is supported. use FLOW_LIF_DBAL_TABLE_SET() to add the
     *  table. for match applications up to FLOW_MAX_NOF_MATCH_DBAL_TABLES can be added, in case that more than one DBAL
     *  table is added need to add the indication FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES */
    dbal_tables_e flow_table[FLOW_MAX_NOF_MATCH_DBAL_TABLES];

    /** Second Pass parm is valid when BCM_FLOW_HANDLE_INFO_BUD is used. if FLOW_APP_TERM_MATCH_IND_BUD_BY_KEY_FIELD is set
     *  the parameter is used as the key field ID that indicate second_pass. otherwise it means the related DBAL table */

    int second_pass_param;

    /*
     * Flow app type(term, init, match)
     */
    dnx_flow_app_type_e flow_app_type;

    /*
     * For initiator applications, default mapping for encap access when encap access =  bcmEncapAccessInvalid
     * (if no default mapping set to bcmEncapAccessInvalid)
     */
    bcm_encap_access_t encap_access_default_mapping;

    /*
     * Match payload Flow applications - A list of applications that this Match can point to. 
     * This field is applicable only for Match applications.
     */
    const char match_payload_apps[FLOW_MAX_NOF_MATCH_PAYLOAD_APPS][FLOW_STR_MAX];

    /** Bitwise for supported common fields according to BCM_FLOW_INITIATOR_XXX_VALID  */
    uint32 valid_common_fields_bitmap;

    /*
     * specific table special fields
     */
    flow_special_fields_e special_fields[BCM_FLOW_SPECIAL_FIELD_MAX_NOF_FIELDS];

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

    /*
     *  Generic callback, used to indicate if the app is valid. usually the app should be
     *  valid according to dnx_data
     */
    dnx_flow_is_valid_cb is_valid_cb;

    /*
     *  Valid for match applications only, in case there are many match applications that mapped to the same
     *  DBAL table, When performing travesrse there is a need to check if the entry is related to the app,
     *  since there are many application that shared the dbal_table. To learn if the entry is related user can
     *  use: sw state, result type, key fields, LIF related table or more. Related indication: FLOW_APP_TERM_MATCH_IND_MANY_TO_ONE
     */
    dnx_flow_is_entry_related_cb is_entry_related_cb;

    /** this call back is used when FLOW_APP_TERM_MATCH_IND_MULTIPLE_DBAL_TABLES is set. used to decide what is the dbal
     *  table to use for the operation (in set/get/delete). in traverse the cb is used to map between the dbal table to the
     *  key fields, in this case the function should update the key_special_fields value. (look for the require key field
     *  and update its value. please note that when using this callback need to update the ut special value get to return
     *  the related value according to the dbal table. */
    dnx_flow_match_dbal_table_to_key_field_map_cb dbal_table_to_key_field_map_cb;

} dnx_flow_app_config_t;

#define DNX_FLOW_HANDLE_VALIDATE(handle_id)                                  \
    if ((handle_id >= dnx_flow_number_of_apps()) || (handle_id < 1))         \
    {                                                                        \
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal flow handle ID, %d", handle_id); \
    }

/** special fields callbacks definitions */

/** Field description cb */
typedef const char *(
    *flow_special_field_desc_cb) (
    int unit);
/* 
 * Field value printing function type
 * The function needs to format a string for printing the
 * field's values
 */
typedef int (
    *flow_special_field_print_cb) (
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX]);

typedef int (
    *flow_special_field_cb) (
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    /** the field idx in special_fields to use */
    int field_idx,
    /** array of all special fields (incase other fields will be required in the cb)   */
    bcm_flow_special_fields_t * special_fields);

/** should return NOT_FOUND in case the field was not set on handle, else should return the value to the user in
 *  special_field */
typedef int (
    *flow_special_field_get_cb) (
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id,
    /** instance of a special field to update it's value */
    bcm_flow_special_field_t * special_field);

/** recieves the entry handle ID after get operation was done (all field values are on the handle) */
typedef int (
    *flow_special_field_clear_cb) (
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    flow_special_fields_e special_field_id);

typedef int (
    *flow_special_field_enum_string_to_enum_val_cb) (
    int unit,
    char *input_str,
    int *enum_val);

typedef int (
    *flow_special_field_replace_cb) (
    int unit,
    uint32 entry_handle_id,
    uint32 get_entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_field_t * special_field);

/** Special field descriptor cprresponds to the flow_special_fields_e enum */
typedef struct
{
    /** Field name (exposed to user)*/
    char name[FLOW_STR_MAX];

    /** in case the field has simple mapping (1x1 to dbal field) this parameter will be used for dbal operations, in case
     *  of more complex mapping (such as profiles) it is also recommanded to set the related dbal field (for example
     *  profile field). But The mapping will be done by the set/get/delete CBs or app_specific_cb */
    uint32 mapped_dbal_field;

    /** field's payload type, in case of enum user must define description and print cb */
    flow_payload_type_e payload_type;

    /** link to another special field following links exists MASKABLE to MASK or PART of profile */
    flow_special_fields_e linked_special_field;

    /** Field description callback. If a cb is defined - will be used in diagnostic */
    flow_special_field_desc_cb description;

    /** Optional call back for printing the field value. If no function is supplied - the field value will be printed
     *  as an integer. */
    flow_special_field_print_cb print;

    /** field indication bit map each bit indicate property according to FLOW_SPECIAL_FIELD_IND_ */
    uint32 indications_bm;

    /** field operations call backs - in case of no direct dbal mapping should be implemented, for match applications
     *  only "set" callback is used since it is a key field */
    flow_special_field_cb set;
    flow_special_field_get_cb get;
    flow_special_field_clear_cb clear; /** this is optional - only for profiles */

    
    flow_special_field_enum_string_to_enum_val_cb enum_string_to_enum_val;

} flow_special_field_info_t;

/** Field description cb */
typedef const char *(
    *dnx_flow_field_desc_cb) (
    int unit);

/** Field value printing function type. The function formats a string for printing the field's values */
typedef int (
    *dnx_flow_field_print_cb) (
    int unit,
    dnx_flow_app_type_e flow_app_type,
    void *val,
    char print_string[FLOW_STR_MAX]);

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

/** Generic field cb, for replace
 *  Mainly used for profile fields */
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

    /** delete callback (optional)
     *  Needs to be implemented and used when removing a field requires some resources handling (mainly profiles).
     *  This callback is also called when performing replace, if a field relevant valid_elements_clear bit is set and delete cb exists.
     *  In this case - the get() cb is called, followed by the delete cb */
    dnx_flow_field_cb field_delete_cb;
} dnx_flow_field_cbs_t;

/** Common field descriptor */
typedef struct
{
    char field_name[FLOW_STR_MAX];      /* Field name */
    uint32 term_field_enabler;  /* Field matching term enabler from flow.h (i.e - BCM_FLOW_TERMINATOR_ELEMENT_...) */
    uint32 init_field_enabler;  /* Field matching init enabler from flow.h (i.e - BCM_FLOW_INITIATOR_ELEMENT_...) */
    uint8 is_profile;           /* Indicate if field is a profile, there is a dedicated logic in order to set the
                                 * field. Used by diagnostics in case the field is a profile - the hw writes are done
                                 * to other tables as well */
    /** In case the field has simple mapping (1x1 to dbal field) this parameter will be used for dbal operations, in case of
     *  more complex mapping (such as profiles) it is also recommanded to set the related dbal field (for example profile
     *  field). But The mapping will be done by the set/get/delete CBs or dedicated logic */
    uint32 mapped_dbal_field;
    dnx_flow_field_desc_cb desc_cb;     /* Field description callback. If a cb is defined - will be used in diagnostic */
    dnx_flow_field_cbs_t field_cbs;     /* Set, Get, Delete callbacks */
    dnx_flow_field_print_cb print_cb;   /* An optional call back function for printing the field value. If no function
                                         * is supplied - the field will be printed as an integer. */
} dnx_flow_common_field_desc_t;

/** internal function prototypes   */
int dnx_flow_app_is_ind_set(
    int unit,
    const dnx_flow_app_config_t * flow_app_info,
    uint32 ind);

int dnx_flow_special_field_is_ind_set(
    int unit,
    const flow_special_field_info_t * field,
    uint32 ind);

const dnx_flow_app_config_t *dnx_flow_app_info_get(
    int unit,
    bcm_flow_handle_t flow_handle_id);

int dnx_flow_number_of_apps(
    void);

bcm_flow_handle_t dnx_flow_handle_by_app_name_get(
    int unit,
    const char *flow_app_name,
    dnx_flow_app_type_e flow_app_type);

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

shr_error_e flow_special_field_info_get(
    int unit,
    flow_special_fields_e field_id,
    const flow_special_field_info_t ** special_field);

shr_error_e flow_special_field_value_print(
    int unit,
    bcm_flow_special_field_t * special_field,
    char print_string[FLOW_STR_MAX]);

/** for mask fields, there is a special handling in order to perform "get", both the value and the mask are retrieved
 *  with one call. */
shr_error_e flow_special_fields_mask_field_value_get(
    int unit,
    int entry_handle_id,
    const flow_special_field_info_t * curr_field,
    bcm_flow_special_fields_t * key_special_fields,
    int key_idx);

/*Get printable special field value accordint to its payload type*/
shr_error_e flow_special_field_printable_string_get(
    int unit,
    flow_special_fields_e field_id,
    bcm_flow_special_field_t * special_field,
    char print_buffer[FLOW_STR_MAX]);

shr_error_e flow_special_fields_mask_field_value_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    int field_idx,
    bcm_flow_special_fields_t * special_fields);

shr_error_e flow_special_field_form_dbal_field_info_get(
    int unit,
    dbal_fields_e dbal_field_id,
    flow_special_fields_e * field_id);

shr_error_e flow_special_fields_get_one_field(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    uint32 entry_handle_id,
    flow_special_fields_e special_field_id,
    const flow_special_field_info_t * curr_field,
    bcm_flow_special_field_t * special_field);

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

const char *dnx_flow_special_fields_indications_to_string(
    int unit,
    flow_special_fields_indications_e special_field_inidication);

const char *dnx_flow_payload_types_to_string(
    int unit,
    flow_payload_type_e payload_type);

const char *dnx_flow_common_field_enabler_to_string(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    uint32 common_field_enabler);

const char *dnx_flow_special_field_to_string(
    int unit,
    flow_special_fields_e field_id);

const char *flow_encap_access_to_string(
    int unit,
    bcm_encap_access_t encap_access);

shr_error_e dnx_flow_common_fields_index_by_enabler_get(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int enabler,
    int *index);

shr_error_e dnx_flow_special_fields_set(
    int unit,
    uint32 entry_handle_id,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources,
    bcm_flow_special_fields_t * special_fields);

shr_error_e flow_special_field_key_direct_mapping_get(
    int unit,
    flow_special_field_info_t * curr_field,
    uint32 entry_handle_id,
    dbal_table_type_e table_type,
    bcm_flow_special_field_t * special_field);

shr_error_e dnx_flow_commit_error_check(
    int unit,
    int entry_handle_id);

#endif  /*_DNX_FLOW_DEF_INCLUDED__*/
