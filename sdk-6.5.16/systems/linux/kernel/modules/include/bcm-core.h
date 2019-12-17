/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */
#ifndef __LINUX_BCM_CORE_H__
#define __LINUX_BCM_CORE_H__
#include <soc/cm.h>
#include <soc/enet.h>
#include <bcm/tx.h>

/*
 * The following define allows linux-bcm-net to work with
 * linux-bcm-core as well as with linux-bcm-diag-full.
 */
#define bcore_init system_init

/* Exported functions */

extern int bcore_debug_set_default(void);
extern int bcore_assert_set_default(void);

extern int bcore_sysconf_probe(void);
extern int bcore_sysconf_attach(int unit);
extern int bcore_sysconf_detach(int unit, int clean_shut);

extern int bcore_init(int unit);
extern int bcore_init_all(void);


#ifndef BCM_SYSCONF_INC
#ifdef LKM_2_6
#if  defined(INCLUDE_MACSEC)
#define   BMF_INCLUDE_FOR_LKM
#include "bbase_util.h"
#endif  /* INCLUDE_MACSEC */
#endif  /* LKM_2_6 */
#endif  /* !BCM_SYSCONF_INC */


#endif /* __LINUX_BCM_CORE_H__ */
