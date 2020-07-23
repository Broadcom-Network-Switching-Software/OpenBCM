/*! \file bcmdrd_pt.c
 *
 * BCMDRD PT routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_field.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/*******************************************************************************
 * Private functions
 */

/*
 * Get the pointer to bcmdrd_symbol_t struct from the specified symbol ID.
 * The bcmdrd_sym_info_t information will be provided as well
 * if the input argument 'sifno' is not NULL.
 */
static const bcmdrd_symbol_t *
symbol_get(int unit, bcmdrd_sid_t sid, bcmdrd_sym_info_t *sinfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    return bcmdrd_sym_info_get(symbols, sid, sinfo);
}

/*
 * Get symbol's pipe information.
 * The returned 'pmask' is non-zero (pipe mask) if the symbol is with
 * UNIQUE access type.
 * The returned 'linst' is non-zero (number of BASETYPE instances)
 * if the symbol is with BASETYPE defined.
 */
static void
pt_pipe_info_get(int unit, bcmdrd_sym_info_t *sinfo,
                 uint32_t *pmask, int *linst)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = sinfo->offset;
    pi->acctype = bcmdrd_pt_acctype_get(unit, sinfo->sid);
    pi->blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    if (pmask) {
        *pmask = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_PMASK);
    }
    if (linst) {
        *linst = bcmdrd_dev_pipe_info(unit, pi, BCMDRD_PIPE_INFO_TYPE_LINST);
    }
}

/*
 * Check whether the specified port number is valid for a port-based symbol.
 */
static bool
pt_port_valid(int unit, const bcmdrd_chip_info_t *cinfo,
              bcmdrd_sym_info_t *sinfo, int port, int idx)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    int blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    bcmdrd_port_num_domain_t pnd_pt, pnd_blk;

    pnd_pt = bcmdrd_chip_port_num_domain(cinfo, sinfo->sid, blktype);
    pnd_blk = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);

    /* Check if the port is valid for IPORT/EPORT base types for MMU blocks */
    if (pnd_pt == pnd_blk) {
        bcmdrd_pipe_info_type_t pi_type = BCMDRD_PIPE_INFO_TYPE_BASEIDX_INVALID;

        sal_memset(pi, 0, sizeof(*pi));
        pi->offset = sinfo->offset;
        pi->acctype = bcmdrd_pt_acctype_get(unit, sinfo->sid);
        pi->blktype = blktype;
        pi->baseidx = port;
        if (bcmdrd_dev_pipe_info(unit, pi, pi_type) != 0) {
            return false;
        }
    }
    /* The port_reg_valid function treats a negative table index as zero */
    return bcmdrd_chip_port_reg_valid(cinfo, sinfo->sid, port, idx);
}

/*
 * Check whether the specified base index is valid for a symbol associate with
 * BASETYPE.
 */
static bool
pt_single_acc_baseidx_valid(int unit, bcmdrd_sym_info_t *sinfo, int baseidx)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    uint32_t imask = 0, blk_pipe_bmp = 0;

    sal_memset(pi, 0, sizeof(*pi));
    pi->offset = sinfo->offset;
    pi->acctype = bcmdrd_pt_acctype_get(unit, sinfo->sid);
    pi->blktype = bcmdrd_pt_blktype_get(unit, sinfo->sid, 0);
    pi->baseidx = baseidx;
    imask = bcmdrd_dev_pipe_info(unit, pi,
                                 BCMDRD_PIPE_INFO_TYPE_BASETYPE_SINGLE_PMASK);
    if (imask != 0) {
        (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, pi->blktype,
                                             &blk_pipe_bmp);
        if ((imask & blk_pipe_bmp) == 0) {
            return false;
        }
    }
    return true;
}

/*
 * Check whether the specified index is valid.
 */
static bool
pt_index_valid(int unit, bcmdrd_sym_info_t *sinfo, int tbl_idx)
{
    bcmdrd_sid_t sid = sinfo->sid;

    if (tbl_idx > bcmdrd_pt_index_max(unit, sid) ||
        tbl_idx < bcmdrd_pt_index_min(unit, sid)) {
        return false;
    }
    if (sinfo->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        uint32_t invalid;
        bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

        sal_memset(pi, 0, sizeof(*pi));
        pi->acctype = bcmdrd_pt_acctype_get(unit, sid);
        pi->blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
        pi->memidx = tbl_idx;
        pi->memidx_max = sinfo->index_max;
        invalid = bcmdrd_dev_pipe_info(unit, pi,
                                       BCMDRD_PIPE_INFO_TYPE_MEMIDX_INVALID);
        return (invalid == 0);
    }
    return true;
}

