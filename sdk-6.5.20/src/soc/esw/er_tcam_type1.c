/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      er_tcam_type1.c
 */

#include <soc/error.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH_SUPPORT)
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#include <soc/er_cmdmem.h>
#include <soc/er_tcam.h>
#include <soc/tcam/tcamtype1.h>

#ifdef BCM_TRIUMPH3_SUPPORT
typedef struct _seg_info {
    uint8 seg_offset[10];
    uint8 seg_len[10];
} seg_info_t;

typedef struct _ltr_info {
    /* to get the blocks for BSR */
    soc_tcam_partition_type_t partition[4][NUM_PARTITIONS_PER_SEARCH]; 

    /* one kpu per set of super blocks in the search */
    int sblk_kpu_select[4]; 

    /* one search result port per set of blocks in the search */
    int parallel_srch[4]; 

    int num_searches;

    /* block mask register select for each of the parallel searches */
    uint8 bmr_select[4]; 

    /* one key construction register per kpu */
    seg_info_t key_info[4]; 

    /* false for now */
    uint8 enable_range; 
} ltr_info_t;

/* per unit ltr installation info */
typedef struct _ltr_install_s {
    uint8 nl_ltr;
    uint8 installed;
    uint8 ref_count;
} ltr_install_t;

typedef struct _fm_ltrs_s {
    ltr_info_t ltr;
    struct _ltr_install_s *nl_ltr_info[SOC_MAX_NUM_DEVICES];
} fm_ltrs_t;

/* indexed by keygen offset */
fm_ltrs_t ltr_profiles[128]; 

#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
type1_tr_enable_next_dev(int unit)
{
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[2];
    int inst_index;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = 0;
    ibus[inst_index++] = TYPE1_OPCODE_ENABLE_NEXT_DEV;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = 0;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    return soc_triumph_tcam_access(unit, TCAM_TR_OP_WRITE, inst_index, 0, 1,
                                   dbus, ibus);
}

/* Can't use regular register write to write bmr if require workaround */
STATIC int
type1_tr_write_bmr(int unit, int addr, uint32 d0_msb, uint32 d1, uint32 d2_lsb)
{
    uint32 dbus[6 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[6];
    int inst_index;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb & 0xc0000000;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb & 0xc0000000;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb & 0xc0000000;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] |= addr;
    ibus[inst_index++] = TYPE1_OPCODE_WR_REG;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb & 0xc0000000;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] |= addr;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    return soc_triumph_tcam_access(unit, TCAM_TR_OP_WRITE, inst_index, 0, 2,
                                   dbus, ibus);
}

