/* $Id: cint_sand_utils_l3.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * L3 utility functions
 */

/* Creating L3 interface */
struct create_l3_intf_s {
    /* Input */
    uint32 flags; /* BCM_L3_XXX */
    uint32 ingress_flags; /* BCM_L3_INGRESS_XXX */
    int no_publc; /* Used to force no public, public is forced if vrf = 0 or scale feature is turned on */
    int vsi;
    bcm_mac_t my_global_mac;
    bcm_mac_t my_lsb_mac;
    int vrf_valid; /* Do we need to set vrf */
    int vrf;
    int rpf_valid; /* Do we need to set rpf */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* avail. when BCM_L3_RPF is set */
    int mtu_valid;
    int mtu;
    int mtu_forwarding;
    int qos_map_valid;
    int qos_map_id;
    int ttl_valid;
    int ttl;
    uint8 native_routing_vlan_tags;
    uint8 oam_default_profile;

    /* Output */
    int rif;
    uint8 skip_mymac;   /* If true, mymac will not be set. Make sure you set it elsewhere. */
};

/* Creating L3 egress */
struct create_l3_egress_s {
    /* Input */
    uint32 allocation_flags; /* BCM_L3_XXX */
    uint32 l3_flags; /* BCM_L3_XXX */
    uint32 l3_flags2; /* BCM_L3_FLAGS2_XXX */

    /* ARP */
    int vlan; /* Outgoing vlan-VSI, relevant for ARP creation. In case set then SA MAC address is retreived from this VSI. */
    bcm_mac_t next_hop_mac_addr; /* Next-hop MAC address, relevant for ARP creation */
    int qos_map_id;                     /* General QOS map id */

    /* FEC */
    bcm_if_t out_tunnel_or_rif; /* *Outgoing intf, can be tunnel/rif, relevant for FEC creation */
    bcm_gport_t out_gport; /* *Outgoing port , relevant for FEC creation */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_if_t failover_if_id;            /* Failover Egress Object index. */


    /* Input/Output ID allocation */
    bcm_if_t fec_id; /* *FEC ID */
    bcm_if_t arp_encap_id; /* *ARP ID, may need for allocation ID or for FEC creation */
};

int default_egr_vlan_port_id_out = 0;

void _l2_print_mac(bcm_mac_t mac_address){
    unsigned int a,b,c,d,e,f;
    a = 0xff & mac_address[0];
    b = 0xff & mac_address[1];
    c = 0xff & mac_address[2];
    d = 0xff & mac_address[3];
    e = 0xff & mac_address[4];
    f = 0xff & mac_address[5];
    printf("%02x:%02x:%02x:%02x:%02x:%02x |",
           a,b,c,
           d,e,f);
}

void _l3_print_ip_addr(uint32 host)
{
  int a,b,c,d;

  a = (host >> 24) & 0xff;
  b = (host >> 16) & 0xff;
  c = (host >> 8 ) & 0xff;
  d = host & 0xff;
  printf("%d.%d.%d.%d", a,b,c,d);
}

void print_host(char *type, uint32 host, int vrf) {
    printf("%s  vrf:%d   ", type, vrf);
    _l3_print_ip_addr(host);
}

void print_route(char *type, uint32 host, uint32 mask, int vrf) {
    printf("%s  vrf:%d   ", type, vrf);
    _l3_print_ip_addr(host);
    printf("/");
    _l3_print_ip_addr(mask);
}

void ipv6_print(bcm_ip6_t *ipv6) {
    int i;
    printf("%02X%02X", ipv6[0], ipv6[1]);
    for(i = 2; i < 16; i += 2) {
        printf(":%02X%02X", ipv6[i], ipv6[i+1]);
    }
}

void ipv6_create_mask(bcm_ip6_t *ip6_mask, uint32 prefix_length) {
    int i;

    for (i=15; i>=0; i--) {
        if (i < prefix_length/8) {
            ip6_mask[i] = 0xFF;
        } else if (i == prefix_length/8) {
            ip6_mask[i] = 0xFF - ((1 << (8 -(prefix_length % 8))) - 1);
        } else {
            ip6_mask[i] = 0;
        }
    }
}

void ipv6_and_mask(bcm_ip6_t *ip6_output, bcm_ip6_t *ip6, bcm_ip6_t *ip6_mask) {
    int i;
    for (i=0; i<16; i++) {
        ip6_output[i] = ip6[i] & ip6_mask[i];
    }
}
/*******************************************************************************
 *
 * Example translations for the usages of L3 APIs from DPP(JR+) to DNX(JR2) begin
 * Add your example below between the frame if you have.
 * {
 */
/*
 * { L3 egress object create, backward compatible example -- begin
 *
 * This is an Example for porting JR+ bcm_l3_egress_create to JR2.
 * It can be used to replace the right BCM API in JR+ cints/applications
 * and works for both DPP(JR+) and DNX(JR2).
 */
int sand_l3_egress_create(int unit, uint32 flags, bcm_l3_egress_t *egr, bcm_if_t *if_id)
{
    int rv;

    /*
     * In DPP devices(JR+ e.g.), FEC and ARP can be created in the same call.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_l3_egress_create(unit, flags, egr, if_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_egress_create failed: %d \n", rv);
            return rv;
        }

        return rv;
    }

    /*
     * In DNX devices (JR2 e.g.), FEC and ARP must created in separate call.
     */
    uint32 allocation_flags;
    int is_ingress, is_egress;
    bcm_if_t l3egid_dummy;
    bcm_if_t rif_if;

    is_ingress = (flags & BCM_L3_EGRESS_ONLY) ? FALSE : TRUE;
    is_egress = (flags & BCM_L3_INGRESS_ONLY) ? FALSE : TRUE;

    /** Create Egress ARP entry. If egr->intf exists, the ARP entry will be linked by egr->intf.*/
    if (is_egress) {
        /** If interface represents RIF then ARP api will not accept it */
        if (!BCM_L3_ITF_TYPE_IS_LIF(egr->intf)) {
            rif_if = egr->intf;
            egr->intf = 0;
        }

        allocation_flags = flags | BCM_L3_EGRESS_ONLY;
        rv = bcm_l3_egress_create(unit, allocation_flags, egr, &l3egid_dummy);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_egress_create egress object failed: %d \n", rv);
            return rv;
        }

        /** returning egr->intf in case of RIF */
        if (rif_if !=0) {
            egr->intf = rif_if;
        }
    }

    /** Create Ingress FEC entry and configure it */
    if (is_ingress) {
        bcm_if_t arp_encap_id = egr->encap_id;

        /** Set egr->intf in FEC if it exists, otherwise egr->encap_id. But won't both unless egr->intf is RIF. */
        if (BCM_L3_ITF_TYPE_IS_LIF(egr->encap_id) && BCM_L3_ITF_TYPE_IS_LIF(egr->intf) && is_egress)
        {
            /** FEC can't carry two outlifs, if encap_id was created in above call, set it to 0 temporarily.*/
            egr->encap_id = 0;
        }
        else if (egr->intf == 0)
        {
            /** If only one of intf and encap_id is valid, it should reside in intf.*/
            egr->intf = egr->encap_id;
            egr->encap_id = 0;
        }

        allocation_flags = flags | BCM_L3_INGRESS_ONLY;
        rv = bcm_l3_egress_create(unit, allocation_flags, egr, if_id);
        if (rv != BCM_E_NONE)
        {
            printf("bcm_l3_egress_create ingress object failed: %d \n", rv);
            return rv;
        }

        /** Recover egr->encap_id*/
        egr->encap_id = arp_encap_id;
    }

    return rv;
}
/*
 * } L3 egress object create -- end
 */

/*******************************************************************************
 *
 * Add your example above between the frame if you have.
 * Examples translation for the usages of L3 APIs from DPP(JR+) to DNX(JR2) end
 * }
 */


/*
 * FEC object -- start
 */

/*
 * Struct for single FEC. Can't be used for protection.
 * For protection see other object (super-fec).
 */
