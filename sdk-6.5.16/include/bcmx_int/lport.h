/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        lport.h
 * Purpose:     BCMX Internal Logical and User Port definitions
 */

#ifndef   _BCMX_INT_LPORT_H_
#define   _BCMX_INT_LPORT_H_

#include <bcm/types.h>
#include <bcmx/types.h>

typedef struct _bcmx_port_s         _bcmx_port_t;
typedef struct _bcmx_uport_hash_s   _bcmx_uport_hash_t;

/* BCMX Logical Port */
struct _bcmx_port_s {
    int             bcm_unit;    /* Unit port belongs to */
    bcm_port_t      bcm_port;    /* Port number relative to bcm unit */
    bcmx_uport_t    uport;       /* Cookie for application lport */
    uint32          flags;       /* Flags BCMX_PORT_F_xxx */
    int             modid;       /* Module id for this lport */
    bcm_port_t      modport;     /* Module relative port (0..31) */
};

/* Hash is allocated on BCMX initialization */
struct _bcmx_uport_hash_s {
    bcmx_uport_t uport;
    bcmx_lport_t lport;
    _bcmx_uport_hash_t *next;
    _bcmx_uport_hash_t *prev;
};

/* 
 * BCMX application logical port definitions.
 * 
 * The mapping: "application port -> BCMX logical port" is currently a
 * hash + doubly linked list. It is assumed that adding/removing ports is
 * a relatively rare occurrance and that an allocation/free call is
 * acceptable during this process.
 * 
 * Alternatively:
 * 
 *   ((((PTR_TO_INT(uport)) >> 16) ^ ((PTR_TO_INT(uport)) & 0xffff)) % \
 *       BCMX_UPORT_HASH_COUNT)
 */
#define BCMX_UPORT_HASH_COUNT   73       /* Prime */
#define BCMX_UPORT_HASH(uport)  ((PTR_TO_INT(uport)) % BCMX_UPORT_HASH_COUNT) 
extern _bcmx_uport_hash_t *_bcmx_uport_hash[BCMX_UPORT_HASH_COUNT];


/*
 * Deprecated - No longer supported.
 */
extern int bcmx_lport_max;                  /* Set to 0 */
extern bcmx_lport_t _bcmx_lport_first;
extern bcmx_uport_t _bcmx_uport_invalid;
extern _bcmx_port_t *_bcmx_port;

#endif /* _BCMX_INT_LPORT_H_ */
