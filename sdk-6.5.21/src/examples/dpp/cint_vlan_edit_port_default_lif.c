/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
* cint_vlan_edit_port_default_lif.c
*
* Set following setup:
* 
*    case 1: basic case,set vlan action for default lif
*     -   call vlan_edit_advanced_default_action(), 
*     -   Send single tag Ethernet traffic  to port_1:
*        -   VLAN tag: (0x8100,10)
*     -   Packet is transmitted to port port_2
*           -   Check the packets tag(0x8100,20) received on port_2 . If not, failed.
*          
*    case 1-1: In that  case,if you set another port default action, it will override the default action that you set before. 
*               Becasue all of ports are using the same default lif 
*     -   call overide_default_action_by_other_port(), 
*     -   Send single tag Ethernet traffic  to port_1:
*        -   VLAN tag: (0x8100,10)
*     -   Packet is transmitted to port port_2
*           -   Check the packets tag(0x8100,30 && 0x8100,10) received on port_2 . If not, failed.
*
*
*    case 2: different port can have different default action when they have different lif.
*      -   Send single tag Ethernet traffic  to port_1:
*          -   VLAN tag:  (0x8100,100)
*      -   Packet is transmitted to port port_2
*          -    Check the packets tag(0x8100,200 && 0x8100,100) received on port_2. If not, failed.
*
*      -   Send single tag Ethernet traffic  to port_2:
*          -   VLAN tag:  (0x8100,100)
*      -   Packet is transmitted to port_1
*          -    Check the packets tag(0x8100,300 && 0x8100,100) received on port_1. If not, failed.
 */
bcm_gport_t gport1;
bcm_gport_t gport2;
int is_advanced_vlan_translation_mode = 0;
int action_id_1 = 20;
int action_id_2 = 21;
int action_id_3 = 22;
bcm_gport_t vlan_port_1;

int vlan_edit_advanced_default_action(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan =10;
    bcm_pbmp_t pbmp,ubmp;

    is_advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    rv = bcm_switch_control_set(unit,bcmSwitchPortVlanTranslationAdvanced,1);
    if (rv != BCM_E_NONE){
        printf("Error, bcm_switch_control_set  rv %d\n", rv);
        return rv;
    }

    if (is_advanced_vlan_translation_mode) {
        int flags = BCM_VLAN_ACTION_SET_WITH_ID;

        /* set class for both ports */
        rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_class_set, port=%d, \n", port1);
            return rv;
        }

        rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_class_set, port=%d, \n", port2);
            return rv;
        }

        /* set TPIDs for port1 */
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            return rv;
        }

        /* set TPIDs for port2 */
        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            return rv;
        }
        /* create a vlan port: */
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port1;
        vp.match_vlan = vlan;
        vp.vsi = vlan;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        vlan_port_1 = vp.vlan_port_id;

        /* Create new Action IDs */
        rv = bcm_vlan_translate_action_id_create( unit, flags|BCM_VLAN_ACTION_SET_INGRESS, &action_id_1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set action */
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionNone;
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x8100;
        action.new_outer_vlan = 20;
        action.new_outer_vlan = 10;
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, action_id_1, &action);

        /* Set port translation */
        bcm_vlan_port_translation_t port_trans;
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.new_outer_vlan = 20;
        port_trans.new_inner_vlan = 10;
        port_trans.gport = vlan_port_1;
        port_trans.vlan_edit_class_id = 2;
        port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        /* Set translation action class */
        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = 2;
        action_class.tag_format_class_id = 2;
        action_class.vlan_translation_action_id = action_id_1;
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }

        rv = bcm_vlan_create(unit,vlan);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_create (vlan 10)rv %d\n", rv);
            return rv;
        }
        /* add static entry to MAC table */
        bcm_l2_addr_t l2addr;
        bcm_mac_t mac  = {0x00, 0x00, 0xdd, 0x11, 0x22, 0x33};
        bcm_l2_addr_t_init(&l2addr, mac, vlan);
        l2addr.port = port2;
        rv = bcm_l2_addr_add(unit, &l2addr);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }
    } else {
        rv = bcm_vlan_create(unit,vlan);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_create (vlan 100)rv %d\n", rv);
            return rv;
        }

        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_PORT_ADD(pbmp,port1);
        BCM_PBMP_PORT_ADD(pbmp,port2);

        rv = bcm_vlan_port_add(unit,vlan,pbmp,ubmp);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_add (add to vlan 100) rv %d\n", rv);
            return rv;
        }

        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        action.ot_outer = bcmVlanActionReplace;
        action.new_outer_vlan = 20;
        rv = bcm_vlan_port_default_action_set(unit,port1,&action);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
            return rv;
        }
    }
    return rv;
}

