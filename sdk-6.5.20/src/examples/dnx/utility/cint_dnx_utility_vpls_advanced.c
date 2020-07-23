/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_sand_utility_mpls.c Purpose: Various examples for MPLS. 
 */

/*
 *  
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_vpls_advanced.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = vpls_util_advanced_main(unit,in_port,out_port);
 * print rv; 
 * 
 * This cint can be used as VPLS utility cint to configure any VPLS setup.
 * This utility uses MPLS utility (cint_dnx_utility_mpls.c) to configure RIF, ARP, FEC and MPLS tunnels
 */

int vpls_util_advanced_verbose = 1;
int vpls_util_use_non_protected_vpls = 0;
int vpls_util_cw_present = 0;
/* indicates if cint parameters were initilized */
int vpls_util_advanced_params_set = 0;
int VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES = 2;
int VPLS_UTIL_ADVANCED_MAX_NOF_VPLS = 2;
int VPLS_UTIL_ADVANCED_MAX_NOF_L2_MAC_FWD = 2;
int VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_PORT = 2;
int VPLS_UTIL_ADVANCED_MAX_NOF_DUMMY = 20;
int VPLS_UTIL_ADVANCED_MAX_NOF_NWK_GROUPS = 49;
int VPLS_UTIL_ADVANCED_MAX_NOF_TPIDS = 4;
int VPLS_UTIL_ADVANCED_MAX_NOF_IVE_EVE = 4;
int VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_EDITS = 4;

int vpls_util_advanced_vpn;
bcm_mac_t vpls_util_advanced_mac1 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
bcm_mac_t vpls_util_advanced_mac2 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
bcm_mac_t vpls_util_advanced_mac3 = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
bcm_mac_t vpls_util_advanced_mac4 = { 0x00, 0x11, 0x00, 0x00, 0x01, 0x12 };

/* dummy variables used for NULL handling */
int dum_idx = 0;
int dummy_params[VPLS_UTIL_ADVANCED_MAX_NOF_DUMMY] = {0};

/* override MPLS globals */
MPLS_UTIL_MAX_NOF_ENTITIES = 2;


struct vpls_util_advanced_mpls_port_add_s
{
    bcm_gport_t *mpls_port_id;   /* GPORT identifier. */
    uint32 flags;               /* BCM_MPLS_PORT_xxx. */
    uint32 flags2;              /* BCM_MPLS_PORT2_xxx. */
    bcm_if_t *encap_id;          /* Encap Identifier. */
    bcm_mpls_label_t label;
    uint32 label_flags;         /* BCM_MPLS_EGRESS_LABEL_xxx flags related to the label */
    bcm_mpls_port_match_t criteria;     /* Match criteria. for ingress */
    bcm_mpls_label_t match_label;       /* VC label to match. */
    bcm_if_t *egress_tunnel_if;  /* MPLS tunnel egress object. */
    int *forwarding_domain;
    int qos_map_id;             /* QOS map identifier. */
    bcm_qos_ingress_model_t ingress_qos_model;   /*ingress qos model*/
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    int nwk_group_valid;             /* indication to whether nwd group is present */
    int pwe_nwk_group_id;             /* pwe network group id */
    bcm_mpls_port_control_channel_type_t vccv_type; /* Indicate VCCV Control Channel */
    bcm_failover_t  failover_id;        /* Failover ID */
    bcm_multicast_t  failover_mc_group; /* Failover ID Multicast group ID*/
    bcm_gport_t failover_port_id;       /* Failover Port ID */
    int nof_service_tags;               /* Number of expected VLAN tags in inner ETH */
    bcm_qos_egress_model_t egress_qos_model; /*egress qos model*/
};

struct vpls_util_advanced_l2_mac_fwd_s
{
    int valid;
    bcm_vlan_t vid;                     /* VLAN identifier. */
    bcm_mac_t mac_addr;         /* mac address used for forwarding */
    bcm_gport_t *gport;
    int *encap_id;                       /* out logical interface */
    uint32 stat_id;                     /* Object statistics ID */
    int stat_pp_profile;                /* Statistics profile */
    uint32 flags;                       /* BCM_L2_xxx flags. */
    uint32 flags2;                      /* BCM_L2_FLAGS2_xxx flags. */
};

struct vpls_util_advanced_network_group_config_s
{
    int valid;
    bcm_switch_network_group_t src;     /* source Split Horizon Destination Network Group Identifier */
    bcm_switch_network_group_t dest;    /* destination Split Horizon Destination Network Group Identifier */
    uint32 flags;                       /* BCM_SWITCH_NETWORK_GROUP_XXX flags */
};

struct vpls_util_advanced_port_tpid_class_s
{
    int valid;
    uint32 class_id;                            /* used for PWE's vlan domain (network layer domain) */
    uint32 flags;                       /* BCM_PORT_TPID_CLASS_XXX */
    bcm_gport_t *port;                   /* Port ID */
    uint32 tpid1;                       /* First TPID value, -1 for invalid */
    uint32 tpid2;                       /* Second TPID value, -1 for invalid */
    bcm_port_tag_format_class_t tag_format_class_id; /* VLAN tag format ID */
    int vlan_translation_action_id;     /* Default translation action ID */
};

