/*! \file bcmdrd_chip.c
 *
 * DRD APIs that operate on the chip_info structure.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_chip.h>

/*
 * Get port index within a block from the top level port index.
 */
static int
port_block_lane_get(const bcmdrd_block_t *blkp,
                    bcmdrd_port_num_domain_t pnd, int port)
{
    int p, lane = 0;

    if (pnd != BCMDRD_PND_PHYS) {
        /* Assign port index to lane for non-physical ports */
        if (BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            return port;
        }
        return -1;
    }

    /*
     * To get the correct lane index, the block port bitmap for physical ports
     * should contain no holes.
     */
    BCMDRD_PBMP_ITER(blkp->pbmps, p) {
        if (p == port) {
            return lane;
        }
        lane++;
    }
    return -1;
}

bcmdrd_chip_mod_t *
bcmdrd_chip_mod_get(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid)
{
    bcmdrd_chip_profile_t *cp;
    bcmdrd_chip_mod_t *mod;

    if (cinfo == NULL) {
        return NULL;
    }

    cp = cinfo->profile;
    if (cp == NULL || cp->mods == NULL) {
        return NULL;
    }

    mod = cp->mods;
    while (mod->sid != BCMDRD_INVALID_ID) {
        if (sid == mod->sid) {
            return mod;
        }
        mod++;
    }

    return NULL;
}

int
bcmdrd_chip_acctype_get(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid)
{
    const bcmdrd_symbol_t *symbol;

    if (cinfo == NULL) {
        return -1;
    }

    symbol = bcmdrd_sym_info_get(cinfo->symbols, sid, NULL);
    if (symbol == NULL) {
        return -1;
    }

    return BCMDRD_SYM_INFO_ACCTYPE(symbol->info);
}

int
bcmdrd_chip_blktype_get(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid,
                        int blktype)
{
    const bcmdrd_symbol_t *symbol;
    int idx, bt;

    if (cinfo == NULL) {
        return -1;
    }

    symbol = bcmdrd_sym_info_get(cinfo->symbols, sid, NULL);
    if (symbol == NULL) {
        return -1;
    }

    if (blktype == 0) {
        bt = BCMDRD_SYM_INFO_BLKTYPE(symbol->info, 0);
    } else {
        idx = 0;
        while ((bt = BCMDRD_SYM_INFO_BLKTYPE(symbol->info, idx++)) > 0) {
            if (blktype == bt + 1) {
                bt = BCMDRD_SYM_INFO_BLKTYPE(symbol->info, idx);
                break;
            }
        }
    }
    return (bt > 0) ? bt : -1;
}

int
bcmdrd_chip_blktype_get_by_name(const bcmdrd_chip_info_t *cinfo,
                                const char *name)
{
    int idx;

    if (cinfo == NULL || name == NULL) {
        return -1;
    }

    for (idx = 0; idx < cinfo->nblktypes; idx++) {
        if (sal_strcasecmp(cinfo->blktype_names[idx], name) == 0) {
            return idx;
        }
    }
    return -1;
}

const bcmdrd_block_t *
bcmdrd_chip_block_types_match(const bcmdrd_chip_info_t *cinfo,
                              uint32_t info, const bcmdrd_block_t *pblk)
{
    const bcmdrd_block_t *pbs, *pbe;
    int bt, idx;

    if (cinfo == NULL || cinfo->blocks == NULL) {
        return NULL;
    }
    pbs = cinfo->blocks;
    pbe = cinfo->blocks + cinfo->nblocks;

    if (pblk == NULL) {
        pblk = pbs;
    }

    while (pblk >= pbs && pblk < pbe) {
        idx = 0;
        while ((bt = BCMDRD_SYM_INFO_BLKTYPE(info, idx++)) > 0) {
            if (bt == pblk->type) {
                return pblk;
            }
        }
        pblk++;
    }
    return NULL;
}

