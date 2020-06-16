/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * set following soc properties                                                                                                                     *
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.                                                                      *
 * bcm886xx_vlan_translate_mode=1                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  * 
 * This CINT is for testing Ingress and Egress Ports VLAN membership                                                                                * 
 *                                                                                                                                                  *
 * The cint creates a basic L2 switching scheme with following Logical Interfaces(LIF)                                                              *
 *                                                                                                                                                  *
 * - Tx Ports :                                                                                                                                     *
 *    Port_1 : To transmit UnTAG packet                                                                                                             *
 *    Port_3 : To transmit DTAG packet {(TPID=0x8100,SVID=100),(TPID=0x9100, CVID=333)}                                                             *
 *    Port_4 : To transmit STAG packet {TPID=0x8100,SVID=100}                                                                                       *
 * - Rx Port:                                                                                                                                       *
 *    Port_2 : To receive                                                                                                                           *
 *            - STAG packet {TPID=0x9100, SVID=102} when ingress packet is UnTAG and STAG                                                           *
 *            - DTAG packet {TPID=0x9100, SVID=102} and {INNER_TPID=0x9100, IVID=333} when ingress packet is DTAG                                   *
 *    Received VID i.e. 102 is changed from incoming VID to check that Egress VLAN membership happens on VID changed by EVE                         *
 * - VLAN_DOMAINs and Ports are 1:1 mapped                                                                                                          *
 * - Lifs are created on Each Tx port as:                                                                                                         *
 *     inlif_Port_1_untag = Port_1.Default_Lif                                                                                                      *
 *     inl/out_lif_Port_2_stag  = ISEM lookup on {Port_2,SVID=100}                                                                                        *
 *     in/out_lif_Port_3_dtag  = ISEM lookup on {Port_3,SVID=100}                                                                                        *
 *     in/out_lif_Port_4_stag  = ISEM lookup on {Port_4,SVID=100}                                                                                        *
 * - OutLif is created on Port_1 as:                                                                                                                *
 *     outlif_Port_1_untag = EEDB based i.e. using criteria MATCH_PORT_VLAN and VID=100                                                             *
 * - Canonical format is NOOP so following is IVE{outer_tag_action,inner_tag_action} command is applied on each InLif:                              *
 *     inlif_Port_1_untag.IVE = {bcmvlanactionNone,bcmvlanactionNone}                                                                               *
 *     inlif_Port_3_dtag.IVE = {bcmvlanactionNone,bcmvlanactionNone}                                                                                *
 *     inlif_Port_4_stag.IVE = {bcmvlanactionNone,bcmvlanactionNone}                                                                                *
 * - All Lifs are part of VSI=100                                                                                                                   *
 * - FWD lookup :                                                                                                                                   *
 *     The packets are forwarded to the egress using MACT (VSI=100 and MAC_DA) with destination as Port_2 with out_lif_Port_2_stag                  *
 *                                                                                                                                                  *
 * - EVE command on OutLif of Port_2 are:                                                                                                           *
 *   - After IVE, UnTAG packet then {bcmvlanactionAdd,bcmvlanactionNone}                                                                            *
 *   - After IVE, STAG packet then {bcmvlanactionReplace,bcmvlanactionNone}                                                                         *
 *   - After IVE, DTAG packet then {bcmvlanactionReplace,bcmvlanactionNone}                                                                         *
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
/* tag formats */
int tag_format_untag = 0;
int tag_format_stag = 4;
int tag_format_ctag = 8;
int tag_format_dtag = 16;


struct global_tpids_s{
    uint16 tpid0;  /* tpid0*/
    uint16 tpid1;  /* tpid1*/
    uint16 tpid2;  /* tpid2*/
    uint16 tpid3;  /* tpid3*/
    uint16 tpid4;  /* tpid4*/
    uint16 tpid5;  /* tpid5*/
    uint16 tpid6;  /* tpid6*/
};

