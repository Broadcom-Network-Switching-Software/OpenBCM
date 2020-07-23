/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ECMP hashing example script
 */

/*
print ecmp_hash_func_config(unit, BCM_HASH_CONFIG_ROUND_ROBIN); 

ecmp_hash_func_config() with BCM_HASH_CONFIG_ROUND_ROBIN uses a counter that is incremented every packet, instead of polynomial hashing, 
so traffic will be divided equally between the ECMP memebers, although ECMP hashing is disabled. 

print ecmp_hash_func_config(unit, BCM_HASH_CONFIG_CRC16_0x101a1); 

return ECMP hashing hashing to be done according to some polynomial, like before.
 
ECMP hashing by IPV4 SIP example: 
---------------------------------
run: 
ecmp_hash_sip(unit); 
 
run same IPV4 traffic. this time ECMP hashing will be done according to the IPV4 SIP. 
in this case, all packets will be divided equally between the ECMP memebers (different ports). 
 
ECMP hashing by MPLS label2 example: 
------------------------------------
run: 
ecmp_hash_label2(unit); 
 
run same MPLS traffic. this time ECMP hashing will be done according to the MPLS label2. 
in this case, all packets will be divided equally between the ECMP memebers (different ports). 
 
ECMP hashing by SRC port: 
-------------------------
run: 
ecmp_src_port_enable(unit, 1);
 
run IPV4 traffic, with fixed SIP, from port 13. All packets will go to one of the ECMP memebers (one dest port). 
run same IPV4 traffic (with fixed SIP) from port 14. All packets will go to one of the ECMP memebers, but it will be a different one than before. 
Changing the SRC port will change the packets' destination, because hashing takes into account the SRC port.
 
ECMP hashing by 2 headers: 
-------------------------
run: 
ecmp_nof_headers_set(unit, <in_port>, 2);
 
run IPoIPoEth: 
    ethernet header with DA 0:C:0:2:01:23, fixed SA and vlan tag id 17
    IPV4 header with DIP 10.0.255.0, and fixed SIP
    and another IPV4 header with random DIP and random SIP
 
packets will be divided between all ECMP memebers (dest ports), because ECMP hashing is set to look at 2 headers, 
starting from the forwrding header, and it is also set to look at IPV4 SIP.

ECMP using ELI-BOS:
-------------------------
run:
ecmp_hashing_eli_bos(int unit, int inpotr, int outport)

This test creates an ECMP of size ELI_BOS_NUMBER_OF_INTERFACES  where, each member in the  ECMP switch the
first MPLS label to a different label to track the selected member.
*/

struct cint_ecmp_hashing_cfg_s {
  int ecmp_api_is_ecmp;
};

struct cint_ecmp_hashing_data_s {
  /* After ecmp_hashing_main is run successfully, this will contain the ECMP object that was created. */
  /* Only works with the bcm_l3_egress_ecmp_* interfaces.*/
  bcm_l3_egress_ecmp_t ecmp;
  /* After ecmp_hashing_main is run successfully, this will contain the FEC objects that were created. */
  bcm_if_t egress_intfs[1000]; /* FECs */
};

cint_ecmp_hashing_cfg_s cint_ecmp_hashing_cfg = { 
  1 /* ecmp_api_is_ecmp */
};

cint_ecmp_hashing_data_s cint_ecmp_hashing_data;

struct label_action {
    int in_label;
    int out_label;
    bcm_mpls_switch_action_t action;
};
label_action mpls_label_stack[9];
static int NOF_INTERFACES = 200;
bcm_mac_t ingr_mac = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
bcm_mac_t egr_mac = {0x00, 0x00, 0x00, 0x00, 0x66, 0x22};
bcm_ip6_t ip6_addr_dip = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x12,0x12,0x12};
bcm_ip6_t ip6_addr_sip = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x34,0x56,0x78};
bcm_ip_t ip_addr_dip = 0x12312312;
bcm_ip_t ip_addr_sip = 0x45645645;

/* delete functions */
int delete_host(int unit, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_intf = intf;

  rc = bcm_l3_host_delete_all(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_delete_all failed: %d \n", rc);
  }
  return rc;
}

