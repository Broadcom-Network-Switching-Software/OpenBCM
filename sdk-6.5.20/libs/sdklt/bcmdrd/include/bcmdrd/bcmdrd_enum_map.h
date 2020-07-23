/*! \file bcmdrd_enum_map.h
 *
 * BCMDRD utility functions for enum name mapping.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_ENUM_MAP_H
#define BCMDRD_ENUM_MAP_H

#include <bcmdrd/bcmdrd_symbol_types.h>

/*!
 * \brief Convert symbol flag to string name.
 *
 * \param [in] flag Symbol flag value(BCMDRD_SYMBOL_FLAG_xxx).
 *
 * \retval String of symbol flag name or NULL if no matching.
 */
extern const char *
bcmdrd_enum_sym_flag_name(uint32_t flag);

/*!
 * \brief Convert symbol flag name to symbol flag value.
 *
 * \param [in] name Symbol flag name.
 *
 * \retval Symbol flag value or 0 if no matching.
 */
extern uint32_t
bcmdrd_enum_sym_flag_val(const char *name);

#endif /* BCMDRD_ENUM_MAP_H */
