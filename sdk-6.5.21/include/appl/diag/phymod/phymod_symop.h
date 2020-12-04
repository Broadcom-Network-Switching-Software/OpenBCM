/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef   _PHYMOD_SYMOP_H_
#define   _PHYMOD_SYMOP_H_

#include <appl/diag/parse.h>

#include <phymod/phymod.h>
#include <phymod/phymod_symbols.h>

extern int
phymod_sym_info(int unit, int port, int intermediate, phymod_symbols_iter_t *iter,
                phymod_phy_access_t *pm_acc);

extern int
phymod_symop_init(phymod_symbols_iter_t *iter, args_t *a);

extern int
phymod_symop_exec(phymod_symbols_iter_t *iter, const phymod_symbols_t *symbols,
                  phymod_phy_access_t *pm_acc, char *hdr);

extern int
phymod_symop_cleanup(phymod_symbols_iter_t *iter);

#endif /* _PHYMOD_SYMOP_H_ */
