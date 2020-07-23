/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _CPRIF_DRV_H_
#define _CPRIF_DRV_H_

#include <soc/cprimod/cprimod.h>
#include <appl/diag/diag.h>


#define CPRIF_REV_LETTER_A              0
#define CPRIF_REV_LETTER_B              1
#define CPRIF_REV_LETTER_C              2
#define CPRIF_REV_LETTER_D              3

#define CPRIF_REV_NUMBER_0              0
#define CPRIF_REV_NUMBER_1              1
#define CPRIF_REV_NUMBER_2              2
#define CPRIF_REV_NUMBER_3              3
#define CPRIF_REV_NUMBER_4              4
#define CPRIF_REV_NUMBER_5              5
#define CPRIF_REV_NUMBER_6              6
#define CPRIF_REV_NUMBER_7              7

#define CPRIF_TECH_PROCESS_28NM         0
#define CPRIF_TECH_PROCESS_16NM         1

#define CPRIF_PM_TYPE_PM4X10            0
#define CPRIF_PM_TYPE_PM4X25            1
#define CPRIF_PM_TYPE_PM12X10           2
#define CPRIF_PM_TYPE_PM4X10Q           3
#define CPRIF_PM_TYPE_FCPM              4
#define CPRIF_PM_TYPE_PM4X10_GEN3       5
#define CPRIF_PM_TYPE_PM4X25_GEN3       6
#define CPRIF_PM_TYPE_CPM4X25_GEN3      7

#define CPRIF_MAX_NUM_OF_QUEUES         80
#define CPRIF_MAX_NUM_OF_DATA_QUEUES    64
#define CPRIF_MAX_NUM_OF_AXC            64
#define CPRIF_BASIC_FRAME_TABLE_0       0
#define CPRIF_BASIC_FRAME_TABLE_1       1
#define CPRIF_INVALID_QUEUE             0x7F
#define CPRIF_FLOW_CLS_INVALID_QUEUE    0xFF

/*!
 * @struct cprimod_bfa_bfp_table_entry_s
 * @brief  Basic Frame Parse/Assembler HW Table Entry
 */
typedef struct cprif_pm_version_s {
    uint32 pm_type;       /**< 0 pm4x10 1 pm4x25 etc.  */
    uint32 tech_process; /**< 0 28nm 1 16nm etc  */
    uint32 rev_number;   /**< Rev Number */
    uint32 rev_letter;   /**< Rev Letter  0 A 1 B 2 C .. */
}cprif_pm_version_t;

typedef enum cprif_reset_en_s {
    cpriResetEnAssert=1,
    cpriResetEnDeassert=0
} cprif_reset_en_t;

#define CPRIF_CLPORT_IS_A0(pm_version)          ((pm_version.rev_letter == CPRIF_REV_LETTER_A) && (pm_version.rev_number == CPRIF_REV_NUMBER_0))
#define CPRIF_CLPORT_IS_B0(pm_version)          ((pm_version.rev_letter == CPRIF_REV_LETTER_B) && (pm_version.rev_number == CPRIF_REV_NUMBER_0))

#define CPRIF_RX_INTR     0x1
#define CPRIF_TX_INTR     0x2

/* CPRI port Rx/Tx interrupt status */
#define CPRIF_PORT_0_RX_INTR     0x1
#define CPRIF_PORT_1_RX_INTR     0x2
#define CPRIF_PORT_2_RX_INTR     0x4
#define CPRIF_PORT_3_RX_INTR     0x8
#define CPRIF_PORT_0_TX_INTR     0x10
#define CPRIF_PORT_1_TX_INTR     0x20
#define CPRIF_PORT_2_TX_INTR     0x40
#define CPRIF_PORT_3_TX_INTR     0x80

#define CPRIF_RX_OP_ERR_INTR     0x1
#define CPRIF_RX_FUNC_ERR_INTR   0x2
#define CPRIF_RX_ECC_ERR_INTR    0x4
#define CPRIF_RX_INTR_ALL        0x7

/* Macros for RX Operational Error Interrupts */
#define CPRI_INTR_RX_ENCAP_DATA_Q_OVRFLOW  0x1
#define CPRI_INTR_RX_ENCAP_CTRL_Q_OVRFLOW  0x2
#define CPRI_INTR_RX_ENCAP_GSM_TIMESTAMP   0x4
#define CPRI_INTR_RX_ENCAP_MISC_ERROR      0x8
#define CPRI_INTR_RX_ENCAP_RSVD4_EVENTS    0x10
#define CPRI_INTR_RX_OP_ERR_INTR_ALL       0x1F

/* Macros for RX Functional Error Interrupts */
#define CPRI_INTR_RXFRM_CW_PARSER   0x1
#define CPRI_INTR_RX_1588_SW        0x2
#define CPRI_INTR_RX_1588_FIFO      0x4
#define CPRI_INTR_RXFRM_GCW         0x8
#define CPRI_INTR_RXPCS             0x10
#define CPRI_INTR_RXPCS_FEC         0x20
#define CPRI_INTR_RX_FUNC_ERR_INTR_ALL   0x3F


#define CPRIF_TX_OP_ERR_INTR     0x1
#define CPRIF_TX_FUNC_ERR_INTR   0x2
#define CPRIF_TX_ECC_ERR_INTR    0x4
#define CPRIF_TX_INTR_ALL        0x7

/* Macros for RX Operational Error Interrupts */
#define CPRI_INTR_TX_DECAP_DATA_Q_OVRFLOW  0x1
#define CPRI_INTR_TX_DECAP_DATA_Q_UNDRFLOW 0x2
#define CPRI_INTR_TX_DECAP_DATA_Q_BUFFSIZE_ERR 0x4
#define CPRI_INTR_TX_DECAP_AG_MODE_INTR    0x8
#define CPRI_INTR_TX_TXPCS_MISC_INTR       0x10
#define CPRI_INTR_TX_OP_ERR_INTR_ALL       0x1F

/* Macros for TX Functional Error Interrupts */
#define CPRI_INTR_TX_1588_SW        0x1
#define CPRI_INTR_TX_1588_FIFO      0x2
#define CPRI_INTR_TX_GCW            0x4
#define CPRI_INTR_TX_FUNC_ERR_INTR_ALL   0x7


/*!
 * @struct cprimod_bfa_bfp_table_entry_s
 * @brief  Basic Frame Parse/Assembler HW Table Entry
 */
typedef struct cprimod_bfa_bfp_table_entry_s {
    uint8 axc_id; /**< AxC Id */
    uint8 rsrv_bit_cnt; /**< Number of Reserve Bits */
    uint8 data_bit_cnt; /**< Number of Data Bits */
}cprif_bfa_bfp_table_entry_t;

#define CPRIF_CONTAINER_MAP_CPRI_METHOD_1             0
#define CPRIF_CONTAINER_MAP_CPRI_METHOD_3             2
#define CPRIF_CONTAINER_MAP_RSVD4                     3

#define CPRIF_CONTAINER_MAP_STUFFING_AT_END           1
#define CPRIF_CONTAINER_MAP_STUFFING_AT_BEGINNING     0


/*!
 * @struct cprif_cpri_container_map_entry_s
 * @brief  CPRI Container Parser Entry Parameter.
 */
typedef struct cprif_cpri_container_map_entry_s {
    uint8 map_method; /**< Map Method. */
    uint8 axc_id; /**< AxC Id. */
    uint8 stuffing_at_end; /**< Stuff at the end. */
    uint32 stuff_cnt; /**< Stuff Cnt */
    uint32 cblk_cnt; /**< Container Block Cnt */
    uint32 naxc_cnt; /**< Container size */
    uint32 Nv; /**< Number of Stuffing Bits for Method 3. */
    uint32 Na; /**< Number of AxC for Method 3. */
    uint8 rfrm_sync; /**< Radio Frame Sync . */
    uint8 hfrm_sync; /**< Hyper Frame Sync . */
    uint32 bfn_offset; /**< Radio Frame Offset */
    uint32 hfn_offset; /**< Hyper Frame Offset. */
    uint32 bfrm_offset; /**< Basic Frame Offset. */
}cprif_cpri_container_map_entry_t;

/*!
 * @struct cprif_rsvd4_container_map_entry_s
 * @brief  CPRI Container Parser Entry Parameter.
 */
typedef struct cprif_rsvd4_container_map_entry_s {
    uint8 map_method; /**< Map Method. */
    uint8 axc_id; /**< AxC Id. */
    uint8 stuffing_at_end; /**< Stuff at the end. */
    uint32 stuff_cnt; /**< Stuff Cnt */
    uint32 cblk_cnt; /**< Container Block Cnt */
    uint32 rfrm_offset; /**< Radio Frame Offset MG. */
    uint32 bfn_offset; /**< Master Frame Offset BFN . */
    cprimod_rsvd4_msg_ts_mode_t msg_ts_mode; /**< MSG timestamp mode */
    uint8 rfrm_sync; /**< Radio Frame Sync . */
    uint8 hfrm_sync; /**< Hyper Frame Sync . */
    uint8 msg_ts_cnt; /**< Num of Msg Per Timeslot */
    uint8 msg_ts_sync; /**< MSG timestamp mode Rx only */
    uint8 use_ts_dbm; /**< use secondary DBM - Tx only */
    uint8 ts_dbm_prof_num; /**< secondary DBM Profile Num - Tx only */
    uint8 num_active_slots; /**< Num Active Slots - Tx only */
    uint32 msg_addr; /**< Header Addr - Tx only */
    uint32 msg_type; /**< Header Type- Tx Only */
    uint32 msg_ts_offset; /**< TS offset- Tx Only */
}cprif_rsvd4_container_map_entry_t;

#define CPRIF_CONTAINER_MAP_MSG_TS_MODE_WCDMA         0
#define CPRIF_CONTAINER_MAP_MSG_TS_MODE_LTE           1
#define CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_DL        2
#define CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_UL        3

typedef struct cprif_container_parser_sync_info_s {
    uint8 rfrm_sync; /**< Radio Frame Sync . */
    uint8 hfrm_sync; /**< Hyper Frame Sync . */
    uint32 hfn_offset; /**< Hyper Frame Offset. */
}cprif_container_parser_sync_info_t;



/*!
 * @struct cprif_iq_buffer_config_t
 * @brief  CPRI IQ Buffer Config
 */
typedef struct cprif_iq_buffer_config_s{
    uint16 payload_size;      /**< Packet Size.      */
    uint8 last_packet_num;   /**< Last Packet Num.  */
    uint16 last_payload_size; /**< Last Packet Size. */
}cprif_iq_buffer_config_t;

/*!
 * @struct cprifi_speed_osmod_div_table_s
 * @brief  CPRI speed_id, osr_mode and pll_div mapping table.
 */
typedef struct cprif_speed_osmod_div_lkup_table_s{
  uint8 speed_id;
  uint8 osr_mode;
  uint8 pll_mode;
  uint8 pll_multiplier;
 }cprif_speed_osmod_div_lkup_table_t;

#define OS_MODE_1      0
#define OS_MODE_2      1
#define OS_MODE_4      2
#define OS_MODE_8      5
#define OS_MODE_16     9
#define OS_MODE_32     13

#define CPRIF_DATA_SIGN_EXT_15_TO_16        0
#define CPRIF_DATA_SIGN_EXT_ADD_0           1
#define CPRIF_DATA_SIGN_EXT_16_TO_15        2
#define CPRIF_DATA_SIGN_EXT_ADD_1           3

#define CPRIF_DATA_BUFFER_BLOCK_SIZE             16 /* 16 bytess per block of encap/decap queue. */

#define CPRIF_PBMP_WORD_WIDTH                           32       /* 32 bits */
#define CPRIF_ENCAP_BUFFER_NUM_ENTRIES                  3648
#define CPRIF_DECAP_BUFFER_NUM_ENTRIES                  8192
#define CPRIF_NUM_OF_BUFFER_ENTRIES(size_in_bytes)      ((size_in_bytes + \
                                                         CPRIF_DATA_BUFFER_BLOCK_SIZE-1)\
                                                         / CPRIF_DATA_BUFFER_BLOCK_SIZE)

#define CPRIF_NUM_PBMP_WORDS(entries)                      \
            ((entries+CPRIF_PBMP_WORD_WIDTH-1)/CPRIF_PBMP_WORD_WIDTH)

#define CPRIF_PBMP_WORD_GET(bm,word)             (bm[(word)])
#define CPRIF_PBMP_WENT(idx)                     ((idx)/CPRIF_PBMP_WORD_WIDTH)
#define CPRIF_PBMP_WBIT(idx)                     \
                                        (1U << ((idx) % CPRIF_PBMP_WORD_WIDTH))

#define CPRIF_PBMP_WORD_ENTRY(bm, idx)                \
                                    (CPRIF_PBMP_WORD_GET(bm,CPRIF_PBMP_WENT(idx)))

#define CPRIF_PBMP_AVAILABLE(bm, idx)               \
                        ((CPRIF_PBMP_WORD_ENTRY(bm, idx) & CPRIF_PBMP_WBIT(idx)) == 0)

