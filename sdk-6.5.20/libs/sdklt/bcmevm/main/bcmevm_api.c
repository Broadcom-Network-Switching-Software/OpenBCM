/*! \file bcmevm_api.c
 *
 * BCM component generated notification API. This file contains the
 * implementation of the component generated notifications API as well
 * as the transaction manager registration for callback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_hash_str.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmdrd_config.h>
#include <bcmevm/bcmevm_api.h>
#include "bcmevm_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMEVM_EVENT

#define EVENT_TABLE_SIZE    32

typedef struct notify_comp_cb_s {
    bcmevm_event_cb *cb;
    struct notify_comp_cb_s *next;
} notify_comp_cb_t;

typedef struct notify_table_cb_s {
    bcmevm_cb *cb;
    uint32_t table_id;
    struct notify_table_cb_s *next;
} notify_table_cb_t;

static bcmevm_cb *cb_func;

static sal_mutex_t nfy_mutex;

static int nfy_active_units;

static shr_hash_str_hdl event_handler_tbl[BCMDRD_CONFIG_MAX_UNITS];

static notify_table_cb_t *tbl_event_list[BCMDRD_CONFIG_MAX_UNITS];

static shr_lmm_hdl_t notify_mem_hdl;

int bcmevm_unit_init(int unit)
{
#define COMP_EVENT_CHUNK_SIZE    32
    int rv;

    SHR_FUNC_ENTER(unit);
    if (event_handler_tbl[unit]) {
        return SHR_E_RESOURCE;
    }

    if (!nfy_mutex) {
        nfy_mutex = sal_mutex_create("nfy");
    }
    SHR_NULL_CHECK(nfy_mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT(
           shr_hash_str_dict_alloc(EVENT_TABLE_SIZE, &event_handler_tbl[unit]));

    if (!notify_mem_hdl) {
        LMEM_MGR_INIT(notify_comp_cb_t,
                      notify_mem_hdl,
                      COMP_EVENT_CHUNK_SIZE,
                      true,
                      rv);
        if (rv != 0) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }
    nfy_active_units++;
exit:
    SHR_FUNC_EXIT();
}

int bcmevm_unit_shutdown(int unit)
{
    notify_comp_cb_t *it;
    void *val;
    int rv;
    char *key;

    SHR_FUNC_ENTER(unit);
    if (!event_handler_tbl[unit]) {
        return SHR_E_INIT;
    }

    /*
     * Free all the registered callback structures by traversing the dictionary
     */
    rv = shr_hash_str_get_first(event_handler_tbl[unit], &key, &val);
    while (rv == SHR_E_NONE) {
        /* Coverity
         * The value of 'val' is being refreshed by the function
         * shr_hash_str_get_next() below.
         */
        /* coverity[use_after_free : FALSE] */
        it = (notify_comp_cb_t *)val;
        while (it) {
            val = it;
            it = it->next;
            shr_lmm_free(notify_mem_hdl, val);
        }
        rv = shr_hash_str_get_next(event_handler_tbl[unit], &key, &val);
    }
    shr_hash_str_dict_free(event_handler_tbl[unit]);
    event_handler_tbl[unit] = NULL;

    if (--nfy_active_units == 0) {
        sal_mutex_destroy(nfy_mutex);
        nfy_mutex = NULL;
        shr_lmm_delete(notify_mem_hdl);
        notify_mem_hdl = NULL;
    }

    if (tbl_event_list[unit]) {
        notify_table_cb_t *table_it = tbl_event_list[unit];
        notify_table_cb_t *helper;
        while (table_it) {
            helper = table_it->next;
            sal_free(table_it);
            table_it = helper;
        }
        tbl_event_list[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

void bcmevm_register_cb(bcmevm_cb *cb)
{
    cb_func = cb;
}

int bcmevm_table_notify(int unit,
                        bool high_pri,
                        uint32_t table_id,
                        bcmlt_opcode_t opcode,
                        bcmltm_field_list_t *fields)
{
    notify_table_cb_t *it = tbl_event_list[unit];

    /* Start by calling the components callback */
    while (it) {
        if (it->table_id == table_id) {
            it->cb(unit, high_pri, table_id, opcode, fields);
        }
        it = it->next;
    }

    /* Now call upper layers so the APP can be informed. */
    if (!cb_func) {
        return SHR_E_NO_HANDLER;
    }
    return cb_func(unit, high_pri, table_id, opcode, fields);
}

int bcmevm_register_tbl_event(int unit,
                              uint32_t table_id,
                              bcmevm_cb *cb)
{
    notify_table_cb_t *it = tbl_event_list[unit];

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER));

    /* Check if function is already registered */
    while (it) {
        if (it->table_id == table_id && it->cb == cb) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        it = it->next;
    }

    /* Allocate new element */
    it = sal_alloc(sizeof(notify_table_cb_t), "bfyTbl");
    SHR_NULL_CHECK(it, SHR_E_MEMORY);
    it->cb = cb;
    it->table_id = table_id;
    it->next = tbl_event_list[unit];
    tbl_event_list[unit] = it;

exit:
    sal_mutex_give(nfy_mutex);
    SHR_FUNC_EXIT();
}

