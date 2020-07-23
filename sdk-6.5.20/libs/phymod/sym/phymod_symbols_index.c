/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 *
 * PHYMOD Symbol Routines
 */

#include <phymod/phymod_symbols.h>

int
phymod_symbols_index(const phymod_symbols_t *symbols, const phymod_symbol_t *symbol)
{
    int size, i; 

    if (symbols == NULL || symbols->symbols == NULL) {
        return -1;
    }

    size = symbols->size;
    for (i = 0; i < size; i++) {
        /* Sufficient to compare just the pointers */
        if (symbol->name == symbols->symbols[i].name) {
            return i;
        }
    }
    return -1;
}
