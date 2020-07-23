
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: Manages common MPLS functions
 */

#ifdef INCLUDE_L3

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/rate.h>
#include <bcm/ipmc.h>
#include <bcm/mpls.h>
#include <bcm/stack.h>
#include <bcm/topo.h>
#include <bcm/stat.h>

/*
 * Function:
 *      bcm_mpls_port_t_init
 * Purpose:
 *      Initialize the MPLS port struct
 * Parameters:
 *      port_info - Pointer to the struct to be init'ed
 */

void
bcm_mpls_port_t_init(bcm_mpls_port_t *port_info)
{
    if (port_info != NULL) {
        sal_memset(port_info, 0, sizeof(*port_info));

        port_info->port       = BCM_GPORT_INVALID;
        port_info->port_group = BCM_PORT_INVALID;
        port_info->match_vlan = BCM_VLAN_INVALID;
        port_info->match_inner_vlan = BCM_VLAN_INVALID;
        port_info->egress_service_vlan = BCM_VLAN_INVALID;
        port_info->egress_class_id = BCM_CLASS_ID_INVALID;
        port_info->pw_seq_number = 1;
        port_info->inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        bcm_mpls_egress_label_t_init(&port_info->egress_label);
        bcm_mpls_egress_label_t_init(&port_info->egress_tunnel_label);
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_egress_label_t_init
 * Purpose:
 *      Initialize MPLS egress label object struct.
 * Parameters:
 *      label - Pointer to MPLS egress label object struct.
 */

void
bcm_mpls_egress_label_t_init(bcm_mpls_egress_label_t *label)
{
    if (label != NULL) {
        sal_memset(label, 0, sizeof(*label));
        label->label = BCM_MPLS_LABEL_INVALID;
        label->outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        label->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        label->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_tunnel_switch_t_init
 * Purpose:
 *      Initialize MPLS tunnel switch object struct.
 * Parameters:
 *      switch - Pointer to MPLS tunnel switch object struct.
 */

void
bcm_mpls_tunnel_switch_t_init(bcm_mpls_tunnel_switch_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
        info->label = BCM_MPLS_LABEL_INVALID;
        info->action = BCM_MPLS_SWITCH_ACTION_INVALID;
        info->action_if_bos = info->action_if_not_bos = info->action;
        info->inlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

        bcm_mpls_egress_label_t_init(&info->egress_label);
        info->tunnel_if = BCM_IF_INVALID;
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_entropy_identifier_t_init 
 * Purpose:
 *      Initialize MPLS entropy identifier object struct.
 * Parameters:
 *      switch - Pointer to MPLS entropy identifier object struct.
 */

void
bcm_mpls_entropy_identifier_t_init(bcm_mpls_entropy_identifier_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
        info->label = BCM_MPLS_LABEL_INVALID;
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_exp_map_t_init
 * Purpose:
 *      Initialize MPLS EXP map object struct.
 * Parameters:
 *      exp_map - Pointer to MPLS EXP map object struct.
 */

void
bcm_mpls_exp_map_t_init(bcm_mpls_exp_map_t *exp_map)
{
    if (exp_map != NULL) {
        sal_memset(exp_map, 0, sizeof(*exp_map));
    }
    return;
}


/*
 * Function:
 *      bcm_mpls_vpn_config_t_init
 * Purpose:
 *      Initialize the MPLS VPN config structure
 * Parameters:
 *      info - Pointer to the struct to be init'ed
 */

void
bcm_mpls_vpn_config_t_init(bcm_mpls_vpn_config_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_range_action_t_init
 * Purpose:
 *      Initialize the MPLS Range action structure
 * Parameters:
 *      info - Pointer to the struct to be init'ed
 */

void
bcm_mpls_range_action_t_init(bcm_mpls_range_action_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_stat_info_t_init
 * Purpose:
 *      Initialize the MPLS tunnel Stat info structure
 * Parameters:
 *      stat_info - Pointer to the MPLS tunnel stat struct to be init'ed
 */

void
bcm_mpls_stat_info_t_init(bcm_mpls_stat_info_t *stat_info)
{
    if (stat_info != NULL) {
        sal_memset(stat_info, 0, sizeof(*stat_info));
    }
    return;
}
/*
 * Function:
 *      bcm_mpls_special_label_t_init
 * Purpose:
 *      Initialize the MPLS special label structure
 * Parameters:
 *      stat_info - Pointer to the MPLS special label struct to be init'ed
 */
void
bcm_mpls_special_label_t_init(bcm_mpls_special_label_t *label_info)
{
    if (label_info != NULL) {
        sal_memset(label_info, 0, sizeof(*label_info));

        label_info->label_value = BCM_MPLS_LABEL_INVALID;
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_special_label_push_element_init
 * Purpose:
 *      Initialize the MPLS special label element structure
 * Parameters:
 *      stat_info - Pointer to the MPLS special label element
 *                  struct to be init'ed
 */
void
bcm_mpls_special_label_push_element_t_init(
        bcm_mpls_special_label_push_element_t *element)
{
    if (element != NULL) {
        sal_memset(element, 0, sizeof(*element));

        element->gport = BCM_GPORT_INVALID;
    }
    return;
}

/*
 * Function:
 *      bcm_mpls_esi_info_t_init
 * Purpose:
 *      Initialize the ESI info structure
 * Parameters:
 *      esi_info - Pointer to the Esi info
 *                  struct to be init'ed
 */
void
bcm_mpls_esi_info_t_init(
        bcm_mpls_esi_info_t *esi_info)
{
    if (esi_info != NULL) {
        sal_memset(esi_info, 0, sizeof(*esi_info));

        esi_info->src_port = BCM_GPORT_INVALID;
    }
    return;
}

/*
 * Function:
 *     bcm_mpls_tunnel_encap_t_init
 * Purpose:
 *      Initialize the MPLS tunnel encap structure.
 * Parameters:
 *      tunnel_encap - Pointer to the MPLS tunnel encap struct to be init'ed.
 */
void
bcm_mpls_tunnel_encap_t_init(bcm_mpls_tunnel_encap_t *tunnel_encap)

{
    if (tunnel_encap != NULL) {
        sal_memset(tunnel_encap, 0, sizeof(*tunnel_encap));
    }
    return;
}

/*
 * Function:
 *     bcm_mpls_control_pkt_match_t_init
 * Purpose:
 *      Initialize the MPLS control packet match structure.
 * Parameters:
 *      match - Pointer to the MPLS control packet match struct to be init'ed.
 */
void
bcm_mpls_control_pkt_match_t_init(bcm_mpls_control_pkt_match_t  *match)
{
    if (match != NULL) {
        sal_memset(match, 0, sizeof(*match));
    }
    return;
}

/*
 * Function:
 *     bcm_mpls_control_pkt_action_t_init
 * Purpose:
 *      Initialize the MPLS control packet action structure.
 * Parameters:
 *      action - Pointer to the MPLS control packet action struct to be init'ed.
 */
void
bcm_mpls_control_pkt_action_t_init(bcm_mpls_control_pkt_action_t  *action)
{
    if (action != NULL) {
        sal_memset(action, 0, sizeof(*action));
    }
    return;
}

#else
typedef int _bcm_mpls_not_empty; /* Make ISO compilers happy. */

#endif  /* INCLUDE_L3 */
