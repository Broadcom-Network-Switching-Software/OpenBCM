/* $Id: cint_l2_basic_bridge_with_vlan_editing.c,
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* **************************************************************************************************************************************************
 * set following soc properties                                                                                                                     *
 * # Specify the "bcm886xx_vlan_translate_mode" mode. 0: disabled, 1: enabled.                                                                      *
 * bcm886xx_vlan_translate_mode=1                                                                                                                   *
 *                                                                                                                                                  *
 *                                                                                                                                                  * 
 * This CINT demonstrates basic l2 bridging with IVE and EVE actions for differnt canonical formats                                                 * 
 * We cover scenario 1(l2 basic bridging with canonical format untag) and scenario 8 (vlan editing for canonical format DTAG)                       *
 *                                                                                                                                                  *
 * From JR2 onwards, the ctag and stag packet formats are distinguished according to the SDK 
 * initial TPID configuration: 0x8100 for ctag and 0x9100 for stag.
 *  
 * The cint creates a basic L2VPN scheme with two different Logical Interfaces(LIF) as follows.                                                     *
 * InLif : Based on incoming pkt format we create inlif                                                                                             *
 *         We are using step argument as the refernece                                                                                              *
 *         -----------------------------------------------------------------------------                                                            *
 *         | STEP  | incoming pkt format | inlif criteria         | Test plan Scenario  |                                                           *
 *         ------------------------------------------------------------------------------                                                           *
 *         |   1   |    untag            | Default LIF            |  8.1                |                                                           *
 *         ------------------------------------------------------------------------------                                                           *
 *         |   2   |     ctag            | ISEM {VDxCVLAN}        |  8.2                |                                                           *
 *         ------------------------------------------------------------------------------                                                           *
 *         |   3   |     dtag            | ISEM {VDxSVLAN}        |  8.3                |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   4   |    untag            | Default LIF            |  1.1                |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   5   |     stag            | ISEM {VDxsVLAN}        |  1.2                |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   6   |     dtag            | ISEM {VDxSVLANxCVLAN}  |  1.3                |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   7   |    untag            | Default LIF            |  13                 |                                                           *
 *         ------------------------------------------------------------------------------                                                           *
 *         |   8   |     stag            | ISEM {VDxsVLAN}        |  QOS remarking      |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   9   |     ctag            | Default Lif            |  VSI_Assignment_mode|                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   10  |     stag            | Default Lif            |  VSI_Assignment_mode|                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   11  |     dtag            | Default Lif            |  VSI_Assignment_mode|                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   12  |     stag            | ISEM {VDxsVLAN}        |  Learning           |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 *         |   13  |     stag            | ISEM {VDxsVLAN}        |  Learning Extended  |                                                           *
 *         -----------------------------------------------------------------------------                                                            *
 * Outlif :                                                                                                                                         *
 *        For all scenarios MACT will result outlif, and lookup this outlif in EEDB to get the Tag actions                                          *
 *                                                                                                                                                  *
 * Ports :                                                                                                                                          *
 * In_port : iPort                                                                                                                                  *
 * Out_port : oPort                                                                                                                                 *
 *                                                                                                                                                  *
 * TAG ACTIONS are applied as follows                                                                                                               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         | STEP  | incoming pkt format |  Canonical format      |       IVE           |       EVE           | Test plan Scenario  |               *
 *         --------------------------------------------------------------------------------------------------------------------------               *
 *         |   1   |    untag            |      DTAG              |  {Add,Add}          |  {None,None}        |  8.1                |               *
 *         --------------------------------------------------------------------------------------------------------------------------               *
 *         |   2   |     ctag            |      DTAG              |  {Add,None}         |  {Delete,Delete}    |  8.2                |               *
 *         --------------------------------------------------------------------------------------------------------------------------               *
 *         |   3   |     dtag            |      DTAG              |  {None,None}        |  {Replace,Replace}  |  8.3                |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   4   |    untag            |      UNTAG             |  {None,None}        |  {Add,None}         |  1.1                |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   5   |     stag            |      UNTAG             |  {Delete,None}      |  {Add,None}         |  1.2                |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   6   |     dtag            |      UNTAG             |  {Delete,Delete}    |  {Add,None}         |  1.3                |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   7   |    untag            |      UNTAG             |  {None,None}        |  {Add,None}         |  13                 |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   8   |     stag            |      UNTAG             |  {Delete,None}      |  {Add,None}         |  QOS remarking      |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   9   |     ctag            |      DTAG              |  {Add,None}         |  {Delete,Delete}    |  VSI_Assignment_mode|               *
 *         --------------------------------------------------------------------------------------------------------------------------               *
 *         |   10  |     stag            |      UNTAG             |  {Delete,None}      |  {Add,None}         |  VSI_Assignment_mode|               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   11  |     dtag            |      DTAG              |  {None,None}        |  {Replace,Replace}  |  VSI_Assignment_mode|               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   12  |     stag            |      UNTAG             |  {Delete,None}      |  {Add,None}         |  Learning           |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *         |   13  |     stag            |      UNTAG             |  {Delete,None}      |  {Add,None}         |  Learning Extended  |               *
 *         ----------------------------------------------------------------------------- --------------------- ---------------------                *
 *  NOTE : Step -7 is to verify delete rxpkt tag based on tx_tag_enable,hence though EVE is ADD,this tag will be deleted by remove_tx_tag           *
 *         Step -8 :                                                                                                                                *
 *              * Ingress : PCP-DE --> TC-DP                                                                                                        *
 *                          PCP-DE --> Nwk-Qos                                                                                                      *
 *              * Egress  : Nwk-Qos --> PCP-DE (pri_cfi_action is add)                                                                              *
 *                                                                                                                                                  *
 * FWD  lookup :                                                                                                                                    *
 * The packets are forwarded to the egress using MACT with oPort and Out-LIF forwarding information.                                                *
 *         SVL Key {VSI x MAC-DA }                                                                                                                  *
 *         IVL Key {Outer-VID x VSI x MAC-DA }                                                                                                      *
 *         -------------------------------------------------------------                                                                            *
 *         | STEP  |    MACT context     | STEP  |    MACT context     |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   1   |      SVL            |   4   |      SVL            |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   2   |      SVL            |   5   |      SVL            |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   3   |      SVL            |   6   |      SVL            |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   7   |      SVL            |   8   |      SVL            |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   9   |      SVL            |   10  |      SVL            |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   11  |      SVL            |   12  |      SVL            |                                                                            *
 *         -------------------------------------------------------------                                                                            *
 *         |   13  |      SVL            |                                                                                                          *
 *         -------------------------------                                                                                                          *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *             +-------+                                                                                                                            *
 *    iPort    |       |      +-----+      +------+                                                                                                 *
 *    +------> | InLIF +----> | IVE +----> |      |                                                                                                 *
 *             |       |      +-----+      |      |     +-------+   oPort                                                                           *
 *             +-------+                   |      |     |       |   Out-LIF                                                                         *
 *                                         | VSI  +---> | MACT  +-------------+                                                                     *
 *                                         |      |     |       |             |                                                                     *
 *                                         |      |     +-------+             |                                                                     *
 *                                         |      |                           |                                                                     *
 *                                         +------+                           |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                  +---------+                               |                                                                     *
 *     oPort            +------+    |         |                               |                                                                     *
 *    <-----------------+ EVE  | <--+ Out LIF +<------------------------------+                                                                     *
 *                      +------+    |         |                                                                                                     *
 *                                  +---------+                                                                                                     *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * l2_basic_bridge_main_run(int unit,  int iPort, int oPort ,int step) Functions needs to be called in TCL                                          *
 *              iPort - Input Port                                                                                                                  *
 *              oPort - Output Port                                                                                                                 *
 *              step encoding is mentioned above                                                                                                    *
 *                                                                                                                                                  *
 * Packet parameters :                                                                                                                              * 
 *      DA                      :       0x000000001234                                                                                              * 
 *      Incoming outer vid      :       100                                                                                                         * 
 *      Incoming inner vid      :       333                                                                                                         * 
 *      vsi                     :       111                                                                                                         * 
 *      IVE outer vid           :       1024                                                                                                        * 
 *      IVE inner vid           :       1500                                                                                                        * 
 *      EVE outer vid           :       2048                                                                                                        * 
 *      EVE inner vid           :       3000                                                                                                        * 
 *      Incoming outer TPID     :       0x8100                                                                                                      * 
 *      Incoming inner TPID     :       0x9100                                                                                                      * 
 *      IVE outer TPID          :       0x88a8                                                                                                      * 
 *      IVE inner TPID          :       0x88e7                                                                                                      * 
 *      EVE outer TPID          :       0xaaaa                                                                                                      * 
 *      EVE inner TPID          :       0xbbbb                                                                                                      * 
 */
 
 
