/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
* File: cint_vlan_control_config.c
* Purpose: Example of VLAN control configurations
*
* Calling sequence:
*  1. Create vlan, Add port and Set the flood group for basic bridging.
*        - Call bcm_vlan_create()
*        - Call bcm_vlan_port_add()
*        - Call bcm_vlan_control_vlan_set()
*        - Call bcm_multicast_create()
*        - Call bcm_multicast_ingress_add()
*  2. Create vlan, Add port, and Create Vsi upper than 4K and Set the flood group for basic bridging.
*        - Call bcm_vlan_create()
*        - Call bcm_vlan_port_add()
*        - Call bcm_vswitch_create()
*        - Call bcm_vlan_control_vlan_set()
*        - Call bcm_multicast_create()
*        - Call bcm_multicast_ingress_add()
*
* Traffic:
*  1. For case Vlan=10 : 
*	a) unknow unicast group=unknow multicast group=broadcast group=10.
*	b) unknow unicast group=10; unknow multicast group=4096+10; broadcast group=8192+10.
*  unkown unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is transmitted to port 2.
*
*  unkown multicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknow multicast DA 01:00:5E:01:01:01
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is transmitted to port 2.
*
*  broadcast 
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA FF:FF:FF:FF:FF:FF
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is flooded to port 2(as DA is not known).
*
*  2. For case Vlan=10 Vsi=4096: 
*	a) unknow unicast group=unknow multicast group=broadcast group=4096.
*	b) unknow unicast group=4096; unknow multicast group=4096+4096; broadcast group=8192+4096.
*  unkown unicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is transmitted to port 2.
*
*  unkown multicast:
*      -   Send Ethernet packet  to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknow multicast DA 01:00:5E:01:01:01
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is transmitted to port 2.
*
*  broadcast 
*      -   Send Ethernet packet to port 1:
*          -   SA 00:00:00:00:00:05
*          -   DA FF:FF:FF:FF:FF:FF
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is flooded to port 2(as DA is not known).
*
* To Activate Above Settings Run: 
*      BCM> cint examples/dpp/utility/cint_utils_multicast.c
*      BCM> cint examples/dpp/cint_vlan_control_config.c
*      BCM> cint
*      cint> vlan_control_config_run(unit, vid, vpn_lower_4k, dflt_frwrd); 
* 
* Please note: 
* 1. In ARAD, dflt_frwrd must be set to 1. 
* 2. In order to set various default forwarding modes, e.g unknown unicast, unknown multicast and broadcast ,use :
* bcmPortControlFloodUnknownUcastGroup, bcmPortControlFloodUnknownMcastGroup, bcmPortControlFloodBroadcastGroup per port or VLAN-Port.
*/

struct vlan_control_config_s 
{
    int ports[10];
    int nof_ports;
	int vsi;
	int mc_group;
	int bc_group;
};

vlan_control_config_s vlan_control_info;

int vlan_control_config_init(int unit, int port1, int port2){
    vlan_control_info.ports[0] = port1;
    vlan_control_info.ports[1] = port2;

    vlan_control_info.nof_ports = 2;

    return BCM_E_NONE;
}

int vlan_control_config_vpn_lower_4k(int unit, int vid, int dflt_frwrd)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int port_id;
    bcm_vlan_control_vlan_t vsi_control;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    for (port_id = 0; port_id < vlan_control_info.nof_ports; port_id++) {
        BCM_PBMP_PORT_ADD(pbmp, vlan_control_info.ports[port_id]);
    }
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    bcm_vlan_control_vlan_t_init(&vsi_control);

    vsi_control.forwarding_vlan = vid;

    if (dflt_frwrd) {
        vsi_control.unknown_unicast_group    = vid;
        vsi_control.unknown_multicast_group  = vid;
        vsi_control.broadcast_group          = vid;
    }
    else {
        vsi_control.unknown_unicast_group    = vid;
        vsi_control.unknown_multicast_group  = vid+4096;
        vsi_control.broadcast_group          = vid+8192;
    }

    rv = bcm_vlan_control_vlan_set(unit, vid, vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }

	vlan_control_info.mc_group = vsi_control.unknown_multicast_group;
	vlan_control_info.bc_group = vsi_control.broadcast_group;

    /* On JR2, need to create multicast group and add ports via multicast APIs*/
    if (is_device_or_above(unit, JERICHO2)){
        bcm_multicast_t mc_group;
        bcm_multicast_replication_t rep_array;
        uint32 max_nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
        int flags;
        mc_group = vid;

        rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID| BCM_MULTICAST_TYPE_L2, &mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP;

        for (port_id = 0; port_id < vlan_control_info.nof_ports; port_id++) {
            bcm_multicast_replication_t_init(&rep_array);
            rep_array.port = vlan_control_info.ports[port_id];
            rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x \n", mc_group, port_id);
                return rv;
            }
        }

        /* Configure the MC group to work with both cores */
        rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_group, mc_group);
        if (rv != BCM_E_NONE) {
           printf("Error, multicast__open_ingress_mc_group_for_egress_mc \n");
           return rv;
        }

    }
    return rv;
}

