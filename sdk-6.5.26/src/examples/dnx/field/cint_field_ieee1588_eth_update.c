/*
 *File: cint_field_ieee1588_eth_update.c
 * Purpose: An example of update ETH of PTPoE traffic. Traffic as below:
 *
 * 1. PTPoETH Flow from North to South
 *
 *  DMAC(DMAC-JR2-A) SMAC(SMAC-FPGA) Type=0x8100 Vlan-X Type=0x88F7 PTP
 *                              |
 *                             \|/
 *                            NIF-T
 *                          ---------
 *                          |  JR2   |
 *                          ---------
 *                            NIF-1
 *                              |
 *                             \|/
 *  DMAC(Rsvd Mcast Mac) SMAC(NIF-1 Mac) Type=0x8100 Vlan-Y Type=0x88F7 PTP
 *
 * 2. PTPoETH Flow from South to North
 *
 *  DMAC(FPGA Mac) SMAC(NIF-T Mac) Type=0x8100 Vlan-X Type=0x88F7 PTP
 *                             /|\
 *                              |
 *                            NIF-T
 *                          ---------
 *                          |  JR2   |
 *                          ---------
 *                            NIF-1
 *                             /|\
 *                              |
 *  DMAC(Rsvd Mcast Mac) SMAC(peer Mac) Type=0x8100 Vlan-Y Type=0x88F7 PTP
 */

/*
 * Example of updating ETH of PPPoE traffic, South to North
 *
 * Configuration example start:
 * 1. Add required SOC property
 *      setenv SOC_BOOT_FLAGS 0x1000
 *      ./bcm.user
 *      config add phy_1588_dpll_frequency_lock.0=1
 *      rcload rc.soc;
 * 2. Enable one-step 1588 on NIF-1 and NIF-T and config PMF qualifies and actions
       cint ../../../../src/examples/dnx/ptp/cint_ptp_1588.c
       cint ../../../../src/examples/dnx/field/cint_field_ieee1588_eth_update.c
       cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
       cint ../../../../src/examples/sand/cint_ip_route_basic.c
       cint;
       int unit = 0;
       int nif_1_port = 13;
       int nif_t_port = 14;
       bcm_field_context_t context_id = 0;
       ieee_1588_port_set(unit, nif_1_port, 1);
       ieee_1588_port_set(unit, nif_t_port, 1);
       cint_field_ieee1588_eth_update_main(unit, context_id, nif_1_port, nif_t_port);
 * 3. Inject PTPoE traffic like:
 *     tx 1 psrc=13 data=0x0180c200000e0000000000038100000a88F70002002c000200000000000000000000000000030000000000fffe0000660001fc00000000000000000000fa
 *     Note: Please use IXIA to inject the same traffic in order to check CF.
 * 4. The received traffic is expected to be with new DA/SA/VID
 *     DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
 *     SA = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
 *     VID = 0x64
 *     EthType = 0x88F7
 Configuration example end
 *
 */

int vlan_x = 0x64;
int vlan_y = 0xa;
bcm_mac_t nif_t_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
bcm_mac_t fgpa_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
int fec_id_enc = 0xCDDE;

bcm_field_group_t cint_ieee1588_eth_update_fg_id;
bcm_field_entry_t cint_ieee1588_eth_update_ent_id;

/*
 * Function:
 *      cint_field_ieee1588_eth_update_main
 * Purpose:
 *      Forward PPPoETH and update ETH header with specified DA/SA/VID, update CF
 *      Using qualifiers
 *          bcmFieldQualifyInPort
 *          bcmFieldQualifyDstMac
 *          bcmFieldQualifyEtherType
 *      if match perform:
 *          bcmFieldActionL3Switch
 *          bcmFieldActionForwardingLayerIndex
 *          bcmFieldActionEgressForwardingIndex
 * Parameters:
 *      unit       - Device id
 *      context_id - PMF context Id
 *      nif_1_port - External port
 *      nif_t_port - Port connecting to Timing-FPGA
 * Returns:
 *      int, error type.
 * Notes:
 *      Only for 1588oEth, South to North.
 *      North to South is similar
 */