/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 */
int NUM_OF_TEST_STEPS = 13;
/* vid for tag actions */
int new_outer_vid = 1024;
int new_inner_vid = 1500;
/* tag formats */
int tag_format_untag = 0;
int tag_format_stag = 4;
int tag_format_ctag = 8;
int tag_format_dtag = 16;


/* vlan edit profiles */
int ing_vlan_edit_profile_untag=1;
int ing_vlan_edit_profile_stag=2;
int ing_vlan_edit_profile_ctag=3;
int ing_vlan_edit_profile_dtag=4;
int egr_vlan_edit_profile_untag=5;
int egr_vlan_edit_profile_stag=6;
int egr_vlan_edit_profile_ctag=7;
int egr_vlan_edit_profile_dtag=8;

int egr_qos_map_id = -1;

struct global_tpids_s {
    uint16 tpid0;  /* tpid0*/
    uint16 tpid1;  /* tpid1*/
    uint16 tpid2;  /* tpid2*/
    uint16 tpid3;  /* tpid3*/
    uint16 tpid4;  /* tpid4*/
    uint16 tpid5;  /* tpid5*/
    uint16 tpid6;  /* tpid6*/
};

/* Structure to define IVE and EVE actions*/
struct tag_action_s {
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    bcm_vlan_tpid_action_t outer_tpid_action;
    bcm_vlan_tpid_action_t inner_tpid_action;
    int vlan_edit_profile;
};

/* Structure to defing tag_format Ingress and Egress LLVP configs*/
struct tag_format_s {
    uint32 outer_tpid;
    uint32 inner_tpid;
    int tag_format;
};