/* 
ecmp_hash_sip(): 
set ECMP hashing to done according to IPV4 SIP. 
in this case, hashing result will be the same for every packet with the same SIP.
*/ 
int ecmp_hash_sip(int unit) {
  int rc;
  int arg;
  bcm_switch_control_t type = bcmSwitchHashIP4Field0;

  /* only the whole SIP can be used for hashing, so both LO and HI must be used together */
  arg = BCM_HASH_FIELD_IP4SRC_LO | BCM_HASH_FIELD_IP4SRC_HI;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 failed: %d \n", rc);
  }

  return rc;
}

/* 
ecmp_hash_sip(): 
set ECMP hashing to done according to IPV4 SIP. 
in this case, hashing result will be the same for every packet with the same SIP.
*/ 
int ecmp_hash_sipv6(int unit) {
  int rc;
  int arg;
  bcm_switch_control_t type = bcmSwitchHashIP6Field0;

  /* only the whole SIP can be used for hashing, so both LO and HI must be used together */
  arg = BCM_HASH_FIELD_IP6SRC_LO | BCM_HASH_FIELD_IP6SRC_HI;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 failed: %d \n", rc);
  }

  return rc;
}

/* 
ecmp_hash_label2(): 
set ECMP hashing to done according to MPLS label2. 
in this case, hashing result will be the same for every packet with the same label2.
*/ 
int ecmp_hash_label2(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_2ND_LABEL;
  bcm_switch_control_t type = bcmSwitchHashMPLSField0;
  if (!is_device_or_above(unit, JERICHO2))
  {
      rc = bcm_switch_control_set(unit, type, arg);
      if (rc != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField0 failed: %d \n", rc);
      }
  }

  return rc;
}

/* 
ecmp_hash_l4_dest(): 
set ECMP hashing to done according to destination L4 port. 
*/ 
int ecmp_hash_l4_dst(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_DSTL4;
  bcm_switch_control_t type = bcmSwitchHashIP4Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
ecmp_src_port_enable(): 
Make the Source port a part of the ECMP hash.
arg = 1- enable, 0- disable. 
*/ 
int ecmp_src_port_enable(int unit, int arg) {
  int rc;
  bcm_switch_control_t type = bcmSwitchECMPHashSrcPortEnable;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashSrcPortEnable failed: %d \n", rc);
  }

  return rc;
}

/* 
ecmp_nof_headers_set(): 
Selects the number of headers to consider in ECMP hashing.
nof_headers - can be 1/2. 
*/ 
int ecmp_nof_headers_set(int unit, int in_port, int nof_headers) {
  int rc;
  bcm_switch_control_t type = bcmSwitchECMPHashPktHeaderCount;

  rc = bcm_switch_control_port_set(unit, in_port, type, nof_headers);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_port_set with type bcmSwitchECMPHashPktHeaderCount failed: %d \n", rc);
  }

  return rc;
}

/*
 * Flip One bit in the MPLS label
 */
uint32 flipBitInMPLSlabel(uint32 label, uint32 bit) {

    if (((1 << bit) & label) > 0) {
        label &=  (~(1 << bit));
    } else {
        label |= (1 << bit);
    }
    return (0xFFFFF & label);
}

/* This function is used to set up the LB configuration.
 * It creates an ECMP group of size NOF_INTERFACES. It can be flexible of stateful based on the parameter provided
 * to the function - is_flb - 1 for FLB and 0 for CLB.
 * Each FEC in the ECMP group swaps the first MPLS label to a different label 
 * than the other FECs to identify from which in-coming packet the ECMP member was passed.
 */
