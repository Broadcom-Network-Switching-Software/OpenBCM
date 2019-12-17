/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx.c
 * Purpose:     Basic infrastructure code for BCMX
 */

#include <shared/bsl.h>

#include <shared/alloc.h>
#include <sal/core/libc.h>

#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/rx.h>
#include <bcmx/l2.h>
#include <bcmx/link.h>

#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm_int/rpc/rlink.h>

#include "bcmx_int.h"
#include "api_ref.h"


/*
 * BCMX Port Information
 *
 * BCMX Port information will be kept in a 'hash' table
 * rather than an array indexed by the 'lport' value.  This allows
 * BCMX interfaces to take a 'gport' any place where 'lport' is present.
 *
 * The GPORT types that are used for BCMX ports are:
 *     MODPORT - For devices with module id
 *     DEVPORT - For devices with NO module id
 */

/* BCMX Port Information Hash Table */
#define BCMX_PORT_HASH_COUNT          73        /* Prime */
#define BCMX_PORT_HASH_INDEX(_key)    \
    (((unsigned int)(_key)) % BCMX_PORT_HASH_COUNT)

typedef struct _bcmx_port_info_s _bcmx_port_info_t;

/* List to maintain ports in the order in which ports were added in BCMX */
typedef struct _bcmx_port_list_s {
    _bcmx_port_info_t  *prev;
    _bcmx_port_info_t  *next;
} _bcmx_port_list_t;

/* Hash Table Data Structure */
struct _bcmx_port_info_s {
    bcmx_lport_t       lport;         /* Logical port number, hash key*/
    _bcmx_port_t       port;          /* BCMX port data struct */
    _bcmx_port_info_t  *prev;
    _bcmx_port_info_t  *next;
    _bcmx_port_list_t  list;          /* Use for ordered port list */
};

/* Port Information Hash Table */
static _bcmx_port_info_t  *_bcmx_port_info[BCMX_PORT_HASH_COUNT];

/* Port List */
static _bcmx_port_info_t *_bcmx_port_list_first = NULL;
static _bcmx_port_info_t *_bcmx_port_list_last  = NULL;


/*
 * Port Mapping - DEVPORT hash
 *
 * Definitions for faster search based on gport DEVPORT hash key
 * (key is composed of unit/port)
 */
typedef struct _bcmx_devport_hash_s  _bcmx_devport_hash_t;
struct _bcmx_devport_hash_s {
    bcm_gport_t           key;        /* Hash key, gport DEVPORT */
    _bcmx_port_info_t     *port_info; /* Reference to port info object */
    _bcmx_devport_hash_t  *next;
    _bcmx_devport_hash_t  *prev;
};
static _bcmx_devport_hash_t  *_bcmx_devport_hash[BCMX_PORT_HASH_COUNT];


/*
 * BCMX Uport <-> Lport Mapping
 */
_bcmx_uport_hash_t *_bcmx_uport_hash[BCMX_UPORT_HASH_COUNT];

/* Application definition for Uport */
bcmx_uport_create_f bcmx_uport_create;
bcmx_lport_remove_notify_f bcmx_lport_remove_notify;

/*
 * Default Uport
 *
 * If application 'uport' definition is not provided, the default
 * uport is applied (see _bcmx_uport_create_default).
 * The default uport is of type 'int'.
 * Values are assigned starting with the lowest unused integer number
 * (this assignment logic is the same* as the one used before
 * 'lport' was converted to 'gport').
 */
#define BCMX_UPORT_TO_INT(_uport)    PTR_TO_INT(_uport)


/*
 * BCMX unit list
 */
int bcmx_unit_list[BCMX_UNITS_MAX];
int bcmx_unit_count;

/* Cache for device->CPU port map */
bcmx_lport_t bcmx_lport_local_cpu[BCM_CONTROL_MAX];

sal_mutex_t bcmx_config_lock;


/*
 * BCMX generic conversion routines for destinations
 *
 * Check macros
 */
#define BCMX_DEST_MODID_PORT_FLAG_CHECK(_flags) \
    if (((_flags) | BCMX_DEST_GPORT_AWARE) != BCMX_DEST_GPORT_AWARE) { \
        return BCM_E_PARAM; \
    }

#define BCMX_DEST_UNIT_PORT_FLAG_CHECK(_flags) \
    BCMX_DEST_MODID_PORT_FLAG_CHECK(_flags)



/*
 * DEPRECATED
 */
int bcmx_lport_max = 0;       /* Number of lports supported */
_bcmx_port_t *_bcmx_port;
bcmx_lport_t _bcmx_lport_first = BCMX_LPORT_INVALID;
bcmx_uport_t _bcmx_uport_invalid = BCMX_UPORT_INVALID;



/****************************************************************
 * Internal Functions
 */

STATIC INLINE void
_bcmx_uport_remove(bcmx_uport_t uport)
{
    _bcmx_uport_hash_t *uph;
    int h_idx;

    h_idx = BCMX_UPORT_HASH(uport);
    uph = _bcmx_uport_hash[h_idx];
    while (uph != NULL) {
        if (BCMX_UPORT_EQ(uph->uport, uport)) {
            break;
        }
        uph = uph->next;
    }

    if (uph == NULL) {  /* Not there */
        return;
    }

    /* Is this the first entry? */
    if (uph->prev == NULL) {  /* Yes */
        _bcmx_uport_hash[h_idx] = uph->next;
    } else {
        uph->prev->next = uph->next;
    }

    /* Is this the last entry? */
    if (uph->next != NULL) {  /* No */
        uph->next->prev = uph->prev;
    }

    sal_free(uph);
}

STATIC INLINE _bcmx_uport_hash_t *
_bcmx_uport_add(bcmx_uport_t uport, bcmx_lport_t lport)
{
    _bcmx_uport_hash_t *uph;
    int h_idx;

    h_idx = BCMX_UPORT_HASH(uport);

    uph = sal_alloc(sizeof(_bcmx_uport_hash_t), "bcmx_uport_add");
    if (uph == NULL) {
        return NULL;
    }

    uph->uport = uport;
    uph->lport = lport;
    uph->prev = NULL;
    uph->next = _bcmx_uport_hash[h_idx];
    if (uph->next != NULL) {
        uph->next->prev = uph;
    }
    _bcmx_uport_hash[h_idx] = uph;

    return uph;
}


/*
 * Function:
 *     _bcmx_uport_create_default
 * Purpose:
 *     Return a new user port number using the default
 *     definition for uport (integer).
 * Parameters:
 *     none
 * Returns:
 *     A new 'uport' value.
 * Notes:
 *    Assumes lock is held.
 */
STATIC bcmx_uport_t
_bcmx_uport_create_default(void)
{
    int  i = 0;

    /* Find lowest unused uport integer */
    while(bcmx_uport_to_lport(BCMX_UPORT_CAST(i)) != BCMX_LPORT_INVALID) {
        i++;
    }

    return BCMX_UPORT_CAST(i);
}


/*
 * Function:
 *     _bcmx_devport_hash_add
 * Purpose:
 *     Add the port information reference to the devport hash
 *     table array.
 * Parameters:
 *     unit      - Device number
 *     port      - Device port number
 *     port_info - Pointer to port information object
 * Returns:
 *     Pointer to added devport hash, if successful.
 *     NULL, on failure to allocate struct.
 * Notes:
 *     Assumes lock is held.
 */
STATIC _bcmx_devport_hash_t *
_bcmx_devport_hash_add(int unit, bcm_port_t port, _bcmx_port_info_t *port_info)
{
    bcm_gport_t           key;
    int                   index;
    _bcmx_devport_hash_t  *devport;

    if ((devport = sal_alloc(sizeof(_bcmx_devport_hash_t),
                             "bcmx_devport_add")) == NULL) {
        return NULL;
    }

    BCM_GPORT_DEVPORT_SET(key, unit, port);
    index = BCMX_PORT_HASH_INDEX(key);

    devport->key = key;
    devport->port_info = port_info;
    devport->prev = NULL;
    devport->next = _bcmx_devport_hash[index];
    if (devport->next != NULL) {
        devport->next->prev = devport;
    }
    _bcmx_devport_hash[index] = devport;

    return devport;
}


