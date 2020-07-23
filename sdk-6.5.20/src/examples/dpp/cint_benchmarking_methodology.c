/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_benchmarking_methodology.c
 * Purpose: Example Usage of egress programs used for benchmarking methedology (RFC-2544)
 */

/*
  Below is an example of how to use the egress editor programs specifically 
  designed for devices operating as reflectors 
  when set up in the following configuration: 
 
       +----------+         +-------------+        +-----------+
       |sender/   |-------->|  device(s)  |------->|           |
       |receiver  |         |    under    |        | reflector |
       |          |<--------|  testing    |<-------|           |
       +----------+         +-------------+        +-----------+

 
Usage: 
1) 
    One of the soc properties
		RFC2544_reflector_mac_swap_port 
		RFC2544_reflector_mac_and_ip_swap_port 
    should be set to the reflector port. The reflector port must be defined as an Ethernet port.
2) 
    Load the cints
    utility/cint_utils_vlan.c
    cint_benchmarking_methodology.c
    cint_advanced_vlan_translation_mode.c
    and call
    setup_port_for_reflector_program(unit, selection_port,selection_port2,reflector_port,reflector_port2,vlan_port).
    At this stage all trafic coming from port "selection_port" will be have the out-TM port changed to the reflector_port.
	The out-PP port will remain as it was originally.
3) 
    The reflector port should be defined as recycle port with the soc property
    ucode_port_[reflector port]=RCY
    The packet will enter the recycle interface with a PTCH with the injected port indication as the original Out-PP-Port
3) 
    Set up L2/L3 configurations on the out-PP port.
4) 
	Send traffic from port "selection_port" 
 */





/* Egress PMF redirection isn't supported in Jericho due to the double
   core. need to check the device type in order to use StatVport instead. */
int device_is_jer(int unit, uint8* is_jer)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  *is_jer = ((info.device == 0x8375) || (info.device == 0x8675));
  return rv;
}



/**
 * Function sets up an egress PMF rule changing the Out-TM-Port 
 * of all traffic coming from selection_port to reflector_port.
 * For JR (and above), restore out-pp-port using the bcmFieldActionStat
 * action.
 *  Runs in advanced vlan editing mode.
 * 
 * @author sinai (07/04/2014)
 * 
 * @param unit 
 * @param selection_port 
 * @param reflector_port 
 * 
 * @return int 
 */
int setup_port_for_reflector_program(int unit, int selection_port, int selection_port2, int reflector_port, int reflector_port2, int vlan_port) {
    int rv;
    char *proc_name ;
    bcm_vlan_port_t vp;
    int i, reflector_port_num = 1;

    proc_name = "setup_port_for_reflector_program" ;
    printf("%s(): Enter\r\n",proc_name) ;

    bcm_vlan_port_t_init(& vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = vlan_port;
    vp.match_vlan = 10;
    vp.egress_vlan = 10;
    vp.vsi = 100; 

    uint8 is_jer;

    rv = device_is_jer(unit, &is_jer);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (is_jer && (selection_port2 != -1) && (reflector_port2 != -1)) {
        reflector_port_num = 2;
    }

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    g_eve_edit_utils.edit_profile =0;



    rv = vlan__eve_default__set(unit, vp.vlan_port_id, 10,BCM_VLAN_NONE,bcmVlanActionReplace,bcmVlanActionNone);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    int counter_proc;
    int mod_id = 0;
    int port;
    int pri = 3;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    
    uint64 value;
    bcm_gport_t destination_gport, selection_gport, forwarding_gport;
    unsigned long destination_gport_as_long;
    int outlif ;

/* qset */
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageEgress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcPort);

    BCM_FIELD_ASET_INIT(aset);
    if (!is_jer) {
        /*
         * Another Traffic-Management OutPort is set to so that the packet will be
         * selected by the reflector port and sent to the recycling interface.
         * However, the same Packet processing is  performed on the packet,
         * according to the original Out-PP-Port.
         * The  bcmFieldActionRedirect Field action modifies only the
         * Traffic-Management OutPort. It is in general coupled at egress with
         * bcmFieldActionStat to modify also the Out-PP-Port.
         */
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    } else {
        /*
         * In Jericho, use bcmFieldActionRedirect
         * with the core-specific API bcm_field_action_config_add().
         * It does change the out-TM-port to the desired new value.
         * But after passing from ERPP to ETPP, the out-PP-port also gets updated.
         * To restore the old value only for out-PP-port before it reaches PRGE, 
         * we use also bcmFieldActionStat.
         */
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionStat);
    }

