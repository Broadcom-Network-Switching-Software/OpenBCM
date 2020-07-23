/*! \file bcma_testutil_symbols.h
 *
 * Symbols operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_SYMBOLS_H
#define BCMA_TESTUTIL_SYMBOLS_H

#include <bcmdrd/bcmdrd_symbols.h>

/*!
 * \brief Symbols filter function.
 *
 * \param [in] symbol Symbol structure (single symbol).
 *
 * \retval 0 on success, -1 on failure.
 */
typedef int (*bcma_testutil_symbols_filter_f)(bcmdrd_symbol_t *symbol);

/*!
 * \brief Symbol table selector structure.
 *
 * This structure contains symbol matching information and filter
 * function information for the symbol table selector function.
 */
typedef struct bcma_testutil_symbol_selector_s {

    /*! Symbol index. */
    uint32_t sindex;

    /*! Symbol flags that must be present. */
    uint32_t pflags;

    /*! Symbol flags that must be absent. */
    uint32_t aflags;

    /*! Symbol info that must be present. */
    uint32_t pinfo;

    /*! Symbol info that must be absent. */
    uint32_t ainfo;

    /*! Symbol filter function. */
    bcma_testutil_symbols_filter_f filter_function;

} bcma_testutil_symbol_selector_t;

/*!
 * \brief Iterate over all symbols in a symbol table and get the filter-in symbol.
 *
 * Search symbol table and select the matched symbol.
 *
 * \param [in] symbols - Symbols information structure for device.
 * \param [in] ss - Symbol selector structure.
 * \param [out] s - Symbol information structure to fill.
 *
 * \retval 0 No errors, otherwise call-back function return value.
 */
extern int
bcma_testutil_symbol_get(
    const bcmdrd_symbols_t *symbols,
    bcma_testutil_symbol_selector_t *ss,
    bcmdrd_symbol_t *s);

#endif /* BCMA_TESTUTIL_SYMBOLS_H */

