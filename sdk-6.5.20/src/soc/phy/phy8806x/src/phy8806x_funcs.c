/*******************************************************************************
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <soc/error.h>
#include "phy8806x_defs.h"
#include "phy8806x_syms.h"
#include <shared/bsl.h>

/* Block Numbers */
#define PHY8806X_BLK_FCMAC_32G         0
#define PHY8806X_BLK_FCMAC_8G          1
#define PHY8806X_BLK_LINESIDE_CL_PM    2
#define PHY8806X_BLK_LINE_FCPORT       3
#define PHY8806X_BLK_MAPPER            4
#define PHY8806X_BLK_PSM               5
#define PHY8806X_BLK_SYSSIDE_CL_PM     6
#define PHY8806X_BLK_SYS_FCPORT        7

extern int
soc_miimc45_read(int unit, uint16 phy_id, uint8 phy_devad,
                 uint16 phy_reg_addr, uint16 *phy_rd_data);
extern int
soc_miimc45_write(int unit, uint16 phy_id, uint8 phy_devad,
                 uint16 phy_reg_addr, uint16 phy_wr_data);

/*
 * Common functions - One Size Fits All
 */
int mt2_xgsd_reg_read(int unit, uint32 block, uint32 addr,
                      uint32 * data, int size)
{
    /* Implement when Necessary */
    return 0;
}

int mt2_xgsd_reg_write(int unit, uint32 block, uint32 addr,
                       uint32 * data, int size)
{
    return 0;
}

int mt2_xgsd_mem_read(int unit, uint32 block, uint32 addr, uint32 idx,
                      void *vptr, int size)
{
    return 0;
}

int mt2_xgsd_mem_write(int unit, uint32 block, uint32 addr, uint32 idx,
                       void *vptr, int size)
{
    return 0;
}

/* Calculate address of port-based register */
uint32 mt2_xgsd_port_block(int unit, uint32 blkacc, int port, int speed)
{
    

    uint32 block = 0;

    switch (blkacc) {
    case ((1 << PHY8806X_BLKTYPE_LINESIDE_CL_PM) | (1 << PHY8806X_BLKTYPE_SYSSIDE_CL_PM)):
        block = (port < 4) ? PHY8806X_BLK_LINESIDE_CL_PM : PHY8806X_BLK_SYSSIDE_CL_PM;
        break;
    case ((1 << PHY8806X_BLKTYPE_LINE_FCPORT) | (1 << PHY8806X_BLKTYPE_SYS_FCPORT)):
        block = (port < 4) ? PHY8806X_BLK_LINE_FCPORT : PHY8806X_BLK_SYS_FCPORT;
        break;
    case (1 << PHY8806X_BLKTYPE_FCMAC):
        block = (speed == 32) ? PHY8806X_BLK_FCMAC_32G : PHY8806X_BLK_FCMAC_8G;
        break;
    case (1 << PHY8806X_BLKTYPE_MAPPER):
        block = PHY8806X_BLK_MAPPER;
        break;
    case (1 << PHY8806X_BLKTYPE_PSM):
        block = PHY8806X_BLK_PSM;
        break;
    }
    return block;
}

mt2_sym_t *mt2_syms_find_name(char *c)
{
    int i;
    for (i=0; i<phy8806x_syms_numels; i++)
    {
        if (sal_strcasecmp(c, phy8806x_syms[i].name) == 0)
        {
            return (mt2_sym_t *)&phy8806x_syms[i];
        }
    }
    return NULL;
}


const char *
mt2_strcasestr(const char *s, const char *sub)
{
    int len;

    for (len = sal_strlen(sub); *s; s++) {
        if (sal_strncasecmp(s, sub, len) == 0) {
            return s;
        }
    }

    return 0;
}

