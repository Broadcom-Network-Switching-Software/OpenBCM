/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System resource example:
 * In the following CINT we will try to provide an example of System resources in local mode.
 *    The focus of the CINT is on AC-LIFs (bcm_vlan_port_t object in BCM API).
 * Similiar principles are apply also to other LIF types such as bcm_mpls_port_t
 * (see cint_system_vswitch_vpls.c).
 * Each AC-LIF allocates two objects: An Ingress LIF entry and an Egress LIF entry.
 * In case of 1:1 protection also a L2 FEC object that stores
 * the protection pairs is also being created.
 *    When working with system resources in global mode all 3 objects are required to be
 * allocated on all devices with the same object ID.
 * Please see an example of such schemes in:
 * cint_vswitch_metro_mp_multi_devices.c, cint_vswitch_cross_connect_p2p_multi_device.c
 *    When working with system resources in local mode not all 3 objects are required to be
 * allocated on all devices:
 *  * In/Out-AC-Lif need to be created only on devices where the AC is attached to.
 *  In case of LAGs, these may be all the potential devices that connect to the LAG port members.
 *  * The L2 FEC needs to be created only on devices that forward to the Lif.
 *  The following CINT provides some basic examples of AC-LIf metro vswitch allocation schemes when
 *  working in local mode.
 *  1. VLAN P2P unprotected: see system_vlan_port_p2p_example
 *  2. VLAN MP unprotection: see system_vlan_port_mp_example
 *  3. VLAN P2P/MP protected: see system_protected_vlan_port_example
 * 
 * assumptions:
 * 1. run two devices in same CPU.
 * 2. in call unit1 != unit2.
 * 3. diag_chassis=1
 * 4. bcm88xxx_system_resource_management.<unit1>=1
 *    bcm88xxx_system_resource_management.<unit2>=1
 
 * How to run:
cint;
cint_reset();
exit; 
cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
cint ../../../../src/examples/dpp/cint_port_tpid.c
cint ../../../../src/examples/dpp/cint_mact.c
cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p.c
cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c
cint ../../../../src/examples/dpp/cint_multi_device_utils.c
cint ../../../../src/examples/dpp/cint_system_vswitch_encoding.c
cint ../../../../src/examples/dpp/cint_system_vswitch.c
cint
verbose = 3;
int rv; 
int unit1 = 0; 
int unit2 = 1; 
int sysport1, sysport2, sysport3; 
print port_to_system_port(unit1, 13, &sysport1); 
print port_to_system_port(unit2, 14, &sysport2); 
print port_to_system_port(unit2, 15, &sysport3); 
 
for P2P unprotected case: 
print system_vlan_port_p2p_example(unit1, sysport1, unit2, sysport2); 
 
for MP unprotected case:
print system_vlan_port_mp_example(unit1, sysport1, sysport2, unit2, sysport3); 
 
for P2P protection case: 
print system_protected_vlan_port_example(unit1, sysport1, unit2, sysport2, sysport3, 0);  
 
for MP protection case: 
print system_protected_vlan_port_example(unit1, sysport1, unit2, sysport2, sysport3, 1);  
 
*/ 

/* set to one for informative prints */
int verbose = 3;

/* set in order to do encoding using utility functions and not using APIs. */
int encoding = 0;

/* GLOBAL VARIABLES */
int g_failover_id;


/* 
 * The following utility functions replaces the call to bcm_l2_gport_frwrd_info_get,
 * by mapping the VLAN PORT gport info to frwrd-info directly
 * handling seperated into 3 cases according to gport-id and vlan_port usage.
 *  A) 1:1 porection: Forwarding = FEC.
 *  B) 1+1 proection: Forwarding = Multicast
 *  C) no protection: Forwarding = <Port, Outlif>
 */
