/*
 * Purpose: VPLS basic examples
 */

/*
 * Test Scenarios
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/vpls/cint_dnx_cint_vpls_incorrect_parsing_to_rcy.c
  cint
  int unit = 0;
  int pwe_label = 3555;
  int pwe_vpn = 1;
  int rcyP = 200;
  int outP = 201;
  vpls_incorrect_parsing_to_rcy(unit,pwe_label,pwe_vpn,rcyP,outP);"
  exit;
 *
 *  Scenario configured in this cint:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   VPLS incorrect speculative parsing
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  - Configure PWE tunnel termination
 *  - Configure bcmRxTrapMplsSpeculativeParseFail trap to RCY port and RCY outlif
 *  - Configure bridge entry for the second pass
 *
 * Test Scenario - end
 *
 */

int pwe_encap_id = 0x501;
int pwe_egress_tunnel_if = 0x20000001;

int
vpls_incorrect_parsing_to_rcy(
    int unit,
    int pwe_label,
    int pwe_vpn,
    int rcy_port,
    int out_port)
{
    /*
     * Create a PWE termination
     */
    bcm_mac_t mac_with_leading_4 = {0x40, 0x00, 0x00, 0x00, 0xcd, 0x1d};
    bcm_mpls_port_t mpls_port_entry;
    bcm_mpls_port_t_init(&mpls_port_entry);
    mpls_port_entry.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port_entry.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port_entry.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_STAT_ENABLE | BCM_MPLS_PORT2_EXTENDED_TERMINATION;
    mpls_port_entry.match_label = pwe_label;
    mpls_port_entry.encap_id  = pwe_encap_id;
    mpls_port_entry.egress_tunnel_if = pwe_egress_tunnel_if;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, pwe_vpn, &mpls_port_entry),"failed in bcm_mpls_port_add");

    /*
     * Create a Bridge from PWE VPN to outP, based on inner DMAC
     */
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac_with_leading_4 ,pwe_vpn);
    l2_addr.flags  = BCM_L2_STATIC;
    l2_addr.port = out_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr),"failed in bcm_l2_addr_add");

    /*
     * Configure the RCY port as an extended termination port
     */
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), "failed in bcm_l2_addr_add");
    }

    /*
     * Create the Extended termination RCH LIF
     */
    bcm_gport_t rcy_gport;
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "fail in bcm_l2_egress_create");
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(rcy_gport, recycle_entry.encap_id);

    /*
     * Configure the parsing fail trap to an extended termination port and LIF
     */
    int trap_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_type_create(unit, 0, bcmRxTrapMplsSpeculativeParseFail, &trap_id), "fail in bcm_rx_trap_type_create");

    bcm_rx_trap_config_t rx_trap_config;
    bcm_rx_trap_config_t_init(&rx_trap_config);
    rx_trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_DEST;
    rx_trap_config.encap_id = rcy_gport;
    rx_trap_config.dest_port = rcy_port;
    BCM_IF_ERROR_RETURN_MSG(bcm_rx_trap_set(unit, trap_id, &rx_trap_config),"failed in bcm_rx_trap_set");

    return 0;
}
