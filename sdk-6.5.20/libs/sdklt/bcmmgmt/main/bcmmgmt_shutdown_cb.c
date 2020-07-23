/*! \file bcmmgmt_shutdown_cb.c
 *
 * Single entry point shut-down of core SDK components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmmgmt/bcmmgmt_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

/*******************************************************************************
 * Local definitions
 */

/*
 * Callback list object.
 *
 * We create and insert one of these object for each registration.
 */
typedef struct shutdown_cb_s {

    /* Pointer to next object in list */
    struct shutdown_cb_s *next;

    /* Callback function for this entry */
    bcmmgmt_shutdown_cb_f cb_func;

    /* Callback function context for this entry */
    void *cb_data;

} shutdown_cb_t;

/*******************************************************************************
 * Local data
 */

/* List of registered callback functions */
static shutdown_cb_t *shutdown_cb;

/*******************************************************************************
 * Public functions
 */

void
bcmmgmt_shutdown_cb_execute(int unit)
{
    shutdown_cb_t *scb = shutdown_cb;

    while (scb) {
        if (scb->cb_func) {
            scb->cb_func(unit, scb->cb_data);
        }
        scb = scb->next;
    }
}

int
bcmmgmt_shutdown_cb_register(bcmmgmt_shutdown_cb_f cb_func, void *cb_data)
{
    shutdown_cb_t *scb = shutdown_cb;

    while (scb) {
        if (cb_func == scb->cb_func) {
            /* Callback entry already in list */
            return SHR_E_NONE;
        }
        scb = scb->next;
    }

    /* Add new entry in list */
    scb = sal_alloc(sizeof(*scb), "bcmmgmtShutdownCb");
    if (scb == NULL) {
        return SHR_E_MEMORY;
    }
    scb->cb_func = cb_func;
    scb->cb_data = cb_data;
    scb->next = shutdown_cb;
    shutdown_cb = scb;

    return SHR_E_NONE;
}

int
bcmmgmt_shutdown_cb_unregister(bcmmgmt_shutdown_cb_f cb_func)
{
    shutdown_cb_t *scb, **parent = &shutdown_cb;

    while (*parent) {
        scb = *parent;
        if (cb_func == NULL) {
            /* Remove all entries from list */
            shutdown_cb = scb->next;
            sal_free(scb);
            continue;
        }
        if (cb_func == scb->cb_func) {
            /* Remove single entry from list */
            *parent = scb->next;
            sal_free(scb);
            return SHR_E_NONE;
        }
        parent = &(scb->next);
    }

    return SHR_E_NONE;
}
