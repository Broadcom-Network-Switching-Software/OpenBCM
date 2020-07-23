/*! \file trunk.c
 *
 * BCM level APIs for stacking.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/types.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/stack_int.h>
#include <bcm_int/ltsw/mbcm/stack.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/multicast.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <bcmltd/chip/bcmltd_str.h>

#define BSL_LOG_MODULE BSL_LS_BCM_STK

/*!
 * \brief Stack database.
 */
typedef struct ltsw_stack_db_s {

    /*! Stack initialized flag. */
    int         initialized;

    /*! Stack module lock. */
    sal_mutex_t lock;
} ltsw_stack_db_t;

/* SW stack related information. */
ltsw_stack_info_t *ltsw_stack_info[BCM_MAX_NUM_UNITS];
/* Other module use stk_my_modid before stk module init.*/
int stk_my_modid[BCM_MAX_NUM_UNITS] = {0};

/*!
 * \brief One stack control entry for each SOC device containing stack
 * book keeping information for that device.
 */
static ltsw_stack_db_t bcm_stack_db[BCM_MAX_NUM_UNITS];

/*! Cause a routine to return SHR_E_INIT if stack subsystem is not initialized. */
#define STACK_INIT(unit) \
    do { \
        if (bcm_stack_db[unit].initialized == FALSE) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while (0)

/*! Stack mutex lock. */
#define STACK_LOCK(unit) \
    do { \
        if (bcm_stack_db[unit].lock) { \
            sal_mutex_take(bcm_stack_db[unit].lock, SAL_MUTEX_FOREVER); \
        } \
    } while (0)

/*! Stack mutex unlock. */
#define STACK_UNLOCK(unit) \
    do { \
        if (bcm_stack_db[unit].lock) { \
            sal_mutex_give(bcm_stack_db[unit].lock); \
        } \
    } while (0)

/*
 * Module ID Mapping on Switch Chips
 *
 * The programming of module/port destinations that are external to a
 * switch chip must match the mapping that is programmed into the
 * associated fabric chips.  Rather than trying to provide an
 * arbitrarily complex set of APIs to do this for the application, a
 * callback can be registered to map each module/port pair when it is
 * stored into hardware or fetched from hardware.
 *
 * For more information, refer to $SDK/doc/modid-mapping.txt.
 */

bcm_stk_modmap_cb_t     _bcm_stk_modmap_cb[BCM_MAX_NUM_UNITS];

/* Maximun logical port number allowed. */
#define PORT_ADDR_MAX(_u) bcmi_ltsw_dev_max_port_addr(_u)

/* Maximun module id supported. */
#define MODID_MAX(_u) bcmi_ltsw_dev_max_modid(_u)

/* Module id count. */
#define NUM_MODID(_u) bcmi_ltsw_dev_modid_count(_u)

/* Validate whether it is XL port. */
#define IS_CL_PORT(_u, _p) bcmi_ltsw_port_is_type((_u), (_p), BCMI_LTSW_PORT_TYPE_CL)

/* Validate whether it is CL port. */
#define IS_XL_PORT(_u, _p) bcmi_ltsw_port_is_type((_u), (_p), BCMI_LTSW_PORT_TYPE_XL)

/* Validate whether it is GE port. */
#define IS_GE_PORT(_u, _p) bcmi_ltsw_port_is_type((_u), (_p), BCMI_LTSW_PORT_TYPE_GE)

/* Validate whether system supports Stack feature. */
#define SUPPORT_STK(_u) \
    do { \
        if (ltsw_feature(_u, LTSW_FT_STK) == 0) { \
            return SHR_E_UNAVAIL; \
        } \
    } while(0)

/*!
 * \brief Create protection mutex for Stack module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_stack_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcm_stack_db[unit].lock == NULL) {
        bcm_stack_db[unit].lock = sal_mutex_create("bcmStackMutex");
        SHR_NULL_CHECK(bcm_stack_db[unit].lock, SHR_E_MEMORY);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for stack module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static void
ltsw_stack_lock_destroy(int unit)
{
    if (bcm_stack_db[unit].lock != NULL) {
        sal_mutex_destroy(bcm_stack_db[unit].lock);
        bcm_stack_db[unit].lock = NULL;
    }
}

static int
bcmi_ltsw_stk_info_deinit(int unit)
{
    int size, modid;
    ltsw_stack_info_t *si;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];
    if (si == NULL) {
        SHR_EXIT();
    }
    size = sizeof(int) * (PORT_ADDR_MAX(unit) + 1);
    if (si->modport) {
        for (modid = 0; modid <= MODID_MAX(unit); modid++) {
            sal_memset(si->modport[modid].l2_if, 0, size);
            SHR_FREE(si->modport[modid].l2_if);
            si->modport[modid].l2_if = NULL;
        }

        size = sizeof(modport_control_t) * (MODID_MAX(unit) + 1);
        sal_memset(si->modport, 0, size);
        SHR_FREE(si->modport);
        si->modport = NULL;
    }

    if (!warm && si->stack_ports_current) {
        (void)bcmi_ltsw_ha_mem_free(unit, si->stack_ports_current);
    }
    if (!warm && si->stack_ports_inactive) {
        (void)bcmi_ltsw_ha_mem_free(unit, si->stack_ports_inactive);
    }
    if (!warm && si->stack_ports_previous) {
        (void)bcmi_ltsw_ha_mem_free(unit, si->stack_ports_previous);
    }

    size = sizeof(ltsw_stack_info_t);
    sal_memset(si, 0, size);
    SHR_FREE(si);
    ltsw_stack_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmi_ltsw_stk_info_init(int unit)
{
    uint32 alloc_size;
    bcm_port_t p;
    uint64_t modid;
    ltsw_stack_info_t *si;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_FREE(ltsw_stack_info[unit]);
    alloc_size = sizeof(ltsw_stack_info_t);
    SHR_ALLOC(ltsw_stack_info[unit], alloc_size, "ltsw_stack_info");
    SHR_NULL_CHECK(ltsw_stack_info[unit], SHR_E_MEMORY);
    sal_memset(ltsw_stack_info[unit], 0, alloc_size);
    si = ltsw_stack_info[unit];

    SHR_FREE(si->modport);
    alloc_size = sizeof(modport_control_t) * (MODID_MAX(unit) + 1);
    SHR_ALLOC(si->modport, alloc_size, "MODPORT info");
    SHR_NULL_CHECK(si->modport, SHR_E_MEMORY);
    sal_memset(si->modport, 0, alloc_size);

    alloc_size = sizeof(int) * (PORT_ADDR_MAX(unit) + 1);
    for (modid = 0; modid <= MODID_MAX(unit); modid++) {
        SHR_FREE(si->modport[modid].l2_if);
        SHR_ALLOC(si->modport[modid].l2_if, alloc_size,
                  "L2_EIF and MODPORT mapping info");
        SHR_NULL_CHECK(si->modport[modid].l2_if,
                       SHR_E_MEMORY);
        sal_memset(si->modport[modid].l2_if, 0, alloc_size);
    }

    /* Assigning defaul value for stack info */
    for (modid = 0; modid <= MODID_MAX(unit); modid++) {
        si->modport[modid].port = BCM_PORT_INVALID;
        for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
            si->modport[modid].l2_if[p] = BCMI_L2_IF_INVALID;
        }
    }
    si->modport_max = bcmi_ltsw_dev_max_modport(unit);
    si->modport_max_first = bcmi_ltsw_dev_modport_max_first(unit);

    alloc_size = sizeof(bcm_pbmp_t);
    si->stack_ports_current = bcmi_ltsw_ha_mem_alloc(
                                  unit,BCMI_HA_COMP_ID_STACK,
                                  BCMINT_STACK_SUB_COMP_ID_STACK_PORTS_CURRENT,
                                  "bcmStackPortsCurrent",
                                  &alloc_size);
    SHR_NULL_CHECK(si->stack_ports_current, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(si->stack_ports_current, 0, alloc_size);
    }

    si->stack_ports_inactive = bcmi_ltsw_ha_mem_alloc(
                                  unit,BCMI_HA_COMP_ID_STACK,
                                  BCMINT_STACK_SUB_COMP_ID_STACK_PORTS_INACTIVE,
                                  "bcmStackPortsInactive",
                                  &alloc_size);
    SHR_NULL_CHECK(si->stack_ports_inactive, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(si->stack_ports_inactive, 0, alloc_size);
    }

    si->stack_ports_previous = bcmi_ltsw_ha_mem_alloc(
                                  unit,BCMI_HA_COMP_ID_STACK,
                                  BCMINT_STACK_SUB_COMP_ID_STACK_PORTS_PREVIOUS,
                                  "bcmStackPortsPrevious",
                                  &alloc_size);
    SHR_NULL_CHECK(si->stack_ports_previous, SHR_E_MEMORY);
    if (!warm) {
        sal_memset(si->stack_ports_previous, 0, alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmi_ltsw_stk_info_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the STACK module.
 *
 * This function is used to de-initialize the STACK module.
 *
 * \param [in]  unit      Unit Number.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
bcmi_ltsw_stk_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcm_stack_db[unit].initialized == FALSE) {
        SHR_EXIT();
    }
    (void)bcmi_ltsw_stk_info_deinit(unit);

    (void)ltsw_stack_lock_destroy(unit);

    bcm_stack_db[unit].initialized = FALSE;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the stack module.
 *
 * Initial stack database data structure and
 * clear the stack related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_stk_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ltsw_stack_lock_create(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_stk_info_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_stack_init(unit));

    bcm_stack_db[unit].initialized = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmi_ltsw_stk_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_modport_traverse
 * Purpose:
 *      Traverse the created modports.
 * Parameters:
 *      unit - (IN) Unit number
 *      cb - (IN) Traverse call back function
 *      user_data - (IN) User data
 * Returns:
 *      BCM_E_xxx
 */
int
bcmi_ltsw_stk_modport_traverse(int unit,
                               bcmi_ltsw_port_traverse_cb cb,
                               void *user_data)
{
    ltsw_stack_info_t *si;
    int modid, port, p, rv;

    SHR_FUNC_ENTER(unit);

    STACK_LOCK(unit);

    STACK_INIT(unit);

    if (user_data == NULL || cb == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    si = ltsw_stack_info[unit];
    for (modid = 0; modid <= MODID_MAX(unit); modid++) {
        if (si->modport[modid].port == BCM_PORT_INVALID) {
            continue;
        }
        for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
            if (si->modport[modid].l2_if[p] != BCMI_L2_IF_INVALID) {
                BCM_GPORT_MODPORT_SET(port, modid, p);
                rv = cb(unit, port, user_data);
                if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                    SHR_ERR_EXIT(rv);
                }
            }
        }
    }

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
* Function:
*      bcmi_ltsw_stk_asymmetric_dual_modmap_map
* Purpose:
*      Internal function for asymmetric dual module ID mapping/unmapping
* Parameters:
*      unit     - (IN)  SOC unit #
*      setget   - (IN)  Mapping direction identifier BCM_STK_MODMAP_GET
                        or BCM_STK_MODMAP_SET
*      mod_in   - (IN)  Module Id to map
*      port_in  - (IN)  Physical port to map no GPORT accepted
*      mod_out  - (OUT) Module Id after mapping
*      port_out - (OUT) Physical port after mapping not a GPORT
* Notes:
*      This function is called from within the BCM API implementations
*      whenever asymmetric dual module ID mapping is needed.
*/
int
bcmi_ltsw_stk_asymmetric_dual_modmap_map(int unit, int setget,
                                         bcm_module_t *mod_in,
                                         bcm_port_t *port_in)
{
    bcm_module_t my_modid;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    STACK_INIT(unit);

    si = ltsw_stack_info[unit];
    /* Get local modulel ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));

    /*
     * Adjust mod/port such that any port number between modport_max_first
     * and modport_max considered to be in next module.
     */
    if((*mod_in == my_modid) && (BCM_MODID_INVALID != *mod_in)) {
        if (setget == BCM_STK_MODMAP_GET &&
                        *port_in > si->modport_max_first) {
            *port_in += (si->modport_max + 1) -
                        (si->modport_max_first + 1);
            *mod_in += *port_in / (si->modport_max + 1);
            *port_in %= (si->modport_max + 1);
        }
    }

    /* Adjust the mod/port such that all ports are considered to be in
     * the first module.
     */
    if (setget == BCM_STK_MODMAP_SET) {
        *port_in += (*mod_in - my_modid) * (si->modport_max_first + 1);
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *      bcmi_ltsw_stk_modmap_map
 * Purpose:
 *      Internal function to call module ID mapping/unmapping callback function
 * Parameters:
 *      unit    - (IN) SOC unit #
 *      setget  - (IN) Mapping direction identifier BCM_STK_MODMAP_GET or BCM_STK_MODMAP_SET
 *      mod_in  - (IN) Module Id to map
 *      port_in  - (IN) Physical port to map no GPORT accepted
 *      mod_out - (OUT) Module Id after mapping
 *      port_out - (OUT) Physical port after mapping not a GPORT
 * Notes:
 *      This function is called from within the BCM API implementations
 *      whenever module ID mapping is needed.
 */
int
bcmi_ltsw_stk_modmap_map(int unit, int setget,
                         bcm_module_t mod_in, bcm_port_t port_in,
                         bcm_module_t *mod_out, bcm_port_t *port_out)
{
    int         rv = SHR_E_NONE;
    bcm_module_t mod;
    bcm_port_t  port;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_STK) == 0) {
        if (port_out != NULL) {
            *port_out = port_in;
        }
        if (mod_out != NULL) {
            *mod_out = mod_in;
        }
        /* EXIT */
        SHR_EXIT();
    }

    si = ltsw_stack_info[unit];
    if ((port_in != -1) && (setget == BCM_STK_MODMAP_GET)) {
        /* Call asymmetric dual modid mapping function to split the
         * ports asymmetrically between two modids.
         */
        if (ltsw_feature(unit, LTSW_FT_ASY_DUAL_MODID) &&
                        port_in > si->modport_max_first) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_stk_asymmetric_dual_modmap_map(unit, setget,
                                                         &mod_in, &port_in));
        } else {
        /* Before the mapping, if multiple modid mode is enabled, adjust the
         * mod/port such that any port number above modport_max setting is
         * considered to be in the next module.
         */
             if (NUM_MODID(unit) > 1) {
                if (port_in > si->modport_max) {
                    bcm_module_t my_modid;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
                    if ((mod_in == my_modid) && (BCM_MODID_INVALID != mod_in)) {
                        mod_in += port_in / (si->modport_max + 1);
                        port_in %= (si->modport_max + 1);
                    }
                }
            }
        }
    }

    if ((_bcm_stk_modmap_cb[unit] != NULL) && (BCM_MODID_INVALID != mod_in)) {
        rv = (*_bcm_stk_modmap_cb[unit])(unit, setget,
                                         mod_in, port_in,
                                         &mod, &port);
        if (rv < 0) {
            mod = mod_in;
            port = port_in;
        }
    } else {
        /* default identity mapping */
        mod = mod_in;
        port = port_in;
    }
    if ((rv >= 0) && (port_in != -1) && (setget == BCM_STK_MODMAP_SET)) {
        /* After the mapping, if multiply-modid mode is enabled, adjust the
         * mod/port such that all ports are considered to be in the first
         * module.
         */
        if (NUM_MODID(unit) > 1) {
            bcm_module_t my_modid;
            int          isLocal;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
            if (BCM_MODID_INVALID != my_modid){
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_modid_is_local(unit, mod, &isLocal));
                if (isLocal) {
                    /* Call asymmetric dual modid unmapping function */
                    if (ltsw_feature(unit, LTSW_FT_ASY_DUAL_MODID)) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_ltsw_stk_asymmetric_dual_modmap_map(
                                unit,setget, &mod_in, &port_in));
                        port = port_in;
                        mod = my_modid;
                    } else {
                        port += (mod - my_modid) * (si->modport_max + 1);
                        mod = my_modid;
                    }
                }
            }
        }
    }
    if (port_out != NULL) {
        *port_out = port;
    }
    if (mod_out != NULL) {
        *mod_out = mod;
    }

