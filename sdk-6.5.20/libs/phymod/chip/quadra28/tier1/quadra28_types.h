/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef __QUADRA28_TYPES_H__
#define __QUADRA28_TYPES_H__

/*
 * Includes
 */
#include <phymod/phymod_types.h>
/*
 *  Defines
 */
#define QUADRA28_MAX_LANE   4 

#define Q28_SPEED_1G                   1000
#define Q28_SPEED_10G                  10000
#define Q28_SPEED_HG11                 11000
#define Q28_SPEED_11P5G                11500
#define Q28_SPEED_12P5G                12500
#define Q28_SPEED_40G                  40000
#define SPEED_1G_OS8                   1000
#define SPEED_1G_OS8P25                0
#define Q28_SPEED_20G                  20000
#define Q28_SPEED_42G                  42000

typedef enum {
    Q28_SINGLE_PMD = 1,
    Q28_MULTI_PMD
} Q28_PMD_MODE;

/*Input Command Messages.*/
#define Q28_WR_CPU_CTRL_REGS    0x11    /* Write CPU/SPI Control Regs, followed by Count And CPU/SPI Controller Reg add/data pairs.*/
#define Q28_RD_CPU_CTRL_REGS    0xEE    /* Read CPU/SPI Control Regs, followed by Count and CPU/SPI Controller Register Add.*/
#define Q28_WR_CPU_CTRL_FIFO    0x66    /* Write CPU/SPI Control Regs Continously, followed by Count and CPU/SPI Controller Reg addr and data's.*/
#define Q28_SPI_CTRL_1_L        0xC000
#define Q28_SPI_CTRL_1_H        0xC002
#define Q28_SPI_CTRL_2_L        0xC400
#define Q28_SPI_CTRL_2_H        0xC402
#define Q28_SPI_TXFIFO          0xD000
#define Q28_SPI_RXFIFO          0xD400
/* SPI Controller Commands (known As messages).*/
#define Q28_MSGTYPE_HWR         0x40
#define Q28_MSGTYPE_HRD         0x80
#define Q28_WRSR_OPCODE         0x01
#define Q28_WR_OPCODE           0x02
#define Q28_RD_OPCODE           0x03
#define Q28_WRDI_OPCODE         0x04
#define Q28_RDSR_OPCODE         0x05
#define Q28_WREN_OPCODE         0x06
#define Q28_WR_BLOCK_SIZE       0x40    /* Maximum 64 Bytes Writes.*/
#define Q28_TOTAL_WR_BYTE       0xF000  /* Code Size is 60k bytes.*/
#define Q28_RETRY_CNT           5

#endif
