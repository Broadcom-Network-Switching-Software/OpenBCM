/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Register address and value manipulations for CMICx.
 */

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <soc/register.h>
#include <soc/drv.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/mcm/cmicx.h>

soc_cmicx_reg_t cmicx_regs[] = CMICX_REG_INIT;

STATIC soc_cmicx_reg_t
*soc_cmicx_srch (int unit, uint32 addr) {
    int start = 0;
    int end = NUM_CMICX_REGS - 1;
    int mid = (start + end) >> 1;
    while (start <= end && cmicx_regs[mid].addr != addr) {
        if (cmicx_regs[mid].addr > addr) {
            end = mid - 1;
        } else {
            start = mid + 1;
        }
        mid = (start + end) >> 1;
    }
    return (cmicx_regs[mid].addr == addr && SOC_REG_IS_VALID(unit, cmicx_regs[mid].reg)) ? &(cmicx_regs[mid]) : NULL;

}

soc_cmicx_reg_t
*soc_cmicx_reg_get (uint32 idx) {
    return &(cmicx_regs[idx]);
}

soc_reg_t
soc_cmicx_addr_reg (int unit, uint32 addr) {
    soc_cmicx_reg_t *cmreg = soc_cmicx_srch(unit, addr);
    return (cmreg == NULL)?INVALIDr:cmreg->reg;
}

char *
soc_cmicx_addr_name (int unit, uint32 addr) {
    soc_cmicx_reg_t *cmreg = soc_cmicx_srch(unit,addr);
    return (cmreg == NULL)?"???":cmreg->name;
}

/*
 * Parse a CMICX register name and return offset.  Matching is
 * case-insensitive and works with or without the "CMIC_" prefix.
 */

int
soc_parse_cmicx_regname(int unit, char *name, uint32 *offset_ptr) {
    char *s;
    int i;
    soc_cmicx_reg_t *creg;
    for (i = 0; i < NUM_CMICX_REGS; i++) {
        creg = soc_cmicx_reg_get(i);
        s = creg->name;
        if (sal_strcasecmp(name, s) == 0 || sal_strcasecmp(name, s + 5) == 0) {
            *offset_ptr = creg->addr;
            return 0;
        }
    }
    return -1;
}
#endif
