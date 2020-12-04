/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef DUNE_CHIPS_INCLUDED
/* { */
#define DUNE_CHIPS_INCLUDED

#define __ATTRIBUTE_PACKED__  __attribute__ ((packed))

#include "bits_bytes_macros.h"
/*
 * Definitions related to 'version' register.
 * This register is mutual to all Dune's devices.
 */

#define DUNE_VERSION_REGISTERS_ADDRESS 0x0

#define CHIP_TYPE_MS_BIT     23
#define CHIP_TYPE_NUM_BITS   20
#define CHIP_TYPE_LS_BIT     (CHIP_TYPE_MS_BIT + 1 - CHIP_TYPE_NUM_BITS)
#define CHIP_TYPE_MASK   \
    (((unsigned long)SOC_SAND_BIT(CHIP_TYPE_MS_BIT) - SOC_SAND_BIT(CHIP_TYPE_LS_BIT)) + SOC_SAND_BIT(CHIP_TYPE_MS_BIT))
#define CHIP_TYPE_SHIFT      CHIP_TYPE_LS_BIT

#define DBG_VER_MS_BIT       27
#define DBG_VER_NUM_BITS     4
#define DBG_VER_LS_BIT       (DBG_VER_MS_BIT + 1 - DBG_VER_NUM_BITS)
#define DBG_VER_MASK   \
    (((unsigned long)SOC_SAND_BIT(DBG_VER_MS_BIT) - SOC_SAND_BIT(DBG_VER_LS_BIT)) + SOC_SAND_BIT(DBG_VER_MS_BIT))
#define DBG_VER_SHIFT        DBG_VER_LS_BIT

#define CHIP_VER_MS_BIT      31
#define CHIP_VER_NUM_BITS    4
#define CHIP_VER_LS_BIT      (CHIP_VER_MS_BIT + 1 - CHIP_VER_NUM_BITS)
#define CHIP_VER_MASK   \
    (((unsigned long)SOC_SAND_BIT(CHIP_VER_MS_BIT) - SOC_SAND_BIT(CHIP_VER_LS_BIT)) + SOC_SAND_BIT(CHIP_VER_MS_BIT))
#define CHIP_VER_SHIFT       CHIP_VER_LS_BIT


/*
 * Memory structures describing FE registers in a way that
 * enables the cpu to both display and change it.
 * {
 */
/*
 * Structure to use to interpret a value of a field to
 * human readable text.
 */
typedef struct
{
    /*
     * Value of field.
     */
  unsigned long   value ;
    /*
     * Human readable text description of this value for this field.
     */
  const char            *value_text_description ;
} VAL_TO_TEXT ;
/*
 * Maximal number of of 'VAL_TO_TEXT' items per field.
 */
/*
 * Structure to use to describe the structure of a group:
 * A group is assumed to be represented by a set of 32-bits
 * registers. Each register is located at a specified offset
 * from the beginning of the group (at long word intervals)
 * and may be repeated a specific number of times.
 */
typedef struct
{
    /*
     * Number of repetitions of 32-bits register.
     * Must be '1' or more.
     */
  unsigned short   num_repetitions ;
    /*
     * Identifier of representative 32-bits register.
     * This ID must be one of those specified
     * in 'Register identifiers' below.
     */
  unsigned short   register_id ;
    /*
     * Byte offset, from the beginning of memory block,
     * assigned to FE, of first 32-bits register.
     * The LS two bits are supposed to be '0' and
     * are ignored, anyway.
     */
  unsigned long    byte_offset ;
} ELEMENT_OF_GROUP ;
/*
 * Maximal number of of 'ELEMENT_OF_GROUP' items per group.
 */
#define MAX_ELEMENT_OF_GROUP_ITEMS 10
/*
 * Maximal number of of 32-bits regsiters per group.
 */
#define MAX_REGS_IN_ONE_GROUP      256
/*
 * Field identifiers
 */