/*  Main Struct  */
struct l2_basic_bridge_s {
    bcm_gport_t in_port;
    bcm_gport_t out_port;
    bcm_vlan_t vsi;
    bcm_mac_t d_mac;
    bcm_mac_t s_mac;
    global_tpids_s tpids;
    bcm_gport_t inlif;
    bcm_gport_t outlif;
    bcm_vlan_t outer_vid;
    bcm_vlan_t inner_vid;
    tag_format_s incoming_tag_format[NUM_OF_TEST_STEPS];
    tag_action_s ive_action[NUM_OF_TEST_STEPS];
    tag_format_s egress_tag_format[NUM_OF_TEST_STEPS];
    tag_action_s eve_action[NUM_OF_TEST_STEPS];

};

/* Initialization of Main struct */
l2_basic_bridge_s g_l2_basic_bridge = {
    /*In_port*/
    0,
    /*out_port*/
    0,
    /*vsi*/
    111,
    /*d_mac*/
    {0x00, 0x00, 0x00, 0x00, 0x12, 0x34},
    /*s_mac*/
    {0x00, 0x00, 0x00, 0xab, 0xcd, 0xef},
    /*TPIDS*/
    {0x8100, 0x9100, 0x88a8, 0x88e7, 0xaaaa, 0xbbbb, 0x5555},
    /* Inlif */
    0,
    /* outlif */
    0,
    /* outer_vid */
    100,
    /* inner_vid */
    333,
    /* Incoming tag format*/
    {
            /*
             * Test - 1
             * Untag incoming_tag_format[0]*/
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /*Test2 CTAG incoming_tag_format[1] */
            {0x8100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_ctag},
            /*Test3 DTAG incoming_tag_format[2] */
            {0x9100,0x8100,tag_format_dtag},
            /*Test4 Untag incoming_tag_format[3]*/
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /*Test5 STAG incoming_tag_format[4] */
            {0x9100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_stag},
            /*Test6 DTAG incoming_tag_format[5] */
            {0x9100,0x8100,tag_format_dtag},
            /* Untag incoming_tag_format[6]*/
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /*Test8 STAG incoming_tag_format[7] */
            {0x9100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_stag},
            /*Test9 CTAG incoming_tag_format[8] */
            {0x8100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_ctag},
            /*Test10 STAG incoming_tag_format[9] */
            {0x9100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_stag},
            /*Test11 DTAG incoming_tag_format[10] */
            {0x9100,0x8100,tag_format_dtag},
            /*Test12 STAG incoming_tag_format[11] */
            {0x9100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_stag},
            /*Test13 STAG incoming_tag_format[12] */
            {0x9100,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_stag},
    },

    /* IVE Action */
    {
            /* Canonical format - DTAG
             * Untag to DTAG 
             * ive_action[0] */
            {bcmVlanActionAdd,bcmVlanActionAdd,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_untag},
            /* Ctag to DTAG 
             * ive_action[1] */
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_ctag},
            /* Dtag to DTAG 
             * ive_action[2] */
            {bcmVlanActionNone,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_dtag},
           /* Canonical format - UnTAG
            * Untag to STAG 
            * ive_action[3] */
            {bcmVlanActionNone,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_untag},
           /* Canonical format - UnTAG
            * STAG to STAG 
            * ive_action[4] */
            {bcmVlanActionDelete,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_stag},
           /* Canonical format - UnTAG
            * DTAG to STAG 
            * ive_action[5] */
            {bcmVlanActionDelete,bcmVlanActionDelete,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_dtag},
           /* Canonical format - UnTAG
            * DTAG to STAG 
            * ive_action[6] */
            {bcmVlanActionNone,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_untag},
            /* IVE None
             * ive_action[7] */
            {bcmVlanActionDelete,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_stag},
            /* Ctag to DTAG 
             * ive_action[8] */
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_ctag},
            /* Canonical format - UnTAG
            * STAG to STAG 
            * ive_action[9] */
            {bcmVlanActionDelete,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_stag},
            /* Dtag to DTAG 
             * ive_action[10] */
            {bcmVlanActionNone,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_dtag},
           /* Canonical format - UnTAG
            * STAG to STAG 
            * ive_action[11] */
            {bcmVlanActionDelete,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_stag},
           /* Canonical format - UnTAG
            * STAG to STAG 
            * ive_action[12] */
            {bcmVlanActionDelete,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,ing_vlan_edit_profile_stag},
    },

    /* Egress tag format */
     {
            /* After IVE egress tag will be in canonical format which is DTAG
               egress_tag_format[0] */
            {0x88a8,0x88e7,tag_format_dtag},
            /* egress_tag_format[1] */
            {0x9100, 0x88a8, tag_format_dtag},
            /* egress_tag_format[2] */
            {0x9100,0x8100,tag_format_dtag},
            /* egress_tag_format[3] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[4] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[5] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[6] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[7] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[8] */
            {0x9100, 0x88a8, tag_format_dtag},
            /* egress_tag_format[9] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[10] */
            {0x8100,0x9100,tag_format_dtag},
            /* egress_tag_format[11] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
            /* egress_tag_format[12] */
            {BCM_PORT_TPID_CLASS_TPID_INVALID,BCM_PORT_TPID_CLASS_TPID_INVALID,tag_format_untag},
     },

    /* EVE Action */
    {
            /* Canonical format - DTAG
             * DTag to DTAG 
             * eve_action[0] */
            {bcmVlanActionNone,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_dtag},
            /* DTag to Untag 
             * eve_action[1] */
            {bcmVlanActionDelete,bcmVlanActionDelete,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
            /* Dtag to DTAG' 
             * eve_action[2] */
            {bcmVlanActionReplace,bcmVlanActionReplace,bcmVlanTpidActionModify,bcmVlanTpidActionModify,egr_vlan_edit_profile_dtag},
               /* Canonical format - UnTAG
	        	* Untag to STAG
	        	* eve_action[3]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
               /* Canonical format - UnTAG
	        	* Untag to STAG
	        	* eve_action[4]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
               /* Canonical format - UnTAG
	        	* Untag to STAG
	        	* eve_action[5]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
               /* Canonical format - UnTAG
	        	* Untag to STAG
	        	* eve_action[6]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
            /* stag to STAG' 
             * eve_action[7] */
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
            /* DTag to Untag 
             * eve_action[8] */
            {bcmVlanActionDelete,bcmVlanActionDelete,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
            /* Untag to STAG
	        	* eve_action[9]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
            /* Dtag to DTAG' 
             * eve_action[10] */
            {bcmVlanActionReplace,bcmVlanActionReplace,bcmVlanTpidActionModify,bcmVlanTpidActionModify,egr_vlan_edit_profile_dtag},
               /* Canonical format - UnTAG
	        	* Untag to STAG
	        	* eve_action[11]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
               /* Canonical format - UnTAG
	        	* Untag to STAG
	        	* eve_action[12]*/
            {bcmVlanActionAdd,bcmVlanActionNone,bcmVlanTpidActionNone,bcmVlanTpidActionNone,egr_vlan_edit_profile_untag},
    },
};


 
/* **************************************************************************************************
  --------------          Global  Variables Definitions and Initialization  END       ---------------
 */

/* Initialization of main struct
 * Function allows to re-write default values 
 *
 * INPUT: 
 *   param: new values for g_l2_basic_bridge
 */
int l2_basic_bridge_init(int unit, l2_basic_bridge_s *param){
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
void l2_basic_bridge_modify_struct_get(int unit, l2_basic_bridge_s *param){

    sal_memcpy( param, &g_l2_basic_bridge, sizeof(g_l2_basic_bridge));

    return;
}


/* This function runs the main test configuration function with given ports
 *  
 * INPUT: unit     - unit
 *        iPort - ingress port
 *        oPort - egress port
 */
int l2_basic_bridge_main_run(int unit,  int iPort, int oPort ,int step){
    
    l2_basic_bridge_s param;

    l2_basic_bridge_modify_struct_get(unit, &param);

    param.in_port = iPort;
    param.out_port = oPort;

    return l2_basic_bridge_run(unit, &param ,step);
}


/*
  */
int l2_basic_bridge_run(int unit,  l2_basic_bridge_s *param,int step){
    int rv;
    int i;

    rv = l2_basic_bridge_init(unit, param);

    
    if (step == 3) {
        g_l2_basic_bridge.vsi = 11;
    }

    if (rv != BCM_E_NONE) {
        printf("Error, in l2_basic_bridge_init\n");
        return rv;
    }

    /* Set vlan domain:*/
    rv = vlan_domain_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_domain_create\n");
        return rv;
    }


    /* Set TPIDs*/
    rv = tpids_assign(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in tpids_assign\n");
        return rv;
    }

    /* VSI assignment mode = 1 (FODO.base + VLAN)*/ 
    if (step == 9 || step == 10 || step == 11) {
        g_l2_basic_bridge.vsi = g_l2_basic_bridge.outer_vid;
    }

    /* Create VSI:*/
    rv = bcm_vlan_create(unit, g_l2_basic_bridge.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vis=%d)\n", g_l2_basic_bridge.vsi);
        return rv;
    }

    /* 
     * Set VLAN port membership
     */
    bcm_pbmp_t pbmp, untag_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(untag_pbmp);
   
    if (step == 1 || step == 4) {
        /* Adding TX port to untag pbm to have a symmetry for Ingress and Egress*/
        BCM_PBMP_PORT_ADD(untag_pbmp, g_l2_basic_bridge.in_port);
        BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.out_port);
    } else if (step == 7) {
        BCM_PBMP_PORT_ADD(untag_pbmp, g_l2_basic_bridge.in_port);
        BCM_PBMP_PORT_ADD(untag_pbmp, g_l2_basic_bridge.out_port);
        BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.in_port);
        BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.out_port);
    } else {
        BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.in_port);
        BCM_PBMP_PORT_ADD(pbmp, g_l2_basic_bridge.out_port);
    }
    /* Adding dummy ports to make sure the MACT lookup happens and forwarded to correct port
    bcm_gport_t dummy_port_1 = 250;
    bcm_gport_t dummy_port_2 = 251;
    BCM_PBMP_PORT_ADD(pbmp, dummy_port_1);
    BCM_PBMP_PORT_ADD(pbmp, dummy_port_2); */

    rv = bcm_vlan_port_add(unit, g_l2_basic_bridge.vsi, pbmp, untag_pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, g_l2_basic_bridge.outer_vid, g_l2_basic_bridge.in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, g_l2_basic_bridge.outer_vid, g_l2_basic_bridge.out_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, g_l2_basic_bridge.in_port, g_l2_basic_bridge.outer_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set\n");
        return rv;
    }

    
    /* Create Lif and IVE based on the incoming tag_type */
    rv = create_lif_and_ive(unit,step);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_lif_and_ive\n");
        return rv;
    }
    
    /* VSI assignment mode = 1 (FODO.base (0) + VLAN)*/ 
    if (step == 9 || step == 10 || step == 11) {
        g_l2_basic_bridge.vsi = 0 + g_l2_basic_bridge.outer_vid;
    }

    /* Create Outlif in EEDB */
    bcm_gport_t outlif;
    outlif=(17<<26|2<<22|10000);

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    if( step == 3) {
        /* ESEM */
        /*vlan_port.flags = (BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_VLAN_TRANSLATION); */
        vlan_port.flags = (BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS); 
        vlan_port.vsi = g_l2_basic_bridge.vsi;
    } else {
        /* EEDB */
        vlan_port.flags = BCM_VLAN_PORT_WITH_ID; 
        vlan_port.vsi = 0;
    }
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    if( step != 3) {
        vlan_port.vlan_port_id = outlif;
        vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
    }
    vlan_port.port = g_l2_basic_bridge.out_port;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.outlif = vlan_port.vlan_port_id;
    printf("OutLif create: port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);

    if (step != 3) {
        rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi,g_l2_basic_bridge.outlif);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.outlif);
            return rv;
        }
    }

    /*
     * Egress LLVP configs
     * For all scenarios canonical format is DTAG, Hence LLVP configs are same in Egress
     */
    bcm_port_tpid_class_t port_tpid_class;
    bcm_port_tpid_class_t_init(&port_tpid_class);

    port_tpid_class.tpid1 = g_l2_basic_bridge.egress_tag_format[step -1].outer_tpid;
    port_tpid_class.tpid2 = g_l2_basic_bridge.egress_tag_format[step -1].inner_tpid;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.port =  g_l2_basic_bridge.out_port;
    port_tpid_class.tag_format_class_id = g_l2_basic_bridge.egress_tag_format[step -1].tag_format;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set( oPort )\n");
        return rv;
    }

    /* Programming EVE based on the outlif*/
    rv = set_eve_based_on_incoming_tag(unit,step);
    if (rv != BCM_E_NONE) {
        printf("Error, in set_eve_based_on_incoming_tag");
        return rv;
    }
    
    bcm_vlan_control_vlan_t control;
    control.flags2 = 0;
    control.forwarding_vlan = g_l2_basic_bridge.vsi;
    control.unknown_unicast_group = g_l2_basic_bridge.vsi;
    control.broadcast_group = g_l2_basic_bridge.vsi;
    control.unknown_multicast_group = g_l2_basic_bridge.vsi;
    rv = bcm_vlan_control_vlan_set (unit, g_l2_basic_bridge.vsi, control);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_control_vlan_set");
        return rv;
    }
    /*
     * Add l2 address - 
     * adding oPort and outLif to MACT
     */
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge.d_mac, g_l2_basic_bridge.vsi);

    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = g_l2_basic_bridge.out_port;
    bcm_gport_t forward_encap_id;
    if (step != 3) {
        /* Step 3 we will derive outlif properties from ESEM */
        BCM_FORWARD_ENCAP_ID_VAL_SET(forward_encap_id,BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF,BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_VLAN_PORT,10000);
        l2_addr.encap_id = forward_encap_id;
    }

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    /* Learning enable for steps 12-13 */
    if ((step == 12) || (step == 13)) {
        rv = l2_basic_bridge_learning(unit);
        if (rv != BCM_E_NONE) {
        printf("Error,l2_basic_bridge_learning\n");
        return rv;
        }
    }

    if (step == 8) {
        int ingress_qos_id = 5;
        int egress_qos_id = -1;
        int opcode_ingress = -1;
        int opcode_egress = -1;
        rv = qos_map_l2(unit,&ingress_qos_id,&egress_qos_id,&opcode_ingress,&opcode_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, qos_map_l2_example\n");
            return rv;
        }
        egr_qos_map_id = egress_qos_id;

        rv = qos_map_gport(unit, g_l2_basic_bridge.inlif,ingress_qos_id,-1);
        if (rv != BCM_E_NONE) {
            return rv;
        }

        rv = qos_map_gport(unit, g_l2_basic_bridge.outlif,-1,egress_qos_id);
        if (rv != BCM_E_NONE) {
            return rv;
        }
    }

    return rv;
}