int
bcmdrd_chip_port_block(const bcmdrd_chip_info_t *cinfo,
                       int port, int blktype, bcmdrd_pblk_t *pblk)
{
    /* Get the block association for a port number */
    int idx, start_idx;
    int lane, inst = 0;
    bcmdrd_port_num_domain_t pnd;

    if (blktype < 0) {
        return -1;
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return -1;
    }

    /* Foreach block in the chip */
    start_idx = cinfo->blktype_start_idx[blktype];
    for (idx = start_idx; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = &cinfo->blocks[idx];

        if (blkp->type != blktype) {
            continue;
        }

        if (BCMDRD_PBMP_MEMBER(blkp->pbmps, port)) {
            lane = port_block_lane_get(blkp, pnd, port);
            if (lane >= 0) {
                pblk->type = blkp->type;
                pblk->inst = inst;
                pblk->blknum = blkp->blknum;
                pblk->lane = lane;
                return 0;
            }
        }
        inst++;
    }
    return -1;
}

int
bcmdrd_chip_port_number(const bcmdrd_chip_info_t *cinfo, int blknum, int lane)
{
    int idx, port;
    bcmdrd_port_num_domain_t pnd;

    if (cinfo == NULL || lane < 0) {
        return -1;
    }

    /* Foreach block in the chip */
    for (idx = 0; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = &cinfo->blocks[idx];

        if (blkp->blknum != blknum) {
            continue;
        }

        pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blkp->type);

        BCMDRD_PBMP_ITER(blkp->pbmps, port) {
            if (port_block_lane_get(blkp, pnd, port) == lane) {
                if (BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
                    return port;
                }
                return -1;
            }
        }
    }
    return -1;
}

const bcmdrd_block_t *
bcmdrd_chip_block(const bcmdrd_chip_info_t *cinfo, int blktype, int blkinst)
{
    /* Calculate the physical block number for a given blocktype instance */
    int idx, start_idx;

    if (cinfo == NULL || blktype < 0) {
        return NULL;
    }

    start_idx = cinfo->blktype_start_idx[blktype];
    for (idx = start_idx; idx < cinfo->nblocks; idx++) {
        if (cinfo->blocks[idx].type == blktype) {
            if (blkinst == 0) {
                return &cinfo->blocks[idx];
            }
            blkinst--;
        }
    }
    /* The requested block instance is not valid */
    return NULL;
}

int
bcmdrd_chip_block_number(const bcmdrd_chip_info_t *cinfo,
                         int blktype, int blkinst)
{
    const bcmdrd_block_t *blkp;

    blkp = bcmdrd_chip_block(cinfo, blktype, blkinst);
    if (blkp == NULL) {
        return -1;
    }

    return blkp->blknum;
}

int
bcmdrd_chip_blktype_pbmp(const bcmdrd_chip_info_t *cinfo, int blktype,
                         bcmdrd_pbmp_t *pbmp)
{
    const bcmdrd_block_t *blkp;
    bcmdrd_port_num_domain_t pnd;
    int idx, start_idx;

    if (cinfo == NULL || pbmp == NULL || blktype < 0) {
        return -1;
    }

    assert(cinfo->blocks);

    BCMDRD_PBMP_CLEAR(*pbmp);

    /* Iterate over all physical blocks of this type */
    start_idx = cinfo->blktype_start_idx[blktype];
    blkp = &cinfo->blocks[start_idx];
    for (idx = start_idx; idx < cinfo->nblocks; idx++, blkp++) {
        if (blkp->type == blktype) {
            BCMDRD_PBMP_OR(*pbmp, blkp->pbmps);
        }
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, -1, blktype);
    BCMDRD_PBMP_AND(*pbmp, cinfo->valid_pbmps[pnd]);

    return 0;
}

/*
 * Calculate the blocktype and instance for a physical block number
 */
int
bcmdrd_chip_block_type(const bcmdrd_chip_info_t *cinfo, int blknum,
                       int *blktype, int *blkinst)
{
    int idx;

    if (cinfo == NULL) {
        return -1;
    }

    /* Get block type */
    for (idx = 0; idx < cinfo->nblocks; idx++) {
        const bcmdrd_block_t *blkp = cinfo->blocks + idx;

        if (blkp->blknum == blknum) {
            if (blktype) {
                *blktype = blkp->type;
            }
            if (blkinst) {
                /* Get instance if requested */
                *blkinst = 0;
                while (idx > 0) {
                    idx--;
                    blkp--;
                    if (blkp->type == *blktype) {
                        (*blkinst)++;
                    }
                }
            }
            return 0;
        }
    }
    return -1;
}