#define FE_NO_FIELD                      1
#define FE_LAST_FIELD                    65535
#define FE_CHIP_TYPE_FIELD               2
#define FE_DBG_VER_FIELD                 3
#define FE_CHIP_VER_FIELD                4
#define FE_CHIP_ID_FIELD                 5
#define FE_MODE_FE2_FIELD                6
#define FE_REPEATER_FIELD                7
#define FE_PLANES_FIELD                  8
#define FE_MULTISTAGE_FIELD              9
#define FE_TX_DATA_CELL_TRIGGER_FIELD    10
#define FE_CSRFO_1_FIELD                 11
#define FE_SRFNE_1_FIELD                 12
#define FE_CSRFO_2_FIELD                 13
#define FE_SRFNE_2_FIELD                 14
#define FE_PEC_1_FIELD                   15
#define FE_PECO_1_FIELD                  16
#define FE_PEC_2_FIELD                   17
#define FE_PECO_2_FIELD                  18
#define FE_RCI_P_TRH_FIELD               19
#define FE_GCI_P_TRH_1_FIELD             20
#define FE_GCI_P_TRH_2_FIELD             21
#define FE_GCI_P_TRH_3_FIELD             22
#define FE_CTR_CELL_CNT_1_FIELD          23
#define FE_CTR_CELL_CNT_O_1_FIELD        24
#define FE_CTR_CELL_CNT_2_FIELD          25
#define FE_CTR_CELL_CNT_O_2_FIELD        26
#define FE_SRC_D_ID_N_1_FIELD            27
#define FE_SRC_D_LVL_N_1_FIELD           28
#define FE_SRC_D_LN_N_N_1_FIELD          29
#define FE_SRC_D_ID_N_2_FIELD            30
#define FE_SRC_D_LVL_N_2_FIELD           31
#define FE_SRC_D_LN_N_N_2_FIELD          32
#define FE_FEL_N_00_CHANGED_FIELD        33
#define FE_FEL_N_32_CHANGED_FIELD        34
#define FE_SRC_MUL_FIELD                 35
#define FE_RM_DID_OR_FIELD               36
#define FE_FE_23_DUP_ID_FIELD            37
#define FE_FE_23_DDEST1_FIELD            38
#define FE_FE_23_DDEST2_FIELD            39
#define FE_FE_23_DEST_ID_FIELD           40
#define FE_OR_DEST_ID_FIELD              41
#define FE_OR_INPUT_LN_FIELD             42
#define FE_RMAC_CRC_D_FIELD              43
#define FE_TX_CDC_FIELD                  44
#define FE_RX_CDC_FIELD                  45
#define FE_RX_GC_CNT_N_1_FIELD           46
#define FE_RX_GC_CNT_N_2_FIELD           47
#define FE_RX_GC_CNT_N_3_FIELD           48
#define FE_RX_GC_CNT_N_4_FIELD           49
#define FE_RX_CRC_ERR_N_1_FIELD          50
#define FE_RX_CRC_ERR_N_2_FIELD          51
#define FE_RX_CRC_ERR_N_3_FIELD          52
#define FE_RX_CRC_ERR_N_4_FIELD          53
#define FE_RX_MA_ERR_N_1_FIELD           54
#define FE_RX_MA_ERR_N_2_FIELD           55
#define FE_RX_MA_ERR_N_3_FIELD           56
#define FE_RX_MA_ERR_N_4_FIELD           57
#define FE_RX_CODEG_ERR_N_1_FIELD        58
#define FE_RX_CODEG_ERR_N_2_FIELD        59
#define FE_RX_CODEG_ERR_N_3_FIELD        60
#define FE_RX_CODEG_ERR_N_4_FIELD        61
#define FE_TX_CNT_N_1_FIELD              62
#define FE_TX_CNT_N_2_FIELD              63
#define FE_TX_CNT_N_3_FIELD              64
#define FE_TX_CNT_N_4_FIELD              65
#define FE_PRBS_1_GENO_FIELD             66
#define FE_PRBS_1_CKO_FIELD              67
#define FE_PRBS_2_GENO_FIELD             68
#define FE_PRBS_2_CKO_FIELD              69
#define FE_PRBS_3_GENO_FIELD             70
#define FE_PRBS_3_CKO_FIELD              71
#define FE_PRBS_4_GENO_FIELD             72
#define FE_PRBS_4_CKO_FIELD              73
#define FE_PRBS_1_GENL_FIELD             74
#define FE_PRBS_1_CKL_FIELD              75
#define FE_PRBS_2_GENL_FIELD             76
#define FE_PRBS_2_CKL_FIELD              77
#define FE_PRBS_3_GENL_FIELD             78
#define FE_PRBS_3_CKL_FIELD              79
#define FE_PRBS_4_GENL_FIELD             80
#define FE_PRBS_4_CKL_FIELD              81
#define FE_BER_CNT_1_FIELD               82
#define FE_BER_VALID_1_FIELD             83
#define FE_BER_CNT_2_FIELD               84
#define FE_BER_VALID_2_FIELD             85
#define FE_BER_CNT_3_FIELD               86
#define FE_BER_VALID_3_FIELD             87
#define FE_BER_CNT_4_FIELD               88
#define FE_BER_VALID_4_FIELD             89
#define FE_PD_TBG_FIELD                  90
#define FE_PD_FCT_FIELD                  91
#define FE_AMP_FIELD                     92
#define FE_PEN_FIELD                     93
#define FE_MEN_FIELD                     94
#define FE_VCMS_FIELD                    95
#define FE_LAST_TX_FIELD                 96
#define FE_PAYLOAD_TX_FIELD              97
#define FE_FIRST_BYTE_TX_FIELD           98
#define FE_FE3_SW_FIELD                  99
#define FE_FE2_SW_FIELD                  100
#define FE_DEST_LVL_TX_FIELD             101
#define FE_SRC_LVL_TX_FIELD              102
#define FE_SRC_ID_TX_FIELD               103
#define FE_COLN_FIELD                    104
#define FE_LAST_RX_FIELD                 105
#define FE_PAYLOAD_RX_FIELD              106
#define FE_FIRST_BYTE_RX_FIELD           107
#define FE_SRC_LVL_RX_FIELD              108
#define FE_SRC_ID_RX_FIELD               109
#define FE_PURE_IND_ADDR_FIELD           110
#define FE_MODULE_FIELD                  111
#define FE_RD_NOT_WR_FIELD               112
#define FE_IND_TRIGGER_FIELD             113
#define FE_WRITE_BUFF_ANY_FIELD          114
#define FE_WRITE_BUFF_LAST_FIELD         115
#define FE_RTP_RR_FIELD                  116
#define FE_DCHA_RR_ANY_FIELD             117
#define FE_DCHA_RR_LAST_FIELD            118
#define FE_DCHB_RR_ANY_FIELD             119
#define FE_DCHB_RR_LAST_FIELD            120
#define FE_DCLA_RR_ANY_FIELD             121
#define FE_DCLA_RR_LAST_FIELD            122
#define FE_DCLB_RR_ANY_FIELD             123
#define FE_DCLB_RR_LAST_FIELD            124
#define FE_CCSA_RR_ANY_FIELD             125
#define FE_CCSA_RR_LAST_FIELD            126
#define FE_CCSB_RR_ANY_FIELD             127
#define FE_CCSB_RR_LAST_FIELD            128
#define FE1_MULTICAST_DISTRIBUTION_FIELD 129
#define FE2_MULTICAST_REPLICATION_L_FIELD  130
#define FE2_MULTICAST_REPLICATION_H_FIELD  131
#define FE3_MULTICAST_REPLICATION_FIELD  132
#define FE1_UNICAST_DISTRIBUTION_FIELD   133
#define FE2_UNICAST_ROUTING_FIELD        134
#define FE3_UNICAST_ROUTING_FIELD        135
/*
 * Interrupt Fields
 */