int mpls_eth_ecmp_no_termination(int unit, int in_port, int out_port, int vlan, int label_ingress, int is_flb)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egr;
    bcm_if_t l3_egr_id;
    int i, label_temp, ecmp_intf;
    int out_label = 2056;
    int egr_label_invalid = -1; /*A number showing that the egress mpls label is invalid.*/

    /*Create L3 interface*/
    bcm_l3_intf_t_init(l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, ingr_mac, 6);
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_vrf = vlan;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create - %d \n", rv);
        return rv;
    }

    /*Create L3 egress*/
    bcm_l3_egress_t_init(&l3_egr);
    sal_memcpy(l3_egr.mac_addr, ingr_mac, 6);
    l3_egr.vlan   = vlan;
    l3_egr.port   = out_port;
    l3_egr.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3_egr, &l3_egr_id);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_create failed \n");
        return rv;
    }

    rv = create_mpls_ecmp_group(unit, out_label, l3_egr_id, out_port, vlan, is_flb, &ecmp_intf);
    if (rv != BCM_E_NONE)
    {
        printf ("create_mpls_ecmp_group failed - (%d)\n", rv);
        return rv;
    }

    /* Create tunnel switches for each bit of the MPLS label saved in label_ingress.
     * The used label for each mpls switch is different than the rest -
     * each iteration a different bit of the label is switched to its opposite.
     * Iteration 0 of label 0x00008 will result in a tunnel switch with label = 0x00009.
     * Iteration 3 of label 0x00008 will result in a tunnel switch with label = 0x00000.
     * This is done to verify that each different MPLS label will result to different hashing.
     */
    for(i = 0; i < 21; i++)
    {
        label_temp = flipBitInMPLSlabel(label_ingress, i);
        rv = mpls_tunnel_switch_create(unit, label_temp, ecmp_intf, BCM_MPLS_SWITCH_ACTION_NOP, in_port, egr_label_invalid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
    }

    return rv;
}

/* This function creates an mpls tunnel switch with given label, port, egress interface.
 * egress_label will be added to the structure if it's value is valid (not equal to -1)
 * The action that will be taken upon encountering a matching packet is provided by the "action" variable.
 * Valid actions are BCM_MPLS_SWITCH_ACTION_SWAP, BCM_MPLS_SWITCH_ACTION_PHP, BCM_MPLS_SWITCH_ACTION_POP,
 * BCM_MPLS_SWITCH_ACTION_POP_DIRECT, BCM_MPLS_SWITCH_ACTION_NOP, BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH,
 * BCM_MPLS_SWITCH_ACTION_INVALID.
 */
int mpls_tunnel_switch_create(int unit, int label_in, int rif_intf, bcm_mpls_switch_action_t action, int in_port, int egress_label)
{
    int rv = BCM_E_NONE;
    int invalid = -1;
    
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
    mpls_tunnel_info.action = action;
    mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
    mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    mpls_tunnel_info.label = label_in; /* incoming label */
    mpls_tunnel_info.egress_if = rif_intf;
    mpls_tunnel_info.port = in_port;
    if (egress_label != invalid)
    {
        mpls_tunnel_info.egress_label.label = egress_label; /* outgoing (egress) label */
    }
    /*BCM_MPLS_INDEXED_LABEL_SET(mpls_tunnel_info.label, i+bos, index);*/
    rv = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
    if (rv != BCM_E_NONE) {
        printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rv);
        return rv;
    }
    return rv;
}

/* This function is used to create an mpls tunnel initiator. It initialises a single tunnel.
 * It accepts an outgoing label, an interface ID and action that will be taken upon encountering the tunnel.
 * Valid actions are:
 * BCM_MPLS_EGRESS_ACTION_SWAP, BCM_MPLS_EGRESS_ACTION_PHP, BCM_MPLS_EGRESS_ACTION_PUSH, BCM_MPLS_EGRESS_ACTION_NOP,
 * BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH.
 * tunnel_id is a callback variable that is used to return the ID of the created tunnel.
 */
int mpls_tunnel_initiate(int unit, int out_label, int egr_id, int *tunnel_id, bcm_mpls_egress_action_t action)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t mpls_tunnel[1];
    bcm_mpls_egress_label_t_init(mpls_tunnel[0]);
    mpls_tunnel[0].exp = 0;
    mpls_tunnel[0].label = out_label;
    mpls_tunnel[0].flags = BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    mpls_tunnel[0].action = action;
    mpls_tunnel[0].l3_intf_id = egr_id;
    mpls_tunnel[0].ttl = 30;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,1,mpls_tunnel);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_create Label = 0x%x\n", mpls_tunnel[0].label);
        return rv;
    }

    *tunnel_id = mpls_tunnel[0].tunnel_id;
    return rv;
}

