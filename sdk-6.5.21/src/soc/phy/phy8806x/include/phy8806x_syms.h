/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       phy8806x_syms.h
 */

#ifndef   _PHY8806X_SYMS_H_
#define   _PHY8806X_SYMS_H_

#include <sal/types.h>


#define MT2_CONFIG_INCLUDE_CHIP_SYMBOLS 1
#define MT2_CONFIG_INCLUDE_FIELD_NAMES 1
#define MT2_CONFIG_INCLUDE_FIELD_INFO 1
#define MT2_CONFIG_INCLUDE_RESET_VALUES 1
#define MT2_CONFIG_INCLUDE_TEST_MASK 1

typedef struct mt2_sym_s {
        uint32 addr;
#if MT2_CONFIG_INCLUDE_FIELD_INFO == 1
        uint32* fields;
#endif
        uint32 index;   /* "size" for registers */
        uint32 flags;
        uint32 blktypes;
        uint32 block;
        const char *name;
#if MT2_CONFIG_INCLUDE_RESET_VALUES == 1
        uint32 rstval;
        uint32 rstval_hi;
#endif
#if MT2_CONFIG_INCLUDE_TEST_MASK == 1
        uint32 test_mask;
        uint32 test_mask_hi;
#endif
} mt2_sym_t;

/* Flags */
#define MT2_SYMBOL_FLAG_REGISTER 0x00000001
#define MT2_SYMBOL_FLAG_R64      0x00000002
#define MT2_SYMBOL_FLAG_PORT     0x00000004
#define MT2_SYMBOL_FLAG_SOFT_PORT 0x00000008
#define MT2_SYMBOL_FLAG_MEMORY   0x00000010
#define MT2_SYMBOL_FLAG_COUNTER  0x00000020
#define MT2_SYMBOL_FLAG_MEMMAPPED 0x00000040
#define MT2_SYMBOL_FLAG_NOTEST   0x00000080
#define MT2_SYMBOL_FLAG_READONLY 0x00000100

/* Size & Index.. Encodingt TBD */
#define MT2_SYMBOL_INDEX_SIZE_ENCODE(x) (x)
#define MT2_SYMBOL_INDEX_MIN_ENCODE(x) (x << 8)
#define MT2_SYMBOL_INDEX_MAX_ENCODE(x) (x<<16)

#define MT2_SYMBOL_INDEX_SIZE_DECODE(x) (x & 0xff)
#define MT2_SYMBOL_INDEX_MIN_DECODE(x) ((x >> 8) & 0xff)
#define MT2_SYMBOL_INDEX_MAX_DECODE(x) (x >> 16)

#define CDK_SYMBOL_FIELD_FLAG_LAST (((uint32)1) << 31)
#define CDK_SYMBOL_FIELD_ENCODE(x, y, z)  ((x << 16) | (y << 8) | z)

extern const mt2_sym_t phy8806x_syms[];
extern const uint32 phy8806x_syms_numels;
extern const char* phy8806x_fields[];


/* forward definitions of the access functions */
extern int mt2_xgsd_reg32_block_read(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg32_block_write(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_block_read(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_block_write(int unit, uint32 blkacc, int block,
				uint32 offset, int idx, void *data);

extern int mt2_xgsd_reg32_port_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg32_port_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_port_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg64_port_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);

extern int mt2_xgsd_reg32_port_speed_read(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);
extern int mt2_xgsd_reg32_port_speed_write(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);
extern int mt2_xgsd_reg64_port_speed_read(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);
extern int mt2_xgsd_reg64_port_speed_write(int unit, uint32 blkacc, int port,
					uint32 offset, int idx, int speed,
					void *data);

extern int mt2_xgsd_reg32_blocks_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);
extern int mt2_xgsd_reg32_blocks_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data);

extern int mt2_xgsd_mem_blocks_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *entry_data,
				int size);
extern int mt2_xgsd_mem_blocks_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *entry_data,
				int size);
extern int mt2_xgsd_mem_read(int unit, uint32 block, uint32 addr,
			uint32 idx, void *vptr, int size);
extern int mt2_xgsd_mem_write(int unit, uint32 block, uint32 addr,
			uint32 idx, void *vptr, int size);

extern uint32 *mt2_field_get(const uint32 * entbuf, int sbit, int ebit,
			uint32 * fbuf);
extern void mt2_field_set(uint32 * entbuf, int sbit, int ebit,
			uint32 * fbuf);
extern uint32 mt2_field32_get(const uint32 * entbuf, int sbit, int ebit);
extern void mt2_field32_set(uint32 * entbuf, int sbit, int ebit,
			uint32 fval);

extern int mt2_sbus_reg_read(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, uint32 * data);
extern int mt2_sbus_reg_write(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, uint32 * data);
extern int mt2_sbus_mem_read(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, int idx, uint32 * data);
extern int mt2_sbus_mem_write(int unit, uint16 phyaddr, int blknum,
			mt2_sym_t *mt2_sym, int idx, uint32 * data);
extern int mt2_sbus_to_tsc_read(int unit, uint16 phyaddr, uint32 addr,
			uint32 * data);
extern int mt2_sbus_to_tsc_write(int unit, uint16 phyaddr, uint32 addr,
			uint32 * data);
extern int mt2_axi_read(int unit, uint16 phyaddr, uint32 regaddr,
			uint32 * data);
extern int mt2_axi_write(int unit, uint16 phyaddr, uint32 regaddr,
			uint32 * data);

#endif  /* _PHY8806X_SYMS_H_ */