struct sand_utils_l3_fec_s
{
    /* IN */
    uint32 allocation_flags; /* BCM_L3_XXX - API flags */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */
    uint32 fec_id; /* In case of WITH_ID */
    bcm_gport_t destination; /* FEC destination. Can be FEC or TM destination */
    bcm_if_t tunnel_gport; /* Either Tunnel or RIF or ARP */
    bcm_if_t tunnel_gport2; /* In case of two pointers, tunnel_gport2 is the second pointer */
    int qos_map_id; /* Used only in case of EEI PHP */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action; /* MPLS action. */
    bcm_gport_t hierarchical_gport;     /* Hierarchical TM-Flow. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_failover_t failover_id;  /* Failover Object Index. */
    bcm_if_t failover_if_id;     /* Failover Egress Object index. */

    /* OUT */
    bcm_if_t l3eg_fec_id; /* BCM encoding for out L3 FEC ID */
};

/* Initilize FEC structure to all 0s */
void sand_utils_l3_fec_s_init(int unit, uint32 flags, sand_utils_l3_fec_s *fec_structure)
{
    sal_memset(fec_structure, 0, sizeof (*fec_structure));
    fec_structure->mpls_label = BCM_MPLS_LABEL_INVALID;
}

/* Initilize FEC structure with common params */
void sand_utils_l3_fec_s_common_init(int unit, uint32 allocation_flags, uint32 flags, sand_utils_l3_fec_s *fec_structure, uint32 fec_id, uint32 l3_flags, uint32 l3_flags2, bcm_gport_t destination, bcm_if_t tunnel_gport, bcm_if_t tunnel_gport2)
{
    sand_utils_l3_fec_s_init(unit, 0, fec_structure);

    fec_structure->destination = destination;
    fec_structure->tunnel_gport = tunnel_gport;
    fec_structure->tunnel_gport2 = tunnel_gport2;
    fec_structure->flags = l3_flags;
    fec_structure->flags2 = l3_flags2;
    fec_structure->fec_id = fec_id;
    fec_structure->allocation_flags = allocation_flags;
}

/*
 * Create or Replace a FEC entry, No ARP allocation is done in this API call.
 * Note: Assume sand_utils_l3_fec_s_*_init function is being called before.
 */
int sand_utils_l3_fec_create(int unit, sand_utils_l3_fec_s *fec_structure) {
    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);

    fec_structure->allocation_flags |= BCM_L3_INGRESS_ONLY;

    /* FEC properties */
    l3eg.intf           = fec_structure->tunnel_gport ? fec_structure->tunnel_gport : fec_structure->tunnel_gport2;
    l3eg.port           = fec_structure->destination;
    l3eg.encap_id       = fec_structure->tunnel_gport ? fec_structure->tunnel_gport2 : 0;
    l3eg.flags          = fec_structure->flags ;
    l3eg.flags2         = fec_structure->flags2;
    l3eg.mpls_action    = fec_structure->mpls_action;
    l3eg.mpls_label     = fec_structure->mpls_label;
    l3eg.hierarchical_gport = fec_structure->hierarchical_gport;
    l3eg.stat_id        = fec_structure->stat_id;
    l3eg.stat_pp_profile    = fec_structure->stat_pp_profile;
    l3eg.failover_id    = fec_structure->failover_id;
    l3eg.failover_if_id = fec_structure->failover_if_id;
    l3egid              = fec_structure->fec_id;

    rc = bcm_l3_egress_create(unit, fec_structure->allocation_flags, &l3eg, &l3egid);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rc;
    }

    fec_structure->l3eg_fec_id = l3egid;

    if(verbose >= 1) {
        printf("Created FEC-id = 0x%08x, allocation_flags = 0x%08x , flags = 0x%08x, flags2 = 0x%08x", fec_structure->l3eg_fec_id, fec_structure->allocation_flags, fec_structure->flags, fec_structure->flags2);
    }

    if(verbose >= 2) {
        printf(", Tunnel1 = 0x%08x, Tunnel2 = 0x%08x, out-port =0x%x stat_id = 0x%x stat_pp_profile = 0x%x", fec_structure->tunnel_gport,  fec_structure->tunnel_gport2, fec_structure->destination, fec_structure->stat_id, fec_structure->stat_pp_profile);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}

/*
 * FEC object -- end
 */


/*
 * ARP object -- start
 */
/*
 * Struct for ARP
 */
struct sand_utils_l3_arp_s
{
    /* IN */
    uint32 allocation_flags; /* BCM_L3_XXX - API flags */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */
    uint32 arp_id; /* In case of WITH_ID */
    uint32 intf_id;  /* Interface ID */

    int eth_rif; /* ETH-RIF, L3 VSI. In case set then SA MAC address is retreived from this VSI and EVE can take this field. */
    bcm_mac_t next_hop_mac_addr; /* Next-hop MAC address, relevant for ARP creation */
    int qos_map_id; /* General QOS map id */
    bcm_mac_t src_mac_addr; /* Source MAC address */

    /* OUT */
    bcm_if_t l3eg_arp_id; /* BCM encoding for out L3 ARP ID */
    int mtu;
};

/* Initilize ARP structure to all 0s */
void sand_utils_l3_arp_s_init(int unit, uint32 flags, sand_utils_l3_arp_s *arp_structure)
{
    sal_memset(arp_structure, 0, sizeof (*arp_structure));
}

/* Initilize ARP structure with common params */
void sand_utils_l3_arp_s_common_init(int unit, uint32 flags, sand_utils_l3_arp_s *arp_structure, uint32 arp_id, uint32 l3_flags, uint32 l3_flags2, bcm_mac_t next_hop, int eth_rif)
{
    sand_utils_l3_arp_s_init(unit, 0, arp_structure);

    arp_structure->allocation_flags = flags;
    arp_structure->next_hop_mac_addr = next_hop;
    arp_structure->eth_rif = eth_rif;
    arp_structure->flags = l3_flags;
    arp_structure->flags2 = l3_flags2;
    arp_structure->arp_id = arp_id;
}

int sand_utils_l3_arp_create(int unit, sand_utils_l3_arp_s *arp_structure)
{
    int rc;
    uint32 flags;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not used */

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = arp_structure->next_hop_mac_addr;
    l3eg.encap_id = arp_structure->arp_id;
    l3eg.vlan = arp_structure->eth_rif;
    l3eg.intf = arp_structure->intf_id;
    l3eg.qos_map_id = arp_structure->qos_map_id;
    l3eg.flags2 = arp_structure->flags2;
    l3eg.flags = arp_structure->flags;
    l3eg.mtu = arp_structure->mtu;
    if (arp_structure->flags2 & BCM_L3_FLAGS2_SRC_MAC) 
    {
        l3eg.src_mac_addr = arp_structure->src_mac_addr;
    }

    flags |= (BCM_L3_EGRESS_ONLY | arp_structure->allocation_flags);
    rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rc != BCM_E_NONE)
    {
        printf("Error, create egress object, \n");
        return rc;
    }

    arp_structure->l3eg_arp_id = l3eg.encap_id;

    if(verbose >= 1) {
        printf("Created ARP encap-id = 0x%08x", arp_structure->l3eg_arp_id);
    }

    if(verbose >= 2) {
        printf(", Next hop MAC = ");
        _l2_print_mac(arp_structure->next_hop_mac_addr);
        printf(", VSI = 0x%08x, flags =0x%08x, flags2 =0x%08x", arp_structure->eth_rif,  arp_structure->flags, arp_structure->flags2);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}
/*
 * ARP object -- end
 */

/*
 * { Utils Creating L3 egress - begin
 */
struct l3_egress_s {
    /* Input */
    uint32 allocation_flags; /* BCM_L3_XXX */
    uint32 l3_flags;         /* BCM_L3_XXX */
    uint32 l3_flags2;        /* BCM_L3_FLAGS2_XXX */

    /* ARP */
    int vlan;                    /* Outgoing vlan-VSI, relevant for ARP creation. In case set then SA MAC address is retreived from this VSI. */
    bcm_mac_t next_hop_mac_addr; /* Next-hop MAC address, relevant for ARP creation */
    int qos_map_id;              /* General QOS map id */