int system_vlan_port_to_l2_gport_forward_info(
   int unit,
   bcm_vlan_port_t  *vlan_port,
   bcm_l2_gport_forward_info_t *frwrd_info)
{

    int outlif=0;
    uint32 eei=0;
    uint8 is_eei=0;
    int fec_id;
    uint32 mc_id;

    bcm_l2_gport_forward_info_t_init(frwrd_info);

    /* for vlan port forwarding informaton is always out_lif*/
    is_eei = 0;

    /* 
     * A) in case of 1:1 protection the forwarding information is FEC
     * the FEC id is passed in the vlan_port_id.
     */

    /* 1:1 protection identified if failover-id != 0*/
    if (vlan_port->failover_id != 0) {
        if (!(vlan_port->flags & BCM_VLAN_PORT_WITH_ID)) {
            printf("Error, WITH_ID flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        fec_id = BCM_GPORT_SUB_TYPE_PROTECTION_GET(vlan_port->vlan_port_id);
        BCM_GPORT_FORWARD_PORT_SET(frwrd_info->phy_gport,fec_id);
        frwrd_info->encap_id = BCM_FORWARD_ENCAP_ID_INVALID;
    }

    /* 
     * B) in case of 1+1 protection the forwarding information is Multicast (Bi-cast)
     * the Multicast id is passed in the vlan_port_id.
     */

    /* 1+1 protection identified if failover_mc_group is valid MC group*/
    else if (vlan_port->failover_mc_group != 0) {
        if (!(vlan_port->flags & BCM_VLAN_PORT_WITH_ID)) {
            printf("Error, WITH_ID flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        mc_id = BCM_GPORT_SUB_TYPE_MULTICAST_VAL_GET(vlan_port->vlan_port_id);
        BCM_GPORT_MCAST_SET(frwrd_info->phy_gport,mc_id);        
        frwrd_info->encap_id = BCM_FORWARD_ENCAP_ID_INVALID;
    }
    else{
        /* 
         * C) otherwise no protection the forwarding information is <port + outLIF>
         * the outLIF id is passed in the encap_id or on the vlan_port_id.
         */
        if (vlan_port->flags & BCM_VLAN_PORT_ENCAP_WITH_ID) {
            outlif = BCM_ENCAP_ID_GET(vlan_port->encap_id);
        }
        else if (vlan_port->flags & BCM_VLAN_PORT_WITH_ID) {
            outlif = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(vlan_port->vlan_port_id);
        }
        else{
            printf("Need to supply WITH_ID or ECAN_WITH_ID flags\n"); 
            return BCM_E_PARAM;
        }
        frwrd_info->phy_gport = vlan_port->port;
        BCM_FORWARD_ENCAP_ID_VAL_SET(frwrd_info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF, BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT, outlif);
    }

    

    return BCM_E_NONE;
}

int system_vlan_port_to_forward_info(
   int unit,
   bcm_vlan_port_t vlan_port,
   int gport,
   bcm_l2_gport_forward_info_t *forward_info)
{
    int rv = BCM_E_NONE;
    
    /* map gport to system encapsulation info */
    if (encoding) {
      rv = system_vlan_port_to_l2_gport_forward_info(unit, &vlan_port, forward_info);
    } else {
      rv = bcm_l2_gport_forward_info_get(unit,gport, forward_info);
    }    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_gport_forward_info_get\n");
        return rv;
    }
    rv = port_to_system_port(unit, forward_info->phy_gport, &forward_info->phy_gport);
    if (rv != BCM_E_NONE) {
        printf("Error, port_to_system_port\n");
        return rv;
    }

    if(verbose >= 2) {  
        printf("forwarding info phy_port:0x%08x encap_id:0x%08x \n",forward_info->phy_gport, forward_info->encap_id);
    }

    return rv;
}

/*
 * SYSTEM EXAMPLE
 */

/*
 * bcm_vlan_port_create: for unprotected case.
 */
int
system_vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t vlan,
    int vsi, 
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;
    bcm_gport_t gport_encoding, gport_encoding_subtype;    
    bcm_vlan_action_set_t action;

    bcm_vlan_port_t_init(vp);
    vp->port = port_id;    
    vp->vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    if (!is_device_or_above(unit, JERICHO2)) {
        vp->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp->match_vlan = vlan; 
        vp->egress_vlan = vlan; /* when forwarded to this port, packet will be set with this out-vlan */
        if (*gport!=0) {
            vp->flags = BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); 
        }
    } else {
        vp->criteria = BCM_VLAN_PORT_MATCH_NONE;
        if (*gport!=0) {
            vp->flags = BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_SUB_TYPE_LIF_SET(vp->vlan_port_id, 0, *gport);
            BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id, vp->vlan_port_id); 
        }
    }    

    if (encoding) {
        BCM_GPORT_SUB_TYPE_LIF_SET(gport_encoding_subtype, 0, *gport);
        BCM_GPORT_VLAN_PORT_ID_SET(gport_encoding, gport_encoding_subtype); 
    }
    rv = bcm_vlan_port_create(unit, vp);	
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    if (encoding) {
        printf("after bcm_vlan_port_create: vp->vlan_port_id:%d gport_encoding:%d\n ",vp->vlan_port_id,gport_encoding);
    }
    
    if(verbose >= 1) {
        printf("created vlan_port:0x%08x in unit %d \n",vp->vlan_port_id, unit);
    }
   
    if (!encoding) {
        *gport = vp->vlan_port_id; 
    }
    else{
        *gport = gport_encoding;
    }

    if (is_device_or_above(unit, JERICHO2)) {
        bcm_port_match_info_t port_match_info; 
        
        /* Add port in the vlan membership */
        rv = bcm_vlan_gport_add(unit, vlan, port_id, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_gport_add\n");
            return rv;
        }
        
        bcm_port_match_info_t_init(&port_match_info); 

        port_match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY; 
        port_match_info.match = is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT_CVLAN: BCM_PORT_MATCH_PORT_VLAN; 
        port_match_info.port = port_id;
        port_match_info.match_vlan = vlan;

        rv = bcm_port_match_add(unit,vp->vlan_port_id, &port_match_info);
        if (rv != BCM_E_NONE) {
           printf("Error, in bcm_port_match_add\n");
           return rv;
        }
     }

    rv = vlan_port_translation_set(unit, vlan, 0, vp->vlan_port_id, g_eve_edit_utils.edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }


    /* Create action IDs*/
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_eve_edit_utils.action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action 1. outer action, set TPID 0x8100. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionReplace;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    rv = bcm_vlan_translate_action_id_set(unit,
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           g_eve_edit_utils.action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /* set action class */
    rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_class_set\n");
        return rv;
    }
    
    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
        if(verbose){
            printf("add port 0x%08x to vswitch in unit %d\n",*gport, unit);
        }
    }
    return BCM_E_NONE;
}

