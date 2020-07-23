/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXC_VERIFY_H__
#define __DNXC_VERIFY_H__

#define DNXC_VERIFICATION

#ifdef DNXC_VERIFICATION

extern int dnxc_verify_allowed[];
extern int dnxc_verify_allowed_fast_init_flag;

#define DNXC_VERIFY_ALLOWED_GET(_unit)\
        dnxc_verify_allowed[unit]

#define DNXC_VERIFY_INVOKE(_expr)\
    if (DNXC_VERIFY_ALLOWED_GET(unit)) \
    {\
        _expr;\
    }

#else
#define DNXC_VERIFY_INVOKE(_expr)
#define DNXC_VERIFY_ALLOWED_GET(_unit) 0
#endif

void dnxc_verify_allowed_init(
    int unit);

void dnxc_verify_allowed_set(
    int unit,
    int value);

int dnxc_verify_allowed_get(
    int unit);

void dnxc_verify_fast_init_enable(
    int enable);

#endif
