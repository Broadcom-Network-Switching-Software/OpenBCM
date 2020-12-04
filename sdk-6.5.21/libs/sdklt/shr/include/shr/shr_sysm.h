/*! \file shr_sysm.h
 *
 * System manager API definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_SYSM_H
#define SHR_SYSM_H

#include <shr/shr_types.h>
#include <shr/shr_sysm_state_mon.h>

/*!
 * \brief System manager init.
 *
 * This function initializes the system manager by providing some operational
 * parameters.
 *
 * \param [in] max_component_id Indicates the maximal value of a component ID.
 * \param [in] internal_thread_mode Indicates the threading mode. If not
 * internal, the system manager will not create any thread and will rely on
 * an application thread to be the main engine that runs the system manager.
 * If internal mode, the system manager will create thread for every system
 * manager instance.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int shr_sysm_init(uint32_t max_component_id, bool internal_thread_mode);

/*!
 * \brief System manager delete.
 *
 * This function deletes the system manager by cleaning up all its resources.
 * Call this function only after all the instances had been deleted
 * (see \ref shr_sysm_instance_delete).
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int shr_sysm_delete(void);

/*!
 * \brief System manager instance categories.
 *
 * These are the system manager instance categories. A component will typically
 * register itself to one type of category, however, some will be registered for
 * multiple categories. If a component registers itself into multiple categories
 * it will (typically) use different callback functions as the activity in each
 * category is different.
 */
typedef enum {
    SHR_SYSM_CAT_UNIT = 0, /*!< type unit instance, instance for every unit*/
    SHR_SYSM_CAT_GENERIC,  /*!< type is generic, not multi instances       */
    SHR_SYSM_CAT_COUNT     /*!< Determine the maximal types number         */
} shr_sysm_categories_t;


/*!
 * \brief System manager define category.
 *
 * This function defines a system manager category parameters. IN particular it
 * specifies what are the maximal instance number that should be part of this
 * category.
 *
 * \param [in] cat Is the category to define.
 * \param [in] max_num_of_instances Indicates the maximal instance number
 * under this category.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int shr_sysm_category_define(shr_sysm_categories_t cat,
                                    uint32_t max_num_of_instances);

/*!
 * \brief System manager returned value.
 *
 * These are the values that a system manager callback function may return.
 */
typedef enum {
    SHR_SYSM_RV_BLOCKED,/*!< Can't finish due to dependency on other component*/
    SHR_SYSM_RV_DONE,   /*!< Operation completed                               */
    SHR_SYSM_RV_ERROR   /*!< Operation failed                                  */
} shr_sysm_rv_t;


/*!
 * \brief System manager callback function.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in] instance_cat Is an indicator of the system manager
 *              instance. For SHR_SYSM_CAT_GENERIC the unit parameter
 *              should be ignored.
 * \param [in] unit The device unit number. This identified the instance
 *              of the system manager.
 * \param [in] comp_data Is a pointer to the component context provided
 *              during the component registration.
 * \param [in] warm Is a boolean value indicate warm or cold start.
 * \param [out] blocking_comp Set this parameter if the component is blocked
 *              by other component. In this case the function returns
 *              SHR_SYSM_RV_BLOCKED and sets the blocking_cmponent to
 *              point to the name of the blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, BCM_SYS_MGR_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
typedef shr_sysm_rv_t (*shr_sysm_cb)(shr_sysm_categories_t instance_cat,
                                     int unit,
                                     void *comp_data,
                                     bool warm,
                                     uint32_t *blocking_comp);

/*!
 * \brief System manager stop callback function.
 *
 * This function signature is used by the system manager to invoke a component
 * stop and shutdown callback events. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in] instance_cat Is an indicator of the system manager
 *              instance. For SHR_SYSM_CAT_GENERIC the unit parameter
 *              should be ignored.
 * \param [in] unit The device unit number. This identified the instance
 *              of the system manager.
 * \param [in] comp_data Is a pointer to the component context provided
 *              during the component registration.
 * \param [in] graceful Is a boolean value indicating if the stop should
 * be performed gracefully or not (i.e. A.S.A.P). Graceful stopping allows
 * the completion of all the activity that is already in the pipeline (for
 * example transactions in staging). The same concept applied for the shutdown
 * callback.
 * If graceful is FALSE the stop function should signal the component to halt
 * its activities immediately. This is very critical for components that
 * interact with the h/w as it is likely that the device will not be accessible
 * (for example line card being yanked out). In such a scenario there is no
 * reason to try maintaining the coherence of the device content.
 * Later, during the shutdown state, the components can verify that indeed they
 * had stopped interacting with the device before they start to free their
 * resources.
 * \param [out] blocking_comp Set this parameter if the component is blocked
 *              by other component. In this case the function returns
 *              SHR_SYSM_RV_BLOCKED and sets the blocking_cmponent to
 *              point to the name of the blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, BCM_SYS_MGR_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
typedef shr_sysm_rv_t (*shr_sysm_stop_cb)(shr_sysm_categories_t instance_cat,
                                          int unit,
                                          void *comp_data,
                                          bool graceful,
                                          uint32_t *blocking_comp);

/*!
 * \brief System manager run callback function
 *
 * This function signature is used for the run function. A
 * component should provide this callback function if it chooses to be
 * called for the run event.
 *
 * \param [in] instance_cat Is an indicator of the system manager
 *              instance. For SHR_SYSM_CAT_GENERIC the unit parameter
 *              should be ignored.
 * \param [in] unit The device unit number. This identified the instance
 *              of the system manager.
 * \param [in] comp_data Is an opaque pointer to the component context provided
 *              during the component registration.
 *
 * \return none.
 */
typedef void (*shr_sysm_run_cb)(shr_sysm_categories_t instance_cat,
                                int unit,
                                void *comp_data);

/*!
 * \brief Component callback function set.
 */
typedef struct shr_sysm_cb_func_set_s {
    /*! The init function callback (or NULL if N/A) */
    shr_sysm_cb init;
    /*! The component configuration function callback (or NULL if N/A) */
    shr_sysm_cb comp_config;
    /*! The component pre system configuration function callback
     * (or NULL if N/A)
     */
    shr_sysm_cb pre_config;
    /*! The run function callback (or NULL if N/A). This
     * function will be called when moving into the run state
     */
    shr_sysm_run_cb run;
    /*! The stop function callback (or NULL if N/A). The
     * stop function stops the processing of new component events that are
     * associated with this instance. By stopping the process the system
     * manager enables the next step of shut down where it knows that there
     * are no more requests in transition. The stop callback function is being
     * called as a result of calling ref shr_sysm_instance_stop() function.
     */
    shr_sysm_stop_cb stop;
    /*! The shutdown function callback (or NULL if N/A). This function
     * should free all the component resources associated with this type of
     * instance.
     */
    shr_sysm_stop_cb shutdown;
}shr_sysm_cb_func_set_t;

/*!
 * \brief Component registeration with system manager.
 *
 * This function enables a component to register callback functions with
 * a particular instance type of the system manager. As a result, the
 * system manager will call the registered components functions whenever
 * a new instance of this type will be created or destroyed.
 *
 * \param [in] comp_id Component ID.
 * \param [in] instance_cat System manager instance category.
 * \param [in] cb_set Pointer to the set of the component callback functions.
 * \param [in] comp_data Pointer to the component context.
 *
 * \return none.
 */
extern int shr_sysm_register(uint32_t comp_id,
                             shr_sysm_categories_t instance_cat,
                             shr_sysm_cb_func_set_t *cb_set,
                             void *comp_data);


/*!
 * \brief System manager component complete query
 *
 * This function returns true if the component of a particular system manager
 * instance has completed.
 *
 * \param [in] comp_id is the component ID.
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return true is the component completed its operation for this phase and
 *         false otherwise.
 */
extern bool shr_sysm_is_comp_complete(uint32_t comp_id,
                                      shr_sysm_categories_t instance_cat,
                                      int unit);

/*!
 * \brief State change callback function.
 *
 * This function prototype is used when calling system manager state
 * change function for particular instance. Since state change
 * operation might take some time, this function used to
 * asynchronously inform the caller about the completion of the state
 * change operation.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] status Is the status of the completion. SHR_E_NONE if successful
 * and error code otherwise.
 *
 * \return none.
 */
typedef void (*sysm_instance_state_chng_cb)(shr_sysm_categories_t instance_cat,
                                            int unit,
                                            int status);
/*!
 * \brief System manager instance new.
 *
 * This function creates a new instance of the system manager based on the
 * instance category and unit supplied. Every component that was previously
 * registered with the system manager using this instance category will
 * automatically be associated with this system manager instance. See
 * \ref shr_sysm_register().
 *
 * Typically, when new unit being attached this function will be called with
 * SHR_SYSM_CAT_UNIT and the new unit number as input parameters.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] warm is a boolean value indicate cold or warm start.
 * \param [in] single_step is a boolean value indicate if the instance
 * should be executed in single step mode.
 * \param [in] func is user provided callback function to inform the
 * caller that the state transition had been completed. This callback function
 * can be called multiple times in case of an error that is being handled by
 * calling \ref shr_sysm_instance_restart. In other words, for the first call to
 * \ref shr_sysm_instance_new the callback function will be called once. Then
 * if the callback function indicated an error with the instance the application
 * may attempt to retry by calling \ref shr_sysm_instance_restart. This call
 * will trigger another callback to this callback function.
 *
 * \return SHR_E_NONE on success and error code otherwise
 */
extern int shr_sysm_instance_new(shr_sysm_categories_t instance_cat,
                                 int unit,
                                 bool warm,
                                 bool single_step,
                                 sysm_instance_state_chng_cb func);