struct vpls_util_advanced_vlan_port_s
{
    bcm_vlan_port_match_t criteria;     /* Match criteria. */
    uint32 flags;                       /* BCM_VLAN_PORT_xxx. */
    int *vsi;                           /* Populated for bcm_vlan_port_find only */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match. */
    bcm_gport_t *port;                   /* Gport: local or remote Physical or logical gport. */
    bcm_vlan_t egress_vlan;             /* Egress Outer VLAN or SD-TAG VLAN ID. */
    bcm_if_t encap_id;                  /* Encapsulation Index. */
    bcm_gport_t *vlan_port_id;          /* GPORT identifier */
    int nwk_group_valid;                /* indication to whether nwd group is present */
    bcm_switch_network_group_t ingress_network_group_id; /* Split Horizon ingress network group identifier */
    bcm_switch_network_group_t egress_network_group_id; /* Split Horizon egress network group identifier */
    bcm_gport_t *tunnel_id;              /* Pointer from Native AC to next entry in EEDB list. */
};

struct vpls_util_advanced_port_match_info_s 
{
    bcm_port_match_t match;             /* Match criteria */
    bcm_gport_t *port;                   /* Match port */
    bcm_vlan_t match_vlan;              /* Outer VLAN ID to match */
    bcm_vlan_t match_inner_vlan;        /* Inner VLAN ID to match */
    uint32 flags;                       /* BCM_PORT_MATCH_XXX flags */
};

struct vpls_util_advanced_vlan_translate_ive_eve_translation_s
{
    bcm_gport_t *lif;
    int outer_tpid;
    int inner_tpid;
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    bcm_vlan_t new_outer_vid;
    bcm_vlan_t new_inner_vid;
    uint32 vlan_edit_profile;
    uint16 tag_format;
    uint8 is_ive;
};

struct vpls_util_advanced_vlan_edit_s
{
    int valid;
    vpls_util_advanced_vlan_port_s vlan;
    vpls_util_advanced_port_match_info_s port_match;
    vpls_util_advanced_vlan_translate_ive_eve_translation_s ve_translate;
};

struct vpls_util_advanced_entity_s
{
    vpls_util_advanced_mpls_port_add_s vpls_ingress[VPLS_UTIL_ADVANCED_MAX_NOF_VPLS];
    vpls_util_advanced_mpls_port_add_s vpls_egress[VPLS_UTIL_ADVANCED_MAX_NOF_VPLS];
    vpls_util_advanced_vlan_port_s vlan_ports[VPLS_UTIL_ADVANCED_MAX_NOF_L2_MAC_FWD];
    vpls_util_advanced_l2_mac_fwd_s l2_macs[VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_PORT];
    vpls_util_advanced_network_group_config_s nwk_groups[VPLS_UTIL_ADVANCED_MAX_NOF_NWK_GROUPS];
    vpls_util_advanced_port_tpid_class_s tpids[VPLS_UTIL_ADVANCED_MAX_NOF_TPIDS];
    vpls_util_advanced_vlan_edit_s vlan_edits[VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_EDITS];
};

vpls_util_advanced_entity_s vpls_util_entity[VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES];

void
vpls_util_advanced_vlan_edit_s_init(
    vpls_util_advanced_vlan_edit_s *vlan_edit,
    int nof)
{
    int idx;

    for(idx = 0; idx < nof; idx++)
    {
        vlan_edit[idx].valid = 0;
        vlan_edit[idx].port_match.match = BCM_PORT_MATCH_INVALID;
        vlan_edit[idx].vlan.criteria = BCM_VLAN_PORT_MATCH_INVALID;
    }
}

void
vpls_util_advanced_mpls_port_add_s_init(
    vpls_util_advanced_mpls_port_add_s * port_info,
    int nof)
{
    int idx = 0;

    for(idx = 0; idx < nof; idx++)
    {
        port_info->criteria = BCM_MPLS_PORT_MATCH_INVALID;
        port_info->flags = 0;
        port_info->flags2 = 0;
        port_info->label = BCM_MPLS_LABEL_INVALID;
        port_info->label_flags = 0;
        port_info->match_label = BCM_MPLS_LABEL_INVALID;
        port_info->qos_map_id = 0;
        port_info->encap_access = bcmEncapAccessInvalid;
        port_info->vccv_type = bcmMplsPortControlChannelNone;
        port_info->failover_id = 0;
        port_info->failover_mc_group = 0;
        port_info->failover_port_id = 0;
        port_info->nof_service_tags = 0;
    }
}

void
vpls_util_advanced_network_group_config_s_init(
    vpls_util_advanced_network_group_config_s *groups,
    int nof)
{
    int idx;

    for(idx = 0; idx < nof; idx++)
    {
        groups[idx].valid = 0;
    }
}

void
vpls_util_advanced_vlan_port_s_init(
    vpls_util_advanced_vlan_port_s * vlan_ports,
    int nof)
{
    int idx;

    for(idx = 0; idx < nof; idx++)
    {
        vlan_ports[idx].criteria = BCM_VLAN_PORT_MATCH_INVALID;
    }
}

void
vpls_util_advanced_port_tpid_class_s_init(
    vpls_util_advanced_port_tpid_class_s * tpids,
    int nof)
{
    int idx;

    for(idx = 0; idx < nof; idx++)
    {
        tpids[idx].valid = 0;
        tpids[idx].tpid1 = 0;
        tpids[idx].tpid2 = 0;
    }
}

void
vpls_util_advanced_l2_mac_fwd_s_init(
    vpls_util_advanced_l2_mac_fwd_s * l2_fwd,
    int nof)
{
    int idx;

    for(idx = 0; idx < nof; idx++)
    {
        l2_fwd[idx].valid = 0;
    }
}

