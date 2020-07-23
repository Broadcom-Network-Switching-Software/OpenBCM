/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides L3 basic functionality and defines L3 global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */

/* Struct definitions */
struct l3_ipv4_route_entry_utils_s
{
    uint32 address;
    uint32 mask;
    bcm_ip6_t address6;
    bcm_ip6_t mask6;
    uint8 use_ipv6;
};

l3_ipv4_route_entry_utils_s g_l3_route_entry_utils    = { 0x01010100, /* address */
                                                          0xffffffff /*  mask    */ };



 /* ************************************************************************************************** */

/* Delete all l3 Interfaces*/
int l3__intf__delete_all(int unit){
    int rc;

    rc=bcm_l3_intf_delete_all(unit);

    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_delete_all\n");
    }

    return rc;
}

/* Delete l3 Interface*/
int l3__intf__delete(int unit, int intf)
{
    int rc;
    bcm_l3_intf_t l3if;

    bcm_l3_intf_t_init(l3if);

    l3if.l3a_intf_id = intf;

    rc = bcm_l3_intf_delete(unit, l3if);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_delete\n");
    }

    return rc;
}



/* Creating EEI with MPLS Label and Push Command
 * No Out-Lif created.
 */
int l3_egress__mpls_push_command__create(int unit, create_l3_egress_with_mpls_s *l3_egress) {
    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);

    /* FEC properties */
    l3eg.intf           = l3_egress->out_tunnel_or_rif;
    l3eg.port           = l3_egress->out_gport;

    /* FEC properties - protection */
    l3eg.failover_id    = l3_egress->failover_id;
    l3eg.failover_if_id = l3_egress->failover_if_id;

    l3eg.flags    = l3_egress->l3_flags;
    l3eg.flags2   = l3_egress->l3_flags2;
    l3egid        = l3_egress->fec_id;

    l3eg.mpls_flags     = l3_egress->mpls_flags;
    l3eg.mpls_label     = l3_egress->mpls_label;
    l3eg.mpls_action    = l3_egress->mpls_action;
    l3eg.mpls_ttl       = l3_egress->mpls_ttl;
    l3eg.mpls_exp       = l3_egress->mpls_exp;

    rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | l3_egress->allocation_flags, &l3eg, &l3egid);
    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, unit=%d, \n", unit);
        return rc;
    }

    l3_egress->fec_id = l3egid;
    l3_egress->arp_encap_id = l3eg.encap_id;

    if(verbose >= 1) {
        printf("unit %d: created FEC-id =0x%08x, ", unit, l3_egress->fec_id);
        printf("encap-id = %08x", l3_egress->arp_encap_id );
        printf("INSIDE UTIL\n");
    }

    if(verbose >= 2) {
        printf("outRIF = 0x%08x out-port =%d", l3_egress->out_tunnel_or_rif, l3_egress->out_gport);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}


/* update rpf according to flag
   and update vrf */
int l3__update_rif_vrf_rpf(int unit, bcm_l3_intf_t* intf, int vrf, int rpf_flag) {

    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_intf_t aux_intf;

    aux_intf.l3a_vid = intf->l3a_vid;
    aux_intf.l3a_mac_addr = intf->l3a_mac_addr;
    rv = bcm_l3_intf_find(unit, &aux_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_find\n");
        return rv;
    }

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;  /* must, as we update exist RIF */


    if (rpf_flag & BCM_L3_RPF) {
      ingress_intf.urpf_mode = L3_uc_rpf_mode; /* RPF mode is loose has to match global configuration or Disabled */
    }
    else{
      ingress_intf.urpf_mode = bcmL3IngressUrpfDisable;
    }
    ingress_intf.vrf = vrf;

    rv = bcm_l3_ingress_create(unit, &ingress_intf, intf->l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;

}


/* add ipv4 route entry to FEC*/
int l3__ipv4_route__add(int unit, l3_ipv4_route_entry_utils_s route_entry, int vrf, int intf) {

    int rc;
    bcm_l3_route_t l3rt;

    bcm_l3_route_t_init(l3rt);

    if (!is_device_or_above(unit, JERICHO2)) {
        l3rt.l3a_flags |= BCM_L3_RPF;
    }

    if (route_entry.use_ipv6) {
        sal_memcpy(l3rt.l3a_ip6_mask, route_entry.mask6, sizeof(route_entry.mask6));
        sal_memcpy(l3rt.l3a_ip6_net, route_entry.address6, sizeof(route_entry.address6));
        l3rt.l3a_flags |= BCM_L3_IP6;
    } else {
        l3rt.l3a_subnet = route_entry.address;
        l3rt.l3a_ip_mask = route_entry.mask;
    }
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %x \n", rc);
    }

    if(verbose >= 1) {
        printf("l3__ipv4_route__add address:0x%x, mask:0x%x, vrf:%d ", route_entry.address, route_entry.mask,vrf);
        printf("---> egress-object=0x%08x, \n", intf);
    }

    return rc;
}

