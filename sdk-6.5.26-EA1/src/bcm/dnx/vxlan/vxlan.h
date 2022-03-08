/*
 * \file src/bcm/dnx/vxlan/vxlan.h
 * Reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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
    DNX_VXLAN_LERAN_TYPE_NO_LIF_BASIC_SYM_LIF
} dnx_vxlan_learn_type_e;

/** verify is outlif exists */
#define _BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) (UTILEX_NUM2BOOL((_vxlan_port)->egress_tunnel_id != 0))

/* indicate if global out lif is symmetric with global in lif : global outlif = global inlif */
#define _BCM_VXLAN_PORT_HAS_SYMMETRIC_IN_LIF_WITH_OUT_LIF(_vxlan_port, _tunnel_term_gport_hw_resources, _tunnel_init_gport_hw_resources) \
                                                          (_BCM_VXLAN_PORT_HAS_OUTLIF(_vxlan_port) &&                                    \
                                                           ((_tunnel_term_gport_hw_resources)->global_in_lif ==                          \
                                                            (_tunnel_init_gport_hw_resources)->global_out_lif))

int dnx_vxlan_port_gport_hw_resources_get(
    int unit,
    bcm_vxlan_port_t * vxlan_port,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_term_gport_hw_resources,
    dnx_algo_gpm_gport_hw_resources_t * tunnel_init_gport_hw_resources);