STATIC int
type1_tr_write_reg(int unit, int addr, uint32 d0_msb, uint32 d1, uint32 d2_lsb)
{
    uint32 dbus[4 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[4];
    int inst_index;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_WR_REG;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = d2_lsb;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = d1;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = d0_msb;
    ibus[inst_index++] = TYPE1_OPCODE_NOP;

    return soc_triumph_tcam_access(unit, TCAM_TR_OP_WRITE, inst_index, 0, 2,
                                   dbus, ibus);
}

#ifdef BCM_TRIUMPH3_SUPPORT
/* d0_msb contains upper 16 bits of write data */
STATIC int
tr3_write_reg(int unit, int addr, uint32 d0_msb, uint32 d1, uint32 d2_lsb)
{
    /* convert to byte array for easy swapping */
    uint32 dbus[6];
    int ibus[4];
    int inst_index, num_pre_nops = 0;
	int tmp;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[1] = MAKE_WORD(GET_BYTE(addr, 0), GET_BYTE(addr, 1), GET_BYTE(addr, 2),
                        (GET_BYTE(addr, 3) & 0x3));

    dbus[0] = MAKE_WORD(GET_BYTE(d2_lsb, 0), GET_BYTE(d2_lsb, 1), 
                        GET_BYTE(d2_lsb, 2), GET_BYTE(d2_lsb, 3));

    dbus[3] = MAKE_WORD(GET_BYTE(d1, 0), GET_BYTE(d1, 1), GET_BYTE(d1, 2), 
                        GET_BYTE(d1, 3));

    dbus[2] = MAKE_WORD(GET_BYTE(d0_msb, 0), GET_BYTE(d0_msb, 1), 0, 0);
    ibus[inst_index++] = TR3_OPCODE_WR_REG_DB;

    /* if it is a block config register insert 2 nops before write */
	tmp = addr & 0xfffff;
	if (((tmp % 32) == 0) && ((tmp & 0xf0000) == 0)) {
		num_pre_nops = 2;
    }
    if (LOG_CHECK(BSL_LS_SOC_TCAM | BSL_INFO)) {
        LOG_CLI((BSL_META_U(unit,
                            "Write Addr=0x%08x "), addr));
        LOG_CLI((BSL_META_U(unit,
                            "Data = 0x%08x 0x%08x 0x%08x\n"), d0_msb, d1, d2_lsb));
    }
    /* num_bits to write includes all data being written */
    return soc_triumph3_tcam_access(unit, TCAM_TR3_OP_WRITE, inst_index, 
                                    num_pre_nops, 0, (uint32 *)dbus, ibus, 
                                    64*3, 1);
}

#ifdef NOTUSED
STATIC int
not_used_tr3_read_reg(int unit, int addr, uint32 *d0_msb, uint32 *d1,
                  uint32 *d2_lsb)
{
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[2];
    int inst_index;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_RD_REG;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_RD_REG;

    SOC_IF_ERROR_RETURN(soc_triumph_tcam_access(unit, TCAM_TR_OP_READ,
                                                inst_index, 0, 9, dbus, ibus));
    *d2_lsb = dbus[0];
    *d1 = dbus[1];
    *d0_msb = dbus[2];
    return SOC_E_NONE;
}
#endif /* NOTUSED */
#endif /* BCM_TRIUMPH3_SUPPORT */

STATIC int
type1_tr_read_reg(int unit, int addr, uint32 *d0_msb, uint32 *d1,
                  uint32 *d2_lsb)
{
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[2];
    int inst_index;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_RD_REG;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_RD_REG;

    SOC_IF_ERROR_RETURN(soc_triumph_tcam_access(unit, TCAM_TR_OP_READ,
                                                inst_index, 0, 9, dbus, ibus));
    *d2_lsb = dbus[0];
    *d1 = dbus[1];
    *d0_msb = dbus[2];
    return SOC_E_NONE;
}

#ifdef BCM_TRIUMPH3_SUPPORT
/* on return d0_msb will have 16 bits */
STATIC int
tr3_read_reg(int unit, int addr, uint32 *d0_msb, uint32 *d1,
                  uint32 *d2_lsb)
{
    uint32 dbus[4];
    int ibus[2];
    int inst_index;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    /*
     * for raw accesses, address needs to be in little-endian format. So
     * make sure bytes are swapped
     */
    dbus[0] = 0;
    dbus[1] = MAKE_WORD(GET_BYTE(addr, 0), GET_BYTE(addr, 1), GET_BYTE(addr, 2),
                        (GET_BYTE(addr, 3) & 0x3));
    /* register v/s DB read is determined by bit 25 of addr */
    ibus[inst_index++] = TR3_OPCODE_RD_REG_DBX;

    /* currently supporting only one device read at a time */
#ifdef not_def
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_RD_REG;
#endif

#if 0
    LOG_CLI((BSL_META_U(unit,
                        "Read Addr=0x%08x\n"), addr));
#endif
    SOC_IF_ERROR_RETURN(soc_triumph3_tcam_access(unit, 
						TCAM_TR3_OP_REG_DB_X_READ, inst_index, 0, 0, 
						(uint32 *) dbus, ibus, 80, 1));

    /* result has bytes swapped. So make appropriate words out of the result */
#if 0
    *d2_lsb = MAKE_WORD(dbus[7], dbus[6], dbus[5], dbus[4]);
    *d1 = MAKE_WORD(dbus[3], dbus[2], dbus[1], dbus[0]);
    *d0_msb = (dbus[15] << 8) | dbus[14];

    *d2_lsb = (dbus[1] << 24) | ((dbus[1] << 8) & 0xff0000) | ((dbus[1] >> 8) 
               & 0xff00) | (dbus[1] >> 24);
    *d1 = (dbus[0] << 24) | ((dbus[0] << 8) & 0xff0000) | ((dbus[0] >> 8) 
               & 0xff00) | (dbus[0] >> 24);
    *d0_msb = (dbus[3] >> 24) | ((dbus[3] >> 8) & 0xff00);
#else
    *d2_lsb = MAKE_WORD(GET_BYTE(dbus[1], 0), GET_BYTE(dbus[1], 1), 
                        GET_BYTE(dbus[1], 2), GET_BYTE(dbus[1], 3));
    *d1 = MAKE_WORD(GET_BYTE(dbus[0], 0), GET_BYTE(dbus[0], 1), 
                        GET_BYTE(dbus[0], 2), GET_BYTE(dbus[0], 3));
    *d0_msb = MAKE_WORD(0, 0, GET_BYTE(dbus[3], 2), GET_BYTE(dbus[3], 3));
#endif
    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

int
soc_tr_tcam_type1_write_entry(int unit, int part, int index, uint32 *mask,
                              uint32 *data)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[4 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[4];
    int rv, addr, inst_index, len, offset, i;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (!mask || !data) {
        return SOC_E_PARAM;
    }

    if (part >= TCAM_PARTITION_COUNT ||
        index >= tcam_info->partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    addr = (index << tcam_info->partitions[part].tcam_width_shift) +
        tcam_info->partitions[part].tcam_base;
    len = 1 << tcam_info->partitions[part].tcam_width_shift;

    for (i = 0; i < len; i++) {
        sal_memset(dbus, 0, sizeof(dbus));
        inst_index = 0;
        offset = (len - 1 - i) * 4;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr + i;
        ibus[inst_index++] = TYPE1_OPCODE_WR_RECORD;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = data[offset + 3];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = data[offset + 2];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = data[offset + 1];
        ibus[inst_index++] = TYPE1_OPCODE_NOP;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = mask[offset + 3];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = mask[offset + 2];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = mask[offset + 1];
        ibus[inst_index++] = TYPE1_OPCODE_NOP;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = mask[offset + 3];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = mask[offset + 2];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = mask[offset + 1];
        ibus[inst_index++] = TYPE1_OPCODE_NOP;

        rv = soc_triumph_tcam_access(unit, TCAM_TR_OP_WRITE, inst_index, 0, 2,
                                     dbus, ibus);
        if (rv < 0) {
            return rv;
        }
    }

    return SOC_E_NONE;
}

#ifdef BCM_TRIUMPH3_SUPPORT
int
soc_tr3_tcam_write_entry(int unit, int part, int index, uint32 *mask,
                              uint32 *data, int valid_bit)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[4 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[4];
    int rv, addr, len, offset, i, tmp;
    int vbit = (valid_bit)?SET_VBIT:0;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (!mask || !data) {
        return SOC_E_PARAM;
    }

    if (part >= TCAM_PARTITION_COUNT ||
        index >= tcam_info->partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    addr = (index << tcam_info->partitions[part].tcam_width_shift) +
        tcam_info->partitions[part].tcam_base;
    len = 1 << tcam_info->partitions[part].tcam_width_shift;

    for (i = 0; i < len; i++) {
        sal_memset(dbus, 0, sizeof(dbus));
        offset = (len - 1 - i) * 4;

		/* set bit 25 of addr to 1 to indicate database write */
		/* might need to be smarter about vbit setting */

        /* normalize address for cascaded mode */
        tmp = (((addr+i) / (128 * 4096)) << 23) | ((addr+i) % (128 * 4096));
        dbus[1] = MAKE_WORD(GET_BYTE(tmp, 0), GET_BYTE(tmp, 1),
                            GET_BYTE(tmp, 2), 
                            /*SET_XY_WRITE |*/ vbit | (GET_BYTE(tmp, 3) & 0x3) | 0x2);

        ibus[0] = TR3_OPCODE_WR_REG_DB;

        dbus[0] = MAKE_WORD(GET_BYTE(data[offset+3], 0), 
                            GET_BYTE(data[offset+3], 1),
                            GET_BYTE(data[offset+3], 2),
                            GET_BYTE(data[offset+3], 3));

        dbus[3] = MAKE_WORD(GET_BYTE(data[offset+2], 0), 
                            GET_BYTE(data[offset+2], 1),
                            GET_BYTE(data[offset+2], 2),
                            GET_BYTE(data[offset+2], 3));

        dbus[2] = MAKE_WORD(GET_BYTE(data[offset+1], 0), 
                            GET_BYTE(data[offset+1], 1),
                            GET_BYTE(mask[offset+3], 0),
                            GET_BYTE(mask[offset+3], 1));
        dbus[5] = MAKE_WORD(GET_BYTE(mask[offset+3], 2), 
                            GET_BYTE(mask[offset+3], 3),
                            GET_BYTE(mask[offset+2], 0),
                            GET_BYTE(mask[offset+2], 1));
        dbus[4] = MAKE_WORD(GET_BYTE(mask[offset+2], 2), 
                            GET_BYTE(mask[offset+2], 3),
                            GET_BYTE(mask[offset+1], 0),
                            GET_BYTE(mask[offset+1], 1));
        rv = soc_triumph3_tcam_access(unit, TCAM_TR_OP_WRITE, 1, 0, 0,
                                     (uint32 *)dbus, ibus, 3*64, 1);
        if (rv < 0) {
            return rv;
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

int
soc_tr_tcam_type1_read_entry(int unit, int part, int index, uint32 *mask,
                             uint32 *data, int *valid)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[2];
    int rv, addr, inst_index, len, offset, i;
    uint32 d0_msb, d1, d2_lsb;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (!mask || !data || !valid) {
        return SOC_E_PARAM;
    }

    if (part >= TCAM_PARTITION_COUNT ||
        index >= tcam_info->partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    addr = (index << tcam_info->partitions[part].tcam_width_shift) +
        tcam_info->partitions[part].tcam_base;
    len = 1 << tcam_info->partitions[part].tcam_width_shift;

    *valid = 1;
    for (i = 0; i < len; i++) {
        sal_memset(dbus, 0, sizeof(dbus));
        inst_index = 0;
        offset = (len - 1 - i) * 4;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr + i;
        ibus[inst_index++] = TYPE1_OPCODE_RD_DATA;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr + i;
        ibus[inst_index++] = TYPE1_OPCODE_RD_DATA;

        rv = soc_triumph_tcam_access(unit, TCAM_TR_OP_READ, inst_index, 0, 9,
                                     dbus, ibus);
        if (rv < 0) {
            *valid = 0;
            return rv;
        }
        data[offset + 3] = dbus[0];
        data[offset + 2] = dbus[1];
        data[offset + 1] = dbus[2];
        data[offset] = 0;

        sal_memset(dbus, 0, sizeof(dbus));
        inst_index = 0;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr + i;
        ibus[inst_index++] = TYPE1_OPCODE_RD_MASK;

        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr + i;
        ibus[inst_index++] = TYPE1_OPCODE_RD_MASK;

        rv = soc_triumph_tcam_access(unit, TCAM_TR_OP_READ, inst_index, 0, 9,
                                     dbus, ibus);
        if (rv < 0) {
            *valid = 0;
            return rv;
        }
        mask[offset + 3] = dbus[0];
        mask[offset + 2] = dbus[1];
        mask[offset + 1] = dbus[2] & 0xff;
        mask[offset] = 0;

        rv = type1_tr_read_reg(unit, TYPE1_DBREG_RESULT_ADDR(0, 0),
                               &d0_msb, &d1, &d2_lsb);
        if (rv < 0) {
            *valid = 0;
            return rv;
        }
        if (d2_lsb & (1 << 22)) {
            *valid = 0;
        }
    }
    return SOC_E_NONE;
}

#ifdef BCM_TRIUMPH3_SUPPORT
/* upper bit of output is D, lower nibble of output is M */
uint8 xy_to_dm[] = {1, 0, 2, 3};

void _soc_convert_xy_to_dm(uint8 *data, uint8 *mask, uint8 *db_x, uint8 *db_y)
{
    int i;
    uint8 tmp, dm, idx0, idx1;
    for (i = 0; i < 80; i++) {
        idx0 = (db_x[i/8] >> (8 - (i % 8))) & 1;
        idx1 = (db_y[i/8] >> (8 - (i % 8))) & 1;
        dm = xy_to_dm[(idx0 << 1) | idx1];
        /* if data bit is 1 */
        tmp = (1 << (8 - (i%8)));
        if (dm & 0x2) {
            data[i/8] |= tmp;
        } else {
            data[i/8] &= ~tmp;
        }
        /* if mask bit is 1 */
        if (dm & 1 ) {
            mask[i/8] |= tmp;
        } else {
            mask[i/8] &= ~tmp;
        }
    }
}

int
soc_tr3_tcam_read_entry(int unit, int part, int index, uint32 *db_y,
                             uint32 *db_x, int *valid)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
#if 0
    uint32 db_x[2 * TCAM_TR_WORDS_PER_ENTRY];
    uint32 db_y[2 * TCAM_TR_WORDS_PER_ENTRY];
    uint8 *data_bp, *mask_bp, *dbx_bp, *dby_bp;
#endif
    int ibus[2];
    int rv, addr, len, offset, i, tmp;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (!db_x || !db_y || !valid) {
        return SOC_E_PARAM;
    }

    if (part >= TCAM_PARTITION_COUNT ||
        index >= tcam_info->partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    addr = (index << tcam_info->partitions[part].tcam_width_shift) +
        tcam_info->partitions[part].tcam_base;
    len = 1 << tcam_info->partitions[part].tcam_width_shift;

    /* we only do a max of 480 bits */
    if (len == 8) len = 6;

    *valid = 1;
    for (i = 0; i < len; i++) {
        sal_memset(dbus, 0, sizeof(dbus));
        offset = (len - 1 - i) * 4;

        /* normalize address for cascaded mode */
        tmp = (((addr+i) / (128 * 4096)) << 23) | ((addr+i) % (128 * 4096));

		/* set bit 25 of addr to 1 to indicate database read */
        dbus[1] = MAKE_WORD(GET_BYTE(tmp, 0), GET_BYTE(tmp, 1),
                            GET_BYTE(tmp, 2),
                            (GET_BYTE(tmp, 3) & 0x3) | 0x2);

        ibus[0] = TR3_OPCODE_RD_REG_DBX;

        rv = soc_triumph3_tcam_access(unit, TCAM_TR3_OP_REG_DB_X_READ, 
									  1, 0, 0, (uint32 *)dbus, ibus, 2*64, 1);
        if (rv < 0) {
            *valid = 0;
            return rv;
        }
		/* bits 31-0 */
        db_x[offset + 3] = MAKE_WORD(GET_BYTE(dbus[1], 0), GET_BYTE(dbus[1], 1),
                                    GET_BYTE(dbus[1], 2), GET_BYTE(dbus[1], 3));
		/* bits 63-32 */
        db_x[offset + 2] = MAKE_WORD(GET_BYTE(dbus[0], 0), GET_BYTE(dbus[0], 1),
                                    GET_BYTE(dbus[0], 2), GET_BYTE(dbus[0], 3));
		/* bits 79-64 */
        db_x[offset + 1] = MAKE_WORD(0, 0, GET_BYTE(dbus[3], 2), 
                                     GET_BYTE(dbus[3], 3));
        db_x[offset] = 0;

		*valid &= ((dbus[3] >> 12) & 1);

        sal_memset(dbus, 0, sizeof(dbus));

        dbus[1] = MAKE_WORD(GET_BYTE(tmp, 0), GET_BYTE(tmp, 1),
                            GET_BYTE(tmp, 2),
                            (GET_BYTE(tmp, 3) & 0x3) | 0x2);
        ibus[0] = TR3_OPCODE_RD_DBY;

        rv = soc_triumph3_tcam_access(unit, TCAM_TR3_OP_DB_Y_READ, 1, 0, 0,
                                     (uint32 *)dbus, ibus, 2*64, 1);
        if (rv < 0) {
            *valid = 0;
            return rv;
        }

		/* bits 31-0 */
        db_y[offset + 3] = MAKE_WORD(GET_BYTE(dbus[1], 0), GET_BYTE(dbus[1], 1),
                                    GET_BYTE(dbus[1], 2), GET_BYTE(dbus[1], 3));
		/* bits 63-32 */
        db_y[offset + 2] = MAKE_WORD(GET_BYTE(dbus[0], 0), GET_BYTE(dbus[0], 1),
                                    GET_BYTE(dbus[0], 2), GET_BYTE(dbus[0], 3));
		/* bits 79-64 */
        db_y[offset + 1] = MAKE_WORD(0, 0, GET_BYTE(dbus[3], 2), 
                                     GET_BYTE(dbus[3], 3));
        db_y[offset] = 0;

#if 0
        /* 4 words are input, we fill in 80 bits starting from least signficant
         * word, which is word 3 
         */
        sal_memset(data, 0, 6);
        sal_memset(mask, 0, 6);
        dbx_bp = (uint8 *) &db_x[0]; dbx_bp += 6;
        dby_bp = (uint8 *) &db_y[0]; dby_bp += 6;
        data_bp = (uint8 *) data; data_bp += 6;
        mask_bp = (uint8 *) mask; mask_bp += 6;
        _soc_convert_xy_to_dm(data_bp, mask_bp, dbx_bp, dby_bp);
#endif
    }
    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

int
soc_tr_tcam_type1_delete_entry(int unit, int part, int index)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[2 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[2];
    int addr, inst_index;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (part >= TCAM_PARTITION_COUNT ||
        index >= tcam_info->partitions[part].num_entries) {
        return SOC_E_PARAM;
    }

    addr = (index << tcam_info->partitions[part].tcam_width_shift) +
        tcam_info->partitions[part].tcam_base;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_DEL_RECORD;

    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = addr;
    ibus[inst_index++] = TYPE1_OPCODE_DEL_RECORD;

    return soc_triumph_tcam_access(unit, TCAM_TR_OP_WRITE, inst_index, 0, 3,
                                   dbus, ibus);
}

#ifdef BCM_TRIUMPH3_SUPPORT
/* Currently can search single partition. No parallel searches */
int
soc_tr3_tcam_search_entry(int unit, int part0, int part1, uint32 *data,
                               int *index0, int *index1)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[4 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[8], byte_index, op_type;
    int ltr, width_shift, len;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (!data || !index0 || !index1) {
        return SOC_E_PARAM;
    }

    switch (part0) {
        case TCAM_PARTITION_FWD_L2:
            ltr =TR3_LTR_CP_L2_ALL;
            break;
        case TCAM_PARTITION_FWD_IP4:
            ltr =TR3_LTR_CP_V4_ALL;
            break;
        case TCAM_PARTITION_FWD_IP6U:
            ltr =TR3_LTR_CP_V6_64_DEFIP;
            break;
        case TCAM_PARTITION_FWD_IP6:
            ltr =TR3_LTR_CP_V6_128_ALL;
            break;
        default: return SOC_E_PARAM;
    }

    width_shift = tcam_info->partitions[part0].tcam_width_shift;
    len = 1 << width_shift;

    sal_memset(dbus, 0, sizeof(dbus));
    byte_index=0;
    /* copy the incoming key 10bytes at a time, little-endian */
    if (len == 1) {
        dbus[0] = MAKE_WORD(GET_BYTE(data[1], 0),
                            GET_BYTE(data[1], 1),
                            GET_BYTE(data[1], 2),
                            GET_BYTE(data[1], 3));
        dbus[1] = MAKE_WORD(GET_BYTE(data[0], 0),
                            GET_BYTE(data[0], 1),
                            GET_BYTE(data[0], 2),
                            GET_BYTE(data[0], 3));
        byte_index = 10;
    } else if (len == 2) {
        dbus[0] = MAKE_WORD(GET_BYTE(data[4], 0), 
                            GET_BYTE(data[4], 1),
                            GET_BYTE(data[4], 2),
                            GET_BYTE(data[4], 3));
        dbus[1] = MAKE_WORD(GET_BYTE(data[0], 0), 
                            GET_BYTE(data[0], 1),
                            GET_BYTE(data[0], 2),
                            GET_BYTE(data[0], 3));
        dbus[2] = MAKE_WORD(GET_BYTE(data[1], 0), 
                            GET_BYTE(data[1], 1),
                            GET_BYTE(data[1], 2),
                            GET_BYTE(data[1], 3));
        dbus[3] = MAKE_WORD(GET_BYTE(data[2], 0), 
                            GET_BYTE(data[2], 1),
                            GET_BYTE(data[2], 2),
                            GET_BYTE(data[2], 3));
        dbus[5] = MAKE_WORD(GET_BYTE(data[3], 0),
                            GET_BYTE(data[3], 1),
                            GET_BYTE(data[3], 2),
                            GET_BYTE(data[3], 3));
        byte_index = 20;
    }
    if (len > 2) {
        return SOC_E_PARAM;
    }
    if (byte_index < 40) {
        ibus[0] = TR3_OPCODE_CB_WR_CMP1(ltr);
        op_type = TCAM_TR3_OP_CB_WR_CMP1;
    } else {
        /* coverity[dead_error_begin] */
        ibus[0] = TR3_OPCODE_CB_WR_CMP2(ltr);
        op_type = TCAM_TR3_OP_CB_WR_CMP2;
    }
    SOC_IF_ERROR_RETURN(soc_triumph3_tcam_access(unit, op_type, 1, 0, 0, 
                                  (uint32 *)dbus, ibus, len*80, 1));
    /* for single search, dbus[1] bits 23-0 have result */

    *index0 = *(uint32 *)&dbus[1];
    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * part0 specifies the table to be searched. If part1 is also specified then
 * it would be a parallel search
 */
int
soc_tr_tcam_type1_search_entry(int unit, int part0, int part1, uint32 *data,
                               int *index0, int *index1)
{
    soc_tcam_info_t *tcam_info;
    uint32 dbus[8 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[8];
    int op_type, inst_index, ltr, width_shift, len, offset, i;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    if (!data || !index0 || !index1) {
        return SOC_E_PARAM;
    }

    switch (part0) {
    case TCAM_PARTITION_RAW:
        switch (part1) {
        case TCAM_PARTITION_ACL_IP4:
            ltr = TYPE1_TR_LTBL_ACL_IP4_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_IP6S:
            ltr = TYPE1_TR_LTBL_ACL_IP6S_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_IP6F:
            ltr = TYPE1_TR_LTBL_ACL_IP6F_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_L2C:
            ltr = TYPE1_TR_LTBL_ACL_L2C_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_IP4C:
            ltr = TYPE1_TR_LTBL_ACL_IP4C_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_IP6C:
            ltr = TYPE1_TR_LTBL_ACL_IP6C_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_L2IP4:
            ltr = TYPE1_TR_LTBL_ACL_L2IP4_FWD_NONE;
            break;
        case TCAM_PARTITION_ACL_L2IP6:
            ltr = TYPE1_TR_LTBL_ACL_L2IP6_FWD_NONE;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_L2:
        switch (part1) {
        case TCAM_PARTITION_RAW:
            /* need to find a way to do TYPE1_TR_LTBL_ACL_NONE_FWD_L2_K2 */
            ltr = TYPE1_TR_LTBL_ACL_NONE_FWD_L2;
            break;
        case TCAM_PARTITION_FWD_IP4:
            if (!tcam_info->mode) { /* not for 500 Mhz */
                return SOC_E_PARAM;
            }
            ltr = TYPE1_TR_LTBL_FWD_IP4_FWD_L2;
            break;
        case TCAM_PARTITION_FWD_IP6U:
            if (!tcam_info->mode) { /* not for 500 Mhz */
                return SOC_E_PARAM;
            }
            ltr = TYPE1_TR_LTBL_FWD_IP6U_FWD_L2;
            break;
        case TCAM_PARTITION_ACL_L2:
            ltr = TYPE1_TR_LTBL_ACL_L2_FWD_L2;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP4:
        switch (part1) {
        case TCAM_PARTITION_RAW:
            /* need to find a way to do TYPE1_TR_LTBL_ACL_NONE_FWD_IP4_K2 */
            ltr = TYPE1_TR_LTBL_ACL_NONE_FWD_IP4;
            break;
        case TCAM_PARTITION_ACL_IP4:
            ltr = TYPE1_TR_LTBL_ACL_IP4_FWD_IP4;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP6U:
        switch (part1) {
        case TCAM_PARTITION_RAW:
            /* need to find a way to do TYPE1_TR_LTBL_ACL_NONE_FWD_IP6U_K4 */
            ltr = TYPE1_TR_LTBL_ACL_NONE_FWD_IP6U;
            break;
        case TCAM_PARTITION_ACL_IP6S:
            ltr = TYPE1_TR_LTBL_ACL_IP6S_FWD_IP6U;
            break;
        case TCAM_PARTITION_ACL_IP6F:
            ltr = TYPE1_TR_LTBL_ACL_IP6F_FWD_IP6U;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_FWD_IP6:
        switch (part1) {
        case TCAM_PARTITION_RAW:
            /* need to find a way to do TYPE1_TR_LTBL_ACL_NONE_FWD_IP6_K4 */
            ltr = TYPE1_TR_LTBL_ACL_NONE_FWD_IP6;
            break;
        case TCAM_PARTITION_ACL_IP6F:
            ltr = TYPE1_TR_LTBL_ACL_IP6F_FWD_IP6;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_DEV0_TBL72:
        switch (part1) {
        case TCAM_PARTITION_DEV0_TBL144:
            ltr = TYPE1_TR_LTBL_DEV0_TBL144_DEV0_TBL72;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    case TCAM_PARTITION_DEV1_TBL72:
        switch (part1) {
        case TCAM_PARTITION_DEV1_TBL144:
            ltr = TYPE1_TR_LTBL_DEV1_TBL144_DEV1_TBL72;
            break;
        default:
            return SOC_E_PARAM;
        }
        break;
    default:
        return SOC_E_PARAM;
    }

    if (part0 == TCAM_PARTITION_RAW) {
        op_type = TCAM_TR_OP_SINGLE_SEARCH1;
    } else  if (part1 == TCAM_PARTITION_RAW) {
        op_type = TCAM_TR_OP_SINGLE_SEARCH0;
    } else {
        op_type = TCAM_TR_OP_PARALLEL_SEARCH;
    }
    width_shift = tcam_info->partitions[part0].tcam_width_shift >
        tcam_info->partitions[part1].tcam_width_shift ?
        tcam_info->partitions[part0].tcam_width_shift :
        tcam_info->partitions[part1].tcam_width_shift;
    len = 1 << width_shift;

    sal_memset(dbus, 0, sizeof(dbus));
    inst_index = 0;
    for (i = 1; i < len; i++) {
        offset = (len - 1 - i) * 4;
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = data[offset + 3];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = data[offset + 2];
        dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = data[offset + 1];
        ibus[inst_index++] = TYPE1_OPCODE_CMP_WORD(i);
    }

    offset = (len - 1) * 4;
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY] = data[offset + 3];
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 1] = data[offset + 2];
    dbus[inst_index * TCAM_TR_WORDS_PER_ENTRY + 2] = data[offset + 1];
    ibus[inst_index++] = TYPE1_OPCODE_SEARCH(ltr);

    SOC_IF_ERROR_RETURN(soc_triumph_tcam_access(unit, op_type, inst_index,
                                                0, 0, dbus, ibus));
    *index0 = dbus[0] & 0xfffff;
    *index1 = dbus[1] & 0xfffff;
    return SOC_E_NONE;
}

int
soc_tr_tcam_type1_memtest_dpeo(int unit, int num_entries, uint32 oe_map,
                               uint32 *data)
{
    uint32 dbus[8 * TCAM_TR_WORDS_PER_ENTRY];
    int ibus[8];
    int offset, i;

    for (i = 0; i < num_entries; i++) {
        offset = i * 4;
        dbus[i * TCAM_TR_WORDS_PER_ENTRY] = data[offset];
        dbus[i * TCAM_TR_WORDS_PER_ENTRY + 1] = data[offset + 1];
        dbus[i * TCAM_TR_WORDS_PER_ENTRY + 2] = data[offset + 2];
        if (oe_map & (1 << i)) {
            ibus[i] = TYPE1_OPCODE_CMP_WORD(i);
        } else {
            ibus[i] = 0x100;
        }
    }

    return soc_triumph_tcam_access(unit, TCAM_TR_OP_WRITE, num_entries, 0, 0,
                                   dbus, ibus);
}

int
soc_tr_tcam_type1_write_reg(int unit, uint32 addr, uint32 d0_msb, uint32 d1,
                            uint32 d2_lsb)
{
    if (SOC_CONTROL(unit)->tcam_info == NULL) {
        return SOC_E_INIT;
    }

    return type1_tr_write_reg(unit, addr, d0_msb, d1, d2_lsb);
}

#ifdef BCM_TRIUMPH3_SUPPORT
int
soc_tr3_tcam_write_reg(int unit, uint32 addr, uint32 d0_msb, uint32 d1,
                        uint32 d2_lsb)
{
#if 0
    if (SOC_CONTROL(unit)->tcam_info == NULL) {
        return SOC_E_INIT;
    }
#endif
    return tr3_write_reg(unit, addr, d0_msb, d1, d2_lsb);
}
#endif /* BCM_TRIUMPH3_SUPPORT */

int
soc_tr_tcam_type1_read_reg(int unit, uint32 addr, uint32 *d0_msb, uint32 *d1,
                           uint32 *d2_lsb)
{
    if (SOC_CONTROL(unit)->tcam_info == NULL) {
        return SOC_E_INIT;
    }

    return type1_tr_read_reg(unit, addr, d0_msb, d1, d2_lsb);
}

#ifdef BCM_TRIUMPH3_SUPPORT
int
soc_tr3_tcam_read_reg(int unit, uint32 addr, uint32 *d0_msb, uint32 *d1,
                      uint32 *d2_lsb)
{
#if 0
    if (SOC_CONTROL(unit)->tcam_info == NULL) {
        return SOC_E_INIT;
    }
#endif

    return tr3_read_reg(unit, addr, d0_msb, d1, d2_lsb);
}
#endif /* BCM_TRIUMPH3_SUPPORT */

int
tr_tcam_init_type1(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int rv, part, tcam_base, entries_per_blk, blks_per_dev, i;
    int dev, blk, blk_base, num_blks, mask_len, valid_mask_len;
    int tcam_base0, tcam_base1;
    uint32 addr, d0, d1, d2, ltid, width;
    int require_bmr_workaround = FALSE;
    int maj_rev, min_rev;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }
    partitions = tcam_info->partitions;

    entries_per_blk = TYPE1_ST3_ENTRIES_PER_BLK;
    blks_per_dev = TYPE1_ST3_BLKS_PER_DEV;

    /* 1. enable next device only on the first time after tcam reset */
    if (!tcam_info->num_tcams) {
        SOC_IF_ERROR_RETURN(type1_tr_enable_next_dev(unit));
    }

    /* read device model number and size */
    tcam_info->monolith_dev = FALSE;
    rv = type1_tr_read_reg(unit, TYPE1_DBREG_DBCFG_ADDR(0), &d0, &d1, &d2);
    if (SOC_SUCCESS(rv)) {
        maj_rev = (d2 >> TYPE1_ST3_DBREG_DBCFG_LSB_MAJ_REV_oft) &
            TYPE1_ST3_DBREG_DBCFG_LSB_MAJ_REV_mask;
        min_rev = (d2 >> TYPE1_ST3_DBREG_DBCFG_LSB_MIN_REV_oft) &
            TYPE1_ST3_DBREG_DBCFG_LSB_MIN_REV_mask;
        if (tcam_info->subtype_override == 0) {
            switch ((d1 >> TYPE1_ST3_DBREG_DBCFG_MSB_FAMILY_oft) &
                    TYPE1_ST3_DBREG_DBCFG_MSB_FAMILY_mask) {
            case 1:
            default:
                tcam_info->subtype = 3;
                break;
            case 2:
                tcam_info->subtype = 4;
                break;
            }
        } else {
            tcam_info->subtype = tcam_info->subtype_override;
        }
        tcam_info->num_tcams = 1;
        switch ((((d1 >> TYPE1_ST3_DBREG_DBCFG_MSB_SIZE_oft) &
                  TYPE1_ST3_DBREG_DBCFG_MSB_SIZE_mask) << 8) |
                ((d2 >> TYPE1_ST3_DBREG_DBCFG_LSB_SIZE_oft) &
                 TYPE1_ST3_DBREG_DBCFG_LSB_SIZE_mask)) {
        case 0x0100: /* 512k 36-bit records (256k 72-bit entries, 18M) */
        default:
            if (tcam_info->subtype == 4 && maj_rev == 1) {
                /* monolith 1024k 36-bit records (512k 72-bit entries, 36M) */
                partitions[TCAM_PARTITION_RAW].num_entries = 512 * 1024;
                tcam_info->num_tcams = 2;
                tcam_info->monolith_dev = TRUE;
            } else if (tcam_info->num_tcams_override == 2) {
                /* mcm 1024k 36-bit records (512k 72-bit entries, 36M) */
                partitions[TCAM_PARTITION_RAW].num_entries = 512 * 1024;
                tcam_info->num_tcams = 2;
            } else {
                partitions[TCAM_PARTITION_RAW].num_entries = 256 * 1024;
            }
            break;
        case 0x0080: /* 256k 36-bit records (128k 72-bit entries, 9M) */
            partitions[TCAM_PARTITION_RAW].num_entries = 128 * 1024;
            break;
        case 0x0010: /* 128k 36-bit records (64k 72-bit entries, 4.5M) */
            partitions[TCAM_PARTITION_RAW].num_entries = 64 * 1024;
            break;
        }
        if (tcam_info->subtype == 4 && maj_rev == 2 && min_rev == 1 &&
            (partitions[TCAM_PARTITION_RAW].num_entries == 128 * 1024 ||
             partitions[TCAM_PARTITION_RAW].num_entries == 256 * 1024)) {
            require_bmr_workaround = TRUE;
        }
    } else {
        if (tcam_info->subtype_override != 0) {
            tcam_info->subtype = tcam_info->subtype_override;
        } else {
            tcam_info->subtype = 3;
        }
        if (tcam_info->num_tcams_override != 0) {
            tcam_info->num_tcams = tcam_info->num_tcams_override;
        } else {
            tcam_info->num_tcams = 1;
        }
        partitions[TCAM_PARTITION_RAW].num_entries =
            entries_per_blk * blks_per_dev * tcam_info->num_tcams;
    }

    partitions[TCAM_PARTITION_RAW].tcam_width_shift = 0;
    if (tcam_info->num_tcams == 1 || tcam_info->monolith_dev) {
        partitions[TCAM_PARTITION_DEV1_TBL72].num_entries = 0;
        partitions[TCAM_PARTITION_DEV1_TBL144].num_entries = 0;
    }

    /* 2. device id register and device configuration register */
    for (i = 0; i < tcam_info->num_tcams; i++) {
        addr = TYPE1_DBREG_DEVID_ADDR(i);
        d2 = MAKE_FIELD(TYPE1_DBREG_DEVID_ID, i);
        SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

        if (tcam_info->subtype == 4) {
            addr = TYPE1_DBREG_DBCFG_ADDR(i);
            d0 = MAKE_FIELD(TYPE1_ST3_DBREG_DBCFG_TOP8_CMP_ANY, 1) |
                 MAKE_FIELD(TYPE1_ST3_DBREG_DBCFG_TOP8_MAP_KEY, 1);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_DBCFG_LSB_FG_PS, 1);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, d0, 0, d2));
        }
    }

    tcam_base = 0;
    tcam_base0 = 0;
    tcam_base1 = 0;
    for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT; part++) {
        if (!partitions[part].num_entries) {
            continue;
        }
        switch (part) {
        case TCAM_PARTITION_FWD_L2:
        case TCAM_PARTITION_FWD_IP4:
        case TCAM_PARTITION_FWD_IP6U:
        case TCAM_PARTITION_DEV0_TBL72:
        case TCAM_PARTITION_DEV1_TBL72:
            partitions[part].tcam_width_shift = 0;
            break;
        case TCAM_PARTITION_FWD_IP6:
        case TCAM_PARTITION_ACL_L2C:
        case TCAM_PARTITION_ACL_IP4C:
        case TCAM_PARTITION_ACL_IP6C:
        case TCAM_PARTITION_DEV0_TBL144:
        case TCAM_PARTITION_DEV1_TBL144:
            partitions[part].tcam_width_shift = 1;
            break;
        case TCAM_PARTITION_ACL_L2:
        case TCAM_PARTITION_ACL_IP4:
            partitions[part].tcam_width_shift = 2;
            break;
        case TCAM_PARTITION_ACL_IP6S:
        case TCAM_PARTITION_ACL_IP6F:
        case TCAM_PARTITION_ACL_L2IP4:
        case TCAM_PARTITION_ACL_L2IP6:
            partitions[part].tcam_width_shift = 3;
            break;
        default:
            break;
        }

        num_blks = ((partitions[part].num_entries <<
                     partitions[part].tcam_width_shift) +
                    entries_per_blk - 1) / entries_per_blk;
        partitions[part].num_entries_include_pad =
            (num_blks * entries_per_blk) >> partitions[part].tcam_width_shift;
        if (part == TCAM_PARTITION_DEV0_TBL72 ||
            part == TCAM_PARTITION_DEV0_TBL144) {
            partitions[part].tcam_base = tcam_base0;
            tcam_base0 += num_blks * entries_per_blk;
        } else if (part == TCAM_PARTITION_DEV1_TBL72 ||
            part == TCAM_PARTITION_DEV1_TBL144) {
            partitions[part].tcam_base = tcam_base1 +
                partitions[TCAM_PARTITION_RAW].num_entries / 2;
            tcam_base1 += num_blks * entries_per_blk;
        } else {
            partitions[part].tcam_base = tcam_base;
            tcam_base += num_blks * entries_per_blk;
        }
    }
    if (tcam_base > partitions[TCAM_PARTITION_RAW].num_entries) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "tr_tcam_init_type1: unit %d number of entries "
                              "configured exceeds physical device size\n"),
                   unit));
        return SOC_E_PARAM;
    }
    if (tcam_base && tcam_base0 && tcam_base1) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "tr_tcam_init_type1: unit %d has both normal "
                              "and test entries in config\n"),
                   unit));
        return SOC_E_PARAM;
    }

    /*
     * 3. program block configuration registers for all devices
     */
    blk = 0;
    for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT; part++) {
        if (!partitions[part].num_entries) {
            continue;
        }

        switch (part) {
        case TCAM_PARTITION_FWD_L2: ltid = TYPE1_TR_LTID_FWD_L2; break;
        case TCAM_PARTITION_FWD_IP4: ltid = TYPE1_TR_LTID_FWD_IP4; break;
        case TCAM_PARTITION_FWD_IP6U: ltid = TYPE1_TR_LTID_FWD_IP6U; break;
        case TCAM_PARTITION_FWD_IP6: ltid = TYPE1_TR_LTID_FWD_IP6; break;
        case TCAM_PARTITION_ACL_L2: ltid = TYPE1_TR_LTID_ACL_L2; break;
        case TCAM_PARTITION_ACL_IP4: ltid = TYPE1_TR_LTID_ACL_IP4; break;
        case TCAM_PARTITION_ACL_IP6S: ltid = TYPE1_TR_LTID_ACL_IP6S; break;
        case TCAM_PARTITION_ACL_IP6F: ltid = TYPE1_TR_LTID_ACL_IP6F; break;
        case TCAM_PARTITION_ACL_L2C: ltid = TYPE1_TR_LTID_ACL_L2C; break;
        case TCAM_PARTITION_ACL_IP4C: ltid = TYPE1_TR_LTID_ACL_IP4C; break;
        case TCAM_PARTITION_ACL_IP6C: ltid = TYPE1_TR_LTID_ACL_IP6C; break;
        case TCAM_PARTITION_ACL_L2IP4: ltid = TYPE1_TR_LTID_ACL_L2IP4; break;
        case TCAM_PARTITION_ACL_L2IP6: ltid = TYPE1_TR_LTID_ACL_L2IP6; break;
        case TCAM_PARTITION_DEV0_TBL72: ltid = TYPE1_TR_LTID_DEV0_TBL72; break;
        case TCAM_PARTITION_DEV0_TBL144: ltid = TYPE1_TR_LTID_DEV0_TBL144;
            break;
        case TCAM_PARTITION_DEV1_TBL72: ltid = TYPE1_TR_LTID_DEV1_TBL72; break;
        case TCAM_PARTITION_DEV1_TBL144: ltid = TYPE1_TR_LTID_DEV1_TBL144;
            break;
        default: continue;
        }

        switch (partitions[part].tcam_width_shift) {
        case 0: width = TYPE1_DBREG_BCFG_WIDTH_72; break;
        case 1: width = TYPE1_DBREG_BCFG_WIDTH_144; break;
        case 2: width = TYPE1_DBREG_BCFG_WIDTH_288; break;
        case 3: width = TYPE1_DBREG_BCFG_WIDTH_576; break;
        default: continue;
        }

        num_blks = ((partitions[part].num_entries <<
                     partitions[part].tcam_width_shift) +
                    entries_per_blk - 1) / entries_per_blk;
        blk_base = partitions[part].tcam_base /entries_per_blk;
        for (blk = blk_base; blk < blk_base + num_blks; blk++) {
            addr = TYPE1_DBREG_BCFG_ADDR(blk / blks_per_dev,
                                         blk % blks_per_dev);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_BCFG_LPWR, 0) |
                MAKE_FIELD(TYPE1_ST3_DBREG_BCFG_LTID, ltid) |
                MAKE_FIELD(TYPE1_DBREG_BCFG_WIDTH, width);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));
        }
    }
    /*
     * Fill the unused block configuration registers with zero.
     * Since blocks are arranged in continuously ascending order, all free
     * blocks are at the bottom */
    for (; blk < tcam_info->num_tcams * blks_per_dev; blk++) {
        addr = TYPE1_DBREG_BCFG_ADDR(blk / blks_per_dev, blk % blks_per_dev);
        SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, 0));
    }

    /*
     * 4. program block configuration registers and logical table registers
     * of each device
     */
    for (dev = 0; dev < tcam_info->num_tcams; dev++) {
        /*
         * 4-1. block mask registers
         */
        blk = 0;
        for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT;
             part++) {
            if (!partitions[part].num_entries) {
                continue;
            }

            mask_len = 1 << partitions[part].tcam_width_shift;
            if (part == TCAM_PARTITION_ACL_IP6S) {
                valid_mask_len = 5; /* use only 72 * 5 = 360 bits */
            } else if (part == TCAM_PARTITION_ACL_IP6F ||
                       part == TCAM_PARTITION_ACL_L2IP4 ||
                       part == TCAM_PARTITION_ACL_L2IP6) {
                valid_mask_len = 6; /* use only 72 * 6 = 432 bits */
            } else {
                valid_mask_len = mask_len;
            }

            num_blks = ((partitions[part].num_entries <<
                         partitions[part].tcam_width_shift) +
                        entries_per_blk - 1) / entries_per_blk;
            blk_base = partitions[part].tcam_base /entries_per_blk;
            for (blk = blk_base; blk < blk_base + num_blks; blk++) {
                if (blk /blks_per_dev != dev) {
                    continue;
                }
                addr = TYPE1_DBREG_BMASK_ADDR(dev, blk % blks_per_dev,
                                              TYPE1_TR_BMASK);
                /* useful portion */
                for (i = 0; i < valid_mask_len; i++) {
                    if (require_bmr_workaround) {
                        SOC_IF_ERROR_RETURN(type1_tr_write_bmr(unit, addr + i,
                                                               0, 0, 0));
                    } else {
                        SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr + i,
                                                               0, 0, 0));
                    }
                }
                /* unused portion */
                for (; i < mask_len; i++) {
                    if (require_bmr_workaround) {
                        SOC_IF_ERROR_RETURN(type1_tr_write_bmr(unit, addr + i,
                                                               0xff,
                                                               0xffffffff,
                                                               0xffffffff));
                    } else {
                        SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr + i,
                                                               0xff,
                                                               0xffffffff,
                                                               0xffffffff));
                    }
                }
            }
        }

        /*
         * 4-2. logical table registers
         */
        if (tcam_base) {
            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_NONE_FWD_L2);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_FWD_L2) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_UNUSED);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_NONE_FWD_IP4);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_FWD_IP4) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_UNUSED);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_NONE_FWD_IP6U);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                           TYPE1_TR_LTID_FWD_IP6U) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_UNUSED);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_NONE_FWD_IP6);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_FWD_IP6) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_UNUSED);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_L2_FWD_L2);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_FWD_L2) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_L2);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP4_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP4);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP4_FWD_IP4);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_FWD_IP4) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP4);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP6S_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP6S);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP6S_FWD_IP6U);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                           TYPE1_TR_LTID_FWD_IP6U) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP6S);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP6F_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP6F);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP6F_FWD_IP6U);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                           TYPE1_TR_LTID_FWD_IP6U) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP6F);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP6F_FWD_IP6);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_FWD_IP6) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP6F);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_L2C_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_L2C);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP4C_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP4C);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR(dev, TYPE1_TR_LTBL_ACL_IP6C_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1, TYPE1_TR_LTID_ACL_IP6C);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR
                (dev, TYPE1_TR_LTBL_ACL_L2IP4_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                           TYPE1_TR_LTID_ACL_L2IP4);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR
                (dev, TYPE1_TR_LTBL_ACL_L2IP6_FWD_NONE);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0, TYPE1_TR_LTID_UNUSED) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                           TYPE1_TR_LTID_ACL_L2IP6);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            if (tcam_info->subtype == 4) {
                if (tcam_info->mode) { /* 4 cycles per packet - 350 Mhz */
                    addr = TYPE1_DBREG_LTBL_ADDR
                        (dev, TYPE1_TR_LTBL_ACL_NONE_FWD_L2_K2);
                    d1 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_KEY, 2);
                    d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0,
                                    TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                                   TYPE1_TR_LTID_FWD_L2) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1,
                                   TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                                   TYPE1_TR_LTID_UNUSED);
                    SOC_IF_ERROR_RETURN
                        (type1_tr_write_reg(unit, addr, 0, d1, d2));

                    addr = TYPE1_DBREG_LTBL_ADDR
                        (dev, TYPE1_TR_LTBL_ACL_NONE_FWD_IP4_K2);
                    d1 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_KEY, 2);
                    d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0,
                                    TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                                   TYPE1_TR_LTID_FWD_IP4) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1,
                                   TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                                   TYPE1_TR_LTID_UNUSED);
                    SOC_IF_ERROR_RETURN
                        (type1_tr_write_reg(unit, addr, 0, d1, d2));

                    addr = TYPE1_DBREG_LTBL_ADDR
                        (dev, TYPE1_TR_LTBL_FWD_IP4_FWD_L2);
                    d1 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_PCOMP, 1);
                    d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0,
                                    TYPE1_TR_BMASK) |
                     MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                                TYPE1_TR_LTID_FWD_L2) |
                     MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                     MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                                TYPE1_TR_LTID_FWD_IP4);
                    SOC_IF_ERROR_RETURN
                        (type1_tr_write_reg(unit, addr, 0, d1, d2));

                    addr = TYPE1_DBREG_LTBL_ADDR
                        (dev, TYPE1_TR_LTBL_FWD_IP6U_FWD_L2);
                    d1 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_PCOMP, 1);
                    d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0,
                                    TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                                   TYPE1_TR_LTID_FWD_L2) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1,
                                   TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                                   TYPE1_TR_LTID_FWD_IP6U);
                    SOC_IF_ERROR_RETURN
                        (type1_tr_write_reg(unit, addr, 0, d1, d2));
                } else { /* 6 cycles per packet - 500 Mhz */
                    addr = TYPE1_DBREG_LTBL_ADDR
                        (dev, TYPE1_TR_LTBL_ACL_NONE_FWD_IP6U_K4);
                    d1 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_KEY, 4);
                    d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0,
                                    TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                                   TYPE1_TR_LTID_FWD_IP6U) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1,
                                   TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                                   TYPE1_TR_LTID_UNUSED);
                    SOC_IF_ERROR_RETURN
                        (type1_tr_write_reg(unit, addr, 0, d1, d2));

                    addr = TYPE1_DBREG_LTBL_ADDR
                        (dev, TYPE1_TR_LTBL_ACL_NONE_FWD_IP6_K4);
                    d1 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_KEY, 4);
                    d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0,
                                    TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                                   TYPE1_TR_LTID_FWD_IP6) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1,
                                   TYPE1_TR_BMASK) |
                        MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                                   TYPE1_TR_LTID_UNUSED);
                    SOC_IF_ERROR_RETURN
                        (type1_tr_write_reg(unit, addr, 0, d1, d2));
                }
            }
        } else {
            addr = TYPE1_DBREG_LTBL_ADDR
                (dev, TYPE1_TR_LTBL_DEV0_TBL144_DEV0_TBL72);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                           TYPE1_TR_LTID_DEV0_TBL72) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                           TYPE1_TR_LTID_DEV0_TBL144);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));

            addr = TYPE1_DBREG_LTBL_ADDR
                (dev, TYPE1_TR_LTBL_DEV1_TBL144_DEV1_TBL72);
            d2 = MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK0, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID0,
                           TYPE1_TR_LTID_DEV1_TBL72) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_BMASK1, TYPE1_TR_BMASK) |
                MAKE_FIELD(TYPE1_ST3_DBREG_LTBL_LTID1,
                           TYPE1_TR_LTID_DEV1_TBL144);
            SOC_IF_ERROR_RETURN(type1_tr_write_reg(unit, addr, 0, 0, d2));
        }
    }

    return SOC_E_NONE;
}

