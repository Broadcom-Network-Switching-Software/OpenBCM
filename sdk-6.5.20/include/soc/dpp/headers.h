/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains the various header formats for Dune SOC_SAND
 */
#include <soc/types.h>

#ifndef _SOC_DPP_HEADERS_H
#define _SOC_DPP_HEADERS_H

typedef union soc_dpp_itmh_base_u {
    struct {
        uint8   bytes[4];
    } raw;

    struct {                     
#if defined(LE_HOST)
        unsigned    snoop_cmd:4;                
        unsigned    in_mirr_dis:1;
        unsigned    out_mirr_dis:1;
        unsigned    pph_present:1;
        unsigned    version:1;
        unsigned    fwd_type:2;                 
        unsigned    dp:2;
        unsigned    traffic_class:3;
        unsigned    exclude_src:1;
#else
        unsigned    version:1;                  
        unsigned    pph_present:1;
        unsigned    out_mirr_dis:1;
        unsigned    in_mirr_dis:1;
        unsigned    snoop_cmd:4;
        unsigned    exclude_src:1;              
        unsigned    traffic_class:3;
        unsigned    dp:2;
        unsigned    fwd_type:2;
#endif 
        unsigned    __CONTAINER__:16;           
    } common;

    struct {                     
        unsigned    __COMMON_FIELDS__:16;       
#if defined(LE_HOST)
        unsigned    dest_sys_port_hi:5;         
        unsigned    _rsvd:3;
#else
        unsigned    _rsvd:3;                    
        unsigned    dest_sys_port_hi:5;         
#endif 
        unsigned    dest_sys_port_lo:8;         
    } unicast_direct;

    struct {                     
        unsigned    __COMMON_FIELDS__:16;       
#if defined(LE_HOST)
        unsigned    flow_id_hi:7;               
        unsigned    _rsvd:1;
#else
        unsigned    _rsvd:1;                    
        unsigned    flow_id_hi:7;
#endif 
        unsigned    flow_id_lo:8;               
    } unicast_flow;

    struct {                     
        unsigned    __COMMON_FIELDS__:16;       
#if defined(LE_HOST)
        unsigned    multicast_id_hi:6;          
        unsigned    _rsvd:2;                    
#else
        unsigned    _rsvd:2;                    
        unsigned    multicast_id_hi:6;            
#endif 
        unsigned    multicast_id_lo:8;          
    } sys_multicast;
#ifdef BCM_ARAD_SUPPORT
    struct {                     
#if defined(LE_HOST)
        unsigned    traffic_class_hi:1;         
        unsigned    snoop_cmd:4;
        unsigned    in_mirr_flag:1;
        unsigned    pph_type:2;
        unsigned    fwd_type:4;                 
        unsigned    dp:2;
        unsigned    traffic_class_lo:2;
#else
        unsigned    pph_type:2;                 
        unsigned    in_mirr_flag:1;
        unsigned    snoop_cmd:4;
        unsigned    traffic_class_hi:1;
        unsigned    traffic_class_lo:2;         
        unsigned    dp:2;
        unsigned    fwd_type:4;
#endif 
        unsigned    __CONTAINER__:16;           
    } arad_common;

    struct {                     
#if defined(LE_HOST)
        unsigned    fwd_type_hi:3;
        unsigned    dp:2;
        unsigned    in_mirr_flag:1;
        unsigned    pph_type:2;
        unsigned    fwd_type_lo:16;
        unsigned    itmh_base_ext:1;
        unsigned    traffic_class_lo:2;
        unsigned    traffic_class_hi:1;
        unsigned    snoop_cmd:4;
#else
        unsigned    pph_type:2;
        unsigned    in_mirr_flag:1;
        unsigned    dp:2;
        unsigned    fwd_type_hi:3;
        unsigned    fwd_type_lo:16;
        unsigned    snoop_cmd:4;
        unsigned    traffic_class_hi:1;
        unsigned    traffic_class_lo:2;
        unsigned    itmh_base_ext:1;
#endif 
        unsigned    __CONTAINER__:16;           
    } jer_common;

    struct {                       
#if defined(LE_HOST)
        unsigned    _rsvd_0:8;                  
        unsigned    dest_info_hi:4;             
        unsigned    _rsvd_1:4;
#else
        unsigned    _rsvd_0:8;       
        unsigned    _rsvd_1:4;                    
        unsigned    dest_info_hi:4;         
#endif 
        unsigned    dest_info_lo:16;            
    } dest_info;

#endif
    struct {                       
#if defined(LE_HOST)
        unsigned    dest_info_hi:3;
        unsigned    _rsvd_0:5;
        unsigned    dest_info_mi:8;
        unsigned    dest_info_lo:8;
#else
        unsigned    _rsvd_0:5;
        unsigned    dest_info_hi:3;
        unsigned    dest_info_mi:8;
        unsigned    dest_info_lo:8;
#endif 
        unsigned    _rsvd_1:8;
    } jer_dest_info;
} soc_dpp_itmh_base_t;

