/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 $Id$
 */
#ifdef INCLUDE_PLP_IMACSEC

#include <soc/phy/phyctrl.h>
#include <shared/bsl.h>
#include "bcm_imacsec.h"

/*! \brief imacsec_plp_base_t_secy_init
 *
 * Initializes a SecY device instance identified by macsec_side parameter.
 *
 * API use order:
 *       This function must be executed before any other imacsec_plp_base_t_secy_*()
 *       or imacsec_plp_base_t_secy_*() functions.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param mutex (input) - User defined mutex function registration.
 * User, optionally, can pass user data as part of this parameter
 * that in turn gets passed into the mutex function.
 * @param settings_p (input)
 *      Pointer to the memory location where the device settings are stored.
 *
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK 0 : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_init(int unit, soc_port_t port,
                               unsigned int macsec_side,
                               bcm_plp_base_t_sec_mutex_t *mutex,
                               bcm_plp_base_t_secy_settings_t *setting_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    pa->mutex.user_data=mutex->user_data;
    pa->mutex.SecY0_mutex_take=mutex->SecY0_mutex_take;
    pa->mutex.SecY1_mutex_take=mutex->SecY1_mutex_take;
    pa->mutex.SecY0_mutex_give=mutex->SecY0_mutex_give;
    pa->mutex.SecY1_mutex_give=mutex->SecY1_mutex_give;
    rv = bcm_base_t_secy_device_init(pa,setting_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"bcm_base_t_secy_device_init failed:%d\n "), rv));
    }
   return rv;
}
/*! \brief imacsec_plp_base_t_secy_port_add
 *
 * Adds a new vPort (vPort policy) for one classification device instance
 * identified by macsec_side parameter.
 *
 * Note: If a vPort is added for an SA then it must be added to the same SecY
 *       device (macsec_side) where the SA was added via the imacsec_plp_base_t_secy_sa_add() function.
 *
 * API use order:
 *       A vPort can be added to a classification device instance only after this
 *       device has been initialized via the imacsec_plp_base_t_secy_init() function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param vport_handle_p (output)
 *      Pointer to the memory location where the vPort handle will be stored.
 *
 * This function is NOT re-entrant for the same vPort of the same macsec_side.
 * This function is re-entrant for different macsec_side's or different vPorts
 * of the same macsec_side.
 *
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided that the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_vport_add(int unit, soc_port_t port, unsigned int macsec_side,
                                    bcm_plp_base_t_secy_vport_handle_t *vport_handle_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_vport_add(pa,vport_handle_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"bcm_base_t_secy_vport_add  failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_vport_remove
 *
 * Removes an already added vPort (vPort policy) from one classification device
 * instance identified by macsec_side parameter.
 *
 * API use order:
 *       A vPort can be removed from a classification device instance only after
 *       this vPort has been added to the device via the imacsec_plp_base_t_secy_vport_add()
 *       function.
 *
 *       Before this function is called all the rules associated with
 *       this vPort must be removed.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param vport_handle (input)
 *      vPort handle for vPort to be removed.
 *
 * This function is NOT re-entrant for the same vPort of the same macsec_side.
 * This function is re-entrant for different macsec_side's or different vPorts
 * of the same macsec_side.
 *
 * This function cannot be called concurrently with imacsec_plp_base_t_secy_rule_add() and
 * imacsec_plp_base_t_secy_rule_remove() functions for the same macsec_side.
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided that the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_vport_remove(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_vport_handle_t vport_handle)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_vport_remove(pa,vport_handle);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_vport_remove  failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_uninit
 *
 * Uninitializes a SecY device instance identified by macsec_side parameter.
 *
 * API use order:
 *       This function must be called when the SecY device for this macsec_side
 *       is no longer needed. After this function is called no other imacsec_plp_base_t_secy_*()
 *       or imacsec_plp_base_t_secy_*() functions may be called for this macsec_side except the
 *       imacsec_plp_base_t_secy_init() function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * This function cannot be called concurrently with any other SecY
 * API function for the same macsec_side.
 * This function can be called concurrently with any other SecY API
 * function for the different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 */