/* The aim of this function is to define the cint's global variables for ingress and egress MAC address from tcl.
 * Using the is_ingr variable with value of:
 *          - 1 the ingr_mac address will be updated;
 *          - 0 the egr_mac address will be updated. 
 * The function accesspts the values of all 6 bytes of the mac address.
 */
int set_mac_addr (int is_ingr, int byte0, int byte1, int byte2, int byte3, int byte4, int byte5)
{
    bcm_mac_t mac_array = {byte0, byte1, byte2, byte3, byte4, byte5};
    int rv = BCM_E_NONE, i;
    if (is_ingr == 1)
    {
        for(i = 0;i < 6; i++)
        {
            ingr_mac[i] = mac_array[i];
        }
    }
    else
    {
        for(i = 0;i < 6; i++)
        {
            egr_mac[i] = mac_array[i];
        }
    }
    return rv;
}

/* This function is used to create an L2 MAC entry in the MACT.
 * It uses the provided mac_addr and vlan to form the key and it results to port.
 */
int create_l2_mac_entry(int unit, bcm_mac_t mac_addr, int vlan, int port)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, mac_addr, vlan);
    sal_memcpy(l2_addr.mac, mac_addr, 6);
    l2_addr.vid = vlan;
    l2_addr.port = port;
    l2_addr.flags = BCM_L2_STATIC;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Erorr, bcm_l2_addr_add - %d\n", rv);
    }
    return rv;
}

/* This function flips a single bit of the mac address.
 * The bit provided to the function must be between 0 and 47. 
 * It is then translated to the exact bit (0 to 7) of the exact byte (0 to 5).
 * The mac address is changed by using its address.
 */
void flip_bit_in_mac_addr (bcm_mac_t * mac_addr, uint32 bit) 
{
    uint32 byte = bit / 8;
    bit = 7 - (bit % 8);
    if (((1 << bit) & mac_addr[byte]) > 0)
    {
        mac_addr[byte] &= (~(1 << bit));
    }
    else 
    {
        mac_addr[byte] |= (1 << bit);
    }
    return;
}

/* This function flips a single bit in the provided bcm_ip_t IPv4 address.
 * It changes the given bit number in the given IP address from 0 to 1 or from 1 to 0.
 * The bit number must be between 0 and 31.
 * The updated IP address is returned byt the function.
 */
bcm_ip_t flip_bit_in_ip_addr (bcm_ip_t ip_addr, uint32 bit) 
{
    if (((1 << bit) & ip_addr) > 0)
    {
        ip_addr &= (~(1 << bit));
    }
    else 
    {
        ip_addr |= (1 << bit);
    }
    return ip_addr;
}

/* This function is used to flip a single bit in the provided IPv6 address.
 * The bit number must be between 0 and 127 and it is then translated 
 * to a particular byte (0 to 16) and bit (0 to 7).
 * The resulting ipv6 address is returned as a parameter.
 */
bcm_ip6_t flip_bit_in_ip6_addr (bcm_ip6_t ip_addr, uint32 bit) 
{
    uint32 byte = bit / 8;
    bit = 7 - (bit % 8);
    if (((1 << bit) & ip_addr[byte]) > 0)
    {
        ip_addr[byte] &= (~(1 << bit));
    }
    else 
    {
        ip_addr[byte] |= (1 << bit);
    }
    return ip_addr;
}

/* This function creates a UC route entry in the table using full mask, 
 * the provided ipv4 addresses, vlan and results to a fec.
 */
int create_ip_route(int unit, bcm_ip_t ipv4_addr_dip, int vlan, int fec_res)
{
    int rv = BCM_E_NONE;
    uint32 full_mask = 0xFFFFFFFF;
    bcm_l3_route_t l3route;
    bcm_l3_route_t_init(&l3route);
    l3route.l3a_subnet = ipv4_addr_dip;
    l3route.l3a_ip_mask = full_mask;
    l3route.l3a_vrf = vlan;
    l3route.l3a_intf = fec_res;     /* fec */
    rv = bcm_l3_route_add(unit, &l3route);
    if(rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_route_add - (%d)\n", rv);
    }
    return rv;
}

