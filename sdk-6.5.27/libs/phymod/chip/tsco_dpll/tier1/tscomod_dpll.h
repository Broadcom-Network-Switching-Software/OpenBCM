/*
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
/*
 * tscomod_dpll.h
 * Description:  Define enumration and macro
 */

#ifndef _TSCOMOD_DPLL_H_
#define _TSCOMOD_DPLL_H_

#include <phymod/phymod.h>
#include <phymod/phymod_debug.h>

#define TSCO_DPLL_MODEL_NUMBER 0x2A

typedef enum {
    TSCOMOD_DPLL_FCLK_SEL_PLL0    = 0,
    TSCOMOD_DPLL_FCLK_SEL_PLL1    = 1,
    TSCOMOD_DPLL_FCLK_SEL__COUNT
} tscomod_dpll_fclk_sel_t;

extern int tscomod_dpll_fclk_select_set(PHYMOD_ST *pc, int pll_index);
extern int tscomod_dpll_fclk_select_get(PHYMOD_ST *pc, int* pll_index);

#endif /* _TSCOMOD_DPLL_H_ */
