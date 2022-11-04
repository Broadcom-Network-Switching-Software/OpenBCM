/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 *
 * Configuration:
 *
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
  cint ../../../../src/examples/dnx/mpls/cint_dnx_mpls_frr_basic.c
  cint
  int unit = 0;
  int rv = 0;
  int port1 = 200;
  int vlan_id = 100;
  int frr_label = 0x1111;
  rv = mpls_frr_basic_main(unit,port1,vlan_id,frr_label);
  print rv;
 * Test Scenario - end
 *
 * Set the in port to support MPLS FRR
 * Configure the MPLS FRR label in FRR table
 *
 *  ###################################################################################################
 */

int
mpls_frr_basic_main(
    int unit,
    int port,
    int vlan,
    int frr_label)
{
    uint32 flags = 0;
    bcm_mpls_special_label_t label_info = { 0 };

    BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, port, bcmPortControlMplsFRREnable, 1), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, vlan, port, 0), "");

    label_info.label_value = frr_label;
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_special_label_identifier_add(unit, bcmMplsSpecialLabelTypeFrr, label_info), "");

    return BCM_E_NONE;
}
