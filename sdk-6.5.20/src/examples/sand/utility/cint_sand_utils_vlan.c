/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * This file provides vlan editing basic functionality and defines vlan editing global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 */

/* Used for action definition*/
struct vlan_action_utils_s{
    int action_id;
    uint16 o_tpid;
    bcm_vlan_action_t o_action;
    uint16 i_tpid;
    bcm_vlan_action_t i_action;
};

/* Entry for vlan edit profile table */
struct vlan_edit_utils_s{
    int edit_profile;
    bcm_port_tag_format_class_t tag_format;
    int action_id;    
};


vlan_edit_utils_s g_eve_edit_utils = {2,    /* edit profile */
                                      5,    /* tag format */
                                      7};   /* action ID */


/* ************************************************************************************************** */

/* Define a basic VLAN Translation Action.
 * The VIDs source actions and the TPID values are supplied, but with no PCP-DEI configuration. 
 * Applicable only in AVT mode. 
 *  
 * INPUT: 
 *   is_ingress: Applies the API on the Ingress ot Egress VLAN translate.
 *   outer_tpid: Outer TPID value
 *   inner_tpid: Inner TPID value
 *   outer_action: Source for the Outer VID value
 *   inner_action: Source for the Inner VID value
 *   action_id: Returned Action ID as allocated by the function.
 */