/*
 *  Set the value of the encap_id to indicate remote/local LIF id
 */
void system_vswitch_encap_id_set(int *encap_id, int lif_id, int is_remote){
    *encap_id = 0;
    BCM_ENCAP_ID_SET(*encap_id, lif_id);
    if (is_remote) {
        BCM_ENCAP_REMOTE_SET(*encap_id);
    }
}

/*
 *  bcm_vlan_port_create: for protected gport that allocate only LIF.
 *  the indication is by setting failover_port_id (protection gport) this will be used as learning
 *  information of the LIF
 */
int
system_vlan_port_lif_only_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t vlan,
    int vsi,
    bcm_gport_t learn_fec, 
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;
    bcm_vlan_action_set_t action;

    bcm_vlan_port_t_init(vp);
    vp->port = port_id;    
    vp->vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    if (!is_device_or_above(unit, JERICHO2)) {
        vp->criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        BCM_GPORT_VLAN_PORT_ID_SET(vp->failover_port_id,learn_fec);
        vp->match_vlan = vlan; 
        vp->egress_vlan = vlan; /* when forwarded to this port, packet will be set with this out-vlan */
        if (*gport!=0) {
            vp->flags = BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); 
        }
    } else {
        vp->criteria = BCM_VLAN_PORT_MATCH_NONE;
        if (*gport!=0) {
            vp->flags = BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_SUB_TYPE_LIF_SET(vp->vlan_port_id, 0, *gport);
            BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id, vp->vlan_port_id); 
        }
    } 
    
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        return rv;
    }
    if(verbose >= 1) {
        printf("unit (%d): created vlan_port (LIF) :0x%08x with learn info FEC:0x%08x\n",unit,vp->vlan_port_id,vp->failover_port_id);
    }
    *gport = vp->vlan_port_id;  

    if (is_device_or_above(unit, JERICHO2)) {
        bcm_port_match_info_t port_match_info;         

        /* Add port in the vlan membership */
        rv = bcm_vlan_gport_add(unit, vlan, port_id, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_vlan_gport_add\n");
            return rv;
        }
        
        bcm_port_match_info_t_init(&port_match_info); 

        port_match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY; 
        port_match_info.match = is_device_or_above(unit, JERICHO2) ? BCM_PORT_MATCH_PORT_CVLAN: BCM_PORT_MATCH_PORT_VLAN; 
        port_match_info.port = port_id;
        port_match_info.match_vlan = vlan;

        rv = bcm_port_match_add(unit,vp->vlan_port_id, &port_match_info);
        if (rv != BCM_E_NONE) {
           printf("Error, in bcm_port_match_add\n");
           return rv;
        }        
    }
    
    rv = vlan_port_translation_set(unit, vlan, 0, vp->vlan_port_id, g_eve_edit_utils.edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }


    /* Create action IDs*/
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &g_eve_edit_utils.action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action 1. outer action, set TPID 0x8100. */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = bcmVlanActionReplace;
    action.dt_inner = bcmVlanActionNone;
    action.outer_tpid = 0x8100;
    rv = bcm_vlan_translate_action_id_set(unit,
                                           BCM_VLAN_ACTION_SET_EGRESS,
                                           g_eve_edit_utils.action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /* set action class */
    rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_class_set\n");
        return rv;
    }

    if (vsi > 0) {
        rv = bcm_vswitch_port_add(unit, vsi, *gport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
        if(verbose){
            printf("add port  0x%08x to vswitch in unit %d\n",*gport, unit);
        }
    }
    return BCM_E_NONE;
}