#define FE_FECINT_FIELD                  500
#define FE_FECINT_MASK_FIELD             600
/*
 * Register identifiers
 */
#define FE_VERSION_REG                   2
#define FE_CONFIGURATION_REG             3
#define FE_TX_DATA_CELL_TRIGGER_REG      4
#define FE_SR_DATA_CELL_FIFO_IND_1_REG   5
#define FE_SR_DATA_CELL_FIFO_IND_2_REG   6
#define FE_PKT_DATA_CELL_COUNTER_1_REG   7
#define FE_PKT_DATA_CELL_COUNTER_2_REG   8
#define FE_RCI_N_CGI_THRESHOLDS_REG      9
#define FE_CONTROL_CELLS_CNTR_1_REG      10
#define FE_CONTROL_CELLS_CNTR_2_REG      11
#define FE_CONNECTIVITY_1_REG            12
#define FE_CONNECTIVITY_2_REG            13
#define FE_CONNECTIVITY_MAP_CHANGE_1_REG 14
#define FE_CONNECTIVITY_MAP_CHANGE_2_REG 15
#define FE_ROUTING_PROC_ENABLES_REG      16
#define FE_REACH_FAULT_IND_REG           17
#define FE_REACH_DUP_DEST_REG            18
#define FE_REACH_DEST_OR_REG             19
#define FE_MAC_ENABLERS_REG              20
#define FE_MAC_RX_CNTRS_1_REG            21
#define FE_MAC_RX_CNTRS_2_REG            22
#define FE_MAC_RX_CNTRS_3_REG            23
#define FE_MAC_RX_CNTRS_4_REG            24
#define FE_MAC_TX_CNTRS_1_REG            25
#define FE_MAC_TX_CNTRS_2_REG            26
#define FE_MAC_TX_CNTRS_3_REG            27
#define FE_MAC_TX_CNTRS_4_REG            28
#define FE_PRBS_ENABLERS_REG             29
#define FE_PRBS_LINK_NUMS_REG            30
#define FE_MAC_BER_CNTRS_1_REG           31
#define FE_MAC_BER_CNTRS_2_REG           32
#define FE_MAC_BER_CNTRS_3_REG           33
#define FE_MAC_BER_CNTRS_4_REG           34
#define FE_SERDES_FRST_REG               35
#define FE_SERDES_REST_REG               36
#define FE_LAST_TX_REG                   37
#define FE_BODY_TX_REG                   38
#define FE_SWITCHES_TX_REG               39
#define FE_PATHS_TX_REG                  40
#define FE_LAST_RX_REG                   41
#define FE_BODY_RX_REG                   42
#define FE_SWITCHES_RX_REG               43
#define FE_PATHS_RX_REG                  44
#define FE_ACCESS_ADDR_REG               45
#define FE_IND_TRIGGER_REG               46
#define FE_WRITE_BUFF_ANY_REG            47
#define FE_WRITE_BUFF_LAST_REG           48
#define FE_RTP_RR_REG                    49
#define FE_DCHA_RR_ANY_REG               50
#define FE_DCHA_RR_LAST_REG              51
#define FE_DCHB_RR_ANY_REG               52
#define FE_DCHB_RR_LAST_REG              53
#define FE_DCLA_RR_ANY_REG               54
#define FE_DCLA_RR_LAST_REG              55
#define FE_DCLB_RR_ANY_REG               56
#define FE_DCLB_RR_LAST_REG              57
#define FE_CCSA_RR_ANY_REG               58
#define FE_CCSA_RR_LAST_REG              59
#define FE_CCSB_RR_ANY_REG               60
#define FE_CCSB_RR_LAST_REG              61
#define FE1_MULTICAST_DISTRIBUTION_REG   62
#define FE2_MULTICAST_REPLICATION_L_REG  63
#define FE2_MULTICAST_REPLICATION_H_REG  64
#define FE3_MULTICAST_REPLICATION_REG    65
#define FE1_UNICAST_DISTRIBUTION_REG     66
#define FE2_UNICAST_ROUTING_REG          67
#define FE3_UNICAST_ROUTING_REG          68
/*
 * Main interrupt register
 */