/*
 * Function:
 *     _bcmx_devport_hash_remove
 * Purpose:
 *     Remove devport hash information for given BCM unit/port.
 * Parameters:
 *     unit - Device number
 *     port - Device port number
 * Returns:
 *     None
 * Notes:
 *     Assumes lock is held.
 */
STATIC void
_bcmx_devport_hash_remove(int unit, bcm_port_t port)
{
    bcm_gport_t           key;
    int                   index;
    _bcmx_devport_hash_t  *devport;

    BCM_GPORT_DEVPORT_SET(key, unit, port);
    index = BCMX_PORT_HASH_INDEX(key);


    devport = _bcmx_devport_hash[index];
    while (devport != NULL) {
        if (devport->key == key) {
            break;
        }
        devport = devport->next;
    }

    if (devport == NULL) {  /* Not found */
        return;
    }

    /* Check first entry */
    if (devport->prev == NULL) {  /* Yes */
        _bcmx_devport_hash[index] = devport->next;
    } else {
        devport->prev->next = devport->next;
    }

    /* Check last entry */
    if (devport->next != NULL) {  /* No */
        devport->next->prev = devport->prev;
    }

    sal_free(devport);
}


/*
 * Function:
 *     _bcmx_port_info_find
 * Purpose:
 *     Find the BCMX port information for a given lport key.
 * Parameters:
 *     lport     - BCMX logical port number (gport format)
 *     tbl_index - (OUT) Table array index for given port
 * Returns:
 *     Pointer to BCXM port information struct, if key was found.
 *     NULL, on failure to find key.
 * Notes:
 *     Assumes lock is held.
 */
STATIC _bcmx_port_info_t*
_bcmx_port_info_find(bcmx_lport_t lport, int *tbl_index)
{
    _bcmx_port_info_t  *port_info = NULL;

    *tbl_index = BCMX_PORT_HASH_INDEX(lport);

    port_info = _bcmx_port_info[*tbl_index];
    while (port_info != NULL) {
        if (port_info->lport == lport) {
            break;
        }
        port_info = port_info->next;
    }

    return port_info;
}


/*
 * Function:
 *     _bcmx_port_add
 * Purpose:
 *     Add the port information into the BCMX port hash
 *     table array.
 * Parameters:
 *     unit  - Device number
 *     port  - Device port number
 *     flags - Port flags BCMX_PORT_F_xxx
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_EXISTS   - Port already exist in BCMX port table
 *     BCM_E_RESOURCE - Could not allocate port information structure
 * Notes:
 *     Assumes lock is held.
 */
STATIC int
_bcmx_port_add(int unit, bcm_port_t port, bcm_gport_t lport, uint32 flags)
{
    int                index;
    bcm_port_t         modport;
    bcm_module_t       modid;
    bcmx_uport_t       uport;
    _bcmx_port_info_t  *port_info;

    /* BCMX lport should unique */
    if (_bcmx_port_info_find(lport, &index) != NULL) {
        return BCM_E_EXISTS;
    }

    if ((port_info = sal_alloc(sizeof(_bcmx_port_info_t),
                               "bcmx_port_info_add")) == NULL) {
        return BCM_E_RESOURCE;
    }

    if (BCM_GPORT_IS_MODPORT(lport)) {
        modport = BCM_GPORT_MODPORT_PORT_GET(lport);
        modid   = BCM_GPORT_MODPORT_MODID_GET(lport);
    } else {
        modport = port;
        modid   = -1;
    }

    /* Set uport */
    if (bcmx_uport_create != NULL) {
        uport = bcmx_uport_create(lport, unit, port, flags);
    } else {
        /* Use default uport assignment */
        uport = _bcmx_uport_create_default();
    }
    /* Update uport <-> lport map table */
    if (_bcmx_uport_add(uport, lport) == NULL) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META_U(unit,
                             "BCMX: Failed to add uport hash, "
                             "u %d, p %d\n"), unit, port));
    }

    /* Set Cache CPU port */
    if (flags & BCMX_PORT_F_CPU) {
        bcmx_lport_local_cpu[unit] = lport;
    }

    /* Set BCMX port information */
    port_info->lport         = lport;
    port_info->port.bcm_unit = unit;
    port_info->port.bcm_port = port;
    port_info->port.modid    = modid;
    port_info->port.modport  = modport;
    port_info->port.uport    = uport;
    port_info->port.flags    = flags;

    /* Update hash table link list */
    index           = BCMX_PORT_HASH_INDEX(lport);
    port_info->prev = NULL;
    port_info->next = _bcmx_port_info[index];
    if (port_info->next != NULL) {
        port_info->next->prev = port_info;
    }
    _bcmx_port_info[index] = port_info;

    /* Update port list */
    if (_bcmx_port_list_first == NULL) {
        _bcmx_port_list_first = port_info;
    }
    if (_bcmx_port_list_last != NULL) {
        _bcmx_port_list_last->list.next = port_info;
    }
    port_info->list.prev = _bcmx_port_list_last;            
    port_info->list.next = NULL;
    _bcmx_port_list_last = port_info;

    /* Update devport hash table */
    if (_bcmx_devport_hash_add(unit, port, port_info) == NULL) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META_U(unit,
                             "BCMX: Failed to add devport hash, "
                             "u %d, p %d\n"), unit, port));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcmx_port_info_remove_by_unit
 * Purpose:
 *     Remove all port information associated with the given unit.
 * Parameters:
 *     unit - Device number to remove ports for
 * Returns:
 *     BCM_E_NONE - Success
 * Notes:
 *     Assumes lock is held.
 */
STATIC int
_bcmx_port_info_remove_by_unit(int unit)
{
    int                i;
    _bcmx_port_info_t  *port_info;
    _bcmx_port_info_t  *next;

    /* Check each link list in port table */
    for (i = 0; i < BCMX_PORT_HASH_COUNT; i++) {

        port_info = _bcmx_port_info[i];

        /* Check each entry in link list */
        while (port_info != NULL) {
            next = port_info->next;

            /* Remove port if unit matches */
            if (port_info->port.bcm_unit == unit) {

                /* Clear uport */
                if (bcmx_lport_remove_notify != NULL) {
                    bcmx_lport_remove_notify(port_info->lport,
                                             port_info->port.uport);
                }
                /* Update uport <-> lport map table */
                _bcmx_uport_remove(port_info->port.uport);

                /* Update devport hash table */
                _bcmx_devport_hash_remove(port_info->port.bcm_unit,
                                          port_info->port.bcm_port);

                /* Update hash table */
                /* Check first entry */
                if (port_info->prev == NULL) {  /* Yes */
                    _bcmx_port_info[i] = port_info->next;
                } else {
                    port_info->prev->next = port_info->next;
                }
                /* Check last entry */
                if (port_info->next != NULL) {  /* No */
                    port_info->next->prev = port_info->prev;
                }

                /* Update port list */
                if (_bcmx_port_list_first == port_info) {
                    _bcmx_port_list_first = port_info->list.next;
                }
                if (_bcmx_port_list_last == port_info) {
                    _bcmx_port_list_last = port_info->list.prev;
                }
                if (port_info->list.prev != NULL) {
                    port_info->list.prev->list.next = port_info->list.next;
                }
                if (port_info->list.next != NULL) {
                    port_info->list.next->list.prev = port_info->list.prev;
                }
                
                sal_free(port_info);
            }

            port_info = next;
        }
    }

    /* Clear cached CPU port info */
    bcmx_lport_local_cpu[unit] = BCMX_LPORT_INVALID; 

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcmx_lport_flags_get
 * Purpose:
 *     Get the port flags for given lport.
 * Parameters:
 *     lport - The port to get flags for
 *     flags - (OUT) Flags for given port
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
_bcmx_lport_flags_get(bcmx_lport_t lport, uint32 *flags)
{
    int                rv = BCM_E_NOT_FOUND;
    int                index;
    _bcmx_port_info_t  *port_info;

    BCMX_READY_CHECK;
    BCMX_LPORT_CHECK(lport);

    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
        *flags = port_info->port.flags;
        rv = BCM_E_NONE;
    }
    BCMX_CONFIG_UNLOCK;

    return rv;
}


