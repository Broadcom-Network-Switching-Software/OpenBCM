/*~~~~~~~~~~~~~~~~~~~~~~~~~~Traditional Bridge: Spanning-Tree Groups~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_stg.c
 * Purpose: Example of simple spanning-tree group configuration
 *
 * Default settings:
 * It is assumed diag_init is executed:
 *  o    All ports set with TPID 0x8100 and all packet formats (tagged/untagged) are accepted.
 *  o    PVID of all ports set to VID = 1, thus untagged packet is associated to VLAN 1.
 *  o    By default, learning is managed by device without interference from the CPU.
 *  o    By default, aging is disabled.
 *  o    All VLANs are mapped to Default STG (0).
 *  o    STP state of all ports in default topology is set to forwarding.
 *
 * Note: Without running diag_init, ports are not associated with any TPID,
 *       and thus all packets would be considered as untagged.
 *       Besides, only untagged packets are accepted.
 *
 * Settings include:
 *  o    Creating one STG.
 *  o    Creating 2 VLANs.
 *  o    Adding the VLANs to the stg.
 *  o    Connecting the stg with 2 ports, for first port with stp state forward
 *       and the second one with stp state block.
 *
 * Application Sequence:
 *  o    Run application: dnx_stg_run()
 *  o    Add mac entries that forward traffic with the two vids from the two ports to a third port.
 *  o    Send packets with one of the VLANs (in our example, vid = 100 or 300) on first port
 *       (in our example, port 1) and check that it is indeed forwarded to the third port.
 *  o    Send packets with one of the VLANs (in our example, vid = 100 or 300) on second port
 *       (in our example, port 2) and see that it is dropped.
 *  o    To delete the VLANs and stg that were created for the test use dnx_stg_revert().
 *  o    Set bandwidth limitation of 500 Mbps on scheduler.
 *
 * Note: You can modify the ports, the VLANs, and the stp states that the script uses by changing
 *       the values in the dnx_stg_init() function.
 *
 */
/*
 * Test Scenario - start 
 *
 * ./bcm.user
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/stg/cint_dnx_stg.c
 * cint
 * dnx_stg_run(0,200,201);
 * exit;
 *
 * tx 1 psrc=203 data=0x000096eb8d4b00005a46cc39810000640899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 * // Source port: 0, Destination port: 0 
 * // Data: 0x000096eb8d4b00005a46cc390899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * tx 1 psrc=203 data=0x000096eb8d4b00005a46cc398100012c0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 * // Source port: 0, Destination port: 0 
 * // Data: 0x000096eb8d4b00005a46cc398100012c0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * tx 1 psrc=203 data=0x000096eb8d4b00005a46cc39810000250899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 * // Source port: 0, Destination port: 0 
 * // Data: 0x000096eb8d4b00005a46cc39810000250899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * tx 1 psrc=203 data=0x0000bf45150300005a46cc39810000640899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 *
 * tx 1 psrc=203 data=0x0000bf45150300005a46cc398100012c0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 *
 * tx 1 psrc=203 data=0x0000bf45150300005a46cc39810000250899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * // Received packets on unit 0 should be: 
 * // Source port: 0, Destination port: 0 
 * // Data: 0x0000bf45150300005a46cc39810000250899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * Test Scenario - end
 */


struct dnx_stg_info_s {
    int vlan_1;
    int vlan_2;
    int port_1;
    int port_2;
    int stp_state_1;
    int stp_state_2;
    bcm_stg_t stg;
};
dnx_stg_info_s info;

int
dnx_stg_init(int unit, dnx_stg_info_s *info,int port1,int port2) {

    info->vlan_1 = 100;
    info->vlan_2 = 300;
    info->port_1 = port1;
    info->port_2 = port2;
    info->stp_state_1 = BCM_STG_STP_FORWARD;
    info->stp_state_2 = BCM_STG_STP_BLOCK;
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_create(unit,&(info->stg)), "");
    return BCM_E_NONE;
}