int vlan__avt_vid_action__set(int unit,
                              uint32 is_ingress,
                              uint16 outer_tpid,
                              uint16 inner_tpid,
                              bcm_vlan_action_t outer_action,
                              bcm_vlan_action_t inner_action,
                              int *action_id)
{
    int rv;
    uint32 flags;
    bcm_vlan_action_set_t action;
    
    /* The configuration is either for Ingress or Egress */
    flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /* Create an Action ID */
    rv = bcm_vlan_translate_action_id_create(unit, flags, action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create failed, rv - %d\n", rv);
        return rv;
    }

    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;
    rv = bcm_vlan_translate_action_id_set(unit, flags, *action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set for action_id - %d, rv - %d\n", *action_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Configure a LIF with VLAN translation information.
 * The VID values for new Outer/Inner tag and a VLAN-Edit Profile. No PCP-DEI configuration. 
 * Applicable only in AVT mode. 
 *  
 * INPUT: 
 *   is_ingress: Applies the API on the Ingress ot Egress VLAN translate.
 *   gport_id: The LIF for which the configuration is performed.
 *   new_outer_vid: VID value for a new Outer tag
 *   new_inner_vid: VID value for a new Inner tag
 *   vlan_edit_profile: VLAN-Edit profile value
 */
int vlan__avt_lif_info__set(int unit,
                            uint32 is_ingress,
                            bcm_gport_t gport_id,
                            bcm_vlan_t new_outer_vid,
                            bcm_vlan_t new_inner_vid,
                            uint32 vlan_edit_profile)
{
    bcm_vlan_port_translation_t lif_translation_info;
    int rv;

    bcm_vlan_port_translation_t_init(&lif_translation_info);
    
    /* The configuration is either for Ingress or Egress */
    lif_translation_info.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /* Set port translation */
    lif_translation_info.new_outer_vlan = new_outer_vid;		
    lif_translation_info.new_inner_vlan = new_inner_vid;
    lif_translation_info.gport = gport_id;
    lif_translation_info.vlan_edit_class_id = vlan_edit_profile;
    rv = bcm_vlan_port_translation_set(unit, &lif_translation_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set failed for gport_id - %d, rv - %d\n", lif_translation_info.gport, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Perform a basic EVE configuration.
 * The LIF, TPID values, VID values and VLAN-Edit profile are supplied.
 * The Action is defined, the LIF VLAN-Edit information is configured as well 
 * as mapping between the VLAN-Edit Profile and q fixed tag format.
 *  
 * INPUT: 
 *   lif: Out-LIF to set
 *   gport_id: The LIF for which the configuration is performed.
 *   new_outer_vid: VID value for a new Outer tag
 *   new_inner_vid: VID value for a new Inner tag
 *   vlan_edit_profile: VLAN-Edit profile value
 */
int vlan__avt_eve_vids__set(int unit,
                            bcm_gport_t lif,
                            int outer_tpid,
                            int inner_tpid,
                            bcm_vlan_action_t outer_action,
                            bcm_vlan_action_t inner_action,
                            bcm_vlan_t new_outer_vid,
                            bcm_vlan_t new_inner_vid,
                            uint32 vlan_edit_profile)
{
    int rv, action_id;
    uint32 is_ingress = 0;

    rv = vlan__avt_vid_action__set(unit, is_ingress, outer_tpid, inner_tpid, outer_action, inner_action, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__avt_vid_action__set failed, rv - %d\n", rv);
        return rv;
    }

    rv = vlan__avt_lif_info__set(unit, is_ingress, lif, new_outer_vid, new_inner_vid, vlan_edit_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__avt_lif_info__set failed for lif - %d, rv - %d\n", lif, rv);
        return rv;
    }

    rv = vlan__translate_action_class__set(unit, action_id, vlan_edit_profile, g_eve_edit_utils.tag_format, is_ingress);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__translate_action_class__set failed for action_id - %d, rv - %d\n", action_id, rv);
        return rv;
    }

    return BCM_E_NONE;
}








/* set translation actions (replace) */


/* set translation action classes for different tag formats */


int vlan__eve_default__set(int unit, bcm_gport_t  lif, int outer_vlan, int inner_vlan , bcm_vlan_action_t outer_action, bcm_vlan_action_t inner_action){

    int rv;

    /* configure out-ac with vlans and vlan edit profile */
    rv = vlan_port_translation_set(unit, outer_vlan, inner_vlan, lif, g_eve_edit_utils.edit_profile, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    if (verbose) {
        printf("AC (0x%x) configured: outer-vlan: %d, inner-vlan: %d, ve profile: %d \n", lif, outer_vlan, inner_vlan, g_eve_edit_utils.edit_profile); 

    }

    /* set editing actions:
       create 1 vlan edit command (only action 1 provided),
    */
    rv = vlan_translate_action_set(unit, 
                                   g_eve_edit_utils.action_id, 
                                   0 /* action 2*/, 
                                   0 /* is ingress */, 
                                   outer_action /* action for the outer vlan */,
                                   inner_action /* action for the inner vlan */
                                   );
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_set\n");
        return rv;
    }

    if (verbose) {
        printf("set action id: %d\n", g_eve_edit_utils.action_id);
    }

    
    rv = vlan_default_translate_action_class_set(unit, g_eve_edit_utils.action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_translate_action_class_set\n");
        return rv;
    }

    return BCM_E_NONE;

}

/* Configure VLAN editing
   Used for routing / ROO.
   For routing, at EVE, a new LL is built, so the tag format is untagged (LL is built without tag)
 
   no outgoing parameters
 
   Create out AC.
   set vlan edit profile for out AC.
   set outer vlan and inner for out AC.
   create 2 actions: add 2 vlans.
   map AC lif.vlan edit profile and tag format (0 for untag) to action id
 
   Incoming parameters:
   - port, vsi: used for ESEM lookup: port x VSI -> outAC
   - outer vlan, inner vlan: vlans to add to the packet.
 */

int vlan__eve__routing__set(int unit, bcm_port_t port, bcm_vlan_t vsi, bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan) {
    int rv = BCM_E_NONE; 

    int vlan_edit_profile = 1; 

    /* create out AC */
    bcm_vlan_port_t vlan_port; 
    bcm_vlan_port_t_init(&vlan_port); 
/*  vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY; */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;  
    vlan_port.port = port; 
    vlan_port.vsi = vsi; 
    rv = bcm_vlan_port_create(unit, &vlan_port); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    if (verbose >=2) {
        printf("create out AC: 0x%x. \n", vlan_port.vlan_port_id); 
        printf("add ESEM entry for out AC: port x vsi -> out AC: %d x %d -> 0x%x \n", port, vsi, vlan_port.vlan_port_id); 
    }

    /* set vlan edit profile for out AC.
       set outer vlan and inner for out AC.*/
    rv = vlan_port_translation_set(unit, 
                                   outer_vlan, inner_vlan, /* vlans are a property of the out AC */
                                   vlan_port.vlan_port_id, /* out AC */
                                   vlan_edit_profile,  
                                   0); /* is ingress */
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set\n");
        return rv;
    }

    if (verbose >=2) {
        printf("update AC %d with vlan edit profile %d, outer vlan %d and inner vlan:%d\n", vlan_port.vlan_port_id, vlan_edit_profile, outer_vlan, inner_vlan); 
    }

    /* set edit actions */
    int action_id_1 = 50; 
    int action_id_2 = 51; 

    rv = vlan_translate_action_set(
       unit, 
       action_id_1, 
       action_id_2, 
       0, /* is_ingress */ 
       bcmVlanActionAdd, /* outer action */
       bcmVlanActionAdd); /* inner action */
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_default_translate_action_set\n");
        return rv;
    }

    /* map AC lif.vlan edit profile and tag format (0 for untag) to action id */
    /* Set translation action class for untagged */
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile; /* vlan edit profile */
    action_class.tag_format_class_id = 0; /* tag format: 0 for untagged  */
    action_class.vlan_translation_action_id	= action_id_2; /* action id: build 2 tags */
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}


/* set vlan port translation, determine edit profile for LIF */
int vlan__port_translation__set(int unit, bcm_vlan_t new_vid, bcm_vlan_t new_inner_vid, bcm_gport_t vlan_port, uint32 edit_class_id, uint8 is_ingress) {
    bcm_vlan_port_translation_t port_trans;
    int rv;
    
    /* Set port translation */
    bcm_vlan_port_translation_t_init(&port_trans);	
    port_trans.new_outer_vlan = new_vid;		
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = vlan_port;
    port_trans.vlan_edit_class_id = edit_class_id;
    port_trans.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    return rv;
}


/* Define actions */
int vlan__translate_action_with_id__set(int unit, int action_id,
                                              uint16 outer_tpid, bcm_vlan_action_t outer_action,
                                              uint16 inner_tpid, bcm_vlan_action_t inner_action,  
                                              uint8 is_ingress) {
    bcm_vlan_action_set_t action;
    uint32 flags;
    int rv;

    flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;

    /* Create action ID*/
    rv = bcm_vlan_translate_action_id_create( unit, flags | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }
    
    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;

    rv = bcm_vlan_translate_action_id_set( unit, 
                                           flags,
                                           action_id,
                                           &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set 1\n");
        return rv;
    }  
    
    return rv; 
}


/* Set translation action - connect tag format and edit profile with action*/
int vlan__translate_action_class__set(int unit, int action_id, 
                                                uint32 edit_profile, 
                                                bcm_port_tag_format_class_t tag_format,
                                                uint8 is_ingress ) {
    bcm_vlan_translate_action_class_t action_class;
    int rv;

    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id	= action_id;
    action_class.flags = is_ingress ? BCM_VLAN_ACTION_SET_INGRESS : BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}


int vlan__init_vlan(int unit, int vlan) {
    bcm_error_t rv;
    bcm_port_config_t c;
    bcm_pbmp_t p,u;
    bcm_module_t mreps[2];
    uint32 max_nof_cores = 0, is_mesh = 0, is_mesh_mc, is_tdm_supported;

    rv = bcm_port_config_get(unit, &c);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get\n");
        return rv;
    }
    BCM_PBMP_ASSIGN(p, c.e);
    
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create, vlan=%d, \n", vlan);
        return rv;
    }
    
    rv = bcm_vlan_port_add(unit, vlan, p, u);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        return rv;
    }

    /*
     * 1). Create the corresponding mc-id to the vlan for JR2(For JR, this is done internally during init).
     * 2). Add the port to the mc-id
     */
    if (is_device_or_above(unit, JERICHO2)) {

        bcm_multicast_destroy(unit, vlan);

        uint32 flags;
        flags = BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP;
        rv = bcm_multicast_create(unit, flags, vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_create, flags $flags mc_group_id $mc_group_id \n");
            return rv;
        }

        BCM_PBMP_CLEAR(p);
        rv = bcm_vlan_port_get(unit, vlan, &p, &u);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_get, vlan %d\n", vlan);
            return rv;
        }

        /* Add cpu port only since nwk ports can be distinguished by adapter.*/
        BCM_PBMP_AND(p, c.cpu);

        bcm_multicast_replication_t rep_array[512];
        int logical_port;
        int nof_rep = 0;
        BCM_PBMP_ITER(p, logical_port) {
            bcm_multicast_replication_t_init(&rep_array[nof_rep]);

            rep_array[nof_rep].port = logical_port;
            rep_array[nof_rep].encap1= 0;
            nof_rep++;
        }

        flags = BCM_MULTICAST_EGRESS_GROUP; 
        rv = bcm_multicast_add(unit, vlan, flags, nof_rep, rep_array);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_multicast_add, group %d\n", vlan);
            return rv;
        }

        if (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)) {
            /* FABRIC_CONNECT_MODE_FE don't support API bcm_fabric_multicast_set */

            if (*dnxc_data_get(unit, "fabric", "general", "connect_mode", NULL) != 2) {
                is_mesh = 1;
            }

            is_mesh_mc = *(dnxc_data_get(unit, "fabric", "mesh", "multicast_enable", NULL));
            is_tdm_supported = *(dnxc_data_get(unit, "tdm", "params", "mode", NULL));

            if (is_mesh && (is_mesh_mc || is_tdm_supported)) {
                max_nof_cores = *(dnxc_data_get(unit, "device", "general", "nof_cores", NULL));
                mreps[0] = 0;
                mreps[1] = 1;
                rv = bcm_fabric_multicast_set(unit, vlan, 0, max_nof_cores, mreps);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_fabric_multicast_set, group %d\n", vlan);
                    return rv;
                }
            }
        }

        if (verbose >=1) {
           printf("Create multicast group for vlan %d and add ports to it!\n", vlan); 
        }
    }

    return rv;
}