/*
 * Function:
 *     _bcmx_lport_flags_set
 * Purpose:
 *     Set the port flags for given lport.
 * Parameters:
 *     lport - The port to set flags for
 *     flags - Flags for given port
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
_bcmx_lport_flags_set(bcmx_lport_t lport, uint32 flags)
{
    int                rv = BCM_E_NOT_FOUND;
    int                index;
    _bcmx_port_info_t  *port_info;

    BCMX_READY_CHECK;
    BCMX_LPORT_CHECK(lport);

    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
        port_info->port.flags = flags;
        rv = BCM_E_NONE;
    }
    BCMX_CONFIG_UNLOCK;

    return rv;
}


/*
 * Function:
 *     _bcmx_port_flags_create
 * Purpose:
 *     Create the BCMX port flags for the given port.
 * Parameters:
 *     unit - unit
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
_bcmx_port_flags_create(bcm_port_config_t *config,
                        bcm_port_t port, uint32 *flags)
{
    uint32 port_flags;

    port_flags = BCMX_PORT_F_VALID;
    if (BCM_PBMP_MEMBER(config->cpu, port)) {
        port_flags |= BCMX_PORT_F_CPU;
    }
    if (BCM_PBMP_MEMBER(config->hg, port)) {
        port_flags |= BCMX_PORT_F_HG;
    }
    if (BCM_PBMP_MEMBER(config->fe, port)) {
        port_flags |= BCMX_PORT_F_FE;
    }
    if (BCM_PBMP_MEMBER(config->ge, port)) {
        port_flags |= BCMX_PORT_F_GE;
    }
    if (BCM_PBMP_MEMBER(config->xe, port)) {
        port_flags |= BCMX_PORT_F_XE;
    }
    if (BCM_PBMP_MEMBER(config->ce, port)) {
        port_flags |= BCMX_PORT_F_CE;
    }
    if (BCM_PBMP_MEMBER(config->stack_int, port)) {
        port_flags |= BCMX_PORT_F_STACK_INT;
    }
    if (BCM_PBMP_MEMBER(config->stack_ext, port)) {
        port_flags |= BCMX_PORT_F_STACK_EXT;
    }

    *flags = port_flags;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcmx_port_changed
 * Purpose:
 *     Update internal BCMX information for the given port
 * Parameters:
 *     port - port
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
_bcmx_port_changed(bcmx_lport_t lport)
{
    int rv = BCM_E_PORT;
    int bcm_unit;
    bcm_port_config_t config;
    bcm_port_t bcm_port;
    uint32 port_flags;
    
    bcm_unit = bcmx_lport_bcm_unit(lport);
    bcm_port = bcmx_lport_bcm_port(lport);

    if (bcm_unit >= 0 && bcm_port >= 0) {
        rv = bcm_port_config_get(bcm_unit, &config);
        if (BCM_SUCCESS(rv)) {
            rv = _bcmx_port_flags_create(&config, bcm_port, &port_flags);
            if (BCM_SUCCESS(rv)) {
                rv = _bcmx_lport_flags_set(lport, port_flags);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     _bcmx_port_changed_callback
 * Purpose:
 *     Update cached port information when called
 * Parameters:
 *     port - port
 *     info - port info
 * Returns:
 *     void
 * Notes:
 *     Registered with bcmx linkscan to track port changes.
 *     Limitation: will not catch changes to ports if the
 *       port link state is always down.
 */
STATIC void
_bcmx_port_changed_callback(bcmx_lport_t lport, bcm_port_info_t *info)
{
    COMPILER_REFERENCE(info);
    (void)_bcmx_port_changed(lport);
}

/*
 * Function:
 *     bcmx_lport_first
 * Purpose:
 *     Return the first lport found in the BCMX port information table.
 * Parameters:
 *     none
 * Returns:
 *     First BCMX logical port found in the BCMX port information table.
 *     (-1) on empty table
 * Notes:
 *     - The 'first' port returned is the first port added in BCMX.
 *     - This routine should have been private to BCMX, but is exposed
 *       to maintain support of existing public macros.
 */
bcmx_lport_t
bcmx_lport_first(void)
{
    bcmx_lport_t  lport_first = BCMX_LPORT_INVALID;

    if (!BCMX_READY) {
        return BCMX_LPORT_INVALID;
    }

    BCMX_CONFIG_LOCK;
    if (_bcmx_port_list_first != NULL) {
        lport_first = _bcmx_port_list_first->lport;
    }
    BCMX_CONFIG_UNLOCK;

    return lport_first;
}


/*
 * Function:
 *     bcmx_lport_next
 * Purpose:
 *     Return the lport next to the given port in the BCMX port
 *     information table.
 * Parameters:
 *     lport - BCMX logical port number (gport format)
 * Returns:
 *     Next lport to given port in the BCMX port information table.
 *     (-1) if no more ports is found.
 * Notes:
 *     - The 'next' port returned is the port that was added in BCMX
 *       after the specified port.
 *     - This routine should have been private to BCMX, but is exposed
 *       to maintain support of existing public macros.
 */
bcmx_lport_t
bcmx_lport_next(bcmx_lport_t lport)
{
    bcmx_lport_t       lport_next = BCMX_LPORT_INVALID;
    int                index;
    _bcmx_port_info_t  *port_info;

    if (!BCMX_READY || !BCMX_IS_LPORT(lport)) {
        return BCMX_LPORT_INVALID;
    }

    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
        /* Get next one */
        if (port_info->list.next != NULL) {
            lport_next = port_info->list.next->lport;
        }
    }
    BCMX_CONFIG_UNLOCK;

    return lport_next;
}


/*
 * Function:
 *     bcmx_lport_init
 * Purpose:
 *     Initialize the internal BCMX port information structures.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_MEMORY if unable to allocate structures
 * Notes:
 *     Destroys all current information and initializes structures
 *     Destroys user port info; does not make callouts that they're
 *     disappearing.
 */
