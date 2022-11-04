/* $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_tsn.c
 * Purpose: iPMF for TSN frames
 *
 * IRPP sends the streams frames's stream-filter in the TM-command
 * - admit-profile (3b)
 * - meter-command (23b)in TM-CMD/SO0: SO0-id (20b), SO0-type (2b), SO0-valid (1b); SO0-id is the stream's meter-id
 * - stream info in TM-CMD/SO3: gate-mode (2b, SO3-id[1-0]), gate-id (8b, SO3-id[9-2]), max-frame-size (8b, SO3-id[17-10]); SO3-valid is clear; SO3-type is don't care
 * - counter-command (23b)in TM-CMD/SO4: SO4-id (20b), SO4-type (2b), SO4-valid (1b). SO4-id is the stream's counter-id; SO4-type is the "stream-type"
 *
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  cint ../../../../src/examples/sand/cint_bridge_application.c
  cint ../../../../src/examples/dnx/field/cint_field_presel_fwd_layer.c
  cint ../../../../src/examples/dnx/field/cint_field_presel_tsn.c

  cint;
  int unit = 0;
  int in_port = 200;
  int out_port = 201;

  //bridge_application_init(unit, in_port, out_port);
  //bridge_application_run(unit);
  bcm_port_force_forward_set(unit, in_port, out_port, 1);

  bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
  bcm_field_layer_type_t fwd_layer = bcmFieldLayerTypeEth;
  field_presel_fwd_layer_main(unit, cint_field_tsn_presel_id, stage, fwd_layer, &cint_field_tsn_context_id, "ctx_example");

  // config cint_field_tsn_field_info as needed
  cint_field_tsn_main(unit);
  exit;
  Tx 1 psrc=200 data=0x0000000000020000000000038100000188F70002002c000200000000000000000000000000030000000000fffe0000660001fc00000000000000000000fa
 * Test Scenario - end
 *
 * Configuration example end
 *
 */

bcm_field_presel_t cint_field_tsn_presel_id = 0;
bcm_field_context_t cint_field_tsn_context_id = 0;
bcm_field_group_t cint_field_tsn_fg_id = 0;
bcm_field_entry_t cint_field_tsn_ent_id;
bcm_field_qualify_t cint_field_tsn_id;

/*
 * Structure of Stream-filter
 */
struct cint_field_tsn_stream_field_info_s {
    /*
     * dest mac
     */
    bcm_mac_t dmac;

    /*
     * source mac
     */
    bcm_mac_t smac;
    /*
     * Gate-mode is the stream's gate mode
     * Gate-mode values are: 0 - gate closed, 1 - gate open, 2 - time gate
     */
    int gate_mode;
    /*
     * Gate-id is the stream's gate id
     */
    int gate_id;
    /*
     * Max-frame-size is the maximum frame size in 64B granularity
     */
    int max_frame_size;
    int meter_id;
    int counter_id;
    int admit_profile;
};

cint_field_tsn_stream_field_info_s cint_field_tsn_field_info =
{
    /*
     * dmac
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
    /*
     * smac
     */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x03},
    /*
     * gate_mode
     */
    1,
    /*
     * gate_id
     */
    0xff,
    /*
     * max_frame_size
     */
    0xff,
    /*
     * meter_id
     */
    0xfffff,
    /*
     * counter_id
     */
    0xfffff,
    /*
     * admit_profile
     */
    0x7
};