/*
 * Function:
 *      vlan__vlan_port_vsi_create 
 * Purpose:
 *      Given a vsi, port, and one or two vlans this function:
 *          1. Sets the port's vlan domain to be = port.
 *          2. Maps the <port,vlan> or <port,vlan,vlan> combinations the the vsi.
 * Parameters: 
 *      unit            - Device id to be configured.
 *      vsi             - Vsi to be mapped to.
 *      port            - Mod/port/trunk in the key.
 *      outer_vid       - Outer vid in the key.
 *      inner_vid       - (Optional) inner vid in the key.
 * Returns:
 *      BCM_E_*
 */
int vlan__vlan_port_vsi_create(int unit, int vsi, bcm_port_t port, bcm_vlan_t outer_vid, bcm_vlan_t inner_vid, uint32 flags, bcm_gport_t *gport){
    bcm_vlan_port_t vlan_port;
    uint8 dtag = inner_vid != BCM_VLAN_INVALID;
    int rv;

    /* First, set the port's vlan domain to match the port itself. */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_class_set, port %d, vlan_domain %d\n", port, port);
      return rv;
    }

    /* Now, set the port. */
    bcm_vlan_port_t_init(&vlan_port);

    if (flags == BCM_VLAN_PORT_CREATE_INGRESS_ONLY){
        vlan_port.criteria = (dtag) ? BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED : BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.port = port;
        vlan_port.match_vlan = outer_vid;
        vlan_port.match_inner_vlan = (dtag) ? inner_vid : 0;
        vlan_port.vsi = vsi;

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_create\n");
            print rv;
        } else {
            printf("Created vlan port for vsi mapping: 0x%x\n", vlan_port.vlan_port_id);
        }
    } else {
        vlan_port.criteria = (dtag) ? BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED : BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.flags = flags;
        vlan_port.port = port;
        vlan_port.match_vlan = outer_vid;
        vlan_port.match_inner_vlan = (dtag) ? inner_vid : 0;
        vlan_port.vsi = 0;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_create\n");
            print rv;
        } else {
            printf("Created vlan port for vsi mapping: 0x%x\n", vlan_port.vlan_port_id);
        }
        if (vsi != 0 && is_device_or_above(unit,JERICHO2)) {
            rv = bcm_vswitch_create_with_id(unit, vsi);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("Error, in bcm_vswitch_create(vsi = %d)\n", vsi);
                return rv;
            }
            rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_vswitch_port_add(port = %d)\n", vlan_port.vlan_port_id);
                return rv;
            }
        }
    }
    if (gport != NULL) {
        *gport = vlan_port.vlan_port_id;
    }
    return rv;
}