/* This function creates an IPv6 UC route entry using a full mask, the provided IPv6 address and a vid.
 * The entry results to a FEC.
 */
int create_ip6_route(int unit, bcm_ip6_t ipv6_addr, int vlan, int fec_res)
{
    int rv = BCM_E_NONE;
    bcm_ip6_t full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_l3_route_t l3route;
    bcm_l3_route_t_init(&l3route);
    l3route.l3a_ip6_net = ipv6_addr;
    l3route.l3a_ip6_mask = full_mask;
    l3route.l3a_vrf = vlan;
    l3route.l3a_intf = fec_res;     /* fec */
    l3route.l3a_flags = BCM_L3_IP6;
    rv = bcm_l3_route_add(unit, &l3route);
    if(rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_route_add - (%d)\n", rv);
    }
    return rv;
}

/* The aim of the function is to be called from tcl with the values of the 16 bytes of a ipv6 address.
 * The IPv6 address is defined globally for this file and the user chooses whether to change the IPv6 dip address
 * or the sip address by providing 1 or 0 respectively to the is_dst value.
 */
int set_ipv6_addr(int is_dst, uint8 bt0, uint8 bt1, uint8 bt2, uint8 bt3, uint8 bt4, uint8 bt5, uint8 bt6,
    uint8 bt7, uint8 bt8, uint8 bt9, uint8 bt10, uint8 bt11, uint8 bt12, uint8 bt13, uint8 bt14, uint8 bt15)
{
    int rv = BCM_E_NONE;
    bcm_ip6_t temp_ip6 = {bt0, bt1, bt2, bt3, bt4, bt5, bt6, bt7, bt8, bt9, bt10, bt11, bt12, bt13, bt14, bt15};
    int nof_bytes = 16;
    int i;
    if(is_dst == 1)
    {
        for(i = 0; i < nof_bytes; i++)
        {
            ip6_addr_dip[i] = temp_ip6[i];
        }
    }
    else
    {
        for(i = 0; i < nof_bytes; i++)
        {
            ip6_addr_sip[i] = temp_ip6[i];
        }
    }
    return rv;
}

/* This function is used to set the global ipv4 addresses - DIP and SIP from tcl.
 * This is needed for mpls full termination case.
 */
int set_ipv4_addr(int is_dst, bcm_ip_t ip_addr)
{
    int rv = BCM_E_NONE;
    if (is_dst == 1)
    {
        ip_addr_dip = ip_addr;
    }
    else
    {
        ip_addr_sip = ip_addr;
    }
    return rv;
}

/* The aim of this function is to be called from tcl to set the different actions and labels of the mpls stack.
 * It can be used for all none, partial and full termination.
 * The out_label is used only if the specified action requires an out_label - swap or push.
 * The information is saved in the array mpls_label_stack and allows up to 9 labels.
 *
 * An example for partial termination for 3 labels in the stack:
 *      * label1 with action POP, label2 with action POP, label3 with action SWAP;
 *      * label1 with action POP, label2 with action SWAP, label3 with action NOP;
 */
int set_label_action(int id, int label_in, int label_out, bcm_mpls_switch_action_t action)
{
    int rv = BCM_E_NONE;
    if (id >= 9)
    {
        printf("Incorrect mpls_label_stack ID provided - expecting number between 0 and 8.\n");
        return BCM_E_PARAM;
    }
    mpls_label_stack[id].in_label = label_in;
    if (action == BCM_MPLS_SWITCH_ACTION_SWAP || action == BCM_MPLS_SWITCH_EGRESS_ACTION_PUSH)
    {
        mpls_label_stack[id].out_label = label_out;
    }
    else
    {
        mpls_label_stack[id].out_label = -1;
    }
    mpls_label_stack[id].action = action;
    
    return rv;
}

/* This function is used to set up the ecmp group for configurations for none or partial MPLS termination.
 * A given number of interfaces in NOF_INTERFACES are created along with a tunnel initiator.
 * Their IDs are saved in an array and then they are passed when creating the ecmp group.
 * The intf Id of the created ecmp group is returned as a callback value.
 */
