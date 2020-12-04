/*! \file sysm.c
 *
 * Broadcom System manager API
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* Header files required to build this file */
#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_sleep.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_sysm.h>
/* Header file with prototypes for public functions in this file */
#include "sysm_internal.h"

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_SHR_SYSM

#define VALIDATE_INSTANCE_INPUT_PARAMS(_category,_unit) \
    if ((_category >= SHR_SYSM_CAT_COUNT) || (_category < 0)){\
        LOG_WARN(BSL_LOG_MODULE,                            \
                (BSL_META_U(_unit,                          \
                         "Invalid category\n")));           \
        SHR_ERR_EXIT(SHR_E_PARAM);                   \
    }                                                       \
    if ((_unit >= (int)sysm_instance_db[_category].array_size) ||\
        (_unit < 0)) {                                      \
        LOG_WARN(BSL_LOG_MODULE,                            \
                (BSL_META_U(_unit,                          \
                         "Invalid unit\n")));               \
        SHR_ERR_EXIT(SHR_E_UNIT);                    \
    }

#define VALIDATE_INSTANCE(_instance_cat, _unit, _p_instance)  \
    if (!sysm_instance_db[_instance_cat].instance_array) {    \
        LOG_WARN(BSL_LOG_MODULE,                              \
                (BSL_META_U(_unit,                            \
                         "The system was not initialized properly\n"))); \
        SHR_ERR_EXIT(SHR_E_INIT);                  \
    }                                                         \
    _p_instance = sysm_instance_db[_instance_cat].instance_array[_unit]; \
    if (!_p_instance) {                                       \
        LOG_WARN(BSL_LOG_MODULE,                              \
                (BSL_META_U(_unit,                            \
                         "instance does not exist\n")));      \
        SHR_ERR_EXIT(SHR_E_PARAM);                     \
    }

typedef struct {
    sysm_component_t *l_comp;
    uint32_t comp_count;
} sysm_category_db_t;
static sysm_category_db_t sysm_db[SHR_SYSM_CAT_COUNT];

typedef struct {
    sysm_instance_t **instance_array;
    size_t array_size;
} sysm_instances_db_t;

/*!
 * \brief System monitor callback list
 */
typedef struct shr_sysm_monitor_cb_s {
    /*! The monitor callback function */
    shr_sysm_state_monitor_f cb_func;
    /*! The context associated with the callback */
    void *cb_data;
    /*! Next element in the list */
    struct shr_sysm_monitor_cb_s *next;
} shr_sysm_monitor_cb_t;

static sysm_instances_db_t sysm_instance_db[SHR_SYSM_CAT_COUNT];

static enum {INTERNAL, EXTERNAL} sysm_thread_mode;
static uint32_t sysm_max_num_of_comp;
static sal_mutex_t sysm_mutex;
static uint32_t sysm_instance_count;

static shr_sysm_monitor_cb_t *state_monitor_cb;

/*******************************************************************************
 * Private functions
 */

