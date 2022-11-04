/*
 * \file src/bcm/dnx/vxlan/vxlan.h
 * Reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/vxlan.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

/** Internal learn type */
typedef enum
{
    DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_FEC_ONLY,
    DNX_VXLAN_LERAN_TYPE_NO_LIF_BASIC_SYM_LIF,
    NOF_DNX_VXLAN_LEARN_TYPES
} dnx_vxlan_learn_type_e;

/** verify is outlif exists */
#define _BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) (UTILEX_NUM2BOOL((_vxlan_port)->egress_tunnel_id != 0))

/* indicate if global out lif is symmetric with global in lif : global outlif = global inlif */
#define _BCM_VXLAN_PORT_HAS_SYMMETRIC_IN_LIF_WITH_OUT_LIF(_vxlan_port, _tunnel_term_gport_hw_resources, _tunnel_init_gport_hw_resources) \
                                                          (_BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) &&                                    \
                                                           ((_tunnel_term_gport_hw_resources)->global_in_lif ==                          \
                                                            (_tunnel_init_gport_hw_resources)->global_out_lif))

/* indicate that the destination is a FEC. */
#define _BCM_VXLAN_PORT_HAS_FEC(_vxlan_port) (UTILEX_NUM2BOOL(_vxlan_port->flags & BCM_VXLAN_PORT_EGRESS_TUNNEL))

int dnx_vxlan_port_tunnel_term_gport_hw_resources_get(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources);

int dnx_vxlan_port_gport_hw_resources_get(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources);

/**
 * \brief
 * Get the trap gport of the VXLAN VNI miss configuration
 *
 * \param [in] unit - Relevant unit
 * \param [out] trap_gport - gport of configured trap
 * \return
 *  * Error indication according to shr_error_e enum
 * \remark
 * None
 */
shr_error_e dnx_vxlan_vni_miss_get(
    int unit,
    int *trap_gport);

/**
 * \brief
 * Set the trap gport of the VXLAN VNI miss configuration
 *
 * \param [in] unit - Relevant unit
 * \param [in] trap_gport - gport of configured trap
 * \return
 *  * Error indication according to shr_error_e enum
 * \remark
 * None
 */
shr_error_e dnx_vxlan_vni_miss_set(
    int unit,
    int trap_gport);

/**
 * \brief - Initialize VXLAN module. \n
 * Set default value in termination profile label table.
 * \par DIRECT_INPUT:
 *     \param [in] unit - Unit-ID
 * \par INDIRECT INPUT:
 * \par DIRECT OUTPUT:
 *     shr_error_e
 */
shr_error_e dnx_vxlan_module_init(
    int unit);