    /* FEC */
    bcm_if_t out_tunnel_or_rif;  /* *Outgoing intf, can be tunnel/rif, relevant for FEC creation */
    bcm_gport_t out_gport;       /* *Outgoing port , relevant for FEC creation */
    bcm_failover_t failover_id;  /* Failover Object Index. */
    bcm_if_t failover_if_id;     /* Failover Egress Object index. */


    /* Input/Output ID allocation */
    bcm_if_t fec_id;             /* *FEC ID */
    bcm_if_t arp_encap_id;       /* *ARP ID, may need for allocation ID or for FEC creation */
};

/* Initilize L3-port structure to all 0s */
void sand_utils_l3_egress_s_init(int unit, uint32 flags, l3_egress_s *l3_egress)
{
    sal_memset(l3_egress, 0, sizeof (*l3_egress));
}

/* Create egress object(FEC and ARP entry)*/
int sand_utils_l3_egress_create(int unit, l3_egress_s *l3_egress) {
    int rc;

    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);

    /* FEC properties */
    l3eg.intf           = l3_egress->out_tunnel_or_rif;
    l3eg.port           = l3_egress->out_gport;
    l3eg.encap_id       = l3_egress->arp_encap_id;
    /* FEC properties - protection */
    l3eg.failover_id    = l3_egress->failover_id;
    l3eg.failover_if_id = l3_egress->failover_if_id;

    /* ARP */
    l3eg.mac_addr   = l3_egress->next_hop_mac_addr;
    l3eg.vlan       = l3_egress->vlan;
    l3eg.qos_map_id = l3_egress->qos_map_id;

    l3eg.flags    = l3_egress->l3_flags ;
    l3eg.flags2   = l3_egress->l3_flags2;
    l3egid        = l3_egress->fec_id;

    rc = sand_l3_egress_create(unit, l3_egress->allocation_flags, &l3eg, &l3egid);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rc;
    }

    l3_egress->fec_id = l3egid;
    l3_egress->arp_encap_id = l3eg.encap_id;

    if(verbose >= 1) {
        printf("unit %d: created FEC-id =0x%08x, ", unit, l3_egress->fec_id);
        printf("encap-id = %08x", l3_egress->arp_encap_id );
    }

    if(verbose >= 2) {
        printf("outRIF = 0x%08x out-port =%d", l3_egress->out_tunnel_or_rif, l3_egress->out_gport);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}
/*
 * } Utils Creating L3 egress - end
 */


/*
 * ETH-RIF object -- start
 */
/*
 * Struct for ETH-RIF
 */
struct sand_utils_l3_eth_rif_s
{
    /* IN */
    uint32 flags; /* BCM_L3_XXX */
    uint32 qos_flags;     /* BCM_L3_INTF_QOS_* */
    uint32 ingress_flags; /* BCM_L3_INGRESS_XXX */
    int eth_rif_id; /* L3 VSI interface */
    bcm_mac_t my_mac;
    int vrf;
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* avail. when BCM_L3_RPF is set */
    int ing_qos_map_id;
    int egr_qos_map_id;
    int mtu_valid;
    int mtu;
    int mtu_forwarding; /* Forwarding Layer MTU. Only relevant to JER1 and below */
    int ttl_valid;      /* TTL valid. Only relevant to JER1 and below */
    int ttl;            /* TTL. Only relevant to JER1 and below */
    bcm_qos_ingress_model_t ingress_qos_model;
    uint8 native_routing_vlan_tags;     /* Set number of VLAN tags expected when interface is used for native routing. Only relevant to JER1 and below */
    /* OUT */
    bcm_if_t l3_rif;
};

struct create_l3_egress_with_mpls_s {

    uint32 allocation_flags; /* BCM_L3_XXX */
    uint32 l3_flags; /* BCM_L3_XXX */
    uint32 l3_flags2; /* BCM_L3_FLAGS2_XXX */

    /* ARP */
    int vlan; /* Outgoing vlan-VSI, relevant for ARP creation. In case set then SA MAC address is retreived from this VSI. */
    bcm_mac_t next_hop_mac_addr; /* Next-hop MAC address, relevant for ARP creation */
    int qos_map_id;                     /* General QOS map id */

    /* FEC */
    bcm_if_t out_tunnel_or_rif; /* *Outgoing intf, can be tunnel/rif, relevant for FEC creation */
    bcm_gport_t out_gport; /* *Outgoing port , relevant for FEC creation */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_if_t failover_if_id;            /* Failover Egress Object index. */

    uint32 mpls_flags;
    bcm_mpls_label_t mpls_label;
    bcm_mpls_egress_action_t mpls_action;
    int mpls_ttl;
    int mpls_exp;

    /* Input/Output ID allocation */
    bcm_if_t fec_id; /* *FEC ID */
    bcm_if_t arp_encap_id; /* *ARP ID, may need for allocation ID or for FEC creation */
};

/* etpp mtu set for rif */
int
mtu_check_etpp_rif_set (
    int unit,
    int eth_rif_id,
    int compressed_layer_type,
    int mtu,
    int is_set)
{
    int rc;
    bcm_rx_mtu_config_t mtu_config;
    bcm_gport_t trap_gport;
    int fwd_strength = 15;
    int snp_strength = 0;
    int trap_id = 0;
    int mtu_profile;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;

    bcm_rx_mtu_config_t_init(&mtu_config);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = mtu_profile;
    mtu_key.cmp_layer_type = compressed_layer_type;

    if (is_set) {
        trap_id = (mtu > 0) ? BCM_RX_TRAP_EG_TX_TRAP_ID_DROP : BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT;
        BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);
        mtu_value.trap_gport = trap_gport;
        mtu_value.mtu_val = mtu;
    }

    rc = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_RIF, &mtu_key, &mtu_value);
    if(rc!= BCM_E_NONE)
    {
        printf("Error in bcm_rx_mtu_profile_set \n");
        return rc;
    }

    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = eth_rif_id;
    mtu_config.mtu_profile = mtu_profile;
    rc = bcm_rx_mtu_set(unit, &mtu_config);
    if(rc!= BCM_E_NONE)
    {
        printf("Error in bcm_rx_mtu_set \n");
        return rc;
    }

    return rc;
}

/* Initilize ETH-RIF structure to all 0s */
void sand_utils_l3_eth_rif_s_init(int unit, uint32 flags, sand_utils_l3_eth_rif_s *eth_rif_structure)
{
    sal_memset(eth_rif_structure, 0, sizeof (*eth_rif_structure));
}
/* Initilize ETH-RIF structure with common params */
void sand_utils_l3_eth_rif_s_common_init(int unit, uint32 flags, sand_utils_l3_eth_rif_s *eth_rif_structure, uint32 eth_rif_id, uint32 l3_flags, uint32 l3_ingress_flags, bcm_mac_t my_mac, int vrf)
{
    sand_utils_l3_eth_rif_s_init(unit, 0, eth_rif_structure);

    eth_rif_structure->my_mac = my_mac;
    eth_rif_structure->flags = l3_flags;
    eth_rif_structure->ingress_flags = l3_ingress_flags;
    eth_rif_structure->vrf = vrf;
    eth_rif_structure->eth_rif_id = eth_rif_id;
}

void sand_utils_l3_eth_rif_s_additonal_mtu_ttl_init(int unit, sand_utils_l3_eth_rif_s *eth_rif_structure, uint32 mtu_valid, uint32 mtu, uint32 mtu_fwd, uint32 ttl_valid, uint32 ttl)
{
    if (ttl_valid) {
        eth_rif_structure->ttl_valid = 1;
        eth_rif_structure->ttl = ttl;
    }
    
    /* mtu_forwarding only valid for JER1 and below */
    if (!is_device_or_above(unit, JERICHO2)) {
        if (mtu_valid) {
            eth_rif_structure->mtu_valid = 1;
            eth_rif_structure->mtu_forwarding = mtu_fwd;
        }
    }
    
    if (mtu_valid) {
        eth_rif_structure->mtu_valid = 1;
        eth_rif_structure->mtu = mtu;
    }
}