static int allocate_component_list(sysm_instance_t *instance,
                                   shr_sysm_categories_t category)
{
    uint32_t j;
    sysm_component_instance_t *component_inst = NULL;
    sysm_component_t *component;
    size_t alloc_size;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    /* create the list of components */
    alloc_size = sizeof(sysm_component_instance_t) *
                sysm_db[category].comp_count;
    SHR_ALLOC(component_inst,
              alloc_size, "shrSysmCompInst");
    SHR_NULL_CHECK(component_inst, SHR_E_MEMORY);
    sal_memset(component_inst, 0, alloc_size);
    /* save the pointer so we can free later */
    instance->allocated_component_mem = component_inst;

    instance->completed_array = NULL;
    SHR_ALLOC(instance->completed_array,
              sysm_max_num_of_comp, "shrSysmCompletedArray");
    SHR_NULL_CHECK(instance->completed_array, SHR_E_MEMORY);
    /*
     * Everything marked as complete so we will not get stuck on components
     * that are not there.
     */
    sal_memset(instance->completed_array, 1, sysm_max_num_of_comp);

    for (j = 0, component = sysm_db[category].l_comp;
          j < sysm_db[category].comp_count;
          component = component->next, j++, component_inst++) {
        component_inst->component_db = component;
        /* Count the number of components to process in init state */
        if (component->cb_vector[SHR_SYSM_INIT]) {
            instance->component_proc_count++;
            /* Add this to the active list */
            component_inst->next = instance->l_component_active;
            instance->l_component_active = component_inst;
            instance->completed_array[component->component_id] = 0;
        } else {
            component_inst->next = instance->l_component_complete;
            instance->l_component_complete = component_inst;
        }
    }
    instance->component_active_count = instance->component_proc_count;
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public functions
 */

int shr_sysm_category_define(shr_sysm_categories_t category,
                            uint32_t max_num_of_instances)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if ((category >= SHR_SYSM_CAT_COUNT) || (category < 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sysm_instance_db[category].array_size = max_num_of_instances;
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_init(uint32_t max_component_id, bool internal_thread_mode)
{
    int j;
    bool allocated = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (internal_thread_mode) {
        sysm_thread_mode = INTERNAL;
    } else {
        sysm_thread_mode = EXTERNAL;
    }
    sysm_max_num_of_comp = max_component_id;
    sysm_mutex = sal_mutex_create("shrSysm");
    SHR_NULL_CHECK(sysm_mutex, SHR_E_MEMORY);

    for (j = 0; j < SHR_SYSM_CAT_COUNT; j++) {
        size_t alloc_size;
        sysm_instances_db_t *idb = &sysm_instance_db[j];
        if (idb->array_size == 0) {
            continue;
        }
        allocated = true;
        alloc_size = sizeof(sysm_instance_t *) * idb->array_size;
        SHR_ALLOC(idb->instance_array, alloc_size, "shrSysmInstArray");
        SHR_NULL_CHECK(idb->instance_array, SHR_E_MEMORY);
        sal_memset(idb->instance_array, 0, alloc_size);
        sysm_instance_count += idb->array_size;
    }
    if (!allocated) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (j = 0; j < SHR_SYSM_CAT_COUNT; j++) {
            SHR_FREE(sysm_instance_db[j].instance_array);
        }
        if (sysm_mutex) {
            sal_mutex_destroy(sysm_mutex);
            sysm_mutex = NULL;
        }
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META("System manager initialized\n")));
    }

    SHR_FUNC_EXIT();
}

int shr_sysm_delete(void)
{
    int j;
    sysm_component_t *p_component;
    sysm_component_t *tmp;
    shr_sysm_monitor_cb_t *it = state_monitor_cb;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Clean the component DB */
    for (j = 0; j < SHR_SYSM_CAT_COUNT; j++) {
        p_component = sysm_db[j].l_comp;
        while (p_component) {
            tmp = p_component;
            p_component = p_component->next;
            SHR_FREE(tmp);
        }
        sal_memset(&sysm_db[j], 0, sizeof(sysm_db[0]));
        SHR_FREE(sysm_instance_db[j].instance_array);
    }

    /* No need to protect the component DB anymore. Delete the guard */
    sal_mutex_destroy(sysm_mutex);
    sysm_mutex = NULL;

    /* Clean the state monitor records */
    while (it) {
        shr_sysm_monitor_cb_t *tmp_mon;
        tmp_mon = it;
        it = it->next;
        SHR_FREE(tmp_mon);
    }
    state_monitor_cb = NULL;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("System manager cleaned up\n")));

    SHR_FUNC_EXIT();
}