typedef union soc_dpp_itmh_sp_ext_u {
    struct {
        uint8   bytes[2];
    } raw;

    struct {                
#if defined(LE_HOST)
        unsigned    src_sys_port_hi:5;      
        unsigned    src_sys_port_flag:1;
        unsigned    _rsvd:2;
#else
        unsigned    _rsvd:2;                
        unsigned    src_sys_port_flag:1;
        unsigned    src_sys_port_hi:5;      
#endif 
        unsigned    src_sys_port_lo:8;      
    } system;

    struct {                
#if defined(LE_HOST)
        unsigned    _rsvd2:5;                
        unsigned    src_sys_port_flag:1;
        unsigned    _rsvd1:2;                
#else
        unsigned    _rsvd1:2;                
        unsigned    src_sys_port_flag:1;
        unsigned    _rsvd2:5;
#endif 
        unsigned    src_local_port:8;       
    } local;
}soc_dpp_itmh_sp_ext_t;

#ifdef BCM_ARAD_SUPPORT
typedef union soc_dpp_itmh_dest_ext_u {
    struct {
        uint8   bytes[3];
    } raw;

    struct {                
#if defined(LE_HOST)
        unsigned    dest_info_hi:4;              
        unsigned    _rsvd:4;                  
        unsigned    dest_info_mi:8;      
        unsigned    dest_info_lo:8;
#else
        unsigned    _rsvd:4;                
        unsigned    dest_info_hi:4;
        unsigned    dest_info_mi:8;      
        unsigned    dest_info_lo:8;
#endif 
        unsigned    src_sys_port_lo:8;      
    } dest;
}soc_dpp_itmh_dest_ext_t;
#endif

typedef struct soc_dpp_itmh_s {
    uint8                   sp_ext_valid; 
    soc_dpp_itmh_base_t     base;     
    soc_dpp_itmh_sp_ext_t   ext;      
#ifdef BCM_ARAD_SUPPORT
    soc_dpp_itmh_dest_ext_t dest_ext; 
#endif
} soc_dpp_itmh_t;

typedef enum {
    ITMH_invalid = -1,
    ITMH_version = 0,
    ITMH_pph_present,
    ITMH_out_mirr_dis,
    ITMH_in_mirr_dis,
    ITMH_snoop_cmd,
    ITMH_exclude_src,
    ITMH_traffic_class,
    ITMH_dp,
    ITMH_fwd_type,
    ITMH_dest_sys_port,
    ITMH_flow_id,
    ITMH_multicast_id,
    ITMH_src_sys_port_flag,
    ITMH_src_sys_port,
    ITMH_src_local_port,
    ITMH_COUNT
} soc_dpp_itmh_field_t;

#define SOC_ITMH_FIELD_NAMES_INIT       \
    "version",         \
    "pph_present",                      \
    "out_mirr_dis",                     \
    "in_mirr_dis",                      \
    "snoop_cmd",                        \
    "exclude_src",                      \
    "traffic_class",                    \
    "dp",                               \
    "fwd_type",                         \
    "dest_sys_port",                    \
    "flow_id",                          \
    "multicast_id",                     \
    "src_sys_port_flag",                \
    "src_sys_port",                     \
    "src_local_port",                   \
    NULL