/**
* \brief
*  Configure IPMF1.
*  - Qual: Header[Current SA](48), Header[Current DA](48)
*  - Actions: {Admission_Profile(4), Stat_Object[0], Stat_Object[3],  Stat_Object[4]}
* \param [in] unit -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_tsn_main(int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    int so3_id = 0;
    void *dest_char;
    uint32 dmac_32lsb = 0;
    uint32 smac_32lsb = 0;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifySrcMac;
    fg_info.qual_types[1] = bcmFieldQualifyDstMac;
    fg_info.nof_actions = 7;
    fg_info.action_types[0] = bcmFieldActionAdmitProfile;
    fg_info.action_types[1] = bcmFieldActionStatId0;
    fg_info.action_types[2] = bcmFieldActionStatId3;
    fg_info.action_types[3] = bcmFieldActionStatId4;
    fg_info.action_types[4] = bcmFieldActionStatProfile0;
    fg_info.action_types[5] = bcmFieldActionStatProfile3;
    fg_info.action_types[6] = bcmFieldActionStatProfile4;

    printf("Creating IPMF1 FG\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &cint_field_tsn_fg_id), "");

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    attach_info.payload_info.action_types[3] = fg_info.action_types[3];
    attach_info.payload_info.action_types[4] = fg_info.action_types[4];
    attach_info.payload_info.action_types[5] = fg_info.action_types[5];
    attach_info.payload_info.action_types[6] = fg_info.action_types[6];

    printf("Attaching IPMF1 FG\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, cint_field_tsn_fg_id, cint_field_tsn_context_id, &attach_info), "");
    printf ("TCAM FG_ID %d is attached to Context %d at stage iPMF1\n", cint_field_tsn_fg_id, cint_field_tsn_context_id);

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    ent_info.nof_entry_quals = fg_info.nof_quals;
    /*
     * smac 32lsb
     */
    smac_32lsb  = cint_field_tsn_field_info.smac[5];
    smac_32lsb |= cint_field_tsn_field_info.smac[4] << 8;
    smac_32lsb |= cint_field_tsn_field_info.smac[3] << 16;
    smac_32lsb |= cint_field_tsn_field_info.smac[2] << 24;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = smac_32lsb;
    ent_info.entry_qual[0].mask[0] = 0xFFFFFFFF;
    /*
     * dmac 32lsb
     */
    dmac_32lsb  = cint_field_tsn_field_info.dmac[5];
    dmac_32lsb |= cint_field_tsn_field_info.dmac[4] << 8;
    dmac_32lsb |= cint_field_tsn_field_info.dmac[3] << 16;
    dmac_32lsb |= cint_field_tsn_field_info.dmac[2] << 24;
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = dmac_32lsb;
    ent_info.entry_qual[1].mask[0] = 0xFFFFFFFF;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    /*
     * admit-profile (3b)
     */
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = cint_field_tsn_field_info.admit_profile & 0x7;
    /*
     * meter-command (23b): SO0-id (20b), SO0-type (2b), SO0-valid (1b); SO0-id is the stream's meter-id
     */
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = cint_field_tsn_field_info.meter_id & 0xfffff;
    /*
     * SO3-id includes the stream info: gate-mode (2b, SO3-id[1-0]), gate-id (8b, SO3-id[9-2]), max-frame-size (8b, SO3-id[17-10])
     */
    ent_info.entry_action[2].type = fg_info.action_types[2];
    so3_id |= (cint_field_tsn_field_info.max_frame_size & 0xff) << 10;
    so3_id |= (cint_field_tsn_field_info.gate_id & 0xff) << 2;
    so3_id |= cint_field_tsn_field_info.gate_mode & 0x3;
    ent_info.entry_action[2].value[0] = so3_id;
    /*
     * counter-command (23b): SO4-id (20b), SO4-type (2b), SO4-valid (1b). SO4-id is the stream's counter-id; SO4-type is the "stream-type"
     */
    ent_info.entry_action[3].type = fg_info.action_types[3];
    ent_info.entry_action[3].value[0] = cint_field_tsn_field_info.counter_id & 0xfffff;

    /*
     * valid
     */
    ent_info.entry_action[4].type = fg_info.action_types[4];
    ent_info.entry_action[4].value[3] = 1;
    ent_info.entry_action[5].type = fg_info.action_types[5];
    ent_info.entry_action[5].value[3] = 1;
    ent_info.entry_action[6].type = fg_info.action_types[6];
    ent_info.entry_action[6].value[3] = 1;
    /*
     * SO4-type is the "stream-type"
     */
    /* ent_info.entry_action[6].value[2] = "stream-type"; */

    printf("Adding Entry to the IPMF1 FG\n");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, cint_field_tsn_fg_id, &ent_info, &cint_field_tsn_ent_id), "");
    printf("Entry add: id:(0x%x)\n", cint_field_tsn_ent_id);
    printf("Qual_val[0]:(0x%x) Qual_val[1]:(0x%x)\n", ent_info.entry_qual[0].value[0],
                                                  ent_info.entry_qual[1].value[0]);
    printf("Act_val[0]:(0x%x) Act_val[1]:(0x%x) Act_val[2]:(0x%x) Act_val[3]:(0x%x)\n", ent_info.entry_action[0].value[0],
                                                                                  ent_info.entry_action[1].value[0],
                                                                                  ent_info.entry_action[2].value[0],
                                                                                  ent_info.entry_action[3].value[0]);
    return BCM_E_NONE;
}

/**
* \brief
*  Destroy all configuration done for IPMF1 in cint_field_tsn_main.
* \param [in] unit -  Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_tsn_destroy(int unit)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, cint_field_tsn_fg_id, NULL, cint_field_tsn_ent_id), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, cint_field_tsn_fg_id, cint_field_tsn_context_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, cint_field_tsn_fg_id), "");
    return BCM_E_NONE;
}