void
vpls_util_advanced_entity_s_init(
    vpls_util_advanced_entity_s* entity,
    int nof)
{
    int idx = 0;

    for(idx = 0; idx < nof; idx++)
    {
        vpls_util_advanced_mpls_port_add_s_init(entity[idx].vpls_egress, VPLS_UTIL_ADVANCED_MAX_NOF_VPLS);
        vpls_util_advanced_mpls_port_add_s_init(entity[idx].vpls_ingress, VPLS_UTIL_ADVANCED_MAX_NOF_VPLS);
        vpls_util_advanced_vlan_port_s_init(entity[idx].vlan_ports, VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_PORT);
        vpls_util_advanced_l2_mac_fwd_s_init(entity[idx].l2_macs, VPLS_UTIL_ADVANCED_MAX_NOF_L2_MAC_FWD);
        vpls_util_advanced_network_group_config_s_init(entity[idx].nwk_groups, VPLS_UTIL_ADVANCED_MAX_NOF_NWK_GROUPS);
        vpls_util_advanced_port_tpid_class_s_init(entity[idx].tpids, VPLS_UTIL_ADVANCED_MAX_NOF_TPIDS);
        vpls_util_advanced_vlan_edit_s_init(entity[idx].vlan_edits, VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_EDITS);
    }
}

int
init_default_vpls_util_advanced_params(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char *proc_name = "init_default_vpls_util_advanced_params";
    int ii, jj;

    if(vpls_util_advanced_verbose)
    {
        printf("%s(): ENTER. port1 %d, port2 %d \r\n", proc_name, port1, port2);
    }

    vpls_util_advanced_vpn = 5;

    mpls_util_s_init(mpls_util_entity, MPLS_UTIL_MAX_NOF_ENTITIES);

    /*
     * l2, mac, arp, fec parameters
     */
    mpls_util_entity[0].ports[0].port = port1;
    mpls_util_entity[0].ports[0].eth_rif_id = &vpls_util_advanced_vpn;

    mpls_util_entity[1].ports[0].port = port2;
    mpls_util_entity[1].ports[0].eth_rif_id = &mpls_util_entity[1].rifs[0].intf;
    /***************************************/
    mpls_util_entity[1].rifs[0].intf = 40;
    mpls_util_entity[1].rifs[0].mac_addr = vpls_util_advanced_mac2;
    mpls_util_entity[1].rifs[0].vrf = 100;
    /***************************************/
    mpls_util_entity[1].arps[0].arp = 0;
    mpls_util_entity[1].arps[0].next_hop = vpls_util_advanced_mac1;
    mpls_util_entity[1].arps[0].intf = &mpls_util_entity[1].rifs[0].intf;
    /***************************************/
    mpls_util_entity[0].fecs[0].fec_id = 0;
    mpls_util_entity[0].fecs[0].port = &mpls_util_entity[1].ports[0].port;
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    mpls_util_entity[0].fecs[0].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    /***************************************/
    /* encapsulation entry */    
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = 3333;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[1].arps[0].arp;
    mpls_util_entity[0].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;

    /* termination entry */
    mpls_util_entity[1].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_util_entity[1].mpls_switch_tunnel[0].label = mpls_util_entity[0].mpls_encap_tunnel[0].label[0];
    mpls_util_entity[1].mpls_switch_tunnel[0].vrf = 100;
    /***************************************/

    mpls_params_set = 1;

    /* 
    * configure VPLS related params
    */
    vpls_util_advanced_entity_s_init(vpls_util_entity, VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES);
    /* PWE tunnel to be encapsulated parameters */
    vpls_util_entity[0].vpls_egress[0].flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    vpls_util_entity[0].vpls_egress[0].flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    vpls_util_entity[0].vpls_egress[0].label = 3456;
    vpls_util_entity[0].vpls_egress[0].label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    vpls_util_entity[0].vpls_egress[0].encap_access = bcmEncapAccessTunnel1;
    vpls_util_entity[0].vpls_egress[0].mpls_port_id = &dummy_params[dum_idx++];
    vpls_util_entity[0].vpls_egress[0].encap_id = &dummy_params[dum_idx++];
    if (vpls_util_use_non_protected_vpls)
    {
        vpls_util_entity[0].vpls_egress[0].egress_tunnel_if = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    }

    /* PWE tunnel to be terminated parameters */
    vpls_util_entity[1].vpls_ingress[0].criteria = BCM_MPLS_PORT_MATCH_LABEL;
    vpls_util_entity[1].vpls_ingress[0].encap_id = vpls_util_entity[0].vpls_egress[0].encap_id;
    vpls_util_entity[1].vpls_ingress[0].flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    vpls_util_entity[1].vpls_ingress[0].flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    vpls_util_entity[1].vpls_ingress[0].match_label = vpls_util_entity[0].vpls_egress[0].label;
    vpls_util_entity[1].vpls_ingress[0].egress_tunnel_if = &mpls_util_entity[0].fecs[0].fec_id;
    vpls_util_entity[1].vpls_ingress[0].forwarding_domain = &vpls_util_advanced_vpn;
    vpls_util_entity[1].vpls_ingress[0].mpls_port_id = &dummy_params[dum_idx++];
    vpls_util_entity[1].vpls_ingress[0].encap_id = vpls_util_entity[0].vpls_egress[0].encap_id;

    vpls_util_entity[0].l2_macs[0].valid = 1;
    vpls_util_entity[0].l2_macs[0].mac_addr = vpls_util_advanced_mac3;
    vpls_util_entity[0].l2_macs[0].flags |= BCM_L2_STATIC;
    vpls_util_entity[0].l2_macs[0].gport = vpls_util_entity[1].vpls_ingress[0].mpls_port_id;
    vpls_util_entity[0].l2_macs[0].vid = vpls_util_advanced_vpn;

    vpls_util_entity[1].vlan_ports[0].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vpls_util_entity[1].vlan_ports[0].port = &mpls_util_entity[0].ports[0].port;
    vpls_util_entity[1].vlan_ports[0].vsi = &vpls_util_advanced_vpn;
    vpls_util_entity[1].vlan_ports[0].flags = 0;
    vpls_util_entity[1].vlan_ports[0].match_vlan = vpls_util_advanced_vpn;
    vpls_util_entity[1].vlan_ports[0].egress_vlan = vpls_util_advanced_vpn;
    vpls_util_entity[1].vlan_ports[0].vlan_port_id = &dummy_params[dum_idx++];

    vpls_util_entity[1].l2_macs[0].valid = 1;
    vpls_util_entity[1].l2_macs[0].mac_addr = vpls_util_advanced_mac4;
    vpls_util_entity[1].l2_macs[0].flags |= BCM_L2_STATIC;
    vpls_util_entity[1].l2_macs[0].gport = vpls_util_entity[1].vlan_ports[0].vlan_port_id;
    vpls_util_entity[1].l2_macs[0].vid = vpls_util_advanced_vpn;

    vpls_util_advanced_params_set = 1;

    if(vpls_util_advanced_verbose)
    {
        printf("%s(): EXIT. port1 %d, port2 %d \r\n", proc_name, port1, port2);
    }
    return rv;
}