/*
 * Function:
 *      vlan__open_vlan_per_ports 
 * Purpose:
 *      Open new vsi and adds all ports in ports in the array as members.
 * Parameters: 
 *      unit            - Device id to be configured.
 *      vid             - The vlan to open.
 *      ports           - An array of ports to be added to the vlan.
 *      tagged          - An array indicating per port whether it should be tagged or untagged. Must be same
 *                              length as the ports array.
 *      nof_ports       - How many members in the ports and tagged arrays.
 * Returns:
 *      BCM_E_*
 */
int
vlan__open_vlan_per_ports(int unit, uint16 vid, bcm_gport_t *ports, uint8 *tagged, int nof_ports) {

    bcm_error_t rv;
    int port_id;
    int flags;

    /* open vlan */
    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE) {
      if(rv == BCM_E_EXISTS) {
          printf("already exists vlan=%d, \n", vid);
          rv = BCM_E_NONE;
      }

      if (rv != BCM_E_NONE) {
          printf("Error, in bcm_vlan_create, vlan=%d, \n", vid);
          return rv;
      }
    }

    printf("Open vlan %d \n", vid);

    /* add ports to vlan */
    for(port_id = 0; port_id < nof_ports; ++port_id) {
       flags = 0;
       if (!tagged[port_id]) {
          flags = BCM_VLAN_PORT_UNTAGGED;
       }
        rv = bcm_vlan_gport_add(unit, vid, ports[port_id], flags);
        if (rv != BCM_E_NONE) {
            printf("fail add port(0x%08x) to vlan(%d)\n", port, vid);
          return rv;
        }
        printf(" - port %d: (0x%08x) tagged:%d\n", port_id,ports[port_id], tagged[port_id]);
    }
    return rv;
}