int
bcmx_lport_init(void)
{
    int                   i;
    _bcmx_port_info_t     *port_info;
    _bcmx_port_info_t     *port_info_next;
    _bcmx_uport_hash_t    *uport_next;
    _bcmx_devport_hash_t  *devport;
    _bcmx_devport_hash_t  *devport_next;

    BCMX_INIT_CHECK;

    BCMX_CONFIG_LOCK;

    /* Clear BCMX port information table */
    for (i = 0; i < BCMX_PORT_HASH_COUNT; i++) {
        port_info = _bcmx_port_info[i];
        while (port_info != NULL) {
            port_info_next = port_info->next;
            sal_free(port_info);
            port_info = port_info_next;
        }
        _bcmx_port_info[i] = NULL;
    }

    /* Clear port lists */
    _bcmx_port_list_first = NULL;
    _bcmx_port_list_last  = NULL;

    /* Clear out BCMX uport <-> lport mapping */
    for (i = 0; i < BCMX_UPORT_HASH_COUNT; i++) {
        while (_bcmx_uport_hash[i] != NULL) {
            uport_next = _bcmx_uport_hash[i]->next;
            sal_free(_bcmx_uport_hash[i]);
            _bcmx_uport_hash[i] = uport_next;
        }
        _bcmx_uport_hash[i] = NULL;
    }

    /* Clear devport hash table */
    for (i = 0; i < BCMX_PORT_HASH_COUNT; i++) {
        devport = _bcmx_devport_hash[i];
        while (devport != NULL) {
            devport_next = devport->next;
            sal_free(devport);
            devport = devport_next;
        }
        _bcmx_devport_hash[i] = NULL;
    }

    /* Clear cache CPU port */
    for (i = 0; i < BCM_CONTROL_MAX; i++) {
        bcmx_lport_local_cpu[i] = BCMX_LPORT_INVALID;
    }

    BCMX_CONFIG_UNLOCK;

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmx_init
 * Purpose:
 *     Initialize the BCMX subsystem.
 * Returns:
 *     BCM_E_NONE if successful
 *     BCM_E_MEMORY if unable to allocate structures
 * Notes:
 *     Initialize mutex.
 */
int
bcmx_init(void)
{
    int   rv = BCM_E_NONE;
    void  *api_ptr = (void *)bcmx_api_tbl;

    /* This should never be true; done to include api table */
    if (api_ptr == NULL) {
    /*
     * Coverity
     * Defencive check
     * coverity[dead_error_line]
     */
        return 0;
    }

    if (bcmx_config_lock == NULL) {
        bcmx_config_lock = sal_mutex_create("bcmx_init");
        if (bcmx_config_lock == NULL) {
            return BCM_E_MEMORY;
        }
    }
    
    /* Reset port information tables */
    rv = bcmx_lport_init();

    /* Register with linkscan to track port changes */
    bcmx_linkscan_register(_bcmx_port_changed_callback);
    
    return rv;
}


/*
 * Function:
 *     bcmx_uport_create_callback_set
 * Purpose:
 *     Set the callback for uport create.
 * Parameters:
 *     create_callback - Function to callback on
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     Call with NULL pointer to turn off the callback.
 */
int
bcmx_uport_create_callback_set(bcmx_uport_create_f create_callback)
{
    bcmx_uport_create = create_callback;
    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmx_device_attach
 * Purpose:
 *     Make BCMX aware of the indicated device.
 * Parameters:
 *     bcm_unit - The BCM (dispatch) unit number of the device
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     This should only be used for "normal" units.
 */
int
bcmx_device_attach(int bcm_unit)
{
    uint32 port_flags;
    bcm_port_config_t config;
    bcm_port_t port;
    int i;
    int rv;
    bcm_gport_t lport[BCM_PBMP_PORT_MAX];
    
    if (bcmx_config_lock == NULL) {
        BCM_IF_ERROR_RETURN(bcmx_init());
    }

    /* Gather information that requires RPC call before applying lock */
    rv = bcm_port_config_get(bcm_unit, &config);
    if (rv < 0) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("BCMX: Could not get port config %d: %s\n"),
                  rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_VERBOSE(BSL_LS_BCMX_COMMON,
                (BSL_META("BCMX: Attaching unit %d\n"),
                 bcm_unit));
    BCMX_CONFIG_LOCK;
    /* See if the unit is on the list of those known */
    for (i = 0; i < bcmx_unit_count; i++) {
        if (bcm_unit == bcmx_unit_list[i]) {
            BCMX_CONFIG_UNLOCK;
            LOG_ERROR(BSL_LS_BCMX_COMMON,
                      (BSL_META("BCMX: unit exists\n")));
            return BCM_E_EXISTS;
        }
    }
    if (bcmx_unit_count >= BCMX_UNITS_MAX) {
        BCMX_CONFIG_UNLOCK;
        return BCM_E_RESOURCE;
    }
    bcmx_unit_list[bcmx_unit_count++] = bcm_unit;

    i = 0;
    BCM_PBMP_ITER(config.all, port) {

        _bcmx_port_flags_create(&config, port, &port_flags);

        /* Add port */
        rv = _bcmx_port_add(bcm_unit, port, lport[i], port_flags);
        if (BCM_FAILURE(rv)) {
            LOG_WARN(BSL_LS_BCMX_COMMON,
                     (BSL_META("BCMX: Failed to add lport 0x%x (unit %d, port %d). "
                               "%d: %s.\n"),
                      lport[i], bcm_unit, port, rv, bcm_errmsg(rv)));

            /* If BCM_E_EXISTS, do not exit, try remaining ports */
            if (rv != BCM_E_EXISTS) {
                BCMX_CONFIG_UNLOCK;
                return rv;
            }
        }
        i++;
    }

    BCMX_CONFIG_UNLOCK;

    /*
     * Check modules that are running that need to know about
     * new units
     */
    rv = bcmx_rx_device_add(bcm_unit);
    if (rv < 0) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("BCMX: Failed to add unit to RX, "
                           "u %d.  %d: %s\n"), bcm_unit, rv, bcm_errmsg(rv)));
    }

    rv = bcmx_l2_device_add(bcm_unit);
    if (rv < 0 && rv != BCM_E_UNAVAIL) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("BCMX: Failed to add unit for L2 notify, "
                           "u %d.  %d: %s\n"), bcm_unit, rv, bcm_errmsg(rv)));
    }

    rv = bcmx_linkscan_device_add(bcm_unit);
    if (rv < 0) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("BCMX: Failed to add unit for linkscan, "
                           "u %d.  %d: %s\n"), bcm_unit, rv, bcm_errmsg(rv)));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmx_device_detach
 * Purpose:
 *     Remove unit from BCMX configuration.
 * Parameters:
 *     bcm_unit - The BCM (dispatch) unit number of the device
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_device_detach(int bcm_unit)
{
    int i, j;
    int found = FALSE;
#if defined(BCM_RPC_SUPPORT)
    int rv;
#endif

    BCMX_INIT_CHECK;

    LOG_VERBOSE(BSL_LS_BCMX_COMMON,
                (BSL_META("BCMX: Detaching unit %d\n"),
                 bcm_unit));

    BCMX_CONFIG_LOCK;

    /* First, look for the unit on the list of known units */
    for (i = 0; i < bcmx_unit_count; i++) {
        if (bcm_unit == bcmx_unit_list[i]) {
            found = TRUE;
            for (j = i + 1; j < bcmx_unit_count; j++) {
                bcmx_unit_list[j - 1] = bcmx_unit_list[j];
            }
            bcmx_unit_count--;
            break;
        }
    }
    if (!found) {
        BCMX_CONFIG_UNLOCK;
        return BCM_E_NOT_FOUND;
    }

    /* Remove all port information for given unit */
    _bcmx_port_info_remove_by_unit(bcm_unit);

    BCMX_CONFIG_UNLOCK;

    /* De-register rlink facilities in case local */
    bcmx_rx_device_remove(bcm_unit);
    bcmx_linkscan_device_remove(bcm_unit);
    bcmx_l2_device_remove(bcm_unit);

#if defined(BCM_RPC_SUPPORT)
    /* Detach device from rlink */
    if ((rv = bcm_rlink_device_detach(bcm_unit)) < 0) {
        LOG_WARN(BSL_LS_BCMX_COMMON,
                 (BSL_META("BCMX(unit %d): Rlink device detach failed: %d\n"),
                  bcm_unit, rv));
    }
#endif

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmx_lport_valid
 * Purpose:
 *     Check if given LPORT is an existing valid BCMX logical port.
 * Parameters:
 *     lport - The logical port
 * Returns:
 *     TRUE  - LPORT is valid and exists
 *     FALSE - LPORT value is invalid or it does not exist
 */
int
bcmx_lport_valid(bcmx_lport_t lport)
{
    int                index;
    _bcmx_port_info_t  *port_info;

    if (!BCMX_READY || !BCMX_IS_LPORT(lport)) {
        return FALSE;
    }

    BCMX_CONFIG_LOCK;
    port_info = _bcmx_port_info_find(lport, &index);
    BCMX_CONFIG_UNLOCK;

    return (port_info != NULL);
}


/****************************************************************
 *
 * CPU port accessor
 */

/*
 * Function:
 *     bcmx_lport_local_cpu_get
 * Purpose:
 *     Get the lport associated to the device CPU port.
 * Parameters:
 *     bcm_unit - Device number
 *     lport    - (OUT) BCMX logical port
 * Returns:
 *     BCM_E_NONE on success; sets lport to the CPU port.
 *     BCM_E_NOT_FOUND otherwise; lport is set to BCMX_LPORT_INVALID
 * Notes:
 */
int
bcmx_lport_local_cpu_get(int bcm_unit, bcmx_lport_t *lport)
{
    BCMX_READY_CHECK;
    BCMX_PARAM_NULL_CHECK(lport);

    *lport = bcmx_lport_local_cpu[bcm_unit];
    if (bcmx_lport_local_cpu[bcm_unit] == BCMX_LPORT_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}


/****************************************************************
 *
 * Logical and User port functions
 */

/*
 * Function:
 *     bcmx_uport_get
 * Purpose:
 *     Get the uport associated with an lport.
 * Parameters:
 *     lport - The BCMX logical port
 * Returns:
 *     The uport for the given lport.
 *     'uport' is invalid on failure (lport not found)
 *     You can also use the macro BCMX_UPORT_GET.
 */
bcmx_uport_t
bcmx_uport_get(bcmx_lport_t lport)
{
    int                index;
    _bcmx_port_info_t  *port_info;
    bcmx_uport_t       uport = BCMX_UPORT_INVALID;

    if (!BCMX_READY || !BCMX_IS_LPORT(lport)) {
        return BCMX_UPORT_INVALID;
    }

    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
        uport = port_info->port.uport;
    }
    BCMX_CONFIG_UNLOCK;

    return uport;
}


/*
 * Function:
 *     bcmx_uport_set
 * Purpose:
 *     Set the uport associated with an lport.
 * Parameters:
 *     lport - The BCMX logical port
 *     uport - The user port
 * Returns:
 *     BCM_E_XXX;
 *     Returns an error if the uport is already bound to a
 *     different lport.
 */
int
bcmx_uport_set(bcmx_lport_t lport, bcmx_uport_t uport)
{
    int                 rv = BCM_E_NONE;
    int                 index;
    _bcmx_port_info_t   *port_info;
    bcmx_lport_t        tmp_lport;

    BCMX_READY_CHECK;
    BCMX_LPORT_CHECK(lport);

    BCMX_CONFIG_LOCK;

    /* Invalid uport: clear current lport <-> uport */
    if (uport == BCMX_UPORT_INVALID) {
        if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
            _bcmx_uport_remove(port_info->port.uport);
            port_info->port.uport = uport;
        } else {
            rv = BCM_E_PORT;
        }

        BCMX_CONFIG_UNLOCK;
        return rv;
    }

    /* See if uport is already mapped to a logical port */
    tmp_lport = BCMX_UPORT_TO_LPORT(uport);
    if (BCMX_LPORT_VALID(tmp_lport)) {
        /* Check if logical port is same, if so, nothing to do, return ok */
        if (tmp_lport != lport) {
            rv = BCM_E_EXISTS;
        }

        BCMX_CONFIG_UNLOCK;
        return rv;
    }

    /* Update lport <-> uport */
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {

        /* Update uport <-> lport mapping, if different */
        if (port_info->port.uport != uport) {
            if (_bcmx_uport_add(uport, lport) == NULL) {
                rv = BCM_E_MEMORY;
            } else {
                _bcmx_uport_remove(port_info->port.uport);
                port_info->port.uport = uport;
            }
        }

    } else {
        rv = BCM_E_PORT;
    }

    BCMX_CONFIG_UNLOCK;

    return rv;
}