/* add ipv4 route entry to host table */
int l3__ipv4_route_to_overlay_host__add(int unit, uint32 ipv4_address, int vrf, int encap_id, int intf, int fec)
{

    int rc;

    bcm_l3_host_t l3_host;
    bcm_l3_host_t_init(l3_host);

    /* key of host entry */
    l3_host.l3a_vrf = vrf;               /* router interface */
    l3_host.l3a_ip_addr = ipv4_address;  /* ip host entry */

    /* data of host entry */
    l3_host.l3a_port_tgid = fec;         /* overlay tunnel: vxlan gport */
    l3_host.l3a_intf = intf;             /* out rif */
    l3_host.encap_id = encap_id;         /* arp pointer: encap/eei entry */

    rc = bcm_l3_host_add(unit, &l3_host);
    print l3_host;
    if (rc != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");
    }

    return rc;
}


/*
 * Add Route default
 */
int l3__ipv6_route__add(int *units_ids, int nof_units) {

  int rc;
  bcm_l3_route_t l3rt;
  bcm_ip6_t addr_int;
  bcm_ip6_t mask_int;
  int unit, i;

  bcm_l3_route_t_init(&l3rt);

  /* UC IPV6 DIP: */
  addr_int[15]= 0; /* LSB */
  addr_int[14]= 0;
  addr_int[13]= 0;
  addr_int[12]= 0;
  addr_int[11]= 0x7;
  addr_int[10]= 0xdb;
  addr_int[9] = 0;
  addr_int[8] = 0;
  addr_int[7] = 0;
  addr_int[6] = 0x70;
  addr_int[5] = 0;
  addr_int[4] = 0x35;
  addr_int[3] = 0;
  addr_int[2] = 0x16;
  addr_int[1] = 0;
  addr_int[0] = 0x1; /* MSB */

  /* UC IPV6 DIP MASK: */
  mask_int[15]= 0;
  mask_int[14]= 0;
  mask_int[13]= 0;
  mask_int[12]= 0;
  mask_int[11]= 0;
  mask_int[10]= 0;
  mask_int[9] = 0;
  mask_int[8] = 0;
  mask_int[7] = 0xff;
  mask_int[6] = 0xff;
  mask_int[5] = 0xff;
  mask_int[4] = 0xff;
  mask_int[3] = 0xff;
  mask_int[2] = 0xff;
  mask_int[1] = 0xff;
  mask_int[0] = 0xff;

  sal_memcpy(&(l3rt.l3a_ip6_net),(addr_int),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask_int),16);
  l3rt.l3a_vrf = default_vrf;
  l3rt.l3a_intf = ipv6_fap_default_intf;

  l3rt.l3a_flags = BCM_L3_IP6;
  l3rt.l3a_modid = 0;
  l3rt.l3a_port_tgid = 0;

  for (i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];

      rc = bcm_l3_route_add(unit, l3rt);
      if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %d \n", rc);
      }
  }

  return rc;
}

/*
 * get hit bit after running bcm_l3_egress_get
 */
int L3EgressGetHit(int unit, int intf_id,int clear) {

    if((clear != 0) && (clear != 1)){
        return 0;
    }

    int rv;
    bcm_l3_egress_t intf_temp1;
    bcm_l3_egress_t_init(&intf_temp1);

    if(clear == 1){
        intf_temp1.flags = BCM_L3_HIT_CLEAR;
    }

    rv = bcm_l3_egress_get(unit,intf_id,&intf_temp1);

    if(rv != BCM_E_NONE){
        return -1;
    }

    else {
        if ((intf_temp1.flags & BCM_L3_HIT) != 0){
            return 1;
        }
        else {
            return 0;
        }
    }
}