/* create database */
    rv =  bcm_field_group_create(unit, qset, pri, &group);
    if (rv != BCM_E_NONE) {
        printf("field group create: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_group_action_set(unit, group, aset);  
    if (rv != BCM_E_NONE) {
        printf("field group action set: (%s) \n",bcm_errmsg(rv));
        return rv;
    }

/* adding entry in database */
    rv =  bcm_field_entry_create(unit, group, &entry);
    if (rv != BCM_E_NONE) {
        printf("field entry create \n",bcm_errmsg(rv));
        return rv;
    }

    for ( i = 0; i < reflector_port_num; i++ ) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(selection_gport, (i == 0) ? selection_port : selection_port2);
        rv = bcm_field_qualify_SrcPort(unit, entry, 0, 0, selection_gport, 0xffffffff);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_qualify_SrcPort\n");
          return result;
        }

        
        BCM_GPORT_SYSTEM_PORT_ID_SET(destination_gport, (i == 0) ? reflector_port : reflector_port2);
        destination_gport_as_long = destination_gport ;
        /* forwarding port and reflector port are the same port..
        * This is for simplicity (and aligned with regression), for real application, 
        * forwarding port should be set according to the real forwarding destination of the packet.
        * E.g., add a qualifier of bcmFieldQualifyOutPort and for each out pp port, 
        * set forwarding_gport(thus the out pp port) to be equal to the original one:
        *     if bcmFieldQualifyOutPort == 13, forwarding_gport = 13
        *     if bcmFieldQualifyOutPort == 14, forwarding_gport = 14,
        *     ....
        */
        forwarding_gport = destination_gport;
        if (!is_jer) {
            /*
             * Enter for ARAD PLUS and below
             */
            printf("%s(): ARAD: Call bcm_field_action_add(): bcmFieldActionRedirect, module %d destination_gport %d\n",proc_name,0,destination_gport) ;

            rv = bcm_field_action_add(unit, entry, bcmFieldActionRedirect, mod_id, destination_gport);
            if (rv != BCM_E_NONE) {
                printf("%s(): field action add: %s\n",proc_name,bcm_errmsg(rv));
                return rv;
            }
        }
        else {
            /*
             * Enter for JERICHO and up
             */
            int cores_num = 2;
            int core_id;
            bcm_field_action_core_config_t core_config_arr[cores_num] ;
            int old_outpp_port[cores_num];
            bcm_gport_t old_outpp_gport[cores_num];
            uint32 signal_value;
            int statId = 1000 ; /* Set internal data */
            unsigned long statId_as_long;

            /* Redirect to reflector out_TM-port */
            printf("%s(): JERICHO: Call bcm_field_action_config_add(): bcmFieldActionRedirect\n",proc_name) ;
            printf("==> entry %d statId %d (0x%08lX) destination_gport %d (0x%08lX)\n",entry,statId,statId_as_long,destination_gport,destination_gport_as_long) ;
            mod_id = BCM_GPORT_EGRESS_MODPORT_MODID_GET(destination_gport);
            for (core_id = 0 ; core_id < cores_num ; core_id++) {
              /* Only the one with core ID equals to the core id of reflector port matters */
              core_config_arr[core_id].param0 = mod_id ;
              core_config_arr[core_id].param1 = destination_gport ;
              core_config_arr[core_id].param2 = BCM_ILLEGAL_ACTION_PARAMETER ;
            }
            rv = bcm_field_action_config_add(unit, entry, bcmFieldActionRedirect, cores_num, &core_config_arr[0]) ;
            if (rv != BCM_E_NONE) {
                printf("%s(): bcm_field_action_config_add() bcmFieldActionRedirect: %s\n",proc_name,bcm_errmsg(rv));
                return rv;
            }

            /* Restore the out-PP-port value, using Stat action */
            for (core_id = 0 ; core_id < cores_num ; core_id++) {
                core_config_arr[core_id].param0 = statId ;
                core_config_arr[core_id].param1 = forwarding_gport;
                core_config_arr[core_id].param2 = BCM_ILLEGAL_ACTION_PARAMETER ;
            }
            rv = bcm_field_action_config_add(unit, entry, bcmFieldActionStat, cores_num, &core_config_arr[0]) ;
            if (rv != BCM_E_NONE) {
                printf("%s(): bcm_field_action_config_add() bcmFieldActionStat: %s\n",proc_name,bcm_errmsg(rv));
                return rv;
            }
        }
    }

    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("field group install: \n",bcm_errmsg(rv));
        return rv;
    }
    printf("%s(): Exit OK\r\n",proc_name) ;
    return 0;
}

