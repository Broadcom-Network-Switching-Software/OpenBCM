/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef   _PHYMOD_SYMOP_H_
#define   _PHYMOD_SYMOP_H_

#include <appl/diag/parse.h>

#include <phymod/phymod.h>
#include <phymod/phymod_symbols.h>

#if defined(PHYMOD_SUPPORT)

#define PHY_FIELD_NAME_MAX      80
#define PHY_FIELDS_MAX          32

typedef struct phy_field_s {
    char name[PHY_FIELD_NAME_MAX + 1];
        uint32 val;
} phy_field_t;

typedef struct phy_field_info_s {
    uint32 regval; /* raw value */
    int num_fields;
    phy_field_t field[PHY_FIELDS_MAX];
} phy_field_info_t;
typedef struct phy_iter_s {
    const char *name;
    const char *found_name;
#define PHY_SHELL_SYM_RAW       0x1     /* Do not decode fields */
#define PHY_SHELL_SYM_LIST      0x2     /* List symbol only */
#define PHY_SHELL_SYM_NZ        0x4     /* Skip if reg/field is zero  */
#define PHY_SHELL_SYM_RESET     0x8     /* Reset register */
    uint32 flags;
    uint32 addr;
    int lane;
    int pll_idx;
    int count;
    const phymod_symbols_t *symbols;
    phymod_phy_access_t *phy_access;
    phy_field_info_t *finfo;
    char tmpstr[64];
} phy_iter_t; 
#endif

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