int
vpls_util_advanced_tag_formats_set(
    int unit,
    vpls_util_advanced_port_tpid_class_s* tpids,
    int nof_tpids)
{
    int rv = BCM_E_NONE;
    bcm_switch_tpid_info_t tpid_info;
    bcm_port_tpid_class_t port_tpid_class;
    int tpid_idx;
    char *proc_name = "vpls_util_advanced_tag_formats_set";

    for(tpid_idx = 0; tpid_idx < nof_tpids; tpid_idx++)
    {
        if(tpids[tpid_idx].valid)
        {
            /* Set port's vlan domain (network layer domain). */
            rv = bcm_port_class_set(unit, *tpids[tpid_idx].port, bcmPortClassIngress, tpids[tpid_idx].class_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_class_set, port=0x%x, class_id = %d\n", *tpids[tpid_idx].port, tpids[tpid_idx].class_id);
                return rv;
            }

            /* add TPID to global array */
            if(tpids[tpid_idx].tpid1)
            {
                rv = tpid__tpid_add(unit, tpids[tpid_idx].tpid1);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, tpid__tpid_add for 0x$x\n", tpids[tpid_idx].tpid1);
                    return rv;
                }
            }

            if(tpids[tpid_idx].tpid2)
            {
                /* add TPID to global array */
                rv = tpid__tpid_add(unit, tpids[tpid_idx].tpid2);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, tpid__tpid_add for 0x%x\n", tpids[tpid_idx].tpid2);
                    return rv;
                }
            }

            bcm_port_tpid_class_t_init(&port_tpid_class);
            port_tpid_class.port = *tpids[tpid_idx].port;
            port_tpid_class.tpid1 = tpids[tpid_idx].tpid1;
            port_tpid_class.tpid2 = tpids[tpid_idx].tpid2;
            port_tpid_class.tag_format_class_id = tpids[tpid_idx].tag_format_class_id;
            port_tpid_class.flags = tpids[tpid_idx].flags;
            port_tpid_class.vlan_translation_action_id = tpids[tpid_idx].vlan_translation_action_id;
            rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_port_tpid_class_set, port = 0x%x, \n", *tpids[tpid_idx].port);
                return rv;
            }

            if(vpls_util_advanced_verbose)
            {
                printf("%s(): tpid_idx = %d, port = 0x%x, tpid1 = 0x%x, tpid2 = 0x%x, tag_format_class_id = %d, flags = 0x%x, vlan_translation_action_id = %d\n",
                    proc_name, tpid_idx, port_tpid_class.port, port_tpid_class.tpid1, port_tpid_class.tpid2, port_tpid_class.tag_format_class_id, port_tpid_class.flags, port_tpid_class.vlan_translation_action_id);
            }
        }
    }

    return rv;
}