int
dnx_stg_config(int unit, dnx_stg_info_s *info) {
    char error_msg[200]={0,};
    bcm_pbmp_t pbmp1;
    bcm_pbmp_t pbmp2;

    /* create the two vlans*/
    snprintf(error_msg, sizeof(error_msg), "vid=%d", info->vlan_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit,info->vlan_1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "vid=%d", info->vlan_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit,info->vlan_2), error_msg);

    BCM_PBMP_CLEAR(pbmp1);
    BCM_PBMP_CLEAR(pbmp2);
    BCM_PBMP_PORT_ADD(pbmp1, info->port_1);
    BCM_PBMP_PORT_ADD(pbmp2, info->port_2);

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, info->vlan_1, pbmp1, pbmp1), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, info->vlan_2, pbmp2, pbmp2), "");

    /* add the vlans to the stg */
    snprintf(error_msg, sizeof(error_msg), "vid=%d", info->vlan_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_vlan_add(unit,info->stg,info->vlan_1), error_msg);
    snprintf(error_msg, sizeof(error_msg), "vid=%d", info->vlan_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_vlan_add(unit,info->stg,info->vlan_2), error_msg);

    /* add two ports to the stg and attach stp state for each of them */
    snprintf(error_msg, sizeof(error_msg), "stg=%d", info->stg);
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_stp_set(unit,info->stg,info->port_1,info->stp_state_1), error_msg);
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_stp_set(unit,info->stg,info->port_2,info->stp_state_2), error_msg);

    return BCM_E_NONE;
}

/*
 * this function should be called if the user wants to destroy the stg and vlans that were created in main()
 */
int
dnx_stg_revert(int unit) {
    char error_msg[200]={0,};

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_destroy(unit,info.stg), "");

    snprintf(error_msg, sizeof(error_msg), "vid=%d", info.vlan_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy(unit,info.vlan_1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "vid=%d", info.vlan_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy(unit,info.vlan_2), error_msg);
    return BCM_E_NONE;
}

int dnx_stg_run(int unit, int port1,int port2) {

    BCM_IF_ERROR_RETURN_MSG(dnx_stg_init(unit, &info, port1, port2), "");
    BCM_IF_ERROR_RETURN_MSG(dnx_stg_config(unit, &info), "");
    return BCM_E_NONE;
}

/*
 *Add test case to verify Spanning tree port state in certain sequence
 *   step1: create stag and set status to block
 *   step2: delete stag
 *   step3: recreate that stag, the status should be forwarding
*/
int dnx_stg_verify(int unit, int port1,int port2)
{
     int stg=2;
     int state;
     int vlan=100;
     int port_1=port1;
     int port_2=port2;
     bcm_pbmp_t pbmp;
     bcm_pbmp_t u_pbmp;

     printf("in stg_verify, port1 =%d, port2 %d, \n", port1, port2);

     BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vlan), "");
     BCM_PBMP_CLEAR(pbmp);
     BCM_PBMP_CLEAR(u_pbmp);
     BCM_PBMP_PORT_ADD(pbmp, port1);
     BCM_PBMP_PORT_ADD(pbmp, port2);
     BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_add(unit, vlan, pbmp, u_pbmp), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_create_id(unit, stg), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_vlan_add(unit, stg, vlan), "");

    /* Set state to block and check SW and HW config
    SW = BCM_STG_STP_BLOCK(1), HW=SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK(2) */
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_stp_set(unit, stg, port1, BCM_STG_STP_BLOCK), "");

    /* Set state to block and check SW and HW config after deleting and recreating the STG */
    BCM_IF_ERROR_RETURN_MSG(bcm_stg_vlan_remove(unit, stg, vlan), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_destroy(unit, stg), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_create_id(unit, stg), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_vlan_add(unit, stg, vlan), "");

    return BCM_E_NONE;
}

int dnx_stg_verify_clean(int unit, int port1,int port2)
{
    int vlan=100;
    int stg=2;
    int port_1=port1;
    int port_2=port2;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t u_pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(u_pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port1);
    BCM_PBMP_PORT_ADD(pbmp, port2);

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_vlan_remove(unit, stg, vlan), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_remove(unit,vlan,pbmp), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_destroy(unit, vlan), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_stg_destroy(unit, stg), "");
    return BCM_E_NONE;
}