int sand_utils_l3_eth_rif_create(int unit, sand_utils_l3_eth_rif_s *eth_rif_structure)
{
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;
    int rc;

    /*
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;

    /*
     * My-MAC
     */
    l3if.l3a_mac_addr = eth_rif_structure->my_mac;
    eth_rif_structure->l3_rif = l3if.l3a_intf_id = l3if.l3a_vid = eth_rif_structure->eth_rif_id;
    l3if.dscp_qos.qos_map_id = eth_rif_structure->egr_qos_map_id;
    if (is_device_or_above(unit, JERICHO2))
    {
        sal_memcpy(&l3if.ingress_qos_model, &eth_rif_structure->ingress_qos_model,sizeof(l3if.ingress_qos_model));
    }
    l3if.l3a_flags |= eth_rif_structure->flags;
    if (!is_device_or_above(unit, JERICHO2)) {
        /* for JR2, setting mtu size via bcm_l3_intf_create is not supported */
        l3if.l3a_mtu = 1524;      /* default settings */
        if (eth_rif_structure->mtu_valid) {
            l3if.l3a_mtu = eth_rif_structure->mtu;
            l3if.l3a_mtu_forwarding = eth_rif_structure->mtu_forwarding;
        }
    }
    if (eth_rif_structure->ttl_valid)
    {
        l3if.l3a_ttl = eth_rif_structure->ttl;
    }
    /* native ethernet compensation is supported until Jericho.
           Helps to to calculate the UDP.length by indicating how many vlans are expected
           Not needed in JERICHO_PLUS and above device, since native LL has already been built in native LL block */
    if (!is_device_or_above(unit,JERICHO_PLUS)) {
        l3if.native_routing_vlan_tags = eth_rif_structure->native_routing_vlan_tags; /* native ethernet compensation */
    }

    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create %d\n", rc);
        return rc;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        if (eth_rif_structure->mtu_valid) {
            if (eth_rif_structure->mtu_forwarding != 0) {
                printf("Error, setting mtu_forwarding is not supported by this device\n");
                return BCM_E_PARAM;
            }
            rc = mtu_check_etpp_rif_set(unit, eth_rif_structure->l3_rif, 2/*compressed_layer_type*/, eth_rif_structure->mtu, 1/*is_set*/);
            if (rc != BCM_E_NONE) {
                printf("Error, mtu_check_etpp_rif_set %d\n", rc);
                return rc;
            }
        }
    }

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
    l3_ing_if.vrf = eth_rif_structure->vrf;

    /* set RIF enable RPF*/
    if (eth_rif_structure->flags & BCM_L3_RPF) {
        l3_ing_if.urpf_mode = eth_rif_structure->urpf_mode;
    } else {
        l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;
    }


    l3_ing_if.flags |= eth_rif_structure->ingress_flags;


    l3_ing_if.qos_map_id = eth_rif_structure->ing_qos_map_id;


    rc = bcm_l3_ingress_create(unit, l3_ing_if, &l3if.l3a_intf_id);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rc;
    }

    if(verbose >= 1) {
        printf("Created ETH-RIF id = 0x%08x", eth_rif_structure->l3_rif);
    }

    if(verbose >= 2) {
        printf(", My MAC = ");
        _l2_print_mac(eth_rif_structure->my_mac);
        printf(" vrf = 0x%08x, flags =0x%08x, ingress_flags =0x%08x", eth_rif_structure->vrf,  eth_rif_structure->flags, eth_rif_structure->ingress_flags);
    }

    if(verbose >= 1) {
        printf("\n");
    }


    return rc;
}
/*
 * ETH-RIF object -- end
 */

/*
 * Host entry -- start
 */
/*
 * Struct for Host IPv4
 */
struct sand_utils_l3_host_ipv4_s
{
    /* IN */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */

    /* Key */
    bcm_ip_t addr; /* Destination host IP address (IPv4). */
    bcm_vrf_t vrf; /* VRF ID */

    /* Result */
    bcm_if_t fec_id; /* Option1 FEC */
    bcm_gport_t destination; /* Option 2 destination that is not FEC */
    bcm_if_t tunnel_gport; /* Usually used for Option2, then it is Out-RIF */
    bcm_if_t tunnel_gport2; /* Usually used for Option2, then it is ARP */
};

/* Initilize Host ipv4  structure to all 0s */
void sand_utils_l3_host_ipv4_s_init(int unit, uint32 flags, sand_utils_l3_host_ipv4_s *host_ipv4_structure)
{
    sal_memset(host_ipv4_structure, 0, sizeof (*host_ipv4_structure));
}
/* Initilize Host structure with common params */
void sand_utils_l3_host_ipv4_s_common_init(int unit, uint32 flags, sand_utils_l3_host_ipv4_s *host_ipv4_structure, bcm_ip_t addr, bcm_vrf_t vrf, uint32 l3_flags, uint32 l3_flags2, bcm_if_t fec_id)
{
    sand_utils_l3_host_ipv4_s_init(unit, 0, host_ipv4_structure);

    host_ipv4_structure->addr = addr;
    host_ipv4_structure->flags = l3_flags;
    host_ipv4_structure->flags2 = l3_flags2;
    host_ipv4_structure->vrf = vrf;
    host_ipv4_structure->fec_id = fec_id;
}

int sand_utils_l3_host_ipv4_add(int unit, sand_utils_l3_host_ipv4_s *host_ipv4_structure)
{
    int rc;
    bcm_l3_host_t host;
    bcm_l3_host_t_init(&host);

    host.l3a_ip_addr = host_ipv4_structure->addr;
    host.l3a_vrf = host_ipv4_structure->vrf;
    host.l3a_intf = host_ipv4_structure->fec_id;       /* FEC-ID or OUTRIF */
    host.encap_id = host_ipv4_structure->tunnel_gport2;
    host.l3a_flags = host_ipv4_structure->flags;
    host.l3a_flags2 = host_ipv4_structure->flags2;
    if (host_ipv4_structure->destination != 0)
    {
        /*
         * if destination is assigned then intf should be outrif and not FEC
         */
        if (host_ipv4_structure->fec_id == 0)
        {
            host.l3a_intf = host_ipv4_structure->tunnel_gport;       /* FEC-ID or OUTRIF */
            host.l3a_port_tgid = host_ipv4_structure->destination;
        }
        else
        {
            printf("sand_utils_l3_host_ipv4_add failed on invalid params: intf is FEC 0x%08x but destination 0x%08x is non 0 \n", host_ipv4_structure->fec_id, host_ipv4_structure->destination);
            return BCM_E_PARAM;
        }

    }
    rc = bcm_l3_host_add(unit, host);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rc);
        return rc;
    }

    if(verbose >= 1) {
        printf("Added Host IPv4 entry for VRF = 0x%08x", host_ipv4_structure->vrf);
    }

    if(verbose >= 2) {
        printf(", Addr = ");
        _l3_print_ip_addr(host_ipv4_structure->addr);
        if (host_ipv4_structure->fec_id)
        {
            printf(", fec = 0x%08x, flags =0x%08x, flags2 =0x%08x", host_ipv4_structure->fec_id,  host_ipv4_structure->flags, host_ipv4_structure->flags2);
        }
        else
        {
            printf(", dest = 0x%08x, tunnel1 = 0x%08x, tunnel2 = 0x%08x, flags =0x%08x, flags2 =0x%08x",
                    host_ipv4_structure->destination,  host_ipv4_structure->tunnel_gport, host_ipv4_structure->tunnel_gport2, host_ipv4_structure->flags, host_ipv4_structure->flags2);
        }
    }

    if(verbose >= 1) {
        printf("\n");
    }


    return rc;
}

/*
 * Struct for Host IPv6
 */
struct sand_utils_l3_host_ipv6_s
{
    /* IN */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */

    /* Key */
    bcm_ip6_t addr; /* Destination host IP address (IPv6). */
    bcm_vrf_t vrf; /* VRF ID */