int
vpls_util_advanced_vlan_editing_config(
    int unit,
    vpls_util_advanced_vlan_edit_s *vlan_edits,
    int nof_vlan)
{
    int rv = BCM_E_NONE;
    int vlan_idx;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    char *proc_name = "vpls_util_advanced_vlan_editing_config";
    bcm_gport_t vlan_gport;
    int print_val_1, print_val_2;

    for(vlan_idx = 0; vlan_idx < nof_vlan; vlan_idx++)
    {
        if(vlan_edits[vlan_idx].valid)
        {
             printf("%s(): vlan_idx = %d\n", proc_name, vlan_idx);
            /* add port. */
            if(vlan_edits[vlan_idx].vlan.criteria != BCM_VLAN_PORT_MATCH_INVALID)
            {
                bcm_vlan_port_t_init(&vlan_port);
                vlan_port.flags = vlan_edits[vlan_idx].vlan.flags;
                if (vlan_edits[vlan_idx].vlan.nwk_group_valid)
                {
                    vlan_port.ingress_network_group_id = vlan_edits[vlan_idx].vlan.ingress_network_group_id;
                    vlan_port.egress_network_group_id = vlan_edits[vlan_idx].vlan.egress_network_group_id;
                }
                if(vlan_edits[vlan_idx].vlan.vsi)
                {
                    vlan_port.vsi = *vlan_edits[vlan_idx].vlan.vsi;
                }
                vlan_port.criteria = vlan_edits[vlan_idx].vlan.criteria;
                if(vlan_edits[vlan_idx].vlan.tunnel_id)
                {
                    vlan_port.tunnel_id = *vlan_edits[vlan_idx].vlan.tunnel_id;
                }
                if(vlan_edits[vlan_idx].vlan.port)
                {
                    vlan_port.port = *vlan_edits[vlan_idx].vlan.port;
                }
                if(vlan_edits[vlan_idx].vlan.flags & BCM_VLAN_PORT_WITH_ID)
                {
                    if(BCM_GPORT_SUB_TYPE_IS_LIF(*vlan_edits[vlan_idx].vlan.vlan_port_id))
                    {
                        vlan_gport = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(*vlan_edits[vlan_idx].vlan.vlan_port_id);
                        BCM_GPORT_SUB_TYPE_LIF_SET(vlan_gport, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, vlan_gport);
                        BCM_GPORT_VLAN_PORT_ID_SET(vlan_gport, vlan_gport);
                    }
                    else
                    {
                        vlan_gport = *vlan_edits[vlan_idx].vlan.vlan_port_id;
                    }
                    vlan_port.vlan_port_id = vlan_gport;
                }
                rv = bcm_vlan_port_create(unit, &vlan_port);
                if (rv != BCM_E_NONE)
                {
                    printf("Error(%d), in bcm_vlan_port_create\n", rv);
                    return rv;
                }
                /* Save the created vlan_port_id */
                *vlan_edits[vlan_idx].vlan.vlan_port_id = vlan_port.vlan_port_id;
                vlan_edits[vlan_idx].vlan.encap_id = vlan_port.encap_id;

                if(vpls_util_advanced_verbose)
                {
                    print_val_1 = vlan_port.criteria;
                    printf("    VLAN_PORT - vlan_port_id = 0x%x, encap_id = 0x%x, flags = 0x%x, criteria = %d, vsi =%d, port = 0x%x, tunnel_id = 0x%x\n",
                        vlan_port.vlan_port_id, vlan_edits[vlan_idx].vlan.encap_id, vlan_port.flags, print_val_1, vlan_port.vsi, vlan_port.port, vlan_port.tunnel_id);
                }
            }

            /* Add Match */
            if(vlan_edits[vlan_idx].port_match.match != BCM_PORT_MATCH_INVALID)
            {
                bcm_port_match_info_t_init(&match_info);
                match_info.flags = vlan_edits[vlan_idx].port_match.flags;
                match_info.port = *vlan_edits[vlan_idx].port_match.port;
                match_info.match = vlan_edits[vlan_idx].port_match.match;
                match_info.match_vlan = vlan_edits[vlan_idx].port_match.match_vlan;
                match_info.match_inner_vlan = vlan_edits[vlan_idx].port_match.match_inner_vlan;
                rv = bcm_port_match_add(unit, *vlan_edits[vlan_idx].vlan.vlan_port_id, &match_info);
                if (rv != BCM_E_NONE)
                {
                    printf("Error(%d), in bcm_port_match_add\n", rv);
                    return rv;
                }

                if(vpls_util_advanced_verbose)
                {
                    print_val_1 = match_info.match;
                    printf("    MATCH_INFO - port = 0x%x, match_vlan = 0x%x, match_inner_vlan = 0x%x, match = %d, flags = 0x%x\n",
                        match_info.port, match_info.match_vlan, match_info.match_inner_vlan, print_val_1, match_info.flags);
                }
            }

            /* Configure IVE */
            rv = vlan_translate_ive_eve_translation_set(unit, *vlan_edits[vlan_idx].vlan.vlan_port_id,
                                                   vlan_edits[vlan_idx].ve_translate.outer_tpid, vlan_edits[vlan_idx].ve_translate.inner_tpid, 
                                                   vlan_edits[vlan_idx].ve_translate.outer_action, vlan_edits[vlan_idx].ve_translate.inner_action,
                                                   vlan_edits[vlan_idx].ve_translate.new_outer_vid, vlan_edits[vlan_idx].ve_translate.new_inner_vid,
                                                   vlan_edits[vlan_idx].ve_translate.vlan_edit_profile, vlan_edits[vlan_idx].ve_translate.tag_format,vlan_edits[vlan_idx].ve_translate.is_ive);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n", *vlan_edits[vlan_idx].vlan.vlan_port_id);
                return rv;
            }

            if(vpls_util_advanced_verbose)
            {
                print_val_1 = vlan_edits[vlan_idx].ve_translate.inner_action;
                print_val_2 = vlan_edits[vlan_idx].ve_translate.outer_action;
                printf("    IVE_EVE - vlan_port_id = 0x%x, outer_tpid = 0x%x, inner_tpid = 0x%x, outer_action = %d, inner_action = %d, new_outer_vid = 0x%x, new_inner_vid = 0x%x, vlan_edit_profile = %d, tag_format = %d, is_ive = %d\n",
                    *vlan_edits[vlan_idx].vlan.vlan_port_id, vlan_edits[vlan_idx].ve_translate.outer_tpid, vlan_edits[vlan_idx].ve_translate.inner_tpid, print_val_2, print_val_1, vlan_edits[vlan_idx].ve_translate.new_outer_vid, vlan_edits[vlan_idx].ve_translate.new_inner_vid, vlan_edits[vlan_idx].ve_translate.vlan_edit_profile, vlan_edits[vlan_idx].ve_translate.tag_format, vlan_edits[vlan_idx].ve_translate.is_ive);
            }
        }
    }

    return rv;
}