/*
 *  bcm_vlan_port_create: for protected gport that allocate only FEC.
 *  the indication is by setting the encap_id with BCM_ENCAP_REMOTE_SET().
 *  from SDK implementation side if given bcm_vlan_port_create called with BCM_VLAN_PORT_ENCAP_WITH_ID
 *  where the encap_id parameter has BCM_ENCAP_REMOTE_SET(): then this encap used as remote-LIF
 *  i.e. it's pointed by FEC but not allocated or configured.
 *  Note:
 *  - this call can be done only for protected valn_port i.e. failover_id valid.
 *  - when configure attribute of LIF the user need to supply the gport_id returned
 *  when calling bcm_vlan_port_create for the LIF. if FEC gport provided error is returned
 */
int
system_vlan_port_fec_only_create(
    int unit,
    bcm_port_t  port_id,
    bcm_gport_t failover_id, 
    bcm_gport_t failover_port_id, /* secondary, if this is primary*/
    bcm_if_t    encap_id, /* outLif ID */
    bcm_gport_t *gport,
    bcm_vlan_port_t *vp
    ){

    int rv;   

    if (is_device_or_above(unit, JERICHO2)) {
        sand_utils_l3_fec_s fec_structure;
        
        sand_utils_l3_fec_s_common_init(unit, 0, 0, &fec_structure, 0, 0, 0, port_id, 0, 0);
        BCM_L3_ITF_SET(fec_structure.tunnel_gport,BCM_L3_ITF_TYPE_LIF, encap_id);
        fec_structure.failover_id = failover_id;
        fec_structure.failover_if_id = failover_port_id;
        rv = sand_utils_l3_fec_create(unit, &fec_structure);
        if (rv != BCM_E_NONE)
        {
            printf("Error, sand_utils_l3_fec_create\n");
            return rv;
        }
        *gport = fec_structure.l3eg_fec_id;
    } else {    
        vp->criteria = BCM_VLAN_PORT_MATCH_NONE; /* no AC key */
        vp->port = port_id; /* physical port */
        system_vswitch_encap_id_set(&(vp->encap_id),encap_id,1);
        vp->flags |= BCM_VLAN_PORT_ENCAP_WITH_ID; 
        vp->failover_port_id = failover_port_id;
        vp->failover_id = failover_id; /* no FEC allocated */
        if (*gport!=0) {
            vp->flags |= BCM_VLAN_PORT_WITH_ID;
            BCM_GPORT_VLAN_PORT_ID_SET(vp->vlan_port_id,*gport); /* this is the LIF id */
        }
        rv = bcm_vlan_port_create(unit, vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n"); 
            return rv;
        }        
        *gport = vp->vlan_port_id;
    }   

    if(verbose >= 1) {
        printf("unit (%d): created vlan_port (FEC) :0x%08x point to remote LIF:0x%08x\n",unit,*gport,encap_id);
    }

    return BCM_E_NONE;
}


/* 
 * Multipoint Example:
 *  devices : unit1 and unit2
 * 
 *  physical port:
 *      - unit1: port1,second_port1 
 *      - unit2: port2 
 * 
 *  gports (LIFs):
 *      - unit1:
 *          - gport1:       on <port2,v1=10>
 *          - second_gport1 on <second_port1,v1=10>
 *      - unit2:
 *          - gport2        on <port2,v2=30>
 * 
 * << NOTE: port parameters should be given as system-gport >>
 */