#define CPRIF_PBMP_IS_SET(bm, idx)               \
                        ((CPRIF_PBMP_WORD_ENTRY(bm, idx) & CPRIF_PBMP_WBIT(idx)) != 0)

#define CPRIF_PBMP_IDX_MARK(bm, idx)             \
                            ( CPRIF_PBMP_WORD_ENTRY(bm, idx) |=  CPRIF_PBMP_WBIT(idx))

#define CPRIF_PBMP_IDX_UNMARK(bm, idx)             \
                            ( CPRIF_PBMP_WORD_ENTRY(bm, idx) &= ~CPRIF_PBMP_WBIT(idx))

#define CPRIF_PBMP_NEXT_AVAILABLE(bm, tot ,  idx)                \
    for ((idx) = (idx); (idx) < tot ; (idx)++) \
        if (CPRIF_PBMP_AVAILABLE((bm), (idx)))


/*!
 * @struct cprif_encap_decap_data_entry_t
 * @brief  Encapsulation Data Configuration
 */
typedef struct cprif_encap_decap_data_entry_s{
    uint8 sample_size;          /**< sample size in bits.      */
    uint8 out_sample_size;      /**< out sample size in bits.  */
    uint8 valid;                /**< valid.                    */
    uint8 mux_enable;           /**< mux/demux enable.      */
    uint16 queue_offset;        /**< queue offset.             */
    uint16 queue_size;          /**< queue size. Num of 16bytes*/
    uint8 sign_ext_enable;      /**< .      */
    uint8 sign_ext_type;        /**< .      */
    uint8 work_queue_select;    /**< work queue.               */
    uint8 bit_reversal;         /**< bit reversal.             */
    uint8 tx_cycle_size;        /**< Num of packet in buffer  */
}cprif_encap_decap_data_entry_t;


#define CPRIF_RSVD4_GSM_CONTROL_LOCATION_127_120            1
#define CPRIF_RSVD4_GSM_CONTROL_LOCATION_63_56              0


#define CPRIF_RSVD4_GSM_PAD_DISABLE                         0
#define CPRIF_RSVD4_GSM_PAD_ENABLE                          1

/*!
 * @struct cprif_rsvd4_gsm_config_t
 * @brief  Encapsulation Data Configuration
 */
typedef struct cprif_rsvd4_encap_decap_data_entry_s{
    uint8 gsm_pad_size;             /**<  GSM Pad Size */
    uint8 valid;                    /**< valid.                    */
    uint16 queue_offset;             /**< queue offset.             */
    uint32 queue_size;               /**< queue size. Num of bytes*/
    uint8 gsm_extra_pad_size;       /**< GSM extra Pad Size  */
    uint8 work_queue_select;        /**< work queue.               */
    uint8 tx_cycle_size;            /**< Num of packet in buffer  */
    uint8 gsm_pad_enable;           /**< out sample size in bits.  */
    uint8 gsm_control_location;     /**< GSM control location 1: 127:120  0: 63:56 RxOnly */
    uint8 bit_reversal;             /**< Bit reversal control.  */    
}cprif_rsvd4_encap_decap_data_entry_t;


#define CPRIF_ENCAP_HEADER_HDR_TYPE_ROE                     0
#define CPRIF_ENCAP_HEADER_HDR_TYPE_ENCAP_NONE              1


#define CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED               0
#define CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_0          1 /* tagged use vlan 0 id. */
#define CPRIF_ENCAP_HEADER_VLAN_TYPE_Q_IN_Q                 2
#define CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_1          3 /* tagged use vlan 1 id. */



#define CPRIF_ENCAP_HEADER_ETHER_TYPE_INDEX_0               0
#define CPRIF_ENCAP_HEADER_ETHER_TYPE_INDEX_1               1

#define CPRIF_ENCAP_HEADER_FLAGS_HDR_CONFIG_SET             0x1
#define CPRIF_ENCAP_HEADER_FLAGS_GSM_CONFIG_SET             0x2


/*!
 * @struct cprif_encap_header_entry_t
 * @brief  Encapsulation Header Configuration
 */
typedef struct cprif_encap_header_entry_s{
    uint8 header_type;          /**< Encap Header Type 0: ROE   1: no encap */
    uint16 flow_id;             /**< ROE flow id */
    uint32  ver;                /**< Ver Field in Reg. */
    uint32  pkttype;            /**< PktType Field in Reg. */
    uint8 ordering_info_index;
    uint8 mac_da_index;
    uint8 mac_sa_index;
    uint8 vlan_type;
    uint8 vlan_id_0_index;
    uint8 vlan_id_1_index;
    uint8 vlan_0_priority;
    uint8 vlan_1_priority;
    uint8 ether_type_index;
    uint8 use_tagid_for_vlan;
    uint8 use_tagid_for_flowid;
    uint8 use_opcode;
    uint8 roe_opcode;
    uint8 tsn_bitmap; /* overlay */
    uint8 gsm_q_cnt_disable;
}cprif_encap_header_entry_t;


#define CPRIF_ORDERING_INFO_TYPE_ROE                  0x0
#define CPRIF_ORDERING_INFO_TYPE_BFN_FOR_QCNT         0x1
#define CPRIF_ORDERING_INFO_TYPE_USE_PINFO            0x2


#define CPRIF_ORDERING_INFO_PROP_NO_INC               0x0
#define CPRIF_ORDERING_INFO_PROP_INC_CONST            0x1
#define CPRIF_ORDERING_INFO_PROP_INC_BY_PAYLOAD       0x2


#define CPRIF_ORDERING_INFO_TABLE_SIZE                24


/*!
 * @struct cprif_encap_ordering_info_entry_t
 * @brief  Encapsulation Ordering info Entry
 */
typedef struct cprif_encap_ordering_info_entry_s{
    uint8 type;
    uint8 p_size;
    uint8 q_size;
    uint32 max;
    uint32 increment;
    uint8 pcnt_prop;
    uint8 qcnt_prop;

}cprif_encap_ordering_info_entry_t;



/*!
 * @struct cprif_decap_ordering_info_entry_t
 * @brief  Decapsulation Ordering info + TSN Entry
 */
typedef struct cprif_decap_ordering_info_entry_s{
    uint8 type;
    uint8 p_size;
    uint8 q_size;
    uint32 max;
    uint32 increment;
    uint8 pcnt_inc_p2;
    uint8 qcnt_inc_p2;
    uint8 pcnt_extended;
    uint8 pcnt_pkt_count;
    uint8 modulo_2;
    uint32 bias;
    uint8 gsm_tsn_bitmap;
}cprif_decap_ordering_info_entry_t;

#define CPRIF_NUM_BYTES_IN_MAC_ADDR     6

/* Number of MAC Addr in SA and DA table. */
#define CPRIF_MAC_ADDR_TABLE_SIZE           16

#define CPRIF_VLAN_ID_TABLE_SIZE            64

#define CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM            0
#define CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE     1
#define CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE     2

#define CPRIF_DECAP_CLS_FLOW_TYPE_DATA                  0
#define CPRIF_DECAP_CLS_FLOW_TYPE_DATA_WITH_EXT         1   /* NOT USED */
#define CPRIF_DECAP_CLS_FLOW_TYPE_CTRL_WITH_OPCODE      2
#define CPRIF_DECAP_CLS_FLOW_TYPE_CTRL                  3

#define CPRIF_ETHTYPE_ROE_ETHTYPE_0                     0
#define CPRIF_ETHTYPE_ROE_ETHTYPE_1                     1
#define CPRIF_ETHTYPE_VLAN_TAGGED                       2
#define CPRIF_ETHTYPE_VLAN_QINQ                         3

#define CPRIF_DECAP_ROE_ETHTYPE                         0
#define CPRIF_DECAP_FAST_ETH_ETHTYPE                    1
#define CPRIF_DECAP_VLAN_TAGGED_ETHTYPE                 2
#define CPRIF_DECAP_VLAN_QINQ_ETHTYPE                   3



#define CPRIF_TX_TGEN_DIVIDER_NO_DIV                    1
#define CPRIF_TX_TGEN_DIVIDER_DIV_BY_2                  2
#define CPRIF_TX_TGEN_DIVIDER_DIV_BY_4                  4
#define CPRIF_TX_TGEN_DIVIDER_DIV_BY_8                  8
#define CPRIF_TX_TGEN_DIVIDER_DIV_BY_16                 16
#define CPRIF_TX_TGEN_DIVIDER_DIV_BY_32                 32

#define CPRIF_TX_TGEN_USE_BFN                           0
#define CPRIF_TX_TGEN_USE_HFN                           1



#define CPRIF_HEADER_COMPARE_DATA_FLOW                  0
#define CPRIF_HEADER_COMPARE_CTRL_FLOW                  1

/*!
 * @struct cprif_rsvd4_header_compare_entry_t
 * @brief RSVD4 Header Compare entry
 */
typedef struct cprif_header_compare_entry_s{
    uint32 valid;
    uint32 match_data;
    uint32 mask;
    uint32 flow_id;
    uint32 ctrl_flow;
}cprif_header_compare_entry_t;

#define CPRIF_HEADER_COMPARE_TABLE_SIZE             80

/*!
 * @struct cprif_modulo_rule_entry_t
 * @brief RSVD4 Modulo Rule Entry
 */
typedef struct cprif_modulo_rule_entry_s{
    uint8 active;
    uint8 modulo;
    uint8 modulo_index;
    uint8 dbm_enable;
    uint8 flow_id;
    uint8 ctrl_flow;
}cprif_modulo_rule_entry_t;



#define CPRIF_MODULO_RULE_USE_DATA_RULE                     0
#define CPRIF_MODULO_RULE_USE_CTRL_RULE                     1

#define CPRIF_MODULO_RULE_DATA_RULE_SIZE                    72
#define CPRIF_MODULO_RULE_CTRL_RULE_SIZE                    16

#define CPRIF_MODULO_RULE_DATA_FLOW                         0
#define CPRIF_MODULO_RULE_CTRL_FLOW                         1

#define CPRIF_MODULO_RULE_MOD_1                             0
#define CPRIF_MODULO_RULE_MOD_2                             1
#define CPRIF_MODULO_RULE_MOD_4                             2
#define CPRIF_MODULO_RULE_MOD_8                             3
#define CPRIF_MODULO_RULE_MOD_16                            4
#define CPRIF_MODULO_RULE_MOD_32                            5
#define CPRIF_MODULO_RULE_MOD_64                            6

#define CPRIF_MODULO_RULE_DBM_PROFILE_TABLE_SIZE            8
#define CPRIF_MODULO_RULE_MAX_DATA_FLOW                     64  /* 0-63 */
#define CPRIF_MODULO_RULE_MAX_CTRK_FLOW                     18  /* 0-17 */


/*!
 * @struct cprif_rsvd4_dbm_rule_entry_t
 * @brief RSVD4 Dual Bit Map  Rule Entry
 */
typedef struct cprif_rsvd4_dbm_rule_entry_s{
    uint8 bm1_mult;
    uint32 bm1[3];
    uint8 bm1_size;
    uint32 bm2[2];
    uint8 bm2_size;
    uint8 num_slots;
    uint8 pos_index;
    uint8 pos_entries;
}cprif_rsvd4_dbm_rule_entry_t;


#define CPRIF_DBM_RULE_PROFILE_TABLE_SIZE                   8
#define CPRIF_DBM_RULE_POS_INDEX_START_0                    0
#define CPRIF_DBM_RULE_POS_INDEX_START_10                   1
#define CPRIF_DBM_RULE_POS_INDEX_START_30                   2
#define CPRIF_DBM_RULE_POS_INDEX_START_40                   3
#define CPRIF_DBM_RULE_POS_INDEX_START_50                   4
#define CPRIF_DBM_RULE_POS_INDEX_START_60                   5
#define CPRIF_DBM_RULE_POS_INDEX_START_70                   6


#define CPRIF_DBM_RULE_POS_ENTRIES_10_IN_GROUP              0
#define CPRIF_DBM_RULE_POS_ENTRIES_20_IN_GROUP              1
#define CPRIF_DBM_RULE_POS_ENTRIES_40_IN_GROUP              2
#define CPRIF_DBM_RULE_POS_ENTRIES_80_IN_GROUP              3

typedef struct cprif_rsvd4_secondary_dbm_rule_entry_s{
  uint8 bm1_mult;
  uint8 bm1;
  uint8 bm1_size;
  uint8 bm2;
  uint8 bm2_size;
  uint8 num_slots;
 }cprif_rsvd4_secondary_dbm_rule_entry_t;

#define CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE         8

typedef struct cprif_rsvd4_dbm_pos_table_entry_s{
  uint8 valid;
  uint8 flow_id;
  uint8 ctrl_flow;
  uint8 flow_index;
 }cprif_rsvd4_dbm_pos_table_entry_t;

#define CPRIF_DBM_POSITION_TABLE_SIZE           80

/* Standard HDLC rate definition. */
#define CPRIF_HDLC_RATE_NO_HDLC             0
#define CPRIF_HDLC_RATE_240KBP              1
#define CPRIF_HDLC_RATE_480KBP              2
#define CPRIF_HDLC_RATE_960KBP              3
#define CPRIF_HDLC_RATE_1920KBP             4
#define CPRIF_HDLC_RATE_2400KBP             5
#define CPRIF_HDLC_RATE_HIGHEST             6
#define CPRIF_HDLC_RATE_NEGOTIATE           7