int mt2_syms_find_next_name(char *c, mt2_sym_t **mt2_sym, int startidx)
{
    int i;
    for (i=startidx; i<phy8806x_syms_numels; i++)
    {
        if (mt2_strcasestr(phy8806x_syms[i].name, c) != NULL)
        {
            *mt2_sym = (mt2_sym_t *)&phy8806x_syms[i];
            i = (i < (phy8806x_syms_numels - 1)) ? (i+1) : -1;
            return i;
        }
    }
    return -1;
}

/*
 * Registers Read Write functions
 */

int mt2_xgsd_reg32_block_read(int unit, uint32 blkacc, int block,
			uint32 offset, int idx, void *data)
{
	uint32 addr = offset | (1 << 25);	/* RT = 1 */
	return mt2_xgsd_reg_read(unit, (uint32) block, addr,
				(uint32 *) data, 1);
}

int mt2_xgsd_reg32_block_write(int unit, uint32 blkacc, int block,
			uint32 offset, int idx, void *data)
{
	uint32 addr = offset | (1 << 25);	/* RT = 1 */
	return mt2_xgsd_reg_write(unit, (uint32) block, addr,
				(uint32 *) data, 1);
}

int mt2_xgsd_reg64_block_read(int unit, uint32 blkacc, int block,
			uint32 offset, int idx, void *data)
{
	uint32 addr = offset | (1 << 25);	/* RT = 1 */
	return mt2_xgsd_reg_read(unit, (uint32) block, addr,
				(uint32 *) data, 2);
}

int mt2_xgsd_reg64_block_write(int unit, uint32 blkacc, int block,
			uint32 offset, int idx, void *data)
{
	uint32 addr = offset | (1 << 25);	/* RT = 1 */
	return mt2_xgsd_reg_write(unit, (uint32) block, addr,
				(uint32 *) data, 2);
}

int mt2_xgsd_reg32_port_read(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_read(unit, block, addr, (uint32 *) data, 1);
}

int mt2_xgsd_reg32_port_write(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_write(unit, block, addr, (uint32 *) data, 1);
}

int mt2_xgsd_reg64_port_read(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_read(unit, block, addr, (uint32 *) data, 2);
}

int mt2_xgsd_reg64_port_write(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_write(unit, block, addr, (uint32 *) data, 2);
}

int mt2_xgsd_reg32_port_speed_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, int speed,
				void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, speed);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_read(unit, block, addr, (uint32 *) data, 1);
}

int mt2_xgsd_reg32_port_speed_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, int speed,
				void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, speed);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_write(unit, block, addr, (uint32 *) data, 1);
}

int mt2_xgsd_reg64_port_speed_read(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, int speed,
				void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, speed);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_read(unit, block, addr, (uint32 *) data, 2);
}

int mt2_xgsd_reg64_port_speed_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, int speed,
				void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, speed);
	addr = offset | (port % 4);

	return mt2_xgsd_reg_write(unit, block, addr, (uint32 *) data, 2);
}

int mt2_xgsd_reg32_blocks_read(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);
	addr = offset | (1 << 25);	/* RT = 1 . Non-Per-Port Register */

	return mt2_xgsd_reg_read(unit, block, addr, (uint32 *) data, 1);
}

int mt2_xgsd_reg32_blocks_write(int unit, uint32 blkacc, int port,
				uint32 offset, int idx, void *data)
{
	uint32 addr, block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);
	addr = offset | (1 << 25);

	return mt2_xgsd_reg_write(unit, block, addr, (uint32 *) data, 1);
}

int mt2_xgsd_mem_blocks_read(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *entry_data,
			int size)
{
	uint32 block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);

	return mt2_xgsd_mem_read(unit, block, offset, idx, entry_data, size);
}

int mt2_xgsd_mem_blocks_write(int unit, uint32 blkacc, int port,
			uint32 offset, int idx, void *entry_data,
			int size)
{
	uint32 block;

	block = mt2_xgsd_port_block(unit, blkacc, port, 0);

	return mt2_xgsd_mem_write(unit, block, offset, idx, entry_data, size);
}