int cint_field_ieee1588_eth_update_main(int unit, bcm_field_context_t context_id, int nif_1_port, int nif_t_port)
{
    bcm_if_t arp_id_enc;
    int gport;

    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_action_info_t action_info;
    bcm_gport_t in_port_gport;
    void *dest_char;


    /*
     * 1. Create encap FEC
     * 1.1 Set NIF-1-Port default ETh-RIF
     */
    BCM_IF_ERROR_RETURN(in_port_intf_set(unit, nif_1_port, vlan_y));
    /*
     * 1.2. Set NIF-T-Port default properties
     */
    BCM_IF_ERROR_RETURN(out_port_set(unit, nif_t_port));
    /*
     * 1.3. Create ETH-RIF and set MY-Mac
     */
    BCM_IF_ERROR_RETURN(intf_eth_rif_create(unit, vlan_x, nif_t_mac));

    /*
     * 1.4. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN(l3__egress_only_encap__create(unit, 0, &arp_id_enc, fgpa_mac, vlan_x));

    /*
     * 1.5. Create FEC and set its properties
     */
    BCM_GPORT_LOCAL_SET(gport, nif_t_port);
    BCM_IF_ERROR_RETURN(l3__egress_only_fec__create(unit, fec_id_enc, 0, arp_id_enc, gport, 0));

    /*
     * 2. PMF configuration
     * 2.1.  Create Field Group
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    fg_info.qual_types[1] = bcmFieldQualifyDstMac;
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionL3Switch;
    fg_info.action_types[1] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[2] = bcmFieldActionForwardingLayerIndex;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "ieee1588_eth_update", sizeof(fg_info.name));
    BCM_IF_ERROR_RETURN(bcm_field_group_add(unit, 0, &fg_info, &cint_ieee1588_eth_update_fg_id));

    /*
     * 2.2. Attach the FG to context
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_info[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 11);
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];
    attach_info.payload_info.action_info[2].priority = BCM_FIELD_ACTION_PRIORITY(0, 12);
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[1].input_arg = 0;
    attach_info.key_info.qual_info[1].offset = 0;

    BCM_IF_ERROR_RETURN(bcm_field_group_context_attach(unit, 0, cint_ieee1588_eth_update_fg_id, context_id, &attach_info));

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;
    BCM_GPORT_LOCAL_SET(in_port_gport,nif_1_port);
    ent_info.nof_entry_quals = fg_info.nof_quals;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = in_port_gport;
    ent_info.entry_qual[0].mask[0] = 0x1ff;
    ent_info.entry_qual[1].type = fg_info.qual_types[1];
    ent_info.entry_qual[1].value[0] = 0xC200000E;
    ent_info.entry_qual[1].mask[0] = 0xFFFFFFFF;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 0x2000CDDE;
    ent_info.entry_action[1].type = fg_info.action_types[1];
    ent_info.entry_action[1].value[0] = 0;
    ent_info.entry_action[2].type = fg_info.action_types[2];
    ent_info.entry_action[2].value[0] = 1;

    /*
     * 2.3. Attach the FG to context
     */
    BCM_IF_ERROR_RETURN(bcm_field_entry_add(unit, 0, cint_ieee1588_eth_update_fg_id, &ent_info, &cint_ieee1588_eth_update_ent_id));
    return BCM_E_NONE;
}

/**
* \brief
*  Destroy all configuration done
* \param [in] unit        -  Device id
* \param [in] context_id  -  PMF context Id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_ieee1588_eth_update_destroy(int unit, bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN(bcm_field_entry_delete(unit, cint_ieee1588_eth_update_fg_id, NULL, cint_ieee1588_eth_update_ent_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_context_detach(unit, cint_ieee1588_eth_update_fg_id, context_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_delete(unit, cint_ieee1588_eth_update_fg_id));

    return BCM_E_NONE;
}
