/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_dnx_qos_php.c
* Purpose: An example of the QoS mpls php. 
*
*          When PHP happened, QoS can work in UNIFORM or PIPE mode. 
*          If QOS works UNIFORM mode, then qos is the exp from popped label.
*          If QOS works PIPE mode, then qos is mapping from up layer qos according php qos profile, default profile 0 is 1:1 mapping.
*          The following CINT provides a calling sequence example for PHP QOS in three ways: FEC, tunnel switch, tunnel initiator. 
*
* Calling sequence:
*
*
* Set up sequence:
*  1. Create QOS profile and mapping
*        - Call bcm_qos_map_create() to create php qos profile with flag BCM_QOS_MAP_MPLS_PHP
*        - Call bcm_qos_map_create() to create opcode with flag BCM_QOS_MAP_MPLS_PHP
*        - Call bcm_qos_map_add() to mapping profile to opcode with layer protocol flag 
*        - Call bcm_qos_map_add() to qos value mapping with opcode
*  2. Create mpls php tunnel.
*  3. Create FEC
*  4. Create egress ARP encapsulation
*
* Traffic:
*  1.  IPoMPLSoETH: 
          TPID 0x8100, VID 100, label 100, IPv4.TOS
*  2.  output packet :
*           pipe model :      TPID 0x8100, VID 101, IPV4.TOS=IP.TOS+1
*           uniform model :   TPID 0x8100, VID 101, IPV4.TOS=EXP
*
* To Activate Above Settings Run:
*      BCM> cint src/examples/dnx/cint_dnx_qos_php.c
*      BCM> cint
*      cint> dnx_qos_php_service(unit,in_Port,out_Port,is_pipe,mode);
*/

/********** 
  Globals/Aux Variables
 */
int qos_php_map_id = 0;
int qos_php_map_opcode= 0;
int mpls_php_tunnel_id = 0;
int mpls_php_initiator_id = 0;
int fec_id_0 = 0;
int fec_id_1 = 0;
int arp_id = 0;
int vlan_port_id = 0;
int l3_in_intf_id = 0;
int l3_out_intf_id = 0;

bcm_mac_t my_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
bcm_mac_t next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x64};
int in_label =100;
int in_vlan=100;
int out_vlan=101;

/*
 *create php profile and add qos mapping
 */
int dnx_qos_mpls_php_qos_map(int unit)
{
    bcm_qos_map_t qos_php_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;
 
    /* Clear structure */
    bcm_qos_map_t_init(&qos_php_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_MPLS_PHP, &qos_php_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress php bcm_qos_map_create()\n");
        printf("(%s)\n", bcm_errmsg(rv));
    }
    print qos_php_map_id;

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_OPCODE, &qos_php_map_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in  mpls php opcode bcm_qos_map_create()\n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    print qos_php_map_opcode;

    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     */ 
    flags = BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_OPCODE;
    qos_php_map.opcode = qos_php_map_opcode;
    rv = bcm_qos_map_add(unit, flags, &qos_php_map, qos_php_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in mpls php bcm_qos_map_add()\n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    /*add current layer mapping */
    for (dscp=0; dscp<256; dscp++) {
        bcm_qos_map_t_init(&qos_php_map);

        flags = BCM_QOS_MAP_MPLS_PHP | BCM_QOS_MAP_IPV4;
        /* packet qos */
        qos_php_map.int_pri = dscp;
        /* Set new nwk_qos */
        if(dscp!=255)
        {
            qos_php_map.remark_int_pri = dscp+1;
        }
        else
        {
            qos_php_map.remark_int_pri = dscp;
        }

        rv = bcm_qos_map_add(unit, flags, &qos_php_map, qos_php_map_opcode);
        if (rv != BCM_E_NONE) {
             printf("error in mpls php bcm_qos_map_add()\n");
             printf("(%s)\n", bcm_errmsg(rv));
             return rv;
        }
    }
    return rv;
}

/*
 * create ac and L3 interface
 */
int dnx_qos_php_l2_l3_interface_create(int unit, int in_port)
{
    bcm_vlan_port_t vlan_port;
    bcm_l3_intf_t l3_intf;
    bcm_l3_ingress_t l3_ing_intf;
    int rv = BCM_E_NONE;

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.match_vlan = in_vlan;
    vlan_port.port = in_port;
    vlan_port.vsi = in_vlan;

    rv =  bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_vlan_port_create()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }
    print vlan_port.vlan_port_id;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = in_vlan;
    sal_memcpy(&l3_intf.l3a_mac_addr, my_mac,6);
    rv = bcm_l3_intf_create(unit, l3_intf);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_l3_intf_create()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }
    l3_in_intf_id = l3_intf.l3a_intf_id;

    bcm_l3_ingress_t_init(&l3_ing_intf);
    rv = bcm_l3_ingress_get(unit, l3_intf.l3a_intf_id, &l3_ing_intf);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_l3_ingress_get()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }
    l3_ing_intf.flags |= BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE;
    rv = bcm_l3_ingress_create(unit, l3_ing_intf, &l3_intf.l3a_intf_id);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_l3_ingress_create()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = out_vlan;
    sal_memcpy(&l3_intf.l3a_mac_addr, my_mac,6);
    rv = bcm_l3_intf_create(unit, l3_intf);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_l3_intf_create()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }
    l3_out_intf_id = l3_intf.l3a_intf_id;
    return rv;
}