int system_vlan_port_mp_example(int unit1, int port1, int second_port1, int unit2, int port2){
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    bcm_vlan_port_t second_vp1;
    bcm_vlan_port_t_init(&second_vp1);
    bcm_vlan_port_t vp2;
    bcm_vlan_port_t_init(&vp2);
    uint32 v1 = 10;
    bcm_gport_t gport1= is_device_or_above(unit1, JERICHO2) ? 4098 : 8200;
    uint32 v2 = 30;
    bcm_gport_t gport2= is_device_or_above(unit2, JERICHO2) ? 4098 : 8200;
    bcm_gport_t second_gport1= is_device_or_above(unit1, JERICHO2) ? 4099 : 8201;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t mac2  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t mac3  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    bcm_vlan_t vsi = 6000;
    bcm_l2_gport_forward_info_t forward_info;

    /* set the unit 1 and unit 2 learning mode as BCM_L2_INGRESS_DIST */
    rv = bcm_switch_control_set(unit1, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set unit 1\n"); 
        return rv;
    }

    rv = bcm_switch_control_set(unit2, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set unit 2\n"); 
        return rv;
    }
    

    /* create vswitch on both devices*/
    vswitch_create(unit1,&vsi,1);
    vswitch_create(unit2,&vsi,1);

    /* create gport1 in unit1 attach to vsi */
    rv = system_vlan_port_create(unit1,port1,v1,vsi,&gport1,&vp1);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_create port 1\n"); 
        return rv;
    }

    /* create second_gport1 in unit1 */
    rv = system_vlan_port_create(unit1,second_port1,v1,vsi,&second_gport1,&second_vp1);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_create port 2\n"); 
        return rv;
    }

    /* create gport2 (different gport) in unit2 */
    rv = system_vlan_port_create(unit2,port2,v2,vsi,&gport2,&vp2);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_create port 2\n"); 
        return rv;
    }

    /* Adding the gports to the multicast */
    rv = system_multicast_vlan_port_add(unit1, unit2, port1, gport1, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in system_multicast_vlan_port_add\n");
        return rv;
    }

    rv = system_multicast_vlan_port_add(unit1, unit2, second_port1, second_gport1, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in system_multicast_vlan_port_add\n");
        return rv;
    }

    rv = system_multicast_vlan_port_add(unit2, unit1, port2, gport2, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in system_multicast_vlan_port_add\n");
        return rv;
    }


    /* 
     * adding MACT entries to unit1
     */

    /* dest is on gport2 in unit2 (remote)*/
    bcm_l2_addr_t_init(&l2addr, mac1, vsi);
    l2addr.vid = vsi;
    l2addr.flags = BCM_L2_STATIC;
    /* map gport to system encapsulation info for dest is remote */
    rv = system_vlan_port_to_forward_info(unit2, vp2, gport2, &forward_info);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_to_forward_info\n");
        return rv;
    }
    l2addr.port = forward_info.phy_gport;
    l2addr.encap_id = forward_info.encap_id;
    rv = bcm_l2_addr_add(unit1, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add 1\n");
        return rv;
    }
    if (verbose > 1) {
        printf("add 1:");
        bcm_l2_addr_dump(&l2addr);
    }
    /* dest is gport1 in unit1 (local)
       there are two options:
        1. set port as gport1.
        2. similar to above */
    bcm_l2_addr_t_init(&l2addr, mac2, vsi);
    l2addr.vid = vsi;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = gport1;
    rv = bcm_l2_addr_add(unit1, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add 1\n");
        return rv;
    }
    if (verbose > 1) {
        printf("add 2:");
        bcm_l2_addr_dump(&l2addr);
    }

    /* map gport to system encapsulation info for dest is remote */
    rv = system_vlan_port_to_forward_info(unit1, vp1, gport1, &forward_info);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_to_forward_info\n");
        return rv;
    }
    l2addr.port = forward_info.phy_gport;
    l2addr.encap_id = forward_info.encap_id;
    rv = bcm_l2_addr_add(unit2, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add 1\n");
        return rv;
    }
    if (verbose > 1) {
        printf("add 3:");
        bcm_l2_addr_dump(&l2addr);
    }


    /* Add mapping to second_gport1 from both units */
    bcm_l2_addr_t_init(&l2addr, mac3, vsi);
    l2addr.vid = vsi;
    l2addr.flags = BCM_L2_STATIC;
    /* map gport to system encapsulation info for dest is remote */
    rv = system_vlan_port_to_forward_info(unit1, second_vp1, second_gport1, &forward_info);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_to_forward_info\n");
        return rv;
    }
    l2addr.port = forward_info.phy_gport;
    l2addr.encap_id = forward_info.encap_id;    
    rv = bcm_l2_addr_add(unit2, &l2addr);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_l2_addr_add 1\n");
      return rv;
    }
    if (verbose > 1) {
        printf("add 4:");
        bcm_l2_addr_dump(&l2addr);
    }
    rv = bcm_l2_addr_add(unit1, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add 1\n");
        return rv;
    }
    if (verbose > 1) {
        printf("add 5:");
        bcm_l2_addr_dump(&l2addr);
    }



    return rv;
}

