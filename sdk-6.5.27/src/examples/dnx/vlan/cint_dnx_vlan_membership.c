
/* **************************************************************************************************************************************************
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
 ****************************************************************************************************************************************************/


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */
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
 **************************************************************************************************** */

/* Initialization of main struct
 * Function allows to re-write default values
 *
 * INPUT:
 *   param: new values for g_vlan_translate_tpid_modify
 */
void l2_basic_bridge_vlan_membership_init(l2_basic_bridge_s *param){

    if (param != NULL) {
       sal_memcpy(&g_l2_basic_bridge, param, sizeof(g_l2_basic_bridge));
    }

}


/* 
 * Return l2_basic_bridge information
 */
void l2_basic_bridge_vlan_membership_struct_get(l2_basic_bridge_s *param){

    sal_memcpy(param, &g_l2_basic_bridge, sizeof(g_l2_basic_bridge));

    return;
}


/* This function runs the main test configuration function with given ports
 *
 * INPUT: unit, Port_1, Port_2, Port_3 and Port4
 */
int l2_basic_bridge_vlan_membership_main_run(int unit,  int Port_1, int Port_2, int Port_3, int Port_4){
 
    l2_basic_bridge_s param;

    l2_basic_bridge_vlan_membership_struct_get(&param);

    param.Port_1 = Port_1;
    param.Port_2 = Port_2;
    param.Port_3 = Port_3;
    param.Port_4 = Port_4;

    return l2_basic_bridge_vlan_membership_run(unit, &param);
}


