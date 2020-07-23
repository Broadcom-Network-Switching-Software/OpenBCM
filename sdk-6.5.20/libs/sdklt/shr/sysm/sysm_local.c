/*! \file sysm_local.c
 *
 * Broadcom System manager back-end implementation
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <bsl/bsl.h>
#include <sal/sal_msgq.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_assert.h>
#include <sal/sal_sleep.h>
#include <shr/shr_thread.h>
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

#define SYSM_LOG_UNIT(_inst) \
    (((_inst)->type == SHR_SYSM_CAT_UNIT) ? (_inst)->unit : BSL_UNIT_UNKNOWN)


/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get name of system manager state.
 *
 * For invalid states the special string "?" will be returned.
 *
 * \param [in] state System manager state.
 *
 * \return ASCII string.
 */
static const char *state_name(shr_sysm_states_t state)
{
    switch (state) {
    case SHR_SYSM_INIT:
        return "init";
    case SHR_SYSM_COMP_CONFIG:
        return "comp-config";
    case SHR_SYSM_PRE_CONFIG:
        return "pre-config";
    case SHR_SYSM_RUN:
        return "run";
    case SHR_SYSM_STOP:
        return "stop";
    case SHR_SYSM_SHUTDOWN:
        return "shutdown";
    default:
        break;
    }
    return "?";
}

/*!
 * \brief Move blocked component to active list
 *
 * This function move components from the block array entry (based on
 * component_id) to the active list. The dependent array contains pointers to
 * blocked components. The array arranged according to the blocking component
 * ID, so component that is blocked by component K will be pointed by the
 * K'th entry of the array
 *
 * \param [in] component_id Blocking component ID.
 * \param [in] p_instance System manager instance.
 *
 * \return Nothing.
 */
static void move_blocked_to_active(uint32_t component_id,
                                   sysm_instance_t *p_instance)
{
    sysm_component_instance_t *p_active_component =
                                    p_instance->l_component_active;

    if (!p_active_component) {
        p_instance->l_component_active =
                p_instance->dependencies[component_id];
    } else {
        /* Find the last component in the active list */
        while (p_active_component->next) {
            p_active_component = p_active_component->next;
        }
        /* Add the blocked components to the end of the list */
        p_active_component->next = p_instance->dependencies[component_id];
    }
    /* Check how many in the blocked list */
    p_active_component = p_instance->dependencies[component_id];
    while (p_active_component) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance),
                                 p_active_component->component_db->component_id,
                                 "Component "CFMT" unblocked "
                                 "from component "CFMT"\n"),
                     CPRM(p_active_component->component_db->component_id),
                     CPRM(component_id)));
        p_instance->component_active_count++;
        p_active_component = p_active_component->next;
    }
    /* Clear the dependecy list */
    p_instance->dependencies[component_id] = NULL;
}

/*!
 * \brief Prepare the active list
 *
 * This function scans the complete component list and extract all the
 * components that have action function for the current state of the instance
 * The extracted component moved into the active list. The active list counter
 * and the component proc counts are counting the number of extracted
 * components.
 * Also the complete array being reset
 *
 * \param [in] instance point to the system manager instance
 *
 * \return none
 */