int vlan_control_config_vpn_upper_4k(int unit, int vid, int dflt_frwrd)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vsi;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int port_id;
    bcm_vlan_control_vlan_t vsi_control;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    /* add port to vlan */
    BCM_PBMP_CLEAR(pbmp);
    for (port_id = 0; port_id < vlan_control_info.nof_ports; port_id++) {
        BCM_PBMP_PORT_ADD(pbmp, vlan_control_info.ports[port_id]);
    }
    BCM_PBMP_CLEAR(ubmp);
    rv = bcm_vlan_port_add(unit, vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vid, rv);
        return rv;
    }

    rv = bcm_vswitch_create(unit, &vsi);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_vswitch_create\n");
      return rv;
    }

	int cuds[2]={0};
	rv = multicast__open_ingress_mc_group_with_gports(unit, vsi, vlan_control_info.ports, cuds, vlan_control_info.nof_ports, BCM_MULTICAST_TYPE_L2);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
        return rv;
    }

    bcm_vlan_control_vlan_t_init(&vsi_control);

    vsi_control.forwarding_vlan = vsi;
    
    if (dflt_frwrd) {
        vsi_control.unknown_unicast_group    = vsi;
        vsi_control.unknown_multicast_group  = vsi;
        vsi_control.broadcast_group          = vsi;
    }
    else {
        vsi_control.unknown_unicast_group    = vsi;
        vsi_control.unknown_multicast_group  = vsi+4096;
        vsi_control.broadcast_group          = vsi+8192;
    }

    rv = bcm_vlan_control_vlan_set(unit, vsi, vsi_control);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_vlan_set\n");
        return rv;
    }

	vlan_control_info.vsi = vsi;
	vlan_control_info.mc_group = vsi_control.unknown_multicast_group;
	vlan_control_info.bc_group = vsi_control.broadcast_group;

    /* On JR2, need to create multicast group and add ports via multicast APIs*/
    if (is_device_or_above(unit, JERICHO2)){
        bcm_multicast_t mc_group;
        bcm_multicast_replication_t rep_array;
        int flags;
        mc_group = vid;

        rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID| BCM_MULTICAST_TYPE_L2, &mc_group);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create \n");
            return rv;
        }
        flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP;

        for (port_id = 0; port_id < vlan_control_info.nof_ports; port_id++) {
            bcm_multicast_replication_t_init(&rep_array);
            rep_array.port = vlan_control_info.ports[port_id];
            rv = bcm_multicast_add(unit, mc_group, flags, 1, &rep_array);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_multicast_add mc_group_id:  0x%08x  port:  0x%08x \n", mc_group, port_id);
                return rv;
            }
        }

        rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_group, mc_group);
        if (rv != BCM_E_NONE) {
           printf("Error, multicast__open_ingress_mc_group_for_egress_mc \n");
           return rv;
        }
    }
    return rv;
}

int vlan_control_config_revert(int unit, bcm_vlan_t vid, int vpn_lower_4k, int dflt_frwrd)
{
    int rv = BCM_E_NONE;

	rv = bcm_vlan_destroy(unit, vid);
	if (rv != BCM_E_NONE){
		printf("Error, bcm_vlan_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
		return rv;
	}

	if (!vpn_lower_4k)
	{
	    rv = bcm_vswitch_destroy(unit, vlan_control_info.vsi);
	    if (rv != BCM_E_NONE) {
	      printf("Error, bcm_vswitch_create\n");
	      return rv;
	    }

		rv = bcm_multicast_destroy(unit, vlan_control_info.vsi);
		if (rv != BCM_E_NONE){
			printf("Error, bcm_multicast_destroy unit %d, mc_group %d, rv %d\n", unit, vlan_control_info.vsi, rv);
			return rv;
		}
	}

	if (!dflt_frwrd)
	{			
		rv = bcm_multicast_destroy(unit, vlan_control_info.mc_group);
		if (rv != BCM_E_NONE){
			printf("Error, bcm_multicast_destroy unit %d, mc_group %d, rv %d\n", unit, vlan_control_info.mc_group, rv);
			return rv;
		}

		rv = bcm_multicast_destroy(unit, vlan_control_info.bc_group);
		if (rv != BCM_E_NONE){
			printf("Error, bcm_multicast_destroy unit %d, bc_group %d, rv %d\n", unit, vlan_control_info.bc_group, rv);
			return rv;
		}
	} else if (is_device_or_above(unit, JERICHO2)){
        rv = bcm_multicast_destroy(unit,vid);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_multicast_destroy unit %d, vid %d, rv %d\n", unit, vid, rv);
            return rv;
        }
    }

	return rv;
}

int vlan_control_config_run(int unit, bcm_vlan_t vid, int vpn_lower_4k, int dflt_frwrd)
{
    int rv = BCM_E_NONE;
	int cuds[2]={0};

    if (vpn_lower_4k) {
        rv = vlan_control_config_vpn_lower_4k(unit, vid, dflt_frwrd);
    }
    else {
        rv = vlan_control_config_vpn_upper_4k(unit, vid, dflt_frwrd);
    }

    if (!dflt_frwrd)
    {
		rv = multicast__open_ingress_mc_group_with_gports(unit, vlan_control_info.mc_group, vlan_control_info.ports, cuds, vlan_control_info.nof_ports, BCM_MULTICAST_TYPE_L2);
		if (rv != BCM_E_NONE) {
			printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
			return rv;
		}

		rv = multicast__open_ingress_mc_group_with_gports(unit, vlan_control_info.bc_group, vlan_control_info.ports, cuds, vlan_control_info.nof_ports, BCM_MULTICAST_TYPE_L2);
		if (rv != BCM_E_NONE) {
			printf("Error, multicast__open_ingress_mc_group_with_local_ports \n");
			return rv;
		}
    }

    return rv;
}

