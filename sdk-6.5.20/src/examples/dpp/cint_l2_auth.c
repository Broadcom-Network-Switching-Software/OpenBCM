/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 
* File: cint_l2_auth.c
* Purpose: Example of SA Authentication
*
* Calling sequence:
*  1. Create vlan for basic bridging and modify SA Authentication trap strength
*        - Call bcm_vlan_create() and bcm_vlan_port_add()
*        - Call bcm_rx_trap_set()
*  2. Enable SA Authentication per port and Add SA Authentication information to port 1
*        - Call bcm_vlan_control_port_set()
*        - Call bcm_switch_control_port_set()
*        - Call bcm_l2_auth_add() with following flags: 
*           BCM_L2_AUTH_CHECK_IN_PORT | BCM_L2_AUTH_CHECK_VLAN | BCM_L2_AUTH_PERMIT_TAGGED_ONLY;
*  3. Get SA Authentication information by Source Mac Address
*        - Call bcm_l2_auth_get()
*  4. Traverse the lem table to get the SA Authentication information
*        - Call bcm_l2_auth_traverse()
*  5. Delete the SA Authentication information by bcm_l2_auth_delete
*        - Call bcm_l2_auth_delete()
*  6. Add SA Authentication information to port 1 once again.
*        - Call bcm_l2_auth_add()
*  7. Delete the SA Authentication information by bcm_l2_auth_delete_all
*        - Call bcm_l2_auth_delete_all()
*  8. Disable SA Authentication per port.
*        - Call bcm_vlan_control_port_set()
*        - Call bcm_switch_control_port_set()
*
* Traffic:
*  The information about Authenticated SA
*          -   SA 00:00:00:00:00:05
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*          -   PORT 1
*
*  Flooding (sending packet with the Authenticated SA):
*      -   Send Ethernet packet with the Authenticated SA to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is flooded (as DA is not known and SA is Authenticated).
*
* Trapping
*      -   Send Ethernet packet with the unAuthenticated SA to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   Untagged
*      -   Packet is trapped (as packet is untagged).
*
*      -   Send Ethernet packet with the unAuthenticated SA to port 1:
*          -   SA 00:00:00:00:00:08
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is trapped (as mac address is unAuthenticated).
*
*      -   Send Ethernet packet with the unAuthenticated SA to port 1:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =1
*      -   Packet is trapped (as vid is unAuthenticated).
*
*      -   Send Ethernet packet with the Authenticated SA to port 2:
*          -   SA 00:00:00:00:00:05
*          -   Unknown DA
*          -   VLAN tag: VLAN tag type 0x8100, VID =10
*      -   Packet is trapped (as port is unAuthenticated).
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_l2_auth.c
*      BCM> cint
*      cint> l2_auth_run_with_defaults(unit, mac_lsb, vlan, src_port, dst_port);
* Note: In ARAD-A0 Permit-All-Ports is not supported. (SA Auth is per port only)
*/

struct l2_auth_info_s
{
    uint8 mac_lsb;          /* SA MAC address to match. */
    bcm_vlan_t vlan;        /* VLAN to match. */
    bcm_port_t src_port;    /* ingress port to match. */
    bcm_port_t dst_port;    /* egress port to capture packets. */
    int saLookupTrapId[3];
};

l2_auth_info_s l2_auth_info;

/*set the sa authentication failed trap strength, so that SA authentication can take effect*/
int l2_auth_trap_code_strength_set(int unit, int trap_type, int strength)
{   
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int cpuCaptureTrapId;
    int idx;

    rv = bcm_rx_trap_type_create(unit, 0, trap_type, &cpuCaptureTrapId);
    if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_trap_type_create rv %d\n", rv);
        return rv;
    }
    
    /* record the original setting for reverting*/
    idx = trap_type - bcmRxTrapAuthSaLookupFail;
    l2_auth_info.saLookupTrapId[idx] = cpuCaptureTrapId;

    sal_memset(&config, 0, sizeof(config));
    
    config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    config.trap_strength = strength;
    BCM_GPORT_LOCAL_SET(config.dest_port,0);
    rv = bcm_rx_trap_set(unit, cpuCaptureTrapId, &config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_set returned %d\n", rv);
        return rv;
    }

    return rv;
}