int shr_sysm_register(uint32_t comp_id,
                      shr_sysm_categories_t instance_cat,
                      shr_sysm_cb_func_set_t *cb_set,
                      void *comp_data)
{
    sysm_component_t *p_component;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_UX(BSL_UNIT_UNKNOWN, comp_id,
                             "Register component "CFMT"\n"),
                 CPRM(comp_id)));

    if ((comp_id > sysm_max_num_of_comp) ||
        (instance_cat >= SHR_SYSM_CAT_COUNT) ||
        (instance_cat < 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(sysm_mutex, SAL_MUTEX_FOREVER));

    do {
        /* Make sure component is not registered already */
        p_component = sysm_db[instance_cat].l_comp;
        while (p_component) {
            if (p_component->component_id == comp_id) {
                rv = SHR_E_EXISTS;
                break;
            }
            p_component = p_component->next;
        }
        if (rv != SHR_E_NONE) {
            SHR_ERR_EXIT(rv);
        }

        /* Allocate new component structure */
        SHR_ALLOC(p_component, sizeof(sysm_component_t), "shrSysmComp");
        SHR_NULL_CHECK(p_component, SHR_E_MEMORY);
        p_component->component_id = comp_id;
        p_component->cb_vector[SHR_SYSM_INIT] = cb_set->init;
        p_component->cb_vector[SHR_SYSM_COMP_CONFIG] = cb_set->comp_config;
        p_component->cb_vector[SHR_SYSM_PRE_CONFIG] = cb_set->pre_config;
        p_component->cb_vector[SHR_SYSM_RUN] = (shr_sysm_cb)cb_set->run;
        p_component->cb_vector[SHR_SYSM_STOP] = cb_set->stop;
        p_component->cb_vector[SHR_SYSM_SHUTDOWN] = cb_set->shutdown;
        p_component->comp_data = comp_data;
        /* Add the component into the component list */
        p_component->next = sysm_db[instance_cat].l_comp;
        sysm_db[instance_cat].l_comp = p_component;
        sysm_db[instance_cat].comp_count++;
    } while (0);

    sal_mutex_give(sysm_mutex);

exit:
    SHR_FUNC_EXIT();
}

bool shr_sysm_is_comp_complete(uint32_t comp_id,
                               shr_sysm_categories_t instance_cat,
                               int unit)
{
    sysm_instance_t *p_instance;

    do {
        /* Validation section, return false for invalid parameters */
        if ((instance_cat >= SHR_SYSM_CAT_COUNT) ||
            (instance_cat < 0)) {
             LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Component looking for invalid category %d\n"),
                      instance_cat));
            break;
        }
        if ((unit >= (int)sysm_instance_db[instance_cat].array_size) ||
            (unit < 0)) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("Component looking for invalid unit %d\n"),
                     unit));
            break;
        }
        p_instance = sysm_instance_db[instance_cat].instance_array[unit];
        if (!p_instance) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META("Component looking for wrong category %d unit "
                               "%d combination\n"),
                     instance_cat, unit));
            break;
        }
        if (comp_id >= sysm_max_num_of_comp) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META("Component checking for completion of invalid "
                              "component %d\n"),
                     comp_id));
            break;
        }
        /* body section */
        return p_instance->completed_array[comp_id] == 1;
    } while (0);
    /*
     * If here it is because sanity had failed. Still don't want
     * the component to get stuck, so return true
     */
    return true;
}