typedef struct cprif_drv_rx_control_word_status_s{
    uint32 hyper_frame;
    uint32 radio_frame;
    uint32 basic_frame;
    uint32 protocol_ver;
    uint32 hdlc_rate;
    uint32 reset;
    uint32 l1_lof;
    uint32 l1_los;
    uint32 l1_sdi;
    uint32 l1_rai;
    uint32 eth_pointer;
 }cprif_drv_rx_control_word_status_t;


typedef struct cprif_drv_tx_control_word_status_s{
    uint32 enet_ptr;
    uint32 l1_fun;
    uint32 hdlc_rate;
    uint32 protocol_ver;
 }cprif_drv_tx_control_word_t;


#define CPRIF_HDLC_CRC_BYTE_NO_SWAP                         0   /* [15:8][7:0] */
#define CPRIF_HDLC_CRC_BYTE_SWAP                            1   /* [7:0][15:8] */

#define CPRIF_HDLC_CRC_INIT_VAL_ALL_0S                      0
#define CPRIF_HDLC_CRC_INIT_VAL_ALL_1S                      1

#define CPRIF_HDLC_USE_HDLC                                 0
#define CPRIF_HDLC_USE_FE_MAC                               1

#define CPRIF_HDLC_FCS_SIZE_8_BITS                          0
#define CPRIF_HDLC_FCS_SIZE_16_BITS                         1
#define CPRIF_HDLC_FCS_SIZE_32_BITS                         2

#define CPRIF_HDLC_FCS_CHECK                                0
#define CPRIF_HDLC_FCS_NO_CHECK                             1 /* ignore fcs */

#define CPRIF_HDLC_FILLING_FLAG_PATTERN_7E                  0
#define CPRIF_HDLC_FILLING_FLAG_PATTERN_7F                  1
#define CPRIF_HDLC_FILLING_FLAG_PATTERN_FF                  3

#define CPRIF_HDLC_CRC_MODE_APPEND                          0
#define CPRIF_HDLC_CRC_MODE_REPLACE                         1
#define CPRIF_HDLC_CRC_MODE_NO_UPDATE                       2

#define CPRIF_HDLC_FLAG_SIZE_1_BYTE                         0
#define CPRIF_HDLC_FLAG_SIZE_2_BYTE                         1

typedef struct cprif_cpri_rx_hdlc_config_s{
  uint32 cw_sel;
  uint32 cw_size;
  uint32 crc_byte_swap;         /* CPRIF_HDLC_CRC_BYTE_ */
  uint32 crc_init_val;          /* CPRIF_HDLC_CRC_INIT_VAL ..   */
  uint32 queue_num;
  uint32 use_fe_mac;            /* 0 - HDLC   1- use Fast Ethernet MAC */
  uint32 ignore_fcs_err;        /* 0 - check fcs error.   1 - ignore fcs error. */
  uint32 fcs_size;              /* CPRIF_HDLC_FCS_SIZE_  */
  uint32 runt_frame_drop;
  uint32 long_frame_drop;
  uint32 min_frame_size;        /* in bits */
  uint32 max_frame_size;        /* in bits */
 }cprif_cpri_rx_hdlc_config_t;

typedef struct cprif_cpri_tx_hdlc_config_s{
  uint32 cw_sel;
  uint32 cw_size;
  uint32 filling_flag_pattern;     /* CPRIF_HDLC_FLAG_FILL_PATTERN */
  uint32 flag_size;             /* CPRIF_HDLC_FLAG_SIZE .. */
  uint32 crc_byte_swap;         /* CPRIF_HDLC_CRC_BYTE_ */
  uint32 crc_init_val;          /* CPRIF_HDLC_CRC_INIT_VA   */
  uint32 queue_num;
  uint32 use_fe_mac;            /* 0 - HDLC   1- use Fast Ethernet MAC */
  uint32 fcs_size;              /* CPRIF_HDLC_FCS_SIZE_  */
  uint32 crc_mode;              /* CPRIF_HDLC_CRC_MODE_  */
 }cprif_cpri_tx_hdlc_config_t;


#define CPRIF_FAST_ETH_FCS_CHECK                                0
#define CPRIF_FAST_ETH_FCS_NO_CHECK                             1 /* ignore fcs */

#define CPRIF_FAST_ETH_CRC_NO_STRIP                             0
#define CPRIF_FAST_ETH_CRC_STRIP                                1

#define CPRIF_FAST_ETH_CRC_MODE_APPEND                          0
#define CPRIF_FAST_ETH_CRC_MODE_REPLACE                         1
#define CPRIF_FAST_ETH_CRC_MODE_NO_UPDATE                       2

typedef struct cprif_cpri_rx_fast_eth_word_config_s{
  uint32 sub_channel_start;
  uint32 sub_channel_size;
  uint32 cw_sel;
  uint32 cw_size;
 }cprif_cpri_rx_fast_eth_word_config_t;


typedef struct cprif_cpri_rx_fast_eth_config_s{
  uint32 queue_num;
  uint32 ignore_fcs_err;        /* 0 - check fcs error.   1 - ignore fcs error. */
  uint32 min_packet_drop;
  uint32 max_packet_drop;
  uint32 min_packet_size;        /* in  bytes */
  uint32 max_packet_size;        /* in  bytes */
  uint32 strip_crc;
 }cprif_cpri_rx_fast_eth_config_t;


typedef struct cprif_cpri_tx_fast_eth_config_s{
  uint32 sub_channel_start;
  uint32 sub_channel_size;
  uint32 cw_sel;
  uint32 cw_size;
  uint32 min_ipg;   /* in bytes */
  uint32 queue_num;
  uint32 crc_mode;              /* CPRIF_FAST_ETH_CRC_MODE_  */
 }cprif_cpri_tx_fast_eth_config_t;

#define CPRIF_VSD_SCHAN_NUM_OF_BYTES_1                          0
#define CPRIF_VSD_SCHAN_NUM_OF_BYTES_2                          1
#define CPRIF_VSD_SCHAN_NUM_OF_BYTES_4                          2

#define CPRIF_VSD_FLOW_NUM_OF_BYTES_2                           0
#define CPRIF_VSD_FLOW_NUM_OF_BYTES_4                           1


#define CPRIF_VSD_CTRL_BYTE_NO_SWAP                         0   /* [15:8][7:0] */
#define CPRIF_VSD_CTRL_BYTE_SWAP                            1   /* [7:0][15:8] */
#define CPRIF_VSD_CTRL_MAX_NUM_SECTION                      128
#define CPRIF_VSD_CTRL_MAX_NUM_SECTOR                       64
#define CPRIF_VSD_CTRL_NUM_GROUPS                           64
#define CPRIF_VSD_CTRL_NUM_GROUP_PTR                        64
#define CPRIF_VSD_CTRL_MAX_GROUP_SIZE                       4
#define CPRIF_VSD_CTRL_MAX_FLOW_ID                          128



typedef struct cprif_cpri_vsd_config_s{
  uint32 sub_channel_start;
  uint32 sub_channel_size;
  uint32 sub_channel_num_bytes; /* CPRIF_VSD_NUM_OF_BYTES .. */
  uint32 sub_channel_steps;
  uint32 flow_bytes;  /* CPRIF_VSD_FLOW_NUM_OF_BYTES */
  uint32 queue_num;
  uint32 byte_order;
  uint64 rsvd_mask[2];
 }cprif_cpri_vsd_config_t;

typedef struct cprif_cpri_rx_vsd_ctrl_flow_config_s{
  uint32 group_id;
  uint32 hyper_frame_number;
  uint32 hyper_frame_modulo;
  uint32 filter_zero_data;
 }cprif_cpri_rx_vsd_ctrl_flow_config_t;

typedef struct cprif_cpri_vsd_ctrl_group_config_s{
  uint32 hyper_frame_number;
  uint32 hyper_frame_modulo;
  uint32 filter_zero_data;
  uint32 num_sec;
  uint32 sec_num[4];
 }cprif_cpri_vsd_ctrl_group_config_t;

typedef struct cprif_cpri_tx_vsd_ctrl_flow_config_s{
  uint32 hyper_frame_number;
  uint32 hyper_frame_modulo;
  uint32 repeat_mode;
 }cprif_cpri_tx_vsd_ctrl_flow_config_t;


#define CPRIF_VSD_RAW_FILTER_MODE_DISABLE           0
#define CPRIF_VSD_RAW_FILTER_MODE_NON_ZERO          1
#define CPRIF_VSD_RAW_FILTER_MODE_PERIODIC          2
#define CPRIF_VSD_RAW_FILTER_MODE_CHANGE            3
#define CPRIF_VSD_RAW_FILTER_MODE_PATTERN_MATCH     4






typedef struct cprif_cpri_vsd_raw_config_s{
  uint32 schan_start;
  uint32 schan_size;
  uint32 cw_select;
  uint32 cw_size;
 }cprif_cpri_vsd_raw_config_t;

typedef struct cprif_cpri_rx_vsd_raw_filter_config_s{
  uint32 filter_mode;
  uint32 hfn_index;
  uint32 hfn_modulo;
  uint32 match_offset;
  uint32 match_value;
  uint32 match_mask;
  uint32 queue_num;
 }cprif_cpri_rx_vsd_raw_filter_config_t;

#define CPRIF_VSD_RAW_MAX_GROUP                     8
#define CPRIF_VSD_RAW_MAP_MODE_USE_MODULO           0
#define CPRIF_VSD_RAW_MAP_MODE_USE_ROE_HDR          1

#define CPRIF_CW_REPEAT_MODE_ONCE          0
#define CPRIF_CW_REPEAT_MODE_CONTINUOUS    1

#define CPRIF_GCW_MAX_GROUP          16
#define CPRIF_GCW_MASK_NONE          0
#define CPRIF_GCW_MASK_LSB           1
#define CPRIF_GCW_MASK_MSB           2
#define CPRIF_GCW_MASK_BOTH          3

#define CPRIF_GCW_NUM_WORD_IN_STATUS 4


typedef struct cprif_cpri_tx_vsd_raw_filter_config_s{
  uint32 queue_num;
  uint32 map_mode;
  uint32 repeat_mode;
  uint32 bfn0_filter_enable;
  uint32 bfn1_filter_enable;
  uint32 hfn_index;
  uint32 hfn_modulo;
  uint32 idle_value;
 }cprif_cpri_tx_vsd_raw_filter_config_t;

typedef struct cprif_cpri_rx_gcw_config_s{
  uint32 Ns;
  uint32 Xs;
  uint32 Y;
  uint32 mask;
  uint32 filter_mode;
  uint32 hfn_index;
  uint32 hfn_modulo;
  uint32 match_value;
  uint32 match_mask;
 }cprif_cpri_rx_gcw_config_t;

typedef struct cprif_cpri_tx_gcw_config_s{
  uint32 Ns;
  uint32 Xs;
  uint32 Y;
  uint32 mask;
  uint32 repeat_mode;
  uint32 bfn0_filter_enable;
  uint32 bfn1_filter_enable;
  uint32 hfn_index;
  uint32 hfn_modulo;
 }cprif_cpri_tx_gcw_config_t;

#define CPRIF_CTRL_FLOW_PROC_TYPE_ETH                   0
#define CPRIF_CTRL_FLOW_PROC_TYPE_FCB                   1
#define CPRIF_CTRL_FLOW_PROC_TYPE_PAYLOAD               2
#define CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG              3
#define CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG_WITH_TAG     4

#define CPRIF_CTRL_FLOW_MAX                             16

#define CPRIF_CTRL_MAX_ROE_FLOW_ID                      256

typedef struct cprif_rsvd4_rx_ctrl_flow_config_s{
  uint32 proc_type;
  uint32 queue_num;
  uint32 sync_profile;
  uint32 sync_enable;
 }cprif_rsvd4_rx_ctrl_flow_config_t;


#define CPRIF_SYNC_PROFILE_MAX              4

#define CPRIF_SYNC_PROFILE_EVERY_MF         0 /* every frame */
#define CPRIF_SYNC_PROFILE_UP_TO_6MF        1 /* up to 6 MF  */

typedef struct cprif_rsvd4_rx_sync_profile_entry_s{
  uint32 bfn_offset;
  uint32 rfrm_offset;
  uint32 count_cycle;
 }cprif_rsvd4_rx_sync_profile_entry_t;

#define CPRIF_TAG_FLOW_MAP_MAX                  64
#define CPRIF_TAG_GEN_MAX_ENTRY                 64
typedef struct cprif_rx_tag_gen_entry_s{
  uint32 valid;
  uint32 header;
  uint32 mask;
  uint32 tag_id;
  uint32 rtwp_word_count;

 }cprif_rx_tag_gen_entry_t;

#define CPRIF_TX_CONTROL_MAX_MEMBER            4
#define CPRIF_TX_CONTROL_MAX_GROUP             2
typedef struct cprif_control_group_entry_t{
  uint32 valid_mask;
  uint32 queue_num[4];
  uint32 proc_type[4];
 }cprif_control_group_entry_t;

