/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/* **************************************************************************************************************************************************
 *                                                                                                                                                  * 
 * This CINT demonstrates MACT L2 unicast IVL learning (Indenpendent VLAN Learning) and forwarding with IVE actions for differnt canonical formats  *
 * IVL MACT entry FWD and Learn lookup is using key in the form of {Vlan-tag; FID; MAC}, the Vlan-tag is Outer VLAN after IVE.                      *
 * So IVE is must for IVL feature.                                                                                                                  *
 * We cover scenario 802.1Q(user interface with canonical format untag/c-tag/s-tag) and 802.1AD (user interface with canonical format untag/c-tag   *
 * and network interface with canonical format s-tag/s+c tags)                                                                                      *
 *                                                                                                                                                  *
 * From JR2 onwards, the ctag and stag packet formats are distinguished according to the SDK                                                        *
 * initial TPID configuration: 0x8100 for ctag and 0x9100 for stag.                                                                                 *
 *                                                                                                                                                  *
 * Ports :                                                                                                                                          *
 * In_port  : port_1                                                                                                                                *
 * Out_port : port_2                                                                                                                                *
 *                                                                                                                                                  *
 *             +-------+                                                                                                                            *
 *    iPort    |       |      +-----+      +------+                                                                                                 *
 *    -------> | InLIF +----> | IVE +----> |      |                                                                                                 *
 *             |       |      +-----+      |      |     +-------+                                                                                   *
 *             +-------+                   |      |     |       |                                                                                   *
 *                                         | VSI  +---> | MACT  +-------------+                                                                     *
 *                                         |      |     |       |             |                                                                     *
 *                                         |      |     +-------+             |                                                                     *
 *                                         |      |                           |                                                                     *
 *                                         +------+                           |                                                                     *
 *                                                                            |                                                                     *
 *                                                                            |                                                                     *
 *                                  +---------+                               |                                                                     *
 *     oPort                        |         |                               |                                                                     *
 *    <-----------------------------+ Out LIF +<------------------------------+                                                                     *
 *                                  |         |                                                                                                     *
 *                                  +---------+                                                                                                     *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * TAG ACTIONS are applied as follows                                                                                                               *
 *         ----------------------------------------------------------------------------- --------------------- -----------------------------        *
 *         | STEP  | incoming pkt format |  Canonical format      |       IVE           |   inlif criteria     | Test plan Scenario         |       *
 *         ----------------------------------------------------------------------------------------------------------------------------------       *
 *         |   1   |    untag            |      CTAG              |  {Add,None}         |   match port         | 802.1Q UNI untag           |       *
 *         ----------------------------------------------------------------------------------------------------------------------------------       *
 *         |   2   |     ctag            |      CTAG              |  {None,None}        |   match port         | 802.1Q UNI ctag(1:1)       |       *
 *         ----------------------------------------------------------------------------------------------------------------------------------       *
 *         |   3   |     ctag            |      CTAG              |  {None,None}        |  ISEM {VDxCVLAN}     | 802.1Q UNI ctag(m:1)       |       *
 *         ----------------------------------------------------------------------------------------------------------------------------------       *
 *         |   4   |     stag            |      STAG              |  {None,None}        |  match port          | 802.1Q UNI/1AD NNI(1:1)    |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
  *        |   5   |     stag            |      STAG              |  {None,None}        |  ISEM {VDxSVLAN}     | 802.1Q UNI/1AD NNI(m:1)    |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
 *         |   6   |    untag            |      STAG              |   {Add,None}        |  match port          | 802.1AD UNI untag          |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
 *         |   7   |     ctag            |      S + C TAGS        |  {Delete,None}      |  match port          | 802.1AD UNI ctag           |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
 *         |   8   |     S + C TAGS      |      CTAG              |  {Delete,None}      |  match port          | 802.1AD NNI S + C tags     |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
 *         |   9   |    Native ctag      |      Native CTAG       |  {None,None}        |  match PWE           | VPLS NNI  Native ctag      |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
 *         |   10  |  Native S + C TAGS  |      Native CTAG       |  {Delete,None}      |  match PWE           | VPLS NNI  Native S + C     |       *
 *         ----------------------------------------------------------------------------- --------------------- ------------------------------       *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 *                                                                                                                                                  *
 * How to run:
 * scenario 802.1Q(user interface with canonical format untag/c-tag/s-tag):
 *
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/dnx/cint_dnx_mact_ivl.c
 * cint
 *     ivl_example(0, 200, 201, 100, 0x444, 0x445);
 *     exit;
 *
 *     debug bcm rx +
 *  untag:
 *     tx 1 psrc=201 data=0x0000000000010000000000020899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *     tx 1 psrc=200 data=0x0000000000020000000000010899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  ctag:
 *     tx 1 psrc=201 data=0x000000000001000000000002810004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *     tx 1 psrc=200 data=0x000000000002000000000001810004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  stag:
 *     tx 1 psrc=201 data=0x000000000001000000000002910004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *     tx 1 psrc=200 data=0x000000000002000000000001910004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Expect that the 2nd packet will be forwarded to CPU port 201 by unicast.
 *
 * scenario 802.1AD(user interface with canonical format untag/c-tag, network interface with canonical format s-tag/s+c tags):
 *
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/dnx/cint_dnx_mact_ivl.c
 * cint
 *     print ivl_example_802_1ad(0, 200, 201, 100, 0x444, 0x445);
 *     exit;
 *
 *     debug bcm rx +
 *  untag:
 *     tx 1 psrc=200 data=0x0000000000020000000000010899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  ctag:
 *     tx 1 psrc=200 data=0x000000000002000000000001810004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  stag:
 *     tx 1 psrc=201 data=0x000000000001000000000002910004450899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  S+C tags:
 *     tx 1 psrc=201 data=0x00000000000100000000000291000445810004440899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 */