int
vpls_util_advanced_config_networt_group_id(
    int unit,
    vpls_util_advanced_network_group_config_s *groups,
    int nof_groups)
{
    int rv = BCM_E_NONE;
    bcm_switch_network_group_config_t config;
    int group_idx;
    char *proc_name = "vpls_util_advanced_config_networt_group_id";

    for(group_idx = 0; group_idx < nof_groups; group_idx++)
    {
        if(groups[group_idx].valid)
        {
            bcm_switch_network_group_config_t_init(&config);
            config.config_flags = groups[group_idx].flags;
            config.dest_network_group_id = groups[group_idx].dest;
            rv = bcm_switch_network_group_config_set(unit, groups[group_idx].src, &config);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
                return rv;
            }

            if(vpls_util_advanced_verbose)
            {
                printf("%s(): group_idx = %d, src(%d) -> dest(%d), flags = 0x%x\n", proc_name, group_idx, groups[group_idx].src, groups[group_idx].dest, groups[group_idx].flags);
            }
        }
    }

    return rv;
}

int
vpls_util_advanced_mpls_port_add_encapsulation(
    int unit,
    vpls_util_advanced_mpls_port_add_s * mpls_port_encap,
    int nof_vpls_encap)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int vpls_idx;
    char *proc_name = "vpls_util_advanced_mpls_port_add_encapsulation";

    for(vpls_idx = 0; vpls_idx < nof_vpls_encap; vpls_idx++)
    {
        if(mpls_is_valid_lable(mpls_port_encap[vpls_idx].label))
        {
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.flags = mpls_port_encap[vpls_idx].flags;
            if (vpls_util_cw_present)
            {
                mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
            }
            mpls_port.flags2 = mpls_port_encap[vpls_idx].flags2;
            if(mpls_port_encap[vpls_idx].encap_id)
            {
                mpls_port.encap_id = *mpls_port_encap[vpls_idx].encap_id;
            }
            if(mpls_port_encap[vpls_idx].mpls_port_id)
            {
                BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, *mpls_port_encap[vpls_idx].mpls_port_id);
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
            }
            mpls_port.egress_label.label = mpls_port_encap[vpls_idx].label;
            mpls_port.egress_label.flags = mpls_port_encap[vpls_idx].label_flags;
            mpls_port.egress_label.encap_access = mpls_port_encap[vpls_idx].encap_access;
            mpls_port.egress_label.qos_map_id = mpls_port_encap[vpls_idx].qos_map_id;
            mpls_port.egress_label.egress_qos_model.egress_qos = mpls_port_encap[vpls_idx].egress_qos_model.egress_qos;
            mpls_port.egress_label.egress_qos_model.egress_ttl = mpls_port_encap[vpls_idx].egress_qos_model.egress_ttl;
            if (vpls_util_use_non_protected_vpls)
            {
                mpls_port.egress_tunnel_if = *(mpls_port_encap[vpls_idx].egress_tunnel_if);
            }
            if (mpls_port_encap[vpls_idx].nwk_group_valid)
            {
                mpls_port.network_group_id = mpls_port_encap[vpls_idx].pwe_nwk_group_id;
            }

            rv = bcm_mpls_port_add(unit, 0, mpls_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
                return rv;
            }

            *mpls_port_encap[vpls_idx].mpls_port_id = mpls_port.mpls_port_id;
            *mpls_port_encap[vpls_idx].encap_id = mpls_port.encap_id;

            if(vpls_util_advanced_verbose)
            {
                printf("%s(): vpls_idx = %d, egress_mpls_port_id = 0x%x, encap_id = 0x%x, flags = 0x%x, flags2 = 0x%x, egress_label = 0x%x, egress_label_flags = 0x%x\n",
                    proc_name, vpls_idx, mpls_port.mpls_port_id, mpls_port.encap_id, mpls_port.flags, mpls_port.flags2, mpls_port.egress_label.label, mpls_port.egress_label.flags);
            }
        }
    }

    return rv;
}

