/*~~~~~~~~~~~~~~~~~~~~~~~PWE native eve on QAX~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_pw_raw_tag_mode.c
 * Purpose: Example of Native EVE of  QAX feature. 
 *                In this example, this feature is used to support raw/tag mode of PWE. 
 *
 * Attachment circuit (AC): Ethernet port attached to the service based on port-vlan-vlan, 
 *                          connect from access side. 
 * PWE: Virtual circuit attached to the service based on VC-label. Connect to the MPLS 
 *      network side.
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
 * |                                             . . .       . . .                                       |
 * |                                           .       .   .       .                                     |
 * |                                       . .    +---------+     .   . .                                |
 * |                                     .        |         |  /\   .     .                              |
 * |                                   .          |   PE3   |  \\          .                             |
 * |                                   .       /\ | Switch  |/\ \\         .                             |
 * |                                    .     || +---------+\\  \\       .                              |
 * |                                   .      ||              \\  +------+ .                             |
 * |                                  . +------+               \\ |TUNNEL| .                             |
 * |                                 .  |TUNNEL|                \\+------+.                              |
 * |                                 .  +------+                 \\  \\    .          . . .              |
 * |                  . .            .    ||                      \\  \\    .      . .      .     .  .   |
 * |             .  .     . .       .    /\/    +------------+    \/\ \/\   .     . +----+    . .      . | 
 * |    . .   .   +----+     .  +---------+ <---| - - - - - -|---- +---------+   .  |UNIc|             . |
 * |  .     .     |UNIa|--------|         | ----| - - - - - -|---> |         |----- +----+  +----+   .   |           
 * |  .  +----+   +----+     .  |   PE1   |     |    MPLS    |     |   PE2   |--------------|UNId|     . |
 * |   . |UNIb|-----------------| Switch  | <---| - TUNNEL- -|---- | Switch  |--- +----+    +----+     . |
 * | .   +----+              .  +---------+ ----| - - - - - -|---> +---------+  . |UNIe|             .   |
 * |.            ETHERNET     .      .     . .  +------------+              .  .  +----+   ETHERNET .    |
 * | .     .       .     . . .         . .     .       .       .    .      .    .      .       .     .   |
 * |   . .   . . .   . .                         . . .   . . .   . .  . . .      . . .   . . .   . .     |
 * |                                                                                                     |
 * |   +--------------------+      +----------------------------------------+                            |
 * |   | Ethr | ctag | Data |      | Ethr | MPLS | PWE | Ethr | ctag | Data |                            |
 * |   +--------------------+      +----------------------------------------+                            | 
 * |                                                                                                     |
 * |                                +-----------------------------------------------------------+        |
 * |           PWE Tag Mode:        | Ethr | MPLS | PWE | Ethr | stag | ctag | payload          |        |
 * |                                +-----------------------------------------------------------+        |
 * |                                +-----------------------------------------------------------+        |
 * |           PWE Raw Mode:        | Ethr | MPLS | PWE | Ethr |  payload                       |        |
 * |                                +-----------------------------------------------------------+        |
 * |                      +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                              |
 * |                      | Figure 13: Multipoing VPLS Service Attachment |                              |
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 *
 * Explanation:
 *  -   Access-P4: Access port, defined on port 1 with VID 20.
 *  -   Access-P3: Access port, defined on port 1 untagged..
 *  -   Access-P2: Access port, defined on port 1 with outer-VID 15 and inner-VID 30.
 *  -   Access-P1: Access port, defined on port 1 with outer-VID 10 and inner-VID 20.
 *  -   PWE-1: network port with incoming VC = 2010, egress-VC = 1982 defined over two tunnels 1000 and 1002.
 *  -   For access ports outer-Tag TPID is 0x8100 and inner Tag TPID is 0x9100.
 *  -   P2 refers to the physical ports Access-P1 and Access-P2 are attached to.
 *  -   PWE1 and PWE2 refers to MPLS network ports.
 *  -   P1 refers to the physical ports PWE1 and PWE2 are attached to.
 *
 * Headers:
 *  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8100|    |100||Label:1000|Label:1002||Lable:2010||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1: Packets Received from PWE1 at raw mode                | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||native-vid1 ||native vid2 || data
 *   |  |0:11|  ||0x8100|    |100||Label:1000|Label:1002||Lable:2010||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 2: Packets Received from PWE1 at tag mode                | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 

 * 
 * Access side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 10  || 0x9100|      | 20  ||            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 3: Packets Received/Transmitted on Access Port 1 |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 15  || 0x9100|      | 30  ||            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 4: Packets Received/Transmitted on Access Port 2 |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                                      |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 20  |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 5: Packets Received/Transmitted on Access Port 3 |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

 *
 * Calling sequence:
 *  -  legacy PWE and AC of vpls creating
 *  -  create native eve ac for native eve with BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_NATIVE flag
 *  -  create the vlan translation class with adt legacy process.
 *  -  call the api of bcm_port_match_add() with below parameter:
 *                     port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY;  indicate that it's for native VE 
 *                      port_match_info.match = BCM_PORT_MATCH_PORT_VPN;    indicate that lookup fields are port and VPN 
 *                      port_match_info.vpn = vsi; 
 *                      port_match_info.port = PWE gport id; 
 * 
 * To Activate Above Settings Run:
 *  	BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *  	BCM> cint examples/dpp/utility/cint_utils_vlan.c
 *  	BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
 *  	BCM> cint examples/dpp/utility/cint_utils_mpls_port.c
 *      BCM> cint examples/dpp/utility/cint_utils_multicast.c
 *      BCM> cint examples/dpp/utility/cint_utils_l2.c
 *      BCM> cint examples/dpp/utility/cint_utils_l3.c
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_qos.c
 *      BCM> cint examples/dpp/cint_mpls_lsr.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls.c
 *      BCM> cint examples/dpp/cint_pw_raw_tag_mode.c
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = pw_raw_tag_mode_init_with_default(unit, acP, pwP); 
*/
enum raw_tag_mode_s{
    raw_mode_1,
    tag_mode_1,
    mode_num
};