bcmdrd_port_num_domain_t
bcmdrd_chip_port_num_domain(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid,
                            int blktype)
{
    if (cinfo && cinfo->port_num_domain) {
        return cinfo->port_num_domain(sid, blktype);
    }

    return BCMDRD_PND_PHYS;
}

int
bcmdrd_chip_valid_pbmp(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp)
{
    if (cinfo == NULL || pbmp == NULL) {
        return -1;
    }

    if (pnd >= BCMDRD_PND_COUNT) {
        return -1;
    }

    BCMDRD_PBMP_ASSIGN(*pbmp, cinfo->valid_pbmps[pnd]);

    return 0;
}

int
bcmdrd_chip_pipe_pbmp(const bcmdrd_chip_info_t *cinfo, int pipe_no,
                      bcmdrd_port_num_domain_t pnd, bcmdrd_pbmp_t *pbmp)
{
    int port, pipe;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    if (cinfo == NULL || pbmp == NULL) {
        return -1;
    }

    if (pnd >= BCMDRD_PND_COUNT) {
        return -1;
    }

    BCMDRD_PBMP_CLEAR(*pbmp);

    sal_memset(pi, 0, sizeof(*pi));
    pi->pnd = pnd;

    BCMDRD_PBMP_ITER(cinfo->valid_pbmps[pnd], port) {
        pi->port = port;
        pipe = bcmdrd_chip_pipe_info(cinfo, pi,
                                     BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT);
        if (pipe == pipe_no) {
            BCMDRD_PBMP_PORT_ADD(*pbmp, port);
        }
    }

    return 0;
}

bool
bcmdrd_chip_port_valid(const bcmdrd_chip_info_t *cinfo,
                       bcmdrd_port_num_domain_t pnd, int port)
{
    if (cinfo == NULL) {
        return false;
    }

    if (pnd >= BCMDRD_PND_COUNT) {
        return false;
    }

    if (!BCMDRD_PBMP_MEMBER(cinfo->valid_pbmps[pnd], port)) {
        return false;
    }

    return true;
}

int
bcmdrd_chip_port_reg_pbmp(const bcmdrd_chip_info_t *cinfo,
                          bcmdrd_sid_t sid, bcmdrd_pbmp_t *pbmp)
{
    bcmdrd_port_num_domain_t pnd;
    int bt;

    if (cinfo == NULL) {
        return -1;
    }

    bt = bcmdrd_chip_blktype_get(cinfo, sid, 0);
    if (bt < 0) {
        return -1;
    }

    pnd = bcmdrd_chip_port_num_domain(cinfo, sid, bt);
    if (pnd == bcmdrd_chip_port_num_domain(cinfo, -1, bt)) {
        bcmdrd_chip_blktype_pbmp(cinfo, bt, pbmp);
        while ((bt = bcmdrd_chip_blktype_get(cinfo, sid, bt + 1)) >= 0) {
            bcmdrd_pbmp_t blkpbmp;

            assert(pnd == bcmdrd_chip_port_num_domain(cinfo, -1, bt));
            bcmdrd_chip_blktype_pbmp(cinfo, bt, &blkpbmp);
            BCMDRD_PBMP_OR(*pbmp, blkpbmp);
        }
    } else {
        bcmdrd_chip_valid_pbmp(cinfo, pnd, pbmp);
    }

    return 0;
}

bool
bcmdrd_chip_port_reg_valid(const bcmdrd_chip_info_t *cinfo,
                           bcmdrd_sid_t sid, int port, int regidx)
{
    bcmdrd_pbmp_t pbmp;
    const bcmdrd_symbol_t *symbol;
    int encoding, maxidx;

    if (cinfo == NULL) {
        return false;
    }

    symbol = bcmdrd_sym_info_get(cinfo->symbols, sid, NULL);
    if (symbol == NULL) {
        return false;
    }

    if (bcmdrd_chip_port_reg_pbmp(cinfo, sid, &pbmp) < 0) {
        return false;
    }
    if (!BCMDRD_PBMP_MEMBER(pbmp, port)) {
        return false;
    }

    encoding = BCMDRD_SYMBOL_INDEX_ENC_GET(symbol->index);
    maxidx = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
    if (encoding || maxidx) {
        if (bcmdrd_chip_reg_index_valid(cinfo, port, regidx < 0 ? 0 : regidx,
                                        encoding, maxidx) == 0) {
            return false;
        }
    }

    return true;
}

