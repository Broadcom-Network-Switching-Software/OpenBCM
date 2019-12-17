/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <shared/bslext.h>

#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/io.h>

#include <soc/drv.h>

#include <appl/diag/sysconf.h>

#include <bcm-core.h>

int
sysconf_probe(void)
{
    return 0;
}

int
sysconf_attach(int unit)
{
    
    if ( (soc_attached(unit)) ||
        (CMDEV(unit).dev.info->dev_type & SOC_ETHER_DEV_TYPE) ) {
        printk("OK - already attached\n");
        return 0;
    }
    return -1;
}

int
sysconf_detach(int unit)
{
    return 0;
}

int
sysconf_init(void)
{
    return 0;
}