/* 
 * P2P Example:
 *  devices : unit1 and unit2
 * 
 *  physical port:
 *      - unit1: port1 
 *      - unit2: port2 
 * 
 *  gports (LIFs):
 *      - unit1: gport1 on <port1,v1=102>
 *      - unit2: gport2 on <port2,v2=201>
 * 
 * << NOTE: port parameters should be given as system-gport >>
 */
int system_vlan_port_p2p_example(int unit1, int port1, int unit2, int port2){
    int rv = BCM_E_NONE;
    uint32 v1 = 102;
    bcm_gport_t gport1= is_device_or_above(unit1, JERICHO2) ? 4098 : 8200;
    uint32 v2 = 201;
    bcm_gport_t gport2= is_device_or_above(unit2, JERICHO2) ? 4098 : 8200;
    bcm_l2_gport_forward_info_t forward_info;
    bcm_vswitch_cross_connect_t cross_connect;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t vp2;

    bcm_vswitch_cross_connect_t_init(&cross_connect);

    /* create vswitch on both devices*/
    rv = bcm_vlan_create(unit1, v1);
    rv = bcm_vlan_create(unit2, v2);

    /* create gport1 in unit1 attach to vsi */
    rv = system_vlan_port_create(unit1,port1,v1,0,&gport1,&vp1);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_create port 1\n"); 
        return rv;
    }

    /* create gport2 (different gport) in unit2 */
    rv = system_vlan_port_create(unit2,port2,v2,0,&gport2,&vp2);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_create port 2\n"); 
        return rv;
    }

    /* gport1 --> gport2 */
    bcm_vswitch_cross_connect_t_init(&cross_connect);
    cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    cross_connect.port1 = gport1;
    /* map gport to system encapsulation info for dest is remote */
    rv = system_vlan_port_to_forward_info(unit2, vp2, gport2, &forward_info);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_to_forward_info\n");
        return rv;
    }
    cross_connect.port2 = port2;
    cross_connect.encap2 = forward_info.encap_id;
    rv = bcm_vswitch_cross_connect_add(unit1, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    /* gport2 --> gport1 */
    bcm_vswitch_cross_connect_t_init(&cross_connect);
    cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    cross_connect.port1 = gport2;
    /* map gport to system encapsulation info for dest is remote */
    rv = system_vlan_port_to_forward_info(unit1, vp1, gport1, &forward_info);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_to_forward_info\n");
        return rv;
    }
    cross_connect.port2 = port1;
    cross_connect.encap2 = forward_info.encap_id;
    rv = bcm_vswitch_cross_connect_add(unit2, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

void cross_connect_get(int unit, int gport){
    bcm_vswitch_cross_connect_t cross_connect;
    bcm_vswitch_cross_connect_t_init(&cross_connect);
    cross_connect.port1 = gport;
    print bcm_vswitch_cross_connect_get(unit,&cross_connect);
    print cross_connect;
}

 /*
 * Multipoint Example:
 *  devices : unit1 and unit2
 * 
 *  physical port:
 *      - unit1: port1,second_port1 
 *      - unit2: port2 
 *  
 *  two gports
 *  - gport1
 *      - not protected
 *      - resources:
 *          - unit1: LIF 
 *  - gport2
 *      - protected
 *      - resources:
 *          - unit1: fec_a, fec_b
 *          - unit2: lif_a, lif_b
 * 
 * << NOTE: port parameters should be given as system-gport >> 
 */ 
int system_protected_vlan_port_example(int unit1, int port1, int unit2, int port2,int port3, int is_mp){
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vp1_lif;
    bcm_vlan_port_t vp2_lif_a;
    bcm_vlan_port_t vp2_lif_b;
    bcm_vlan_port_t vp2_fec_a;
    bcm_vlan_port_t vp2_fec_b;
    uint32 v1 = 10;
    /* on unit2 */
    bcm_gport_t gport2_lif_a = is_device_or_above(unit2, JERICHO2) ? 4098 : 8200;
    bcm_gport_t gport2_lif_b = is_device_or_above(unit2, JERICHO2) ? 4099 : 8201;
    /* on unit1 */
    bcm_gport_t gport2_fec_a = 0xCCCE;
    bcm_gport_t gport2_fec_b = 0xCCCF;
    uint32 v2 = 20;
    uint32 v3 = 30;
    /* on unit1 */
    bcm_gport_t gport1_lif = is_device_or_above(unit1, JERICHO2) ? 4098 :  8200;

    /* mp data */
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac1  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t mac2  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22};
    bcm_mac_t mac3  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    bcm_vlan_t vsi;
    bcm_failover_t failover_id;
    /* p2p data */
    bcm_vswitch_cross_connect_t cross_connect;
    bcm_l2_gport_forward_info_t forward_info;

    bcm_vswitch_cross_connect_t_init(&cross_connect);

    if (is_mp) {
      vsi = 7000;
      /* create vswitch on both devices*/
      vswitch_create(unit1,&vsi,1);
      vswitch_create(unit2,&vsi,1);
    }
    else {
      vsi = 0;/* 0 means not connected to mp VSI */
    }

    /* create gport1 in unit1 without protection  */
    rv = system_vlan_port_create(unit1,port1,v1,vsi,&gport1_lif,&vp1_lif);
    if (rv != BCM_E_NONE) {
      printf("Error, system_vlan_port_create port 1\n"); 
      return rv;
    }

    /* create failover id */
    rv = bcm_failover_create(unit1, BCM_FAILOVER_FEC, &failover_id);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_failover_create 1\n"); 
      return rv;
    }

    /* 4 calls below, create protected vlan-port which includes:
         unit1: FEC1, FEC2
         unit2: LIF1, LIF2
    */

    g_failover_id = failover_id;

    /* create two FEC entries, each point to <port,LIF> and share the same Failver-id*/
    rv = system_vlan_port_fec_only_create(unit1, port2, failover_id, 0,            gport2_lif_b, &gport2_fec_b,&vp2_fec_b);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_fec_only_create 1\n"); 
        return rv;
    }
    rv = system_vlan_port_fec_only_create(unit1, port3, failover_id, gport2_fec_b, gport2_lif_a, &gport2_fec_a,&vp2_fec_a);
    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_fec_only_create 1\n"); 
        return rv;
    }
    /* create gport2.LIFa with learning info as FEC but without creating FEC entry */

    rv = system_vlan_port_lif_only_create(unit2,port2,v2,vsi,gport2_fec_a,&gport2_lif_b,&vp2_lif_b);

    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_lif_only_create port 1\n"); 
        return rv;
    }
    /* create gport2.LIFb with learning info as FEC but without creating FEC entry */

    rv = system_vlan_port_lif_only_create(unit2,port3,v3,vsi,gport2_fec_b,&gport2_lif_a,&vp2_lif_a);

    if (rv != BCM_E_NONE) {
        printf("Error, system_vlan_port_lif_only_create port 1\n"); 
        return rv;
    }

    if (verbose > 3) {
        printf("gport2_fec_a: 0x%x, gport2_fec_b: 0x%x, gport2_lif_a: 0x%x, gport2_lif_b: 0x%x\n", gport2_fec_a, gport2_fec_b, gport2_lif_a, gport2_lif_b);
    }

    /* 
     * adding MACT entries to unit1 point to the FEC
     */
    if (is_device_or_above(unit1, JERICHO2)) {
        BCM_GPORT_FORWARD_PORT_SET(gport2_fec_a, gport2_fec_a);
    }
    if (is_mp) {
        /* set the unit 1 and unit 2 learning mode as BCM_L2_INGRESS_DIST */
        rv = bcm_switch_control_set(unit1, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set unit 1\n"); 
            return rv;
        }

        rv = bcm_switch_control_set(unit2, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set unit 2\n"); 
            return rv;
        }
        
        /* Adding the gports to the multicast */
        rv = system_multicast_vlan_port_add(unit1, unit2, port1, gport1_lif, vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, in system_multicast_vlan_port_add\n");
            return rv;
        }

        /* Adding the gports to the multicast */
        rv = system_multicast_vlan_port_add(unit2, unit1, port2, gport2_lif_a, vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, in system_multicast_vlan_port_add\n");
            return rv;
        }

        /* Adding the gports to the multicast */
        rv = system_multicast_vlan_port_add(unit2, unit1, port3, gport2_lif_b, vsi);
        if (rv != BCM_E_NONE) {
            printf("Error, in system_multicast_vlan_port_add\n");
            return rv;
        }

        bcm_l2_addr_t_init(&l2addr, mac2, vsi);
        l2addr.vid = vsi;
        l2addr.flags = BCM_L2_STATIC;
        l2addr.port = gport2_fec_a;
        rv = bcm_l2_addr_add(unit1, &l2addr);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add 1\n");
            return rv;
        }
        if (verbose > 1) {
            printf("add 2:");
            bcm_l2_addr_dump(&l2addr);
        }
    }
    else{

        /* gport1_lif --> gport2_fec_a (both exist in unit1 */
        bcm_vswitch_cross_connect_t_init(&cross_connect);
        cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        /* gport info [optional see option2 below ] */
        if (encoding && !is_device_or_above(unit1, JERICHO2)) {
          rv = system_vlan_port_to_l2_gport_forward_info(unit1, &vp2_fec_a, &forward_info);
        } else {
          rv = bcm_l2_gport_forward_info_get(unit1,gport2_fec_a, &forward_info);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_gport_forward_info_get\n");
            return rv;
        }
        cross_connect.port1 = gport1_lif;
        cross_connect.port2 = forward_info.phy_gport;
        cross_connect.encap2 = forward_info.encap_id;
        if(verbose >= 2) {
            printf("forwarding info of gport(gport2_fec_a):0x%08x is phy_port:0x%08x encap_id:0x%08x \n",gport2_fec_a, forward_info.phy_gport, forward_info.encap_id);
        }
        rv = bcm_vswitch_cross_connect_add(unit1, &cross_connect);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
        /* option2: FOR FEC IT POSSIBLE TO ADD as destination-gport directly (without calling bcm_l2_gport_forward_info_get) as in this case gport = FEC,
          bcm_vswitch_cross_connect_t_init(&cross_connect);
          cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
          cross_connect.port1 = gport1_lif;
          cross_connect.port2 = forward_info.gport2_fec_a;
          rv = bcm_vswitch_cross_connect_add(unit1, &cross_connect);
          if (rv != BCM_E_NONE) {
              printf("Error, bcm_vswitch_cross_connect_add:gport1_lif\n");
              return rv;
          }*/


        /* gport2_lif_a --> gport1_lif */
        bcm_vswitch_cross_connect_t_init(&cross_connect);
        cross_connect.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        /* remote gport info */
        if (encoding) {
          rv = system_vlan_port_to_l2_gport_forward_info(unit1, &vp1_lif, &forward_info);
        } else {
          rv = bcm_l2_gport_forward_info_get(unit1,gport1_lif, &forward_info);
        }
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_gport_forward_info_get\n");
            return rv;
        }
        cross_connect.port1 = gport2_lif_a;
        cross_connect.port2 = port1;
        cross_connect.encap2 = forward_info.encap_id;
        if(verbose >= 2) {
            printf("forwarding info of gport(gport1_lif):0x%08x is phy_port:0x%08x encap_id:0x%08x \n",gport2_fec_a, forward_info.phy_gport, forward_info.encap_id);
        }
        rv = bcm_vswitch_cross_connect_add(unit2, &cross_connect);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add:gport2_lif_a\n");
            return rv;
        }

        /* gport2_lif_b --> gport1_lif */
        cross_connect.port1 = gport2_lif_b;
        rv = bcm_vswitch_cross_connect_add(unit2, &cross_connect);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add:gport2_lif_b\n");
            return rv;
        }
    }

    return rv;
}