int
vpls_util_advanced_mpls_port_add_termination(
    int unit,
    vpls_util_advanced_mpls_port_add_s * port_term,
    int nof_vpls_term)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int vpls_idx;
    char *proc_name = "vpls_util_advanced_mpls_port_add_termination";
    int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));

    for(vpls_idx = 0; vpls_idx < nof_vpls_term; vpls_idx++)
    {
        if(mpls_is_valid_lable(port_term[vpls_idx].match_label))
        {
            bcm_mpls_port_t_init(&mpls_port);
            mpls_port.flags = port_term[vpls_idx].flags;
            if (vpls_util_cw_present)
            {
                mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
            }
            mpls_port.flags2 = port_term[vpls_idx].flags2;
            if (!is_jericho2_mode) {
                mpls_port.flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP_EEI;
            }
            mpls_port.ingress_qos_model.ingress_phb = port_term[vpls_idx].ingress_qos_model.ingress_phb;
            mpls_port.ingress_qos_model.ingress_remark = port_term[vpls_idx].ingress_qos_model.ingress_remark;
            mpls_port.ingress_qos_model.ingress_ttl = port_term[vpls_idx].ingress_qos_model.ingress_ttl;
            if(port_term[vpls_idx].mpls_port_id)
            {
                BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, *port_term[vpls_idx].mpls_port_id);
                BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
            }
            /*
             * encap_id is the egress outlif - used for learning 
             */
            if(port_term[vpls_idx].encap_id)
            {
                mpls_port.encap_id = *port_term[vpls_idx].encap_id;
            }
            mpls_port.criteria = port_term[vpls_idx].criteria;
            mpls_port.match_label = port_term[vpls_idx].match_label;
            mpls_port.nof_service_tags = port_term[vpls_idx].nof_service_tags;

            if (port_term[vpls_idx].nwk_group_valid)
            {
                mpls_port.network_group_id = port_term[vpls_idx].pwe_nwk_group_id;
            }

            /** Set protection parameters if we have VPLS Ingress 1+1 Protection */
            mpls_port.ingress_failover_id = port_term[vpls_idx].failover_id;
            mpls_port.ingress_failover_port_id = port_term[vpls_idx].failover_port_id;
            /** connect PWE entry to created Failover MC group for learning */
            mpls_port.failover_mc_group = port_term[vpls_idx].failover_mc_group;

            /* 
             * In the case of Ingres protection the egress_interface_if must be 0.
             * The learning information is taken from the failover_mc_group
             */
            if ((port_term[vpls_idx].failover_id == 0) && (port_term[vpls_idx].egress_tunnel_if))
            {
                /** connect PWE entry to created MPLS encapsulation entry for learning */
                BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, *port_term[vpls_idx].egress_tunnel_if);
            }

            mpls_port.vccv_type = port_term[vpls_idx].vccv_type;

            rv = bcm_mpls_port_add(unit, *port_term[vpls_idx].forwarding_domain, mpls_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
                return rv;
            }

            *port_term[vpls_idx].mpls_port_id = mpls_port.mpls_port_id;
            
            if(vpls_util_advanced_verbose)
            {
                printf("%s(): vpls_idx = %d, ingress_mpls_port_id = 0x%x, match_label = 0x%x, flags = 0x%x, flags2 = 0x%x, encap_id = 0x%x, egress_tunnel_if = 0x%x, match_criteria = %d\n",
                    proc_name, vpls_idx, mpls_port.mpls_port_id, mpls_port.match_label, mpls_port.flags, mpls_port.flags2, mpls_port.encap_id, mpls_port.egress_tunnel_if, mpls_port.egress_tunnel_if, mpls_port.criteria);
            }
        }
    }

    return rv;
}

int
vpls_util_advanced_l2_addr_add(
    int unit,
    vpls_util_advanced_l2_mac_fwd_s *l2_macs,
    int nof_l2)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;
    char *proc_name = "vpls_util_advanced_l2_addr_add";
    int l2_idx = 0;

    for(l2_idx = 0; l2_idx < nof_l2; l2_idx++)
    {
        if(l2_macs[l2_idx].valid)
        {
            bcm_l2_addr_t_init(&l2addr, l2_macs[l2_idx].mac_addr, l2_macs[l2_idx].vid);
            l2addr.flags = l2_macs[l2_idx].flags;
            l2addr.flags2 = l2_macs[l2_idx].flags2;
            if(l2_macs[l2_idx].gport)
            {
                l2addr.port = *l2_macs[l2_idx].gport;
            }
            if(l2_macs[l2_idx].encap_id)
            {
                l2addr.encap_id= *l2_macs[l2_idx].encap_id;
            }
            l2addr.stat_id = l2_macs[l2_idx].stat_id;
            l2addr.stat_pp_profile = l2_macs[l2_idx].stat_pp_profile;

            rv = bcm_l2_addr_add(unit, l2addr);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), in bcm_l2_addr_add\n", rv);
                return rv;
            }

            if(vpls_util_advanced_verbose)
            {
                printf("%s(): l2_idx = %d, vid = %d, mac = ", proc_name, l2_idx, l2_macs[l2_idx].vid);
                _l2_print_mac(l2_macs[l2_idx].mac_addr);
                printf(", flags = 0x%x, flags2 = 0x%x, port = 0x%x, encap_id = 0x%x\n", l2_macs[l2_idx].flags, l2_macs[l2_idx].flags2, l2addr.port, l2addr.encap_id);
            }
        }
    }

    return rv;
}