/* Function to create vlan domain*/

int vlan_domain_create(int unit) {
    int rv;
        /*Input Port */
    rv = bcm_port_class_set(unit, g_l2_basic_bridge.in_port, bcmPortClassId, g_l2_basic_bridge.in_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.in_port);
        return rv;
    }
    
    /*Output Port*/
    rv = bcm_port_class_set(unit, g_l2_basic_bridge.out_port, bcmPortClassId, g_l2_basic_bridge.out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set(port = %d)\n", g_l2_basic_bridge.out_port);
        return rv;
    }

    return rv;
}

/* Configure global tpids of switch*/
int tpids_assign(int unit) {
    int rv;

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

   return rv;

}


/* create_lif_and_ive - Functions configures the following
 * Lif and IVE for are created based on incoming tag packet
 * Inlif and IVE details are mentioned in the main description above
 */

int create_lif_and_ive(int unit,int step) {
    int rv;
    int outer_vlan_action;
    int inner_vlan_action;
    int vlan_edit_profile;
    int ive_tag_format;
    int global_lif_id = 0x1388;

    if (is_device_or_above(unit, JERICHO2))
    {
        global_lif_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x388;
    }

    /*
     * Creates In-Lif based on incoming pkt type
     */

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    bcm_gport_t inlif0;
    
    /* Dummy lif creation to get non zero value of local lif*/
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan = 10;
    vlan_port.vsi = 0;
    vlan_port.port =  g_l2_basic_bridge.in_port;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in Dummy bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    /*InLIF creation*/
    vlan_port.flags = BCM_VLAN_PORT_WITH_ID;

    if (step == 1 || step == 4 || step == 7){
        /* Untag scenario */
        vlan_port.flags |= BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port.vsi =  g_l2_basic_bridge.vsi;
        inlif0 |= (1<<20);
    } else if (step == 2)  {
        /* CTAG */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
        vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
    } else if (step == 3 || step == 5 || step == 8 || step == 12 || step == 13)  {
        /* S-TAG */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
    } else if (step == 6) {
        /* D-TAG */
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
        vlan_port.match_vlan = g_l2_basic_bridge.outer_vid;
        vlan_port.match_inner_vlan = g_l2_basic_bridge.inner_vid;
    }

    /* VSI assignment mode =1*/ 
    if (step == 9 || step == 10 || step == 11) {
        vlan_port.flags |= BCM_VLAN_PORT_VSI_BASE_VID | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
        inlif0 |= (1<<20);
    }

    /* Large Entry */
    if (step == 13)
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }


    /*GPORT_TYPE <<26, GPORT_SUB_TYPE <<22; ingress_only/egress_only/symmetric<<20; lif vlaue*/
    if (vlan_port.flags & BCM_VLAN_PORT_CREATE_INGRESS_ONLY) {
       BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, global_lif_id);
    }
    else if (vlan_port.flags & BCM_VLAN_PORT_CREATE_EGRESS_ONLY) {
       BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_lif_id);
    }
    else{
       BCM_GPORT_SUB_TYPE_LIF_SET(inlif0, 0, global_lif_id);
    }
    BCM_GPORT_VLAN_PORT_ID_SET(inlif0, inlif0);

    vlan_port.vlan_port_id = inlif0;
    vlan_port.port = g_l2_basic_bridge.in_port;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    g_l2_basic_bridge.inlif = vlan_port.vlan_port_id;
    printf("InLif create : port=%d, vlan_port_id=0x%08x,\n", vlan_port.port, vlan_port.vlan_port_id);
 
    /* VSI Assignment mode =1 , VSI value should not be set*/
    if (step != 1 && step != 4 && step != 7 &&
        step != 9 && step != 10 && step != 11) {
        rv = bcm_vswitch_port_add(unit, g_l2_basic_bridge.vsi, g_l2_basic_bridge.inlif);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vswitch_port_add(port = %d)\n", g_l2_basic_bridge.inlif);
            return rv;
        }
    }


    /*  Ingress LLVP configs  */
    bcm_port_tpid_class_t port_tpid_class;
    bcm_port_tpid_class_t_init(&port_tpid_class);
    
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    if (step == 2 || step == 9) {
        port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_C;
    }
    port_tpid_class.tpid1 = g_l2_basic_bridge.incoming_tag_format[step -1].outer_tpid;
    port_tpid_class.tpid2 = g_l2_basic_bridge.incoming_tag_format[step -1].inner_tpid;
    port_tpid_class.tag_format_class_id = g_l2_basic_bridge.incoming_tag_format[step -1].tag_format;
    port_tpid_class.port =  g_l2_basic_bridge.in_port;

    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in untag bcm_port_tpid_class_set( iPort$d )\n", i);
        return rv;
    }

    /* Set outer and inner vlan actions according to incoming pkt   */
    /* Canonical format : DTAG                                      */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            g_l2_basic_bridge.inlif,                                         /*  inlif             */
                            g_l2_basic_bridge.tpids.tpid2,                                   /*  outer_tpid        */
                            g_l2_basic_bridge.tpids.tpid3,                                   /*  inner_tpid        */
                            g_l2_basic_bridge.ive_action[step-1].outer_action,               /*  outer_action      */
                            g_l2_basic_bridge.ive_action[step-1].inner_action,               /*  inner_action      */
                            new_outer_vid,                                                   /*  new_outer_vid     */
                            new_inner_vid,                                                   /*  new_inner_vid     */
                            g_l2_basic_bridge.ive_action[step-1].vlan_edit_profile,          /*  vlan_edit_profile */
                            g_l2_basic_bridge.incoming_tag_format[step -1].tag_format,       /*  tag_format        */
                            TRUE                                                             /*  is_ive            */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {Delete,none}\n");
        return rv;
    }

    return rv;
}