int imacsec_plp_base_t_secy_uninit(int unit, soc_port_t port, unsigned int macsec_side)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_uninit(pa);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_uninit  failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_sa_add
 *
 * Adds a new SA for a SecY device instance identified by macsec_side parameter.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param vport_handle (input)
 *      vPort handle for the vPort where the new SA must be added.
 *
 * @param sa_handle_p (output)
 *      Placeholder where the SA handle will be stored. It is used
 *      in the bcm_base_t_secy_sa*() function calls for this macsec_side.
 *
 * @param sa_p (input)
 *      Pointer to the memory location where the data for the new SA is stored.
 *
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_remove()
 * function for the same macsec_side.
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_add(int unit, soc_port_t port, unsigned int macsec_side,
                                 const bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                 bcm_plp_base_t_secy_sa_handle_t *sa_handle_p,
                                 const bcm_plp_base_t_secy_sa_t *sa_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_add(pa,vport_handle,sa_handle_p,sa_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_add  failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_sa_update
 *
 * Updates SA flow parameters for an already added SA for a SecY device
 * instance identified by macsec_side parameter.
 *
 * Note: This function does not update the SA transform record data and it
 *       cannot be used to update the key in the transform record.
 *
 * API use order:
 *      This function may be called for the sa_handle obtained via
 *      the imacsec_plp_base_t_secy_sa_add() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param sa_handle (input)
 *      SA handle of the SA to be updated.
 *
 * @param sa_p (input)
 *      Pointer to the memory location where the new parameters for the SA are
 *      stored.
 *
 * This function is re-entrant for the same of different macsec_side's.
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_remove()
 * function for the same macsec_side.
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_update(int unit, soc_port_t port, unsigned int macsec_side,
                                    const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                    const bcm_plp_base_t_secy_sa_t *sa_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv=bcm_base_t_secy_sa_update(pa,sa_handle,sa_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_update  failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_sa_read
 *
 * Reads (part of) a transform record of an already added SA from a SecY
 * device instance identified by macsec_side parameter.
 *
 * API use order:
 *      This function may be called for the sa_handle obtained via the
 *      imacsec_plp_base_t_secy_sa_add() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param sa_handle (input)
 *      SA handle of the SA to be read.
 *
 * @param word_offset (input)
 *      Word offset in transform record where the data should be read from.
 *
 * @param word_count (input)
 *      Number of words which must be read from the SA transform record.
 *
 * @param transform_p (output)
 *      Pointer to the memory location where the word_count 32-bit words
 *      of the SA transform record will be stored.
 *
 * This function is re-entrant.
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different sa_handle of the same or different
 * macsec_side provided that the user order of the API is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_read(int unit, soc_port_t port, unsigned int macsec_side,
                                  const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                  const unsigned int word_offset,
                                  const unsigned int word_count,
                                  unsigned int * transform_p)

{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv=bcm_base_t_secy_sa_read(pa,sa_handle,word_offset,word_count,transform_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_read  failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_sa_remove
 *
 * Removes a previously added SA from a SecY device instance identified by
 * macsec_side parameter.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param sa_handle (input)
 *      SA handle of the SA to be removed. After this function returns this
 *      handle should not be used anymore with the imacsec_plp_base_t_secy_sa*() functions.
 *
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_add()
 * function for the same macsec_side.
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_read()
 * function for the same sa_handle.
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_remove(int unit, soc_port_t port, unsigned int macsec_side,
                                    const bcm_plp_base_t_secy_sa_handle_t sa_handle
                                    )
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_remove(pa, sa_handle);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_remove failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_rule_add
 *
 * Adds a new rule for matching a packet to a vPort identified by vport_handle
 * for one classification device instance identified by macsec_side parameter.
 *
 * Note: If a rule is added for a vPort then it must be added to the same
 *       device (macsec_side) where the vPort was added via the imacsec_plp_base_t_secy_vport_add()
 *       function.
 *
 * API use order:
 *       A rule can be added to a classification device instance only after
 *       the vPort identified by vport_handle has been added to this device
 *       via the imacsec_plp_base_t_secy_vport_add() function.
 *
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param vport_handle (input)
 *      vPort handle for the vPort where the packet matching rule must be added.
 *
 * @param rule_handle_p (output)
 *      Pointer to the memory location where where the rule handle will be stored.
 *
 * @param rule_p (input)
 *      Pointer to the memory location where the data for the rule is stored.
 *
 * This function is NOT re-entrant for the same rule of the same macsec_side.
 * This function is re-entrant for different macsec_side's or different rules
 * of the same macsec_side.
 *
 * This function cannot be called concurrently with imacsec_plp_base_t_secy_vport_remove() and
 * imacsec_plp_base_t_secy_rule_remove() functions for the same macsec_side.
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided that the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_rule_add(int unit, soc_port_t port, unsigned int macsec_side,
                                   const bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                   bcm_plp_base_t_secy_rule_handle_t *rule_handle_p,
                                   const bcm_plp_base_t_secy_rule_t *rule_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv=bcm_base_t_secy_rule_add(pa,vport_handle,rule_handle_p,rule_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_add failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_rule_remove
 *
 * Removes an already added rule from one classification device
 * instance identified by macsec_side parameter.
 *
 * API use order:
 *       A rule can be removed from a classification device instance only after
 *       this rule has been added to the device via the imacsec_plp_base_t_secy_rule_add()
 *       function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param rule_handle (input)
 *      Rule handle for the rule to be removed.
 *
 * This function is NOT re-entrant for the same rule of the same macsec_side.
 * This function is re-entrant for different macsec_side's or different rules
 * of the same macsec_side.
 *
 * This function cannot be called concurrently with imacsec_plp_base_t_secy_vport_remove() and
 * imacsec_plp_base_t_secy_rule_add() functions for the same macsec_side.
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided that the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_rule_remove(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_remove(pa,rule_handle);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_remove failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_rule_update
 *
 * Updates a packet matching rule for one classification device instance
 * identified by macsec_side parameter.
 *
 * Note: A rule must be updated to the same device (macsec_side) where
 *       the corresponding vPort was added via the imacsec_plp_base_t_secy_vport_add() function.
 *
 * API use order:
 *       A rule can be updated for a classification device instance only after
 *       the corresponding vPort has been added to this device
 *       via the imacsec_plp_base_t_secy_vport_add() function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param rule_handle (input)
 *      Rule handle for rule to be enabled.
 *
 * @param rule_p (input)
 *      Pointer to the memory location where the data for the rule is stored.
 *
 * This function is NOT re-entrant for the same rule of the same macsec_side.
 * This function is re-entrant for different macsec_side's or different rules
 * of the same macsec_side.
 *
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided that the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_rule_update(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                      const bcm_plp_base_t_secy_rule_t *rule_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_update(pa,rule_handle,rule_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_update failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_rule_enable
 *
 * Enables an already added rule for one classification device instance
 * identified by macsec_side parameter.
 *
 * API use order:
 *       A rule can be enabled after it has been added
 *       via the secy API imacsec_plp_base_t_secy_rule_add() function or updated via
 *       the imacsec_plp_base_t_secy_rule_updated() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param rule_handle (input)
 *      Rule handle for rule to be enabled.
 *
 * @param fsync (input)
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the rule is enabled.
 *
 * This function is NOT re-entrant for the same Device when fsync is true.
 * This function is re-entrant for the same Device when fsync is false.
 * This function is re-entrant for different Devices.
 *
 * This function can be called concurrently with any other secy API function
 * for the same or different rule_handle of the same Device or
 * different Devices provided that the API use order is followed.
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the secy API which also takes fsync parameter set
 * to true for the same Device.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_rule_enable(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                      const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_enable(pa,rule_handle,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_enable failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_rule_disable
 *
 * Disables an already added rule for one classification device instance
 * identified by macsec_side parameter.
 *
 * API use order:
 *       A rule can be enabled after it has been added
 *       via the secy API imacsec_plp_base_t_secy_rule_add() function or updated via
 *       the imacsec_plp_base_t_secy_rule_update() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param rule_handle (input)
 *      Rule handle for rule to be enabled.
 *
 * @param fsync (input)
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the rule is disabled.
 *
 * This function is NOT re-entrant for the same Device when fsync is true.
 * This function is re-entrant for the same Device when fsync is false.
 * This function is re-entrant for different Devices.
 *
 * This function can be called concurrently with any other secy API function
 * for the same or different rule_handle of the same Device or
 * different Devices provided that the API use order is followed.
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the secy API which also takes fsync parameter set
 * to true for the same Device.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_rule_disable(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                      const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_disable(pa,rule_handle,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_disable failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_rule_enable_disable
 *
 * Enables and/or disables an already added rule from one Classification device
 * instance identified by macsec_side parameter. Can also enable or disable all
 * rules for this device instance at once.
 *
 * API use order:
 *       An rule can be enabled or disabled after its transform record is
 *       installed via the SecY API imacsec_plp_base_t_secy_sa_add() function and the rule is
 *       added via the imacsec_plp_base_t_secy_rule_add() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param rule_handle_enable (input)
 *      Rule handle for rule to be enabled. Will be ignored if equal to
 *      bcm_plp_base_t_secy_rule_handle_NULL.
 *
 * @param rule_handle_disable (input)
 *      Rule handle for rule to be disabled. Will be ignored if equal to
 *      bcm_plp_base_t_secy_rule_handle_NULL.
 *
 * @param enable_all (input)
 *      When set to true all rules will be enabled. Takes precedence over the
 *      other parameters.
 *
 * @param disable_all (input)
 *      When set to true all rules will be disabled. Takes precedence over the
 *      other parameters, except enable_all.
 *
 * @param fsync (input)
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the rule is enabled or disabled or both.
 *
 * This function is NOT re-entrant for the same Device when fsync is true.
 * This function is re-entrant for the same Device when fsync is false.
 * This function is re-entrant for different Devices.
 *
 * This function can be called concurrently with any other secy API function
 * for the same or different rule_handles of the same Device or
 * different Devices provided that the API use order is followed.
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the secy API which also takes fsync parameter set
 * to true for the same Device.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_rule_enable_disable(int unit, soc_port_t port, unsigned int macsec_side,
                                              const bcm_plp_base_t_secy_rule_handle_t rule_handle_disable,
                                              const bcm_plp_base_t_secy_rule_handle_t rule_handle_enable,
                                              const unsigned char enable_all,
                                              const unsigned char disable_all,
                                              const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_enable_disable(pa,rule_handle_disable,rule_handle_enable,enable_all,disable_all,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_enable_disable failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_sa_chain
 *
 * Adds a new SA transform record and chain it to the current active SA.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param active_sa_handle (input)
 *      SA handle of the current active SA.
 *
 * @param new_sa_handle_p (output)
 *      Place holder where the SA handle will be stored that can be used
 *      in the imacsec_plp_base_t_secy_sa*() function calls for this macsec_side.
 *
 * @param new_sa_p (input)
 *      Pointer to a memory location where the data for the new SA is stored.
 *
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_add() and
 * imacsec_plp_base_t_secy_sa_remove() function for the same macsec_side.
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_chain(int unit, soc_port_t port, unsigned int macsec_side,
                                               const bcm_plp_base_t_secy_sa_handle_t active_sa_handle,
                                               bcm_plp_base_t_secy_sa_handle_t * const new_sa_handle_p,
                                               const bcm_plp_base_t_secy_sa_t * const new_sa_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_chain(pa, active_sa_handle, new_sa_handle_p, new_sa_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_chain failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_sa_switch
 *
 * Manually switch to the new SA from the current active SA.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param active_sa_handle (input)
 *      SA handle of the current active SA.
 *
 * @param new_sa_handle (input)
 *      SA handle of the SA to be switched to.
 *
 * @param new_sa_p (input)
 *      Pointer to a memory location where the data for the new SA is stored.
 *
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_remove()
 * function for the same macsec_side.
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_switch(int unit, soc_port_t port, unsigned int macsec_side,
                                                const bcm_plp_base_t_secy_sa_handle_t active_sa_handle,
                                                const bcm_plp_base_t_secy_sa_handle_t new_sa_handle,
                                                const bcm_plp_base_t_secy_sa_t * const new_sa_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_switch(pa, active_sa_handle, new_sa_handle, new_sa_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_switch failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_limits
 *
 * Returns the maximum number of rules, vPorts, Security Associations (SAs)
 * and/or Secure Channels (SCs) of the XtSecY device instance identified by
 * DeviceId parameter.
 *
 * API use order:
 *       This function can be called after the function imacsec_plp_base_t_secy_init(pa)
 *       and before the function imacsec_plp_base_t_secy_uninit(pa) is called.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] max_vport_count_p
 *      Pointer to a memory location where the maximum number of vPorts must
 *      be stored. Skipped if the pointer is NULL.
 *
 * @param [out] max_rule_count_p
 *      Pointer to a memory location where the maximum number of TCAM rules
 *      must be stored. Skipped if the pointer is NULL.
 *
 * @param [out] max_sa_count_p
 *      Pointer to a memory location where the maximum number of SAs must be
 *      stored. Skipped if the pointer is NULL.
 *
 * @param [out] max_sc_count_p
 *      Pointer to a memory location where the maximum number of SCs must be
 *      stored. Skipped if the pointer is NULL.
 *
 * This function is re-entrant and called always as long as the API use order
 * is respected.
 *
 * @return BCM_PLP_BASE_T_SECY_STATUS_OK : success\n
 * @return BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter\n
 * @return BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_device_limits(int unit, soc_port_t port, unsigned int macsec_side,
                                                    unsigned int * const max_vport_count_p,
                                                    unsigned int * const max_rule_count_p,
                                                    unsigned int * const max_sa_count_p,
                                                    unsigned int * const max_sc_count_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_limits(pa, max_vport_count_p, max_rule_count_p, max_sa_count_p,max_sc_count_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_limits failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_vport_handle_issame
 *
 * Check whether provided Handle1 is equal to provided Handle2.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param handle1_p
 *      First handle
 *
 * @param handle2_p
 *      Second handle
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with
 * any other SecY API function for the same or different macsec_side.
 *
 * @return
 *      true:  provided handles are equal\n
 *      false: provided handles are not equal
 */

int imacsec_plp_base_t_secy_vport_handle_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                          const bcm_plp_base_t_secy_vport_handle_t * const handle1_p,
                                          const bcm_plp_base_t_secy_vport_handle_t * const handle2_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_vport_handle_issame(pa, handle1_p, handle2_p);
    if(rv ==0 || rv ==1){
        return rv;
    }else{
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_vport_handle_issame failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_sa_handle_issame
 *
 * Check whether the provided Handle1 is equal to provided Handle2.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param handle1_p
 *      First handle
 *
 * @param handle2_p
 *      Second handle
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *      true:  provided handles are equal\n
 *      false: provided handles are not equal
 */

int imacsec_plp_base_t_secy_sa_handle_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                                          const bcm_plp_base_t_secy_sa_handle_t * const handle1_p,
                                                          const bcm_plp_base_t_secy_sa_handle_t * const handle2_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_handle_issame(pa, handle1_p, handle2_p);
    if(rv ==0 || rv ==1){
        return rv;
    }else{
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_handle_issame failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_sa_handle_sa_index_issame
 *
 * Check whether the provided Handle points to the same SA as the provided
 * SA index.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param sa_handle
 *      SA handle
 *
 * @param sa_index
 *      SA index
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *      true:  provided sa index are equal \n
 *      false: provided sa index are not equal
 */

int imacsec_plp_base_t_secy_sa_handle_sa_index_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                                                      const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                                      const unsigned int sa_index)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_handle_sa_index_issame(pa, sa_handle, sa_index);
    if(rv ==0 || rv ==1){
        return rv;
    }else{
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_handle_sa_index_issame failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_sa_nextpn_update
 *
 * Updates the NextPN field for an inbound SA.
 *
 * API use order:
 *      This function may be called for the sa_handle obtained via the
 *      imacsec_plp_base_t_secy_sa_add() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param sa_handle (input)
 *      SA handle of the SA to be updated.
 *
 * @param next_pn_lo (input)
 *      Least significant 32 bits of new NextPN. For 32-bit sequence numbers
 *      this is the entire sequence number.
 *
 * @param next_pn_hi (input)
 *      Most significant 32 bits of new NextPN. Only applicable for 64-bit
 *      sequence numbers, will be ignored for 32-bit sequence numbers.
 *
 * @param fnext_pn_written_p (output)
 *      flag to indicate that NextPN is actually written. Output not provided
 *      if pointer is NULL.
 *
 * This function is NOT re-entrant for the same macsec_side.
 * This function is re-entrant for different macsec_side's.
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_remove()
 * function for the same macsec_side.
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_nextpn_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                     const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                     const unsigned int next_pn_lo,
                                                     const unsigned int next_pn_hi,
                                                     unsigned char * const fnext_pn_written_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_nextpn_update(pa, sa_handle, next_pn_lo,next_pn_hi,fnext_pn_written_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_nextpn_update failed:%d\n "), rv));
    }
    return rv;

}

/*! imacsec_plp_base_t_secy_device_update
 *
 *
 * This function updates the SecY device control settings.
 *
 * The BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR and BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR events can
 * be configured via the bcm_plp_base_t_secy_device_params_t:Control_p data structure by
 * specifying the packet counter threshold to be reached in order for both
 * events to be generated by the Classification Engine.
 *
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] device_p
 *      Pointer to the memory location where the control parameters
 *      are stored, see bcm_plp_base_t_secy_device_params_t.
 * Note: Application has to pass NULL for pointer elements inside bcm_plp_base_t_secy_device_params_t for which
 * update is not required.
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_device_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const bcm_plp_base_t_secy_device_params_t  * const device_p )
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_update(pa, device_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_update failed:%d\n "), rv));
    }
    return rv;

}

/*! imacsec_plp_base_t_secy_bypass_set
 *
 * Configure the port in low-latency bypass mode/normal mode.
 *
 * API use order:
 * 	  This function can be called for a SecY device only, after the
 * 	  device has been initialized via the bcm_base_t_secy_device_init() function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] fbypass
 *   true if port is set to low-latency bypass
 *   false if port is set to normal mode
 *
 * @return
  *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_bypass_set(int unit, soc_port_t port, unsigned int macsec_side,
                                                 const unsigned char fbypass)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_bypass_set(pa, fbypass);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_bypass_set failed:%d\n "), rv));
    }
    return rv;
}

/*! imacsec_plp_base_t_secy_bypass_get
 *
 *  Get the status of low-latency bypass mode
 *
 * API use order:
 *       This function can be called for a SecY device only, after the
 *       device has been initialized via the bcm_base_t_secy_device_init() function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] fbypass
 *   true if port is in low-latency bypass mode 
 *   false if port is in for normal operation
 *
 * @return
  *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_bypass_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                unsigned char *fbypass)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_bypass_get(pa, fbypass);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_bypass_get failed:%d\n "), rv));
    }
    return rv;

}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_crypt_auth_bypass_len_update
 *
 * Updates the length of the bypass data during crypt-authenticate operation
 * for a SecY device identified by macsec_side. The bypass data should contain
 * MAC header to be able to classify the packet into the corresponding
 * crypto-authenticate flow control register and additionally it might include
 * the IV used in the transformation.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] bypass_length
 *      The length in bytes of the bypass data.
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other function for the
 * same or different macsec_side's.
 *
 * @return
  *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_crypt_auth_bypass_len_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                                          const unsigned int bypass_length)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_crypt_auth_bypass_len_update(pa, bypass_length);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_crypt_auth_bypass_len_update failed:%d\n "), rv));
    }
    return rv;

}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_rules_mtu_check_update
 *
 * Updates rule for outbound MTU checks.
 *
 * API use order:
 *       Rules can be updated for a SecY device instance only after
 *       this device has been initialized via the imacsec_plp_base_t_secy_init(pa)
 *       function.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] sci_index
 *      Secure Channel index for which the MTU check rule must be updated.
 *
 * @param [in] mtucheck_rule_p
 *     Pointer to a memory location where the MTU compare rule
 *     for this Secure Channel is stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
   *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
   *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
   *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure

 */

int imacsec_plp_base_t_secy_rules_mtu_check_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                                  const unsigned int sci_index,
                                                                  const bcm_plp_base_t_secy_sc_rule_mtu_check_t * const mtucheck_rule_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rules_mtu_check_update(pa, sci_index, mtucheck_rule_p );
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rules_mtu_check_update failed:%d\n "), rv));
    }
    return rv;

}

/*! \brief imacsec_plp_base_t_secy_rule_handle_issame
 *
 * Check whether provided Handle1 is equal to provided Handle2.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param handle1_p
 *      First handle
 *
 * @param handle2_p
 *      Second handle
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other secy API function
 * for the same or different macsec_side.
 *
 * @return
 *      true:  provided handles are equal\n
 *      false: provided handles are not equal
 */

unsigned char imacsec_plp_base_t_secy_rule_handle_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                                                      const bcm_plp_base_t_secy_rule_handle_t * const handle1_p,
                                                                      const bcm_plp_base_t_secy_rule_handle_t * const handle2_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_handle_issame(pa, handle1_p, handle2_p );
    if(rv ==0 || rv ==1){
        return rv;
    }else{
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_handle_issame failed:%d\n "), rv));
    }
    return rv;

}
/*! \brief imacsec_plp_base_t_secy_sa_index_get
 *
 * Get the SA index from an SA, using the SA handle.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @pre API use order:\n
 *      This function may be called for the SA Handle obtained via
 *      the secy_sa_add() function for the same macsec_side.
 *
 * @param [in] sa_handle
 *      SA handle of the SA to get the index from.
 *
 * @param [out] sa_index_p
 *      Pointer to a memory location where the SA index will be stored.
 *
 * @param [out] sc_index_p
 *      Pointer to a memory location where the SC index will be stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_index_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                    const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                    unsigned int * const sa_index_p,
                                                    unsigned int * const sc_index_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_index_get(pa, sa_handle, sa_index_p, sc_index_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_index_get failed:%d\n "), rv));
    }
    return rv;

}
/*! \brief imacsec_plp_base_t_secy_rule_index_get
 *
 * Gets the Rule index from a Rule, using the Rule handle.
 *
 * API use order:
 *      This function may be called for the RuleHandle obtained via
 *      the imacsec_plp_base_t_secy_rule_add() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] rule_handle
 *      Rule handle of the Rule to get the index from.
 *
 * @param [out] rule_index_p
 *      Pointer to a memory location where the Rule index will be stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_rule_index_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                                      unsigned int * const rule_index_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_index_get(pa, rule_handle, rule_index_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_index_get failed:%d\n "), rv));
    }
    return rv;

}
/*! \brief imacsec_plp_base_t_secy_vport_index_get
 *
 * Gets the vPort index from an vPort, using the vPort handle.
 *
 * @pre API use order:
 *      This function may be called for the vPortHandle obtained via
 *      the imacsec_plp_base_t_secy_vport_add() function for the same DeviceId.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] vport_handle
 *      vPort handle of the vPort to get the index from.
 *
 * @param [out] vport_index_p
 *      Pointer to a memory location where the vPort index will be stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
 *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_vport_index_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                     const bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                     unsigned int * const vport_index_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_vport_index_get(pa, vport_handle, vport_index_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_vport_index_get failed:%d\n "), rv));
    }
    return rv;

}
/*! \brief imacsec_plp_base_t_secy_sa_handle_get
 *
 * Get the SA handle from an SA, using the SA index.
 *
 * @pre API use order:\n
 *      This function can be called after the function imacsec_plp_base_t_secy_init()
 *    and secy_sa_add is called.
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] sa_index
 *      Secure Association index for which the SA handle must be returned.
 *
 * @param [out] sa_handle_p
 *     Pointer to a memory location where the SA handle will be stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
  *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure

 */

int imacsec_plp_base_t_secy_sa_handle_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const unsigned int sa_index,
                                                      bcm_plp_base_t_secy_sa_handle_t * const sa_handle_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_handle_get(pa, sa_index, sa_handle_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_handle_get failed:%d\n "), rv));
    }
    return rv;

}
/*! \brief  imacsec_plp_base_t_secy_rule_handle_get
 *
 * Gets the Rule handle from a Rule, using the Rule index
 *
 * @pre API use order:
 *      This function can be called after the function imacsec_plp_base_t_secy_init()
 *      and before the function imacsec_plp_base_t_secy_unint() is called.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] rule_index
 *      Rule index for which the Rule handle must be returned.
 *
 * @param [out] rule_handle_p
 *     Pointer to a memory location where the Rule handle will be stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
  *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure

 */

int imacsec_plp_base_t_secy_rule_handle_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const unsigned int rule_index,
                                                      bcm_plp_base_t_secy_rule_handle_t * const rule_handle_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rule_handle_get(pa, rule_index, rule_handle_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rule_handle_get failed:%d\n "), rv));
    }
    return rv;

}
/*! \brief imacsec_plp_base_t_secy_vport_handle_get
 *
 * Gets the vPort handle from an vPort, using the vPort index.
 *
 * @pre API use order:
 *      This function can be called after the function imacsec_plp_base_t_secy_init()
 *      and before the function imacsec_plp_base_t_secy_unint() is called.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] vport_index
 *     vPort index for which the vPort handle must be returned.
 *
 * @param [out] vport_handle_p
 *     Pointer to a memory location where the vPort handle will be stored.
 *
 * This function is re-entrant for the same or different Devices.
 *
 * This function can be called concurrently with any other secy API
 * function for the same or different macsec_side provided the API use order
 * is followed.
 *
 * @return
  *     BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *     BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *     BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure

 */

int imacsec_plp_base_t_secy_vport_handle_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const unsigned int vport_index,
                                                         bcm_plp_base_t_secy_vport_handle_t * const vport_handle_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_vport_handle_get(pa, vport_index, vport_handle_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_vport_handle_get failed:%d\n "), rv));
    }
    return rv;

}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_tcam_statistics_get
 *
 * This function reads the TCAM device statistics.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 * @param [in] rule_handle
 *      The Rule handle to read the statistics for.
 *
 * @param [out] stat_p
 *      Pointer to the memory location where the device statistics
 *      counter identified by RuleIndex will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same Device when fsync is true.
 * This function is re-entrant for the same Device when fsync is false.
 * This function is re-entrant for different Devices.
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fsync parameter
 * set to true for the same macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_tcam_statistics_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                          const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                                          bcm_plp_base_t_secy_tcam_stat_t * const stat_p,
                                                          const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_tcam_statistics_get(pa, rule_handle, stat_p, fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_tcam_statistics_get failed:%d\n "), rv));
    }
    return rv;
}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_rxcam_statistics_get
 *
 * Reads RxCAM statistics (ingress only) from a SecY device instance
 * identified by macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the ingress or combined egress/ingress role.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] scindex
 *      Index of the Secure Channel for which the RxCAM statistics must be
 *      read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the ingress RxCAM statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same scindex no matter what fsync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fsync is true.\n
 * This function is re-entrant for the same Device when fSync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fsync parameter
 * set to true for the same Device.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_rxcam_statistics_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const unsigned int scindex,
                                                         bcm_plp_base_t_secy_rxcam_stat_t* const stats_p,
                                                         const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_rxcam_statistics_get(pa, scindex, stats_p, fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_rxcam_statistics_get failed:%d\n "), rv));
    }
    return rv;

}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_secy_statistics_i_get
 *
 * Reads ingress SecY statistics from a SecY device instance identified by
 * macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the ingress or combined egress/ingress role.
 *
 * API use order:
 *      This function may be called for the SAHandle obtained via
 *      the imacsec_plp_base_t_secy_sa_add(pa) function for the same macsec_side.
 *
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] vport_handle
 *      vPort handle for which the SecY statistics must be read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the ingress SecY statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same vPort no matter what fsync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fsync is true.\n
 * This function is re-entrant for the same Device when fsync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fsync parameter
 * set to true for the same Device.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_secy_statistics_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                              bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                              bcm_plp_base_t_secy_secy_stat_i_t * const stats_p,
                                                              const unsigned char fsync)

{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_secy_statistics_i_get(pa, vport_handle, stats_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_secy_statistics_i_get failed:%d\n "), rv));
    }
    return rv;
}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_secy_statistics_e_get
 *
 * Reads egress SecY statistics from a SecY device instance identified by
 * macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the egress or combined egress/ingress role.
 *
 * API use order:
 *      This function may be called for the SAHandle obtained via
 *      the bcm_base_t_secy_sa_add(pa) function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] vport_handle
 *      vPort handle for which the SecY statistics must be read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the egress SecY statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same vPort no matter what fsync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fsync is true.\n
 * This function is re-entrant for the same Device when fSync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fSync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fSync parameter
 * set to true for the same Device.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_secy_statistics_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                              bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                              bcm_plp_base_t_secy_secy_stat_e_t * const stats_p,
                                                              const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_secy_statistics_e_get(pa, vport_handle, stats_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_secy_statistics_e_get failed:%d\n "), rv));
    }
    return rv;
}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_sa_statistics_i_get
 *
 * Reads ingress SA statistics from a SecY device instance identified by
 * macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the ingress or combined egress/ingress role.
 *
 * API use order:
 *      This function may be called for the SAHandle obtained via
 *      the imacsec_plp_base_t_secy_sa_add(pa) function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] sa_handle
 *      SA handle of the SA for which the statistics must be read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the ingress SA statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same SAHandle no matter what fSync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fsync is true.\n
 * This function is re-entrant for the same Device when fsync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fSync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fSync parameter
 * set to true for the same macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_statistics_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                          const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                          bcm_plp_base_t_secy_sa_stat_i_t * const stats_p,
                                                          const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_statistics_i_get(pa, sa_handle, stats_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_statistics_i_get failed:%d\n "), rv));
    }
    return rv;

}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_sa_statistics_e_get
 *
 * Reads egress SA statistics from a SecY device instance identified by
 * macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the egress or combined egress/ingress role.
 *
 * API use order:
 *      This function may be called for the SAHandle obtained via
 *      imacsec_plp_base_t_secy_sa_add(pa) function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] sa_handle
 *      SA handle of the SA for which the statistics must be read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the egress SA statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same SAHandle no matter what fsync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fsync is true.\n
 * This function is re-entrant for the same Device when fsync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fsync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fsync parameter
 * set to true for the same macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */


int imacsec_plp_base_t_secy_sa_statistics_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                         bcm_plp_base_t_secy_sa_stat_e_t * const stats_p,
                                                         const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_statistics_e_get(pa, sa_handle, stats_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_statistics_e_get failed:%d\n "), rv));
    }
    return rv;

}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_global_statistics_get
 *
 * This function reads the Global device statistics.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] stat_p
 *      Pointer to the memory location where the device statistics
 *      counter be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same Device when fsync is true.
 * This function is re-entrant for the same Device when fsync is false.
 * This function is re-entrant for different Devices.
 *
 * If fSync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fsync parameter
 * set to true for the same macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_global_statistics_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                            bcm_plp_base_t_secy_global_stat_t * const stat_p,
                                                            const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_global_statistics_get(pa, stat_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_global_statistics_get failed:%d\n "), rv));
    }
    return rv;

}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_ifc_statistics_e_get
 *
 * Reads egress interface statistics from a SecY device instance identified
 * by macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the egress or combined egress/ingress role.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] vport_handle
 *      vPort handle for which the interface statistics must be read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the egress interface statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same vPort no matter what fSync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fSync is true.\n
 * This function is re-entrant for the same Device when fSync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fSync is set to true then this function cannot be called concurrently
 * with any other function of the SecY API which also takes fSync parameter
 * set to true for the same Device.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_ifc_statistics_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                           bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                           bcm_plp_base_t_secy_ifc_stat_e_t * const stats_p,
                                                           const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_ifc_statistics_e_get(pa, vport_handle,stats_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_ifc_statistics_e_get failed:%d\n "), rv));
    }
    return rv;

}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_ifc_statistics_i_get
 *
 * Reads ingress interface statistics from a SecY device instance identified
 * by macsec_side parameter.
 *
 * @note This function can be called for the device that has been initialized
 *       for the ingress or combined egress/ingress role.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] vport_handle
 *      vPort handle for which the interface statistics must be read.
 *
 * @param [out] stats_p
 *      Pointer to a memory location where the ingress interface statistics
 *      will be stored.
 *
 * @param [in] fsync
 *      If true then this function will ensure that all the packets
 *      available in the device at the time of the function call are processed
 *      before the statistics is read.
 *
 * This function is NOT re-entrant for the same vPort no matter what fsync
 * value is.\n
 * This function is NOT re-entrant for the same Device when fSync is true.\n
 * This function is re-entrant for the same Device when fSync is false.\n
 * This function is re-entrant for different Devices.\n
 *
 * If fSync is set to true then this function cannot be called concurrently
 * with any other function of the XtSecY API which also takes fSync parameter
 * set to true for the same Device.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_ifc_statistics_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                          bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                          bcm_plp_base_t_secy_ifc_stat_i_t * const stats_p,
                                                          const unsigned char fsync)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_ifc_statistics_i_get(pa, vport_handle,stats_p,fsync);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_ifc_statistics_i_get failed:%d\n "), rv));
    }
    return rv;
}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_pifc_checkandclear
 *
 * Reads the values for Per-IFC count summary registers and clears the bits
 * that are set by hardware. Each bit represents an IFC (interface) that has
 * a statistics counter that crossed the threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [out] vport_handle_pp
 *      Pointer to a memory location where a set of vPort handles, that have
 *      their IFC statistics crossed the threshold, will be stored.
 *
 * @param [out] num_ifc_indexes_p
 *      Pointer to a memory location where to save number of IFC indexes
 *      whose statistics counters crossed the threshold.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_device_count_summary_pifc_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                         bcm_plp_base_t_secy_vport_handle_t ** const vport_handle_pp,
                                                                                         unsigned int * const num_ifc_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_pifc_checkandclear(pa, vport_handle_pp,num_ifc_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_pifc_checkandclear failed:%d\n "), rv));
    }
    return rv;

}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_prxcam_checkandclear
 *
 * Reads the values for Per-RxCAM count summary registers and clears the bits
 * that are set by hardware. Each bit represents an RxCAM hit counter that has
 * crossed the threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] rx_cam_indexes_pp
 *      Pointer to a memory location where a set of RxCAM hit counter indexes
 *      that have their statistics crossed the threshold will be stored.
 *
 * @param [out] num_rx_cam_indexes_p
 *      Pointer to a memory location where to save number of RxCAM hit counter
 *      indexes whose statistics counters crossed the threshold.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_device_count_summary_prxcam_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                             unsigned int ** const rx_cam_indexes_pp,
                                                                                             unsigned int * const num_rx_cam_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_prxcam_checkandclear(pa, rx_cam_indexes_pp, num_rx_cam_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_prxcam_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_psa_checkandclear
 *
 * Reads the values for Per-SA expired summary registers and clears the bits
 * that are set by hardware. Each bit represents a SA that has a statistics
 * counter that crossed the threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [out] sa_handle_pp
 *      Pointer to a memory location where a set of SA handles, that have
 *      their SA statistics crossed the threshold, will be stored.
 *
 * @param [out] num_sa_indexes_p
 *      Pointer to a memory location where to save number of SA indexes whose
 *      statistics counters crossed the threshold.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * This function cannot be called concurrently with bcm_base_t_secy_sa_remove
 * for the same macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_device_count_summary_psa_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                       bcm_plp_base_t_secy_sa_handle_t ** const sa_handle_pp,
                                                                                       unsigned int * const num_sa_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_psa_checkandclear(pa, sa_handle_pp, num_sa_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_psa_checkandclear failed:%d\n "), rv));
    }
    return rv;

}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_psecy_checkandclear
 *
 * Reads the values for the Per-SecY count summary registers and clears the
 * bits that are set by hardware. Each bit represents a SecY that has a
 * statistics counter that crossed the threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] vport_handle_pp
 *      Pointer to a memory location where a set of vPort Handles, that have
 *      their SecY statistics crossed the threshold, will be stored.
 *
 * @param [out] nm_secy_indexes_p
 *      Pointer to a memory location where to save number of SecY indexes
 *      whose statistics counters crossed the threshold.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_device_count_summary_psecy_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                         bcm_plp_base_t_secy_vport_handle_t ** const vport_handle_pp,
                                                                                         unsigned int * const nm_secy_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_psecy_checkandclear(pa, vport_handle_pp, nm_secy_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_psecy_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_ptcam_checkandclear
 *
 * Reads the values for Per-TCAM count summary registers and clear the bits
 * that are set by hardware. Each bit represents an TCAM that has
 * a statistics counter that crossed the threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] rule_handle_pp
 *      Pointer to a memory location where a set of Rule handles, that have
 *      their TCAM statistics crossed the threshold, will be stored.
 *
 * @param [out] num_tcam_indexes_p
 *      Pointer to a memory location where to save number of TCAM indexes
 *      whose statistics counters crossed the threshold.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_device_count_summary_ptcam_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                 bcm_plp_base_t_secy_rule_handle_t ** const rule_handle_pp,
                                                                                 unsigned int * const num_tcam_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_ptcam_checkandclear(pa, rule_handle_pp, num_tcam_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_ptcam_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_sa_checkandclear
 *
 * Reads the value of a SA statistics summary register and clears the bits that
 * are set by hardware. Each bit represents a SA statistics counter. If the
 * bit is set, it indicates that the corresponding counter has crossed the set
 * threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] sa_handle
 *      SA handle for the SA to be checked.
 *
 * @param [out] count_summary_sa_p
 *      Pointer to a memory location where saves a set of bits, each of them
 *      represents one SA statistics counters.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * This function cannot be called concurrently with imacsec_plp_base_t_secy_sa_remove
 * for the same macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_device_count_summary_sa_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                    bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                                                    unsigned int * const count_summary_sa_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_sa_checkandclear(pa, sa_handle, count_summary_sa_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_sa_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_secy_checkandclear
 *
 * Reads the value of a SecY counter summary register and clears the bits that
 * are set by hardware. Each bit represents a SecY statistics counter. If the
 * bit is set, it indicates that the corresponding counter has crossed the set
 * threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] vport_handle
 *      vPort handle of the SecY to be checked.
 *
 * @param [out] count_summary_secy_p
 *      Pointer to a memory location in which a set of bits is returned,
 *      each bit represents a SecY statistics counter.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_device_count_summary_secy_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                        bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                                                        unsigned int * const count_summary_secy_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_secy_checkandclear(pa, vport_handle, count_summary_secy_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_secy_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_tcam_global_checkandclear
 *
 * Reads the value of the TCAM global counter summary register and clears the
 * bits that are set by hardware. Each bit represents an TCAM statistics
 * counter. If the bit is set, it indicates that the corresponding counter has
 * crossed the set threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [out] count_summary_tcam_global_p
 *      Pointer to a memory location in which a set of bits is returned,
 *      each bit represents one IFC statistics counter.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_device_count_summary_tcam_global_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                             unsigned int * const count_summary_tcam_global_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_tcam_global_checkandclear(pa, count_summary_tcam_global_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_tcam_global_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_device_count_summary_ifc_checkandclear
 *
 * Reads the value of a IFC counter summary register and clears the bits that
 * are set by hardware. Each bit represents an IFC statistics counter. If the
 * bit is set, it indicates that the corresponding counter has crossed the set
 * threshold.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] vport_handle
 *      vPort handle of the IFC to be checked.
 *
 * @param [out] count_summary_ifc_p
 *      Pointer to a memory location in which a set of bits is returned,
 *      each bit represents one IFC statistics counter.
 *
 * This function is NOT re-entrant for the same Device.
 * This function is re-entrant for different Devices.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure

 */
int imacsec_plp_base_t_secy_device_count_summary_ifc_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                     bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                                                     unsigned int * const count_summary_ifc_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_device_count_summary_ifc_checkandclear(pa, vport_handle, count_summary_ifc_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_device_count_summary_ifc_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_sa_expired_summary_checkandclear
 *
 * Reads the values for SA expired summary registers and clears the bits that
 * are set by hardware. Each bit tells if one SA expired.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [out] sa_handle_pp
 *      Pointer to a memory location where the set of SA handles is stored,
 *      whose SA have processed more than the threshold.
 *
 * @param [out] num_sa_indexes_p
 *      Pointer to a memory location where to save number of SA indexes whose
 *      statistics counters crossed the threshold.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_sa_expired_summary_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                           bcm_plp_base_t_secy_sa_handle_t ** const sa_handle_pp,
                                                                           unsigned int * const num_sa_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_expired_summary_checkandclear(pa, sa_handle_pp, num_sa_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_expired_summary_checkandclear failed:%d\n "), rv));
    }
    return rv;
}
/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_sa_pnthr_summary_checkandclear
 *
 * Reads the values for SA packet number threshold summary registers and clears
 * the bits that are set by hardware. Each bit tells if one SA processed more
 * packets than the threshold set.
 *
 * @param pa
 *      bcm_plp_base_t_sec phy access
 *
 *
 * @param [out] sa_handle_pp
 *      Pointer to a memory location where the set of SA handles is stored,
 *      whose SA have processed more than the threshold.
 *
 * @param [out] num_sa_indexes_p
 *      Pointer to a memory location where to save number of SA indexes whose
 *      statistics counters crossed the threshold.
 *
* @return
*    BCM_PLP_BASE_T_SECY_STATUS_OK : success
*    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
*    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_sa_pnthr_summary_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                             bcm_plp_base_t_secy_sa_handle_t ** const sa_handle_pp,
                                                                             unsigned int * const num_sa_indexes_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_pnthr_summary_checkandclear(pa, sa_handle_pp, num_sa_indexes_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_pnthr_summary_checkandclear failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_intr_enable_set
 *
 * This API is used to enable SecY interrupts for specified events resulting in interrupts
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param[in] secy_intr_p
 *      pointer to secy interrupt structure to send interrupt event list\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_intr_enable_set(int unit, soc_port_t port, unsigned int macsec_side,
                                                       const bcm_plp_base_t_secy_intr_t *secy_intr_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_intr_enable_set(pa, secy_intr_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_intr_enable_set failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_intr_enable_get
 *
 * This API is used to get enabled SecY interrupts events resulting in interrupts
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param[out] secy_intr_p
 *     pointer to secy interrupt structure to get enabled interrupt event list\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_intr_enable_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                       bcm_plp_base_t_secy_intr_t *secy_intr_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_intr_enable_get(pa, secy_intr_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_intr_enable_get failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_intr_status_get
 *
 * This API is used to get the status of enabled SecY interrupts
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param[out] secy_intr_p
 *     pointer to secy interrupt structure to get interrupt event list\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_intr_status_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                       bcm_plp_base_t_secy_intr_t *secy_intr_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_intr_status_get(pa, secy_intr_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_intr_status_get failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_intr_status_clear
 *
 * This API clears enabled interrupts status of SecY interrupts for specified interrupt
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param[in] secy_intr_p
 *     pointer to secy interrupt structure to send interrupt event_mask list to clear intr status\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_intr_status_clear(int unit, soc_port_t port, unsigned int macsec_side,
                                                   const bcm_plp_base_t_secy_intr_t *secy_intr_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_intr_status_clear(pa, secy_intr_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_intr_status_clear failed:%d\n "), rv));
    }
    return rv;
}
/*! \brief imacsec_plp_base_t_secy_event_status_get
 *
 * This API is used to get raw status of all events
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param[out] secy_intr_p
 *     pointer to secy interrupt structure to get event list\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_event_status_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                        bcm_plp_base_t_secy_intr_t *secy_intr_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_event_status_get(pa, secy_intr_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_event_status_get failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_intr_set
 *
 * This API is used to enable/disable SecY interrupts for specified events resulting in interrupts
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] secy_intr
 *      pointer to secy interrupt structure to send interrupt event list\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * @param enable [in]
 *    1    - Enable the interrupt
 *    0    - Disable the interrupt
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

bcm_plp_base_t_secy_status_t imacsec_plp_base_t_secy_intr_set(int unit, soc_port_t port, unsigned int macsec_side,
                                                                    const bcm_plp_base_t_secy_intr_t *secy_intr, const unsigned int enable)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_intr_set(pa, secy_intr, enable);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_intr_set failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_intr_get
 *
 * This API is used to get enabled SecY interrupts events resulting in interrupts
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param[out] secy_intr
 *     pointer to secy interrupt structure to get enabled interrupt event list\n
 *
 * --------------------------------------------------------------\n
 *      SECY Global AIC interrupt resources \n
 * --------------------------------------------------------------\n
 *         Interrupt name                                     event_mask bit number   \n
 *    ------------------------------------------------------------\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_CLASS              0\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_PP                   1\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DROP_MTU                 2\n
 *      BCM_PLP_BASE_T_SECY_EVENT_CTRL_PKT                  3\n
 *      BCM_PLP_BASE_T_SECY_EVENT_DATA_PKT                  4\n
 *      BCM_PLP_BASE_T_SECY_EVENT_ENG_IRQ                    5\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MISS                6\n
 *      BCM_PLP_BASE_T_SECY_EVENT_TCAM_MULT_HIT         7\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SA_THR             8\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_SECY_THR          9\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_IFC_THR            10\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_GLOBAL_THR      11\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_TC_THR            12\n
 *      BCM_PLP_BASE_T_SECY_EVENT_STAT_RC_THR            13\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MISS                   14\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SC_MULT_HIT            15\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_PN_THR               16\n
 *      BCM_PLP_BASE_T_SECY_EVENT_SA_EXPIRED              17\n
 *
 * @param enable [out]
 *    Return the status of enabled interrupt
 *    1    - Interrupts are enabled for specified event mask in interrupt source  
 *    0    - Interrupts are disabled for specified event mask in interrupt source  
 * This function is re-entrant for the same or different macsec_side's.
 *
 * This function can be called concurrently with any other SecY API function
 * for the same or different macsec_side.
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

bcm_plp_base_t_secy_status_t imacsec_plp_base_t_secy_intr_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                                     bcm_plp_base_t_secy_intr_t *secy_intr, unsigned int *enable)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_intr_get(pa, secy_intr, enable);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_intr_get failed:%d\n "), rv));
    }
    return rv;
}

/*! \brief imacsec_plp_base_t_secy_build_transform_record
 * * Construct the SA record for the operation described in params in
 * a memory buffer sa_buffer_p.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param params (input)
 *    Pointer to the SA parameters structure.
 * @param sa_buffer_p (input)
 *    Pointer to the the SA memory buffer where transfer record is stored,
 * The sa_buffer_p argument must point to a word-aligned
 * memory buffer whose size in words is at least equal to the
 * corresponding size parameter returned by SABuilder_GetSize().
 *
 * @return
 *   BCM_PLP_BASE_T_SAB_STATUS_OK : Success
 *   BCM_PLP_BASE_T_SAB_INVALID_PARAMETER : params is invalid, or the sa_buffer_p
 *   BCM_PLP_BASE_T_SAB_UNSUPPORTED_FEATURE : params describes an operations that
 *    is not supported on the hardware for which this SA builder
 *    is configured.
 */

int imacsec_plp_base_t_secy_build_transform_record(int unit, soc_port_t port, unsigned int macsec_side,
                                                                 bcm_plp_base_t_sa_builder_params_t *params,
                                                                 unsigned int *sa_buffer_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_build_transform_record(pa, params,sa_buffer_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_build_transform_record failed:%d\n "), rv));
    }
    return rv;

}

/**! \brief imacsec_plp_base_t_secy_sa_active_e_get
 *
 * Return the handle of the currently active egress SA for a given vPort.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] vport
 *      vPort number to which the SA applies.
 *
 * @param [out] active_sa_handle_p
 *      Currently active egress SA for the given vPort.
 *
 * This function is re-entrant for the same macsec_side.\n
 * This function is re-entrant for different macsec_side's.\n
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.\n
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
  */

int imacsec_plp_base_t_secy_sa_active_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                        const unsigned int vport,
                                                        bcm_plp_base_t_secy_sa_handle_t * const active_sa_handle_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_active_e_get(pa, vport,active_sa_handle_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_active_e_get failed:%d\n "), rv));
    }
    return rv;
}

/**! \brief imacsec_plp_base_t_secy_sa_active_i_get
 *
 * Return the handles of the currently active ingress SA's for a given vPort
 * and SCI.
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 *
 * @param [in] vport
 *      vPort number to which the SA applies.
 *
 * @param [in] sci_p
 *      SCI for which the SA is desired.
 *
 * @param [out] active_sa_handle_p
 *      Array of four currently active egress SA for the given vPort, one
 *      for each AN from 0 to 3. Any SA that is not active is represented
 *      by XtSecY_SAHandle_NULL.
 *
 * This function is re-entrant for the same macsec_side.\n
 * This function is re-entrant for different macsec_side's.\n
 *
 * This function can be called concurrently with any other SecY API
 * function for the same or different macsec_side.\n
 *
 * @return
 *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
 *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
 *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */

int imacsec_plp_base_t_secy_sa_active_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                  const unsigned int vport,
                                  const unsigned char * const sci_p,
                                  bcm_plp_base_t_secy_sa_handle_t * const active_sa_handle_p)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_active_i_get(pa, vport,sci_p, active_sa_handle_p);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_active_i_get failed:%d\n "), rv));
    }
    return rv;
}

/**----------------------------------------------------------------------------
 * imacsec_plp_base_t_secy_sa_window_size_update
 *
 * Updates the Window Size field for an inbound SA.
 *
 * API use order:
 *      This function may be called for the SAHandle obtained via the
 *      imacsec_plp_base_t_secy_sa_add() function for the same macsec_side.
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param macsec_side (input) - selects egress or ingress path
 *  0 - egress path
 *  1 - ingress path
 *
 * @param [in] sa_handle
 *      SA handle of the SA to be updated.
 *
 * @param [in] window_size
 *      32-bit integer representing the anti-replay window size.
 *
 * This function is NOT re-entrant for the same macsec_side.\n
 * This function is re-entrant for different macsec_side's.\n
 *
 * This function cannot be called concurrently with the imacsec_plp_base_t_secy_sa_remove()
 * function for the same macsec_side.\n
 * This function can be called concurrently with any other SecY API
 * function for the same or different DeviceId.
 *
 * @return
  *    BCM_PLP_BASE_T_SECY_STATUS_OK : success
  *    BCM_PLP_BASE_T_SECY_ERROR_BAD_PARAMETER : incorrect input parameter
  *    BCM_PLP_BASE_T_SECY_INTERNAL_ERROR : failure
 */
int imacsec_plp_base_t_secy_sa_window_size_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                                 const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                                 const unsigned int window_size)
{
    phy_ctrl_t  *pc;
    int rv=0;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,macsec_side);
    rv = bcm_base_t_secy_sa_window_size_update(pa, sa_handle,window_size);
    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_secy_sa_window_size_update failed:%d\n "), rv));
    }
    return rv;
}


