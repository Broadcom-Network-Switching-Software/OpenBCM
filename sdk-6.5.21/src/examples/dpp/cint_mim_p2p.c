/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * set following soc properties
 * # Specify the "Auxiliary table" mode. 0: Private VLAN support, 1: Split horizon mode, 2: Mac-In-Mac support.
 * bcm886xx_auxiliary_table_mode=2
 * # Select whether to enable/disable SA authentication on the device
 * sa_auth_enabled=0 
 * # Set the ARP table (next Hop MAC address) not to be extended 
 * bcm886xx_next_hop_mac_extension_enable=0 
 *  
 * the cint creates a MiM (backbone) port and a vlan (access) port. 
 * then it cross-connects the vlan port and the MiM port.
 *  
 * run:
 * cd ../../../../src/examples/dpp
 * cint cint_mim_mp.c 
 * cint cint_mim_p2p.c 
 * cint cint_port_tpid.c 
 * cint 
 * p2p_main(unit,port1,port2); 
 *  
 *  
 * run packet: 
 *      ethernet header with DA 5, SA 1 
 *      Vlan tag: vlan-id 8, vlan tag type 0x8100
 * from <port2>
 * the packet will arrive at port 1, after encapsulation with I-tag (I-SID = 16777215)
 *  
 * run the packet: 
 *      mac-in-mac header with B-DA abcdef123412, B-SA 010203040506 
 *      B-tag: B-VID 5, ethertype 0x81a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7
 * from <port1>
 * the packet will arrive at port 3, after termination
 */

/* Globals */
bcm_vswitch_cross_connect_t mim_vswitch_cross_connect_gports;

struct mim_p2p_info_s {

    bcm_gport_t port_1; /* physical MiM (backbone) port */
    bcm_port_t port_2; /* physical vlan (access) port */

    bcm_gport_t vlan_port; /* logical vlan port */
    bcm_mim_port_t mim_port; /* logical MiM port */

    bcm_mim_vpn_config_t bvid; /* B-VID */

    bcm_mac_t src_bmac; /* B-SA */
    bcm_mac_t dest_bmac; /* B-DA */

    /*bcm_stg_t stg;*/
};

mim_p2p_info_s mim_p2p_info;

int
mim_p2p_info_init(int unit, int port1, int port2) {

    int rv;

    mim_p2p_info.port_1 = port1; /* MiM (backbone) port */
    mim_p2p_info.port_2 = port2; /* access port */

    bcm_mim_port_t_init(&(mim_p2p_info.mim_port));

    bcm_vswitch_cross_connect_t_init(&mim_vswitch_cross_connect_gports);

    bcm_mim_vpn_config_t_init(&(mim_p2p_info.bvid));

    mim_p2p_info.src_bmac[0] = 0x01;
    mim_p2p_info.src_bmac[1] = 0x02;
    mim_p2p_info.src_bmac[2] = 0x03;
    mim_p2p_info.src_bmac[3] = 0x04;
    mim_p2p_info.src_bmac[4] = 0x05;
    mim_p2p_info.src_bmac[5] = 0x06;

    mim_p2p_info.dest_bmac[0] = 0xab;
    mim_p2p_info.dest_bmac[1] = 0xcd;
    mim_p2p_info.dest_bmac[2] = 0xef;
    mim_p2p_info.dest_bmac[3] = 0x12;
    mim_p2p_info.dest_bmac[4] = 0x34;
    mim_p2p_info.dest_bmac[5] = 0x12;

    return BCM_E_NONE;
}

