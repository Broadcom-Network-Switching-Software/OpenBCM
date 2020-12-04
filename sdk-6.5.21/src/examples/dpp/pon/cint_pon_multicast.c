/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_pon_multicast.c
 *
 * Purpose: An example of how to use BCM APIs to implement multicast function.
 *          There are 2 multicast group, one is for unknown-uc, unknown-mc and bc.
 *          Another is for known-mc.
 *
 * PP model:
 *     PON Port 1  <------------------------------->  VSI  <---> NNI Port 128
 *     Pon 1 Tunnel-ID 100 clvan 2000 --------------------|---4096  |----------- CVLAN 100
 *     Pon 1 Tunnel-ID 101 clvan 2001 --------------------|
 *
 *     Pon 1 Tunnel-ID 1000 multicast vlan 4001 ------ ---|---4097  |----------- multicast vlan 4001
 *
 *
 * Calling sequence:
 *
 * Initialization:
 *
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_multicast.c
 *      BCM> cint
 *      cint> 
 *      cint> print pon_mc_app(unit, pon1, nni1, nni2);
 *      cint> print pon_mc_group_create(unit);
 *      cint> print pon_join_group(unit, port, gport);
 *      cint> print pon_leave_group(unit, port, gport);
 * To clean all above configuration:
 *      cint> print pon_leave_group(unit, port, gport);
 *      cint> print pon_mc_app_clean(unit);
 */

struct pon_mc_info_s {
    bcm_vlan_t  vsi;
    bcm_vlan_t  multicast_vsi;
    bcm_gport_t pon_gport[3];
    bcm_gport_t nni_gport;
    bcm_gport_t multicast_nni_gport;
    bcm_port_t pon_port;
    bcm_port_t nni_port;
    bcm_port_t pon_port2;
    bcm_port_t nni_port2;
    int tunnel_id[3]; 
    int nni_cvlan;
    int nni_multcast_vlan;
    int nni_multicast_vlan;
    int pon_cvlan[3];
    int lif_offset;
};

enum pon_mc_pon_lif_e {
    onu1_lif = 0,  /* ONU1 PON LIF */
    onu2_lif,      /* ONU2 PON LIF */
    mc_lif         /* PON LIF for down multicast */
};


pon_mc_info_s pon_mc_info;
int g_mc_grp_id = 0x01000001;
bcm_mac_t mul_mac_onu1 = {0x01, 0x00, 0x5E, 0x01, 0x01, 0x01};
bcm_mac_t mul_mac_onu2 = {0x01, 0x00, 0x5E, 0x01, 0x01, 0x02};

/* Initialize PON application configurations.
 * Example: 
 */
void pon_mc_init(int unit, bcm_port_t pon_port1, bcm_port_t nni_port1, bcm_port_t nni_port2)
{
    sal_memset(&pon_mc_info, 0, sizeof(pon_mc_info));
    pon_mc_info.pon_port = pon_port1;
    pon_mc_info.nni_port = nni_port1;
    pon_mc_info.tunnel_id[0] = 100; /* ONU1 */
    pon_mc_info.tunnel_id[1] = 101; /* ONU2 */
    pon_mc_info.tunnel_id[2] = 1000; /* known mc */
    pon_mc_info.nni_cvlan = 100;
    pon_mc_info.nni_multcast_vlan = 4001; /* known mc */
    pon_mc_info.pon_cvlan[0] = 2000; /* ONU1 */
    pon_mc_info.pon_cvlan[1] = 2001; /* ONU2 */
    pon_mc_info.pon_cvlan[2] = 4001; /* known mc  */
    pon_mc_info.nni_multicast_vlan = 4001;
    pon_mc_info.lif_offset = 0;

    pon_app_init(unit, pon_port1, nni_port1, nni_port2, 0);

}

/*********************************************************************************
* Setup pon test model(N:1)
*     PON Port 1  <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 100 clvan 2000 --------------------|---4096  |----------- CVLAN 100
*     Pon 1 Tunnel-ID 101 clvan 2001 --------------------|
*
*     Pon 1 Tunnel-ID 1000 multicast vlan 4001 ------ ---|---4097  |----------- multicast vlan 4001
*
* Input parameters
*     int unit              - unit number
*     bcm_port_t pon_port_1 - First PON port  
*     bcm_port_t nni_port_1 - First NNI port
*     bcm_port_t nni_port_2 - Second NNI port
*
* steps
 */