int
vpls_util_advanced_vlan_port_config(
    int unit,
    vpls_util_advanced_vlan_port_s *vlans,
    int nof_vlan)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    char *proc_name = "vpls_util_advanced_vlan_port_config";
    int vlan_idx;

    for(vlan_idx = 0; vlan_idx < nof_vlan; vlan_idx++)
    {
        if(vlans[vlan_idx].criteria != BCM_VLAN_PORT_MATCH_INVALID)
        {
            bcm_vlan_port_t_init(&vlan_port);
            if(vlans[vlan_idx].vlan_port_id)
            {
                vlan_port.vlan_port_id = *vlans[vlan_idx].vlan_port_id;
            }
            vlan_port.criteria = vlans[vlan_idx].criteria;
            if(vlans[vlan_idx].port)
            {
                vlan_port.port = *vlans[vlan_idx].port;
            }
            vlan_port.match_vlan = vlans[vlan_idx].match_vlan;
            vlan_port.vsi = 0;
            vlan_port.flags = vlans[vlan_idx].flags;
            if (vlans[vlan_idx].nwk_group_valid)
            {
                vlan_port.ingress_network_group_id = vlans[vlan_idx].ingress_network_group_id;
                vlan_port.egress_network_group_id = vlans[vlan_idx].egress_network_group_id;
            }

            rv = bcm_vlan_port_create(unit, &vlan_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vlan_port_create\n");
                return rv;
            }
            *vlans[vlan_idx].vlan_port_id = vlan_port.vlan_port_id;
            vlans[vlan_idx].encap_id = vlan_port.encap_id;

            rv = bcm_vswitch_port_add(unit, *vlans[vlan_idx].vsi, vlan_port.vlan_port_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_vswitch_port_add\n");
                return rv;
            }

            if(vpls_util_advanced_verbose)
            {
                printf("%s(): vlan_idx = %d, vlan_port_id = 0x%x, encap_id = 0x%x, port = %d, match_vlan = %d, egress_vlan = %d, flags = 0x%x\n",
                    proc_name, vlan_idx, vlan_port.vlan_port_id, vlan_port.encap_id, vlan_port.port, vlan_port.match_vlan, vlan_port.egress_vlan, vlan_port.flags);
            }
        }
    }

    return rv;
}

/*
 * Main function for ingress, egress VPLS configuration based on MPLS utility: 
 */
int
vpls_util_advanced_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    int i;
    int vpls_idx;
    bcm_gport_t vlan_default;
    char *proc_name = "vpls_util_advanced_main";

    if(vpls_util_advanced_verbose)
    {
        printf("%s(): ENTER. port1 %d, port2 %d \r\n", proc_name, port1, port2);
    }

    if(!is_device_or_above(unit, JERICHO2))
    {
        printf("Only Jericho2 is supported\n");
        return BCM_E_CONFIG;
    }

    if (!vpls_util_advanced_params_set)
    {
        rv = init_default_vpls_util_advanced_params(unit, port1, port2);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in init_default_vpls_util_advanced_params\n", rv);
            return rv;
        }
    }
    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, vpls_util_advanced_vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, vpls_util_advanced_vpn);
        return rv;
    }
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure ingress VPLS entries\n", proc_name, i);
        }
        for(vpls_idx = 0; vpls_idx < VPLS_UTIL_ADVANCED_MAX_NOF_VPLS; vpls_idx++)
        {
            if(mpls_is_valid_lable(vpls_util_entity[i].vpls_ingress[vpls_idx].match_label))
            {

                /* create vlan based on the vsi (vpn) */
                rv = bcm_vlan_create(unit, *vpls_util_entity[i].vpls_ingress[vpls_idx].forwarding_domain);
                if ((rv != BCM_E_NONE) && (rv != BCM_E_EXISTS))
                {
                    printf("Failed (%d) to create VLAN %d\n", rv, vpls_util_entity[i].vpls_ingress[vpls_idx].forwarding_domain);
                    return rv;
                }
            }
        }
    }

    rv = mpls_util_main(unit, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_util_main\n", rv);
        return rv;
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure egress VPLS entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure egress VPLS entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_mpls_port_add_encapsulation(unit, vpls_util_entity[i].vpls_egress, VPLS_UTIL_ADVANCED_MAX_NOF_VPLS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_mpls_port_add_encapsulation\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure ingress VPLS entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure ingress VPLS entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_mpls_port_add_termination(unit, vpls_util_entity[i].vpls_ingress, VPLS_UTIL_ADVANCED_MAX_NOF_VPLS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_mpls_port_add_termination\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure VLAN entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure VLAN entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_vlan_port_config(unit, vpls_util_entity[i].vlan_ports, VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_PORT);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_vlan_port_config\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure static MACs entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure static MAC entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_l2_addr_add(unit, vpls_util_entity[i].l2_macs, VPLS_UTIL_ADVANCED_MAX_NOF_L2_MAC_FWD);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_l2_addr_add\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure network group entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure network group entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_config_networt_group_id(unit, vpls_util_entity[i].nwk_groups, VPLS_UTIL_ADVANCED_MAX_NOF_NWK_GROUPS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_config_networt_group_id\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure TPID entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure TPIDs entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_tag_formats_set(unit, vpls_util_entity[i].tpids, VPLS_UTIL_ADVANCED_MAX_NOF_TPIDS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_tag_formats_set\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure vlan editing entries */
    for (i = 0; i < VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose)
        {
            printf("%s(): entity %d: Configure vlan editing entries\n", proc_name, i);
        }
        rv = vpls_util_advanced_vlan_editing_config(unit, vpls_util_entity[i].vlan_edits, VPLS_UTIL_ADVANCED_MAX_NOF_VLAN_EDITS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in vpls_util_advanced_vlan_editing_config\n");
            return rv;
        }
    }
    
    if(vpls_util_advanced_verbose)
    {
        printf("%s(): EXIT. port1 %d, port2 %d \r\n", proc_name, port1, port2);
    }
    return rv;
}