int
mim_p2p_vlan_edit_action_add(int unit, bcm_vlan_t new_ovid, bcm_vlan_t new_ivid)
{
    bcm_vlan_action_set_t action;
    bcm_vlan_port_translation_t port_trans;
    bcm_vlan_translate_action_class_t action_class;
    int rv;
    int action_id_1= 5, action_id_2=6;

    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    
    if (advanced_vlan_translation_mode) {

        /* Create action IDs*/
        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set translation action 1. Replace outer and add inner.
           Should be applied for single tagged packets.  */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;

        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x9100;
        rv = bcm_vlan_translate_action_id_set( unit, 
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               action_id_1,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set 1\n");
            return rv;
        }

        /* Set translation action 2. Delete outer and delete inner.
           Should be applied for double tagged packets.  */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x9100;                      
        rv = bcm_vlan_translate_action_id_set( unit, 
                                               BCM_VLAN_ACTION_SET_EGRESS,
                                               action_id_2,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set 2\n");
            return rv;
        }   

        /* Set port translation */
        bcm_vlan_port_translation_t_init(&port_trans);	
        port_trans.new_outer_vlan = new_ovid;		
        port_trans.new_inner_vlan = new_ivid;
        port_trans.gport = mim_p2p_info.vlan_port;
        port_trans.vlan_edit_class_id = 2;
        port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        /* Set translation action class for single tagged packets */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = 2;
        action_class.tag_format_class_id = 2;
        action_class.vlan_translation_action_id	= action_id_1;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }

        /* Set translation action class for double tagged packets */
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = 2;
        action_class.tag_format_class_id = 6;
        action_class.vlan_translation_action_id	= action_id_2;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }
    else {
        bcm_vlan_action_set_t_init(&action);
        action.ot_outer = bcmVlanActionReplace;
        action.ot_inner = bcmVlanActionAdd;
        action.dt_outer = bcmVlanActionDelete;
        action.dt_inner = bcmVlanActionDelete;
        action.new_outer_vlan = new_ovid;
        action.new_inner_vlan = new_ivid;
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.inner_tpid_action = bcmVlanTpidActionModify;
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x9100;
       
        rv = bcm_vlan_translate_egress_action_add(unit, 
                                                  mim_p2p_info.vlan_port, 
                                                  BCM_VLAN_NONE, 
                                                  BCM_VLAN_NONE, 
                                                  action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add\n");
            return rv;
        }
    }
		
	return rv;
}

int advanced_vlan_edit_ingress(int unit, int *action_id, bcm_vlan_action_t action_type,  bcm_vlan_action_set_t *action,  int flags, int mim_port, bcm_port_tpid_class_t *tpid_class, uint32 tpid){
				
	   int rv = BCM_E_NONE;

       rv = bcm_vlan_translate_action_id_create(unit,flags,action_id);
       if (rv != BCM_E_NONE) {
           printf("Error, bcm_vlan_translate_action_id_create\n");
           return rv;
       }
       action->outer_tpid = tpid;
       action->action_id = *action_id;
       action->dt_outer = action_type;
       rv = bcm_vlan_translate_action_id_set(unit,flags,*action_id,action);
       if (rv != BCM_E_NONE) {
           printf("Error, bcm_vlan_translate_action_id_set\n");
           print rv;
           return rv;
       }
       printf("mim_port: %d\n", mim_port);
       printf("tpid: %d\n", tpid);
       bcm_port_tpid_class_t_init(tpid_class);
       tpid_class->port = mim_port;
       tpid_class->tpid1 = tpid;
       tpid_class->tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
       rv = bcm_port_tpid_class_set(unit,tpid_class);
       if (rv != BCM_E_NONE) {
           printf("Error, bcm_port_tpid_class_set\n");
           print rv;
           return rv;
       }
       
       return rv;
} 

int advanced_vlan_edit_egress(int unit, int *action_id, bcm_vlan_port_translation_t *vlan_port_translation, bcm_gport_t gport,uint32 vlan, bcm_vlan_translate_action_class_t *action_class,   uint32 vlan_edit_class_id,  bcm_vlan_action_t action_type, bcm_vlan_action_set_t *action,  int flags, uint32 tpid){
				
	   int rv = BCM_E_NONE;

        bcm_vlan_port_translation_t_init(vlan_port_translation);
        vlan_port_translation->flags = flags;
        vlan_port_translation->vlan_edit_class_id = vlan_edit_class_id;
        vlan_port_translation->gport = gport;
        vlan_port_translation->new_outer_vlan = vlan;


        rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        bcm_vlan_translate_action_class_t_init(action_class);
        action_class->flags = flags;
        action_class->vlan_edit_class_id = vlan_edit_class_id;
        action_class->tag_format_class_id = 0;
        action_class->vlan_translation_action_id = *action_id;

        rv = bcm_vlan_translate_action_class_set(unit, action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }

        flags |= BCM_VLAN_ACTION_SET_WITH_ID;
        rv = bcm_vlan_translate_action_id_create(unit,flags,*action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }
            
        action->action_id = *action_id;
        action->dt_outer = action_type;
        action->outer_tpid = tpid;

        rv = bcm_vlan_translate_action_id_set(unit,flags,*action_id,action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
            return rv;
        }
       
       return rv;
}

