/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __XPT_OP_H
#define __XPT_OP_H

#include <stdint.h>
#include <stdio.h>

#include "errors.h"
#include "allocator.h"


#ifdef __cplusplus
extern "C" {
#endif


#define KBP_OPCODE_WRITE_LOW         (0x1)
#define KBP_OPCODE_READX_LOW         (0x2)
#define KBP_OPCODE_READY_LOW         (0x3)
#define KBP_OPCODE_FREADX_LOW        (0x4)

#define KBP_OPCODE_WRITE_READ_X_Y_HIGH       (0x0)


#define KBP_OPCODE_CMP1_HIGH        (0x1)
#define KBP_OPCODE_CMP2_HIGH        (0x2)
#define KBP_OPCODE_STAT_COUNT       (0x3)
#define KBP_OPCODE_CBW_HIGH         (0x4)
#define KBP_OPCODE_CBWLPM_HIGH      (0x5)
#define KBP_OPCODE_CBNWCMP_HIGH     (0x6)
#define KBP_OPCODE_NCBCMP_HIGH      (0x7)


#define KBP_OPCODE_BULK_READ        (0x6)
#define KBP_OPCODE_BULK_WRITE       (0x7)

#define KBP_OPCODE_BLKCP            (0x8)
#define KBP_OPCODE_BLKMV            (0x9)
#define KBP_OPCODE_BLKCLR           (0xa)
#define KBP_OPCODE_BLKINV           (0xb)

#define KBP_COMMAND_OPCODE          (0x20)



struct kbp_search_result;



struct op_xpt {
    uint32_t device_type;              
    void *handle;                      
    void *search_handle;               
    struct kbp_allocator *alloc;       


    

    kbp_status (*op_write_reg)(void *handle, uint32_t address, const uint8_t *data, uint32_t core_bitmap);

    

    kbp_status (*op_read_reg)(void *handle, uint32_t address, uint8_t *data, uint32_t core_bitmap);

    

    kbp_status (*op_write_dba_entry)(void *handle, uint32_t address, const uint8_t *data,
                                     const uint8_t *mask, uint32_t is_xy, uint32_t valid_bit, uint32_t core_bitmap);
    

    kbp_status (*op_read_dba_entry)(void *handle, uint32_t address, uint32_t read_x_or_y,
                                    uint8_t *entry_x_or_y, uint32_t *valid_bit, uint32_t *parity, uint32_t core_bitmap);

    

    kbp_status (*op_write_uda)(void *handle, uint32_t address_32, uint8_t is_uda_64b, uint64_t value, uint32_t core_bitmap);

    

    kbp_status (*op_read_uda)(void *handle, uint32_t address_32, uint8_t is_uda_64b, uint64_t *value, uint32_t core_bitmap);

    

    kbp_status (*op_kbp_command)(void *handle, uint32_t opcode, uint32_t nbytes, uint8_t *bytes, uint32_t core_bitmap);

    

    int32_t (*mdio_read)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value);

    

    int32_t (*mdio_write)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value);

    

    kbp_status (*op_search)(void *handle, uint32_t ltr, uint32_t ctx,
                            const uint8_t *key, uint32_t key_len,
                            struct kbp_search_result *result);

    

    kbp_status (*sw_reset)(uint32_t device_type, void *handle, uint32_t reset_type);

    

    int32_t (*ext_mdio_read)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value);

    

    int32_t (*ext_mdio_write)(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value);

    FILE *debug_fp;                         
};

#ifdef __cplusplus
}
#endif
#endif 