/*
 * Function:
 *      vlan__open_vlan_per_mc 
 * Purpose:
 *      Open new vsi and attach it to the give mc_group.
 * Parameters: 
 *      unit            - Device id to be configured.
 *      vlan            - The vlan to open.
 *      mc_group        - The mc_group to be assigned.
 * Returns:
 *      BCM_E_*
 */
int 
vlan__open_vlan_per_mc(int unit, int vlan, bcm_multicast_t mc_group)
{
  bcm_vlan_control_vlan_t control_vlan;
  int rc;

  bcm_vlan_control_vlan_t_init(&control_vlan);
  
  /* The vlan in this case may also represent a vsi. in that case, it should be created with a different api */
  if (vlan <= BCM_VLAN_MAX) {
      rc = bcm_vlan_create(unit, vlan);
  } else {
      rc = bcm_vswitch_create_with_id(unit, vlan);
  }
  if (rc != BCM_E_NONE) {
      printf("fail open vlan(%d)", vlan);
      if (rc != BCM_E_EXISTS) {
          return rc;
      }
      else if(verbose >= 3) {
          /* The vlan may have been opened on a previous run. If that's the case, don't fail the test */
          printf("    continue..\n");
      }
      
  }

  
  /* Set VLAN with MC */
  rc = bcm_vlan_control_vlan_get(unit, vlan, &control_vlan);
  if (rc != BCM_E_NONE) {
      printf("fail get control vlan(%d)\n", vlan);
      return rc;
  }

  control_vlan.unknown_unicast_group = mc_group;
  control_vlan.unknown_multicast_group = mc_group;
  control_vlan.broadcast_group = mc_group;
  rc = bcm_vlan_control_vlan_set(unit, vlan, control_vlan);
  if (rc != BCM_E_NONE) {
      printf("fail set control vlan(%d)\n", vlan);
      return rc;
  }

  return rc;
}

/*****************************************************************************
* Function:  vlan__control_vlan_vsi_profile_set
* Purpose:  Set vsi profile to certian vsi 
* Params:
* unit        - 
* vlan      - 
* vsi_profile - 
* Return:    (int)
 */
int vlan__control_vlan_vsi_profile_set(int unit,int vlan,uint32 vsi_profile)
{
    bcm_vlan_control_vlan_t control_vlan;
    int rc;

    bcm_vlan_control_vlan_t_init(&control_vlan);

    rc = bcm_vlan_control_vlan_get(unit, vlan, &control_vlan);
    if (rc != BCM_E_NONE) {
        printf("fail get control vlan(%d)\n", vlan);
        return rc;
    }

    control_vlan.if_class = vsi_profile;
    rc = bcm_vlan_control_vlan_set(unit, vlan, control_vlan);
    if (rc != BCM_E_NONE) {
        printf("fail set control vlan(%d)\n", vlan);
        return rc;
    }

    return rc;
    
}


