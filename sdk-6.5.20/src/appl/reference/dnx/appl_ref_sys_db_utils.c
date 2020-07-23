/** \file appl_ref_sys_db_utils.c
 * 
 *
 * System data base utils procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INITSEQDNX

/** Include files. */
/** { */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/property.h>
#include <soc/drv.h>
#include <appl/reference/dnx/appl_ref_sys_ports.h>
#include "appl_ref_sys_db_utils.h"
/** } */

/** DEFINEs */
/** { */
/** } */

/** MACROs */
/** { */
/** } */

/**
 * \brief - interpret given token to int value
 */
static shr_error_e
appl_dnx_sys_db_utils_token_interpret(
    int unit,
    char *token,
    int *value)
{
    int length;
    char *end_ptr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    if (token == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "token is invalid (NULL)");
    }

    /** check token length, if 0 return error */
    length = sal_strlen(token);
    if (length == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "token read is invalid (size is 0)");
    }

    /** convert token to integer */
    *value = sal_ctoi(token, &end_ptr);
    /** check end ptr to know if all chars were used, if not return an error */
    if (end_ptr - token != length)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid token received, token \"%s\" is not ok", token);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - interpret given port_type token to int value
 */
static shr_error_e
appl_dnx_sys_db_utils_token_interpret_port_type(
    int unit,
    char *token,
    int *value)
{
    int length;
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check - port type is none if NULL */
    if (token == NULL)
    {
        *value = APPL_PORT_TYPE_NONE;
        SHR_EXIT();
    }

    /** check token length, if 0 set to none - meaning no special handling is required for this port */
    length = sal_strlen(token);
    if (length == 0)
    {
        *value = APPL_PORT_TYPE_NONE;
        SHR_EXIT();
    }

    /** convert token to integer */
    if (sal_strcmp(token, "TDM") == 0)
    {
        *value = APPL_PORT_TYPE_TDM;
    }
    else if (sal_strcmp(token, "OLP") == 0)
    {
        *value = APPL_PORT_TYPE_OLP;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid token received, token \"%s\" is not OK", token);
    }

exit:
    SHR_FUNC_EXIT;
}

/*** \brief - interpret "appl_param_sys_port_X = <dev>:<port>:<core>:<tm_port>:<rate>" appl soc property, see header */
shr_error_e
appl_dnx_sys_db_utils_prop_interpret(
    int unit,
    char *sys_port_prop,
    system_port_mapping_t * sys_port_map)
{
    char *saveptr = NULL;
    char *token;
    int value;
    char copied_prop[SOC_PROPERTY_NAME_MAX];

    SHR_FUNC_INIT_VARS(unit);

    /** copy soc property to parse - parsing is done on soc itself so need to copy it in order not to mess it up */
    sal_strncpy(copied_prop, sys_port_prop, SOC_PROPERTY_NAME_MAX);
    /** this is to make coverity happy - even though there's no way that this str is not NULL terminated */
    copied_prop[SOC_PROPERTY_NAME_MAX - 1] = 0;

    /** read first token - device index */
    token = sal_strtok_r(copied_prop, ":", &saveptr);
    SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_token_interpret(unit, token, &value));
    sys_port_map->device_index = value;

    /** read next token - local port */
    token = sal_strtok_r(NULL, ":", &saveptr);
    SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_token_interpret(unit, token, &value));
    sys_port_map->local_port = value;

    /** read next token - core */
    token = sal_strtok_r(NULL, ":", &saveptr);
    SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_token_interpret(unit, token, &value));
    sys_port_map->core = value;

    /** read next token - TM port */
    token = sal_strtok_r(NULL, ":", &saveptr);
    SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_token_interpret(unit, token, &value));
    sys_port_map->tm_port = value;

    /** read next token -  rate */
    token = sal_strtok_r(NULL, ":", &saveptr);
    SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_token_interpret(unit, token, &value));
    sys_port_map->rate = value;

    /** read next token -  port_type */
    token = sal_strtok_r(NULL, ":", &saveptr);
    SHR_IF_ERR_EXIT(appl_dnx_sys_db_utils_token_interpret_port_type(unit, token, &value));
    sys_port_map->port_type = value;

exit:
    SHR_FUNC_EXIT;
}
