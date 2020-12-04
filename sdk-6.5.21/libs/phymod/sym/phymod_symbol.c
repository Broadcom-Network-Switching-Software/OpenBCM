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
#include <phymod/phymod_system.h>


#if PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

static const void *
__phymod_symbol_find(const char *name, const void *table, int size, int entry_size)
{
    int i; 
    phymod_symbol_t *sym;
    unsigned char *ptr = (unsigned char*)table; 

    if (table == NULL) {
        return NULL;
    }
    
    for (i = 0; (sym = (phymod_symbol_t*)(ptr)) && (i < size); i++) {
	if (PHYMOD_STRCMP(sym->name, name) == 0) {
	    return (void*) sym; 
	}
#if PHYMOD_CONFIG_INCLUDE_ALIAS_NAMES == 1
	if (sym->ufname && PHYMOD_STRCMP(sym->ufname, name) == 0) {
	    return (void*) sym; 
	}
	if (sym->alias && PHYMOD_STRCMP(sym->alias, name) == 0) {
	    return (void*) sym; 
	}
#endif
	ptr += entry_size; 
    }

    return NULL; 
}
	

const phymod_symbol_t *
phymod_symbol_find(const char *name, const phymod_symbol_t *table, int size)
{
    return (phymod_symbol_t*) __phymod_symbol_find(name, table, size, sizeof(phymod_symbol_t)); 
}


int 
phymod_symbols_find(const char *name, const phymod_symbols_t *symbols, phymod_symbol_t *rsym)
{
    const phymod_symbol_t *s = NULL; 

    if (rsym == NULL) return -1; 

    if ((symbols->symbols) && (s = phymod_symbol_find(name, symbols->symbols, symbols->size))) {
	*rsym = *s;
	return 0; 
    }
    return -1;
}

#endif