/*
 * create php tunnel by API bcm_mpls_tunnel_switch_create
 */
int dnx_qos_php_tunnel_switch_php_create (int unit, int in_port, int is_pipe)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv = BCM_E_NONE;

    bcm_mpls_tunnel_switch_t_init(&entry);
    
    entry.action = BCM_MPLS_SWITCH_ACTION_PHP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    entry.label = 1000;
    if (is_pipe)
    {
        entry.egress_label.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        entry.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    }
    entry.port = in_port;
    entry.egress_if = fec_id_1;
    entry.qos_map_id = qos_php_map_id;
    
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_mpls_tunnel_switch_create()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }
    mpls_php_tunnel_id = entry.tunnel_id;
    print mpls_php_tunnel_id;
}

/*
 * create NOP tunnel for FEC mode or initiator mode
 */
int dnx_qos_php_tunnel_switch_nop_create(int unit, int in_port)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv = BCM_E_NONE;

    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    entry.label = 1000;
    entry.egress_if = fec_id_1;
    entry.port = in_port;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE) {
         printf("error in mpls php bcm_mpls_tunnel_switch_create()\n");
         printf("(%s)\n", bcm_errmsg(rv));
         return rv;
    }
    mpls_php_tunnel_id = entry.tunnel_id;
}

/*
 * create FEC,
 * for FEC mode, create cascaded FEC for packet forward
 */
int dnx_qos_php_fec_create(int unit, int out_port, int is_switch_php, int is_initiator_php, int is_pipe)
{
    bcm_l3_egress_t egr;
    bcm_if_t intf_id;
    int rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&egr);

    if (is_switch_php)
    {
        egr.flags = BCM_L3_CASCADED;
        egr.intf = arp_id;
        egr.port = out_port;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &egr, &intf_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_l3_egress_create \n");
            return rv;
        }
        fec_id_0 = intf_id;
        /*create second fec*/
        bcm_l3_egress_t_init(&egr);
        egr.mpls_action = BCM_MPLS_EGRESS_ACTION_PHP;
        BCM_GPORT_FORWARD_PORT_SET(egr.port, intf_id);
    }
    else
    {
        if (is_initiator_php)
        {
            egr.intf = mpls_php_initiator_id;
        }
        else
        {
            egr.intf = arp_id;
        }
        egr.port = out_port;
    }
    egr.qos_map_id = qos_php_map_id;
    if(is_pipe)
    {
        egr.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        egr.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    }
    rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &egr, &intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_egress_create \n");
        return rv;
    }
    fec_id_1 = intf_id;

    return rv;
}

/*
 * create egress ARP encapsulation
 */
int dnx_qos_php_arp_create(int unit, int out_port, int id)
{
    bcm_l3_egress_t egr;
    bcm_if_t intf_id;
    int flags;
    int rv = BCM_E_NONE;

    bcm_l3_egress_t_init(&egr);

    egr.port = out_port;
    egr.vlan = out_vlan;
    sal_memcpy(&egr.mac_addr, &next_hop_mac, 6);
    egr.flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    flags = BCM_L3_EGRESS_ONLY;
    if(id !=0)
    {
        egr.intf = mpls_php_initiator_id;
        egr.encap_id = id;
        flags |= BCM_L3_WITH_ID | BCM_L3_REPLACE;
    }

    rv = bcm_l3_egress_create(unit, flags, &egr, &intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_l3_egress_create \n");
        return rv;
    }
    arp_id = egr.encap_id;
    print arp_id;

    return rv;
}

/*
 * create php tunnel by API bcm_mpls_tunnel_initiator_create
 */