static void sysm_prep_active_list(sysm_instance_t *instance)
{
    sysm_component_instance_t *component_instance =
                instance->l_component_complete;
    uint32_t comp_id;

    /*
     * p_prev_component_instance pointing to the previous component in the
     * component complete list
     */
    sysm_component_instance_t *p_prev_component_instance = component_instance;
    bool first_element = true;

    /* Mark everything as complete */
    sal_memset(instance->completed_array, 1, shr_sysm_get_max_component_id());
    while (component_instance) {
        comp_id = component_instance->component_db->component_id;
        if (instance->state == SHR_SYSM_RUN) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UX(SYSM_LOG_UNIT(instance), comp_id,
                                  "Component "CFMT" started\n"),
                      CPRM(comp_id)));
        } else if (instance->state == SHR_SYSM_SHUTDOWN) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_UX(SYSM_LOG_UNIT(instance), comp_id,
                                  "Component "CFMT" shut down\n"),
                      CPRM(comp_id)));
        }
        /* if the component has a callback in this state */
        if (component_instance->component_db->cb_vector[instance->state]) {
            /* Disconnect from the component complete list */
            if (first_element) {
                instance->l_component_complete = component_instance->next;
                p_prev_component_instance = component_instance->next;
            } else {
                p_prev_component_instance->next = component_instance->next;
            }
            /* Add to the active list */
            component_instance->next = instance->l_component_active;
            instance->l_component_active = component_instance;
            if (first_element) {
                component_instance = p_prev_component_instance;
            } else {
                component_instance = p_prev_component_instance->next;
            }
            instance->component_proc_count++;
            /* Active component, mark it incomplete */
            instance->completed_array[comp_id] = 0;
        } else {
            first_element = false;
            p_prev_component_instance = component_instance;
            component_instance = p_prev_component_instance->next;
        }
    }
    instance->component_active_count = instance->component_proc_count;
}

/*!
 * \brief Check if any blocked component can be unblocked
 *
 * This function scans the complete component list and check if any blocked
 * components are blocking on the completed component. Typically, this function
 * being called when the active list is empty but more components needs to be
 * processed
 *
 * \param [in] p_instance point to the system manager instance
 *
 * \return true if a component moved into the active list
 */
static bool check_blocked(sysm_instance_t *p_instance)
{
    int component_id;
    sysm_component_instance_t *p_component_instance =
            p_instance->l_component_complete;

    /*
     * Find the first scenario (there shoudl be one) where a completed
     * component is blocking another component. This is possible if one
     * component is blocked by more than one components.
     */
    while (p_component_instance) {
        component_id = p_component_instance->component_db->component_id;
        if (p_instance->dependencies[component_id]) {
            move_blocked_to_active(component_id, p_instance);
            return true;
        }
        p_component_instance = p_component_instance->next;
    }
    return false;
}

/*!
 * \brief Makes the component callback
 *
 * This function makes the call to the component registered function. It
 * validates the return code and blocking component ID (when relevant)
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] component_db is a pointer to the component registered callback
 * functions
 * \param [out] blocking_component is a pointer where the blocking component
 * ID should be placed (when the component is blocked)
 *
 * \return SHR_SYSM_RV_ERROR when error, otherwise success
 */
static shr_sysm_rv_t sysm_invoke_comp_cb(sysm_instance_t *p_instance,
                             sysm_component_t *component_db,
                             uint32_t *blocking_component)
{
    shr_sysm_rv_t rv;
    uint32_t component_id = component_db->component_id;

    if (p_instance->state == SHR_SYSM_RUN) {
        shr_sysm_run_cb cb = (shr_sysm_run_cb)component_db->cb_vector[SHR_SYSM_RUN];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                 "Component "CFMT" callback (%s)\n"),
                     CPRM(component_id), state_name(p_instance->state)));
        cb(p_instance->type, p_instance->unit, component_db->comp_data);
        rv = SHR_SYSM_RV_DONE;
    } else {
        bool bool_param;
        shr_sysm_cb cb = component_db->cb_vector[p_instance->state];
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                 "Component "CFMT" callback (%s)\n"),
                     CPRM(component_id), state_name(p_instance->state)));
        if (p_instance->state == SHR_SYSM_STOP ||
            p_instance->state == SHR_SYSM_SHUTDOWN) {
            bool_param = p_instance->graceful;
        } else {
            bool_param = p_instance->warm_start;
        }
        rv = cb(p_instance->type,
                p_instance->unit,
                component_db->comp_data,
                bool_param,
                blocking_component);
        assert(rv >= SHR_SYSM_RV_BLOCKED);
        assert(rv <= SHR_SYSM_RV_ERROR);
        /* Check for error condition */
        if (rv == SHR_SYSM_RV_BLOCKED) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                     "Component "CFMT" blocked "
                                     "by component "CFMT"\n"),
                         CPRM(component_id), CPRM(*blocking_component)));
            if (*blocking_component > shr_sysm_get_max_component_id()) {
                rv = SHR_SYSM_RV_ERROR;
            }
        }
        if (rv == SHR_SYSM_RV_ERROR) {
            /* Handle error scenario */
            LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(SYSM_LOG_UNIT(p_instance),
                                    "Component "CFMT" returned error "
                                    "at phase %s\n"),
                         CPRM(component_id), state_name(p_instance->state)));
        }
    }
    return rv;
}