enum native_eve_action_id_s{
    native_eve_action_nop=0x20,/*Bit 5 identify the native eve action*/
    native_eve_action_untag_add_o_tag,
    native_eve_action_untag_add_d_tag,
    native_eve_action_i_tag_add_o_tag,
    native_eve_action_i_tag_remove_i_tag,
    native_eve_action_o_tag_remove_o_tag,
    native_eve_action_double_tag_remove_o_tag,
    native_eve_action_double_tag_remove_d_tag,
    native_eve_action_num
};

enum native_eve_profile_id_s{
    native_eve_profile_pw_raw_mode = 0x10,/*do not change any thing*/
    native_eve_profile_pw_tag_mode,              /*add some tag according configuration*/
    native_eve_profile_id_num
};

struct native_eve_rule_s{
    int mode;
    int natvie_eve_profile;
    int tag_format;
    int action_id;
};


struct native_eve_action_s{
    int action_id;
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    uint16       outer_tpid;
    uint16       inner_tpid;
};

native_eve_action_s raw_tag_mode_native_eve_action[] = {

      { native_eve_action_nop,                                                       bcmVlanActionNone,                                    bcmVlanActionNone,     0x8100, 0x9100 },
      { native_eve_action_untag_add_o_tag,                                 bcmVlanActionAdd,                                      bcmVlanActionNone,        0x8100, 0x9100 },
      { native_eve_action_untag_add_d_tag,                                 bcmVlanActionAdd,                                      bcmVlanActionAdd,           0x8100, 0x9100 },
      { native_eve_action_i_tag_add_o_tag,                                   bcmVlanActionAdd,                                      bcmVlanActionNone,     0x8100, 0x9100 },
      { native_eve_action_i_tag_remove_i_tag,                              bcmVlanActionDelete,                                  bcmVlanActionNone,     0x8100, 0x9100},
      { native_eve_action_o_tag_remove_o_tag,                            bcmVlanActionDelete,                                  bcmVlanActionNone,     0x8100, 0x9100},
      { native_eve_action_double_tag_remove_o_tag,                   bcmVlanActionDelete,                                  bcmVlanActionNone,     0x8100, 0x9100},
      { native_eve_action_double_tag_remove_d_tag,                   bcmVlanActionDelete,                                  bcmVlanActionDelete,     0x8100, 0x9100}
};

/*
* Here we define 2 kinds of native eve profile
* raw mode profile: it will do nothing for the native vlan;
* tag mode profile: it will align the tag num to 2 for native vlan:
*                             untag------>add S+C
*                             single tag--->Add S
*                             double tag--->NOP
* it's obvously that customer can define the new profile if they want
*/
native_eve_rule_s native_eve_rule[] = {

           /*raw mode 1: do nothing for any tag format packets*/
           {raw_mode_1,          native_eve_profile_pw_raw_mode,    0,                              native_eve_action_nop},
           {raw_mode_1,          native_eve_profile_pw_raw_mode,    1,  /*c*/                   native_eve_action_i_tag_remove_i_tag},
           {raw_mode_1,          native_eve_profile_pw_raw_mode,    2,  /*S*/                   native_eve_action_o_tag_remove_o_tag},
           {raw_mode_1,          native_eve_profile_pw_raw_mode,    6,  /*s+c*/               native_eve_action_double_tag_remove_d_tag},

           /*tag mode 1:  align the payload tag to 2 tags*/
           {tag_mode_1,          native_eve_profile_pw_tag_mode,    0,                              native_eve_action_untag_add_d_tag},
           {tag_mode_1,          native_eve_profile_pw_tag_mode,    1,                              native_eve_action_i_tag_add_o_tag},
           {tag_mode_1,          native_eve_profile_pw_tag_mode,    2,                              native_eve_action_i_tag_add_o_tag},
           {tag_mode_1,          native_eve_profile_pw_tag_mode,    6,                              native_eve_action_nop}

            /*other mode if you need*/
};

