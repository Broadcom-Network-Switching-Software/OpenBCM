/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_PSTATS_H
#define _BCM_PSTATS_H

#if defined(INCLUDE_PSTATS)
#include <soc/defs.h>
#include <soc/types.h>

typedef int (*_bcm_pstats_session_create_f)(int, int, int,
                                            bcm_pstats_session_element_t*,
                                            bcm_pstats_session_id_t*);
typedef int (*_bcm_pstats_session_destroy_f)(int, bcm_pstats_session_id_t);
typedef int (*_bcm_pstats_session_get_f)(int, bcm_pstats_session_id_t,
                                         int,
                                         bcm_pstats_session_element_t*,
                                         int*);
typedef int (*_bcm_pstats_data_sync_f)(int);
typedef int (*_bcm_pstats_session_data_get_f)(int, bcm_pstats_session_id_t,
                                              int,
                                              bcm_pstats_timestamp_t*,
                                              int,
                                              bcm_pstats_data_t*, int*);
typedef int (*_bcm_pstats_session_data_clear_f)(int, bcm_pstats_session_id_t);

#ifdef BCM_WARM_BOOT_SUPPORT
typedef int (*_bcm_pstats_scache_size_get_f)(int, uint32*);
typedef int (*_bcm_pstats_reinit_f)(int, uint8**);
typedef int (*_bcm_pstats_sync_f)(int, uint8**);
#endif
typedef int (*_bcm_pstats_session_traverse_f)(int, bcm_pstats_session_traverse_cb,
                                              void*);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
typedef void (*_bcm_pstats_sw_dump_f)(int);
#endif

typedef struct _bcm_pstats_unit_driver_s {
    sal_mutex_t                             pstats_lock;
    _bcm_pstats_session_create_f            session_create;
    _bcm_pstats_session_destroy_f           session_destroy;
    _bcm_pstats_session_get_f               session_get;
    _bcm_pstats_data_sync_f                 data_sync;
    _bcm_pstats_session_data_get_f          session_data_get;
    _bcm_pstats_session_data_clear_f        session_data_clear;

#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_pstats_scache_size_get_f           scache_size_get;
    _bcm_pstats_reinit_f                    pstats_reinit;
    _bcm_pstats_sync_f                      pstats_sync;
#endif
    _bcm_pstats_session_traverse_f          session_traverse;

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    _bcm_pstats_sw_dump_f                   pstats_sw_dump;
#endif
} _bcm_pstats_unit_driver_t;

extern _bcm_pstats_unit_driver_t *_bcm_pstats_unit_driver[BCM_MAX_NUM_UNITS];
#define _BCM_PSTATS_UNIT_DRIVER(u) _bcm_pstats_unit_driver[(u)]

#define PSTATS_LOCK(u)\
        do {\
            if (_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock){\
                sal_mutex_take(_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock,\
                               sal_mutex_FOREVER);\
            }\
        }while(0)
#define PSTATS_UNLOCK(u)\
        do {\
            if (_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock){\
                sal_mutex_give(_BCM_PSTATS_UNIT_DRIVER(u)->pstats_lock);\
            }\
        }while(0)

extern int _bcm_pstats_init(int unit);
extern void _bcm_pstats_deinit(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_esw_pstats_scache_size_get(int unit, uint32 *scache_size);
extern int _bcm_esw_pstats_reinit(int unit, uint8 **scache_ptr);
extern int _bcm_esw_pstats_sync(int unit, uint8 **scache_ptr);
#endif
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_esw_pstats_sw_dump(int unit);
#endif

#endif /* INCLUDE_PSTATS */
#endif /* _BCM_PSTATS_H */