/*!
 * \brief Process result from component callback
 *
 * This function takes the proper action as a result of component callback
 * function processing. If a component is blocked the function places the
 * component in the blocking array at that index that matches the blocking
 * component ID. If the component has done it will be placed in the completed
 * list. For done components the function is examining the blocking array
 * for blocked components. If blocked components were found the function
 * moves them into the active list
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] p_component_instance is a pointer to the component instance
 * \param [in] res is the returned value from the component callback
 * \param [in] blocking_id is the blocking component ID in case the res
 * is SHR_SYSM_RV_BLOCKED
 *
 * \return 0 on success and -1 otherwise.
 */
static int sysm_process_result(
                        sysm_instance_t *p_instance,
                        sysm_component_instance_t *p_component_instance,
                        shr_sysm_rv_t res,
                        uint32_t blocking_id)
{
    int component_id;

    if (res == SHR_SYSM_RV_ERROR) {
        return SHR_E_INTERNAL;
    }
    if (res == SHR_SYSM_RV_BLOCKED) {
        /*
         * Add the component to the list of dependencies for this blocking
         * component ID. This is unsorted linked list.
         */
        p_component_instance->next =
            p_instance->dependencies[blocking_id];
        p_instance->dependencies[blocking_id] = p_component_instance;
        if (p_instance->l_component_active == NULL) {
            /* Check if any of the completed components can free a blocked component */
            check_blocked(p_instance);
        }
    } else {  /* Component done */
        assert(res == SHR_SYSM_RV_DONE); /* Sanity */
        component_id = p_component_instance->component_db->component_id;
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_UX(SYSM_LOG_UNIT(p_instance), component_id,
                                 "Component "CFMT" done state %s\n"),
                     CPRM(component_id), state_name(p_instance->state)));
        /* Add component to the complete list */
        p_component_instance->next = p_instance->l_component_complete;
        p_instance->l_component_complete = p_component_instance;
        /* Mark the component complete array */
        p_instance->completed_array[component_id] = 1;
        /* Check if any component was blocked by this component */
        if (p_instance->dependencies[component_id]) {
            move_blocked_to_active(component_id, p_instance);
            assert(p_instance->l_component_active != NULL);
        }
        /* Check if need to refresh state */
        if (--p_instance->component_proc_count == 0) {
            p_component_instance = shr_sysm_prep_for_next_state(p_instance, true);
        }
    }
    return SHR_E_NONE;
}

/*!
 * \brief Generate dependency deadlock error report.
 *
 * This function generates reports of all the components that are blocked
 * by other components. This function will be called when the active list
 * is empty but there are still components waiting for others. In this case
 * the function will print out all the blocked components and the components
 * that are blocking them so the user can determine the dependency deadlock.
 *
 * \param [in] p_instance The SYSM instance.
 *
 * \return None.
 */
static void sysm_generate_error_report(sysm_instance_t *p_instance)
{
    uint32_t j;
    sysm_component_instance_t *p_comp;

    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(SYSM_LOG_UNIT(p_instance),
                          "Component cyclic dependency\n")));
    for (j = 0; j < shr_sysm_get_max_component_id(); j++) {
        if (p_instance->dependencies[j]) {
            p_comp = p_instance->dependencies[j];
            while (p_comp) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Component "CFMT" blocked on "
                                    "component "CFMT"\n"),
                           CPRM(p_comp->component_db->component_id), CPRM(j)));
                p_comp = p_comp->next;
            }
        }
    }
}

