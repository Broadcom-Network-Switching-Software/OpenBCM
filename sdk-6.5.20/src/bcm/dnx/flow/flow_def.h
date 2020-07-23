 /** \file flow_def.h
 *
 * All dnx flow internal structs, callbacks structures etc.
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <shared/bsl.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/flow.h>

#define FLOW_MAX_SPECIAL_FIELDS 20
#define FLOW_STR_MAX 100

/** Enum for termination, initiator, match */
typedef enum
{
    /** application related to LIF termonation */
    FLOW_APP_TYPE_TERM,

    /** application related to LIF initiator */
    FLOW_APP_TYPE_INIT,

    /** application related to MATCH LIF */
    FLOW_APP_TYPE_MATCH,

    FLOW_APP_TYPE_NOF_TYPES
} dnx_flow_app_type_e;

/** Generic field cb, for set,get,delete */
typedef int (
    *dnx_flow_field_cb) (
    int unit,
    uint32 entry_handle_id,
    dnx_flow_app_type_e flow_app_type,
    bcm_flow_handle_info_t * flow_handle_info,
    void *field_data);

/** set,get,delete field cbs */
typedef struct
{
    dnx_flow_field_cb field_set_cb;
    dnx_flow_field_cb field_get_cb;
    dnx_flow_field_cb field_delete_cb;
} dnx_flow_field_cbs_t;

/** Field description cb */
typedef const char *(
    *dnx_flow_field_desc_cb) (
    int unit);

/** Common field descriptor */
typedef struct
{
    char field_name[FLOW_STR_MAX];      /* Field name */
    dnx_flow_field_desc_cb *desc_cb;    /* Field description */
    dnx_flow_field_cbs_t field_cbs;     /* Set, Get, Delete callbacks */
} dnx_flow_common_field_desc_t;

/** Special field descriptor */
typedef struct
{
    char field_name[FLOW_STR_MAX];      /* Field name in case we want to overrun the DBAL name */
    dnx_flow_field_desc_cb *desc_cb;    /* Field description */
    uint32 field_mapping;       /* default Mapping to dbal field */
    dnx_flow_field_cbs_t field_cbs;     /* Set, Get, Delete callbacks */
} dnx_flow_special_field_desc_t;

/** Generic application verify cb, flow_info should be casted */
typedef int (
    *dnx_flow_verify_cb) (
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    void *flow_info,
    bcm_flow_special_fields_t * special_fields);

/** Common struct, contains fields which can be shared between term, init, match flow apps */
typedef struct
{
/* application params*/
    uint8 is_verify_disabled;
    uint8 is_error_recovery_disabled;
} dnx_flow_app_common_config_t;

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

    /*
     * Common application configuration 
     */
    dnx_flow_app_common_config_t *flow_app_common_config;

    /*
     * Bitwise for supported common fields
     */
    uint32 valid_common_fields_bitmap;

    /*
     * specific table special fields
     */
    dbal_fields_e special_fields[FLOW_MAX_SPECIAL_FIELDS];

    /*
     * verify cb 
     */
    dnx_flow_verify_cb verify_cb;

    
    /*
     * dnx_flow_result_type_pick_cb result_type_pick_cb;
     */
} dnx_flow_app_config_t;

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

int dnx_flow_init(
    int unit);

int dnx_flow_number_of_apps(
    void);

int dnx_flow_number_of_terminator_common_fields(
    void);

int dnx_flow_number_of_initiator_common_fields(
    void);

int dnx_flow_number_of_special_fields(
    void);

shr_error_e dnx_flow_number_of_common_fields(
    int unit,
    dnx_flow_app_type_e flow_app_type,
    int *nof_fields);
#endif  /*_DNX_FLOW_DEF_INCLUDED__*/
