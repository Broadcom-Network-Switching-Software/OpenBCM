/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: An example of how to use BCM APIs to implement mirror function.
 * Used for tests: AT_Cint_mirror_tests_1, AT_Cint_mirror_vlan_tests_1.
 *
 * Purpose: An example of how to use BCM APIs to implement mirror function.
 * The example uses both inbound mirroring (according to ingress port/vlan),
 * and outbound mirroring (according to egress/outbound port/vlan).
 *
 * Mirror Service Model:
 *     Port 1  <------------------------  CrossConnect  ------------> Port 3
 *      SVLAN 100 <--------------------------------------------------> SVLAN 200
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_old_mirror_tests.c
 *      BCM> cint
 *      cint> print mirror_service(unit, up_port, down_port, up_svlan, down_svlan);
 *      cint> print mirror_run(unit, in_port, out_port, mirror_port)
 *      cint> print mirror_run_cleanup(unit, in_port, out_port)
 *      cint> print mirror_vlan_run(unit, vlan, in_port, out_port, mirror_port)
 *      cint> print mirror_vlan_run_cleanup(unit, vlan, in_port, out_port)
 *      cint> print mirror_service_cleanup(unit);
 */

int up_gport = 0;
int down_gport = 0;
bcm_gport_t mirr_dest_id[4] = {0};

/*
 *Initialize the service models and set up PON application configurations.
 */
int mirror_service_init(int unit, bcm_port_t up_port, bcm_port_t down_port)
{
    int rv = 0;
    int index = 0;

    bcm_port_class_set(unit, up_port, bcmPortClassId, up_port);
    bcm_port_class_set(unit, down_port, bcmPortClassId, down_port);

    /* set ports to identify double tags packets and treat packets with ctags to stag in Arad */
    port_tpid_init(up_port, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }

    /* set inner tpids of PON port to recognize single ctag frames */
    for(index = 0; index < port_tpid_info1.nof_inners; ++index){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
            return rv;
        }
    }

    port_tpid_init(down_port,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        print rv;
        return rv;
    }

    /* set inner tpids of NNI port to recognize single ctag frames */
    for(index = 0; index < port_tpid_info1.nof_inners; ++index){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
            return rv;
        }
    }

    /* Remove Ports from VLAN 1 (Done by init application) */
    bcm_vlan_gport_delete_all(unit, 1);

    /* Disable membership in PON ports */
    rv = bcm_port_vlan_member_set(unit, up_port, 0x0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_member_set\n");
        print rv;
        return rv;
    }

    return 0;
}

/*
 * Setup mirror service model.
 */
