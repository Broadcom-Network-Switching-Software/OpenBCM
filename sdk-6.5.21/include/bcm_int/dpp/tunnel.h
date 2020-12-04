/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tunnel.h
 * Purpose:     tunnel internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_TUNNEL_H_
#define   _BCM_INT_DPP_TUNNEL_H_


/* tunnel subtype.
   For example, for a tunnel type bcmTunnelTypeL2gre, subtype l2gre_is_erspan indicate we're building an IP tunnel for ERPSAN
                                                      subtype none indicate we're building an L2gre tunnel  */
typedef enum _bcm_tunnel_subtype_e {
    _bcm_tunnel_subtype_none = 0,
    _bcm_tunnel_subtype_l2gre_is_erspan = 1, /* IP_Tunnel.size will include erspan */
    _bcm_tunnel_subtype_gre_with_lb = 2      /* bcmTunnelTypeGreAnyIn4 will include LB key */
} _bcm_tunnel_subtype_t;



int _bcm_ip_tunnel_sw_init(int unit);
int _bcm_ip_tunnel_sw_cleanup(int unit);

int bcm_petra_tunnel_initiator_data_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int vsi_param,
    int ll_eep_param,
    int *tunnel_eep
    ); /* needed only in case intf is of type EEP */

int 
bcm_petra_data_entry_set(
    int unit,
    bcm_tunnel_initiator_t *tunnel);

int 
bcm_petra_data_entry_clear(
    int unit,
    bcm_l3_intf_t *intf);

int
_bcm_petra_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel
    ); 

int 
_bcm_petra_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info) ;

int
_bcm_dpp_in_lif_tunnel_term_get(int unit, bcm_tunnel_terminator_t *tunnel_term, int lif);

/* see _bcm_petra_tunnel_ipv4_type_to_ppd comments */
int
_bcm_petra_tunnel_ipv4_type_from_ppd(int unit, 
                                     SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info, 
                                     bcm_tunnel_type_t *tunnel_type, 
                                     _bcm_tunnel_subtype_t *tunnel_subtype);


/*
 * setup template, encapsulation size and IP.protocol according to tunnel type
 * supported From jericho B0 / QAX
 */
int
_bcm_petra_tunnel_ipv4_type_to_ppd_template(int unit,
                                            bcm_tunnel_type_t tunnel_type,
                                            _bcm_tunnel_subtype_t tunnel_subtype,
                                            SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *ip_tunnel_size_protocol_template);

/* 
 * resolve the ppd tunnel type to use. 
 *  
 * For Jericho A0 and below, tunnel type is bcmTunnelType.
 * For Jericho B0 and above, tunnel type is composed of bcmTunnelType and tunnel_subtype. 
 * In most case there are no tunnel_subtype. 
 * Ex:  
 * For VxLAN:  bcmTunnelType: bcmTunnelTypeVxlan and tunnel_subtype: _bcm_tunnel_subtype_none
 * For Erspan: bcmTunnelType: bcmTunnelTypeL2Gre and tunnel_subtype: _bcm_tunnel_subtype_l2gre_is_erspan. 
 *             tunnel_subtype indicates that the IP_Tunnel.size should include erspan. 
 */   
int 
_bcm_petra_tunnel_ipv4_type_to_ppd(int unit, 
                                   bcm_tunnel_type_t tunnel_type, 
                                   _bcm_tunnel_subtype_t tunnel_subtype, 
                                   SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info, 
                                   uint32 is_replace); 

#endif /* _BCM_INT_DPP_TUNNEL_H_ */
