/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/test.h>
#include <appl/diag/system.h>
#include <sal/appl/io.h>
#include <sal/compiler.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include "testlist.h"
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>

#if defined(BCM_HELIX5_SUPPORT)

/* initialize the test */
int a72_access_test_init(int uint, args_t *a, void **p)
{
    cli_out("a72_access_test_init...\n");
    return BCM_E_NONE;
}

/* run the test */
int a72_access_test(int unit, args_t *a, void *p)
{
    uint32      regval = 0;
    soc_reg_t   reg;
    int rv;

    reg = ICFG_IPROCPERIPH_COMMONr;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &regval));

    /* For A72 to be up, both UART0_CLK_SELf/UART1_CLK_SELf
       should be 0 */
    if (soc_reg_field_get(unit, reg, regval, UART0_CLK_SELf) == 0) {
        if (soc_reg_field_get(unit, reg, regval, UART1_CLK_SELf) == 0) {
            rv = BCM_E_NONE;
            cli_out("A72 boot complete ...\n");
        } else {
            rv = BCM_E_INTERNAL;
            cli_out("A72 boot failed ...!! \n");
        }
    } else {
        rv = BCM_E_INTERNAL;
        cli_out("A72 boot failed ...!! \n");
    }

    return rv;
}

/* end the test */
int a72_access_test_done(int uint, void *p)
{
    cli_out("A72 access test done ...\n");
    return 0;
}

#endif /* BCM_HELIX5_SUPPORT */