/* function to check if the device is Jericho and above */
int device_is_jer(int unit,
                  uint8* is_jer) {
    bcm_info_t info;
    char *proc_name;

    proc_name = "device_is_jer";
    printf("%s(): Checking if device is Jericho...\n", proc_name);
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error in: bcm_info_get() returned %d (%s)\n", proc_name, rv, bcm_errmsg(rv));
        return rv;
    }
    printf(" info.device = 0x%x...\n", info.device);
    *is_jer = ((info.device == 0x8375) || (info.device == 0x8675));
    if (*is_jer) {
        printf("%s(): Done. Device is Jericho.\n", proc_name);
    } else {
        printf("%s(): Done. Device is not Jericho.\n", proc_name);
    }
    return rv;
}

int 
p2p_main(int unit, int port1, int port2, uint32 isid) {

    int rv, action_id;
    uint32 advanced_vlan_editing;
    bcm_pbmp_t pbmp, ubmp;
    bcm_vlan_action_set_t action;
    bcm_port_tpid_class_t tpid_class;
    int flags_ingress, flags_egress;
    bcm_vlan_port_translation_t vlan_port_translation;
    bcm_vlan_translate_action_class_t action_class;
    uint32 vlan_edit_class_id;
    uint32 tpid = 0x81a8;
    int     station_id;
    uint8 is_jer;
    int vsi;

    /* First check if this is a Jericho device which has the MiM erratum and assign the VSI accordingly */
    device_is_jer(unit, &is_jer);
    vsi = (is_jer) ? 0x7FFF : 0xFFFF;

    /* configurations relevant for advanced vlan editing*/
    sal_srand(sal_time());
    vlan_edit_class_id = (sal_rand() % 7) +1;/* we deal only with vlan_edit_profile between 1 and 7*/
    action_id = 59; /* reserved default action_id for single tagged packet*/
    flags_ingress = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    flags_egress = BCM_VLAN_ACTION_SET_EGRESS;
    advanced_vlan_editing = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    rv = mim_p2p_info_init(unit, port1, port2);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_p2p_info_init\n");
        return rv;
    }

    /* init MiM on the device */
    rv = bcm_mim_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_init\n");
        print rv;
        return rv;
    }

    /* set the PBB-TE B-VID ranges */
    rv = bcm_switch_control_set(unit, bcmSwitchMiMTeBvidLowerRange, 2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set for lower range\n");
        print rv;
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchMiMTeBvidUpperRange, 22);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set for upper range\n");
        print rv;
        return rv;
    }

    /* set one physical port as MiM (backbone) port */
    rv = bcm_port_control_set(unit, mim_p2p_info.port_1, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set with port_1\n");
        print rv;
        return rv;
    }

    /* set TPIDs for user port */
    port_tpid_init(mim_p2p_info.port_2, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_2\n");
        print rv;
        return rv;
    }

    /* make the MiM physical port recognize the B-tag ethertype */
    /* Set drop to all packets without I-tag. */
    port_tpid_init(mim_p2p_info.port_1, 1, 0);
    if (advanced_vlan_translation_mode) { /* configuring the default action id for this port*/
        port_tpid_info1.vlan_transation_profile_id = action_id;
    }
    port_tpid_info1.outer_tpids[0] = 0x81a8;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_TAG;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }

    /* set MiM port ingress vlan-editing to remove B-tag if exists */
    bcm_vlan_action_set_t_init(&action);
    

    if (advanced_vlan_translation_mode) {  /* remove outer tag in case of single tagged packet in avt mode */
        
        rv = advanced_vlan_edit_ingress(unit, &action_id, bcmVlanActionDelete, &action, flags_ingress, mim_p2p_info.port_1, &tpid_class, tpid);
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit\n");
            print rv;
            return rv;
        }
    } 
    else{
        action.ot_outer = bcmVlanActionDelete; /* remove outer tag in case of single tagged packet */ 

        rv = bcm_vlan_port_default_action_set(unit, mim_p2p_info.port_1, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_set with port_2\n");
            print rv;
            return rv;
        }
    }

    /* set the global My-B-Mac MSB for termination and B-SA MSB for encapsulation */
    rv = mim_l2_station_add(unit, 1 /* MSB */, 0, mim_p2p_info.dest_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global MSB)\n");
        print rv;
        return rv;
    }

    /* set the global My-B-Mac LSB for termination */
    rv = mim_l2_station_add(unit, 0 /* LSB */, -1 /* invalid port for global configuration */, mim_p2p_info.dest_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (glaobal LSB)\n");
        print rv;
        return rv;
    }

    /* set per-src-port B-SA LSB for encapsulation */
    rv = mim_l2_station_add(unit, 0 /* LSB */, mim_p2p_info.port_2, mim_p2p_info.dest_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (per port LSB)\n");
        print rv;
        return rv;
    }

    /* prepare the port bit maps to set the B-VID port membership, with the the backbone port (used for filtering) */ 
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, mim_p2p_info.port_1);

    /* create B-VID */
    rv = mim_bvid_create(unit, &(mim_p2p_info.bvid), 5, pbmp, ubmp/*, mim_p2p_info.stg*/); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with bvid\n");
        return rv;
    }

    /* create MiM (backbone) P2P port */
    mim_port_config(&(mim_p2p_info.mim_port), mim_p2p_info.port_1, mim_p2p_info.bvid.vpn, mim_p2p_info.src_bmac,
                    mim_p2p_info.bvid.vpn, mim_p2p_info.dest_bmac, isid /* I-SID for P2P port */); 
    rv = bcm_mim_port_add(unit, vsi, &(mim_p2p_info.mim_port)); /* use invalid VPN for P2P (0x7fff in Jericho, 0xffff in other devices) */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add\n");
        print rv;
        return rv;
    }

    /* Set TPIDs for the ISID-LIF */
    port_tpid_init(mim_p2p_info.mim_port.service_encap_id, 1, 1);
    rv = port_lif_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with ISID-LIF\n");
        print rv;
        return rv;
    }


    /* create an access (vlan) port:
       egress_vlan (out vlan) = match_vlan (in vlan) */
    rv = vlan_port_create(unit, mim_p2p_info.port_2, &(mim_p2p_info.vlan_port), 8);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create\n");
        return rv;
    }
    /* Adding customer vlan (equals 8) via avt action == bcmVlanActionAdd
       * Upon entrance we perform delete, so upon exit we need to add the vlan
       * this configuration replaces the normal mode configuration  in vlan_port_create (ni cint_mim_mp.c), in which 
       * we do vp.egress_vlan = vlan */ 
    if (advanced_vlan_editing) { 

        action_id += 1; /* setting to action_id == 60 */ 
        tpid = 0x8100;
                          
        rv = advanced_vlan_edit_egress(unit, &action_id, &vlan_port_translation, mim_p2p_info.vlan_port,8, &action_class,  vlan_edit_class_id,  bcmVlanActionAdd, &action, flags_egress, tpid);

        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_egress\n");
            return rv;
        }

    }

    /* cross connect the MiM port with the vlan port */
    mim_vswitch_cross_connect_gports.port1 = mim_p2p_info.mim_port.mim_port_id;
    mim_vswitch_cross_connect_gports.port2 = mim_p2p_info.vlan_port;

    rv = bcm_vswitch_cross_connect_add(unit, &mim_vswitch_cross_connect_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    vp1.vlan_port_id = mim_p2p_info.vlan_port;
    rv = bcm_vlan_port_find(unit, &vp1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_find with vlan_port_id: 0x%08X\n", mim_p2p_info.vlan_port);
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}


int p2p_replace_mim_port(int unit)
{
    int rv;
    int vsi;
    uint8 is_jer;
    bcm_mim_port_t mim_port;

    device_is_jer(unit, &is_jer);
    vsi = (is_jer) ? 0x7FFF : 0xFFFF;

    bcm_mim_port_t_init(&mim_port);
    mim_port.mim_port_id = mim_p2p_info.mim_port.mim_port_id;
    rv = bcm_mim_port_get(unit, vsi, &mim_port);
    if (rv != 0) {
        printf("Error, bcm_mim_port_add\n");
        print rv;
        return rv;
    }

    mim_port.flags |= BCM_MIM_PORT_WITH_ID | BCM_MIM_PORT_ENCAP_WITH_ID | BCM_MIM_PORT_REPLACE;
    rv = bcm_mim_port_add(unit, vsi, &mim_port);
    if (rv != 0) {
        printf("Error, bcm_mim_port_add\n");
        print rv;
        return rv;
    }

    return rv;
}