/*
 * Function:
 *     bcmx_lport_to_unit_port
 * Purpose:
 *     Map an LPORT to a BCM dispatch unit/port pair.
 * Parameters:
 *     lport - The port to map
 *     unit  - (OUT) Device unit mapped to lport
 *     port  - (OUT) Device port mapped to lport
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_lport_to_unit_port(bcmx_lport_t lport, int *unit, bcm_port_t *port)
{
    int                rv = BCM_E_PORT;
    int                index;
    _bcmx_port_info_t  *port_info;

    BCMX_READY_CHECK;
    BCMX_LPORT_CHECK(lport);
    BCMX_PARAM_NULL_CHECK(unit);
    BCMX_PARAM_NULL_CHECK(port);

    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
        *unit = port_info->port.bcm_unit;
        *port = port_info->port.bcm_port;
        rv = BCM_E_NONE;
    }
    BCMX_CONFIG_UNLOCK;

    return rv;
}


/*
 * Function:
 *     bcmx_lport_to_modid_port
 * Purpose:
 *     Map an LPORT to a device module ID and physical port.
 * Parameters:
 *     lport - The port to map
 *     modid - (OUT) System mod id mapped to lport
 *     port  - (OUT) Unit's port mapped to lport
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_lport_to_modid_port(bcmx_lport_t lport,
                         int *modid, bcm_port_t *modport)
{
    int                rv = BCM_E_PORT;
    int                index;
    _bcmx_port_info_t  *port_info;

    BCMX_READY_CHECK;
    BCMX_LPORT_CHECK(lport);
    BCMX_PARAM_NULL_CHECK(modid);
    BCMX_PARAM_NULL_CHECK(modport);

    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(lport, &index)) != NULL) {
        *modid = port_info->port.modid;
        *modport  = port_info->port.modport;
        rv = BCM_E_NONE;
    }
    BCMX_CONFIG_UNLOCK;

    return rv;
}


/*
 * Function:
 *     bcmx_lport_to_modid
 * Purpose:
 *     Map an LPORT to its mod id, if appropriate.
 * Parameters:
 *     lport - The lport to lookup
 *     modid - (OUT) Returns the modid for given port
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_lport_to_modid(bcmx_lport_t lport, int *modid)
{
    int         rv;
    bcm_port_t  modport;

    rv = bcmx_lport_to_modid_port(lport, modid, &modport);

    return rv;
}


/*
 * Function:
 *     bcmx_unit_port_to_lport
 * Purpose:
 *     Map BCM unit and port number to logical port.
 * Parameters:
 *     unit - BCM unit to map
 *     port - Unit's port to map
 * Returns:
 *     lport value
 * Notes:
 */
bcmx_lport_t
bcmx_unit_port_to_lport(int unit, bcm_port_t port)
{
    bcmx_lport_t          lport = BCMX_LPORT_INVALID;
    bcm_gport_t           key;
    int                   index;
    _bcmx_devport_hash_t  *devport;

    if (!BCMX_READY) {
        return BCMX_LPORT_INVALID;
    }

    BCMX_CONFIG_LOCK;

    BCM_GPORT_DEVPORT_SET(key, unit, port);
    index = BCMX_PORT_HASH_INDEX(key);

    devport = _bcmx_devport_hash[index];
    while (devport != NULL) {
        if (devport->key == key) {
            if (devport->port_info != NULL) {
                lport = devport->port_info->lport;
            }
            break;
        }
        devport = devport->next;
    }

    BCMX_CONFIG_UNLOCK;   

    return lport;
}


/*
 * Function:
 *     bcmx_modid_port_to_lport
 * Purpose:
 *     Map module ID and physical port to logical port.
 * Parameters:
 *     modid - System mod id to map
 *     port  - Unit's port to map
 * Returns:
 *     lport value
 * Notes:
 */
bcmx_lport_t
bcmx_modid_port_to_lport(int modid, bcm_port_t port)
{
    bcmx_lport_t       lport = BCMX_LPORT_INVALID;
    bcm_gport_t        key;
    int                index;
    _bcmx_port_info_t  *port_info;

    if (!BCMX_READY) {
        return BCMX_LPORT_INVALID;
    }

    /* Generate hash key (gport value) from mod/port */
    BCM_GPORT_MODPORT_SET(key, modid, port);
    
    BCMX_CONFIG_LOCK;
    if ((port_info = _bcmx_port_info_find(key, &index)) != NULL) {
        if ((port_info->port.modid == modid) &&
            (port_info->port.modport == port)) {
            lport = key;
        }
    }
    BCMX_CONFIG_UNLOCK;

    return lport;
}