int shr_sysm_instance_new(shr_sysm_categories_t instance_cat,
                          int unit,
                          bool warm,
                          bool single_step,
                          sysm_instance_state_chng_cb func)
{
    sysm_instance_t *instance = NULL;
    sysm_component_t *p_component;

    SHR_FUNC_ENTER(unit);

    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    p_component = sysm_db[instance_cat].l_comp;
    if (!p_component) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No components associated with the category\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* allocate the instance */
    instance = NULL;
    SHR_ALLOC(instance, sizeof(sysm_instance_t), "shrSysmInst");
    SHR_NULL_CHECK(instance, SHR_E_MEMORY);
    sal_memset(instance, 0, sizeof(sysm_instance_t));

    SHR_IF_ERR_EXIT(allocate_component_list(instance, instance_cat));

    /* Allocate array for blocked components */
    SHR_ALLOC(instance->dependencies,
              sizeof(sysm_component_instance_t *) * sysm_max_num_of_comp,
              "shrSysmInstDependencies");
    SHR_NULL_CHECK(instance->dependencies, SHR_E_MEMORY);
    sal_memset(instance->dependencies,
               0,
               sizeof(sysm_component_instance_t *) * sysm_max_num_of_comp);

    /* Set other instance variables */
    instance->warm_start = warm;
    if (single_step) {
        instance->steps_left = 0;
    } else {
        instance->steps_left = 0xFFFFFFFF;    /* maximum steps */
    }
    instance->state = SHR_SYSM_INIT;
    instance->type = instance_cat;
    instance->unit = unit;
    instance->state_chng_func = func;
    instance->instance_mutex = sal_mutex_create("shrSysmInst");
    instance->in_error = false;
    instance->error_status = SHR_E_NONE;
    SHR_NULL_CHECK(instance->instance_mutex, SHR_E_MEMORY);

    sysm_instance_db[instance_cat].instance_array[unit] = instance;

    /* Call the monitor function when entering the INIT state */
    shr_sysm_state_mon_report(instance_cat, unit, SHR_SYSM_INIT);

    /* Based on threading mode starts the local thread */
    if (sysm_thread_mode == EXTERNAL) {
        instance->thread_hdl = NULL;
    } else {
        instance->assisted_mutex = sal_mutex_create("shrSysmInstAssist");
        SHR_NULL_CHECK(instance->assisted_mutex, SHR_E_MEMORY);
        /*
         * thread create must be the last operation since once it been created
         * thread switch might occur.
         */
        instance->thread_hdl = shr_thread_start("shrSysmInst", -1,
                                                shr_sysm_instance_thread,
                                                (void *)instance);
        SHR_NULL_CHECK(instance->thread_hdl, SHR_E_MEMORY);
    }
exit:
    if (SHR_FUNC_ERR() && instance) {
        if (instance->assisted_mutex) {
            sal_mutex_destroy(instance->assisted_mutex);
        }
        if (instance->instance_mutex) {
            sal_mutex_destroy(instance->instance_mutex);
        }
        SHR_FREE(instance->dependencies);
        SHR_FREE(instance->allocated_component_mem);
        SHR_FREE(instance);
    }
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_delete(shr_sysm_categories_t instance_cat,
                             int unit)
{
    sysm_instance_t *instance;
    int attempts = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, instance);

    instance->steps_left = 0; /* stop the instance thread */
    /* remove the instance from the instances DB */
    sysm_instance_db[instance_cat].instance_array[unit] = NULL;

    /* Now start tearing everything down */
    /* Start with shutting down the thread instance */
    if (sysm_thread_mode == INTERNAL) {
        rv = shr_thread_stop(instance->thread_hdl, 1000 * MSEC_USEC);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Instance thread did not complete\n")));
        }
        instance->thread_hdl = NULL;
    }

    /* Wait for all active components to complete */
    attempts = 0;
    while (instance->component_active_count && (attempts++ < 1000)) {
        sal_usleep(100);
    }
    if (instance->component_active_count > 0) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    sal_mutex_destroy(instance->instance_mutex);
    sal_mutex_destroy(instance->assisted_mutex);
    SHR_FREE(instance->dependencies);
    SHR_FREE(instance->completed_array);
    SHR_FREE(instance->allocated_component_mem);
    SHR_FREE(instance);
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_stop(shr_sysm_categories_t instance_cat,
                          int unit,
                          bool graceful,
                          sysm_instance_state_chng_cb func)
{
    sysm_instance_t *instance;
    sysm_component_instance_t *active_comp;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, instance);

    if (!instance->in_error &&
        ((instance->state != SHR_SYSM_RUN) ||
        (instance->component_proc_count > 0))) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    /* Sanity passed, start the operation */
    SHR_IF_ERR_EXIT(sal_mutex_take(instance->instance_mutex,
                                   SAL_MUTEX_FOREVER));
    instance->state_chng_func = func;
    instance->graceful = graceful;
    /* Force the system to be in RUN state if in error */
    if (instance->in_error) {
        instance->state = SHR_SYSM_RUN;
        /*
         * Move all the components that were in process into the complete
         * list
         */
        active_comp = instance->l_component_active;
        if (active_comp) {
            while (active_comp->next) {
                active_comp = active_comp->next;
            }
            active_comp->next = instance->l_component_complete;
            instance->l_component_complete = instance->l_component_active;
        }
        instance->l_component_active = NULL;
    }
    active_comp = shr_sysm_prep_for_next_state(instance, false);
    sal_mutex_give(instance->instance_mutex);
    /* If there is nothing to do in stop */
    if (active_comp == NULL) {
        SHR_EXIT();
    }
    if (sysm_thread_mode == INTERNAL) {
        /* let the thread process the instance */
        shr_thread_wake(instance->thread_hdl);
    }
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_restart(shr_sysm_categories_t instance_cat, int unit)
{
    sysm_instance_t *instance;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, instance);

    if (!instance->in_error) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (sysm_thread_mode != INTERNAL) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    instance->in_error = false;
    shr_thread_wake(instance->thread_hdl);
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_step(shr_sysm_categories_t instance_cat,
                          int unit,
                          uint32_t number_of_steps)
{
    sysm_instance_t *instance;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, instance);

    if (instance->steps_left > 0) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }
    instance->steps_left = number_of_steps;
    if (number_of_steps == 0xFFFFFFFF) {    /* back to normal execution? */
        if (sysm_thread_mode == INTERNAL) {   /* kick start the internal thread */
            shr_thread_wake(instance->thread_hdl);
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(shr_sysm_process_instance(instance));
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_instance_process(shr_sysm_categories_t instance_cat,
                             int unit)
{
    sysm_instance_t *p_instance;

    SHR_FUNC_ENTER(unit);
    if (sysm_thread_mode == INTERNAL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    VALIDATE_INSTANCE(instance_cat, unit, p_instance);

    SHR_IF_ERR_EXIT(shr_sysm_process_instance(p_instance));
exit:
    SHR_FUNC_EXIT();
}

int shr_sysm_state_monitor_register(shr_sysm_state_monitor_f cb,
                                    void *context)
{
    shr_sysm_monitor_cb_t *it = state_monitor_cb;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (!cb) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Allow to register even before the system manager was initialized. This
     * enables the application to use high level initialization (i.e. bcmmgnt).
     * The down side is that the code assumes that the mutual exclusion for
     * this function is done outside the scope of this function (again only
     * when called before shr_sysm_init()).
     */
    if (sysm_mutex) {
        SHR_IF_ERR_EXIT(sal_mutex_take(sysm_mutex, SAL_MUTEX_FOREVER));
    }

    /* First search if the callback already exist */
    while (it) {
        if (it->cb_func == cb) {
            SHR_EXIT();
        }
        it = it->next;
    }

    /* Allocate a new link in the chain */
    SHR_ALLOC(it, sizeof(shr_sysm_monitor_cb_t), "shrSysmMonCb");
    SHR_NULL_CHECK(it, SHR_E_MEMORY);
    it->cb_func = cb;
    it->cb_data = context;
    it->next = state_monitor_cb;
    state_monitor_cb = it;

exit:
    if (sysm_mutex) {
        sal_mutex_give(sysm_mutex);
    }
    SHR_FUNC_EXIT();
}

void shr_sysm_state_monitor_unregister(shr_sysm_state_monitor_f cb)
{
    shr_sysm_monitor_cb_t *it = state_monitor_cb;
    shr_sysm_monitor_cb_t *tmp;

    if (!sysm_mutex) {
        return;
    }

    /* Find the callback structure and delete from the list */
    if (!it) {
        return;
    }
    sal_mutex_take(sysm_mutex, SAL_MUTEX_FOREVER);
    if (it->cb_func == cb) {
        state_monitor_cb = it->next;
        sal_free(it);
    } else {
        while (it->next) {
            if (it->next->cb_func == cb) {
                tmp = it->next;
                it->next = tmp->next;
                sal_free(tmp);
            }
            it = it->next;
        }
    }

    sal_mutex_give(sysm_mutex);
}

/* None exported functions */
uint32_t shr_sysm_get_max_component_id (void)
{
    return sysm_max_num_of_comp;
}

uint32_t shr_sysm_get_instance_count(void)
{
    return sysm_instance_count;
}

void shr_sysm_state_mon_report(shr_sysm_categories_t instance_cat,
                               int unit,
                               shr_sysm_states_t state)
{
    shr_sysm_monitor_cb_t *it = state_monitor_cb;

    if (instance_cat == SHR_SYSM_CAT_GENERIC) {
        unit = -1;
    }

    /* Make sure that the list is not changing while we make call-backs */
    sal_mutex_take(sysm_mutex, SAL_MUTEX_FOREVER);
    while (it) {
        it->cb_func(unit, state, it->cb_data);
        it = it->next;
    }
    sal_mutex_give(sysm_mutex);
}

int shr_sysm_instance_state_get(
    shr_sysm_categories_t instance_cat,
    int unit,
    shr_sysm_states_t *state)
{
    sysm_instance_t *p_instance;

    SHR_FUNC_ENTER(unit);
    VALIDATE_INSTANCE_INPUT_PARAMS(instance_cat, unit);
    SHR_NULL_CHECK(state, SHR_E_PARAM);

    p_instance = sysm_instance_db[instance_cat].instance_array[unit];
    *state = p_instance->state;

exit:
    SHR_FUNC_EXIT();
}