/*
 * Check whether the specified symbol ID is valid to be accessed.
 */
static bool
pt_is_valid(const bcmdrd_chip_info_t *cinfo,
            const bcmdrd_symbol_t *symbol, bcmdrd_sid_t sid)
{
    /* Check for SKU override */
    if (!bcmdrd_chip_sym_valid(cinfo, sid)) {
        return false;
    }

    if ((symbol->flags &
         (BCMDRD_SYMBOL_FLAG_REGISTER | BCMDRD_SYMBOL_FLAG_MEMORY)) == 0) {
        return false;
    }

    return true;
}

/*******************************************************************************
 * Public functions
 */

int
bcmdrd_pt_info_get(int unit, bcmdrd_sid_t sid,
                   bcmdrd_sym_info_t *sinfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_info_get(symbols, sid, sinfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_info_get_by_name(int unit, const char *sname,
                           bcmdrd_sym_info_t *sinfo)
{
    const bcmdrd_symbols_t *symbols;
    bcmdrd_sid_t sid;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_symbols_find(sname, symbols, &sid) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    if (bcmdrd_sym_info_get(symbols, sid, sinfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_acctype_get(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_chip_info_t *cinfo;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    return bcmdrd_chip_acctype_get(cinfo, sid);
}

int
bcmdrd_pt_blktype_get(int unit, bcmdrd_sid_t sid, int blktype)
{
    const bcmdrd_chip_info_t *cinfo;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    return bcmdrd_chip_blktype_get(cinfo, sid, blktype);
}

int
bcmdrd_pt_index_max(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;
    int maxidx;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return -1;
    }

    maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);

    return bcmdrd_dev_mem_maxidx(unit, sid, maxidx);
}

int
bcmdrd_pt_index_min(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return -1;
    }

    return BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
}

uint32_t
bcmdrd_pt_index_count(int unit, bcmdrd_sid_t sid)
{
    int minidx, maxidx;

    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return 0;
    }

    minidx = bcmdrd_pt_index_min(unit, sid);
    maxidx = bcmdrd_pt_index_max(unit, sid);

    return (maxidx - minidx + 1);
}

bool
bcmdrd_pt_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int tbl_idx)
{
    int rv;
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    bool valid = true;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_info_get(symbols, sid, sinfo);
    if (symbol == NULL) {
        return false;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return false;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return false;
    }

    /* First check the exceptions in device-specific index valid function */
    rv = bcmdrd_dev_pt_index_valid(unit, sid, tbl_inst, tbl_idx, &valid);
    if (SHR_SUCCESS(rv)) {
        return valid;
    }

    /* Skip check if table index is negative */
    if (tbl_idx >= 0) {
        if (!pt_index_valid(unit, sinfo, tbl_idx)) {
            return false;
        }
    }

    /* Skip check if table instance is negative */
    if (tbl_inst >= 0) {
        uint32_t pmask = 0;
        int linst = 0;

        if (symbol->flags & BCMDRD_SYMBOL_FLAG_ACC_UNIQUE_PIPE) {
            int pipe = bcmdrd_pt_acctype_get(unit, sid);
            int blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
            uint32_t blk_pipe_bmp = 0;

            (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype,
                                                 &blk_pipe_bmp);
            if (pipe < 0 || ((1 << pipe) & blk_pipe_bmp) == 0) {
                return false;
            }
        }

        pt_pipe_info_get(unit, sinfo, &pmask, &linst);
        if (pmask != 0) {
            uint32_t sub_pipe_map = 0;
            /* Check for holes in per-pipe PTs */
            if ((pmask & (1 << tbl_inst)) == 0) {
                return false;
            }
            rv = bcmdrd_pt_sub_pipe_map(unit, sid, tbl_inst, &sub_pipe_map);
            if (SHR_SUCCESS(rv) && sub_pipe_map == 0) {
                return false;
            }
        } else if (bcmdrd_pt_is_port_reg(unit, sid)) {
            return pt_port_valid(unit, cinfo, sinfo, tbl_inst, tbl_idx);
        } else if (bcmdrd_pt_is_soft_port_reg(unit, sid)) {
            int blktype;
            const bcmdrd_block_t *blkp;
            bcmdrd_pbmp_t pbmp;
            bcmdrd_port_num_domain_t pnd;

            blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
            blkp = bcmdrd_chip_block(cinfo, blktype, tbl_inst);
            if (blkp == NULL) {
                return false;
            }
            /* Check for holes in soft-port-based PTs */
            BCMDRD_PBMP_ASSIGN(pbmp, blkp->pbmps);
            pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
            BCMDRD_PBMP_AND(pbmp, cinfo->valid_pbmps[pnd]);
            if (BCMDRD_PBMP_IS_NULL(pbmp)) {
                return false;
            }
        } else if (linst != 0) {
            if (tbl_inst >= linst ||
                !pt_single_acc_baseidx_valid(unit, sinfo, tbl_inst)) {
                return false;
            }
        }
    }

    return true;
}

