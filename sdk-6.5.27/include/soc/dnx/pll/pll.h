/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DNX_PLL_INIT_H
#define _SOC_DNX_PLL_INIT_H

typedef enum
{
    DNX_PLL1_TYPE_INVALID = -1,
    DNX_PLL1_TYPE_FIRST = 0,

    DNX_PLL1_TYPE_TS = DNX_PLL1_TYPE_FIRST,
    DNX_PLL1_TYPE_BS = 1,

    DNX_PLL1_TYPE_NIF = 2,

    DNX_PLL1_TYPE_FAB = 3,

    DNX_PLL1_TYPE_FLEXE = 4,

    DNX_PLL1_TYPE_COUNT
} DNX_PLL1_TYPE;

typedef enum
{
    DNX_PLL3_TYPE_INVALID = -1,
    DNX_PLL3_TYPE_FIRST = 0,

    DNX_PLL3_TYPE_NIF0 = DNX_PLL3_TYPE_FIRST,
    DNX_PLL3_TYPE_NIF1 = 1,

    DNX_PLL3_TYPE_MAS0 = 2,
    DNX_PLL3_TYPE_MAS1 = 3,

    DNX_PLL3_TYPE_COUNT
} DNX_PLL3_TYPE;

typedef enum
{
    DNX_PLL4_TYPE_INVALID = -1,
    DNX_PLL4_TYPE_FIRST = 0,

    DNX_PLL4_TYPE_TS = DNX_PLL4_TYPE_FIRST,
    DNX_PLL4_TYPE_BS = 1,

    DNX_PLL4_TYPE_NIF = 2,

    DNX_PLL4_TYPE_FAB = 3,

    DNX_PLL4_TYPE_NIF_TSC = 4,

    DNX_PLL4_TYPE_FRAMER = 5,

    DNX_PLL4_TYPE_SAR = 6,

    DNX_PLL4_TYPE_COUNT
} DNX_PLL4_TYPE;

int soc_dnx_synce_pll_set(
    int unit,
    int synce_index,
    uint32 ndiv,
    uint32 mdiv,
    uint32 pdiv,
    uint32 fref,
    uint32 is_bypass);

int soc_dnx_synce_pll_power_enable_get(
    int unit,
    int synce_index,
    uint32 *enable);

int soc_dnx_synce_pll_power_enable_set(
    int unit,
    int synce_index,
    int enable);

int soc_dnx_pre_reset_pll_init(
    int unit);
int soc_dnx_post_reset_pll_init(
    int unit);

int soc_dnx_pll_init(
    int unit,
    int pre_reset);

shr_error_e soc_dnx_pll_3_div_get(
    int unit,
    DNX_PLL3_TYPE pll_type,
    uint32 *ndiv,
    uint32 *mdiv,
    uint32 *pdiv);

int soc_dnx_pll_1_init_all(
    int unit);

int soc_dnx_pll_3_init_all(
    int unit);

int soc_dnx_pll_4_init_all(
    int unit);

#endif