int vlan__not_vlan_member_trap_config(int unit, int strength, bcm_gport_t dst_port)
{
    int rv = BCM_E_NONE;

    bcm_rx_trap_config_t config;
    int trap_id;
    bcm_rx_trap_config_t_init(&config);
    bcm_rx_trap_t trap = bcmRxTrapPortNotVlanMember;

    /*drop*/
    config.flags = (BCM_RX_TRAP_UPDATE_DEST); 
    config.trap_strength = strength;
    config.dest_port = dst_port;

    rv = bcm_rx_trap_type_create(unit,0,trap,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_type_create\n");
        return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_rx_trap_set\n");
        return rv;
    }

    return rv;
}

int vlan__ingress_membership_create(int unit, bcm_vlan_t vlan, bcm_gport_t port, bcm_gport_t trap_dst_port)
{
    int rv = BCM_E_NONE;

    int flags = BCM_VLAN_GPORT_ADD_INGRESS_ONLY ;
    int trap_strength = 7;

    rv = vlan__not_vlan_member_trap_config(unit, trap_strength, trap_dst_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan__not_vlan_member_trap_config\n");
        return rv;
    }

    rv = bcm_vlan_gport_add(unit,vlan, port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_port_vlan_member_set(unit,port,BCM_PORT_VLAN_MEMBER_INGRESS);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }

    return rv;
}

int vlan__egress_membership_create(int unit, bcm_vlan_t vsi, bcm_gport_t port)
{
    int rv = BCM_E_NONE;

    int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY ;

    rv = bcm_vlan_gport_add(unit,vsi, port,flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_port_vlan_member_set(unit,port,BCM_PORT_VLAN_MEMBER_EGRESS);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }

    return rv;
}

/*
   modify default VE profile:
   Only for AVT (vlan edit profile are managed by user only in AVT)
   Create vlan edit command. action nope.
   Set the vlan edit command for ve profile 1.
   Set default ve profile=1
*/
int vlan__default_ve_profile_set(int unit, int ve_profile) {
    int rv = BCM_E_NONE;

    /* create vlan edit command */
    /* create vlan edit command */
    int action_id; /* Vlan edit command id */

    if (is_device_or_above(unit, JERICHO2)) {
        return BCM_E_UNAVAIL;
    }

    rv = vlan__avt_vid_action__set(unit,
                                   0 /* is ingress */,
                                   0x8100 /* outer TPID */,
                                   0x8100 /* inner TPID */,
                                   bcmVlanActionNone, /* outer action */
                                   bcmVlanActionNone, /* inner action */
                                   &action_id/* Vlan edit command id */);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    /* map VE profile and tag format to vlan edit command */
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ve_profile; /* vlan edit profile */
    action_class.vlan_translation_action_id = action_id; /* vlan edit command */
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    bcm_vlan_translate_action_class_set(unit, action_class);

    if (verbose) {
        printf("map ve profile and tag format to vlan edit command (%d) for default VE profile \n", action_id);
    }

    /* set default ve profile */
    rv = bcm_switch_control_set(unit,  bcmSwitchDefaultEgressVlanEditClassId, ve_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

    return rv;
}

/* set port default out AC. */
int vlan__default_out_ac_set(int unit, bcm_gport_t port, bcm_gport_t vlan_port) {

    int rv = BCM_E_NONE; 

    bcm_port_match_info_t port_match_info; 
    bcm_port_match_info_t_init(&port_match_info); 

    port_match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY; 
    port_match_info.match = BCM_PORT_MATCH_PORT; /* indicate we set default out ac per port */
    port_match_info.match_vlan = BCM_VLAN_ALL; 
    port_match_info.port = port;

    rv = bcm_port_match_add(unit,vlan_port, &port_match_info);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_port_match_add\n");
       return rv;
    }

    if (verbose) {
        printf("set default AC: 0x%x for port: %d\n", vlan_port, port); 
    }
         
    return rv; 
}

/* allocate a vlan port and configure default out ac
  1. Modify the defaut out ac per port
  1.1 Create a new vlan port, configure it so it adds 2 tags:
    outer tag: vlan = ac.outer_vlan
    inner tag: vlan = ac.inner_vlan
  1.2 set port's default out aac

   */