bcmdrd_index_class_t
bcmdrd_pt_index_class(int unit, bcmdrd_sid_t sid, int tbl_inst,
                      int tbl_idx)
{
    if (!bcmdrd_pt_is_valid(unit, sid)) {
        return BCMDRD_IC_INVALID;
    }

    if (!bcmdrd_pt_index_valid(unit, sid, tbl_inst, tbl_idx)) {
        if (bcmdrd_dev_is_variant_sku(unit)) {
            return BCMDRD_IC_IGNORE;
        }

        return BCMDRD_IC_INVALID;
    }

    return BCMDRD_IC_VALID;
}

uint32_t
bcmdrd_pt_entry_size(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return 0;
    }

    return BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
}

uint32_t
bcmdrd_pt_entry_wsize(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;
    uint32_t size;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return 0;
    }

    size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
    return BCMDRD_BYTES2WORDS(size);
}

bcmdrd_port_num_domain_t
bcmdrd_pt_port_num_domain(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_chip_info_t *cinfo;
    int blktype;

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_PARAM;
    }

    return bcmdrd_chip_port_num_domain(cinfo, sid, blktype);
}

int
bcmdrd_pt_name_to_sid(int unit, const char *name, bcmdrd_sid_t *sid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_INIT;
    }
    symbol = bcmdrd_symbols_find(name, symbols, sid);
    if (symbol == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

const char *
bcmdrd_pt_sid_to_name(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    return symbols->symbols[sid].name;
}

int
bcmdrd_pt_sid_sect_list_get(int unit, int sect, size_t list_max,
                            bcmdrd_sid_t *sid_list, size_t *num_sid)
{
    const bcmdrd_symbols_t *symbols;
    uint32_t offs, size, idx;
    int sdx;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return SHR_E_INIT;
    }
    offs = 0;
    size = 0;
    for (sdx = 0; sdx < COUNTOF(symbols->sect_size); sdx++) {
        if (sdx == sect) {
            size = symbols->sect_size[sdx];
            break;
        }
        offs += symbols->sect_size[sdx];
    }
    if (size == 0) {
        offs = 0;
        size = symbols->size;
    }
    *num_sid = size;
    if (list_max == 0) {
        return SHR_E_NONE;
    }
    if (sid_list == NULL) {
        return SHR_E_PARAM;
    }
    for (idx = 0; idx < size; idx++) {
        if (idx >= list_max) {
            break;
        }
        sid_list[idx] = offs + idx;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_sid_list_get(int unit, size_t list_max,
                       bcmdrd_sid_t *sid_list, size_t *num_sid)
{
    return bcmdrd_pt_sid_sect_list_get(unit, -1, list_max, sid_list, num_sid);
}

const uint32_t *
bcmdrd_pt_default_value_get(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return NULL;
    }
    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return NULL;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        return bcmdrd_sym_reg_resetval_get(symbols, symbol);
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        return bcmdrd_sym_mem_null_ent_get(symbols, symbol);
    }
    return NULL;
}

bool
bcmdrd_pt_default_value_is_nonzero(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols = bcmdrd_dev_symbols_get(unit, 0);
    const bcmdrd_symbol_t *symbol = bcmdrd_sym_info_get(symbols, sid, NULL);

    return bcmdrd_sym_default_value_is_nonzero(symbols, symbol);
}

bool
bcmdrd_pt_is_valid(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_chip_info_t *cinfo;
    const bcmdrd_symbol_t *symbol;

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return false;
    }
    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_OVERLAY) == 0);
}

bool
bcmdrd_pt_is_reg(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) != 0);
}

bool
bcmdrd_pt_is_mem(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) != 0);
}

bool
bcmdrd_pt_is_port_reg(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_PORT) != 0);
}