/* Create EVE based on Incoming tag pkt and outgoing format required */
int set_eve_based_on_incoming_tag(int unit,int step){
    
     /*
      * Egress Vlan Edit
      * Tag After IVE = Dtag
      * Canonical Format is DTAG
      * Out Pkt is depending on the incoming pkt
      */
    int rv;
    int egress_outer_vid = new_outer_vid*2;
    int egress_inner_vid = new_inner_vid*2;

    /*
     * Step 7 Is to remove rx tag if tx_tag.vid=rx.vid and tx_tag_enble = 1
     * As TX_TAG_VID is configured as VSI by bcm_vlan_port_add, In EVE we will add VID=VSI so that TX_TAG.VID=OUTER_TAG.VID */
    if (step == 7) {
        egress_outer_vid = g_l2_basic_bridge.vsi;
    }

    if (step != 8 ) {
        /* For DTAG pkt tag actions is replace along with tpid */
        rv = vlan_translate_ive_eve_translation_set_with_tpid_action(
                                                        unit,
                                                        g_l2_basic_bridge.outlif,                                   /* outlif                  */
                                                        g_l2_basic_bridge.tpids.tpid4,                              /* outer_tpid              */
                                                        g_l2_basic_bridge.tpids.tpid5,                              /* inner_tpid              */
                                                        g_l2_basic_bridge.eve_action[step -1].outer_tpid_action,    /* Outer tpid action modify*/
                                                        g_l2_basic_bridge.eve_action[step -1].inner_tpid_action,    /* Inner tpid action modify*/
                                                        g_l2_basic_bridge.eve_action[step -1].outer_action,         /* outer_action            */
                                                        g_l2_basic_bridge.eve_action[step -1].inner_action,         /* inner_action            */
                                                        egress_outer_vid,                                           /* new_outer_vid           */
                                                        egress_inner_vid,                                           /* new_inner_vid           */
                                                        g_l2_basic_bridge.eve_action[step -1].vlan_edit_profile,    /* vlan_edit_profile       */
                                                        g_l2_basic_bridge.egress_tag_format[step -1].tag_format,    /* tag_format              */
                                                        FALSE                                                       /* is_ive                  */
                                                        );
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translate_ive_eve_translation_set_with_tpid_action(setting 0Port%d EVE action\n", g_l2_basic_bridge.out_port);
            return rv;
        }
    } else {
        /* For QoS to get updated in pkt we need out_pri_action hence seperate function called */
        rv = vlan_translate_ive_eve_translation_set_with_pri_action(
                                                        unit,
                                                        g_l2_basic_bridge.outlif,                                   /* outlif                  */
                                                        g_l2_basic_bridge.tpids.tpid4,                              /* outer_tpid              */
                                                        g_l2_basic_bridge.tpids.tpid5,                              /* inner_tpid              */
                                                        g_l2_basic_bridge.eve_action[step -1].outer_tpid_action,    /* Outer tpid action modify*/
                                                        g_l2_basic_bridge.eve_action[step -1].inner_tpid_action,    /* Inner tpid action modify*/
                                                        g_l2_basic_bridge.eve_action[step -1].outer_action,         /* outer_action            */
                                                        g_l2_basic_bridge.eve_action[step -1].inner_action,         /* inner_action            */
                                                        bcmVlanActionAdd,                                           /* outer_pri_action        */
                                                        bcmVlanActionNone,                                          /* inner_pri_action        */
                                                        egress_outer_vid,                                           /* new_outer_vid           */
                                                        egress_inner_vid,                                           /* new_inner_vid           */
                                                        g_l2_basic_bridge.eve_action[step -1].vlan_edit_profile,    /* vlan_edit_profile       */
                                                        g_l2_basic_bridge.egress_tag_format[step -1].tag_format,    /* tag_format              */
                                                        FALSE                                                       /* is_ive                  */
                                                        );
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translate_ive_eve_translation_set_with_pri_action(setting 0Port%d EVE action\n", g_l2_basic_bridge.out_port);
            return rv;
        }
    }

    return rv;

}