/*
 * Function:
 *      mt2_field_get
 * Purpose:
 *      Extract multi-word field value from multi-word register/memory.
 * Parameters:
 *      entbuf - current contents of register/memory (word array)
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 *      fbuf - buffer where to store extracted field value
 * Returns:
 *      Pointer to extracted field value.
 */
uint32 *mt2_field_get(const uint32 * entbuf, int sbit, int ebit,
			uint32 * fbuf)
{
	int i, wp, bp, len;

	bp = sbit;
	len = ebit - sbit + 1;

	wp = bp / 32;
	bp = bp & (32 - 1);
	i = 0;

	for (; len > 0; len -= 32, i++) {
		if (bp) {
			fbuf[i] = (entbuf[wp++] >> bp) & ((1 << (32 - bp)) - 1);
			if (len > (32 - bp)) {
				fbuf[i] |= entbuf[wp] << (32 - bp);
			}
		} else {
			fbuf[i] = entbuf[wp++];
		}
		if (len < 32) {
			fbuf[i] &= ((1 << len) - 1);
		}
	}

	return fbuf;
}

/*
 * Function:
 *      mt2_field_set
 * Purpose:
 *      Assign multi-word field value in multi-word register/memory.
 * Parameters:
 *      entbuf - current contents of register/memory (word array)
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 *      fbuf - buffer with new field value
 * Returns:
 *      Nothing.
 */
void mt2_field_set(uint32 * entbuf, int sbit, int ebit, uint32 * fbuf)
{
	uint32 mask;
	int i, wp, bp, len;

	bp = sbit;
	len = ebit - sbit + 1;

	wp = bp / 32;
	bp = bp & (32 - 1);
	i = 0;

	for (; len > 0; len -= 32, i++) {
		if (bp) {
			if (len < 32) {
				mask = (1 << len) - 1;
			} else {
				mask = ~0;
			}
			entbuf[wp] &= ~(mask << bp);
			entbuf[wp++] |= fbuf[i] << bp;
			if (len > (32 - bp)) {
				entbuf[wp] &= ~(mask >> (32 - bp));
				entbuf[wp] |= fbuf[i] >> (32 - bp) & ((1 << bp) - 1);
			}
		} else {
			if (len < 32) {
				mask = (1 << len) - 1;
				entbuf[wp] &= ~mask;
				entbuf[wp++] |= (fbuf[i] & mask) << bp;
			} else {
				entbuf[wp++] = fbuf[i];
			}
		}
	}
}

/*
 * Function:
 *      mt2_field32_get
 * Purpose:
 *      Extract field value from multi-word register/memory.
 * Parameters:
 *      entbuf - current contents of register/memory (word array)
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 * Returns:
 *      Extracted field value.
 */
uint32 mt2_field32_get(const uint32 * entbuf, int sbit, int ebit)
{
	uint32 fval;

	mt2_field_get(entbuf, sbit, ebit, &fval);

	return fval;
}

/*
 * Function:
 *      mt2_field32_set
 * Purpose:
 *      Assign field value in multi-word register/memory.
 * Parameters:
 *      entbuf - current contents of register/memory (word array)
 *      sbit - bit number of first bit of the field to extract
 *      ebit - bit number of last bit of the field to extract
 * Returns:
 *      Nothing.
 */
void mt2_field32_set(uint32 * entbuf, int sbit, int ebit, uint32 fval)
{
	mt2_field_set(entbuf, sbit, ebit, &fval);
}

/* Read registers from external host via MDIO */