#define CPRIF_TX_HEADER_CONFIG_TABLE_SIZE   64

/*!
 * @struct cprif_cpri_ingress_pres_time_ctrl_s
 * @brief  CPRI Presentation time control.
 */
typedef struct cprif_cpri_ingress_pres_time_ctrl_s{
    uint32 pres_mod_cnt; /**< Presentation Mod Count. */
    uint32 rfrm_sync; /**< Radio Frame Sync . */
    uint32 hfrm_sync; /**< Hyper Frame Sync . */
    uint32 bfn_offset; /**< Radio Frame Offset */
    uint32 hfn_offset; /**< Hyper Frame Offset. */
    int    approx_inc_disable; /* Disable Approximate Increment. */
}cprif_cpri_ingress_pres_time_ctrl_t;

/*!
 * @struct cprif_cpri_egress_agn_config_s
 * @brief  CPRI Egress Agnostic Config.
 */
typedef struct cprif_cpri_egress_agnostic_config_s{
    uint32 rd_ptr_max; /**< Max Buffer Ptr.# of 16 bytes buffers. */
    uint32 queue_num; /**< Queue number, ususally 0 . */
    uint32 mod_cnt ; /**< Mode Count, equivalent to Cycle Size. */
}cprif_cpri_egress_agnostic_config_t;


#define CPRIF_COMPRESSION_TABLE_SIZE            1024
#define CPRIF_DECOMPRESSION_TABLE_SIZE          256

#define CPRIF_CONTROL_FLOW_TABLE_SIZE           16
#define CPRIF_CONTROL_FLOW_MIN_QUEUE_NUM        64

#define CPRIF_TUNNEL_MODE_NUM_OF_BFP_ENTRIES    24
#define CPRIF_TUNNEL_MODE_NUM_OF_BFA_ENTRIES    48

#define CPRIF_RSVD4_AGNOSTIC_MODE_NUM_OF_BFP_ENTRIES    25
#define CPRIF_RSVD4_AGNOSTIC_MODE_NUM_OF_BFA_ENTRIES    25




/* Field IDs for CPRI_ENCAP_HDR_FLD_MAPPING */
#define CPRIF_ENCAP_HEADER_FIELD_ID_SUBTYPE             0
#define CPRIF_ENCAP_HEADER_FIELD_ID_FLOW_ID             1
#define CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_15_8         2
#define CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_7_0          3
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_31_24    4
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_23_16    5
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_15_8     6
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_7_0      7
#define CPRIF_ENCAP_HEADER_FIELD_ID_OPCODE              8
#define CPRIF_ENCAP_HEADER_FIELD_ID_UNMAPPED            0xf


/*!
 * @struct cprif_cpri_encap_hdr_field_mapping_s
 * @brief  CPRI_ENCAP_HDR_FLD_MAPPING.
 */
typedef struct cprif_drv_encap_hdr_field_mapping_s{
    uint32 hdr_byte0_sel;  /**< Header Byte 0 select */
    uint32 hdr_byte1_sel;  /**< Header Byte 1 select */
    uint32 hdr_byte2_sel;  /**< Header Byte 2 select */
    uint32 hdr_byte3_sel;  /**< Header Byte 3 select */
    uint32 hdr_byte4_sel;  /**< Header Byte 4 select */
    uint32 hdr_byte5_sel;  /**< Header Byte 5 select */
    uint32 hdr_byte6_sel;  /**< Header Byte 6 select */
    uint32 hdr_byte7_sel;  /**< Header Byte 7 select */
    uint32 hdr_byte8_sel;  /**< Header Byte 8 select */
    uint32 hdr_byte9_sel;  /**< Header Byte 9 select */
    uint32 hdr_byte10_sel; /**< Header Byte 10 select */
    uint32 hdr_byte11_sel; /**< Header Byte 11 select */
}cprif_drv_encap_hdr_field_mapping_t;




/* Field IDs for CPRI_DECAP_HDR_FLD_MAPPING */
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_0          0
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_1          1
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_2          2
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_3          3
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_4          4
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_5          5
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_6          6
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_7          7
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_8          8
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_9          9
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_10         10
#define CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_11         11
#define CPRIF_DECAP_HEADER_FIELD_ID_VLAN_0_BYTE_0       12
#define CPRIF_DECAP_HEADER_FIELD_ID_VLAN_0_BYTE_1       13
#define CPRIF_DECAP_HEADER_FIELD_ID_VLAN_1_BYTE_0       14
#define CPRIF_DECAP_HEADER_FIELD_ID_VLAN_1_BYTE_1       15

/*!
 * @struct cprif_drv_cpri_decap_hdr_field_mapping_s
 * @brief  CPRI_DECAP_HDR_FLD_MAPPING.
 */
typedef struct cprif_drv_decap_hdr_field_mapping_s{
    uint32 subtype;             /**< SubType Field select            */
    uint32 flowid;              /**< FlowID Field select             */
    uint32 length_15_8;         /**< Length 15:8 Field select        */
    uint32 length_7_0;          /**< Length 7:0 Field select         */
    uint32 orderinginfo_31_24;  /**< OrderingInfo 31:24 Field select */
    uint32 orderinginfo_23_16;  /**< OrderingInfo 23:16 Field select */
    uint32 orderinginfo_15_8;   /**< OrderingInfo 15:8  Field select */
    uint32 orderinginfo_7_0;    /**< OrderingInfo 7:0   Field select */
}cprif_drv_decap_hdr_field_mapping_t;


/* LEN field adjustment */

#define CPRIF_LEN_ADJUST_POS_MAX            8
#define CPRIF_LEN_ADJUST_NEG_MAX            (-8)
#define CPRIF_LEN_ADJUST_SIGNED_BIT     0x00000010
#define CPRIF_LEN_ADJUST_VALUE_BITS     0x0000000F

#define CPRIF_LEN_ADJUST_ZERO           0
#define CPRIF_LEN_ADJUST_ADD_1          1       /*  Add 1       */
#define CPRIF_LEN_ADJUST_ADD_2          2       /*  Add 2       */
#define CPRIF_LEN_ADJUST_ADD_3          3       /*  Add 3       */
#define CPRIF_LEN_ADJUST_ADD_4          4       /*  Add 4       */
#define CPRIF_LEN_ADJUST_ADD_5          5       /*  Add 5       */
#define CPRIF_LEN_ADJUST_ADD_6          6       /*  Add 6       */
#define CPRIF_LEN_ADJUST_ADD_7          7       /*  Add 7       */
#define CPRIF_LEN_ADJUST_ADD_8          8       /*  Add 8       */

#define CPRIF_LEN_ADJUST_SUB_1          0x1F    /*  Subtract 1       */
#define CPRIF_LEN_ADJUST_SUB_2          0x1E    /*  Subtract 2       */
#define CPRIF_LEN_ADJUST_SUB_3          0x1D    /*  Subtract 3       */
#define CPRIF_LEN_ADJUST_SUB_4          0x1C    /*  Subtract 4       */
#define CPRIF_LEN_ADJUST_SUB_5          0x1B    /*  Subtract 5       */
#define CPRIF_LEN_ADJUST_SUB_6          0x1A    /*  Subtract 6       */
#define CPRIF_LEN_ADJUST_SUB_7          0x19    /*  Subtract 7       */
#define CPRIF_LEN_ADJUST_SUB_8          0x18    /*  Subtract 8       */

/* CMPPORT_TOP_LANE_CTRL , ingress rate control */
#define CPRIF_INGRESS_RATE_FULL             3  /* Full rate of IP TDM.  */
#define CPRIF_INGRESS_RATE_TWO_THIRD        2  /* 2/3  rate of IP TDM.  */
#define CPRIF_INGRESS_RATE_HALF             1  /* half rate of IP TDM.  */
#define CPRIF_INGRESS_RATE_ONE_THIRD        0  /* 1/3  rate of IP TDM.  */

#define CPRIF_RSVD4_MSG_GRP_SIZE        3200


int cprif_bfa_bfp_table_entry_t_init( cprif_bfa_bfp_table_entry_t* cprimod_bfa_bfp_table_entry);
int cprif_cpri_container_map_entry_t_init( cprif_cpri_container_map_entry_t* cprif_cpri_container_map_entry);
int cprif_rsvd4_container_map_entry_t_init( cprif_rsvd4_container_map_entry_t* cprif_rsvd4_container_map_entry);
int cprif_iq_buffer_config_t_init( cprif_iq_buffer_config_t* cprif_iq_buffer_config_entry);
int cprif_encap_decap_data_entry_t_init( cprif_encap_decap_data_entry_t* cprif_encap_decap_data_entry);
int cprif_rsvd4_encap_decap_data_entry_t_init( cprif_rsvd4_encap_decap_data_entry_t* cprif_rsvd4_encap_decap_data_entry);
int cprif_encap_header_entry_t_init( cprif_encap_header_entry_t* cprif_encap_header_entry);
int cprif_encap_ordering_info_entry_t_init( cprif_encap_ordering_info_entry_t* cprif_encap_ordering_info_entry);
int cprif_decap_ordering_info_entry_t_init( cprif_decap_ordering_info_entry_t* cprif_decap_ordering_info_entry);
int cprif_header_compare_entry_t_init( cprif_header_compare_entry_t* cprif_header_compare_entry);
int cprif_modulo_rule_entry_t_init( cprif_modulo_rule_entry_t* cprif_rsvd4_modulo_rule_entry);
int cprif_rsvd4_dbm_rule_entry_t_init( cprif_rsvd4_dbm_rule_entry_t* cprif_rsvd4_dbm_rule_entry);
int cprif_rsvd4_secondary_dbm_rule_entry_t_init( cprif_rsvd4_secondary_dbm_rule_entry_t* cprif_rsvd4_secondary_dbm_rule_entry);

/* use for both encap and decap */
int cprif_drv_encap_data_allocate_buffer(int unit, int port,
                                         cprimod_direction_t dir,
                                         uint16 size_in_bytes,
                                         uint16* offset);

int cprif_drv_basic_frame_parser_active_table_set(int unit, int port, uint8 table);
int cprif_drv_basic_frame_parser_active_table_get(int unit, int port, uint8* table);
int cprif_drv_basic_frame_parser_table_num_entries_set(int unit, int port, uint8 table, uint32 num_entries);
int cprif_drv_basic_frame_parser_table_num_entries_get(int unit, int port, uint8 table, uint32* num_entries);
int cprif_drv_rx_framer_agnostic_mode_set(int unit, int port, uint32 axc_id,  int enable,  cprimod_agnostic_mode_type_t mode, int restore_rsvd4);
int cprif_drv_rx_framer_agnostic_mode_get(int unit, int port, uint32* axc_id, int* enable, cprimod_agnostic_mode_type_t* mode);
int cprif_drv_tx_framer_rsvd4_agnostic_mode_set(int unit, int port, int enable);
int cprif_drv_basic_frame_parser_cw_to_encap_config_set(int unit, int port, uint32 axc_id, int fwd_cw_to_encap);
int cprif_drv_basic_frame_assembly_active_table_set(int unit, int port, uint8 table);
int cprif_drv_basic_frame_assembly_active_table_get(int unit, int port, uint8* table);
int cprif_drv_basic_frame_assembly_table_num_entries_set(int unit, int port, uint8 table, uint32 num_entries);
int cprif_drv_basic_frame_assembly_table_num_entries_get(int unit, int port, uint8 table, uint32* num_entries);

int cprif_drv_basic_frame_entry_set (int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     uint8 index,
                                     const cprif_bfa_bfp_table_entry_t *entry);

int cprif_drv_basic_frame_entry_get (int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     uint8 index,
                                     cprif_bfa_bfp_table_entry_t *entry);

int cprif_drv_basic_frame_usage_entry_add (int unit, int port,
                                           uint8 table,
                                           cprimod_direction_t dir,
                                           cprimod_basic_frame_usage_entry_t new_entry,
                                           cprimod_basic_frame_usage_entry_t *user_table,
                                           int num_entries_in_table,
                                           int *num_entries );

int cprif_drv_basic_frame_usage_entry_delete (int unit, int port,
                                              uint8 table,
                                              cprimod_direction_t dir,
                                              uint8 axc_id,
                                              cprimod_basic_frame_usage_entry_t *user_table,
                                              int num_entries_in_table,
                                              int *num_entries );
int cprif_drv_basic_frame_usage_entries_get (int unit, int port,
                                             uint8 table,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_basic_frame_usage_entry_t *user_table,
                                             int num_entries_in_table,
                                             int *num_entries );

int cprif_drv_basic_frame_usage_table_to_hw_table_set (int unit, int port,
                                                       uint8 table,
                                                       cprimod_direction_t dir,
                                                       int basic_frm_len,
                                                       cprimod_basic_frame_usage_entry_t *user_table,
                                                       int *num_entries);

int cprif_drv_basic_frame_debug_usage (int unit, int port, uint32 axc_id, uint8 table,  uint8 tx_table);
int cprif_drv_basic_frame_debug_raw(int unit, int port, uint32 axc_id, uint8 rx_table,  uint8 tx_table);