/*!
 * \brief System manager process component
 *
 * This function process a single component in an instance
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] p_component_instance points to the component that should be
 * processed by the assisting thread
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_INTERNAL Error indicating that one of the component failed.
 */
static int sysm_process_component(sysm_instance_t *p_instance,
                             sysm_component_instance_t *p_component_instance)
{
    sal_usecs_t proc_start, proc_time;
    uint32_t blocking_component;
    shr_sysm_rv_t rv;

    proc_start = sal_time_usecs();
    rv = sysm_invoke_comp_cb(p_instance,
                           p_component_instance->component_db,
                           &blocking_component);
    proc_time = SAL_USECS_SUB(sal_time_usecs(), proc_start);

    if (rv == SHR_SYSM_RV_DONE) {
        LOG_VERBOSE(BSL_LS_SHR_BPROF,
                    (BSL_META_U(p_instance->unit,
                                "Done processing component %16s (%02"PRId32")"
                                "[%12s] in %2d.%06d seconds\n"),
                     CPRM(p_component_instance->component_db->component_id),
                     state_name(p_instance->state),
                     (int) proc_time / SECOND_USEC,
                     (int) proc_time % SECOND_USEC));
    }

    if (rv == SHR_SYSM_RV_ERROR) {
        if (p_instance->state == SHR_SYSM_SHUTDOWN) {
            /* Allow error during shutdown state but mark it as such */
            p_instance->error_status = SHR_E_PARTIAL;
            rv = SHR_SYSM_RV_DONE;
        } else {
            return SHR_E_INTERNAL;
        }
    }
    p_instance->component_active_count--;
    rv = sysm_process_result(p_instance,
                             p_component_instance,
                             rv,
                             blocking_component);
    return rv;
}


/*!
 * \brief Report error to the application
 *
 * This function reports error condition to the application that invoked the
 * activity on the instance.
 *
 * \param [in] instance point to the system manager instance
 *
 * \return None.
 */
static void report_error_chg(sysm_instance_t *instance)
{
    if (instance->state_chng_func) {
        instance->state_chng_func(instance->type,
                                  instance->unit,
                                  SHR_E_FAIL);
    }
}


/*******************************************************************************
 * Public functions
 */
int shr_sysm_process_instance(sysm_instance_t *instance)
{
    sysm_component_instance_t *p_component_instance;
    int rv = SHR_E_NONE;

    sal_mutex_take(instance->instance_mutex, SAL_MUTEX_FOREVER);
    while ((instance->component_proc_count > 0) &&
           (instance->steps_left > 0)) {
        /* Make sure that there is an active component waiting */
        if (!instance->l_component_active) {
            /* If not, check if any blocked component can be unblocked */
            check_blocked(instance);
        }
        if (!instance->l_component_active) {
            /*
             * there is nothing to do while there are still component
             * to process. Therefore this is a component dependency deadlock.
             * report that and stop.
             */
            sysm_generate_error_report(instance);
            /* stop this instance from executing */
            instance->steps_left = 0;
            report_error_chg(instance);
            rv = SHR_E_RESOURCE;
            break;
        }
        /* First extract the component out from the active list */
        p_component_instance = instance->l_component_active;
        instance->l_component_active = p_component_instance->next;
        instance->steps_left--;
        rv = sysm_process_component(instance, p_component_instance);
    }
    sal_mutex_give(instance->instance_mutex);
    return rv;
}

