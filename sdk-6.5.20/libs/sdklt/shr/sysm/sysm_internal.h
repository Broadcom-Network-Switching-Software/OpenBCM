/*! \file sysm_internal.h
 *
 *  System manager internal declarations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SYSM_INTERNAL_H
#define SYSM_INTERNAL_H

#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>

#include <shr/shr_thread.h>
#include <shr/shr_sysm.h>

/*! Formatting string for printing component ID. */
#define CFMT "%s (%"PRId32")"

/*! Parameter list for printing component ID (see \ref CFMT above). */
#define CPRM(_id) shr_sysm_comp_name(_id), _id

/*!
 * \brief Component registration information.
 */
typedef struct sysm_component_s {

    /*! Component ID. */
    uint32_t component_id;

    /*! Vector containing component callback functions. */
    shr_sysm_cb cb_vector[SHR_SYSM_STATE_LAST + 1];

    /*! Component opaque data. */
    void *comp_data;

    /*! Pointer to the next element in the list. */
    struct sysm_component_s *next;

} sysm_component_t;

/*!
 * \brief Component instance.
 *
 * This data structure being created for every component and every
 * instance of the system manager.
 */
typedef struct sysm_component_instance_s {

    /*! Pointer to the component information. */
    sysm_component_t *component_db;

    /*! Pointer to the next element. */
    struct sysm_component_instance_s *next;

} sysm_component_instance_t;

/* Forward decleration */
struct sysm_instance_s;

/*!
 * \brief System manager instance data structure
 */
typedef struct sysm_instance_s {

    /*! The instance state  */
    shr_sysm_states_t state;

    /*! List of active components */
    sysm_component_instance_t *l_component_active;

    /*! List of completed components */
    sysm_component_instance_t *l_component_complete;

    /*! Array of lists of blocked components indexed by the blocker ID */
    sysm_component_instance_t **dependencies;

    /*! Count how many components left for processing */
    uint32_t component_proc_count;

    /*!
     * Count how many components are in the active list always
     * component_active_count <= component_proc_count
     */
    uint32_t component_active_count;

    /*! How many steps before stopping. */
    uint32_t steps_left;

    /*! Warm start? */
    bool warm_start;

    /*! Graceful termination. */
    bool graceful;

    /*! The associated thread handle. */
    shr_thread_t thread_hdl;

    /*! The instance category. */
    shr_sysm_categories_t type;

    /*! The unit associated with the instance. */
    int unit;

    /*! Use for freeing the component instances. */
    sysm_component_instance_t *allocated_component_mem;

    /*! Pointer to application state change function. */
    sysm_instance_state_chng_cb state_chng_func;

    /*! Array of completed components. */
    uint8_t *completed_array;

    /*!
     * Synchronization object between the assisting thread and the
     * instance dedicated thread.
     */
    sal_mutex_t assisted_mutex;

    /*!
     * Instance content mutex used to protect access from external
     * functions and the instance dedicated thread.
     */
    sal_mutex_t instance_mutex;

    /*! Is the instance in error state? */
    bool in_error;

    /*! The current error status of this instance */
    shr_error_t error_status;
} sysm_instance_t;

/*!
 * \brief Move the instance into the next state
 *
 * This function moves the state machine of an instance into the next state
 * and update the active list with the components that should be processed
 * for the next state. If the active list for the new state is empty the
 * function automatically jumps into the next state. The RUN state is special
 * state that the function will not progress unless the auto_mode is off.
 * Also the last state is SHUTDOWN so no progress can be made from there.
 *
 * \param [in] p_instance point to the system manager instance
 * \param [in] auto_mode indicates if while in RUN state the function
 * should move to the next state (auto_mode is false) or stay in RUN
 * state (auto_mode is true)
 *
 * \return A pointer to the active list after state change
 */
extern sysm_component_instance_t* shr_sysm_prep_for_next_state(
                                        sysm_instance_t *p_instance,
                                        bool auto_mode);

/*!
 * \brief Process instance through its state machine
 *
 * This function process a single instance of the system manager. It assumed
 * to be called for external threading model so there is no assistant thread.
 * The processing will continue until there is nothing to process. This can
 * be a result of a component dependency loop or when the state machine arrives
 * into a state that the system manager should stay in.
 *
 * \param [in] p_instance point to the system manager instance
 *
 * \return SHR_E_NONE on success and SHR_E_RESOURCE on failure due to component
 * circular dependency.
 */
extern int shr_sysm_process_instance(sysm_instance_t *p_instance);

/*!
 * \brief Obtain the maximal component ID
 *
 * This function returns the system maximal valid component ID
 *
 * \return the maximal component ID
 */
extern uint32_t shr_sysm_get_max_component_id(void);

/*!
 * \brief Obtain the total number of instances
 *
 * This function returns the total number of system manager instances
 * (from all categories) in the system
 *
 * \return the total number of instances
 */
extern uint32_t shr_sysm_get_instance_count(void);

/*!
 * \brief System manager instance thread
 *
 * This is the instance main thread function.
 *
 * \param [in] th Thread handle for this thread.
 * \param [in] arg Pointer to the instance associated with this thread.
 *
 * \return none
 */
extern void shr_sysm_instance_thread(shr_thread_t th, void *arg);

/*!
 * \brief Invoke the state monitor function (if registered).
 *
 * This function invoked an application state monitor callback function. The
 * application should register such a function via the function
 * shr_sysm_state_monitor_register(). The function does nothing if no callback
 * function is registered.
 *
 * \param [in] instance_cat Indicates the category of the system manager.
 * \param [in] unit Indicates the instance of he system manager.
 * \param [in] state Indicates the state of the system manager instance.
 *
 * \return None.
 */
extern void shr_sysm_state_mon_report(shr_sysm_categories_t instance_cat,
                                      int unit,
                                      shr_sysm_states_t state);

#endif /* SYSM_INTERNAL_H */
