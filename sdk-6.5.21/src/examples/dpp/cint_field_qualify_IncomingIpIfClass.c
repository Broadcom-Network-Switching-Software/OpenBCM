/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

int set_in_rif_profile(int unit,int inPort, int outPort, int in_rif_profile_set )
{
    int rv= BCM_E_NONE;
    int vrf;
    int fecId;
    bcm_l3_intf_t intf;
    bcm_l3_ingress_t ingress_intf;
	
	/* must be the same mac address as the mac in
	*  AT_Cint_field_qualify_IncomingIpIfClass
	*  my_mac_address = MyMac
	*  out_mac = outMac
	*/
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t out_mac         = {0x00, 0x45, 0x45, 0x45, 0x45, 0x00};
	
    bcm_l3_egress_t l3eg;
    bcm_l3_host_t l3host;
    uint32 IPaddr             = 0x22664500;
	
    vrf = 300 ;
    /* create L3 interface */
    bcm_l3_intf_t_init(&intf);
    sal_memcpy(intf.l3a_mac_addr, my_mac_address, 6);
    intf.l3a_vid = vrf;
    intf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }
 
    /* create ingress */
    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_intf.vrf= vrf;
    rv = bcm_l3_ingress_create(unit, &ingress_intf, &intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
    }
 
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, out_mac, 6);
    l3eg.vlan   = 400;
    l3eg.port   = outPort;
    l3eg.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3eg, &fecId);
    if (rv != BCM_E_NONE) {
      printf ("bcm_l3_egress_create failed \n");
      return rv;
    }
 
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_RPF;
    l3host.l3a_ip_addr = IPaddr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fecId;
    l3host.encap_id = l3eg.encap_id;
 
    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rv);
       return rv;
    }
	
    rv= bcm_l3_ingress_get(unit,intf.l3a_intf_id,&ingress_intf);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_ingress_get failed \n");
        return rv;
    }
    print ingress_intf;

    /* setting the FG */
    field__qualify_IncomingIpIfClass(unit, ingress_intf.intf_class_route_disable, bcmFieldActionDrop,0,0);
 
    return 0;
 
}