int mt2_sbus_reg_read(int unit, uint16 phyaddr, int blknum, mt2_sym_t *mt2_sym,
		uint32 * data)
{
    int    i, rt = 5, rv = SOC_E_TIMEOUT;
    uint32 reg_addr = mt2_sym->addr;
    uint32 reg_blk = (blknum == -1) ? mt2_sym->block : (uint32)blknum;
    uint32 sbus_cmd;
    uint16 sbus_ctrl;
    uint16 sbus_stat;
    uint16 reg_data[4];

    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_MDIO)) {
        rv = soc_miimc45_read(unit, phyaddr, 1, reg_addr, &reg_data[0]);
        data[0] = reg_data[0];
        return rv;
    }

    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_UCSS)) {
        /* Different cores at different addresses */
        /* reg_addr += ((phyaddr % 4) * 0x1000000) + 0x02000000; */
        return mt2_axi_read(unit, phyaddr, reg_addr, data);
    }

    if (mt2_sym->flags & MT2_SYMBOL_FLAG_PORT) {
        reg_addr |= (phyaddr % 4);
    } else {
        reg_addr |= 0x02000000;
    }
    /*
    reg_addr &= 0x03ffffff;
    reg_addr |= ((reg_blk & 0x3f) << 26);
     */

retry:
    sbus_cmd = (0x0b << 26) |   /* Reg Read CMD */
        ((reg_blk & 0x7f) << 19) | ((mt2_sym->index) << 7);

    /* Ports 0-3 = MIG0, 4-7 = MIG1 */
    sbus_ctrl = ((phyaddr % 8) < 4) ? 0 : 0x1000;
    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_TOP)) {
        sbus_ctrl = 0x2000;
    }
/*    printf("Read:\n"); */
    
/*    printf("Addr Word 0x%08x\n", reg_addr);
    printf("Command Word 0x%08x\n", sbus_cmd);
    printf("Control Word 0x%08x\n", sbus_ctrl);
*/
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_UPPERr, (uint16)(reg_addr >> 16)));
/*    printf("\t 0x8020 : 0x%04x\n", (uint16)(reg_addr >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_LOWERr, (uint16)(reg_addr & 0xffff)));
/*    printf("\t 0x8021 : 0x%04x\n", (uint16)(reg_addr & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_UPPERr, (uint16)(sbus_cmd >> 16)));
/*    printf("\t 0x8022 : 0x%04x\n", (uint16)(sbus_cmd >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_LOWERr, (uint16)(sbus_cmd & 0xffff)));
/*    printf("\t 0x8023 : 0x%04x\n", (uint16)(sbus_cmd & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */
    sbus_ctrl |= 0x8000;    /*Transaction starts when "valid" bit toggles from 0 to 1 */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */

    for (i = 0; i < 100; i++) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_STATUSr, &sbus_stat));
        if (sbus_stat & 0x8000) { /* Complete */
            if (sbus_stat & 0x4000) { /* Error */
                LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "  Error..\n")));
                if (rt-- > 0) {
                    LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "  Retrying\n")));
                    goto retry;
                }
            } else {
                rv = SOC_E_NONE;
            }
            break;
        }
        sal_usleep(1000);
    }
/*    printf("SBUS Status is 0x%04x\n", sbus_stat); */

    if (rv == SOC_E_NONE) {
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_1_0r, &reg_data[0]));
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_3_2r, &reg_data[1]));
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_5_4r, &reg_data[2]));
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_7_6r, &reg_data[3]));

        data[0] = reg_data[0] | (reg_data[1] << 16);
        data[1] = reg_data[2] | (reg_data[3] << 16);

        if (mt2_sym->index == 2) {
            data[0] &= 0xffff;
        }
    }
    return rv;
}