int overide_default_action_by_other_port(int unit, int port)
{
   int rv =BCM_E_NONE;
   bcm_vlan_action_set_t action;
   bcm_vlan_action_set_t_init(&action);

   if (is_advanced_vlan_translation_mode) {
       action.dt_outer = bcmVlanActionReplace;
       action.dt_inner = bcmVlanActionAdd;
       action.outer_tpid = 0x8100;
       action.inner_tpid = 0x8100;
       action.new_outer_vlan = 30;
       action.new_outer_vlan = 10;
       rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, action_id_1, &action);

       /* Set port translation */
       bcm_vlan_port_translation_t port_trans;
       bcm_vlan_port_translation_t_init(&port_trans);
       port_trans.new_outer_vlan = 30;
       port_trans.new_inner_vlan = 10;
       port_trans.gport = vlan_port_1;
       port_trans.vlan_edit_class_id = 2;
       port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
       rv = bcm_vlan_port_translation_set(unit, &port_trans);
       if (rv != BCM_E_NONE) {
           printf("Error, bcm_vlan_port_translation_set\n");
           return rv;
       }
   }
   else {
       action.ot_outer = bcmVlanActionAdd;
       action.new_outer_vlan = 30;
       rv = bcm_vlan_port_default_action_set(unit,port,&action);/*using port 13 to overide port_1 default action*/
       if (rv != BCM_E_NONE){
           printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
           return rv;
       }
   }
   return rv;
}

int vlan_edit_default_action_for_different_port(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan =100;
    bcm_vswitch_cross_connect_t gports;

    if (is_advanced_vlan_translation_mode) {
        int flags = BCM_VLAN_ACTION_SET_WITH_ID;
        bcm_vlan_action_set_t action;

        /* set class for both ports */
        rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_class_set, port=%d, \n", port1);
            return rv;
        }

        rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_class_set, port=%d, \n", port2);
            return rv;
        }

        /* set TPIDs for port1 */
        port_tpid_init(port1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            return rv;
        }

        /* set TPIDs for port2 */
        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_2\n");
            return rv;
        }
        /* create vlan port */
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port1;
        vp.match_vlan = vlan;
        vp.vsi = vlan;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        gport1 = vp.vlan_port_id;
        bcm_vlan_port_t_init(&vp);
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = port2;
        vp.match_vlan = vlan;
        vp.vsi = vlan;
        rv = bcm_vlan_port_create(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        gport2 = vp.vlan_port_id;

        /* Create new Action IDs */
        rv = bcm_vlan_translate_action_id_create( unit, flags|BCM_VLAN_ACTION_SET_INGRESS, &action_id_2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }
        rv = bcm_vlan_translate_action_id_create( unit, flags|BCM_VLAN_ACTION_SET_INGRESS, &action_id_3);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        /* Set action */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x8100;
        action.new_outer_vlan = 200;
        action.new_outer_vlan = 100;
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, action_id_2, &action);

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionReplace;
        action.dt_inner = bcmVlanActionAdd;
        action.outer_tpid = 0x8100;
        action.inner_tpid = 0x8100;
        action.new_outer_vlan = 300;
        action.new_outer_vlan = 100;
        rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_INGRESS, action_id_3, &action);

        /* Set port translation */
        bcm_vlan_port_translation_t port_trans;
        bcm_vlan_port_translation_t_init(&port_trans);
        port_trans.new_outer_vlan = 200;
        port_trans.new_inner_vlan = 100;
        port_trans.gport = gport1;
        port_trans.vlan_edit_class_id = 3;
        port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }
        port_trans.new_outer_vlan = 300;
        port_trans.new_inner_vlan = 100;
        port_trans.gport = gport2;
        port_trans.vlan_edit_class_id = 4;
        port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_port_translation_set(unit, &port_trans);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_translation_set\n");
            return rv;
        }

        /* Set translation action class for single tagged packets with TPID 0x8100 */
        bcm_vlan_translate_action_class_t action_class;
        bcm_vlan_translate_action_class_t_init(&action_class);
        action_class.vlan_edit_class_id = 3;
        action_class.tag_format_class_id = 2;
        action_class.vlan_translation_action_id = action_id_2;
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }

        action_class.vlan_edit_class_id = 4;
        action_class.tag_format_class_id = 2;
        action_class.vlan_translation_action_id = action_id_3;
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
        rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }

        /* Cross connect the 2 LIFs */
        bcm_vswitch_cross_connect_t_init(&gports);

        gports.port1 = gport1;
        gports.port2 = gport2;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }else {
        bcm_pbmp_t pbmp,ubmp;
        bcm_vlan_port_t vlan_port1;
        bcm_vlan_port_t vlan_port2;
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t action2;
        bcm_vlan_action_set_t action_e;
        bcm_vlan_action_set_t action_e2;

        rv = bcm_vlan_create(unit,vlan);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_create (vlan 100)rv %d\n", rv);
            return rv;
        }

        /* port1,2 add vlan 100 with tag mode*/
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp,port1);
        BCM_PBMP_PORT_ADD(pbmp,port2);

        rv = bcm_vlan_port_add(unit,vlan,pbmp,ubmp);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_add (add to vlan 100) rv %d\n", rv);
            return rv;
        }

        bcm_vlan_port_t_init(vlan_port1);
        vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port1.port =port1;
        bcm_vlan_port_create(unit,&vlan_port1);
        if (rv != BCM_E_NONE){
        printf("Error,bcm_vlan_port_create ( 1) ,rv %d\n", rv);
        return rv;
        }

        bcm_vlan_port_t_init(vlan_port2);
        vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port2.port =port2;
        bcm_vlan_port_create(unit,&vlan_port2);
        if (rv != BCM_E_NONE){
            printf("Error,bcm_vlan_port_create ( 2) ,rv %d\n", rv);
            return rv;
        }

        gport1 = vlan_port1.vlan_port_id;
        gport2 = vlan_port2.vlan_port_id;

        /* Cross connect the 2 LIFs */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = vlan_port1.vlan_port_id;
        gports.port2 = vlan_port2.vlan_port_id;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        action.ot_outer = bcmVlanActionAdd;
        action.new_outer_vlan = 200;
        rv = bcm_vlan_port_default_action_set(unit,port1,&action);
        /* rv = bcm_vlan_translate_action_create(unit,vlan_port1.vlan_port_id, bcmVlanTranslateKeyPortOuter,BCM_VLAN_INVALID, BCM_VLAN_NONE, &action);*/
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
            return rv;
        }

        bcm_vlan_action_set_t_init(&action_e);
        action_e.ot_outer = bcmVlanActionNone;
        rv = bcm_vlan_translate_egress_action_add(unit, vlan_port2.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action_e);
        if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
        return rv;
        }

        bcm_vlan_action_set_t_init(&action2);
        action2.ot_outer = bcmVlanActionAdd;
        action2.new_outer_vlan = 300;
        /* rv = bcm_vlan_translate_action_create(unit,vlan_port2.vlan_port_id, bcmVlanTranslateKeyPortOuter,BCM_VLAN_INVALID, BCM_VLAN_NONE, &action2);*/
        rv = bcm_vlan_port_default_action_set(unit,port2,&action2);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_set ( add new_outer_vlan 100) ,rv %d\n", rv);
            return rv;
        }

        bcm_vlan_action_set_t_init(&action_e2);
        action_e2.ot_outer = bcmVlanActionNone;
        rv = bcm_vlan_translate_egress_action_add(unit, vlan_port1.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action_e2);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