#define FE_INT_MODULE_SRC_REG            100
#define FE_INT_MODULE_SRC_MASK_REG       101
#define FE_CCS_OVERFLOW_REG              102
/*
 * Group identifiers
 */
#define FE_NO_GROUP                1
#define FE_TX_SR_DATA_CELL_GROUP   2
#define FE_CONNECT_MAP_GROUP       3
#define FE_MAC_RX_CNTRS_GROUP      4
#define FE_MAC_TX_CNTRS_GROUP      5
#define FE_MAC_BER_CNTRS_GROUP     6
#define FE_SERDES_ENABLERS_GROUP   7
#define FE_RX_SR_DATA_CELL_GROUP   8
#define FE_IND_WRITE_BUFF_GROUP    9
#define FE_DCHA_RR_GROUP           10
#define FE_DCHB_RR_GROUP           11
#define FE_DCLA_RR_GROUP           12
#define FE_DCLB_RR_GROUP           13
#define FE_CCSA_RR_GROUP           14
#define FE_CCSB_RR_GROUP           15
#define FE1_MULTICAST_DISTRIBUTION_GROUP   16
#define FE2_MULTICAST_REPLICATION_L_GROUP  17
#define FE2_MULTICAST_REPLICATION_H_GROUP  18
#define FE3_MULTICAST_REPLICATION_GROUP    19
#define FE1_UNICAST_DISTRIBUTION_GROUP     20
#define FE2_UNICAST_ROUTING_GROUP          21
#define FE3_UNICAST_ROUTING_GROUP          22