bool
bcmdrd_pt_is_soft_port_reg(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_SOFT_PORT) != 0);
}

bool
bcmdrd_pt_is_readonly(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_READONLY) != 0);
}

bool
bcmdrd_pt_is_writeonly(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_WRITEONLY) != 0);
}

bool
bcmdrd_pt_is_notest(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_NOTEST) != 0);
}

bool
bcmdrd_pt_is_counter(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_COUNTER) != 0);
}

bool
bcmdrd_pt_is_cacheable(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_CACHEABLE) != 0);
}

bool
bcmdrd_pt_attr_is_cam(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_CAM) != 0);
}

bool
bcmdrd_pt_attr_is_hashed(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_HASHED) != 0);
}

bool
bcmdrd_pt_attr_is_fifo(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    return ((symbol->flags & BCMDRD_SYMBOL_FLAG_FIFO) != 0);
}

int
bcmdrd_pt_num_tbl_inst(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    int tbl_num = 0;
    int linst = 0;
    uint32_t pmask = 0;

    symbol = symbol_get(unit, sid, sinfo);
    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_PARAM;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return 0;
    }

    /* If access type is UNIQUE, tbl_num is the number of unique pipes */
    pt_pipe_info_get(unit, sinfo, &pmask, &linst);
    while (pmask) {
        tbl_num++;
        pmask >>= 1;
    }

    /*
     * Check for port-based registers if access type
     * is not UNIQUE (tbl_num == 0)
     */
    if (tbl_num == 0 && bcmdrd_pt_is_port_reg(unit, sid)) {
        int port;
        bcmdrd_pbmp_t pbmps;

        if (bcmdrd_chip_port_reg_pbmp(cinfo, sid, &pbmps) == 0) {
            /* Get maximum port number from pbmp */
            if (!BCMDRD_PBMP_IS_NULL(pbmps)) {
                for (port = BCMDRD_CONFIG_MAX_PORTS - 1; port >= 0; port--) {
                    if (BCMDRD_PBMP_MEMBER(pbmps, port)) {
                        tbl_num = port + 1;
                        break;
                    }
                }
            }
        }
    }

    /* Check for per-block symbols if tbl_num == 0 */
    if (tbl_num == 0 && bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        int blktype, idx;

        /* Soft-port PTs should have only one block type */
        if ((blktype = bcmdrd_pt_blktype_get(unit, sid, 0)) >= 0) {
            bcmdrd_pbmp_t blktype_pbmps;

            /* Get the valid port bitmap of this block type. */
            bcmdrd_chip_blktype_pbmp(cinfo, blktype, &blktype_pbmps);

            /* Start from the back */
            for (idx = cinfo->nblocks - 1; idx >= 0; idx--) {
                const bcmdrd_block_t *blkp = &cinfo->blocks[idx];

                if (blkp->type != blktype) {
                    continue;
                }

                /* Skip trailing block instances with empty port bitmap */
                if (tbl_num == 0) {
                    bcmdrd_pbmp_t pbmps;

                    /* Check the block port bitmap with the valid port bitmap */
                    BCMDRD_PBMP_ASSIGN(pbmps, blktype_pbmps);
                    BCMDRD_PBMP_AND(pbmps, blkp->pbmps);
                    if (BCMDRD_PBMP_IS_NULL(pbmps)) {
                        continue;
                    }
                }
                tbl_num++;
            }
        }
    }

    /* Check for basetype instances if tbl_num == 0 */
    if (tbl_num == 0 && linst > 1) {
        tbl_num = linst;
    }

    return ((tbl_num == 0) ? 1 : tbl_num);
}

int
bcmdrd_pt_tbl_inst_from_port(int unit, bcmdrd_sid_t sid, int port)
{
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    int tbl_inst = -1;

    symbol = symbol_get(unit, sid, sinfo);
    if (symbol == NULL) {
        return -1;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return -1;
    }

    /* Check if the port is valid */
    if (!pt_port_valid(unit, cinfo, sinfo, port, -1)) {
        return -1;
    }

    if (bcmdrd_pt_is_port_reg(unit, sid)) {
        tbl_inst = port;
    } else if (bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        bcmdrd_pblk_t drd_pblk, *pblk = &drd_pblk;
        int blktype;

        sal_memset(pblk, 0, sizeof(*pblk));

        if ((blktype = bcmdrd_pt_blktype_get(unit, sid, 0)) >= 0) {
            if (bcmdrd_chip_port_block(cinfo, port, blktype, pblk) == 0) {
                tbl_inst = pblk->inst;
            }
        }
    }

    /* Check the exceptions in device-specific index valid function */
    if (tbl_inst >= 0) {
        int rv;
        bool valid = true;

        rv = bcmdrd_dev_pt_index_valid(unit, sid, tbl_inst, -1, &valid);
        if (SHR_SUCCESS(rv) && !valid) {
            tbl_inst = -1;
        }
    }

    return tbl_inst;
}

