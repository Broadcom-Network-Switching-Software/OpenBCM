/** \file bcm_int/dnx/synce/synce.h
 *
 * Functions for handling synce init sequences.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SYNCE_INCLUDED__
/*
 * {
 */

#define _SYNCE_INCLUDED__

/*************
 * INCLUDES  *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * DEFINES   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */
/*
 * }
 */

/*************
 * ENUMS     *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * TYPE DEFS *
 */
/*
 * {
 */
typedef enum
{
    /**
     *  Synchronous Ethernet signal - differential (two signals
     *  per clock) recovered clock, two differential outputs
     */
    DNX_NIF_SYNCE_MODE_TWO_DIFF_CLK = 0,
    /**
     *  Synchronous Ethernet signal - recovered clock accompanied
     *  by a valid indication, two clk+valid outputs
     */
    DNX_NIF_SYNCE_MODE_TWO_CLK_AND_VALID = 1,
    /**
     *  Number of types in ARAD_NIF_SYNCE_MODE
     */
    DNX_NIF_SYNCE_NOF_MODES = 2
} SOC_DNX_PLL_NIF_SYNCE_MODE;

typedef enum
{
    /**
     *  SyncE reference clock PM divider 1
     */
    DNX_NIF_SYNCE_PM_DIV_1 = 0,
    /**
     *  SyncE reference clock PM divider 7
     */
    DNX_NIF_SYNCE_PM_DIV_7 = 1,
    /**
     *  SyncE reference clock PM divider 11
     */
    DNX_NIF_SYNCE_PM_DIV_11 = 2,

    DNX_NIF_SYNCE_NOF_PM_DIVIDERS = 3
} DNX_SYNCE_PM_DIVIDER;
/*
 * }
 */

/*************
 * GLOBALS   *
 */
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */
shr_error_e dnx_synce_init(
    int unit);

shr_error_e dnx_synce_deinit(
    int unit);

int dnx_synce_port_set(
    int unit,
    uint32 synce_index,
    bcm_port_t port);

int dnx_synce_port_get(
    int unit,
    uint32 synce_index,
    bcm_port_t * port);

int dnx_synce_divider_set(
    int unit,
    uint32 synce_index,
    int divider);

int dnx_synce_divider_get(
    int unit,
    uint32 synce_index,
    int *divider);

int dnx_nif_synce_enable_get(
    int unit,
    int core,
    int *enable);

int dnx_nif_synce_enable_set(
    int unit,
    int core,
    int enable);

int dnx_nif_synce_ref_clk_div_get(
    int unit,
    bcm_port_t port,
    int *pm_synce_div,
    int *nif_synce_div);
/*
 * }
 */
#endif /* __SYNCE_INCLUDED__ */