struct ivl_info_t
{
    bcm_vlan_t vsi;
    bcm_vlan_t s_vid;
    bcm_vlan_t c_vid;
    int port_1;
    int port_2;
    bcm_gport_t in_lif[4];
};

ivl_info_t ivl_info;

int tag_format_untag = 0;
int tag_format_stag = 4;
int tag_format_ctag = 8;
int tag_format_dtag = 16;

/* Default global tpid configuration in appl_dnx_global_tpid_init() */
int tpid_value_ctag = 0x8100;
int tpid_value_stag = 0x9100;

/* vlan edit profiles */
int ing_vlan_edit_profile_uni = 1;
int ing_vlan_edit_profile_nni_stag = 2;
int ing_vlan_edit_profile_nni_dtag = 3;

bcm_mac_t mac_1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}; /* SA */
bcm_mac_t mac_2 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}; /* DA */

/*
 * Add L2 MACT IVL entry. The lookup will be:
 *     {VID, VSI, MAC} => dest_port
 */
int ivl_addr_add(int unit, bcm_mac_t mac, bcm_vlan_t vsi, bcm_vlan_t vid, int dest_port)
{
    bcm_l2_addr_t addr;
    bcm_l2_addr_t_init(&addr, mac, vsi);
    addr.flags |= BCM_L2_STATIC;
    addr.modid = vid;
    addr.port = dest_port;

    return bcm_l2_addr_add(unit, &addr);
}

int ivl_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vsi, bcm_vlan_t vid)
{
    bcm_l2_addr_t addr;
    bcm_l2_addr_t_init(&addr, mac, vsi);
    addr.modid = vid;

    return bcm_l2_addr_by_struct_delete(unit, &addr);
}

/*
 * IVL example:
 *     Create two LIFs {port_1, vid}, {port_2, vid};
 *     Add the two LIFs to vsi;
 *     Enable IVl on vsi;
 */