int mt2_sbus_reg_write(int unit, uint16 phyaddr, int blknum, mt2_sym_t *mt2_sym,
		uint32 * data)
{
    int    i, rt = 5, rv = SOC_E_TIMEOUT;
    uint32 reg_addr = mt2_sym->addr;
    uint32 reg_blk = (blknum == -1) ? mt2_sym->block : (uint32)blknum;
    uint32 sbus_cmd;
    uint16 sbus_ctrl;
    uint16 sbus_stat;
    uint16 reg_data[4];

    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_MDIO)) {
        reg_data[0] = data[0] & 0xffff;
        return soc_miimc45_write(unit, phyaddr, 1, reg_addr, reg_data[0]);
    }

    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_UCSS)) {
        /* Different cores at different addresses */
        /* reg_addr += ((phyaddr % 4) * 0x1000000) + 0x02000000; */
        return mt2_axi_write(unit, phyaddr, reg_addr, data);
    }

    if (mt2_sym->flags & MT2_SYMBOL_FLAG_PORT) {
        reg_addr |= (phyaddr % 4);
    } else {
        reg_addr |= 0x02000000;
    }
    /*
    reg_addr &= 0x03ffffff;
    reg_addr |= ((reg_blk & 0x3f) << 26);
     */

retry:

    sbus_cmd = (0x0d << 26) |   /* Reg Write CMD */
        ((reg_blk & 0x7f) << 19) | ((mt2_sym->index) << 7);

    /* Ports 0-3 = MIG0, 4-7 = MIG1 */
    sbus_ctrl = ((phyaddr % 8) < 4) ? 0 : 0x1000;
    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_TOP)) {
        sbus_ctrl = 0x2000;
    }

    reg_data[0] = data[0] & 0xffff;
    reg_data[1] = data[0] >> 16;
    reg_data[2] = data[1] & 0xffff;
    reg_data[3] = data[1] >> 16;

/*printf("Write:\n"); */

/*    printf("Addr Word 0x%08x\n", reg_addr);
    printf("Command Word 0x%08x\n", sbus_cmd);
    printf("Control Word 0x%08x\n", sbus_ctrl);
*/
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_UPPERr, (uint16)(reg_addr >> 16)));
/*    printf("\t 0x8020 : 0x%04x\n", (uint16)(reg_addr >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_LOWERr, (uint16)(reg_addr & 0xffff)));
/*    printf("\t 0x8021 : 0x%04x\n", (uint16)(reg_addr & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_UPPERr, (uint16)(sbus_cmd >> 16)));
/*    printf("\t 0x8022 : 0x%04x\n", (uint16)(sbus_cmd >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_LOWERr, (uint16)(sbus_cmd & 0xffff)));
/*    printf("\t 0x8023 : 0x%04x\n", (uint16)(sbus_cmd & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */

    SOC_IF_ERROR_RETURN (soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_1_0r, reg_data[0]));
/*    printf("\t 0x8024 : 0x%04x\n", reg_data[0]); */
    SOC_IF_ERROR_RETURN (soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_3_2r, reg_data[1]));
/*    printf("\t 0x8025 : 0x%04x\n", reg_data[1]); */
    SOC_IF_ERROR_RETURN (soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_5_4r, reg_data[2]));
/*    printf("\t 0x8026 : 0x%04x\n", reg_data[2]); */
    SOC_IF_ERROR_RETURN (soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_DATA_BYTES_7_6r, reg_data[3]));
/*    printf("\t 0x8027 : 0x%04x\n", reg_data[3]); */

    sbus_ctrl |= 0x8000;    /*Transaction starts when "valid" bit toggles from 0 to 1 */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */
    

    for (i = 0; i < 100; i++) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_STATUSr, &sbus_stat));
        if (sbus_stat & 0x8000) { /* Complete */
            if (sbus_stat & 0x4000) { /* Error */
                LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "  Error..\n"))); 
                if (rt-- > 0) {
                    LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "  Retrying\n")));
                    goto retry;
                }
            } else {
                rv = SOC_E_NONE;
            }
            break;
        }
        sal_usleep(1000);
    }
/*    printf("SBUS Status is 0x%04x\n", sbus_stat); */

    return rv;
}

