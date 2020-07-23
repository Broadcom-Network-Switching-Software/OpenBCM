/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BLMI_MMI1_H
#define BLMI_MMI1_H


/* MMI1 specific registers */
#define BLMI_MMI1_REG_CFG                (0x00)
#define BLMI_MMI1_REG_INTR               (0x01)
#define BLMI_MMI1_ID1                    (0x02)
#define BLMI_MMI1_ID2                    (0x03)
#define BLMI_MMI1_REG_PORT_CMD           (0x04) 
    #define BLMI_MMI1_PORT_CMD_DOIT           (0x1)
    #define BLMI_MMI1_PORT_CMD_WRITE_EN       (0x2)
    #define BLMI_MMI1_PORT_CMD_MEM_EN         (0x4)
    #define BLMI_MMI1_PORT_CMD_RESET          (0x8000)

#define BLMI_MMI1_REG_PORT_EXT_CMD       (0x05) 
    #define BLMI_MMI1_PORT_EXT_BURST_EN       (0x01)
    #define BLMI_MMI1_PORT_EXT_BURST_LEN(l)   (((l) & 0x1ff) << 1)
    #define BLMI_MMI1_PORT_EXT_BLK_INIT_EN       (1 << 11)
    #define BLMI_MMI1_PORT_EXT_BLK_COPY_EN       (1 << 12)

#define BLMI_MMI1_REG_PORT_STATUS        (0x06) 
    #define BLMI_MMI1_PORT_STATUS_READY        (0x1)
    #define BLMI_MMI1_PORT_STATUS_ERROR        (0x7f8)

#define BLMI_MMI1_REG_PORT_ADDR_LSB      (0x08) 
#define BLMI_MMI1_REG_PORT_ADDR_MSB      (0x09) 
#define BLMI_MMI1_REG_PORT_DATA_LSB      (0x0A) 
#define BLMI_MMI1_REG_PORT_DATA_MSB      (0x0B)

#define BLMI_MMI1_REG_PORT_SRC_ADDR_LSB      (0x0C) 
#define BLMI_MMI1_REG_PORT_SRC_ADDR_MSB      (0x0D)
#define BLMI_MMI1_REG_PORT_DST_ADDR_LSB      (0x0E) 
#define BLMI_MMI1_REG_PORT_DST_ADDR_MSB      (0x0F)
#define BLMI_MMI1_REG_PORT_FIFO_WR_PTR   (0x12) 
#define BLMI_MMI1_REG_PORT_FIFO_RD_PTR   (0x13) 

#define BLMI_MMI1_REG_QUAD_SEL           (0x1F) 

#endif /* BLMI_MMI1_H */

