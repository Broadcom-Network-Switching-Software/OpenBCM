/*
 * \file src/bcm/dnx/vxlan/vxlan.h
 * Reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 Internal learn type
 */
typedef enum
{
    DNX_VXLAN_LEARN_TYPE_FEC_ASYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_FEC_SYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_PORT_SYM_OUTLIF,
    DNX_VXLAN_LEARN_TYPE_FEC_ONLY
} dnx_vxlan_learn_type_e;