int mod2profile[] = {native_eve_profile_pw_raw_mode,native_eve_profile_pw_tag_mode};

struct native_eve_config_info_t{
    bcm_gport_t ac[2];

    int native_outer_vlan;
    int native_inner_vlan;

    bcm_gport_t native_ac_id;

};

native_eve_config_info_t neve_cfg_info;

void native_eve_cfg_info_init()
{
    neve_cfg_info.native_inner_vlan = 2000;
    neve_cfg_info.native_outer_vlan = 1000;

}


/*
* this function try to add 2 aceess port into the vpn 6202
* AC1 : port+cvid
* AC2 : port
* the ingress vlant translation operation for them is nop
* it set the vlan translation of AC0( port + s +c) as nop 
* as well. 
*/
int
pw_raw_tag_mode_add_access_port(int unit)
{
    int rv;  
    bcm_vlan_port_t vlan_port;
    int port;
    int i = 0;
    
    port = vswitch_vpls_info_1.ac_in_port;


     for(i = 0 ;i<2;i++){

        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = vswitch_vpls_info_1.ac_in_port;

        switch(i) {
            case 0:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan = vswitch_vpls_info_1.ac_port1_inner_vlan;
            vlan_port.egress_vlan = vswitch_vpls_info_1.ac_port1_inner_vlan;

            break;

            case 1:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
            break;
        }

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }

         neve_cfg_info.ac[i]= vlan_port.vlan_port_id;

        if(verbose1){
            printf("vlan_port_encap_id1: 0x%08x\n\r",vlan_port.encap_id);
        }



        /* Configre ingress editing */
        rv = vlan_port_translation_set(unit, 0, 0, vlan_port.vlan_port_id, INGRESS_NOP_PROFILE, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set, ingress profile\n");
            return rv;
        }

        /* Configure egress editing*/
        rv = vlan_port_translation_set(unit, 0, 0, vlan_port.vlan_port_id, EGRESS_NOP_PROFILE, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set, egress profile\n");
            return rv;
        }



        rv = bcm_vswitch_port_add(unit, vswitch_vpls_shared_info_1.vpn, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
        
        /* update Multicast to have the added port  */

/*        rv = multicast__vlan_port_add(unit, vswitch_vpls_info_1.vpn_id, port , vlan_port.vlan_port_id, egress_mc);*/
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__vlan_port_add\n");
            return rv;
        }

        if(verbose1){
            printf("add port   0x%08x to multicast \n\r",vlan_port.vlan_port_id);
        }
     }    
    
    /*For the port+s+c AC ,configure the vlan translation to nop*/
    /* Configre ingress editing */
    rv = vlan_port_translation_set(unit, 0, 0, vswitch_vpls_info_1.access_port_id, INGRESS_NOP_PROFILE, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set, ingress profile\n");
        return rv;
    }

    /* Configure egress editing*/
    rv = vlan_port_translation_set(unit, 0, 0, vswitch_vpls_info_1.access_port_id, EGRESS_NOP_PROFILE, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_translation_set, egress profile\n");
        return rv;
    }


    return rv;

}

int pw_raw_tag_mode_set(int unit,uint32 mode)
{
     int rv = 0;

     if(mode >= mode_num) {
         printf("\n Invalid mode value");
         return -1;
     }

    rv = vlan__port_translation__set(unit, 
                                     neve_cfg_info.native_outer_vlan /* outer vlan */,
                                     neve_cfg_info.native_inner_vlan /* inner vlan */, 
                                     neve_cfg_info.native_ac_id /* ac-lif */, 
                                     mod2profile[mode], 
                                     0 /* is ingress */); 
    if (rv != BCM_E_NONE) {
        printf("Error, vlan__port_translation__set\n");
        return rv;
    }     
     return rv;

}

int pw_native_port_match_clear(int unit)
{
    int rv = 0;
    bcm_port_match_info_t port_match_info; 

    /*bind eve ac to PWE port*/
    bcm_port_match_info_t_init(&port_match_info); 

    port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY; /* indicate that it's for native VE */
    port_match_info.match = BCM_PORT_MATCH_PORT_VPN; /* indicate that lookup fields are port and VPN */
    port_match_info.vpn = vswitch_vpls_info_1.vpn_id; 
    port_match_info.port = network_port_id; 

    rv = bcm_port_match_delete(unit, neve_cfg_info.native_ac_id, &port_match_info); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_match_add \n "); 
    }

   return rv;
}