int pon_mc_app(int unit, bcm_port_t pon_port_1, bcm_port_t nni_port_1, bcm_port_t nni_port_2)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    /*bcm_gport_t trunk_gport;*/
    int action_id = 0;

    pon_mc_init(unit, pon_port_1, nni_port_1, nni_port_2);

    /*BCM_GPORT_TRUNK_SET(trunk_gport, nni_trunk_id);*/


    rv = vswitch_create(unit, &(pon_mc_info.vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    for (i = 0; i <= onu2_lif; i++)
    {
        rv = pon_lif_create(unit, 
                            pon_mc_info.pon_port, 
                            match_otag,
                            0, 0, 
                            pon_mc_info.tunnel_id[i],
                            pon_mc_info.tunnel_id[i],
                            pon_mc_info.pon_cvlan[i],
                            0,
                            0, 0, 
                           &(pon_mc_info.pon_gport[i]));

        if (rv != BCM_E_NONE)
        {
            printf("Error, pon_lif_create returned %s i:%d\n", bcm_errmsg(rv), i);
            return rv;
        }
        /* Add PON LIF to vswitch */
        rv = bcm_vswitch_port_add(unit, 
                                  pon_mc_info.vsi, 
                                  pon_mc_info.pon_gport[i]
                                 );
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_port_add returned %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* Add pon_gport for ONU1, ONU2 to mc for unkown uc, unknown mc and bc downstream */
        rv = multicast_vlan_port_add(unit, 
                                     pon_mc_info.vsi+lif_offset, 
                                     pon_mc_info.pon_port, 
                                     pon_mc_info.pon_gport[i]);
        if (rv != BCM_E_NONE)
        {
            printf("Error, multicast_vlan_port_add returned %s gport %d\n", bcm_errmsg(rv), pon_mc_info.pon_gport[i]);
            return rv;
        }

        /* Set PON LIF ingress VLAN editor */
        rv = pon_port_ingress_vt_set(unit,
                                     otag_to_otag2,
                                     pon_mc_info.nni_cvlan,
                                     0,
                                     pon_mc_info.pon_gport[i],
                                     0,
                                     &action_id
                                    );
        if (rv != BCM_E_NONE)
        {
            printf("Error, pon_port_ingress_vt_set returned %s\n", bcm_errmsg(rv));
            return rv;
        }                                             
        /* Set PON LIF egress VLAN editor */
        rv = pon_port_egress_vt_set(unit,
                                    otag_to_otag2,
                                    pon_mc_info.tunnel_id[i],
                                    pon_mc_info.pon_cvlan[i],
                                    0,
                                    pon_mc_info.pon_gport[i]
                                   );
        if (rv != BCM_E_NONE)
        {
            printf("Error, pon_port_egress_vt_set returned %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Create NNI LIF */
    rv = nni_lif_create(unit,
                     pon_mc_info.nni_port,
                     /*trunk_gport,*/
                     match_otag,
                     0,
                     pon_mc_info.nni_cvlan, 
                     0,
                     0,
                     &(pon_mc_info.nni_gport),
                     &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*For upstream*/
    rv = multicast_vlan_port_add(unit, 
                              pon_mc_info.vsi,
                              pon_mc_info.nni_port,
                              /*trunk_gport,*/
                              pon_mc_info.nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast_vlan_port_add nni_gport returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit,
                           pon_mc_info.vsi,
                           pon_mc_info.nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add nni_gport returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
    bcm_port_control_set(unit, pon_mc_info.nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
    bcm_port_control_set(unit, pon_mc_info.nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
    bcm_port_control_set(unit, pon_mc_info.nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);

    /*******************************
     * known multicast vsi and group
 */
    rv = vswitch_create(unit, &(pon_mc_info.multicast_vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create multicast_vsi returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    i = mc_lif;
    rv = pon_lif_create(unit, 
                        pon_mc_info.pon_port, 
                        match_otag,
                        0, 0, 
                        pon_mc_info.tunnel_id[i],
                        pon_mc_info.tunnel_id[i],
                        pon_mc_info.pon_cvlan[i],
                        0,
                        0, 0, 
                       &(pon_mc_info.pon_gport[i]));

    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create returned %s i:%d\n", bcm_errmsg(rv), i);
        return rv;
    }

    /* Create NNI LIF */
    rv = nni_lif_create(unit,
                     pon_mc_info.nni_port,
                     /*trunk_gport,*/
                     match_otag,
                     0,
                     pon_mc_info.nni_multicast_vlan, 
                     0,
                     0,
                     &(pon_mc_info.multicast_nni_gport),
                     &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit,
                           pon_mc_info.multicast_vsi,
                           pon_mc_info.multicast_nni_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add nni_gport returned %s\n", bcm_errmsg(rv));
        return rv;
    }

     /* In advance mode, need add tunnel ID by EVE */
    rv = pon_port_egress_vt_set(unit,
                                otag_to_otag2,
                                pon_mc_info.tunnel_id[i],
                                pon_mc_info.pon_cvlan[i],
                                0,
                                pon_mc_info.pon_gport[i]
                               );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_egress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


int pon_mc_group_create(int unit )
{	
	int rv;
	
	rv = open_ingress_mc_group(unit, g_mc_grp_id);
	if (rv != BCM_E_NONE) 
	{
    	printf("Error, open_ingress_mc_group returned %s\n", bcm_errmsg(rv));
    	return rv;
	}
	return rv;
}

int pon_mc_group_delete(int unit)
{	
	int rv;
	
	rv = bcm_multicast_destroy(unit, g_mc_grp_id);
	if (rv != BCM_E_NONE) 
	{
        printf("Error, bcm_multicast_destroy returned %s\n", bcm_errmsg(rv));
        return rv;
	}
	
	return rv;
}

int pon_add_mc_addr(int unit, bcm_mac_t mc_mac)
{
    bcm_l2_addr_t l2addr;
    int rv = 0;

    /* Add mc mac address for ONU1 */
    bcm_l2_addr_t_init(&l2addr, mc_mac, pon_mc_info.multicast_vsi);
    l2addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2addr.l2mc_group = g_mc_grp_id;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_add Down returned %s for ONU\n",
               bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int pon_delete_mc_addr(int unit, bcm_mac_t mc_mac)
{
    int rv = 0;

    /* Delete mc mac address for ONU1 */
    rv = bcm_l2_addr_delete(unit, mc_mac, pon_mc_info.multicast_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_delete Down returned %s for ONU\n",
               bcm_errmsg(rv));
        return rv;
    }

    return rv;
}



/*
 * Join a group
 */
int pon_join_group(int unit, int port, bcm_gport_t gport)
{	
	int rv = 0;

	rv = multicast_vlan_port_add(unit, g_mc_grp_id, port, gport);
	if (rv != BCM_E_NONE) 
	{
        printf("Error, multicast_vlan_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
	}

    rv = pon_add_mc_addr(unit, mul_mac_onu1);
	if (rv != BCM_E_NONE) 
	{
        printf("Error in ONU1, pon_add_mc_addr returned %s\n", bcm_errmsg(rv));
        return rv;
	}

    rv = pon_add_mc_addr(unit, mul_mac_onu2);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in ONU2, pon_add_mc_addr returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
 * Leave a group
 */
int pon_leave_group(int unit, int port, bcm_gport_t gport)
{
    int rv;

    rv = pon_delete_mc_addr(unit, mul_mac_onu1);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in ONU1, pon_delete_mc_addr returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = pon_delete_mc_addr(unit, mul_mac_onu2);
    if (rv != BCM_E_NONE) 
    {
        printf("Error in ONU2, pon_delete_mc_addr returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = multicast_vlan_port_del(unit, g_mc_grp_id, port, gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, multicast_vlan_port_del returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/*********************************************************************************
* clean all configuration. 
*
* Steps
*     1. clean pon service configuration
 */
int pon_mc_app_clean(int unit)
{
    int rv = 0;
    int i = 0;

    /* Delete pon gport */
    for (i = 0; i <= onu2_lif; i++)
    {
        rv = vswitch_delete_port(unit, pon_mc_info.vsi, pon_mc_info.pon_gport[i]);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, vswitch_delete_port() returned %s, remove pon gport[%d] %d failed\n", \
                   bcm_errmsg(rv), i, pon_mc_info.pon_gport[i]);
            return rv;
        }
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_mc_info.vsi, pon_mc_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s, remove nni gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_delete(unit, pon_mc_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete returned %s, remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }

    i = mc_lif;
    rv = vswitch_delete_port(unit, pon_mc_info.multicast_vsi, pon_mc_info.pon_gport[i]);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, multicast_vsi vswitch_delete_port() returned %s, remove pon gport[%d] %d failed\n", \
               bcm_errmsg(rv), i, pon_mc_info.pon_gport[i]);
        return rv;
    }
    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_mc_info.multicast_vsi, pon_mc_info.multicast_nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast_vsi vswitch_delete_port returned %s, remove nni gport failed\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = vswitch_delete(unit, pon_mc_info.multicast_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast_vsi vswitch_delete returned %s, remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }


    return rv;
}