int vlan__default_out_ac_allocate_and_set(int unit, int port) {
    int rv = BCM_E_NONE; 

    int ve_profile = 4; 
    /* create out AC LIF */
    bcm_vlan_port_t vlan_port; 
    bcm_vlan_port_t_init(&vlan_port); 
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY; 
    if (is_device_or_above(unit, JERICHO2))
    {
        vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    }
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;  
    rv = bcm_vlan_port_create(unit, &vlan_port); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    if (verbose) {
        printf("AC created: 0x%x \n", vlan_port.vlan_port_id);
    }

    /* configure vlan edit profile, and set vlans for vlan port */
    uint32 outer_vlan = 16; 
    uint32 inner_vlan = is_device_or_above(unit, JERICHO2) ? 0:456;
    /* configure AC-lif.vlans and AC-lif.VE-profile */
    rv = vlan__port_translation__set(unit, 
                                     outer_vlan /* outer vlan */,
                                     inner_vlan /* inner vlan */, 
                                     vlan_port.vlan_port_id /* ac-lif */, 
                                     ve_profile, 
                                     0 /* is ingress */); 
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    if (verbose) {
        printf("AC (0x%x) configured: outer-vlan: %d, inner vlan: %d, ve profile: %d \n", vlan_port.vlan_port_id, outer_vlan, inner_vlan, ve_profile);
    }

    /* create vlan edit command */
    int vlan_action_id; 
    rv = vlan__avt_vid_action__set(unit, 
                                   0 /* is ingress */, 
                                   0x8100 /* outer TPID */, 
                                   0x9100 /* inner TPID */, 
                                   bcmVlanActionAdd, /* outer action */
                                   bcmVlanActionAdd, /* inner action */
                                   &vlan_action_id  /* Vlan edit command id */); 
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }

    if (verbose) {
        printf("create vlan edit command (%d) for native AC (0x%x) \n", vlan_action_id, vlan_port.vlan_port_id);
    }

    /* map VE profile and tag format to vlan edit command */
    bcm_vlan_translate_action_class_t action_class; 
    bcm_vlan_translate_action_class_t_init(&action_class); 
    action_class.vlan_edit_class_id = ve_profile; /* vlan edit profile */
    action_class.vlan_translation_action_id = vlan_action_id; /* vlan edit command */
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS; 
    rv = bcm_vlan_translate_action_class_set(unit, action_class); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }
    if (verbose) {
        printf("map ve profile and tag format to vlan edit command (%d) for native AC (0x%x) \n", vlan_action_id, vlan_port.vlan_port_id); 
    }

    /* set port's default out ac */
    rv = vlan__default_out_ac_set(unit,port,vlan_port.vlan_port_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__default_out_ac_set\n");
        return rv;
    }


    return rv; 
}

/* Add native default out AC match for JR2. */
int vlan__native_default_out_ac_set(int unit, bcm_gport_t l2lif_port, bcm_gport_t native_vlan_port) {

    int rv = BCM_E_NONE; 

    bcm_port_match_info_t port_match_info; 
    bcm_port_match_info_t_init(&port_match_info); 

    port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY; 
    port_match_info.match = BCM_PORT_MATCH_PORT; /* indicate that lookup fields are port */
    port_match_info.port = l2lif_port;

    rv = bcm_port_match_add(unit,native_vlan_port, &port_match_info);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_port_match_add\n");
       return rv;
    }

    if (verbose) {
        printf("set default AC: 0x%x for port: %d\n", native_vlan_port, l2lif_port); 
    }

    return rv; 
}

/* For QAX / Jer+ devices only. 
   Create a native AC-LIF.
   Configure it as default AC. */ 
