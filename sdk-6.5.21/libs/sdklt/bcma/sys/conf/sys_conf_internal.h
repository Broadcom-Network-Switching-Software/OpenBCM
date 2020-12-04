/*! \file sys_conf_internal.h
 *
 * Systemn configuration definitions intended for internal use only.
 *
 * Application code should normally not need to include this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SYS_CONF_INTERNAL_H
#define SYS_CONF_INTERNAL_H

#include <sal/sal_internal.h>

/*! Signature for sanity checks. */
#define SYS_CONF_SIGNATURE      SAL_SIG_DEF('S', 'Y', 'S', 'C')

/*! Check that this is a valid SYS_CONF object. */
#define BAD_SYS_CONF(_sc)   ((_sc)->sys_conf_sig != SYS_CONF_SIGNATURE)

#endif /* SYS_CONF_INTERNAL_H */