int mt2_sbus_mem_read(int unit, uint16 phyaddr, int blknum, mt2_sym_t *mt2_sym,
		int idx, uint32 * data)
{
    int    i, rv = SOC_E_TIMEOUT;
    uint32 sbus_addr = mt2_sym->addr + idx;
    uint32 mem_blk = (blknum == -1) ? mt2_sym->block : (uint32)blknum;
    uint32 sbus_cmd;
    uint16 sbus_ctrl;
    uint16 sbus_stat;
    uint16 sbus_data[8];
    int cnt = (mt2_sym->index & 0xff) / 4;    /* Number of 32-bit words */

    sbus_cmd = (0x07 << 26) |   /* Mem Read CMD */
        ((mem_blk & 0x7f) << 19) | ((MT2_SYMBOL_INDEX_SIZE_DECODE(mt2_sym->index)) << 7);

    /* Ports 0-3 = MIG0, 4-7 = MIG1 */
    sbus_ctrl = ((phyaddr % 8) < 4) ? 0 : 0x1000;
    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_TOP)) {
        sbus_ctrl = 0x2000;
    }
/*    printf("Read:\n"); */
    
/*    printf("Addr Word 0x%08x\n", sbus_addr);
    printf("Command Word 0x%08x\n", sbus_cmd);
    printf("Control Word 0x%08x\n", sbus_ctrl);
*/
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_UPPERr, (uint16)(sbus_addr >> 16)));
/*    printf("\t 0x8020 : 0x%04x\n", (uint16)(sbus_addr >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_LOWERr, (uint16)(sbus_addr & 0xffff)));
/*    printf("\t 0x8021 : 0x%04x\n", (uint16)(sbus_addr & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_UPPERr, (uint16)(sbus_cmd >> 16)));
/*    printf("\t 0x8022 : 0x%04x\n", (uint16)(sbus_cmd >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_LOWERr, (uint16)(sbus_cmd & 0xffff)));
/*    printf("\t 0x8023 : 0x%04x\n", (uint16)(sbus_cmd & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */
    sbus_ctrl |= 0x8000;    /*Transaction starts when "valid" bit toggles from 0 to 1 */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */

    for (i = 0; i < 100; i++) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_STATUSr, &sbus_stat));
        if (sbus_stat & 0x8000) {
            rv = SOC_E_NONE;
            break;
        }
        sal_usleep(1000);
    }
/*    printf("SBUS Status is 0x%04x\n", sbus_stat); */

    if (rv == SOC_E_NONE) {
        for (i=0; i<(cnt * 2); i++) {   /* Number of 16 bit words */
            SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, (PHY8806X_SBUS_DATA_BYTES_1_0r + i), &sbus_data[i]));
        }
        for (i=0; i<cnt; i++) {   /* Number of 32 bit words */
            data[i] = sbus_data[i*2] | (sbus_data[(i*2)+1] << 16);
        }
    }
    return rv;
}

