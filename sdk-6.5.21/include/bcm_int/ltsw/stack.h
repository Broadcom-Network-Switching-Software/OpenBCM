/*! \file stack.h
 *
 * STACK internal management header file.
 * This file contains the management for STACK module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_STACK_H
#define BCMI_LTSW_STACK_H

/* SDK6 API header file */
#include <bcm/stack.h>

/* Mixed header file */
#include <bcm_int/ltsw/port.h>

/*!
 * \brief Initialize the Stack module.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcm_ltsw_stk_init(int unit);

/*!
 * \brief De-initialize the Stack module.
 *
 * This function is used to de-initialize the Stack module.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval SHR_E_NONE              No errors.
 * \retval !SHR_E_NONE             Failure.
 */
extern int
bcmi_ltsw_stk_deinit(int unit);

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
extern int
bcmi_ltsw_stk_modmap_map(int unit,
                         int setget,
                         bcm_module_t mod_in,
                         bcm_port_t port_in,
                         bcm_module_t *mod_out,
                         bcm_port_t *port_out);

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
 *      SHR_E_xxx
 */
extern int
bcmi_ltsw_stk_modport_traverse(int unit,
                               bcmi_ltsw_port_traverse_cb cb,
                               void *user_data);

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
extern int
bcmi_ltsw_stk_modport_to_l2_if(int unit,
                               int modid,
                               int port,
                               int *l2_if);

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
extern int
bcmi_ltsw_stk_l2_if_to_modport(int unit,
                               int l2_if,
                               int *port);
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
extern int
bcmi_ltsw_stk_pbmp_get(int unit,
                       bcm_pbmp_t *curr_pbmp,
                       bcm_pbmp_t *inac_pbmp,
                       bcm_pbmp_t *pre_pbmp);

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
extern int
bcmi_ltsw_stk_remote_port_modport_get(int unit,
                                      bcm_gport_t remote_port,
                                      bcm_port_t *local_port);

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
extern int
bcmi_ltsw_stk_sys_port_get(int unit,
                           int modid,
                           bcm_port_t port,
                           int *sys_port);

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
extern int
bcmi_ltsw_stk_sys_port_to_modport_get(int unit,
                                      int sys_port,
                                      int *modid,
                                      bcm_port_t *port);

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
extern int
bcmi_ltsw_stk_ifa_system_source_alloc(int unit,
                                      int *sys_source_ifa_probe_copy);

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
extern int
bcmi_ltsw_stk_ifa_system_source_destroy(int unit);

#endif /* BCMI_LTSW_STACK_H */