    /* Result */
    bcm_if_t fec_id; /* Option1 FEC */
    bcm_gport_t destination; /* Option 2 destination that is not FEC */
    bcm_if_t tunnel_gport; /* Usually used for Option2, then it is Out-RIF */
    bcm_if_t tunnel_gport2; /* Usually used for Option2, then it is ARP */
};

/* Initilize Host ipv4  structure to all 0s */
void sand_utils_l3_host_ipv6_s_init(int unit, uint32 flags, sand_utils_l3_host_ipv6_s *host_ipv6_structure)
{
    sal_memset(host_ipv6_structure, 0, sizeof (*host_ipv6_structure));
}
/* Initilize Host structure with common params */
void sand_utils_l3_host_ipv6_s_common_init(int unit, uint32 flags, sand_utils_l3_host_ipv6_s *host_ipv6_structure, bcm_ip6_t addr, bcm_vrf_t vrf, uint32 l3_flags, uint32 l3_flags2, bcm_if_t fec_id)
{
    sand_utils_l3_host_ipv6_s_init(unit, 0, host_ipv6_structure);

    host_ipv6_structure->addr = addr;
    host_ipv6_structure->flags = l3_flags;
    host_ipv6_structure->flags2 = l3_flags2;
    host_ipv6_structure->vrf = vrf;
    host_ipv6_structure->fec_id = fec_id;
}

int sand_utils_l3_host_ipv6_add(int unit, sand_utils_l3_host_ipv6_s *host_ipv6_structure)
{
    int rc;
    bcm_l3_host_t host;
    bcm_l3_host_t_init(&host);

    host.l3a_ip6_addr = host_ipv6_structure->addr;
    host.l3a_vrf = host_ipv6_structure->vrf;
    host.l3a_intf = host_ipv6_structure->fec_id;       /* FEC-ID or OUTRIF */
    host.encap_id = host_ipv6_structure->tunnel_gport2;
    host.l3a_flags = BCM_L3_IP6 | host_ipv6_structure->flags;
    host.l3a_flags2 = host_ipv6_structure->flags2;
    if (host_ipv6_structure->destination != 0)
    {
        /*
         * if destination is assigned then intf should be outrif and not FEC
         */
        if (host_ipv6_structure->fec_id == 0)
        {
            host.l3a_intf = host_ipv6_structure->tunnel_gport;       /* FEC-ID or OUTRIF */
            host.l3a_port_tgid = host_ipv6_structure->destination;
        }
        else
        {
            printf("sand_utils_l3_host_ipv6_add failed on invalid params: intf is FEC 0x%08x but destination 0x%08x is non 0 \n", host_ipv4_structure->fec_id, host_ipv4_structure->destination);
            return BCM_E_PARAM;
        }
    }
    rc = bcm_l3_host_add(unit, host);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_host_add failed: %x \n", rc);
        return rc;
    }

    if(verbose >= 1) {
        printf("Added Host IPv6 entry for VRF = 0x%08x", host_ipv6_structure->vrf);
    }

    if(verbose >= 2) {
        printf(", Addr = ");
        ipv6_print(host_ipv6_structure->addr);
        if (host_ipv6_structure->fec_id)
        {
            printf(", fec = 0x%08x, flags =0x%08x, ingress_flags =0x%08x", host_ipv6_structure->fec_id,  host_ipv6_structure->flags, host_ipv6_structure->flags2);
        }
        else
        {
            printf(", dest = 0x%08x, tunnel1 = 0x%08x, tunnel2 = 0x%08x, flags =0x%08x, flags2 =0x%08x",
                    host_ipv6_structure->destination,  host_ipv6_structure->tunnel_gport, host_ipv6_structure->tunnel_gport2, host_ipv6_structure->flags, host_ipv6_structure->flags2);
        }
    }

    if(verbose >= 1) {
        printf("\n");
    }


    return rc;
}

/*
 * Host entry -- end
 */

/*
 * Route entry -- start
 */
/*
 * Struct for Route IPv4
 */
struct sand_utils_l3_route_ipv4_s
{
    /* IN */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */

    /* Key */
    bcm_ip_t addr; /* Destination route IP address (IPv4). */
    bcm_ip_t mask; /* Mask for Destination route IP address (IPv4). */
    bcm_vrf_t vrf; /* VRF ID */

    /* Result */
    bcm_if_t fec_id; /* FEC */
};

/* Initilize Route ipv4  structure to all 0s */
void sand_utils_l3_route_ipv4_s_init(int unit, uint32 flags, sand_utils_l3_route_ipv4_s *route_ipv4_structure)
{
    sal_memset(route_ipv4_structure, 0, sizeof (*route_ipv4_structure));
}
/* Initilize Route structure with common params */
void sand_utils_l3_route_ipv4_s_common_init(int unit, uint32 flags, sand_utils_l3_route_ipv4_s *route_ipv4_structure, bcm_ip_t addr, bcm_ip_t mask, bcm_vrf_t vrf, uint32 l3_flags, uint32 l3_flags2, bcm_if_t fec_id)
{
    sand_utils_l3_route_ipv4_s_init(unit, 0, route_ipv4_structure);

    route_ipv4_structure->addr = addr;
    route_ipv4_structure->mask = mask;
    route_ipv4_structure->flags = l3_flags;
    route_ipv4_structure->flags2 = l3_flags2;
    route_ipv4_structure->vrf = vrf;
    route_ipv4_structure->fec_id = fec_id;
}

int sand_utils_l3_route_ipv4_add(int unit, sand_utils_l3_route_ipv4_s *route_ipv4_structure)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    l3rt.l3a_subnet = route_ipv4_structure->addr;
    l3rt.l3a_ip_mask = route_ipv4_structure->mask;
    l3rt.l3a_vrf = route_ipv4_structure->vrf;
    l3rt.l3a_intf = route_ipv4_structure->fec_id;       /* FEC-ID */
    l3rt.l3a_flags = route_ipv4_structure->flags;
    l3rt.l3a_flags2 = route_ipv4_structure->flags2;
    if (is_jericho2_kbp_ipv4_split_rpf_enabled(unit))
    {
        if (!(l3rt.l3a_flags2 & BCM_L3_FLAGS2_RPF_ONLY))
        {
            l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }
    }

    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_route_add failed: %x \n", rc);
        return rc;
    }

    if(verbose >= 1) {
        printf("Added Route IPv4 entry for VRF = 0x%08x", route_ipv4_structure->vrf);
    }

    if(verbose >= 2) {
        printf(", Addr = ");
        _l3_print_ip_addr(route_ipv4_structure->addr);
        printf(", Mask = 0x%08x", route_ipv4_structure->mask);
        printf(", fec = 0x%08x, flags =0x%08x, flags2 =0x%08x", route_ipv4_structure->fec_id,  route_ipv4_structure->flags, route_ipv4_structure->flags2);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}

/*
 * Struct for Route IPv6
 */
struct sand_utils_l3_route_ipv6_s
{
    /* IN */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */

    /* Key */
    bcm_ip6_t addr; /* Destination route IP address (IPv6). */
    bcm_ip6_t mask; /* Mask for Destination route IP address (IPv6). */
    bcm_vrf_t vrf; /* VRF ID */

    /* Result */
    bcm_if_t fec_id; /* FEC */
};

/* Initilize Route ipv4  structure to all 0s */
void sand_utils_l3_route_ipv6_s_init(int unit, uint32 flags, sand_utils_l3_route_ipv6_s *route_ipv6_structure)
{
    sal_memset(route_ipv6_structure, 0, sizeof (*route_ipv6_structure));
}
/* Initilize Route structure with common params */
void sand_utils_l3_route_ipv6_s_common_init(int unit, uint32 flags, sand_utils_l3_route_ipv6_s *route_ipv6_structure, bcm_ip6_t addr, bcm_ip6_t mask, bcm_vrf_t vrf, uint32 l3_flags, uint32 l3_flags2, bcm_if_t fec_id)
{
    sand_utils_l3_route_ipv6_s_init(unit, 0, route_ipv6_structure);

    route_ipv6_structure->addr = addr;
    route_ipv6_structure->mask = mask;
    route_ipv6_structure->flags = l3_flags;
    route_ipv6_structure->flags2 = l3_flags2;
    route_ipv6_structure->vrf = vrf;
    route_ipv6_structure->fec_id = fec_id;
}