typedef union soc_dpp_ftmh_base_u {
    struct {
        uint8   bytes[6];
    } raw;

    struct {                     
#if defined(LE_HOST)
        unsigned    packet_size_hi:6;           
        unsigned    version:2;                  
        unsigned    packet_size_lo:8;           
        unsigned    src_sys_port_hi:5;             
        unsigned    traffic_class:3;            
        unsigned    src_sys_port_lo:8;          
        unsigned    out_fap_port:8;             
        unsigned    sys_mc:1;                   
        unsigned    exclude_src:1;
        unsigned    out_mirror_disable:1;
        unsigned    pph_present:1;
        unsigned    action_type:2;
        unsigned    dp:2;                       
#else
        unsigned    version:2;                  
        unsigned    packet_size_hi:6;           
        unsigned    packet_size_lo:8;           
        unsigned    traffic_class:3;            
        unsigned    src_sys_port_hi:5;            
        unsigned    src_sys_port_lo:8;          
        unsigned    out_fap_port:8;             
        unsigned    dp:2;                       
        unsigned    action_type:2;
        unsigned    pph_present:1;
        unsigned    out_mirror_disable:1;
        unsigned    exclude_src:1;
        unsigned    sys_mc:1;
#endif 
    } fields;
} soc_dpp_ftmh_base_t;

typedef union soc_dpp_ftmh_outlif_ext_u {
    struct {
        uint8   bytes[2];
    } raw;

    struct {                     
#if defined(LE_HOST)
        unsigned    egr_multicast_id_hi:6;      
        unsigned    _rsvd:2;                    
#else
        unsigned    _rsvd:2;                    
        unsigned    egr_multicast_id_hi:6;      
#endif 
        unsigned    egr_multicast_id_lo:8;      
    } multicast_id;

    struct {                     
        unsigned    cud_hi:8;                   
        unsigned    cud_lo:8;                   
    } multicast_cud;

    struct {                     
#if defined(LE_HOST)
        unsigned    flow_id_hi:7;               
        unsigned    is_flow:1;
#else
        unsigned    is_flow:1;                  
        unsigned    flow_id_hi:7;
#endif  
        unsigned    flow_id_lo:8;               
    } unicast_flow;

    struct {                     
#if defined(LE_HOST)
        unsigned    dest_port_hi:4;             
        unsigned    _rsvd:3;
        unsigned    is_flow:1;
#else
        unsigned    is_flow:1;                  
        unsigned    _rsvd:3;
        unsigned    dest_port_hi:4;
#endif 
        unsigned    dest_port_lo:8;             
    } unicast_dest_port;
} soc_dpp_ftmh_outlif_ext_t;

typedef union soc_dpp_ftmh_mc_lb_ext_u {
    struct {
        uint8   bytes[2];
    } raw;

    struct {                     
        unsigned    bmp_h:8;                    
        unsigned    bmp_l:8;                    
    } fields;
} soc_dpp_ftmh_mc_lb_ext_t;

typedef struct soc_dpp_ftmh_s {
    uint8                       outlif_ext_valid;
    uint8                       mc_lb_ext_valid;
    soc_dpp_ftmh_base_t         base;
    soc_dpp_ftmh_outlif_ext_t   outlif_ext;
    soc_dpp_ftmh_mc_lb_ext_t    mc_lb_ext;
} soc_dpp_ftmh_t;

typedef enum {
    FTMH_invalid = -1,
    FTMH_version = 0,
    FTMH_packet_size,
    FTMH_traffic_class,
    FTMH_src_sys_port,
    FTMH_out_fap_port,
    FTMH_dp,
    FTMH_action_type,
    FTMH_pph_present,
    FTMH_out_mirror_disable,
    FTMH_exclude_src,
    FTMH_sys_mc,
    FTMH_egr_multicast_id,
    FTMH_cud,
    FTMH_is_flow,
    FTMH_flow_id,
    FTMH_dest_port,
    FTMH_bmp_h,
    FTMH_bmp_l,
    FTMH_COUNT
} soc_dpp_ftmh_field_t;

#define SOC_FTMH_FIELD_NAMES_INIT       \
    "version",         \
    "packet_size",                      \
    "traffic_class",                    \
    "src_sys_port",                     \
    "out_fap_port",                     \
    "dp",                               \
    "action_type",                      \
    "pph_present",                      \
    "out_mirror_disable",               \
    "exclude_src",                      \
    "sys_mc",                           \
    "egr_multicast_id",                 \
    "cud",                              \
    "is_flow",                          \
    "flow_id",                          \
    "dest_port",                        \
    "bmp_h",                            \
    "bmp_l",                            \
    NULL


