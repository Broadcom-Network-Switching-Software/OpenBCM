/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: diag_framer_symbols.h
 * Purpose: framer symbol definations
 */

#ifndef _DIAG_DNX_FRAMER_SYMBOLS_H
#define _DIAG_DNX_FRAMER_SYMBOLS_H

#include <sal/types.h>

#ifndef NO_FRAMER_LIB_BUILD
#include <lib_sal.h>
#include <lib_hal.h>

#define FRAMER_SYMBOL_NAME_MAX_STR 256
#define FRAMER_SYMBOL_FLD_NAME_MAX_STR 64
#define FRAMER_SYMBOL_FLD_WIDTH_MAX (128)
#define FRAMER_SYMBOL_DWORD_MAX      (320 * 2)

#define FRAMER_REGSFILE_F_REG        (0x1 << 0)
#define FRAMER_REGSFILE_F_MEM        (0x1 << 1)
#define FRAMER_REGSFILE_F_FMT        (0x1 << 2)
#define FRAMER_REGSFILE_F_RAW        (0x1 << 3)
#define FRAMER_REGSFILE_F_CHG        (0x1 << 4)
#define FRAMER_REGSFILE_F_HEX        (0x1 << 5)
#define FRAMER_REGSFILE_F_CTR        (0x1 << 6)
#define FRAMER_REGSFILE_F_ALM        (0x1 << 7)
#define FRAMER_REGSFILE_F_DBG        (0x1 << 8)
#define FRAMER_REGSFILE_F_PFX        (0x1 << 9)

#define FRAMER_REGSFILE_F_EXT        (0x1 << 16)
#define FRAMER_REGSFILE_F_EM         (0x1 << 17)

#define FRAMER_SYMBOL_F_REG          (0x1 << 0)
#define FRAMER_SYMBOL_F_MEM          (0x1 << 1)
#define FRAMER_SYMBOL_F_ARRAY        (0x1 << 2)
#define FRAMER_SYMBOL_F_WIDTH        (0x1 << 3)
#define FRAMER_SYMBOL_F_INSTANCE     (0x1 << 4)

#define FRAMER_SYMBOL_F_INIT_ON_READ (0x1 << 10)
#define FRAMER_SYMBOL_F_INTR         (0x1 << 11)
#define FRAMER_SYMBOL_F_RO           (0x1 << 12)
#define FRAMER_SYMBOL_F_WO           (0x1 << 13)
#define FRAMER_SYMBOL_F_COUNTER      (0x1 << 14)
#define FRAMER_SYMBOL_F_RST          (0x1 << 15)
#define FRAMER_SYMBOL_F_ALARM        (0x1 << 16)

#define FRAMER_FILTER_DBG            (0x1 << 0)
#define FRAMER_FILTER_CONT_ON_ERR    (0x1 << 1)
#define FRAMER_FILTER_LOOP           (0x1 << 2)

#define FRAMER_SYMBOL_FIELD_F_DEFAULT_VALUE          (0x1 << 0)
#define FRAMER_SYMBOL_FIELD_F_DBG                    (0x1 << 1)

#define FRAMER_SYMBOL_M_SET(_value, m) \
        do { \
            (_value) |= (m) << 16; \
        } while (0)
#define FRAMER_SYMBOL_M_GET(_value, m) \
        do { \
            (m) = ((_value) >> 16) & 0xffff; \
        } while (0)
#define FRAMER_SYMBOL_W_SET(_value, w) \
        do { \
            (_value) |= (w) <<  0; \
        } while (0)
#define FRAMER_SYMBOL_W_GET(_value, w) \
        do { \
            (w) = ((_value) >>  0) & 0xffff; \
        } while (0)

#define FRAMER_SYMBOL_PARSER_F_BLOCK          (0x1 << 0)
#define FRAMER_SYMBOL_PARSER_F_ARRAY          (0x1 << 1)
#define FRAMER_SYMBOL_PARSER_F_BASE_ADDR      (0x1 << 2)
#define FRAMER_SYMBOL_PARSER_F_OFFSET         (0x1 << 3)
#define FRAMER_SYMBOL_PARSER_F_INDEX          (0x1 << 4)
#define FRAMER_SYMBOL_PARSER_F_EXACT_MATCH    (0x1 << 5)

