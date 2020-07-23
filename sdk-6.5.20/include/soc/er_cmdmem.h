/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        er_cmdmem.h
 */

#ifndef _SOC_ER_CMDMEM_H_
#define _SOC_ER_CMDMEM_H_

#define SOC_MEM_CMD_BASE_TO_OFFSET      24
#define SOC_MEM_CMD_TARGET_MASK         0xff
#define SOC_MEM_CMD_DATA_WORDS          17
#define SOC_MEM_CMD_NEWCMD              0x80000000
#define SOC_MEM_CMD_TAB_FULL            0x40000000
#define SOC_MEM_CMD_NOT_FOUND           0x20000000
#define SOC_MEM_CMD_PARITY_ERR          0x10000000
#define SOC_MEM_CMD_INVALID_ADDR        0x08000000
#define SOC_MEM_CMD_L2_MOD_FIFO_FULL    0x04000000
#define SOC_MEM_CMD_INVALID_CMD         0x02000000


typedef struct soc_mem_cmd_s {
    uint32      command;         /* Operation */
    uint32      sub_command;     /* Additional operation info */
    uint32      addr0;           /* Entry number or lower bound */
    uint32      addr1;
    void        *entry_data;
    uint32      output_data[SOC_MEM_CMD_DATA_WORDS];
} soc_mem_cmd_t;

#define SOC_MEM_CMD_READ                 0
#define SOC_MEM_CMD_WRITE                1
#define SOC_MEM_CMD_SEARCH               2
#define SOC_MEM_CMD_LEARN                3
#define SOC_MEM_CMD_DELETE               4
#define SOC_MEM_CMD_PASS_WRITE           5
#define SOC_MEM_CMD_PASS_READ            6
#define SOC_MEM_CMD_SHIFT_DOWN           7
#define SOC_MEM_CMD_SHIFT_UP             8
#define SOC_MEM_CMD_CLEAR_L3_HIT         9
#define SOC_MEM_CMD_SET_L3_HIT          10
#define SOC_MEM_CMD_SWAP_L3_HIT         11
#define SOC_MEM_CMD_READ_ONE            12
#define SOC_MEM_CMD_SEARCH_72           13
#define SOC_MEM_CMD_INSERT_LPM          14
#define SOC_MEM_CMD_REMOVE_LPM          15
#define SOC_MEM_CMD_PER_PORT_AGING      16

/* DEFIP table entry type markers */
#define SOC_ER_DEFIP_NOT_V6_FF          0xff
#define SOC_ER_DEFIP_KEY_TYPE_IPV4      4
#define SOC_ER_DEFIP_KEY_TYPE_MPLS_1L   6
#define SOC_ER_DEFIP_KEY_TYPE_MPLS_2L   7

#define SOC_MEM_CMD_SUB_V4               0
#define SOC_MEM_CMD_SUB_V6               1
#define SOC_MEM_CMD_SUB_MPLS1            2
#define SOC_MEM_CMD_SUB_MPLS2            3

#define SOC_MEM_CMD_HASH_ADDR_MASK         0xfffff
#define SOC_MEM_CMD_EXT_L2_ADDR_MASK_HI    0xffff8
#define SOC_MEM_CMD_EXT_L2_ADDR_MASK_LO    0x00003
#define SOC_MEM_CMD_EXT_L2_ADDR_SHIFT_HI   1
#define SOC_MEM_CMD_HASH_TABLE_SHIFT       20
#define SOC_MEM_CMD_HASH_TABLE_MASK        0x3

#define SOC_MEM_CMD_HASH_TABLE_L2EXT       0x0
#define SOC_MEM_CMD_HASH_TABLE_DFLT        0x1
#define SOC_MEM_CMD_HASH_TABLE_L2OVR       0x2

extern int soc_mem_cmd_op(int unit, soc_mem_t mem,
                          soc_mem_cmd_t *cmd_info, int intr);

#define SOC_MEM_CMD_TCAM_ADDR_MAX          6

extern int soc_mem_tcam_op(int unit, uint32 *addr, uint32 *data,
                           int num_addr, int write);


#endif	/* !_SOC_ER_CMDMEM_H_ */
