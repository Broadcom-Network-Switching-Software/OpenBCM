/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_PURE_DEFI_PSS_INCLUDED
/* { */
#define UI_PURE_DEFI_PSS_INCLUDED
/*
 * Note:
 * the following definitions must range between PARAM_PSS_START_RANGE_ID (600)
 * and PARAM_PSS_END_RANGE_ID (799).
 * See ui_pure_defi.h
 */
#define PARAM_PSS_MODULE_OPEN                        600
#define PARAM_PSS_SEND_CMD_ID                        601
#define PARAM_PSS_CMD_FIELDS_ID                      602
#define PARAM_PSS_MODULE_CLOSE                       603
#define PARAM_PSS_VLAN_ID                            604
#define PARAM_PSS_VLAN_SELECT_LEARNING_MODE_ID       605
#define PARAM_PSS_CREATE_NEW_ID                      606
#define PARAM_PSS_ADD_PORT_ID                        607
#define PARAM_PSS_VLAN_ID_ID                         608
#define PARAM_PSS_STANDARD_PORT_TYPE_ID              609
#define PARAM_PSS_TRUNK_PORT_TYPE_ID                 610
#define PARAM_PSS_ID_OF_TRUNK_PORT_ID                611
#define PARAM_PSS_VLAN_PORT_TAGGED_ID                612
#define PARAM_PSS_VLAN_CPU_INCLUDED_ID               613
#define PARAM_PSS_UNIT_ID                            614
#define PARAM_PSS_DEVICE_ID                          615
#define PARAM_PSS_PORT_ID                            616
#define PARAM_PSS_MULTICAST_ID                       617
#define PARAM_PSS_MAC_ADDR_ID_ID                     618
#define PARAM_PSS_USER_TX_TABLE_ID                   619
#define PARAM_PSS_GET_ID                             620
#define PARAM_PSS_INDEX_ID                           621
#define PARAM_PSS_ADD_ID                             622
#define PARAM_PSS_ENTRY_ID_ID                        624
#define PARAM_PSS_USER_PRIORITY_ID                   625
#define PARAM_PSS_DROP_PRECEDENCE_ID                 626
#define PARAM_PSS_TRAFFIC_CLASS_ID                   627
#define PARAM_PSS_ENCAPSULATION_ID                   628
#define PARAM_PSS_TX_BY_VLAN_ID                      629
#define PARAM_PSS_TX_BY_IF_ID                        630
#define PARAM_PSS_TX_BY_LPORT_ID                     631
#define PARAM_PSS_APPEND_CRC_ID                      632
#define PARAM_PSS_TAGGED_ID                          633
#define PARAM_PSS_PCKTS_NUM_ID                       634
#define PARAM_PSS_PCKT_DATA_LEN_ID                   635
#define PARAM_PSS_GAP_BET_PCKTS_ID                   636
#define PARAM_PSS_WAIT_BET_GROUPS_ID                 637
#define PARAM_PSS_PCKT_DATA_ID                       638
#define PARAM_PSS_EXCLUDE_NO_PORT_ID                 639
#define PARAM_PSS_EXCLUDE_ONE_PORT_ID                640
#define PARAM_PSS_LOGICAL_IF_INDEX_ID                641
#define PARAM_PSS_BRIDGE_ID                          642
#define PARAM_PSS_BRIDGE_GEN_CMNDS_ID                643
#define PARAM_PSS_SOURCE_MAC_ID                      644
#define PARAM_PSS_DEST_MAC_ID                        645
#define PARAM_PSS_SET_TX_MODE_ID                     646
#define PARAM_PSS_NUM_TX_CYCLES_ID                   647
#define PARAM_PSS_TOT_NUM_PACKETS_ID                 648
#define PARAM_PSS_GAP_BETWEEN_CYCLES_ID              649
#define PARAM_PSS_TX_ACTION_ID                       650
#define PARAM_PSS_USER_RX_TABLE_ID                   652
#define PARAM_PSS_RX_ACTION_ID                       653
#define PARAM_PSS_GET_RX_MODE_ID                     654
#define PARAM_PSS_SET_RX_MODE_ID                     656
#define PARAM_PSS_BUFFER_MODE_ID                     657
#define PARAM_PSS_BUFF_SIZE_ID                       658
#define PARAM_PSS_NUM_ENTRIES_ID                     659
#define PARAM_PSS_GET_FIRST_RX_ID                    660
#define PARAM_PSS_GET_NEXT_RX_ID                     661
#define PARAM_PSS_CLEAR_RX_ID                        662
#define PARAM_PSS_TEST_ID                            663
#define PARAM_PSS_TEST_SEND_BUFF_TO_CORE_ID          664
#define PARAM_PSS_DESTINATION_PP_ID_ID               665
#define PARAM_PSS_DESTINATION_NOT_PP_ID_ID           666
#define PARAM_PSS_TEST_BUFF_SIZE_ID                  667
#define PARAM_PSS_DATA_ID                            668
#define PARAM_PSS_TEST_SEND_BUFF_TO_TAPI_ID          669
#define PARAM_PSS_TEST_SEND_BUFF_TO_CORE_DEFERRED_ID 670
#define PARAM_PSS_DIAGNOSTICS_ID                     671
#define PARAM_PSS_DIAGNOSTICS_TESTMEM_ID             672
#define PARAM_PSS_DIAGNOSTICS_MEMTYPE_ID             673
#define PARAM_PSS_DIAGNOSTICS_OFFEST_ID              674
#define PARAM_PSS_DIAGNOSTICS_SIZE_ID                675
#define PARAM_PSS_DIAGNOSTICS_TESTPROFILE_ID         676
#define PARAM_PSS_DIAGNOSTICS_TESTREG_ID             677
#define PARAM_PSS_DIAGNOSTICS_DUMPREG_ID             678
#define PARAM_PSS_DIAGNOSTICS_TESTMACUPDMSG_ID       679
#define PARAM_PSS_TEST_DMA_STATUS_ID                 680
#define PARAM_PSS_PRINT_TABLE_ID                     681
#define PARAM_PSS_PHY_ID                             683
#define PARAM_PSS_READ_ID                            684
#define PARAM_PSS_WRITE_ID                           685
#define PARAM_PSS_PORT_UTILS_ID                      686
#define PARAM_PSS_PHY_DATA_ID                        687
#define PARAM_PSS_PHY_DATA_MASK_ID                   688
#define PARAM_PSS_PORT_CTRL_ID                       690
#define PARAM_PSS_PORT_FORCE_LINK_PASS_ID            691
#define PARAM_PSS_PORT_LOOPBACK_ID                   692
#define PARAM_PSS_PORT_STATUS_ID                     693
#define PARAM_PSS_ADD_MAC_ENTRY_ID                   694
#define PARAM_PSS_LEARN_ENABLE_ID                    695
#define PARAM_PSS_SET_DEFAULT_VLAN_ID                696
#define PARAM_PSS_SRC_TRAFFIC_CLASS_ID               697
#define PARAM_PSS_DST_TRAFFIC_CLASS_ID               698
#define PARAM_PSS_DEL_MAC_ENTRY_ID                   699
#define PARAM_PSS_UNKNOWN_CMD_ID                     700
#define PARAM_PSS_DISP_ALL_MAC_COUNTERS_ID           701
#define PARAM_PSS_MAC_COUNTERS_DISP_FORMAT_ID        702
#define PARAM_PSS_CLEAR_AFTER_READ_ID                703
#define PARAM_PSS_NOF_CONSECUTIVE_MACS_ID            704
#define PARAM_PSS_INCREMENT_DEVICE_ID_EVERY_N_ID     705


typedef enum
{
  FDB_VLAN_TBL_EQUIVALENT = 0,
  MAC_ADDR_TBL_EQUIVALENT
} PSS_BRIDGE_TABLE;


/* } */
#endif /*UI_PURE_DEFI_PSS_INCLUDED*/