#ifdef BCM_TRIUMPH3_SUPPORT

uint32 tr3_ltr_free_bitmap[SOC_MAX_NUM_DEVICES][2];

int
_soc_tr3_ltr_free(int unit, int ltr)
{ 
    tr3_ltr_free_bitmap[unit][ltr/32] |= (1 << ltr % 32);
    return SOC_E_NONE;
}

int
_soc_tr3_ltr_alloc(int unit)
{
    int i;
    for (i =0; i < 61; i++) {
        if (tr3_ltr_free_bitmap[unit][i/32] & (1 << i % 32)) {
            tr3_ltr_free_bitmap[unit][i/32] &= ~(1 << i % 32);
            return i;
        }
    }
    return SOC_E_MEMORY;
}

/* mapping of which key segment goes into which key construction reg word */
uint8 kcr_word_map[10] = {0, 0, 0, 1, 1, 2, 2, 2, 3, 3};

/* mapping of shift modulo per key segment */
/* key construction registers are 55bit in nl11k */
uint8 kcr_shift_map[10] = {0, 0, 0, 32, 32, 55, 55, 55, 87, 87};

int _tr3_build_ltr(int unit, int dev, ltr_info_t *ltr_info, 
                      int entries_per_blk, int ltr, uint32 raw_bsr) 
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int srch, i, ii, jj;
    uint32 addr, d1, d2, bsr[4*NUM_TR3_TCAMS], sblk_kpu_select[2*NUM_TR3_TCAMS];
    uint32 misc1, misc2=0;
    /* 2 words per 32 blocks: 0,1 for 0-31, 1,2 for 32-63, etc */
    uint32 parallel_srch[8*NUM_TR3_TCAMS];
    uint32 kcr[4][4];

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    /* if needed allocate an LTR */
    if (ltr < 0) {
        /* allocate LTR */
        ltr = _soc_tr3_ltr_alloc(unit);
        if (ltr < 0) {
            return SOC_E_MEMORY;
        }
    }
    partitions = tcam_info->partitions;

    sal_memset(bsr, 0, sizeof(bsr));
    sal_memset(sblk_kpu_select, 0, sizeof(sblk_kpu_select));
    sal_memset(parallel_srch, 0, sizeof(parallel_srch));
    d2 = 0;
    for (ii = 0; ii < ltr_info->num_searches; ii++) {
        for (jj = 0; jj < NUM_PARTITIONS_PER_SEARCH; jj++) {
            if (!ltr_info->partition[ii][jj]) continue;
	        MAKE_BSR_WORDS(bsr, ltr_info->partition[ii][jj], entries_per_blk);
            MAKE_SUPER_BLOCK_KPU_SELECT(sblk_kpu_select, 
                                 ltr_info->partition[ii][jj],
                                 entries_per_blk, ltr_info->sblk_kpu_select[ii]);
            MAKE_PARALLEL_SEARCH_REGISTER(parallel_srch, 
                                         ltr_info->partition[ii][jj],
                                         entries_per_blk, 
                                         ltr_info->parallel_srch[ii]);
        }
        misc2 |= (MAKE_FIELD(TR3_DBREG_BMR_SELECT0, ltr_info->bmr_select[ii]) 
                 << (ii * 4));
    }

    if (raw_bsr) {
        /* For testing. Override partition blocks */
        bsr[0] = raw_bsr;
    }
    SET_BLOCK_SELECT_REG(unit, dev, ltr, bsr[dev*4 + 3], 
                         bsr[dev*4 + 2], 
                         bsr[dev*4 + 1], 
                         bsr[dev*4 + 0]); 
            
    addr = TR3_DBREG_SBLK_KPU_SEL_ADDR(dev, ltr);
    SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, sblk_kpu_select[dev*2 + 1],
                        sblk_kpu_select[dev * 2 + 0]));

    WRITE_PARALLEL_SEARCH_REGISTERS(unit, dev, ltr, 
                            parallel_srch[dev*8 + 1], parallel_srch[dev*8 + 0],
                            parallel_srch[dev*8 + 3], parallel_srch[dev*8 + 2],
                            parallel_srch[dev*8 + 5], parallel_srch[dev*8 + 4],
                            parallel_srch[dev*8 + 7], parallel_srch[dev*8 + 6]);

    /* misc register: */
    misc1 = MAKE_FIELD(TR3_DBREG_NUM_SEARCH_RESULTS, ltr_info->num_searches);
    addr = TR3_DBREG_LTR_MISC_REG_ADDR(dev, ltr);
    SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, misc1, misc2));

    sal_memset(&kcr, 0, sizeof(kcr));
    for (srch = 0; srch < ltr_info->num_searches; srch++) {
        for (i = 0; i < 10; i++) {
#if 1
            kcr[srch][kcr_word_map[i]] |= ((MAKE_FIELD(TR3_DBREG_KCR_NUM_BYTE0, 
                                         ltr_info->key_info[srch].seg_len[i]) |
                        MAKE_FIELD(TR3_DBREG_KCR_START_BYTE0, 
                            ltr_info->key_info[srch].seg_offset[i])) << 
                            (11*i - kcr_shift_map[i]));
#else
            ii = ((ltr_info->key_info[srch].seg_len[i] & 0xf) << 7) |
                  (ltr_info->key_info[srch].seg_offset[i] & 0x7f);
            kcr[srch][kcr_word_map[i]] |= (ii << ((11 * i) - 32 * (11*i)/32));
#endif
        }
#if 0
        if (ltr_info->key_info[srch].seg_len[2] >= 8) {
            kcr[srch][1] |= 1;
        }
        if (ltr_info->key_info[srch].seg_len[5] >= 8) {
            kcr[srch][1] |= 1;
        }
        if (ltr_info->key_info[srch].seg_len[8] >= 8) {
            kcr[srch][3] |= 1;
        }
#else
        kcr[srch][1] |= (ltr_info->key_info[srch].seg_len[2] >> 3);
        kcr[srch][3] |= (ltr_info->key_info[srch].seg_len[7] >> 3);
#endif
        /* addr of key construction reg 0 */
        addr = TR3_DBREG_LTR_KCR_ADDR(dev, srch, ltr); 
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, kcr[srch][1], 
                            kcr[srch][0]));
        addr++;
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, kcr[srch][3], 
                            kcr[srch][2]));
    }

    /* disable range insertion */
    d2 = 0xffffffff;
    d1 = 0xffffff;
    addr = TR3_DBREG_LTR_RANGE_INS_ADDR(dev, 0, ltr);
    SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, d1, d2));
    addr += 1;
    SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, d1, d2));
    return ltr;
}