/*
 * Function:
 *     bcmx_uport_to_lport
 * Purpose:
 *     Map a user port to a BCMX lport.
 * Parameters:
 *     uport - The user port
 * Returns:
 *     lport; may be invalid if uport not found.
 *     You can also use the macro BCMX_UPORT_TO_LPORT
 */
bcmx_lport_t
bcmx_uport_to_lport(bcmx_uport_t uport)
{
    _bcmx_uport_hash_t  *cur;
    bcmx_lport_t        lport = BCMX_LPORT_INVALID;

    if (!BCMX_READY) {
        return BCMX_LPORT_INVALID;
    }

    BCMX_CONFIG_LOCK;
    cur = _bcmx_uport_hash[BCMX_UPORT_HASH(uport)];
    while (cur != NULL) {
        if (BCMX_UPORT_EQ(uport, cur->uport)) {
            lport = cur->lport;
            break;
        }
        cur = cur->next;
    }
    BCMX_CONFIG_UNLOCK;

    return lport;
}


/*
 * Function:
 *     bcmx_modid_to_bcm_unit
 * Purpose:
 *     Get the device unit number associated with given module id.
 * Parameters:
 *     modid    - Device module id
 *     bcm_unit - (OUT) The device unit number for given module id
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_modid_to_bcm_unit(int modid, int *bcm_unit)
{
    int                rv = BCM_E_NOT_FOUND;
    int                i;
    _bcmx_port_info_t  *port_info;

    BCMX_READY_CHECK;
    BCMX_PARAM_NULL_CHECK(bcm_unit);

    BCMX_CONFIG_LOCK;

    /* Check each list */
    for (i = 0; i < BCMX_PORT_HASH_COUNT; i++) {
        port_info = _bcmx_port_info[i];

        /* Check each entry in link list */
        while (port_info != NULL) {
            if (port_info->port.modid == modid) {
                *bcm_unit = port_info->port.bcm_unit;
                rv = BCM_E_NONE;
                break;
            }
            port_info = port_info->next;
        }
    }

    BCMX_CONFIG_UNLOCK;

    return rv;
}

/*
 * Function:
 *     bcmx_uport_to_unit_port
 * Purpose:
 *     Map a user port to BCM unit and port pair.
 * Parameters:
 *     uport    - The user port
 *     bcm_unit - (OUT) BCM unit
 *     bcm_port - (OUT) BCM port
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_uport_to_unit_port(bcmx_uport_t uport,
                        int *bcm_unit, bcm_port_t *bcm_port)
{
    int           rv = BCM_E_NOT_FOUND;
    bcmx_lport_t  lport;

    BCMX_READY_CHECK;
    BCMX_PARAM_NULL_CHECK(bcm_unit);
    BCMX_PARAM_NULL_CHECK(bcm_port);

    BCMX_CONFIG_LOCK;
    if ((lport = bcmx_uport_to_lport(uport)) != BCMX_LPORT_INVALID) {
        rv = bcmx_lport_to_unit_port(lport, bcm_unit, bcm_port);
    }
    BCMX_CONFIG_UNLOCK;

    return rv;
}


/*
 * NOTE
 * The following wrapper routines are necessary to maintain
 * support of old public macros.
 */
int
bcmx_lport_bcm_unit(bcmx_lport_t lport)
{
    int         unit = -1;
    bcm_port_t  port = -1;

    (void)bcmx_lport_to_unit_port(lport, &unit, &port);

    return unit;
}

bcm_port_t
bcmx_lport_bcm_port(bcmx_lport_t lport)
{
    int         unit = -1;
    bcm_port_t  port = -1;

    (void)bcmx_lport_to_unit_port(lport, &unit, &port);

    return port;
}

int
bcmx_lport_modid(bcmx_lport_t lport)
{
    int         modid = -1;
    bcm_port_t  modport = -1;

    (void)bcmx_lport_to_modid_port(lport, &modid, &modport);

    return modid;
}

bcm_port_t
bcmx_lport_modport(bcmx_lport_t lport)
{
    int         modid = -1;
    bcm_port_t  modport = -1;

    (void)bcmx_lport_to_modid_port(lport, &modid, &modport);

    return modport;
}

uint32
bcmx_lport_flags(bcmx_lport_t lport)
{
    uint32  flags = 0x0;

    (void)_bcmx_lport_flags_get(lport, &flags);

    return flags;
}


/*
 * Function:
 *     _bcmx_port_info_dump
 * Purpose:
 *     Displays internal BCMX port information table.
 *     Used for debugging purposes.
 * Parameters:
 *     none
 * Returns:
 *     BCM_E_XXX
 */
int
_bcmx_port_info_dump(void)
{
    int                i;
    _bcmx_port_info_t  *port_info;

    BCMX_INIT_CHECK;

    LOG_INFO(BSL_LS_BCMX_COMMON,
             (BSL_META("BCMX port info dump\n")));

    BCMX_CONFIG_LOCK;

    /* Iterate over hash table link list */
    for (i  = 0; i < BCMX_PORT_HASH_COUNT; i++) {
        port_info = _bcmx_port_info[i];

        /* Check each entry in link list */
        while (port_info != NULL) {
            LOG_INFO(BSL_LS_BCMX_COMMON,
                     (BSL_META(" %d: lport=0x%x unit=%d port=%d uport=%d flags=0x%x "
                      "modid=%d modport=%d\n"),
                      i, port_info->lport,
                      port_info->port.bcm_unit, port_info->port.bcm_port,
                      PTR_TO_INT(port_info->port.uport), port_info->port.flags,
                      port_info->port.modid, port_info->port.modport));
            port_info = port_info->next;
        }
    }

    BCMX_CONFIG_UNLOCK;

    return BCM_E_NONE;
}


/*
 * BCMX Generic Conversion Destination Functions
 *
 * The following set of internal conversion routines for destination
 * to/from BCMX/BCM handle both, old and new gport, BCM port formats.
 *
 * The BCMX driver should use these new routines rather than
 * the previous conversion routines (e.g. bcmx_lport_to_modid_port)
 * which only converts to/from old BCM port format.
 *
 * In addition, these generic routines will help with the current work
 * in progress at the BCM layer to have all APIs handle the new gport
 * format.
 */
 
/*
 * Function:
 *     _bcmx_dest_bcmx_t_init
 * Purpose:
 *     Initializer routine for BCMX destination structure.
 * Parameters:
 *     dest_bcmx - (OUT) Pointer to structure to initialize
 * Returns:
 *     None
 */
void
_bcmx_dest_bcmx_t_init(_bcmx_dest_bcmx_t *dest_bcmx)
{
    if (dest_bcmx != NULL) {
        dest_bcmx->port  = BCMX_LPORT_INVALID;
        dest_bcmx->trunk = BCM_TRUNK_INVALID;
        dest_bcmx->mcast = -1;
    }
}

/*
 * Function:
 *     _bcmx_dest_bcm_t_init
 * Purpose:
 *     Initializer routine for BCM destination structure.
 * Parameters:
 *     dest_bcm - (OUT) Pointer to structure to initialize
 * Returns:
 *     None
 */
void
_bcmx_dest_bcm_t_init(_bcmx_dest_bcm_t *dest_bcm)
{
     if (dest_bcm != NULL) {
         dest_bcm->unit        = -1;
         dest_bcm->port        = BCM_GPORT_INVALID;
         dest_bcm->module_id   = -1;
         dest_bcm->module_port = BCM_GPORT_INVALID;
         dest_bcm->trunk       = BCM_TRUNK_INVALID;
         dest_bcm->mcast       = -1;
     }
}