int
bcmdrd_pt_port_from_tbl_inst(int unit, bcmdrd_sid_t sid, int tbl_inst)
{
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    int port = -1;

    symbol = symbol_get(unit, sid, sinfo);
    if (symbol == NULL) {
        return -1;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return -1;
    }

    if (bcmdrd_pt_is_port_reg(unit, sid)) {
        /* Table instance is the port number. */
        if (pt_port_valid(unit, cinfo, sinfo, tbl_inst, -1)) {
            port = tbl_inst;
        }
    } else if (bcmdrd_pt_is_soft_port_reg(unit, sid)) {
        int blktype = -1, p;
        const bcmdrd_block_t *blkp;

        while ((blktype = bcmdrd_pt_blktype_get(unit, sid, blktype + 1)) >= 0) {
            blkp = bcmdrd_chip_block(cinfo, blktype, tbl_inst);
            if (blkp == NULL) {
                continue;
            }
            BCMDRD_PBMP_ITER(blkp->pbmps, p) {
                port = p;
                break;
            }
            if (port >= 0) {
                break;
            }
        }
    }

    return port;
}

int
bcmdrd_pt_port_number_get(int unit, bcmdrd_sid_t sid, int blknum, int lane)
{
    int btype = -1;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    bcmdrd_port_num_domain_t pnd;

    symbol = symbol_get(unit, sid, sinfo);
    if (symbol == NULL) {
        return -1;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return -1;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return -1;
    }

    if (bcmdrd_chip_block_type(cinfo, blknum, &btype, NULL) < 0) {
        return -1;
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, sid, btype);
    if (pnd != bcmdrd_chip_port_num_domain(cinfo, -1, btype)) {
        if (pt_port_valid(unit, cinfo, sinfo, lane, -1)) {
            return lane;
        }
        return -1;
    }
    return bcmdrd_chip_port_number(cinfo, blknum, lane);
}

int
bcmdrd_pt_port_block_lane_get(int unit, bcmdrd_sid_t sid, int port,
                              int *blknum, int *lane)
{
    int btype = -1;
    const bcmdrd_symbol_t *symbol;
    const bcmdrd_chip_info_t *cinfo;
    bcmdrd_sym_info_t sym_info, *sinfo = &sym_info;
    bcmdrd_pblk_t drd_pblk, *pblk = &drd_pblk;
    bcmdrd_port_num_domain_t pnd;

    symbol = symbol_get(unit, sid, sinfo);
    if (symbol == NULL) {
        return SHR_E_NOT_FOUND;
    }

    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (cinfo == NULL) {
        return SHR_E_UNIT;
    }

    if (!pt_is_valid(cinfo, symbol, sid)) {
        return SHR_E_PARAM;
    }

    sal_memset(pblk, 0, sizeof(*pblk));

    while ((btype = bcmdrd_pt_blktype_get(unit, sid, btype + 1)) >= 0) {
        if (bcmdrd_chip_port_block(cinfo, port, btype, pblk) == 0) {
            break;
        }
    }
    if (btype < 0) {
        return SHR_E_PORT;
    }

    if (blknum != NULL) {
        *blknum = bcmdrd_chip_block_number(cinfo, btype, pblk->inst);
    }
    if (lane != NULL) {
        pnd = bcmdrd_chip_port_num_domain(cinfo, sid, btype);
        if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, btype)) {
            *lane = pblk->lane;
        } else {
            *lane = port;
        }
    }

    return SHR_E_NONE;
}

int
bcmdrd_pt_num_pipe_inst(int unit, bcmdrd_sid_t sid)
{
    int pipe_inst;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    if (!bcmdrd_dev_exists(unit)) {
        return -1;
    }

    sal_memset(pi, 0, sizeof(*pi));
    pi->blktype = bcmdrd_pt_blktype_get(unit, sid, 0);

    pipe_inst = bcmdrd_dev_pipe_info(unit, pi,
                                     BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);

    return (pipe_inst == 0) ? 1 : pipe_inst;
}