uint32_t
bcmdrd_chip_mem_maxidx(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid,
                       uint32_t maxidx)
{
    bcmdrd_chip_mod_t *mod;

    if ((mod = bcmdrd_chip_mod_get(cinfo, sid)) != NULL) {
        if (mod->index_max.override) {
            maxidx = mod->index_max.value;
        }
    }

    return maxidx;
}

bool
bcmdrd_chip_sym_valid(const bcmdrd_chip_info_t *cinfo, bcmdrd_sid_t sid)
{
    bcmdrd_chip_mod_t *mod;

    if ((mod = bcmdrd_chip_mod_get(cinfo, sid)) != NULL) {
        if (mod->valid.override && mod->valid.value == 0) {
            return false;
        }
    }

    return true;
}

int
bcmdrd_chip_param_get(const bcmdrd_chip_info_t *cinfo,
                      const char *prm_name, uint32_t *prm_val)
{
    bcmdrd_chip_profile_t *cp = cinfo->profile;
    bcmdrd_chip_param_t *param;

    if (cp == NULL || cp->params == NULL) {
        return -1;
    }
    if (prm_name == NULL || prm_val == NULL) {
        return -1;
    }
    param = cp->params;
    while (param->name) {
        if (sal_strcmp(param->name, prm_name) == 0) {
            *prm_val = param->value;
            return 0;
        }
        param++;
    }
    return -1;
}

const char *
bcmdrd_chip_param_next(const bcmdrd_chip_info_t *cinfo,
                       const char *prm_name)
{
    bcmdrd_chip_profile_t *cp = cinfo->profile;
    bcmdrd_chip_param_t *param;
    bool found = false;

    if (cp == NULL || cp->params == NULL) {
        return NULL;
    }
    param = cp->params;
    if (prm_name == NULL) {
        return param->name;
    }
    while (param->name && !found) {
        if (sal_strcmp(param->name, prm_name) == 0) {
            found = true;
        }
        param++;
    }
    return param->name;
}

int
bcmdrd_chip_reg_index_valid(const bcmdrd_chip_info_t *cinfo, int port,
                            int regidx, int encoding, int regidx_max)
{
    bcmdrd_numel_info_t *numel_info = cinfo->numel_info;
    bcmdrd_numel_range_t *numel_range;
    int idx;
    int *range_id;

    if (encoding > 0 && numel_info != NULL) {
        /* The first range ID of encoding 0 is the number of valid encodings */
        if (encoding < numel_info->encodings[0].range_id[0]) {
            range_id = numel_info->encodings[encoding].range_id;
            idx = 0;
            while (range_id[idx] >= 0) {
                numel_range = &numel_info->chip_ranges[range_id[idx]];
                if (regidx >= numel_range->min && regidx <= numel_range->max
                    && BCMDRD_PBMP_MEMBER(numel_range->pbmp, port)) {
                    return 1;
                }
                idx++;
            }
        }
    } else if (regidx <= regidx_max || regidx_max < 0) {
        return 1;
    }
    return 0;
}

uint32_t
bcmdrd_chip_pipe_info(const bcmdrd_chip_info_t *cinfo,
                      bcmdrd_pipe_info_t *pi, bcmdrd_pipe_info_type_t pi_type)
{
    if (cinfo && cinfo->pipe_info) {
        return cinfo->pipe_info(cinfo, pi, pi_type);
    }

    return 0;
}

int
bcmdrd_chip_port_type_pbmp(const bcmdrd_chip_info_t *cinfo,
                           bcmdrd_port_type_t ptype, bcmdrd_pbmp_t *pbmp)
{
    int blktype;

    if (cinfo == NULL || cinfo->blktype_from_porttype == NULL) {
        return -1;
    }

    blktype = cinfo->blktype_from_porttype(ptype);
    if (blktype < 0) {
        return -1;
    }

    return bcmdrd_chip_blktype_pbmp(cinfo, blktype, pbmp);
}