int cprif_drv_cpri_container_entry_set (int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      const cprif_cpri_container_map_entry_t *entry);
int cprif_drv_cpri_container_entry_get (int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      cprif_cpri_container_map_entry_t *entry);

int cprif_drv_cpri_container_parser_sync_info_swap (int unit, int port,
                                                    uint8 index,
                                                    cprif_container_parser_sync_info_t *sync_info);
int cprif_drv_rsvd4_container_entry_set (int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      const cprif_rsvd4_container_map_entry_t *entry);

int cprif_drv_rsvd4_container_entry_get (int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      cprif_rsvd4_container_map_entry_t *entry);
int cprif_drv_iq_buffer_config_set(int unit, int port,
                               cprimod_direction_t dir,
                               uint8 index,
                               cprif_iq_buffer_config_t* entry);

int cprif_drv_iq_buffer_config_get(int unit, int port,
                               cprimod_direction_t dir,
                               uint8 index,
                               cprif_iq_buffer_config_t* entry);

int cprif_drv_encap_data_entry_set (int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 const cprif_encap_decap_data_entry_t *entry);

int cprif_drv_encap_data_entry_get (int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 cprif_encap_decap_data_entry_t *entry);

int cprif_drv_rsvd4_encap_data_entry_set (int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 const cprif_rsvd4_encap_decap_data_entry_t *entry);

int cprif_drv_rsvd4_encap_data_entry_get (int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 cprif_rsvd4_encap_decap_data_entry_t *entry);

int cprif_drv_encap_header_entry_set (int unit, int port,
                                      uint32 queue_num,
                                      uint32 flags,
                                      const cprif_encap_header_entry_t *entry);

int cprif_drv_encap_header_entry_get (int unit, int port,
                                      uint32 queue_num,
                                      cprif_encap_header_entry_t *entry);


int cprif_drv_cprimod_to_cprif_ordering_info_type (cprimod_ordering_info_type_t cprimod_type, uint8* cprif_type);
int cprif_drv_cprif_to_cprimod_ordering_info_type (uint8 cprif_type , cprimod_ordering_info_type_t *cprimod_type);
int cprif_drv_cprimod_to_cprif_ordering_info_inc_prop (cprimod_ordering_info_prop_t  cprimod_prop, uint8 *cprif_prop);
int cprif_drv_cprif_to_cprimod_ordering_info_inc_prop (uint8 cprif_prop, cprimod_ordering_info_prop_t  *cprimod_prop);


int cprif_drv_encap_ordering_info_entry_set (int unit, int port,
                                      uint8 index,
                                      const cprif_encap_ordering_info_entry_t *entry);

int cprif_drv_encap_ordering_info_entry_get (int unit, int port,
                                      uint8 index,
                                      cprif_encap_ordering_info_entry_t *entry);

int cprif_drv_decap_ordering_info_entry_set (int unit, int port,
                                      uint8 index,
                                      const cprif_decap_ordering_info_entry_t *entry);

int cprif_drv_decap_ordering_info_entry_get (int unit, int port,
                                      uint8 index,
                                      cprif_decap_ordering_info_entry_t *entry);

int cprif_drv_decap_queue_to_ordering_info_index_set(int unit, int port,
                                                     uint32 queue,
                                                     uint32 index);

int cprif_drv_decap_queue_to_ordering_info_index_get(int unit, int port,
                                                  uint32 queue,
                                                  uint32* index);

int cprif_drv_encap_queue_ordering_info_sequence_offset_set (int unit, int port,
                                                             uint32 queue,
                                                             uint32 pq_offset);

int cprif_drv_encap_queue_ordering_info_sequence_offset_get (int unit, int port,
                                                             uint32 queue,
                                                             uint8 control,
                                                             uint32* pq_offset);

int cprif_drv_decap_queue_ordering_info_sequence_offset_set (int unit, int port,
                                                                uint32 queue,
                                                                uint32 pq_offset);

int cprif_drv_decap_queue_ordering_info_sequence_offset_get (int unit, int port,
                                                                uint32 queue,
                                                                uint32* pq_offset);



int cprif_drv_encap_mac_da_set (int unit, int port,
                                 uint32 index,
                                 uint64 mac_addr);

int cprif_drv_encap_mac_da_get (int unit, int port,
                                 uint32 index,
                                 uint64* mac_addr);

int cprif_drv_encap_mac_sa_set (int unit, int port,
                                 uint32 index,
                                 uint64 mac_addr);

int cprif_drv_encap_mac_sa_get (int unit, int port,
                                 uint32 index,
                                 uint64* mac_addr);

int cprif_drv_encap_vlan_entry_set (int unit, int port,
                                    uint32 vlan_table_num,
                                    uint32 index,
                                    uint32 vlan_id);

int cprif_drv_encap_vlan_entry_get (int unit, int port,
                                    uint32 vlan_table_num,
                                    uint32 index,
                                    uint32* vlan_id);

int cprif_drv_decap_vlan_ethtype_set (int unit, int port,
                                      uint8 ethtype_id,
                                      uint16 ethtype);

int cprif_drv_decap_vlan_ethtype_get (int unit, int port,
                                      uint8 ethtype_id,
                                      uint16* ethtype);

int cprif_drv_encap_vlan_ethtype_set (int unit, int port,
                                      uint8 ethtype_id,
                                      uint16 ethtype);

int cprif_drv_encap_vlan_ethtype_get (int unit, int port,
                                      uint8 ethtype_id,
                                      uint16* ethtype);

int cprif_drv_decap_pkt_type_config_set(int unit, int port,
                                        uint16 roe_ethtype,
                                        uint16 fast_eth_ethtype,
                                        uint16 vlan_ethtype,
                                        uint16 qinq_ethtype);

int cprif_drv_decap_pkt_type_config_get(int unit, int port,
                                        uint16* roe_ethtype,
                                        uint16* fast_eth_ethtype,
                                        uint16* vlan_ethtype,
                                        uint16* qinq_ethtype);

int cprif_drv_decap_flow_classification_entry_set (int unit, int port,
                                                   uint8 subtype,
                                                   uint32 queue_num,
                                                   uint32 option,
                                                   uint32 flow_type );
int cprif_drv_decap_flow_classification_entry_get (int unit, int port,
                                                   uint8 subtype,
                                                   uint32* queue_num,
                                                   uint32* option,
                                                   uint32* flow_type );

int cprif_drv_decap_flow_to_queue_map_set (int unit, int port,
                                           uint32 flow_id,
                                           uint32 queue_num);

int cprif_drv_decap_flow_to_queue_map_get (int unit, int port,
                                           uint32 flow_id,
                                           uint32* queue_num);

int cprif_drv_tx_framer_tgen_tick_set (int unit, int port,
                                       uint32 divider,
                                       uint32 bfrm_ticks,
                                       uint32 bfrm_ticks_bitmap,
                                       uint32 bitmap_size);

int cprif_drv_tx_framer_tgen_tick_get (int unit, int port,
                                       uint32* divider,
                                       uint32* bfrm_ticks,
                                       uint32* bfrm_ticks_bitmap,
                                       uint32* bitmap_size);

int cprif_drv_tx_framer_tgen_enable (int unit, int port,
                                     uint8 enable);
int cprif_drv_tx_framer_tgen_offset_set (int unit, int port,
                                         uint64  offset);
int cprif_drv_tx_framer_tgen_offset_get (int unit, int port,
                                         uint64*  offset);
int cprif_drv_tx_framer_tgen_next_bfn_config_set (int unit, int port,
                                                  uint32  bfn);
int cprif_drv_tx_framer_tgen_next_bfn_config_get (int unit, int port,
                                                  uint32*  bfn);
int cprif_drv_tx_framer_tgen_next_hfn_config_set (int unit, int port,
                                           uint32  hfn);
int cprif_drv_tx_framer_tgen_next_hfn_config_get (int unit, int port,
                                           uint32*  hfn);
int cprif_drv_tx_framer_next_bfn_get (int unit, int port,
                                                  uint32*  bfn);
int cprif_drv_tx_framer_next_hfn_get (int unit, int port,
                                                  uint32*  bfn);

int cprif_drv_tx_framer_tgen_select_counter_set (int unit, int port,
                                                 uint8  hfn_bfn);
int cprif_drv_tx_framer_tgen_select_counter_get (int unit, int port,
                                                 uint8*  hfn_bfn);

int cprif_drv_rsvd4_header_compare_entry_set (int unit, int port,
                                              int index,
                                              cprimod_header_compare_entry_t* entry);
int cprif_drv_rsvd4_header_compare_entry_get (int unit, int port,
                                              int index,
                                              cprimod_header_compare_entry_t* entry);
int cprif_drv_rsvd4_header_compare_entry_valid_bit_swap (int unit, int port,
                                                     int index,
                                                     uint32* valid_bit);
int cprif_drv_rsvd4_modulo_rule_entry_set (int unit, int port,
                                           uint8  index,
                                           uint8  control_rule,
                                           cprimod_modulo_rule_entry_t* entry);
int cprif_drv_rsvd4_modulo_rule_entry_get (int unit, int port,
                                           uint8  index,
                                           uint8  control_rule,
                                           cprimod_modulo_rule_entry_t* entry);

int cprif_drv_rsvd4_dual_bitmap_rule_entry_set (int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_rule_entry_t* entry);
int cprif_drv_rsvd4_dual_bitmap_rule_entry_get (int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_rule_entry_t* entry);

int cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_set (int unit, int port,
                                           uint8  index,
                                           cprimod_secondary_dbm_rule_entry_t* entry);
int cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_get (int unit, int port,
                                           uint8  index,
                                           cprimod_secondary_dbm_rule_entry_t* entry);

int cprif_drv_rsvd4_dbm_position_entry_set(int unit, int port,
                                       uint8  index,
                                       cprimod_dbm_pos_table_entry_t* entry);

int cprif_drv_rsvd4_dbm_position_entry_get(int unit, int port,
                                       uint8  index,
                                       cprimod_dbm_pos_table_entry_t* entry);

int cprif_drv_encap_queue_sts_clear(int unit, int port,
                                    int  queue_num);

int cprif_drv_encap_ordering_info_sts_clear(int unit, int port,
                                            int  queue_num);

int cprif_drv_encap_eth_hdr_clear(int unit, int port,
                                  int  queue_num);

int cprif_drv_encap_data_tab_clear(int unit, int port,
                                   int  queue_num);

int cprif_drv_container_parser_mapp_state_tab_clear(int unit, int port,
                                                    int axc_id);

int cprif_drv_container_parser_map_ts_prev_valid_set(int unit, int port, 
                                                     int flow_id, 
                                                     uint32 valid);

int cprif_drv_iq_pak_buff_state_tab_clear(int unit, int port,
                                          int axc_id);

int cprif_drv_iq_pak_buff_payld_tab_clear(int unit, int port,
                                          int axc_id);

int cprif_drv_iq_unpsk_buff_state_tbl_clear(int unit, int port,
                                            int axc_id);

int cprif_drv_cont_assembly_map_state_tbl_clear(int unit, int port,
                                                int axc_id);

int cprif_drv_decap_queue_state_tbl_clear(int unit, int port,
                                          int queue_num);

int cprif_drv_decap_queue_data_tbl_clear(int unit, int port,
                                         int queue_num);

int cprif_drv_rxpcs_interface_width_get(int unit, int port, int *is_10bit);
int cprif_drv_rxpcs_tunnel_mode_set(int unit, int port, int enable,
                                    uint32 timestamp_interval,
                                    int restore_rsvd4,
                                    cprimod_port_speed_t restore_speed);
int cprif_drv_rxpcs_tunnel_mode_get(int unit, int port, int* enable, uint32* timestamp_interval);
int cprif_drv_txpcs_agnostic_mode_set(int unit, int port,
                                      int enable,
                                      cprimod_agnostic_mode_type_t mode,
                                      uint32 timestamp_interval,
                                      int restore_rsvd4,
                                      cprimod_port_speed_t restore_spee);
int cprif_drv_txpcs_agnostic_mode_get(int unit, int port, int* enable, cprimod_agnostic_mode_type_t* mode,  uint32* timestamp_interval);

int cprif_drv_txpcs_tunnel_mode_set(int unit, int port, int enable, uint32 timestamp_interval);
int cprif_drv_txpcs_tunnel_mode_get(int unit, int port, int* enable, uint32* timestamp_interval);

int cprif_drv_cpri_rxpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                   cprimod_port_speed_t speed);

int cprif_drv_cpri_rxpcs_speed_get(int unit, int port,
                                   cprimod_port_speed_t *speed);

int cprif_drv_cpri_txpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                   cprimod_port_speed_t speed);

int cprif_drv_cpri_txpcs_speed_get(int unit, int port,
                                   cprimod_port_speed_t *speed);

int cprif_drv_rsvd4_rxpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                    cprimod_port_rsvd4_speed_mult_t speed);

int cprif_drv_rsvd4_rxpcs_speed_get(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t *speed);