typedef union soc_dpp_otmh_base_u {
    struct {
        uint8   bytes[2];
    } raw; 

    struct {                     
#if defined(LE_HOST)
        unsigned    dp:2;                       
        unsigned    _rsvd1:2;                   
        unsigned    multicast:1;
        unsigned    action_type:2;
        unsigned    pph_present:1;
        unsigned    version:1;
        unsigned    cud_h:2;                    
        unsigned    _rsvd3:2;
        unsigned    _class:3;
        unsigned    _rsvd2:1;
#else
        unsigned    version:1;                  
        unsigned    pph_present:1;
        unsigned    action_type:2;
        unsigned    multicast:1;
        unsigned    _rsvd1:2;
        unsigned    dp:2;
        unsigned    _rsvd2:1;                   
        unsigned    _class:3;
        unsigned    _rsvd3:2;
        unsigned    cud_h:2;
#endif 
    } fields;
} soc_dpp_otmh_base_t;

typedef union soc_dpp_otmh_outlif_ext_u {
    struct {
        uint8   bytes[2];
    } raw; 

    struct {
        unsigned    out_lif_hi:8;
        unsigned    out_lif_lo:8;
    } fields;
} soc_dpp_otmh_outlif_ext_t;

typedef union soc_dpp_otmh_sp_ext_u {
    struct {
        uint8   bytes[2];
    } raw; 

    struct {                     
#if defined(LE_HOST)
        unsigned    src_sys_port_hi:5;          
        unsigned    _rsvd:3;
#else
        unsigned    _rsvd:3;                    
        unsigned    src_sys_port_hi:5;
#endif 
        unsigned    src_sys_port_lo:8;          
    } fields;
} soc_dpp_otmh_sp_ext_t;

typedef union soc_dpp_otmh_dp_ext_u {
    struct {
        uint8   bytes[2];
    } raw; 

    struct {                     
#if defined(LE_HOST)
        unsigned    dest_sys_port_hi:5;         
        unsigned    _rsvd:3;
#else
        unsigned    _rsvd:3;                    
        unsigned    dest_sys_port_hi:5;
#endif 
        unsigned    dest_sys_port_lo:8;         
    } fields;
} soc_dpp_otmh_dp_ext_t;

typedef struct soc_dpp_otmh_s {
    uint8                       outlif_ext_valid;
    uint8                       sp_ext_valid;
    uint8                       dp_ext_valid;
    soc_dpp_otmh_base_t         base;
    soc_dpp_otmh_outlif_ext_t   outlif_ext;
    soc_dpp_otmh_sp_ext_t       sp_ext;
    soc_dpp_otmh_dp_ext_t       dp_ext;
} soc_dpp_otmh_t;

typedef enum {
    OTMH_invalid = -1,
    OTMH_version = 0,
    OTMH_pph_present,
    OTMH_action_type,
    OTMH_multicast,
    OTMH_dp,
    OTMH_class,
    OTMH_cud_h,
    OTMH_out_lif,
    OTMH_src_sys_port,
    OTMH_dest_sys_port,
    OTMH_COUNT
} soc_dpp_otmh_field_t;

#define SOC_OTMH_FIELD_NAMES_INIT       \
    "version",         \
    "pph_present",                      \
    "action_type",                      \
    "multicast",                        \
    "dp",                               \
    "class",                            \
    "cud_h",                            \
    "out_lif",                          \
    "src_sys_port",                     \
    "dest_sys_port",                    \
    NULL


typedef enum {
    DPP_HDR_none = 0,
    DPP_HDR_itmh_base,              
    DPP_HDR_itmh_sp_ext,            
    DPP_HDR_itmh_dest_ext,          
    DPP_HDR_ftmh_base,              
    DPP_HDR_ftmh_outlif_ext,        
    DPP_HDR_ftmh_mc_lb_ext,         
    DPP_HDR_ftmh_outlif_mc_lb_ext,  
    DPP_HDR_otmh_base,              
    DPP_HDR_otmh_outlif_ext,        
    DPP_HDR_otmh_sp_ext,            
    DPP_HDR_otmh_dp_ext,            
    DPP_HDR_otmh_outlif_sp_ext,     
    DPP_HDR_otmh_outlif_dp_ext,     
    DPP_HDR_otmh_outlif_sp_dp_ext,  
    DPP_HDR_COUNT
}DPP_HDR_hdr_type_t;

#define SOC_DPP_HDR_NAMES_INIT                          \
    "none",                              \
    "ITMH base",                                        \
    "ITMH src-port extension",                          \
    "ITMH destination extension",                       \
    "FTMH base",                                        \
    "FTMH OUTLIF/CUD extension",                        \
    "FTMH LB-Key extension",                            \
    "FTMH OUTLIF and LB-Key extensions",                \
    "OTMH base",                                        \
    "OTMH OUTLIF extension",                            \
    "OTMH src-port extension",                          \
    "OTMH dest-port extension",                         \
    "OTMH OUTLIF + src-port extensions",                \
    "OTMH OUTLIF + dest-port extensions",               \
    "OTMH OUTLIF + src-port + dest-port extensions",    \
    NULL


                           
