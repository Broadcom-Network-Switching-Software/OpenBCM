/*! \file stack.h
 *
 * BCM STACK module APIs and Structures used only internally.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_MBCM_STACK_H
#define BCMI_LTSW_MBCM_STACK_H

#include <sal/sal_types.h>
#include <bcm/types.h>


/*!
 * \brief Set the module id of a unit.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_set_f)(int unit,
                           int my_modid);

/*!
 * \brief Set the port in PORT_SYSTEM_DESTINATION entry for modid.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 * \param [in] port - port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_modport_set_f)(int unit,
                                   int modid,
                                   bcm_port_t dest_port,
                                   bcm_port_t gport);

/*!
 * \brief Get the port in PORT_SYSTEM_DESTINATION entry for modid.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 * \param [in] port - port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_modport_get_f)(int unit,
                                   int modid,
                                   bcm_port_t dest_port,
                                   bcm_port_t *gport);

/*!
 * \brief Cleat the port in PORT_SYSTEM_DESTINATION entry for modid.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_modport_clear_f)(int unit,
                                     int modid,
                                     bcm_port_t dest_port);

/*!
 * \brief Map the l2_if for the remote port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_l2_if_map_f)(int unit,
                                 int modid,
                                 bcm_port_t dest_port);

/*!
 * \brief Cleat the l2_if attach to remote port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_l2_if_clear_f)(int unit,
                                   int modid,
                                   bcm_port_t dest_port);

/*!
 * \brief Init the stack system.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_init_f)(int unit);

/*!
 * \brief Transform modport into system port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] port - port identifier.
 * \param [out] sys_port - system port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_sys_port_get_f)(int unit,
                                    int modid,
                                    bcm_port_t port,
                                    int *sys_port);

/*!
 * \brief Transform system port into modport.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port - system port.
 * \param [out] modid   - module id.
 * \param [out] port - port identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_sys_port_to_modport_get_f)(int unit,
                                               int sys_port,
                                               int *modid,
                                               bcm_port_t *port);
/*!
 * \brief Allocate available IFA 1.0 system_source.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_source_ifa_probe_copy - system port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_ifa_system_source_alloc_f)(int unit,
                                               int *sys_source_ifa_probe_copy);

/*!
 * \brief Destroy IFA 1.0 system_source.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*stack_ifa_system_source_destroy_f)(int unit);

/*! Stack driver structure. */
typedef struct mbcm_ltsw_stack_drv_s {

    /*! Set the module id of a unit. */
    stack_set_f stack_set;
    /* Set the port in PORT_SYSTEM_DESTINATION entry for modid. */
    stack_modport_set_f   stack_modport_set;
    /* Get the port in PORT_SYSTEM_DESTINATION entry for modid. */
    stack_modport_get_f   stack_modport_get;
    /* Clear the port in PORT_SYSTEM_DESTINATION entry for modid. */
    stack_modport_clear_f stack_modport_clear;
    /* Map the L2_IF for the remote port. */
    stack_l2_if_map_f stack_l2_if_map;
    /* Clear the L2_IF attached to remote port. */
    stack_l2_if_clear_f stack_l2_if_clear;
    /* Init the stack system. */
    stack_init_f   stack_init;
    /* Transform modport into system port. */
    stack_sys_port_get_f stack_sys_port_get;
    /* Transform system port into modport. */
    stack_sys_port_to_modport_get_f stack_sys_port_to_modport_get;
    /* Allocate available IFA 1.0 system_source. */
    stack_ifa_system_source_alloc_f stack_ifa_system_source_alloc;
    /* Destroy IFA 1.0 system_source. */
    stack_ifa_system_source_destroy_f stack_ifa_system_source_destroy;
} mbcm_ltsw_stack_drv_t;
/*!
 * \brief Set the stack driver of the device.
 *
 * \param [in]  unit            Unit Number.
 * \param [in]  drv             Failover driver to set.
 *
 * \retval      SHR_E_NONE      No errors.
 * \retval      !SHR_E_NONE     Failure.
 */
extern int
mbcm_ltsw_stack_drv_set(int unit, mbcm_ltsw_stack_drv_t *drv);


/*!
 * \brief Set the module id of a unit.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_set(int unit,
                    int my_modid);

/*!
* \brief Set the port in PORT_SYSTEM_DESTINATION entry for modid.
*
* \param [in] unit Unit Number.
* \param [in] modid   - module id.
* \param [in] port - port number.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
extern int
mbcm_ltsw_stack_modport_set(int unit,
                            int my_modid,
                            bcm_port_t dest_port,
                            bcm_port_t gport);


/*!
* \brief Get the port in PORT_SYSTEM_DESTINATION entry for modid.
*
* \param [in] unit Unit Number.
* \param [in] modid   - module id.
* \param [in] port - port number.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
extern int
mbcm_ltsw_stack_modport_get(int unit,
                            int my_modid,
                            bcm_port_t dest_port,
                            bcm_port_t *gport);


/*!
* \brief Cleat the port in PORT_SYSTEM_DESTINATION entry for modid.
*
* \param [in] unit Unit Number.
* \param [in] modid   - module id.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
extern int
mbcm_ltsw_stack_modport_clear(int unit,
                              int my_modid,
                              bcm_port_t dest_port);

/*!
 * \brief Map the l2_if for the remote port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_l2_if_map(int unit,
                          int my_modid,
                          bcm_port_t dest_port);

/*!
 * \brief Cleat the l2_if attach to remote port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid   - module id.
 * \param [in] dest_port - destination port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_l2_if_clear(int unit,
                            int my_modid,
                            bcm_port_t dest_port);

/*!
 * \brief Init the stack system.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_init(int unit);

/*!
 * \brief Transform modport into system port.
 *
 * \param [in] unit Unit Number.
 * \param [in] modid - module id.
 * \param [in] port - port identifier.
 * \param [out] sys_port - system port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_sys_port_get(int unit,
                             int modid,
                             bcm_port_t port,
                             int *sys_port);

/*!
 * \brief Transform system port into modport.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_port - system port.
 * \param [out] modid - module id.
 * \param [out] port - port identifier.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_sys_port_to_modport_get(int unit,
                                        int sys_port,
                                        int *modid,
                                        bcm_port_t *port);

/*!
 * \brief Allocate IFA 1.0 system_source.
 *
 * \param [in] unit Unit Number.
 * \param [in] sys_source_ifa_probe_copy - system port.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_ifa_system_source_alloc(int unit,
                                        int *sys_source_ifa_probe_copy);

/*!
 * \brief Destroy IFA 1.0 system_source.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
mbcm_ltsw_stack_ifa_system_source_destroy(int unit);
#endif /* BCMI_LTSW_MBCM_STACK_H */