/*
 * Block identifiers
 */
/*
 * 0X0000 - 0x00FF
 */
#define FE_GEN_REGS_BLOCK          2
/*
 * 0x0100 - 0x01FF
 */
#define FE_DCS_DCHA_BLOCK          3
/*
 * 0x0200 - 0x02FF
 */
#define FE_DCS_DCLA_BLOCK          4
/*
 * 0x0300 - 0x03FF
 */
#define FE_DCS_DCHB_BLOCK          5
/*
 * 0x0400 - 0x04FF
 */
#define FE_DCS_DCLB_BLOCK          6
/*
 * 0x0500 - 0x054F
 */
#define  FE_DCS_ECI_BLOCK          7
/*
 * 0x0550 - 0x05FF
 */
/*
 * 0x0600 - 0x06FF
 */
#define  FE_CCSA_CCS_BLOCK         9
/*
 * 0x0700 - 0x07FF
 */
#define  FE_CCSB_CCS_BLOCK         10
/*
 * 0x0800 - 0x08FF
 */
#define  FE_RTP_ECI_BLOCK          11
/*
 * 0x0900 - 0x09FF
 */
#define  FE_RTP_RTP_BLOCK          12
/*
 * 0x0A00 - 0x0AF0
 */
#define  FE_MAC_ECI_BLOCK          13
/*
 * 0x0AF0 - 0x0AFF
 */
/*
 * 0x0B00 - 0x0DFF
 */
#define  FE_MAC_MAC_BLOCK          15
/*
 * 0x0E00 - 0x0EFF
 */
/*
 * 0x0F00 - 0x0FFF
 */
#define  FE_SERDES_ECI_1_BLOCK     17
/*
 * 0x30002000 - 0x3000381F
 */
#define FE_INDIRECT_BLOCK          18

/*
 * Values for 'display_mode' element
 */
#define DISPLAY_HEX                SOC_SAND_BIT(0)
#define DISPLAY_UNSIGNED_INT       SOC_SAND_BIT(1)
#define DISPLAY_BINARY             SOC_SAND_BIT(2)
/*
 * Values for 'access_type' field
 */