/*
 * Main function for QOS settings 
 * Create QOS ingress profile and set its mapping ETH VLAN fields to TC, DP (PHB QOS) 
 * Create QOS egress profile and set its mapping TC,DP to ETH VLAN fields
 */
int
qos_map_l2(int unit, int* ingress_qos_id, int* egress_qos_id, int* ingress_opcode_id, int* egress_opcode_id)
{
    int rv = BCM_E_NONE;    
    uint32 flags = 0;
    int opcode = -1;

    if (ingress_qos_id != illegal_qos_id)
    {
        flags = (BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK);
        if (*ingress_qos_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        rv = bcm_qos_map_create(unit, flags, ingress_qos_id);

        if (rv != BCM_E_NONE) {
            printf("error in L3 ingress bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
        } 

        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE ;
        if (*ingress_opcode_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
            opcode = *ingress_opcode_id;
        }
        rv = bcm_qos_map_create(unit, flags, &opcode);
        if (rv != BCM_E_NONE) {
            printf("error in ingress l3 opcode bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = qos_map_ingress_profile_map(unit, *ingress_qos_id, opcode, BCM_QOS_MAP_PHB | BCM_QOS_MAP_L2_OUTER_TAG);
        if (rv) {
            printf("error setting up qos_map_ingress_profile_map\n");
            return rv;
        }
        /* To configure nwk-qos
 */
        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
        if (*ingress_opcode_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
            opcode = *ingress_opcode_id;
        }
        rv = bcm_qos_map_create(unit, flags, &opcode);
        if (rv != BCM_E_NONE) {
            printf("error in ingress l3 opcode bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
        rv = qos_map_ingress_profile_map(unit, *ingress_qos_id, opcode, BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG);
        if (rv) {
            printf("error setting up qos_map_ingress_profile_map\n");
            return rv;
        }
    }

    if (*egress_qos_id != illegal_qos_id)
    {
        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
        if (*egress_qos_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        rv = bcm_qos_map_create(unit, flags, egress_qos_id);

        if (rv != BCM_E_NONE) {
            printf("error in L3 egress bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
        } 

        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
        if (*egress_opcode_id != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
            opcode = *egress_opcode_id;
        }
        rv = bcm_qos_map_create(unit, flags, &opcode);
        if (rv != BCM_E_NONE) {
            printf("error in egress l3 opcode bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = qos_map_egress_profile_map(unit, *egress_qos_id, opcode, BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG);
        if (rv) {
            printf("error setting up qos_map_l2_egress_profile\n");
            return rv;
        }
    }
    return rv;
}

/*
 * Configuration for Forwarding plus one header(Generally L3 header) remarking. 
 * In this Example, the Forwarding header is ETH, and Forwarding plus one header is IPv4.
 * The maps for remarking Qos is : remarking_qos = nwk_qos + 1 + DP.
 *
 * egress_remark_profile: egress remark profile, must be a valid profile.
 * opcode: opcode for plus one header QoS mapping. If -1, the example will create an opcode itself.
 * is_ipv6: If the confiiguration is for IPv6.
 */
int qos_map_l2_fwd_plus_one_remark(int unit, int egress_remark_profile, int opcode, int is_ipv6)
{
    int rv;
    int fwd_plus_1_opcode;
    int qos_value, dp_value;
    uint32 flags;
    bcm_qos_map_t eg_qos_map;

    /** Check if the given profile is valid*/
    if (egress_remark_profile == -1)
    {
        printf("error (%d) in qos_map_l2_fwd_plus_one_remark. Given profile (%d) is invalid!\n", BCM_E_PARAM, egress_remark_profile);
        return BCM_E_PARAM;
    }

    /** Create an opcode for fwd plus one remark.*/
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
    if (opcode != -1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
        fwd_plus_1_opcode = opcode;
    }

    rv = bcm_qos_map_create(unit, flags, &fwd_plus_1_opcode);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for fwd  plus one opcode\n", rv);
        return rv;
    }

    printf("Create an opcode for fwd plus one header remarking: 0x%08X\n", fwd_plus_1_opcode);

    /** Map given profile to the opcode */
    bcm_qos_map_t_init(&eg_qos_map);

    eg_qos_map.opcode = fwd_plus_1_opcode;
    flags = is_ipv6 ? BCM_QOS_MAP_IPV6 : BCM_QOS_MAP_IPV4;
    flags |= BCM_QOS_MAP_L3_L2 | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_add(unit, flags, &eg_qos_map, egress_remark_profile);
    if (rv != BCM_E_NONE)
    {
        printf("error (%d) in bcm_qos_map_add() for mapping profile to fwd plus one opcode\n", rv);
        return rv;
    }

    /** Add QoS map for the protocol: QoS = nwk_qos + 1 + dp*/
    for (qos_value = 0; qos_value < 256; qos_value++)
    {
        for (dp_value = 0; dp_value < 3; dp_value++)
        {
            bcm_qos_map_t_init(&eg_qos_map);

            flags = is_ipv6 ? BCM_QOS_MAP_IPV6 : BCM_QOS_MAP_IPV4;
            flags |= BCM_QOS_MAP_REMARK;
            eg_qos_map.color = dp_value;
            eg_qos_map.int_pri = qos_value;
            eg_qos_map.dscp = (qos_value + 1 + dp_value) & 0xFF;
            rv = bcm_qos_map_add(unit, flags, &eg_qos_map, fwd_plus_1_opcode);
            if (rv != BCM_E_NONE)
            {
                printf("error (%d) in bcm_qos_map_add() for mapping qos value with fwd plus one opcode\n", rv);
                return rv;
            }
        }
    }

    return rv;
}

/* Learning */
int l2_basic_bridge_learning(
        int unit){
    int rv;
    /* Enable ingress distributed learn*/
    /*rv = bcm_switch_control_set(unit, bcmSwitchL2LearnMode, BCM_L2_INGRESS_DIST);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }*/

    /* learn enable on port*/
    rv = bcm_port_learn_set(unit,g_l2_basic_bridge.in_port,BCM_PORT_LEARN_ARL);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_learn_set\n");
        return rv;
    }

    /* L2 entry for mac transplant*/
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, g_l2_basic_bridge.s_mac, g_l2_basic_bridge.vsi);
   
    l2_addr.flags = 0;
    l2_addr.port = g_l2_basic_bridge.out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add with source mac\n");
        return rv;
    }

    return rv;
}