int sand_utils_l3_route_ipv6_add(int unit, sand_utils_l3_route_ipv6_s *route_ipv6_structure)
{
    int rc;
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    l3rt.l3a_ip6_net = route_ipv6_structure->addr;
    l3rt.l3a_ip6_mask = route_ipv6_structure->mask;
    l3rt.l3a_vrf = route_ipv6_structure->vrf;
    l3rt.l3a_intf = route_ipv6_structure->fec_id;       /* FEC-ID */
    l3rt.l3a_flags = BCM_L3_IP6 | route_ipv6_structure->flags;
    l3rt.l3a_flags2 = route_ipv6_structure->flags2;
    if (is_jericho2_kbp_ipv6_split_rpf_enabled(unit))
    {
        if (!(l3rt.l3a_flags2 & BCM_L3_FLAGS2_RPF_ONLY))
        {
            l3rt.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }
    }

    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE)
    {
        printf("bcm_l3_route_add failed: %x \n", rc);
        return rc;
    }

    if(verbose >= 1) {
        printf("Added Route IPv6 entry for VRF = 0x%08x", route_ipv6_structure->vrf);
    }

    if(verbose >= 2) {
        printf(", Addr = ");
        ipv6_print(route_ipv6_structure->addr);
        printf(", Mask = ");
        ipv6_print(route_ipv6_structure->mask);
        printf(", fec = 0x%08x, flags =0x%08x, flags2 =0x%08x", route_ipv6_structure->fec_id,  route_ipv6_structure->flags, route_ipv6_structure->flags2);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}
/*
 * Route entry -- end
 */

/*
 * vPBR entry -- start
 */
/*
 * Struct for vPBR IPv4
 */
struct sand_utils_l3_vpbr_ipv4_s
{
    /* IN */
    uint32 flags; /* BCM_L3_XXX */
    uint32 flags2; /* BCM_L3_FLAGS2_XXX */

    /* Key */
    bcm_ip_t dip_addr; /* Destination route IP address (IPv4). */
    bcm_ip_t dip_mask; /* Mask for Destination route IP address (IPv4). */
    bcm_ip_t sip_addr; /* Source route IP address (IPv4). */
    bcm_ip_t sip_mask; /* Mask for Source route IP address (IPv4). */
    int vrf_valid;
    bcm_vrf_t vrf;
    int dscp_valid;
    int dscp;
    uint32 src_port_valid;
    uint32 src_port;
    uint32 dst_port_valid;
    uint32 dst_port;
    int eth_rif_valid;
    bcm_if_t eth_rif;

    /* Location */
    int priority;

    /* Result */
    bcm_vrf_t new_vrf; /* VRF ID */
};

/* Initilize Route ipv4  structure to all 0s */
void sand_utils_l3_vpbr_ipv4_s_init(int unit, uint32 flags, sand_utils_l3_vpbr_ipv4_s *vpbr_ipv4_structure)
{
    sal_memset(vpbr_ipv4_structure, 0, sizeof (*vpbr_ipv4_structure));
}
/* Initilize ARP structure with common params */
void sand_utils_l3_vpbr_ipv4_s_common_init(int unit, uint32 flags, sand_utils_l3_vpbr_ipv4_s *vpbr_ipv4_structure, bcm_ip_t dip_addr, bcm_ip_t dip_mask,
        bcm_ip_t sip_addr, bcm_ip_t sip_mask, bcm_vrf_t vrf, uint32 vpbr_flags, bcm_vrf_t new_vrf)
{
    sand_utils_l3_vpbr_ipv4_s_init(unit, 0, vpbr_ipv4_structure);

    vpbr_ipv4_structure->dip_addr = dip_addr;
    vpbr_ipv4_structure->dip_mask = dip_mask;
    vpbr_ipv4_structure->sip_addr = sip_addr;
    vpbr_ipv4_structure->dip_mask = dip_mask;
    vpbr_ipv4_structure->flags = vpbr_flags;
    vpbr_ipv4_structure->vrf = vrf;
    vpbr_ipv4_structure->vrf_valid = 1;
    vpbr_ipv4_structure->new_vrf = new_vrf;
    vpbr_ipv4_structure->priority = 1;
}