exit:
    SHR_FUNC_EXIT();
}

/****************************************************************
 *
 * Stack port set calls:
 *
 *    Callback into BCM layer for notification
 *    Callback to application for notification
 */


/* Linked list of callback registrants */
typedef struct stk_callback_s {
    struct stk_callback_s *next;
    bcm_stk_cb_f fn;
    void *cookie;
} stk_callback_t;

/*
 * Function:
 *      bcm_ltsw_stk_update_callback_register/unregister
 * Purpose:
 *      (Un)register an application function for calbacks from stack port set
 * Parameters:
 *      unit   - ignored
 *      cb     - callback function
 *      cookie - anonymous data passed to callback
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Will only register a given (callback, cookie) pair once.
 *      On unregister, both the callback and cookie must match.
 *      Callbacks are made in the order they are registered.  Enqueue
 *      new registrants at the end of the linked list.
 */

static stk_callback_t *stk_cb;
static stk_callback_t *stk_cb_tail;

/* Register stack update callback at end of list */

int
bcm_ltsw_stk_update_callback_register(int unit, bcm_stk_cb_f cb, void *cookie)
{
    stk_callback_t *node;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    STACK_LOCK(unit);
    /* See if already present */
    node = stk_cb;
    while (node != NULL) {
        if (node->fn == cb && node->cookie == cookie) {
            break; /* Found it */
        }
        node = node->next;
    }

    if (node == NULL) { /* Not there yet */
        SHR_ALLOC(node, sizeof(*node), "stk_cb");
        if (node != NULL) {
            node->fn = cb;
            node->cookie = cookie;
            node->next = NULL;

            /* Enqueue at tail */
            if (stk_cb_tail == NULL) {
                stk_cb = node;
            } else {
                stk_cb_tail->next = node;
            }
            stk_cb_tail = node;
        } else {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_stk_update_callback_unregister(int unit, bcm_stk_cb_f cb, void *cookie)
{
    stk_callback_t *node;
    stk_callback_t *prev;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    STACK_LOCK(unit);
    node = stk_cb;
    prev = NULL;

    /* Scan list for match of both function and cookie */
    while (node != NULL) {
        if ((node->fn == cb) && (node->cookie == cookie)) {
            break; /* Found it */
        }
        prev = node;
        node = node->next;
    }

    if (node != NULL) { /* Found */
        if (prev == NULL) { /* First on list */
            stk_cb = node->next;
        } else { /* Update previous */
            prev->next = node->next;
        }
        if (stk_cb_tail == node) { /* Was last on list */
            stk_cb_tail = prev;
        }
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (node != NULL) {
        SHR_FREE(node);
    }

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

STATIC int
bcmi_ltsw_stk_update_callback(int unit,
                              bcm_port_t port,
                              uint32 flags,
                              int inactiveChkReqd)
{
    stk_callback_t *node;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];
    if ((BCM_PBMP_NEQ(*(si->stack_ports_current),
                      *(si->stack_ports_previous))) || inactiveChkReqd ) {
        /* Make internal callbacks if stack ports have changed */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_vlan_stk_update(unit, flags));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_stk_update(unit));
    }

    /* Make registered callbacks.  Assumes lock is held by caller. */
    node = stk_cb;
    while (node != NULL) {
        (node->fn)(unit, port, flags, node->cookie);
        node = node->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
* Function:
*      bcmi_ltsw_stk_modport_to_l2_if
* Purpose:
*      convert modport to the L2 interface Id.
* Parameters:
*      unit         BCM device number
*      modid      (in) module id Identifier Number.
*      port       (in) port Identifier Number.
*      l2_if      (out) l2_if L2 interface Id.
* Returns:
*      BCM_E_XXX
* Notes:
 */
int
bcmi_ltsw_stk_modport_to_l2_if(int unit, int modid, int port, int *l2_if)
{
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    STACK_LOCK(unit);

    STACK_INIT(unit);

    si = ltsw_stack_info[unit];

    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (port < 0 || port > PORT_ADDR_MAX(unit)) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    *l2_if = si->modport[modid].l2_if[port];

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_l2_if_to_modport
 * Purpose:
 *      convert the L2 interface Id to modport
 * Parameters:
 *      unit         BCM device number
 *      l2_if     (in) l2_if L2 interface Id.
 *      port      (out) port Identifier Number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_ltsw_stk_l2_if_to_modport(int unit, int l2_if, int *port)
{
    ltsw_stack_info_t *si;
    int modid, p;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];

    STACK_LOCK(unit);

    STACK_INIT(unit);

    for (modid = 0; modid <= MODID_MAX(unit); modid++) {
        for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
            if (l2_if == si->modport[modid].l2_if[p]) {
                BCM_GPORT_MODPORT_SET(*port, modid, p);
                SHR_EXIT();
            }
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_pbmp_get
 * Purpose:
 *      Get stack pbmp information
 * Parameters:
 *      unit         BCM device number
 *      curr_pbmp    (OUT) Ports currently engaged in stacking
 *      inac_pbmp    (OUT) Ports stacked, but explicitly blocked
 *      pre_pbmp     (OUT) Last stack port; to detect changes
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_ltsw_stk_pbmp_get(int unit,
                       bcm_pbmp_t *curr_pbmp,
                       bcm_pbmp_t *inac_pbmp,
                       bcm_pbmp_t *pre_pbmp)
{
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    STACK_INIT(unit);

    si = ltsw_stack_info[unit];

    if (curr_pbmp != NULL) {
        *curr_pbmp = *(si->stack_ports_current);
    }

    if (inac_pbmp != NULL) {
        *inac_pbmp = *(si->stack_ports_inactive);
    }

    if (pre_pbmp != NULL) {
        *pre_pbmp = *(si->stack_ports_previous);
    }

exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmi_ltsw_stk_port_set(int unit, bcm_port_t port, uint32 flags)
{
    int isPrevInactiveSet = 0;
    int inactiveChkReqd = 0;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    STACK_INIT(unit);

    STACK_LOCK(unit);

    si = ltsw_stack_info[unit];
    if (BCM_PBMP_MEMBER(*(si->stack_ports_inactive), port))
        isPrevInactiveSet = 1;

    /* Record old stack port states; make changes to current */
    BCM_PBMP_ASSIGN(*(si->stack_ports_previous), *(si->stack_ports_current));

    if (flags & BCM_STK_ENABLE) {
        BCM_PBMP_PORT_ADD(*(si->stack_ports_current), port);
        if (flags & BCM_STK_INACTIVE) {
            BCM_PBMP_PORT_ADD(*(si->stack_ports_inactive), port);
            if (!isPrevInactiveSet)
                inactiveChkReqd = 1;
        } else {
            BCM_PBMP_PORT_REMOVE(*(si->stack_ports_inactive), port);
            if (isPrevInactiveSet)
                inactiveChkReqd = 1;
        }
    } else {
        BCM_PBMP_PORT_REMOVE(*(si->stack_ports_current), port);
        BCM_PBMP_PORT_REMOVE(*(si->stack_ports_inactive), port);
    }
    STACK_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stk_update_callback(unit, port, flags, inactiveChkReqd));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_sys_port_get
 * Purpose:
 *      Transform modport into system port
 * Parameters:
 *      unit  - SOC unit#
 *      modid - module id
 *      port - Port identifier.
 *      sys_port - System port
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcmi_ltsw_stk_sys_port_get(int unit,
                           int modid,
                           bcm_port_t gport,
                           int *sys_port)
{
    bcm_port_t port;

    SHR_FUNC_ENTER(unit);

    if (sys_port == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, gport, &port));
    } else if (gport >= 0 && gport <= PORT_ADDR_MAX(unit)) {
        port = gport;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_sys_port_get(unit, modid, port, sys_port));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_sys_port_to_modport_get
 * Purpose:
 *      Transform system port into modport
 * Parameters:
 *      unit  - SOC unit#
 *      sys_port - System port
 *      modid - module id
 *      port - Port identifier.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcmi_ltsw_stk_sys_port_to_modport_get(int unit,
                                      int sys_port,
                                      int *modid,
                                      bcm_port_t *port)
{
    SHR_FUNC_ENTER(unit);

    if (port == NULL || modid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_sys_port_to_modport_get(unit, sys_port, modid, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create the software data structure for the specified trunk ID.
 *
 * \param [in] unit Device unit number.
 * \param [in] tid The trunk ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
 /*
 * Function:
 *      bcm_ltsw_stk_modmap_map
 * Purpose:
 *      Call module ID mapping/unmapping callback function
 * Parameters:
 *      unit - SOC unit #
 * Notes:
 *      This function is called from within the BCM API implementations
 *      whenever module ID mapping is needed, but it can also be called
 *      by the user application for informational purposes.
 */
int
bcm_ltsw_stk_modmap_map(int unit,
                        int setget,
                        bcm_module_t mod_in,
                        bcm_port_t port_in,
                        bcm_module_t *mod_out,
                        bcm_port_t *port_out)
{
    bcm_port_t      tmp_port;
    bcm_module_t    tmp_modid;
    bcm_trunk_t     tmp_tgid;
    int             tmp_id, isGport;
    bcmi_ltsw_gport_info_t gport_st;

    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_SET(port_in)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, port_in, &tmp_modid, &tmp_port,
                                         &tmp_tgid, &tmp_id));
        if ((BCM_TRUNK_INVALID != tmp_tgid) ||
            (-1 != tmp_id)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    } else {
        tmp_port = port_in;
        tmp_modid = mod_in;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stk_modmap_map(unit, setget, tmp_modid, tmp_port,
                                mod_out, port_out));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
    if (TRUE == isGport) {
        gport_st.modid = *mod_out;
        gport_st.port = *port_out;
        gport_st.gport_type = _SHR_GPORT_TYPE_MODPORT;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_st, port_out));
        *mod_out = -1;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_my_modid_set
 * Purpose:
 *      Set the MY_MODID field
 * Parameters:
 *      unit  - SOC unit#
 *      my_modid - the value to set
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_ltsw_stk_my_modid_set(int unit, int my_modid)
{
    ltsw_stack_info_t *si;
    bcm_port_t p;

    SHR_FUNC_ENTER(unit);

    si = ltsw_stack_info[unit];
    /* Modual ID available range check */
    STACK_LOCK(unit);
    if (!MODID_ADDRESSABLE(unit, my_modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }
    if (ltsw_feature(unit, LTSW_FT_STK)) {
        /* Check whether the modid has been set as a modport for remote module. */
        if (si->modport[my_modid].port != BCM_PORT_INVALID) {
            LOG_ERROR(BSL_LS_BCM_STK,
                     (BSL_META_U(unit,
                                 "Error: Cannot set my module id as "
                                 "enabled remote module id for modport.")));
            SHR_ERR_EXIT(SHR_E_BADID);
        }

        /* Check whether the modid has been enabled as a remote module. */
        for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
            if (my_modid == stk_my_modid[unit]) {
                break;
            }
            if (si->modport[my_modid].l2_if[p] != BCMI_L2_IF_INVALID) {
                LOG_ERROR(BSL_LS_BCM_STK,
                         (BSL_META_U(unit,
                                     "Error: Cannot set my module id as "
                                     "enabled remote module id.")));
                SHR_ERR_EXIT(SHR_E_BADID);
            }
        }
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (mbcm_ltsw_stack_set(unit, my_modid), SHR_E_UNAVAIL);
    }
    stk_my_modid[unit] = my_modid;

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_stk_my_modid_get
 * Purpose:
 *      Get the MY_MODID field
 * Parameters:
 *      unit  - SOC unit#
 *      my_modid - (out)the value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_ltsw_stk_my_modid_get(int unit, int *my_modid)
{
    SHR_FUNC_ENTER(unit);

    if (my_modid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *my_modid = stk_my_modid[unit];

exit:
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_stk_modid_set
 * Purpose:
 *      Set the module id of a unit
 * Parameters:
 *      unit    - device number
 *      modid   - module id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      calls bcm_stk_modmap_map (my_modid_set does not)
 */
int
bcm_ltsw_stk_modid_set(int unit, int modid)
{
    bcm_module_t        mod_out;
    bcm_port_t          port_out;

    SHR_FUNC_ENTER(unit);

    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, modid, -1,
                                  &mod_out, &port_out));
    LOG_INFO(BSL_LS_BCM_STK,
             (BSL_META_U(unit, "STK %d: modid set to %d; mapped %d\n"),
              unit, modid, mod_out));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_set(unit, mod_out));

exit:
    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_stk_modid_get
 * Purpose:
 *      Get the module id of a unit
 * Parameters:
 *      unit    - device number
 *      modid   - module id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      calls bcm_stk_modmap_map (my_modid_set does not)
 */
int
bcm_ltsw_stk_modid_get(int unit, int *modid)
{
    bcm_module_t        mod_out;
    bcm_port_t          port_out;

    SHR_FUNC_ENTER(unit);

    if (modid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, modid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, *modid, -1,
                                  &mod_out, &port_out));
    *modid = mod_out;
exit:
    SHR_FUNC_EXIT();

}


/*
 * Function:
 *      bcm_ltsw_stk_modid_count
 * Purpose:
 *      The number of module identifiers (fabric IDs) needed by the unit
 * Parameters:
 *      unit  - SOC unit#
 *      num_modid - (out)# of MODIDs
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_ltsw_stk_modid_count(int unit, int *num_modid)
{
    SHR_FUNC_ENTER(unit);

    *num_modid = NUM_MODID(unit);

    SHR_FUNC_EXIT();

}

/*
 * Function:
 *      bcm_ltsw_esw_stk_modmap_register
 * Purpose:
 *      Register module ID mapping/unmapping callback function
 * Parameters:
 *      unit - SOC unit #
 *      func - User callback function for module ID mapping/unmapping
 */
int
bcm_ltsw_stk_modmap_register(int unit, bcm_stk_modmap_cb_t func)
{
    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    _bcm_stk_modmap_cb[unit] = func;

    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_esw_stk_modmap_unregister
 * Purpose:
 *      Unregister module ID mapping/unmapping callback function
 * Parameters:
 *      unit - SOC unit #
 */
int
bcm_ltsw_stk_modmap_unregister(int unit)
{
    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    _bcm_stk_modmap_cb[unit] = NULL;

    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_module_remote_port_enable
 * Purpose:
 *      Enable remote ports for the specified module based on the specified
 *      port bitmap. Required for devices that have all remote ports disabled
 *      by default. Note that local ports are always enabled and not
 *      applicable here.
 * Parameters:
 *      unit  - SOC unit#
 *      modid - Module id
 *      pbmp - Bitmap of ports from remote module.
 *      enable - TRUE to enable, FALSE to disable.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcm_ltsw_stk_module_remote_port_enable_set(
    int unit,
    int modid,
    bcm_pbmp_t pbmp)
{
    bcm_port_t p;
    int my_modid;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    STACK_LOCK(unit);

    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    /* Validate ports in pbmp. */
    BCM_PBMP_ITER(pbmp, p) {
        if (p < 0 || p > PORT_ADDR_MAX(unit)) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
    if (modid == my_modid) {
        LOG_ERROR(BSL_LS_BCM_STK,
                 (BSL_META_U(unit,
                             "Error: Cannot enable local module ports.")));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
        if (BCM_PBMP_MEMBER(pbmp, p)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_stack_l2_if_map(unit, modid, p));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_stack_l2_if_clear(unit, modid, p));
        }
    }
exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_module_remote_port_enable_get
 * Purpose:
 *      Get enabled remote port bitmap for the specified module.
 * Parameters:
 *      unit  - SOC unit#
 *      modid - Module id
 *      pbmp - Bitmap of ports from remote module.
 *      enable - TRUE to enable, FALSE to disable.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcm_ltsw_stk_module_remote_port_enable_get(
    int unit,
    int modid,
    bcm_pbmp_t *pbmp)
{
    bcm_port_t p;
    ltsw_stack_info_t *si;
    int my_modid;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    si = ltsw_stack_info[unit];

    STACK_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
    if (modid == my_modid) {
        LOG_ERROR(BSL_LS_BCM_STK,
                 (BSL_META_U(unit,
                             "Error: Cannot enable local module ports.")));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (!MODID_ADDRESSABLE(unit, modid) || pbmp == NULL) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    BCM_PBMP_CLEAR(*pbmp);
    for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
        if (si->modport[modid].l2_if[p] != BCMI_L2_IF_INVALID) {
            BCM_PBMP_PORT_ADD(*pbmp, p);
        }
    }
exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_modport_set
 * Purpose:
 *      Set the port in MODPORT entry for modid
 * Parameters:
 *      unit  - SOC unit#
 *      modid - module id
 *      gport - Global port identifier.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcm_ltsw_stk_modport_set(int unit, int modid, bcm_port_t gport)
{
    bcm_port_t port, p;
    ltsw_stack_info_t *si;
    int my_modid;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    si = ltsw_stack_info[unit];
    LOG_INFO(BSL_LS_BCM_STK,
             (BSL_META_U(unit,
                         "STK %d: modport set: modid %d to port %d\n"),
              unit, modid, gport));
    STACK_LOCK(unit);
    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
    if (modid == my_modid) {
        LOG_ERROR(BSL_LS_BCM_STK,
                 (BSL_META_U(unit,
                             "Error: Cannot set modport for local module.")));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (BCM_GPORT_IS_SET(gport) && !BCM_GPORT_IS_TRUNK(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, gport, &port));
        gport = port;
    }
    /* Setting all the available ports with the modport. */
    for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
    SHR_IF_ERR_VERBOSE_EXIT
        /* If port is a trunk, it store its gport type. */
        (mbcm_ltsw_stack_modport_set(unit, modid, p, gport));
    }
    si->modport[modid].port = gport;
exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_modport_get
 * Purpose:
 *      Get the port in MODPORT entry for modid
 * Parameters:
 *      unit  - SOC unit#
 *      modid - module id
 *      port - (OUT) port number.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcm_ltsw_stk_modport_get(int unit, int modid, bcm_port_t *port)
{
    int isGport, my_modid;
    bcmi_ltsw_gport_info_t gport_st;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    si = ltsw_stack_info[unit];
    if (port == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));
    if (modid == my_modid) {
        LOG_ERROR(BSL_LS_BCM_STK,
                 (BSL_META_U(unit,
                             "Error: Cannot get modport for local module.")));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    *port = si->modport[modid].port;
    if (*port == BCM_PORT_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
    if (TRUE == isGport && !BCM_GPORT_IS_TRUNK(*port)) {
        gport_st.gport_type = _SHR_GPORT_TYPE_MODPORT;
        gport_st.modid = modid;
        gport_st.port = *port;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_construct(unit, &gport_st, port));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_remote_port_modport_get
 * Purpose:
 *      Get the local forwarding port from the remote port.
 * Parameters:
 *      unit         BCM device number
 *      remote_port  (IN) destination port on remote module
 *      local_port   (OUT) local modport for the remote module
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_ltsw_stk_remote_port_modport_get(int unit,
                                      bcm_gport_t remote_port,
                                      bcm_port_t *local_port)
{
    int modid;

    SHR_FUNC_ENTER(unit);

    STACK_INIT(unit);

    modid = BCM_GPORT_MODPORT_MODID_GET(remote_port);
    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_modport_get(unit, modid, local_port));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_ifa_system_source_alloc
 * Purpose:
 *      Allocate IFA 1.0 system_source.
 * Parameters:
 *      unit         BCM device number
 *      sys_source_ifa_probe_copy  (out) ifa system source
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_ltsw_stk_ifa_system_source_alloc(int unit,
                                      int *sys_source_ifa_probe_copy)
{
    SHR_FUNC_ENTER(unit);

    STACK_INIT(unit);

    if (sys_source_ifa_probe_copy == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_ifa_system_source_alloc(unit, sys_source_ifa_probe_copy));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcmi_ltsw_stk_ifa_system_source_destroy
 * Purpose:
 *      Destroy IFA 1.0 system_source.
 * Parameters:
 *      unit         BCM device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_ltsw_stk_ifa_system_source_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    STACK_INIT(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_stack_ifa_system_source_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_port_set
 * Purpose:
 *      Set stacking mode for a port
 * Parameters:
 *      unit     BCM device number
 *      port     BCM port number on device
 *      flags    See bcm/stack.h
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If neither simplex or duplex is specified, port should be
 *      considered "unresolved"; for example, if discovery has not
 *      completed yet.  In general, this should not change a port's
 *      settings, but it may affect the port's setup.
 *
 *      If port < 0, use the unique IPIC_PORT if present
 *
 *      If flags == 0, then the port does not have to be a stack port.
 */

int
bcm_ltsw_stk_port_set(int unit, bcm_port_t port, uint32 flags)
{
    int txp, rxp;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LS_BCM_STK,
                (BSL_META_U(unit,
                            "STK %d: Port set: p %d. flags 0x%x\n"),
                 unit, port, flags));
    SUPPORT_STK(unit);

    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, port, &port));
    }

    if (flags & (BCM_STK_ENABLE | BCM_STK_CAPABLE)) {
        if (!bcmi_ltsw_port_is_type(unit, port, BCMI_LTSW_PORT_TYPE_HG)) {

            /* Setting a non-HG port to stacking implies SL mode */
            if ((flags & BCM_STK_HG) || (!IS_GE_PORT(unit, port))) {
                LOG_WARN(BSL_LS_BCM_STK,
                         (BSL_META_U(unit,
                                     "STK: Invalid SL stk cfg. unit %d, port %d\n"),
                          unit, port));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            flags |= BCM_STK_SL; /* Set courtesy flag for callbacks */

            /* Turn pause off on SL stack ports */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_pause_get(unit, port, &txp, &rxp));

            if ((txp != 0) || (rxp != 0)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm_ltsw_port_pause_set(unit, port, 0, 0));
            }
        } else { /* HG port/HG2 over GE port */
            if (flags & BCM_STK_SL) {
                LOG_WARN(BSL_LS_BCM_STK,
                         (BSL_META_U(unit,
                                     "STK: Invalid HG stk cfg. unit %d, port %d\n"),
                          unit, port));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            flags |= BCM_STK_HG; /* Set courtesy flag for callbacks */
        }

        if ((flags & BCM_STK_SIMPLEX) && (flags & BCM_STK_DUPLEX)) {
            LOG_WARN(BSL_LS_BCM_STK,
                     (BSL_META_U(unit,
                                 "STK: Dimplex not supported. unit %d, port %d\n"),
                      unit, port));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if ((flags & BCM_STK_CUT) && (flags & BCM_STK_SL)) {
        flags |= BCM_STK_INACTIVE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stk_port_set(unit, port, flags));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_port_get
 * Purpose:
 *      Get stack port mode information
 * Parameters:
 *      unit      BCM device number
 *      port      BCM port number on device
 *      flags     (OUT) flags related to stacking
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_ltsw_stk_port_get(int unit, bcm_port_t port, uint32 *flags)
{
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    *flags = 0;
    si = ltsw_stack_info[unit];
    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_local_get(unit, port, &port));
    }

    if (BCM_PBMP_MEMBER(*(si->stack_ports_current), port)) {
        *flags |= BCM_STK_ENABLE;
        if (BCM_PBMP_MEMBER(*(si->stack_ports_inactive), port)) {
            *flags |= BCM_STK_INACTIVE;
        }
    } else {
        *flags |= BCM_STK_NONE;
    }

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *      bcm_ltsw_stk_modport_clear
 * Purpose:
 *      Clear entry in MODPORT table
 * Parameters:
 *      unit  - SOC unit#
 *      modid - module id
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcm_ltsw_stk_modport_clear(int unit, int modid)
{
    ltsw_stack_info_t *si;
    bcm_port_t p;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    si = ltsw_stack_info[unit];
    LOG_VERBOSE(BSL_LS_BCM_STK,
                (BSL_META_U(unit,
                            "STK %d: Clearing mod port info of modid %d.\n"),
                 unit, modid));
    STACK_LOCK(unit);
    if (!MODID_ADDRESSABLE(unit, modid)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }
    for (p = 0; p <= PORT_ADDR_MAX(unit); p++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_stack_modport_clear(unit, modid, p));
    }
    si->modport[modid].port = BCM_PORT_INVALID;

exit:
    STACK_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_stk_modport_clear_all
 * Purpose:
 *      Clear all entries in MODPORT table
 * Parameters:
 *      unit  - SOC unit#
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Supported by all devices except Strata
 */

int
bcm_ltsw_stk_modport_clear_all(int unit)
{
    int i, my_modid;
    ltsw_stack_info_t *si;

    SHR_FUNC_ENTER(unit);

    SUPPORT_STK(unit);

    si = ltsw_stack_info[unit];
    LOG_VERBOSE(BSL_LS_BCM_STK,
                (BSL_META_U(unit,
                            "STK %d: Clearing all mod port info.\n"),
                 unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &my_modid));

    for (i = 0; i <= MODID_MAX(unit); i++) {
        if (i != my_modid && si->modport[i].port != BCM_PORT_INVALID){
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_stk_modport_clear(unit, i));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