#define ACCESS_READ                SOC_SAND_BIT(0)
#define ACCESS_WRITE               SOC_SAND_BIT(1)
#define ACCESS_CLEAR               SOC_SAND_BIT(2)
#define ACCESS_INDIRECT            SOC_SAND_BIT(3)
typedef struct
{
    /*
     * Identifier of field. For example 'FE_CHIP_TYPE_FIELD'.
     */
  unsigned short   field_id ;
    /*
     * Identifier of register. For example 'FE_CONGIGURATION_REG'.
     */
  unsigned short   reg_id ;
    /*
     * Identifier of block to which this register belongs.
     * For example 'FE_DCHA_DCS_BLOCK'.
     */
  unsigned short  reg_block_id ;
    /*
     * Identifier of group of registers (long variables)
     * to which this register belongs.
     * Usually, registers do not belong to any group.
     * For example 'FE_TX_SR_DATA_CELL_GROUP'.
     */
  unsigned short  reg_group_id ;
    /*
     * Byte offset (from the beginning of memory area
     * assigned to FE) of long variable to which this
     * register belongs.
     * This offset must be long aligned (must have its
     * two LS bits set to zero) and, in any case, those
     * bits are ignored.
     * Memory area assigned to FE is assumed to be long-aligned.
     */
  unsigned long   reg_base_offset ;
    /*
     * Mask of the bits which make this field within the
     * long variable register to which this field belongs.
     */
  unsigned long   field_mask ;
    /*
     * Shift that should be applied to field in order
     * to set it so that its LS bit is also the LS bit
     * of containing long variable register.
     */
  unsigned long   field_shift ;
    /*
     * Human readable identifier of field.
     * For example "FE_CHIP_TYPE_FIELD".
     */
  char            *field_name ;
    /*
     * Human readable text description of field.
     */
  char            *field_text_description ;
    /*
     * Human readable identifier of register.
     * For example "FE_CONGIGURATION_REG".
     */
  char            *reg_name ;
    /*
     * Human readable text description of register.
     */
  char            *reg_text_description ;
    /*
     * Human readable identifier of group of registers
     * (long variables) to which this register belongs.
     * For example "FE_TX_SR_DATA_CELL_GROUP".
     */
  char            *reg_group_name ;
    /*
     * Human readable text description of group of
     * registers (long variables) to which this register
     * belongs.
     */
  char            *reg_group_text_description ;
    /*
     * Human readable identifier of block to which this
     * register belongs.
     * For example "FE_DCHA_DCS_BLOCK".
     */
  char            *reg_block_name ;
    /*
     * Human readable text description of block to
     * which this register belongs.
     */
  char            *reg_block_text_description ;
    /*
     * Required display mode for this field
     * DISPLAY_HEX
     * DISPLAY_UNSIGNED_INT
     * DISPLAY_BINARY
     */
  unsigned char   display_mode ;
    /*
     * Access type allowed for this field
     * ACCESS_READ
     * ACCESS_WRITE
     * ACCESS_CLEAR
     */
  unsigned char   access_type ;
  VAL_TO_TEXT     *val_to_text ;
  /*
   * This element is only relevant for entries which
   * are, essentially, descriptors of groups.
   */
  ELEMENT_OF_GROUP *element_of_group ;
    /*
     * Attribute bits and fields. Same as in VAL_NUM_DESCRIPTOR.
     * an be :
     *   HAS_NO_MIN_VALUE, HAS_MIN_VALUE,
     *   HAS_NO_MAX_VALUE, HAS_MAX_VALUE
     * This parameter is only relevant for registers which
     * are writable.
     */
  unsigned long val_numeric_attributes ;
    /*
     * Maximal allowed value. Same as in VAL_NUM_DESCRIPTOR.
     * This parameter is only relevant for registers which
     * are writable.
     */
  long val_max ;
    /*
     * Minimal allowed value. Same as in VAL_NUM_DESCRIPTOR.
     * This parameter is only relevant for registers which
     * are writable.
     */
  long val_min ;
} DESC_FE_REG ;
/*
 * }
 */
