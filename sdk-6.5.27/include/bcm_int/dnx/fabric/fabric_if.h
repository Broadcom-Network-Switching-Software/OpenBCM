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
#error "This file is for use by DNX family only!"
#endif

#include <bcm/types.h>
#include <soc/dnxc/fabric.h>

/**
 * \brief
 *   Force traffic through fabric or to local cores
 */
typedef enum
{
    /**
     * Restore default
     */
    DNX_FABRIC_FORCE_RESTORE = 0,       /* Restore default */
    /**
     * Force Fabric
     */
    DNX_FABRIC_FORCE_FABRIC = 1,        /* Force Fabric */
    /**
     * Force Local Core
     */
    DNX_FABRIC_FORCE_LOCAL = 2  /* Force Local Core */
} dnx_fabric_force_e;

#define DNX_FABRIC_FORCE_CORE_ALL_CORES     BCM_CORE_ALL

/*
 * Define the bits definition in SW
 * - Bits 0 is meanless
 * - Bits 1-15 stand for 1,2 ... ..., 15 errors
 * - Bit 16 stand for uncorrectable error
 * For 5T FEC, the uncorrectable error indication bit in DBAL is bit 6
 */
#define DNX_15T_FEC_UNCORRECTABLE_ERROR_INDICATION_BIT_IN_DBAL   (DNXC_FABRIC_LINK_UNCORRECTABLE_ERROR_INDICATION_BIT)
#define DNX_5T_FEC_UNCORRECTABLE_ERROR_INDICATION_BIT_IN_DBAL    6

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
 *   Return bitmap of supported FMACs
 * \param [in] unit -
 *   The unit number.
 * \param [out] supported_fmacs -
 *   Bitmap of supported fmac blocks.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_supported_fmacs_get(
    int unit,
    bcm_pbmp_t * supported_fmacs);

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
shr_error_e dnx_fabric_if_fsrd_fmac_last_in_chain_set(
    int unit);

/**
 * \brief
 *   Return bitmap of FMACs related to the FSRD
 * \param [in] unit -
 *   The unit number.
 * \param [in] fsrd_block -
 *   FSRD block
 * \param [out] fmac_bmp -
 *   bitmap of FMACs related to the FSRD
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fsrd_fmac_bmp_get(
    int unit,
    int fsrd_block,
    bcm_pbmp_t * fmac_bmp);

/**
 * \brief
 *   Enable/disable all FMACs and the coressponding FSRDs if possible blocks.
 *   The actual FSRD power down happens in the Portmode code
 *   Here is monitored if master FSRD is enabled/disabled per its corresponding FSRDs state 
 *   and the global variables allowing FSRD and FMAC access (block is valid)
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmacs_bmp -
 *   Bitmap of all FMACs to be powered up or down. FSRD pbmp is derived from the FMAC pbmp
 * \param [in] power_up -
 *   1:power up, 0:power down
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fmac_fsrd_block_power_set(
    int unit,
    bcm_pbmp_t fmacs_bmp,
    int power_up);

/**
 * \brief
 *   Initialize all FMAC FSRD blocks needed to power up.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fmac_fsrd_blocks_power_up_init(
    int unit);

/**
 * \brief
 *   Initialize all FMAC FSRD blocks needed to power down
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fmac_fsrd_blocks_power_down_init(
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
 *   force traffic to fabric
 * \param [in] unit -
 *   The unit number.
 * \param [in] enable -
 *   enable or disable
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_force_traffic_fabric(
    int unit,
    int enable);

/**
 * \brief
 *   force traffic to local core
 * \param [in] unit -
 *   The unit number.
 * \param [in] core -
 *   valid core number - enable for single core, if core == -1, disable for all cores.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_force_traffic_local(
    int unit,
    int core);

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

/**
 * \brief - Get fabric symbol error per frame counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] port     - logic port num.
 * \param [out] val     - symbol error
 *   
 * \return
 *   shr_error_e
 */
shr_error_e dnx_fabric_if_fmac_symb_error_get(
    int unit,
    bcm_port_t port,
    uint32 *val);

/**
 * \brief - Set all traffic to go through fabric
 * 
 * \param [in] unit     - unit id.
 * \param [in] enable   - enable
 *   
 * \return
 *   shr_error_e
 */
shr_error_e dnx_fabric_if_force_traffic_fabric_set(
    int unit,
    int enable);

/**
 * \brief - Get if all traffic is configured to go through fabric
 * 
 * \param [in] unit     - unit id.
 * \param [in] enable   - enable
 *   
 * \return
 *   shr_error_e
 */
shr_error_e dnx_fabric_if_force_traffic_fabric_get(
    int unit,
    int *enable);

    /**
 * \brief
 *   Convert bmp of FMACs to bmp of FSRDs.
 *   Return bitmap of FMACs related to the FSRD
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmac_bmp -
 *   bitmap of FMACs
 * \param [out] fsrd_bmp -
 *   bitmap of FMACs related to the FSRD
 * \param [in] return_only_full_fsrd -
 *   If set filters the FSRDs that doesn't have all of its FMACs provided
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None
 * \see
 *   None.
 */
shr_error_e dnx_fabric_if_fmac_bmp_to_fsrd_bmp_convert(
    int unit,
    bcm_pbmp_t fmac_bmp,
    bcm_pbmp_t * fsrd_bmp,
    int return_only_full_fsrd);

#endif /*_DNX_FABRIC_IF_H_INCLUDED_*/