int create_mpls_ecmp_group(int unit, int out_label, int l3_egr_id, int out_port, int vlan, int is_flb, int *ecmp_intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t l3_eg_fec;
    int tunnel_id, ecmp_fec_id, i;
    bcm_if_t ecmp_interfaces[NOF_INTERFACES];
    
    /*
     * Create FECs (NOF_INTERFACES in number) that point to an MPLS switch tunnel.
     * Each MPLS tunnel initiator is with a different label and identifies a different ECMP path.
     */
    for(i=0; i < NOF_INTERFACES; i++)
    {
        rv = mpls_tunnel_initiate(unit, (out_label+i), l3_egr_id, &tunnel_id, BCM_MPLS_EGRESS_ACTION_SWAP);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_mpls_tunnel_initiator_create Label = 0x%x\n", (out_label+i));
            return rv;
        }

        bcm_l3_egress_t_init(&l3_eg_fec);
        l3_eg_fec.intf = tunnel_id;
        l3_eg_fec.port = out_port;
        l3_eg_fec.vlan = vlan;
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_eg_fec, &ecmp_fec_id);
        if (rv != BCM_E_NONE) {
           printf("Error, bcm_l3_egress_create\n");
           return rv;
        }
        /*This array holds information about the IDs of all created FECs.*/
        ecmp_interfaces[i] = ecmp_fec_id;
    }
    
    /* create the ECMP group. It has information about all initiated mpls tunnels - number and IDs.*/
    bcm_l3_egress_ecmp_t ecmp_group;
    bcm_l3_egress_ecmp_t_init(&ecmp_group);
    ecmp_group.max_paths = NOF_INTERFACES;
    if (is_flb == 1) /*Configure the ecmp group to be flexible.*/
    {    
        ecmp_group.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    }
    rv = bcm_l3_egress_ecmp_create(unit, &ecmp_group, NOF_INTERFACES, ecmp_interfaces);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_egress_ecmp_create - (%d)\n", rv);
        return rv;
    }
    *ecmp_intf = ecmp_group.ecmp_intf;
    return rv;
}

/* This is the function that sets the configuration for the full and partial MPLS termination.
 * It accepts:
 *       * an in_port used for the tunnel initiators;
 *       * an out_port used to create the egress interface and the ecmp group;
 *       * nof_labels - number of labels that have been configured using set_label_action;
 *       * vlan - VID;
 *       * is_flb - a boolean variable that marks user choice for FLB or CLB ecmp group (1 for FLB, 0 for CLB);
 *       * ip_version - a boolean variable used for the full termination case to mark the header under the MPLS stack.
 *              * 1 - expecting ipv4 header and therefore will create ipv4 entries.
 *              * 0 - expecting ipv6 header and will create ipv6 entries.
 * IPv4 or IPv6 entries will be created only if it's a full termination mode.
 * Full termination mode is activated only if all MPLS labels in the mpls_label_stack array (configured by calling set_label_action)
 * have an action of POP, POP_DIRECT or PHP.
 * If it's partial termination mode, IPv4/6 entries are not created.
 * Instead many MPLS tunnel switch entries are created whose label is formed by flipping a single bit in the out_label.
 * The out_label that is used for forwarding is the first out label in the mpls_label_stack array.
 */
