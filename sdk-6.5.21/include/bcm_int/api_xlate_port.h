/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM internal port translation functions
 */

#ifndef _BCM_XLATE_PORT_H
#define _BCM_XLATE_PORT_H

#include <sal/core/libc.h> /* sal_memcpy */
#include <bcm/types.h>

extern int _bcm_api_xlate_port_init(int unit);

extern int _bcm_api_xlate_port_cleanup(int unit);

extern int _bcm_api_xlate_port_map(int unit, bcm_port_t aport, bcm_port_t pport);

extern int _bcm_api_xlate_port_a2p(int unit, bcm_port_t *port);

extern int _bcm_api_xlate_port_p2a(int unit, bcm_port_t *port);

extern int _bcm_api_xlate_port_pbmp_a2p(int unit, bcm_pbmp_t *pbmp);

extern int _bcm_api_xlate_port_pbmp_p2a(int unit, bcm_pbmp_t *pbmp);

extern int _bcm_xlate_sysport_s2p(int unit, bcm_port_t *port);

extern int _bcm_xlate_sysport_p2s(int unit, bcm_port_t *port);

#ifdef INCLUDE_BCM_API_XLATE_PORT

#define BCM_API_XLATE_PORT_A2P(_u,_p)           _bcm_api_xlate_port_a2p(_u,(bcm_port_t*)(_p))
#define BCM_API_XLATE_PORT_P2A(_u,_p)           _bcm_api_xlate_port_p2a(_u,(bcm_port_t*)(_p))
#define BCM_API_XLATE_PORT_DECL(_var)           bcm_port_t _var = 0
#define BCM_API_XLATE_PORT_ASSIGN(_dst,_src)    _dst = _src
#define BCM_API_XLATE_PORT_SAVE(_dst,_src)      BCM_API_XLATE_PORT_ASSIGN(_dst,_src)
#define BCM_API_XLATE_PORT_RESTORE(_dst,_src)   BCM_API_XLATE_PORT_ASSIGN(_dst,_src)

#define BCM_API_XLATE_PORT_PBMP_A2P(_u,_p)      _bcm_api_xlate_port_pbmp_a2p(_u,(_p))
#define BCM_API_XLATE_PORT_PBMP_P2A(_u,_p)      _bcm_api_xlate_port_pbmp_p2a(_u,(_p))
#define BCM_API_XLATE_PORT_PBMP_DECL(_t)        bcm_pbmp_t _t
/* We avoid BCM_PBMP_ASSIGN since this may cause (invalid) compiler warnings */
#define BCM_API_XLATE_PORT_PBMP_ASSIGN(_t,_p)   sal_memcpy(&(_t),&(_p),sizeof(_t))
#define BCM_API_XLATE_PORT_PBMP_SAVE(_t,_p)     BCM_API_XLATE_PORT_PBMP_ASSIGN(_t,_p)
#define BCM_API_XLATE_PORT_PBMP_RESTORE(_p,_t)  BCM_API_XLATE_PORT_PBMP_ASSIGN(_p,_t)

#define BCM_API_XLATE_PORT_COND(_c)             if(_c)
#define BCM_API_XLATE_PORT_ITER_DECL(_i)        int _i
#define BCM_API_XLATE_PORT_ITER(_c,_i)          for(_i=0;_i<(_c);_i++)

#else

#define BCM_API_XLATE_PORT_A2P(_u,_p)
#define BCM_API_XLATE_PORT_P2A(_u,_p)
#define BCM_API_XLATE_PORT_DECL(_var)
#define BCM_API_XLATE_PORT_ASSIGN(_dst,_src)
#define BCM_API_XLATE_PORT_SAVE(_dst,_src)
#define BCM_API_XLATE_PORT_RESTORE(_dst,_src)

#define BCM_API_XLATE_PORT_PBMP_A2P(_u,_p)
#define BCM_API_XLATE_PORT_PBMP_P2A(_u,_p)
#define BCM_API_XLATE_PORT_PBMP_DECL(_t)
#define BCM_API_XLATE_PORT_PBMP_ASSIGN(_t,_p)
#define BCM_API_XLATE_PORT_PBMP_SAVE(_t,_p)
#define BCM_API_XLATE_PORT_PBMP_RESTORE(_p,_t)

#define BCM_API_XLATE_PORT_COND(_c)
#define BCM_API_XLATE_PORT_ITER_DECL(_t)
#define BCM_API_XLATE_PORT_ITER(_c,_i)

#endif /* BCM_INCLUDE_API_XLATE_PORT */

#define BCM_XLATE_SYSPORT_S2P(_u,_p)            _bcm_xlate_sysport_s2p(_u,(bcm_port_t*)(_p))
#define BCM_XLATE_SYSPORT_P2S(_u,_p)            _bcm_xlate_sysport_p2s(_u,(bcm_port_t*)(_p))

#endif /* _BCM_XLATE_PORT_H */