int pw_raw_tag_mode_init_with_default(int unit, int acP, int pweP)
{
    int rv = 0;
    int i,pw_mode,eve_rule_id;
    uint32 flags;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_t vlan_port; 
    bcm_port_match_info_t port_match_info; 

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    native_eve_cfg_info_init();

    vswitch_vlps_info_init(0,acP,pweP,pweP,10,20,15,30,default_vpn_id);

    /*IVE do nothing when raw/tag mode enabled*/
    vswitch_vpls_run(unit, -1, 0);

    /*create ac ports for test*/
    rv = pw_raw_tag_mode_add_access_port(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in pw_raw_tag_mode_add_access_port\n");
        return rv;
    }

   /*initiate the eve action*/
    flags = BCM_VLAN_ACTION_SET_WITH_ID | BCM_VLAN_ACTION_SET_EGRESS;
    for(i = 0;i <sizeof(raw_tag_mode_native_eve_action)/sizeof(raw_tag_mode_native_eve_action[0]);i++ ) {
         /* Create an Action ID */
         int act_id = raw_tag_mode_native_eve_action[i].action_id;
        rv = bcm_vlan_translate_action_id_create(unit, flags, &act_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create failed, rv - %d\n", rv);
            return rv;
         }     
      }

      for(pw_mode = raw_mode_1; pw_mode < mode_num; pw_mode++) {
             for(eve_rule_id = 0; eve_rule_id < sizeof(native_eve_rule)/sizeof(native_eve_rule[0]); eve_rule_id++){
                   if (native_eve_rule[eve_rule_id].mode == pw_mode) {
			    bcm_vlan_translate_action_class_t action_class; 

			    bcm_vlan_translate_action_class_t_init(&action_class); 
			    action_class.vlan_edit_class_id = native_eve_rule[eve_rule_id].natvie_eve_profile; /* vlan edit profile */
			    action_class.vlan_translation_action_id = native_eve_rule[eve_rule_id].action_id; /* vlan edit command */
			    action_class.tag_format_class_id= native_eve_rule[eve_rule_id].tag_format; /* vlan edit command */
			    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS; 
			    rv = bcm_vlan_translate_action_class_set(unit, action_class); 
			    if (rv != BCM_E_NONE) {
			        printf("Error, bcm_vlan_translate_action_class_set\n");
			        return rv;
			    }
                            
                   }
           }
     }

    for(i = 0;i <sizeof(raw_tag_mode_native_eve_action)/sizeof(raw_tag_mode_native_eve_action[0]);i++ ) {
         int act_id = raw_tag_mode_native_eve_action[i].action_id;

	bcm_vlan_action_set_t_init(&action);
	action.dt_outer = raw_tag_mode_native_eve_action[i].outer_action;
	action.dt_inner = raw_tag_mode_native_eve_action[i].inner_action;
	action.outer_tpid = raw_tag_mode_native_eve_action[i].outer_tpid;
	action.inner_tpid = raw_tag_mode_native_eve_action[i].inner_tpid;
        rv = bcm_vlan_translate_action_id_set(unit, flags, act_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set for action_id - %d, rv - %d\n", act_id, rv);
            return rv;
        }
      }


    /*create the native eve ac*/
    bcm_vlan_port_t_init(&vlan_port); 
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_NATIVE; 
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;  
    rv = bcm_vlan_port_create(unit, &vlan_port); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    neve_cfg_info.native_ac_id = vlan_port.vlan_port_id;

    /*bind eve ac to PWE port*/
    bcm_port_match_info_t_init(&port_match_info); 

    port_match_info.flags = BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY; /* indicate that it's for native VE */
    port_match_info.match = BCM_PORT_MATCH_PORT_VPN; /* indicate that lookup fields are port and VPN */
    port_match_info.vpn = vswitch_vpls_info_1.vpn_id; 
    port_match_info.port = network_port_id; 

    rv = bcm_port_match_add(unit, neve_cfg_info.native_ac_id, &port_match_info); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_match_add \n "); 
    }

    printf("entry in ESEM: local outlif (global outlif is: 0x%x) x VSI (%d) -> native AC-LIF (0x%x) \n", network_port_id, vswitch_vpls_info_1.vpn_id, neve_cfg_info.native_ac_id); 



   rv = pw_raw_tag_mode_set(unit,0);
    if (rv != BCM_E_NONE) {
        printf("Error, pw_raw_tag_mode_set \n "); 
    }
   
   return rv;

}