typedef enum framer_blk_e
{
    FRAMER_BLK_FLFR,
    FRAMER_BLK_FLFT,
    FRAMER_BLK_FFOA,
    FRAMER_BLK_FFOB,
    FRAMER_BLK_FDMX,
    FRAMER_BLK_FAWR,
    FRAMER_BLK_FAWT,
    FRAMER_BLK_FBSW,
    FRAMER_BLK_FOSW,
    FRAMER_BLK_FPMR,
    FRAMER_BLK_FPMT,
    FRAMER_BLK_FLOTN,
    FRAMER_BLK_FFLXO,
    FRAMER_BLK_FDMXC,
    FRAMER_BLK_FDMXBI,
    FRAMER_BLK_FDMXA,
    FRAMER_BLK_FDMXT,
    FRAMER_BLK_FMXC,
    FRAMER_BLK_FMXBI,
    FRAMER_BLK_FMXBJ,
    FRAMER_BLK_FMXAI,
    FRAMER_BLK_FMXT,
    FRAMER_BLK_FMX,
    FRAMER_BLK_FRA,
    FRAMER_BLK_FMAP,
    FRAMER_BLK_FDMP,
    FRAMER_BLK_FOAM,
    FRAMER_BLK_FSAR,
    FRAMER_BLK_FGMAP,
    FRAMER_BLK_FMACR,
    FRAMER_BLK_FGDMP,
    FRAMER_BLK_FMACT,
    FRAMER_BLK_FECRL,
    FRAMER_BLK_FDCRL,
    FRAMER_BLK_FECRS,
    FRAMER_BLK_FDCRS,
    FRAMER_BLK_FPTPR,
    FRAMER_BLK_FPTPT,
    FRAMER_BLK_FODUO,
    FRAMER_BLK_FMXAJ,
    FRAMER_BLK_FMXBK,
    FRAMER_BLK_FDMXBJ,
    FRAMER_BLK_FECPB,
    FRAMER_BLK_FICPB,
    FRAMER_BLK_COUNT
} framer_blk_t;

#define FRAMER_BLK_NAME_RAW "UNKNOWN"

#define FRAMER_BLK_NAME_STR { \
    "FLFR", \
    "FLFT", \
    "FFOA", \
    "FFOB", \
    "FDMX", \
    "FAWR", \
    "FAWT", \
    "FBSW", \
    "FOSW", \
    "FPMR", \
    "FPMT", \
    "FLOTN", \
    "FFLXO", \
    "FDMXC", \
    "FDMXBI", \
    "FDMXA", \
    "FDMXT", \
    "FMXC", \
    "FMXBI", \
    "FMXBJ", \
    "FMXAI", \
    "FMXT", \
    "FMX", \
    "FRA", \
    "FMAP", \
    "FDMP", \
    "FOAM", \
    "FSAR", \
    "FGMAP", \
    "FMACR", \
    "FGDMP", \
    "FMACT", \
    "FECRL", \
    "FDCRL", \
    "FECRS", \
    "FDCRS", \
    "FPTPR", \
    "FPTPT", \
    "FODUO", \
    "FMXAJ", \
    "FMXBK", \
    "FDMXBJ", \
    "FECPB", \
    "FICPB", \
    FRAMER_BLK_NAME_RAW \
}

#define FRAMER_AER_REG(_addr)        ((_addr) & 0x100000)
#define FRAMER_BLK_ID_GET(_addr)     (((_addr) & 0xfc000) >> 14)

typedef int (
    *framer_symbol_blk_cb_f) (
    int unit,
    uint32 flags,
    int blk_id,
    int blk,
    int blk_no,
    int blk_num,
    int schan_num,
    void *user_data);

extern int framer_symbol_blk_invalid(
    int unit,
    int blk_id);

extern int framer_symbol_blk_iter(
    int unit,
    uint32 flags,
    framer_symbol_blk_cb_f cb,
    void *user_data);