int cprif_drv_rsvd4_txpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                    cprimod_port_rsvd4_speed_mult_t speed);

int cprif_drv_rsvd4_txpcs_speed_get(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t *speed);

int cprif_drv_txcpri_port_type_get(int unit, int port,
                                   cprimod_port_interface_type_t *interface);

int cprif_drv_rxcpri_port_type_get(int unit, int port,
                                   cprimod_port_interface_type_t *interface);

int cprif_drv_rx_frame_optional_config_set(int unit, int port,
                                           cprimod_rx_config_field_t field,
                                           int value);

int cprif_drv_rx_frame_optional_config_get(int unit, int port,
                                           cprimod_rx_config_field_t field,
                                           int* value);

int cprif_drv_tx_frame_optional_config_set(int unit, int port,
                                           cprimod_tx_config_field_t field,
                                           uint32 value);

int cprif_drv_tx_frame_optional_config_get(int unit, int port,
                                           cprimod_tx_config_field_t field,
                                           uint32* value);

int cprif_drv_rsvd4_rx_frame_optional_config_set(int unit, int port,
                                                 cprimod_rsvd4_rx_config_field_t field,
                                                 int value);

int cprif_drv_rsvd4_rx_frame_optional_config_get(int unit, int port,
                                                 cprimod_rsvd4_rx_config_field_t field,
                                                 int* value);

int cprif_drv_rsvd4_rx_fsm_state_set(int unit, int port,
                                     cprimod_rsvd4_rx_fsm_state_t state);

int cprif_drv_rsvd4_rx_fsm_state_get(int unit, int port,
                                     cprimod_rsvd4_rx_fsm_state_t* state);

int cprif_drv_rsvd4_rx_overide_set(int unit, int port,
                                   cprimod_rsvd4_rx_overide_t parameter,
                                   int enable, int value);

int cprif_drv_rsvd4_rx_overide_get(int unit, int port,
                                   cprimod_rsvd4_rx_overide_t parameter,
                                   int* enable, int* value);

int cprif_drv_rsvd4_tx_frame_optional_config_set(int unit, int port,
                                                 cprimod_rsvd4_tx_config_field_t field,
                                                 int value);

int cprif_drv_rsvd4_tx_frame_optional_config_get(int unit, int port,
                                                 cprimod_rsvd4_tx_config_field_t field,
                                                 int* value);

int cprif_drv_rsvd4_tx_fsm_state_set(int unit, int port,
                                     cprimod_rsvd4_tx_fsm_state_t state);

int cprif_drv_rsvd4_tx_fsm_state_get(int unit, int port,
                                     cprimod_rsvd4_tx_fsm_state_t* state);

int cprif_drv_rsvd4_tx_overide_set(int unit, int port,
                                   cprimod_rsvd4_tx_overide_t parameter,
                                   int enable, int value);

int cprif_drv_rsvd4_tx_overide_get(int unit, int port,
                                   cprimod_rsvd4_tx_overide_t parameter,
                                   int* enable, int* value);

int cprif_drv_pmd_port_status_get(int unit, int port, cprimod_pmd_port_status_t status,
                                uint32* value);

int cprif_drv_rx_pcs_status_get(int unit, int port,
                                cprimod_rx_pcs_status_t status, uint32* value);

int cprif_drv_cip_top_ctrl_set(int unit, int port, int val);

int cprif_drv_cip_top_ctrl_get(int unit, int port, int *val);

int cprif_drv_pmd_iddq_set(int unit, int port, int val);

int cprif_drv_pmd_iddq_get(int unit, int port, int *val);

int cprif_drv_top_ctrl_config_pll_ctrl_set(int unit, int port, int pll_num,
                                           int cip_pll_ctrl);

int cprif_drv_top_ctrl_config_pll_ctrl_get(int unit, int port, int pll_num,
                                           int *cip_pll_ctrl);

int cprif_drv_top_ctrl_config_ref_clk_ctrl_set(int unit, int port, int pll_num,
                                               int refin, int refout);

int cprif_drv_top_ctrl_config_ref_clk_ctrl_get(int unit, int port, int pll_num,
                                               int *refin, int *refout);

int cprif_drv_pmd_ctrl_por_h_rstb_set(int unit, int port, int por_h_rstb);

int cprif_drv_pmd_ctrl_ln_por_h_rstb_set(int unit, int port);

int cprif_drv_pmd_ctrl_por_h_rstb_get(int unit, int port, int *por_h_rstb);

int cprif_drv_pmd_core_datapath_hard_reset_set(int unit, int port, int pll_num, int rst);

int cprif_drv_pmd_core_datapath_hard_reset_get(int unit, int port, int pll_num, int *rst);

int cprif_drv_cpmport_pll_ctrl_config_set(int unit, int port, int pll_num, int lcref_sel);

int cprif_drv_cpmport_pll_ctrl_config_get(int unit, int port, int pll_num, int *lcref_sel);

int cprif_drv_lane_swap_set(int unit, int port, int lane, cprimod_direction_t dir,
                            int phyport);

int cprif_drv_lane_swap_get(int unit, int port, int lane, cprimod_direction_t dir,
                            int *phyport);

int cprif_drv_rsvd4_rx_master_frame_sync_config_set(int unit, int port,
                                                    uint32  master_frame_number,
                                                    uint64  master_frame_start_time);

int cprif_drv_rsvd4_rx_master_frame_sync_config_get(int unit, int port,
                                                    uint32*  master_frame_number,
                                                    uint64*  master_frame_start_time);
int cprif_drv_rx_control_word_status_get(int unit, int port, cprif_drv_rx_control_word_status_t* info);
int cprif_drv_cpri_rx_l1_signal_signal_protection_set(int unit, int port, uint32 signal_map, uint32 enable);
int cprif_drv_cpri_rx_l1_signal_signal_protection_get(int unit, int port, uint32 signal_map, uint32* enable);
int cprif_drv_cpri_port_rx_cw_slow_hdlc_config_set(int unit, int port, cprif_cpri_rx_hdlc_config_t* config_info);
int cprif_drv_cpri_port_rx_cw_slow_hdlc_config_get(int unit, int port, cprif_cpri_rx_hdlc_config_t* config_info);
int cprif_drv_cpri_port_tx_cw_slow_hdlc_config_set(int unit, int port, cprif_cpri_tx_hdlc_config_t* config_info);
int cprif_drv_cpri_port_tx_cw_slow_hdlc_config_get(int unit, int port, cprif_cpri_tx_hdlc_config_t* config_info);
int cprif_drv_cpri_port_rx_cw_fast_eth_word_config_set(int unit, int port, cprif_cpri_rx_fast_eth_word_config_t* config_info);
int cprif_drv_cpri_port_rx_cw_fast_eth_word_config_get(int unit, int port, cprif_cpri_rx_fast_eth_word_config_t* config_info);
int cprif_drv_cpri_port_rx_cw_hdlc_fast_eth_sizes_swap(int unit, int port, uint32* hdlc_size, uint32* fast_eth_size);
int cprif_drv_cpri_port_rx_cw_fast_eth_config_set(int unit, int port, cprif_cpri_rx_fast_eth_config_t* config_info);
int cprif_drv_cpri_port_rx_cw_fast_eth_config_get(int unit, int port, cprif_cpri_rx_fast_eth_config_t* config_info);
int cprif_drv_cpri_port_tx_cw_fast_eth_config_set(int unit, int port, cprif_cpri_tx_fast_eth_config_t* config_info);
int cprif_drv_cpri_port_tx_cw_fast_eth_config_get(int unit, int port, cprif_cpri_tx_fast_eth_config_t* config_info);

int cprif_drv_rx_vsd_ctrl_copy_id_get(int unit, int port, uint32* copy_id);
int cprif_drv_rx_vsd_ctrl_copy_id_set(int unit, int port, uint32 copy_id);
int cprif_drv_rx_vsd_ctrl_config_set(int unit, int port, cprif_cpri_vsd_config_t* config_info);
int cprif_drv_rx_vsd_ctrl_config_get(int unit, int port, cprif_cpri_vsd_config_t* config_info);
int cprif_drv_rx_vsd_ctrl_rsvd_mask_set(int unit, int port, uint32* rsvd_mask);
int cprif_drv_rx_vsd_ctrl_rsvd_mask_get(int unit, int port, uint32* rsvd_mask);
int cprif_drv_rx_vsd_ctrl_ctrl_mask_set(int unit, int port, uint32 copy_id, uint32* ctrl_mask);
int cprif_drv_rx_vsd_ctrl_ctrl_mask_get(int unit, int port, uint32 copy_id, uint32* ctrl_mask);
int cprif_drv_rx_vsd_ctrl_flow_config_set(int unit, int port,
                                          uint32 sector_num,
                                          const cprif_cpri_rx_vsd_ctrl_flow_config_t *entry);
int cprif_drv_rx_vsd_ctrl_flow_config_get(int unit, int port,
                                          uint32 sector_num,
                                          cprif_cpri_rx_vsd_ctrl_flow_config_t *entry);
int cprif_drv_rx_vsd_ctrl_group_num_sector_set(int unit, int port,
                                               uint32 group_id,
                                               uint32 num_sector);
int cprif_drv_rx_vsd_ctrl_group_num_sector_get(int unit, int port,
                                               uint32 group_id,
                                               uint32* num_sector);
int cprif_drv_rx_vsd_ctrl_group_config_set(int unit, int port,
                                           uint32 group_id,
                                           uint32 tag_id,
                                           uint32 grp_ptr_index);
int cprif_drv_rx_vsd_ctrl_group_config_get(int unit, int port,
                                           uint32 group_id,
                                           uint32* tag_id,
                                           uint32* grp_ptr_index);
int cprif_drv_rx_vsd_ctrl_group_assign_ptr_set(int unit, int port,
                                               uint32 ptr_index,
                                               uint32 sec_num);
int cprif_drv_rx_vsd_ctrl_group_assign_ptr_get(int unit, int port,
                                               uint32 ptr_index,
                                               uint32* sec_num);
int cprif_drv_allocate_grp_ptr (uint32* grp_ptr_mask, uint32 num_sec, uint32* ptr_index);


int cprif_drv_tx_vsd_ctrl_copy_id_get(int unit, int port, uint32* copy_id);
int cprif_drv_tx_vsd_ctrl_copy_id_set(int unit, int port, uint32 copy_id);
int cprif_drv_tx_vsd_ctrl_rsvd_mask_set(int unit, int port, uint32* rsvd_mask);
int cprif_drv_tx_vsd_ctrl_rsvd_mask_get(int unit, int port, uint32* rsvd_mask);
int cprif_drv_tx_vsd_ctrl_ctrl_mask_set(int unit, int port, uint32 copy_id, uint32* ctrl_mask);
int cprif_drv_tx_vsd_ctrl_ctrl_mask_get(int unit, int port, uint32 copy_id, uint32* ctrl_mask);
int cprif_drv_tx_vsd_ctrl_flow_config_set(int unit, int port,
                                       uint32 sector_num,
                                       const cprif_cpri_tx_vsd_ctrl_flow_config_t *entry);
int cprif_drv_tx_vsd_ctrl_flow_config_get(int unit, int port,
                                       uint32 sector_num,
                                       cprif_cpri_tx_vsd_ctrl_flow_config_t *entry);
int cprif_drv_tx_vsd_ctrl_flow_id_to_group_mapping_set(int unit, int port,
                                                       uint32 roe_flow_id,
                                                       uint32 group_id);
int cprif_drv_tx_vsd_ctrl_flow_id_to_group_mapping_get(int unit, int port,
                                                       uint32 roe_flow_id,
                                                       uint32* group_id);
int cprif_drv_tx_vsd_ctrl_group_config_set(int unit, int port,
                                           uint32 group_id,
                                           uint32 num_sector,
                                           uint32 grp_ptr_index);
int cprif_drv_tx_vsd_ctrl_group_config_get(int unit, int port,
                                           uint32 group_id,
                                           uint32* num_sector,
                                           uint32* grp_ptr_index);
int cprif_drv_tx_vsd_ctrl_group_assign_ptr_set(int unit, int port,
                                               uint32 ptr_index,
                                               uint32 sec_num);
int cprif_drv_tx_vsd_ctrl_group_assign_ptr_get(int unit, int port,
                                               uint32 ptr_index,
                                               uint32* sec_num);
int cprif_drv_encap_tag_to_flow_id_set(int unit, int port,
                                       uint32 tag_id,
                                       uint32 flow_ida);
int cprif_drv_encap_tag_to_flow_id_get(int unit, int port,
                                       uint32 tag_id,
                                       uint32* flow_id);
int cprif_drv_tx_vsd_ctrl_config_set(int unit, int port, const cprif_cpri_vsd_config_t* config_info);
int cprif_drv_tx_vsd_ctrl_config_get(int unit, int port, cprif_cpri_vsd_config_t* config_info);
int cprif_drv_rx_vsd_raw_config_set(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config);
int cprif_drv_rx_vsd_raw_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config);
int cprif_drv_rx_vsd_raw_config_cw_sel_size_swap(int unit, int port,
                                                 uint32 index,
                                                 uint32* cw_sel,
                                                 uint32* cw_size);