int  clean_vlan_edit_default_lif(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan1 =10;
    
    if (is_advanced_vlan_translation_mode) {
        rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_INGRESS, action_id_1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_translate_action_id_destroy ,rv %d\n", rv);
            return rv;
        }
        bcm_vlan_port_destroy(unit,vlan_port_1);
        rv = bcm_vlan_destroy(unit, vlan1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
            return rv;
        }
    }else {
        bcm_pbmp_t pbmp,ubmp;
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_PORT_ADD(pbmp,port1);
        BCM_PBMP_PORT_ADD(pbmp,port2);

        rv = bcm_vlan_port_default_action_delete(unit, port1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_delete ( port1) ,rv %d\n", rv);
            return rv;
        }

        rv = bcm_vlan_port_default_action_delete(unit, 13);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_delete ( 13) ,rv %d\n", rv);
            return rv;
        }

        rv = bcm_vlan_port_remove(unit,vlan1,pbmp);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
            return rv;
        }

        rv = bcm_vlan_destroy(unit, vlan1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
            return rv;
        }
    }

    
    return rv;
}

int  clean_vlan_edit_default_lif_for_different_port(int unit, int port_1, int port_2)
{
    int rv =BCM_E_NONE;
    int port1 = port_1;
    int port2 = port_2;
    bcm_vlan_t vlan1 =100;

    if (is_advanced_vlan_translation_mode) {
        rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_INGRESS, action_id_2);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_translate_action_id_destroy ,rv %d\n", rv);
            return rv;
        }
        rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_INGRESS, action_id_3);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_translate_action_id_destroy ,rv %d\n", rv);
            return rv;
        }
        bcm_vlan_port_destroy(unit,gport1);
        bcm_vlan_port_destroy(unit,gport2);
        rv = bcm_vlan_destroy(unit, vlan1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
            return rv;
        }
    }else {
        bcm_pbmp_t pbmp,ubmp;
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_CLEAR(ubmp);
        BCM_PBMP_PORT_ADD(pbmp,port1);
        BCM_PBMP_PORT_ADD(pbmp,port2);
        rv = bcm_vlan_port_default_action_delete(unit, port1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_delete ( port1) ,rv %d\n", rv);
            return rv;
        }

        rv = bcm_vlan_port_default_action_delete(unit, port2);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_default_action_delete ( port1) ,rv %d\n", rv);
            return rv;
        }

        bcm_vlan_port_destroy(unit,gport1);
        bcm_vlan_port_destroy(unit,gport2);

        rv = bcm_vlan_port_remove(unit,vlan1,pbmp);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_port_remove ,rv %d\n", rv);
            return rv;
        }

        rv = bcm_vlan_destroy(unit, vlan1);
        if (rv != BCM_E_NONE){
            printf("Error, bcm_vlan_destroy ,rv %d\n", rv);
            return rv;
        }
    }
    
    return rv;
}