/* wrapper to build ltr given a ltr spec */
int
soc_tr3_build_ltr(int unit, int ltr_lib_idx)
{
    int dev;
    soc_tcam_info_t *tcam_info;
    int nl_ltr, entries_per_blk=4096;
    fm_ltrs_t *ltr_profile;
    
    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_INIT;
    }

    ltr_profile = &ltr_profiles[ltr_lib_idx];
    if (ltr_profile->nl_ltr_info[unit] == NULL) {
        ltr_profile->nl_ltr_info[unit] = 
        sal_alloc(sizeof(ltr_install_t), "NL ltr activation info");
        if (ltr_profile->nl_ltr_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(ltr_profile->nl_ltr_info[unit], 0, sizeof(ltr_install_t));
    }

    /* check if already installed */
    if (ltr_profile->nl_ltr_info[unit]->installed) {
        ltr_profile->nl_ltr_info[unit]->ref_count++;
        return ltr_profile->nl_ltr_info[unit]->nl_ltr;
    }
    nl_ltr = -1;
    for (dev = 0; dev < tcam_info->num_tcams; dev++) {
        /* use nl_ltr second time thru loop to avoid double alloc of ltr */
        nl_ltr = _tr3_build_ltr(unit, dev, &ltr_profiles[ltr_lib_idx].ltr, 
                                entries_per_blk, nl_ltr, 0);
        if (nl_ltr < 0) {
            return SOC_E_MEMORY;
        }
    }
    ltr_profile->nl_ltr_info[unit]->nl_ltr = nl_ltr;
    ltr_profile->nl_ltr_info[unit]->installed = 1;
    ltr_profile->nl_ltr_info[unit]->ref_count = 1;
    return nl_ltr;
}