int bcmevm_register_published_event(int unit,
                                    const char *event,
                                    bcmevm_event_cb *cb)
{
    int rv;
    notify_comp_cb_t *comp_cb;
    notify_comp_cb_t *it = NULL;
    void *val;
    bool new_event;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER));
    if (!event_handler_tbl[unit]) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    rv = shr_hash_str_dict_lookup(event_handler_tbl[unit], event, &val);
    if (rv != SHR_E_NONE) {
        new_event = true;
    } else {
        comp_cb = (notify_comp_cb_t *)val;
        /*
         * It is possible that two events will have the same hash value.
         * Therefore need to check matching event string as well.
         */
        if (comp_cb->cb == cb) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        new_event = false;
        it = comp_cb;
        while (it->next && it->next->cb != cb) {
            it = it->next;
        }
        if (it->next) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }
    comp_cb = shr_lmm_alloc(notify_mem_hdl);
    SHR_NULL_CHECK(comp_cb, SHR_E_MEMORY);

    /* Need to create the entry */
    comp_cb->cb = cb;
    comp_cb->next = NULL;
    if (new_event) {
        SHR_IF_ERR_EXIT(
               shr_hash_str_dict_insert(event_handler_tbl[unit], event, comp_cb));
    } else {
        it->next = comp_cb;
    }

exit:
    sal_mutex_give(nfy_mutex);
    SHR_FUNC_EXIT();
}

int bcmevm_unregister_published_event(int unit,
                                      const char *event,
                                      bcmevm_event_cb *cb)
{
    notify_comp_cb_t *comp_cb;
    notify_comp_cb_t *it;
    void *val;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER));

    if (!event_handler_tbl[unit]) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    SHR_IF_ERR_EXIT(
        shr_hash_str_dict_lookup(event_handler_tbl[unit], event, &val));

    comp_cb = (notify_comp_cb_t *)val;

    if (comp_cb->cb == cb) {
        /* If the first element matches then need to delete the hash entry */
        shr_hash_str_dict_delete(event_handler_tbl[unit], event);
        /*
         * If there are more callbacks for this hash slot insert the next
         * element into the hash dtring
         */
        if (comp_cb->next) {
            SHR_IF_ERR_EXIT(shr_hash_str_dict_insert(event_handler_tbl[unit],
                                                     event,
                                                     comp_cb->next));
        }
    } else {
        it = comp_cb;
        while (it->next && it->next->cb != cb) {
            it = it->next;
        }
        if (it->next) {
            comp_cb = it->next;
            it->next = comp_cb->next;
        } else {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }
    shr_lmm_free(notify_mem_hdl, comp_cb);
exit:
    sal_mutex_give(nfy_mutex);
    SHR_FUNC_EXIT();
}

void bcmevm_publish_event_notify(int unit,
                                 const char *event,
                                 uint64_t ev_data)
{
    notify_comp_cb_t *it;
    notify_comp_cb_t *tmp_list = NULL;
    notify_comp_cb_t *tmp_it = NULL;
    void *val;
    int rv;

    if (!event_handler_tbl[unit]) {
        return;
    }
    do {
        sal_mutex_take(nfy_mutex, SAL_MUTEX_FOREVER);
        rv = shr_hash_str_dict_lookup(event_handler_tbl[unit], event, &val);
        if (rv == SHR_E_NONE) { /* Create temporary list that is duplicate. */
            it = (notify_comp_cb_t *)val;
            while (it) {
                if (tmp_it) {
                    tmp_it->next = shr_lmm_alloc(notify_mem_hdl);
                    tmp_it = tmp_it->next;
                } else {
                    tmp_it = shr_lmm_alloc(notify_mem_hdl);
                    tmp_list = tmp_it;
                }
                if (!tmp_it) {
                    break;
                }
                tmp_it->cb = it->cb;
                it = it->next;
            }
            if (tmp_it) {
                tmp_it->next = NULL;     /* NULL terminate the list */
            }
        }
        sal_mutex_give(nfy_mutex);
        if (rv != SHR_E_NONE) { /* Nobody registered for this event. */
            break;
        }
        it = tmp_list;
        while (it) {
            it->cb(unit, event, ev_data);
            tmp_it = it;
            it = it->next;
            shr_lmm_free(notify_mem_hdl, tmp_it);
        }
    } while (0);
}

