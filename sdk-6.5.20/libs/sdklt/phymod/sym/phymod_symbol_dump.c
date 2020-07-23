/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod_symbols.h>

#if PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/*
 * Function:
 *	phymod_symbol_dump
 * Purpose:
 *	Lookup symbol name and display all fields
 * Parameters:
 *	name - symbol name
 *	symbols - symbol table to search
 *	data - symbol data (one or more 32-bit words)
 * Returns:
 *      -1 if symbol not found
 */
int 
phymod_symbol_dump(const char *name, const phymod_symbols_t *symbols,
                   uint32_t *data,
                   int (*print_str)(const char *str))
{
    int rv;
    phymod_symbol_t symbol;

    if ((rv = phymod_symbols_find(name, symbols, &symbol)) == 0) {
        phymod_symbol_show_fields(&symbol, symbols->field_names, data, 0,
                                  print_str,
                                  phymod_symbol_field_filter, data);
    }

    return rv;
}

#endif /* PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS */
