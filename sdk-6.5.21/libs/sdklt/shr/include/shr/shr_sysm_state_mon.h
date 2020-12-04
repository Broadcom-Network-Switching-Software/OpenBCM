/*! \file shr_sysm_state_mon.h
 *
 * System manager state change notification
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SHR_SYSM_STATE_MON_H
#define SHR_SYSM_STATE_MON_H

/*!
 * \brief System manager states
 */
typedef enum shr_sysm_states_e {
    SHR_SYSM_INIT = 0,      /*!< Init state.                    */
    SHR_SYSM_COMP_CONFIG,   /*!< Component configuration state. */
    SHR_SYSM_PRE_CONFIG,    /*!< Pre-config state.              */
    SHR_SYSM_RUN,           /*!< Run state.                     */
    SHR_SYSM_STOP,          /*!< Stop state.                    */
    SHR_SYSM_SHUTDOWN,      /*!< Shutdown state.                */
    SHR_SYSM_STATE_LAST = SHR_SYSM_SHUTDOWN /*!< The last state.*/
} shr_sysm_states_t;

/*!
 * \brief System manager state monitor callback function.
 *
 * This function will be called by the system manager when an instance is
 * entering a new state. This function will be called prior to any processing
 * associated with this state.
 *
 * \param [in] unit Is the system manager instance identifier. Value of -1
 *             indicates that the instance is the generic instance.
 * \param [in] state Is the state of which this instance of the system manager
 *             has just entered. The callback occurs prior to executing this
 *             state.
 * \param [in] context Is the caller context used during the registration
 *             call to \ref shr_sysm_state_monitor_register().
 *
 * \return None.
 */
typedef void (*shr_sysm_state_monitor_f)(int unit,
                                         shr_sysm_states_t state,
                                         void *context);

/*!
 * \brief Register system manager state monitor.
 *
 * This function enables the caller to register a callback function to be
 * invoked before the system manager is starting to process new state.
 * This function might be called multiple times with different callback
 * functions to support multiple clients.
 *
 * This function can only be called after calling \ref shr_sysm_init()
 *
 * \param [in] cb The function callback to be called when the system
 * manager is entered a state for every instance.
 * \param [in] context The context associated with the callback. This opaque
 * pointer will be used when the callback is being called.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to allocate resources.
 */
extern int shr_sysm_state_monitor_register(shr_sysm_state_monitor_f cb,
                                           void *context);

/*!
 * \brief Unregister previously registered system manager state monitor.
 *
 * This function remove the previously registered monitor function from being
 * called. See \ref shr_sysm_state_monitor_register to register monitor
 * callback function.
 * If the callback function was not previously registered then this function
 * has no effect.
 *
 * \param [in] cb is the function callback that was previously registered.
 *
 * \return None
 */
extern void shr_sysm_state_monitor_unregister(shr_sysm_state_monitor_f cb);



#endif /* SHR_SYSM_STATE_MON_H_ */
