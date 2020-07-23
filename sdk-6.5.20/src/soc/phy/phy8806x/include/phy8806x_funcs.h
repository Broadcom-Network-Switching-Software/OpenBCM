/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy8806x_funcs.h
 */

#ifndef   _PHY8806X_FUNCS_H_
#define   _PHY8806X_FUNCS_H_

#include <sal/types.h>
#include "phy8806x_syms.h"

extern mt2_sym_t * mt2_syms_find_name(char *c);
extern mt2_sym_t **mt2_syms_find_entries(char *c);
extern int mt2_syms_find_next_name(char *c, mt2_sym_t **mt2_sym, int startidx);
extern int mt2_test_run(int unit, uint16 phyaddr, uint32 testno, int silent);
extern int mt2_diag_run(int unit, uint16 phy_addr, char *cmd_str, char *para);

#endif  /* _PHY8806X_FUNCS_H_ */

