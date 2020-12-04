/** \file diag_dnx_snif.h
 *
 * DNX TM SNIF diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_SNIF_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_SNIF_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/**
* \brief - Enumerator of the Snif types
*/
typedef enum
{
/** Type is invalid */
    DNX_SNIF_TYPE_INVALID = -1,
/** First Snif type */
    DNX_SNIF_TYPE_FIRST = DBAL_ENUM_FVAL_SNIF_TYPE_SNOOP,
/** Snif type is Snoop */
    DNX_SNIF_TYPE_SNOOP = DNX_SNIF_TYPE_FIRST,
/** Snif type is Mirror */
    DNX_SNIF_TYPE_MIRROR = DBAL_ENUM_FVAL_SNIF_TYPE_MIRROR,
/** Snif type is Statistical Sample */
    DNX_SNIF_TYPE_STATISTICAL_SAMPLE = DBAL_ENUM_FVAL_SNIF_TYPE_STATISTICAL_SAMPLE,
/** Number of Snif types */
    DNX_SNIF_TYPE_COUNT
} dnx_snif_type_e;

/**
* \brief - Enumerator of the ingress/egress option
*/
typedef enum
{
/** Type is invalid */
    DNX_SNIF_INVALID = -1,
/** First */
    DNX_SNIF_FIRST = 0,
/** Show ingres attributes */
    DNX_SNIF_INGRESS = DNX_SNIF_FIRST,
/** Show egress attributes */
    DNX_SNIF_EGRESS = 1,
/** Show both attributes */
    DNX_SNIF_COUNT
} dnx_ing_egr_e;


extern sh_sand_option_t sh_dnx_snif_options[];
extern sh_sand_man_t sh_dnx_snif_man;

/**
 * \brief - This function presents the command(profile) attributes for different types of Snif
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_dnx_snif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_SNIF_H_INCLUDED */