/*
 * Function:
 *     _bcmx_dest_to_bcm
 * Purpose:
 *     Converts BCMX to BCM destination information.
 *
 *     Translation of data is on flags setting and one in following order:
 *     (a) If MCAST flag is set, set mcast field using old format
 *     (b) If TRUNK flag is set, set trunk field using old format
 *     (c) Else,
 *         (c.1) If logical port is in GPORT format:
 *               - If GPORT_AWARE flag is set, direct assignment to port field,
 *                 with the exception of DEVPORT.  See notes below.
 *               - Else, convert to old non-gport format based on GPORT type
 *         (c.2) Else, error
 * Parameters:
 *     from_bcmx - BCMX destination information to convert
 *     to_bcm    - (OUT) Returns converted BCM destination information
 *     flags     - (IN/OUT) Indicates what is being converted
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 * Notes:
 * (1) A BLACK HOLE gport is being handled at the BCMX layer by setting
 *     the 'discard' flag (if any).   The extra type check can be removed
 *     once BCM handles the BLACK HOLE gport type.
 * (2) Note that when GPORT format is returned, the GPORT value is
 *     returned in both members "port" and "module_port".
 * (3) If given logical port is a DEVPORT, port will return
 *     in old BCM port format regardless of flag.  The unit in DEVPORT
 *     has meaning only in BCMX.
 */