int mirror_service(int unit, int up_port, int down_port, int up_svlan, int down_svlan)
{
    int rv;
    int index, is_with_id = 0;
    bcm_vswitch_cross_connect_t gports;
    bcm_if_t encap_id;
    bcm_vlan_port_t up_vlan_port;
    bcm_vlan_port_t down_vlan_port;
    bcm_vlan_action_set_t action;
    
    mirror_service_init(unit, up_port, down_port);

    /* Create a up gport base up_port */
    bcm_vlan_port_t_init(&up_vlan_port);
    up_vlan_port.port = up_port;
    up_vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
    up_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    up_vlan_port.match_vlan = up_svlan;

    rv = bcm_vlan_port_create(unit, &up_vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("create lif %d failed! %s\n", bcm_errmsg(rv));
    }
    up_gport = up_vlan_port.vlan_port_id;

    /* Create a down gport base down_port */
    bcm_vlan_port_t_init(&down_vlan_port);
    down_vlan_port.port = down_port;
    down_vlan_port.flags |= (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
    down_vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    down_vlan_port.match_vlan = down_svlan;

    rv = bcm_vlan_port_create(unit, &down_vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("create lif %d failed! %s\n", bcm_errmsg(rv));
    }
    down_gport = down_vlan_port.vlan_port_id;
    
    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionReplace;
    action.new_outer_vlan = down_svlan;
    rv = bcm_vlan_translate_action_create(unit, up_gport, bcmVlanTranslateKeyPortOuter, BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_action_create %s\n", bcm_errmsg(rv));
        return rv;
    }

	  bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionReplace;
    action.new_outer_vlan = up_svlan;
    rv = bcm_vlan_translate_egress_action_add(unit, up_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
      return rv;
    }
    
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionNone;
    action.dt_inner = bcmVlanActionNone;
    action.ot_outer = bcmVlanActionNone;
    action.it_outer = bcmVlanActionNone;
    rv = bcm_vlan_translate_egress_action_add(unit, down_gport, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
      printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
      return rv;
    }

    /* Cross connect the 2 LIFs */
    bcm_vswitch_cross_connect_t_init(&gports);

    gports.port1 = up_vlan_port.vlan_port_id;
    gports.port2 = down_vlan_port.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * clean up the configurations of mirror sercies.
 */
int mirror_service_cleanup(int unit)
{
    int rv;
    int index;
    bcm_vswitch_cross_connect_t gports;

    /* Delete the cross connected LIFs */
    gports.port1 = up_gport;
    gports.port2 = down_gport;
    rv = bcm_vswitch_cross_connect_delete(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete\n");
        print rv;
        return rv;
    }

    /* Delete PON LIF */
    rv = bcm_vlan_port_destroy(unit, up_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }

    /* Delete NNI LIF */
    rv = bcm_vlan_port_destroy(unit, down_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy \n");
        return rv;
    }
      
    return rv;        
}

/*
 * Setup a mirror service.
 *
 * mirror_type:BCM_MIRROR_PORT_EGRESS or BCM_MIRROR_PORT_INGRESS
 */
int mirror_port_setup(int unit, int src_port, int mirror_port, int mirror_type, bcm_gport_t *mirror_dest_id) 
{
    int rv;
    bcm_mirror_destination_t dest = {0};
     
    /* set mirror destinations parameters, as his is simple mirroring,
     * only physical port is in use other parameters should be cleared (0)
     */ 
    dest.gport = mirror_port;
    dest.flags = 0;
    dest.tunnel_id = 0;    

    if (rv = bcm_mirror_destination_create(unit, &dest)) 
    {
        printf("Failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    *mirror_dest_id = dest.mirror_dest_id;

    printf("mirror_dest_id:%d %d\n", *mirror_dest_id, dest.mirror_dest_id);

    if (rv = bcm_mirror_port_dest_add(unit, src_port, mirror_type, dest.mirror_dest_id)) 
    {
        printf("Failed to add inbound/outbind port to be mirrored, return value %d\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Destroy a mirror service.
 *
 * mirror_type:BCM_MIRROR_PORT_EGRESS or BCM_MIRROR_PORT_INGRESS
 */
int mirror_port_destory(int unit, int src_port, int mirror_type, bcm_gport_t mirror_dest_id)
{
    int rv;

    if (rv = bcm_mirror_port_dest_delete_all(unit, src_port, mirror_type))
    {
        printf("Failed to delete inbound/outbound port to be mirrored, return value %d\n", rv);
        return rv;
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_dest_id))
    {
        printf("Failed to destroy destination mirror_dest_id:%d, return value %d\n", mirror_dest_id, rv);
        return rv;
    }

    return rv;
}

/*
 * Setup a mirror service base vlan.
 *
 * mirror_type:BCM_MIRROR_PORT_EGRESS or BCM_MIRROR_PORT_INGRESS
 */
int mirror_port_vlan_setup(int unit, int vlan, int src_port, int mirror_port, int mirror_type, bcm_gport_t *mirror_dest_id) 
{
    int rv;
    bcm_mirror_destination_t dest = {0};
     
    /* set mirror destinations parameters, as his is simple mirroring,
     * only physical port is in use other parameters should be cleared (0)
     */ 

    dest.gport = mirror_port;
    dest.flags = 0;
    dest.tunnel_id = 0;    

    if (rv = bcm_mirror_destination_create(unit, &dest)) 
    {
        printf("Failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    *mirror_dest_id = dest.mirror_dest_id;

    if (rv = bcm_mirror_port_vlan_dest_add(unit, src_port, vlan, mirror_type, dest.mirror_dest_id)) 
    {
        printf("failed to add inbound port1+vlan1 to be mirrored, return value %d\n", rv);
        return rv;
    }

    return rv;
}

/*
 * Destory a mirror service base vlan.
 *
 * mirror_type:BCM_MIRROR_PORT_EGRESS or BCM_MIRROR_PORT_INGRESS
 */
int mirror_port_vlan_destory(int unit, int vlan, int src_port, int mirror_type, bcm_gport_t mirror_dest_id)
{
    int rv;

    if (rv = bcm_mirror_port_vlan_dest_delete_all(unit, src_port, vlan, mirror_type))
    {
        printf("Failed to delete inbound/outbound port1 to be mirrored, return value %d\n", rv);
        return rv;
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_dest_id))
    {
        printf("Failed to destroy destination mirror_dest_id:%d, return value %d\n", mirror_dest_id, rv);
        return rv;
    }

    return rv;
}

/* 
 * Setup INGRESS and EGRESS mirror service base port
 */
int mirror_run(int unit, int in_port, int out_port, int mirror_port)
{
    int rv = 0;
 
    if (rv = mirror_port_setup(unit, in_port, mirror_port, BCM_MIRROR_PORT_INGRESS, &(mirr_dest_id[0])))
    {
        printf("Failed to setup ingress mirror service, return value %d\n", rv);
        return rv;
    }

    if (mirror_port_setup(unit, out_port, mirror_port, BCM_MIRROR_PORT_EGRESS, &(mirr_dest_id[1])))
    {
        printf("Failed to setup egress mirror service, return value %d\n", rv);
        return rv;
    }

    return rv;
}

/* 
 * Cleanup INGRESS and EGRESS mirror service base port
 */
int mirror_run_cleanup(int unit, int in_port, int out_port)
{
    int rv = 0;

    if (mirror_port_destory(unit, in_port, BCM_MIRROR_PORT_INGRESS, mirr_dest_id[0]))
    {
        printf("Failed to cleanup ingress mirror service, return value %d\n", rv);
        return rv;
    }

    if (mirror_port_destory(unit, out_port, BCM_MIRROR_PORT_EGRESS, mirr_dest_id[1]))
    {
        printf("Failed to cleanup egress mirror service, return value %d\n", rv);
        return rv;
    }

    return rv;
}

/* 
 * Setup INGRESS and EGRESS mirror service base port and vlan
 */
int mirror_vlan_run(int unit, int vlan, int in_port, int out_port, int mirror_port)
{
    int rv = 0;
 
    
    if (mirror_port_vlan_setup(unit, vlan, in_port, mirror_port, BCM_MIRROR_PORT_INGRESS, &(mirr_dest_id[2])))
    {
        printf("Failed to setup ingress mirror vlan service, return value %d\n", rv);
        return rv;
    }

        
    if (mirror_port_vlan_setup(unit, vlan, out_port, mirror_port, BCM_MIRROR_PORT_EGRESS, &(mirr_dest_id[3])))
    {
        printf("Failed to setup egress mirror vlan service, return value %d\n", rv);
        return rv;
    }

    return rv;
}

/* 
 * Cleanup INGRESS and EGRESS mirror service base port and vlan
 */
int mirror_vlan_run_cleanup(int unit, int vlan, int in_port, int out_port)
{
    int rv = 0;

    if (mirror_port_vlan_destory(unit, vlan, in_port, BCM_MIRROR_PORT_INGRESS, mirr_dest_id[2]))
    {
        printf("Failed to cleanup ingress mirror vlan service, return value %d\n", rv);
        return rv;
    }

    if (mirror_port_vlan_destory(unit, vlan, out_port, BCM_MIRROR_PORT_EGRESS, mirr_dest_id[3]))
    {
        printf("Failed to cleanup egress mirror vlan service, return value %d\n", rv);
        return rv;
    }

    return rv;
}



