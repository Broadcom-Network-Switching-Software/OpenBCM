/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* This file contains structures and functions declarations for 
* processing Packets.
 */
#ifndef _SOC_DPP_PKT_H
#define _SOC_DPP_PKT_H

#include <soc/error.h>
#include <shared/pkt.h>
#include <soc/types.h>


typedef uint8 soc_mac_t[6];

#ifdef BCM_ARAD_SUPPORT
#endif 


typedef struct  {
    _shr_pkt_dnx_itmh_dest_type_t dest_type; 
    _shr_pkt_dnx_itmh_dest_type_t dest_extension_type; 
    _shr_gport_t destination;            
    soc_multicast_t multicast_id;       
    _shr_gport_t destination_ext;        
} soc_pkt_dnx_itmh_dest_t;


typedef struct  {
    uint8 inbound_mirror_disable;   
    uint32 snoop_cmnd;              
    uint32 prio;                    
    _shr_color_t color;              
    soc_pkt_dnx_itmh_dest_t dest;   
} soc_pkt_dnx_itmh_t;

typedef struct  {
    uint8 valid;    
    uint32 lb_key;  
} soc_pkt_dnx_ftmh_lb_extension_t;


typedef struct  {
    uint8 valid;                
    _shr_gport_t dst_sysport;    
} soc_pkt_dnx_ftmh_dest_extension_t;


typedef struct  {
    uint8 valid;                    
    uint32 stack_route_history_bmp; 
} soc_pkt_dnx_ftmh_stack_extension_t;    


typedef enum soc_pkt_dnx_ase_type_e {
    socPktDnxAseTypeNone = 0,           
    socPktDnxAseTypeTrajectoryTrace = 1,
    socPktDnxAseTypeErspan = 2          
} soc_pkt_dnx_ase_type_t;


typedef struct soc_pkt_dnx_erspan_info_s {
    uint8 direction;        
    uint8 truncated_flag;   
    uint8 en;               
    uint8 cos;              
    uint16 vlan;            
} soc_pkt_dnx_erspan_info_t;


typedef struct soc_pkt_dnx_ase_info_s {
    soc_pkt_dnx_erspan_info_t ersapn_info; 
} soc_pkt_dnx_ase_info_t;


typedef struct soc_pkt_dnx_ftmh_ase_extension_s {
    uint8 valid;                        
    soc_pkt_dnx_ase_type_t ase_type;    
    soc_pkt_dnx_ase_info_t ase_info;    
} soc_pkt_dnx_ftmh_ase_extension_t;


typedef struct soc_pkt_dnx_ftmh_flow_id_extension_s {
    uint8 valid;        
    uint32 flow_id;     
    uint8 flow_profile; 
} soc_pkt_dnx_ftmh_flow_id_extension_t;


typedef struct  {
    uint32 packet_size;                 
    uint32 prio;                        
    _shr_gport_t src_sysport;            
    _shr_gport_t dst_port;               
    _shr_color_t ftmh_dp;                
    _shr_pkt_dnx_ftmh_action_type_t action_type; 
    uint8 out_mirror_disable;           
    uint8 is_mc_traffic;                
    soc_multicast_t multicast_id;       
    _shr_gport_t out_vport;              
    uint32 cni;                         
    soc_pkt_dnx_ftmh_lb_extension_t lb_ext; 
    soc_pkt_dnx_ftmh_dest_extension_t dest_ext; 
    soc_pkt_dnx_ftmh_stack_extension_t stack_ext; 
    soc_pkt_dnx_ftmh_ase_extension_t ase_ext; 
    soc_pkt_dnx_ftmh_flow_id_extension_t flow_id_ext; 
} soc_pkt_dnx_ftmh_t;



typedef struct  {
    _shr_pkt_dnx_otsh_type_t otsh_type;  
    _shr_pkt_dnx_otsh_oam_subtype_t oam_sub_type; 
    uint32 oam_up_mep;                  
    uint32 tp_cmd;                      
    uint8 ts_encap;                     
    uint64 oam_ts_data;                 
    uint32 latency_flow_ID;             
    uint32 offset;                      
} soc_pkt_dnx_otsh_t;
   
typedef struct  {
    uint8 valid;                
    _shr_gport_t src_sysport;    
} _shr_pkt_dnx_otmh_src_sysport_extension_t;


typedef struct  {
    uint8 valid;            
    _shr_gport_t out_vport;  
} soc_pkt_dnx_otmh_vport_extension_t;
typedef struct {
    _shr_pkt_dnx_ftmh_action_type_t action_type; 
    _shr_color_t ftmh_dp;                
    uint8 is_mc_traffic;                
    uint32 prio;                        
    _shr_gport_t dst_port;               
    _shr_pkt_dnx_otmh_src_sysport_extension_t src_sysport_ext; 
    soc_pkt_dnx_otmh_vport_extension_t out_vport_ext; 
} soc_pkt_dnx_otmh_t;
 

typedef struct  {
    uint32 forward_domain;
    uint32 trap_qualifier; 
    uint32 trap_id; 
} soc_pkt_dnx_internal_t;



typedef struct  soc_pkt_dnx_s {
    _shr_pkt_dnx_type_t type;    
    soc_pkt_dnx_itmh_t itmh;    
    soc_pkt_dnx_ftmh_t ftmh;    
    soc_pkt_dnx_otsh_t otsh;    
    soc_pkt_dnx_otmh_t otmh;    
    soc_pkt_dnx_internal_t internal;      
} soc_pkt_dnx_t;

typedef struct soc_pkt_s{ 
    soc_pkt_dnx_t dnx_header_stack[_SHR_PKT_NOF_DNX_HEADERS]; 
    uint8 dnx_header_count;             
} soc_pkt_t;


#endif  