extern int framer_symbol_blk_name2id(
    int unit,
    char *blk_name,
    int *blk,
    int *blk_no);

extern int framer_symbol_blk_num2name_id(
    int unit,
    int blk_num,
    char **blk_name,
    int *blk,
    int *blk_no);

extern int framer_symbol_array2index(
    int unit,
    char *array,
    int *index);

extern int framer_symbol_str2data(
    int unit,
    char *str,
    int data_count,
    uint32 *data);

typedef struct framer_symbol_inst_s
{
    int inst;
    int inst_max;
    int inst_per_blk;
} framer_symbol_inst_t;

typedef struct framer_symbol_s
{
    char sname[FRAMER_SYMBOL_NAME_MAX_STR];
    char alias[FRAMER_SYMBOL_NAME_MAX_STR];
    uint32 sname_len;
    uint32 flags;
    uint32 base_addr;
    uint32 offset;
    int blk;
    int blk_no;
    int index;
    int last;
    REG_INFO_S **sinfo;
    int count;
    int depth;
    int width;
    framer_symbol_inst_t *inst;
} framer_symbol_t;

typedef struct framer_symbol_field_modify_ud_s
{
    struct modify_param_s
    {
        char *fld;
        char *data;
    }  *param;
    int count;
    void *cookie;
} framer_symbol_field_modify_ud_t;

typedef struct framer_symbol_field_print_ud_s
{
    char *pfx;
    char *sfx;
    int dw;
    uint32 *data;
} framer_symbol_field_print_ud_t;

typedef struct framer_symbol_field_s
{
    char fldname[FRAMER_SYMBOL_FLD_NAME_MAX_STR];
    int lsb;
    int msb;
    int rcnt;
    uint32 reset_value[FRAMER_SYMBOL_FLD_WIDTH_MAX];
} framer_symbol_field_t;

typedef struct framer_symbol_parser_s
{
    char sname[FRAMER_SYMBOL_NAME_MAX_STR];
    uint32 flags;
    int aidx;
    int aidx_max;
    int blk;
    int blk_no;
    uint32 base_addr;
    uint32 offset;
    uint32 index;
} framer_symbol_parser_t;

typedef int (
    *framer_symbol_cb_f) (
    int unit,
    uint32 flags,
    framer_symbol_t * symbol,
    void *user_data);

typedef int (
    *framer_symbol_field_cb_f) (
    int unit,
    uint32 flags,
    framer_symbol_t * symbol,
    framer_symbol_field_t * field,
    void *user_data);

extern int framer_symbol_name2sym_parser(
    int unit,
    uint32 flags,
    char *name,
    framer_symbol_parser_t * sym_parse);

extern int framer_symbol_mask(
    int unit,
    framer_symbol_t * sym,
    int count,
    uint32 *mask);

extern int framer_symbol_field_modify(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_field_t * fld,
    void *user_data);

extern int framer_symbol_field_print(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_field_t * fld,
    void *user_data);

extern int framer_symbol_field_iter(
    int unit,
    uint32 flags,
    framer_symbol_t * sym,
    framer_symbol_field_cb_f cb,
    void *user_data);

extern int framer_symbol_iter(
    int unit,
    uint32 flags,
    framer_symbol_cb_f cb,
    void *user_data);

typedef struct framer_symbol_stat_s
{
    uint32 success;
    uint32 failure;
    uint32 skipped;
} framer_symbol_stat_t;

typedef struct framer_symbol_filter_s
{
    char *name;
    uint32 flags;
    framer_symbol_cb_f cb;
    void *user_data;
    char *summary;
    framer_symbol_stat_t stat;
} framer_symbol_filter_t;

extern void framer_symbol_filter_t_init(
    framer_symbol_filter_t * filter);

extern int framer_symbol_filter(
    int unit,
    uint32 flags,
    framer_symbol_filter_t * filter);
#endif /* NO_FRAMER_LIB_BUILD */
#endif /* _DIAG_DNX_FRAMER_SYMBOLS_H */