/*
 * EPLD registers
 * {
 */
typedef struct
{
  unsigned char version ;              /* offset: 0x00  */
  unsigned char test_register ;        /* offset: 0x01  */
  unsigned char wait_state_3_0 ;       /* offset: 0x02  */
  unsigned char wait_state_7_4 ;       /* offset: 0x03  */
  unsigned char wait_state_11_8 ;      /* offset: 0x04  */
  unsigned char wait_state_15_12 ;     /* offset: 0x05  */
  unsigned char leds_control ;         /* offset: 0x06  */
  unsigned char slot_id ;              /* offset: 0x07  */
  unsigned char reset_control ;        /* offset: 0x08  */
  unsigned char watchdog ;             /* offset: 0x09  */
  unsigned char gp_bus_data ;          /* offset: 0x0A  */
  unsigned char gp_io_conf ;           /* offset: 0x0B  */
  unsigned char nvram_erase_control ;  /* offset: 0x0C  */
  unsigned char interrupt_cause ;      /* offset: 0x0D  */
  unsigned char interrupt_mask ;       /* offset: 0x0E  */
} __ATTRIBUTE_PACKED__ EPLD_REGS  ;
/*
 * Mask of meaningful bits on slot id register.
 */
/*
 * Maximal slot_id for line card.
 * Line cards have slot_id from 1 to MAX_SLOT_ID_LINE_CARD
 */
#define MAX_SLOT_ID_LINE_CARD   16
/*
 * Maximal slot_id for fabric card.
 * Fabric cards have slot_id from (MAX_SLOT_ID_LINE_CARD+1)
 * to MAX_SLOT_ID_LINE_CARD
 */
#define MAX_SLOT_ID_FABRIC_CARD 24
/*
 * Mask of crate ID bits on internal register C of MPC860.
 */
/*
 * Shift of crate ID bits (from bit 0) on internal register C of MPC860.
 */
/*
 * Mask of crate TYPE bits on internal register C of MPC860.
 */
/*
 * Shift of crate TYPE bits (from bit 0) on internal register C of MPC860.
 */
/*
 * The various types of crates:
 */
#define CRATE_TEST_EVAL              (0)  /*Negev-Rev-A*/
#define CRATE_32_PORT_40G_STANDALONE (1)  /*Gobi: SOC_SAND_FAP20V*/
#define CRATE_32_PORT_40G_IO         (2)
#define CRATE_FABRIC_BOX             (3)
#define CRATE_64_PORT_10G_STANDALONE (4)  /*Gobi: SOC_SAND_FAP10M*/
#define CRATE_32_PORT_10G_IO         (5)
#define CRATE_NEGEV_REV_B            (6)

/*
 * Bit in 'watchdog' which, when zeroed, enables INTERNAL watchdog
 * and, when set, disables it.
 */
/*
 * Bits containing INTERNAL watchdog timer. Every unit is
 * 100 ms.
 */
#define INTERNAL_WATCHDOG_MASK       0x1F
/*
 * Bit in 'watchdog' which, when set, causes only IRQ0 on
 * watchdog expiry but NO reset.
 */
/*
 * Bit in 'watchdog' which, when zeroed, enables external watchdog
 * and, when set, disables it.
 */
/*
 * Units of internal watchdog timer, in ms.
 */
/*
 * Inrerrupt cause register: This register contains bit map
 * of interrupts from underlying board. There are
 * currently five sources defined. Names below relate to
 * DB-MX620-10GS with PCI (but the same entries may get other
 * names for other boards).
 */
/*
 * Inrerrupt mask register: Same bit map as Inrerrupt cause register
 */
/*
 * }
 */

/* } */
#endif