sysm_component_instance_t *shr_sysm_prep_for_next_state(
                                    sysm_instance_t *p_instance,
                                    bool auto_mode)
{
    bool cont_to_next_step;
    bool prep_active_list = false;
    shr_sysm_states_t next_state = SHR_SYSM_INIT;

    p_instance->l_component_active = NULL;
    p_instance->component_proc_count = 0;
    /* Find the next state to process */
    do {
        cont_to_next_step = true;
        switch (p_instance->state) {
        case SHR_SYSM_INIT:
            next_state = SHR_SYSM_COMP_CONFIG;
            prep_active_list = true;
            break;
        case SHR_SYSM_COMP_CONFIG:
            next_state = SHR_SYSM_PRE_CONFIG;
            prep_active_list = true;
            break;
        case SHR_SYSM_PRE_CONFIG:
            next_state = SHR_SYSM_RUN;
            prep_active_list = true;
            break;
        case SHR_SYSM_RUN:
            if (auto_mode) {
                if (p_instance->state_chng_func) {
                    p_instance->state_chng_func(p_instance->type,
                                                p_instance->unit,
                                                SHR_E_NONE);
                    p_instance->state_chng_func = NULL;
                }
                cont_to_next_step = false;
                next_state = SHR_SYSM_RUN;  /* Not incrementing state */
            } else {
                prep_active_list = true;
                next_state = SHR_SYSM_STOP;
            }
            break;
        case SHR_SYSM_STOP:
            next_state = SHR_SYSM_SHUTDOWN;
            prep_active_list = true;
            break;
        case SHR_SYSM_SHUTDOWN:
            if (p_instance->state_chng_func) {
                p_instance->state_chng_func(p_instance->type,
                                            p_instance->unit,
                                            p_instance->error_status);
                p_instance->state_chng_func = NULL;
            }
            cont_to_next_step = false;
            next_state = SHR_SYSM_SHUTDOWN; /* Not incrementing state */
            break;
        default:
            assert(0);
        }

        /* Call the monitor functions if state is changing */
        if (p_instance->state != next_state) {
            shr_sysm_state_mon_report(p_instance->type,
                                      p_instance->unit,
                                      next_state);
            p_instance->state = next_state;
        }

        if (prep_active_list) {
            sysm_prep_active_list(p_instance);
        }
        /*
         * Automatically continue to the next state if the active component list
         * is empty
         */
    } while (cont_to_next_step && !p_instance->l_component_active);

    return p_instance->l_component_active;
}

void shr_sysm_instance_thread(shr_thread_t th, void *arg)
{
    sysm_instance_t *instance = (sysm_instance_t *)arg;
    sysm_component_instance_t *component_inst;
    int rv;

    while (!shr_thread_stopping(th)) {
        sal_mutex_take(instance->instance_mutex, SAL_MUTEX_FOREVER);
        while ((instance->component_proc_count > 0) &&
               (instance->steps_left > 0)) {
            /*
             * Make sure that the active list is not empty. First check that
             * nothing became unblocked. If the list is still empty it might
             * be due to pending assistent thread request. In this case wait
             * for it to complete
             */
            if (!instance->l_component_active) {
                check_blocked(instance);
            }
            if (!instance->l_component_active) {
                /*
                 * There is nothing to do while there are still
                 * components to process, and therefore this is a
                 * component dependency deadlock. Report that and
                 * stop.
                 */
                sysm_generate_error_report(instance);
                /* Stop this instance from executing */
                instance->steps_left = 0;
                report_error_chg(instance);
                instance->in_error = true;
                break;
            }
            assert(instance->l_component_active != NULL);
            /* Extract the component out from the active list */
            component_inst = instance->l_component_active;
            instance->l_component_active = component_inst->next;
            instance->steps_left--;

            rv = 0;
            rv = sysm_process_component(instance, component_inst);

            /*
             * Check if the block above had critical failure. If it did only
             * report the error and break from the loop. The instance can retry
             * later where the last failed component will be tried again.
             */
            if (rv !=  SHR_E_NONE) {
                instance->in_error = true;
                /* Return the component to the head of the active list */
                component_inst->next = instance->l_component_active;
                instance->l_component_active = component_inst;
                instance->steps_left++;
                report_error_chg(instance);
                break;
            }
        }
        sal_mutex_give(instance->instance_mutex);
        shr_thread_sleep(th, SAL_SEM_FOREVER);
    }
}