int l2_auth_trap_code_strength_revert(int unit)
{   
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int cpuCaptureTrapId;
    int trap_type;
    int idx;

    for (idx = 0; idx < 3; idx++) {     
        cpuCaptureTrapId = l2_auth_info.saLookupTrapId[idx];
        rv = bcm_rx_trap_type_destroy(unit, cpuCaptureTrapId);
        if (rv != BCM_E_NONE) {
            printf("Error in: bcm_rx_trap_type_destroy rv %d\n", rv);
            return rv;
        }
    }

    return rv;
}

int l2_auth_init(int unit, uint8 mac_lsb, bcm_vlan_t vlan, bcm_port_t src_port, bcm_port_t dst_port)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;

    l2_auth_info.mac_lsb = mac_lsb;
    l2_auth_info.vlan = vlan;
    l2_auth_info.src_port = src_port;
    l2_auth_info.dst_port = dst_port;

    /* vlan 1 is already created with all ports */
    if (vlan != 1)
    {
        /* create vlan add port */
        rv = bcm_vlan_create(unit, vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vlan, rv);
            return rv;
        }
        
        /* add port to vlan */
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, src_port);
        BCM_PBMP_PORT_ADD(pbmp, dst_port);
        BCM_PBMP_CLEAR(ubmp);
        rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_add unit %d, vid %d, rv %d\n", unit, vlan, rv);
            return rv;
        }

        /* for validating untag packet coming to port which only accept tag packet (BCM_L2_AUTH_PERMIT_TAGGED_ONLY) */
        rv = bcm_port_untagged_vlan_set(unit, src_port, vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_untagged_vlan_set unit %d, src_port %d, vid %d, rv %d\n", unit, src_port, vlan, rv);
            return rv;
        }
    }

    rv = l2_auth_trap_code_strength_set(unit, bcmRxTrapAuthSaLookupFail, 7);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_auth_trap_code_strength_set rv %d\n", rv);
        return rv;
    }

    rv = l2_auth_trap_code_strength_set(unit, bcmRxTrapAuthSaPortFail, 7);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_auth_trap_code_strength_set rv %d\n", rv);
        return rv;
    }

    rv = l2_auth_trap_code_strength_set(unit, bcmRxTrapAuthSaVlanFail, 7);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_auth_trap_code_strength_set rv %d\n", rv);
        return rv;
    }

    return BCM_E_NONE;  
}

int l2_auth_revert(int unit)
{
    int rv = BCM_E_NONE;

    /* vlan 1 is default vlan */
    if (l2_auth_info.vlan != 1)
    {
        /* create vlan add port */
        rv = bcm_vlan_destroy(unit, l2_auth_info.vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_create unit %d, vid %d, rv %d\n", unit, vlan, rv);
            return rv;
        }
        
        /* revert the port provide default vlan to 1 */
        rv = bcm_port_untagged_vlan_set(unit, l2_auth_info.src_port, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_untagged_vlan_set unit %d, src_port %d, vid %d, rv %d\n", unit, l2_auth_info.src_port, 1, rv);
            return rv;
        }
    }

    rv = l2_auth_trap_code_strength_revert(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_auth_trap_code_strength_revert rv %d\n", rv);
        return rv;
    }

    rv = l2_auth_port_info_set(unit, l2_auth_info.src_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_auth_port_info_set rv %d\n", rv);
        return rv;
    }

    rv = l2_auth_port_info_set(unit, l2_auth_info.dst_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_auth_port_info_set rv %d\n", rv);
        return rv;
    }

    return BCM_E_NONE;  
}

int l2_auth_port_info_set(int unit, bcm_port_t src_port, int enable)
{
    int rv = BCM_E_NONE;

    rv = bcm_vlan_control_port_set(unit, src_port, bcmVlanLookupMACEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_control_port_set rv %d\n", rv);
        return rv;
    }
    
    rv = bcm_port_control_set(unit, src_port, bcmPortControlL2SaAuthenticaion, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set rv %d\n", rv);
        return rv;
    }

    return rv;
}

