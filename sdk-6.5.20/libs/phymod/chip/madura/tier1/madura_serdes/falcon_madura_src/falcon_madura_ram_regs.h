/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 */

/* 
 */

/** @file furia_ram_regs.h
 * Address of registers used to access the RAM of on-chip processors.
 */

#ifndef FURIA_RAM_REGS_H_
#define FURIA_RAM_REGS_H_

#define FUR_M0ACCESS_ADDR_master_cram_mem_add_Adr     (0x8400)
#define FUR_M0ACCESS_ADDR_master_dram_mem_add_Adr     (0x8401)
#define FUR_M0ACCESS_ADDR_slave_cram_mem_add_Adr      (0x8402)
#define FUR_M0ACCESS_ADDR_slave_dram_mem_add_Adr      (0x8403)

#define FUR_M0ACCESS_DATA_master_cram_lsb_wdata_Adr   (0x8410)
#define FUR_M0ACCESS_DATA_master_cram_lsb_rdata_Adr   (0x8411)
#define FUR_M0ACCESS_DATA_master_cram_msb_wdata_Adr   (0x8412)
#define FUR_M0ACCESS_DATA_master_cram_msb_rdata_Adr   (0x8413)
#define FUR_M0ACCESS_DATA_master_dram_wdata_Adr       (0x8414)
#define FUR_M0ACCESS_DATA_master_dram_rdata_Adr       (0x8415)
#define FUR_M0ACCESS_DATA_slave_cram_lsb_wdata_Adr    (0x8416)
#define FUR_M0ACCESS_DATA_slave_cram_lsb_rdata_Adr    (0x8417)
#define FUR_M0ACCESS_DATA_slave_cram_msb_wdata_Adr    (0x8418)
#define FUR_M0ACCESS_DATA_slave_cram_msb_rdata_Adr    (0x8419)
#define FUR_M0ACCESS_DATA_slave_dram_wdata_Adr        (0x841A)
#define FUR_M0ACCESS_DATA_slave_dram_rdata_Adr        (0x841B)

#endif /* FURIA_RAM_REGS_H_ */