int mpls_stack_full_partial_termination(int unit, int in_port, int out_port, int nof_labels, int vlan, int is_flb, int ip_version)
{
    int rv = BCM_E_NONE;
    int i, j, l3_egr_id, ecmp_intf;
    int invalid_egr = -1;
    int label_id, label_temp;
    int full_term = 0;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t l3_egr;

    /*Create L3 interface*/
    bcm_l3_intf_t_init(l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, ingr_mac, 6);
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_vrf = vlan;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_intf_create - %d \n", rv);
        return rv;
    }

    /*Create L3 egress*/
    bcm_l3_egress_t_init(&l3_egr);
    sal_memcpy(l3_egr.mac_addr, ingr_mac, 6);
    l3_egr.vlan   = vlan;
    l3_egr.port   = out_port;
    l3_egr.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3_egr, &l3_egr_id);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_create failed \n");
        return rv;
    }
    /* Label ID will mark a valid out_label for partial termination
     * or the last valid in_label for full termination.
     */
    label_id = get_forwarding_label_from_mpls_stack();
    if (label_id < 0)
    {
        printf("No MPLS stack information has been provided.\nPlease use set_label_action to configure this.\n");
        return BCM_E_PARAM;
    }
    if (mpls_label_stack[label_id].out_label == -1)
    {
        /* Id the forwarding label ID points to out_label -1, then this is full termination mode.*/
        full_term = 1;
    }
    rv = create_mpls_ecmp_group(unit, mpls_label_stack[label_id].out_label, l3_egr_id, out_port, vlan, is_flb, &ecmp_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_mpls_ecmp_group\n");
        return rv;
    }
    for (i = 0; i < nof_labels; i++)
    {
        rv = mpls_tunnel_switch_create(unit, mpls_label_stack[i].in_label, ecmp_intf, mpls_label_stack[i].action, in_port, mpls_label_stack[i].out_label);
        if (rv != BCM_E_NONE)
        {
            printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rv);
            return rv;
        }
        /*Add additional tunnel switches if the out_label is valid.*/
        if (mpls_label_stack[i].out_label != -1)
        {
            for(j = 0; j < 20; j++)
            {
                label_temp = flipBitInMPLSlabel(mpls_label_stack[i].in_label, j);
                rv = mpls_tunnel_switch_create(unit, label_temp, ecmp_intf, mpls_label_stack[i].action, in_port, mpls_label_stack[i].out_label);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_mpls_tunnel_switch_create\n");
                    return rv;
                }
            }
        }
    }
    /* If MPLS stack is to be fully terminated,
     * information for forwarding over the IP address under the MPLS needs to be added.
     */
    if (full_term == 1)
    {
        if (ip_version == 1)
        {
            rv = forwarding_over_ipv4_header(unit, ip_addr_dip, vlan, ecmp_intf);
        }
        else if (ip_version == 0)
        {
            rv = forwarding_over_ipv6_header(unit, vlan, ecmp_intf);
        }
        if (rv != BCM_E_NONE)
        {
            printf("Error, forwarding_over_ipv*_header \n");
            return rv;
        }
    }

    return rv;
}

/* The aim of this function is to return the last valid label in the mpls stack.
 * It will either return the ID of the first valid out_label which will be the forwarding label in partial termination case
 * or it will return the ID of the last valid in_label for full termination case.
 */
int get_forwarding_label_from_mpls_stack()
{
    int max_nof_labels = 9;
    int i;
    for (i = 0; i < max_nof_labels; i++)
    {
        if (mpls_label_stack[i].out_label != -1)
        {
            return i;
        }
        if (mpls_label_stack[i].in_label == -1)
        {
            return i-1;
        }
    }
    return -1;
}

/* This function is called to create 33 different ipv4 route entries.
 * Each entry differs with the original one by a single bit which is flipped using 
 * the function flip_bit_in_ip_addr. 
 * The result of the entry is the value of the fec_res variable.
 */
int forwarding_over_ipv4_header(int unit, bcm_ip_t ip_addr, int vlan, int fec_res)
{
    int rv = BCM_E_NONE;
    int nof_bits_ip4 = 32;
    bcm_ip_t ipv4_addr;
    int i;
    for(i = 0; i < (nof_bits_ip4 + 1); i++)
    {
        ipv4_addr = ip_addr;
        if (i < nof_bits_ip4)
        {
            ipv4_addr = flip_bit_in_ip_addr(ipv4_addr, i);
        }
        printf("IPV4 %d = %02x\n", i, ipv4_addr);
        rv = create_ip_route(unit, ipv4_addr, vlan, fec_res);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in create_ip_route \n");
            return rv;
        }
    }
    return rv;
}

/* This function is called to create 129 different ipv4 route entries.
 * Each entry (128 of 129) differs with the original one by a single bit which is flipped using 
 * the function flip_bit_in_ip6_addr.
 * The 129th entry is one using the original IP.
 * The global variable ip6_addr_dip is used as base DIP.
 * The result of the entry is the value of the fec_res variable.
 */