/*  Main Struct  */
struct l2_basic_bridge_s{
    bcm_gport_t Port_1;
    bcm_gport_t Port_2;
    bcm_gport_t Port_3;
    bcm_gport_t Port_4;
    bcm_vlan_t vsi;
    bcm_mac_t d_mac;
    global_tpids_s tpids;
    bcm_gport_t inlif_Port_1_untag;
    bcm_gport_t inlif_Port_2_stag;
    bcm_gport_t inlif_Port_3_dtag;
    bcm_gport_t inlif_Port_4_stag;
    bcm_gport_t outlif_Port_1;
    bcm_gport_t outlif_Port_2;
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    bcm_vlan_t egr_new_outer_vid;
};

/* Initialization of Main struct */
l2_basic_bridge_s g_l2_basic_bridge = {
    /*Port_1*/
    0,
    /*Port_2*/
    0,
    /*Port_3*/
    0,
    /*Port_4*/
    0,
    /*vsi*/
    100,
    /*d_mac*/
    {0x00, 0x00, 0x00, 0x00, 0x12, 0x34},
    /*TPIDS*/
    {0x8100, 0x9100, 0x88a8, 0x88e7, 0xaaaa, 0xbbbb, 0x5555},
    /*inlif_Port_1_untag  */
    0,
    /*inlif_Port_2_stag  */
    0,
    /*inlif_Port_3_dtag  */
    0,
    /* inlif_Port_4_stag */
    0,
    /* outlif_Port_1 */
    0,
    /* outlif_Port_2 */
    0,
    /* outer_vid */
    100,
    /*inner_vid*/
    333,
	/* egr_new_outer_vid*/
    102,
};



/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_vlan_translate_tpid_modify
 */
