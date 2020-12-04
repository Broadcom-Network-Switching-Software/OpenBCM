/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Asynchronous BCM API Server
 */

#ifndef	_BCM_INT_ASYNC_SERVER_H
#define	_BCM_INT_ASYNC_SERVER_H

#ifdef	BCM_ASYNC_SUPPORT

#include <bcm_int/async_req.h>

extern void	bcm_async_add(bcm_async_req_t *req);
extern int	bcm_async_start(void);
extern int	bcm_async_stop(void);
extern void	bcm_async_run(bcm_async_req_t *req);

#endif	/* BCM_ASYNC_SUPPORT */
#endif	/* _BCM_INT_ASYNC_SERVER_H */
