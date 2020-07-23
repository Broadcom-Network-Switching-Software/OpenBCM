/* $Id: jer_parser.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_PARSER_INCLUDED__
#define __JER_PARSER_INCLUDED__



#include <soc/dpp/ARAD/arad_parser.h>




#define     JER_SEGMENT_NOF         32
#define     JER_SEGMENT_COMPLEX_NOF 4
#define     JER_SEGMENT_COMPLEX_MASK 0x3
#define     JER_SEGMENT_FULL_SIZE   64
#define     JER_SEGMENT_HALF_SIZE   32
#define     JER_SEGMENT_SMALL_SIZE  16
#define     JER_SEGMENT_RANGE_MAX   8

#define     JER_SEGMENT_REAL_FLAG   0x100
#define     JER_SEGMENT_END         0x7F
#define     JER_SEGMENT_FIN         0

#define     JER_MACRO_NA            0x8


#define     JER_MACRO_TM            0x1
#define     JER_MACRO_TM_EXT        0x2

#define     JER_CUSTOM_MACRO_NOF    4

#define     JER_MACRO_ETH           0x4
#define     JER_MACRO_IPv4          0x5
#define     JER_MACRO_IPv6          0x6
#define     JER_MACRO_MPLS          0x7

#define     JER_IP_MC               0x20


typedef enum
{
    DPP_MACRO_NA = -1,
    DPP_MACRO_FIRST,
    DPP_MACRO_ETH = DPP_MACRO_FIRST,
    DPP_MACRO_IPv4,
    DPP_MACRO_IPv6,
    DPP_MACRO_MPLS,
    DPP_MACRO_TRILL,
    DPP_MACRO_FCoE1,
    DPP_MACRO_FCoE2,
    DPP_MACRO_VxLAN,
    DPP_MACRO_UDP,
    DPP_MACRO_IPv6Ext1,
    DPP_MACRO_IPv6Ext2,
    DPP_MACRO_ADD_TPID,
    DPP_MACRO_RAW_MPLS,
    DPP_MACRO_MPLS_CW,
    DPP_MACRO_EoE,
    DPP_MACRO_GTP,
    DPP_MACRO_UDP_TUNNEL,
    DPP_MACRO_TM,
    DPP_MACRO_TM_EXT,
    DPP_MACRO_PPPoES,
	DPP_MACRO_L2TP1,
	DPP_MACRO_L2TP2,
    DPP_MACRO_NOF
} DPP_MACRO_E;

typedef enum {
    DPP_SEGMENT_NA = -1,
    DPP_SEGMENT_FIRST,
    DPP_SEGMENT_FIN = DPP_SEGMENT_FIRST,
    DPP_SEGMENT_BASE,
    DPP_SEGMENT_TM,
    DPP_SEGMENT_RAW_MPLS,
    DPP_SEGMENT_ETH,
    DPP_SEGMENT_IPv4,
    DPP_SEGMENT_IPv6,
    DPP_SEGMENT_MPLS,
    DPP_SEGMENT_TRILL,
    DPP_SEGMENT_EoTRILL,
    DPP_SEGMENT_FCoE1,
    DPP_SEGMENT_FCoE2,
    DPP_SEGMENT_FCoE3,
    DPP_SEGMENT_MinM,
    DPP_SEGMENT_MPLSoMPLS3,
    DPP_SEGMENT_IPv4oMPLS1,
    DPP_SEGMENT_IPv4oMPLS2,
    DPP_SEGMENT_IPv4oMPLS3,
    DPP_SEGMENT_IPv4oMPLS4P,
    DPP_SEGMENT_UDPoIPv4oMPLS1,
    DPP_SEGMENT_UDPoIPv4oMPLS2,
    DPP_SEGMENT_UDPoIPv4oMPLS3,
    DPP_SEGMENT_IPv6oMPLS1,
    DPP_SEGMENT_IPv6oMPLS2,
    DPP_SEGMENT_IPv6oMPLS3,
    DPP_SEGMENT_IPv6oMPLS4P,
    DPP_SEGMENT_CWoMPLS1,
    DPP_SEGMENT_CWoMPLS2,
    DPP_SEGMENT_CWoMPLS3,
    DPP_SEGMENT_CWoMPLS4P,
    DPP_SEGMENT_ETHoMPLS1,
    DPP_SEGMENT_ETHoMPLS2,
    DPP_SEGMENT_ETHoMPLS3,
    DPP_SEGMENT_ETHoMPLS4P,
    DPP_SEGMENT_IPv4oETHoMPLS1,
    DPP_SEGMENT_IPv4oETHoMPLS2,
    DPP_SEGMENT_IPv4oETHoMPLS3,
    DPP_SEGMENT_IPv4oETHoMPLS4P,
    DPP_SEGMENT_IPv6oETHoMPLS1,
    DPP_SEGMENT_IPv6oETHoMPLS2,
    DPP_SEGMENT_IPv6oETHoMPLS3,
    DPP_SEGMENT_IPv6oETHoMPLS4P,
    DPP_SEGMENT_IPv4oIPv4,
    DPP_SEGMENT_UDPoIPv4,
    DPP_SEGMENT_UDPoIPv4_gtp,
    DPP_SEGMENT_GTPoUDPoIPv4,
    DPP_SEGMENT_UDPoIPv4_2,
    DPP_SEGMENT_VxLAN,
    DPP_SEGMENT_UDPoIPv6,
    DPP_SEGMENT_GTPoUDPoIPv6,
    DPP_SEGMENT_ADD_TPID,
    DPP_SEGMENT_IPv6Ext1,
    DPP_SEGMENT_IPv6Ext2_IPv4,
    DPP_SEGMENT_IPv6Ext2_IPv6,
    DPP_SEGMENT_IPv6Ext2_UDP,
	DPP_SEGMENT_PPPoE_SESSION,
	DPP_SEGMENT_L2TP,
    DPP_SEGMENT_VxLAN6,
    DPP_SEGMENT_END,
    DPP_SEGMENT_COMPLEX,
    DPP_SEGMENT_NOF,
	DPP_SEGMENT_END_7F = 0x7f
} DPP_SEGMENT_E;

typedef enum
{
    DPP_CB_FIRST,
    DPP_CB_ADD_TPID = DPP_CB_FIRST,
    DPP_CB_EoE,
    DPP_CB_RAW_MPLS,
    DPP_CB_BFD_IPv4,
    DPP_CB_BFD_IPv6,
    DPP_CB_UDP,
    DPP_CB_VxLAN,
    DPP_CB_UDP_TUNNEL,
    DPP_CB_IPv6Ext,
    DPP_CB_TRILL,
    DPP_CB_FCoE,
    DPP_CB_GTP,
    DPP_CB_TM,
    DPP_CB_PPPoES,
    DPP_CB_L2TP,
    DPP_CB_NOF
} DPP_CB_E;


#define JER_PARSER_L2TP_OPTION_L_BIT_MASK				   (0x40000000)

typedef struct
{
    int offset;
    int num;
    int step;
} dpp_offset_range_t;


typedef int (*dpp_parser_cb_t) (int unit);

typedef struct
{
    dpp_offset_range_t offset_range[JER_SEGMENT_RANGE_MAX];
    DPP_MACRO_E     macro_id;
    DPP_SEGMENT_E   segment_id;
    DPP_PFC_E       pfc_sw;
    DPP_PLC_E       plc_sw;
    dpp_parser_cb_t parser_cb;
} dpp_leaf_t;

typedef struct
{
    DPP_SEGMENT_E   id;
    char *          name;
    dpp_leaf_t*     leafs;
    int             nof_leafs;
    
    int             size;
    
    int             address;
} dpp_segment_t;

typedef struct
{
    DPP_MACRO_E        id;
    char *             name;
    uint32             static_macro_sel;
    int                tm;
} dpp_macro_t;

extern DPP_SEGMENT_E segment_map[SOC_MAX_NUM_DEVICES][JER_SEGMENT_NOF];
extern DPP_SEGMENT_E segment_complex_map[SOC_MAX_NUM_DEVICES][JER_SEGMENT_NOF][JER_SEGMENT_COMPLEX_NOF];
extern DPP_MACRO_E   macro_map[SOC_MAX_NUM_DEVICES][JER_CUSTOM_MACRO_NOF];

uint32 jer_parser_init(
    int unit);

char *jer_parser_segment_string_by_sw(
    DPP_SEGMENT_E   segment_id);

int jer_parser_segment_size_by_sw(
        DPP_SEGMENT_E segment_id);

char *jer_parser_segment_string_by_base(
    int             unit,
    uint32          base);

DPP_SEGMENT_E jer_parser_segment_id_by_base(
    int unit,
    uint32 base);

char *jer_parser_macro_string_by_sw(
    DPP_MACRO_E     macro_id);

char *jer_parser_macro_string_by_hw(
    int             unit,
    uint32          macro_sel,
    int             tm_flag);

#endif 