int forwarding_over_ipv6_header(int unit, int vlan, int fec_res)
{
    int rv = BCM_E_NONE;
    int nof_bits_ip6 = 128;
    
    bcm_ip6_t ipv6_addr;
    int i;
    for(i = 0; i < (nof_bits_ip6 + 1); i++)
    {
        ipv6_addr = ip6_addr_dip;
        if( i < nof_bits_ip6 )
        {
            ipv6_addr = flip_bit_in_ip6_addr(ipv6_addr, i);
        }
        printf("IPV6 %d = %02x%02x.%02x%02x.%02x%02x.%02x%02x.%02x%02x.%02x%02x.%02x%02x.%02x%02x\n", i, 
        ipv6_addr[0],ipv6_addr[1],ipv6_addr[2],ipv6_addr[3],ipv6_addr[4],ipv6_addr[5],ipv6_addr[6],ipv6_addr[7],
        ipv6_addr[8],ipv6_addr[9],ipv6_addr[10],ipv6_addr[11],ipv6_addr[12],ipv6_addr[13],ipv6_addr[14],ipv6_addr[15]);
        rv = create_ip6_route(unit, ipv6_addr, vlan, fec_res);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in create_ip_route \n");
            return rv;
        }
    }
    return rv;
}

/* This function creates a LAG and uses three ports for members out_port0, out_port1 and out_port2.
 * It creates 33 L2 entries with different mac addresses that differ only by a flipped bit.
 */
int lag_eth_header_hashing(int unit, int in_port, int out_port0, int out_port1, int out_port2, int vlan)
{
    int rv = BCM_E_NONE;
    bcm_l3_intf_t l3_intf;
    int i, nof_ports = 3, trunk_gport;
    bcm_trunk_t trunk_id; 
    bcm_mac_t mac_temp;
    bcm_trunk_member_t member;
    int out_ports[3] = {out_port0, out_port1, out_port2};
    bcm_if_t l3_egr_id;
    bcm_l3_egress_t l3_egr;
    
    /*Create L3 interface - In-Rif*/
    bcm_l3_intf_t_init(l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, ingr_mac, 6);
    l3_intf.l3a_vid = vlan;
    l3_intf.l3a_vrf = vlan;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l3_intf_create - %d\n", rv);
        return rv;
    }

    /* create trunk */
    rv = bcm_trunk_create(unit, 0, &trunk_id);
    if (rv != BCM_E_NONE) 
    {
        printf ("bcm_trunk_create failed: %d \n", rv);
        return rv;
    }

    /* add ports to trunk */
    rv = bcm_trunk_member_delete_all(unit, trunk_id);
    for (i = 0; i < nof_ports; i++) 
    {
        bcm_trunk_member_t_init(&member);
        BCM_GPORT_LOCAL_SET(member.gport, out_ports[i]); /* phy port to local port */
        printf("Adding trunk member: %d \n", out_ports[i]);
        rv = bcm_trunk_member_add(unit, trunk_id, &member);
        if (rv != BCM_E_NONE) 
        {
            printf ("bcm_trunk_member_add with port %d failed: %d \n", out_ports[i], rv);
            return rv;
        }
    }
    /* create FEC and send to LAG (instead of out-port) */
    /* create a trunk gport and give this gport to create_l3_egress() instead of the dest-port */
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);

    /*Create L3 egress*/
    bcm_l3_egress_t_init(&l3_egr);
    sal_memcpy(l3_egr.mac_addr, egr_mac, 6);
    l3_egr.vlan   = vlan;
    l3_egr.port   = trunk_gport;
    l3_egr.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3_egr, &l3_egr_id);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_egress_create failed \n");
        return rv;
    }
    for(i = 0; i < 49; i++)
    {
        sal_memcpy(mac_temp, ingr_mac, 6);
        flip_bit_in_mac_addr(mac_temp, i);
        rv = create_l2_mac_entry(unit, mac_temp, vlan, trunk_gport);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, in create_l2_mac_entry \n");
            return rv;
        }
    }

    return rv;
}