int l2_auth_add(int unit)
{
    int rv = BCM_E_NONE;
    bcm_l2_auth_addr_t l2_auth_addr;
    int idx;

    l2_auth_addr.flags = BCM_L2_AUTH_CHECK_IN_PORT | BCM_L2_AUTH_CHECK_VLAN | BCM_L2_AUTH_PERMIT_TAGGED_ONLY;
    l2_auth_addr.vlan = l2_auth_info.vlan;
    l2_auth_addr.src_port = l2_auth_info.src_port;

    for (idx = 0; idx < 5; idx++) {
        l2_auth_addr.sa_mac[idx] = 0;
    }
    l2_auth_addr.sa_mac[5] = l2_auth_info.mac_lsb;
    
    rv = bcm_l2_auth_add(unit, &l2_auth_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_petra_l2_auth_add rv %d\n", rv);
        return rv;
    }

    return rv;
}

int l2_auth_delete(int unit)
{
    int rv = BCM_E_NONE;
    bcm_l2_auth_addr_t l2_auth_addr;
    int idx;

    l2_auth_addr.flags = BCM_L2_AUTH_CHECK_IN_PORT | BCM_L2_AUTH_CHECK_VLAN | BCM_L2_AUTH_PERMIT_TAGGED_ONLY;
    l2_auth_addr.vlan = l2_auth_info.vlan;
    l2_auth_addr.src_port = l2_auth_info.src_port;

    for (idx = 0; idx < 5; idx++) {
        l2_auth_addr.sa_mac[idx] = 0;
    }
    l2_auth_addr.sa_mac[5] = l2_auth_info.mac_lsb;
    
    rv = bcm_l2_auth_delete(unit, &l2_auth_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_petra_l2_auth_add rv %d\n", rv);
        return rv;
    }

    return rv;
}

int l2_auth_delete_all(int unit)
{
    int rv = BCM_E_NONE;
    
    rv = bcm_l2_auth_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_auth_delete_all rv %d\n", rv);
        return rv;
    }

    return rv;
}

int l2_auth_addr_print(int unit, bcm_l2_auth_addr_t *addr, void *user_data)
{
    printf("--addr flag 0x%x\n", addr->flags);
    printf("--sa_mac %02x:%02x:%02x:%02x:%02x:%02x\n", addr->sa_mac[0],addr->sa_mac[1],
        addr->sa_mac[2], addr->sa_mac[3],addr->sa_mac[4],addr->sa_mac[5]);
    printf("--vlan %d\n", addr->vlan);
    printf("--src_port 0x%x\n\n", addr->src_port);

    return BCM_E_NONE;
}

int l2_auth_get(int unit)
{
    int rv = BCM_E_NONE;
    bcm_l2_auth_addr_t l2_auth_addr;
    int idx;

    l2_auth_addr.flags = 0;
    l2_auth_addr.src_port = 0;
    l2_auth_addr.vlan = 0;

    for (idx = 0; idx < 5; idx++) {
        l2_auth_addr.sa_mac[idx] = 0;
    }
    l2_auth_addr.sa_mac[5] = l2_auth_info.mac_lsb;

    rv = bcm_l2_auth_get(unit, &l2_auth_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_auth_get failed! rv %d\n", rv);
    }

    l2_auth_addr_print(unit, &l2_auth_addr, NULL);

    return rv;
}

int l2_auth_traverse(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_l2_auth_traverse(unit, l2_auth_addr_print, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_auth_traverse failed! rv %d\n", rv);
    }

    return rv;
}

int l2_auth_run_with_defaults(int unit, uint8 mac_lsb, bcm_vlan_t vlan, bcm_port_t src_port, bcm_port_t dst_port)
{
    int rv = BCM_E_NONE;

    rv = l2_auth_init(unit, mac_lsb, vlan, src_port, dst_port);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    rv = l2_auth_port_info_set(unit, src_port, 1);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    
    rv = l2_auth_port_info_set(unit, dst_port, 1);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    
    rv = l2_auth_add(unit);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    return rv;
}