int
_bcmx_dest_to_bcm(_bcmx_dest_bcmx_t *from_bcmx, _bcmx_dest_bcm_t *to_bcm,
                  uint32 *flags)
{
    uint32  flags_out = 0x0;

    BCMX_PARAM_NULL_CHECK(to_bcm);
    BCMX_PARAM_NULL_CHECK(from_bcmx);
    BCMX_PARAM_NULL_CHECK(flags);

    _bcmx_dest_bcm_t_init(to_bcm);

    /* Convert to BCM */

    /* Check MCAST flag first */
    if (*flags & BCMX_DEST_MCAST) {
        flags_out |= BCMX_DEST_MCAST;
        to_bcm->mcast = from_bcmx->mcast;

    } else if (*flags & BCMX_DEST_TRUNK) {
        /* Check TRUNK flag next */
        flags_out |= BCMX_DEST_TRUNK;
        to_bcm->trunk = from_bcmx->trunk;

    } else {
        
        if (BCM_GPORT_IS_SET(from_bcmx->port)) {
            /*
             * If GPORT_AWARE flag is set, direct assignment
             * (BCM can handle gport format).
             */
            if (*flags & BCMX_DEST_GPORT_AWARE) {
                flags_out |= BCMX_DEST_GPORT_AWARE;
                to_bcm->port = from_bcmx->port;
                to_bcm->module_port = from_bcmx->port;

                /* Get also the 'unit' and 'module-id', if applicable */
                if (BCM_GPORT_IS_MODPORT(from_bcmx->port) ||
                    BCM_GPORT_IS_DEVPORT(from_bcmx->port)) {
                    bcm_port_t  port_tmp = -1;

                    (void)bcmx_lport_to_unit_port(from_bcmx->port,
                                                  &to_bcm->unit,
                                                  &port_tmp);
                    /*
                     * If DEVPORT, set the port to old format
                     * since the unit in DEVPORT is in BCMX context and
                     * has no meaning in the corresponding remote device.
                     */
                    if (BCM_GPORT_IS_DEVPORT(from_bcmx->port)) {
                        to_bcm->port = port_tmp;
                        to_bcm->module_port = port_tmp;
                    }

                    (void)bcmx_lport_to_modid_port(from_bcmx->port,
                                                   &to_bcm->module_id,
                                                   &port_tmp);
                }

            } else {
                /*
                 * Convert gport to old format based on GPORT type:
                 *     MCAST           ==> set mcast flag, set mcast
                 *     TRUNK           ==> set trunk flag, set trunk
                 *     BLACK_HOLE      ==> set discard flag, set port
                 *     LOCAL_CPU       ==> set local_cpu flag, set port
                 *     MODPORT/DEVPORT ==> set unit/port and mod/port
                 *     Other           ==> direct assignment
                 */
                if (BCM_GPORT_IS_MCAST(from_bcmx->port)) {
                    flags_out |= BCMX_DEST_MCAST;
                    to_bcm->mcast = BCM_GPORT_MCAST_GET(from_bcmx->port);
                    
                } else if (BCM_GPORT_IS_TRUNK(from_bcmx->port)) {
                    flags_out |= BCMX_DEST_TRUNK;
                    to_bcm->trunk = BCM_GPORT_TRUNK_GET(from_bcmx->port);

                } else if (BCM_GPORT_IS_BLACK_HOLE(from_bcmx->port)) {
                    flags_out |= (BCMX_DEST_DISCARD | BCMX_DEST_GPORT_AWARE);
                    to_bcm->port = from_bcmx->port;
                    to_bcm->module_port = from_bcmx->port;

                } else if (BCM_GPORT_IS_LOCAL_CPU(from_bcmx->port)) {
                    flags_out |= (BCMX_DEST_LOCAL_CPU | BCMX_DEST_GPORT_AWARE);
                    to_bcm->port = from_bcmx->port;
                    to_bcm->module_port = from_bcmx->port;

                } else if (BCM_GPORT_IS_MODPORT(from_bcmx->port) ||
                           BCM_GPORT_IS_DEVPORT(from_bcmx->port)) {
                    /*
                     * NOTE that bcmx_lport_to_modid_port() returns
                     * success when it finds an existing bcmx logical port
                     * (so, some calls could return a modid = -1, but a valid
                     * bcm port number, in cases when the device does not
                     * have a module id assigned to id.
                     */
                    BCM_IF_ERROR_RETURN
                        (bcmx_lport_to_unit_port(from_bcmx->port,
                                                 &to_bcm->unit,
                                                 &to_bcm->port));
                    BCM_IF_ERROR_RETURN
                        (bcmx_lport_to_modid_port(from_bcmx->port,
                                                  &to_bcm->module_id,
                                                  &to_bcm->module_port));
                } else {
                    flags_out |= BCMX_DEST_GPORT_AWARE;
                    to_bcm->port = from_bcmx->port;
                    to_bcm->module_port = from_bcmx->port;
                }
            }

        } else {
            return BCM_E_PORT;
        }
    }

    *flags = flags_out;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcmx_dest_from_bcm
 * Purpose:
 *     Converts BCM to BCMX destination information.
 *
 *     Translation of data is on flags setting and one in following order:
 *     (a) If MCAST flag is set, set mcast field using old format
 *     (b) If TRUNK flag is set, set trunk field using old format
 *     (c) Else,
 *         (c.1) If port (either 'port' or 'module_port') is in GPORT format,
 *               direct assignment to BCMX port field
 *         (c.2) Else, translate BCM old format, unit/port or mod/port, to
 *               BCMX lport
 *
 * Parameters:
 *     to_bcmx  - (OUT) Returns converted BCMX destination information
 *     from_bcm - BCM destination information to convert
 *     flags    - (IN/OUT) Indicates what is being converted
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 */
int
_bcmx_dest_from_bcm(_bcmx_dest_bcmx_t *to_bcmx, _bcmx_dest_bcm_t *from_bcm,
                    uint32 *flags)
{
    uint32  flags_out = 0x0;

    BCMX_PARAM_NULL_CHECK(to_bcmx);
    BCMX_PARAM_NULL_CHECK(from_bcm);
    BCMX_PARAM_NULL_CHECK(flags);

    _bcmx_dest_bcmx_t_init(to_bcmx);

    /* Check MCAST flag first */
    if (*flags & BCMX_DEST_MCAST) {
        flags_out |= BCMX_DEST_MCAST;
        to_bcmx->mcast = from_bcm->mcast;

    } else if (*flags & BCMX_DEST_TRUNK) {
        /* Check TRUNK flag next */
        flags_out |= BCMX_DEST_TRUNK;
        to_bcmx->trunk = from_bcm->trunk;

    } else {
        /* BCMX lport is always returning in GPORT format */
        flags_out |= BCMX_DEST_GPORT_AWARE;

        /*
         * If port is in GPORT format ==> direct assigment
         * NOTE: Use either 'port' or 'module_port' value.
         *       If both are 'valid' gport values, they need to be
         *       the same.
         *
         * Otherwise get the BCMX lport from old format.
         * Try mod/port pair first, if fails, try unit/port
         */
        if (BCM_GPORT_IS_SET(from_bcm->port) && 
            BCM_GPORT_IS_SET(from_bcm->module_port)) {
            if (from_bcm->port != from_bcm->module_port) {
                return BCM_E_PORT;
            }
            to_bcmx->port = (bcmx_lport_t) from_bcm->port;

        } else if (BCM_GPORT_IS_SET(from_bcm->port)) {
            to_bcmx->port = (bcmx_lport_t) from_bcm->port;

        } else if (BCM_GPORT_IS_SET(from_bcm->module_port)) {
            to_bcmx->port = (bcmx_lport_t) from_bcm->module_port;

        } else {
            /* Convert from old format */
            to_bcmx->port = bcmx_modid_port_to_lport(from_bcm->module_id,
                                                     from_bcm->module_port);
            if (to_bcmx->port == BCMX_LPORT_INVALID) {
                to_bcmx->port = bcmx_unit_port_to_lport(from_bcm->unit,
                                                        from_bcm->port);
            }

            if (to_bcmx->port == BCMX_LPORT_INVALID) {
                return BCM_E_PORT;
            }
        }
    }

    *flags = flags_out;

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcmx_dest_to_modid_port
 * Purpose:
 *     Converts BCMX logical port to BCM module/port destination in
 *     old or new port format, based on flags.
 * Parameters:
 *     port       - BCMX logical port to convert
 *     bcm_module - (OUT) Returns BCM module id (when in old format)
 *     bcm_port   - (OUT) Returns BCM port in old or new gport format
 *     flags      - Indicates whether to convert to old or new BCM port format
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 * Notes:
 *     Valid flags are:
 *     BCMX_DEST_GPORT_AWARE - Convert to new gport format
 */
int
_bcmx_dest_to_modid_port(bcmx_lport_t port,
                         bcm_module_t *bcm_module, bcm_port_t *bcm_port,
                         uint32 flags)
{
    uint32             flags_orig = flags;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;

    BCMX_DEST_MODID_PORT_FLAG_CHECK(flags);

    _bcmx_dest_bcmx_t_init(&from_bcmx);
    from_bcmx.port = port;

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_bcm(&from_bcmx, &to_bcm, &flags));

    if ((to_bcm.module_port == BCM_GPORT_INVALID) ||
        ((flags_orig & BCMX_DEST_GPORT_AWARE) &&
         !BCM_GPORT_IS_SET(to_bcm.module_port))) {
        return BCM_E_PORT;
    }

    *bcm_module = to_bcm.module_id;
    *bcm_port   = to_bcm.module_port;

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcmx_dest_from_modid_port
 * Purpose:
 *     Converts BCM module/port, in old or new gport format, to
 *     BCMX logical port destination.
 * Parameters:
 *     port       - (OUT) Returns BCMX logical port
 *     bcm_module - BCM module id to convert
 *     bcm_port   - BCM port to convert (in old or new gport format)
 *     flags      - Not used currently, but valid flag must be passed in
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 * Notes:
 *     Valid flags are:
 *     BCMX_DEST_GPORT_AWARE - No effect when converting 'from' BCM
 */
int
_bcmx_dest_from_modid_port(bcmx_lport_t *port,
                           bcm_module_t bcm_module, bcm_port_t bcm_port,
                           uint32 flags)
{
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    BCMX_DEST_MODID_PORT_FLAG_CHECK(flags);

    _bcmx_dest_bcm_t_init(&from_bcm);
    from_bcm.module_id   = bcm_module;
    from_bcm.module_port = bcm_port;

    BCM_IF_ERROR_RETURN(_bcmx_dest_from_bcm(&to_bcmx, &from_bcm, &flags));

    *port = to_bcmx.port;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcmx_dest_to_unit_port
 * Purpose:
 *     Converts BCMX logical port to BCM unit/port destination in
 *     old or new port format, based on flags.
 * Parameters:
 *     port     - BCMX logical port to convert
 *     bcm_unit - (OUT) Returns BCM device unit
 *     bcm_port - (OUT) Returns BCM port in old or new gport format
 *     flags    - Indicates whether to convert to old or new BCM port format
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 * Notes:
 *     Valid flags are:
 *     BCMX_DEST_GPORT_AWARE - Convert to new gport format
 *
 *     If given 'port' is a DEVPORT, 'bcm_port' will return
 *     in old BCM port format regardless of flag.  The unit in DEVPORT
 *     has meaning only in BCMX.
 */
int
_bcmx_dest_to_unit_port(bcmx_lport_t port,
                        int *bcm_unit, bcm_port_t *bcm_port,
                        uint32 flags)
{
    uint32             flags_orig = flags;
    _bcmx_dest_bcm_t   to_bcm;
    _bcmx_dest_bcmx_t  from_bcmx;

    BCMX_DEST_UNIT_PORT_FLAG_CHECK(flags);

    _bcmx_dest_bcmx_t_init(&from_bcmx);
    from_bcmx.port = port;

    BCM_IF_ERROR_RETURN(_bcmx_dest_to_bcm(&from_bcmx, &to_bcm, &flags));

    if ((to_bcm.unit == -1) ||
        (to_bcm.port == BCM_GPORT_INVALID) ||
        (!BCM_GPORT_IS_DEVPORT(from_bcmx.port) &&
         ((flags_orig & BCMX_DEST_GPORT_AWARE) &&
          !BCM_GPORT_IS_SET(to_bcm.port)))) {
        return BCM_E_PORT;
    }

    *bcm_unit = to_bcm.unit;
    *bcm_port = to_bcm.port;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcmx_dest_from_unit_port
 * Purpose:
 *     Converts BCM unit/port, in old or new gport format, to
 *     BCMX logical port destination.
 * Parameters:
 *     port     - (OUT) Returns BCMX logical port
 *     bcm_unit - BCM device unit to convert
 *     bcm_port - BCM port to convert (in old or new gport format)
 *     flags    - Not used currently, but valid flag must be passed in
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_xxx  - Failure
 * Notes:
 *     Valid flags are:
 *     BCMX_DEST_GPORT_AWARE - No effect when converting 'from' BCM
 */
int
_bcmx_dest_from_unit_port(bcmx_lport_t *port,
                          int bcm_unit, bcm_port_t bcm_port,
                          uint32 flags)
{
    _bcmx_dest_bcm_t   from_bcm;
    _bcmx_dest_bcmx_t  to_bcmx;

    BCMX_DEST_UNIT_PORT_FLAG_CHECK(flags);

    _bcmx_dest_bcm_t_init(&from_bcm);
    from_bcm.unit = bcm_unit;
    from_bcm.port = bcm_port;

    BCM_IF_ERROR_RETURN(_bcmx_dest_from_bcm(&to_bcmx, &from_bcm, &flags));

    *port = to_bcmx.port;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcmx_error_check
 * Purpose:
 *     Checks given BCM error code as follows:
 *     - If error code is BCM_E_UNAVAIL and unit is fabric,
 *       'rv' value is unchanged and function returns successful.
 *     - If error code is successful or matches the ignore code,
 *       'rv' and function returns successful.
 *     - Else, 'rv' and function returns failure code.
 * Parameters:
 *     unit   - Device unit
 *     check  - Error code to be checked
 *     ignore - Error code to ignore (treated as BCM_E_NONE)
 *     rv     - (IN/OUT) Returns value based on above logic.
 * Returns:
 *    BCM_E_NONE    - Successful, given error code to check is:
 *                      is a successful error code, or
 *                      matches ignore code, or
 *                      is BCM_E_UNAVAIL on a fabric unit
 *    BCM_E_XXX     - Failure
 */
int
_bcmx_error_check(int unit, int check, int ignore, int *rv)
{
    if (BCMX_FABRIC_E_UNAVAIL(unit, check)) {
        return BCM_E_NONE;
    }

    if (check == ignore) {
        check = BCM_E_NONE;
    }

    *rv = check;
    
    return *rv;
}