int sand_utils_l3_vpbr_ipv4_add(int unit, sand_utils_l3_vpbr_ipv4_s *vpbr_ipv4_structure)
{
    int rc;
    bcm_l3_vpbr_entry_t entry;
    bcm_l3_vpbr_entry_t_init(&entry);

    entry.dip_addr = vpbr_ipv4_structure->dip_addr;
    entry.dip_addr_mask = vpbr_ipv4_structure->dip_mask;
    entry.sip_addr = vpbr_ipv4_structure->sip_addr;
    entry.sip_addr_mask = vpbr_ipv4_structure->sip_mask;
    entry.priority = vpbr_ipv4_structure->priority;
    entry.flags = vpbr_ipv4_structure->flags;

    if (vpbr_ipv4_structure->vrf_valid)
    {
        entry.vrf = vpbr_ipv4_structure->vrf;
        entry.vrf_mask = 0xFFFFFFFFF;
    }
    if (vpbr_ipv4_structure->eth_rif_valid)
    {
        entry.l3_intf_id  = vpbr_ipv4_structure->eth_rif;
        entry.l3_intf_id_mask = 0xFFFFFFFFF;
    }
    if (vpbr_ipv4_structure->dscp_valid)
    {
        entry.dscp = vpbr_ipv4_structure->dscp;
        entry.dscp_mask = 0xFF;
    }
    if (vpbr_ipv4_structure->src_port_valid)
    {
        entry.src_port = vpbr_ipv4_structure->src_port;
        entry.src_port_mask = 0xFFFF;
    }
    if (vpbr_ipv4_structure->dst_port_valid)
    {
        entry.dst_port = vpbr_ipv4_structure->dst_port;
        entry.dst_port_mask = 0xFFFF;
    }

    entry.new_vrf = vpbr_ipv4_structure->new_vrf;
    rc = bcm_l3_vpbr_entry_add(unit, &entry);
    if (rc != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_vpbr_entry_add\n");
        return rc;
    }

    if(verbose >= 1) {
        if (vpbr_ipv4_structure->vrf_valid)
        {
            printf("Added vPBR IPv4 entry for VRF = 0x%08x to new VRF = 0x%08x", vpbr_ipv4_structure->vrf, vpbr_ipv4_structure->new_vrf);
        }
        else
        {
            printf("Added vPBR IPv4 entry to new VRF = 0x%08x", vpbr_ipv4_structure->new_vrf);
        }
    }

    if(verbose >= 2) {
        printf(",DIP Addr = ");
        _l3_print_ip_addr(vpbr_ipv4_structure->dip_addr);
        printf(",DIP Mask = 0x%08x", vpbr_ipv4_structure->dip_mask);
        printf(",SIP Addr = ");
        _l3_print_ip_addr(vpbr_ipv4_structure->sip_addr);
        printf(",SIP Mask = 0x%08x", vpbr_ipv4_structure->sip_mask);
        printf(",priority = %d, flags =0x%08x", vpbr_ipv4_structure->priority,  vpbr_ipv4_structure->flags);
        if (vpbr_ipv4_structure->eth_rif_valid)
        {
            printf(",ETH RIF = 0x%08x", vpbr_ipv4_structure->eth_rif);
        }
        if (vpbr_ipv4_structure->dscp_valid)
        {
            printf(",DSCP = 0x%08x", vpbr_ipv4_structure->dscp);
        }
        if (vpbr_ipv4_structure->src_port_valid)
        {
            printf(",SRC Port = 0x%08x", vpbr_ipv4_structure->src_port);
        }
        if (vpbr_ipv4_structure->dst_port_valid)
        {
            printf(",DST Port = 0x%08x", vpbr_ipv4_structure->dst_port);
        }
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}

/*
 * L3 port object -- start
 */
/*
 * Struct for L3 port
 */
struct sand_utils_l3_port_s
{
    /* IN */
    bcm_gport_t port;
    int eth_rif_id; /* L3 VSI interface */    
    bcm_gport_t ing_vlan_port_id; /* GPORT identifier */
    bcm_gport_t eg_vlan_port_id; /* GPORT identifier */
};
/* Initilize L3-port structure to all 0s */
void sand_utils_l3_port_s_init(int unit, uint32 flags, sand_utils_l3_port_s *l3_port_structure)
{
    sal_memset(l3_port_structure, 0, sizeof (*l3_port_structure));
}
/* Initilize L3-port structure with common params */
void sand_utils_l3_port_s_common_init(int unit, uint32 flags, sand_utils_l3_port_s *l3_port_structure, bcm_gport_t port, uint32 eth_rif_id)
{
    sand_utils_l3_port_s_init(unit, 0, l3_port_structure);

    l3_port_structure->port = port;
    l3_port_structure->eth_rif_id = eth_rif_id;
}

int sand_utils_l3_port_set(int unit, sand_utils_l3_port_s *l3_port_structure)
{
    bcm_vlan_port_t vlan_port;
    int rc;

    /* Ingress */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = l3_port_structure->port;
    vlan_port.vsi = l3_port_structure->eth_rif_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rc = bcm_vlan_port_create(unit, &vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rc;
    }
    l3_port_structure->ing_vlan_port_id = vlan_port.vlan_port_id;

    /* Egress */
    if (!l3_port_structure->eg_vlan_port_id)
    {
        bcm_vlan_port_t vlan_port2;
        bcm_vlan_port_t_init(&vlan_port2);
        vlan_port2.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port2.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
        if (is_device_or_above(unit, JERICHO2))
        {
            vlan_port2.flags |= BCM_VLAN_PORT_DEFAULT;
            vlan_port2.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        }
        rc = bcm_vlan_port_create(unit, &vlan_port2);
        if (rc != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create\n");
            return rc;
        }
        l3_port_structure->eg_vlan_port_id = vlan_port2.vlan_port_id;
    }

    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = l3_port_structure->port;
    rc = bcm_port_match_add(unit, l3_port_structure->eg_vlan_port_id, &match_info);
    if (rc != 0)
    {
        printf("Error, in bcm_port_match_add \n");
        return rc;
    }
    default_egr_vlan_port_id_out = l3_port_structure->eg_vlan_port_id;
    rc = bcm_vlan_gport_add(unit, l3_port_structure->eth_rif_id, l3_port_structure->port, 0);
    if (rc != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_gport_add \n");
        return rc;
    }

    if(verbose >= 1) {
        printf("Set L3 port properties. Port = %d, eth_rif = 0x%08X", l3_port_structure->port, l3_port_structure->eth_rif_id);
    }

    if(verbose >= 2) {
        printf(", in_lif = 0x%08x, out_lif = 0x%08x", vlan_port.vlan_port_id, l3_port_structure->eg_vlan_port_id);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}
/*
 * L3 port object -- end
 */

/*
 * create l3 interface - ingress/egress
 *  Creates Router interface
 *  - packets sent in from this interface identified by <port, vlan> with specificed MAC address is subject of routing
 *  - packets sent out through this interface will be encapsulated with <vlan, mac_addr>
 *  Parmeters:
 *  - flags: special controls set to zero.
 *  - open_vlan - if TRUE create given vlan, FALSE: vlan already opened juts use it
 *  - port - where interface is defined
 *  - vlan - router interface vlan
 *  - vrf - VRF to map to.
 *  - mac_addr - my MAC
 *  - intf - returned handle of opened l3-interface
 */
int l3__intf_rif__create(int unit, create_l3_intf_s *l3_intf){
    bcm_l3_intf_t l3if, l3if_old;
    int rc, station_id;
    bcm_l2_station_t station;
    bcm_l3_ingress_t l3_ing_if;
    int is_urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);
    int enable_public = 0;

    /* Initialize a bcm_l3_intf_t structure. */
    bcm_l3_intf_t_init(&l3if);
    bcm_l3_intf_t_init(&l3if_old);
    bcm_l2_station_t_init(&station);
    bcm_l3_ingress_t_init(&l3_ing_if);

    /*In JR2, my MAC handled solely in bcm_l3_intf_create*/
    if (!l3_intf->skip_mymac&&!is_device_or_above(unit, JERICHO2)) {
        /* set my-Mac global MSB */
        station.flags = 0;
        sal_memcpy(station.dst_mac, l3_intf->my_global_mac, 6);
        station.src_port_mask = 0; /* port is not valid */
        station.vlan_mask = 0; /* vsi is not valid */
        station.dst_mac_mask[0] = 0xff; /* dst_mac my-Mac MSB mask is -1 */
        station.dst_mac_mask[1] = 0xff;
        station.dst_mac_mask[2] = 0xff;
        station.dst_mac_mask[3] = 0xff;
        station.dst_mac_mask[4] = 0xff;
        station.dst_mac_mask[5] = 0xff;

        rc = bcm_l2_station_add(unit, &station_id, &station);
        if (rc != BCM_E_NONE) {
            printf("Error, in bcm_l2_station_add\n");
            return rc;
        }
    }

    l3if.l3a_flags = BCM_L3_WITH_ID; /* WITH-ID or without-ID does not really matter. Anyway for now RIF equal VSI */
    if ((l3_intf->no_publc == 0) &&
        (/* Update the in_rif to have public searches enabled for vrf == 0 */
         (l3_intf->vrf == 0 && is_device_or_above(unit,JERICHO)) ||
         /* This feature changes the default non-public non-RPF program behavior, change in_rifs to use the public program instead */
         ((soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_LONG, 0)) ||
          (soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH_IPV6_SHORT, 0)) ||
          (soc_property_get(unit, spn_ENHANCED_FIB_SCALE_PREFIX_LENGTH, 0))
          )
         )
        )
    {
        l3_intf->vrf_valid = 1;
        enable_public = 1;
    }

    l3if.l3a_vid = l3_intf->vsi;
    l3if.l3a_ttl = 31; /* default settings */
    if (l3_intf->ttl_valid) {
        l3if.l3a_ttl = l3_intf->ttl;
    }
    l3if.l3a_mtu = 1524; /* default settings */
    if (l3_intf->mtu_valid) {
        l3if.l3a_mtu = l3_intf->mtu;
        l3if.l3a_mtu_forwarding = l3_intf->mtu_forwarding;
    }
    l3if.native_routing_vlan_tags = l3_intf->native_routing_vlan_tags;
    l3_intf->rif = l3if.l3a_intf_id = l3_intf->vsi; /* In DNX Arch VSI always equal RIF */

    sal_memcpy(l3if.l3a_mac_addr, l3_intf->my_lsb_mac, 6);
    sal_memcpy(l3if.l3a_mac_addr, l3_intf->my_global_mac, 4); /* ovewriting 4 MSB bytes with global MAC configuration*/
    l3if.native_routing_vlan_tags = l3_intf->native_routing_vlan_tags;


    l3if_old.l3a_intf_id = l3_intf->vsi;
    rc = bcm_l3_intf_get(unit, &l3if_old);
    if (rc == BCM_E_NONE) {
        /* if L3 INTF already exists, replace it*/
        l3if.l3a_flags = l3if.l3a_flags | BCM_L3_REPLACE;
        printf("intf 0x%x already exist, just replacing: bcm_l3_intf_create\n",l3if_old.l3a_intf_id);
    }

    if (l3_intf->qos_map_valid) {
        l3if.dscp_qos.qos_map_id = l3_intf->qos_map_id;
    }

    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create %d\n",rc);
        return rc;
    }

    if (l3_intf->vrf_valid || l3_intf->rpf_valid) {
        l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
        l3_ing_if.vrf = l3_intf->vrf;

        /* set RIF enable RPF*/
        /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
        if (l3_intf->rpf_valid && !is_urpf_mode_per_rif) {
            /* Set uRPF global configuration */
            rc = bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, l3_intf->urpf_mode);
            if (rc != BCM_E_NONE) {
                return rc;
            }
        }
        if (l3_intf->flags & BCM_L3_RPF) {
            l3_ing_if.urpf_mode = l3_intf->urpf_mode;
        } else {
            l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;
        }

        if ((l3_intf->ingress_flags & BCM_L3_INGRESS_GLOBAL_ROUTE) || ((enable_public == 1) && is_device_or_above(unit,JERICHO))) {
            l3_ing_if.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_DSCP_TRUST) {
            l3_ing_if.flags |= BCM_L3_INGRESS_DSCP_TRUST;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST) {
            l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_UCAST;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST) {
            l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP6_UCAST;
        }
        if (l3_intf->ingress_flags & BCM_L3_INGRESS_ROUTE_DISABLE_MPLS) {
            l3_ing_if.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_MPLS;
        }

        if (l3_intf->qos_map_valid) {
            l3_ing_if.qos_map_id = l3_intf->qos_map_id;
        }

        if (l3_intf->oam_default_profile) {
            l3_ing_if.oam_default_profile = l3_intf->oam_default_profile;
        }

        rc = bcm_l3_ingress_create(unit, l3_ing_if, l3if.l3a_intf_id);
        if (rc != BCM_E_NONE) {
            printf("Error, bcm_l3_ingress_create\n");
            return rc;
        }
        l3_intf->rif = l3if.l3a_intf_id;
    }

    printf("created ingress interface = 0x%08x, on vlan = %d in unit %d, vrf = %d\n",l3_intf->rif,l3_intf->vsi,unit,l3_intf->vrf);
    printf("mac-address: %02x:%02x:%02x:%02x:%02x:%02x\n\r",l3_intf->my_global_mac[0],l3_intf->my_global_mac[1],l3_intf->my_global_mac[2],
        l3_intf->my_global_mac[3],l3_intf->my_global_mac[4],l3_intf->my_global_mac[5]);

    return rc;
}