int cprif_drv_rx_vsd_raw_filter_config_set(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_rx_vsd_raw_filter_config_t* config);
int cprif_drv_rx_vsd_raw_filter_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_rx_vsd_raw_filter_config_t* config);


int cprif_drv_tx_vsd_raw_config_set(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config);
int cprif_drv_tx_vsd_raw_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config);
int cprif_drv_tx_vsd_raw_filter_config_set(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_tx_vsd_raw_filter_config_t* config);
int cprif_drv_tx_vsd_raw_filter_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_tx_vsd_raw_filter_config_t* config);
int cprif_drv_tx_vsd_raw_radio_frame_filter_set(int inst_num, int port_num,
                                                uint32 bfn0_value,
                                                uint32 bfn0_mask,
                                                uint32 bfn1_value,
                                                uint32 bfn1_mask);
int cprif_drv_tx_vsd_raw_radio_frame_filter_get(int unit, int port_num,
                                                uint32* bfn0_value,
                                                uint32* bfn0_mask,
                                                uint32* bfn1_value,
                                                uint32* bfn1_mask);
int cprif_drv_rx_brcm_rsvd5_config_set(int unit, int port, uint32 schan_start, uint32 schan_size, uint32 queue_num, int parity_disable);
int cprif_drv_rx_brcm_rsvd5_config_get(int unit, int port, uint32* schan_start, uint32* schan_size, uint32*queue_num, int* parity_disable);
int cprif_drv_rx_brcm_rsvd5_config_size_swap (int unit, int port,
                                              uint32* schan_size);
int cprif_drv_tx_brcm_rsvd5_config_set(int unit, int port,
                                       uint32 schan_start,
                                       uint32 schan_size,
                                       uint32 queue_num,
                                       int crc_enable);
int cprif_drv_tx_brcm_rsvd5_config_get(int unit, int port,
                                       uint32* schan_start,
                                       uint32* schan_size,
                                       uint32* queue_num,
                                       int* crc_enable);

int cprif_drv_rx_gcw_config_set(int unit, int port,
                                uint32 index,
                                cprif_cpri_rx_gcw_config_t* config);
int cprif_drv_rx_gcw_config_get(int unit, int port,
                                uint32 index,
                                cprif_cpri_rx_gcw_config_t* config);
int cprif_drv_tx_gcw_config_set(int unit, int port,
                                uint32 index,
                                cprif_cpri_tx_gcw_config_t* config);
int cprif_drv_tx_gcw_config_get(int unit, int port,
                                uint32 index,
                                cprif_cpri_tx_gcw_config_t* config);
int cprif_drv_tx_gcw_filter_set(int unit, int port,
                                   uint32 bfn0_value, uint32 bfn0_mask,
                                   uint32 bfn1_value, uint32 bfn1_mask);
int cprif_drv_tx_gcw_filter_get(int unit, int port,
                                   uint32* bfn0_value, uint32* bfn0_mask,
                                   uint32* bfn1_value, uint32* bfn1_mask);
int cprif_drv_tx_control_word_set(int unit, int port,
                                 cprif_drv_tx_control_word_t* control);
int cprif_drv_tx_control_word_get(int unit, int port,
                                 cprif_drv_tx_control_word_t* control);

int cprif_drv_tx_control_word_size_set(int unit, int port, 
                                       uint32 cw_size_in_bytes);


/* RSVD4 control */
int cprif_drv_rsvd4_rx_control_message_config_set( int unit, int port,
                                                   uint32 sm_queue_num,
                                                   uint32 default_tag,
                                                   uint32 no_match_tag);

int cprif_drv_rsvd4_rx_control_flow_config_set ( int unit, int port,
                                                 uint32 control_flow_id,
                                                 cprif_rsvd4_rx_ctrl_flow_config_t* config);
int cprif_drv_rsvd4_rx_control_flow_config_get( int unit, int port,
                                                uint32 control_flow_id,
                                                cprif_rsvd4_rx_ctrl_flow_config_t* config);

int cprif_drv_rx_control_flow_tag_option_set(int unit, int port,
                                             uint32 control_flow_id,
                                             uint32 tag_option);

int cprif_drv_rx_sync_profile_entry_set (int unit, int port,
                                          uint32 profile_id,
                                          cprif_rsvd4_rx_sync_profile_entry_t* profile);
int cprif_drv_rx_sync_profile_entry_get (int unit, int port,
                                          uint32 profile_id,
                                          cprif_rsvd4_rx_sync_profile_entry_t* profile);

int cprif_drv_rx_tag_config_set ( int unit, int port,
                                  uint32 default_tag,
                                  uint32 no_match_tag);

int cprif_drv_rx_tag_gen_entry_set( int unit, int port,
                                    uint32 index,
                                    cprif_rx_tag_gen_entry_t* entry);
int cprif_drv_rx_tag_gen_entry_get( int unit, int port,
                                    uint32 index,
                                    cprif_rx_tag_gen_entry_t* entry);
int cprif_drv_brcm_rsvd4_tx_control_group_entry_set( int unit, int port,
                                                  uint32 index,
                                                  cprif_control_group_entry_t* entry);
int cprif_drv_brcm_rsvd4_tx_control_group_entry_get( int unit, int port,
                                                  uint32 index,
                                                  cprif_control_group_entry_t* entry);

int cprif_drv_tx_control_eth_msg_config_set(int unit, int port,
                                            uint32 msg_node,
                                            uint32 msg_subnode,
                                            uint32 msg_type,
                                            uint32 msg_padding);
int cprif_drv_tx_control_eth_msg_config_get(int unit, int port,
                                            uint32* msg_node,
                                            uint32* msg_subnode,
                                            uint32* msg_type,
                                            uint32* msg_padding);
int cprif_drv_tx_control_single_msg_config_set(int unit, int port,
                                            uint32 msg_id,
                                            uint32 msg_type);
int cprif_drv_tx_control_single_msg_config_get(int unit, int port,
                                            uint32* msg_id,
                                            uint32* msg_type);
int cprif_drv_tx_control_single_tunnel_msg_config_set(int unit, int port,
                                            uint32 crc_option);

int cprif_drv_brcm_rsvd4_tx_control_flow_config_set( int unit, int port,
                                                 uint32 index,
                                                 uint32 queue_num,
                                                 uint32 proc_type);
int cprif_drv_brcm_rsvd4_tx_control_flow_config_get( int unit, int port,
                                                 uint32 index,
                                                 uint32* queue_num,
                                                 uint32* proc_type);

int cprif_drv_brcm_rsvd4_control_flow_header_index_set( int unit, int port,
                                                        uint32 flow_id,
                                                        uint32 header_index);
int cprif_drv_brcm_rsvd4_control_flow_header_index_get( int unit, int port,
                                                        uint32 flow_id,
                                                        uint32* header_index);

int cprif_drv_brcm_rsvd4_control_header_entry_set( int unit, int port,
                                                   uint32 index,
                                                   uint32 header_node,
                                                   uint32 header_subnode,
                                                   uint32 node);

int cprif_drv_brcm_rsvd4_control_header_entry_get( int unit, int port,
                                                   uint32 index,
                                                   uint32* header_node,
                                                   uint32* header_subnode,
                                                   uint32* node);

int cprif_drv_datapath_credit_reset_set(int unit, int port, int val);

int cprif_drv_datapath_credit_reset_get(int unit, int port, int* val);

int cprif_drv_cip_rx_dp_reset_set(int unit, int port, int val);

int cprif_drv_cip_rx_dp_reset_get(int unit, int port, int* val);

int cprif_drv_cip_tx_dp_reset_set(int unit, int port, int val);

int cprif_drv_cip_tx_dp_reset_get(int unit, int port, int* val);

int cprif_drv_cip_tx_h_reset_set(int unit, int port, int val);

int cprif_drv_cip_tx_h_reset_get(int unit, int port, int* val);

int cprif_drv_cip_rx_h_reset_set(int unit, int port, int val);

int cprif_drv_cip_rx_h_reset_get(int unit, int port, int* val);

int cprif_drv_datapath_rx_dp_reset_set(int unit, int port, int val);

int cprif_drv_datapath_rx_dp_reset_get(int unit, int port, int* val);

int cprif_drv_datapath_tx_dp_reset_set(int unit, int port, int val);

int cprif_drv_datapath_tx_dp_reset_get(int unit, int port, int* val);

int cprif_drv_datapath_rx_h_reset_set(int unit, int port, int val);

int cprif_drv_datapath_rx_h_reset_get(int unit, int port, int* val);

int cprif_drv_datapath_tx_h_reset_set(int unit, int port, int val);

int cprif_drv_datapath_tx_h_reset_get(int unit, int port, int* val);

int cprif_drv_rx_dp_reset_set(int unit, int port, int val);

int cprif_drv_rx_dp_reset_get(int unit, int port, int* val);

int cprif_drv_rx_h_reset_set(int unit, int port, int val);

int cprif_drv_rx_h_reset_get(int unit, int port, int* val);

int cprif_drv_tx_dp_reset_set(int unit, int port, int val);

int cprif_drv_tx_dp_reset_get(int unit, int port, int* val);

int cprif_drv_tx_h_reset_set(int unit, int port, int val);

int cprif_drv_tx_h_reset_get(int unit, int port, int* val);

int cprif_drv_glas_reset_set(int unit, int port, int val);

int cprif_drv_glas_reset_get(int unit, int port, int* val);

int cprif_drv_disable_cip_clk_set(int unit, int port, int val);

int cprif_drv_disable_cip_clk_get(int unit, int port, int* val);

int cprif_drv_disable_glas_clk_set(int unit, int port, int val);

int cprif_drv_disable_glas_clk_get(int unit, int port, int* val);

int cprif_drv_disable_rx_clk_set(int unit, int port, int val);

int cprif_drv_disable_rx_clk_get(int unit, int port, int* val);

int cprif_drv_disable_tx_clk_set(int unit, int port, int val);

int cprif_drv_disable_tx_clk_get(int unit, int port, int* val);

int cprif_drv_cip_clk_pll_select_set(int unit, int port, int val);

int cprif_drv_cip_clk_pll_select_get(int unit, int port, int* val);

int cprif_drv_cip_clk_div_ctrl_set(int unit, int port, int val);

int cprif_drv_cip_clk_div_ctrl_get(int unit, int port, int* val);

int cprif_drv_tx_flush_set(int unit, int port, int val);

int cprif_drv_tx_flush_get(int unit, int port, int* val);

int cprif_drv_rx_disable_set(int unit, int port, int val);

int cprif_drv_rx_disable_get(int unit, int port, int* val);

int cprif_drv_cpri_or_enet_port_set(int unit, int port, int val);

int cprif_drv_cpri_or_enet_port_get(int unit, int port, int* val);

int cprif_drv_stuff_rsrv_bits_set( int unit, int port,
                                   int roe_stuffing_bit,
                                   int roe_reserved_bit);

int cprif_drv_pmd_reset_seq(int unit, int port, int pmd_touched);

int cprif_drv_cpri_port_interrupt_enable_set(int unit, int port,
                                             int int_dir, int enable);
int cprif_drv_cpri_port_interrupt_enable_get(int unit, int port,
                                             int int_dir, int *enable);
int cprif_drv_cpri_port_interrupt_status_get(int unit, int port, int *status);

int cprif_drv_cpri_port_rx_interrupts_enable_set(int unit,
                                                 int port,
                                                 int rx_intr_flags,
                                                 int enable);
int cprif_drv_cpri_port_rx_interrupt_enable_get(int unit,
                                                int port,
                                                int rx_intr_flags,
                                                int *status);
int cprif_drv_cpri_port_rx_func_interrupts_enable_set(int unit,
                                                      int port,
                                                      int rx_func_intr_flags,
                                                      int enable);
int cprif_drv_cpri_port_rx_func_interrupts_enable_get(int unit,
                                                      int port,
                                                      int rx_func_intr_flags,
                                                      int *status);
/* Enable interrupt to detect link down - LOS signal is latched low */
int cprif_drv_rx_pcs_link_down_intr_enable_set(int unit,
                                               int port,
                                               int enable);
/* Enable interrupt to detect link up - LOS signal is latched low */
int cprif_drv_rx_pcs_link_up_intr_enable_set(int unit,
                                               int port,
                                               int enable);
/* Get interrupt enable Status to detect LOS signal - latched high */
int cprif_drv_rx_pcs_link_up_intr_enable_get(int unit,
                                               int port,
                                               int *status);
int cprif_drv_rx_pcs_link_up_intr_status_get(int unit,
                                               int port,
                                               int *status);
int cprif_drv_rx_pcs_link_up_intr_status_clr_get(int unit,
                                                 int port,
                                                 int *status);
/* Get interrupt enable Status to detect LOS signal - latched low */
int cprif_drv_rx_pcs_link_down_intr_enable_get(int unit,
                                               int port,
                                               int *status);
int cprif_drv_rx_pcs_link_down_intr_status_get(int unit,
                                               int port,
                                               int *status);
int cprif_drv_rx_pcs_link_down_intr_status_clr_get(int unit,
                                                   int port,
                                                   int *status);