int mt2_sbus_mem_write(int unit, uint16 phyaddr, int blknum, mt2_sym_t *mt2_sym,
		int idx, uint32 * data)
{
    int    i, rv = SOC_E_TIMEOUT;
    uint32 sbus_addr = mt2_sym->addr + idx;
    uint32 mem_blk = (blknum == -1) ? mt2_sym->block : (uint32)blknum;
    uint32 sbus_cmd;
    uint16 sbus_ctrl;
    uint16 sbus_stat;
    uint16 sbus_data[8];
    int cnt = (mt2_sym->index & 0xff) / 4;    /* Number of 32-bit words */

    sbus_cmd = (0x09 << 26) |   /* Mem Write CMD */
        ((mem_blk & 0x7f) << 19) | ((MT2_SYMBOL_INDEX_SIZE_DECODE(mt2_sym->index)) << 7);

    /* Ports 0-3 = MIG0, 4-7 = MIG1 */
    sbus_ctrl = ((phyaddr % 8) < 4) ? 0 : 0x1000;
    if (mt2_sym->blktypes == (1 << PHY8806X_BLKTYPE_TOP)) {
        sbus_ctrl = 0x2000;
    }

    for (i=0; i<cnt; i++) {
        sbus_data[i*2] = data[i] & 0xffff;
        sbus_data[(i*2)+1] = data[i] >> 16;
    }

/* printf("Write:\n"); */

/*    printf("Addr Word 0x%08x\n", sbus_addr);
    printf("Command Word 0x%08x\n", sbus_cmd);
    printf("Control Word 0x%08x\n", sbus_ctrl);
*/
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_UPPERr, (uint16)(sbus_addr >> 16)));
/*    printf("\t 0x8020 : 0x%04x\n", (uint16)(sbus_addr >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_ADDR_LOWERr, (uint16)(sbus_addr & 0xffff)));
/*    printf("\t 0x8021 : 0x%04x\n", (uint16)(sbus_addr & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_UPPERr, (uint16)(sbus_cmd >> 16)));
/*    printf("\t 0x8022 : 0x%04x\n", (uint16)(sbus_cmd >> 16)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_COMMAND_LOWERr, (uint16)(sbus_cmd & 0xffff)));
/*    printf("\t 0x8023 : 0x%04x\n", (uint16)(sbus_cmd & 0xffff)); */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */


    for (i=0; i<(cnt * 2); i++) {   /* Number of 16 bit words */
        SOC_IF_ERROR_RETURN (soc_miimc45_write(unit, phyaddr, 1, (PHY8806X_SBUS_DATA_BYTES_1_0r + i), sbus_data[i]));
    }

    sbus_ctrl |= 0x8000;    /*Transaction starts when "valid" bit toggles from 0 to 1 */
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_SBUS_CONTROLr, sbus_ctrl));
/*    printf("\t 0x802c : 0x%04x\n", sbus_ctrl); */
    

    for (i = 0; i < 100; i++) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_read(unit, phyaddr, 1, PHY8806X_SBUS_STATUSr, &sbus_stat));
        if (sbus_stat & 0x8000) {
            rv = SOC_E_NONE;
            break;
        }
        sal_usleep(1000);
    }
/*    printf("SBUS Status is 0x%04x\n", sbus_stat); */

    return rv;
}

int mt2_sbus_to_tsc_read(int unit, uint16 phyaddr, uint32 addr,
			uint32 * data)
{
    uint32 to_data[4];
    mt2_sym_t *ucmem_data_sym = mt2_syms_find_name("FCPORT_WC_UCMEM_DATAm");
    int port,blk;

    if (ucmem_data_sym == NULL) {
        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Error: UCMEM_DATAm not found\n")));
        return SOC_E_FAIL;
    }

    port = (addr >> 19) & 0x1f;     /* Port is 1 thru 16 */
    blk = (port <= 8) ? PHY8806X_BLK_LINE_FCPORT : PHY8806X_BLK_SYS_FCPORT;

    to_data[0] = addr;
    to_data[1] = 0;
    to_data[2] = 0;     /* Read Operation */
    to_data[3] = 0;
    mt2_sbus_mem_write(unit, phyaddr, blk, ucmem_data_sym,0, to_data);
    mt2_sbus_mem_read(unit, phyaddr, blk, ucmem_data_sym,0, to_data);
    data[0] = to_data[1] & 0xffff;
    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit, "to_tsc_read %s indx 0x%08x\n"),
                              ucmem_data_sym->name, ucmem_data_sym->index));
    return SOC_E_NONE;
}