#define DPP_HDR_PTCH_TYPE1_LEN              (3)
#define DPP_HDR_PTCH_TYPE2_LEN              (2)
#define DPP_HDR_ITMH_BASE_LEN               (4)
#define DPP_HDR_ITMH_SP_EXT_LEN             (6)
#define DPP_HDR_ITMH_DEST_EXT_LEN           (7)
#define DPP_HDR_FTMH_BASE_LEN               (6)
#define DPP_HDR_FTMH_OUTLIF_EXT_LEN         (8)
#define DPP_HDR_FTMH_MC_LB_EXT_LEN          (8)
#define DPP_HDR_FTMH_OUTLIF_MC_LB_EXT_LEN   (10)
#define DPP_HDR_OTMH_BASE_LEN               (2)
#define DPP_HDR_OTMH_OUTLIF_EXT_LEN         (4)
#define DPP_HDR_OTMH_SP_EXT_LEN             (4)
#define DPP_HDR_OTMH_DP_EXT_LEN             (4)
#define DPP_HDR_OTMH_OUTLIF_SP_EXT_LEN      (6)
#define DPP_HDR_OTMH_OUTLIF_DP_EXT_LEN      (6)
#define DPP_HDR_OTMH_OUTLIF_SP_DP_EXT_LEN   (8)
#define DPP_HDR_MAX_LEN                     (10) 

#define DPP_HDR_ITMH_FWD_TYPE_UNICAST_DIRECT    (0)
#define DPP_HDR_ITMH_FWD_TYPE_SYSTEM_MULTICAST  (2)
#define DPP_HDR_ITMH_FWD_TYPE_UNICAST_FLOW      (3)
#ifdef BCM_ARAD_SUPPORT
#define DPP_HDR_ITMH_FWD_TYPE_ISQ_FLOW          (4)
#define DPP_HDR_ITMH_FWD_TYPE_OUT_LIF           (5)
#define DPP_HDR_ITMH_FWD_TYPE_MULTICAST_FLOW    (6)
#define DPP_HDR_ITMH_FWD_TYPE_FEC_POINTER       (7)
#endif 

extern soc_dpp_itmh_field_t soc_dpp_itmh_name_to_field(int unit, char *name);
extern char* soc_dpp_itmh_field_to_name(int unit, soc_dpp_itmh_field_t field);
extern void soc_dpp_itmh_field_set(int unit, soc_dpp_itmh_t *itmh, 
                                   soc_dpp_itmh_field_t field, uint32 val);
extern uint32 soc_dpp_itmh_field_get(int unit, soc_dpp_itmh_t *itmh, 
                                     soc_dpp_itmh_field_t field);

extern soc_dpp_ftmh_field_t soc_dpp_ftmh_name_to_field(int unit, char *name);
extern char* soc_dpp_ftmh_field_to_name(int unit, soc_dpp_ftmh_field_t field);
extern void soc_dpp_ftmh_field_set(int unit, soc_dpp_ftmh_t *ftmh, 
                                   soc_dpp_ftmh_field_t field, uint32 val);
extern uint32 soc_dpp_ftmh_field_get(int unit, soc_dpp_ftmh_t *ftmh, 
                                     soc_dpp_ftmh_field_t field);

extern soc_dpp_otmh_field_t soc_dpp_otmh_name_to_field(int unit, char *name);
extern char* soc_dpp_otmh_field_to_name(int unit, soc_dpp_otmh_field_t field);
extern void soc_dpp_otmh_field_set(int unit, soc_dpp_otmh_t *otmh, 
                                   soc_dpp_otmh_field_t field, uint32 val);
extern uint32 soc_dpp_otmh_field_get(int unit, soc_dpp_otmh_t *otmh, 
                                     soc_dpp_otmh_field_t field);

extern int soc_dpp_hdr_len_get(int unit, DPP_HDR_hdr_type_t type);

extern DPP_HDR_hdr_type_t soc_dpp_hdr_type_get(int unit, char *name);
extern char* soc_dpp_hdr_type_to_name(int unit, DPP_HDR_hdr_type_t type);


#endif 

