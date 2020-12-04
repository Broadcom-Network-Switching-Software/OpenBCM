/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_FRAMEWORK_H_INCLUDED__

#define __ARAD_API_FRAMEWORK_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>







#define ARAD_REGISTER_DEVICE                                                       (   0|ARAD_PROC_BITS)
#define ARAD_UNREGISTER_DEVICE                                                     (   1|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_FIXES_APPLY                                        (   2|ARAD_PROC_BITS)
#define ARAD_MGMT_CREDIT_WORTH_SET                                                 (   7|ARAD_PROC_BITS)
#define ARAD_MGMT_CREDIT_WORTH_GET                                                 (   8|ARAD_PROC_BITS)
#define ARAD_MGMT_CREDIT_WORTH_VERIFY                                              (   9|ARAD_PROC_BITS)
#define ARAD_CALC_ASSIGNED_REBOUNDED_CREDIT_CONF                                   (  20|ARAD_PROC_BITS)
#define ARAD_MGMT_CREDIT_WORTH_GET_UNSAFE                                          (  21|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_PHASE1                                             (  22|ARAD_PROC_BITS)
#define ARAD_MGMT_FAP_ID_SET                                                       (  23|ARAD_PROC_BITS)
#define ARAD_MGMT_FAP_ID_GET                                                       (  24|ARAD_PROC_BITS)
#define ARAD_MGMT_FAP_ID_VERIFY                                                    (  25|ARAD_PROC_BITS)
#define ARAD_MGMT_FAP_ID_SET_UNSAFE                                                (  26|ARAD_PROC_BITS)
#define ARAD_MGMT_FAP_ID_GET_UNSAFE                                                (  27|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_PHASE2                                             (  28|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_INTERFACES_SET                                                (  29|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_INTERFACES_GET                                                (  40|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_INTERFACES_VERIFY                                             (  41|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_INTERFACES_SET_UNSAFE                                         (  42|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_INTERFACES_GET_UNSAFE                                         (  43|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_SET                                        (  44|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_GET                                        (  45|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_VERIFY                                     (  46|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_SET_UNSAFE                                 (  47|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_GET_UNSAFE                                 (  48|ARAD_PROC_BITS)
#define ARAD_MGMT_ENABLE_TRAFFIC_SET                                               (  49|ARAD_PROC_BITS)
#define ARAD_MGMT_ENABLE_TRAFFIC_GET                                               (  50|ARAD_PROC_BITS)
#define ARAD_CHIP_TIME_TO_TICKS                                                    (  51|ARAD_PROC_BITS)
#define ARAD_CHIP_TICKS_TO_TIME                                                    (  52|ARAD_PROC_BITS)
#define ARAD_MGMT_DEVICE_INIT                                                      (  53|ARAD_PROC_BITS)
#define ARAD_MGMT_ENABLE_TRAFFIC_VERIFY                                            (  61|ARAD_PROC_BITS)
#define ARAD_MGMT_ENABLE_TRAFFIC_SET_UNSAFE                                        (  62|ARAD_PROC_BITS)
#define ARAD_MGMT_ENABLE_TRAFFIC_GET_UNSAFE                                        (  63|ARAD_PROC_BITS)
#define ARAD_NIF_SERDES_LINKS_PARAM_SET                                            (  64|ARAD_PROC_BITS)
#define ARAD_NIF_SERDES_LINKS_PARAM_GET                                            (  65|ARAD_PROC_BITS)
#define ARAD_NIF_SERDES_LINKS_PARAM_VERIFY                                         (  66|ARAD_PROC_BITS)
#define ARAD_NIF_SERDES_LINKS_PARAM_SET_UNSAFE                                     (  67|ARAD_PROC_BITS)
#define ARAD_NIF_SERDES_LINKS_PARAM_GET_UNSAFE                                     (  68|ARAD_PROC_BITS)
#define ARAD_NIF_SERDES_LINKS_STATUS_GET                                           (  69|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_CONF_SET                                                    (  80|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_CONF_GET                                                    (  81|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_CONF_VERIFY                                                 (  82|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_CONF_SET_UNSAFE                                             (  83|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_CONF_GET_UNSAFE                                             (  84|ARAD_PROC_BITS)
#define ARAD_NIF_XAUI_CONF_SET                                                     (  85|ARAD_PROC_BITS)
#define ARAD_NIF_XAUI_CONF_GET                                                     (  86|ARAD_PROC_BITS)
#define ARAD_NIF_XAUI_CONF_VERIFY                                                  (  87|ARAD_PROC_BITS)
#define ARAD_NIF_XAUI_CONF_SET_UNSAFE                                              (  88|ARAD_PROC_BITS)
#define ARAD_NIF_XAUI_CONF_GET_UNSAFE                                              (  89|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_CONF_SET                                                    ( 100|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_CONF_GET                                                    ( 101|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_CONF_VERIFY                                                 ( 102|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_CONF_SET_UNSAFE                                             ( 103|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_CONF_GET_UNSAFE                                             ( 104|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CONF_SET                                                    ( 105|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CONF_GET                                                    ( 106|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CONF_VERIFY                                                 ( 107|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CONF_SET_UNSAFE                                             ( 108|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CONF_GET_UNSAFE                                             ( 109|ARAD_PROC_BITS)
#define ARAD_NIF_OC768C_CONF_SET                                                   ( 120|ARAD_PROC_BITS)
#define ARAD_NIF_OC768C_CONF_GET                                                   ( 121|ARAD_PROC_BITS)
#define ARAD_NIF_OC768C_CONF_VERIFY                                                ( 122|ARAD_PROC_BITS)
#define ARAD_NIF_OC768C_CONF_SET_UNSAFE                                            ( 123|ARAD_PROC_BITS)
#define ARAD_NIF_OC768C_CONF_GET_UNSAFE                                            ( 124|ARAD_PROC_BITS)
#define ARAD_NIF_FLOW_CONTROL_CONF_SET                                             ( 125|ARAD_PROC_BITS)
#define ARAD_NIF_FLOW_CONTROL_CONF_GET                                             ( 126|ARAD_PROC_BITS)
#define ARAD_NIF_FLOW_CONTROL_CONF_VERIFY                                          ( 127|ARAD_PROC_BITS)
#define ARAD_NIF_FLOW_CONTROL_CONF_SET_UNSAFE                                      ( 128|ARAD_PROC_BITS)
#define ARAD_NIF_FLOW_CONTROL_CONF_GET_UNSAFE                                      ( 129|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_ENABLE_SET                                                  ( 140|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_ENABLE_GET                                                  ( 141|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_ENABLE_VERIFY                                               ( 142|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_ENABLE_SET_UNSAFE                                           ( 143|ARAD_PROC_BITS)
#define ARAD_NIF_LINKS_ENABLE_GET_UNSAFE                                           ( 144|ARAD_PROC_BITS)
#define ARAD_NIF_DIAG_LAST_PACKET_GET                                              ( 146|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_NIF_PORTS_SET                                        ( 147|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_NIF_PORTS_GET                                        ( 148|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_NIF_PORTS_VERIFY                                     ( 149|ARAD_PROC_BITS)
#define ARAD_NIF_DIAG_LAST_PACKET_GET_UNSAFE                                       ( 150|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_NIF_PORTS_SET_UNSAFE                                 ( 160|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_NIF_PORTS_GET_UNSAFE                                 ( 161|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_RCY_PORTS_SET                                        ( 162|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_RCY_PORTS_GET                                        ( 163|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_RCY_PORTS_VERIFY                                     ( 164|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_RCY_PORTS_SET_UNSAFE                                 ( 165|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_RCY_PORTS_GET_UNSAFE                                 ( 166|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_CPU_PORTS_SET                                        ( 167|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_CPU_PORTS_GET                                        ( 168|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_CPU_PORTS_VERIFY                                     ( 169|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_CPU_PORTS_SET_UNSAFE                                 ( 180|ARAD_PROC_BITS)
#define ARAD_NIF_FAP_PORTS_TO_CPU_PORTS_GET_UNSAFE                                 ( 181|ARAD_PROC_BITS)
#define ARAD_PORT_OPTIONS_SET                                                      ( 182|ARAD_PROC_BITS)
#define ARAD_PORT_OPTIONS_GET                                                      ( 183|ARAD_PROC_BITS)
#define ARAD_PORT_OPTIONS_VERIFY                                                   ( 184|ARAD_PROC_BITS)
#define ARAD_PORT_OPTIONS_SET_UNSAFE                                               ( 185|ARAD_PROC_BITS)
#define ARAD_PORT_OPTIONS_GET_UNSAFE                                               ( 186|ARAD_PROC_BITS)
#define ARAD_PORT_RAW_SWITCHING_CONF_SET                                           ( 187|ARAD_PROC_BITS)
#define ARAD_PORT_RAW_SWITCHING_CONF_GET                                           ( 188|ARAD_PROC_BITS)
#define ARAD_PORT_RAW_SWITCHING_CONF_VERIFY                                        ( 189|ARAD_PROC_BITS)
#define ARAD_PORT_RAW_SWITCHING_CONF_SET_UNSAFE                                    ( 200|ARAD_PROC_BITS)
#define ARAD_PORT_RAW_SWITCHING_CONF_GET_UNSAFE                                    ( 201|ARAD_PROC_BITS)
#define ARAD_PORT_STATISTICS_TAG_CONF_SET                                          ( 202|ARAD_PROC_BITS)
#define ARAD_PORT_STATISTICS_TAG_CONF_GET                                          ( 203|ARAD_PROC_BITS)
#define ARAD_PORT_STATISTICS_TAG_CONF_VERIFY                                       ( 204|ARAD_PROC_BITS)
#define ARAD_PORT_STATISTICS_TAG_CONF_SET_UNSAFE                                   ( 205|ARAD_PROC_BITS)
#define ARAD_PORT_STATISTICS_TAG_CONF_GET_UNSAFE                                   ( 206|ARAD_PROC_BITS)
#define ARAD_CPU_IF_PORT_CONF_SET                                                  ( 207|ARAD_PROC_BITS)
#define ARAD_CPU_IF_PORT_CONF_GET                                                  ( 208|ARAD_PROC_BITS)
#define ARAD_CPU_IF_PORT_CONF_VERIFY                                               ( 209|ARAD_PROC_BITS)
#define ARAD_CPU_IF_PORT_CONF_SET_UNSAFE                                           ( 220|ARAD_PROC_BITS)
#define ARAD_CPU_IF_PORT_CONF_GET_UNSAFE                                           ( 221|ARAD_PROC_BITS)
#define ARAD_EGRESS_HEADER_SET                                                     ( 228|ARAD_PROC_BITS)
#define ARAD_EGRESS_HEADER_GET                                                     ( 229|ARAD_PROC_BITS)
#define ARAD_EGRESS_HEADER_VERIFY                                                  ( 240|ARAD_PROC_BITS)
#define ARAD_EGRESS_HEADER_SET_UNSAFE                                              ( 241|ARAD_PROC_BITS)
#define ARAD_EGRESS_HEADER_GET_UNSAFE                                              ( 242|ARAD_PROC_BITS)
#define ARAD_MIRROR_INGRESS_PORT_SET                                               ( 243|ARAD_PROC_BITS)
#define ARAD_MIRROR_INGRESS_PORT_GET                                               ( 244|ARAD_PROC_BITS)
#define ARAD_MIRROR_INGRESS_PORT_VERIFY                                            ( 245|ARAD_PROC_BITS)
#define ARAD_MIRROR_INGRESS_PORT_SET_UNSAFE                                        ( 246|ARAD_PROC_BITS)
#define ARAD_MIRROR_INGRESS_PORT_GET_UNSAFE                                        ( 247|ARAD_PROC_BITS)
#define ARAD_MIRROR_EGRESS_PORT_SET                                                ( 248|ARAD_PROC_BITS)
#define ARAD_MIRROR_EGRESS_PORT_GET                                                ( 249|ARAD_PROC_BITS)
#define ARAD_MIRROR_EGRESS_PORT_VERIFY                                             ( 260|ARAD_PROC_BITS)
#define ARAD_MIRROR_EGRESS_PORT_SET_UNSAFE                                         ( 261|ARAD_PROC_BITS)
#define ARAD_MIRROR_EGRESS_PORT_GET_UNSAFE                                         ( 262|ARAD_PROC_BITS)
#define ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET                                    ( 263|ARAD_PROC_BITS)
#define ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET                                    ( 264|ARAD_PROC_BITS)
#define ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_VERIFY                                 ( 265|ARAD_PROC_BITS)
#define ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_SET_UNSAFE                             ( 266|ARAD_PROC_BITS)
#define ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_GET_UNSAFE                             ( 267|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MAP_SET                                             ( 268|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MAP_GET                                             ( 269|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MAP_VERIFY                                          ( 280|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MAP_SET_UNSAFE                                      ( 281|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MAP_GET_UNSAFE                                      ( 282|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MULTICAST_PRIORITY_MAP_SET                          ( 283|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MULTICAST_PRIORITY_MAP_SET_VERIFY                   ( 284|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MULTICAST_PRIORITY_MAP_SET_UNSAFE                   ( 285|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MULTICAST_PRIORITY_MAP_GET                          ( 286|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MULTICAST_PRIORITY_MAP_GET_VERIFY                   ( 287|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_MULTICAST_PRIORITY_MAP_GET_UNSAFE                   ( 288|ARAD_PROC_BITS)
#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET                          ( 289|ARAD_PROC_BITS) 
#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET                          ( 300|ARAD_PROC_BITS) 
#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_VERIFY                       ( 301|ARAD_PROC_BITS) 
#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_SET_UNSAFE                   ( 302|ARAD_PROC_BITS) 
#define ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_QUEUE_ID_GET_UNSAFE                   ( 303|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_SET                                     ( 304|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_GET                                     ( 305|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_VERIFY                                  ( 306|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_SET_UNSAFE                              ( 307|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_INTERDIGITATED_MODE_GET_UNSAFE                              ( 308|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_SET                                         ( 309|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_GET                                         ( 310|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_VERIFY                                      ( 311|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_SET_UNSAFE                                  ( 312|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUEUE_TO_FLOW_MAPPING_GET_UNSAFE                                  ( 313|ARAD_PROC_BITS) 
#define ARAD_IPQ_QUARTET_RESET                                                     ( 314|ARAD_PROC_BITS)
#define ARAD_IPQ_QUARTET_RESET_VERIFY                                              ( 315|ARAD_PROC_BITS)

#define ARAD_IPQ_QUARTET_RESET_UNSAFE                                              ( 329|ARAD_PROC_BITS)
#define ARAD_IPQ_K_QUARTET_RESET                                                   ( 330|ARAD_PROC_BITS)
#define ARAD_IPQ_K_QUARTET_RESET_VERIFY                                            ( 331|ARAD_PROC_BITS)
#define ARAD_IPQ_K_QUARTET_RESET_UNSAFE                                            ( 332|ARAD_PROC_BITS)

#define ARAD_IPQ_SYS_PHYSICAL_TO_DEST_PORT_SET                                     ( 339|ARAD_PROC_BITS)
#define ARAD_IPQ_SYS_PHYSICAL_TO_DEST_PORT_GET                                     ( 340|ARAD_PROC_BITS)
#define ARAD_IPQ_SYS_PHYSICAL_TO_DEST_PORT_VERIFY                                  ( 341|ARAD_PROC_BITS)
#define ARAD_IPQ_SYS_PHYSICAL_TO_DEST_PORT_SET_UNSAFE                              ( 342|ARAD_PROC_BITS)
#define ARAD_IPQ_SYS_PHYSICAL_TO_DEST_PORT_GET_UNSAFE                              ( 343|ARAD_PROC_BITS)
#define ARAD_IPQ_LOCAL_TO_SYSTEM_PORT_SET                                          ( 344|ARAD_PROC_BITS)
#define ARAD_IPQ_LOCAL_TO_SYSTEM_PORT_GET                                          ( 345|ARAD_PROC_BITS)
#define ARAD_IPQ_LOCAL_TO_SYSTEM_PORT_VERIFY                                       ( 346|ARAD_PROC_BITS)
#define ARAD_IPQ_LOCAL_TO_SYSTEM_PORT_SET_UNSAFE                                   ( 347|ARAD_PROC_BITS)
#define ARAD_IPQ_LOCAL_TO_SYSTEM_PORT_GET_UNSAFE                                   ( 348|ARAD_PROC_BITS)
#define ARAD_IPQ_LAG_GLOBAL_INFO_SET                                               ( 349|ARAD_PROC_BITS)
#define ARAD_IPQ_LAG_GLOBAL_INFO_GET                                               ( 360|ARAD_PROC_BITS)
#define ARAD_IPQ_LAG_GLOBAL_INFO_VERIFY                                            ( 361|ARAD_PROC_BITS)
#define ARAD_IPQ_LAG_GLOBAL_INFO_SET_UNSAFE                                        ( 362|ARAD_PROC_BITS)
#define ARAD_IPQ_LAG_GLOBAL_INFO_GET_UNSAFE                                        ( 363|ARAD_PROC_BITS)

#define ARAD_PORTS_LAG_ORDER_PRESERVE_SET                                          ( 369|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_ORDER_PRESERVE_GET                                          ( 380|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_ORDER_PRESERVE_VERIFY                                       ( 381|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_ORDER_PRESERVE_SET_UNSAFE                                   ( 382|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_ORDER_PRESERVE_GET_UNSAFE                                   ( 383|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_MODE_SET                                                      ( 384|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_MODE_GET                                                      ( 385|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_NOF_LAG_ENTRIES_GET_UNSAFE                                  ( 386|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_NOF_LAG_GROUPS_GET_UNSAFE                                   ( 387|ARAD_PROC_BITS)

#define ARAD_ITM_CATEGORY_RNGS_SET                                                 ( 389|ARAD_PROC_BITS)
#define ARAD_ITM_CATEGORY_RNGS_GET                                                 ( 400|ARAD_PROC_BITS)
#define ARAD_ITM_CATEGORY_RNGS_VERIFY                                              ( 401|ARAD_PROC_BITS)
#define ARAD_ITM_CATEGORY_RNGS_SET_UNSAFE                                          ( 402|ARAD_PROC_BITS)
#define ARAD_ITM_CATEGORY_RNGS_GET_UNSAFE                                          ( 403|ARAD_PROC_BITS)
#define ARAD_ITM_ADMIT_TEST_TMPLT_SET                                              ( 404|ARAD_PROC_BITS)
#define ARAD_ITM_ADMIT_TEST_TMPLT_GET                                              ( 405|ARAD_PROC_BITS)
#define ARAD_ITM_ADMIT_TEST_TMPLT_VERIFY                                           ( 406|ARAD_PROC_BITS)
#define ARAD_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE                                       ( 407|ARAD_PROC_BITS)
#define ARAD_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE                                       ( 408|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_SET                                                    ( 409|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_GET                                                    ( 420|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_VERIFY                                                 ( 421|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_SET_UNSAFE                                             ( 422|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_GET_UNSAFE                                             ( 423|ARAD_PROC_BITS)
#define ARAD_ITM_CR_DISCOUNT_SET                                                   ( 424|ARAD_PROC_BITS)
#define ARAD_ITM_CR_DISCOUNT_GET                                                   ( 425|ARAD_PROC_BITS)
#define ARAD_ITM_CR_DISCOUNT_VERIFY                                                ( 426|ARAD_PROC_BITS)
#define ARAD_ITM_CR_DISCOUNT_SET_UNSAFE                                            ( 427|ARAD_PROC_BITS)
#define ARAD_ITM_CR_DISCOUNT_GET_UNSAFE                                            ( 428|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_EXP_WQ_SET                                                   ( 429|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_EXP_WQ_GET                                                   ( 430|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_EXP_WQ_SET_UNSAFE                                            ( 431|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_EXP_WQ_VERIFY                                                ( 432|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_EXP_WQ_GET_UNSAFE                                            ( 433|ARAD_PROC_BITS)
#define ARAD_ITM_TAIL_DROP_SET                                                     ( 434|ARAD_PROC_BITS)
#define ARAD_ITM_TAIL_DROP_GET                                                     ( 435|ARAD_PROC_BITS)
#define ARAD_ITM_TAIL_DROP_SET_UNSAFE                                              ( 436|ARAD_PROC_BITS)
#define ARAD_ITM_TAIL_DROP_GET_UNSAFE                                              ( 437|ARAD_PROC_BITS)
#define ARAD_ITM_TAIL_DROP_VERIFY                                                  ( 438|ARAD_PROC_BITS)

#define ARAD_ITM_WRED_SET                                                          ( 444|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_GET                                                          ( 445|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_SET_UNSAFE                                                   ( 446|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_VERIFY                                                       ( 447|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_GET_UNSAFE                                                   ( 448|ARAD_PROC_BITS)
#define ARAD_ITM_CR_WD_SET                                                         ( 449|ARAD_PROC_BITS)
#define ARAD_ITM_CR_WD_GET                                                         ( 460|ARAD_PROC_BITS)
#define ARAD_ITM_CR_WD_VERIFY                                                      ( 461|ARAD_PROC_BITS)
#define ARAD_ITM_CR_WD_SET_UNSAFE                                                  ( 462|ARAD_PROC_BITS)
#define ARAD_ITM_CR_WD_GET_UNSAFE                                                  ( 463|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_QT_RT_CLS_SET                                                 ( 464|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_QT_RT_CLS_GET                                                 ( 465|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_QT_RT_CLS_VERIFY                                              ( 466|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_QT_RT_CLS_SET_UNSAFE                                          ( 467|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_QT_RT_CLS_GET_UNSAFE                                          ( 468|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_FC_SET                                                        ( 469|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_FC_GET                                                        ( 480|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_FC_VERIFY                                                     ( 481|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_FC_SET_UNSAFE                                                 ( 482|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_FC_GET_UNSAFE                                                 ( 483|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_SET                                                      ( 484|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GET                                                      ( 485|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_VERIFY                                                   ( 486|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_SET_UNSAFE                                               ( 487|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GET_UNSAFE                                               ( 488|ARAD_PROC_BITS)
#define ARAD_ITM_STAG_SET                                                          ( 489|ARAD_PROC_BITS)
#define ARAD_ITM_STAG_GET                                                          ( 500|ARAD_PROC_BITS)
#define ARAD_ITM_STAG_VERIFY                                                       ( 501|ARAD_PROC_BITS)
#define ARAD_ITM_STAG_SET_UNSAFE                                                   ( 502|ARAD_PROC_BITS)
#define ARAD_ITM_STAG_GET_UNSAFE                                                   ( 503|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_INFO_SET                                                    ( 504|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_INFO_GET                                                    ( 505|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_INFO_VERIFY                                                 ( 506|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_INFO_SET_UNSAFE                                             ( 507|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_INFO_GET_UNSAFE                                             ( 508|ARAD_PROC_BITS)
#define ARAD_ITM_INGRESS_SHAPE_SET                                                 ( 509|ARAD_PROC_BITS)
#define ARAD_ITM_DYN_TOTAL_THRESH_SET                                              ( 510|ARAD_PROC_BITS)
#define ARAD_ITM_DYN_TOTAL_THRESH_SET_UNSAFE                                       ( 511|ARAD_PROC_BITS)
#define ARAD_ITM_INGRESS_SHAPE_GET                                                 ( 520|ARAD_PROC_BITS)
#define ARAD_ITM_INGRESS_SHAPE_VERIFY                                              ( 521|ARAD_PROC_BITS)
#define ARAD_ITM_INGRESS_SHAPE_SET_UNSAFE                                          ( 522|ARAD_PROC_BITS)
#define ARAD_ITM_INGRESS_SHAPE_GET_UNSAFE                                          ( 523|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SET                                            ( 524|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_GET                                            ( 525|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_VERIFY                                         ( 526|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SET_UNSAFE                                     ( 527|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_GET_UNSAFE                                     ( 528|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET                                     ( 529|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET                                     ( 540|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_VERIFY                                  ( 541|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_UNSAFE                              ( 542|ARAD_PROC_BITS)
#define ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_UNSAFE                              ( 543|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_DROP_PROB_SET                                             ( 544|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_DROP_PROB_GET                                             ( 545|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_DROP_PROB_VERIFY                                          ( 546|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_DROP_PROB_SET_UNSAFE                                      ( 547|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_DROP_PROB_GET_UNSAFE                                      ( 548|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET                                 ( 549|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET                                 ( 558|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_VERIFY                              ( 559|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_UNSAFE                          ( 560|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_UNSAFE                          ( 561|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_Q_BASED_SET                                               ( 562|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_Q_BASED_GET                                               ( 563|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_Q_BASED_VERIFY                                            ( 564|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_Q_BASED_SET_UNSAFE                                        ( 565|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_Q_BASED_GET_UNSAFE                                        ( 566|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_EG_SET                                                    ( 567|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_EG_GET                                                    ( 568|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_EG_VERIFY                                                 ( 569|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_EG_SET_UNSAFE                                             ( 570|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_EG_GET_UNSAFE                                             ( 571|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_GLOB_RCS_SET                                              ( 572|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_GLOB_RCS_GET                                              ( 573|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_GLOB_RCS_VERIFY                                           ( 574|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_GLOB_RCS_SET_UNSAFE                                       ( 575|ARAD_PROC_BITS)
#define ARAD_ITM_SYS_RED_GLOB_RCS_GET_UNSAFE                                       ( 576|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_TEST_TMPLT_SET                                              ( 577|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_TEST_TMPLT_GET                                              ( 578|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_TEST_TMPLT_SET_UNSAFE                                       ( 579|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_TEST_TMPLT_VERIFY                                           ( 580|ARAD_PROC_BITS)
#define ARAD_ITM_QUEUE_TEST_TMPLT_GET_UNSAFE                                       ( 581|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_SET                                                 ( 582|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_GET                                                 ( 583|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_DEFAULT_GET                                         ( 584|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_SET_UNSAFE                                          ( 585|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_GET_UNSAFE                                          ( 586|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_VERIFY                                              ( 587|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_TAIL_DROP_DEFAULT_GET_UNSAFE                                  ( 588|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GEN_SET                                                  ( 589|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GEN_GET                                                  ( 590|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GEN_SET_UNSAFE                                           ( 591|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GEN_VERIFY                                               ( 592|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GEN_GET_UNSAFE                                           ( 593|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_FC_SET                                                   ( 594|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_FC_GET                                                   ( 595|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_DROP_SET                                                 ( 596|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_DROP_GET                                                 ( 597|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_FC_SET_UNSAFE                                            ( 598|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_FC_VERIFY                                                ( 599|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_FC_GET_UNSAFE                                            ( 600|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_DROP_SET_UNSAFE                                          ( 601|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_DROP_VERIFY                                              ( 602|ARAD_PROC_BITS)
#define ARAD_ITM_GLOB_RCS_DROP_GET_UNSAFE                                          ( 603|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_MESH_SET                                            ( 611|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_MESH_GET                                            ( 612|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_MESH_VERIFY                                         ( 613|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_MESH_SET_UNSAFE                                     ( 614|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_MESH_GET_UNSAFE                                     ( 615|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_SET                                            ( 616|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_GET                                            ( 617|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_VERIFY                                         ( 618|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_SET_UNSAFE                                     ( 619|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_GET_UNSAFE                                     ( 620|ARAD_PROC_BITS)
#define ARAD_EGRESS_SCH_QUEUE_TYPE_SET                                             ( 621|ARAD_PROC_BITS)
#define ARAD_EGRESS_SCH_QUEUE_TYPE_GET                                             ( 622|ARAD_PROC_BITS)
#define ARAD_EGRESS_SCH_QUEUE_TYPE_VERIFY                                          ( 623|ARAD_PROC_BITS)
#define ARAD_EGRESS_SCH_QUEUE_TYPE_SET_UNSAFE                                      ( 624|ARAD_PROC_BITS)
#define ARAD_EGRESS_SCH_QUEUE_TYPE_GET_UNSAFE                                      ( 625|ARAD_PROC_BITS)
#define ARAD_EGRESS_QUEUES_CONF_SET                                                ( 626|ARAD_PROC_BITS)
#define ARAD_EGRESS_QUEUES_CONF_GET                                                ( 627|ARAD_PROC_BITS)
#define ARAD_EGRESS_QUEUES_CONF_VERIFY                                             ( 628|ARAD_PROC_BITS)
#define ARAD_EGRESS_QUEUES_CONF_SET_UNSAFE                                         ( 629|ARAD_PROC_BITS)
#define ARAD_EGRESS_QUEUES_CONF_GET_UNSAFE                                         ( 630|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_RATE_ENTRY_SET                                             ( 631|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_RATE_ENTRY_GET                                             ( 640|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_RATE_ENTRY_VERIFY                                          ( 641|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_RATE_ENTRY_SET_UNSAFE                                      ( 642|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_RATE_ENTRY_GET_UNSAFE                                      ( 643|ARAD_PROC_BITS)
#define ARAD_SCH_EGRESS_PORTS_QOS_SET                                              ( 669|ARAD_PROC_BITS)
#define ARAD_SCH_EGRESS_PORTS_QOS_GET                                              ( 680|ARAD_PROC_BITS)
#define ARAD_SCH_EGRESS_PORTS_QOS_VERIFY                                           ( 681|ARAD_PROC_BITS)
#define ARAD_SCH_EGRESS_PORTS_QOS_SET_UNSAFE                                       ( 682|ARAD_PROC_BITS)
#define ARAD_SCH_EGRESS_PORTS_QOS_GET_UNSAFE                                       ( 683|ARAD_PROC_BITS)
#define ARAD_SCH_ONE_EGRESS_PORT_QOS_SET                                           ( 684|ARAD_PROC_BITS)
#define ARAD_SCH_ONE_EGRESS_PORT_QOS_GET                                           ( 685|ARAD_PROC_BITS)
#define ARAD_SCH_ONE_EGRESS_PORT_QOS_VERIFY                                        ( 686|ARAD_PROC_BITS)
#define ARAD_SCH_ONE_EGRESS_PORT_QOS_SET_UNSAFE                                    ( 687|ARAD_PROC_BITS)
#define ARAD_SCH_ONE_EGRESS_PORT_QOS_GET_UNSAFE                                    ( 688|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_SET                                             ( 689|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_GET                                             ( 700|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_VERIFY                                          ( 701|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_SET_UNSAFE                                      ( 702|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_GET_UNSAFE                                      ( 703|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_SET                                       ( 704|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_GET                                       ( 705|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_VERIFY                                    ( 706|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_SET_UNSAFE                                ( 707|ARAD_PROC_BITS)
#define ARAD_SCH_CLASS_TYPE_PARAMS_TABLE_GET_UNSAFE                                ( 708|ARAD_PROC_BITS)
#define ARAD_SCH_SLOW_MAX_RATES_SET                                                ( 709|ARAD_PROC_BITS)
#define ARAD_SCH_SLOW_MAX_RATES_GET                                                ( 720|ARAD_PROC_BITS)
#define ARAD_SCH_SLOW_MAX_RATES_VERIFY                                             ( 721|ARAD_PROC_BITS)
#define ARAD_SCH_SLOW_MAX_RATES_SET_UNSAFE                                         ( 722|ARAD_PROC_BITS)
#define ARAD_SCH_SLOW_MAX_RATES_GET_UNSAFE                                         ( 723|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_SET                                                          ( 724|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_GET                                                          ( 725|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_VERIFY                                                       ( 726|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_SET_UNSAFE                                                   ( 727|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_GET_UNSAFE                                                   ( 728|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_SCHED_VERIFY                                                 ( 729|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_SCHED_SET_UNSAFE                                             ( 730|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_SCHED_GET_UNSAFE                                             ( 731|ARAD_PROC_BITS)

#define ARAD_SCH_AGGREGATE_SET                                                     ( 749|ARAD_PROC_BITS)
#define ARAD_SCH_AGGREGATE_GROUP_SET                                               ( 750|ARAD_PROC_BITS)
#define ARAD_SCH_AGGREGATE_GET                                                     ( 760|ARAD_PROC_BITS)
#define ARAD_SCH_AGGREGATE_VERIFY                                                  ( 761|ARAD_PROC_BITS)
#define ARAD_SCH_AGGREGATE_SET_UNSAFE                                              ( 762|ARAD_PROC_BITS)
#define ARAD_SCH_AGGREGATE_GET_UNSAFE                                              ( 763|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_DELETE_UNSAFE                                                ( 764|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_DELETE                                                       ( 765|ARAD_PROC_BITS)
#define ARAD_SCH_AGGREGATE_GROUP_SET_UNSAFE                                        ( 766|ARAD_PROC_BITS)

#define ARAD_MULT_ING_TRAFFIC_CLASS_MAP_SET                                        ( 800|ARAD_PROC_BITS)
#define ARAD_MULT_ING_TRAFFIC_CLASS_MAP_GET                                        ( 801|ARAD_PROC_BITS)
#define ARAD_MULT_ING_TRAFFIC_CLASS_MAP_VERIFY                                     ( 802|ARAD_PROC_BITS)
#define ARAD_MULT_ING_TRAFFIC_CLASS_MAP_SET_UNSAFE                                 ( 803|ARAD_PROC_BITS)
#define ARAD_MULT_ING_TRAFFIC_CLASS_MAP_GET_UNSAFE                                 ( 804|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_OPEN                                                   ( 805|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_UPDATE                                                 ( 806|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_CLOSE                                                  ( 807|ARAD_PROC_BITS)
#define ARAD_MULT_ING_DESTINATION_ADD                                              ( 808|ARAD_PROC_BITS)
#define ARAD_MULT_ING_DESTINATION_REMOVE                                           ( 809|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_SIZE_GET                                               ( 810|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GET_GROUP                                                    ( 811|ARAD_PROC_BITS)
#define ARAD_MULT_ING_ALL_GROUPS_CLOSE                                             ( 812|ARAD_PROC_BITS)
#define ARAD_MULT_DOES_GROUP_EXIST                                                 ( 813|ARAD_PROC_BITS)

#define ARAD_MULT_CUD_TO_PORT_MAP_SET                                              ( 814|ARAD_PROC_BITS)
#define ARAD_MULT_CUD_TO_PORT_MAP_GET                                              ( 815|ARAD_PROC_BITS)
#define ARAD_MULT_CUD_TO_PORT_MAP_SET_UNSAFE                                       ( 816|ARAD_PROC_BITS)
#define ARAD_MULT_CUD_TO_PORT_MAP_GET_UNSAFE                                       ( 817|ARAD_PROC_BITS)
#define ARAD_MULT_CUD_TO_PORT_MAP_SET_VERIFY                                       ( 818|ARAD_PROC_BITS)
#define ARAD_MULT_CUD_TO_PORT_MAP_GET_VERIFY                                       ( 819|ARAD_PROC_BITS)

#define ARAD_MULT_ING_GROUP_OPEN_UNSAFE                                            ( 820|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_UPDATE_UNSAFE                                          ( 821|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_CLOSE_UNSAFE                                           ( 822|ARAD_PROC_BITS)
#define ARAD_MULT_ING_DESTINATION_ADD_UNSAFE                                       ( 823|ARAD_PROC_BITS)
#define ARAD_MULT_ING_DESTINATION_REMOVE_UNSAFE                                    ( 824|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_SIZE_GET_UNSAFE                                        ( 825|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GET_GROUP_UNSAFE                                             ( 826|ARAD_PROC_BITS)
#define ARAD_MULT_ING_ALL_GROUPS_CLOSE_UNSAFE                                      ( 827|ARAD_PROC_BITS)

#define ARAD_MULT_EG_VLAN_MEMBERSHIP_MULTICAST_ID_NDX_VERIFY                       ( 828|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET                               ( 829|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET                               ( 830|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_VERIFY                            ( 831|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_SET_UNSAFE                        ( 832|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_GET_UNSAFE                        ( 833|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_SET                                                     ( 839|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_UPDATE                                                  ( 840|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_CLOSE                                                   ( 841|ARAD_PROC_BITS)
#define ARAD_MULT_EG_PORT_ADD                                                      ( 842|ARAD_PROC_BITS)
#define ARAD_MULT_EG_PORT_REMOVE                                                   ( 843|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_SIZE_GET                                                ( 844|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GET_GROUP                                                     ( 845|ARAD_PROC_BITS)
#define ARAD_MULT_EG_ALL_GROUPS_CLOSE                                              ( 846|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN                                    ( 847|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE                                  ( 848|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE                                   ( 849|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD                                      ( 850|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE                                   ( 851|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET                                     ( 852|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE                              ( 853|ARAD_PROC_BITS)

#define ARAD_MULT_EG_GROUP_SET_UNSAFE                                              ( 854|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_UPDATE_UNSAFE                                           ( 855|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_CLOSE_UNSAFE                                            ( 856|ARAD_PROC_BITS)
#define ARAD_MULT_EG_PORT_ADD_UNSAFE                                               ( 857|ARAD_PROC_BITS)
#define ARAD_MULT_EG_PORT_REMOVE_UNSAFE                                            ( 858|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_SIZE_GET_UNSAFE                                         ( 859|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GET_GROUP_UNSAFE                                              ( 860|ARAD_PROC_BITS)
#define ARAD_MULT_EG_ALL_GROUPS_CLOSE_UNSAFE                                       ( 861|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_OPEN_UNSAFE                             ( 862|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_UPDATE_UNSAFE                           ( 863|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_CLOSE_UNSAFE                            ( 864|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE                               ( 865|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE                            ( 866|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_GROUP_GET_UNSAFE                              ( 867|ARAD_PROC_BITS)
#define ARAD_MULT_EG_VLAN_MEMBERSHIP_ALL_GROUPS_CLOSE_UNSAFE                       ( 868|ARAD_PROC_BITS)

#define ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET                    ( 869|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET                    ( 870|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_VERIFY                 ( 871|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE             ( 872|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE             ( 873|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_BASE_QUEUE_SET                                            ( 874|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_BASE_QUEUE_GET                                            ( 875|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_BASE_QUEUE_VERIFY                                         ( 876|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_BASE_QUEUE_SET_UNSAFE                                     ( 877|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_BASE_QUEUE_GET_UNSAFE                                     ( 878|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_SET                                         ( 879|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_GET                                         ( 880|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_SET_UNSAFE                                  ( 881|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_GET_UNSAFE                                  ( 882|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ACTIVE_LINKS_SET                                          ( 883|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ACTIVE_LINKS_GET                                          ( 884|ARAD_PROC_BITS)

#define ARAD_FABRIC_LINKS_SERDES_PARAM_SET                                         ( 885|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_SERDES_PARAM_GET                                         ( 886|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_SERDES_PARAM_VERIFY                                      ( 887|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_SERDES_PARAM_SET_UNSAFE                                  ( 888|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_SERDES_PARAM_GET_UNSAFE                                  ( 889|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_LOGIC_CONF_SET                                           ( 890|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_LOGIC_CONF_GET                                           ( 891|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_LOGIC_CONF_VERIFY                                        ( 892|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_LOGIC_CONF_SET_UNSAFE                                    ( 893|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINKS_LOGIC_CONF_GET_UNSAFE                                    ( 894|ARAD_PROC_BITS)
#define ARAD_FABRIC_MODE_SET                                                       ( 895|ARAD_PROC_BITS)
#define ARAD_FABRIC_MODE_GET                                                       ( 896|ARAD_PROC_BITS)
#define ARAD_FABRIC_MODE_VERIFY                                                    ( 897|ARAD_PROC_BITS)
#define ARAD_FABRIC_MODE_SET_UNSAFE                                                ( 898|ARAD_PROC_BITS)
#define ARAD_FABRIC_MODE_GET_UNSAFE                                                ( 899|ARAD_PROC_BITS)
#define ARAD_FABRIC_STAND_ALONE_FAP_MODE_SET                                       ( 900|ARAD_PROC_BITS)
#define ARAD_FABRIC_STAND_ALONE_FAP_MODE_GET                                       ( 920|ARAD_PROC_BITS)
#define ARAD_FABRIC_STAND_ALONE_FAP_MODE_VERIFY                                    ( 921|ARAD_PROC_BITS)
#define ARAD_FABRIC_STAND_ALONE_FAP_MODE_SET_UNSAFE                                ( 922|ARAD_PROC_BITS)
#define ARAD_FABRIC_STAND_ALONE_FAP_MODE_GET_UNSAFE                                ( 923|ARAD_PROC_BITS)
#define ARAD_FABRIC_STAND_ALONE_FAP_MODE_DETECT                                    ( 924|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_HANDLER                                                     ( 925|ARAD_PROC_BITS)
#define ARAD_STATISTICS_IF_INFO_SET                                                ( 926|ARAD_PROC_BITS)
#define ARAD_STATISTICS_IF_INFO_GET                                                ( 927|ARAD_PROC_BITS)
#define ARAD_STATISTICS_IF_INFO_VERIFY                                             ( 928|ARAD_PROC_BITS)
#define ARAD_STATISTICS_IF_INFO_SET_UNSAFE                                         ( 929|ARAD_PROC_BITS)
#define ARAD_STATISTICS_IF_INFO_GET_UNSAFE                                         ( 940|ARAD_PROC_BITS)
#define ARAD_SEND_SR_DATA_CELL                                                     ( 941|ARAD_PROC_BITS)
#define ARAD_RECV_SR_DATA_CELL                                                     ( 942|ARAD_PROC_BITS)
#define ARAD_SEND_CONTROL_CELL                                                     ( 943|ARAD_PROC_BITS)
#define ARAD_PACKET_SEND                                                           ( 944|ARAD_PROC_BITS)
#define ARAD_PACKET_RECV                                                           ( 945|ARAD_PROC_BITS)
#define ARAD_SSR_GET_BUFF_SIZE                                                     ( 949|ARAD_PROC_BITS)
#define ARAD_SSR_TO_BUFF                                                           ( 960|ARAD_PROC_BITS)
#define ARAD_SSR_FROM_BUFF                                                         ( 961|ARAD_PROC_BITS)
#define ARAD_REGISTER_CALLBACK_FUNCTION                                            ( 962|ARAD_PROC_BITS)
#define ARAD_UNREGISTER_CALLBACK_FUNCTION                                          ( 963|ARAD_PROC_BITS)
#define ARAD_UNREGISTER_ALL_CALLBACK_FUNCTIONS                                     ( 964|ARAD_PROC_BITS)
#define ARAD_SRD_VERIFY_ALL                                                        (1154|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATUS_GET_UNSAFE                                            (1158|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_SET_UNSAFE                                                (1159|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_VERIFY                                                    (1160|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_GET_UNSAFE                                                (1161|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POLARITY_SET_UNSAFE                                          (1162|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POLARITY_VERIFY                                              (1163|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POLARITY_GET_UNSAFE                                          (1164|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_DIVISOR_VERIFY                                               (1165|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_CMU_SET_UNSAFE                                            (1166|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_CMU_VERIFY                                                (1167|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_CMU_GET_UNSAFE                                            (1168|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATUS_INFO_GET                                              (1169|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POLARITY_SET                                                 (1171|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POLARITY_GET                                                 (1172|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_SET_UNSAFE                                                (1173|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_VERIFY                                                    (1174|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_GET_UNSAFE                                                (1175|ARAD_PROC_BITS)
#define ARAD_SRD_SET_ALL_UNSAFE                                                    (1176|ARAD_PROC_BITS)
#define ARAD_SRD_GET_ALL_UNSAFE                                                    (1177|ARAD_PROC_BITS)
#define ARAD_SRD_PRINT_ALL_UNSAFE                                                  (1178|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_SYNC_UNSAFE                                                  (1179|ARAD_PROC_BITS)
#define ARAD_SRD_REF_CLOCK_SET                                                     (1180|ARAD_PROC_BITS)
#define ARAD_SRD_REF_CLOCK_GET                                                     (1181|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_CMU_SET                                                   (1182|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_CMU_GET                                                   (1183|ARAD_PROC_BITS)
#define ARAD_SRD_SET_ALL                                                           (1184|ARAD_PROC_BITS)
#define ARAD_SRD_GET_ALL                                                           (1185|ARAD_PROC_BITS)
#define ARAD_SRD_PRINT_ALL                                                         (1186|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_SYNC                                                         (1187|ARAD_PROC_BITS)
#define ARAD_DEST_SYS_PORT_INFO_VERIFY                                             (1188|ARAD_PROC_BITS)
#define ARAD_DEST_INFO_VERIFY                                                      (1189|ARAD_PROC_BITS)
#define ARAD_SCH_IF_SHAPER_RATE_SET                                                (1190|ARAD_PROC_BITS)
#define ARAD_SCH_IF_SHAPER_RATE_GET                                                (1191|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_IDX_SET                                          (1192|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_IDX_GET                                          (1193|ARAD_PROC_BITS)
#define ARAD_SCH_IF_WEIGHT_CONF_SET                                                (1194|ARAD_PROC_BITS)
#define ARAD_SCH_IF_WEIGHT_CONF_GET                                                (1195|ARAD_PROC_BITS)
#define ARAD_SCH_IF_SHAPER_RATE_VERIFY                                             (1196|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_IDX_SET_UNSAFE                                   (1197|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_IDX_VERIFY                                       (1198|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_IDX_GET_UNSAFE                                   (1199|ARAD_PROC_BITS)
#define ARAD_SCH_IF_WEIGHT_CONF_SET_UNSAFE                                         (1200|ARAD_PROC_BITS)
#define ARAD_SCH_IF_WEIGHT_CONF_VERIFY                                             (1201|ARAD_PROC_BITS)
#define ARAD_SCH_IF_WEIGHT_CONF_GET_UNSAFE                                         (1202|ARAD_PROC_BITS)
#define ARAD_INTERFACE_ID_VERIFY                                                   (1203|ARAD_PROC_BITS)
#define ARAD_SW_DB_DEVICE_SCH_INITIALIZE                                           (1204|ARAD_PROC_BITS)
#define ARAD_SW_DB_DEVICE_INIT                                                     (1205|ARAD_PROC_BITS)
#define ARAD_SW_DB_DEVICE_CLOSE                                                    (1206|ARAD_PROC_BITS)
#define ARAD_SW_DB_INIT                                                            (1207|ARAD_PROC_BITS)
#define ARAD_FAP_PORT_ID_VERIFY                                                    (1208|ARAD_PROC_BITS)
#define ARAD_DROP_PRECEDENCE_VERIFY                                                (1209|ARAD_PROC_BITS)
#define ARAD_TRAFFIC_CLASS_VERIFY                                                  (1210|ARAD_PROC_BITS)
#define ARAD_SW_DB_DEV_EGR_PORTS_INITIALIZE                                        (1211|ARAD_PROC_BITS)
#define ARAD_SW_DB_MULTICAST_INITIALIZE                                            (1212|ARAD_PROC_BITS)

#define ARAD_SRD_LANE_REG_WRITE                                                    (1213|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_REG_WRITE_UNSAFE                                             (1214|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_REG_READ                                                     (1215|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_REG_READ_UNSAFE                                              (1216|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_RATE_FACTORS_SET                                          (1217|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_RATE_FACTORS_SET_UNSAFE                                   (1218|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_RATE_FACTORS_GET                                          (1219|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_RATE_FACTORS_VERIFY                                       (1220|ARAD_PROC_BITS)
#define ARAD_SRD_QUARTET_RATE_FACTORS_GET_UNSAFE                                   (1221|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATE_SET                                                    (1222|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATE_SET_UNSAFE                                             (1223|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATE_GET                                                    (1224|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATE_VERIFY                                                 (1225|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_STATE_GET_UNSAFE                                             (1226|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_DIVISOR_SET                                                  (1227|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_DIVISOR_SET_UNSAFE                                           (1228|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_DIVISOR_GET                                                  (1229|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_DIVISOR_VERIFY                                               (1230|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_DIVISOR_GET_UNSAFE                                           (1231|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_SET                                          (1232|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_SET_UNSAFE                                   (1233|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_GET                                          (1234|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_VERIFY                                       (1235|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_GET_UNSAFE                                   (1236|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_SET                                                       (1237|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_GET                                                       (1238|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_REG_WRITE                                                     (1239|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_REG_WRITE_UNSAFE                                              (1240|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_REG_READ                                                      (1241|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_REG_READ_UNSAFE                                               (1242|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_INIT                                                          (1243|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_INIT_UNSAFE                                                   (1244|ARAD_PROC_BITS)
#define ARAD_SRD_CMU_REG_WRITE                                                     (1245|ARAD_PROC_BITS)
#define ARAD_SRD_CMU_REG_WRITE_UNSAFE                                              (1246|ARAD_PROC_BITS)
#define ARAD_SRD_CMU_REG_READ                                                      (1247|ARAD_PROC_BITS)
#define ARAD_SRD_CMU_REG_READ_UNSAFE                                               (1248|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_FACTORS_CALCULATE                                            (1249|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_FACTORS_CALCULATE_UNSAFE                                     (1250|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_INFO_CALCULATE                                      (1251|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_INFO_CALCULATE_UNSAFE                               (1252|ARAD_PROC_BITS)
#define ARAD_SRD_LINK_RX_EYE_MONITOR                                               (1253|ARAD_PROC_BITS)
#define ARAD_SRD_LINK_RX_EYE_MONITOR_UNSAFE                                        (1254|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_LOOPBACK_MODE_SET                                            (1255|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_LOOPBACK_MODE_SET_UNSAFE                                     (1256|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_LOOPBACK_MODE_GET                                            (1257|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_LOOPBACK_MODE_VERIFY                                         (1258|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_LOOPBACK_MODE_GET_UNSAFE                                     (1259|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_SET                                                          (1262|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_SET_UNSAFE                                                   (1263|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_GET                                                          (1264|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_GET_UNSAFE                                                   (1265|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_VERIFY                                                       (1266|ARAD_PROC_BITS)

#define ARAD_SRD_REGS_FIELD_FROM_REG_SET                                           (1270|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_FIELD_FROM_REG_GET                                           (1271|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_WRITE_REG_UNSAFE                                             (1272|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_WRITE_REG                                                    (1273|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_READ_REG                                                     (1274|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_WRITE_FLD                                                    (1275|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_READ_FLD                                                     (1276|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_SET_DO_PRINTS                                                (1277|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_SCIF_CNTRL                                                   (1278|ARAD_PROC_BITS)

#define ARAD_SRD_LLA_RDWR_VERIFY                                                   (1279|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_SET_DO_PRINTS                                                 (1280|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_WRITE                                                         (1281|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_READ                                                          (1282|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_EARAD_BITSTREAM_TO_EARAD_CMD                                      (1283|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_EARAD_EARAD_CMD_TO_BITSTREAM                                      (1284|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_EARAD_CMD_WRITE                                                 (1285|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_EARAD_CMD_READ                                                  (1286|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_EARAD_WRITE                                                     (1287|ARAD_PROC_BITS)
#define ARAD_SRD_LLA_EARAD_READ                                                      (1288|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_ACTUAL_ID_GET                                                (1289|ARAD_PROC_BITS)
#define ARAD_AQFM_AGGS_DEFAULT_GET                                                 (1290|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_INFO_ITEM_SET                                                (1291|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_INFO_ITEM_GET                                                (1292|ARAD_PROC_BITS)
#define ARAD_AQFM_AGGS_ACTUAL_IDS_SET                                              (1293|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_AGGREGATES_OPEN_UNSAFE                                       (1294|ARAD_PROC_BITS)
#define ARAD_AQFM_CLOSE_PORT_AGGS                                                  (1295|ARAD_PROC_BITS)
#define ARAD_AQFM_UPDATE_PORT_AGGS                                                 (1296|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_PORT_SCHEME_AGGREGATES_PRINT                                 (1297|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOWS_GET_DEFAULT                                                (1298|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOW_SET_PHYSICAL_IDS                                            (1299|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOW_PORT_FLOWS_OPEN                                             (1301|ARAD_PROC_BITS)
#define ARAD_AQFM_CLOSE_PORT_FLOWS                                                 (1302|ARAD_PROC_BITS)
#define ARAD_AQFM_UPDATE_PORT_FLOWS                                                (1303|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_FLOWS_OPEN_UNSAFE                                            (1304|ARAD_PROC_BITS)
#define ARAD_API_AUTO_PRINT_PORT_SCHEME_FLOWS                                      (1305|ARAD_PROC_BITS)
#define ARAD_AQFM_SYSTEM_INFO_SAVE                                                 (1306|ARAD_PROC_BITS)
#define ARAD_AQFM_SYSTEM_INFO_DEFAULTS_GET                                         (1307|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_PORTS_OPEN                                                   (1309|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_AGGREGATES_OPEN                                              (1310|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_QUEUES_OPEN                                                  (1311|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_FLOWS_OPEN                                                   (1312|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_OPEN                                                        (1313|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_UPDATE                                                      (1314|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_CLOSE                                                       (1315|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_PORT_AGGS_OPEN                                               (1316|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_PORT_AGGS_CLOSE                                              (1317|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_PORT_AGGS_UPDATE                                             (1318|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_PORT_SINGLE_AGGS_UPDATE                                      (1319|ARAD_PROC_BITS)
#define ARAD_AQFM_QUEUES_OPEN                                                      (1320|ARAD_PROC_BITS)
#define ARAD_AQFM_QUEUES_UPDATE                                                    (1321|ARAD_PROC_BITS)
#define ARAD_AQFM_QUEUES_CLOSE                                                     (1322|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOW_OPEN                                                        (1323|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOW_UPDATE                                                      (1324|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOW_CLOSE                                                       (1325|ARAD_PROC_BITS)
#define ARAD_AQFM_SYSTEM_INFO_SAVE_UNSAFE                                          (1326|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_FIRST_RELATIVE_ID_GET                                       (1327|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_ACTUAL2RELATIVE_GET                                         (1328|ARAD_PROC_BITS)
#define ARAD_AQFM_QUEUE_INFO_DEFAULT_GET                                           (1329|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_RELATIVE2ACTUAL_GET                                         (1330|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_RELATIVE_ID_GET                                              (1331|ARAD_PROC_BITS)
#define ARAD_AQFM_QUEUE_ID_GET                                                     (1332|ARAD_PROC_BITS)
#define ARAD_AQFM_FLOW_ID_GET                                                      (1333|ARAD_PROC_BITS)
#define ARAD_AQFM_SYSTEM_PHYSICAL_PORT_ID_GET                                      (1334|ARAD_PROC_BITS)
#define ARAD_AQFM_DESTINATION_ID_GET                                               (1335|ARAD_PROC_BITS)
#define ARAD_AQFM_AGG_BASE_IDS_GET                                                 (1336|ARAD_PROC_BITS)
#define ARAD_AQFM_QUEUE_PORT_SCHEME_QUEUES_PRINT                                   (1337|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_QUEUES_OPEN_UNSAFE                                           (1338|ARAD_PROC_BITS)
#define ARAD_AQFM_OPEN_PORT_AGGS                                                   (1339|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_DEFAULT_GET                                                 (1340|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_SCHEDULER_PORT_DEFAULT_GET                                  (1341|ARAD_PROC_BITS)
#define ARAD_AQFM_ALL_PORTS_OPEN_UNSAFE                                            (1342|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_UPDATE_UNSAFE                                               (1343|ARAD_PROC_BITS)
#define ARAD_AQFM_PORT_PORT_SCHEME_PORT_PRINT                                      (1344|ARAD_PROC_BITS)
#define ARAD_AQFM_GET_CREDIT_SOURCES                                               (1345|ARAD_PROC_BITS)
#define ARAD_AQFM_NOF_RELATIVE_PORT_GET                                            (1346|ARAD_PROC_BITS)

#define ARAD_SW_DB_AUTO_SCHEME_INITIALIZE                                          (1350|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_SEND_UNSAFE                                                (1351|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECV_UNSAFE                                                (1352|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_SEND                                                       (1353|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECV                                                       (1354|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_TX_LOAD_MEM                                                (1355|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_TX_LOAD_MEM_UNSAFE                                         (1356|ARAD_PROC_BITS)
#define ARAD_STAT_PKT_COUNTERS_COLLECT                                             (1357|ARAD_PROC_BITS)
#define ARAD_STAT_COUNTER_GET                                                      (1358|ARAD_PROC_BITS)
#define ARAD_STAT_ALL_COUNTERS_GET                                                 (1359|ARAD_PROC_BITS)
#define ARAD_STAT_PKT_COUNTERS_COLLECT_UNSAFE                                      (1362|ARAD_PROC_BITS)
#define ARAD_STAT_COUNTER_GET_UNSAFE                                               (1363|ARAD_PROC_BITS)
#define ARAD_STAT_ALL_COUNTERS_GET_UNSAFE                                          (1364|ARAD_PROC_BITS)
#define ARAD_COUNTER_TO_DEVICE_COUNTERS_ADD                                        (1367|ARAD_PROC_BITS)
#define ARAD_STAT_STATISTICS_MODULE_INITIALIZE                                     (1368|ARAD_PROC_BITS)
#define ARAD_STAT_STATISTICS_DEVICE_INITIALIZE                                     (1369|ARAD_PROC_BITS)
#define ARAD_STAT_STATISTICS_DEFERRED_COUNTER_CLEAR                                (1370|ARAD_PROC_BITS)
#define ARAD_STAT_COUNTER_INFO_GET                                                 (1371|ARAD_PROC_BITS)
#define ARAD_STAT_PKT_CNT_CALLBACK_GET                                             (1372|ARAD_PROC_BITS)
#define ARAD_TODO_ERR_DEF                                                          (1399|ARAD_PROC_BITS)

#define ARAD_READ_FLD                                                              (1400|ARAD_PROC_BITS)
#define ARAD_WRITE_FLD                                                             (1401|ARAD_PROC_BITS)
#define ARAD_READ_REG                                                              (1402|ARAD_PROC_BITS)
#define ARAD_WRITE_REG                                                             (1403|ARAD_PROC_BITS)
#define ARAD_READ_FLD_UNSAFE                                                       (1404|ARAD_PROC_BITS)
#define ARAD_WRITE_FLD_UNSAFE                                                      (1405|ARAD_PROC_BITS)
#define ARAD_READ_REG_UNSAFE                                                       (1406|ARAD_PROC_BITS)
#define ARAD_WRITE_REG_UNSAFE                                                      (1407|ARAD_PROC_BITS)
#define ARAD_GET_BLKN_TBLN_ENTRY                                                   (1408|ARAD_PROC_BITS)
#define ARAD_GET_BLKN_TBLN_ENTRY_UNSAFE                                            (1409|ARAD_PROC_BITS)
#define ARAD_WRITE_REG_BUFFER_UNSAFE                                               (1410|ARAD_PROC_BITS)


#define ARAD_REGS_GET                                                              (1420|ARAD_PROC_BITS)
#define ARAD_REGS_INIT                                                             (1421|ARAD_PROC_BITS)
#define ARAD_TBLS_GET                                                              (1422|ARAD_PROC_BITS)
#define ARAD_TBLS_INIT                                                             (1423|ARAD_PROC_BITS)

#define ARAD_WRITE_ARRAY_OF_FLDS                                                   (1424|ARAD_PROC_BITS)
#define ARAD_READ_ARRAY_OF_FLDS                                                    (1425|ARAD_PROC_BITS)


#define ARAD_FIELD_IN_PLACE_SET                                                    (1489|ARAD_PROC_BITS)
#define ARAD_FIELD_IN_PLACE_GET                                                    (1490|ARAD_PROC_BITS)

#define ARAD_MGMT_INDIRECT_MEMORY_MAP_GET                                          (1491|ARAD_PROC_BITS)
#define ARAD_MGMT_INDIRECT_MEMORY_MAP_INIT                                         (1492|ARAD_PROC_BITS)
#define ARAD_MGMT_INDIRECT_TABLE_MAP_GET                                           (1493|ARAD_PROC_BITS)
#define ARAD_MGMT_INDIRECT_TABLE_MAP_INIT                                          (1494|ARAD_PROC_BITS)
#define ARAD_MGMT_INDIRECT_MODULE_INFO_GET                                         (1495|ARAD_PROC_BITS)
#define ARAD_MGMT_INDIRECT_MODULE_INFO_INIT                                        (1496|ARAD_PROC_BITS)
#define ARAD_MGMT_INDIRECT_MODULE_INIT                                             (1497|ARAD_PROC_BITS)
#define ARAD_ACCESS_DB_INIT                                                        (1498|ARAD_PROC_BITS)

#define ARAD_OLP_PGE_MEM_TBL_GET_UNSAFE                                            (1501|ARAD_PROC_BITS)
#define ARAD_OLP_PGE_MEM_TBL_SET_UNSAFE                                            (1503|ARAD_PROC_BITS)
#define ARAD_IRE_NIF_CTXT_MAP_TBL_GET_UNSAFE                                       (1505|ARAD_PROC_BITS)
#define ARAD_IRE_NIF_CTXT_MAP_TBL_SET_UNSAFE                                       (1507|ARAD_PROC_BITS)
#define ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_GET_UNSAFE                           (1509|ARAD_PROC_BITS)
#define ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_SET_UNSAFE                           (1511|ARAD_PROC_BITS)
#define ARAD_IRE_RCY_CTXT_MAP_TBL_GET_UNSAFE                                       (1513|ARAD_PROC_BITS)
#define ARAD_IRE_RCY_CTXT_MAP_TBL_SET_UNSAFE                                       (1515|ARAD_PROC_BITS)
#define ARAD_IDR_COMPLETE_PC_TBL_GET_UNSAFE                                        (1517|ARAD_PROC_BITS)
#define ARAD_IDR_COMPLETE_PC_TBL_SET_UNSAFE                                        (1519|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_TBL_GET_UNSAFE                                            (1520|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_TBL_SET_UNSAFE                                            (1521|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_TBL_GET_RAW_UNSAFE                                        (1522|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_TBL_SET_RAW_UNSAFE                                        (1523|ARAD_PROC_BITS)
#define ARAD_IRR_MIRROR_TABLE_TBL_GET_UNSAFE                                       (1525|ARAD_PROC_BITS)
#define ARAD_IRR_MIRROR_TABLE_TBL_SET_UNSAFE                                       (1527|ARAD_PROC_BITS)
#define ARAD_IRR_SNOOP_TABLE_TBL_GET_UNSAFE                                        (1529|ARAD_PROC_BITS)
#define ARAD_IRR_SNOOP_TABLE_TBL_SET_UNSAFE                                        (1531|ARAD_PROC_BITS)
#define ARAD_IRR_GLAG_TO_LAG_RANGE_TBL_GET_UNSAFE                                  (1533|ARAD_PROC_BITS)
#define ARAD_IRR_GLAG_TO_LAG_RANGE_TBL_SET_UNSAFE                                  (1535|ARAD_PROC_BITS)
#define ARAD_IRR_SMOOTH_DIVISION_TBL_GET_UNSAFE                                    (1537|ARAD_PROC_BITS)
#define ARAD_IRR_SMOOTH_DIVISION_TBL_SET_UNSAFE                                    (1539|ARAD_PROC_BITS)
#define ARAD_IRR_GLAG_MAPPING_TBL_GET_UNSAFE                                       (1541|ARAD_PROC_BITS)
#define ARAD_IRR_GLAG_MAPPING_TBL_SET_UNSAFE                                       (1543|ARAD_PROC_BITS)
#define ARAD_IRR_DESTINATION_TABLE_TBL_GET_UNSAFE                                  (1545|ARAD_PROC_BITS)
#define ARAD_IRR_DESTINATION_TABLE_TBL_SET_UNSAFE                                  (1547|ARAD_PROC_BITS)
#define ARAD_IRR_GLAG_NEXT_MEMBER_TBL_GET_UNSAFE                                   (1549|ARAD_PROC_BITS)
#define ARAD_IRR_GLAG_NEXT_MEMBER_TBL_SET_UNSAFE                                   (1551|ARAD_PROC_BITS)
#define ARAD_IRR_RLAG_NEXT_MEMBER_TBL_GET_UNSAFE                                   (1553|ARAD_PROC_BITS)
#define ARAD_IRR_RLAG_NEXT_MEMBER_TBL_SET_UNSAFE                                   (1555|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_INFO_TBL_GET_UNSAFE                                          (1557|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_INFO_TBL_SET_UNSAFE                                          (1559|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_GET_UNSAFE                             (1561|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_SET_UNSAFE                             (1563|ARAD_PROC_BITS)
#define ARAD_IHP_STATIC_HEADER_TBL_GET_UNSAFE                                      (1565|ARAD_PROC_BITS)
#define ARAD_IHP_STATIC_HEADER_TBL_SET_UNSAFE                                      (1567|ARAD_PROC_BITS)
#define ARAD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_GET_UNSAFE                          (1569|ARAD_PROC_BITS)
#define ARAD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_SET_UNSAFE                          (1571|ARAD_PROC_BITS)
#define ARAD_IHP_PTC_COMMANDS1_TBL_GET_UNSAFE                                      (1573|ARAD_PROC_BITS)
#define ARAD_IHP_PTC_COMMANDS1_TBL_SET_UNSAFE                                      (1575|ARAD_PROC_BITS)
#define ARAD_IHP_PTC_COMMANDS2_TBL_GET_UNSAFE                                      (1577|ARAD_PROC_BITS)
#define ARAD_IHP_PTC_COMMANDS2_TBL_SET_UNSAFE                                      (1579|ARAD_PROC_BITS)
#define ARAD_IHP_PTC_KEY_PROGRAM_LUT_TBL_GET_UNSAFE                                (1581|ARAD_PROC_BITS)
#define ARAD_IHP_PTC_KEY_PROGRAM_LUT_TBL_SET_UNSAFE                                (1583|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM0_TBL_GET_UNSAFE                                       (1585|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM0_TBL_SET_UNSAFE                                       (1587|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM1_TBL_GET_UNSAFE                                       (1589|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM1_TBL_SET_UNSAFE                                       (1591|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM2_TBL_GET_UNSAFE                                       (1593|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM2_TBL_SET_UNSAFE                                       (1595|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM3_TBL_GET_UNSAFE                                       (1597|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM3_TBL_SET_UNSAFE                                       (1599|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM4_TBL_GET_UNSAFE                                       (1601|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM4_TBL_SET_UNSAFE                                       (1603|ARAD_PROC_BITS)
#define ARAD_IHP_PROGRAMMABLE_COS_TBL_GET_UNSAFE                                   (1605|ARAD_PROC_BITS)
#define ARAD_IHP_PROGRAMMABLE_COS_TBL_SET_UNSAFE                                   (1607|ARAD_PROC_BITS)
#define ARAD_IHP_PROGRAMMABLE_COS1_TBL_GET_UNSAFE                                  (1609|ARAD_PROC_BITS)
#define ARAD_IHP_PROGRAMMABLE_COS1_TBL_SET_UNSAFE                                  (1611|ARAD_PROC_BITS)
#define ARAD_SRD_EYE_SCAN_RUN                                                      (1612|ARAD_PROC_BITS)
#define ARAD_SRD_EYE_SCAN_RUN_MULTIPLE_ALLOC                                       (1613|ARAD_PROC_BITS)
#define ARAD_SRD_EYE_SCAN_RUN_MULTIPLE_ALLOC_UNSAFE                                (1614|ARAD_PROC_BITS)

#define ARAD_IQM_BDB_LINK_LIST_TBL_GET_UNSAFE                                      (1621|ARAD_PROC_BITS)
#define ARAD_IQM_BDB_LINK_LIST_TBL_SET_UNSAFE                                      (1623|ARAD_PROC_BITS)
#define ARAD_IQM_DYNAMIC_TBL_GET_UNSAFE                                            (1625|ARAD_PROC_BITS)
#define ARAD_IQM_DYNAMIC_TBL_SET_UNSAFE                                            (1627|ARAD_PROC_BITS)
#define ARAD_IQM_STATIC_TBL_GET_UNSAFE                                             (1629|ARAD_PROC_BITS)
#define ARAD_IQM_STATIC_TBL_SET_UNSAFE                                             (1631|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_GET_UNSAFE                          (1633|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_SET_UNSAFE                          (1635|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_GET_UNSAFE                      (1637|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_SET_UNSAFE                      (1639|ARAD_PROC_BITS)
#define ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_GET_UNSAFE                              (1641|ARAD_PROC_BITS)
#define ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_SET_UNSAFE                              (1643|ARAD_PROC_BITS)
#define ARAD_IQM_FULL_USER_COUNT_MEMORY_TBL_GET_UNSAFE                             (1645|ARAD_PROC_BITS)
#define ARAD_IQM_FULL_USER_COUNT_MEMORY_TBL_SET_UNSAFE                             (1647|ARAD_PROC_BITS)
#define ARAD_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_GET_UNSAFE                   (1649|ARAD_PROC_BITS)
#define ARAD_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_SET_UNSAFE                   (1651|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TAIL_DROP_MANTISSA_NOF_BITS           (1652|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE                  (1653|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE                  (1655|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_GET_UNSAFE                  (1657|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_SET_UNSAFE                  (1659|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_GET_UNSAFE                  (1661|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_SET_UNSAFE                  (1663|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_GET_UNSAFE                  (1665|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_SET_UNSAFE                  (1667|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_GET_UNSAFE                  (1669|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_SET_UNSAFE                  (1671|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_GET_UNSAFE                           (1673|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_SET_UNSAFE                           (1675|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_GET_UNSAFE                           (1677|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_SET_UNSAFE                           (1679|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_GET_UNSAFE                           (1681|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_SET_UNSAFE                           (1683|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_GET_UNSAFE                           (1685|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_SET_UNSAFE                           (1687|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_GET_UNSAFE                   (1689|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_SET_UNSAFE                   (1691|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_GET_UNSAFE                   (1693|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_SET_UNSAFE                   (1695|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_GET_UNSAFE                   (1697|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_SET_UNSAFE                   (1699|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_GET_UNSAFE                   (1701|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_SET_UNSAFE                   (1703|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE            (1705|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE            (1707|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE                   (1721|ARAD_PROC_BITS)
#define ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE                   (1723|ARAD_PROC_BITS)
#define ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE                        (1737|ARAD_PROC_BITS)
#define ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE                        (1739|ARAD_PROC_BITS)
#define ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_GET_UNSAFE                 (1741|ARAD_PROC_BITS)
#define ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_SET_UNSAFE                 (1743|ARAD_PROC_BITS)
#define ARAD_IQM_SYSTEM_RED_TBL_GET_UNSAFE                                         (1745|ARAD_PROC_BITS)
#define ARAD_IQM_SYSTEM_RED_TBL_SET_UNSAFE                                         (1746|ARAD_PROC_BITS)
#define ARAD_QDR_MEMORY_TBL_GET_UNSAFE                                             (1747|ARAD_PROC_BITS)
#define ARAD_QDR_MEMORY_TBL_SET_UNSAFE                                             (1748|ARAD_PROC_BITS)
#define ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE                  (1749|ARAD_PROC_BITS)
#define ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE                  (1751|ARAD_PROC_BITS)
#define ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE           (1753|ARAD_PROC_BITS)
#define ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE           (1755|ARAD_PROC_BITS)
#define ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_UNSAFE                               (1757|ARAD_PROC_BITS)
#define ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_UNSAFE                               (1759|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_UNSAFE                            (1761|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_UNSAFE                            (1763|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_UNSAFE                          (1765|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_UNSAFE                          (1767|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_UNSAFE                          (1769|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_UNSAFE                          (1771|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE                  (1773|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE                  (1775|ARAD_PROC_BITS)
#define ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE              (1777|ARAD_PROC_BITS)
#define ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE              (1779|ARAD_PROC_BITS)
#define ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_UNSAFE                   (1781|ARAD_PROC_BITS)
#define ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_UNSAFE                   (1783|ARAD_PROC_BITS)
#define ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_UNSAFE                   (1785|ARAD_PROC_BITS)
#define ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_UNSAFE                   (1787|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_GET_UNSAFE                             (1789|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_SET_UNSAFE                             (1791|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_SIZE_TABLE_TBL_GET_UNSAFE                                   (1793|ARAD_PROC_BITS)
#define ARAD_IPS_QUEUE_SIZE_TABLE_TBL_SET_UNSAFE                                   (1795|ARAD_PROC_BITS)
#define ARAD_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_GET_UNSAFE                    (1797|ARAD_PROC_BITS)
#define ARAD_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_SET_UNSAFE                    (1799|ARAD_PROC_BITS)
#define ARAD_DPI_DLL_RAM_TBL_GET_UNSAFE                                            (1805|ARAD_PROC_BITS)
#define ARAD_DPI_DLL_RAM_TBL_SET_UNSAFE                                            (1807|ARAD_PROC_BITS)
#define ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_GET_UNSAFE         (1809|ARAD_PROC_BITS)
#define ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_SET_UNSAFE         (1811|ARAD_PROC_BITS)
#define ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_GET_UNSAFE      (1813|ARAD_PROC_BITS)
#define ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_SET_UNSAFE      (1815|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH0_SCM_TBL_GET_UNSAFE                                       (1817|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH0_SCM_TBL_SET_UNSAFE                                       (1819|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH1_SCM_TBL_GET_UNSAFE                                       (1821|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH1_SCM_TBL_SET_UNSAFE                                       (1823|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH2_SCM_TBL_GET_UNSAFE                                       (1825|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH2_SCM_TBL_SET_UNSAFE                                       (1827|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH3_SCM_TBL_GET_UNSAFE                                       (1829|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_CH3_SCM_TBL_SET_UNSAFE                                       (1831|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH0_SCM_TBL_GET_UNSAFE                                       (1833|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH0_SCM_TBL_SET_UNSAFE                                       (1835|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH1_SCM_TBL_GET_UNSAFE                                       (1837|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH1_SCM_TBL_SET_UNSAFE                                       (1839|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH2_SCM_TBL_GET_UNSAFE                                       (1841|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH2_SCM_TBL_SET_UNSAFE                                       (1843|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH3_SCM_TBL_GET_UNSAFE                                       (1845|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_CH3_SCM_TBL_SET_UNSAFE                                       (1847|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFAB_NCH_SCM_TBL_GET_UNSAFE                                      (1849|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFAB_NCH_SCM_TBL_SET_UNSAFE                                      (1851|ARAD_PROC_BITS)
#define ARAD_EGQ_RCY_SCM_TBL_GET_UNSAFE                                            (1853|ARAD_PROC_BITS)
#define ARAD_EGQ_RCY_SCM_TBL_SET_UNSAFE                                            (1855|ARAD_PROC_BITS)
#define ARAD_EGQ_CPU_SCM_TBL_GET_UNSAFE                                            (1857|ARAD_PROC_BITS)
#define ARAD_EGQ_CPU_SCM_TBL_SET_UNSAFE                                            (1859|ARAD_PROC_BITS)
#define ARAD_EGQ_CCM_TBL_GET_UNSAFE                                                (1861|ARAD_PROC_BITS)
#define ARAD_EGQ_CCM_TBL_SET_UNSAFE                                                (1863|ARAD_PROC_BITS)
#define ARAD_EGQ_PMC_TBL_GET_UNSAFE                                                (1865|ARAD_PROC_BITS)
#define ARAD_EGQ_PMC_TBL_SET_UNSAFE                                                (1867|ARAD_PROC_BITS)
#define ARAD_EGQ_FBM_TBL_GET_UNSAFE                                                (1869|ARAD_PROC_BITS)
#define ARAD_EGQ_FBM_TBL_SET_UNSAFE                                                (1871|ARAD_PROC_BITS)
#define ARAD_EGQ_FDM_TBL_GET_UNSAFE                                                (1873|ARAD_PROC_BITS)
#define ARAD_EGQ_FDM_TBL_SET_UNSAFE                                                (1875|ARAD_PROC_BITS)
#define ARAD_EGQ_DWM_TBL_GET_UNSAFE                                                (1877|ARAD_PROC_BITS)
#define ARAD_EGQ_DWM_TBL_SET_UNSAFE                                                (1879|ARAD_PROC_BITS)
#define ARAD_EGQ_RRDM_TBL_GET_UNSAFE                                               (1881|ARAD_PROC_BITS)
#define ARAD_EGQ_RRDM_TBL_SET_UNSAFE                                               (1883|ARAD_PROC_BITS)
#define ARAD_EGQ_RPDM_TBL_GET_UNSAFE                                               (1885|ARAD_PROC_BITS)
#define ARAD_EGQ_RPDM_TBL_SET_UNSAFE                                               (1887|ARAD_PROC_BITS)
#define ARAD_EGQ_PCT_TBL_GET_UNSAFE                                                (1889|ARAD_PROC_BITS)
#define ARAD_EGQ_PCT_TBL_SET_UNSAFE                                                (1891|ARAD_PROC_BITS)
#define ARAD_EGQ_VLAN_TABLE_TBL_GET_UNSAFE                                         (1893|ARAD_PROC_BITS)
#define ARAD_EGQ_VLAN_TABLE_TBL_SET_UNSAFE                                         (1895|ARAD_PROC_BITS)
#define ARAD_EGQ_TDMMCID_TBL_GET_UNSAFE                                            (1897|ARAD_PROC_BITS)
#define ARAD_EGQ_TDMMCID_TBL_SET_UNSAFE                                            (1899|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH0_SCM_TBL_GET_UNSAFE                                      (1901|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH0_SCM_TBL_SET_UNSAFE                                      (1903|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH1_SCM_TBL_GET_UNSAFE                                      (1905|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH1_SCM_TBL_SET_UNSAFE                                      (1907|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH2_SCM_TBL_GET_UNSAFE                                      (1909|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH2_SCM_TBL_SET_UNSAFE                                      (1911|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH3_SCM_TBL_GET_UNSAFE                                      (1913|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFA_CH3_SCM_TBL_SET_UNSAFE                                      (1915|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH0_SCM_TBL_GET_UNSAFE                                      (1917|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH0_SCM_TBL_SET_UNSAFE                                      (1919|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH1_SCM_TBL_GET_UNSAFE                                      (1921|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH1_SCM_TBL_SET_UNSAFE                                      (1923|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH2_SCM_TBL_GET_UNSAFE                                      (1925|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH2_SCM_TBL_SET_UNSAFE                                      (1927|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH3_SCM_TBL_GET_UNSAFE                                      (1929|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFB_CH3_SCM_TBL_SET_UNSAFE                                      (1931|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFAB_NCH_SCM_TBL_GET_UNSAFE                                     (1933|ARAD_PROC_BITS)
#define ARAD_EPNI_NIFAB_NCH_SCM_TBL_SET_UNSAFE                                     (1935|ARAD_PROC_BITS)
#define ARAD_EPNI_RCY_SCM_TBL_GET_UNSAFE                                           (1937|ARAD_PROC_BITS)
#define ARAD_EPNI_RCY_SCM_TBL_SET_UNSAFE                                           (1939|ARAD_PROC_BITS)
#define ARAD_EPNI_CPU_SCM_TBL_GET_UNSAFE                                           (1941|ARAD_PROC_BITS)
#define ARAD_EPNI_CPU_SCM_TBL_SET_UNSAFE                                           (1943|ARAD_PROC_BITS)
#define ARAD_EPNI_CCM_TBL_GET_UNSAFE                                               (1945|ARAD_PROC_BITS)
#define ARAD_EPNI_CCM_TBL_SET_UNSAFE                                               (1947|ARAD_PROC_BITS)
#define ARAD_EPNI_PMC_TBL_GET_UNSAFE                                               (1949|ARAD_PROC_BITS)
#define ARAD_EPNI_PMC_TBL_SET_UNSAFE                                               (1951|ARAD_PROC_BITS)
#define ARAD_EPNI_CBM_TBL_GET_UNSAFE                                               (1953|ARAD_PROC_BITS)
#define ARAD_EPNI_CBM_TBL_SET_UNSAFE                                               (1955|ARAD_PROC_BITS)
#define ARAD_EPNI_FBM_TBL_GET_UNSAFE                                               (1957|ARAD_PROC_BITS)
#define ARAD_EPNI_FBM_TBL_SET_UNSAFE                                               (1959|ARAD_PROC_BITS)
#define ARAD_EPNI_FDM_TBL_GET_UNSAFE                                               (1961|ARAD_PROC_BITS)
#define ARAD_EPNI_FDM_TBL_SET_UNSAFE                                               (1963|ARAD_PROC_BITS)
#define ARAD_EPNI_DWM_TBL_GET_UNSAFE                                               (1965|ARAD_PROC_BITS)
#define ARAD_EPNI_DWM_TBL_SET_UNSAFE                                               (1967|ARAD_PROC_BITS)
#define ARAD_EPNI_RRDM_TBL_GET_UNSAFE                                              (1969|ARAD_PROC_BITS)
#define ARAD_EPNI_RRDM_TBL_SET_UNSAFE                                              (1971|ARAD_PROC_BITS)
#define ARAD_EPNI_RPDM_TBL_GET_UNSAFE                                              (1973|ARAD_PROC_BITS)
#define ARAD_EPNI_RPDM_TBL_SET_UNSAFE                                              (1975|ARAD_PROC_BITS)
#define ARAD_EPNI_PCT_TBL_GET_UNSAFE                                               (1977|ARAD_PROC_BITS)
#define ARAD_EPNI_PCT_TBL_SET_UNSAFE                                               (1979|ARAD_PROC_BITS)
#define ARAD_EPNI_VLAN_TABLE_TBL_GET_UNSAFE                                        (1981|ARAD_PROC_BITS)
#define ARAD_EPNI_VLAN_TABLE_TBL_SET_UNSAFE                                        (1983|ARAD_PROC_BITS)
#define ARAD_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_GET_UNSAFE              (1985|ARAD_PROC_BITS)
#define ARAD_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_SET_UNSAFE              (1987|ARAD_PROC_BITS)
#define ARAD_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_GET_UNSAFE                   (1989|ARAD_PROC_BITS)
#define ARAD_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_SET_UNSAFE                   (1991|ARAD_PROC_BITS)
#define ARAD_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_GET_UNSAFE                   (1993|ARAD_PROC_BITS)
#define ARAD_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_SET_UNSAFE                   (1995|ARAD_PROC_BITS)
#define ARAD_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_GET_UNSAFE     (1997|ARAD_PROC_BITS)
#define ARAD_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_SET_UNSAFE     (1999|ARAD_PROC_BITS)
#define ARAD_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_GET_UNSAFE     (2001|ARAD_PROC_BITS)
#define ARAD_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_SET_UNSAFE     (2003|ARAD_PROC_BITS)
#define ARAD_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_GET_UNSAFE                  (2005|ARAD_PROC_BITS)
#define ARAD_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_SET_UNSAFE                  (2007|ARAD_PROC_BITS)
#define ARAD_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_GET_UNSAFE                  (2009|ARAD_PROC_BITS)
#define ARAD_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_SET_UNSAFE                  (2011|ARAD_PROC_BITS)
#define ARAD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_GET_UNSAFE                    (2013|ARAD_PROC_BITS)
#define ARAD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_SET_UNSAFE                    (2015|ARAD_PROC_BITS)
#define ARAD_SCH_CAL_TBL_GET_UNSAFE                                                (2017|ARAD_PROC_BITS)
#define ARAD_SCH_CAL_TBL_SET_UNSAFE                                                (2019|ARAD_PROC_BITS)
#define ARAD_SCH_DRM_TBL_GET_UNSAFE                                                (2021|ARAD_PROC_BITS)
#define ARAD_SCH_DRM_TBL_SET_UNSAFE                                                (2023|ARAD_PROC_BITS)
#define ARAD_SCH_DSM_TBL_GET_UNSAFE                                                (2025|ARAD_PROC_BITS)
#define ARAD_SCH_DSM_TBL_SET_UNSAFE                                                (2027|ARAD_PROC_BITS)
#define ARAD_SCH_FDMS_TBL_GET_UNSAFE                                               (2029|ARAD_PROC_BITS)
#define ARAD_SCH_FDMS_TBL_SET_UNSAFE                                               (2031|ARAD_PROC_BITS)
#define ARAD_SCH_SHDS_TBL_GET_UNSAFE                                               (2033|ARAD_PROC_BITS)
#define ARAD_SCH_SHDS_TBL_SET_UNSAFE                                               (2035|ARAD_PROC_BITS)
#define ARAD_SCH_SEM_TBL_GET_UNSAFE                                                (2037|ARAD_PROC_BITS)
#define ARAD_SCH_SEM_TBL_SET_UNSAFE                                                (2039|ARAD_PROC_BITS)
#define ARAD_SCH_FSF_TBL_GET_UNSAFE                                                (2041|ARAD_PROC_BITS)
#define ARAD_SCH_FSF_TBL_SET_UNSAFE                                                (2043|ARAD_PROC_BITS)
#define ARAD_SCH_FGM_TBL_GET_UNSAFE                                                (2045|ARAD_PROC_BITS)
#define ARAD_SCH_FGM_TBL_SET_UNSAFE                                                (2047|ARAD_PROC_BITS)
#define ARAD_SCH_SHC_TBL_GET_UNSAFE                                                (2049|ARAD_PROC_BITS)
#define ARAD_SCH_SHC_TBL_SET_UNSAFE                                                (2051|ARAD_PROC_BITS)
#define ARAD_SCH_SCC_TBL_GET_UNSAFE                                                (2053|ARAD_PROC_BITS)
#define ARAD_SCH_SCC_TBL_SET_UNSAFE                                                (2055|ARAD_PROC_BITS)
#define ARAD_SCH_SCT_TBL_GET_UNSAFE                                                (2057|ARAD_PROC_BITS)
#define ARAD_SCH_SCT_TBL_SET_UNSAFE                                                (2059|ARAD_PROC_BITS)
#define ARAD_SCH_FQM_TBL_GET_UNSAFE                                                (2061|ARAD_PROC_BITS)
#define ARAD_SCH_FQM_TBL_SET_UNSAFE                                                (2063|ARAD_PROC_BITS)
#define ARAD_SCH_FFM_TBL_GET_UNSAFE                                                (2065|ARAD_PROC_BITS)
#define ARAD_SCH_FFM_TBL_SET_UNSAFE                                                (2067|ARAD_PROC_BITS)
#define ARAD_SCH_TMC_TBL_GET_UNSAFE                                                (2069|ARAD_PROC_BITS)
#define ARAD_SCH_TMC_TBL_SET_UNSAFE                                                (2071|ARAD_PROC_BITS)
#define ARAD_SCH_PQS_TBL_GET_UNSAFE                                                (2073|ARAD_PROC_BITS)
#define ARAD_SCH_PQS_TBL_SET_UNSAFE                                                (2075|ARAD_PROC_BITS)
#define ARAD_SCH_SCHEDULER_INIT_TBL_GET_UNSAFE                                     (2077|ARAD_PROC_BITS)
#define ARAD_SCH_SCHEDULER_INIT_TBL_SET_UNSAFE                                     (2079|ARAD_PROC_BITS)
#define ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_GET_UNSAFE                               (2081|ARAD_PROC_BITS)
#define ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_SET_UNSAFE                               (2083|ARAD_PROC_BITS)
#define ARAD_EGQ_CBM_TBL_GET_UNSAFE                                                (2085|ARAD_PROC_BITS)
#define ARAD_EGQ_CBM_TBL_SET_UNSAFE                                                (2087|ARAD_PROC_BITS)
#define ARAD_IPT_BDQ_TBL_GET_UNSAFE                                                (2091|ARAD_PROC_BITS)
#define ARAD_IPT_BDQ_TBL_SET_UNSAFE                                                (2093|ARAD_PROC_BITS)
#define ARAD_IPT_PCQ_TBL_GET_UNSAFE                                                (2095|ARAD_PROC_BITS)
#define ARAD_IPT_PCQ_TBL_SET_UNSAFE                                                (2097|ARAD_PROC_BITS)
#define ARAD_IPT_SOP_MMU_TBL_GET_UNSAFE                                            (2099|ARAD_PROC_BITS)
#define ARAD_IPT_SOP_MMU_TBL_SET_UNSAFE                                            (2101|ARAD_PROC_BITS)
#define ARAD_IPT_MOP_MMU_TBL_GET_UNSAFE                                            (2103|ARAD_PROC_BITS)
#define ARAD_IPT_MOP_MMU_TBL_SET_UNSAFE                                            (2105|ARAD_PROC_BITS)
#define ARAD_IPT_FDTCTL_TBL_GET_UNSAFE                                             (2107|ARAD_PROC_BITS)
#define ARAD_IPT_FDTCTL_TBL_SET_UNSAFE                                             (2109|ARAD_PROC_BITS)
#define ARAD_IPT_FDTDATA_TBL_GET_UNSAFE                                            (2111|ARAD_PROC_BITS)
#define ARAD_IPT_FDTDATA_TBL_SET_UNSAFE                                            (2113|ARAD_PROC_BITS)
#define ARAD_IPT_EGQCTL_TBL_GET_UNSAFE                                             (2115|ARAD_PROC_BITS)
#define ARAD_IPT_EGQCTL_TBL_SET_UNSAFE                                             (2117|ARAD_PROC_BITS)
#define ARAD_IPT_EGQDATA_TBL_GET_UNSAFE                                            (2119|ARAD_PROC_BITS)
#define ARAD_IPT_EGQDATA_TBL_SET_UNSAFE                                            (2121|ARAD_PROC_BITS)
#define ARAD_EGQ_CFC_FLOW_CONTROL_TBL_GET_UNSAFE                                   (2123|ARAD_PROC_BITS)
#define ARAD_EGQ_CFC_FLOW_CONTROL_TBL_SET_UNSAFE                                   (2125|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_FLOW_CONTROL_TBL_GET_UNSAFE                                  (2127|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFA_FLOW_CONTROL_TBL_SET_UNSAFE                                  (2129|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_FLOW_CONTROL_TBL_GET_UNSAFE                                  (2131|ARAD_PROC_BITS)
#define ARAD_EGQ_NIFB_FLOW_CONTROL_TBL_SET_UNSAFE                                  (2133|ARAD_PROC_BITS)
#define ARAD_EGQ_CPU_LAST_HEADER_TBL_GET_UNSAFE                                    (2135|ARAD_PROC_BITS)
#define ARAD_EGQ_CPU_LAST_HEADER_TBL_SET_UNSAFE                                    (2137|ARAD_PROC_BITS)
#define ARAD_EGQ_IPT_LAST_HEADER_TBL_GET_UNSAFE                                    (2139|ARAD_PROC_BITS)
#define ARAD_EGQ_IPT_LAST_HEADER_TBL_SET_UNSAFE                                    (2141|ARAD_PROC_BITS)
#define ARAD_EGQ_FDR_LAST_HEADER_TBL_GET_UNSAFE                                    (2143|ARAD_PROC_BITS)
#define ARAD_EGQ_FDR_LAST_HEADER_TBL_SET_UNSAFE                                    (2145|ARAD_PROC_BITS)
#define ARAD_EGQ_CPU_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2147|ARAD_PROC_BITS)
#define ARAD_EGQ_CPU_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2149|ARAD_PROC_BITS)
#define ARAD_EGQ_IPT_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2151|ARAD_PROC_BITS)
#define ARAD_EGQ_IPT_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2153|ARAD_PROC_BITS)
#define ARAD_EGQ_FDR_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2155|ARAD_PROC_BITS)
#define ARAD_EGQ_FDR_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2157|ARAD_PROC_BITS)
#define ARAD_EGQ_RQP_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2159|ARAD_PROC_BITS)
#define ARAD_EGQ_RQP_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2161|ARAD_PROC_BITS)
#define ARAD_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_GET_UNSAFE                         (2163|ARAD_PROC_BITS)
#define ARAD_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_SET_UNSAFE                         (2165|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_GET_UNSAFE                         (2167|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_SET_UNSAFE                         (2169|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE                  (2171|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE                  (2173|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE                   (2175|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE                   (2177|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_GET_UNSAFE                         (2179|ARAD_PROC_BITS)
#define ARAD_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_SET_UNSAFE                         (2181|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE                    (2183|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE                    (2185|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE                     (2187|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE                     (2189|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_GET_UNSAFE                  (2191|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_SET_UNSAFE                  (2193|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_GET_UNSAFE                   (2195|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_SET_UNSAFE                   (2197|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_GET_UNSAFE                     (2199|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_SET_UNSAFE                     (2201|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_GET_UNSAFE                      (2203|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_SET_UNSAFE                      (2205|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_GET_UNSAFE                   (2207|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_SET_UNSAFE                   (2209|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_GET_UNSAFE                    (2211|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_SET_UNSAFE                    (2213|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_GET_UNSAFE                 (2215|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_SET_UNSAFE                 (2217|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_GET_UNSAFE               (2219|ARAD_PROC_BITS)
#define ARAD_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_SET_UNSAFE               (2221|ARAD_PROC_BITS)
#define ARAD_EGQ_FQP_PACKET_COUNTER_TBL_GET_UNSAFE                                 (2223|ARAD_PROC_BITS)
#define ARAD_EGQ_FQP_PACKET_COUNTER_TBL_SET_UNSAFE                                 (2225|ARAD_PROC_BITS)
#define ARAD_MMU_DRAM_ADDRESS_SPACE_TBL_GET_UNSAFE                                 (2231|ARAD_PROC_BITS)
#define ARAD_MMU_DRAM_ADDRESS_SPACE_TBL_SET_UNSAFE                                 (2233|ARAD_PROC_BITS)
#define ARAD_MMU_DRAM_ADDRESS_SPACE_INFO_GET_UNSAFE                                (2234|ARAD_PROC_BITS)
#define ARAD_MMU_IDF_TBL_GET_UNSAFE                                                (2235|ARAD_PROC_BITS)
#define ARAD_MMU_IDF_TBL_SET_UNSAFE                                                (2237|ARAD_PROC_BITS)
#define ARAD_MMU_FDF_TBL_GET_UNSAFE                                                (2239|ARAD_PROC_BITS)
#define ARAD_MMU_FDF_TBL_SET_UNSAFE                                                (2241|ARAD_PROC_BITS)
#define ARAD_MMU_RDFA_WADDR_STATUS_TBL_GET_UNSAFE                                  (2243|ARAD_PROC_BITS)
#define ARAD_MMU_RDFA_WADDR_STATUS_TBL_SET_UNSAFE                                  (2245|ARAD_PROC_BITS)
#define ARAD_MMU_RDFB_WADDR_STATUS_TBL_GET_UNSAFE                                  (2247|ARAD_PROC_BITS)
#define ARAD_MMU_RDFB_WADDR_STATUS_TBL_SET_UNSAFE                                  (2249|ARAD_PROC_BITS)
#define ARAD_MMU_RDFC_WADDR_STATUS_TBL_GET_UNSAFE                                  (2251|ARAD_PROC_BITS)
#define ARAD_MMU_RDFC_WADDR_STATUS_TBL_SET_UNSAFE                                  (2253|ARAD_PROC_BITS)
#define ARAD_MMU_RDFD_WADDR_STATUS_TBL_GET_UNSAFE                                  (2255|ARAD_PROC_BITS)
#define ARAD_MMU_RDFD_WADDR_STATUS_TBL_SET_UNSAFE                                  (2257|ARAD_PROC_BITS)
#define ARAD_MMU_RDFE_WADDR_STATUS_TBL_GET_UNSAFE                                  (2259|ARAD_PROC_BITS)
#define ARAD_MMU_RDFE_WADDR_STATUS_TBL_SET_UNSAFE                                  (2261|ARAD_PROC_BITS)
#define ARAD_MMU_RDFF_WADDR_STATUS_TBL_GET_UNSAFE                                  (2263|ARAD_PROC_BITS)
#define ARAD_MMU_RDFF_WADDR_STATUS_TBL_SET_UNSAFE                                  (2265|ARAD_PROC_BITS)
#define ARAD_MMU_RDF_RADDR_TBL_GET_UNSAFE                                          (2267|ARAD_PROC_BITS)
#define ARAD_MMU_RDF_RADDR_TBL_SET_UNSAFE                                          (2269|ARAD_PROC_BITS)
#define ARAD_MMU_WAF_HALFA_WADDR_TBL_GET_UNSAFE                                    (2271|ARAD_PROC_BITS)
#define ARAD_MMU_WAF_HALFA_WADDR_TBL_SET_UNSAFE                                    (2273|ARAD_PROC_BITS)
#define ARAD_MMU_WAF_HALFB_WADDR_TBL_GET_UNSAFE                                    (2275|ARAD_PROC_BITS)
#define ARAD_MMU_WAF_HALFB_WADDR_TBL_SET_UNSAFE                                    (2277|ARAD_PROC_BITS)
#define ARAD_MMU_WAFA_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2279|ARAD_PROC_BITS)
#define ARAD_MMU_WAFA_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2281|ARAD_PROC_BITS)
#define ARAD_MMU_WAFB_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2283|ARAD_PROC_BITS)
#define ARAD_MMU_WAFB_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2285|ARAD_PROC_BITS)
#define ARAD_MMU_WAFC_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2287|ARAD_PROC_BITS)
#define ARAD_MMU_WAFC_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2289|ARAD_PROC_BITS)
#define ARAD_MMU_WAFD_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2291|ARAD_PROC_BITS)
#define ARAD_MMU_WAFD_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2293|ARAD_PROC_BITS)
#define ARAD_MMU_WAFE_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2295|ARAD_PROC_BITS)
#define ARAD_MMU_WAFE_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2297|ARAD_PROC_BITS)
#define ARAD_MMU_WAFF_HALFA_RADDR_STATUS_TBL_GET_UNSAFE                            (2299|ARAD_PROC_BITS)
#define ARAD_MMU_WAFF_HALFA_RADDR_STATUS_TBL_SET_UNSAFE                            (2301|ARAD_PROC_BITS)
#define ARAD_MMU_WAFA_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2303|ARAD_PROC_BITS)
#define ARAD_MMU_WAFA_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2305|ARAD_PROC_BITS)
#define ARAD_MMU_WAFB_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2307|ARAD_PROC_BITS)
#define ARAD_MMU_WAFB_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2309|ARAD_PROC_BITS)
#define ARAD_MMU_WAFC_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2311|ARAD_PROC_BITS)
#define ARAD_MMU_WAFC_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2313|ARAD_PROC_BITS)
#define ARAD_MMU_WAFD_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2315|ARAD_PROC_BITS)
#define ARAD_MMU_WAFD_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2317|ARAD_PROC_BITS)
#define ARAD_MMU_WAFE_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2319|ARAD_PROC_BITS)
#define ARAD_MMU_WAFE_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2321|ARAD_PROC_BITS)
#define ARAD_MMU_WAFF_HALFB_RADDR_STATUS_TBL_GET_UNSAFE                            (2323|ARAD_PROC_BITS)
#define ARAD_MMU_WAFF_HALFB_RADDR_STATUS_TBL_SET_UNSAFE                            (2325|ARAD_PROC_BITS)
#define ARAD_MMU_RAF_WADDR_TBL_GET_UNSAFE                                          (2327|ARAD_PROC_BITS)
#define ARAD_MMU_RAF_WADDR_TBL_SET_UNSAFE                                          (2329|ARAD_PROC_BITS)
#define ARAD_MMU_RAFA_RADDR_STATUS_TBL_GET_UNSAFE                                  (2331|ARAD_PROC_BITS)
#define ARAD_MMU_RAFA_RADDR_STATUS_TBL_SET_UNSAFE                                  (2333|ARAD_PROC_BITS)
#define ARAD_MMU_RAFB_RADDR_STATUS_TBL_GET_UNSAFE                                  (2335|ARAD_PROC_BITS)
#define ARAD_MMU_RAFB_RADDR_STATUS_TBL_SET_UNSAFE                                  (2337|ARAD_PROC_BITS)
#define ARAD_MMU_RAFC_RADDR_STATUS_TBL_GET_UNSAFE                                  (2339|ARAD_PROC_BITS)
#define ARAD_MMU_RAFC_RADDR_STATUS_TBL_SET_UNSAFE                                  (2341|ARAD_PROC_BITS)
#define ARAD_MMU_RAFD_RADDR_STATUS_TBL_GET_UNSAFE                                  (2343|ARAD_PROC_BITS)
#define ARAD_MMU_RAFD_RADDR_STATUS_TBL_SET_UNSAFE                                  (2345|ARAD_PROC_BITS)
#define ARAD_MMU_RAFE_RADDR_STATUS_TBL_GET_UNSAFE                                  (2347|ARAD_PROC_BITS)
#define ARAD_MMU_RAFE_RADDR_STATUS_TBL_SET_UNSAFE                                  (2349|ARAD_PROC_BITS)
#define ARAD_MMU_RAFF_RADDR_STATUS_TBL_GET_UNSAFE                                  (2351|ARAD_PROC_BITS)
#define ARAD_MMU_RAFF_RADDR_STATUS_TBL_SET_UNSAFE                                  (2353|ARAD_PROC_BITS)
#define ARAD_IPT_SELECT_SOURCE_SUM_TBL_SET_UNSAFE                                  (2354|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_GET_UNSAFE                     (2355|ARAD_PROC_BITS)
#define ARAD_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_SET_UNSAFE                     (2356|ARAD_PROC_BITS)
#define ARAD_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_GET_UNSAFE                         (2357|ARAD_PROC_BITS)
#define ARAD_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_SET_UNSAFE                         (2358|ARAD_PROC_BITS)




#define ARAD_TBL_READ_LOW                                                          (2426|ARAD_PROC_BITS)
#define ARAD_TBL_WRITE_LOW                                                         (2427|ARAD_PROC_BITS)
#define ARAD_TBL_READ_LOW_UNSAFE                                                   (2428|ARAD_PROC_BITS)
#define ARAD_TBL_WRITE_LOW_UNSAFE                                                  (2429|ARAD_PROC_BITS)
#define ARAD_TBL_FIELD_MAX_VALUE_GET                                               (2430|ARAD_PROC_BITS)


#define ARAD_MGMT_SYSTEM_FAP_ID_GET                                                (2431|ARAD_PROC_BITS)
#define ARAD_MGMT_SYSTEM_FAP_ID_GET_UNSAFE                                         (2432|ARAD_PROC_BITS)
#define ARAD_MGMT_SYSTEM_FAP_ID_SET                                                (2433|ARAD_PROC_BITS)
#define ARAD_MGMT_SYSTEM_FAP_ID_SET_UNSAFE                                         (2434|ARAD_PROC_BITS)
#define ARAD_MGMT_SYSTEM_FAP_ID_VERIFY                                             (2435|ARAD_PROC_BITS)



#define ARAD_GET_ERR_TEXT                                                          (2445|ARAD_PROC_BITS)
#define ARAD_PROC_ID_TO_STRING                                                     (2446|ARAD_PROC_BITS)

#define ARAD_MULT_FABRIC_CREDIT_SOURCE_VERIFY                                      (2450|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_GET_UNSAFE                            (2451|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_SET_UNSAFE                            (2452|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_GET_UNSAFE                            (2453|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_SET_UNSAFE                            (2454|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_GET_UNSAFE                            (2455|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_SET_UNSAFE                            (2456|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_3_TBL_GET_UNSAFE                            (2457|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_3_TBL_SET_UNSAFE                            (2458|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_GET_UNSAFE                            (2459|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_SET_UNSAFE                            (2460|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_GET_UNSAFE                            (2461|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_SET_UNSAFE                            (2462|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_GET_UNSAFE                            (2463|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_SET_UNSAFE                            (2464|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_GET_UNSAFE                            (2465|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_SET_UNSAFE                            (2466|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_GET_UNSAFE                      (2467|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_SET_UNSAFE                      (2468|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_GET_UNSAFE                          (2469|ARAD_PROC_BITS)
#define ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_SET_UNSAFE                          (2470|ARAD_PROC_BITS)





#define ARAD_INTERNAL_FUNCS_BASE                                                   ((2500))

#define ARAD_INGRESS_SCHEDULER_CLOS_WEIGHTS_SET                                    ((ARAD_INTERNAL_FUNCS_BASE +    1)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_SET                             ((ARAD_INTERNAL_FUNCS_BASE +    2)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_SET                                 ((ARAD_INTERNAL_FUNCS_BASE +    3)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_SHAPER_VALUES_SET                                   ((ARAD_INTERNAL_FUNCS_BASE +    4)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_WEIGHT_SET                                          ((ARAD_INTERNAL_FUNCS_BASE +    5)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_RATE_TO_DELAY_CAL_FORM                              ((ARAD_INTERNAL_FUNCS_BASE +    6)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_MESH_REG_FLDS_DB_GET                                ((ARAD_INTERNAL_FUNCS_BASE +    7)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_WEIGHT_GET                                          ((ARAD_INTERNAL_FUNCS_BASE +    8)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_SHAPER_VALUES_GET                                   ((ARAD_INTERNAL_FUNCS_BASE +    9)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_DELAY_CAL_TO_MAX_RATE_FORM                          ((ARAD_INTERNAL_FUNCS_BASE +   10)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_WEIGHTS_GET                                    ((ARAD_INTERNAL_FUNCS_BASE +   11)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_GLOBAL_SHAPERS_GET                             ((ARAD_INTERNAL_FUNCS_BASE +   12)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_HP_SHAPERS_GET                                 ((ARAD_INTERNAL_FUNCS_BASE +   13)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_LP_SHAPERS_SET                                 ((ARAD_INTERNAL_FUNCS_BASE +   14)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_CLOS_LP_SHAPERS_GET                                 ((ARAD_INTERNAL_FUNCS_BASE +   15)|ARAD_PROC_BITS)

#define ARAD_SCH_PORT_SCHED_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +   16)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_HP_CLASS_CONF_SET                                            ((ARAD_INTERNAL_FUNCS_BASE +   17)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_SCHED_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +   18)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_SET                                                          ((ARAD_INTERNAL_FUNCS_BASE +   19)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE +   20)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_STATUS_SET                                                   ((ARAD_INTERNAL_FUNCS_BASE +   21)|ARAD_PROC_BITS)
#define ARAD_SCH_PER1K_INFO_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +   22)|ARAD_PROC_BITS)
#define ARAD_SCH_PER1K_INFO_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +   23)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_SET                                         ((ARAD_INTERNAL_FUNCS_BASE +   24)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_GET                                         ((ARAD_INTERNAL_FUNCS_BASE +   25)|ARAD_PROC_BITS)
#define ARAD_SCH_SIMPLE_FLOW_ID_VERIFY_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +   26)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_ID_VERIFY_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +   27)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_ID_VERIFY_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +   28)|ARAD_PROC_BITS)
#define ARAD_SCH_K_FLOW_ID_VERIFY_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +   29)|ARAD_PROC_BITS)
#define ARAD_SCH_QUARTET_ID_VERIFY_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +   30)|ARAD_PROC_BITS)

#define ARAD_SCH_FLOW_SLOW_ENABLE_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +   33)|ARAD_PROC_BITS)
#define ARAD_SCH_IF_SHAPER_RATE_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +   34)|ARAD_PROC_BITS)
#define ARAD_SCH_IF_SHAPER_RATE_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +   35)|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +   36)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_GET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +   37)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_NOF_SUBFLOWS_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +   38)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_NOF_SUBFLOWS_SET                                             ((ARAD_INTERNAL_FUNCS_BASE +   39)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_SET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +   40)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_SLOW_ENABLE_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +   41)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_STATUS_SET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +   42)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_STATUS_VERIFY                                                ((ARAD_INTERNAL_FUNCS_BASE +   43)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_SUBFLOW_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +   44)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_SUBFLOW_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +   45)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_GET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +   46)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_SET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +   47)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_VERIFY                                      ((ARAD_INTERNAL_FUNCS_BASE +   48)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_VERIFY_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +   49)|ARAD_PROC_BITS)
#define ARAD_SCH_FROM_INTERNAL_CL_SUBFLOW_CONVERT                                  ((ARAD_INTERNAL_FUNCS_BASE +   50)|ARAD_PROC_BITS)
#define ARAD_SCH_FROM_INTERNAL_CL_WEIGHT_CONVERT                                   ((ARAD_INTERNAL_FUNCS_BASE +   51)|ARAD_PROC_BITS)
#define ARAD_SCH_FROM_INTERNAL_HR_SUBFLOW_CONVERT                                  ((ARAD_INTERNAL_FUNCS_BASE +   52)|ARAD_PROC_BITS)
#define ARAD_SCH_FROM_INTERNAL_HR_WEIGHT_CONVERT                                   ((ARAD_INTERNAL_FUNCS_BASE +   53)|ARAD_PROC_BITS)
#define ARAD_SCH_GROUP_TO_PORT_ASSIGN                                              ((ARAD_INTERNAL_FUNCS_BASE +   54)|ARAD_PROC_BITS)
#define ARAD_SCH_GROUP_TO_SE_ASSIGN                                                ((ARAD_INTERNAL_FUNCS_BASE +   55)|ARAD_PROC_BITS)
#define ARAD_SCH_HR_LOWEST_HP_CLASS_SELECT_GET                                     ((ARAD_INTERNAL_FUNCS_BASE +   56)|ARAD_PROC_BITS)
#define ARAD_SCH_HR_LOWEST_HP_CLASS_SELECT_SET                                     ((ARAD_INTERNAL_FUNCS_BASE +   57)|ARAD_PROC_BITS)
#define ARAD_SCH_INTERNAL_HR_MODE_TO_HR_MODE_CONVERT                               ((ARAD_INTERNAL_FUNCS_BASE +   58)|ARAD_PROC_BITS)
#define ARAD_SCH_INTERNAL_SUB_FLOW_TO_SUB_FLOW_CONVERT                             ((ARAD_INTERNAL_FUNCS_BASE +   59)|ARAD_PROC_BITS)
#define ARAD_SCH_IS_SUBFLOW_VALID                                                  ((ARAD_INTERNAL_FUNCS_BASE +   60)|ARAD_PROC_BITS)
#define ARAD_SCH_NOF_QUARTETS_TO_MAP_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +   61)|ARAD_PROC_BITS)
#define ARAD_SCH_PER1K_INFO_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +   62)|ARAD_PROC_BITS)
#define ARAD_SCH_PER1K_INFO_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +   63)|ARAD_PROC_BITS)
#define ARAD_SCH_PER1K_INFO_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +   64)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_HP_CLASS_CONF_GET_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE +   65)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_HP_CLASS_CONF_SET_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE +   66)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_HP_CLASS_CONF_VERIFY                                         ((ARAD_INTERNAL_FUNCS_BASE +   67)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_MAX_BURST_SAVE_AND_SET                                        ((ARAD_INTERNAL_FUNCS_BASE +   68)|ARAD_PROC_BITS)

#define ARAD_SCH_SE_CONFIG_GET                                                     ((ARAD_INTERNAL_FUNCS_BASE +   71)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_CONFIG_SET                                                     ((ARAD_INTERNAL_FUNCS_BASE +   72)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_DUAL_SHAPER_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +   73)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_GET_UNSAFE                                                     ((ARAD_INTERNAL_FUNCS_BASE +   74)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_GROUP_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +   75)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_GROUP_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE +   76)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_ID_AND_TYPE_MATCH_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE +   77)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_SET_UNSAFE                                                     ((ARAD_INTERNAL_FUNCS_BASE +   78)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_STATE_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +   79)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_STATE_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE +   80)|ARAD_PROC_BITS)
#define ARAD_SCH_SE_VERIFY_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +   81)|ARAD_PROC_BITS)
#define ARAD_SCH_SUB_FLOW_TO_INTERNAL_SUB_FLOW_CONVERT                             ((ARAD_INTERNAL_FUNCS_BASE +   82)|ARAD_PROC_BITS)
#define ARAD_SCH_SUBFLOWS_VERIFY_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +   83)|ARAD_PROC_BITS)
#define ARAD_SCH_TO_INTERNAL_CL_SUBFLOW_CONVERT                                    ((ARAD_INTERNAL_FUNCS_BASE +   84)|ARAD_PROC_BITS)
#define ARAD_SCH_TO_INTERNAL_CL_WEIGHT_CONVERT                                     ((ARAD_INTERNAL_FUNCS_BASE +   85)|ARAD_PROC_BITS)
#define ARAD_SCH_TO_INTERNAL_HR_WEIGHT_CONVERT                                     ((ARAD_INTERNAL_FUNCS_BASE +   86)|ARAD_PROC_BITS)
#define ARAD_SCH_TO_INTERNAL_SUBFLOW_SHAPER_CONVERT                                ((ARAD_INTERNAL_FUNCS_BASE +   87)|ARAD_PROC_BITS)
#define ARAD_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +   88)|ARAD_PROC_BITS)

#define ARAD_INTERN_RATE2CLOCK                                                     ((ARAD_INTERNAL_FUNCS_BASE +  121)|ARAD_PROC_BITS)
#define ARAD_INTERN_CLOCK2RATE                                                     ((ARAD_INTERNAL_FUNCS_BASE +  122)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_PORT_SET                                    ((ARAD_INTERNAL_FUNCS_BASE +  127)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_PORT_GET                                    ((ARAD_INTERNAL_FUNCS_BASE +  128)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_SHAPER_SET                                  ((ARAD_INTERNAL_FUNCS_BASE +  129)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_SHAPER_GET                                  ((ARAD_INTERNAL_FUNCS_BASE +  130)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_SET                                  ((ARAD_INTERNAL_FUNCS_BASE +  131)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_WFQ_GET                                  ((ARAD_INTERNAL_FUNCS_BASE +  132)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_SET                                      ((ARAD_INTERNAL_FUNCS_BASE +  133)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_BE_GET                                      ((ARAD_INTERNAL_FUNCS_BASE +  134)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_GU_SET                                      ((ARAD_INTERNAL_FUNCS_BASE +  135)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_CREDIT_SOURCE_GU_GET                                      ((ARAD_INTERNAL_FUNCS_BASE +  136)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ACTIVE_LINKS_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE +  137)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ACTIVE_LINKS_SET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  138)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ACTIVE_LINKS_GET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  139)|ARAD_PROC_BITS)

#define ARAD_MULT_MULTICAST_GROUP_INPUT                                            ((ARAD_INTERNAL_FUNCS_BASE +  140)|ARAD_PROC_BITS)

#define ARAD_MULT_MULTICAST_GROUP_ENTRY_TO_TBL                                     ((ARAD_INTERNAL_FUNCS_BASE +  141)|ARAD_PROC_BITS)
#define ARAD_MULT_MULTICAST_ASSERT_INGRESS_REPLICATION_HW_TABLE                    ((ARAD_INTERNAL_FUNCS_BASE +  142)|ARAD_PROC_BITS)
#define ARAD_MULT_MULTICAST_ID_RELOCATION                                          ((ARAD_INTERNAL_FUNCS_BASE +  143)|ARAD_PROC_BITS)
#define ARAD_MULT_ENTRY_CONTENT_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  144)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_ENTRY_CONTENT_GET                                            ((ARAD_INTERNAL_FUNCS_BASE +  145)|ARAD_PROC_BITS)
#define ARAD_MULT_EG_ENTRY_CONTENT_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  146)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_PTR_TO_OLD_LIST_GET                                          ((ARAD_INTERNAL_FUNCS_BASE +  147)|ARAD_PROC_BITS)
#define ARAD_MULT_GROUP_UPDATE_UNSAFE_JOINT                                        ((ARAD_INTERNAL_FUNCS_BASE +  149)|ARAD_PROC_BITS)
#define ARAD_MULT_LINK_LIST_PTR_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  150)|ARAD_PROC_BITS)
#define ARAD_MULT_ERASE_MULTICAST_GROUP                                            ((ARAD_INTERNAL_FUNCS_BASE +  151)|ARAD_PROC_BITS)
#define ARAD_MULT_ERASE_ONE_ENTRY                                                  ((ARAD_INTERNAL_FUNCS_BASE +  152)|ARAD_PROC_BITS)
#define ARAD_MULT_GROUP_CLOSE_UNSAFE_JOINT                                         ((ARAD_INTERNAL_FUNCS_BASE +  153)|ARAD_PROC_BITS)
#define ARAD_MULT_EG_FILL_IN_LAST_ENTRY                                            ((ARAD_INTERNAL_FUNCS_BASE +  154)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_FILL_IN_LAST_ENTRY                                           ((ARAD_INTERNAL_FUNCS_BASE +  155)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_DESTINATION_ADD_UNSAFE_INNER                                 ((ARAD_INTERNAL_FUNCS_BASE +  156)|ARAD_PROC_BITS)
#define ARAD_MULT_LAST_ENTRY_IN_LIST_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +  157)|ARAD_PROC_BITS)
#define ARAD_MULT_ADD_ENTRY_IN_END_OF_LINK_LIST                                    ((ARAD_INTERNAL_FUNCS_BASE +  158)|ARAD_PROC_BITS)
#define ARAD_MULT_FILL_IN_LAST_ENTRY                                               ((ARAD_INTERNAL_FUNCS_BASE +  159)|ARAD_PROC_BITS)
#define ARAD_MULT_EG_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY                            ((ARAD_INTERNAL_FUNCS_BASE +  160)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY                           ((ARAD_INTERNAL_FUNCS_BASE +  161)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_GROUP_SIZE_GET_UNSAFE_INNER                                  ((ARAD_INTERNAL_FUNCS_BASE +  162)|ARAD_PROC_BITS)
#define ARAD_MULT_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY                               ((ARAD_INTERNAL_FUNCS_BASE +  163)|ARAD_PROC_BITS)

#define ARAD_NIF_MAL_BASIC_CONF_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  164)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  165)|ARAD_PROC_BITS)
#define ARAD_NIF_TOPOLOGY_VALIDATE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  166)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_ENABLE_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  167)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_ENABLE_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  168)|ARAD_PROC_BITS)
#define ARAD_NIF_MIN_PACKET_SIZE_SET                                               ((ARAD_INTERNAL_FUNCS_BASE +  169)|ARAD_PROC_BITS)
#define ARAD_NIF_MIN_PACKET_SIZE_GET                                               ((ARAD_INTERNAL_FUNCS_BASE +  170)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  171)|ARAD_PROC_BITS)
#define ARAD_NIF_TOPOLOGY_VALIDATE_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  172)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_ENABLE_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  173)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_ENABLE_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  174)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_ENABLE_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  175)|ARAD_PROC_BITS)
#define ARAD_NIF_MIN_PACKET_SIZE_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  176)|ARAD_PROC_BITS)
#define ARAD_NIF_MIN_PACKET_SIZE_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE +  177)|ARAD_PROC_BITS)
#define ARAD_NIF_MIN_PACKET_SIZE_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  178)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  179)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_VERIFY                                             ((ARAD_INTERNAL_FUNCS_BASE +  180)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_XAUI_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  181)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_SGMII_SET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  182)|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_DEFAULTS_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  183)|ARAD_PROC_BITS)
#define ARAD_NIF_SPAUI_DEFAULTS_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  184)|ARAD_PROC_BITS)
#define ARAD_NIF_FAT_PIPE_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  185)|ARAD_PROC_BITS)
#define ARAD_NIF_FAT_PIPE_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  186)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CLASS_BASED_FC_SET                                          ((ARAD_INTERNAL_FUNCS_BASE +  187)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CLASS_BASED_FC_GET                                          ((ARAD_INTERNAL_FUNCS_BASE +  188)|ARAD_PROC_BITS)
#define ARAD_NIF_INBOUND_FC_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  189)|ARAD_PROC_BITS)
#define ARAD_NIF_INBOUND_FC_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  190)|ARAD_PROC_BITS)
#define ARAD_NIF_FAT_PIPE_SET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  191)|ARAD_PROC_BITS)
#define ARAD_NIF_FAT_PIPE_VERIFY                                                   ((ARAD_INTERNAL_FUNCS_BASE +  192)|ARAD_PROC_BITS)
#define ARAD_NIF_FAT_PIPE_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  193)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CLASS_BASED_FC_SET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  194)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CLASS_BASED_FC_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE +  195)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_CLASS_BASED_FC_GET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  196)|ARAD_PROC_BITS)
#define ARAD_NIF_INBOUND_FC_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  197)|ARAD_PROC_BITS)
#define ARAD_NIF_INBOUND_FC_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  198)|ARAD_PROC_BITS)
#define ARAD_NIF_INBOUND_FC_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  199)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO22_WRITE                                                      ((ARAD_INTERNAL_FUNCS_BASE +  200)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO22_READ                                                       ((ARAD_INTERNAL_FUNCS_BASE +  201)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO45_WRITE                                                      ((ARAD_INTERNAL_FUNCS_BASE +  202)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO45_READ                                                       ((ARAD_INTERNAL_FUNCS_BASE +  203)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO22_WRITE_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  204)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO22_READ_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  205)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO45_WRITE_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  206)|ARAD_PROC_BITS)
#define ARAD_NIF_MDIO45_READ_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  207)|ARAD_PROC_BITS)
#define ARAD_NIF_TYPE_GET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  213)|ARAD_PROC_BITS)
#define ARAD_NIF_TYPE_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  214)|ARAD_PROC_BITS)
#define ARAD_NIF_TYPE_FROM_EGQ_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  215)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_ENABLE_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  217)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_ENABLE_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  218)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CELL_FORMAT_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  219)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CELL_FORMAT_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  220)|ARAD_PROC_BITS)
#define ARAD_FABRIC_COEXIST_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  221)|ARAD_PROC_BITS)
#define ARAD_FABRIC_COEXIST_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  222)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECTIVITY_MAP_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +  223)|ARAD_PROC_BITS)
#define ARAD_FABRIC_STANDALONE_FAP_MODE_DETECT                                     ((ARAD_INTERNAL_FUNCS_BASE +  224)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECT_MODE_SET                                               ((ARAD_INTERNAL_FUNCS_BASE +  225)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECT_MODE_GET                                               ((ARAD_INTERNAL_FUNCS_BASE +  226)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FAP20_MAP_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  227)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FAP20_MAP_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  228)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_ENABLE_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  229)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_ENABLE_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  230)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_ENABLE_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  231)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CELL_FORMAT_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  232)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CELL_FORMAT_VERIFY                                             ((ARAD_INTERNAL_FUNCS_BASE +  233)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CELL_FORMAT_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  234)|ARAD_PROC_BITS)
#define ARAD_FABRIC_COEXIST_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  235)|ARAD_PROC_BITS)
#define ARAD_FABRIC_COEXIST_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  236)|ARAD_PROC_BITS)
#define ARAD_FABRIC_COEXIST_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  237)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECTIVITY_MAP_GET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  238)|ARAD_PROC_BITS)
#define ARAD_FABRIC_STANDALONE_FAP_MODE_DETECT_UNSAFE                              ((ARAD_INTERNAL_FUNCS_BASE +  239)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECT_MODE_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  240)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECT_MODE_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE +  241)|ARAD_PROC_BITS)
#define ARAD_FABRIC_CONNECT_MODE_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  242)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FAP20_MAP_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  243)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FAP20_MAP_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  244)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FAP20_MAP_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  245)|ARAD_PROC_BITS)

#define ARAD_MULT_EG_MULTICAST_GROUP_ENTRY_TO_TBL                                  ((ARAD_INTERNAL_FUNCS_BASE +  246)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_MULTICAST_GROUP_ENTRY_TO_TBL                                 ((ARAD_INTERNAL_FUNCS_BASE +  247)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_NEXT_LINK_LIST_PTR_SET                                       ((ARAD_INTERNAL_FUNCS_BASE +  249)|ARAD_PROC_BITS)
#define ARAD_MULT_ING_NEXT_LINK_LIST_PTR_GET                                       ((ARAD_INTERNAL_FUNCS_BASE +  250)|ARAD_PROC_BITS)
#define ARAD_MULT_RPLCT_TBL_ENTRY_OCCUPIED_BUT_EMPTY_SET                           ((ARAD_INTERNAL_FUNCS_BASE +  251)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  252)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  253)|ARAD_PROC_BITS)

#define ARAD_FABRIC_LINK_STATUS_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  254)|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINK_STATUS_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  255)|ARAD_PROC_BITS)

#define ARAD_ITM_VSQ_CONVERT_GROUP_INDX_TO_GLOBAL_INDX                             ((ARAD_INTERNAL_FUNCS_BASE +  257)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_SET_FC_INFO                                             ((ARAD_INTERNAL_FUNCS_BASE +  258)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_GET_FC_INFO                                             ((ARAD_INTERNAL_FUNCS_BASE +  259)|ARAD_PROC_BITS)
#define ARAD_ITM_CONVERT_ADMIT_ONE_TEST_TMPLT_TO_U32                               ((ARAD_INTERNAL_FUNCS_BASE +  260)|ARAD_PROC_BITS)
#define ARAD_ITM_CONVERT_U32_TO_ADMIT_ONE_TEST_TMPLT                               ((ARAD_INTERNAL_FUNCS_BASE +  261)|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_SET                            ((ARAD_INTERNAL_FUNCS_BASE +  262)|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_SET                 ((ARAD_INTERNAL_FUNCS_BASE +  263)|ARAD_PROC_BITS)
#define ARAD_ITM_MAN_EXP_BUFFER_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  264)|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_GET                            ((ARAD_INTERNAL_FUNCS_BASE +  265)|ARAD_PROC_BITS)
#define ARAD_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_GET                 ((ARAD_INTERNAL_FUNCS_BASE +  266)|ARAD_PROC_BITS)
#define ARAD_ITM_MAN_EXP_BUFFER_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  267)|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_QT_DP_INFO_TO_WRED_TBL_DATA                                  ((ARAD_INTERNAL_FUNCS_BASE +  268)|ARAD_PROC_BITS)
#define ARAD_ITM_WRED_TBL_DATA_TO_WRED_QT_DP_INFO                                  ((ARAD_INTERNAL_FUNCS_BASE +  269)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_CONVERT_GLOBAL_INDX_TO_CTGRY_INDX                             ((ARAD_INTERNAL_FUNCS_BASE +  270)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_A_SET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  271)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_B_SET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  272)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_C_SET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  273)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_D_SET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  274)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_A_GET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  275)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_B_GET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  276)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_C_GET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  277)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_D_GET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  278)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_QT_DP_INFO_TO_WRED_TBL_DATA                              ((ARAD_INTERNAL_FUNCS_BASE +  279)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_TBL_DATA_TO_WRED_QT_DP_INFO                              ((ARAD_INTERNAL_FUNCS_BASE +  280)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_WRED_GROUP_SET_INFO                                           ((ARAD_INTERNAL_FUNCS_BASE +  281)|ARAD_PROC_BITS)

#define ARAD_SCH_SE_DUAL_SHAPER_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  282)|ARAD_PROC_BITS)
#define ARAD_FIELD_FROM_REG_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  283)|ARAD_PROC_BITS)
#define ARAD_FIELD_FROM_REG_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  284)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_THRESH_TYPE_SET                                               ((ARAD_INTERNAL_FUNCS_BASE +  285)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_THRESH_TYPE_GET                                               ((ARAD_INTERNAL_FUNCS_BASE +  286)|ARAD_PROC_BITS)
#define ARAD_EGR_SCHED_DROP_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  287)|ARAD_PROC_BITS)
#define ARAD_EGR_SCHED_DROP_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  288)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  289)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  290)|ARAD_PROC_BITS)
#define ARAD_EGR_DEV_FC_SET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  291)|ARAD_PROC_BITS)
#define ARAD_EGR_DEV_FC_GET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  292)|ARAD_PROC_BITS)
#define ARAD_EGR_CHNIF_FC_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  293)|ARAD_PROC_BITS)
#define ARAD_EGR_CHNIF_FC_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  294)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_FC_SET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  295)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_FC_GET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  296)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_SET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  297)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_GET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  298)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_ENABLE_SET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  299)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_ENABLE_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  300)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_SCHEDULING_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  303)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_SCHEDULING_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  304)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_THRESH_TYPE_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  314)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_THRESH_TYPE_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE +  315)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_THRESH_TYPE_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  316)|ARAD_PROC_BITS)
#define ARAD_EGR_SCHED_DROP_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  317)|ARAD_PROC_BITS)
#define ARAD_EGR_SCHED_DROP_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  318)|ARAD_PROC_BITS)
#define ARAD_EGR_SCHED_DROP_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  319)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  320)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  321)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  322)|ARAD_PROC_BITS)
#define ARAD_EGR_DEV_FC_SET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  323)|ARAD_PROC_BITS)
#define ARAD_EGR_DEV_FC_VERIFY                                                     ((ARAD_INTERNAL_FUNCS_BASE +  324)|ARAD_PROC_BITS)
#define ARAD_EGR_DEV_FC_GET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  325)|ARAD_PROC_BITS)
#define ARAD_EGR_CHNIF_FC_SET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  326)|ARAD_PROC_BITS)
#define ARAD_EGR_CHNIF_FC_VERIFY                                                   ((ARAD_INTERNAL_FUNCS_BASE +  327)|ARAD_PROC_BITS)
#define ARAD_EGR_CHNIF_FC_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  328)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_FC_SET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  329)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_FC_VERIFY                                                     ((ARAD_INTERNAL_FUNCS_BASE +  330)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_FC_GET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  331)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_SET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  332)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_VERIFY                                                     ((ARAD_INTERNAL_FUNCS_BASE +  333)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_GET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  334)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_ENABLE_SET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  335)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_ENABLE_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE +  336)|ARAD_PROC_BITS)
#define ARAD_EGR_MCI_FC_ENABLE_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  337)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_SCHEDULING_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  341)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_SCHEDULING_VERIFY                                             ((ARAD_INTERNAL_FUNCS_BASE +  342)|ARAD_PROC_BITS)
#define ARAD_EGR_OFP_SCHEDULING_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  343)|ARAD_PROC_BITS)
#define ARAD_EGR_THRESH_TO_MNT_EXP                                                 ((ARAD_INTERNAL_FUNCS_BASE +  347)|ARAD_PROC_BITS)
#define ARAD_EGR_MNT_EXP_TO_THRESH                                                 ((ARAD_INTERNAL_FUNCS_BASE +  348)|ARAD_PROC_BITS)
#define ARAD_EGR_THRESH_FLD_TO_MNT_EXP                                             ((ARAD_INTERNAL_FUNCS_BASE +  349)|ARAD_PROC_BITS)
#define ARAD_EGR_MNT_EXP_TO_THRESH_FLD                                             ((ARAD_INTERNAL_FUNCS_BASE +  350)|ARAD_PROC_BITS)

#define ARAD_NIF_PAUSE_QUANTA_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  352)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_QUANTA_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  353)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_FRAME_SRC_ADDR_SET                                          ((ARAD_INTERNAL_FUNCS_BASE +  354)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_FRAME_SRC_ADDR_GET                                          ((ARAD_INTERNAL_FUNCS_BASE +  355)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_QUANTA_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  356)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_QUANTA_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  357)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_QUANTA_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  358)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  359)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_FRAME_SRC_ADDR_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE +  360)|ARAD_PROC_BITS)
#define ARAD_NIF_PAUSE_FRAME_SRC_ADDR_GET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE +  361)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_VSQ_VIA_NIF_SET                                           ((ARAD_INTERNAL_FUNCS_BASE +  362)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_VSQ_VIA_NIF_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +  363)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_HP_VIA_NIF_SET                                        ((ARAD_INTERNAL_FUNCS_BASE +  364)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_HP_VIA_NIF_GET                                        ((ARAD_INTERNAL_FUNCS_BASE +  365)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_SET                                     ((ARAD_INTERNAL_FUNCS_BASE +  366)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_GET                                     ((ARAD_INTERNAL_FUNCS_BASE +  367)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_OOB_SET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  368)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_OOB_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  369)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_VIA_NIF_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  371)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_VIA_NIF_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  372)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  373)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  374)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_VSQ_OFP_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  376)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_VSQ_OFP_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  377)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_GLB_OFP_HR_SET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  378)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_GLB_OFP_HR_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  379)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_HR_ENABLE_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  380)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_HR_ENABLE_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  381)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_VSQ_VIA_NIF_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  384)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_VSQ_VIA_NIF_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE +  385)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_VSQ_VIA_NIF_GET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  386)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_HP_VIA_NIF_SET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  387)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_HP_VIA_NIF_VERIFY                                     ((ARAD_INTERNAL_FUNCS_BASE +  388)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_HP_VIA_NIF_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  389)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_SET_UNSAFE                              ((ARAD_INTERNAL_FUNCS_BASE +  390)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_VERIFY                                  ((ARAD_INTERNAL_FUNCS_BASE +  391)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_GLB_LP_VIA_NIF_CB_GET_UNSAFE                              ((ARAD_INTERNAL_FUNCS_BASE +  392)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_OOB_SET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  393)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_OOB_VERIFY                                                ((ARAD_INTERNAL_FUNCS_BASE +  394)|ARAD_PROC_BITS)
#define ARAD_FC_INGR_GEN_OOB_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  395)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_VIA_NIF_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  397)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_VIA_NIF_VERIFY                                             ((ARAD_INTERNAL_FUNCS_BASE +  398)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_VIA_NIF_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  399)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  400)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  401)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  402)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_VSQ_OFP_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  404)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_VSQ_OFP_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  405)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_VSQ_OFP_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  406)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_GLB_OFP_HR_SET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  407)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_GLB_OFP_HR_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE +  408)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_GLB_OFP_HR_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  409)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_HR_ENABLE_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  410)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_HR_ENABLE_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  411)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_HR_ENABLE_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  412)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_ON_GLB_RCS_OVERRIDE_SET                                        ((ARAD_INTERNAL_FUNCS_BASE +  413)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_ON_GLB_RCS_OVERRIDE_GET                                        ((ARAD_INTERNAL_FUNCS_BASE +  414)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_ON_GLB_RCS_OVERRIDE_SET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  415)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_ON_GLB_RCS_OVERRIDE_VERIFY                                     ((ARAD_INTERNAL_FUNCS_BASE +  416)|ARAD_PROC_BITS)
#define ARAD_FC_RCY_ON_GLB_RCS_OVERRIDE_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  417)|ARAD_PROC_BITS)
#define ARAD_NIF_FC_SET                                                            ((ARAD_INTERNAL_FUNCS_BASE +  418)|ARAD_PROC_BITS)
#define ARAD_NIF_FC_GET                                                            ((ARAD_INTERNAL_FUNCS_BASE +  419)|ARAD_PROC_BITS)
#define ARAD_NIF_FC_SET_UNSAFE                                                     ((ARAD_INTERNAL_FUNCS_BASE +  420)|ARAD_PROC_BITS)
#define ARAD_NIF_FC_LL_VERIFY                                                      ((ARAD_INTERNAL_FUNCS_BASE +  421)|ARAD_PROC_BITS)
#define ARAD_NIF_FC_GET_UNSAFE                                                     ((ARAD_INTERNAL_FUNCS_BASE +  422)|ARAD_PROC_BITS)
#define ARAD_IHP_PROG_N00_LOAD_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  423)|ARAD_PROC_BITS)
#define ARAD_IHP_PROG_N00_LOAD                                                     ((ARAD_INTERNAL_FUNCS_BASE +  424)|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_SET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  425)|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_VERIFY                                                       ((ARAD_INTERNAL_FUNCS_BASE +  426)|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_GET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  427)|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_SET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  428)|ARAD_PROC_BITS)
#define ARAD_IHP_PORT_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  429)|ARAD_PROC_BITS)
#define ARAD_IHP_PROG_PTC_CMD_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  430)|ARAD_PROC_BITS)
#define ARAD_IHP_SHAPING_SET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  431)|ARAD_PROC_BITS)
#define ARAD_IHP_SHAPING_GET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  432)|ARAD_PROC_BITS)
#define ARAD_NIF_COUNTER_GET                                                       ((ARAD_INTERNAL_FUNCS_BASE +  433)|ARAD_PROC_BITS)
#define ARAD_NIF_COUNTER_GET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  434)|ARAD_PROC_BITS)
#define ARAD_NIF_ALL_COUNTERS_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  435)|ARAD_PROC_BITS)
#define ARAD_NIF_ALL_COUNTERS_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  436)|ARAD_PROC_BITS)
#define ARAD_NIF_LINK_STATUS_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  437)|ARAD_PROC_BITS)
#define ARAD_NIF_LINK_STATUS_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  438)|ARAD_PROC_BITS)
#define ARAD_NIF_INTERFACE_VERIFY                                                  ((ARAD_INTERNAL_FUNCS_BASE +  439)|ARAD_PROC_BITS)

#define ARAD_SRD_LANE_PHYSICAL_PARAMS_FOR_FDR_SET                                  ((ARAD_INTERNAL_FUNCS_BASE +  440)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_FOR_HDR_SET                                  ((ARAD_INTERNAL_FUNCS_BASE +  441)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_FOR_QDR_SET                                  ((ARAD_INTERNAL_FUNCS_BASE +  442)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_FOR_FDR_GET                                  ((ARAD_INTERNAL_FUNCS_BASE +  443)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_FOR_HDR_GET                                  ((ARAD_INTERNAL_FUNCS_BASE +  444)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYSICAL_PARAMS_FOR_QDR_GET                                  ((ARAD_INTERNAL_FUNCS_BASE +  445)|ARAD_PROC_BITS)
#define ARAD_SRD_LINK_RX_EYE_MONITOR_MODE_SET                                      ((ARAD_INTERNAL_FUNCS_BASE +  449)|ARAD_PROC_BITS)
#define ARAD_SRD_LINK_RX_EYE_MONITOR_MODE_GET                                      ((ARAD_INTERNAL_FUNCS_BASE +  450)|ARAD_PROC_BITS)
#define ARAD_SRD_LINK_RX_EYE_MONITOR_RUN                                           ((ARAD_INTERNAL_FUNCS_BASE +  451)|ARAD_PROC_BITS)
#define ARAD_SRD_LINK_RX_EYE_MONITOR_HEIGHT_GET                                    ((ARAD_INTERNAL_FUNCS_BASE +  452)|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_MEM_LOAD                                                      ((ARAD_INTERNAL_FUNCS_BASE +  453)|ARAD_PROC_BITS)


#define ARAD_MGMT_OPERATION_MODE_SET                                               ((ARAD_INTERNAL_FUNCS_BASE +  454)|ARAD_PROC_BITS)
#define ARAD_MGMT_OPERATION_MODE_GET                                               ((ARAD_INTERNAL_FUNCS_BASE +  455)|ARAD_PROC_BITS)
#define ARAD_REGISTER_DEVICE_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  456)|ARAD_PROC_BITS)
#define ARAD_MGMT_OPERATION_MODE_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  457)|ARAD_PROC_BITS)
#define ARAD_MGMT_OPERATION_MODE_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE +  458)|ARAD_PROC_BITS)
#define ARAD_MGMT_OPERATION_MODE_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  459)|ARAD_PROC_BITS)
#define ARAD_MGMT_OPERATION_MODE_DEVICE_TYPE_SET                                   ((ARAD_INTERNAL_FUNCS_BASE +  460)|ARAD_PROC_BITS)
#define ARAD_MGMT_OPERATION_MODE_DEVICE_TYPE_GET                                   ((ARAD_INTERNAL_FUNCS_BASE +  461)|ARAD_PROC_BITS)
#define ARAD_SW_DB_REVISION_INIT                                                   ((ARAD_INTERNAL_FUNCS_BASE +  462)|ARAD_PROC_BITS)
#define ARAD_MGMT_MODULE_INIT                                                      ((ARAD_INTERNAL_FUNCS_BASE +  463)|ARAD_PROC_BITS)

#define ARAD_IHP_STAG_HDR_DATA_SET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  464)|ARAD_PROC_BITS)
#define ARAD_IHP_STAG_HDR_DATA_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE +  465)|ARAD_PROC_BITS)
#define ARAD_IHP_STAG_HDR_DATA_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  466)|ARAD_PROC_BITS)
#define ARAD_IHP_TMLAG_HUSH_FIELD_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  467)|ARAD_PROC_BITS)
#define ARAD_IHP_TMLAG_HUSH_FIELD_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  468)|ARAD_PROC_BITS)
#define ARAD_IHP_TMLAG_HUSH_FIELD_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  469)|ARAD_PROC_BITS)
#define ARAD_IHP_STAG_HDR_DATA_SET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  470)|ARAD_PROC_BITS)
#define ARAD_IHP_STAG_HDR_DATA_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  471)|ARAD_PROC_BITS)
#define ARAD_IHP_TMLAG_HUSH_FIELD_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  472)|ARAD_PROC_BITS)
#define ARAD_IHP_TMLAG_HUSH_FIELD_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  473)|ARAD_PROC_BITS)
#define ARAD_DEVICE_CLOSE                                                          ((ARAD_INTERNAL_FUNCS_BASE +  474)|ARAD_PROC_BITS)
#define ARAD_UNREGISTER_DEVICE_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  475)|ARAD_PROC_BITS)
#define ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET                                        ((ARAD_INTERNAL_FUNCS_BASE +  476)|ARAD_PROC_BITS)
#define ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET                                        ((ARAD_INTERNAL_FUNCS_BASE +  477)|ARAD_PROC_BITS)
#define ARAD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET                                        ((ARAD_INTERNAL_FUNCS_BASE +  478)|ARAD_PROC_BITS)
#define ARAD_PORT_TO_INTERFACE_MAP_SET                                             ((ARAD_INTERNAL_FUNCS_BASE +  479)|ARAD_PROC_BITS)
#define ARAD_PORT_TO_DYNAMIC_INTERFACE_MAP_SET                                     ((ARAD_INTERNAL_FUNCS_BASE +  480)|ARAD_PROC_BITS)  
#define ARAD_PORT_TO_INTERFACE_MAP_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  481)|ARAD_PROC_BITS) 
#define ARAD_PORTS_LAG_SET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  482)|ARAD_PROC_BITS) 
#define ARAD_PORTS_LAG_GET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  483)|ARAD_PROC_BITS) 
#define ARAD_PORT_HEADER_TYPE_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  484)|ARAD_PROC_BITS) 
#define ARAD_PORT_HEADER_TYPE_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  485)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_INBOUND_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  486)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_INBOUND_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  487)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_OUTBOUND_SET                                             ((ARAD_INTERNAL_FUNCS_BASE +  488)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_OUTBOUND_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  489)|ARAD_PROC_BITS) 
#define ARAD_PORTS_SNOOP_SET                                                       ((ARAD_INTERNAL_FUNCS_BASE +  490)|ARAD_PROC_BITS) 
#define ARAD_PORTS_SNOOP_GET                                                       ((ARAD_INTERNAL_FUNCS_BASE +  491)|ARAD_PROC_BITS) 
#define ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  492)|ARAD_PROC_BITS) 
#define ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_VERIFY                                     ((ARAD_INTERNAL_FUNCS_BASE +  493)|ARAD_PROC_BITS) 
#define ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  494)|ARAD_PROC_BITS) 
#define ARAD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  495)|ARAD_PROC_BITS) 
#define ARAD_PORT_TO_DYNAMIC_INTERFACE_MAP_SET_UNSAFE                              ((ARAD_INTERNAL_FUNCS_BASE +  496)|ARAD_PROC_BITS) 
#define ARAD_PORT_TO_INTERFACE_MAP_SET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  497)|ARAD_PROC_BITS) 
#define ARAD_PORT_TO_INTERFACE_MAP_VERIFY                                          ((ARAD_INTERNAL_FUNCS_BASE +  498)|ARAD_PROC_BITS) 
#define ARAD_PORT_TO_INTERFACE_MAP_GET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  499)|ARAD_PROC_BITS) 
#define ARAD_PORTS_LAG_SET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  500)|ARAD_PROC_BITS) 
#define ARAD_PORTS_LAG_VERIFY                                                      ((ARAD_INTERNAL_FUNCS_BASE +  501)|ARAD_PROC_BITS) 
#define ARAD_PORTS_LAG_GET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  502)|ARAD_PROC_BITS) 
#define ARAD_PORT_HEADER_TYPE_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  503)|ARAD_PROC_BITS) 
#define ARAD_PORT_HEADER_TYPE_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  504)|ARAD_PROC_BITS) 
#define ARAD_PORT_HEADER_TYPE_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  505)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_INBOUND_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  506)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_INBOUND_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  507)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_INBOUND_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  508)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_OUTBOUND_SET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  509)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_OUTBOUND_VERIFY                                          ((ARAD_INTERNAL_FUNCS_BASE +  510)|ARAD_PROC_BITS) 
#define ARAD_PORTS_MIRROR_OUTBOUND_GET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  511)|ARAD_PROC_BITS) 
#define ARAD_PORTS_SNOOP_SET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  512)|ARAD_PROC_BITS) 
#define ARAD_PORTS_SNOOP_VERIFY                                                    ((ARAD_INTERNAL_FUNCS_BASE +  513)|ARAD_PROC_BITS) 
#define ARAD_PORTS_SNOOP_GET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  514)|ARAD_PROC_BITS) 
#define ARAD_EGQ_PPCT_TBL_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  515)|ARAD_PROC_BITS)
#define ARAD_EGQ_PPCT_TBL_SET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  516)|ARAD_PROC_BITS)
#define ARAD_PORTS_LOGICAL_SYS_ID_BUILD                                            ((ARAD_INTERNAL_FUNCS_BASE +  520)|ARAD_PROC_BITS)
#define ARAD_PORTS_LOGICAL_SYS_ID_PARSE                                            ((ARAD_INTERNAL_FUNCS_BASE +  521)|ARAD_PROC_BITS)
#define ARAD_PORTS_ITMH_EXTENSION_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  522)|ARAD_PROC_BITS)
#define ARAD_PORTS_ITMH_EXTENSION_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  523)|ARAD_PROC_BITS)
#define ARAD_PORTS_SHAPING_HEADER_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  524)|ARAD_PROC_BITS)
#define ARAD_PORTS_SHAPING_HEADER_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  525)|ARAD_PROC_BITS)
#define ARAD_PORTS_FORWARDING_HEADER_SET                                           ((ARAD_INTERNAL_FUNCS_BASE +  526)|ARAD_PROC_BITS)
#define ARAD_PORTS_FORWARDING_HEADER_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +  527)|ARAD_PROC_BITS)
#define ARAD_PORTS_STAG_FIELDS_SET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  528)|ARAD_PROC_BITS)
#define ARAD_PORTS_STAG_FIELDS_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  529)|ARAD_PROC_BITS)
#define ARAD_PORTS_FTMH_EXTENSION_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  530)|ARAD_PROC_BITS)
#define ARAD_PORTS_FTMH_EXTENSION_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  531)|ARAD_PROC_BITS)
#define ARAD_PORTS_OTMH_EXTENSION_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  532)|ARAD_PROC_BITS)
#define ARAD_PORTS_OTMH_EXTENSION_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  533)|ARAD_PROC_BITS)
#define ARAD_PORTS_STAG_FIELDS_SET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  534)|ARAD_PROC_BITS)
#define ARAD_PORTS_STAG_FIELDS_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE +  535)|ARAD_PROC_BITS)
#define ARAD_PORTS_STAG_FIELDS_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  536)|ARAD_PROC_BITS)
#define ARAD_PORTS_FTMH_EXTENSION_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  537)|ARAD_PROC_BITS)
#define ARAD_PORTS_FTMH_EXTENSION_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  538)|ARAD_PROC_BITS)
#define ARAD_PORTS_FTMH_EXTENSION_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  539)|ARAD_PROC_BITS)
#define ARAD_PORTS_OTMH_EXTENSION_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  540)|ARAD_PROC_BITS)
#define ARAD_PORTS_OTMH_EXTENSION_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  541)|ARAD_PROC_BITS)
#define ARAD_PORTS_OTMH_EXTENSION_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  542)|ARAD_PROC_BITS)
#define ARAD_PORTS_FORWARDING_HEADER_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE +  543)|ARAD_PROC_BITS)
#define ARAD_PORTS_FORWARDING_HEADER_GET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  544)|ARAD_PROC_BITS)
#define ARAD_PORTS_ITMH_EXTENSION_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  545)|ARAD_PROC_BITS)
#define ARAD_PORTS_ITMH_EXTENSION_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  546)|ARAD_PROC_BITS)
#define ARAD_PORTS_ITMH_EXTENSION_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  547)|ARAD_PROC_BITS)
#define ARAD_PORTS_SHAPING_HEADER_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  548)|ARAD_PROC_BITS)
#define ARAD_PORTS_SHAPING_HEADER_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  549)|ARAD_PROC_BITS)
#define ARAD_PORTS_SHAPING_HEADER_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  550)|ARAD_PROC_BITS)
#define ARAD_PORTS_FORWARDING_HEADER_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  551)|ARAD_PROC_BITS)
#define ARAD_HPU_ITMH_BUILD_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  560)|ARAD_PROC_BITS)
#define ARAD_HPU_ITMH_BUILD                                                        ((ARAD_INTERNAL_FUNCS_BASE +  561)|ARAD_PROC_BITS)
#define ARAD_HPU_ITMH_PARSE                                                        ((ARAD_INTERNAL_FUNCS_BASE +  562)|ARAD_PROC_BITS)
#define ARAD_HPU_FTMH_BUILD_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  563)|ARAD_PROC_BITS)
#define ARAD_HPU_FTMH_BUILD                                                        ((ARAD_INTERNAL_FUNCS_BASE +  564)|ARAD_PROC_BITS)
#define ARAD_HPU_FTMH_PARSE                                                        ((ARAD_INTERNAL_FUNCS_BASE +  565)|ARAD_PROC_BITS)
#define ARAD_HPU_OTMH_BUILD_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  566)|ARAD_PROC_BITS)
#define ARAD_HPU_OTMH_BUILD                                                        ((ARAD_INTERNAL_FUNCS_BASE +  567)|ARAD_PROC_BITS)
#define ARAD_HPU_OTMH_PARSE                                                        ((ARAD_INTERNAL_FUNCS_BASE +  568)|ARAD_PROC_BITS)
#define ARAD_ITM_DRAM_BUFFS_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  569)|ARAD_PROC_BITS)
#define ARAD_ITM_DRAM_BUFFS_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  570)|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_WAIT_FOR_INIT                                                 ((ARAD_INTERNAL_FUNCS_BASE +  571)|ARAD_PROC_BITS)
#define ARAD_ITM_DRAM_BUFFS_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  573)|ARAD_PROC_BITS)
#define ARAD_ITM_DRAM_BUFFS_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  574)|ARAD_PROC_BITS)
#define ARAD_ITM_DRAM_BUFFS_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  575)|ARAD_PROC_BITS)
#define ARAD_MGMT_BLOCKS_INIT_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  576)|ARAD_PROC_BITS)
#define ARAD_EGR_QUEUING_REGS_INIT                                                 ((ARAD_INTERNAL_FUNCS_BASE +  577)|ARAD_PROC_BITS)
#define ARAD_EGR_QUEUING_INIT                                                      ((ARAD_INTERNAL_FUNCS_BASE +  578)|ARAD_PROC_BITS)
#define ARAD_FABRIC_REGS_INIT                                                      ((ARAD_INTERNAL_FUNCS_BASE +  579)|ARAD_PROC_BITS)
#define ARAD_FABRIC_INIT                                                           ((ARAD_INTERNAL_FUNCS_BASE +  580)|ARAD_PROC_BITS)
#define ARAD_FLOW_CONTROL_REGS_INIT                                                ((ARAD_INTERNAL_FUNCS_BASE +  581)|ARAD_PROC_BITS)
#define ARAD_FLOW_CONTROL_INIT                                                     ((ARAD_INTERNAL_FUNCS_BASE +  582)|ARAD_PROC_BITS)
#define ARAD_INGRESS_HEADER_PARSING_REGS_INIT                                      ((ARAD_INTERNAL_FUNCS_BASE +  583)|ARAD_PROC_BITS)
#define ARAD_INGRESS_HEADER_PARSING_INIT                                           ((ARAD_INTERNAL_FUNCS_BASE +  584)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_REGS_INIT                                           ((ARAD_INTERNAL_FUNCS_BASE +  585)|ARAD_PROC_BITS)
#define ARAD_INGRESS_SCHEDULER_INIT                                                ((ARAD_INTERNAL_FUNCS_BASE +  586)|ARAD_PROC_BITS)
#define ARAD_NIF_REGS_INIT                                                         ((ARAD_INTERNAL_FUNCS_BASE +  587)|ARAD_PROC_BITS)
#define ARAD_NIF_INIT                                                              ((ARAD_INTERNAL_FUNCS_BASE +  588)|ARAD_PROC_BITS)
#define ARAD_PORTS_REGS_INIT                                                       ((ARAD_INTERNAL_FUNCS_BASE +  589)|ARAD_PROC_BITS)
#define ARAD_PORTS_INIT                                                            ((ARAD_INTERNAL_FUNCS_BASE +  590)|ARAD_PROC_BITS)
#define ARAD_SCHEDULER_END2END_REGS_INIT                                           ((ARAD_INTERNAL_FUNCS_BASE +  591)|ARAD_PROC_BITS)
#define ARAD_SCHEDULER_END2END_INIT                                                ((ARAD_INTERNAL_FUNCS_BASE +  592)|ARAD_PROC_BITS)
#define ARAD_SERDES_REGS_INIT                                                      ((ARAD_INTERNAL_FUNCS_BASE +  593)|ARAD_PROC_BITS)
#define ARAD_SERDES_INIT                                                           ((ARAD_INTERNAL_FUNCS_BASE +  594)|ARAD_PROC_BITS)
#define ARAD_MGMT_TBLS_INIT                                                        ((ARAD_INTERNAL_FUNCS_BASE +  595)|ARAD_PROC_BITS)
#define ARAD_MGMT_FUNCTIONAL_INIT                                                  ((ARAD_INTERNAL_FUNCS_BASE +  596)|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_SET_DEFAULTS                                                  ((ARAD_INTERNAL_FUNCS_BASE +  597)|ARAD_PROC_BITS)
#define ARAD_OLP_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  598)|ARAD_PROC_BITS)
#define ARAD_IRE_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  599)|ARAD_PROC_BITS)
#define ARAD_IDR_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  600)|ARAD_PROC_BITS)
#define ARAD_IRR_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  601)|ARAD_PROC_BITS)
#define ARAD_IHP_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  602)|ARAD_PROC_BITS)
#define ARAD_IQM_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  603)|ARAD_PROC_BITS)
#define ARAD_IPS_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  604)|ARAD_PROC_BITS)
#define ARAD_IPT_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  605)|ARAD_PROC_BITS)
#define ARAD_DPI_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  606)|ARAD_PROC_BITS)
#define ARAD_RTP_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  611)|ARAD_PROC_BITS)
#define ARAD_EGQ_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  612)|ARAD_PROC_BITS)
#define ARAD_CFC_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  613)|ARAD_PROC_BITS)
#define ARAD_SCH_SET_REPS_FOR_TBL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  614)|ARAD_PROC_BITS)
#define ARAD_GEN_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  615)|ARAD_PROC_BITS)
#define ARAD_OLP_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  616)|ARAD_PROC_BITS)
#define ARAD_IRE_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  617)|ARAD_PROC_BITS)
#define ARAD_IDR_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  618)|ARAD_PROC_BITS)
#define ARAD_IRR_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  619)|ARAD_PROC_BITS)
#define ARAD_IHP_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  620)|ARAD_PROC_BITS)
#define ARAD_IQM_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  621)|ARAD_PROC_BITS)
#define ARAD_IPS_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  622)|ARAD_PROC_BITS)
#define ARAD_IPT_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  623)|ARAD_PROC_BITS)
#define ARAD_DPI_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  624)|ARAD_PROC_BITS)
#define ARAD_RTP_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  629)|ARAD_PROC_BITS)
#define ARAD_EGQ_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  630)|ARAD_PROC_BITS)
#define ARAD_CFC_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  631)|ARAD_PROC_BITS)
#define ARAD_SCH_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  632)|ARAD_PROC_BITS)
#define ARAD_EPNI_SET_REPS_FOR_TBL                                                 ((ARAD_INTERNAL_FUNCS_BASE +  633)|ARAD_PROC_BITS)
#define ARAD_EPNI_SET_REPS_FOR_TBL_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  634)|ARAD_PROC_BITS)
#define ARAD_MGMT_OLP_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  635)|ARAD_PROC_BITS)
#define ARAD_MGMT_IRE_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  636)|ARAD_PROC_BITS)
#define ARAD_MGMT_IDR_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  637)|ARAD_PROC_BITS)
#define ARAD_MGMT_IRR_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  638)|ARAD_PROC_BITS)
#define ARAD_MGMT_IHP_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  639)|ARAD_PROC_BITS)
#define ARAD_MGMT_IQM_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  640)|ARAD_PROC_BITS)
#define ARAD_MGMT_IPS_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  641)|ARAD_PROC_BITS)
#define ARAD_MGMT_DPI_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  642)|ARAD_PROC_BITS)
#define ARAD_MGMT_RTP_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  643)|ARAD_PROC_BITS)
#define ARAD_MGMT_EGQ_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  644)|ARAD_PROC_BITS)
#define ARAD_MGMT_CFC_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  645)|ARAD_PROC_BITS)
#define ARAD_MGMT_SCH_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  646)|ARAD_PROC_BITS)
#define ARAD_MGMT_IPT_TBLS_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  647)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_SYS_PORT_ADD_VERIFY                                         ((ARAD_INTERNAL_FUNCS_BASE +  648)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_SYS_PORT_ADD_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  649)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_SYS_PORT_ADD                                                ((ARAD_INTERNAL_FUNCS_BASE +  650)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_SYS_PORT_REMOVE_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  651)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_SYS_PORT_REMOVE                                             ((ARAD_INTERNAL_FUNCS_BASE +  652)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DDR                                                         ((ARAD_INTERNAL_FUNCS_BASE +  666)|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_ADJUST_QDR                                                    ((ARAD_INTERNAL_FUNCS_BASE +  667)|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_ADJUST_SERDES                                                 ((ARAD_INTERNAL_FUNCS_BASE +  668)|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_ADJUST_CPU                                                    ((ARAD_INTERNAL_FUNCS_BASE +  669)|ARAD_PROC_BITS)
#define ARAD_MGMT_HW_ADJUST_FABRIC                                                 ((ARAD_INTERNAL_FUNCS_BASE +  670)|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_FCT_DISABLE_POLLING                               ((ARAD_INTERNAL_FUNCS_BASE +  671)|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_ENABLE_WRITE                                      ((ARAD_INTERNAL_FUNCS_BASE +  672)|ARAD_PROC_BITS)
#define ARAD_MGMT_ALL_CTRL_CELLS_FCT_ENABLE_POLLING                                ((ARAD_INTERNAL_FUNCS_BASE +  673)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_PHASE1_VERIFY                                      ((ARAD_INTERNAL_FUNCS_BASE +  674)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_PHASE2_VERIFY                                      ((ARAD_INTERNAL_FUNCS_BASE +  675)|ARAD_PROC_BITS)
#define ARAD_PORTS_FAP_AND_NIF_TYPE_MATCH_VERIFY                                   ((ARAD_INTERNAL_FUNCS_BASE +  677)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_SET_ALL                                            ((ARAD_INTERNAL_FUNCS_BASE +  678)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_GET_ALL                                            ((ARAD_INTERNAL_FUNCS_BASE +  679)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_SET_ALL                                                      ((ARAD_INTERNAL_FUNCS_BASE +  680)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_GET_ALL                                                      ((ARAD_INTERNAL_FUNCS_BASE +  681)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_VERIFY_ALL                                         ((ARAD_INTERNAL_FUNCS_BASE +  682)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_SET_ALL_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE +  683)|ARAD_PROC_BITS)
#define ARAD_NIF_MAL_BASIC_CONF_GET_ALL_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE +  684)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_VERIFY_ALL                                                   ((ARAD_INTERNAL_FUNCS_BASE +  685)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_SET_ALL_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  686)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_GET_ALL_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  687)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  688)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_GET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  689)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE                                                      ((ARAD_INTERNAL_FUNCS_BASE +  690)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SINGLE_PORT_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  691)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_INTERFACE_SHAPER_SET                                        ((ARAD_INTERNAL_FUNCS_BASE +  692)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_INTERFACE_SHAPER_GET                                        ((ARAD_INTERNAL_FUNCS_BASE +  693)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  694)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_VERIFY                                                      ((ARAD_INTERNAL_FUNCS_BASE +  695)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_GET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  696)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  697)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SINGLE_PORT_GET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  698)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_INTERFACE_SHAPER_SET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  699)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_INTERFACE_SHAPER_VERIFY                                     ((ARAD_INTERNAL_FUNCS_BASE +  700)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_INTERFACE_SHAPER_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  701)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE +  703)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_CAL_PER_LEN_BUILD                                           ((ARAD_INTERNAL_FUNCS_BASE +  704)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_CAL_IMPROVE_NOF_SLOTS                                       ((ARAD_INTERNAL_FUNCS_BASE +  705)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_CAL_LEN_CALCULATE                                           ((ARAD_INTERNAL_FUNCS_BASE +  707)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_FIXED_LEN_CAL_BUILD                                         ((ARAD_INTERNAL_FUNCS_BASE +  708)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_DEVICE_INTERFACE_RATE_SET                                   ((ARAD_INTERNAL_FUNCS_BASE +  710)|ARAD_PROC_BITS)
#define ARAD_NIF_IS_CHANNELIZED_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  711)|ARAD_PROC_BITS)
#define ARAD_NIF_IS_CHANNELIZED                                                    ((ARAD_INTERNAL_FUNCS_BASE +  712)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_FROM_SCH_PORT_RATES_TO_CALENDAR                             ((ARAD_INTERNAL_FUNCS_BASE +  713)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_FILL_SHAPER_CALENDAR_CREDITS                                ((ARAD_INTERNAL_FUNCS_BASE +  714)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_FROM_EGQ_PORT_RATES_TO_CALENDAR                             ((ARAD_INTERNAL_FUNCS_BASE +  715)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_FROM_CALENDAR_TO_PORTS_SCH_RATE                             ((ARAD_INTERNAL_FUNCS_BASE +  716)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_FROM_CALENDAR_TO_PORTS_EGQ_RATE                             ((ARAD_INTERNAL_FUNCS_BASE +  718)|ARAD_PROC_BITS)
#define ARAD_EGQ_SCM_TBL_GET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  720)|ARAD_PROC_BITS)
#define ARAD_EGQ_SCM_TBL_SET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  722)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_ACTIVE_CALENDARS_CONFIG                                     ((ARAD_INTERNAL_FUNCS_BASE +  723)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_RATE_BELOW_3_2_PARAMS_TO_DEVICE_CALC                      ((ARAD_INTERNAL_FUNCS_BASE +  724)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_RATE_ABOVE_3_2_PARAMS_TO_DEVICE_CALC                      ((ARAD_INTERNAL_FUNCS_BASE +  725)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_RATE_BELOW_3_2_PARAMS_FROM_DEVICE_CALC                    ((ARAD_INTERNAL_FUNCS_BASE +  726)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_TX_RATE_ABOVE_3_2_PARAMS_FROM_DEVICE_CALC                    ((ARAD_INTERNAL_FUNCS_BASE +  727)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT                                                             ((ARAD_INTERNAL_FUNCS_BASE +  728)|ARAD_PROC_BITS)
#define ARAD_DRAM_WAIT_FOR_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  729)|ARAD_PROC_BITS)
#define ARAD_SCH_PORT_HP_CLASS_CONF_GET                                            ((ARAD_INTERNAL_FUNCS_BASE +  730)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_UNBIND                                      ((ARAD_INTERNAL_FUNCS_BASE +  731)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_TO_QUEUE_MAPPING_UNBIND_VERIFY                               ((ARAD_INTERNAL_FUNCS_BASE +  732)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IS_RESET_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  733)|ARAD_PROC_BITS)
#define ARAD_FLOW_STATUS_INFO_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  734)|ARAD_PROC_BITS)
#define ARAD_AGG_STATUS_INFO_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  735)|ARAD_PROC_BITS)
#define ARAD_FLOW_AND_UP_PRINT_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  736)|ARAD_PROC_BITS)
#define ARAD_SCH_IS_HR_SUBFLOW_VALID                                               ((ARAD_INTERNAL_FUNCS_BASE +  737)|ARAD_PROC_BITS)
#define ARAD_SCH_IS_CL_SUBFLOW_VALID                                               ((ARAD_INTERNAL_FUNCS_BASE +  738)|ARAD_PROC_BITS)
#define ARAD_SCH_HR_TO_PORT_ASSIGN_SET                                             ((ARAD_INTERNAL_FUNCS_BASE +  739)|ARAD_PROC_BITS)
#define ARAD_SCH_HR_TO_PORT_ASSIGN_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  740)|ARAD_PROC_BITS)
#define ARAD_OFP_EGQ_SHAPER_RATE_TO_INTERNAL                                       ((ARAD_INTERNAL_FUNCS_BASE +  741)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SW_DB_WRITE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  742)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_EGQ_SHAPER_CONFIG                                           ((ARAD_INTERNAL_FUNCS_BASE +  743)|ARAD_PROC_BITS)
#define ARAD_OFP_EGQ_SHAPER_RATE_FROM_INTERNAL                                     ((ARAD_INTERNAL_FUNCS_BASE +  744)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_EXACT_TBL_INIT                                              ((ARAD_INTERNAL_FUNCS_BASE +  745)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_TABLE_CONSTRUCT                                             ((ARAD_INTERNAL_FUNCS_BASE +  746)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_ACTIVE_CALENDARS_RETRIEVE                                   ((ARAD_INTERNAL_FUNCS_BASE +  747)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_EGQ_SHAPER_RETRIEVE                                         ((ARAD_INTERNAL_FUNCS_BASE +  748)|ARAD_PROC_BITS)
#define ARAD_AQFM_MAL_DFLT_SHAPER_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  749)|ARAD_PROC_BITS)
#define ARAD_PORT_OFP_MAL_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  750)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_ACTIVE_MAL_BUILD                                            ((ARAD_INTERNAL_FUNCS_BASE +  751)|ARAD_PROC_BITS)
#define ARAD_SW_DB_EGR_PORTS_ACTIVE_MALS_GET                                       ((ARAD_INTERNAL_FUNCS_BASE +  752)|ARAD_PROC_BITS)
#define ARAD_SRD_FLD_FROM_REG_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  753)|ARAD_PROC_BITS)
#define ARAD_SRD_FLD_FROM_REG_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  754)|ARAD_PROC_BITS)
#define ARAD_SRD_EARAD_STATUS_PARSE                                                ((ARAD_INTERNAL_FUNCS_BASE +  755)|ARAD_PROC_BITS)
#define ARAD_SRD_EARAD_CMD_BUILD                                                   ((ARAD_INTERNAL_FUNCS_BASE +  756)|ARAD_PROC_BITS)
#define ARAD_SRD_ACCESS_VERIFY                                                     ((ARAD_INTERNAL_FUNCS_BASE +  757)|ARAD_PROC_BITS)
#define ARAD_SRD_REG_READ_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  758)|ARAD_PROC_BITS)
#define ARAD_SRD_FLD_WRITE_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  759)|ARAD_PROC_BITS)
#define ARAD_SRD_FLD_READ_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  760)|ARAD_PROC_BITS)
#define ARAD_SRD_REG_WRITE                                                         ((ARAD_INTERNAL_FUNCS_BASE +  761)|ARAD_PROC_BITS)
#define ARAD_SRD_REG_READ                                                          ((ARAD_INTERNAL_FUNCS_BASE +  762)|ARAD_PROC_BITS)
#define ARAD_SRD_FLD_WRITE                                                         ((ARAD_INTERNAL_FUNCS_BASE +  763)|ARAD_PROC_BITS)
#define ARAD_SRD_FLD_READ                                                          ((ARAD_INTERNAL_FUNCS_BASE +  764)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_PHYS_PARAMS_CONFIG                                           ((ARAD_INTERNAL_FUNCS_BASE +  765)|ARAD_PROC_BITS)
#define ARAD_SRD_REGS_INIT_REGS                                                    ((ARAD_INTERNAL_FUNCS_BASE +  767)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_CONFIG                                                      ((ARAD_INTERNAL_FUNCS_BASE +  768)|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_INIT_ALL_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  769)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_BEFORE_BLOCKS_OOR                                           ((ARAD_INTERNAL_FUNCS_BASE +  770)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_AFTER_BLOCKS_OOR                                            ((ARAD_INTERNAL_FUNCS_BASE +  771)|ARAD_PROC_BITS)
#define ARAD_INIT_DRAM_FBC_BUFFS_GET                                               ((ARAD_INTERNAL_FUNCS_BASE +  772)|ARAD_PROC_BITS)
#define ARAD_INIT_DRAM_MAX_WITHOUT_FBC_GET                                         ((ARAD_INTERNAL_FUNCS_BASE +  773)|ARAD_PROC_BITS)
#define ARAD_ITM_REGS_INIT                                                         ((ARAD_INTERNAL_FUNCS_BASE +  774)|ARAD_PROC_BITS)
#define ARAD_ITM_INIT                                                              ((ARAD_INTERNAL_FUNCS_BASE +  775)|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_ENABLE_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  776)|ARAD_PROC_BITS)
#define ARAD_SRD_IPU_ENABLE_ALL_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  777)|ARAD_PROC_BITS)
#define ARAD_DIAG_IDDR_SET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  779)|ARAD_PROC_BITS)
#define ARAD_DIAG_IDDR_GET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  780)|ARAD_PROC_BITS)
#define ARAD_DIAG_REGS_DUMP                                                        ((ARAD_INTERNAL_FUNCS_BASE +  781)|ARAD_PROC_BITS)
#define ARAD_DIAG_TBLS_DUMP                                                        ((ARAD_INTERNAL_FUNCS_BASE +  782)|ARAD_PROC_BITS)
#define ARAD_DIAG_PACKET_WALKTROUGH_GET                                            ((ARAD_INTERNAL_FUNCS_BASE +  783)|ARAD_PROC_BITS)
#define ARAD_DIAG_DEV_TBLS_DUMP                                                    ((ARAD_INTERNAL_FUNCS_BASE +  784)|ARAD_PROC_BITS)
#define ARAD_IPQ_ATTACHED_FLOW_PORT_GET                                            ((ARAD_INTERNAL_FUNCS_BASE +  786)|ARAD_PROC_BITS)
#define ARAD_DIAG_TBLS_DUMP_TABLES_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  787)|ARAD_PROC_BITS)
#define ARAD_DIAG_IDDR_SET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  788)|ARAD_PROC_BITS)
#define ARAD_DIAG_IDDR_GET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  789)|ARAD_PROC_BITS)
#define ARAD_READ_REG_BUFFER_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  790)|ARAD_PROC_BITS)
#define ARAD_IPQ_ATTACHED_FLOW_PORT_GET_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE +  791)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_BIST_TEST_START                                              ((ARAD_INTERNAL_FUNCS_BASE +  792)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_BIST_TEST_START_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  793)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_BIST_TEST_RESULT_GET                                         ((ARAD_INTERNAL_FUNCS_BASE +  794)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_BIST_TEST_RESULT_GET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +  795)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_BIST_TEST_START                                             ((ARAD_INTERNAL_FUNCS_BASE +  796)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_BIST_TEST_START_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  797)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_BIST_TEST_RESULT_GET                                        ((ARAD_INTERNAL_FUNCS_BASE +  798)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_BIST_TEST_RESULT_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE +  799)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_BIST_TEST_START_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE +  800)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_BIST_TEST_COUNTERS_GET_UNSAFE                                ((ARAD_INTERNAL_FUNCS_BASE +  801)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_STATUS_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  802)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_STATUS_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  803)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_BIST_TEST_COUNTERS_GET_UNSAFE                               ((ARAD_INTERNAL_FUNCS_BASE +  804)|ARAD_PROC_BITS)
#define ARAD_ITM_DBUFF_SIZE2INTERNAL                                               ((ARAD_INTERNAL_FUNCS_BASE +  805)|ARAD_PROC_BITS)
#define ARAD_ITM_DBUFF_INTERNAL2SIZE                                               ((ARAD_INTERNAL_FUNCS_BASE +  806)|ARAD_PROC_BITS)
#define ARAD_NIF_FAT_PIPE_ENABLE_GET                                               ((ARAD_INTERNAL_FUNCS_BASE +  807)|ARAD_PROC_BITS)
#define ARAD_SW_DB_FAT_PIPE_INITIALIZE                                             ((ARAD_INTERNAL_FUNCS_BASE +  808)|ARAD_PROC_BITS)
#define ARAD_MGMT_OP_MODE_RELATED_INIT                                             ((ARAD_INTERNAL_FUNCS_BASE +  809)|ARAD_PROC_BITS)
#define ARAD_PORT_OFP_MAL_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  810)|ARAD_PROC_BITS)
#define ARAD_NIF_IS_FAT_PIPE_PORT                                                  ((ARAD_INTERNAL_FUNCS_BASE +  811)|ARAD_PROC_BITS)
#define ARAD_NIF_IS_FAT_PIPE_MAL                                                   ((ARAD_INTERNAL_FUNCS_BASE +  812)|ARAD_PROC_BITS)
#define ARAD_DIAG_TBLS_DUMP_ALL                                                    ((ARAD_INTERNAL_FUNCS_BASE +  813)|ARAD_PROC_BITS)
#define ARAD_OFP_FAT_PIPE_RATE_SET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  814)|ARAD_PROC_BITS)
#define ARAD_OFP_FAT_PIPE_RATE_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE +  815)|ARAD_PROC_BITS)
#define ARAD_OFP_FAT_PIPE_RATE_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  816)|ARAD_PROC_BITS)
#define ARAD_OFP_FAT_PIPE_RATE_SET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  817)|ARAD_PROC_BITS)
#define ARAD_OFP_FAT_PIPE_RATE_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  818)|ARAD_PROC_BITS)
#define ARAD_MC_REGS_INIT                                                          ((ARAD_INTERNAL_FUNCS_BASE +  819)|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM_TBL_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  820)|ARAD_PROC_BITS)
#define ARAD_IHP_KEY_PROGRAM_TBL_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  821)|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_SET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  822)|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  823)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_PARAMS_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  824)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_PARAMS_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  825)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POWER_STATE_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  826)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POWER_STATE_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  827)|ARAD_PROC_BITS)
#define ARAD_SRD_STAR_SET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  828)|ARAD_PROC_BITS)
#define ARAD_SRD_STAR_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  829)|ARAD_PROC_BITS)
#define ARAD_SRD_ALL_SET                                                           ((ARAD_INTERNAL_FUNCS_BASE +  830)|ARAD_PROC_BITS)
#define ARAD_SRD_ALL_GET                                                           ((ARAD_INTERNAL_FUNCS_BASE +  831)|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_SET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  832)|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_VERIFY                                                       ((ARAD_INTERNAL_FUNCS_BASE +  833)|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_GET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  834)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_PARAMS_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  835)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_PARAMS_VERIFY                                             ((ARAD_INTERNAL_FUNCS_BASE +  836)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_PARAMS_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  837)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POWER_STATE_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  838)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POWER_STATE_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  839)|ARAD_PROC_BITS)
#define ARAD_SRD_LANE_POWER_STATE_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  840)|ARAD_PROC_BITS)
#define ARAD_SRD_STAR_SET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  841)|ARAD_PROC_BITS)
#define ARAD_SRD_STAR_VERIFY                                                       ((ARAD_INTERNAL_FUNCS_BASE +  842)|ARAD_PROC_BITS)
#define ARAD_SRD_STAR_GET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE +  843)|ARAD_PROC_BITS)
#define ARAD_SRD_ALL_SET_UNSAFE                                                    ((ARAD_INTERNAL_FUNCS_BASE +  844)|ARAD_PROC_BITS)
#define ARAD_SRD_ALL_VERIFY                                                        ((ARAD_INTERNAL_FUNCS_BASE +  845)|ARAD_PROC_BITS)
#define ARAD_SRD_ALL_GET_UNSAFE                                                    ((ARAD_INTERNAL_FUNCS_BASE +  846)|ARAD_PROC_BITS)
#define ARAD_SRD_INTERNAL_RATE_CALC                                                ((ARAD_INTERNAL_FUNCS_BASE +  847)|ARAD_PROC_BITS)
#define ARAD_SW_DB_DEVICE_SRD_INIT                                                 ((ARAD_INTERNAL_FUNCS_BASE +  848)|ARAD_PROC_BITS)
#define ARAD_SRD_INTERNAL_RATE_SET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  849)|ARAD_PROC_BITS)
#define ARAD_SRD_INTERNAL_RATE_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  850)|ARAD_PROC_BITS)
#define ARAD_SRD_RATE_GET_AND_VALIDATE                                             ((ARAD_INTERNAL_FUNCS_BASE +  851)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_PARAMS_CALCULATE                                          ((ARAD_INTERNAL_FUNCS_BASE +  852)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_EXPLICIT_TO_INTERN                                        ((ARAD_INTERNAL_FUNCS_BASE +  853)|ARAD_PROC_BITS)
#define ARAD_SRD_CMU_TRIM                                                          ((ARAD_INTERNAL_FUNCS_BASE +  854)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SW_DB_READ                                                  ((ARAD_INTERNAL_FUNCS_BASE +  855)|ARAD_PROC_BITS)
#define ARAD_SRD_MAC_RATE_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  856)|ARAD_PROC_BITS)
#define ARAD_SRD_AUTO_EQUALIZE                                                     ((ARAD_INTERNAL_FUNCS_BASE +  859)|ARAD_PROC_BITS)
#define ARAD_SRD_AUTO_EQUALIZE_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  860)|ARAD_PROC_BITS)
#define ARAD_SRD_RX_PHYS_PARAMS_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  861)|ARAD_PROC_BITS)
#define ARAD_SRD_RX_PHYS_PARAMS_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  862)|ARAD_PROC_BITS)
#define ARAD_SRD_RX_PHYS_PARAMS_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  863)|ARAD_PROC_BITS)
#define ARAD_SRD_RX_PHYS_PARAMS_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  864)|ARAD_PROC_BITS)
#define ARAD_SRD_RX_PHYS_PARAMS_VERIFY                                             ((ARAD_INTERNAL_FUNCS_BASE +  865)|ARAD_PROC_BITS)
#define ARAD_SW_DB_DEVICE_LBG_INIT                                                 ((ARAD_INTERNAL_FUNCS_BASE +  866)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_MODE_VERIFY                                                  ((ARAD_INTERNAL_FUNCS_BASE +  867)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_MODE_GET_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  868)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_START_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  869)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_STOP_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE +  870)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_GET_AND_CLEAR_STAT_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  871)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_MODE_SET                                                     ((ARAD_INTERNAL_FUNCS_BASE +  872)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_MODE_GET                                                     ((ARAD_INTERNAL_FUNCS_BASE +  873)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_START                                                        ((ARAD_INTERNAL_FUNCS_BASE +  874)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_STOP                                                         ((ARAD_INTERNAL_FUNCS_BASE +  875)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_GET_AND_CLEAR_STAT                                           ((ARAD_INTERNAL_FUNCS_BASE +  876)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_CLEAR_STATUSES                                               ((ARAD_INTERNAL_FUNCS_BASE +  877)|ARAD_PROC_BITS)
#define ARAD_DIAG_LPM_PORT_STATE_SAVE                                              ((ARAD_INTERNAL_FUNCS_BASE +  878)|ARAD_PROC_BITS)
#define ARAD_DIAG_LPM_PORT_STATE_LOAD_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  879)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_CONF_SET                                                     ((ARAD_INTERNAL_FUNCS_BASE +  880)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_CONF_SET_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  881)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_TRAFFIC_SEND                                                 ((ARAD_INTERNAL_FUNCS_BASE +  882)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_TRAFFIC_SEND_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  883)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_RESULT_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  884)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_RESULT_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  885)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_CLOSE                                                        ((ARAD_INTERNAL_FUNCS_BASE +  886)|ARAD_PROC_BITS)
#define ARAD_DIAG_LBG_CLOSE_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE +  887)|ARAD_PROC_BITS)
#define ARAD_NIF_ALL_NIFS_ALL_COUNTERS_GET                                         ((ARAD_INTERNAL_FUNCS_BASE +  888)|ARAD_PROC_BITS)
#define ARAD_NIF_ALL_NIFS_ALL_COUNTERS_GET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +  889)|ARAD_PROC_BITS)
#define ARAD_INIT_DRAM_NOF_BUFFS_CALC                                              ((ARAD_INTERNAL_FUNCS_BASE +  890)|ARAD_PROC_BITS)
#define ARAD_INIT_DRAM_BUFF_BOUNDARIES_CALC                                        ((ARAD_INTERNAL_FUNCS_BASE +  891)|ARAD_PROC_BITS)
#define ARAD_IPQ_QUEUE_ID_VERIFY                                                   ((ARAD_INTERNAL_FUNCS_BASE +  892)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_RATE_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  893)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_RATE_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  894)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_RATE_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  895)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_RATE_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  896)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_RATE_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  897)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_STATUS_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  898)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_STATUS_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  899)|ARAD_PROC_BITS)
#define ARAD_OFP_ALL_OFP_RATES_GET                                                 ((ARAD_INTERNAL_FUNCS_BASE +  900)|ARAD_PROC_BITS)
#define ARAD_OFP_ALL_OFP_RATES_GET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  901)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_SPECIAL_ID_DETECT                                           ((ARAD_INTERNAL_FUNCS_BASE +  902)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_MODULE_INITIALIZE                                           ((ARAD_INTERNAL_FUNCS_BASE +  903)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_INFO_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  904)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_MASK_ALL_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  905)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_UNMASK_ALL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  906)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_MASK_CLEAR_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  907)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_IS_ALL_MASKED_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +  908)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MONITOR_START_UNSAFE                         ((ARAD_INTERNAL_FUNCS_BASE +  909)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP_UNSAFE                          ((ARAD_INTERNAL_FUNCS_BASE +  910)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET_UNSAFE                          ((ARAD_INTERNAL_FUNCS_BASE +  911)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET_UNSAFE                          ((ARAD_INTERNAL_FUNCS_BASE +  912)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_MASK_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  913)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_LEVEL_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  914)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_INITIAL_MASK_LIFT                                           ((ARAD_INTERNAL_FUNCS_BASE +  915)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SINGLE_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  916)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SINGLE_INFO_READ                                            ((ARAD_INTERNAL_FUNCS_BASE +  917)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_INFO_READ                                                   ((ARAD_INTERNAL_FUNCS_BASE +  918)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_GET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  919)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MONITOR_START                                ((ARAD_INTERNAL_FUNCS_BASE +  920)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MONITOR_STOP                                 ((ARAD_INTERNAL_FUNCS_BASE +  921)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_SET                                 ((ARAD_INTERNAL_FUNCS_BASE +  922)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_SPECIFIC_CAUSE_MASK_BIT_GET                                 ((ARAD_INTERNAL_FUNCS_BASE +  923)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_MASK_ALL                                                    ((ARAD_INTERNAL_FUNCS_BASE +  924)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_UNMASK_ALL                                                  ((ARAD_INTERNAL_FUNCS_BASE +  925)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_MASK_CLEAR                                                  ((ARAD_INTERNAL_FUNCS_BASE +  926)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_HANDLER_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE +  927)|ARAD_PROC_BITS)
#define ARAD_CALLBACK_ALL_FUNCTIONS_UNREGISTER_UNSAFE                              ((ARAD_INTERNAL_FUNCS_BASE +  928)|ARAD_PROC_BITS)
#define ARAD_CALLBACK_FUNCTION_REGISTER_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE +  929)|ARAD_PROC_BITS)
#define ARAD_CALLBACK_USER_CALLBACK_FUNCTION                                       ((ARAD_INTERNAL_FUNCS_BASE +  930)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_CLEAR_ALL                                                   ((ARAD_INTERNAL_FUNCS_BASE +  931)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_PRIO_SET                                             ((ARAD_INTERNAL_FUNCS_BASE +  932)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_PRIO_GET                                             ((ARAD_INTERNAL_FUNCS_BASE +  933)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_PRIO_SET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  934)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_PRIO_VERIFY                                          ((ARAD_INTERNAL_FUNCS_BASE +  935)|ARAD_PROC_BITS)
#define ARAD_EGR_UNSCHED_DROP_PRIO_GET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  936)|ARAD_PROC_BITS)
#define ARAD_IPQ_BASE_Q_DFLT_INVALID_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  937)|ARAD_PROC_BITS)
#define ARAD_IPQ_BASE_Q_IS_VALID_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  938)|ARAD_PROC_BITS)
#define ARAD_IPQ_INIT                                                              ((ARAD_INTERNAL_FUNCS_BASE +  939)|ARAD_PROC_BITS)
#define ARAD_SSR_FORBIDDEN_VER_SIZE                                                ((ARAD_INTERNAL_FUNCS_BASE +  940)|ARAD_PROC_BITS)
#define ARAD_SSR_FORBIDDEN_VER_TRANS                                               ((ARAD_INTERNAL_FUNCS_BASE +  941)|ARAD_PROC_BITS)
#define ARAD_SSR_MAX_SW_DB_SIZE_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  942)|ARAD_PROC_BITS)
#define ARAD_SSR_SW_DB_SIZE_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  943)|ARAD_PROC_BITS)
#define ARAD_SSR_CFG_VERSION_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  944)|ARAD_PROC_BITS)
#define ARAD_SSR_TRANSFORM_DB2CURR                                                 ((ARAD_INTERNAL_FUNCS_BASE +  945)|ARAD_PROC_BITS)
#define ARAD_SSR_DATA_LOAD                                                         ((ARAD_INTERNAL_FUNCS_BASE +  946)|ARAD_PROC_BITS)
#define ARAD_SSR_DATA_SAVE                                                         ((ARAD_INTERNAL_FUNCS_BASE +  947)|ARAD_PROC_BITS)
#define ARAD_SSR_BUFFER_SAVE                                                       ((ARAD_INTERNAL_FUNCS_BASE +  948)|ARAD_PROC_BITS)
#define ARAD_SSR_BUFFER_LOAD                                                       ((ARAD_INTERNAL_FUNCS_BASE +  949)|ARAD_PROC_BITS)
#define ARAD_SSR_BUFF_SIZE_GET                                                     ((ARAD_INTERNAL_FUNCS_BASE +  950)|ARAD_PROC_BITS)
#define ARAD_NIF_LOOPBACK_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  951)|ARAD_PROC_BITS)
#define ARAD_NIF_LOOPBACK_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  952)|ARAD_PROC_BITS)
#define ARAD_NIF_LOOPBACK_SET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  953)|ARAD_PROC_BITS)
#define ARAD_NIF_LOOPBACK_VERIFY                                                   ((ARAD_INTERNAL_FUNCS_BASE +  954)|ARAD_PROC_BITS)
#define ARAD_NIF_LOOPBACK_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  955)|ARAD_PROC_BITS)
#define ARAD_MGMT_PCKT_SIZE_RANGE_SET                                              ((ARAD_INTERNAL_FUNCS_BASE +  956)|ARAD_PROC_BITS)
#define ARAD_MGMT_PCKT_SIZE_RANGE_GET                                              ((ARAD_INTERNAL_FUNCS_BASE +  957)|ARAD_PROC_BITS)
#define ARAD_MGMT_PCKT_SIZE_RANGE_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  958)|ARAD_PROC_BITS)
#define ARAD_MGMT_PCKT_SIZE_RANGE_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  959)|ARAD_PROC_BITS)
#define ARAD_MGMT_PCKT_SIZE_RANGE_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  960)|ARAD_PROC_BITS)
#define ARAD_TEXT_ERR_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE +  961)|ARAD_PROC_BITS)
#define ARAD_SRD_STAR_RESET                                                        ((ARAD_INTERNAL_FUNCS_BASE +  962)|ARAD_PROC_BITS)
#define ARAD_TOPOLOGY_STATUS_CONNECTIVITY_GET                                      ((ARAD_INTERNAL_FUNCS_BASE +  963)|ARAD_PROC_BITS)
#define ARAD_TOPOLOGY_STATUS_CONNECTIVITY_GET_UNSAFE                               ((ARAD_INTERNAL_FUNCS_BASE +  964)|ARAD_PROC_BITS)
#define ARAD_TOPOLOGY_STATUS_CONNECTIVITY_PRINT                                    ((ARAD_INTERNAL_FUNCS_BASE +  965)|ARAD_PROC_BITS)
#define ARAD_LINKS_STATUS_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE +  966)|ARAD_PROC_BITS)
#define ARAD_TRANSACTION_WITH_FE600                                                ((ARAD_INTERNAL_FUNCS_BASE +  967)|ARAD_PROC_BITS)
#define ARAD_READ_FROM_FE600_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE +  968)|ARAD_PROC_BITS)
#define ARAD_WRITE_FROM_FE600_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE +  969)|ARAD_PROC_BITS)
#define ARAD_INDIRECT_READ_FROM_FE600_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE +  970)|ARAD_PROC_BITS)
#define ARAD_INDIRECT_WRITE_FROM_FE600_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE +  971)|ARAD_PROC_BITS)
#define ARAD_SR_SEND_CELL                                                          ((ARAD_INTERNAL_FUNCS_BASE +  972)|ARAD_PROC_BITS)
#define ARAD_SR_RCV_CELL                                                           ((ARAD_INTERNAL_FUNCS_BASE +  973)|ARAD_PROC_BITS)
#define ARAD_SR_SEND_AND_WAIT_ACK                                                  ((ARAD_INTERNAL_FUNCS_BASE +  974)|ARAD_PROC_BITS)
#define ARAD_READ_FROM_FE600                                                       ((ARAD_INTERNAL_FUNCS_BASE +  975)|ARAD_PROC_BITS)
#define ARAD_WRITE_FROM_FE600                                                      ((ARAD_INTERNAL_FUNCS_BASE +  976)|ARAD_PROC_BITS)
#define ARAD_INDIRECT_READ_FROM_FE600                                              ((ARAD_INTERNAL_FUNCS_BASE +  977)|ARAD_PROC_BITS)
#define ARAD_INDIRECT_WRITE_FROM_FE600                                             ((ARAD_INTERNAL_FUNCS_BASE +  978)|ARAD_PROC_BITS)
#define ARAD_WRITE_INDIRECT_OFFSET_TO_RTP_TABLE                                    ((ARAD_INTERNAL_FUNCS_BASE +  979)|ARAD_PROC_BITS)
#define ARAD_SRD_IVCDL_SET                                                         ((ARAD_INTERNAL_FUNCS_BASE +  980)|ARAD_PROC_BITS)
#define ARAD_DBG_ROUTE_FORCE_SET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  981)|ARAD_PROC_BITS)
#define ARAD_DBG_ROUTE_FORCE_SET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  982)|ARAD_PROC_BITS)
#define ARAD_DBG_ROUTE_FORCE_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  983)|ARAD_PROC_BITS)
#define ARAD_DBG_ROUTE_FORCE_VERIFY                                                ((ARAD_INTERNAL_FUNCS_BASE +  984)|ARAD_PROC_BITS)
#define ARAD_DBG_ROUTE_FORCE_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE +  985)|ARAD_PROC_BITS)
#define ARAD_DBG_AUTOCREDIT_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  986)|ARAD_PROC_BITS)
#define ARAD_DBG_AUTOCREDIT_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  987)|ARAD_PROC_BITS)
#define ARAD_DBG_AUTOCREDIT_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  988)|ARAD_PROC_BITS)
#define ARAD_DBG_AUTOCREDIT_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  989)|ARAD_PROC_BITS)
#define ARAD_DBG_AUTOCREDIT_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  990)|ARAD_PROC_BITS)
#define ARAD_DBG_EGRESS_SHAPING_ENABLE_SET                                         ((ARAD_INTERNAL_FUNCS_BASE +  991)|ARAD_PROC_BITS)
#define ARAD_DBG_EGRESS_SHAPING_ENABLE_GET                                         ((ARAD_INTERNAL_FUNCS_BASE +  992)|ARAD_PROC_BITS)
#define ARAD_DBG_EGRESS_SHAPING_ENABLE_VERIFY                                      ((ARAD_INTERNAL_FUNCS_BASE +  993)|ARAD_PROC_BITS)
#define ARAD_DBG_EGRESS_SHAPING_ENABLE_GET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +  994)|ARAD_PROC_BITS)
#define ARAD_DBG_FLOW_CONTROL_ENABLE_SET                                           ((ARAD_INTERNAL_FUNCS_BASE +  995)|ARAD_PROC_BITS)
#define ARAD_DBG_FLOW_CONTROL_ENABLE_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  996)|ARAD_PROC_BITS)
#define ARAD_DBG_FLOW_CONTROL_ENABLE_GET                                           ((ARAD_INTERNAL_FUNCS_BASE +  997)|ARAD_PROC_BITS)
#define ARAD_DBG_FLOW_CONTROL_ENABLE_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE +  998)|ARAD_PROC_BITS)
#define ARAD_DBG_FLOW_CONTROL_ENABLE_GET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE +  999)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_PORT_MAL_GET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE + 1000)|ARAD_PROC_BITS)
#define ARAD_DBG_RATE2AUTOCREDIT_RATE                                              ((ARAD_INTERNAL_FUNCS_BASE + 1001)|ARAD_PROC_BITS)
#define ARAD_DBG_AUTOCREDIT_RATE2RATE                                              ((ARAD_INTERNAL_FUNCS_BASE + 1002)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_SET_UNSAFE                               ((ARAD_INTERNAL_FUNCS_BASE + 1003)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_VERIFY                                   ((ARAD_INTERNAL_FUNCS_BASE + 1004)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_GET_UNSAFE                               ((ARAD_INTERNAL_FUNCS_BASE + 1005)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_SET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE + 1006)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_VERIFY                                          ((ARAD_INTERNAL_FUNCS_BASE + 1007)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_GET_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE + 1008)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_SET                                      ((ARAD_INTERNAL_FUNCS_BASE + 1009)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_GET                                      ((ARAD_INTERNAL_FUNCS_BASE + 1010)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_SET                                        ((ARAD_INTERNAL_FUNCS_BASE + 1011)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_GET                                        ((ARAD_INTERNAL_FUNCS_BASE + 1012)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_STAT_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1013)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_STAT_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1014)|ARAD_PROC_BITS)
#define ARAD_CPU2CPU_WITH_FE600_WRITE_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1015)|ARAD_PROC_BITS)
#define ARAD_CPU2CPU_WITH_FE600_WRITE                                              ((ARAD_INTERNAL_FUNCS_BASE + 1016)|ARAD_PROC_BITS)
#define ARAD_BUILD_DATA_CELL_FOR_FE600                                             ((ARAD_INTERNAL_FUNCS_BASE + 1017)|ARAD_PROC_BITS)
#define ARAD_CPU2CPU_WITH_FE600_READ_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1018)|ARAD_PROC_BITS)
#define ARAD_CPU2CPU_WITH_FE600_READ                                               ((ARAD_INTERNAL_FUNCS_BASE + 1019)|ARAD_PROC_BITS)
#define ARAD_SRD_INVERT_POLARITY_FROM_ORIG                                         ((ARAD_INTERNAL_FUNCS_BASE + 1020)|ARAD_PROC_BITS)
#define ARAD_INIT_MEM_CORRECTION_ENABLE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1021)|ARAD_PROC_BITS)
#define ARAD_NIF_LOCAL_FAULT_OVRD_SET                                              ((ARAD_INTERNAL_FUNCS_BASE + 1022)|ARAD_PROC_BITS)
#define ARAD_NIF_LOCAL_FAULT_OVRD_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1023)|ARAD_PROC_BITS)
#define ARAD_NIF_LOCAL_FAULT_OVRD_GET                                              ((ARAD_INTERNAL_FUNCS_BASE + 1024)|ARAD_PROC_BITS)
#define ARAD_NIF_LOCAL_FAULT_OVRD_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1025)|ARAD_PROC_BITS)
#define ARAD_NIF_LOCAL_FAULT_OVRD_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE + 1026)|ARAD_PROC_BITS)
#define ARAD_SRD_EYE_SCAN_CALC_OPTIMUM                                             ((ARAD_INTERNAL_FUNCS_BASE + 1027)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_ENABLE_STATE_SET                                            ((ARAD_INTERNAL_FUNCS_BASE + 1028)|ARAD_PROC_BITS)
#define ARAD_NIF_SGMII_ENABLE_STATE_GET                                            ((ARAD_INTERNAL_FUNCS_BASE + 1029)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_EGQ_CALENDAR_VALIDATE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1030)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_EGQ_CALENDAR_VALIDATE_UNSAFE                                ((ARAD_INTERNAL_FUNCS_BASE + 1031)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_TEST_SETTINGS                                               ((ARAD_INTERNAL_FUNCS_BASE + 1032)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_TEST_SETTINGS_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1033)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_TEST_RANDOM                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1034)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_TEST_RANDOM_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE + 1035)|ARAD_PROC_BITS)
#define ARAD_STATUS_FLD_POLL                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1036)|ARAD_PROC_BITS)
#define ARAD_STATUS_FLD_POLL_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE + 1037)|ARAD_PROC_BITS)
#define ARAD_DRAM_INFO_SET                                                         ((ARAD_INTERNAL_FUNCS_BASE + 1038)|ARAD_PROC_BITS)
#define ARAD_DRAM_INFO_SET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1039)|ARAD_PROC_BITS)
#define ARAD_DRAM_INFO_GET                                                         ((ARAD_INTERNAL_FUNCS_BASE + 1040)|ARAD_PROC_BITS)
#define ARAD_DRAM_INFO_VERIFY                                                      ((ARAD_INTERNAL_FUNCS_BASE + 1041)|ARAD_PROC_BITS)
#define ARAD_DRAM_INFO_GET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1042)|ARAD_PROC_BITS)
#define ARAD_FC_EGR_REC_OOB_TABLES_SET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE + 1043)|ARAD_PROC_BITS)
#define ARAD_LINK_ON_OFF_SET                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1044)|ARAD_PROC_BITS)
#define ARAD_LINK_ON_OFF_GET                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1045)|ARAD_PROC_BITS)
#define ARAD_LINK_ON_OFF_SET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE + 1046)|ARAD_PROC_BITS)
#define ARAD_LINK_ON_OFF_VERIFY                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1047)|ARAD_PROC_BITS)
#define ARAD_LINK_ON_OFF_GET_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE + 1048)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_INIT                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1049)|ARAD_PROC_BITS)
#define ARAD_SRD_PRBS_MODE_SET_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE + 1050)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_BASIC_WRITE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1051)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_PRESETTINGS_SET                                             ((ARAD_INTERNAL_FUNCS_BASE + 1052)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_BASIC_READ_AND_COMPARE                                      ((ARAD_INTERNAL_FUNCS_BASE + 1053)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_WRITE                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1054)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_WRITE_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE + 1055)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_READ_AND_COMPARE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1056)|ARAD_PROC_BITS)
#define ARAD_DIAG_BIST_READ_AND_COMPARE_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE + 1057)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_INFO_SET                                                      ((ARAD_INTERNAL_FUNCS_BASE + 1058)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_INFO_SET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE + 1059)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_INFO_GET                                                      ((ARAD_INTERNAL_FUNCS_BASE + 1060)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_INFO_VERIFY                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1061)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_INFO_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE + 1062)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_REPORT_INFO_SET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1063)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_REPORT_INFO_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1064)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_REPORT_INFO_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1065)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_REPORT_INFO_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE + 1066)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_REPORT_INFO_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1067)|ARAD_PROC_BITS)
#define ARAD_DIAG_INIT                                                             ((ARAD_INTERNAL_FUNCS_BASE + 1068)|ARAD_PROC_BITS)
#define ARAD_DIAG_REGS_INIT                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1069)|ARAD_PROC_BITS)
#define ARAD_DIAG_OFFSET2BIST_ADDR_CONVERT                                         ((ARAD_INTERNAL_FUNCS_BASE + 1070)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_SET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1071)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_VERIFY                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1072)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_GET_UNSAFE                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1073)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_SET                                                          ((ARAD_INTERNAL_FUNCS_BASE + 1074)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE + 1075)|ARAD_PROC_BITS)
#define ARAD_SRD_SCIF_ENABLE_SET                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1076)|ARAD_PROC_BITS)
#define ARAD_SRD_SCIF_ENABLE_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1077)|ARAD_PROC_BITS)
#define ARAD_SRD_SCIF_ENABLE_SET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1078)|ARAD_PROC_BITS)
#define ARAD_SRD_SCIF_ENABLE_VERIFY                                                ((ARAD_INTERNAL_FUNCS_BASE + 1079)|ARAD_PROC_BITS)
#define ARAD_SRD_SCIF_ENABLE_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1080)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_RESET                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1081)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_RESET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1082)|ARAD_PROC_BITS)
#define ARAD_SRD_VALIDATE_AND_RELOCK                                               ((ARAD_INTERNAL_FUNCS_BASE + 1083)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_IDX_VERIFY                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1084)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_SET                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1085)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1086)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_READ                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1087)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_VERIFY                                                ((ARAD_INTERNAL_FUNCS_BASE + 1088)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_SET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1089)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1090)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_COUNTER_READ_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1091)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_CALLBACK_SET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1092)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_CALLBACK_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1093)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_CALLBACK_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1094)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_CALLBACK_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE + 1095)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_CALLBACK_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1096)|ARAD_PROC_BITS)
#define ARAD_PKT_INIT                                                              ((ARAD_INTERNAL_FUNCS_BASE + 1097)|ARAD_PROC_BITS)
#define ARAD_PKT_ASYNC_INTERFACE_PACKET_SEND                                       ((ARAD_INTERNAL_FUNCS_BASE + 1098)|ARAD_PROC_BITS)
#define ARAD_PKT_ASYNC_INTERFACE_PACKET_RECV                                       ((ARAD_INTERNAL_FUNCS_BASE + 1099)|ARAD_PROC_BITS)
#define ARA_PKT_PACKET_CALLBACK_SET_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE + 1100)|ARAD_PROC_BITS)
#define ARAD_DIAG_DLL_STATUS_GET                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1101)|ARAD_PROC_BITS)
#define ARAD_DIAG_DLL_STATUS_GET_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1102)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_STATUS_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1103)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_STATUS_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1104)|ARAD_PROC_BITS)
#define ARAD_SRD_STATUS_FLD_POLL_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1105)|ARAD_PROC_BITS)
#define ARAD_FABRIC_SRD_QRTT_RESET                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1106)|ARAD_PROC_BITS)
#define ARAD_NIF_ON_OFF_SET                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1107)|ARAD_PROC_BITS)
#define ARAD_NIF_ON_OFF_GET                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1108)|ARAD_PROC_BITS)
#define ARAD_FABRIC_SRD_QRTT_RESET_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE + 1109)|ARAD_PROC_BITS)
#define ARAD_NIF_ON_OFF_SET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1110)|ARAD_PROC_BITS)
#define ARAD_NIF_ON_OFF_VERIFY                                                     ((ARAD_INTERNAL_FUNCS_BASE + 1111)|ARAD_PROC_BITS)
#define ARAD_NIF_ON_OFF_GET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1112)|ARAD_PROC_BITS)
#define ARAD_SRD_TX_PHYS_EXPLICITE_TO_INTERN                                       ((ARAD_INTERNAL_FUNCS_BASE + 1113)|ARAD_PROC_BITS)
#define ARAD_MGMT_CTRL_CELLS_COUNTER_CLEAR                                         ((ARAD_INTERNAL_FUNCS_BASE + 1114)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_MEMBER_ADD                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1115)|ARAD_PROC_BITS)
#define ARAD_PORTS_LAG_MEMBER_ADD_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1116)|ARAD_PROC_BITS)
#define ARAD_DBG_QUEUE_FLUSH                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1117)|ARAD_PROC_BITS)
#define ARAD_DBG_QUEUE_FLUSH_UNSAFE                                                ((ARAD_INTERNAL_FUNCS_BASE + 1118)|ARAD_PROC_BITS)
#define ARAD_DBG_QUEUE_FLUSH_ALL                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1119)|ARAD_PROC_BITS)
#define ARAD_DBG_QUEUE_FLUSH_ALL_UNSAFE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1120)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECEIVE_MODE_SET                                           ((ARAD_INTERNAL_FUNCS_BASE + 1121)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECEIVE_MODE_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE + 1122)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECEIVE_MODE_GET                                           ((ARAD_INTERNAL_FUNCS_BASE + 1123)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECEIVE_MODE_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1124)|ARAD_PROC_BITS)
#define ARAD_PKT_PACKET_RECEIVE_MODE_GET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE + 1125)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE_DEVICE_SET                                           ((ARAD_INTERNAL_FUNCS_BASE + 1126)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE_DEVICE_SET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE + 1127)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE_DEVICE_GET                                           ((ARAD_INTERNAL_FUNCS_BASE + 1128)|ARAD_PROC_BITS)
#define ARAD_OFP_RATES_UPDATE_DEVICE_GET_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE + 1129)|ARAD_PROC_BITS)
#define ARAD_DBG_QUEUE_FLUSH_INTERNAL_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1130)|ARAD_PROC_BITS)
#define ARAD_DBG_INGR_RESET                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1131)|ARAD_PROC_BITS)
#define ARAD_DBG_INGR_RESET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1132)|ARAD_PROC_BITS)
#define ARAD_SRD_RELOCK                                                            ((ARAD_INTERNAL_FUNCS_BASE + 1133)|ARAD_PROC_BITS)
#define ARAD_SRD_RELOCK_UNSAFE                                                     ((ARAD_INTERNAL_FUNCS_BASE + 1134)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ENHANCED_SET                                              ((ARAD_INTERNAL_FUNCS_BASE + 1135)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ENHANCED_SET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1136)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ENHANCED_GET                                              ((ARAD_INTERNAL_FUNCS_BASE + 1137)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ENHANCED_SET_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE + 1138)|ARAD_PROC_BITS)
#define ARAD_MULT_FABRIC_ENHANCED_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1139)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_DIAGNOSTIC_GET                                              ((ARAD_INTERNAL_FUNCS_BASE + 1140)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_DIAGNOSTIC_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1141)|ARAD_PROC_BITS)
#define ARAD_DIAG_DRAM_DIAGNOSTIC_GET_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE + 1142)|ARAD_PROC_BITS)
#define ARAD_MULT_RPLCT_TBL_ENTRY_UNOCCUPIED_SET                                   ((ARAD_INTERNAL_FUNCS_BASE + 1143)|ARAD_PROC_BITS)
#define ARAD_MULT_RPLCT_TBL_ENTRY_UNOCCUPIED_SET_ALL                               ((ARAD_INTERNAL_FUNCS_BASE + 1144)|ARAD_PROC_BITS)
#define ARAD_MULT_RPLCT_TBL_ENTRY_IS_OCCUPIED                                      ((ARAD_INTERNAL_FUNCS_BASE + 1145)|ARAD_PROC_BITS)
#define ARAD_MULT_RPLCT_TBL_ENTRY_IS_EMPTY                                         ((ARAD_INTERNAL_FUNCS_BASE + 1146)|ARAD_PROC_BITS)
#define ARAD_SCH_MAL_RATE_SET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE + 1147)|ARAD_PROC_BITS)
#define ARAD_SCH_MAL_RATE_GET_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE + 1148)|ARAD_PROC_BITS)
#define ARAD_SSR_IS_DEVICE_INIT_DONE                                               ((ARAD_INTERNAL_FUNCS_BASE + 1149)|ARAD_PROC_BITS)
#define ARAD_SSR_IS_DEVICE_INIT_DONE_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1150)|ARAD_PROC_BITS)
#define ARAD_DBG_ECI_ACCESS_TST                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1151)|ARAD_PROC_BITS)
#define ARAD_DBG_ECI_ACCESS_TST_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE + 1152)|ARAD_PROC_BITS)
#define ARAD_IPQ_QUEUE_QRTT_UNMAP                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1153)|ARAD_PROC_BITS)
#define ARAD_IPQ_QUEUE_QRTT_UNMAP_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1154)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_WRITE                                                     ((ARAD_INTERNAL_FUNCS_BASE + 1155)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_WRITE_PRINT                                               ((ARAD_INTERNAL_FUNCS_BASE + 1156)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_WRITE_UNSAFE                                              ((ARAD_INTERNAL_FUNCS_BASE + 1157)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_WRITE_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE + 1158)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_READ                                                      ((ARAD_INTERNAL_FUNCS_BASE + 1159)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_READ_PRINT                                                ((ARAD_INTERNAL_FUNCS_BASE + 1160)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_READ_UNSAFE                                               ((ARAD_INTERNAL_FUNCS_BASE + 1161)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_READ_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE + 1162)|ARAD_PROC_BITS)
#define ARAD_CELL_INIT                                                             ((ARAD_INTERNAL_FUNCS_BASE + 1163)|ARAD_PROC_BITS)
#define ARAD_CELL_MC_TBL_INBAND_BUILD                                              ((ARAD_INTERNAL_FUNCS_BASE + 1164)|ARAD_PROC_BITS)
#define ARAD_CELL_ACK_GET                                                          ((ARAD_INTERNAL_FUNCS_BASE + 1165)|ARAD_PROC_BITS)
#define ARAD_STAT_IFP_CNT_SELECT_SET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1166)|ARAD_PROC_BITS)
#define ARAD_STAT_IFP_CNT_SELECT_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1167)|ARAD_PROC_BITS)
#define ARAD_STAT_IFP_CNT_SELECT_GET_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1168)|ARAD_PROC_BITS)
#define ARAD_STAT_IFP_CNT_SELECT_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1169)|ARAD_PROC_BITS)
#define ARAD_STAT_IFP_CNT_SELECT_SET_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1170)|ARAD_PROC_BITS)
#define ARAD_STAT_IFP_CNT_SELECT_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1171)|ARAD_PROC_BITS)
#define ARAD_STAT_VOQ_CNT_SELECT_SET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1172)|ARAD_PROC_BITS)
#define ARAD_STAT_VOQ_CNT_SELECT_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1173)|ARAD_PROC_BITS)
#define ARAD_STAT_VOQ_CNT_SELECT_GET_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1174)|ARAD_PROC_BITS)
#define ARAD_STAT_VOQ_CNT_SELECT_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1175)|ARAD_PROC_BITS)
#define ARAD_STAT_VOQ_CNT_SELECT_SET_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1176)|ARAD_PROC_BITS)
#define ARAD_STAT_VOQ_CNT_SELECT_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1177)|ARAD_PROC_BITS)
#define ARAD_STAT_VSQ_CNT_SELECT_SET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1178)|ARAD_PROC_BITS)
#define ARAD_STAT_VSQ_CNT_SELECT_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1179)|ARAD_PROC_BITS)
#define ARAD_STAT_VSQ_CNT_SELECT_GET_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1180)|ARAD_PROC_BITS)
#define ARAD_STAT_VSQ_CNT_SELECT_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1181)|ARAD_PROC_BITS)
#define ARAD_STAT_VSQ_CNT_SELECT_SET_VERIFY                                        ((ARAD_INTERNAL_FUNCS_BASE + 1182)|ARAD_PROC_BITS)
#define ARAD_STAT_VSQ_CNT_SELECT_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1183)|ARAD_PROC_BITS)
#define ARAD_MGMT_CORE_FREQUENCY_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1184)|ARAD_PROC_BITS)
#define ARAD_MGMT_CORE_FREQUENCY_GET_PRINT                                         ((ARAD_INTERNAL_FUNCS_BASE + 1185)|ARAD_PROC_BITS)
#define ARAD_MGMT_CORE_FREQUENCY_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1186)|ARAD_PROC_BITS)
#define ARAD_DRAM_WINDOW_VALIDITY_GET                                              ((ARAD_INTERNAL_FUNCS_BASE + 1187)|ARAD_PROC_BITS)
#define ARAD_DRAM_WINDOW_VALIDITY_GET_PRINT                                        ((ARAD_INTERNAL_FUNCS_BASE + 1188)|ARAD_PROC_BITS)
#define ARAD_DRAM_WINDOW_VALIDITY_GET_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1189)|ARAD_PROC_BITS)
#define ARAD_DRAM_WINDOW_VALIDITY_GET_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE + 1190)|ARAD_PROC_BITS)
#define ARAD_SRD_EARAD_ACCESS_VALIDATE_DONE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1191)|ARAD_PROC_BITS)
#define ARAD_PA_EGR_UNSCHED_DROP_SET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1192)|ARAD_PROC_BITS)
#define ARAD_PA_EGR_UNSCHED_DROP_GET_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE + 1193)|ARAD_PROC_BITS)
#define ARAD_PA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE                              ((ARAD_INTERNAL_FUNCS_BASE + 1196)|ARAD_PROC_BITS)
#define ARAD_STAT_IS_RELEVANT_COUNTER                                              ((ARAD_INTERNAL_FUNCS_BASE + 1197)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_STATUS_PRINT                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1250)|ARAD_PROC_BITS)
#define ARAD_MC_INIT                                                               ((ARAD_INTERNAL_FUNCS_BASE + 1251)|ARAD_PROC_BITS)
#define ARAD_DBG_EGRESS_SHAPING_ENABLE_SET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE + 1252)|ARAD_PROC_BITS)

#define ARAD_INTERRUPT_INIT                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1253)|ARAD_PROC_BITS)
#define ARAD_INTERRUPT_ALL_INTERRUPTS_AND_INDICATIONS_CLEAR_UNSAFE                 ((ARAD_INTERNAL_FUNCS_BASE + 1254)|ARAD_PROC_BITS)
#define ARAD_SRD_QRTT_SYNC_FIFO_EN                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1261)|ARAD_PROC_BITS)
#define ARAD_SRD_INIT                                                              ((ARAD_INTERNAL_FUNCS_BASE + 1262)|ARAD_PROC_BITS)
#define ARAD_MULT_EG_GROUP_ENTRIES_SET                                             ((ARAD_INTERNAL_FUNCS_BASE + 1263)|ARAD_PROC_BITS)
#define ARAD_MULT_EG_TBL_ENTRY_FORMAT_GET                                          ((ARAD_INTERNAL_FUNCS_BASE + 1264)|ARAD_PROC_BITS)
#define ARAD_MULT_EG_PROGRESS_INDEX_GET                                            ((ARAD_INTERNAL_FUNCS_BASE + 1265)|ARAD_PROC_BITS)
#define ARAD_IF_TYPE_FROM_ID                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1266)|ARAD_PROC_BITS)
#define ARAD_DIAG_QDR_WINDOW_VALIDITY_GET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE + 1273)|ARAD_PROC_BITS)
#define ARAD_DBG_PCM_READINGS_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1276)|ARAD_PROC_BITS)
#define ARAD_DBG_PCM_READINGS_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1277)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_RESET                                                         ((ARAD_INTERNAL_FUNCS_BASE + 1280)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_RESET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1281)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_DETECT                                            ((ARAD_INTERNAL_FUNCS_BASE + 1282)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_DETECT_PRINT                                      ((ARAD_INTERNAL_FUNCS_BASE + 1283)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_DETECT_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE + 1284)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_DETECT_VERIFY                                     ((ARAD_INTERNAL_FUNCS_BASE + 1285)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_CONFIRM                                           ((ARAD_INTERNAL_FUNCS_BASE + 1286)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_CONFIRM_UNSAFE                                    ((ARAD_INTERNAL_FUNCS_BASE + 1287)|ARAD_PROC_BITS)
#define ARAD_DBG_SCH_SUSPECT_SPR_CONFIRM_VERIFY                                    ((ARAD_INTERNAL_FUNCS_BASE + 1288)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_SET_PRINT                                    ((ARAD_INTERNAL_FUNCS_BASE + 1289)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_SET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE + 1290)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_SET_VERIFY                                   ((ARAD_INTERNAL_FUNCS_BASE + 1291)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_GET                                          ((ARAD_INTERNAL_FUNCS_BASE + 1292)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_GET_PRINT                                    ((ARAD_INTERNAL_FUNCS_BASE + 1293)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_GET_UNSAFE                                   ((ARAD_INTERNAL_FUNCS_BASE + 1295)|ARAD_PROC_BITS)
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_SET                                          ((ARAD_INTERNAL_FUNCS_BASE + 1296)|ARAD_PROC_BITS)
#define ARAD_DEBUG_GET_PROCS_PTR                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1297)|ARAD_PROC_BITS)
#define ARAD_DEBUG_GET_ERRS_PTR                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1298)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_START                                            ((ARAD_INTERNAL_FUNCS_BASE + 1299)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_START_PRINT                                      ((ARAD_INTERNAL_FUNCS_BASE + 1300)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_START_UNSAFE                                     ((ARAD_INTERNAL_FUNCS_BASE + 1301)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_START_VERIFY                                     ((ARAD_INTERNAL_FUNCS_BASE + 1302)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_RESULT_GET                                       ((ARAD_INTERNAL_FUNCS_BASE + 1303)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_RESULT_GET_PRINT                                 ((ARAD_INTERNAL_FUNCS_BASE + 1304)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_RESULT_GET_UNSAFE                                ((ARAD_INTERNAL_FUNCS_BASE + 1305)|ARAD_PROC_BITS)
#define ARAD_DIAG_SOFT_ERROR_TEST_RESULT_GET_VERIFY                                ((ARAD_INTERNAL_FUNCS_BASE + 1306)|ARAD_PROC_BITS)
#define ARAD_DBG_DEV_RESET                                                         ((ARAD_INTERNAL_FUNCS_BASE + 1307)|ARAD_PROC_BITS)
#define ARAD_DBG_DEV_RESET_PRINT                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1308)|ARAD_PROC_BITS)
#define ARAD_DBG_DEV_RESET_UNSAFE                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1309)|ARAD_PROC_BITS)
#define ARAD_DBG_DPRC_RESET_UNSAFE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1310)|ARAD_PROC_BITS)
#define ARAD_DBG_DEV_RESET_VERIFY                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1311)|ARAD_PROC_BITS)
#define ARAD_CTRL_CELLS_EN_FAST                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1314)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_INIT                                                     ((ARAD_INTERNAL_FUNCS_BASE + 1315)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_CLOSE                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1316)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_FREE                                                     ((ARAD_INTERNAL_FUNCS_BASE + 1317)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_MALLOC                                                   ((ARAD_INTERNAL_FUNCS_BASE + 1318)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_WRITE                                                    ((ARAD_INTERNAL_FUNCS_BASE + 1319)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_OP_WRITE                                                 ((ARAD_INTERNAL_FUNCS_BASE + 1320)|ARAD_PROC_BITS)
#define ARAD_SHD_ENTRY_READ                                                        ((ARAD_INTERNAL_FUNCS_BASE + 1321)|ARAD_PROC_BITS)
#define ARAD_SHD_ENTRY_COMPARE                                                     ((ARAD_INTERNAL_FUNCS_BASE + 1322)|ARAD_PROC_BITS)
#define ARAD_SHD_ENTRY_WRITE                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1323)|ARAD_PROC_BITS)
#define ARAD_SHD_INDIRECT_RANGE_COMPARE                                            ((ARAD_INTERNAL_FUNCS_BASE + 1324)|ARAD_PROC_BITS)
#define ARAD_SHD_SCRUB_RANGE                                                       ((ARAD_INTERNAL_FUNCS_BASE + 1325)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_SEQUENCE_FIXES_APPLY_UNSAFE                                 ((ARAD_INTERNAL_FUNCS_BASE + 1326)|ARAD_PROC_BITS)
#define ARAD_DRAM_PCKT_FROM_BUFF_READ                                              ((ARAD_INTERNAL_FUNCS_BASE + 1327)|ARAD_PROC_BITS)
#define ARAD_DRAM_PCKT_FROM_BUFF_READ_UNSAFE                                       ((ARAD_INTERNAL_FUNCS_BASE + 1328)|ARAD_PROC_BITS)
#define ARAD_DRAM_PCKT_FROM_BUFF_READ_VERIFY                                       ((ARAD_INTERNAL_FUNCS_BASE + 1329)|ARAD_PROC_BITS)
#define ARAD_QDR_SET_REPS_FOR_TBL_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1330)|ARAD_PROC_BITS)
#define ARAD_DBG_FAPS_WITH_NO_MESH_LINKS_GET                                       ((ARAD_INTERNAL_FUNCS_BASE + 1331)|ARAD_PROC_BITS)
#define ARAD_DBG_FAPS_WITH_NO_MESH_LINKS_GET_UNSAFE                                ((ARAD_INTERNAL_FUNCS_BASE + 1332)|ARAD_PROC_BITS)
#define ARAD_PORTS_EXPECTED_CHAN_GET                                               ((ARAD_INTERNAL_FUNCS_BASE + 1340)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_SHAPER_SET                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1341)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_SHAPER_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE + 1342)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_SHAPER_SET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1342)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_SHAPER_GET_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE + 1343)|ARAD_PROC_BITS)
#define ARAD_FABRIC_FC_SHAPER_VERIFY                                               ((ARAD_INTERNAL_FUNCS_BASE + 1344)|ARAD_PROC_BITS)
#define ARAD_DIAG_LAST_PACKET_INFO_GET_UNSAFE                                      ((ARAD_INTERNAL_FUNCS_BASE + 1345)|ARAD_PROC_BITS)
#define ARAD_DIAG_LAST_PACKET_INFO_GET_VERIFY                                      ((ARAD_INTERNAL_FUNCS_BASE + 1345)|ARAD_PROC_BITS)
#define ARAD_DIAG_LAST_PACKET_INFO_GET                                             ((ARAD_INTERNAL_FUNCS_BASE + 1346)|ARAD_PROC_BITS)

#define ARAD_ITM_VSQ_GROUP_E_SET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  1347)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_F_SET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  1348)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_E_GET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  1349)|ARAD_PROC_BITS)
#define ARAD_ITM_VSQ_GROUP_F_GET_RT_CLASS                                          ((ARAD_INTERNAL_FUNCS_BASE +  1350)|ARAD_PROC_BITS)

#define ARAD_IPQ_TC_PROFILE_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  1351)|ARAD_PROC_BITS)
#define ARAD_IPQ_TC_PROFILE_SET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  1352)|ARAD_PROC_BITS)
#define ARAD_IPQ_TC_PROFILE_VERIFY                                                 ((ARAD_INTERNAL_FUNCS_BASE +  1353)|ARAD_PROC_BITS)
#define ARAD_IPQ_TC_PROFILE_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1354)|ARAD_PROC_BITS)
#define ARAD_IPQ_TC_PROFILE_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1355)|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_SET                                     ((ARAD_INTERNAL_FUNCS_BASE +  1356)|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_GET                                     ((ARAD_INTERNAL_FUNCS_BASE +  1357)|ARAD_PROC_BITS)
#define ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_VERIFY                                  ((ARAD_INTERNAL_FUNCS_BASE +  1358)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_WRITE_BYPASS_PLL_MODE                                       ((ARAD_INTERNAL_FUNCS_BASE +  1359)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_ENABLE_DDR_CLK_DIVIDERS                                     ((ARAD_INTERNAL_FUNCS_BASE +  1360)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DRC_CLAM_SHELL_CFG                                          ((ARAD_INTERNAL_FUNCS_BASE +  1361)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DPRC_OUT_OF_RESET                                           ((ARAD_INTERNAL_FUNCS_BASE +  1362)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DRC_PARAM_REGISTER_WRITE                                    ((ARAD_INTERNAL_FUNCS_BASE +  1379)|ARAD_PROC_BITS)
#define ARAD_MGMT_DRAM_INIT_DRC_SOFT_INIT                                          ((ARAD_INTERNAL_FUNCS_BASE +  1380)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_WAIT_DRAM_INIT_FINISHED                                     ((ARAD_INTERNAL_FUNCS_BASE +  1382)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DRC_INIT                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1383)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_DDR_CONFIGURE                                               ((ARAD_INTERNAL_FUNCS_BASE +  1384)|ARAD_PROC_BITS)
#define ARAD_MGMT_INIT_PLL_RESET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  1385)|ARAD_PROC_BITS)
#define ARAD_POLLING                                                               ((ARAD_INTERNAL_FUNCS_BASE +  1386)|ARAD_PROC_BITS)
#define ARAD_INIT_SERDES_PLL_SET                                                   ((ARAD_INTERNAL_FUNCS_BASE +  1387)|ARAD_PROC_BITS)
#define ARAD_STAT_IF_RATE_LIMIT_PRD_GET                                            ((ARAD_INTERNAL_FUNCS_BASE +  1388)|ARAD_PROC_BITS)
#define ARAD_DRAM_RUBS_WRITE                                                       ((ARAD_INTERNAL_FUNCS_BASE +  1392)|ARAD_PROC_BITS)
#define ARAD_DRAM_RUBS_WRITE_BR                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1393)|ARAD_PROC_BITS)
#define ARAD_DRAM_RUBS_READ                                                        ((ARAD_INTERNAL_FUNCS_BASE +  1394)|ARAD_PROC_BITS)
#define ARAD_DRAM_RUBS_MODIFY                                                      ((ARAD_INTERNAL_FUNCS_BASE +  1395)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_POWER_DOWN_UNUSED_DPRCS                                     ((ARAD_INTERNAL_FUNCS_BASE +  1396)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_CFG_DPRCS_LAST_IN_CHAIN                                     ((ARAD_INTERNAL_FUNCS_BASE +  1397)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_READ_UNSAFE                            ((ARAD_INTERNAL_FUNCS_BASE +  1398)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_WRITE_UNSAFE                           ((ARAD_INTERNAL_FUNCS_BASE +  1399)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_READ_UNSAFE                                ((ARAD_INTERNAL_FUNCS_BASE +  1400)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_WRITE_UNSAFE                               ((ARAD_INTERNAL_FUNCS_BASE +  1401)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_TBL_MOVE_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  1402)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_REP_VALID_POLLING                                         ((ARAD_INTERNAL_FUNCS_BASE +  1403)|ARAD_PROC_BITS)
#define ARAD_PP_IHP_TCAM_TBL_READ_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  1404)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_TBL_WRITE_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  1405)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_VALID_BIT_TBL_SET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +  1406)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_VALID_BIT_TBL_GET_UNSAFE                                  ((ARAD_INTERNAL_FUNCS_BASE +  1407)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_TBL_FLUSH_UNSAFE                                          ((ARAD_INTERNAL_FUNCS_BASE +  1408)|ARAD_PROC_BITS)
#define ARAD_PP_IHB_TCAM_TBL_COMPARE_UNSAFE                                        ((ARAD_INTERNAL_FUNCS_BASE +  1409)|ARAD_PROC_BITS)
#define ARAD_DRAM_CONFIGURE_BIST                                                   ((ARAD_INTERNAL_FUNCS_BASE +  1410)|ARAD_PROC_BITS)
#define ARAD_DRAM_BIST_ATOMIC_ACTION                                               ((ARAD_INTERNAL_FUNCS_BASE +  1413)|ARAD_PROC_BITS)
#define ARAD_DRAM_READ_BIST_ERR_CNT                                                ((ARAD_INTERNAL_FUNCS_BASE +  1414)|ARAD_PROC_BITS)
#define ARAD_DRAM_SET_MODE_REGS                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1436)|ARAD_PROC_BITS)
#define ARAD_DRAM_INFO_SET_INTERNAL_UNSAFE                                         ((ARAD_INTERNAL_FUNCS_BASE +  1437)|ARAD_PROC_BITS)
#define ARAD_DRAM_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO                         ((ARAD_INTERNAL_FUNCS_BASE +  1438)|ARAD_PROC_BITS)
#define ARAD_DRAM_ARAD_DRAM_MR_INFO_2_ARAD_DRAM_INTERNAL_MR_INFO                   ((ARAD_INTERNAL_FUNCS_BASE +  1439)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DRC_PHY_REGISTER_SET                                        ((ARAD_INTERNAL_FUNCS_BASE +  1440)|ARAD_PROC_BITS)
#define ARAD_DRAM_BIST_TEST_START_VERIFY                                           ((ARAD_INTERNAL_FUNCS_BASE +  1443)|ARAD_PROC_BITS)
#define ARAD_DRAM_BIST_TEST_START_UNSAFE                                           ((ARAD_INTERNAL_FUNCS_BASE +  1444)|ARAD_PROC_BITS)
#define ARAD_DRAM_BIST_TEST_START                                                  ((ARAD_INTERNAL_FUNCS_BASE +  1445)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DDR_CONFIGURE_MMU                                           ((ARAD_INTERNAL_FUNCS_BASE +  1446)|ARAD_PROC_BITS)
#define ARAD_B_ITM_GLOB_RCS_DROP_VERIFY                                            ((ARAD_INTERNAL_FUNCS_BASE +  1447)|ARAD_PROC_BITS)
#define ARAD_PORT_EGR_EGQ_NIF_CANCEL_EN_REGS_SET                                   ((ARAD_INTERNAL_FUNCS_BASE +  1448)|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINK_UP_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1449)|ARAD_PROC_BITS) 
#define ARAD_FABRIC_LINK_UP_GET_UNSAFE                                             ((ARAD_INTERNAL_FUNCS_BASE +  1450)|ARAD_PROC_BITS)
#define ARAD_FABRIC_LINK_UP_GET_VERIFY                                              ((ARAD_INTERNAL_FUNCS_BASE +  1451)|ARAD_PROC_BITS) 
#define ARAD_FABRIC_PORT_SPEED_GET                                                ((ARAD_INTERNAL_FUNCS_BASE +  1452)|ARAD_PROC_BITS) 
#define ARAD_FABRIC_PORT_SPEED_SET                                                ((ARAD_INTERNAL_FUNCS_BASE +  1453)|ARAD_PROC_BITS)
#define ARAD_FABRIC_PRIORITY_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1454)|ARAD_PROC_BITS) 
#define ARAD_FABRIC_PRIORITY_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1455)|ARAD_PROC_BITS)
#define ARAD_FABRIC_PRIORITY_BITS_MAPPING_TO_FDT_INDEX_GET                          ((ARAD_INTERNAL_FUNCS_BASE +  1456)|ARAD_PROC_BITS)
#define ARAD_MGMT_IPT_INIT                                                          ((ARAD_INTERNAL_FUNCS_BASE +  1457)|ARAD_PROC_BITS)
#define ARAD_MGMT_MESH_TOPOLOGY_INIT                                                ((ARAD_INTERNAL_FUNCS_BASE +  1458)|ARAD_PROC_BITS)
#define ARAD_DRAM_INIT_DPRC_INIT                                                   ((ARAD_INTERNAL_FUNCS_BASE +  1459)|ARAD_PROC_BITS) 
#define ARAD_DRAM_INIT_TUNE_DDR                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1460)|ARAD_PROC_BITS) 
#define ARAD_MGMT_OCB_PRM_VALUE_TO_FIELD_VALUE                                     ((ARAD_INTERNAL_FUNCS_BASE +  1461)|ARAD_PROC_BITS) 
#define ARAD_SW_DB_MULTICAST_TERMINATE                                             ((ARAD_INTERNAL_FUNCS_BASE +  1462)|ARAD_PROC_BITS) 
#define ARAD_PP_LEM_ACCESS_NEXT_STAMP_GET                                          ((ARAD_INTERNAL_FUNCS_BASE +  1463)|ARAD_PROC_BITS) 
#define ARAD_PORT_RX_ENABLE_SET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1464)|ARAD_PROC_BITS) 
#define ARAD_PORT_RX_ENABLE_GET                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1465)|ARAD_PROC_BITS) 
#define ARAD_PP_IHB_TCAM_TBL_BANK_OFFSET_GET                                       ((ARAD_INTERNAL_FUNCS_BASE +  1466)|ARAD_PROC_BITS) 
#define ARAD_SW_DB_LEVEL_INFO_GET                                                  ((ARAD_INTERNAL_FUNCS_BASE +  1467)|ARAD_PROC_BITS) 
#define ARAD_MGMT_SW_VER_UNSAFE                                                    ((ARAD_INTERNAL_FUNCS_BASE +  1468)|ARAD_PROC_BITS) 

typedef enum
{
  ARAD_INTERRUPT_GEN_CLEAR_READ = ARAD_SW_DB_LEVEL_INFO_GET + 1, 
  ARAD_INTERRUPT_GEN_CLEAR_WRITE,
  ARAD_OFP_RATES_SINGLE_PORT_SET_UNSAFE,
  ARAD_OFP_RATES_IF_ID2CHAN_ARB_GET_UNSAFE,
  ARAD_OFP_RATES_PORT_PRIORITY_SHAPER_SET,
  ARAD_OFP_RATES_PORT_PRIORITY_SHAPER_GET,
  ARAD_OFP_RATES_PORT_PRIORITY_SHAPER_SET_UNSAFE,
  ARAD_OFP_RATES_PORT_PRIORITY_SHAPER_GET_UNSAFE,
  ARAD_OFP_RATES_PORT_PRIORITY_SHAPER_VERIFY,
  ARAD_OFP_RATES_TCG_SHAPER_SET,
  ARAD_OFP_RATES_TCG_SHAPER_GET,
  ARAD_OFP_RATES_TCG_SHAPER_SET_UNSAFE,
  ARAD_OFP_RATES_TCG_SHAPER_GET_UNSAFE,
  ARAD_OFP_RATES_TCG_SHAPER_VERIFY,
  ARAD_OFP_RATES_RETRIEVE_EGRESS_SHAPER_REG_FIELD_NAME,
  ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_SET_UNSAFE,
  ARAD_SCH_PORT_PRIORITY_SHAPER_RATE_GET_UNSAFE,
  ARAD_ITM_COMMITTED_Q_SIZE_SET,
  ARAD_ITM_COMMITTED_Q_SIZE_SET_PRINT,
  ARAD_ITM_COMMITTED_Q_SIZE_SET_UNSAFE,
  ARAD_ITM_COMMITTED_Q_SIZE_SET_VERIFY,
  ARAD_ITM_COMMITTED_Q_SIZE_GET_VERIFY,
  ARAD_ITM_COMMITTED_Q_SIZE_GET,
  ARAD_ITM_COMMITTED_Q_SIZE_GET_PRINT,
  ARAD_ITM_COMMITTED_Q_SIZE_GET_UNSAFE,
  ARAD_ITM_PFC_TC_MAP_SET,
  ARAD_ITM_PFC_TC_MAP_GET,
  ARAD_ITM_DISCARD_DP_SET_VERIFY,
  ARAD_ITM_DISCARD_DP_SET_UNSAFE,
  ARAD_ITM_DISCARD_DP_GET_UNSAFE,
  ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_SET_UNSAFE,
  ARAD_IPQ_TRAFFIC_CLASS_PROFILE_MAP_GET_UNSAFE,
  ARAD_ITM_DISCARD_DP_SET,
  ARAD_ITM_DISCARD_DP_GET,
  ARAD_PORT_CONTROL_PCS_SET,
  ARAD_PORT_CONTROL_PCS_SET_UNSAFE,                                        
  ARAD_PORT_CONTROL_PCS_SET_VERIFY,
  ARAD_PORT_CONTROL_PCS_GET,
  ARAD_PORT_CONTROL_PCS_GET_UNSAFE,                                        
  ARAD_PORT_CONTROL_PCS_GET_VERIFY,

    ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_SET_UNSAFE,
    ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_GET_UNSAFE,                                              
  ARAD_IRE_TDM_CONFIG_TBL_GET_UNSAFE,
  ARAD_IRE_TDM_CONFIG_TBL_SET_UNSAFE,
    ARAD_IDR_CONTEXT_MRU_TBL_SET_UNSAFE,
    ARAD_IDR_CONTEXT_MRU_TBL_GET_UNSAFE,
  ARAD_IRE_CPU_CTXT_MAP_TBL_GET_UNSAFE,
  ARAD_IRE_CPU_CTXT_MAP_TBL_SET_UNSAFE,
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_GET_UNSAFE,
  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_SET_UNSAFE,
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_GET_UNSAFE,
  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_SET_UNSAFE,
  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_GET_UNSAFE,
  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_SET_UNSAFE,
  ARAD_IRR_LAG_MAPPING_TBL_GET_UNSAFE,
  ARAD_IRR_LAG_MAPPING_TBL_SET_UNSAFE,
  ARAD_IRR_LAG_NEXT_MEMBER_TBL_GET_UNSAFE,
  ARAD_IRR_LAG_NEXT_MEMBER_TBL_SET_UNSAFE,
  ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_GET_UNSAFE,
  ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_SET_UNSAFE,
  ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_GET_UNSAFE,
  ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_SET_UNSAFE,
  ARAD_EGQ_TC_DP_MAP_TBL_GET_UNSAFE,
    ARAD_EGQ_TC_DP_MAP_TBL_SET_UNSAFE,
  ARAD_NIF_PORT_BLOCK_GET,
    ARAD_NIF_PORT_BLOCK_FROM_WC_GET,
    ARAD_NIF_MAC_CONF_SET_UNSAFE,
    ARAD_NIF_MAC_ECC_MEMORY_PROTECTION_SET_UNSAFE,
    ARAD_NIF_SRD_LANES_GET,
    ARAD_NIF_TYPE_MODE_VERIFY,
    ARAD_PORT_PARSER_PROGRAM_POINTER_GET_UNSAFE,
  ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_SET_UNSAFE,
  ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_SET_UNSAFE,
    ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
    ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,
    ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
    ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,

  ARAD_IHB_PINFO_LBP_TBL_GET_UNSAFE,
  ARAD_IHB_PINFO_LBP_TBL_SET_UNSAFE,
  ARAD_IHB_PINFO_PMF_TBL_GET_UNSAFE,
  ARAD_IHB_PINFO_PMF_TBL_SET_UNSAFE,
  ARAD_IHB_PTC_INFO_TBL_GET_UNSAFE,  
  ARAD_IHB_PTC_INFO_TBL_SET_UNSAFE, 

  ARAD_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_GET_UNSAFE,
  ARAD_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_SET_UNSAFE,
  ARAD_EPNI_LFEM1_FIELD_SELECT_MAP_TBL_GET_UNSAFE,
  ARAD_EPNI_LFEM1_FIELD_SELECT_MAP_TBL_SET_UNSAFE,
  ARAD_EPNI_LFEM2_FIELD_SELECT_MAP_TBL_GET_UNSAFE,
  ARAD_EPNI_LFEM2_FIELD_SELECT_MAP_TBL_SET_UNSAFE,
    ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,
    ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
  ARAD_TDM_INIT,
  ARAD_TDM_FTMH_SET,
  ARAD_TDM_FTMH_SET_PRINT,
  ARAD_TDM_FTMH_SET_UNSAFE,
  ARAD_TDM_ING_FTMH_FILL_HEADER,
  ARAD_TDM_ING_FTMH_SET_UNSAFE,
  ARAD_TDM_EG_FTMH_SET_UNSAFE,
  ARAD_TDM_FTMH_SET_VERIFY,
  ARAD_TDM_FTMH_GET,
  ARAD_TDM_FTMH_GET_PRINT,
  ARAD_TDM_FTMH_GET_VERIFY,
  ARAD_TDM_FTMH_GET_UNSAFE,
  ARAD_TDM_ING_FTMH_GET_UNSAFE,
  ARAD_TDM_EG_FTMH_GET_UNSAFE,
  ARAD_TDM_OPT_SIZE_SET,
  ARAD_TDM_OPT_SIZE_SET_PRINT,
  ARAD_TDM_OPT_SIZE_SET_UNSAFE,
  ARAD_TDM_OPT_SIZE_SET_VERIFY,
  ARAD_TDM_OPT_SIZE_GET,
  ARAD_TDM_OPT_SIZE_GET_PRINT,
  ARAD_TDM_OPT_SIZE_GET_VERIFY,
  ARAD_TDM_OPT_SIZE_GET_UNSAFE,
  ARAD_TDM_STAND_SIZE_RANGE_SET,
  ARAD_TDM_STAND_SIZE_RANGE_SET_PRINT,
  ARAD_TDM_STAND_SIZE_RANGE_SET_UNSAFE,
  ARAD_TDM_STAND_SIZE_RANGE_SET_VERIFY,
  ARAD_TDM_STAND_SIZE_RANGE_GET,
  ARAD_TDM_STAND_SIZE_RANGE_GET_PRINT,
  ARAD_TDM_STAND_SIZE_RANGE_GET_VERIFY,
  ARAD_TDM_STAND_SIZE_RANGE_GET_UNSAFE,
  ARAD_TDM_OFP_SET_PRINT,
  ARAD_TDM_OFP_SET_UNSAFE,
  ARAD_TDM_OFP_SET_VERIFY,
  ARAD_TDM_OFP_GET_PRINT,
  ARAD_TDM_OFP_GET_VERIFY,
  ARAD_TDM_OFP_GET_UNSAFE,
  ARAD_TDM_IFP_SET_UNSAFE,
  ARAD_TDM_IFP_SET_VERIFY,
  ARAD_TDM_IFP_GET,
  ARAD_TDM_IFP_GET_VERIFY,
  ARAD_TDM_IFP_GET_UNSAFE,
  ARAD_CNT_COUNTERS_SET,
  ARAD_CNT_COUNTERS_SET_PRINT,
  ARAD_CNT_COUNTERS_SET_UNSAFE,
  ARAD_CNT_COUNTERS_SET_VERIFY,
  ARAD_CNT_COUNTERS_GET,
  ARAD_CNT_COUNTERS_GET_PRINT,
  ARAD_CNT_COUNTERS_GET_VERIFY,
  ARAD_CNT_COUNTERS_GET_UNSAFE,
  ARAD_CNT_STATUS_GET,
  ARAD_CNT_STATUS_GET_PRINT,
  ARAD_CNT_STATUS_GET_UNSAFE,
  ARAD_CNT_STATUS_GET_VERIFY,
  ARAD_CNT_ALGORITHMIC_READ,
  ARAD_CNT_ALGORITHMIC_READ_PRINT,
  ARAD_CNT_ALGORITHMIC_READ_UNSAFE,
  ARAD_CNT_ALGORITHMIC_READ_VERIFY,
  ARAD_CNT_DIRECT_READ,
  ARAD_CNT_DIRECT_READ_PRINT,
  ARAD_CNT_DIRECT_READ_UNSAFE,
  ARAD_CNT_DIRECT_READ_VERIFY,
  ARAD_CNT_Q2CNT_ID,
  ARAD_CNT_Q2CNT_ID_PRINT,
  ARAD_CNT_Q2CNT_ID_UNSAFE,
  ARAD_CNT_Q2CNT_ID_VERIFY,
  ARAD_CNT_CNT2Q_ID,
  ARAD_CNT_CNT2Q_ID_PRINT,
  ARAD_CNT_CNT2Q_ID_UNSAFE,
  ARAD_CNT_CNT2Q_ID_VERIFY,
  ARAD_CNT_METER_HDR_COMPENSATION_SET,
  ARAD_CNT_METER_HDR_COMPENSATION_SET_PRINT,
  ARAD_CNT_METER_HDR_COMPENSATION_SET_UNSAFE,
  ARAD_CNT_METER_HDR_COMPENSATION_SET_VERIFY,
  ARAD_CNT_METER_HDR_COMPENSATION_GET,
  ARAD_CNT_METER_HDR_COMPENSATION_GET_PRINT,
  ARAD_CNT_METER_HDR_COMPENSATION_GET_VERIFY,
  ARAD_CNT_METER_HDR_COMPENSATION_GET_UNSAFE,
  ARAD_ACTION_CMD_SNOOP_SET,
  ARAD_ACTION_CMD_SNOOP_SET_PRINT,
  ARAD_ACTION_CMD_SNOOP_SET_UNSAFE,
  ARAD_ACTION_CMD_SNOOP_SET_VERIFY,
  ARAD_ACTION_CMD_SNOOP_GET,
  ARAD_ACTION_CMD_SNOOP_GET_PRINT,
  ARAD_ACTION_CMD_SNOOP_GET_VERIFY,
  ARAD_ACTION_CMD_SNOOP_GET_UNSAFE,
  ARAD_ACTION_CMD_MIRROR_SET,
  ARAD_ACTION_CMD_MIRROR_SET_PRINT,
  ARAD_ACTION_CMD_MIRROR_SET_UNSAFE,
  ARAD_ACTION_CMD_MIRROR_SET_VERIFY,
  ARAD_ACTION_CMD_MIRROR_GET,
  ARAD_ACTION_CMD_MIRROR_GET_PRINT,
  ARAD_ACTION_CMD_MIRROR_GET_VERIFY,
  ARAD_ACTION_CMD_MIRROR_GET_UNSAFE,
  ARAD_CNT_DMA_SET,
  ARAD_CNT_DMA_SET_UNSAFE,
  ARAD_CNT_DMA_SET_VERIFY,
  ARAD_CNT_DMA_UNSET,
  ARAD_CNT_DMA_UNSET_UNSAFE,
  ARAD_CNT_DMA_UNSET_VERIFY,
  ARAD_MGMT_MAX_PCKT_SIZE_SET,
  ARAD_MGMT_MAX_PCKT_SIZE_SET_PRINT,
  ARAD_MGMT_MAX_PCKT_SIZE_SET_UNSAFE,
  ARAD_MGMT_MAX_PCKT_SIZE_SET_VERIFY,
  ARAD_MGMT_MAX_PCKT_SIZE_GET,
  ARAD_MGMT_MAX_PCKT_SIZE_GET_PRINT,
  ARAD_MGMT_MAX_PCKT_SIZE_GET_VERIFY,
  ARAD_MGMT_MAX_PCKT_SIZE_GET_UNSAFE,
  ARAD_MGMT_OCB_MC_RANGE_SET,
  ARAD_MGMT_OCB_MC_RANGE_SET_UNSAFE,
  ARAD_MGMT_OCB_MC_RANGE_GET,
  ARAD_MGMT_OCB_MC_RANGE_GET_UNSAFE,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_SET,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_SET_UNSAFE,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_GET,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_GET_UNSAFE,
  ARAD_NIF_WC_BASIC_CONF_SET,
  ARAD_NIF_WC_BASIC_CONF_SET_PRINT,
    ARAD_NIF_MAC_BASIC_CONF_SET_UNSAFE,
    ARAD_NIF_INIT_CONF_VERIFY,
  ARAD_NIF_WC_BASIC_CONF_SET_UNSAFE,
    ARAD_NIF_MAC_BASIC_CONF_VERIFY,
  ARAD_NIF_WC_BASIC_CONF_VERIFY,
    ARAD_NIF_MODE_GET,
    ARAD_NIF_MODE_GET_UNSAFE,
    ARAD_NIF_DEFAULT_RATE_GET,
  ARAD_NIF_WC_BASIC_CONF_GET,
  ARAD_NIF_WC_BASIC_CONF_GET_PRINT,
  ARAD_NIF_WC_ID_VERIFY,
  ARAD_NIF_IF2EGRESS_OFFSET,
  ARAD_NIF_WC_BASIC_CONF_GET_UNSAFE,
  ARAD_NIF_ILKN_SET,
  ARAD_NIF_ILKN_SET_PRINT,
  ARAD_NIF_ILKN_SET_UNSAFE,
  ARAD_NIF_ILKN_SET_VERIFY,
  ARAD_NIF_ILKN_GET,
  ARAD_NIF_ILKN_GET_PRINT,
  ARAD_NIF_ILKN_GET_VERIFY,
  ARAD_NIF_ILKN_GET_UNSAFE,
    ARAD_NIF_ID_TO_PORT_GET,
    ARAD_NIF_RATE_GET,
  ARAD_NIF_IPG_SET,
  ARAD_NIF_IPG_GET,
  ARAD_NIF_IPG_SET_UNSAFE,
  ARAD_NIF_IPG_GET_UNSAFE,
  ARAD_NIF_IPG_SET_VERIFY,
  ARAD_NIF_IPG_GET_VERIFY,
  ARAD_NIF_LINK_STATUS_GET_PRINT,
  ARAD_NIF_LINK_STATUS_GET_VERIFY,
  ARAD_FC_GEN_INBND_SET,
  ARAD_FC_GEN_INBND_SET_PRINT,
  ARAD_FC_GEN_INBND_SET_UNSAFE,
  ARAD_FC_GEN_INBND_SET_VERIFY,
  ARAD_FC_GEN_INBND_GET,
  ARAD_FC_GEN_INBND_GET_PRINT,
  ARAD_FC_GEN_INBND_GET_VERIFY,
  ARAD_FC_GEN_INBND_GET_UNSAFE,
  ARAD_FC_GEN_INBND_GLB_HP_SET,
  ARAD_FC_GEN_INBND_GLB_HP_SET_PRINT,
  ARAD_FC_GEN_INBND_GLB_HP_SET_UNSAFE,
  ARAD_FC_GEN_INBND_GLB_HP_SET_VERIFY,
  ARAD_FC_GEN_INBND_GLB_HP_GET,
  ARAD_FC_GEN_INBND_GLB_HP_GET_PRINT,
  ARAD_FC_GEN_INBND_GLB_HP_GET_VERIFY,
  ARAD_FC_GEN_INBND_GLB_HP_GET_UNSAFE,
  ARAD_FC_GEN_INBND_CNM_MAP_SET,
  ARAD_FC_GEN_INBND_CNM_MAP_SET_PRINT,
  ARAD_FC_GEN_INBND_CNM_MAP_SET_UNSAFE,
  ARAD_FC_GEN_INBND_CNM_MAP_SET_VERIFY,
  ARAD_FC_GEN_INBND_CNM_MAP_GET,
  ARAD_FC_GEN_INBND_CNM_MAP_GET_PRINT,
  ARAD_FC_GEN_INBND_CNM_MAP_GET_VERIFY,
  ARAD_FC_GEN_INBND_CNM_MAP_GET_UNSAFE,
  ARAD_FC_GEN_INBND_PFC_SET_UNSAFE,
  ARAD_FC_GEN_INBND_PFC_GET_UNSAFE,
  ARAD_FC_GEN_INBND_LL_SET_UNSAFE,
  ARAD_FC_GEN_INBND_LL_GET_UNSAFE,
  ARAD_FLOW_CONTROL_INIT_OOB_TX,
  ARAD_FLOW_CONTROL_INIT_OOB_RX,
  ARAD_FC_REC_INBND_SET,
  ARAD_FC_REC_INBND_SET_PRINT,
  ARAD_FC_REC_INBND_SET_UNSAFE,
  ARAD_FC_REC_INBND_SET_VERIFY,
  ARAD_FC_REC_INBND_GET,
  ARAD_FC_REC_INBND_GET_PRINT,
  ARAD_FC_REC_INBND_GET_VERIFY,
  ARAD_FC_REC_INBND_GET_UNSAFE,
  ARAD_FC_REC_INBND_OFP_MAP_SET,
  ARAD_FC_REC_INBND_OFP_MAP_SET_PRINT,
  ARAD_FC_REC_INBND_OFP_MAP_SET_UNSAFE,
  ARAD_FC_REC_INBND_OFP_MAP_SET_VERIFY,
  ARAD_FC_REC_INBND_OFP_MAP_GET,
  ARAD_FC_REC_INBND_OFP_MAP_GET_PRINT,
  ARAD_FC_REC_INBND_OFP_MAP_GET_VERIFY,
  ARAD_FC_REC_INBND_OFP_MAP_GET_UNSAFE,
  ARAD_FC_RCY_OFP_SET,
  ARAD_FC_RCY_OFP_SET_PRINT,
  ARAD_FC_RCY_OFP_SET_UNSAFE,
  ARAD_FC_RCY_OFP_SET_VERIFY,
  ARAD_FC_RCY_OFP_GET,
  ARAD_FC_RCY_OFP_GET_PRINT,
  ARAD_FC_RCY_OFP_GET_VERIFY,
  ARAD_FC_RCY_OFP_GET_UNSAFE,
  ARAD_FC_RCY_HR_ENABLE_SET_PRINT,
  ARAD_FC_RCY_HR_ENABLE_SET_VERIFY,
  ARAD_FC_RCY_HR_ENABLE_GET_PRINT,
  ARAD_FC_RCY_HR_ENABLE_GET_VERIFY,
  ARAD_FC_GEN_CAL_SET,
  ARAD_FC_GEN_CAL_SET_PRINT,
  ARAD_FC_GEN_CAL_SET_UNSAFE,
  ARAD_FC_GEN_CAL_SET_VERIFY,
  ARAD_FC_GEN_CAL_GET,
  ARAD_FC_GEN_CAL_GET_PRINT,
  ARAD_FC_GEN_CAL_GET_VERIFY,
  ARAD_FC_GEN_CAL_GET_UNSAFE,
  ARAD_FC_REC_CAL_SET,
  ARAD_FC_REC_CAL_SET_PRINT,
  ARAD_FC_REC_CAL_SET_UNSAFE,
  ARAD_FC_REC_CAL_SET_VERIFY,
  ARAD_FC_REC_CAL_GET,
  ARAD_FC_REC_CAL_GET_PRINT,
  ARAD_FC_REC_CAL_GET_VERIFY,
  ARAD_FC_REC_CAL_GET_UNSAFE,
  ARAD_FC_EGR_REC_OOB_STAT_GET_PRINT,
  ARAD_FC_EGR_REC_OOB_STAT_GET_VERIFY,
  ARAD_FC_ILKN_LLFC_SET,
  ARAD_FC_ILKN_LLFC_SET_PRINT,
  ARAD_FC_ILKN_LLFC_SET_UNSAFE,
  ARAD_FC_ILKN_LLFC_SET_VERIFY,
  ARAD_FC_ILKN_LLFC_GET,
  ARAD_FC_ILKN_LLFC_GET_PRINT,
  ARAD_FC_ILKN_LLFC_GET_VERIFY,
  ARAD_FC_ILKN_LLFC_GET_UNSAFE,
  ARAD_FC_ILKN_MUB_REC_SET_UNSAFE,
  ARAD_FC_ILKN_MUB_REC_GET_UNSAFE,
  ARAD_FC_ILKN_MUB_GEN_CAL_SET_UNSAFE,
  ARAD_FC_ILKN_MUB_GEN_CAL_GET_UNSAFE,
  ARAD_FC_ILKN_RETRANSMIT_SET_UNSAFE,
  ARAD_FC_ILKN_RETRANSMIT_GET_UNSAFE,
  ARAD_FC_PFC_GENERIC_BITMAP_SET,
  ARAD_FC_PFC_GENERIC_BITMAP_GET,
  ARAD_FC_PFC_GENERIC_BITMAP_VERIFY,
  ARAD_FC_PFC_GENERIC_BITMAP_GET_UNSAFE,
  ARAD_FC_PFC_GENERIC_BITMAP_SET_UNSAFE,
  ARAD_FC_REC_SCH_OOB_SET_UNSAFE,
  ARAD_FC_REC_SCH_OOB_SET_VERIFY,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_SET,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_SET_PRINT,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_SET_UNSAFE,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_SET_VERIFY,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_GET,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_GET_PRINT,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_GET_VERIFY,
  ARAD_FC_NIF_OVERSUBSCR_SCHEME_GET_UNSAFE,
  ARAD_FC_NIF_PAUSE_QUANTA_SET,
  ARAD_FC_NIF_PAUSE_QUANTA_SET_PRINT,
  ARAD_FC_NIF_PAUSE_QUANTA_SET_UNSAFE,
  ARAD_FC_NIF_PAUSE_QUANTA_SET_VERIFY,
  ARAD_FC_NIF_PAUSE_QUANTA_GET,
  ARAD_FC_NIF_PAUSE_QUANTA_GET_PRINT,
  ARAD_FC_NIF_PAUSE_QUANTA_GET_VERIFY,
  ARAD_FC_NIF_PAUSE_QUANTA_GET_UNSAFE,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_PRINT,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_UNSAFE,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_VERIFY,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_PRINT,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_VERIFY,
  ARAD_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_UNSAFE,
  ARAD_FC_EXTEND_Q_GRP_SET,
  ARAD_FC_EXTEND_Q_GRP_SET_PRINT,
  ARAD_FC_EXTEND_Q_GRP_SET_UNSAFE,
  ARAD_FC_EXTEND_Q_GRP_SET_VERIFY,
  ARAD_FC_EXTEND_Q_GRP_GET,
  ARAD_FC_EXTEND_Q_GRP_GET_PRINT,
  ARAD_FC_EXTEND_Q_GRP_GET_VERIFY,
  ARAD_FC_EXTEND_Q_GRP_GET_UNSAFE,
  ARAD_FC_CLEAR_CALENDAR_UNSAFE,
  ARAD_FC_HCFC_BITMAP_SET_UNSAFE,
  ARAD_FC_HCFC_BITMAP_GET_UNSAFE,
  ARAD_FC_INIT_PFC_MAPPING_UNSAFE,
  ARAD_FC_INIT_PFC_MAPPING,
  ARAD_FC_ILKN_RETRANSMIT_CAL_SET_VERIFY,
  ARAD_FC_ILKN_RETRANSMIT_CAL_SET_UNSAFE,
  ARAD_FC_ILKN_RETRANSMIT_CAL_GET_UNSAFE,
  ARAD_FC_ILKN_RETRANSMIT_CAL_SET,
  ARAD_FC_ILKN_RETRANSMIT_CAL_GET,
  ARAD_CNM_CP_PACKET_SET,
  ARAD_CNM_CP_PACKET_GET,
  ARAD_CNM_CP_PACKET_SET_UNSAFE,
  ARAD_CNM_CP_PACKET_GET_UNSAFE,
  ARAD_CNM_CP_PROFILE_GET,
  ARAD_CNM_CP_PROFILE_SET,
  ARAD_CNM_CP_PROFILE_GET_UNSAFE,
  ARAD_CNM_CP_PROFILE_SET_UNSAFE,
  ARAD_CNM_CP_PROFILE_VERIFY,
  ARAD_CNM_CPQ_GET,
  ARAD_CNM_CPQ_SET,
  ARAD_CNM_CPQ_GET_UNSAFE,
  ARAD_CNM_CPQ_SET_UNSAFE,
  ARAD_CNM_CPQ_VERIFY,
  ARAD_CNM_QUEUE_MAPPING_GET,
  ARAD_CNM_QUEUE_MAPPING_SET,
  ARAD_CNM_QUEUE_MAPPING_GET_UNSAFE,
  ARAD_CNM_QUEUE_MAPPING_SET_UNSAFE,
  ARAD_CNM_QUEUE_MAPPING_SET_VERIFY,
  ARAD_CNM_CP_SET,
  ARAD_CNM_CP_GET,
  ARAD_CNM_CP_SET_VERIFY,
  ARAD_CNM_CP_SET_UNSAFE,
  ARAD_CNM_CP_GET_UNSAFE,  
  ARAD_IQM_CNM_PROFILE_TBL_SET_UNSAFE,
  ARAD_IQM_CNM_PROFILE_TBL_GET_UNSAFE,
  ARAD_IQM_CNM_DS_TBL_SET_UNSAFE,
  ARAD_IQM_CNM_DS_TBL_GET_UNSAFE,
  ARAD_NIF_COUNTER_GET_VERIFY,
  ARAD_NIF_ALL_COUNTERS_GET_VERIFY,
  ARAD_VERIFY_MULT_IN_INGRESS_AND_NOT_SINGLE_COPY,
  ARAD_MULT_PROPERTIES_SET_UNSAFE,
  ARAD_MULT_PROPERTIES_GET_UNSAFE,
    ARAD_EGR_PROG_EDITOR_UNSAFE,
  ARAD_EGR_Q_PRIO_SET,
  ARAD_EGR_Q_PRIO_VERIFY,
  ARAD_EGR_OFP_SCHEDULING_WFQ_SET_UNSAFE,
  ARAD_EGR_OFP_SCHEDULING_WFQ_GET_UNSAFE,
  ARAD_EGR_Q_PRIO_SET_UNSAFE,
  ARAD_EGR_Q_PRIO_SET_VERIFY,
  ARAD_EGR_Q_PRIO_GET,
  ARAD_EGR_Q_PRIO_GET_VERIFY,
  ARAD_EGR_Q_PRIO_GET_UNSAFE,
  ARAD_EGR_Q_PRIO_MAP_ENTRY_GET,
  ARAD_EGR_Q_PROFILE_MAP_SET,
  ARAD_EGR_Q_PROFILE_MAP_SET_PRINT,
  ARAD_EGR_Q_PROFILE_MAP_SET_UNSAFE,
  ARAD_EGR_Q_PROFILE_MAP_SET_VERIFY,
  ARAD_EGR_Q_PROFILE_MAP_GET,
  ARAD_EGR_Q_PROFILE_MAP_GET_PRINT,
  ARAD_EGR_Q_PROFILE_MAP_GET_VERIFY,
  ARAD_EGR_Q_PROFILE_MAP_GET_UNSAFE,
  ARAD_EGR_OTM_PORT_CUD_EXTENSION_ID_VERIFY,
  ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_GET_UNSAFE,
  ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_SET_VERIFY,
  ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_GET_VERIFY,
  ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_SET_UNSAFE,
  ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_SET,
  ARAD_EGR_DSP_PP_TO_BASE_Q_PAIR_GET,
  ARAD_EGR_DSP_PP_PRIORITIES_MODE_SET,
  ARAD_EGR_DSP_PP_PRIORITIES_MODE_GET,
  ARAD_EGR_DSP_PP_PRIORITIES_MODE_SET_UNSAFE,
  ARAD_EGR_DSP_PP_PRIORITIES_MODE_SET_VERIFY,
  ARAD_EGR_DSP_PP_PRIORITIES_MODE_GET_UNSAFE,
  ARAD_EGR_DSP_PP_PRIORITIES_MODE_GET_VERIFY,  
  ARAD_EGR_DSP_PP_SHAPER_MODE_GET_UNSAFE,
  ARAD_EGR_DSP_PP_SHAPER_MODE_GET_VERIFY,
  ARAD_EGR_DSP_PP_SHAPER_MODE_SET_UNSAFE,
  ARAD_EGR_DSP_PP_SHAPER_MODE_SET_VERIFY,
  ARAD_EGR_QUEUING_PARTITION_SCHEME_SET,
  ARAD_EGR_QUEUING_PARTITION_SCHEME_SET_UNSAFE,
  ARAD_EGR_QUEUING_DEV_SET,
  ARAD_EGR_QUEUING_DEV_GET,
  ARAD_EGR_QUEUING_GLOBAL_DROP_SET,
  ARAD_EGR_QUEUING_GLOBAL_DROP_SET_UNSAFE,
  ARAD_EGR_QUEUING_GLOBAL_DROP_GET,
  ARAD_EGR_QUEUING_GLOBAL_DROP_GET_UNSAFE,
  ARAD_EGR_QUEUING_SP_TC_DROP_SET,
  ARAD_EGR_QUEUING_SP_TC_DROP_SET_UNSAFE,
  ARAD_EGR_QUEUING_SP_TC_DROP_GET,
  ARAD_EGR_QUEUING_SP_TC_DROP_GET_UNSAFE,
  ARAD_EGR_QUEUING_SP_RESERVED_SET,
  ARAD_EGR_QUEUING_SP_RESERVED_SET_UNSAFE,
  ARAD_EGR_QUEUING_SCH_UNSCH_DROP_SET,
  ARAD_EGR_QUEUING_SCH_UNSCH_DROP_SET_UNSAFE,
  ARAD_EGR_QUEUING_SCHED_PORT_FC_THRESH_SET,
  ARAD_EGR_QUEUING_SCHED_PORT_FC_THRESH_SET_UNSAFE,
  ARAD_EGR_QUEUING_SCHED_Q_FC_THRESH_SET,
  ARAD_EGR_QUEUING_GLOBAL_FC_SET,
  ARAD_EGR_QUEUING_GLOBAL_FC_SET_UNSAFE,
  ARAD_EGR_QUEUING_GLOBAL_FC_GET,
  ARAD_EGR_QUEUING_GLOBAL_FC_GET_UNSAFE,
  ARAD_EGR_QUEUING_MC_TC_FC_SET,
  ARAD_EGR_QUEUING_MC_TC_FC_SET_UNSAFE,
  ARAD_EGR_QUEUING_MC_TC_FC_GET,
  ARAD_EGR_QUEUING_MC_TC_FC_GET_UNSAFE,
  ARAD_EGR_QUEUING_MC_DROP_SET,
  ARAD_EGR_QUEUING_DEV_SET_UNSAFE,
  ARAD_EGR_QUEUING_DEV_GET_UNSAFE,
  ARAD_EGR_QUEUING_MC_COS_MAP_SET,
  ARAD_EGR_QUEUING_MC_COS_MAP_GET,
  ARAD_EGR_QUEUING_MC_COS_MAP_SET_UNSAFE,
  ARAD_EGR_QUEUING_MC_COS_MAP_GET_UNSAFE,
  ARAD_EGR_QUEUING_IF_FC_SET,
  ARAD_EGR_QUEUING_IF_FC_GET,
  ARAD_EGR_QUEUING_IF_FC_SET_UNSAFE,
  ARAD_EGR_QUEUING_IF_FC_GET_UNSAFE,
  ARAD_EGR_QUEUING_IF_FC_UC_SET,
  ARAD_EGR_QUEUING_IF_FC_UC_GET,
  ARAD_EGR_QUEUING_IF_FC_UC_SET_UNSAFE,
  ARAD_EGR_QUEUING_IF_FC_UC_GET_UNSAFE,
  ARAD_EGR_QUEUING_IF_FC_MC_SET,
  ARAD_EGR_QUEUING_IF_FC_MC_GET,
  ARAD_EGR_QUEUING_IF_FC_MC_SET_UNSAFE,
  ARAD_EGR_QUEUING_IF_FC_MC_GET_UNSAFE,
  ARAD_EGR_QUEUING_IF_UC_MAP_SET,
  ARAD_EGR_QUEUING_IF_UC_MAP_SET_UNSAFE,
  ARAD_EGR_QUEUING_IF_MC_MAP_SET,
  ARAD_EGR_QUEUING_IF_MC_MAP_SET_UNSAFE,
  ARAD_EGR_QUEUING_OFP_TCG_SET,
  ARAD_EGR_QUEUING_OFP_TCG_GET,
  ARAD_EGR_QUEUING_OFP_TCG_SET_UNSAFE,
  ARAD_EGR_QUEUING_OFP_TCG_GET_UNSAFE,
  ARAD_EGR_QUEUING_OFP_TCG_SET_VERIFY,
  ARAD_EGR_QUEUING_OFP_TCG_GET_VERIFY,
  ARAD_EGR_QUEUING_TCG_WEIGHT_SET,
  ARAD_EGR_QUEUING_TCG_WEIGHT_GET,
  ARAD_EGR_QUEUING_TCG_WEIGHT_SET_UNSAFE,
  ARAD_EGR_QUEUING_TCG_WEIGHT_GET_UNSAFE,
  ARAD_EGR_QUEUING_TCG_WEIGHT_SET_VERIFY_UNSAFE,
  ARAD_EGR_QUEUING_TCG_WEIGHT_GET_VERIFY_UNSAFE,
    ARAD_EGR_QUEUING_Q_PAIR_PORT_TC_FIND,
  ARAD_MGMT_HW_ADJUST_NIF,
  ARAD_NIF_WC_XAUI_SET,
  ARAD_NIF_ID_VERIFY,
  ARAD_NIF_TYPE_VERIFY,
  ARAD_NIF_WC_RXAUI_SET,
  ARAD_NIF_WC_TYPE_GET,
  ARAD_MGMT_IHB_TBLS_INIT,
  ARAD_MGMT_EPNI_TBLS_INIT,
  ARAD_DIAG_SAMPLE_ENABLE_SET,
  ARAD_DIAG_SAMPLE_ENABLE_GET,
  ARAD_DIAG_SAMPLE_ENABLE_SET_UNSAFE,
  ARAD_DIAG_SAMPLE_ENABLE_GET_UNSAFE,
  ARAD_DIAG_SIGNALS_DUMP,
  ARAD_DIAG_DBG_VAL_GET_UNSAFE,
  ARAD_DIAG_SIGNALS_DUMP_UNSAFE,
  ARAD_PMF_LOW_LEVEL_INIT_UNSAFE,
  ARAD_PMF_LOW_LEVEL_TM_INIT_SET_UNSAFE,
  ARAD_PMF_LOW_LEVEL_STACK_INIT_SET_UNSAFE,
  ARAD_PMF_LOW_LEVEL_STACK_PGM_SET_UNSAFE,
  ARAD_PMF_LOW_LEVEL_RAW_PGM_SET_UNSAFE,
  ARAD_PMF_LOW_LEVEL_ETH_PGM_SET_UNSAFE,
  ARAD_PMF_LOW_LEVEL_TM_PGM_SET_UNSAFE,
  ARAD_PMF_CE_ENTRY_VERIFY,
  ARAD_PMF_CE_INSTRUCTION_FLD_GET,
  ARAD_PMF_CE_PACKET_HEADER_ENTRY_SET,
  ARAD_PMF_CE_PACKET_HEADER_ENTRY_SET_PRINT,
  ARAD_PMF_CE_PACKET_HEADER_ENTRY_SET_UNSAFE,
    ARAD_PMF_CE_INTERNAL_FIELD_INFO_FIND,
    ARAD_PMF_CE_INTERNAL_FIELD_TABLE_SIZE_FIND,
    ARAD_PMF_CE_INTERNAL_FIELD_TABLE_LINE_FIND,
    ARAD_PMF_CE_HEADER_INFO_FIND,
    ARAD_PMF_CE_INTERNAL_FIELD_OFFSET_COMPUTE,
    ARAD_PMF_CE_INTERNAL_FIELD_OFFSET_QUAL_FIND,
  ARAD_PMF_CE_PACKET_HEADER_ENTRY_GET,
  ARAD_PMF_CE_PACKET_HEADER_ENTRY_GET_PRINT,
  ARAD_PMF_CE_PACKET_HEADER_ENTRY_GET_UNSAFE,
  ARAD_PMF_CE_IRPP_INFO_ENTRY_SET,
  ARAD_PMF_CE_IRPP_INFO_ENTRY_SET_PRINT,
  ARAD_PMF_CE_IRPP_INFO_ENTRY_SET_UNSAFE,
  ARAD_PMF_CE_IRPP_INFO_ENTRY_GET,
  ARAD_PMF_CE_IRPP_INFO_ENTRY_GET_PRINT,
  ARAD_PMF_CE_IRPP_INFO_ENTRY_GET_UNSAFE,
  ARAD_PMF_CE_NOP_ENTRY_SET,
  ARAD_PMF_CE_NOP_ENTRY_SET_PRINT,
  ARAD_PMF_CE_NOP_ENTRY_SET_UNSAFE,
  ARAD_PMF_CE_NOP_ENTRY_GET,
  ARAD_PMF_CE_NOP_ENTRY_GET_PRINT,
  ARAD_PMF_CE_NOP_ENTRY_GET_UNSAFE,
  ARAD_PMF_TCAM_LOOKUP_SET,
  ARAD_PMF_TCAM_LOOKUP_SET_PRINT,
  ARAD_PMF_TCAM_LOOKUP_SET_UNSAFE,
  ARAD_PMF_TCAM_LOOKUP_SET_VERIFY,
  ARAD_PMF_TCAM_LOOKUP_GET,
  ARAD_PMF_TCAM_LOOKUP_GET_PRINT,
  ARAD_PMF_TCAM_LOOKUP_GET_VERIFY,
  ARAD_PMF_TCAM_LOOKUP_GET_UNSAFE,
  ARAD_PMF_TCAM_ENTRY_ADD,
  ARAD_PMF_TCAM_ENTRY_ADD_PRINT,
  ARAD_PMF_TCAM_ENTRY_ADD_UNSAFE,
  ARAD_PMF_TCAM_ENTRY_ADD_VERIFY,
  ARAD_PMF_TCAM_ENTRY_LOCATION_GET,
  ARAD_PMF_TCAM_ENTRY_GET,
  ARAD_PMF_TCAM_ENTRY_GET_PRINT,
  ARAD_PMF_TCAM_ENTRY_GET_UNSAFE,
  ARAD_PMF_TCAM_ENTRY_GET_VERIFY,
  ARAD_PMF_TCAM_ENTRY_REMOVE,
  ARAD_PMF_TCAM_ENTRY_REMOVE_PRINT,
  ARAD_PMF_TCAM_ENTRY_REMOVE_UNSAFE,
  ARAD_PMF_TCAM_ENTRY_REMOVE_VERIFY,
  ARAD_PMF_TCAM_KEY_CLEAR,
  ARAD_PMF_TCAM_KEY_CLEAR_PRINT,
  ARAD_TCAM_KEY_CLEAR_UNSAFE,
  ARAD_TCAM_KEY_CLEAR_VERIFY,
  ARAD_PMF_TCAM_KEY_VAL_SET,
  ARAD_PMF_TCAM_KEY_VAL_SET_PRINT,
  ARAD_TCAM_KEY_VAL_SET_UNSAFE,
  ARAD_TCAM_KEY_VAL_SET_VERIFY,
  ARAD_PMF_TCAM_KEY_VAL_GET,
  ARAD_PMF_TCAM_KEY_VAL_GET_PRINT,
  ARAD_TCAM_KEY_VAL_GET_VERIFY,
  ARAD_TCAM_KEY_VAL_GET_UNSAFE,
  ARAD_TCAM_KEY_MASKED_VAL_SET,
  ARAD_TCAM_KEY_MASKED_VAL_SET_PRINT,
  ARAD_TCAM_KEY_MASKED_VAL_SET_UNSAFE,
  ARAD_TCAM_KEY_MASKED_VAL_SET_VERIFY,
  ARAD_TCAM_KEY_MASKED_VAL_GET,
  ARAD_TCAM_KEY_MASKED_VAL_GET_PRINT,
  ARAD_TCAM_KEY_MASKED_VAL_GET_VERIFY,
  ARAD_TCAM_KEY_MASKED_VAL_GET_UNSAFE,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_SET,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PRINT,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_SET_UNSAFE,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_SET_VERIFY,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_GET,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_GET_PRINT,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_GET_VERIFY,
  ARAD_PMF_DB_DIRECT_TBL_KEY_SRC_GET_UNSAFE,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_SET,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_SET_PRINT,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_SET_UNSAFE,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_SET_VERIFY,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_GET,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_GET_PRINT,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_GET_VERIFY,
  ARAD_PMF_DB_DIRECT_TBL_ENTRY_GET_UNSAFE,
  ARAD_PMF_DB_FEM_INPUT_SET,
	ARAD_PMF_DB_FES_SET_UNSAFE,
	ARAD_PMF_DB_FES_SET_VERIFY,
	ARAD_PMF_DB_FES_GET_UNSAFE,
	ARAD_PMF_DB_FES_GET_VERIFY,
    ARAD_PMF_DB_FES_MOVE_UNSAFE,
  ARAD_PMF_DB_FEM_INPUT_SET_PRINT,
  ARAD_PMF_DB_FEM_INPUT_SET_UNSAFE,
  ARAD_PMF_DB_FEM_INPUT_SET_VERIFY,
  ARAD_PMF_DB_FEM_INPUT_GET,
  ARAD_PMF_DB_FEM_INPUT_GET_PRINT,
  ARAD_PMF_DB_FEM_INPUT_GET_VERIFY,
  ARAD_PMF_DB_FEM_INPUT_GET_UNSAFE,
  ARAD_PMF_DB_TAG_SELECT_SET,
  ARAD_PMF_DB_TAG_SELECT_SET_PRINT,
  ARAD_PMF_DB_TAG_SELECT_SET_UNSAFE,
  ARAD_PMF_DB_TAG_SELECT_SET_VERIFY,
  ARAD_PMF_DB_TAG_SELECT_GET,
  ARAD_PMF_DB_TAG_SELECT_GET_PRINT,
  ARAD_PMF_DB_TAG_SELECT_GET_VERIFY,
  ARAD_PMF_DB_TAG_SELECT_GET_UNSAFE,
  ARAD_PMF_FEM_SELECT_BITS_SET,
  ARAD_PMF_FEM_SELECT_BITS_SET_PRINT,
  ARAD_PMF_FEM_SELECT_BITS_SET_UNSAFE,
  ARAD_PMF_FEM_SELECT_BITS_SET_VERIFY,
  ARAD_PMF_FEM_SELECT_BITS_GET,
  ARAD_PMF_FEM_SELECT_BITS_GET_PRINT,
  ARAD_PMF_FEM_SELECT_BITS_GET_VERIFY,
  ARAD_PMF_FEM_SELECT_BITS_GET_UNSAFE,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_SET,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_SET_PRINT,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_SET_UNSAFE,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_SET_VERIFY,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_GET,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_GET_PRINT,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_GET_VERIFY,
  ARAD_PMF_FEM_ACTION_FORMAT_MAP_GET_UNSAFE,
  ARAD_PMF_FEM_ACTION_FORMAT_SET,
  ARAD_PMF_FEM_ACTION_FORMAT_SET_PRINT,
  ARAD_PMF_FEM_ACTION_FORMAT_SET_UNSAFE,
  ARAD_PMF_FEM_ACTION_FORMAT_SET_VERIFY,
  ARAD_PMF_FEM_ACTION_FORMAT_GET,
  ARAD_PMF_FEM_ACTION_FORMAT_GET_PRINT,
  ARAD_PMF_FEM_ACTION_FORMAT_GET_VERIFY,
  ARAD_PMF_FEM_ACTION_FORMAT_GET_UNSAFE,
  ARAD_PMF_PGM_SELECTION_ENTRY_SET,
  ARAD_PMF_PGM_SELECTION_ENTRY_SET_PRINT,
  ARAD_PMF_PGM_SELECTION_ENTRY_SET_UNSAFE,
  ARAD_PMF_PGM_SELECTION_ENTRY_SET_VERIFY,
  ARAD_PMF_PGM_SELECTION_ENTRY_GET,
  ARAD_PMF_PGM_SELECTION_ENTRY_GET_PRINT,
  ARAD_PMF_PGM_SELECTION_ENTRY_GET_VERIFY,
  ARAD_PMF_PGM_SELECTION_ENTRY_GET_UNSAFE,
  ARAD_PMF_PGM_SET,
  ARAD_PMF_PGM_SET_PRINT,
  ARAD_PMF_PGM_SET_UNSAFE,
  ARAD_PMF_PGM_SET_VERIFY,
  ARAD_PMF_PGM_GET,
  ARAD_PMF_PGM_GET_PRINT,
  ARAD_PMF_PGM_GET_VERIFY,
  ARAD_PMF_PGM_GET_UNSAFE,
  ARAD_PMF_LOW_LEVEL_GET_PROCS_PTR,
  ARAD_PMF_LOW_LEVEL_GET_ERRS_PTR,
  ARAD_PMF_FEM_OUTPUT_SIZE_GET,
  ARAD_PMF_PGM_SELECTION_ENTRY_BUILD,
  ARAD_PMF_LOW_LEVEL_PGM_SEL_TBL_NOF_ENTRIES_GET,
  ARAD_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_TO_HW_ADD,
  ARAD_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_SET,
  ARAD_PMF_LOW_LEVEL_PGM_HEADER_PROFILE_GET,
  ARAD_PMF_LOW_LEVEL_PGM_PORT_PROFILE_TO_HW_ADD,
  ARAD_PMF_PGM_MGMT_PROFILE_GET,
  ARAD_PMF_LOW_LEVEL_PGM_PROFILE_TO_HW_ADD,
  ARAD_PMF_LOW_LEVEL_PARSER_PMF_PROFILE_GET,
  ARAD_PMF_LOW_LEVEL_PGM_PORT_PROFILE_ENCODE,
  ARAD_PMF_LOW_LEVEL_PGM_PORT_PROFILE_DECODE,
  ARAD_PMF_LOW_LEVEL_PGM_PORT_PROFILE_GET,
  ARAD_PMF_LOW_LEVEL_PGM_HEADER_TYPE_GET,
  ARAD_PMF_LOW_LEVEL_PGM_COUNTER_INIT_UNSAFE,
  ARAD_PMF_LOW_LEVEL_PGM_INIT_UNSAFE,
  ARAD_PMF_DB_TAG_FLD_SELECT,
  ARAD_TCAM_KEY_FIELD_LSB_SIZE_GET,
  ARAD_PMF_TCAM_KEY_A_B_FIELD_LSB_SIZE_GET,
  ARAD_TCAM_KEY_SIZE_GET,
  ARAD_PMF_CE_IRPP_FIELD_SIZE_AND_VAL_GET,
  ARAD_PMF_DIAG_FORCE_PROG_SET,
  ARAD_PMF_DIAG_FORCE_PROG_SET_PRINT,
  ARAD_PMF_DIAG_FORCE_PROG_SET_UNSAFE,
  ARAD_PMF_DIAG_FORCE_PROG_SET_VERIFY,
  ARAD_PMF_DIAG_FORCE_PROG_GET,
  ARAD_PMF_DIAG_FORCE_PROG_GET_PRINT,
  ARAD_PMF_DIAG_FORCE_PROG_GET_VERIFY,
  ARAD_PMF_DIAG_FORCE_PROG_GET_UNSAFE,
  ARAD_PMF_DIAG_SELECTED_PROGS_GET,
  ARAD_PMF_DIAG_SELECTED_PROGS_GET_PRINT,
  ARAD_PMF_DIAG_SELECTED_PROGS_GET_UNSAFE,
  ARAD_PMF_DIAG_SELECTED_PROGS_GET_VERIFY,
  ARAD_PMF_DIAG_FORCE_ACTION_SET,
  ARAD_PMF_DIAG_FORCE_ACTION_SET_PRINT,
  ARAD_PMF_DIAG_FORCE_ACTION_SET_UNSAFE,
  ARAD_PMF_DIAG_FORCE_ACTION_SET_VERIFY,
  ARAD_PMF_DIAG_FORCE_ACTION_GET,
  ARAD_PMF_DIAG_FORCE_ACTION_GET_PRINT,
  ARAD_PMF_DIAG_FORCE_ACTION_GET_VERIFY,
  ARAD_PMF_DIAG_FORCE_ACTION_GET_UNSAFE,
  ARAD_PMF_DIAG_FORCE_ACTION_UPDATE,
  ARAD_PMF_DIAG_FORCE_ACTION_UPDATE_PRINT,
  ARAD_PMF_DIAG_FORCE_ACTION_UPDATE_UNSAFE,
  ARAD_PMF_DIAG_FORCE_ACTION_UPDATE_VERIFY,
  ARAD_PMF_DIAG_BUILT_KEYS_GET,
  ARAD_PMF_DIAG_BUILT_KEYS_GET_PRINT,
  ARAD_PMF_DIAG_BUILT_KEYS_GET_UNSAFE,
  ARAD_PMF_DIAG_BUILT_KEYS_GET_VERIFY,
  ARAD_PMF_DIAG_FEM_FREEZE_SET,
  ARAD_PMF_DIAG_FEM_FREEZE_SET_PRINT,
  ARAD_PMF_DIAG_FEM_FREEZE_SET_UNSAFE,
  ARAD_PMF_DIAG_FEM_FREEZE_SET_VERIFY,
  ARAD_PMF_DIAG_FEM_FREEZE_GET,
  ARAD_PMF_DIAG_FEM_FREEZE_GET_PRINT,
  ARAD_PMF_DIAG_FEM_FREEZE_GET_VERIFY,
  ARAD_PMF_DIAG_FEM_FREEZE_GET_UNSAFE,
  ARAD_PMF_DIAG_FEMS_INFO_GET,
  ARAD_PMF_DIAG_FEMS_INFO_GET_PRINT,
  ARAD_PMF_DIAG_FEMS_INFO_GET_UNSAFE,
  ARAD_PMF_DIAG_FEMS_INFO_GET_VERIFY,
  ARAD_PP_IHP_PINFO_LLR_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PINFO_LLR_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LLR_LLVP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LLR_LLVP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_GET_UNSAFE,
  ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PORT_PROTOCOL_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PORT_PROTOCOL_TBL_SET_UNSAFE,
  ARAD_PP_IHP_TOS_2_COS_TBL_GET_UNSAFE,
  ARAD_PP_IHP_TOS_2_COS_TBL_SET_UNSAFE,
  ARAD_PP_IHP_RESERVED_MC_TBL_GET_UNSAFE,
  ARAD_PP_IHP_RESERVED_MC_TBL_SET_UNSAFE,
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_GET_UNSAFE,
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_SET_UNSAFE,
  ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_GET_UNSAFE,
  ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PP_PORT_INFO_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PP_PORT_INFO_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_PROGRAM_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_SET_UNSAFE,
  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_GET_UNSAFE,
  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_SET_UNSAFE,
  ARAD_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_GET_UNSAFE,
  ARAD_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_SET_UNSAFE,
  ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_GET_UNSAFE,
  ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_SET_UNSAFE,
  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_GET_UNSAFE,
  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_SET_UNSAFE,
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_UNSAFE,
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_UNSAFE,
  ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_GET_UNSAFE,
  ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_SET_UNSAFE,
  ARAD_PP_IHP_BVD_FID_CLASS_TBL_GET_UNSAFE,
  ARAD_PP_IHP_BVD_FID_CLASS_TBL_SET_UNSAFE,
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_GET_UNSAFE,
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_SET_UNSAFE,
  ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_STP_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_STP_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT_LLVP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT_LLVP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_ISEM_1ST_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
  ARAD_PP_IHP_ISEM_1ST_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,
  ARAD_PP_IHP_ISEM_2ND_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
  ARAD_PP_IHP_ISEM_2ND_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_SET_UNSAFE,
  ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_SET_UNSAFE,
  ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_IHB_PINFO_FER_TBL_GET_UNSAFE,
  ARAD_PP_IHB_PINFO_FER_TBL_SET_UNSAFE,
  ARAD_PP_IHB_LB_PFC_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHB_LB_PFC_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_GET_UNSAFE,
  ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FEC_ECMP_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FEC_ECMP_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_SET_UNSAFE,

  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_SET_UNSAFE,
  ARAD_PP_IHB_PATH_SELECT_TBL_GET_UNSAFE,
  ARAD_PP_IHB_PATH_SELECT_TBL_SET_UNSAFE,
  ARAD_PP_IHB_DESTINATION_STATUS_TBL_GET_UNSAFE,
  ARAD_PP_IHB_DESTINATION_STATUS_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHB_SNOOP_ACTION_TBL_GET_UNSAFE,
  ARAD_PP_IHB_SNOOP_ACTION_TBL_SET_UNSAFE,
  ARAD_PP_IHB_PINFO_FLP_TBL_GET_UNSAFE,
  ARAD_PP_IHB_PINFO_FLP_TBL_SET_UNSAFE,
  ARAD_PP_IHB_VRF_CONFIG_TBL_GET_UNSAFE,
  ARAD_PP_IHB_VRF_CONFIG_TBL_SET_UNSAFE,
  ARAD_PP_IHB_HEADER_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHB_HEADER_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_GET_UNSAFE,
  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_SET_UNSAFE,
  ARAD_PP_IHB_LPM_TBL_GET_UNSAFE,
  ARAD_PP_IHB_LPM_TBL_SET_UNSAFE,
  ARAD_PP_IHB_TCAM_ACTION_TBL_GET_UNSAFE,
  ARAD_PP_IHB_TCAM_ACTION_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_PP_PPCT_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_PP_PPCT_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_TTL_SCOPE_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_TTL_SCOPE_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_AUX_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_AUX_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PCP_DEI_MAP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PCP_DEI_MAP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_TX_TAG_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_TX_TAG_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_STP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_STP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PCP_DEI_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PCP_DEI_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PP_PCT_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PP_PCT_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_LLVP_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_LLVP_TABLE_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_EXP_REMARK_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_EXP_REMARK_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_DSCP_REMARK_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_DSCP_REMARK_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PRGE_PROGRAM_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PRGE_PROGRAM_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PRGE_DATA_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_PRGE_DATA_TBL_SET_UNSAFE,
  ARAD_PP_EGQ_DSP_PTR_MAP_TBL_GET_UNSAFE,
  ARAD_PP_EGQ_DSP_PTR_MAP_TBL_SET_UNSAFE,
  ARAD_PP_FRWRD_IP_TCAM_INIT_UNSAFE,
  ARAD_PORTS_SET_IN_SRC_SYS_PORT_SET_UNSAFE,
  ARAD_PORTS_SET_IN_SRC_SYS_PORT_GET_UNSAFE,
  ARAD_PORTS_FAP_PORT_ID_CUD_EXTENSION_VERIFY,
  ARAD_PORTS_INIT_INTERFACES_CONTEXT_MAP,
  ARAD_PORT_TO_INTERFACE_MAP_SYMMETRIC_GET_UNSAFE,
  ARAD_PARSER_INIT,
  ARAD_PARSER_INGRESS_SHAPE_STATE_SET,
  ARAD_PORT_PP_PORT_SET_UNSAFE,
  ARAD_PORT_PP_PORT_SET_VERIFY,
  ARAD_PORT_PP_PORT_GET,
  ARAD_PORT_PP_PORT_GET_UNSAFE,
  ARAD_PORT_PP_PORT_GET_VERIFY,
  ARAD_PORT_TM_PROFILE_REMOVE,
  ARAD_PORT_TM_PROFILE_REMOVE_UNSAFE,
  ARAD_PORT_TM_PROFILE_REMOVE_VERIFY,
  ARAD_PORT_TO_PP_PORT_MAP_SET,
  ARAD_PORT_TO_PP_PORT_MAP_SET_UNSAFE,
  ARAD_PORT_TO_PP_PORT_MAP_SET_VERIFY,
  ARAD_PORT_TO_PP_PORT_MAP_GET,
  ARAD_PORT_TO_PP_PORT_MAP_GET_VERIFY,
  ARAD_PORT_TO_PP_PORT_MAP_GET_UNSAFE,
  ARAD_A_LOCAL_TO_SYS_PHYS_PORT_MAP_GET_UNSAFE,
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_GET_UNSAFE,
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_SET_UNSAFE,
  ARAD_PORT_INGR_MAP_WRITE_VAL,
  ARAD_PORT2IF_INGR_MAP_NEW_UNMAP_OLD,
  ARAD_PORT_EGR_MAP_WRITE_VAL,
  ARAD_PORT2IF_EGR_MAP_NEW_UNMAP_OLD,
  ARAD_PORT_ADD_EGR_MAP_WRITE_VAL,
  ARAD_PORTS_SWAP_SET_VERIFY,
  ARAD_PORTS_SWAP_SET_UNSAFE,
  ARAD_PORTS_SWAP_GET_VERIFY,
  ARAD_PORTS_SWAP_GET_UNSAFE,
  ARAD_PORTS_SWAP_SET,
  ARAD_PORTS_SWAP_GET,
  ARAD_SWAP_INFO_SET_VERIFY,
  ARAD_SWAP_INFO_SET_UNSAFE,
  ARAD_SWAP_INFO_GET_VERIFY,
  ARAD_SWAP_INFO_GET_UNSAFE,
  ARAD_MGMT_SWAP_INIT,
  ARAD_NIF_ON_OFF_SET_VERIFY,
  ARAD_NIF_ON_OFF_GET_VERIFY,
  ARAD_NIF_LOOPBACK_SET_PRINT,
  ARAD_NIF_LOOPBACK_SET_VERIFY,
  ARAD_NIF_LOOPBACK_GET_PRINT,
  ARAD_NIF_LOOPBACK_GET_VERIFY,
  ARAD_SCH_PORT_TC2SE_ID,
  ARAD_SCH_SE2PORT_TC_ID,
  ARAD_SCH_PORT_TCG_WEIGHT_SET,
  ARAD_SCH_PORT_TCG_WEIGHT_GET,
  ARAD_SCH_PORT_TCG_WEIGHT_SET_UNSAFE,
  ARAD_SCH_PORT_TCG_WEIGHT_GET_UNSAFE,
  ARAD_SCH_PORT_TCG_WEIGHT_SET_VERIFY_UNSAFE,
  ARAD_SCH_PORT_TCG_WEIGHT_GET_VERIFY_UNSAFE,
  ARAD_NIF_ILKN_CLK_CONFIG,
  ARAD_CNT_GET_INGRESS_MODE,
  ARAD_CNT_CHECK_VOQ_PARAMS,
  ARAD_CNT_CHECK_VOQ_SET_SIZE,
  ARAD_CNT_GET_EGRESS_MODE,
  ARAD_CNT_GET_PROCESSOR_ID,
  ARAD_CNT_GET_SOURCE_ID,
  ARAD_CNT_INIT,
  ARAD_INIT_DRAM_PLL_SET,
  ARAD_SW_DB_MULTISET_ADD,
  ARAD_SW_DB_MULTISET_REMOVE,
  ARAD_SW_DB_MULTISET_LOOKUP,
  ARAD_SW_DB_MULTISET_ADD_BY_INDEX,
  ARAD_SW_DB_MULTISET_REMOVE_BY_INDEX,
  ARAD_SW_DB_MULTISET_CLEAR,
  ARAD_TCAM_BANK_INIT_UNSAFE,
  ARAD_TCAM_BANK_INIT_VERIFY,
  ARAD_TCAM_DB_CREATE_UNSAFE,
  ARAD_TCAM_DB_CREATE_VERIFY,
  ARAD_TCAM_DB_DESTROY_UNSAFE,
  ARAD_TCAM_DB_DESTROY_VERIFY,
  ARAD_TCAM_DB_BANK_ADD_UNSAFE,
  ARAD_TCAM_DB_BANK_ADD_VERIFY,
  ARAD_TCAM_DB_BANK_REMOVE_UNSAFE,
  ARAD_TCAM_DB_BANK_REMOVE_VERIFY,
  ARAD_TCAM_DB_NOF_BANKS_GET_UNSAFE,
  ARAD_TCAM_DB_NOF_BANKS_GET_VERIFY,
  ARAD_TCAM_DB_BANK_PREFIX_GET_UNSAFE,
  ARAD_TCAM_DB_BANK_PREFIX_GET_VERIFY,
  ARAD_TCAM_DB_ENTRY_SIZE_GET_UNSAFE,
  ARAD_TCAM_DB_ENTRY_SIZE_GET_VERIFY,
  ARAD_TCAM_DB_DIRECT_TBL_ENTRY_SET_UNSAFE,
  ARAD_TCAM_DB_DIRECT_TBL_ENTRY_SET_VERIFY,
  ARAD_TCAM_DB_ENTRY_ADD_UNSAFE,
  ARAD_TCAM_DB_ENTRY_ADD_VERIFY,
  ARAD_TCAM_DB_ENTRY_GET_UNSAFE,
  ARAD_TCAM_DB_ENTRY_GET_VERIFY,
  ARAD_TCAM_DB_ENTRY_SEARCH_UNSAFE,
  ARAD_TCAM_DB_ENTRY_SEARCH_VERIFY,
  ARAD_TCAM_DB_ENTRY_REMOVE_UNSAFE,
  ARAD_TCAM_DB_ENTRY_REMOVE_VERIFY,
  ARAD_TCAM_DB_IS_BANK_USED_UNSAFE,
  ARAD_TCAM_DB_IS_BANK_USED_VERIFY,
  ARAD_TCAM_ACCESS_PROFILE_CREATE,
  ARAD_TCAM_ACCESS_PROFILE_CREATE_PRINT,
  ARAD_TCAM_ACCESS_PROFILE_CREATE_UNSAFE,
  ARAD_TCAM_ACCESS_PROFILE_CREATE_VERIFY,
  ARAD_TCAM_MANAGED_DB_ENTRY_ADD,
  ARAD_TCAM_MANAGED_DB_ENTRY_ADD_PRINT,
  ARAD_TCAM_MANAGED_DB_ENTRY_ADD_UNSAFE,
  ARAD_TCAM_MANAGED_DB_ENTRY_ADD_VERIFY,
  ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE,
  ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE_PRINT,
  ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE_UNSAFE,
  ARAD_TCAM_MANAGED_DB_ENTRY_REMOVE_VERIFY,
  ARAD_TCAM_MANAGED_DB_ENTRY_GET,
  ARAD_TCAM_MANAGED_DB_ENTRY_GET_PRINT,
  ARAD_TCAM_MANAGED_DB_ENTRY_GET_UNSAFE,
  ARAD_TCAM_MANAGED_DB_ENTRY_GET_VERIFY,
  ARAD_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET,
  ARAD_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_PRINT,
  ARAD_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_UNSAFE,
  ARAD_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_VERIFY,
    ARAD_PMF_PGM_PACKET_FORMAT_CODE_CONVERT,
  ARAD_PMF_PGM_MGMT_SET,
  ARAD_PORT_PARSE_HEADER_TYPE_UNSAFE,
  ARAD_TDM_DIRECT_ROUTING_SET,
  ARAD_TDM_DIRECT_ROUTING_SET_PRINT,
  ARAD_TDM_DIRECT_ROUTING_SET_UNSAFE,
  ARAD_TDM_DIRECT_ROUTING_SET_VERIFY,
  ARAD_TDM_DIRECT_ROUTING_GET,
  ARAD_TDM_DIRECT_ROUTING_GET_PRINT,
  ARAD_TDM_DIRECT_ROUTING_GET_VERIFY,
  ARAD_TDM_DIRECT_ROUTING_GET_UNSAFE,
  ARAD_TDM_PORT_PACKET_CRC_SET,
  ARAD_TDM_PORT_PACKET_CRC_SET_UNSAFE,
  ARAD_TDM_PORT_PACKET_CRC_SET_VERIFY,
  ARAD_TDM_PORT_PACKET_CRC_GET,
  ARAD_TDM_PORT_PACKET_CRC_GET_VERIFY,
  ARAD_TDM_PORT_PACKET_CRC_GET_UNSAFE,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_SET,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_SET_UNSAFE,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_SET_VERIFY,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_GET,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_GET_VERIFY,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_GET_UNSAFE,
  ARAD_EGR_SCHED_DROP_Q_PAIR_THRESH_SET_UNSAFE,
  ARAD_EGR_SCHED_DROP_Q_PAIR_THRESH_GET_UNSAFE,
  ARAD_EGR_UNSCHED_DROP_Q_PAIR_THRESH_SET_UNSAFE,
  ARAD_EGR_UNSCHED_DROP_Q_PAIR_THRESH_GET_UNSAFE,
  ARAD_EGR_OFP_FC_Q_PAIR_THRESH_GET_UNSAFE,
  ARAD_EGR_OFP_FC_Q_PAIR_THRESH_SET_UNSAFE,
  ARAD_SOC_MEM_FIELD32_VALIDATE,

  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FLP_PROCESS_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FLP_PROCESS_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FLP_LOOKUPS_TBL_SET_UNSAFE,
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_GET_UNSAFE,
  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_SET_UNSAFE,
  ARAD_PP_EPNI_ISID_TABLE_TBL_GET_UNSAFE,
  ARAD_PP_EPNI_ISID_TABLE_TBL_SET_UNSAFE,



  ARAD_WB_DB_INIT,
  ARAD_WB_DB_LAYOUT_INIT,
  ARAD_WB_DB_RESTORE_STATE,
  ARAD_WB_DB_INFO_ALLOC,
  ARAD_WB_DB_SYNC,

  ARAD_WB_DB_LAG_RESTORE_IN_USE_STATE,
  ARAD_WB_DB_LAG_RESTORE_LOCAL_TO_SYS_STATE,
  ARAD_WB_DB_LAG_RESTORE_LOCAL_TO_REASSEMBLY_CONTEXT_STATE,
  ARAD_WB_DB_LAG_RESTORE_LOCAL_TO_XGS_MAC_EXTENDER_INFO_STATE,
  ARAD_WB_DB_LAG_SAVE_IN_USE_STATE,
  ARAD_WB_DB_LAG_SAVE_LOCAL_TO_SYS_STATE,
  ARAD_WB_DB_LAG_SAVE_LOCAL_TO_REASSEMBLY_CONTEXT_STATE,
  ARAD_WB_DB_LAG_SAVE_LOCAL_TO_XGS_MAC_EXTENDER_INFO_STATE,
  ARAD_WB_DB_LAG_UPDATE_IN_USE_STATE,
  ARAD_WB_DB_LAG_UPDATE_LOCAL_TO_SYS_STATE,
  ARAD_WB_DB_LAG_UPDATE_LOCAL_TO_REASSEMBLY_CONTEXT_STATE,
  ARAD_WB_DB_LAG_UPDATE_LOCAL_TO_XGS_MAC_EXTENDER_INFO_STATE,
  
  ARAD_WB_DB_EGR_PORT_RESTORE_QUEUE_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_TCG_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_PRIORITY_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_TCG_SHAPER_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_SCH_CHAN_ARB_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_NOF_CALCAL_INSTANCES_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_CALCAL_LENGTH_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_UPDATE_DEVICE_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_UPDATE_DEV_CHANGED_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_EGQ_TCG_QPAIR_SHAPER_ENABLE_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_ERP_INTERFACE_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_DSP_PP_TO_BASE_QUEUE_PAIR_MAPPING_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_DSP_PP_NOF_QUEUE_PAIRS_STATE,
  ARAD_WB_DB_EGR_PORT_RESTORE_PORTS_PROG_EDITOR_PROFILE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_QUEUE_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_TCG_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_PRIORITY_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_TCG_SHAPER_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_SCH_CHAN_ARB_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_NOF_CALCAL_INSTANCES_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_CALCAL_LENGTH_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_UPDATE_DEVICE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_UPDATE_DEV_CHANGED_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_EGQ_TCG_QPAIR_SHAPER_ENABLE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_ERP_INTERFACE_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_DSP_PP_TO_BASE_QUEUE_PAIR_MAPPING_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_DSP_PP_NOF_QUEUE_PAIRS_STATE,
  ARAD_WB_DB_EGR_PORT_SAVE_PORTS_PROG_EDITOR_PROFILE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_QUEUE_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_TCG_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_PRIORITY_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_TCG_SHAPER_RATES_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_SCH_CHAN_ARB_RATE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_NOF_CALCAL_INSTANCES_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_CALCAL_LENGTH_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_UPDATE_DEVICE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_UPDATE_DEV_CHANGED_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_EGQ_TCG_QPAIR_SHAPER_ENABLE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_ERP_INTERFACE_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_DSP_PP_TO_BASE_QUEUE_PAIR_MAPPING_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_DSP_PP_NOF_QUEUE_PAIRS_STATE,
  ARAD_WB_DB_EGR_PORT_UPDATE_PORTS_PROG_EDITOR_PROFILE_STATE,
  ARAD_WB_DB_MULTICAST_RESTORE_NOF_UNOCCUPIED_STATE,
  ARAD_WB_DB_MULTICAST_RESTORE_UNOCCUPIED_LIST_HEAD_STATE,
  ARAD_WB_DB_MULTICAST_RESTORE_NEXT_UNOCCUPIED_PTR_STATE,
  ARAD_WB_DB_MULTICAST_RESTORE_BACKWARDS_PTR_STATE,
  ARAD_WB_DB_MULTICAST_RESTORE_EG_MULT_NOF_VLAN_BITMAPS_STATE,
  ARAD_WB_DB_MULTICAST_SAVE_NOF_UNOCCUPIED_STATE,
  ARAD_WB_DB_MULTICAST_SAVE_UNOCCUPIED_LIST_HEAD_STATE,
  ARAD_WB_DB_MULTICAST_SAVE_NEXT_UNOCCUPIED_PTR_STATE,
  ARAD_WB_DB_MULTICAST_SAVE_BACKWARDS_PTR_STATE,
  ARAD_WB_DB_MULTICAST_SAVE_EG_MULT_NOF_VLAN_BITMAPS_STATE,
  ARAD_WB_DB_MULTICAST_UPDATE_NOF_UNOCCUPIED_STATE,
  ARAD_WB_DB_MULTICAST_UPDATE_UNOCCUPIED_LIST_HEAD_STATE,
  ARAD_WB_DB_MULTICAST_UPDATE_NEXT_UNOCCUPIED_PTR_STATE,
  ARAD_WB_DB_MULTICAST_UPDATE_BACKWARDS_PTR_STATE,
  ARAD_WB_DB_MULTICAST_UPDATE_EG_MULT_NOF_VLAN_BITMAPS_STATE,
  
  ARAD_WB_DB_CELL_RESTORE_CURRENT_CELL_IDENT_STATE,
  ARAD_WB_DB_CELL_SAVE_CURRENT_CELL_IDENT_STATE,
  ARAD_WB_DB_CELL_UPDATE_CURRENT_CELL_IDENT_STATE,


  ARAD_WB_DB_TDM_RESTORE_CONTEXT_MAP_STATE, 
  ARAD_WB_DB_TDM_SAVE_CONTEXT_MAP_STATE,
  ARAD_WB_DB_TDM_UPDATE_CONTEXT_MAP_STATE,

  ARAD_WB_DB_MULTICAST_RESTORE_EGRESS_GROUPS_OPEN_DATA_STATE,
  ARAD_WB_DB_MULTICAST_SAVE_EGRESS_GROUPS_OPEN_DATA_STATE,
  ARAD_WB_DB_MULTICAST_UPDATE_EGRESS_GROUPS_OPEN_DATA_STATE,

  ARAD_WB_DB_VSI_RESTORE_ISID_STATE,
  ARAD_WB_DB_VSI_SAVE_ISID_STATE,
  ARAD_WB_DB_VSI_UPDATE_ISID_STATE,

  ARAD_SW_DB_TM_RESTORE_IS_SIMPLE_Q_TO_RATE_CLS_MODE_STATE,
  ARAD_SW_DB_TM_SAVE_IS_SIMPLE_Q_TO_RATE_CLS_MODE_STATE,
  ARAD_SW_DB_TM_UPDATE_IS_SIMPLE_Q_TO_RATE_CLS_MODE_STATE,

  ARAD_SW_DB_TM_RESTORE_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_STATE,
  ARAD_SW_DB_TM_SAVE_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_STATE,
  ARAD_SW_DB_TM_UPDATE_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_STATE,

  ARAD_SW_DB_TM_RESTORE_SYSPORT_TO_BASEQUEUE_STATE,
  ARAD_SW_DB_TM_SAVE_SYSPORT_TO_BASEQUEUE_STATE,
  ARAD_SW_DB_TM_UPDATE_SYSPORT_TO_BASEQUEUE_STATE,

  ARAD_WB_DB_TM_RESTORE_MODPORT_TO_SYSPORT_STATE,
  ARAD_WB_DB_TM_SAVE_MODPORT_TO_SYSPORT_STATE,
  ARAD_WB_DB_TM_UPDATE_MODPORT_TO_SYSPORT_STATE,

  ARAD_EGR_PROG_EDITOR_ALLOCATE_PROGRAM_AND_ITS_LFEMS,
  ARAD_EGR_PROG_EDITOR_ALLOCATE_PROGRAMS_AND_LFEMS_ALLOC,
  ARAD_EGR_PROG_EDITOR_PROGRAMS_USAGE_INIT,
  ARAD_SW_DB_SRC_BIND_INITIALIZE,
  ARAD_SW_DB_REASSEMBLY_CONTEXT_INITIALIZE,

  ARAD_NOF_PROCS
} ARAD_PROCS;





typedef enum ARAD_ERR_LIST
{

  ARAD_START_ERR_LIST_NUMBER      = SOC_SAND_ARAD_START_ERR_NUMBER,
  ARAD_DO_NO_USE_0001,
  ARAD_DO_NO_USE_0002,
  ARAD_REGS_NOT_INITIALIZED,
  ARAD_TBLS_NOT_INITIALIZED,
  ARAD_FLD_OUT_OF_RANGE,
  ARAD_REG_ACCESS_ERR,

  ARAD_GET_ERR_TEXT_001,

  ARAD_PROC_ID_TO_STRING_001,

  
  ARAD_DIVISION_BY_ZERO_ERR,

  
  ARAD_ING_SCH_WEIGHT_OUT_OF_RANGE_ERR,
  
  ARAD_ING_SCH_MAX_CREDIT_OUT_OF_RANGE_ERR,
  
  ARAD_ING_SCH_MESH_ID_OUT_OF_RANGE_ERR,
  
  ARAD_ING_SCH_MESH_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  
  ARAD_ING_SCH_EXACT_CAL_LARGER_THAN_MAXIMUM_VALUE_ERR,
  
  ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_CR_REQUEST_HUNGRY_TH_MULTIPLIER_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_CR_REQUEST_SATISFIED_EMPTY_Q_TH_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_CR_REQUEST_WD_TH_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_CR_REQUEST_OCB_ONLY_ERR,
  
  ARAD_ITM_CR_DISCOUNT_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_CR_CLS_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_ORDER_ERR,
  ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR,
  
  ARAD_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR,
  ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR,
  ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR,
  ARAD_DRPP_OUT_OF_RANGE_ERR,
  ARAD_ITM_ADMT_TEST_ID_OUT_OF_RANGE_ERR,


  ARAD_ITM_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR,
  ARAD_ITM_WRED_MIN_AVRG_TH_OUT_OF_RANGE_ERR,
  ARAD_ITM_WRED_MAX_AVRG_TH_OUT_OF_RANGE_ERR,
  ARAD_ITM_WRED_PROB_OUT_OF_RANGE_ERR,
  ARAD_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
  ARAD_QUEUE_ID_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR,
  ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_MAX_INST_Q_SIZ_PARAMETER_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR,
  ARAD_ITM_VSQ_MIN_AVRG_TH_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_MAX_AVRG_TH_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_LBL_IN_STAT_TAG_LSB_LARGER_THAN_MSB_ERR,
  ARAD_ITM_VSQ_LBL_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_LBL_DP_LSB_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_VSQ_INDEX_OUT_OF_RANGE,
  ARAD_SIGNATURE_OUT_OF_RANGE_ERR,
  ARAD_ITM_ING_SHAPE_SCH_PORT_OUT_OF_RANGE_ERR,
  ARAD_ITM_ING_SHAPE_SCH_RATE_OUT_OF_RANGE_ERR,
  ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR,
  ARAD_MULTI_FABRIC_QUEUE_ORDER_ERR,
  ARAD_ITM_ING_SHAPE_Q_LOW_ABOVE_HIGH_NUM_OUT_OF_RANGE_ERR,
  ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR,
  ARAD_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR,
  ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR,
  ARAD_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR,
  ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR,
  ARAD_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR,
  ARAD_ITM_SYS_RED_GLOB_RCS_THS_OUT_OF_RANGE_ERR,
  ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR,
  ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR,
  ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR,
  ARAD_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR,
  ARAD_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR,
  ARAD_IPQ_TRAFFIC_CLASS_MAP_TR_CLS_OUT_OF_RANGE_ERR,
  ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_DEST_NDX_OUT_OF_RANGE_ERR,
  ARAD_IPQ_DESTINATION_ID_PACKETS_BASE_BASE_QUEUE_OUT_OF_RANGE_ERR,
  ARAD_IPQ_DESTINATION_ID_PACKETS_CONFIG_ERR,
  ARAD_IPQ_INVALID_TC_PROFILE_ERR,
  ARAD_IPQ_INVALID_FLOW_ID_ERR,
  ARAD_IPQ_INVALID_SYS_PORT_ERR,
  ARAD_IPQ_INVALID_TC_ERR,
  ARAD_PORTS_MIRROR_PORT_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PORTS_MIRROR_DROP_PRCDNC_OUT_OF_RANGE_ERR,
  ARAD_PORTS_MIRROR_TR_CLS_OUT_OF_RANGE_ERR,
  ARAD_SYS_PHYSICAL_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_IPQ_K_QUEUE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_CORE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_FLOW_ID_OUT_OF_RANGE_ERR,
  ARAD_IPQ_BASE_FLOW_FOR_INTERDIGIT_QUEUE_QUARTET_TOO_LOW_ERR,
  ARAD_IPQ_BASE_FLOW_ALREADY_MAPPED_BY_PREVIOUS_QUEUE_QUARTET_ERR,
  ARAD_IPQ_BASE_FLOW_QUARTET_NOT_EVEN_ERR,
  ARAD_IPQ_BASE_FLOW_QUARTET_NOT_MULTIPLY_OF_FOUR_ERR,
  ARAD_MULT_FABRIC_ILLEGAL_MULTICAST_CLASS_ERR,
  ARAD_MULT_FABRIC_ILLEGAL_NUMBER_OF_QUEUE_ERR,
  ARAD_MULT_FABRIC_ILLEGAL_CONF_ERR,
  ARAD_MULT_FABRIC_ILLEGAL_NOF_LINKS,
  ARAD_MULT_EG_ILLEGAL_GROUP_RANG_CONFIG_ERR,
  ARAD_MULT_ILLEGAL_MULT_ID_ERR,
  ARAD_MULT_EG_ILLEGAL_NOF_REPLICATIONS_CONFIG_ERR,
  ARAD_MULT_EG_MULTICAST_ID_NOT_IN_VLAN_MEMBERSHIP_RNG_ERR,
  ARAD_MULT_EG_MULTICAST_ID_IS_IN_VLAN_MEMBERSHIP_RNG_ERR,
  ARAD_MULT_EG_ILLEGAL_VLAN_PTR_ENTRY_IN_LINK_LIST_ERR,
  ARAD_MULT_ENTRY_DOES_NOT_EXIST,
  ARAD_MULT_MC_GROUP_REAL_SIZE_LARGER_THAN_GIVEN_SIZE_ERR,
  ARAD_MULT_LL_ILLEGAL_ENTRY_FOR_RELOCATION_ERR,
  ARAD_MULT_EG_EXPECTED_A_TDM_REPLICATION_ERR,
  ARAD_MULT_EG_ILLEGAL_PORT,
  ARAD_MULT_MC_ID_OUT_OF_RANGE_ERR,
  ARAD_MULT_MC_RAW_ID_OUT_OF_RANGE_ERR,
  ARAD_MULT_REMOVE_ELEMENT_DID_NOT_SUCCEED_ERROR,
  ARAD_PACKET_SIZE_OUT_OF_RANGE_ERR,
  ARAD_SRD_REG_ADDR_OUT_OF_RANGE_ERR,
  ARAD_SRD_EARAD_TIMEOUT_ERR,
  ARAD_DIAG_MBIST_POLL_TIMEOUT_ERR,
  ARAD_SRD_LLA_EARAD_CMD_READ_ERR,
  ARAD_SRD_LANE_ID_OUT_OF_RANGE_ERR,
  ARAD_SRD_STAR_ID_OUT_OF_RANGE_ERR,
  ARAD_SRD_QRTT_ID_OUT_OF_RANGE_ERR,
  ARAD_SRD_LANE_LOOPBACK_MODE_OUT_OF_RANGE_ERR,
  ARAD_SRD_LANE_EQ_MODE_OUT_OF_RANGE_ERR,
  ARAD_SRD_LANE_STATE_OUT_OF_RANGE_ERR,
  ARAD_SRD_RATE_DIVISOR_OUT_OF_RANGE_ERR,
  ARAD_SRD_AMP_VAL_OUT_OF_RANGE_ERR,
  ARAD_SRD_MEDIA_TYPE_OUT_OF_RANGE_ERR,
  ARAD_SRD_EXPLCT_PRE_OUT_OF_RANGE_ERR,
  ARAD_SRD_EXPLCT_POST_OUT_OF_RANGE_ERR,
  ARAD_SRD_EXPLCT_SWING_OUT_OF_RANGE_ERR,
  ARAD_SRD_ATTEN_OUT_OF_RANGE_ERR,
  ARAD_SRD_MAIN_VAL_OUT_OF_RANGE_ERR,
  ARAD_SRD_PRE_EMPHASIS_VAL_OUT_OF_RANGE_ERR,
  ARAD_SRD_POST_EMPHASIS_VAL_OUT_OF_RANGE_ERR,
  ARAD_SRD_UNEXPECTED_LOOPBACK_MODE_ERR,
  ARAD_SRD_MORE_THAN_ONE_LOOPBACK_MODE_DETECTED_ERR,
  ARAD_SRD_LOOPBACK_ENABLED_BUT_NO_MODE_ERR,
  ARAD_SRD_EQ_MORE_THAN_ONE_MODE_DETECTED_ERR,
  ARAD_SRD_UNEXPECTED_LANE_EQ_MODE_ERR,
  ARAD_SRD_CMU_ELEMENT_OUT_OF_RANGE_ERR,
  ARAD_SRD_LANE_ELEMENT_OUT_OF_RANGE_ERR,
  ARAD_SRD_EYE_SCAN_RESOLUTION_OUT_OF_RANGE_ERR,
  ARAD_SRD_EYE_SCAN_PRBS_DURATION_MIN_SEC_OUT_OF_RANGE_ERR,
  ARAD_SRD_EYE_SCAN_RANGE_OUT_OF_RANGE_ERR,
  ARAD_SRD_ELEMENT_OUT_OF_RANGE_ERR,
  ARAD_SCH_INVALID_SIMPLE_FLOW_ID_ERR,
  ARAD_SCH_INVALID_FLOW_ID_ERR,
  ARAD_SCH_INVALID_SE_ID_ERR,
  ARAD_SCH_INVALID_PORT_ID_ERR,
  ARAD_SCH_INVALID_SE_HR_ID_ERR,
  ARAD_SCH_INVALID_K_FLOW_ID_ERR,
  ARAD_SCH_INVALID_QUARTET_ID_ERR,
  ARAD_SCH_AGGR_SE_AND_FLOW_ID_MISMATCH_ERR,
  ARAD_SCH_CL_ID_OUT_OF_RANGE_ERR,
  ARAD_SCH_CL_CLASS_MODE_OUT_OF_RANGE_ERR,
  ARAD_SCH_DESCRETE_WEIGHT_OUT_OF_RANGE_ERR,
  ARAD_SCH_CL_CLASS_WEIGHTS_MODE_OUT_OF_RANGE_ERR,
  ARAD_SCH_CL_ENHANCED_MODE_OUT_OF_RANGE_ERR,
  ARAD_SCH_DEVICE_IF_WEIGHT_OUT_OF_RANGE_ERR,
  ARAD_SCH_FABRIC_LINK_ID_OUT_OF_RANGE_ERR,
  ARAD_SCH_FLOW_HR_AND_SCHEDULER_MODE_MISMATCH_ERR,
  ARAD_SCH_FLOW_HR_CLASS_OUT_OF_RANGE_ERR,
  ARAD_SCH_FLOW_ID_IS_SECOND_SUB_FLOW_ERR,
  ARAD_SCH_FLOW_STATUS_NOT_ON_OFF_ERR,
  ARAD_SCH_FLOW_TO_FIP_SECOND_QUARTET_MISMATCH_ERR,
  ARAD_SCH_FLOW_TO_Q_INVALID_GLOBAL_CONF_ERR,
  ARAD_SCH_FLOW_TO_Q_NOF_QUARTETS_MISMATCH_ERR,
  ARAD_SCH_FLOW_TO_Q_ODD_EVEN_IS_FALSE_ERR,
  ARAD_SCH_FLOW_TYPE_UNDEFINED_ERR,
  ARAD_SCH_GAP_IN_SUB_FLOW_ERR,
  ARAD_SCH_GROUP_OUT_OF_RANGE_ERR,
  ARAD_SCH_GRP_AND_PORT_RATE_MISMATCH_ERR,
  ARAD_SCH_NOF_CLASS_TYPES_OUT_OF_RANGE_ERR,
  ARAD_SCH_HP_CLASS_IDX_OUT_OF_RANGE_ERR,
  ARAD_SCH_HP_CLASS_NOT_AVAILABLE_ERR,
  ARAD_SCH_HP_CLASS_OUT_OF_RANGE_ERR,
  ARAD_SCH_HP_CLASS_VAL_INVALID_ERR,
  ARAD_SCH_HR_MODE_OUT_OF_RANGE_ERR,
  ARAD_SCH_CLCONFIG_OUT_OF_RANGE_ERR,
  ARAD_SCH_ENH_MODE_OUT_OF_RANGE_ERR,
  ARAD_SCH_INTERFACE_IS_SINGLE_PORT_ERR,
  ARAD_SCH_INVALID_IF_TYPE_ERR,
  ARAD_SCH_INTERFACE_RATE_OUT_OF_RANGE_ERR,
  ARAD_SCH_INTERFACE_WEIGHT_INDEX_OUT_OF_RANGE_ERR,
  ARAD_SCH_HR_MODE_INVALID_ERR,
  ARAD_INTERFACE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_INTERFACE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_INVALID_CH_NIF_INDEX_ERR,
  ARAD_INVALID_PORT_NIF_INDEX_ERR,
  ARAD_TCG_OUT_OF_RANGE_ERR,
  ARAD_SCH_TCG_WEIGHT_OUT_OF_RANGE_ERR,
  ARAD_EGQ_TCG_WEIGHT_OUT_OF_RANGE_ERR,
  ARAD_SCH_INVALID_PORT_GROUP_ERR,
  ARAD_SCH_SE_ID_AND_TYPE_MISMATCH_ERR,
  ARAD_SCH_SE_PORT_RATE_OUT_OF_RANGE_ERR,
  ARAD_SCH_SE_PORT_SE_TYPE_NOT_HR_ERR,
  ARAD_SCH_SE_STATE_OUT_OF_RANGE_ERR,
  ARAD_SCH_SE_TYPE_SE_CONFIG_MISMATCH_ERR,
  ARAD_SCH_SE_TYPE_UNDEFINED_ERR,
  ARAD_SCH_SLOW_RATE_OUT_OF_RANGE_ERR,
  ARAD_SCH_SUB_FLOW_AND_SCHEDULER_MODE_MISMATCH_ERR,
  ARAD_SCH_SUB_FLOW_AND_SCHEDULER_TYPE_MISMATCH_ERR,
  ARAD_SCH_SUB_FLOW_ATTACHED_TO_DISABLED_SCHEDULER_ERR,
  ARAD_SCH_SUB_FLOW_CLASS_OUT_OF_RANGE_ERR,
  ARAD_SCH_SUB_FLOW_ENHANCED_SP_MODE_MISMATCH_ERR,
  ARAD_SCH_SUB_FLOW_ID_MISMATCH_WITH_FLOW_ID_ERR,
  ARAD_SCH_SUB_FLOW_SE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_SCH_SUB_FLOW_WEIGHT_OUT_OF_RANGE_ERR,
  ARAD_FAP_PORT_ID_INVALID_ERR,
  ARAD_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR,
  ARAD_EGR_Q_PRIO_OUT_OF_RANGE_ERR,
  ARAD_TCG_NOT_SUPPORTED_ERR,
  ARAD_TCG_SINGLE_MEMBER_ERR,
  ARAD_REGS_FIELD_VAL_OUT_OF_RANGE_ERR,
  ARAD_DROP_PRECEDENCE_OUT_OF_RANGE_ERR,
  ARAD_TRAFFIC_CLASS_OUT_OF_RANGE_ERR,
  ARAD_REGS_DEFAULT_INSTANCE_FOR_MULTI_INST_BLOCK_ERR,
  ARAD_REGS_NON_DEFAULT_INSTANCE_FOR_SINGLE_INST_BLOCK_ERR,
  ARAD_EGR_MANTISSA_OUT_OF_RANGE_ERR,
  ARAD_EGR_EXPONENT_OUT_OF_RANGE_ERR,
  ARAD_EGR_MNT_EXP_FLD_OUT_OF_RANGE_ERR,
  ARAD_NOT_A_CHANNELIZED_IF_ERR,
  ARAD_EGR_MCI_PRIO_OUT_OF_RANGE_ERR,
  ARAD_EGR_MCI_PRIO_AND_ID_MISMATCH_ERR,
  ARAD_EGR_MCI_ERP_AND_MCI_ENABLE_ERR,
  ARAD_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR,
  ARAD_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR,
  ARAD_EGR_Q_PRIO_MAPPING_TYPE_OUT_OF_RANGE_ERR,
  ARAD_EGR_QDCT_PD_VALUE_OUT_OF_RANGE,
  ARAD_EGR_QDCT_DB_VALUE_OUT_OF_RANGE,
  ARAD_PORTS_INVALID_ECI_PORT_IDX_ERR,
  ARAD_NIF_INDEX_OUT_OF_RANGE_ERR,
  ARAD_NIF_SGMII_INTERFACE_NOF_SINGLE_RATE_ERR,
  ARAD_NIF_MAL_INDEX_OUT_OF_RANGE_ERR,
  ARAD_NIF_DIRECTION_OUT_OF_RANGE_ERR,
  ARAD_NIF_INVALID_TYPE_ERR,
  ARAD_NIF_IPG_SIZE_OUT_OF_RANGE_ERR,
  ARAD_NIF_PREAMBLE_AND_BCT_CONF_CONFLICT_ERR,
  ARAD_NIF_BCT_SIZE_OUT_OF_RANGE_ERR,
  ARAD_NIF_BCT_CHANNEL_NDX_OUT_OF_RANGE_ERR,
  ARAD_NIF_MAL_INVALID_CONFIG_STATUS_ON_ERR,
  ARAD_NIF_TWO_LAST_MALS_SERDES_OVERLAP_ERR,
  ARAD_NIF_INCOMPATIBLE_TYPE_ERR,
  ARAD_NIF_MDIO_LESS_THEN_ONE_WORD_DATA_ERR,
  ARAD_NIF_COUNTER_OUT_OF_RANGE_ERR,
  ARAD_PORT_CONTROL_PCS_OUT_OF_RANGE_ERR,             
  ARAD_PORT_CONTROL_PCS_INNER_LINK_OUT_OF_RANGE_ERR, 
  ARAD_PORT_CONTROL_PCS_LINK_OUT_OF_RANGE_ERR,         
  ARAD_PORT_CONTROL_PCS_GET_ENCODING_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PORT_UNOCCUPIED_INTERFACE_FOR_ERP_ERR,
  ARAD_NIF_ILKN_ID_OUT_OF_RANGE_ERR,
  ARAD_IPQ_NIF_ID_NOT_FIRST_IN_MAL_ERR,
  ARAD_CONNECTION_DIRECTION_OUT_OF_RANGE_ERR,
  ARAD_DBG_FORCE_MODE_OUT_OF_RANGE_ERR,
  ARAD_DBG_Q_FLUSH_ALL_TIMEOUT_ERR,
  ARAD_DBG_Q_FLUSH_MODE_OUT_OF_RANGE_ERR,
  ARAD_DBG_FORCE_MODE_FLD_OUT_OF_RANGE_ERR,
  ARAD_FABRIC_ILLEGAL_CONNECT_MODE_FE_ERR,
  ARAD_FABRIC_FAP20_ID_OUT_OF_RANGE_ERR,
  ARAD_FC_INGR_NOF_GEN_NIF_LL_OUT_OF_RANGE_ERR,
  ARAD_FC_INGR_NOF_GEN_NIF_CB_OUT_OF_RANGE_ERR,
  ARAD_FC_NIF_CB_CLASSES_OUT_OF_RANGE_ERR,
  ARAD_FC_NIF_CB_SGMII_INVALID_CONF_ERR,
  ARAD_FC_RCY_CONNECTION_CLS_OUT_OF_RANGE_ERR,
  ARAD_FC_NOF_RCY_OFP_HANDLERS_OUT_OF_RANGE_ERR,
  ARAD_FC_RCY_ON_GLB_RCS_MODE_OUT_OF_RANGE_ERR,
  ARAD_FC_OOB_ID_OUT_OF_RANGE_ERR,
  ARAD_FC_OOB_CAL_REP_OF_RANGE_ERR,
  ARAD_FC_OOB_CAL_LEN_OF_RANGE_ERR,
  ARAD_FC_OOB_CAL_SRC_TYPE_INVALID_ERR,
  ARAD_FC_OOB_CAL_SRC_ID_OF_RANGE_ERR,
  ARAD_FC_OOB_CAL_DEST_TYPE_INVALID_ERR,
  ARAD_FC_OOB_CAL_EXCESSIVE_NOF_SCH_OFP_HRS_ERR,
  ARAD_FC_OOB_CAL_DEST_ID_OF_RANGE_ERR,
  ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_RANGE_ERR,
  ARAD_FC_ILKN_RETRANSMIT_CAL_RX_CFG_ERR,
  ARAD_CNM_ILLEGAL_CP_QUEUE_INDEX,
  ARAD_CNM_ILLEGAL_CP_PROFILE_INDEX,
  ARAD_CNM_ILLEGAL_CNM_GEN_MODE,
  ARAD_CNM_ILLEGAL_CP_QUEUE_SET,
  ARAD_CNM_ILLEGAL_CP_QUEUE_RANGE,
  ARAD_CNM_CP_OPTIONS_GET_UNSAFE,
  ARAD_CNM_CP_OPTIONS_SET_UNSAFE,
  ARAD_CNM_CP_OPTIONS_GET,
  ARAD_CNM_CP_OPTIONS_SET,
  ARAD_IHP_STAG_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_IHP_TMLAG_OFFSET_BASE_OUT_OF_RANGE_ERR,
  ARAD_IHP_TMLAG_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_PORT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_IHP_SOP2HEADER_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_IHP_STRIP_FROM_SOP_OUT_OF_RANGE_ERR,
  ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR,
  ARAD_REVISION_SUB_TYPE_OUT_OF_LIMIT_ERR,
  ARAD_CHIP_TYPE_UNKNOWN_ERR,
  ARAD_CREDIT_SIZE_OUT_OF_RANGE_ERR,
  ARAD_CREDIT_TYPE_INVALID_ERR,
  ARAD_FAP_FABRIC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_ENABLE_NOT_SUPPORTED_ERR,
  ARAD_SYSTEM_PHYSICAL_PORT_OUT_OF_RANGE_ERR,
  ARAD_SYSTEM_PORT_OUT_OF_RANGE_ERR,
  ARAD_CUD_ID_OUT_OF_RANGE_ERR,
  ARAD_DEVICE_ID_OUT_OF_RANGE_ERR,
  ARAD_PORT_DIRECTION_OUT_OF_RANGE_ERR,
  ARAD_IF_CHANNEL_ID_OUT_OF_RANGE_ERR,
  ARAD_PORT_LAG_ID_OUT_OF_RANGE_ERR,
  ARAD_PORT_LAG_NOF_MEMBERS_OUT_OF_RANGE_ERR,
  ARAD_PORT_LAG_SYS_PORT_ALREADY_MEMBER_ERR,
  ARAD_PORT_LAG_LB_KEY_OUT_OF_RANGE,
  ARAD_DEVICE_ID_ABOVE_OUT_OF_RANGE_ERR,
  ARAD_PORTS_OTMH_OUTLIF_EXT_PERMISSION_ERR,
  ARAD_CELL_VARIABLE_IN_FAP20_SYSTEM_ERR,
  ARAD_CELL_VAR_SIZE_IN_FE200_SYSTEM_ERR,
  ARAD_HW_DRAM_CONF_LEN_OUT_OF_RANGE_ERR,
  ARAD_HW_DRAM_NOF_INTERFACES_OUT_OF_RANGE_ERR,
  ARAD_HW_QDR_PROTECT_TYPE_INVALID_ERR,
  ARAD_HW_QDR_SIZE_OUT_OF_RANGE_ERR,
  ARAD_MGMT_EGQ_INIT_FAILS_ERR,
  ARAD_NIF_NOF_PORTS_IN_FAT_PIPE_OUT_OF_RANGE_ERR,
  ARAD_LAG_GROUP_ID_OUT_OF_RANGE_ERR,
  ARAD_LAG_ENTRY_ID_OUT_OF_RANGE_ERR,
  ARAD_DEST_TYPE_OUT_OF_RANGE_ERR,
  ARAD_DEST_SYS_PORT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_FAP_INTERFACE_AND_PORT_TYPE_MISMATCH_ERR,
  ARAD_OUTBND_MIRR_IFP_NOT_MAPPED_TO_RCY_IF_ERR,
  ARAD_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR,
  ARAD_AQFM_CREDIT_SOURCE_ID_ERR,
  ARAD_AQFM_SCH_SUB_FLOW_TYPE_OUT_OF_RANGE_ERR,
  ARAD_AQFM_CANT_ALLOC_AGG_ID_ERR,
  ARAD_AQFM_INVALID_PORT_ID_ERR,
  ARAD_SRD_TX_LANE_DATA_RATE_DIVISOR_INVALID_ERR,
  ARAD_SRD_CMU_REF_CLK_OUT_OF_RANGE_ERR,
  ARAD_MGMT_DRAM_INIT_FAILS_ERR,
  ARAD_PORTS_IF_EXCEEDED_MAX_ITERATIONS_ERR,
    ARAD_PMF_CE_INTERNAL_FIELD_NOT_FOUND_ERR,
  ARAD_OFP_SHPR_MODE_OUT_OF_RANGE_ERR,
  ARAD_OFP_MAL_INDEX_MISMATCH_ERR,
  ARAD_NIF_FC_LL_MODE_OUT_OF_RANGE_ERR,
  ARAD_SRD_ENTITY_TYPE_OUT_OF_RANGE_ERR,
  ARAD_SRD_FLD_SIZE_OUT_OF_RANGE_ERR,
  ARAD_MGMT_BASIC_CONF_NULL_AT_INIT_ERR,
  ARAD_MGMT_BASIC_CONF_NOT_SUPPLIED_ERR,
  ARAD_FABRIC_CONNECT_MESH_MODE_CHANGE_ERR,
  ARAD_FABRIC_SHAPER_MODE_OUT_OF_RANGE_ERR,
  ARAD_FTMH_EXTENSION_OUT_OF_RANGE_ERR,
  ARAD_MGMT_QDR_NOT_READY_ERR,
  ARAD_MGMT_QDR_INIT_BIST_DID_NOT_FINISH_ERR,
  ARAD_MGMT_QDRC_NOT_LOCKED_ERR,
  ARAD_MGMT_QDR_TRAINING_REPLY_FAIL_ERR,
  ARAD_MGMT_INIT_INVALID_FREQUENCY,
  ARAD_MGMT_INIT_INVALID_DBUFF_SIZE_ERR,
  ARAD_MGMT_INIT_FINALIZE_ERR,
  ARAD_INIT_QDR_PLL_NOT_LOCKED_ERR,
  ARAD_INIT_DDR_PLL_NOT_LOCKED_ERR,
  ARAD_INIT_CORE_PLL_NOT_LOCKED_ERR,
  ARAD_SRD_8051_CHECKSUM_STILL_RUNNING_ERR,
  ARAD_SRD_8051_CHECKSUM_FAIL_ERR,
  ARAD_SRD_8051_CHECKSUM_DID_NOT_COMPLETE_ERR,
  ARAD_SRD_TRIM_FAILED_ERR,
  ARAD_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR,
  ARAD_HW_INVALID_NOF_BANKS_FOR_DRAM_TYPE_ERR,
  ARAD_HW_INVALID_NOF_BANKS_ERR,
  ARAD_HW_NOF_COLUMNS_OUT_OF_RANGE_ERR,
  ARAD_SRD_TX_TRIMMING_DID_NOT_END_ERR,
  ARAD_SRD_PLL_RESET_DID_NOT_END_ERR,
  ARAD_SRD_LN_RX_RESET_DID_NOT_END_ERR,
  ARAD_NIF_SRD_RATE_OUT_OF_RANGE_ERR,
  ARAD_NIF_MAL_IS_FABRIC_NOT_NIF_ERR,
  ARAD_STAT_NO_SUCH_COUNTER_ERR,
  ARAD_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR,
  ARAD_SRD_PLL_CONF_NOT_FOUND_ERR,
  ARAD_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR,
  ARAD_DIAG_PATTERN_MODE_OUT_OF_RANGE_ERR,
  ARAD_DIAG_BIST_ADDRESS_ILLEGAL_RANGE_ERR,
  ARAD_DIAG_DRAM_OFFSET_ILLEGAL_RANGE_ERR,
  ARAD_DIAG_INCONSISTENT_DRAM_CONFIG_ERR,
  ARAD_DIAG_BIST_DATA_MODE_OUT_OF_RANGE_ERR,
  ARAD_DIAG_DUMP_MODE_OUT_OF_RANGE_ERR,
  ARAD_DIAG_BIST_DATA_PATERN_OUT_OF_RANGE_ERR,
  ARAD_DIAG_BIST_WRITES_PER_CYCLE_OUT_OF_RANGE_ERR,
  ARAD_DIAG_BIST_READS_PER_CYCLE_OUT_OF_RANGE_ERR,
  ARAD_DIAG_DRAM_ACCESS_TIMEOUT_ERR,
  ARAD_DIAG_DRAM_ACC_NOF_COLUMNS_INVALID_ERR,
  ARAD_SRD_ACCESS_INTERNAL_REG_WITH_EXTERNAL_CONF_ERR,
  ARAD_SRD_CMU_VER_MISMATCH_ERR,
  ARAD_DBUFF_SIZE_INVALID_ERR,
  ARAD_PKT_TX_CPU_PACKET_BYTE_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PKT_ASYNC_MODE_CONFIG_ERR,
  ARAD_FAT_PIPE_NO_SEQ_HDR_ERR,
  ARAD_PORT_ID_CONSUMED_BY_FAT_PIPE_ERR,
  ARAD_FAT_PIPE_NOT_SET_ERR,
  ARAD_FAT_PIPE_MULTIPLE_PORTS_ERR,
  ARAD_FAT_PIPE_MUST_BE_FIRST_ERR,
  ARAD_FAT_PIPE_INVALID_PORT_ID_ERR,
  ARAD_MULT_VLAN_MEMB_ID_OUT_OF_RANGE_ERR,
  ARAD_TBL_PROG_IDX_OUT_OF_RANGE_ERR,
  ARAD_TBL_INSTR_IDX_OUT_OF_RANGE_ERR,
  ARAD_TBL_PRGR_COS_SET_IDX_OUT_OF_RANGE_ERR,
  ARAD_SRD_INVALID_REF_CLOCK_ERR,
  ARAD_SRD_TX_CONF_MODE_OUT_OF_RANGE_ERR,
  ARAD_SRD_INVALID_RATE_ERR,
  ARAD_SRD_MISCONFIGURED_RATE_ON_POWERUP_ERR,
  ARAD_SRD_MISCONFIGURED_RATE_ERR,
  ARAD_UNSUPPORTED_ERR,
  ARAD_UNSUPPORTED_FOR_DEVICE_ERR,
  ARAD_SRD_PLL_NOT_LOCKED_ERR,
  ARAD_SRD_INVALID_RATE_FOR_FABRIC_MAC_ERR,
  ARAD_FBR_LINK_INVALID_ERR,
  ARAD_SRD_AEQ_MODE_INVALID_ERR,
  ARAD_FAP_PORT_ID_INVALID_WHEN_FAT_PIPE_ENABLED_ERR,
  ARAD_SRD_PRBS_MODE_OUT_OF_RANGE_ERR,
  ARAD_DRAM_BUFFERS_UC_FBC_OVERFLOW_ERR,
  ARAD_DRAM_BUFFERS_FBC_OVERFLOW_ERR,
  ARAD_SCH_SLOW_RATE_INDEX_INVALID_ERR,
  ARAD_SCH_INTERNAL_SLOW_RATE_INDEX_INVALID_ERR,
  ARAD_IPQ_INVALID_QUEUE_ID_ERR,
  ARAD_NIF_SGMII_RATE_OUT_OF_RANGE_ERR,
  ARAD_FBR_LINK_ID_OUT_OF_RANGE_ERR,
  ARAD_FBR_LINK_ON_OFF_STATE_OUT_OF_RANGE_ERR,
  ARAD_NIF_1000BASE_X_INVALID_RATE_ERR,
  ARAD_CALLBACK_ALREADY_REGISTERED_ERR,
  ARAD_ITM_INGRESS_SHAPING_LOW_BELOW_HIGH_ERR,
  ARAD_BASE_Q_NOT_SET_ERR,
  ARAD_SSR_FORBIDDEN_FUNCTION_CALL_ERR,
  ARAD_SSR_INCOMPATIBLE_SRC_VERSION_ERR,
  ARAD_SW_DB_BUFF_SIZE_MISMATCH_ERR,
  ARAD_FC_INGR_GEN_GLB_HP_INVALID_ERR,
  ARAD_NIF_FC_RX_DISABLE_ERR,
  ARAD_SRD_INIT_TRIM_ERR,
  ARAD_NIF_SRD_LPBCK_MODE_INCONSISTENT_ERR,
  ARAD_INCOMPATABLE_NIF_ID_ERR,
  ARAD_PCKT_SIZE_VSC_BELOW_MIN_ERR,
  ARAD_PCKT_SIZE_FSC_BELOW_MIN_ERR,
  ARAD_PCKT_SIZE_VSC_ABOVE_MAX_ERR,
  ARAD_PCKT_SIZE_FSC_ABOVE_MAX_ERR,
  ARAD_PCKT_SIZE_MIN_EXCEEDS_MAX_ERR,
  ARAD_MIN_PCKT_SIZE_INCONSISTENT_ERR,
  ARAD_TEXT_NO_ERR_TXT_FOUND_ERR,
  ARAD_SRD_EARAD_ACCESS_ERR,
  ARAD_ECI_ACCESS_ERR,
  ARAD_SRD_LN_TRIM_DONE_IS_DOWN_ERR,
  ARAD_SRD_LN_CLCK_RLS_IS_DOWN_ERR,
  ARAD_SRD_LN_CLCK_RXTDACDONE_IS_DOWN_ERR,
  ARAD_OP_MODE_A1_OR_BELOW_LOCAL_MISMATCH_ERR,
  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_Q_ID_WITH_BASE_Q_MISMATCH_ERR,
  ARAD_MEM_CORRECTION_RM_BIT_INCONSISTENT_ERR,
  ARAD_MEM_CORRECTION_ERR,
  ARAD_MEM_BIST_ERR,
  ARAD_CELL_DIFFERENT_CELL_IDENT_ERR,
  ARAD_CELL_NO_RECEIVED_CELL_ERR,
  ARAD_CELL_WRITE_OUT_OF_BOUNDARY,
  ARAD_MGMT_PCKT_SIZE_CONF_MODE_OUT_OF_RANGE_ERR,
  ARAD_MGMT_PCKT_MAX_SIZE_INTERNAL_OUT_OF_RANGE_ERROR,
  ARAD_MGMT_PCKT_MAX_SIZE_EXTERNAL_OUT_OF_RANGE_ERROR,
  ARAD_MGMT_SMS_ACTION_TIMOUT_ERR,
  ARAD_SCH_SUB_FLOW_INVALID_ERR,
  ARAD_SRD_VALIDATE_AND_RELOCK_FAILED_ERR,
  ARAD_STAG_ENABLE_MODE_OUT_OF_RANGE_ERR,
  ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR,
  ARAD_SRD_IPU_IRQ_ACK_DOWN_ERR,
  ARAD_NIF_ON_OFF_STATE_OUT_OF_RANGE_ERR,
  ARAD_NIF_SGMII_0_OFF_INVALID_ERR,
  ARAD_SRD_NOT_FABRIC_QUARTET_ERR,
  ARAD_SRD_TX_EXPLCT_FAIL_ERR,
  ARAD_SRD_TX_ATTEN_FAIL_ERR,
  ARAD_SRD_TX_ATTEN_ABOVE_3_125_FAIL_ERR,
  ARAD_MDIO_OP_TIMEOUT_ERR,
  ARAD_LBG_PATTERN_DATA_SIZE_OUT_OF_RANGE_ERR,
  ARAD_ALLOC_TO_NON_NULL_ERR,
  ARAD_DRAM_CONF_MODE_OUT_OF_RANGE_ERR,
  ARAD_CR_WD_DEL_TH_OUT_OF_RANGE,
  ARAD_SCH_FLOW_AND_SE_TYPE_MISMATCH_ERR,
  ARAD_PORT_LAG_MEMBER_ID_OUT_OF_RANGE_ERR,
  ARAD_FEATURE_NOT_SUPPORTED_AT_REVISION_ERR,
  ARAD_ITM_CR_REQ_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_ITM_CR_REQ_TYPE_NDX_NOT_ALLOCATED_ERR,
  ARAD_ITM_CREDIT_CLS_OUT_OF_RANGE_ERR,
  ARAD_ITM_RATE_CLS_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_CONNECTION_CLS_OUT_OF_RANGE_ERR,
  ARAD_ITM_VSQ_TRAFFIC_CLS_OUT_OF_RANGE_ERR,
  ARAD_UNSCHED_PRIO_OUT_OF_RANGE_ERR,
  ARAD_EGR_MC_16K_ENABLE_INCOMPATIBLE_ERR,
  ARAD_INGR_RST_INTERN_BLOCK_INIT_ERR,
  ARAD_SCH_ILLEGAL_COMPOSITE_AGGREGATE_ERR,
  ARAD_SCH_COMPOSITE_AGGREGATE_DUAL_SHAPER_ERR,
  ARAD_OFP_RATES_CHAN_ARB_INVALID_ERR,
  ARAD_OFP_RATES_TBL_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_SCH_RATE_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_EGQ_RATE_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_BURST_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_PORTS_FROM_DIFFERENT_IFS_ERR,
  ARAD_OFP_RATES_CAL_LEN_INVALID_ERR,
  ARAD_OFP_RATES_SCH_CHAN_ARB_INVALID_ERR,
  ARAD_OFP_RATES_CAL_NO_SLOTS_AVAILABLE_ERR,
  ARAD_OFP_RATES_CAL_ALLOCATED_AND_REQUESTED_LEN_MISMATCH_ERR,
  ARAD_OFP_RATES_FAP_PORTS_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_SCH_PORT_ID_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_OFP_INDEX_MISMATCH_ERR,
  ARAD_OFP_RATES_NOF_ITERATIONS_EXCEEDS_LIMITS_ERR,
  ARAD_OFP_RATES_PORT_HAS_NO_IF_ERR,
  ARAD_OFP_RATES_CHAN_ARB_INVALID_NON_CHAN_ERR,
  ARAD_OFP_RATES_CONSECUTIVE_CREDIT_DISTRIBUTION_ERR,
  ARAD_OFP_RATES_ACTUAL_AND_EXPECTED_RATE_DIFFERENCE_ERR,
  ARAD_OFP_RATES_INVALID_PORT_ID_ERR,
  ARAD_OFP_RATES_PRIO_ID_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR,
  ARAD_OFP_RATES_TCG_NDX_OUT_OF_RANGE_ERR,
  ARAD_OFP_RATES_UNMAPPED_TCG_ERR,
  ARAD_NON_MESH_CONF_ERR,
  ARAD_FULL_PCKT_MODE_IN_MESH_CONF_ERR,
  ARAD_DRAM_INVALID_DRAM_TYPE_ERR,
  ARAD_FABRIC_CRC_NOT_SUPPORTED_ERR,
  ARAD_CELL_DATA_OUT_OF_RANGE_ERR,
  ARAD_CELL_FE_LOCATION_OUT_OF_RANGE_ERR,
  ARAD_CELL_PATH_LINKS_OUT_OF_RANGE_ERR,
  ARAD_MC_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_REG_ACCESS_NO_PA_ERR,
  ARAD_REG_ACCESS_NO_ARAD_ERR,
  ARAD_REG_ACCESS_UNKNOWN_DEVICE_ERR,
  ARAD_STAT_MULT_ID_UNSUPPORTED_ERR,
  ARAD_STAT_MODE_OUT_OF_RANGE_ERR,
  ARAD_SRD_LANE_AND_QRTT_ENABLE_MISMATCH_ERR,
  ARAD_NIF_MAL_SGMII_CONF_MISMATCH_ERR,
  ARAD_NIF_SPAUI_INTRLV_BCT_SIZE_ERR,
  ARAD_SRD_EYE_SCAN_CRC_NOT_FABRIC_ERR,
  ARAD_SRD_EYE_SCAN_FEC_NOT_CONFIGURED_ERR,
  ARAD_SRD_CNT_SRC_OUT_OF_RANGE_ERR,
  ARAD_SRD_TRAFFIC_SRC_OUT_OF_RANGE_ERR,
  ARAD_CORE_FREQ_OUT_OF_RANGE_ERR,
  ARAD_CR_WD_DELETE_BEFORE_STATUS_MSG_ERR,
  ARAD_CR_WD_DEL_TH_UNSUPPORTED_ERR,
  ARAD_RATE_CONF_MODE_INCONSISTENT_ERR,
  ARAD_FUNC_CALL_NO_PA_ERR,
  ARAD_FUNC_CALL_NO_ARAD_ERR,
  ARAD_FUNC_CALL_UNKNOWN_DEVICE_ERR,
  ARAD_SRD_REF_CLK_OF_RANGE_ERR,
  ARAD_NIF_SPAUI_DS_CONF_NON_DS_BUS_ERR,
  ARAD_NIF_SPAUI_DS_SOP_ODD_EVEN_ERR,
  ARAD_PORTS_NON_CH_IF_ERR,
  ARAD_DRAM_NOF_BANKS_OUT_OF_RANGE_ERR,
  ARAD_DRAM_NOF_COLS_OUT_OF_RANGE_ERR,
  ARAD_DRAM_DDR_TUNE_FAILED_ERR,
  ARAD_DRAM_DDR_TUNE_VALUES_RESTORED_ERR,
  ARAD_DRAM_CL_OUT_OF_RANGE_ERR,
  ARAD_DRAM_WL_OUT_OF_RANGE_ERR,
  ARAD_DRAM_WR_OUT_OF_RANGE_ERR,
  ARAD_DRAM_BL_OUT_OF_RANGE_ERR,
  ARAD_DRAM_RBUS_READ_ERR,
  ARAD_INTERRUPT_DEVICE_BETWEEN_ISR_TO_TCM_ERR,
  ARAD_INTERRUPT_INSUFFICIENT_MEMORY_ERR,
  ARAD_INTERRUPT_INVALID_MONITORED_CAUSE_ERR,
  ARAD_DRAM_NOF_CONF_MODE_OUT_OF_RANGE_ERR,
  ARAD_PORTS_EG_TM_PROFILE_FULL_ERR,
  ARAD_SRD_RATE_UNKNOWN_ERR,
  ARAD_DIAG_QDR_REPLY_LOCK_CANNOT_LOCK_ERR,
  ARAD_DIAG_QDR_DLL_NOT_READY_ERR,
  ARAD_DIAG_QDR_TRAINING_FAIL_ERR,
  ARAD_DIAG_QDR_CANNOT_ACCESS_ERR,
  ARAD_DEPRICATED_ERR,
  ARAD_DBG_PCM_COUNTER_NOT_EXPIRED_ERR,
  ARAD_END2END_SCHEDULER_MODE_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_COUNT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_PATTERN_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_SMS_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_ERR_SP_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_ERR_DP_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_ERR_RF_OUT_OF_RANGE_ERR,
  ARAD_INGR_SHP_Q_ABOVE_MAX_ERR,
  ARAD_DEBUG_RST_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_DBG_CFC_DB_CORRUPT_ERR,
  ARAD_MGMT_DRAM_INIT_RND_TRIP_FAILS_ERR,
  ARAD_API_NOT_FUNCTIONAL_ERR,
  ARAD_SRD_RATE_VCO_BELOW_MIN_ERR,
  ARAD_SRD_RATE_VCO_ABOVE_MAX_ERR,
  ARAD_DBG_STAT_IF_ENABLED_ERR,

  ARAD_GEN_NUM_CLEAR_ERR,
  ARAD_TBL_RANGE_OUT_OF_LIMIT,

  ARAD_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_CELL_SIZE_OUT_OF_RANGE_ERR,
  ARAD_IS_TDM_OUT_OF_RANGE_ERR,
  ARAD_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_SAMPLING_RATE_OUT_OF_RANGE_ERR,
  ARAD_CPQ_NDX_OUT_OF_RANGE_ERR,
  ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR,
  ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR,
  ARAD_QUEUE_NDX_OUT_OF_RANGE_ERR,
  ARAD_HDR_IN_COMPENSATION_OUT_OF_RANGE_ERR,
  ARAD_HDR_OUT_COMPENSATION_OUT_OF_RANGE_ERR,
  ARAD_ACTION_NDX_OUT_OF_RANGE_ERR,
  ARAD_D_DEST_TYPE_OUT_OF_RANGE_ERR,
  ARAD_D_DEST_VAL_OUT_OF_RANGE_ERR,
  ARAD_ITM_ENABLE_MODE_OUT_OF_RANGE_ERR,
  ARAD_ITM_OFFSET_4B_OUT_OF_RANGE_ERR,
  ARAD_ITM_GRNT_BYTES_OUT_OF_RANGE_ERR,
  ARAD_ITM_GRNT_BDS_OUT_OF_RANGE_ERR,
  ARAD_ITM_GRNT_OUT_OF_RESOURCE_ERR,
  ARAD_INGRESS_TRAFFIC_MGMT_RT_CLS4_NDX_OUT_OF_RANGE_ERR,
  ARAD_STAT_FAP20V_MODE_OUT_OF_RANGE_ERR,
  ARAD_STAT_BIILING_MODE_OUT_OF_RANGE_ERR,
  ARAD_STAT_REPORT_MODE_OUT_OF_RANGE_ERR,
  ARAD_STAT_FABRIC_MC_OUT_OF_RANGE_ERR,
  ARAD_STAT_ING_REP_MC_OUT_OF_RANGE_ERR,
  ARAD_STAT_CNM_REPORT_OUT_OF_RANGE_ERR,
  ARAD_STAT_PARITY_MODE_OUT_OF_RANGE_ERR,
  ARAD_STAT_WORD1_OUT_OF_RANGE_ERR,
  ARAD_STAT_WORD1_INVALID_ERR,
  ARAD_STAT_WORD2_INVALID_ERR,
  ARAD_STAT_WORD2_OUT_OF_RANGE_ERR,
  ARAD_STAT_EN_MODE_OUT_OF_RANGE_ERR,
  ARAD_STAT_NOF_DUPLICATIONS_OUT_OF_RANGE_ERR,
  ARAD_STAT_NOF_IDLE_CLOCKS_OUT_OF_RANGE_ERR,
  ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_TDM_CELL_SIZE_OUT_OF_RANGE_ERR,
  ARAD_TDM_IS_TDM_OUT_OF_RANGE_ERR,
  ARAD_TDM_DEST_IF_OUT_OF_RANGE_ERR,
  ARAD_TDM_DEST_FAP_ID_OUT_OF_RANGE_ERR,
  ARAD_TDM_MC_ID_OUT_OF_RANGE_ERR,
  ARAD_TDM_MC_USER_DEF_OUT_OF_RANGE_ERR,
  ARAD_TDM_DEST_FAP_PORT_OUT_OF_RANGE_ERR,
  ARAD_TDM_USER_DEFINE_2_OUT_OF_RANGE_ERR,
  ARAD_TDM_ACTION_ING_OUT_OF_RANGE_ERR,
  ARAD_TDM_ACTION_EG_OUT_OF_RANGE_ERR,
  ARAD_TDM_INVALID_TDM_MODE_ERR,
  ARAD_TDM_API_INVALID_PETRAB_IN_SYSTEM_ERR,
  ARAD_TDM_CUSTOMER_EMBED_IN_OPTIMIZED_MODE_ERR,
  ARAD_TDM_OUT_OF_AVAIABLE_TDM_CONTEXT_ERR,
  ARAD_CNM_Q_SET_OUT_OF_RANGE_ERR,
  ARAD_CNM_ING_VLAN_EDIT_CMD_OUT_OF_RANGE_ERR,
  ARAD_CNM_VERSION_OUT_OF_RANGE_ERR,
  ARAD_CNM_RES_V_OUT_OF_RANGE_ERR,
  ARAD_CNM_TC_OUT_OF_RANGE_ERR,
  ARAD_CNM_CP_ID_4_MSB_OUT_OF_RANGE_ERR,
  ARAD_CNM_PKT_GEN_MODE_OUT_OF_RANGE_ERR,
  ARAD_CNM_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_CNT_START_Q_OUT_OF_RANGE_ERR,
  ARAD_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR,
  ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR,
  ARAD_CNT_MODE_STATISTICS_OUT_OF_RANGE_ERR,
  ARAD_CNT_MODE_EG_OUT_OF_RANGE_ERR,
  ARAD_CNT_STAG_LSB_OUT_OF_RANGE_ERR,
  ARAD_CNT_CACHE_LENGTH_OUT_OF_RANGE_ERR,
  ARAD_CNT_NOF_COUNTERS_OUT_OF_RANGE_ERR,
  ARAD_ACTION_SNOOP_SIZE_OUT_OF_RANGE_ERR,
  ARAD_ACTION_SNOOP_PROB_OUT_OF_RANGE_ERR,
  ARAD_ACTION_MIRROR_PROB_OUT_OF_RANGE_ERR,
  ARAD_WC_NDX_OUT_OF_RANGE_ERR,
  ARAD_PCKT_SIZE_OUT_OF_RANGE_ERR,
  ARAD_RATE_OUT_OF_RANGE_ERR,
  ARAD_ILKN_NDX_OUT_OF_RANGE_ERR,
  ARAD_CLK_NDX_OUT_OF_RANGE_ERR,
  ARAD_MODE_OUT_OF_RANGE_ERR,
  ARAD_FATP_NDX_OUT_OF_RANGE_ERR,
  ARAD_HW_MAL_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_HW_SPAUI_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_HW_GMII_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_HW_ILKN_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_HW_FATP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_INIT_CREDIT_WORTH_OUT_OF_RANGE_ERR,
  ARAD_INIT_EXTENDED_Q_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_NIF_TYPE_OUT_OF_RANGE_ERR,
  ARAD_NIF_SIZE_OUT_OF_RANGE_ERR,
  ARAD_NIF_DIC_MODE_OUT_OF_RANGE_ERR,
  ARAD_NIF_BCT_CHANNEL_BYTE_NDX_OUT_OF_RANGE_ERR,
  ARAD_NIF_CRC_MODE_OUT_OF_RANGE_ERR,
  ARAD_NIF_FAULT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_NIF_CLK_DIVIDER_OUT_OF_RANGE_ERR,
  ARAD_FC_INHERIT_OUT_OF_RANGE_ERR,
  ARAD_FC_VSQ_TRGR_OUT_OF_RANGE_ERR,
  ARAD_FC_REACT_POINT_OUT_OF_RANGE_ERR,
  ARAD_FC_PRIO_OUT_OF_RANGE_ERR,
  ARAD_FC_HANDLER_OUT_OF_RANGE_ERR,
  ARAD_FC_PRIORITY_OUT_OF_RANGE_ERR,
  ARAD_FC_OFP_ID_OUT_OF_RANGE_ERR,
  ARAD_FC_CAL_LEN_OUT_OF_RANGE_ERR,
  ARAD_FC_CAL_REPS_OUT_OF_RANGE_ERR,
  ARAD_FC_SOURCE_OUT_OF_RANGE_ERR,
  ARAD_FC_DESTINATION_OUT_OF_RANGE_ERR,
  ARAD_FC_CAL_CHANNEL_OUT_OF_RANGE_ERR,
  ARAD_FC_NOF_OFP_PRIORITIES_OUT_OF_RANGE_ERR,
  ARAD_FC_MODE_OUT_OF_RANGE_ERR,
  ARAD_FC_TRGR_NDX_OUT_OF_RANGE_ERR,
  ARAD_CPID_TC_NDX_OUT_OF_RANGE_ERR,
  ARAD_FC_SCH_OOB_RANGE_INVALID_ERR,
  ARAD_FC_SCH_OOB_WD_PERIOD_INVALID_ERR,
  ARAD_FC_CLASS_OUT_OF_RANGE_ERR,
  ARAD_FC_CAL_MODE_NDX_OUT_OF_RANGE_ERR,
  ARAD_FC_CAL_IF_NDX_OUT_OF_RANGE_ERR,
  ARAD_FC_OOB_NDX_OUT_OF_RANGE_ERR,
  ARAD_FC_NIF_OVRS_SCHEME_OUT_OF_RANGE_ERR,
  ARAD_FC_DIRECTION_NDX_OUT_OF_RANGE_ERR,
  ARAD_FC_NIF_PAUSE_QUANTA_OUT_OF_RANGE_ERR,
  ARAD_NIF_COUNTER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_CODING_OUT_OF_RANGE_ERR,
  ARAD_FBR_NOF_LINKS_OUT_OF_RANGE_ERR,
  ARAD_FABRIC_SHAPER_BYTES_OUT_OF_RANGE_ERR,
  ARAD_FABRIC_SHAPER_CELLS_OUT_OF_RANGE_ERR,
  ARAD_INIT_HDR_TYPE_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_INIT_IF_MAP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_INIT_PACKET_HDR_INFO_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_HW_MAL_NDX_OUT_OF_RANGE_ERR,
  ARAD_TC_NDX_OUT_OF_RANGE_ERR,
  ARAD_DP_NDX_OUT_OF_RANGE_ERR,
  ARAD_MAP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_MAP_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_TC_OUT_OF_RANGE_ERR,
  ARAD_EGR_DP_OUT_OF_RANGE_ERR,
  ARAD_EGR_BASE_Q_PAIR_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_PRIORITY_OUT_OF_RANGE_ERR,    
  ARAD_MGMT_DEVICE_REVISION_REV_A1_OR_BELOW_ERR,
  ARAD_TDM_LINK_BITMAP_OUT_OF_RANGE_ERR,
  ARAD_CELL_VARIABLE_IN_FAP20_21_SYSTEM_ERR,
  ARAD_INIT_QDR_TYPE_ILLEGAL_ERR,
  ARAD_MGMT_TDM_MODE_OF_RANGE_ERR,
  ARAD_MGMT_FTMH_LB_EXT_MODE_OF_RANGE_ERR,
  ARAD_NIF_OVERSUBSCR_SCHEME_OUT_OF_RANGE_ERR,
  ARAD_INIT_OOR_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_CELL_RECEIVED_CELL_SIZE_ERR,
  
  ARAD_ILKN_INVALID_LANE_FOR_MIN_SIZE_ERR,
  ARAD_ILKN_INVALID_LANE_SPECIFIED_BUT_INACTIVE_ERR,
    ARAD_ILKN_NOF_LANES_OUT_OF_RANGE_ERR,
    ARAD_ILKN_RETRANSMIT_BUFFER_SIZE_ENTRIES_OUT_OF_RANGE_ERR,
    ARAD_ILKN_RETRANSMIT_NOF_SEQ_NUMBER_REPETITIONS_OUT_OF_RANGE_ERR,
    ARAD_ILKN_RETRANSMIT_NOF_REQUESTS_RESENT_OUT_OF_RANGE_ERR,
    ARAD_ILKN_RETRANSMIT_TIMEOUT_OUT_OF_RANGE_ERR,
  ARAD_ILKN_METAFRAME_SYNC_PERIOD_OUT_OF_RANGE_ERR,
  ARAD_UNKNOWN_NIF_TYPE_ERR,
    ARAD_NO_NIF_STAT_IF_FOUND_ERR,
    ARAD_NIF_WC_ALREADY_CONFIGURED_ERR,
    ARAD_NIF_ILKN_NOF_ENTRIES_OUT_OF_RANGE_ERR,
    ARAD_NIF_2ND_INTERFACE_IS_B_ERR,
    ARAD_NIF_NOF_LANES_OUT_OF_RANGE_ERR,
  ARAD_NIF_INVALID_ID_ERR,
  ARAD_NIF_CHAN_INTERFACE_OUT_OF_RANGE_ERR,
  ARAD_NIF_SYNCE_INVALID_CLK_ID_FOR_TWO_DIFF_CLK_ERR,
  ARAD_NIF_SYNCE_CLK_ID_AND_SRC_MISMATCH_ERR,
  ARAD_FATP_MODE_OUT_OF_RANGE_ERR,
  ARAD_NIF_CNT_RD_INVALID_ERR,
  ARAD_NIF_PREAMBLE_COMPRESS_SIZE_OUT_OF_RANGE_ERR,
  ARAD_NIF_FR_LOCAL_OUT_OF_RANGE_ERR,
  ARAD_NIF_FR_REMOTE_OUT_OF_RANGE_ERR,
  ARAD_NIF_GMII_INFO_MODE_OUT_OF_RANGE_ERR,
  ARAD_NIF_GMII_INFO_RATE_OUT_OF_RANGE_ERR,
  ARAD_NIF_GMII_STAT_FAULT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_NIF_CLK_FREQ_KHZ_OUT_OF_RANGE_ERR,
  ARAD_NIF_SYNC_RESET_VAL_MSEC_OUT_OF_RANGE_ERR,
  ARAD_NIF_SYNC_AUTOINC_INTERVAL_MSEC_OUT_OF_RANGE_ERR,
  ARAD_NIF_WIRE_DELAY_NS_OUT_OF_RANGE_ERR,
  ARAD_NIF_SYNCE_MODE_OUT_OF_RANGE_ERR,
  ARAD_NIF_FATP_BASE_PORT_ID_OUT_OF_RANGE_ERR,
  ARAD_NIF_FATP_NOF_PORTS_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_FEM_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_OFFSET_FEM_INVALID_ERR,
  ARAD_PMF_LOW_LEVEL_CE_INIT_UNSAFE,
  ARAD_PMF_CE_NOF_REAL_BITS_COMPUTE_HEADER,
  ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_IRPP_FIELD_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_IS_CE_NOT_VALID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DB_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_KEY_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_KEY_SRC_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_ENTRY_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_FEM_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_TAG_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_TAG_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_FEM_PGM_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SELECTED_BITS_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_INCORRECT_INSTRUCTION_ERR,
  ARAD_PMF_LOW_LEVEL_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_NOF_BITS_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DB_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_ENTRY_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PRIORITY_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_CYCLE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_VAL_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PGM_COUNTER_PARSE,
  ARAD_PMF_LOW_LEVEL_PGM_ID_OUT_OF_RANGE_ERR,
    ARAD_PMF_FES_SHIFT_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SRC_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_VAL_SRC_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_STAT_TAG_LSB_POSITION_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_MODE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_IPV4_FLD_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_IPV6_FLD_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_LOC_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_IRPP_FLD_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SUB_HEADER_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PMF_PGM_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_L2_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SEL_BIT_MSB_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_ACTION_FOMAT_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_MAP_DATA_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_BASE_VALUE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PFQ_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SEM_13_8_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_FWD_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_TTC_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PRSR_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PORT_PMF_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_LLVP_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PMF_PRO_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_EEI_OUTLIF_15_8_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_EEI_OUTLIF_7_0_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_SEM_7_0_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_HEADER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_NOF_BYTES_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_LKP_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_TAG_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_FC_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_COPY_PGM_VAR_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_BIT_LOC_VAL_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_AF_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PGM_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PGM_NOT_ENOUGH_ERR,
  ARAD_PMF_FEM_INVALID_FOR_OFFSET_ERR,
  ARAD_PMF_FEM_OUTPUT_SIZE_ERR,
  ARAD_PMF_FEM_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_PORT_HEADER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIRECT_TBL_VAL_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_TCAM_KEY_SRC_OUT_OF_RANGE_ERR,
  ARAD_PMF_TCAM_NOF_BANKS_VALID_MAX_ERR,
  ARAD_PMF_CE_FLD_NOT_FOUND_ERR,
  ARAD_TCAM_KEY_FLD_VAL_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_PROG_ID_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_ENABLE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_BUFF_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_BUFF_LEN_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_VAL_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_VALUE_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_STRENGTH_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_PASS_NUM_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_PRG_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_IN_KEY_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_IN_PRG_OUT_OF_RANGE_ERR,
  ARAD_PMF_LOW_LEVEL_DIAG_INVALID_ACTS_COMBINATION_ERR,
  ARAD_PP_TBLS_NOT_INITIALIZED,
  ARAD_PP_MAP_TBL_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_TBL_RANGE_OUT_OF_LIMIT,
  ARAD_TCAM_BANK_UNINITIALIZED_ERR,
  ARAD_TCAM_DB_DOESNT_EXIST_ERR,
  ARAD_TCAM_DB_BANK_NOT_USED_ERR,
  ARAD_TCAM_DB_BANK_ALREADY_USED_ERR,
  ARAD_TCAM_DB_ENTRY_SIZE_MISMATCH_ERR,
  ARAD_TCAM_DB_BANK_OWNER_MISMATCH_ERR,
  ARAD_TCAM_DB_ENTRY_NDX_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ACCESS_PROFILE_ALREADY_EXISTS_ERR,
  ARAD_TCAM_BANK_NDX_OUT_OF_RANGE_ERR,
  ARAD_TCAM_BANK_ENTRY_SIZE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_FWDING_USER_CYCLE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_DATABASE_ALREADY_EXISTS_ERR,
  ARAD_TCAM_CYCLE_NDX_OUT_OF_RANGE_ERR,
  ARAD_TCAM_TCAM_USER_OUT_OF_RANGE_ERR,
  ARAD_TCAM_TCAM_USER_NDX_OUT_OF_RANGE_ERR,
  ARAD_TCAM_DB_ID_NDX_OUT_OF_RANGE_ERR,
  ARAD_TCAM_VAL_OUT_OF_RANGE_ERR,
  ARAD_TCAM_NOF_BITS_OUT_OF_RANGE_ERR,
  ARAD_TCAM_PREFIX_LENGTH_OUT_OF_RANGE_ERR,
  ARAD_TCAM_USE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ACL_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ACL_ACE_TYPE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ACCESS_PROFILE_DOESNT_EXIST_ERR,
  ARAD_PORTS_FC_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PORTS_FIRST_HEADER_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PORTS_HEADER_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PP_PORT_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PROCESSOR_ID_OUT_OF_RANGE_ERR,
  ARAD_PORTS_ID_OUT_OF_RANGE_ERR,
  ARAD_PORTS_TR_CLS_OUT_OF_RANGE_ERR,
  ARAD_PORTS_DP_OUT_OF_RANGE_ERR,
  ARAD_PORTS_SNOOP_CMD_NDX_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PROFILE_IS_HEADER_TYPE_INCONSISTENT_ERR,
  ARAD_PORTS_NOT_TM_PORT_IS_PPH_PRESENT_ERR,
  ARAD_PORTS_ETH_PORT_MIRROR_PROFILE_ERR,
  ARAD_PORTS_NOT_TM_PORT_ERR,
  ARAD_PORTS_NOT_PP_PORT_FOR_RAW_PORTS_ERR,
  ARAD_PORT_TRAP_CODE_NOT_SUPPORTED_ERR,
  ARAD_PORTS_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PORTS_NIF_CTXT_MAP_OUT_OF_RANGE_ERR,
  ARAD_PORTS_HDR_FORMAT_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PP_PORT_FOR_TM_TRAFFIC_OUT_OF_RANGE_ERR,
  ARAD_PORTS_INVALID_CTXT_MAPPING_ERR,
  ARAD_PORT_SYMMETRIC_ASSUMPTION_ERR,
  ARAD_PP_TBL_ACCESS_UNKNOWN_MODULE_ID_ERR,
  ARAD_PORTS_NON_ZERO_CHANNEL_FOR_UNCHANNELIZED_IF_TYPE_ERR,
  ARAD_INIT_TM_PROFILE_OUT_OF_RSRC_ERR,
  ARAD_DIAG_LBG_TM_PROFILE_OUT_OF_RSRC_ERR,
  ARAD_DIAG_SAMPLE_NOT_ENABLED_ERR,
  ARAD_MGMT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_MGMT_PP_PORT_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_MGMT_TM_PROFILE_MAP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  ARAD_PORTS_OTMH_SRC_DEST_NOT_EQUAL_ERR,
  ARAD_EGR_PROG_PP_PORT_OUT_OF_RANGE_ERR,
  ARAD_NIF_SRD_LANES_COMP_ERR,
  ARAD_IQM_PRFSEL_TBL_INDEX_OUT_OF_RANGE_ERR,
  ARAD_IQM_PRFCFG_TBL_INDEX_OUT_OF_RANGE_ERR,
  ARAD_DIAG_LBG_PATH_NOF_PORTS_OUT_OF_RANGE_ERR,
  ARAD_CNT_DEFINED_LSB_STAG_ERR,
  ARAD_CNT_EG_MODE_AND_PROC_A,
  ARAD_CNT_EG_MODE_OUT_OF_RANGE_ERR,
  ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR,
  ARAD_CNT_VOQ_PARAMS_OUT_OF_RANGE_ERR,
  ARAD_CNT_START_FIFO_DMA_ERR,
  ARAD_FC_FLOW_CONTROL_NOT_SUPPORTED_ON_IF_ERR,
  ARAD_FC_TC_NOT_SUPPORTED_ON_IF_ERR,
  ARAD_NIF_FATP_DISABLED_BUT_SET_ERR,
  ARAD_CNT_DIRECT_READ_OUT_OF_TIME_ERR,
  ARAD_CNT_PROC_SRC_TYPE_NOT_VOQ_ERR,
  ARAD_ACTION_CMD_DEST_ID_OUT_OF_RANGE_ERR,
  ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR,
  ARAD_MULT_EG_TABLE_FORMAT_INVALID_ERR,
  ARAD_MDIO_CLK_FREQ_ERR,
  ARAD_NIF_SYNCE_MALGB_CLK_WHEN_DISABLED_ERR,
  ARAD_SW_DB_MULTI_SET_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_L2_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_IPV4_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_MPLS_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_TM_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_CODE_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_PROTOCOL_CODE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_PP_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_FWD_TYPE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_ACL_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_KEY_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_KEY_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_DB_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_DB_TYPE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_ENTRY_NDX_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_IS_FOUND_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_VAL_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_ACL_DATA_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_TRAP_CODE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_OFP_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_TC_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_DP_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_CUD_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_PRIORITY_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_DB_IDS_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_TYPE_OUT_OF_RANGE_ERR,
  ARAD_EGR_ACL_TYPE_DIFFERENT_FROM_DB_ERR,
  ARAD_TCAM_KEY_FORMAT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_NIF_API_NOT_APPLICABLE_FOR_ILKN_ERR,
  ARAD_FC_PFC_GENERIC_BITMAP_OUT_OF_RANGE,
  ARAD_FC_PFC_GENERIC_BITMAP_BIT_OUT_OF_RANGE,
  ARAD_FC_INVALID_CALENDAR_ENTRY_ERR,
  ARAD_DRAM_PLL_F_OUT_OF_RANGE_ERR,
  ARAD_DRAM_PLL_Q_OUT_OF_RANGE_ERR,
  ARAD_DRAM_PLL_R_OUT_OF_RANGE_ERR,
  ARAD_CNM_CP_MULTIPLE_GEN_MODE,
  ARAD_DRAM_PLL_CONF_NOT_SET_ERR,
  ARAD_DRAM_BIST_INVALID_TEST_PARAMETERS_ERR,
  ARAD_TCAM_BANK_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ENTRY_SIZE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ACTION_SIZE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_TCAM_DB_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_PREFIX_SIZE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_PRIO_MODE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ENTRY_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_PRIORITY_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ACTION_OUT_OF_RANGE_ERR,
  ARAD_TCAM_BITS_OUT_OF_RANGE_ERR,
  ARAD_TCAM_LENGTH_OUT_OF_RANGE_ERR,
  ARAD_TCAM_ENTRY_OUT_OF_RANGE_ERR,
  ARAD_TCAM_DB_METADATA_INCONSISTENT_ERR,
  ARAD_TCAM_DB_ENTRY_SEARCH_SIZE_NOT_SUPPORTED_ERR,
  ARAD_TCAM_MGMT_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_CYCLE_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_UNIFORM_PREFIX_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_MIN_BANKS_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_USER_DATA_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_BANK_OWNER_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_ENTRY_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_PRIORITY_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_ACTION_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_BANK_ID_OUT_OF_RANGE_ERR,
  ARAD_TCAM_MGMT_ACCESS_DEVICE_OUT_OF_RANGE_ERR,
  ARAD_STACK_LOCAL_STACK_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_STACK_SYS_PHY_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_STACK_SYS_FAP_ID_OUT_OF_RANGE_ERR,
  ARAD_STACK_STACK_PORT_LOCAL_ID_OUT_OF_RANGE_ERR,
  ARAD_STACK_MAX_NOF_TM_DOMAINS_OUT_OF_RANGE_ERR,
  ARAD_STACK_MY_TM_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_STACK_PEER_TM_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_FLOW_CONTROL_TRGR_NDX_OUT_OF_RANGE_ERR,
  ARAD_FLOW_CONTROL_REACT_POINT_NDX_OUT_OF_RANGE_ERR,
  ARAD_FLOW_CONTROL_VSQ_BITMAP_NOT_VALID_ERR,
  ARAD_FLOW_CONTROL_OFP_IS_NOT_DEFINED_AS_RCY_ERR,
  ARAD_FLOW_CONTROL_PRIO_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_PGM_MGMT_PGM_SOURCE_OUT_OF_RANGE_ERR,
  ARAD_PMF_PGM_MGMT_PMF_PGM_NDX_OUT_OF_RANGE_ERR,
  ARAD_PMF_PGM_MGMT_PFG_OUT_OF_RANGE_ERR,
  ARAD_PMF_PGM_MGMT_RESTORE_ERR,
  ARAD_NIF_SYNCE_CLK_SQUELTCH_ON_DIV_20_ERR,
  ARAD_MGMT_TDM_MC_ROUTE_MODE_OUT_OF_RANGE_ERR,  
  ARAD_MGMT_OCB_MC_RANGE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_MGMT_OCB_MC_RANGE_MIN_OUT_OF_RANGE_ERR,
  ARAD_MGMT_OCB_MC_RANGE_MAX_OUT_OF_RANGE_ERR,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_QUEUE_CATEGORY_INDEX_OUT_OF_RANGE_ERR,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_RATE_CLASS_INDEX_OUT_OF_RANGE_ERR,
  ARAD_MGMT_OCB_VOQ_ELIGIBLE_TRAFFIC_CLASS_INDEX_OUT_OF_RANGE_ERR,
  ARAD_FABRIC_MULT_INGRESS_MULT_NOT_SINGLE_COPY_ERR,
  ARAD_TDM_MC_ID_ROUTE_NDX_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_TM_PORT_OUT_OF_RANGE_ERR,
  ARAD_DIAGNOSTICS_PP_PORT_OUT_OF_RANGE_ERR,
  ARAD_INCONSISTENCY_ERR,
  ARAD_WB_DB_UNSUPPORTED_SCACHE_DATA_VERSION_ERR,
  ARAD_STAT_IF_REPORT_PKT_SIZE_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MIN_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_MAX_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_SCRUBBER_QUEUE_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_SCRUBBER_RATE_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_DESC_THRESHOLD_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_BDB_THRESHOLD_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_REPORT_UC_DRAM_THRESHOLD_OUT_OF_RANGE_ERR,
  ARAD_STAT_IF_INIT_FAILED_ERR,
  ARAD_STAT_RATE_NOT_FOUND_ERR,
  ARAD_FABRIC_PRIORITY_OUT_OF_RANGE,
  ARAD_EGR_PROG_EDITOR_OUT_OF_PROGRAMS_ERR,
  ARAD_EGR_PROG_EDITOR_OUT_OF_EVEN_LFEMS_ERR,
  ARAD_EGR_PROG_EDITOR_OUT_OF_ODD_LFEMS_ERR,
  ARAD_EGR_PROG_EDITOR_OUT_OF_INSTRUCTION_ENTRIES_ERR,
  ARAD_EGR_PROG_EDITOR_OUT_OF_TM_PROFILES_ERR,
  ARAD_EGR_PROG_EDITOR_OUT_OF_SELECTION_CAM_LINES,
  ARAD_EGR_PROG_EDITOR_PROGRAM_NOT_USED_ERR,
  ARAD_PORTS_SWAP_OFFSET_OUT_OF_RANGE_ERR,
  ARAD_PORTS_SWAP_MODE_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PON_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PON_TUNNEL_ID_OUT_OF_RANGE_ERR,
  ARAD_PORTS_PON_IN_PP_OUT_OF_RANGE_ERR,
  ARAD_INTERNAL_ASSERT_ERR,
  ARAD_ITM_DP_DISCARD_OUT_OF_RANGE_ERR,
  ARAD_FABRIC_GCI_CONFIG_TYPE_OUT_OF_RANGE,
  ARAD_FABRIC_MIXED_CONFIGURATION_ERR,
  ARAD_NIF_API_NOT_APLICABLE_FOR_NON_PFC_PORT,
  ARAD_PORTS_RX_DISABLE_TDM_MIN_SIZE_ILLEGAL,
  ARAD_UNSUPPORTED_FC_TYPE_ERR,
  ARAD_PP_EG_ENCAP_LSB_MUST_BE_0,
  
  ARAD_LAST_ERR
} ARAD_ERR;








#define ARAD_FLD_SIZE_BITS(msb, lsb) (msb - lsb + 1)

















uint32
  arad_add_arad_errors(void);


uint32
  arad_add_arad_procedure_desc(void);




uint32
  arad_get_err_text(
    uint32  err_id,
    char           **err_name,
    char           **err_text
  ) ;

uint32
  arad_proc_id_to_string(
    uint32 in_proc_id,
    char          **out_module_name,
    char          **out_proc_name
  ) ;


CONST SOC_ERROR_DESC_ELEMENT
  *arad_errors_ptr_get(
    void
  );



uint32
  arad_errors_add(void);


uint32
  arad_procedure_desc_add(void);

uint32
  arad_err_text_get(
    SOC_SAND_IN  uint32  err_id,
    SOC_SAND_OUT char      **err_name,
    SOC_SAND_OUT char      **err_text
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