/* variables for VLAN translation set-up */
int vlan_translation_in_tags;
int vlan_translation_out_tags;
int vlan_translation_type;

/* Configuring Egress VLAN Translation */
/** vlan_translation_type = 1(BCM_VLAN_ACTION_SET_EGRESS)
*   vlan_translation_type = 0(BCM_VLAN_ACTION_SET_INGRESS)
*/
int ive_eve_translation_set(int unit,
                        bcm_gport_t lif,
                        int outer_tpid,
                        int inner_tpid,
                        bcm_vlan_action_t outer_action,
                        bcm_vlan_action_t inner_action,
                        bcm_vlan_t new_outer_vid,
                        bcm_vlan_t new_inner_vid,
                        uint32 vlan_edit_profile,
                        uint16 tag_format)
{
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
    int action_id_1, rv;

    /* set edit profile for  ingress/egress LIF */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = new_outer_vid;
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = lif;
    port_trans.vlan_edit_class_id = vlan_edit_profile;
    port_trans.flags = vlan_translation_type ? BCM_VLAN_ACTION_SET_EGRESS : BCM_VLAN_ACTION_SET_INGRESS ;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Create action ID*/
    rv = bcm_vlan_translate_action_id_create( unit, vlan_translation_type ? BCM_VLAN_ACTION_SET_EGRESS : BCM_VLAN_ACTION_SET_INGRESS, &action_id_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /* Set translation action */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = outer_tpid;
    action.inner_tpid = inner_tpid;	
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    rv = bcm_vlan_translate_action_id_set( unit,
                                            vlan_translation_type ? BCM_VLAN_ACTION_SET_EGRESS : BCM_VLAN_ACTION_SET_INGRESS,
                                            action_id_1,
                                            &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    /* Set translation action class (map edit_profile & tag_format to action_id) */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = vlan_edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id	= action_id_1;
    action_class.flags = vlan_translation_type ? BCM_VLAN_ACTION_SET_EGRESS : BCM_VLAN_ACTION_SET_INGRESS;
    rv = bcm_vlan_translate_action_class_set( unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }
    

    return BCM_E_NONE;
}


/* Sets default egress TPID class for default egress vlan translation in IVE mode*/
int egress_only_tpid_set(int unit, bcm_port_tpid_class_t tpid_class, int port) {

    int rv;   
    bcm_port_tpid_class_t_init(&tpid_class);

    tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    tpid_class.port = port;
    tpid_class.tag_format_class_id = 9; /* some unused tag format*/
    tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port - %d, rv - %d\n", port, rv);
        return rv;
    }

    return rv;
}

int setup_ports_for_vlan_translation(int unit, int selection_port, int reflector_port) {
    int rv;
    char *proc_name ;
    bcm_vlan_port_t vp;
    bcm_vlan_t outer_vlan = 150;
    bcm_vlan_t inner_vlan = 50;
    bcm_port_tpid_class_t tpid_class;

    proc_name = "setup_port_for_reflector_program" ;
    printf("%s(): Enter\r\n",proc_name) ;

    bcm_vlan_port_t_init(&vp);
    vp.port = selection_port;
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    if(vlan_translation_in_tags >= 1) {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN; 
        vp.match_vlan = 15;
    }
    if(vlan_translation_in_tags == 2) {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vp.match_inner_vlan = 32;
    }
    vp.vsi = 100;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* Configure a mac address for the port. */
    l2__mact_properties_s mact_entry0 = {vp.vlan_port_id,
                                        {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},
                                        vp.vsi
    };
    rv = l2__mact_entry_create(unit, &mact_entry0);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create0, rv - %d\n", rv);
        return rv;
    }

    /* vlan_translation_type = 1 (egress vlan translation) */
    /* vlan_translation_type = 0 (ingress vlan translation) */
    if(vlan_translation_type) {
        if(vlan_translation_in_tags == 2) {
            /* Configure the TPIDs for the port and their classification */
            rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 6 /*tag_format*/, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            if (vlan_translation_out_tags == 1) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionDelete,  outer_vlan,  BCM_VLAN_NONE, 11 /*edit_profile*/, 6 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }

            } else if(vlan_translation_out_tags == 2) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionReplace,  outer_vlan,  inner_vlan, 12 /*edit_profile*/, 6 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 0) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionDelete,  bcmVlanActionDelete,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 13 /*edit_profile*/, 6 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        } else if(vlan_translation_in_tags == 1) {
            /* Configure the TPIDs for the port and their classification */
            rv = port__tpids__set(unit, vp.port, 0x8100,0);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }

            rv = port__tag_classification__set(unit, vp.port, 7 /*tag_format*/, 0x8100, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            if(vlan_translation_out_tags == 1) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id, 0x8100, 0x9100, bcmVlanActionReplace, bcmVlanActionNone, outer_vlan,  BCM_VLAN_NONE, 14 /*edit_profile*/, 7 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 2) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id, 0x8100, 0x9100, bcmVlanActionReplace, bcmVlanActionAdd, outer_vlan,  inner_vlan, 15 /*edit_profile*/, 7 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }	
            } else if(vlan_translation_out_tags == 0) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id, 0x8100, 0x9100, bcmVlanActionDelete, bcmVlanActionNone, BCM_VLAN_NONE,  BCM_VLAN_NONE, 16 /*edit_profile*/, 7 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        } else if(vlan_translation_in_tags == 0) {
            if(vlan_translation_out_tags == 1) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id, 0x8100, 0x9100, bcmVlanActionAdd, bcmVlanActionNone, outer_vlan,  BCM_VLAN_NONE, 18 /*edit_profile*/, 0 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }				
            } else if(vlan_translation_out_tags == 2) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id, 0x8100, 0x9100, bcmVlanActionAdd, bcmVlanActionAdd, outer_vlan,  inner_vlan, 19 /*edit_profile*/, 0 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 0) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id, 0x8100, 0x9100, bcmVlanActionNone, bcmVlanActionNone, BCM_VLAN_NONE,  BCM_VLAN_NONE, 20 /*edit_profile*/, 0 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    } else {
        rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
        if (rv != BCM_E_NONE) {
            printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
            return rv;
        } 
       
        rv = egress_only_tpid_set(unit, tpid_class, selection_port);
        /* defining default EGRESS VLAN translation */
        vlan_translation_type = 1;

            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0,  0,  bcmVlanActionNone,  bcmVlanActionNone,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 11 /*edit_profile*/, vlan_translation_out_tags ? 9 : 0/*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            } 

        /* returning the vlan_translation type to be 0(INGRESS) */
        vlan_translation_type = 0;
    
        if(vlan_translation_in_tags == 2) {
            /* Configure the TPIDs for the port and their classification */
            rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 6 /*tag_format*/, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }

            rv = egress_only_tpid_set(unit, tpid_class,selection_port);

            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionReplace,  10,  20, 3 /*edit_profile*/, 6/*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        } else if(vlan_translation_in_tags == 1) {
            rv = port__tpids__set(unit, vp.port, 0x8100, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 7 /*tag_format*/, 0x8100, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }

           rv = egress_only_tpid_set(unit, tpid_class,selection_port);
		   
            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0,  bcmVlanActionReplace,  bcmVlanActionNone,  10,  BCM_VLAN_NONE, 1 /*edit_profile*/, 7/*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            } 			
        } else if(vlan_translation_in_tags == 0) {
            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionNone,  bcmVlanActionNone,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 5 /*edit_profile*/, 0 /*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        }
    }
    bcm_vlan_port_t_init(& vp);
    vp.port = reflector_port;
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    if(vlan_translation_in_tags >= 1) {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.match_vlan = 10;
    }
    if(vlan_translation_in_tags == 2) {
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vp.match_inner_vlan = 20;
    }
    vp.vsi = 100;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Configure a mac address for the port. */
    l2__mact_properties_s mact_entry1 = {vp.vlan_port_id,
                                        {0x00, 0x00, 0x07, 0x00, 0x01, 0x00},
                                        vp.vsi
    };

    rv = l2__mact_entry_create(unit, &mact_entry1);
    if (rv != BCM_E_NONE) {
        printf("Error in l2__mact_entry_create1, rv - %d\n", rv);
        return rv;
    }
    if(vlan_translation_type) {
        if(vlan_translation_in_tags == 2) {
            rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 6 /*tag_format*/, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionReplace,  10,  20, 10 /*edit_profile*/, 6/*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        } else if(vlan_translation_in_tags == 1) {
            rv = port__tpids__set(unit, vp.port, 0x8100, 0);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 7 /*tag_format*/, 0x8100, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }

            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0,  bcmVlanActionReplace, bcmVlanActionNone ,  10,  BCM_VLAN_NONE, 17 /*edit_profile*/, 7 /*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        } else if(vlan_translation_in_tags == 0) {
            rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0,  0,  bcmVlanActionNone, bcmVlanActionNone ,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 21 /*edit_profile*/, 0 /*tag_format*/);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        }
    } else {    

       
        if(vlan_translation_in_tags == 2) {
            rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 6 /*tag_format*/, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
			
			rv = egress_only_tpid_set(unit, tpid_class,reflector_port);

            if(vlan_translation_out_tags == 2) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionReplace,  outer_vlan,  inner_vlan, 4 /*edit_profile*/, 6 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 1) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionDelete,  outer_vlan,  BCM_VLAN_NONE, 2 /*edit_profile*/, 6 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 0) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionDelete,  bcmVlanActionDelete	,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 5 /*edit_profile*/,6 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        } else if(vlan_translation_in_tags == 1) {
            rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 7 /*tag_format*/, 0x8100, 0xFFFFFFFF);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
			
            rv = egress_only_tpid_set(unit, tpid_class,reflector_port);
			
            if(vlan_translation_out_tags == 1) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0,  bcmVlanActionReplace,  bcmVlanActionNone,  outer_vlan,  BCM_VLAN_NONE, 7 /*edit_profile*/, 7 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 2) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionReplace,  bcmVlanActionAdd,  outer_vlan,  inner_vlan, 2 /*edit_profile*/, 7 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 0) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0,  bcmVlanActionDelete,  bcmVlanActionNone,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 2 /*edit_profile*/, 7 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        } else if (vlan_translation_in_tags == 0) {
            rv = port__tpids__set(unit, vp.port, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tpids__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }
            rv = port__tag_classification__set(unit, vp.port, 7 /*tag_format*/, 0x8100, 0x9100);
            if (rv != BCM_E_NONE) {
                printf("Error, port__tag_classification__set for port - %d, rv - %d\n", vp.port, rv);
                return rv;
            }

            if(vlan_translation_out_tags == 1) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0,  bcmVlanActionAdd,  bcmVlanActionNone,  outer_vlan,  BCM_VLAN_NONE, 1 /*edit_profile*/, 0 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 2) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0x9100,  bcmVlanActionAdd,  bcmVlanActionAdd,  outer_vlan,  inner_vlan, 2 /*edit_profile*/, 0 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            } else if(vlan_translation_out_tags == 0) {
                rv = ive_eve_translation_set( unit,  vp.vlan_port_id,  0x8100,  0,  bcmVlanActionNone,  bcmVlanActionNone,  BCM_VLAN_NONE,  BCM_VLAN_NONE, 3 /*edit_profile*/, 0 /*tag_format*/);
                if (rv != BCM_E_NONE) {
                    printf("(%s) \n", bcm_errmsg(rv));
                    return rv;
                }
            }
        }
    }
    printf("%s(): Exit OK\r\n",proc_name) ;
    return 0;
}


