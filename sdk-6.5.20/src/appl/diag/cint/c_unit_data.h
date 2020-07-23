/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __C_UNIT_DATA_H__
#define __C_UNIT_DATA_H__

#include <sdk_config.h>

#if defined(INCLUDE_LIB_CINT) && defined(INCLUDE_LIB_C_UNIT)

#include <cint_interpreter.h>
#include <cint_types.h>
#include <appl/c_unit/c_unit_framework.h>
#include <bcm/error.h>

extern cint_data_t c_unit_data; 

extern int c_unit_context_set(c_unit_context target, void* contextToken);

#endif /* defined(INCLUDE_LIB_CINT) && defined(INCLUDE_LIB_C_UNIT) */

#endif /* __C_UNIT_DATA_H__ */
