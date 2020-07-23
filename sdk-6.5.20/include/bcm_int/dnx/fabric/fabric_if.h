/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/fabric/fabric_if.h
 * Reserved.$ 
 */

#ifndef _DNX_FABRIC_IF_H_INCLUDED_
/*
 * { 
 */
#define _DNX_FABRIC_IF_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>

/**
 * \brief
 *   Force traffic through fabric or to local cores
 */
typedef enum
{
    /**
     * Force Fabric
     */
    DNX_FABRIC_FORCE_FABRIC = 0,        /* Force Fabric */
    /**
     * Force Local Core 0
     */
    DNX_FABRIC_FORCE_LOCAL0,    /* Force Local Core 0 */
    /**
     * Force Local Core 1
     */
    DNX_FABRIC_FORCE_LOCAL1,    /* Force Local Core 1 */
    /**
     * Restore default
     */
    DNX_FABRIC_FORCE_RESTORE    /* Restore default */
} dnx_fabric_force_e;

/**
 * \brief
 *   When link is disabled, we need to force the signal, otherwise it
 *   can cause other blocks to get stuck.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   Fabric link id.
 * \param [in] force_signal -
 *   1 - force signal.
 *   0 - don't force signal.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_if_link_force_signal_set(
    int unit,
    int link,
    int force_signal);

/**
 * \brief
 *   Initialize FMAC configurations
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmac_index -
 *   The index of the FMAC.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_if_mac_init(
    int unit,
    int fmac_index);

/**
 * \brief
 *   Enable/Disable fabric port
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *  Fabric logical port to enable/disable.
 * \param [in] enable -
 *   1 - Enable.
 *   2 - Disable.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_if_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief
 *   Enable/Disable fabric port pahse 1
 *   To complete the operation _phase2 API must follow.
 *   Between the 2 APIs a delay of 1 milisec is mandatory
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *  Fabric logical port to enable/disable.
 * \param [in] enable -
 *   1 - Enable.
 *   2 - Disable.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_if_port_enable_set_phase1(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief
 *   Enable/Disable fabric port pahse 2
 *   To complete the operation _phase1 API must precede.
 *   Between the 2 APIs a delay of 1 milisec is mandatory
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *  Fabric logical port to enable/disable.
 * \param [in] enable -
 *   1 - Enable.
 *   2 - Disable.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_if_port_enable_set_phase2(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - 
 *   Power up fabric ports.
 *   The function adds all given ports to fabric ports bitmap, and power them
 *   up (probe them).
 *  
 * \param [in] unit - Unit-ID 
 * \param [in] pbmp - Bitmap of fabric ports to power up.
 * \param [out] okay_pbmp - Bitmap of fabric ports that were powered up.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_if_port_probe(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * okay_pbmp);

/**
 * \brief - 
 *   Power down fabric ports.
 *   The function removes all given ports from fabric ports bitmap, and power them
 *   down (detach them).
 *  
 * \param [in] unit - Unit-ID 
 * \param [in] pbmp - Bitmap of fabric ports to power down.
 * \param [out] detached - Bitmap of fabric ports that were powered down.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_if_port_detach(
    int unit,
    bcm_pbmp_t pbmp,
    bcm_pbmp_t * detached);

/**
 * \brief - 
 *   Set shaper on fabric link.
 *  
 * \param [in] unit - 
 *   The unit-ID 
 * \param [in] port -
 *   The port id to set shaper upon.
 * \param [in] pps -
 *   Packets Per Second limit.
 * \param [in] burst -
 *   Burst limit.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * Used for connecting the device to a repeater.
 * \see
 *   * None
 */
shr_error_e dnx_fabric_if_link_rate_egress_pps_set(
    int unit,
    bcm_port_t port,
    uint32 pps,
    uint32 burst);

/**
 * \brief - 
 *   Get shaper of fabric link.
 *  
 * \param [in] unit - 
 *   The unit-ID 
 * \param [in] port -
 *   The port id to get its shaper.
 * \param [out] pps -
 *   Packets Per Second limit.
 *   0 means shaper is disabled.
 * \param [out] burst -
 *   Burst limit.
 *   0 means shaper is disabled.
 * \return
 *   See \ref shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_if_link_rate_egress_pps_get(
    int unit,
    bcm_port_t port,
    uint32 *pps,
    uint32 *burst);

/**
 * \brief
 *   Checks if any of the Fabric Quads are disabled and according to that sets
 *   the LAST_IN_CHAIN for the FSRD and FMAC.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_brdc_fsrd_blk_id_set(
    int unit);

/**
 * \brief
 *   Get FSRD's clock power state.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fsrd_id -
 *   FSRD block id
 * \param [out] enable -
 *   1:enable, 0:disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fsrd_clock_power_get(
    int unit,
    int fsrd_id,
    int *enable);

/**
 * \brief
 *   Get whether FSRD block is enabled.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fsrd_block -
 *   FSRD block 
 * \param [out] enable -
 *   1:enable, 0:disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fsrd_block_enable_get(
    int unit,
    int fsrd_block,
    int *enable);

/**
 * \brief
 *   Power up/down a FSRD's clock power.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fsrd_id -
 *   FSRD block id
 * \param [in] enable -
 *   1:enable, 0:disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fsrd_clock_power_set(
    int unit,
    int fsrd_id,
    int enable);

/**
 * \brief
 *   enable/disable a FSRD block.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fsrd_block -
 *   FSRD block id
 * \param [in] enable -
 *   1:enable, 0:disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fsrd_block_enable_set(
    int unit,
    int fsrd_block,
    int enable);

/**
 * \brief
 *   Power up/down a FSRD block.
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *   The port which its FSRD block should be enabled.
 * \param [in] enable -
 *   1:enable, 0:disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_port_fsrd_block_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief
 *   Initialize all FSRD blocks power .
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fsrd_blocks_power_init(
    int unit);

/**
 * \brief
 *   Get a bitmap of all supported FSRDs.
 * \param [in] unit -
 *   The unit number.
 * \param [out] supported_fsrds -
 *   A bitmap of the supported FSRDs.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_supported_fsrd_get(
    int unit,
    bcm_pbmp_t * supported_fsrds);

/**
 * \brief
 *   Get whether a FMAC block is enabled.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmac_block -
 *   The FMAC block index.
 * \param [out] enable -
 *   1 - FMAC block is enabled.
 *   0 - FMAC block is disabled.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fmac_block_enable_get(
    int unit,
    int fmac_block,
    int *enable);

/**
 * \brief
 *   Set fabric force
 * \param [in] unit -
 *   The unit number.
 * \param [in] force -
 *   Force type.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_force_set(
    int unit,
    dnx_fabric_force_e force);

/**
 * \brief
 *   Initialize fabric force tdm to default values according to device
 *   mode.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of init sequence.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_force_tdm_init(
    int unit);

#endif /*_DNX_FABRIC_IF_H_INCLUDED_*/
