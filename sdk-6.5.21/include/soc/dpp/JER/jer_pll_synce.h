
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DPP_JER_PLL_SYNCE_H
#define _SOC_DPP_JER_PLL_SYNCE_H






typedef enum
{
    
    JER_PLL_TYPE_FABRIC_0 = 0,
    JER_PLL_TYPE_FABRIC_1 = 1,
    
    JER_PLL_TYPE_NIF_PMH = 2,
    JER_PLL_TYPE_NIF_PML_0 = 3,
    JER_PLL_TYPE_NIF_PML_1 = 4,
    
    JER_PLL_TYPE_SYNCE_0 = 5,
    JER_PLL_TYPE_SYNCE_1 = 6,
    
    JER_PLL_TYPE_TS = 7,
    JER_PLL_TYPE_BS = 8,
    
    JER_NOF_PLL_TYPES = 9
}JER_PLL_TYPE;


uint32 jer_synce_clk_div_set(int unit, uint32 synce_idx, ARAD_NIF_SYNCE_CLK_DIV   clk_div);
uint32 jer_synce_clk_div_get(int unit, uint32 synce_idx, ARAD_NIF_SYNCE_CLK_DIV*   clk_div);
uint32 jer_synce_clk_port_sel_set(int unit, uint32 synce_idx, soc_port_t port);
uint32 jer_synce_clk_squelch_set(int unit, uint32 synce_idx, int enable);
uint32 jer_synce_clk_squelch_get(int unit, uint32 synce_idx, int* enable);

int jer_pll_3_set(int unit, JER_PLL_TYPE pll_type, uint32 ndiv, uint32 mdiv, uint32 pdiv, uint32 is_bypass);
int jer_plus_pll_set(int unit, JER_PLL_TYPE pll_type, uint32 ndiv, uint32 mdiv, uint32 pdiv, uint32 is_bypass);

#if 1
#define ENABLE_TS_PLL_HW_WAR
#endif

#endif 