int l2_basic_bridge_vlan_membership_init(int unit, l2_basic_bridge_s *param){
     int rv;

    if (param != NULL) {
       sal_memcpy(&g_l2_basic_bridge, param, sizeof(g_l2_basic_bridge));
    }

    int is_jericho2 = 0;
    rv = is_device_jericho2(unit, &is_jericho2);

    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2)
    {
        /* JER1*/
        advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

        if (!advanced_vlan_translation_mode ) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


/* 
 * Return l2_basic_bridge information
 */
void l2_basic_bridge_vlan_membership_struct_get(int unit, l2_basic_bridge_s *param){

    sal_memcpy( param, &g_l2_basic_bridge, sizeof(g_l2_basic_bridge));

    return;
}


/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit, Port_1, Port_2, Port_3 and Port4
 */
int l2_basic_bridge_vlan_membership_main_run(int unit,  int Port_1, int Port_2, int Port_3, int Port_4){
 
    l2_basic_bridge_s param;

    l2_basic_bridge_vlan_membership_struct_get(unit, &param);

    param.Port_1 = Port_1;
    param.Port_2 = Port_2;
    param.Port_3 = Port_3;
    param.Port_4 = Port_4;

    return l2_basic_bridge_vlan_membership_run(unit, &param);
}


int l2_basic_bridge_vlan_membership_run(int unit,  l2_basic_bridge_s *param){
    int rv;

    /* vid for tag actions */
    int new_outer_vid = 1024;
    int egr_new_outer_vid = 102;
    int new_inner_vid = 1500;
    int egr_new_inner_vid = 1500;

    /* vlan edit profiles */
    int ing_vlan_edit_profile_untag=1;
    int ing_vlan_edit_profile_stag=2;
    int ing_vlan_edit_profile_ctag=3;
    int ing_vlan_edit_profile_dtag=4;
    int egr_vlan_edit_profile_untag=5;
    int egr_vlan_edit_profile_stag=6;
    int egr_vlan_edit_profile_ctag=7;
    int egr_vlan_edit_profile_dtag=8;

    rv = l2_basic_bridge_vlan_membership_init(unit, param);

    if (rv != BCM_E_NONE) {
        printf("Error, in l2_basic_bridge_vlan_membership_init\n");
        return rv;
    }

    /* Set vlan domain:*/
    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_1, bcmPortClassId, g_l2_basic_bridge.Port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_1);
        return rv;
    }
 
    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_2, bcmPortClassId, g_l2_basic_bridge.Port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_2);
        return rv;
    }

    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_3, bcmPortClassId, g_l2_basic_bridge.Port_3);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_3);
        return rv;
    }
    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_4, bcmPortClassId, g_l2_basic_bridge.Port_4);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_4);
        return rv;
    }

    /* Enable VLAN Membership on Port_1, Port_2, Port_3 and Port_4*/
	rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_1, bcmPortClassVlanMember, g_l2_basic_bridge.Port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_1);
        return rv;
    }

    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_2, bcmPortClassVlanMember, g_l2_basic_bridge.Port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_2);
        return rv;
    }

    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_3, bcmPortClassVlanMember, g_l2_basic_bridge.Port_3);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_3);
        return rv;
    }
    rv = bcm_port_class_set(unit, g_l2_basic_bridge.Port_4, bcmPortClassVlanMember, g_l2_basic_bridge.Port_4);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.Port_4);
        return rv;
    }

    /* Set TPIDs*/
   rv = tpid__tpids_clear_all(unit);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpids_clear_all, rv = %d\n", rv);
       return rv;
   }

   /*Set all 7 valid TPIDs*/
   /* TPID0 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid0);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid0, rv);
       return rv;
   }

   /* TPID1 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid1);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid1, rv);
       return rv;
   }

   /* TPID2 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid2);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid2, rv);
       return rv;
   }

   /* TPID3 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid3);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid3, rv);
       return rv;
   }

    /* TPID4 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid4);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid4, rv);
       return rv;
   }

   /* TPID5 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid5);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid5, rv);
       return rv;
   }

   /* TPID6 */
   rv = tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid6);
   if (rv != BCM_E_NONE) {
       printf("Error, in tpid__tpid_add(tpid_value=0x%04x), rv = %d\n", g_l2_basic_bridge.tpids.tpid6, rv);
       return rv;
   }


    /* Create VLAN=100:*/
    rv = bcm_vlan_create(unit, g_l2_basic_bridge.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vis=%d)\n", g_l2_basic_bridge.vsi);
        return rv;
    }

    /* Create VLAN=102:*/
    rv = bcm_vlan_create(unit, g_l2_basic_bridge.egr_new_outer_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vis=%d)\n", egr_new_outer_vid);
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, g_l2_basic_bridge.egr_new_outer_vid, param->Port_4, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add(vsi=%d)\n", egr_new_outer_vid);
        return rv;
    }

    /*
     * Set VLAN membership for all Ports
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);

    BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.Port_2);
    BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.Port_3);
    BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.Port_4);
    BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.Port_1); /*Port_1 is part of both untag_pbm and pbm. Putting it in pbm will configure IPPF_VLAN_MEMBERSHIP_TABLE.VLAN_PORT_MEMBER_LINE*/
    BCM_PBMP_PORT_ADD(untag_pbmp, g_l2_basic_bridge.Port_1);

    rv = bcm_vlan_port_add(unit, g_l2_basic_bridge.vsi, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }

    BCM_PBMP_CLEAR(untag_pbmp);
    /*Add all Ports in new translated VLAN=102*/
    rv = bcm_vlan_port_add(unit, g_l2_basic_bridge.egr_new_outer_vid, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }

    /* Add Default_Initial_Vid=100 as Default_Vid on Port_1 */
    rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_1, g_l2_basic_bridge.outer_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    /* Add Default_Initial_Vid=300(dummy vid) as Default_Vid on Port_2,Port_3 and Port4 to make sure that in 
     * Ingress Pipeline initial_vid is taken from packet's TAG and not from this Default_Initial_Vid */
    rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_2, 300);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_3, 300);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_4, 300);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    /* Enable VLAN filtering on each Port both in ingress and egress side*/
    rv = bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_1, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS );
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_2, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS );
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_3, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS );
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }
    rv = bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_4, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS );
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_vlan_member_set\n");
        return rv;
    }

    /*
     * Creates default In-Lif
     */

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    bcm_gport_t inlif0;

    /* Dummy lif creation Just to get non zero in lif*/
    vlan_port.flags = (BCM_VLAN_PORT_CREATE_INGRESS_ONLY);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.vsi = g_l2_basic_bridge.vsi;
    vlan_port.port = g_l2_basic_bridge.Port_3;
    vlan_port.match_vlan = 10;
    vlan_port.match_inner_vlan = 20;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create dummy(port = %d)\n", vlan_port.port);
        return rv;
    }

    /* Default Port based InLif creation on Port_1 */
    /*GPORT_TYPE <<26, GPORT_SUB_TYPE <<22;ingress_only<<20;lif vlaue*/
    BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, 5000);
    BCM_GPORT_VLAN_PORT_ID_SET(inlif0, inlif0);

    vlan_port.flags = (BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_WITH_ID);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = g_l2_basic_bridge.vsi;
    vlan_port.vlan_port_id = inlif0;
    vlan_port.port = g_l2_basic_bridge.Port_1;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.inlif_Port_1_untag = vlan_port.vlan_port_id;
    g_l2_basic_bridge.outlif_Port_1 = vlan_port.vlan_port_id;

    printf("InLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    /* SVLAN_Db based InLif creation on Port_4 */
    /*GPORT_TYPE <<26, GPORT_SUB_TYPE <<22;0<<20;lif vlaue*/
    BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, 0, 6000);
    BCM_GPORT_VLAN_PORT_ID_SET(inlif0, inlif0);

    vlan_port.flags =  BCM_VLAN_PORT_WITH_ID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.vlan_port_id = inlif0;
    vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
    vlan_port.port = g_l2_basic_bridge.Port_4;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.inlif_Port_4_stag = vlan_port.vlan_port_id;

    printf("InLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    /* S_C_VLAN_Db based InLif creation on Port_3*/
    /*GPORT_TYPE <<26, GPORT_SUB_TYPE <<22;0<<20;lif vlaue*/
    BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, 0, 7000);
    BCM_GPORT_VLAN_PORT_ID_SET(inlif0, inlif0);

    vlan_port.flags =  BCM_VLAN_PORT_WITH_ID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;  /*As per Test plan, for DTAG packet as well InLif derivation is based on SVLAN*/
    vlan_port.vsi = 0;
    vlan_port.vlan_port_id = inlif0;
    vlan_port.port = g_l2_basic_bridge.Port_3;
    vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.inlif_Port_3_dtag = vlan_port.vlan_port_id;

    printf("InLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, 0, 8000);
    BCM_GPORT_VLAN_PORT_ID_SET(inlif0, inlif0);

    vlan_port.flags =  BCM_VLAN_PORT_WITH_ID;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.vlan_port_id = inlif0;
    vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
    vlan_port.port = g_l2_basic_bridge.Port_2;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.inlif_Port_2_stag = vlan_port.vlan_port_id;
    g_l2_basic_bridge.outlif_Port_2 = vlan_port.vlan_port_id;

    printf("InLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

	/*Add All symmetric Lifs created to VSI*/
    /*rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_1_untag);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.inlif_Port_1_untag);
        return rv;
    }*/
    rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_2_stag);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.inlif_Port_2_stag);
        return rv;
    }
    rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_3_dtag);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.inlif_Port_3_dtag);
        return rv;
    }
    rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_4_stag);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.inlif_Port_4_stag);
        return rv;
    }

    /*
     * Ingress LLVP configs
     * Incoming STAG
     */
    bcm_port_tpid_class_t port_tpid_class;
    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_1;
    port_tpid_class.tag_format_class_id = tag_format_untag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port_1$d )\n",g_l2_basic_bridge.Port_1 );
        return rv;
    }

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_4;
    port_tpid_class.tag_format_class_id = tag_format_stag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port_4$d )\n",g_l2_basic_bridge.Port_4);
        return rv;
    }

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = g_l2_basic_bridge.tpids.tpid1;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_3;
    port_tpid_class.tag_format_class_id = tag_format_dtag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port_3$d )\n",g_l2_basic_bridge.Port_3);
        return rv;
    }

    /*
     * Incoming is Un-TAG
     * IVE ={None,None}
     */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.inlif_Port_1_untag,/* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid0,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid1,       /* inner_tpid */
                            bcmVlanActionNone,                   /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            new_outer_vid,                       /* new_outer_vid*/
                            new_inner_vid,                       /* new_inner_vid*/
                            ing_vlan_edit_profile_untag,         /* vlan_edit_profile */
                            tag_format_untag,                    /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {None,None}\n");
        return rv;
    }

	/* Incoming is DTAG
    *  IVE ={None,None}
    */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.inlif_Port_3_dtag, /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid0,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid1,       /* inner_tpid */
                            bcmVlanActionNone,                   /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            new_outer_vid,                       /* new_outer_vid*/
                            new_inner_vid,                       /* new_inner_vid*/
                            ing_vlan_edit_profile_dtag,          /* vlan_edit_profile */
                            tag_format_dtag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {None,None}\n");
        return rv;
    }

	/*
     * Incoming is STAG
     * IVE ={None,None}
     */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.inlif_Port_4_stag, /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid0,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid1,       /* inner_tpid */
                            bcmVlanActionNone,                   /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            new_outer_vid,                       /* new_outer_vid*/
                            new_inner_vid,                       /* new_inner_vid*/
                            ing_vlan_edit_profile_stag,          /* vlan_edit_profile */
                            tag_format_stag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {None,None}\n");
        return rv;
    }


    /* Create Outlif in EEDB for Port_2 */
    bcm_gport_t outlif;
    /* Create Outlif in EEDB for Port_1 */
    BCM_GPORT_SUB_TYPE_LIF_SET(outlif, 0, 10003);
    BCM_GPORT_VLAN_PORT_ID_SET(outlif, outlif);

    vlan_port.flags = BCM_VLAN_PORT_WITH_ID; 
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = 0;
    vlan_port.vlan_port_id = outlif;
    vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
    vlan_port.port = g_l2_basic_bridge.Port_1;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.outlif_Port_1 = vlan_port.vlan_port_id;
    printf("OutLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.outlif_Port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.outlif_Port_1);
        return rv;
    }

    /*
     * Egress LLVP configs
     * As Canonical Format is Untag
     * Hence packet after IVE in egress will be untag
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_2;
    port_tpid_class.tag_format_class_id = tag_format_untag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port_2$d )\n",g_l2_basic_bridge.Port_2);
        return rv;
    }

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = tag_format_stag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port=%d )\n",g_l2_basic_bridge.Port_2);
        return rv;
    }

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = g_l2_basic_bridge.tpids.tpid1;
    port_tpid_class.tag_format_class_id = tag_format_dtag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port=%d )\n",g_l2_basic_bridge.Port_2);
        return rv;
    }

    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_1;
    port_tpid_class.tag_format_class_id = tag_format_untag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port_1$d )\n",g_l2_basic_bridge.Port_1);
        return rv;
    }

	port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = tag_format_stag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port=%d )\n",g_l2_basic_bridge.Port_1);
        return rv;
    }

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = g_l2_basic_bridge.tpids.tpid1;
    port_tpid_class.tag_format_class_id = tag_format_dtag;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( Port=%d )\n",g_l2_basic_bridge.Port_1);
        return rv;
    }

    /*
     * Egress Vlan Edit
     * Tag After IVE = Untag
     * Out Pkt = STAG
     * Hence EVE ={Add,None}
     */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.outlif_Port_2,   /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid1,     /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid0,     /* inner_tpid */
                            bcmVlanActionAdd,                  /* outer_action */
                            bcmVlanActionNone,                 /* inner_action */
                            egr_new_outer_vid,                 /* new_outer_vid*/
                            egr_new_inner_vid,                 /* new_inner_vid*/
                            egr_vlan_edit_profile_stag,        /* vlan_edit_profile */
                            tag_format_untag,                  /* tag_format */
                            FALSE                              /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set( Port_2$d )\n",g_l2_basic_bridge.Port_2);
        return rv;
    }

    /*
     * Egress Vlan Edit
     * Tag After IVE = STAG
     * Out Pkt = STAG
     * EVE ={Replace,None}
     */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.outlif_Port_2,     /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid1,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid0,       /* inner_tpid */
                            bcmVlanActionReplace,                   /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            egr_new_outer_vid,                   /* new_outer_vid*/
                            egr_new_inner_vid,                   /* new_inner_vid*/
                            egr_vlan_edit_profile_stag,          /* vlan_edit_profile */
                            tag_format_stag,                     /* tag_format */
                            FALSE                                /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set( Port_2$d )\n",g_l2_basic_bridge.Port_2);
        return rv;
    }

    /*
     * Egress Vlan Edit
     * Tag After IVE = DTAG
     * Out Pkt = DTAG
     * Hence EVE ={Replace,None}
     */
	rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.outlif_Port_2,     /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid1,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid0,       /* inner_tpid */
                            bcmVlanActionReplace,                   /* outer_action */
                            bcmVlanActionNone,                 /* inner_action */
                            egr_new_outer_vid,                   /* new_outer_vid*/
                            egr_new_inner_vid,                   /* new_inner_vid*/
                            egr_vlan_edit_profile_stag,          /* vlan_edit_profile */
                            tag_format_dtag,                     /* tag_format */
                            FALSE                                /* is_ive */
                            );


    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set( Port_2$d )\n",g_l2_basic_bridge.Port_2);
        return rv;
    }

    /*
    * Egress Vlan Edit
    * Tag After IVE = Untag
    * Out Pkt = UnTAG
    * Hence EVE ={None,None}
    */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.outlif_Port_1,     /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid1,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid0,       /* inner_tpid */
                            bcmVlanActionMappedAdd,              /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            egr_new_outer_vid,                   /* new_outer_vid*/
                            egr_new_inner_vid,                   /* new_inner_vid*/
                            egr_vlan_edit_profile_untag,         /* vlan_edit_profile */
                            tag_format_untag,                    /* tag_format */
                            FALSE                                /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set( Port_1$d )\n",g_l2_basic_bridge.Port_1);
        return rv;
    }
    /*
    * Egress Vlan Edit
    * Tag After IVE = STAG
    * Out Pkt = STAG
    * Hence EVE ={None,None}
    */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.outlif_Port_1,     /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid1,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid0,       /* inner_tpid */
                            bcmVlanActionNone,                   /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            egr_new_outer_vid,                   /* new_outer_vid*/
                            egr_new_inner_vid,                   /* new_inner_vid*/
                            egr_vlan_edit_profile_untag,         /* vlan_edit_profile */
                            tag_format_stag,                    /* tag_format */
                            FALSE                                /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set( Port_1$d )\n",g_l2_basic_bridge.Port_1);
        return rv;
    }
    /*
    * Egress Vlan Edit
    * Tag After IVE = DTAG
    * Out Pkt = DTAG
    * Hence EVE ={None,None}
    */

    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.outlif_Port_1,     /* lif - need the gport, thus use inLifList instead of outLifList */
                            g_l2_basic_bridge.tpids.tpid1,       /* outer_tpid */
                            g_l2_basic_bridge.tpids.tpid0,       /* inner_tpid */
                            bcmVlanActionNone,                   /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            egr_new_outer_vid,                   /* new_outer_vid*/
                            egr_new_inner_vid,                   /* new_inner_vid*/
                            egr_vlan_edit_profile_untag,         /* vlan_edit_profile */
                            tag_format_dtag,                    /* tag_format */
                            FALSE                                /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set( Port_1$d )\n",g_l2_basic_bridge.Port_1);
        return rv;
    }

    int is_jericho2;
    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (!is_jericho2)
    {
        /* Set Trap for PortNotVlanMember in ingress pipeline*/
        /* If Ingress Port is NOT VLAN member then that packet will be redirected to Port_1 */
         bcm_rx_trap_config_t config;
         int trap_id =5;

         bcm_rx_trap_config_t_init($config);

         config.flags = (BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_DEST);
         config.trap_strength = 5;
         config.dest_port = g_l2_basic_bridge.Port_1;
         config.encap_id = g_l2_basic_bridge.outlif_Port_1;

         rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapPortNotVlanMember, &trap_id);
         if (rv != BCM_E_NONE) {
            printf("Error, bcm_rx_trap_type_create\n");
            return rv;
         }

         rv = bcm_rx_trap_set(unit, trap_id, &config);
         if (rv != BCM_E_NONE) {
            printf("Error, bcm_rx_trap_set\n");
            return rv;
         }
    }

    /*
     * Add l2 address -
     * adding oPort and outLif to MACT
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge.d_mac, g_l2_basic_bridge.vsi);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_l2_basic_bridge.outlif_Port_2;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}