int l2_basic_bridge_vlan_membership_run(int unit,  l2_basic_bridge_s *param){
    char error_msg[200]={0,};

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

    l2_basic_bridge_vlan_membership_init(param);


    /* Set vlan domain:*/
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_1, bcmPortClassId, g_l2_basic_bridge.Port_1), error_msg);
 
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_2, bcmPortClassId, g_l2_basic_bridge.Port_2), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_3);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_3, bcmPortClassId, g_l2_basic_bridge.Port_3), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_4);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_4, bcmPortClassId, g_l2_basic_bridge.Port_4), error_msg);

    /* Enable VLAN Membership on Port_1, Port_2, Port_3 and Port_4*/
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_1, bcmPortClassVlanMember, g_l2_basic_bridge.Port_1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_2, bcmPortClassVlanMember, g_l2_basic_bridge.Port_2), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_3);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_3, bcmPortClassVlanMember, g_l2_basic_bridge.Port_3), error_msg);
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.Port_4);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, g_l2_basic_bridge.Port_4, bcmPortClassVlanMember, g_l2_basic_bridge.Port_4), error_msg);

    /* Set TPIDs*/
    BCM_IF_ERROR_RETURN_MSG(tpid__tpids_clear_all(unit), "");

    /*Set all 7 valid TPIDs*/
    /* TPID0 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid0);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid0), error_msg);

    /* TPID1 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid1);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid1), error_msg);

    /* TPID2 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid2);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid2), error_msg);

    /* TPID3 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid3);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid3), error_msg);

    /* TPID4 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid4);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid4), error_msg);

    /* TPID5 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid5);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid5), error_msg);

    /* TPID6 */
    snprintf(error_msg, sizeof(error_msg), "(tpid_value=0x%04x)", g_l2_basic_bridge.tpids.tpid6);
    BCM_IF_ERROR_RETURN_MSG(tpid__tpid_add(unit, g_l2_basic_bridge.tpids.tpid6), error_msg);

    /* Create VLAN=100:*/
    snprintf(error_msg, sizeof(error_msg), "(vis=%d)", g_l2_basic_bridge.vsi);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, g_l2_basic_bridge.vsi), error_msg);

    /* Create VLAN=102:*/
    snprintf(error_msg, sizeof(error_msg), "(vis=%d)", g_l2_basic_bridge.egr_new_outer_vid);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, g_l2_basic_bridge.egr_new_outer_vid), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(vis=%d)", g_l2_basic_bridge.egr_new_outer_vid);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, g_l2_basic_bridge.egr_new_outer_vid, param->Port_4, 0), error_msg);

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

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, g_l2_basic_bridge.vsi, pbmp, untag_pbmp), "");

    BCM_PBMP_CLEAR(untag_pbmp);
    /*Add all Ports in new translated VLAN=102*/
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, g_l2_basic_bridge.egr_new_outer_vid, pbmp, untag_pbmp), "");

    /* Add Default_Initial_Vid=100 as Default_Vid on Port_1 */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_1, g_l2_basic_bridge.outer_vid), "");

    /* Add Default_Initial_Vid=300(dummy vid) as Default_Vid on Port_2,Port_3 and Port4 to make sure that in 
     * Ingress Pipeline initial_vid is taken from packet's TAG and not from this Default_Initial_Vid */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_2, 300), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_3, 300), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.Port_4, 300), "");

    /* Enable VLAN filtering on each Port both in ingress and egress side*/
    BCM_IF_ERROR_RETURN_MSG(bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_1, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_2, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_3, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_port_vlan_member_set(unit ,g_l2_basic_bridge.Port_4, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS), "");

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

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    /* Default Port based InLif creation on Port_1 */
    if (*dnxc_data_get(unit, "flow", "general", "is_flow_enabled_for_legacy_applications", NULL))
    {
        BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NON_NATIVE_SET(inlif0,5000);
    } else {
        BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, 5000);
    }
    BCM_GPORT_VLAN_PORT_ID_SET(inlif0, inlif0);

    vlan_port.flags = (BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_WITH_ID);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = g_l2_basic_bridge.vsi;
    vlan_port.vlan_port_id = inlif0;
    vlan_port.port = g_l2_basic_bridge.Port_1;

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

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

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

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

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

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

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    g_l2_basic_bridge.inlif_Port_2_stag = vlan_port.vlan_port_id;
    g_l2_basic_bridge.outlif_Port_2 = vlan_port.vlan_port_id;

    printf("InLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    /*Add All symmetric Lifs created to VSI*/
    /*snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.inlif_Port_1_untag);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_1_untag), error_msg);*/
    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.inlif_Port_2_stag);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_2_stag), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.inlif_Port_3_dtag);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_3_dtag), error_msg);

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.inlif_Port_4_stag);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.inlif_Port_4_stag), error_msg);

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

    snprintf(error_msg, sizeof(error_msg), "( Port_1%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_4;
    port_tpid_class.tag_format_class_id = tag_format_stag;

    snprintf(error_msg, sizeof(error_msg), "( Port_4%d )", g_l2_basic_bridge.Port_4);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = g_l2_basic_bridge.tpids.tpid1;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_3;
    port_tpid_class.tag_format_class_id = tag_format_dtag;

    snprintf(error_msg, sizeof(error_msg), "( Port_3%d )", g_l2_basic_bridge.Port_3);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    /*
     * Incoming is Un-TAG
     * IVE ={None,None}
     */

    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), "(setting IVE to {None,None})");


    /* Incoming is DTAG
    *  IVE ={None,None}
    */

    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), "(setting IVE to {None,None})");


    /*
     * Incoming is STAG
     * IVE ={None,None}
     */

    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), "(setting IVE to {None,None})");


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

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    g_l2_basic_bridge.outlif_Port_1 = vlan_port.vlan_port_id;
    printf("OutLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", g_l2_basic_bridge.outlif_Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.outlif_Port_1), error_msg);

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

    snprintf(error_msg, sizeof(error_msg), "( Port_2%d )", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = tag_format_stag;

    snprintf(error_msg, sizeof(error_msg), "( Port=%d )", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = g_l2_basic_bridge.tpids.tpid1;
    port_tpid_class.tag_format_class_id = tag_format_dtag;

    snprintf(error_msg, sizeof(error_msg), "( Port=%d )", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.Port_1;
    port_tpid_class.tag_format_class_id = tag_format_untag;

    snprintf(error_msg, sizeof(error_msg), "( Port_1%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = tag_format_stag;

    snprintf(error_msg, sizeof(error_msg), "( Port=%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    port_tpid_class.tpid1 = g_l2_basic_bridge.tpids.tpid0;
    port_tpid_class.tpid2 = g_l2_basic_bridge.tpids.tpid1;
    port_tpid_class.tag_format_class_id = tag_format_dtag;

    snprintf(error_msg, sizeof(error_msg), "( Port=%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), error_msg);

    /*
     * Egress Vlan Edit
     * Tag After IVE = Untag
     * Out Pkt = STAG
     * Hence EVE ={Add,None}
     */

    snprintf(error_msg, sizeof(error_msg), "( Port_2%d )", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), error_msg);


    /*
     * Egress Vlan Edit
     * Tag After IVE = STAG
     * Out Pkt = STAG
     * EVE ={Replace,None}
     */

    snprintf(error_msg, sizeof(error_msg), "( Port_2%d )", g_l2_basic_bridge.Port_2);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), error_msg);


    /*
     * Egress Vlan Edit
     * Tag After IVE = DTAG
     * Out Pkt = DTAG
     * Hence EVE ={Replace,None}
     */
    snprintf(error_msg, sizeof(error_msg), "( Port_2%d )", g_l2_basic_bridge.Port_2);
	BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), error_msg);



    /*
    * Egress Vlan Edit
    * Tag After IVE = Untag
    * Out Pkt = UnTAG
    * Hence EVE ={None,None}
    */

    snprintf(error_msg, sizeof(error_msg), "( Port_1%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), error_msg);

    /*
    * Egress Vlan Edit
    * Tag After IVE = STAG
    * Out Pkt = STAG
    * Hence EVE ={None,None}
    */

    snprintf(error_msg, sizeof(error_msg), "( Port_1%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), error_msg);

    /*
    * Egress Vlan Edit
    * Tag After IVE = DTAG
    * Out Pkt = DTAG
    * Hence EVE ={None,None}
    */

    snprintf(error_msg, sizeof(error_msg), "( Port_1%d )", g_l2_basic_bridge.Port_1);
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(
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
                            ), error_msg);


    /*
     * Add l2 address -
     * adding oPort and outLif to MACT
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge.d_mac, g_l2_basic_bridge.vsi);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_l2_basic_bridge.outlif_Port_2;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    return BCM_E_NONE;
}