int vlan__native_default_out_ac_allocate_and_set(int unit, 
                                                 int outer_vlan, 
                                                 int inner_vlan, 
                                                 vlan_action_utils_s *action, /* action_id, actions and tpids */
                                                 bcm_gport_t l2lif_port) {
    int rv = BCM_E_NONE; 

    advanced_vlan_translation_mode = soc_property_get(unit, "bcm886xx_vlan_translate_mode",0);

    int ve_profile = 3; 
    /* create native out AC LIF */
    bcm_vlan_port_t vlan_port; 
    bcm_vlan_port_t_init(&vlan_port); 
    /* SVT doens't work with INGRESS_ONLY vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_NATIVE; */
    vlan_port.flags = BCM_VLAN_PORT_NATIVE;
    if (is_device_or_above(unit, JERICHO2))
    {
        vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.flags |= BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    }
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;

    if (is_device_or_above(unit, JERICHO2) && (l2lif_port == -1)) {
        /** In this case, the global native default ac will be changed.*/
        vlan_port.vlan_port_id = *dnxc_data_get(unit, "esem", "default_result_profile",
                                                 "default_native", NULL);
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
    } else {
        rv = bcm_vlan_port_create(unit, &vlan_port); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
    }
    if (verbose) {
        printf("Native AC created: 0x%x \n", vlan_port.vlan_port_id);
    }


    /* SVT: create VEC and configure vlan to ac */
    if (!advanced_vlan_translation_mode && !is_device_or_above(unit, JERICHO2)) {
        bcm_vlan_action_set_t svt_action; 
        bcm_vlan_action_set_t_init(&svt_action); 
        svt_action.ut_outer = action->o_action;
        svt_action.ut_inner = action->i_action;

        rv = bcm_vlan_translate_egress_action_add(unit, vlan_port.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, svt_action); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_egress_action_add\n");
            return rv;
        }

    } 
    /* AVT or JR2*/
    else {
        /* configure vlan edit profile, and set vlans for vlan port */

        /* configure AC-lif.vlans and AC-lif.VE-profile */
        rv = vlan__port_translation__set(unit, 
                                         outer_vlan,
                                         inner_vlan /* inner vlan */, 
                                         vlan_port.vlan_port_id /* ac-lif */, 
                                         ve_profile, 
                                         0 /* is ingress */); 
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__port_translation__set\n");
            return rv;
        }

        if (verbose) {
            printf("Native AC (0x%x) configured: outer-vlan: %d, inner vlan: %d, ve profile: %d \n", vlan_port.vlan_port_id, outer_vlan, inner_vlan, ve_profile);
        }


        /* create vlan edit command */
        rv = vlan__avt_vid_action__set(unit, 
                                       0 /* is ingress */, 
                                       action->o_tpid /* outer TPID */, 
                                       action->i_tpid /* inner TPID */, 
                                       action->o_action,  /* outer action: default bcmVlanActionAdd  */
                                       action->i_action, /* inner action: default bcmVlanActionNone*/
                                       &(action->action_id) /* Vlan edit command id: g_vxlan_roo.native_vlan_action_id  */); 
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__port_translation__set\n");
            return rv;
        }

        if (verbose) {
            printf("create vlan edit command (%d) for native AC (0x%x)\n", action->action_id, vlan_port.vlan_port_id);
            printf("outer action: %s, inner action: %s\n", (action->o_action == bcmVlanActionNone)? "none" : "other", 
                                                           (action->i_action == bcmVlanActionNone)? "none" : "other");

        }

        /* map VE profile and tag format to vlan edit command */
        bcm_vlan_translate_action_class_t action_class; 
        bcm_vlan_translate_action_class_t_init(&action_class); 
        action_class.vlan_edit_class_id = ve_profile; /* vlan edit profile */
        action_class.vlan_translation_action_id = action->action_id; /* vlan edit command */
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS; 
        bcm_vlan_translate_action_class_set(unit, action_class); 

        if (verbose) {
            printf("map ve profile and tag format to vlan edit command (%d) for native AC (0x%x) \n", action->action_id, vlan_port.vlan_port_id); 
        }
    }

    /* set default native out ac */
    /* JR2 doesn't support default native out ac setting */
    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_switch_control_set(unit, bcmSwitchDefaultNativeOutVlanPort, vlan_port.vlan_port_id); 
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control\n");
            return rv;
        }
    } else if (l2lif_port != -1) {
        /* set port's default out ac */
        rv = vlan__native_default_out_ac_set(unit,l2lif_port,vlan_port.vlan_port_id); 
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__native_default_out_ac_set\n");
            return rv;
        }
    }

    if (verbose) {
        printf("set AC as default native out ac: 0x%x\n", vlan_port.vlan_port_id); 
    }


    return rv; 
}


/* define default native out ac
   with default action: none. */
int vlan__native_default_out_ac_allocate_and_set_dflt_action(int unit, int outer_vlan, int inner_vlan, bcm_gport_t l2lif_port) {

    int rv; 
    vlan_action_utils_s default_action; 
    default_action.o_action = bcmVlanActionNone; 
    default_action.i_action = bcmVlanActionNone;
    default_action.o_tpid = 0x8100; 
    default_action.i_tpid = 0x8100; 

    rv = vlan__native_default_out_ac_allocate_and_set(unit, outer_vlan, inner_vlan, &default_action, l2lif_port); 
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__native_default_out_ac_allocate_and_set\n");
        return rv;
    }

    return rv;

}