int ivl_example(int unit, int port_1, int port_2, bcm_vlan_t vsi, bcm_vlan_t c_vid, bcm_vlan_t s_vid)
{
    bcm_vlan_port_t vlan_port;
    bcm_vlan_control_vlan_t control;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int rv = BCM_E_NONE;
    bcm_port_match_info_t match_info;
    int ivl_feature_support = *dnxc_data_get(unit, "l2", "general", "ivl_feature_support", NULL);

    sal_memset(&ivl_info, 0, sizeof(ivl_info));

    ivl_info.vsi = vsi;
    ivl_info.c_vid = c_vid;
    ivl_info.s_vid = s_vid;
    ivl_info.port_1 = port_1;
    ivl_info.port_2 = port_2;

    rv = bcm_port_untagged_vlan_set(unit, port_1, c_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set(port_1=%d, vlan=%d)\n", port_1, c_vid);
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, port_2, c_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set(port_2=%d, vlan=%d)\n", port_2, c_vid);
        return rv;
    }

    rv = bcm_vlan_create(unit, c_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vlan=%d)\n", c_vid);
        return rv;
    }

    rv = bcm_vlan_create(unit, s_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vlan=%d)\n", s_vid);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);

    rv = bcm_vlan_port_add(unit, c_vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add(vlan=%d)\n", c_vid);
        return rv;
    }

    rv = bcm_vlan_port_add(unit, s_vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add(vlan=%d)\n", s_vid);
        return rv;
    }

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vis=%d)\n", vsi);
        return rv;
    }

    /* create in_lif 0 for 1:1 un-tag/c-tag/s-tag forwarding */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = port_1;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", vlan_port.vlan_port_id);
        return rv;
    }

    ivl_info.in_lif[0] = vlan_port.vlan_port_id;

    /* create in_lif 1 for 1:1 un-tag/c-tag/s-tag learning */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = port_2;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", vlan_port.vlan_port_id);
        return rv;
    }

    ivl_info.in_lif[1] = vlan_port.vlan_port_id;

    /* create in_lif 2 for m:1 s-tag forwarding */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_1;
    vlan_port.match_vlan = c_vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }
    ivl_info.in_lif[2] = vlan_port.vlan_port_id;

    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.match = BCM_PORT_MATCH_PORT_VLAN;
    match_info.port = port_1;
    match_info.match_vlan = s_vid;
    rv = bcm_port_match_add(unit, ivl_info.in_lif[2], &match_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_match_add(port = %d)\n", ivl_info.in_lif[2]);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, ivl_info.in_lif[2]);
    if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vswitch_port_add(port = %d)\n", ivl_info.in_lif[2]);
            return rv;
    }

    /* create in_lif 3 for m:1 s-tag learning */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_2;
    vlan_port.match_vlan = c_vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }
    ivl_info.in_lif[3] = vlan_port.vlan_port_id;

    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    match_info.match = BCM_PORT_MATCH_PORT_VLAN;
    match_info.port = port_2;
    match_info.match_vlan = s_vid;
    rv = bcm_port_match_add(unit, ivl_info.in_lif[3], &match_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_match_add(port = %d)\n", ivl_info.in_lif[3]);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, ivl_info.in_lif[3]);
    if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vswitch_port_add(port = %d)\n", ivl_info.in_lif[3]);
            return rv;
    }

    /* IVL full functionality is not support in JR2 A0 */
    if (ivl_feature_support == 1)
    {
        /* Enable IVL on vsi */
        rv = bcm_vlan_control_vlan_get (unit, vsi, &control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_get");
            return rv;
        }

        control.flags2 |= BCM_VLAN_FLAGS2_IVL;

        rv = bcm_vlan_control_vlan_set (unit, vsi, control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_set");
            return rv;
        }
    }

    /*
         * Incoming is 1:1 un-tag/c-tag/s-tag  in in_lif 0 for forwarding
         *IVE ={bcmVlanActionAdd,None}
         */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            ivl_info.in_lif[0],                  /* lif */
                            tpid_value_ctag,                     /* outer_tpid */
                            0,                                   /* inner_tpid */
                            bcmVlanActionAdd,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            c_vid,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_untag,                    /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {bcmVlanActionAdd,None}\n");
        return rv;
    }

    /*
         * Incoming is 1:1 un-tag/c-tag/s-tag  in in_lif 1 for 1:1 learning
         *IVE ={bcmVlanActionAdd,None}
         */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            ivl_info.in_lif[1],                  /* lif */
                            tpid_value_ctag,                     /* outer_tpid */
                            0,                                   /* inner_tpid */
                            bcmVlanActionAdd,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            c_vid,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_untag,                    /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {bcmVlanActionAdd,None}\n");
        return rv;
    }

    return rv;
}

/*
 * IVL example:
 *     Create two LIFs {port_1, vid}, {port_2, vid};
 *     Add the two LIFs to vsi;
 *     Enable IVl on vsi;
 */
