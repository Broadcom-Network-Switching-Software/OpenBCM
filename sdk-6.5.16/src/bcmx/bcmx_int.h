/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx_int.h
 * Purpose:     Internal header file for BCMX
 */

#ifndef   _BCMX_BCMX_INT_H_
#define   _BCMX_BCMX_INT_H_


#include <sal/core/sync.h>
#include <bcm/types.h>

/*
 * The BCMX logical port is currently defined as a gport type
 * and it can refer to a physical or a virtual port.
 * 
 * BCMX maintains a port list which contains the physical ports
 * of attached units.
 *
 * Some BCMX APIs require the port to be in the BCMX port list;
 * other APIs can take virtual ports.
 *
 * The following macros can be used to check for lport:
 *   BCMX_IS_LPORT / BCMX_LPORT_CHECK
 *   Check that port is within allowed value range.
 *   Port can be of most gport types (physical and virtual ports).
 * 
 *   BCMX_LPORT_VALID / BCMX_LPORT_VALID_CHECK
 *   Check that port is in the BCMX port list.
 *
 *   BCMX_LPORT_IS_PHYSICAL
 *   Check that port is an allowed physical port value.
 */

/* Check that port is within allowed value range (physical/virtual ports) */
#define BCMX_IS_LPORT(_lport)       \
    (BCM_GPORT_IS_SET(_lport) &&    \
     !BCM_GPORT_IS_LOCAL(_lport) && !BCM_GPORT_IS_LOCAL_CPU(_lport))

/* Same as above, but performs a return in calling routine on failure */
#define BCMX_LPORT_CHECK(_lport)   \
    if (!BCMX_IS_LPORT(_lport)) { return BCM_E_PORT; }

/* Check that port is in the BCMX port list; performs return on failure */
#define BCMX_LPORT_VALID_CHECK(_lport)   \
    if (!bcmx_lport_valid(_lport)) { return BCM_E_PORT; }

/* Check that given lport is an allowed physical port value */
#define BCMX_LPORT_IS_PHYSICAL(_lport)                              \
    (BCM_GPORT_IS_MODPORT(_lport) || BCM_GPORT_IS_DEVPORT(_lport))


/* Cast lport <-> gport */
#define BCMX_LPORT_TO_GPORT_CAST(_lport)    ((bcm_gport_t)(_lport))
#define BCMX_GPORT_TO_LPORT_CAST(_gport)    ((bcmx_lport_t)(_gport))


/* Check for 'null' argument (returns on failure) */
#define BCMX_PARAM_NULL_CHECK(_arg)  if ((_arg) == NULL) {return BCM_E_PARAM;}
#define BCMX_PARAM_ARRAY_NULL_CHECK(_size, _arg_array)    \
    if (_size > 0) { BCMX_PARAM_NULL_CHECK(_arg_array); }


#include <bcm_int/control.h>
#define BCMX_UNITS_MAX BCM_CONTROL_MAX

extern int bcmx_unit_list[BCMX_UNITS_MAX];

extern sal_mutex_t bcmx_config_lock;

extern int _bcmx_rx_running;
extern volatile int _bcmx_l2n_running;


/*
 * Sanity check macros for BCMX subsystem state
 *
 * BCMX can be in one of these following states:
 *   - Unitialized
 *       Initial BCMX software layer state
 *   - Initialized, no units attached
 *       State after calling bcmx_init()
 *   - Initialized, unit(s) attached
 *       State after calling bcmx_device_attach()
 */
/* Check that BCMX is initialized */
#define BCMX_INIT          (bcmx_config_lock != NULL)
#define BCMX_INIT_CHECK    if (!BCMX_INIT) { return BCM_E_INIT; }

/* Check that BCMX contains at least one unit */
#define BCMX_UNIT          (bcmx_unit_count > 0)
#define BCMX_UNIT_CHECK    if (!BCMX_UNIT) { return BCM_E_CONFIG; }

/* Check that BCMX is initialized and contains at least one unit */
#define BCMX_READY         (BCMX_INIT && BCMX_UNIT)
#define BCMX_READY_CHECK    do {     \
        BCMX_INIT_CHECK;      \
        BCMX_UNIT_CHECK;      \
    } while (0)


#define BCMX_CONFIG_LOCK sal_mutex_take(bcmx_config_lock, sal_sem_FOREVER)
#define BCMX_CONFIG_UNLOCK sal_mutex_give(bcmx_config_lock)

/*
 * BCMX error code check
 */
extern int _bcmx_error_check(int unit, int check, int ignore, int *rv);

#define BCMX_FABRIC_E_UNAVAIL(_unit, _rv)  \
    ((_rv == BCM_E_UNAVAIL) && BCM_IS_FABRIC(_unit))

#define BCMX_ERROR_IS_VALID(_unit, _rv)  (!BCMX_FABRIC_E_UNAVAIL(_unit, _rv))