/* Restores the LTR block regters info
 * call when there is any LTR parity error */
int soc_tr3_restore_ltr(int unit, int dev, int ltr)
{
    int rv, idx;
    int entries_per_blk = 4096;

    /* ignore errors during pre-init */
    if (SOC_CONTROL(unit)->tcam_info == NULL) {
        return SOC_E_NONE;
    }

    for (idx = 0 ; idx < 128 ; idx ++) {
        if (ltr_profiles[idx].nl_ltr_info[unit] &&
            (ltr_profiles[idx].nl_ltr_info[unit]->nl_ltr == ltr) &&
            (ltr_profiles[idx].nl_ltr_info[unit]->installed)) {
            break;
        }
    }

    /* Error for LTR which was not configured
     * use last profile which has all config set to 0 */
    if (idx == 128) {
        idx = 127;
    }
    /* Restore its contents */
    rv  = _tr3_build_ltr(unit, dev, &ltr_profiles[idx].ltr,
                         entries_per_blk, ltr, 0);
    return rv;
}

int
soc_tr3_clear_ltr_all(int unit)
{
    fm_ltrs_t *ltr_profile;
    ltr_install_t *nl_inst;
    int i, max;

    max = sizeof(ltr_profiles)/sizeof(fm_ltrs_t);

    for (i = 0; i < max ; i++) {
        ltr_profile = &ltr_profiles[i];
        nl_inst = ltr_profile->nl_ltr_info[unit];

        if (nl_inst == NULL) {
            continue;
        }
        SOC_IF_ERROR_RETURN(_soc_tr3_ltr_free(unit, nl_inst->nl_ltr));
        nl_inst->nl_ltr = -1;
        nl_inst->installed = 0;
        sal_free(nl_inst);
        ltr_profile->nl_ltr_info[unit] = NULL;
    }
    return SOC_E_NONE;
}

/* free ltr of the given library index */
int 
soc_tr3_free_ltr(int unit, int ltr_lib_idx)
{
    fm_ltrs_t *ltr_profile; 
    ltr_install_t *nl_inst;

    ltr_profile = &ltr_profiles[ltr_lib_idx];
    nl_inst = ltr_profile->nl_ltr_info[unit];

    if (nl_inst == NULL) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "unit %d: Attempting to free invalid ltr. "), unit));
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Lib idx: %d\n"), ltr_lib_idx));
        return SOC_E_PARAM;
    }
    
    nl_inst->ref_count--;
    if (nl_inst->ref_count == 0) {
        SOC_IF_ERROR_RETURN(_soc_tr3_ltr_free(unit, nl_inst->nl_ltr));
        nl_inst->nl_ltr = -1;
        nl_inst->installed = 0;
        sal_free(nl_inst);
        ltr_profile->nl_ltr_info[unit] = NULL;
    }  
    return SOC_E_NONE;
}

/* write the given block mask into the block mask register for the partition.
 * Value is specified as array of uint32, with set of 3 specifying a 80bit register
 * value
 */
int _tr3_tcam_write_bmr(int unit, int dev, int blks_per_dev, int part, 
                        int bmr_no, uint32 block_mask[])
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int block_start, block_end, entries_per_blk=4096, block, addr;
    int width;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;
    
    block_start = partitions[part].tcam_base >> 12;
    block_end = block_start + ((partitions[part].num_entries <<
                     partitions[part].tcam_width_shift) +
                    entries_per_blk - 1) / entries_per_blk;
    for (block = block_start; block < block_end; block++) {
        if ((block / blks_per_dev) != dev) {
            continue;
        }
        addr = TR3_DBREG_BMR(block / blks_per_dev, block % blks_per_dev, 
                             bmr_no, 79);
        for (width = 0; width < (1 << partitions[part].tcam_width_shift); 
             width++) {
            SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 
                                 block_mask[width*3 + 2], 
                                 block_mask[width*3 + 1], block_mask[width*3]));
            addr++;
        }
    }
    return SOC_E_NONE;
}

int
_soc_tr3_tcam_fast_write_mode_set (int unit, int dev, int enable)
{
    int addr;
    if(enable) {
        /* 
         * Enable fast write mode to broadcast the writes to
         * all blocks in the device
         */
        addr = 0x3FE + TR3_REG_BASE(dev);
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, 0x80000)); 
        addr = 0x3FF + TR3_REG_BASE(dev);
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, 0x1 << 2)); 
    } else {
        addr = 0x3FE + TR3_REG_BASE(dev);
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, 0x80000)); 
        addr = 0x3FF + TR3_REG_BASE(dev);
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, 0x0)); 
    }

    return SOC_E_NONE;
}

int
soc_tr3_tcam_l2_fast_init (int unit, int start_dev_idx, int num_tcams, 
        void *l2_entry)
{
    int i, start_index = 0, blks_per_dev;
    int dev_idx = start_dev_idx;
    soc_tcam_info_t *tcam_info;
    tcam_info = SOC_CONTROL(unit)->tcam_info;
    blks_per_dev = tcam_info->blks_per_dev;
    
    for (; num_tcams > 0; num_tcams--) {
        if (tcam_info->tcam_init_done[dev_idx]) {
            continue;
        }
        /* Enable fast write */
        SOC_IF_ERROR_RETURN(_soc_tr3_tcam_fast_write_mode_set(unit, 
                    dev_idx, 1));

        /* Initialize database */
        for (i = 0; i < TR3_ENTRIES_PER_BLK; i++) {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, EXT_L2_ENTRY_TCAMm,
                        MEM_BLOCK_ANY, i+start_index, l2_entry));
        }

        /* Disable fast write */
        SOC_IF_ERROR_RETURN(_soc_tr3_tcam_fast_write_mode_set(unit,
                    dev_idx, 0));

        tcam_info->tcam_init_done[dev_idx] = TRUE;
        dev_idx++;
        start_index += blks_per_dev * TR3_ENTRIES_PER_BLK;
    }

    return SOC_E_NONE;
}

/*
 * Clear init the TCAM using fast write mode
 */
int
soc_tr3_tcam_fast_init (int unit)
{
    int addr, i;
    uint32 data[4], mask[4];
    int dev, blks_per_dev;
    int part;

    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    partitions = tcam_info->partitions;
    blks_per_dev = tcam_info->blks_per_dev;

    for (dev = 0; dev < tcam_info->num_tcams; dev++) {

        if (tcam_info->tcam_init_done[dev]) {
            continue;
        }
        /* Enable fast write */
        SOC_IF_ERROR_RETURN(_soc_tr3_tcam_fast_write_mode_set(unit, 
                    dev, 1));

        /* Initialize database */
        addr = partitions[TCAM_PARTITION_RAW].tcam_base + 
            (dev * blks_per_dev * TR3_ENTRIES_PER_BLK) ;
        sal_memset(data, 0, sizeof(data));
        sal_memset(mask, 0, sizeof(mask));
        
        for (i = 0; i < TR3_ENTRIES_PER_BLK; i++) {
            SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_entry(unit, 
                        TCAM_PARTITION_RAW, addr+i, mask, data, 0));

        }

        /* Disable fast write */
        SOC_IF_ERROR_RETURN(_soc_tr3_tcam_fast_write_mode_set(unit, 
                    dev, 0));
        tcam_info->tcam_init_done[dev] = TRUE;
    }

    for(part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT;
            part++)
    {
        /* for non standard partition widths write the valid bits for the 
           unused portion of the entry . This applies to all ACL480 ACL
           partitions */
        if (partitions[part].tcam_width_shift == 3) {
            /* normalize the address per cascading */
            addr = partitions[part].tcam_base + 6;
            sal_memset(data, 0, sizeof(data));
            sal_memset(mask, 0, sizeof(mask));
            for (i = 0; i < partitions[part].num_entries; i++) {
                SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_entry(unit, 
                            TCAM_PARTITION_RAW, addr, mask, data, 1));
                SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_entry(unit, 
                            TCAM_PARTITION_RAW, addr+1, mask, data, 1));
                addr += 8;

            }
        }
    }
    return SOC_E_NONE;
}