int
bcmdrd_pt_sub_pipe_map(int unit, bcmdrd_sid_t sid, int tbl_inst,
                       uint32_t *sub_pipe_map)
{
    const bcmdrd_symbol_t *symbol;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    if (sub_pipe_map == NULL || tbl_inst < 0) {
        return SHR_E_PARAM;
    }

    symbol = symbol_get(unit, sid, NULL);
    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    if ((symbol->flags & BCMDRD_SYMBOL_FLAG_SUB_PIPE) == 0) {
        return SHR_E_UNAVAIL;
    }

    sal_memset(pi, 0, sizeof(*pi));
    pi->pipe_inst = tbl_inst;
    pi->sub_pipe_inst = BCMDRD_SYM_INFO_SUBPIPE_INST(symbol->info);
    pi->blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    *sub_pipe_map = bcmdrd_dev_pipe_info(unit, pi,
                                         BCMDRD_PIPE_INFO_TYPE_SUB_PIPE_MAP);
    return SHR_E_NONE;
}

int
bcmdrd_pt_fid_list_get(int unit, bcmdrd_sid_t sid, size_t list_max,
                       bcmdrd_fid_t *fid_list, size_t *num_fid)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_fid_list_get(symbols, sid, list_max, fid_list, num_fid) < 0) {
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_field_info_get(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid,
                         bcmdrd_sym_field_info_t *finfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_field_info_get(symbols, sid, fid, finfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

int
bcmdrd_pt_field_info_get_by_name(int unit, bcmdrd_sid_t sid, const char *fname,
                                 bcmdrd_sym_field_info_t *finfo)
{
    const bcmdrd_symbols_t *symbols;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (bcmdrd_sym_field_info_get_by_name(symbols, sid, fname, finfo) == NULL) {
        return SHR_E_NOT_FOUND;
    }
    return SHR_E_NONE;
}

bool
bcmdrd_pt_field_is_readonly(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    
    return false;
}

int
bcmdrd_pt_field_name_to_fid(int unit, bcmdrd_sid_t sid,
                            const char *fname, bcmdrd_fid_t *fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get_by_name(unit, sid, fname, &finfo);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    *fid = finfo.fid;
    return SHR_E_NONE;
}

int
bcmdrd_pt_field_maxbit(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return -1;
    }
    return finfo.maxbit;
}

int
bcmdrd_pt_field_minbit(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return -1;
    }
    return finfo.minbit;
}

bool
bcmdrd_pt_field_is_counter(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return false;
    }
    return ((finfo.flags & BCMDRD_SYMBOL_FIELD_FLAG_COUNTER) != 0);
}

bool
bcmdrd_pt_field_is_key(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return false;
    }
    return ((finfo.flags & BCMDRD_SYMBOL_FIELD_FLAG_KEY) != 0);
}

bool
bcmdrd_pt_field_is_mask(int unit, bcmdrd_sid_t sid, bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t finfo;

    rv = bcmdrd_pt_field_info_get(unit, sid, fid, &finfo);
    if (SHR_FAILURE(rv)) {
        return false;
    }
    return ((finfo.flags & BCMDRD_SYMBOL_FIELD_FLAG_MASK) != 0);
}

int
bcmdrd_pt_field_get(int unit, bcmdrd_sid_t sid, uint32_t *sbuf,
                    bcmdrd_fid_t fid, uint32_t *fbuf)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    uint32_t size;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_field_info_get(symbols, sid, fid, &finfo);
    if (symbol == NULL) {
        return -1;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
        bcmdrd_field_be_get(sbuf, BCMDRD_BYTES2WORDS(size),
                            finfo.minbit, finfo.maxbit, fbuf);
    } else {
        bcmdrd_field_get(sbuf, finfo.minbit, finfo.maxbit, fbuf);
    }
    return 0;
}

int
bcmdrd_pt_field_set(int unit, bcmdrd_sid_t sid, uint32_t *sbuf,
                    bcmdrd_fid_t fid, uint32_t *fbuf)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;
    bcmdrd_sym_field_info_t finfo;
    uint32_t size;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_field_info_get(symbols, sid, fid, &finfo);
    if (symbol == NULL) {
        return -1;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_BIG_ENDIAN) {
        size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
        bcmdrd_field_be_set(sbuf, BCMDRD_BYTES2WORDS(size),
                            finfo.minbit, finfo.maxbit, fbuf);
    } else {
        bcmdrd_field_set(sbuf, finfo.minbit, finfo.maxbit, fbuf);
    }
    return 0;
}

#endif
