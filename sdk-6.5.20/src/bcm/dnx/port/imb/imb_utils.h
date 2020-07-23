/** \file imb_utils.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_UTILS_H_INCLUDED
/*
 * {
 */
#define IMB_UTILS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm/port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
  * }
  */

/*
 * Functions
 * {
 */
/**
 * \brief - Translate from BCM loopback to Portmod loopback
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] bcm_loopback - BCM loopback type
 * \param [out] portmod_loopback - Portmod loopback type
 *   
 * \return
 *   int - see _SHR_E_ * 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_portmod_loopback_from_bcm_loopback_get(
    int unit,
    bcm_port_t port,
    int bcm_loopback,
    portmod_loopback_mode_t * portmod_loopback);

/**
 * \brief - Translate from Portmod loopback to BCM loopback
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - port id
 * \param [in] portmod_loopback - Portmod loopback type
 * \param [out] bcm_loopback - BCM loopback type
 *   
 * \return
 *   int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_bcm_loopback_from_portmod_loopback_get(
    int unit,
    bcm_port_t port,
    portmod_loopback_mode_t portmod_loopback,
    int *bcm_loopback);

/**
 * \brief - Translate from BCM autoneg abilities to PORTMOD abilities
 * 
 * \param [in] num_abilities - number of abilities
 * \param [in] bcm_abilities - abilities of bcm layer
 * \param [out] portmod_abilities - abilities of portmod layer
 *   
 * \return
 *   void
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
void imb_portmod_an_ability_from_bcm_an_ability_get(
    int num_abilities,
    const bcm_port_speed_ability_t * bcm_abilities,
    portmod_port_speed_ability_t * portmod_abilities);

/**
 * \brief - Translate from PORTMOD autoneg abilities to BCM abilities
 * 
 * \param [in] num_abilities - number of abilities
 * \param [in] portmod_abilities - abilities of portmod layer
 * \param [out] bcm_abilities - abilities of bcm layer
 *   
 * \return
 *   void
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
void imb_bcm_an_ability_from_portmod_an_ability_get(
    int num_abilities,
    portmod_port_speed_ability_t * portmod_abilities,
    bcm_port_speed_ability_t * bcm_abilities);

 /**
  * \brief - Translate from BCM autoneg abilities to legacy PORTMOD abilities
  *
  * \param [in] num_abilities - number of abilities
  * \param [in] bcm_abilities - abilities of bcm layer
  * \param [out] portmod_abilities - abilities of portmod layer
  *
  * \return
  *   void
  *
  * \remarks
  *   * None
  * \see
  *   * None
  */
void imb_portmod_legacy_an_ability_from_bcm_an_ability_get(
    int num_abilities,
    const bcm_port_speed_ability_t * bcm_abilities,
    portmod_port_ability_t * portmod_abilities);

/**
 * \brief - Translate from PORTMOD autoneg abilities to BCM abilities
 *
 * \param [in] unit - chip unit id 
 * \param [in] port - port id
 * \param [in] max_num_abilities - maximum number of abilities
 * \param [in] portmod_abilities - abilities of portmod layer
 * \param [out] actual_num_ability - actual number of abilities retrieved
 * \param [out] bcm_abilities - abilities of bcm layer
 *
 * \return
 *   void
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_bcm_an_ability_from_portmod_legacy_an_ability_get(
    int unit,
    bcm_port_t port,
    int max_num_abilities,
    const portmod_port_ability_t * portmod_abilities,
    int *actual_num_ability,
    bcm_port_speed_ability_t * bcm_abilities);

 /**
  * \brief - Translate from PORTMOD local abilities to BCM abilities
  *
  * \param [in] unit - chip unit id
  * \param [in] port - port id
  * \param [in] max_num_abilities - maximum number of abilities
  * \param [in] portmod_abilities - abilities of portmod layer
  * \param [out] actual_num_ability - actual number of abilities retrieved
  * \param [out] bcm_abilities - abilities of bcm layer
  *
  * \return
  *   void
  *
  * \remarks
  *   * None
  * \see
  *   * None
  */
int imb_bcm_local_an_ability_from_portmod_legacy_an_ability_get(
    int unit,
    bcm_port_t port,
    int max_num_abilities,
    const portmod_port_ability_t * portmod_abilities,
    int *actual_num_ability,
    bcm_port_speed_ability_t * bcm_abilities);

/**
  * \brief - Wrapper of portmod_port_enable_set to support Error Recovery
  *
  * \param [in] unit - chip unit id
  * \param [in] port - port id
  * \param [in] flags - flags
  * \param [in] enable - enable
  *
  * \return
  *   int - Error Type
  *
  * \remarks
  *   * None
  * \see
  *   * None
  */
int imb_er_portmod_port_enable_set(
    int unit,
    int port,
    int flags,
    int enable);

/**
  * \brief - Wrapper of Portmod_port_add to support Error Recovery
  *
  * \param [in] unit - chip unit id
  * \param [in] port - port id
  * \param [in] add_info - Portmod port add info
  *
  * \return
  *   int - Error Type
  *
  * \remarks
  *   * None
  * \see
  *   * None
  */
int imb_er_portmod_port_add(
    int unit,
    int port,
    portmod_port_add_info_t * add_info);

/**
  * \brief - Wrapper of portmod_port_remove to support Error Recovery
  *
  * \param [in] unit - chip unit id
  * \param [in] port - port id
  *
  * \return
  *   int - Error Type
  *
  * \remarks
  *   * None
  * \see
  *   * None
  */
int imb_er_portmod_port_remove(
    int unit,
    int port);
/*
  * }
  */
#endif /* IMB_UTILS_H_INCLUDED */