/*!
 * \brief System manager instance delete.
 *
 * This function deletes an instance of the system manager based on the
 * instance type and unit. This function will free all resources associated with
 * the instance. Note that the instance must be stopped prior to delete by
 * calling the function \ref shr_sysm_instance_stop().
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_delete(shr_sysm_categories_t instance_cat,
                                    int unit);

/*!
 * \brief System manager instance stop.
 *
 * This function stops an instance of the system manager based on the
 * instance category and unit provided. All components associated with this
 * instance will have their stop call-back function invoked. This function
 * is non-blocking and the caller can determine when the operation have
 * completed by providing a call-back function. The provided call-back function
 * will be called only when the instance had shut down.
 * The shutdown process can be done gracefully. In this case all the operations
 * that had been requested will complete their execution. None-graceful
 * shutdown will stop any further operation and will shutdown as soon as
 * possible. One scenario using non-graceful shutdown is when extracting line
 * card from a chassis.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] graceful indicates if the stop is graceful or not.
 * \param [in] func is user provided callback function to inform the
 * caller that the state transition had been completed. This callback function
 * can be called multiple times in case of an error that is being handled by
 * calling \ref shr_sysm_instance_restart. In other words, for the first call to
 * \ref shr_sysm_instance_new the callback function will be called once. Then
 * if the callback function indicated an error with the instance the application
 * may attempt to retry by calling \ref shr_sysm_instance_restart. This call
 * will trigger another callback to this callback function.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_stop(shr_sysm_categories_t instance_cat,
                                  int unit,
                                  bool graceful,
                                  sysm_instance_state_chng_cb func);

/*!
 * \brief System manager instance step.
 *
 * This function execute one or more steps on an instance of the system
 * manager based on the instance category and unit. For this function
 * to be effective, the system manager instance must be created in
 * a single step mode. This function is useful only for debug
 * purposes.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 * \param [in] number_of_steps indicates how many single steps to.
 * execute by system manager. Single step involved single call to
 * single component. Set this parameter to 0xFFFFFFFF to run in normal mode.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM Invalid category or instance wasn't created.
 * \return SHR_E_UNIT Invalid unit number.
 * \return SHR_E_INIT The system was not initialized properly.
 */
extern int shr_sysm_instance_step(shr_sysm_categories_t instance_cat,
                                  int unit,
                                  uint32_t number_of_steps);

/*!
 * \brief System manager instance restart.
 *
 * This function restarts an instance that was paused due to error in one of
 * its components. The restart function restores the status of the instance.
 * The instance thread will resume its execution and will try to process
 * the last failed component action.
 * The application should try calling this function only in the case where the
 * creating new instance (\ref shr_sysm_instance_new) or stopping an instance
 * (\ref shr_sysm_instance_stop) indicated in the callback function associated
 * with these two functions, that the operation failed. In case of failure, the
 * system manager freezes its actions. This function therefore can attempt to
 * resume the operation of the system manager instance.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_BUSY Instance is actively running.
 * \return SHR_E_CONFIG Instance doesn't use the internal thread module.
 * \return SHR_E_PARAM Invalid category or instance wasn't created.
 * \return SHR_E_UNIT Invalid unit number.
 * \return SHR_E_INIT The system was not initialized properly.
 */
extern int shr_sysm_instance_restart(shr_sysm_categories_t instance_cat,
                                     int unit);


/*!
 * \brief System manager instance process.
 *
 * This function processing an instance. The purpose of this function is to
 * be used in an external threading model. The function will block for the
 * duration of processing the system manager state machine.
 * This function should be used in two scenarios:
 * 1) After calling \ref shr_sysm_instance_new()
 * 2) After calling \ref shr_sysm_instance_stop()
 * In both cases the functions will only prepare the system manager instance
 * to be processed but will not block for the actual states processing. It is
 * only \ref shr_sysm_instance_process() that will actually perform the state
 * transitions of the particular system manager instance.
 *
 * \param [in] instance_cat is part of the system manager instance
 *             identifier.
 * \param [in] unit is (potentially) the second portion of the system manager
 *             instance identifier.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int shr_sysm_instance_process(shr_sysm_categories_t instance_cat,
                                     int unit);


/*!
 * \brief Install component name function.
 *
 * In order to make log messages more useful, a function can be
 * installed to translate the component ID into a name.
 *
 * The installed function should return a pointer to the name that
 * corresponds to the provided component ID. If no match is found, the
 * function must return NULL.
 *
 * \param [in] comp_name Function to translate a component ID into a name.
 */
extern void
shr_sysm_comp_name_init(const char *(*comp_name)(uint32_t comp_id));

/*!
 * \brief Get component name from component ID.
 *
 * This function is a wrapper around the application-provided
 * translation function, which is installed via \ref
 * shr_sysm_comp_name_init.
 *
 * \param [in] comp_id Component ID.
 *
 * \return Pointer to component name or NULL on error.
 */
extern const char *
shr_sysm_comp_name(uint32_t comp_id);

/*!
 * \brief Retrieve the state of a system manager instance.
 *
 * This function retrieves the current state of a system manager instance.
 *
 * \param [in] instance_cat First part of the system manager instance
 *             identifier.
 * \param [in] unit Optional second part of the system manager
 *             instance identifier.
 * \param [out] state Current state of the system manager instance.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_PARAM One of the input parameters is invalid.
 */
extern int
shr_sysm_instance_state_get(shr_sysm_categories_t instance_cat,
                            int unit,
                            shr_sysm_states_t *state);
#endif /* SHR_SYSM_H */