#define BCMX_ERROR_CHECK(_unit, _check, _rv)    \
    (_bcmx_error_check(_unit, _check, BCM_E_NONE, _rv))

#define BCMX_SET_ERROR_CHECK(_unit, _check, _rv)    \
    (_bcmx_error_check(_unit, _check, BCM_E_EXISTS, _rv))

#define BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    (_bcmx_error_check(_unit, _check, BCM_E_NOT_FOUND, _rv))


#define BCMX_UNIT_ITER(unit, i)                                           \
    for (i = 0, unit = bcmx_unit_list[0];                                 \
         i < bcmx_unit_count;                                             \
         unit = bcmx_unit_list[++i])

#define BCMX_LOCAL_UNIT_ITER(unit, i)                                     \
    for (i = 0, unit = bcmx_unit_list[0];                                 \
         i < bcmx_unit_count;                                             \
         unit = bcmx_unit_list[++i]) if (BCM_IS_LOCAL(unit))


/* Get local CPU lport if set */
extern bcmx_lport_t bcmx_lport_local_cpu[BCM_CONTROL_MAX];
#define BCMX_LPORT_LOCAL_CPU_GET(bcm_unit) (bcmx_lport_local_cpu[bcm_unit])


/*
 * Conversion BCMX to/from BCM destination information
 */
typedef struct _bcmx_dest_bcmx_s {
    bcmx_lport_t  port;    /* BCMX logical port (gport format) */
    bcm_trunk_t   trunk;   /* Trunk id */
    int           mcast;   /* Mcast */
} _bcmx_dest_bcmx_t;

typedef struct _bcmx_dest_bcm_s {
    int           unit;         /* Device unit number */
    bcm_port_t    port;         /* Device port number (old format) or gport */
    bcm_module_t  module_id;    /* Module id */
    bcm_port_t    module_port;  /* Module relative port (old format) or gport */
    bcm_trunk_t   trunk;        /* Trunk id */
    int           mcast;        /* Mcast */
} _bcmx_dest_bcm_t;


/*
 * Conversion flags BCMX to/from BCM destination information
 *
 * NOTES:
 *   GPORT_AWARE  - Convert to new gport format, used only when
 *                  translating to BCM
 *   TRUNK        - Data is in old format, data is in 'trunk' member
 *   MCAST        - Data is in old format, data is in 'mcast' member
 */
#define BCMX_DEST_GPORT_AWARE  0x00000001  /* BCM can handle GPORT */
#define BCMX_DEST_TRUNK        0x00000002  /* Trunk */
#define BCMX_DEST_MCAST        0x00000004  /* Mcast */
#define BCMX_DEST_DISCARD      0x00000008  /* Black hole */
#define BCMX_DEST_LOCAL_CPU    0x00000010  /* Local CPU */

/*
 * Initializer routines
 */
extern void
_bcmx_dest_bcmx_t_init(_bcmx_dest_bcmx_t *dest_bcmx);

extern void
_bcmx_dest_bcm_t_init(_bcmx_dest_bcm_t *dest_bcm);

/*
 * Conversion routines for destination structures
 */
extern int
_bcmx_dest_to_bcm(_bcmx_dest_bcmx_t *from_bcmx, _bcmx_dest_bcm_t *to_bcm,
                  uint32 *flags);

extern int
_bcmx_dest_from_bcm(_bcmx_dest_bcmx_t *to_bcmx, _bcmx_dest_bcm_t *from_bcm,
                    uint32 *flags);

/*
 * Conversion default flag for BCMX to/from BCM.
 * 
 * Current default flag value indicates to convert to new BCM port
 * format (gport), for _bcmx_dest_to_modid_port() or
 * _bcmx_dest_unit_port() routines.
 */
#define BCMX_DEST_CONVERT_NON_GPORT  0x0
#define BCMX_DEST_CONVERT_GPORT      BCMX_DEST_GPORT_AWARE
#define BCMX_DEST_CONVERT_DEFAULT    BCMX_DEST_CONVERT_GPORT

/*
 * Conversion routines for specific destination information
 * (module/port, unit/port, ...)
 */
extern int
_bcmx_dest_to_modid_port(bcmx_lport_t port,
                         bcm_module_t *bcm_module, bcm_port_t *bcm_port,
                         uint32 flags);
extern int
_bcmx_dest_from_modid_port(bcmx_lport_t *port,
                           bcm_module_t bcm_module, bcm_port_t bcm_port,
                           uint32 flags);
extern int
_bcmx_dest_to_unit_port(bcmx_lport_t port,
                        int *bcm_unit, bcm_port_t *bcm_port,
                        uint32 flags);

extern int
_bcmx_dest_from_unit_port(bcmx_lport_t *port,
                          int bcm_unit, bcm_port_t bcm_port,
                          uint32 flags);

extern int
_bcmx_port_changed(bcmx_lport_t lport);


#endif /* _BCMX_BCMX_INT_H_ */