int
tr3_tcam_init(int unit)
{
    soc_tcam_info_t *tcam_info;
    soc_tcam_partition_t *partitions;
    int rv, part, tcam_base, entries_per_blk, blks_per_dev=64, i;
    int dev, blk, blk_base, num_blks, mask_len, valid_mask_len;
    int order_idx;
    uint32 addr, d0, d1, d2, bmr_val[24];
    uint32 wb_mode = 0;
#if 0
    uint32 bsr[4], sblk_kpu_select[2];
    /* 2 words per 32 blocks: 0,1 for 0-31, 1,2 for 32-63, etc */
    uint32 parallel_srch[8];
    uint64 kcr;
#endif
    uint32 data[4], mask[4];
    ltr_info_t *ltr, ltr_struc;

    tcam_info = SOC_CONTROL(unit)->tcam_info;
    if (tcam_info == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "unit: %d No external tables configured\n"), unit));
        return SOC_E_NONE;
    }
    partitions = tcam_info->partitions;

    entries_per_blk = TR3_ENTRIES_PER_BLK;

    /* 1. enable next device only on the first time after tcam reset */
    if (!tcam_info->num_tcams) {
        return SOC_E_INIT;
    }

    /* read device model number and size */
    tcam_info->monolith_dev = FALSE;

    /* read devid reg of tcam 0 always. In cascade mode, they are
     * identical
     */
    if (SOC_WARM_BOOT(unit)) {
        wb_mode = 1;
        SOC_WARM_BOOT_DONE(unit);
    }
    rv = tr3_read_reg(unit, TR3_REG_DEVID_ADDR(0), &d0, &d1, &d2);
    if (wb_mode) {
        SOC_WARM_BOOT_START(unit);
    }
    if (SOC_SUCCESS(rv)) {
        switch ((d2 >> TR3_REG_DEVID_DB_SIZE_oft) & TR3_REG_DEVID_DB_SIZE_mask){
        case 0x02: /* 512k 40-bit records (256k 80-bit entries) 20M */
            partitions[TCAM_PARTITION_RAW].num_entries = 256 * 1024 *
                                                        tcam_info->num_tcams;
    		tcam_info->blks_per_dev = blks_per_dev = TR3_BLKS_PER_DEV_64;
            break;
        case 0x03: /* 1024k 40-bit records (512k 80-bit entries) 40M  */
            partitions[TCAM_PARTITION_RAW].num_entries = 512 * 1024 * 
                                                        tcam_info->num_tcams;
    		tcam_info->blks_per_dev = blks_per_dev = TR3_BLKS_PER_DEV_128;
            break;
        default:
            LOG_CLI((BSL_META_U(unit,
                                "Bad device ID register:0x%08x. TCAM init failed\n"), 
                     d2));
			return SOC_E_BADID;
        }
    } else {
        /* return error code */
        LOG_CLI((BSL_META_U(unit,
                            "Failure reading NL11K DEV ID register\n")));
        return rv;
    }

    partitions[TCAM_PARTITION_RAW].tcam_width_shift = 0;

    /* 2. device id register and device configuration register */
    for (i = 0; i < tcam_info->num_tcams; i++) {
        addr = TR3_REG_DEVCFG_ADDR(i);
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, 0));
    }

    tcam_base = 0;
    for (order_idx = 0; 
         order_idx < tr3_tcam_partition_order_num_entries; order_idx++) {
        part = tr3_tcam_partition_order[order_idx];
        if ((part == 0) || (!partitions[part].num_entries)) {
            continue;
        }
        switch (part) {
        case TCAM_PARTITION_FWD_L2:
        case TCAM_PARTITION_FWD_L2_DUP:
        case TCAM_PARTITION_FWD_IP4_UCAST_WIDE:
        case TCAM_PARTITION_FWD_IP4_UCAST:
        case TCAM_PARTITION_FWD_IP4:
        case TCAM_PARTITION_FWD_IP4_DUP:
        case TCAM_PARTITION_FWD_IP6U:
        case TCAM_PARTITION_FWD_IP6U_DUP:
        case TCAM_PARTITION_FWD_L2_WIDE:
        case TCAM_PARTITION_ACL80:
            partitions[part].tcam_width_shift = 0;
            break;
        case TCAM_PARTITION_FWD_IP6_128_UCAST:
        case TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE:
        case TCAM_PARTITION_ACL_L2C:
        case TCAM_PARTITION_ACL_IP4C:
        case TCAM_PARTITION_ACL_IP6C:
            partitions[part].tcam_width_shift = 1;
            break;
        case TCAM_PARTITION_FWD_IP6:
        case TCAM_PARTITION_FWD_IP6_DUP:
        case TCAM_PARTITION_ACL160:
            partitions[part].tcam_width_shift = 1;
            break;
        case TCAM_PARTITION_ACL320:
        case TCAM_PARTITION_ACL_L2:
        case TCAM_PARTITION_ACL_IP4:
        case TCAM_PARTITION_ACL_L2IP4:
            partitions[part].tcam_width_shift = 2;
            break;
        case TCAM_PARTITION_ACL480:
        case TCAM_PARTITION_ACL_IP6S:
        case TCAM_PARTITION_ACL_IP6F:
        case TCAM_PARTITION_ACL_L2IP6:
            partitions[part].tcam_width_shift = 3;
            break;
        default:
            break;
        }

        num_blks = ((partitions[part].num_entries <<
                     partitions[part].tcam_width_shift) +
                    entries_per_blk - 1) / entries_per_blk;
        partitions[part].num_entries_include_pad =
            (num_blks * entries_per_blk) >> partitions[part].tcam_width_shift;
        partitions[part].tcam_base = tcam_base;
        /* increment has to be multiple of 4k */
        tcam_base += (num_blks * entries_per_blk);
        if (!(partitions[part].flags & TCAM_PARTITION_FLAGS_COALESCE)) {
            if (tcam_base % (4*entries_per_blk)) {
                tcam_base += (4*entries_per_blk-tcam_base % (4*entries_per_blk));
            }
        }
    }
    if (tcam_base > partitions[TCAM_PARTITION_RAW].num_entries) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "tr3_tcam_init: unit %d number of entries "
                              "configured exceeds physical device size\n"),
                   unit));
        return SOC_E_PARAM;
    }

    /*
     * 3. program block configuration registers for all devices
     */
    blk = 0;
    for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT; part++) {
        if (!partitions[part].num_entries) {
            continue;
        }

        num_blks = ((partitions[part].num_entries <<
                     partitions[part].tcam_width_shift) +
                    entries_per_blk - 1) / entries_per_blk;
        blk_base = partitions[part].tcam_base /entries_per_blk;
        for (blk = blk_base; blk < blk_base + num_blks; blk++) {
            addr = TR3_DBREG_BCFG_ADDR(blk / blks_per_dev,
                                         blk % blks_per_dev);
			/* block width[3:1] enable[0] */
            d2 = MAKE_FIELD(TR3_DBREG_BCFG_WIDTH, 
					        partitions[part].tcam_width_shift) |
				 MAKE_FIELD(TR3_DBREG_BCFG_ENABLE, 1); 
            SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, d2));
        }
        /* for non standard partition widths write the valid bits for the 
           unused portion of the entry . This applies to all ACL480 ACL
           partitions */
        if (partitions[part].tcam_width_shift == 3) {
            /* normalize the address per cascading */
            addr = partitions[part].tcam_base + 6;
            sal_memset(data, 0, sizeof(data));
            sal_memset(mask, 0, sizeof(mask));
            for (i = 0; i < partitions[part].num_entries; i++) {
                SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_entry(unit, 
                                         TCAM_PARTITION_RAW, addr, mask, data, 1));
                SOC_IF_ERROR_RETURN(soc_tr3_tcam_write_entry(unit, 
                                        TCAM_PARTITION_RAW, addr+1, mask, data, 1));
                addr += 8;
                
            }
        }
    }
    /*
     * Fill the unused block configuration registers with zero.
     * Since blocks are arranged in continuously ascending order, all free
     * blocks are at the bottom */
    for (; blk < tcam_info->num_tcams * blks_per_dev; blk++) {
        addr = TR3_DBREG_BCFG_ADDR(blk / blks_per_dev, blk % blks_per_dev);
        SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr, 0, 0, 0));
    }

    /* initialize ltrs free bitmap. All CP ltrs are preallocated */
    tr3_ltr_free_bitmap[unit][1] = 0x03ffffff;
    tr3_ltr_free_bitmap[unit][0] = 0xffffffff;
    
    /*
     * 4. program block mask registers and logical table registers
     * of each device
     */
    for (dev = 0; dev < tcam_info->num_tcams; dev++) {
        /*
         * 4-1. block mask registers
         */
        blk = 0;
        for (part = TCAM_PARTITION_RAW + 1; part < TCAM_PARTITION_COUNT;
             part++) {
            if (!partitions[part].num_entries) {
                continue;
            }

            mask_len = 1 << partitions[part].tcam_width_shift;
            if (part == TCAM_PARTITION_ACL_IP6S) {
                valid_mask_len = 5; /* use only 72 * 5 = 360 bits */
            } else if (part == TCAM_PARTITION_ACL_IP6F ||
                       part == TCAM_PARTITION_ACL_L2IP4 ||
                       part == TCAM_PARTITION_ACL480 ||
                       part == TCAM_PARTITION_ACL_L2IP6) {
                valid_mask_len = 6; /* use only 72 * 6 = 432 bits */
            } else {
                valid_mask_len = mask_len;
            }

            num_blks = ((partitions[part].num_entries <<
                         partitions[part].tcam_width_shift) +
                        entries_per_blk - 1) / entries_per_blk;
            blk_base = partitions[part].tcam_base /entries_per_blk;
            /* TBD: need a way to generically specify entry mask for each
             * partition
             */
            if ((part == TCAM_PARTITION_FWD_L2) || 
               (part == TCAM_PARTITION_FWD_L2_WIDE)) {
                d0 = 0xffff;
            } else {
                d0 = 0;
            }
            for (blk = blk_base; blk < blk_base + num_blks; blk++) {
                if (blk /blks_per_dev != dev) {
                    continue;
                }
                addr = TR3_DBREG_BMASK_ADDR(dev, blk % blks_per_dev, 0);

                /* useful portion */
                for (i = 0; i < valid_mask_len; i++) {
                    SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr + i,
                                                           d0, 0, 0)); 
                }
                /* unused portion */
                for (; i < mask_len; i++) {
                    SOC_IF_ERROR_RETURN(tr3_write_reg(unit, addr + i,
                                                           0xffff,
                                                           0xffffffff,
                                                           0xffffffff));
                }
            }
        }

        /*
         * 4-2. logical table registers
         *      - block select registers
         *      - kpu select per super block
         *      - search result port map select per block
         *      - bmr select for parallel search (?)
         *      - key construction regs for parallel searches.
         *      - range insertion registers
         */
        if (tcam_base) {
			/* block select registers */
			/* blocks should be allocated in multiples of 4 only. Blocks
             * for a table should also start on a multiple of 4 boundary
			 */
            /* write BMR1 of the L2 blocks */
            bmr_val[0] = 0; bmr_val[1] = 0; bmr_val[2] = 0xffff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev, 
                                                    TCAM_PARTITION_FWD_L2, 1, 
                                                    bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev, 
                                                    TCAM_PARTITION_FWD_L2_WIDE, 
                                                    1, bmr_val));
            SOC_IF_ERROR_RETURN(soc_tr3_clear_ltr_all(unit));
            sal_memset(&ltr_profiles[TR3_LTR_LIB_L2_FWD_DA], 0, 
                        sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_L2_FWD_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 2; 
            ltr->key_info[0].seg_len[1] = 3;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 0; 
    
            /* L2 ONLY SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_L2_FWD_SA], 0, 
                        sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_L2_FWD_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 20; 

            /* since DB is duplicated create only 1 LTR. */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_L2_FWD_DUP], 0, 
                        sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_L2_FWD_DUP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2_DUP;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 2; 
            ltr->key_info[0].seg_len[1] = 3;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 0; 

            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 16; 

            /* CP LTR L2 ALL */
            /* control plane LTRs get installed at init time */
            sal_memset(&ltr_struc, 0, sizeof(ltr_struc));
            ltr = &ltr_struc;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 8; 
            ltr->key_info[0].seg_len[1] = 5;
            ltr->key_info[0].seg_offset[1] = 2; 
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 0; 
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit,dev,ltr,entries_per_blk, 
                                TR3_LTR_CP_L2_ALL, 0));
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[0][1] = 0;
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit, dev, ltr, 
                                entries_per_blk, TR3_LTR_CP_L2_WIDE, 0));
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = 0;
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit, dev, ltr, 
                                entries_per_blk, TR3_LTR_CP_L2, 0));


            /* write BMR0, 1 of the IPV4 blocks */
            bmr_val[0] = 0; bmr_val[1] = 0xffffe000; bmr_val[2] = 0xffff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4_UCAST, 0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4_UCAST_WIDE, 0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4_UCAST, 1, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4_UCAST_WIDE, 1, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4, 0, bmr_val));
            bmr_val[0] = 0; bmr_val[1] = 0xffffe000; bmr_val[2] = 0x03ff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4, 1, bmr_val));

            /* CP LTR of duplicate database is not used */
            bmr_val[0] = 0; bmr_val[1] = 0xffffe000; bmr_val[2] = 0xffff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4_DUP, 0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP4_DUP, 1, bmr_val));

            /* V4 ONLY DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V4_ONLY_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V4_ONLY_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 2; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 

            /* V4 ONLY SIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V4_ONLY_SIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V4_ONLY_SIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 6; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 

            /* V4 ONLY CP LTRs */
            sal_memset(&ltr_struc, 0, sizeof(ltr_struc));
            ltr = &ltr_struc;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 1;
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[2] = 3;
            ltr->key_info[0].seg_offset[2] = 6; 
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit, dev, ltr, 
                                entries_per_blk, TR3_LTR_CP_V4_ALL, 0));

            /* write BMR0, 1 of the IPV6 64 blocks */
            bmr_val[0] = 0; bmr_val[1] = 0; bmr_val[2] = 0xe000;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6U,
                                0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6U,
                                1, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6U_DUP,
                                0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6U_DUP,
                                1, bmr_val));

            /* write BMR0, 1 of the IPV6 blocks */
            bmr_val[0] = 0; bmr_val[1] = 0; bmr_val[2] = 0;
            bmr_val[3] = 0; bmr_val[4] = 0xe0000000; bmr_val[5] = 0xffff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6_128_UCAST,
                                0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE, 0, 
                                bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6,
                                0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6_128_UCAST,
                                1, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE, 1, 
                                bmr_val));

            bmr_val[0] = 0; bmr_val[1] = 0xe0000000; bmr_val[2] = 0x00ff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_FWD_IP6,
                                1, bmr_val));

            /* v6 LTRs */
            /* DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_64_ONLY_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_64_ONLY_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 2; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 

            /* SIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_64_ONLY_SIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_64_ONLY_SIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 5;
            ltr->key_info[0].seg_offset[0] = 10; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 20; 
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 0; 

            sal_memset(&ltr_struc, 0, sizeof(ltr_struc));
            ltr = &ltr_struc;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 1;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 8; 
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 0; 
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit, dev, ltr, 
                                entries_per_blk, TR3_LTR_CP_V6_64_DEFIP, 0));


            /* V6 128 only DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_128_ONLY_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_128_ONLY_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 

            /* V6 128 only SIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_128_ONLY_SIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_128_ONLY_SIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 30; 

            /* V6 128 DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS0_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS0_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 20; 

            /* V6 CP LTRs */
            sal_memset(&ltr_struc, 0, sizeof(ltr_struc));
            ltr = &ltr_struc;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 1;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit, dev, ltr, 
                                entries_per_blk, TR3_LTR_CP_V6_128_ALL, 0));

            /* IPV4 SIP + SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V4_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V4_SIP_SA].ltr;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 6; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 20; 
            ltr->key_info[1].seg_len[0] = 5;
            ltr->key_info[1].seg_offset[0] = 10; 

            sal_memset(&ltr_profiles[TR3_LTR_LIB_V4_DUP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V4_DUP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP4_DUP;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 6; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 10; 
            ltr->key_info[2].seg_len[0] = 3;
            ltr->key_info[2].seg_offset[0] = 2; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 0; 

            /* IPV6 128 SIP + SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_128_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_128_SIP_SA].ltr;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[0] = 15;
            if (SAL_BOOT_QUICKTURN) {
                ltr->key_info[0].seg_offset[0] = 40; 
            } else {
                ltr->key_info[0].seg_offset[0] = 38; 
            }
            ltr->key_info[1].seg_len[2] = 1;
            ltr->key_info[1].seg_offset[2] = 22; 
            ltr->key_info[1].seg_len[1] = 3;
            if (SAL_BOOT_QUICKTURN) {
                ltr->key_info[1].seg_offset[1] = 28; 
            } else {
                ltr->key_info[1].seg_offset[1] = 34; 
            }
            ltr->key_info[1].seg_len[0] = 1;
            ltr->key_info[1].seg_offset[0] = 18; 

            /* V6 128 SIP, DIP, SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_128_DUP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_128_DUP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP6_DUP;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 32; 
            ltr->key_info[1].seg_len[2] = 1;
            ltr->key_info[1].seg_offset[2] = 22; 
            ltr->key_info[1].seg_len[1] = 3;
            ltr->key_info[1].seg_offset[1] = 28; 
            ltr->key_info[1].seg_len[0] = 1;
            ltr->key_info[1].seg_offset[0] = 18; 
            ltr->key_info[2].seg_len[0] = 15;
            ltr->key_info[2].seg_offset[0] = 0; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 16; 

            /* IPV6 64 SIP + SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_64_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_64_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 5;
            ltr->key_info[0].seg_offset[0] = 10; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 20; 
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 0; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 22; 
            
        
            /* v6 64 DUP DIP SIP SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_V6_64_DUP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_V6_64_DUP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP6U_DUP;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 10; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 18; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 2; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 0; 

            /* write BMR0, of the ACL7 blocks */
            bmr_val[0] = 0; bmr_val[1] = 0; bmr_val[2] = 0;
            bmr_val[3] = 0; bmr_val[4] = 0; bmr_val[5] = 0;
            bmr_val[6] = 0; bmr_val[7] = 0; bmr_val[8] = 0;
            bmr_val[9] = 0; bmr_val[10] = 0; bmr_val[11] = 0;
            bmr_val[12] = 0; bmr_val[13] = 0; bmr_val[14] = 0;
            bmr_val[15] = 0; bmr_val[16] = 0; bmr_val[17] = 0;
            bmr_val[18] = 0xffffffff; bmr_val[19] = 0xffffffff; bmr_val[20] = 0xffff;
            bmr_val[21] = 0xffffffff; bmr_val[22] = 0xffffffff; bmr_val[23] = 0xffff;
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_ACL480,
                                0, bmr_val));

            /* do the same for all 480 bit ACL partitions */
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_ACL_IP6S,
                                0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_ACL_IP6F,
                                0, bmr_val));
            SOC_IF_ERROR_RETURN(_tr3_tcam_write_bmr(unit, dev, blks_per_dev,
                                TCAM_PARTITION_ACL_L2IP6,
                                0, bmr_val));

            /* ACL7 IPV6 128 SIP + SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS0_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS0_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL480;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 15;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[2] = 15;
            ltr->key_info[0].seg_offset[2] = 32; 
            if (SAL_BOOT_QUICKTURN) {
                ltr->key_info[0].seg_len[3] = 5;
                ltr->key_info[0].seg_offset[3] = 50; 
                ltr->key_info[0].seg_len[4] = 5;
                ltr->key_info[0].seg_offset[4] = 58; 
            } else {
                ltr->key_info[0].seg_len[3] = 5;
                ltr->key_info[0].seg_offset[3] = 48; 
                ltr->key_info[0].seg_len[4] = 5;
                ltr->key_info[0].seg_offset[4] = 56; 
            }
            ltr->key_info[1].seg_len[0] = 15;
            ltr->key_info[1].seg_offset[0] = 30; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 20; 
            ltr->key_info[2].seg_len[0] = 1;
            ltr->key_info[2].seg_offset[0] = 18; 
            if (SAL_BOOT_QUICKTURN) {
                ltr->key_info[2].seg_len[1] = 5;
                ltr->key_info[2].seg_offset[1] = 58; 
            } else {
                ltr->key_info[2].seg_len[1] = 5;
                ltr->key_info[2].seg_offset[1] = 56; 
            }

            /* AFLSS1_V4_DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS1_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS1_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 14; 

            /* AFLSS1_ACL_V4_SIP_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS1_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS1_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL320;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[4] = 5;
            ltr->key_info[0].seg_offset[4] = 38; 
            ltr->key_info[0].seg_len[2] = 5;
            ltr->key_info[0].seg_offset[2] = 30; 
            ltr->key_info[0].seg_len[1] = 7;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 4; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 18; 
            ltr->key_info[2].seg_len[1] = 5;
            ltr->key_info[2].seg_offset[1] = 38; 
            ltr->key_info[2].seg_len[0] = 1;
            ltr->key_info[2].seg_offset[0] = 2; 

            /* AFLSS2_V6_64_DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS2_DIP], 0, sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS2_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 20; 
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 0; 

            /* AFLSS2_ACL_V6_64_SIP_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS2_ACL_SIP_SA], 0, sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS2_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL480;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[4] = 5;
            ltr->key_info[0].seg_offset[4] = 48; 
            ltr->key_info[0].seg_len[2] = 5;
            ltr->key_info[0].seg_offset[2] = 40; 
            ltr->key_info[0].seg_len[1] = 15;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 20; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 8; 
            ltr->key_info[2].seg_len[1] = 5;
            ltr->key_info[2].seg_offset[1] = 48; 
            ltr->key_info[2].seg_len[0] = 1;
            ltr->key_info[2].seg_offset[0] = 18; 

            /* AFLSS3_ACL_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS3_ACL_SA],
                        0, sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS3_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL160;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[2] = 13;
            ltr->key_info[0].seg_offset[2] = 8; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 2; 
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 4; 
            ltr->key_info[1].seg_len[2] = 1;
            ltr->key_info[1].seg_offset[2] = 2; 
            ltr->key_info[1].seg_len[1] = 3;
            ltr->key_info[1].seg_offset[1] = 4; 
            ltr->key_info[1].seg_len[0] = 1;
            ltr->key_info[1].seg_offset[0] = 0; 

            /* AFLSS0A_V6_128_DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS0A_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS0A_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 18; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 

            /* AFLSS0A_ACL_V6_128_SIP_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS0A_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS0A_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL320;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            if (SAL_BOOT_QUICKTURN) {
                ltr->key_info[0].seg_len[2] = 7;
                ltr->key_info[0].seg_offset[2] = 40; 
                ltr->key_info[0].seg_len[1] = 15;
            } else {
                ltr->key_info[0].seg_len[2] = 15;
                ltr->key_info[0].seg_offset[2] = 30; 
                ltr->key_info[0].seg_len[1] = 7;
            }
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            if (SAL_BOOT_QUICKTURN) {
                ltr->key_info[1].seg_len[1] = 7;
                ltr->key_info[1].seg_offset[1] = 40; 
                ltr->key_info[1].seg_len[0] = 7;
                ltr->key_info[1].seg_offset[0] = 24; 
                ltr->key_info[2].seg_len[0] = 7;
                ltr->key_info[2].seg_offset[0] = 48; 
            } else {
                ltr->key_info[1].seg_len[1] = 1;
                ltr->key_info[1].seg_offset[1] = 18; 
                ltr->key_info[1].seg_len[0] = 15;
                ltr->key_info[1].seg_offset[0] = 30; 
                ltr->key_info[2].seg_len[0] = 7;
                ltr->key_info[2].seg_offset[0] = 46; 
            }

            /* AFLSS1A_V4_DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS1A_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS1A_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 2; 
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 8; 

            /* AFLSS1A_ACL_V4_SIP_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS1A_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS1A_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL160;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 2; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 12; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 20; 

            /* AFLSS2A_V6_64_DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS2A_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS2A_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 2; 
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 8; 

            /* AFLSS2A_ACL_V6_64_SIP_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS2A_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS2A_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL320;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[1] = 7;
            ltr->key_info[0].seg_offset[1] = 20; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 2; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 20; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 28; 
            SOC_IF_ERROR_RETURN(_tr3_build_ltr(unit, dev, ltr, 
                                entries_per_blk, TR3_LTR_AFLSS2A_ACL_V6_64_SIP_SA, 0));

            /* AFLSS1AMP_V4_DIP_SIP_SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_AFLSS1AMP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_AFLSS1AMP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL160;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP4_DUP;
            ltr->partition[3][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[3][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->sblk_kpu_select[3] = 3;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->parallel_srch[3] = 3;
            ltr->num_searches = 4;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->bmr_select[3] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 2; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 8; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 2; 
            ltr->key_info[2].seg_len[0] = 3;
            ltr->key_info[2].seg_offset[0] = 12; 
            ltr->key_info[3].seg_len[0] = 7;
            ltr->key_info[3].seg_offset[0] = 16; 

            /* FLSS0 */
            /* SIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_FLSS0_SIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_FLSS0_SIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 18; 
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 30; 

            /* FLSS0 SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_FLSS0_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_FLSS0_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 50; 

            /* TR2 L2_ACL_144 */
            /* DA search */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 2; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 8; 
            ltr->key_info[0].seg_len[2] = 3;
            ltr->key_info[0].seg_offset[2] = 10; 

            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2C;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 20; 
            ltr->key_info[0].seg_len[1] = 5;
            ltr->key_info[0].seg_offset[1] = 22; 
            ltr->key_info[1].seg_len[0] = 0;
            ltr->key_info[1].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 9;
            ltr->key_info[1].seg_offset[1] = 6; 
            ltr->key_info[1].seg_len[2] = 7;
            ltr->key_info[1].seg_offset[2] = 22; 

            /* ACL_L2C only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2C;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 0;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 9;
            ltr->key_info[0].seg_offset[1] = 6; 
            ltr->key_info[0].seg_len[2] = 7;
            ltr->key_info[0].seg_offset[2] = 18; 

            /* L2C DIP search */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 8; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 4; 

            /* L2C SIP + ACL */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL_SIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL_SIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2C;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[2][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 28; 
            ltr->key_info[1].seg_len[0] = 0;
            ltr->key_info[1].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 6; 
            ltr->key_info[1].seg_len[2] = 7;
            ltr->key_info[1].seg_offset[2] = 20; 
            ltr->key_info[1].seg_len[3] = 7;
            ltr->key_info[1].seg_offset[3] = 30; 
            ltr->key_info[2].seg_len[0] = 3;
            ltr->key_info[2].seg_offset[0] = 12; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 4; 

            /* L2C DIPv6_64 search */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_DIPV6_64], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_DIPV6_64].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 16; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 4; 

            /* L2C SIPv6_64 + SA + ACL */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_64], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_64].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2C;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 40; 
            ltr->key_info[1].seg_len[0] = 0;
            ltr->key_info[1].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 9;
            ltr->key_info[1].seg_offset[1] = 6; 
            ltr->key_info[1].seg_len[2] = 7;
            ltr->key_info[1].seg_offset[2] = 42; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 24; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 4; 

            /* L2C DIPV6_128 search */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_DIPV6_128], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_DIPV6_128].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 20; 

            /* L2C SIPV6_128 + SA + ACL */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_128], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2C_SA_ACL_SIPV6_128].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2C;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[2][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 56; 
            ltr->key_info[1].seg_len[0] = 0;
            ltr->key_info[1].seg_offset[0] = 16; 
            ltr->key_info[1].seg_len[1] = 9;
            ltr->key_info[1].seg_offset[1] = 22; 
            ltr->key_info[1].seg_len[2] = 7;
            ltr->key_info[1].seg_offset[2] = 58; 
            ltr->key_info[2].seg_len[0] = 15;
            ltr->key_info[2].seg_offset[0] = 40; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 20; 

            /* TR2 EXT_L2_ACL_1 */
            /* DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 40; 
            ltr->key_info[0].seg_len[1] = 5;
            ltr->key_info[0].seg_offset[1] = 16; 

            /* SA ACL for non-7K mode */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_SA_ACL_N7K], 0,
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_SA_ACL_N7K].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 40;
            ltr->key_info[1].seg_len[0] = 2;
            ltr->key_info[1].seg_offset[0] = 0;
            ltr->key_info[1].seg_len[1] = 7;
            ltr->key_info[1].seg_offset[1] = 4;
            /* segment length greater than 16 bytes has to be broken up into
             * pieces each less than 16 bytes */
            ltr->key_info[1].seg_len[2] = 8;
            ltr->key_info[1].seg_offset[2] = 14;
            ltr->key_info[1].seg_len[3] = 8;
            ltr->key_info[1].seg_offset[3] = 23;
            ltr->key_info[1].seg_len[4] = 7;
            ltr->key_info[1].seg_offset[4] = 40;
            ltr->key_info[1].seg_len[5] = 1;
            ltr->key_info[1].seg_offset[5] = 50;

            /* SA ACL for 7K mode */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_SA_ACL], 0,
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_SA_ACL].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 40; 
            ltr->key_info[1].seg_len[0] = 2;
            ltr->key_info[1].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 7;
            ltr->key_info[1].seg_offset[1] = 4; 
            /* segment length greater than 16 bytes has to be broken up into
             * pieces each less than 16 bytes */
            ltr->key_info[1].seg_len[2] = 8;
            ltr->key_info[1].seg_offset[2] = 14; 
            ltr->key_info[1].seg_len[3] = 8;
            ltr->key_info[1].seg_offset[3] = 23; 
            ltr->key_info[1].seg_len[4] = 5;
            ltr->key_info[1].seg_offset[4] = 42; 
            ltr->key_info[1].seg_len[5] = 1;
            ltr->key_info[1].seg_offset[5] = 50; 

            /* ACL_L2 only for 7K mode */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 2;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 7;
            ltr->key_info[0].seg_offset[1] = 4; 
            /* segment length greater than 16 bytes has to be broken up into
             * pieces each less than 16 bytes */
            ltr->key_info[0].seg_len[2] = 8;
            ltr->key_info[0].seg_offset[2] = 14; 
            ltr->key_info[0].seg_len[3] = 8;
            ltr->key_info[0].seg_offset[3] = 23; 
            ltr->key_info[0].seg_len[4] = 7;
            ltr->key_info[0].seg_offset[4] = 42; 

            /* ACL_L2 only for non-7K mode */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_ONLY_N7K], 0,
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_ONLY_N7K].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 2;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 7;
            ltr->key_info[0].seg_offset[1] = 4; 
            /* segment length greater than 16 bytes has to be broken up into
             * pieces each less than 16 bytes */
            ltr->key_info[0].seg_len[2] = 8;
            ltr->key_info[0].seg_offset[2] = 14; 
            ltr->key_info[0].seg_len[3] = 8;
            ltr->key_info[0].seg_offset[3] = 23; 
            ltr->key_info[0].seg_len[4] = 9;
            ltr->key_info[0].seg_offset[4] = 40; 

            /* ACL_L2 DIPV6_64 */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_DIPV6_64], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_DIPV6_64].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 40; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 6; 

            /* ACL_L2 SIPV6_64 SA ACL */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2_SA_ACL_SIPV6_64], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2_SA_ACL_SIPV6_64].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->partition[1][0] = TCAM_PARTITION_ACL_L2;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 58; 
            ltr->key_info[1].seg_len[0] = 11;
            ltr->key_info[1].seg_offset[0] = 0; 
            ltr->key_info[1].seg_len[1] = 15;
            ltr->key_info[1].seg_offset[1] = 14; 
            ltr->key_info[1].seg_len[2] = 1;
            ltr->key_info[1].seg_offset[2] = 30; 
            ltr->key_info[1].seg_len[3] = 7;
            ltr->key_info[1].seg_offset[3] = 60; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 50; 
            ltr->key_info[2].seg_len[1] = 1;
            ltr->key_info[2].seg_offset[1] = 6; 

            /* TR2 EXT_L2_ACL_2 */
            /* same as above. F5 is now range_chk instead of VRF */

            /* TR2 EXT_ACL_144_IPV4 */
            /* DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4C_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4C_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 12; 

            /* SIP, SA, ACL */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4C_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4C_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP4C;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 0;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[2] = 3;
            ltr->key_info[0].seg_offset[2] = 20; 
            ltr->key_info[0].seg_len[3] = 1;
            ltr->key_info[0].seg_offset[3] = 32; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 20; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 4; 
            ltr->key_info[2].seg_len[0] = 1;
            ltr->key_info[2].seg_offset[0] = 2; 
            ltr->key_info[2].seg_len[1] = 5;
            ltr->key_info[2].seg_offset[1] = 34; 

            /* IP4C ACL only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4C_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4C_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP4C;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 0;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 15;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 28; 

           /* IP4C L2 FWD */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4C_L2_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4C_L2_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 24; 

            /* IP4C l2 only ACL+DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4C_L2_ACL_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4C_L2_ACL_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP4C;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 0;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 3;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[2] = 11;
            ltr->key_info[0].seg_offset[2] = 12; 
            ltr->key_info[0].seg_len[3] = 1;
            ltr->key_info[0].seg_offset[3] = 10; 
            ltr->key_info[1].seg_len[0] = 1;
            ltr->key_info[1].seg_offset[0] = 2; 
            ltr->key_info[1].seg_len[1] = 5;
            ltr->key_info[1].seg_offset[1] = 42; 

            /* TR2 EXT_ACL_IPV4 */
            /* DIP */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 4; 
            ltr->key_info[0].seg_len[0] = 3;
            ltr->key_info[0].seg_offset[0] = 16; 

            /* ACL, SIP, SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP4;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 11;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 12; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 20; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 4; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 30; 

            /* ACL_IP4 only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 11;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 12; 

            /* ACL_IP4 ACL+ SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4_L2_ACL_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4_L2_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP4;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 11;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 12; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 24; 

            /* ACL_IP4 DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP4_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP4_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 4; 
            ltr->key_info[0].seg_len[1] = 5;
            ltr->key_info[0].seg_offset[1] = 42; 

            /* TR2 EXT_ACL_L2_IPV4 */
            /* DIP same as EXT_ACL_IPV4 */
            /* ACL, SIP, SA: */
            /* Only ACL is different */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2IP4;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP4_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP4_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP4;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 1;
                ltr->key_info[0].seg_offset[0] = 0;
                ltr->key_info[0].seg_len[1] = 4;
                ltr->key_info[0].seg_offset[1] = 11;
                ltr->key_info[0].seg_len[2] = 7;
                ltr->key_info[0].seg_offset[2] = 16;
                ltr->key_info[0].seg_len[3] = 15;
                ltr->key_info[0].seg_offset[3] = 30;
                ltr->key_info[1].seg_len[0] = 3;
                ltr->key_info[1].seg_offset[0] = 20;
                ltr->key_info[1].seg_len[1] = 1;
                ltr->key_info[1].seg_offset[1] = 4;
                ltr->key_info[2].seg_len[0] = 1;
                ltr->key_info[2].seg_offset[0] = 2;
                ltr->key_info[2].seg_len[1] = 5;
                ltr->key_info[2].seg_offset[1] = 40;
            } else {
                ltr->key_info[0].seg_len[0] = 11;
                ltr->key_info[0].seg_offset[0] = 0;
                ltr->key_info[0].seg_len[1] = 11;
                ltr->key_info[0].seg_offset[1] = 12;
                ltr->key_info[0].seg_len[2] = 7;
                ltr->key_info[0].seg_offset[2] = 30;
                ltr->key_info[0].seg_len[3] = 5;
                ltr->key_info[0].seg_offset[3] = 40;
                ltr->key_info[1].seg_len[0] = 3;
                ltr->key_info[1].seg_offset[0] = 20;
                ltr->key_info[1].seg_len[1] = 1;
                ltr->key_info[1].seg_offset[1] = 4;
                ltr->key_info[2].seg_len[0] = 7;
                ltr->key_info[2].seg_offset[0] = 30;
            }
            /* ACL_L2IP4 only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2IP4;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 5;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 7;
                ltr->key_info[0].seg_offset[1] = 6; 
                ltr->key_info[0].seg_len[2] = 7;
                ltr->key_info[0].seg_offset[2] = 14; 
                ltr->key_info[0].seg_len[3] = 15;
                ltr->key_info[0].seg_offset[3] = 22;
            } else {
                ltr->key_info[0].seg_len[0] = 11;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 11;
                ltr->key_info[0].seg_offset[1] = 12; 
                ltr->key_info[0].seg_len[2] = 7;
                ltr->key_info[0].seg_offset[2] = 24; 
                ltr->key_info[0].seg_len[3] = 5;
                ltr->key_info[0].seg_offset[3] = 34; 
            }

            /* ACL_L2IP4 L2 DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 1;
                ltr->key_info[0].seg_offset[0] = 2; 
                ltr->key_info[0].seg_len[1] = 5;
                ltr->key_info[0].seg_offset[1] = 26; 
            } else {
                ltr->key_info[0].seg_len[0] = 1;
                ltr->key_info[0].seg_offset[0] = 4; 
                ltr->key_info[0].seg_len[1] = 5;
                ltr->key_info[0].seg_offset[1] = 42; 
            }

            /* ACL_L2IP4 ACL + SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_L2_ACL_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP4_L2_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2IP4;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP4_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 1;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 11;
                ltr->key_info[0].seg_offset[1] = 4; 
                ltr->key_info[0].seg_len[2] = 15;
                ltr->key_info[0].seg_offset[2] = 16;
                ltr->key_info[0].seg_len[3] = 7;
                ltr->key_info[0].seg_offset[3] = 40;
                ltr->key_info[1].seg_len[0] = 1;
                ltr->key_info[1].seg_offset[0] = 2;
                ltr->key_info[1].seg_len[1] = 5;
                ltr->key_info[1].seg_offset[1] = 42;
            } else {
                ltr->key_info[0].seg_len[0] = 11;
                ltr->key_info[0].seg_offset[0] = 0;
                ltr->key_info[0].seg_len[1] = 11;
                ltr->key_info[0].seg_offset[1] = 12; 
                ltr->key_info[0].seg_len[2] = 7;
                ltr->key_info[0].seg_offset[2] = 24; 
                ltr->key_info[0].seg_len[3] = 5;
                ltr->key_info[0].seg_offset[3] = 42; 
                ltr->key_info[1].seg_len[0] = 7;
                ltr->key_info[1].seg_offset[0] = 24; 
            }

            /* TR2 EXT_ACL144_IPV6 */
            /* DIPv6 */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 4; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 0; 

            /* ACL, SIPV6, SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6C;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 13;
            ltr->key_info[0].seg_offset[0] = 2; 
            ltr->key_info[0].seg_len[1] = 3;
            ltr->key_info[0].seg_offset[1] = 20; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 12; 
            ltr->key_info[1].seg_len[1] = 3;
            ltr->key_info[1].seg_offset[1] = 20; 
            ltr->key_info[1].seg_len[2] = 1;
            ltr->key_info[1].seg_offset[2] = 0; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 24; 

            /* IP6C ACL only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6C;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 

            /* ACL IP6C L2 DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 30; 
            ltr->key_info[0].seg_len[2] = 3;
            ltr->key_info[0].seg_offset[2] = 40; 

            /* ACL_IP6C ACL+SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_L2_ACL_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_L2_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6C;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 2; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 18; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 20; 

            /* ACL_IP6C DIPv6_128 */
            /* DIPv6 */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_DIPV6_128], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_DIPV6_128].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 16; 

            /* ACL_IP6C ACL, SIPV6_128, SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6C_ACL_SIPV6_128_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6C_ACL_SIPV6_128_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6C;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 18; 
            ltr->key_info[0].seg_len[1] = 7;
            ltr->key_info[0].seg_offset[1] = 8; 
            ltr->key_info[0].seg_len[2] = 7;
            ltr->key_info[0].seg_offset[2] = 34; 
            ltr->key_info[1].seg_len[0] = 3;
            ltr->key_info[1].seg_offset[0] = 20; 
            ltr->key_info[1].seg_len[1] = 11;
            ltr->key_info[1].seg_offset[1] = 30; 
            ltr->key_info[1].seg_len[2] = 1;
            ltr->key_info[1].seg_offset[2] = 16; 
            ltr->key_info[2].seg_len[0] = 7;
            ltr->key_info[2].seg_offset[0] = 42; 

            /* TR2 EXT_ACL_SHORT_IPV6 */
            /* DIPv6 */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_DIP], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_DIP].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[0][2] = TCAM_PARTITION_FWD_IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 18; 

            /* ACL SIP SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6S;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 11;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 12; 
            ltr->key_info[0].seg_len[2] = 3;
            ltr->key_info[0].seg_offset[2] = 30; 
            ltr->key_info[0].seg_len[3] = 1;
            ltr->key_info[0].seg_offset[3] = 36; 
            ltr->key_info[0].seg_len[4] = 8;
            ltr->key_info[0].seg_offset[4] = 44; 
            ltr->key_info[0].seg_len[5] = 8;
            ltr->key_info[0].seg_offset[5] = 53; 
            ltr->key_info[1].seg_len[0] = 15;
            ltr->key_info[1].seg_offset[0] = 46; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 18; 
            ltr->key_info[2].seg_len[0] = 1;
            ltr->key_info[2].seg_offset[0] = 62; 
            ltr->key_info[2].seg_len[1] = 5;
            ltr->key_info[2].seg_offset[1] = 64; 

            /* ACL_IP6S only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6S;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 30; 
            ltr->key_info[0].seg_len[3] = 15;
            ltr->key_info[0].seg_offset[3] = 38; 
            ltr->key_info[0].seg_len[4] = 1;
            ltr->key_info[0].seg_offset[4] = 54; 

            /* ACL IP6S L2fwd DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 18; 
            ltr->key_info[0].seg_len[1] = 5;
            ltr->key_info[0].seg_offset[1] = 32; 

           /* ACLIP6S L2 fwd ACL SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_L2_ACL_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_L2_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6S;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[2] = 1;
            ltr->key_info[0].seg_offset[2] = 30; 
            ltr->key_info[0].seg_len[3] = 15;
            ltr->key_info[0].seg_offset[3] = 38; 
            ltr->key_info[0].seg_len[4] = 1;
            ltr->key_info[0].seg_offset[4] = 54; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 56; 

            /* ACL_IP6S DIPv6_64 */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_DIPV6_64], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_DIPV6_64].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 7;
            ltr->key_info[0].seg_offset[0] = 8; 
            ltr->key_info[0].seg_len[1] = 1;
            ltr->key_info[0].seg_offset[1] = 18; 

            /* ACL_IP6S ACL SIPv6_64 SA: */
            /* Same as ACL_SIP_SA for ACL_IP6S, with only SIP lookup change: */
            sal_memcpy(&ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ACL_SIPV6_64_SA],
                       &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ACL_SIP_SA],
                       sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6S_ACL_SIPV6_64_SA].ltr;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[1][1] = 0;
            ltr->partition[1][2] = 0;
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 54; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 18; 

            /* IP6F ACL, SIPV6, SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6F;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            ltr->key_info[0].seg_len[0] = 11;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 11;
            ltr->key_info[0].seg_offset[1] = 12; 
            ltr->key_info[0].seg_len[2] = 7;
            ltr->key_info[0].seg_offset[2] = 30; 
            ltr->key_info[0].seg_len[3] = 8;
            ltr->key_info[0].seg_offset[3] = 44; 
            ltr->key_info[0].seg_len[4] = 8;
            ltr->key_info[0].seg_offset[4] = 53; 
            ltr->key_info[1].seg_len[0] = 15;
            ltr->key_info[1].seg_offset[0] = 46; 
            ltr->key_info[1].seg_len[1] = 1;
            ltr->key_info[1].seg_offset[1] = 18; 
            ltr->key_info[2].seg_len[0] = 1;
            ltr->key_info[2].seg_offset[0] = 62; 
            ltr->key_info[2].seg_len[1] = 5;
            ltr->key_info[2].seg_offset[1] = 64; 

            /* IP6F DIPV6_64: Same as ACL_IP6S DIPV6_64 */

            /* ACL_IP6F ACL_SIPV6_64, SA */
            /* same as above with following minor changes: */
            sal_memcpy(&ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ACL_SIPV6_64_SA],
                        &ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ACL_SIP_SA],
                        sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ACL_SIPV6_64_SA].ltr;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6U;
            ltr->partition[1][1] = 0;
            ltr->partition[1][2] = 0;
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 54;

            /* IP6F ACL only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6F_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6F;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 15;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[2] = 15;
            ltr->key_info[0].seg_offset[2] = 38; 
            ltr->key_info[0].seg_len[3] = 1;
            ltr->key_info[0].seg_offset[3] = 54; 

            /* ACL IP6F L2fwd DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6F_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6F_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            ltr->key_info[0].seg_len[0] = 1;
            ltr->key_info[0].seg_offset[0] = 18; 
            ltr->key_info[0].seg_len[1] = 5;
            ltr->key_info[0].seg_offset[1] = 32; 

            /* IP6F l2 fwd ACL SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_IP6F_L2_ACL_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_IP6F_L2_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_IP6F;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->key_info[0].seg_len[0] = 15;
            ltr->key_info[0].seg_offset[0] = 0; 
            ltr->key_info[0].seg_len[1] = 15;
            ltr->key_info[0].seg_offset[1] = 16; 
            ltr->key_info[0].seg_len[2] = 15;
            ltr->key_info[0].seg_offset[2] = 38; 
            ltr->key_info[0].seg_len[3] = 1;
            ltr->key_info[0].seg_offset[3] = 54; 
            ltr->key_info[1].seg_len[0] = 7;
            ltr->key_info[1].seg_offset[0] = 56; 

            /* TR2 EXT_ACL_L2_IPV6 */
            /* DIPV6: same as EXT_ACL_FULL_IPV6 */

            /* ACL, SIP, SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_ACL_SIP_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_ACL_SIP_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2IP6;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_IP6_128_UCAST;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_IP6_128_UCAST_WIDE;
            ltr->partition[1][2] = TCAM_PARTITION_FWD_IP6;
            ltr->partition[2][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[2][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->sblk_kpu_select[2] = 2;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->parallel_srch[2] = 2;
            ltr->num_searches = 3;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            ltr->bmr_select[2] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 11;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 5;
                ltr->key_info[0].seg_offset[1] = 12;
                ltr->key_info[0].seg_len[2] = 4;
                ltr->key_info[0].seg_offset[2] = 19;
                ltr->key_info[0].seg_len[3] = 7;
                ltr->key_info[0].seg_offset[3] = 30; 
                ltr->key_info[0].seg_len[4] = 6;
                ltr->key_info[0].seg_offset[4] = 39; 
                ltr->key_info[0].seg_len[5] = 15;
                ltr->key_info[0].seg_offset[5] = 46; 
                ltr->key_info[0].seg_len[6] = 5;
                ltr->key_info[0].seg_offset[6] = 72; 
                ltr->key_info[1].seg_len[0] = 15;
                ltr->key_info[1].seg_offset[0] = 46; 
                ltr->key_info[1].seg_len[1] = 1;
                ltr->key_info[1].seg_offset[1] = 18; 
                ltr->key_info[2].seg_len[0] = 7;
                ltr->key_info[2].seg_offset[0] = 62; 
            } else {
                ltr->key_info[0].seg_len[0] = 11;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 11;
                ltr->key_info[0].seg_offset[1] = 12; 
                ltr->key_info[0].seg_len[2] = 9;
                ltr->key_info[0].seg_offset[2] = 30; 
                ltr->key_info[0].seg_len[3] = 9;
                ltr->key_info[0].seg_offset[3] = 40; 
                ltr->key_info[0].seg_len[4] = 5;
                ltr->key_info[0].seg_offset[4] = 52; 
                ltr->key_info[0].seg_len[5] = 5;
                ltr->key_info[0].seg_offset[5] = 60; 
                ltr->key_info[1].seg_len[0] = 15;
                ltr->key_info[1].seg_offset[0] = 34; 
                ltr->key_info[1].seg_len[1] = 1;
                ltr->key_info[1].seg_offset[1] = 18; 
                ltr->key_info[2].seg_len[0] = 7;
                ltr->key_info[2].seg_offset[0] = 50; 
            }
            /* ACL_L2IP6 only */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_ONLY], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_ONLY].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2IP6;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 11;
                ltr->key_info[0].seg_offset[0] = 0;
                ltr->key_info[0].seg_len[1] = 5;
                ltr->key_info[0].seg_offset[1] = 12;
                ltr->key_info[0].seg_len[2] = 4;
                ltr->key_info[0].seg_offset[2] = 19;
                ltr->key_info[0].seg_len[3] = 7;
                ltr->key_info[0].seg_offset[3] = 24; 
                ltr->key_info[0].seg_len[4] = 6;
                ltr->key_info[0].seg_offset[4] = 33; 
                ltr->key_info[0].seg_len[5] = 15;
                ltr->key_info[0].seg_offset[5] = 40; 
                ltr->key_info[0].seg_len[6] = 5;
                ltr->key_info[0].seg_offset[6] = 66; 
            } else {
                ltr->key_info[0].seg_len[0] = 15;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 0;
                ltr->key_info[0].seg_offset[1] = 16; 
                ltr->key_info[0].seg_len[2] = 9;
                ltr->key_info[0].seg_offset[2] = 18; 
                ltr->key_info[0].seg_len[3] = 15;
                ltr->key_info[0].seg_offset[3] = 28; 
                ltr->key_info[0].seg_len[4] = 5;
                ltr->key_info[0].seg_offset[4] = 46; 
                ltr->key_info[0].seg_len[5] = 5;
                ltr->key_info[0].seg_offset[5] = 54; 
            }

            /* ACL L2IP6 L2fwd DA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_L2_DA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_L2_DA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[0][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->parallel_srch[0] = 0;
            ltr->num_searches = 1;
            ltr->bmr_select[0] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 1;
                ltr->key_info[0].seg_offset[0] = 20; 
                ltr->key_info[0].seg_len[1] = 5;
                ltr->key_info[0].seg_offset[1] = 58;
            } else {
                ltr->key_info[0].seg_len[0] = 1;
                ltr->key_info[0].seg_offset[0] = 18; 
                ltr->key_info[0].seg_len[1] = 5;
                ltr->key_info[0].seg_offset[1] = 28; 
            }

            /* ACL_L2IP6 L2 fwd ACL+SA */
            sal_memset(&ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_L2_ACL_SA], 0, 
                            sizeof(ltr_profiles[0]));
            ltr = &ltr_profiles[TR3_LTR_LIB_ACL_L2IP6_L2_ACL_SA].ltr;
            ltr->partition[0][0] = TCAM_PARTITION_ACL_L2IP6;
            ltr->partition[1][0] = TCAM_PARTITION_FWD_L2;
            ltr->partition[1][1] = TCAM_PARTITION_FWD_L2_WIDE;
            ltr->sblk_kpu_select[0] = 0;
            ltr->sblk_kpu_select[1] = 1;
            ltr->parallel_srch[0] = 0;
            ltr->parallel_srch[1] = 1;
            ltr->num_searches = 2;
            ltr->bmr_select[0] = 0;
            ltr->bmr_select[1] = 0;
            if (soc_property_get(unit, spn_EXT_L2IP6_ACL_TABLE_MASTER_KEY_TYPE, 0) == 1) {
                ltr->key_info[0].seg_len[0] = 15;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 1;
                ltr->key_info[0].seg_offset[1] = 16;
                ltr->key_info[0].seg_len[2] = 0;
                ltr->key_info[0].seg_offset[2] = 19;
                ltr->key_info[0].seg_len[3] = 1;
                ltr->key_info[0].seg_offset[3] = 70; 
                ltr->key_info[0].seg_len[4] = 9;
                ltr->key_info[0].seg_offset[4] = 22;
                ltr->key_info[0].seg_len[5] = 6;
                ltr->key_info[0].seg_offset[5] = 33;
                ltr->key_info[0].seg_len[6] = 15;
                ltr->key_info[0].seg_offset[6] = 40;
                ltr->key_info[0].seg_len[7] = 5;
                ltr->key_info[0].seg_offset[7] = 58;
                ltr->key_info[1].seg_len[0] = 1;
                ltr->key_info[1].seg_offset[0] = 20;
                ltr->key_info[1].seg_len[1] = 5;
                ltr->key_info[1].seg_offset[1] = 72;
            } else { 
                ltr->key_info[0].seg_len[0] = 15;
                ltr->key_info[0].seg_offset[0] = 0; 
                ltr->key_info[0].seg_len[1] = 0;
                ltr->key_info[0].seg_offset[1] = 16; 
                ltr->key_info[0].seg_len[2] = 1;
                ltr->key_info[0].seg_offset[2] = 20; 
                ltr->key_info[0].seg_len[3] = 1;
                ltr->key_info[0].seg_offset[3] = 34; 
                ltr->key_info[0].seg_len[4] = 5;
                ltr->key_info[0].seg_offset[4] = 22; 
                ltr->key_info[0].seg_len[5] = 15;
                ltr->key_info[0].seg_offset[5] = 36; 
                ltr->key_info[0].seg_len[6] = 5;
                ltr->key_info[0].seg_offset[6] = 54; 
                ltr->key_info[0].seg_len[7] = 5;
                ltr->key_info[0].seg_offset[7] = 62; 
                ltr->key_info[1].seg_len[0] = 7;
                ltr->key_info[1].seg_offset[0] = 52; 
            }
        } 
    }
    LOG_CLI((BSL_META_U(unit,
                        "TR3 tcam init done\n")));

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

#endif /* BCM_TRIUMPH_SUPPORT */