int cprif_drv_rx_1588_fifo_intr_enable_set(int unit, int port, int enable);

int cprif_drv_rx_1588_fifo_intr_enable_get(int unit, int port, int *status);

int cprif_drv_tx_1588_fifo_intr_enable_set(int unit, int port, int enable);
int cprif_drv_tx_1588_fifo_intr_enable_get(int unit, int port, int *status);

int cprif_drv_rx_1588_sw_intr_enable_set(int unit, int port, int enable);

int cprif_drv_rx_1588_sw_intr_enable_get(int unit, int port, int *status);

int cprif_drv_rx_1588_sw_intr_status_clr_get(int unit, int port,
                                             uint64 *rx_captured_ts,
                                             int *status);
int cprif_drv_tx_1588_sw_intr_enable_set(int unit, int port, int enable);
int cprif_drv_tx_1588_sw_intr_enable_get(int unit, int port, int *status);

int cprif_drv_fast_clk_bit_time_period_set(int unit, int port, uint32 bit_time_period);
int cprif_drv_fast_clk_bit_time_period_get(int unit, int port, uint32* bit_time_period);
int cprif_drv_1588_bit_time_period_set(int unit, int port, cprimod_direction_t direction, cprimod_port_speed_t speed);
int cprif_drv_1588_bit_time_period_get(int unit, int port, cprimod_direction_t direction, uint32* bit_time_period);
int cprif_drv_1588_timestamp_capture_config_set(int unit, int port, cprimod_direction_t direction, cprimod_1588_capture_config_t *config);
int cprif_drv_1588_timestamp_capture_config_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_capture_config_t* config);
int cprif_drv_1588_captured_timestamp_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_ts_type_t ts_type, uint8 mx_cnt, uint8* count, uint64* captured_time);
int cprif_drv_1588_timestamp_adjust_set(int unit, int port, cprimod_direction_t direction, cprimod_1588_time_t *adjust_time);
int cprif_drv_1588_timestamp_adjust_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_time_t* adjust_time);
int cprif_drv_1588_timestamp_fifo_config_set(int unit, int port, cprimod_direction_t direction, uint16 modulo);
int cprif_drv_1588_timestamp_fifo_config_get(int unit, int port, cprimod_direction_t direction, uint16* modulo);
int cprif_drv_1588_timestamp_cmic_config_set(int unit, int port, cprimod_direction_t direction, uint16 modulo);
int cprif_drv_1588_timestamp_cmic_config_get(int unit, int port, cprimod_direction_t direction, uint16* modulo);

int cprif_drv_ingress_presentation_time_enable_set(int unit, int port, int enable);
int cprif_drv_ingress_presentation_time_enable_get(int unit, int port, int* enable);
int cprif_drv_ingress_presentation_time_config_set(int unit, int port, cprif_cpri_ingress_pres_time_ctrl_t* config);
int cprif_drv_ingress_presentation_time_config_get(int unit, int port, cprif_cpri_ingress_pres_time_ctrl_t* config);
int cprif_drv_ingress_presentation_time_adjust_set(int unit, int port, uint32 time);
int cprif_drv_ingress_presentation_time_adjust_get(int unit, int port, uint32* time);
int cprif_drv_ingress_presentation_time_approximate_increment_set(int unit, int port, uint32 time);
int cprif_drv_ingress_presentation_time_approximate_increment_get(int unit, int port, uint32* time);
int cprif_drv_egress_agnostic_mode_enable_set(int unit, int port, int enable,  cprimod_cpri_roe_ordering_info_option_t mode);
int cprif_drv_egress_agnostic_mode_enable_get(int unit, int port, int* enable, cprimod_cpri_roe_ordering_info_option_t* mode);
int cprif_drv_egress_agnostic_mode_config_set(int unit, int port, cprif_cpri_egress_agnostic_config_t* config);
int cprif_drv_egress_agnostic_mode_config_get(int unit, int port, cprif_cpri_egress_agnostic_config_t* config);
int cprif_drv_egress_presentation_time_config_set(int unit, int port, uint32 mod_offset, uint32 mod_count);
int cprif_drv_egress_presentation_time_config_get(int unit, int port, uint32* mod_offset, uint32* mod_count);

int cprif_drv_cpri_port_interrupt_type_num_subid_get ( int unit, int port,
                                                       cprimod_cpri_port_intr_type_t intr,
                                                       int* num_subid);

int cprif_drv_cpri_port_interrupt_type_enable_set(int unit, int port,
                                            cprimod_cpri_port_intr_type_t intr,
                                            int data, int enable);
int cprif_drv_cpri_port_interrupt_type_enable_get(int unit, int port,
                                            cprimod_cpri_port_intr_type_t intr,
                                            int data, int *enable);
int cprif_drv_cpri_port_interrupt_type_status_get(int unit, int port,
                                            cprimod_cpri_port_intr_type_t intr,
                                            int data, int *status);

/*
 * Enable CPRI port RX Operational error interrupts.
 * RX Lane Operational Error Interrupts.
 * 1. RX Lane Combined DATA Queue Overflow Error
 * 2. RX Lane Combined Control Queue Overflow Error
 * 3. RX Lane Combined GSM Time-stamp Error
 * 4. RX Lane MISC WQ and CDC Overflow Error
 * 5. RX RX Lane RxFramer BRCM_RSVD4 Time-stamp
 *    detection Error
 */
extern
int cprif_drv_cpri_port_rx_op_err_interrupts_enable_set(int unit,
                                                      int port,
                                                      int rx_op_err_intr_flags,
                                                      int enable);

/*
 * Enable CPRI port RX Operational error interrupts.
 * RX Lane Operational Error Interrupts.
 * 1. RX Lane Combined DATA Queue Overflow Error
 * 2. RX Lane Combined Control Queue Overflow Error
 * 3. RX Lane Combined GSM Time-stamp Error
 * 4. RX Lane MISC WQ and CDC Overflow Error
 * 5. RX RX Lane RxFramer BRCM_RSVD4 Time-stamp
 *    detection Error
 */
extern
int cprif_drv_cpri_port_rx_op_err_interrupts_enable_get(int unit,
                                                  int port,
                                                  int rx_op_err_intr_flags,
                                                  int *status);

/*
 * Enable CPRI port TX interrupts.
 * TX interrupts can be of 3 types
 * 1. TX operational error type interrupts.
 * 2. TX functional error type interrupts.
 * 3. TX memory ECC error type interrupts.
 */
extern
int cprif_drv_cpri_port_tx_interrupts_enable_set(int unit,
                                                 int port,
                                                 int tx_intr_flags,
                                                 int enable);

/*
 * Get CPRI port TX interrupts enable status.
 */
extern
int cprif_drv_cpri_port_tx_interrupt_enable_get(int unit,
                                                int port,
                                                int tx_intr_flags,
                                                int *status);

/*
 * Enable CPRI port TX Functional interrupts.
 * TX Functional interrupts can be of 3 types
 * 1. TX
 * 2. TX functional error type interrupts.
 * 3. TX memory ECC error type interrupts.
 */
extern
int cprif_drv_cpri_port_tx_op_err_interrupts_enable_set(int unit,
                                                      int port,
                                                      int tx_op_err_intr_flags,
                                                      int enable);

/*
 * Get CPRI port TX functional interrupts enable status.
 */
extern
int cprif_drv_cpri_port_tx_op_err_interrupts_enable_get(int unit,
                                                        int port,
                                                        int tx_op_err_intr_flags,
                                                        int *status);

/*
 * Enable CPRI port TX Functional interrupts.
 * TX Functional interrupts can be of 3 types
 * 1. TX
 * 2. TX functional error type interrupts.
 * 3. TX memory ECC error type interrupts.
 */
extern
int cprif_drv_cpri_port_tx_func_interrupts_enable_set(int unit,
                                                      int port,
                                                      int tx_func_intr_flags,
                                                      int enable);

/*
 * Get CPRI port TX functional interrupts enable status.
 */
extern
int cprif_drv_cpri_port_tx_func_interrupts_enable_get(int unit,
                                                      int port,
                                                      int tx_func_intr_flags,
                                                      int *status);

int cprif_drv_compression_entry_set (int unit, int port, int index, 
                                     uint32 comp_val_0, uint32 comp_val_1);
int cprif_drv_compression_entry_get (int unit, int port, int index, 
                                     uint32* comp_val_0, uint32* comp_val_1);
int cprif_drv_decompression_entry_set (int unit, int port, int index, 
                                       uint32 decomp_val);
int cprif_drv_decompression_entry_get (int unit, int port, int index, 
                                       uint32* decomp_val);
int cprif_drv_decap_queue_flow_control_entry_set(int unit, int port,
                                                 int index,
                                                 int enable,
                                                 uint32 queue_num,
                                                 uint32 xon_buff_count,
                                                 uint32 xoff_buff_count);
int cprif_drv_decap_queue_flow_control_entry_get(int unit, int port,
                                                 int index,
                                                 int* enable,
                                                 uint32* queue_num,
                                                 uint32* xon_buff_count,
                                                 uint32* xoff_buff_count);
int cprif_drv_cpri_port_fec_enable_set(int unit, int port, cprimod_direction_t dir, int enable);
int cprif_drv_cpri_port_fec_enable_get(int unit, int port, cprimod_direction_t dir, int* enable);
int cprif_drv_cpri_port_fec_timestamp_config_set(int unit, int port, cprimod_direction_t dir,
                                                 const cprimod_fec_timestamp_config_t* config);
int cprif_drv_cpri_port_fec_timestamp_config_get(int unit, int port, cprimod_direction_t dir,
                                                 cprimod_fec_timestamp_config_t* config);
int cprif_drv_cpri_port_fec_aux_config_set(int unit, int port,
                                           cprimod_fec_aux_config_para_t parameter_id,
                                           uint32 value);
int cprif_drv_cpri_port_fec_aux_config_get(int unit, int port,
                                           cprimod_fec_aux_config_para_t parameter_id,
                                           uint32* value);
int cprif_drv_port_fec_stat_get (int unit, int port, cprimod_cpri_fec_stat_type_t type, uint32* value);

int cprif_drv_init(int unit, int port);
int cprif_drv_port_rx_datapath_reset(int unit, int port, int val);
int cprif_drv_port_tx_datapath_reset(int unit, int port, int val);
int cprif_drv_port_rx_pmd_datapath_reset(int unit, int port, int val);

int cprif_drv_rx_gcw_word_get(int unit, int port, int group_index, uint16* word);
int cprif_drv_tx_gcw_word_set(int unit, int port, int group_index, uint16 word);

int cprif_drv_encap_header_field_mapping_set(int unit, int port, cprif_drv_encap_hdr_field_mapping_t *mapping);
int cprif_drv_encap_header_field_mapping_get(int unit, int port, cprif_drv_encap_hdr_field_mapping_t *mapping);
int cprif_drv_decap_header_field_mapping_set(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mapping);
int cprif_drv_decap_header_field_mapping_get(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mapping);
int cprif_drv_decap_header_field_mask_set(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mask);
int cprif_drv_decap_header_field_mask_get(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mask);
int cprif_drv_length_field_adjustment_set (int unit, int port, cprimod_direction_t dir, int adjust_value);
int cprif_drv_length_field_adjustment_get (int unit, int port, cprimod_direction_t dir, int* adjust_value);
int cprif_drv_extended_header_enable_set (int unit, int port, cprimod_direction_t dir, int enable);
int cprif_drv_extended_header_enable_get (int unit, int port, cprimod_direction_t dir, int* enable);

int cprif_drv_compression_negative_saturation_threshold_set( int unit, int port, uint32 threshold_value);
int cprif_drv_compression_negative_saturation_threshold_get( int unit, int port, uint32* threshold_value);
int cprif_drv_decompression_negative_saturation_value_set( int unit, int port, uint32 sat_value);
int cprif_drv_decompression_negative_saturation_value_get( int unit, int port, uint32* sat_value);
int cprif_drv_ingress_traffic_rate_set(int unit, int port, uint32 rate);
int cprif_drv_ingress_traffic_rate_get(int unit, int port, uint32* rate);
int cprif_drv_decompression_traffic_rate_increase_set(int unit, int port, int enable);
int cprif_drv_decompression_traffic_rate_increase_get(int unit, int port, int* enable);
int cprif_drv_rsvd4_tx_padding_size_set(int unit, int port, uint32 padding_size);
int cprif_drv_rsvd4_tx_padding_size_get(int unit, int port, uint32* padding_size);
int cprif_drv_cpri_work_queue_interrupt_status_clear(int unit, int port);
int cprif_drv_cpri_byte_error_count_clear(int unit, int port);

int cprif_drv_cpri_port_ecc_interrupt_enable_set(int unit, int port,
                                                            int enable);
int cprif_drv_tsc_clk_ext_select(int unit, int port, int ext_clk );
int cprif_drv_cpri_port_ecc_interrupt_status_get(int unit, int port,
                                    cprimod_cpri_ecc_intr_info_t *ecc_err_info);

int cprif_drv_cpri_port_ecc_interrupt_test(int unit, int port);
#endif /*_CPRIF_DRV_H_*/