/* Create egress object(FEC and ARP entry)*/
/*int l3__egress__create(int unit, uint32 flags,  int out_port, int vlan, int l3_eg_intf, bcm_mac_t next_hop_mac_addr, int *intf, int *encap_id){*/
int l3__egress__create(int unit, create_l3_egress_s *l3_egress) {
    int rc;
    if (!is_device_or_above(unit, JERICHO2)){
        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;
        bcm_l3_egress_t_init(&l3eg);

        /* FEC properties */
        l3eg.intf           = l3_egress->out_tunnel_or_rif;
        l3eg.port           = l3_egress->out_gport;
        l3eg.encap_id       = l3_egress->arp_encap_id;
        /* FEC properties - protection */
        l3eg.failover_id    = l3_egress->failover_id;
        l3eg.failover_if_id = l3_egress->failover_if_id;

        /* ARP */
        l3eg.mac_addr   = l3_egress->next_hop_mac_addr;
        l3eg.vlan       = l3_egress->vlan;
        l3eg.qos_map_id = l3_egress->qos_map_id;

        l3eg.flags    = l3_egress->l3_flags ;
        l3eg.flags2   = l3_egress->l3_flags2;
        l3egid        = l3_egress->fec_id;

        rc = bcm_l3_egress_create(unit, l3_egress->allocation_flags, &l3eg, &l3egid);
        if (rc != BCM_E_NONE) {
            printf("Error, create egress object, unit=%d, \n", unit);
            return rc;
        }

        l3_egress->fec_id = l3egid;
        l3_egress->arp_encap_id = l3eg.encap_id;

        if(verbose >= 1) {
            printf("unit %d: created FEC-id =0x%08x, ", unit, l3_egress->fec_id);
            printf("encap-id = %08x", l3_egress->arp_encap_id );
        }

        if(verbose >= 2) {
            printf("outRIF = 0x%08x out-port =%d", l3_egress->out_tunnel_or_rif, l3_egress->out_gport);
        }

        if(verbose >= 1) {
            printf("\n");
        }
    } else {
        /*In JR2, FEC and ARP created sperately*/
        int arp_encap_id = 0;
        rc = l3__egress_only_encap__create(unit, l3_egress);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rc;
        }
        arp_encap_id = l3_egress->arp_encap_id;

        /*JR2, Set intf in FEC if it exists, otherwise arp_encap_id. But won't both unless intf is RIF. */
        if (BCM_L3_ITF_TYPE_IS_LIF(l3_egress->arp_encap_id) && BCM_L3_ITF_TYPE_IS_LIF(l3_egress->out_tunnel_or_rif)) {
            /** FEC can't carry two outlifs, if encap_id was created in above call, set it to 0 temporarily.*/
            l3_egress->arp_encap_id = 0;
        }
        rc = l3__egress_only_fec__create(unit, l3_egress);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress_only_fec__create\n");
            return rc;
        }

        /*Set back arp_encap_id*/
        l3_egress->arp_encap_id = arp_encap_id;
    }
    return rc;
}

/* Set an ARP entry, without allocating FEC entry*/
/*int l3__egress_only_encap__create(int unit, int out_port, int vlan, int l3_eg_intf, bcm_mac_t next_hop_mac_addr, int *encap_id){*/
int l3__egress_only_encap__create(int unit, create_l3_egress_s *l3_egress) {

    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null; /* not intersting */

    bcm_l3_egress_t_init(&l3eg);

    l3eg.flags       = l3_egress->l3_flags;
    l3eg.flags2      = l3_egress->l3_flags2;
    l3eg.mac_addr    = l3_egress->next_hop_mac_addr;
    l3eg.vlan        = l3_egress->vlan;
    l3eg.encap_id = l3_egress->arp_encap_id;
    if (BCM_L3_ITF_TYPE_IS_LIF(l3_egress->out_tunnel_or_rif)) {
        l3eg.intf = l3_egress->out_tunnel_or_rif;
    }
    l3eg.qos_map_id  = l3_egress->qos_map_id;

    rc = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid_null);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, \n");
        return rc;
    }

    l3_egress->arp_encap_id = l3eg.encap_id;

    if(verbose >= 1) {
        printf("encap-id = %08x\n", l3_egress->arp_encap_id);
    }

    return rc;
}

/* Set a FEC entry, without allocating ARP entry */
int l3__egress_only_fec__create(int unit, create_l3_egress_s *l3_egress) {
    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);

    /* FEC properties */
    /*In previous devices, in case single Global Out-LIF pointer in FEC entry was required,
       we could placed it in either intf or encap_id fields. In JR2, it is possible to place it in intf field only.
       encap_id can not be used when only one single Out-LIF is required. */
    if (is_device_or_above(unit, JERICHO2)) {
        l3eg.intf = l3_egress->out_tunnel_or_rif ? l3_egress->out_tunnel_or_rif : l3_egress->arp_encap_id;
        l3eg.encap_id = l3_egress->out_tunnel_or_rif ? l3_egress->arp_encap_id : 0;
    } else {
        l3eg.encap_id     = l3_egress->arp_encap_id;
        l3eg.intf         = l3_egress->out_tunnel_or_rif;
    }
    l3eg.port           = l3_egress->out_gport;
    l3eg.vlan           = l3_egress->vlan;
    /* FEC properties - protection */
    l3eg.failover_id    = l3_egress->failover_id;
    l3eg.failover_if_id = l3_egress->failover_if_id;

    l3eg.flags    = l3_egress->l3_flags ;
    l3eg.flags2   = l3_egress->l3_flags2;
    l3egid        = l3_egress->fec_id;

    rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rc;
    }

    l3_egress->fec_id = l3egid;

    if(verbose >= 1) {
        printf("unit %d: created FEC-id =0x%08x, ", unit, l3_egress->fec_id);
    }

    if(verbose >= 2) {
        printf("outRIF = 0x%08x, encap = 0x%08x, out-port =%d", l3_egress->out_tunnel_or_rif,  l3_egress->arp_encap_id, l3_egress->out_gport);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}