int mt2_sbus_to_tsc_write(int unit, uint16 phyaddr, uint32 addr,
			uint32 * data)
{
    uint32 to_data[4];
    mt2_sym_t *ucmem_data_sym = mt2_syms_find_name("FCPORT_WC_UCMEM_DATAm");
    int port,blk;

    if (ucmem_data_sym == NULL) {
        LOG_ERROR(BSL_LS_SOC_PHY, (BSL_META_U(unit, "Error: UCMEM_DATAm not found\n")));
        return SOC_E_FAIL;
    }

    port = (addr >> 19) & 0x1f;     /* Port is 1 thru 16 */
    blk = (port <= 8) ? PHY8806X_BLK_LINE_FCPORT : PHY8806X_BLK_SYS_FCPORT;

    to_data[0] = addr;
    to_data[1] = data[0];
    to_data[2] = 1;     /* Write Operation */
    to_data[3] = 0;
    mt2_sbus_mem_write(unit, phyaddr, blk, ucmem_data_sym,0, to_data);

    LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit, "to_tsc_write %s indx 0x%08x\n"),
                              ucmem_data_sym->name, ucmem_data_sym->index));
    return SOC_E_NONE;
}

int mt2_axi_read(int unit, uint16 phyaddr, uint32 regaddr,
		uint32 * data)
{
    int    i, rv = SOC_E_TIMEOUT;
    uint16 miimdata[4];
    uint16 miimstatus;

/* printf ("phy addr 0x%x , regaddr 0x%x\n", phyaddr, regaddr); */
    data[0] = 0;
    data[1] = 0;

    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_ADDR_UPPER_F0r, (uint16)(regaddr >> 16)));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_ADDR_LOWER_F0r, (uint16)(regaddr & 0xffff)));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_1_0_F0r, 0));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_3_2_F0r, 0));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_5_4_F0r, 0));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_7_6_F0r, 0));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_CONTROL_F0r, 0xc000)); /* AXI Control : 4 Bytes, read */

    for (i = 0; i < 100; i++) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_read(unit, phyaddr, 1, PHY8806X_AXI_STATUS_F0r, &miimstatus));
        if (miimstatus & 0x8000) {
            rv = SOC_E_NONE;
            break;
        }
        sal_usleep(1000);
/*        printf("miimstatus %08x\n", miimstatus); */
    }

    if (rv == SOC_E_NONE) {
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_1_0_F0r, &miimdata[0]));
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_3_2_F0r, &miimdata[1]));
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_5_4_F0r, &miimdata[2]));
        SOC_IF_ERROR_RETURN (soc_miimc45_read(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_7_6_F0r, &miimdata[3]));

        if (regaddr & 4) {
            data[0] = miimdata[2] | (miimdata[3] << 16);
        } else {
            data[0] = miimdata[0] | (miimdata[1] << 16);
        }
    }
/*  printf("axi_read at 0x%08x = 0x%08x rv %d\n", regaddr, data[0], rv); */
    return rv;
}

int mt2_axi_write(int unit, uint16 phyaddr, uint32 regaddr,
		uint32 * data)
{
    int    i, rv = SOC_E_TIMEOUT;
    uint16 miimstatus;

    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_ADDR_UPPER_F0r, (uint16)(regaddr >> 16)));
    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_ADDR_LOWER_F0r, (uint16)(regaddr & 0xffff)));
    if (regaddr & 4) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_5_4_F0r, data[0] & 0xffff));
        SOC_IF_ERROR_RETURN (
            soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_7_6_F0r, data[0] >> 16));
    } else {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_1_0_F0r, data[0] & 0xffff));
        SOC_IF_ERROR_RETURN (
            soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_DATA_BYTES_3_2_F0r, data[0] >> 16));
    }

    SOC_IF_ERROR_RETURN (
        soc_miimc45_write(unit, phyaddr, 1, PHY8806X_AXI_CONTROL_F0r, 0x8000)); /* AXI Control : 4 Bytes, write */

    for (i = 0; i < 100; i++) {
        SOC_IF_ERROR_RETURN (
            soc_miimc45_read(unit, phyaddr, 1, PHY8806X_AXI_STATUS_F0r, &miimstatus));
        if (miimstatus & 0x8000) {
            rv = SOC_E_NONE;
            break;
        }
        sal_usleep(1000);
    }

/* printf("axi_read at 0x%08x = 0x%08x rv %d\n", regaddr, data[0], rv); */
    return rv;
}