/*! \brief imacsec_plp_base_t_version_get
 *
 * Get the IMACSEC software version information
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param version_info (output) -  Get the version information
 *    version_info structure has two member variable
 *        major_no  - Indicates major software release
 *        minor_no  - Indicates enhancement after major software release
 * @return
 *   return 0 on success
 */

int imacsec_plp_base_t_version_get(int unit,soc_port_t port,bcm_plp_base_t_version_t* version_info)
{
    int rv=0;
    phy_ctrl_t  *pc;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    pa = (bcm_plp_base_t_sec_access_t *)(((char *)pc->driver_data + pc->size) - sizeof(bcm_plp_base_t_sec_access_t));

    if(pa==NULL){
        return SOC_E_INTERNAL;
    }

    rv = bcm_base_t_imacsec_version_get(pa, version_info);

    if(rv !=0){
        LOG_CLI((BSL_META_U(unit,"imacsec_plp_base_t_version_get failed:%d\n "), rv));
    }
    return rv;
}


/*! \brief imacsec_plp_addr_read
 *
 * Reads the register address using mdio bus
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param reg_addr (input)  - Register address
 * @param value (output) -  register value read
 *
 * @return
 *   return 0 on success
 */

int imacsec_plp_addr_read(int unit,
                          unsigned int port,
                          unsigned int reg_addr,
                          unsigned int *value)
{
    phy_ctrl_t  *pc;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    pa = (bcm_plp_base_t_sec_access_t *)(((char *)pc->driver_data + pc->size) - sizeof(bcm_plp_base_t_sec_access_t));
    if(pa==NULL){
        return SOC_E_INTERNAL;
    }
    pa->phy_info.phy_addr= pa->macsec_dev_addr;
    *value = 0xffffffff;
    return plp_raw_read(&(pa->phy_info),reg_addr,value);
}
/*! \brief imacsec_plp_addr_write
 *
 *  write the register address using mdio bus
 *
 * @param unit (input) BCM unit number
 * @param port (input) - port number
 * @param reg_addr (input)  - Register address
 * @param value (input) -  register value to write
 * @return
 *   return 0 on success
 */

int imacsec_plp_addr_write(int unit,
                           unsigned int port,
                           unsigned int reg_addr,
                           unsigned int value)
{
    phy_ctrl_t  *pc;
    bcm_plp_base_t_sec_access_t *pa;
    EXT_PHY_INIT_CHECK(unit,port);
    pc = EXT_PHY_SW_STATE(unit, port);
    pa = (bcm_plp_base_t_sec_access_t *)(((char *)pc->driver_data + pc->size) - sizeof(bcm_plp_base_t_sec_access_t));
    if(pa==NULL){
        return SOC_E_INTERNAL;
    }
    pa->phy_info.phy_addr= pa->macsec_dev_addr;
    return plp_raw_write(&(pa->phy_info),reg_addr,value);
}
#endif /* INCLUDE_PLP_IMACSEC */