int ivl_example_802_1ad(int unit, int port_1, int port_2, bcm_vlan_t vsi, bcm_vlan_t c_vid, bcm_vlan_t s_vid)
{
    bcm_vlan_port_t vlan_port;
    bcm_vlan_control_vlan_t control;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    int rv = BCM_E_NONE;
    int ivl_feature_support = *dnxc_data_get(unit, "l2", "general", "ivl_feature_support", NULL);

    sal_memset(&ivl_info, 0, sizeof(ivl_info));

    ivl_info.vsi = vsi;
    ivl_info.s_vid = s_vid;
    ivl_info.c_vid = c_vid;
    ivl_info.port_1 = port_1;
    ivl_info.port_2 = port_2;

    rv = bcm_vlan_create(unit, s_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vlan=%d)\n", s_vid);
        return rv;
    }

    rv = bcm_vlan_create(unit, c_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vlan=%d)\n", c_vid);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);
    rv = bcm_vlan_port_add(unit, s_vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add(vlan=%d)\n", s_vid);
        return rv;
    }

    rv = bcm_vlan_port_add(unit, c_vid, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add(vlan=%d)\n", c_vid);
        return rv;
    }

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create(vis=%d)\n", vsi);
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = port_1;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", vlan_port.vlan_port_id);
        return rv;
    }

    ivl_info.in_lif[0] = vlan_port.vlan_port_id;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.port = port_2;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", vlan_port.vlan_port_id);
        return rv;
    }

    ivl_info.in_lif[1] = vlan_port.vlan_port_id;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port.match_vlan = s_vid;
    vlan_port.match_inner_vlan = c_vid;
    vlan_port.port = port_2;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create(port = %d)\n", vlan_port.port);
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add(port = %d)\n", vlan_port.vlan_port_id);
        return rv;
    }

    ivl_info.in_lif[2] = vlan_port.vlan_port_id;

    /* IVL full functionality is not support in JR2 A0 */
    if (ivl_feature_support == 1)
    {
        rv = bcm_vlan_control_vlan_get (unit, vsi, &control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_get");
            return rv;
        }

        control.flags2 |= BCM_VLAN_FLAGS2_IVL;

        rv = bcm_vlan_control_vlan_set (unit, vsi, control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_set");
            return rv;
        }
    }

       /*
             * Incoming is Un-TAG
             *IVE ={bcmVlanActionAdd,None}
             */
        rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            ivl_info.in_lif[0],                  /* lif */
                            tpid_value_stag,                     /* outer_tpid */
                            0,                                   /* inner_tpid */
                            bcmVlanActionAdd,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            s_vid,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_untag,                    /* tag_format */
                            TRUE                                 /* is_ive */
                            );

        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {bcmVlanActionAdd,None}\n");
            return rv;
        }

    /*
         * Incoming is CTAG
         * IVE ={bcmVlanActionOuterAdd,None}
         */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            ivl_info.in_lif[0],                  /* lif - need the gport, thus use inLifList instead of outLifList */
                            tpid_value_stag,                     /* outer_tpid */
                            tpid_value_ctag,                     /* inner_tpid */
                            bcmVlanActionAdd,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            s_vid,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_ctag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {None,None}\n");
        return rv;
    }

    /* Incoming is DTAG
         *  IVE ={bcmVlanActionDelete,None}
         */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            ivl_info.in_lif[2],                  /* lif */
                            tpid_value_stag,                     /* outer_tpid */
                            tpid_value_ctag,                     /* inner_tpid */
                            bcmVlanActionDelete,                 /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            c_vid,                               /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_nni_dtag,      /* vlan_edit_profile */
                            tag_format_dtag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {bcmVlanActionDelete,None}\n");
        return rv;
    }

    /* L2 IVL mac for UNI untag and ctag packet to NNI */
    rv = ivl_addr_add(unit, mac_2, vsi, s_vid, port_2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in ivl_addr_add(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    /* L2 IVL mac for NNI S + C tags packet to UNI */
    rv = ivl_addr_add(unit, mac_1, vsi, c_vid, port_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in ivl_addr_add(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    return rv;
}


/*
 * IVL example for PW NNI RAW mode:
 * cint src/examples/sand/utility/cint_sand_utils_global.c.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_vpls_mp_basic.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/dnx/cint_dnx_mact_ivl.c
 * cint
 *     print vpls_mp_basic_main(0, 200, 201);
  *    print ivl_example_pwe_raw_mode_ive(0, 0x444);
 *     exit;
 */
int ivl_example_pwe_raw_mode_ive(int unit, bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t control;
    bcm_gport_t mpls_port_id = cint_vpls_basic_info.mpls_port_id_ingress;
    bcm_vlan_t vsi = cint_vpls_basic_info.vpn;
    bcm_mac_t ac_mac_address = cint_vpls_basic_info.ac_mac_address;
    int dest_port = cint_vpls_basic_info.vlan_port_id;
    int ivl_feature_support = *dnxc_data_get(unit, "l2", "general", "ivl_feature_support", NULL);

    /* IVL full functionality is not support in JR2 A0 */
    if (ivl_feature_support == 1)
    {
        /* Enable IVL on vsi */
        rv = bcm_vlan_control_vlan_get (unit, vsi, &control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_get");
            return rv;
        }

        control.flags2 |= BCM_VLAN_FLAGS2_IVL;

        rv = bcm_vlan_control_vlan_set (unit, vsi, control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_set");
            return rv;
        }
    }

    /*
         * add port.
         */
    bcm_vlan_port_t vlan_port;
    bcm_gport_t native_ac;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.vsi = vsi;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /*
         * Save the created vlan_port_id
         */
    native_ac = vlan_port.vlan_port_id;
    printf("native ac = 0x%08x\n", native_ac);

    /*
         * Add Match
         */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = mpls_port_id;
    match_info.match = BCM_PORT_MATCH_PORT;        /* Match un-tag */
    rv = bcm_port_match_add(unit, native_ac, &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_port_match_add\n", rv);
        return rv;
    }
    printf("match_port done\n");

    /*
         * Incoming is CTAG
         * IVE ={None,None}
         */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            native_ac,                           /* lif */
                            tpid_value_stag,                     /* outer_tpid */
                            tpid_value_ctag,                     /* inner_tpid */
                            bcmVlanActionNone,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            vid,                                 /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_ctag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {None,None}\n");
        return rv;
    }

    /* L2 IVL mac for NNI to UNI */
    rv = ivl_addr_add(unit, ac_mac_address, vsi, vid, dest_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}


/*
 * IVL example for PW NNI RAW mode:
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/dnx/vpls/cint_vpls_pwe_tagged_mode_basic.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/dnx/cint_dnx_mact_ivl.c
 * cint
 *     print vpls_pwe_tagged_mode_basic_main(0, 200, 201);
  *    print ivl_example_pwe_tagged_mode_ive(0, 0xa);
 *     exit;
 */
int ivl_example_pwe_tagged_mode_ive(int unit, bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t control;
    bcm_vlan_t vsi = cint_vpls_pwe_tagged_mode_basic_info.service_vsi;
    bcm_mac_t ac_mac_address = cint_vpls_pwe_tagged_mode_basic_info.service_mac_1;
    int dest_port = cint_vpls_pwe_tagged_mode_basic_info.access_port;
    bcm_gport_t native_ac = cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id;
    int ivl_feature_support = *dnxc_data_get(unit, "l2", "general", "ivl_feature_support", NULL);

    /* IVL full functionality is not support in JR2 A0 */
    if (ivl_feature_support == 1)
    {
        /* Enable IVL on vsi */
        rv = bcm_vlan_control_vlan_get (unit, vsi, &control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_get");
            return rv;
        }

        control.flags2 |= BCM_VLAN_FLAGS2_IVL;

        rv = bcm_vlan_control_vlan_set (unit, vsi, control);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_control_vlan_set");
            return rv;
        }
    }

    /*
         * Incoming is S + C TAGs
         * IVE ={Delete,None}
         */
    rv = vlan_translate_ive_eve_translation_set(
                            unit,
                            native_ac,                           /* lif */
                            tpid_value_stag,                     /* outer_tpid */
                            tpid_value_ctag,                     /* inner_tpid */
                            bcmVlanActionDelete,                    /* outer_action */
                            bcmVlanActionNone,                   /* inner_action */
                            vid,                                 /* new_outer_vid*/
                            0,                                   /* new_inner_vid*/
                            ing_vlan_edit_profile_uni,           /* vlan_edit_profile */
                            tag_format_dtag,                     /* tag_format */
                            TRUE                                 /* is_ive */
                            );

    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translate_ive_eve_translation_set(setting IVE to {None,None}\n");
        return rv;
    }

    /* L2 IVL mac for NNI to UNI */
    rv = ivl_addr_add(unit, ac_mac_address, vsi, vid, dest_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}


/*
 * Clean up procedure  
 *  use_case =1 for ivl_example
 *  use_case =2 for ivl_example_802_1ad
 */
int ivl_clean_up(int unit, int use_case)
{
    int ii = 0;
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    bcm_port_match_info_t match_info;

    if (use_case == 2)
    {
        /* Delete L2 IVL mac for UNI untag and ctag packet to NNI */
        rv = ivl_addr_delete(unit, mac_2, ivl_info.vsi, ivl_info.s_vid);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in ivl_addr_add(BCM_VLAN_ACTION_SET_INGRESS)\n");
            return rv;
        }

        /* Delete L2 IVL mac for NNI S + C tags packet to UNI */
        rv = ivl_addr_delete(unit, mac_1, ivl_info.vsi, ivl_info.c_vid);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in ivl_addr_add(BCM_VLAN_ACTION_SET_INGRESS)\n");
            return rv;
        }
    }

    rv = bcm_port_untagged_vlan_set(unit, ivl_info.port_1, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set(port_1=%d, vlan=1)\n", ivl_info.port_1);
        return rv;
    }

    rv = bcm_port_untagged_vlan_set(unit, ivl_info.port_2, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_untagged_vlan_set(port_2=%d, vlan=1)\n", ivl_info.port_2);
        return rv;
    }

    /*
     * IVE actions:
     */
    rv = bcm_vlan_translate_action_id_destroy_all(unit, BCM_VLAN_ACTION_SET_INGRESS);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_vlan_translate_action_id_destroy_all(BCM_VLAN_ACTION_SET_INGRESS)\n");
        return rv;
    }

    if (use_case == 1)
    {
        bcm_port_match_info_t_init(&match_info);
        match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
        match_info.match = BCM_PORT_MATCH_PORT_VLAN;
        match_info.port = ivl_info.port_1;
        match_info.match_vlan = ivl_info.s_vid;
        rv = bcm_port_match_delete(unit, ivl_info.in_lif[2], &match_info);
        if (rv != BCM_E_NONE)
        {
           printf("Error, in bcm_port_match_delete(lif = %d)\n", ivl_info.in_lif[2]);
           return rv;
        }

        bcm_port_match_info_t_init(&match_info);
        match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
        match_info.match = BCM_PORT_MATCH_PORT_VLAN;
        match_info.port = ivl_info.port_2;
        match_info.match_vlan = ivl_info.s_vid;
        rv = bcm_port_match_delete(unit, ivl_info.in_lif[3], &match_info);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_match_delete(lif = %d)\n", ivl_info.in_lif[3]);
            return rv;
        }
    }

    for (ii = 0; ii < sizeof(ivl_info.in_lif)/sizeof(ivl_info.in_lif[0]); ++ii)
    {
        if (ivl_info.in_lif[ii] != 0)
        {
            rv = bcm_vswitch_port_delete(unit, ivl_info.vsi, ivl_info.in_lif[ii]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vswitch_port_delete(port = %d)\n", ivl_info.in_lif[ii]);
                return rv;
            }
            rv = bcm_vlan_port_destroy(unit, ivl_info.in_lif[ii]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_vlan_port_destroy(lif = %d)\n", ivl_info.in_lif[ii]);
                return rv;
            }
        }
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, ivl_info.port_1);
    BCM_PBMP_PORT_ADD(pbmp, ivl_info.port_2);
    rv = bcm_vlan_port_remove(unit, ivl_info.c_vid, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_remove(vlan=%d)\n", ivl_info.c_vid);
        return rv;
    }

    rv = bcm_vlan_port_remove(unit, ivl_info.s_vid, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_remove(vlan=%d)\n", ivl_info.s_vid);
        return rv;
    }

    rv = bcm_vlan_destroy(unit, ivl_info.s_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy(vlan=%d)\n", ivl_info.s_vid);
        return rv;
    }
    bcm_vlan_destroy(unit, ivl_info.c_vid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy(vlan=%d)\n", ivl_info.c_vid);
        return rv;
    }
    bcm_vlan_destroy(unit, ivl_info.vsi);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_destroy(vlan=%d)\n", ivl_info.vsi);
        return rv;
    }

    return BCM_E_NONE;
}

