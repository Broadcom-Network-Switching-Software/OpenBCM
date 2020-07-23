
/*! \file diag_dnx_fabric.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_FABRIC_H_INCLUDED
/* { */
#define DIAG_DNX_FABRIC_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <appl/diag/sand/diag_sand_framework.h>

/*************
 *  DEFINES  *
 */

/*************
 *  MACROS  *
 */

/*************
 * GLOBALS   *
 */

extern sh_sand_man_t dnx_fabric_mesh_man;
extern sh_sand_man_t dnx_fabric_force_man;
extern sh_sand_option_t dnx_fabric_force_options[];

/**
 * \brief - Check is fabric available in this device
 */
extern shr_error_e sh_cmd_is_fabric_available(
    int unit,
    rhlist_t * test_list);
/**
 * \brief
 *   Check if device is from DNX family and fabric is available in the device
 */
extern shr_error_e sh_cmd_is_dnx_with_fabric_available(
    int unit,
    rhlist_t * test_list);
/**
 * \brief
 *   Check if device is from DNX family and fabric is NOT available in the device
 */
extern shr_error_e sh_cmd_is_dnx_with_fabric_unvailable(
    int unit,
    rhlist_t * test_list);
/**
 * \brief - Callback in shell command framework allowing to enable command for DNX MESH device
 */
extern shr_error_e sh_cmd_is_dnx_mesh(
    int unit,
    rhlist_t * test_list);

/**
 * \brief - Callback in shell command framework allowing to enable command for DNX CLOS devices
 */
extern shr_error_e sh_cmd_is_dnx_clos(
    int unit,
    rhlist_t * test_list);

/**
 * \brief - display fabric mesh info
 */
extern shr_error_e cmd_dnx_fabric_mesh(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - fabric force command
 */
extern shr_error_e cmd_dnx_fabric_force(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FABRIC_H_INCLUDED */
