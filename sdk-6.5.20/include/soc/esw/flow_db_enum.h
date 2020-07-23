	/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    flow_db_enum.h
 * Purpose: Header file for flow database module
 */
#ifndef  FLOW_DB_ENUM_H
#define FLOW_DB_ENUM_H

#define SOC_FLOW_DB_FLOW_ID_MATCH_NONE                       0
#define SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN                     1
#define SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE                     2
#define SOC_FLOW_DB_FLOW_ID_LEGACY_RESERVED                  20

/* using the absolute value insteadd of
 * SOC_FLOW_DB_FLOW_ID_LEGACY_RESERVED +1
 * since this macro is read by the parser */
#define SOC_FLOW_DB_FLOW_ID_START                            21

#define SOC_FLOW_DB_FLOW_OP_ID_START                         1
#define SOC_FLOW_DB_FLOW_OP_ANY                              "ANY"
#define SOC_FLOW_DB_FLOW_OP_MATCH_NONE                       0

#define SOC_FLOW_DB_FUNC_ID_START               0
#define SOC_FLOW_DB_FUNC_MATCH_ID               1
#define SOC_FLOW_DB_FUNC_ENCAP_ID               2
#define SOC_FLOW_DB_FUNC_TUNNEL_INITIATOR_ID    3
#define SOC_FLOW_DB_FUNC_TUNNEL_TERMINATOR_ID   4
#define SOC_FLOW_DB_FUNC_ENCAP_SET_ID           5
#define SOC_FLOW_DB_FUNC_L2_SWITCH_ID           6
#define SOC_FLOW_DB_FUNC_L3_HOST_ID             7
#define SOC_FLOW_DB_FUNC_L3_ROUTE_ID            8
#define SOC_FLOW_DB_FUNC_EGRESS_OBJ_ID          9
#define SOC_FLOW_DB_FUNC_EGRESS_LABEL_ID        10
#define SOC_FLOW_DB_FUNC_EGRESS_INTF_ID         11
#define SOC_FLOW_DB_FUNC_SGPP_CONTROL_ID       12
#define SOC_FLOW_DB_FUNC_ID_END                 13

#define SOC_FLOW_DB_FIELD_FLAG_VIRTUAL  0x80000000
#define SOC_FLOW_DB_FIELD_FLAG_FRAGMENT 0x40000000

#define BCM_FUNCTION_NAME_INITIALIZER \
{ \
    "START",\
    "MATCH",\
    "ENCAP",\
    "TUNNEL_INITIATOR",\
    "TUNNEL_TERMINATOR",\
    "ENCAP_SET",\
    "L2_SWITCH",\
    "L3_HOST",\
    "L3_ROUTE",\
    "EGRESS_OBJ",\
    "EGRESS_LABEL",\
    "EGRESS_INTF",\
    "SGPP_CONTROL",\
    "END",\
}


#define SOC_FLOW_DB_START_FL_TBL_CHUNK 0xAABBCC01
#define SOC_FLOW_DB_END_FL_TBL_CHUNK   0xAABBCC02
#define SOC_FLOW_DB_START_FL_OP_TBL_CHUNK 0xAABBCC03
#define SOC_FLOW_DB_END_FL_OP_TBL_CHUNK   0xAABBCC04
#define SOC_FLOW_DB_START_FFO_TPL_TBL_CHUNK 0xAABBCC04
#define SOC_FLOW_DB_END_FFO_TPL_TBL_CHUNK   0xAABBCC05
#define SOC_FLOW_DB_START_VW_TBL_CHUNK 0xAABBCC06
#define SOC_FLOW_DB_END_VW_TBL_CHUNK   0xAABBCC07
#define SOC_FLOW_DB_START_LG_FL_MAP_TBL_CHUNK 0xAABBCC08
#define SOC_FLOW_DB_END_LG_FL_MAP_TBL_CHUNK   0xAABBCC09

#define SOC_FLOW_DB_MAX_VIEW_FIELDS                    50
#define SOC_FLOW_DB_MAX_VIEW_FFO_TPL                   10
#define SOC_FLOW_DB_MAX_FLOW_NAME                      48
#define SOC_FLOW_DB_MAX_FLOW_OPTION_NAME               64
#define SOC_FLOW_DB_MAX_FIELD_NAME                     48
#define SOC_FLOW_DB_MAX_MEM_NAME                       48

#define SOC_FLOW_DB_KEY_TYPE_INVALID                   ((int) -1)
#define SOC_FLOW_DB_DATA_TYPE_INVALID                  ((int) - 1)
#define SOC_FLOW_DB_MAX_KEY_TYPE                        3
#define SOC_FLOW_DB_MAX_DATA_TYPE                       3
#endif /*FLOW_DB_ENUM_H*/