int dnx_qos_php_initiator_create(int unit, int in_port, int out_port, int is_pipe)
{
    bcm_mpls_egress_label_t label_array[1];
    int rv = BCM_E_NONE, tunnel_idx;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].label = -1;
    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT | BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    if(is_pipe)
    {
        label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    }
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PHP;
    label_array[0].qos_map_id = qos_php_map_id;

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create for index %d\n", tunnel_idx);
        return rv;
    }

    mpls_php_initiator_id = label_array.tunnel_id;
    print mpls_php_initiator_id;
    if(verbose >= 1) {
        printf("Created MPLS tunnel: id = 0x%08x ", mpls_php_tunnel_id);

        printf("\n");
    }
    return rv;
}

/*php qos main function*/
int dnx_qos_php_service(int unit, int in_port, int out_port, int is_pipe, int mode)
{
    int rv = BCM_E_NONE;

    /*create mpls PHP qos profile and mapping*/
    rv = dnx_qos_mpls_php_qos_map(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_qos_mpls_php_qos_map\n");
        return rv;
    }
    /*create l2 & l3 interface*/
    rv = dnx_qos_php_l2_l3_interface_create(unit, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_qos_php_l2_l3_interface_create\n");
        return rv;
    }
    /*create egress encap*/
    rv = dnx_qos_php_arp_create(unit, out_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_qos_php_arp_create\n");
        return rv;
    }
    /*create PHP tunnel*/
    switch (mode)
    {
        case 0:
            /*PHP use API bcm_mpls_tunnel_switch_create*/
            rv = dnx_qos_php_fec_create(unit, out_port, 0, 0, is_pipe);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_fec_create\n");
                return rv;
            }
            rv = dnx_qos_php_tunnel_switch_php_create(unit, in_port, is_pipe);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_tunnel_switch_php_create\n");
                return rv;
            }
            break;
        case 1:
            /*PHP use FEC*/
            rv = dnx_qos_php_fec_create(unit, out_port, 1, 0, is_pipe);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_fec_create\n");
                return rv;
            }
            rv = dnx_qos_php_tunnel_switch_nop_create(unit, in_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_tunnel_switch_create\n");
                return rv;
            }
            break;
        case 2:
            /*PHP use API bcm_mpls_tunnel_initiator_create*/
            rv = dnx_qos_php_initiator_create(unit, in_port, out_port, is_pipe);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_tunnel_switch_create\n");
                return rv;
            }
            rv = dnx_qos_php_arp_create(unit, out_port, arp_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_arp_create\n");
                return rv;
            }
            rv = dnx_qos_php_fec_create(unit, out_port, 0, 1, is_pipe);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_fec_create\n");
                return rv;
            }
            rv = dnx_qos_php_tunnel_switch_nop_create(unit, in_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in dnx_qos_php_tunnel_switch_create\n");
                return rv;
            }
            break;
        default:
            rv = BCM_E_PARAM;
            return rv;
    }
    return rv;
}

/*clean up*/
int dnx_qos_php_clean(int unit)
{
    int rv = BCM_E_NONE;

    if(vlan_port_id != 0)
    {
        print bcm_vlan_port_destroy(unit, vlan_port_id);
        vlan_port_id = 0;
    }
    if(l3_in_intf_id != 0)
    {
        bcm_l3_intf_t l3_intf;
        l3_intf.l3a_intf_id = l3_in_intf_id;
        print bcm_l3_intf_delete(unit, &l3_intf);
        l3_in_intf_id = 0;
    }
    if(l3_out_intf_id != 0)
    {
        bcm_l3_intf_t l3_intf;
        l3_intf.l3a_intf_id = l3_out_intf_id;
        print bcm_l3_intf_delete(unit, &l3_intf);
        l3_out_intf_id = 0;
    }
    if(fec_id_0 != 0)
    {
        print bcm_l3_egress_destroy(unit, fec_id_0);
        fec_id_0 = 0;
    }
    if(fec_id_1 != 0)
    {
        print bcm_l3_egress_destroy(unit, fec_id_1);
        fec_id_1 = 0;
    }
    if(arp_id != 0)
    {
        print bcm_l3_egress_destroy(unit, arp_id);
        arp_id = 0;
    }
    if(qos_php_map_id != 0)
    {
        print bcm_qos_map_destroy(unit, qos_php_map_id);
        qos_php_map_id = 0;
    }
    if(qos_php_map_opcode != 0)
    {
        print bcm_qos_map_destroy(unit, qos_php_map_opcode);
        qos_php_map_opcode = 0;
    }
    if(mpls_php_tunnel_id != 0)
    {
        bcm_mpls_tunnel_switch_t entry;
        entry.label = in_label;
        print bcm_mpls_tunnel_switch_delete(unit, &entry);
        mpls_php_tunnel_id = 0;
    }
    if (mpls_php_initiator_id != 0)
    {
        print bcm_mpls_tunnel_initiator_clear(unit, mpls_php_initiator_id);
        mpls_php_initiator_id = 0;
    }

    return rv;
}
