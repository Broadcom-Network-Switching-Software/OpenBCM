/*! \file bcmdrd_dev.c
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_cht.h>

#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_internal.h>
#include <bcmdrd/bcmdrd_dev.h>

/*******************************************************************************
 * Local definitions
 */

/* Declare all chip_info structures */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmdrd_chip_info_t _bc##_drd_chip_info;
#define BCMDRD_DEVLIST_INCLUDE_ALL
#include <bcmdrd/bcmdrd_devlist.h>

/* Create device table */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _vn, _dv, _rv, _md, #_nm, #_fn, #_bd, BCMDRD_DEV_T_##_bd, &_bc##_drd_chip_info },
#define BCMDRD_DEVLIST_INCLUDE_ALL
static struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t revision;
    uint16_t model;
    const char *name;
    const char *full_name;
    const char *type_str;
    bcmdrd_dev_type_t type;
    const bcmdrd_chip_info_t *chip_info;
} dev_table[] = {
#include <bcmdrd/bcmdrd_devlist.h>
    { 0, 0 } /* end-of-list */
};

static bcmdrd_dev_t bcmdrd_dev[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static int
dev_lookup(bcmdrd_dev_id_t *id)
{
    int match_idx = -1;
    int idx = 0;
    int rev = -1;
    while (dev_table[idx].vendor_id) {
        if (id->vendor_id == dev_table[idx].vendor_id &&
            id->device_id == dev_table[idx].device_id &&
            id->model == dev_table[idx].model) {
            /*
             * Vendor/device ID may match multiple entries,
             * so we look for highest matching revision.
             */
            if (id->revision >= dev_table[idx].revision &&
                rev < dev_table[idx].revision) {
                /* Found a (better) match */
                match_idx = idx;
                rev = dev_table[idx].revision;
            }
        }
        idx++;
    }
    return match_idx;
}

static bool
adj_byte_swap(int unit, bool byte_swap)
{
    bool byte_swap_pio;

    byte_swap_pio = bcmdrd_dev[unit].io.byte_swap_pio;
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_IPROC)) {
        if (bcmdrd_dev_bus_type_get(unit) == BCMDRD_BT_PCI) {
            if (byte_swap_pio) {
                /*
                 * If BE_PIO is set, then we assume that the iProc
                 * PCI bridge will byte-swap all access types,
                 * i.e. both PIO and DMA access.
                 *
                 * In order to avoid double-swapping, we then need to
                 * invert all byte-swap settings provided by the system.
                 */
                byte_swap = !byte_swap;
            }
        }
    }

    return byte_swap;
}

static int
dev_valid_pipes_set(bcmdrd_dev_t *dev)
{
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;
    int port, pipe;

    dev->valid_pipes = 0;
    sal_memset(pi, 0, sizeof(*pi));
    pi->pnd = BCMDRD_PND_PHYS;
    BCMDRD_PBMP_ITER(dev->chip_info->valid_pbmps[BCMDRD_PND_PHYS], port) {
        pi->port = port;
        pipe = bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                     BCMDRD_PIPE_INFO_TYPE_PIPE_INDEX_FROM_PORT);
        dev->valid_pipes |= (1 << pipe);
    }
    return 0;
}

static uint32_t
dev_pipe_num(bcmdrd_dev_t *dev)
{
    return bcmdrd_chip_pipe_info(dev->chip_info, NULL,
                                 BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);
}

/*
 * Calculate hash value for lookups into the chip modifier table.
 */
static uint32_t
chip_mod_hash(uint32_t size, const void *entry)
{
    return ((bcmdrd_chip_mod_t *)entry)->sid % size;
}

/*
 * Compare hash keys for the chip modifier table.
 */
static bool
chip_mod_match(const void *e1, const void *e2)
{
    return (((bcmdrd_chip_mod_t *)e1)->sid == ((bcmdrd_chip_mod_t *)e2)->sid);
}

/*
 * Clean up hash-based lookups for the chip modifier table.
 */
static int
dev_mod_cleanup(bcmdrd_dev_t *dev)
{
    shr_cht_destroy(dev->chip_mod_cht);
    dev->chip_mod_cht = NULL;

    return SHR_E_NONE;
}

/*
 * Initialize hash-based lookups for the chip modifier table.
 *
 * The chip modifier table contains override values for a device
 * variant wherever these differ from the base device. The override
 * values typically redefine the sizes of various SOC memories.
 *
 * Since the number of symbols with override values may be extensive,
 * we use a hash-based lookup to avoid sequential table searches in
 * performance sensitive code paths.
 */
static int
dev_mod_init(bcmdrd_dev_t *dev)
{
    shr_cht_t *cht;
    bcmdrd_chip_profile_t *cp;
    bcmdrd_chip_mod_t *cm;
    uint32_t idx;

    cp = dev->chip_info->profile;

    if (cp == NULL || cp->mods == NULL) {
        /* Nothing to initialize */
        return SHR_E_NONE;
    }

    /* Get array size */
    cm = cp->mods;
    idx = 0;
    while (cm[idx].sid != BCMDRD_INVALID_ID) {
        idx++;
    }

    /* Create hash table */
    cht = shr_cht_create(idx, chip_mod_hash, chip_mod_match);
    if (cht == NULL) {
        return SHR_E_MEMORY;
    }

    /* Populate hash table */
    idx = 0;
    while (cm[idx].sid != BCMDRD_INVALID_ID) {
        if (shr_cht_insert(cht, &cm[idx]) == NULL) {
            return SHR_E_INTERNAL;
        }
        idx++;
    }

    dev->chip_mod_cht = cht;

    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */

bool
bcmdrd_dev_supported(bcmdrd_dev_id_t *id)
{
    return (dev_lookup(id) >= 0);
}

int
bcmdrd_dev_destroy(int unit)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        /* Device has not been created */
        return SHR_E_INIT;
    }

    dev = &bcmdrd_dev[unit];

    /* Free hash table for chip modifiers */
    dev_mod_cleanup(dev);

    sal_memset(dev, 0, sizeof(*dev));

    return SHR_E_NONE;
}

int
bcmdrd_dev_create(int unit, bcmdrd_dev_id_t *id)
{
    bcmdrd_dev_t *dev;
    int match_idx;
    int rv = SHR_E_NOT_FOUND;

    if (unit < 0) {
        for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
                break;
            }
        }
    }
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type != BCMDRD_DEV_T_NONE) {
        return SHR_E_EXISTS;
    }

    match_idx = dev_lookup(id);

    if (match_idx >= 0) {
        /* Warn about possible incompatibility */
        if (id->revision > dev_table[match_idx].revision) {
            LOG_WARN(BSL_LS_BCMDRD_DEV,
                     (BSL_META_U(unit,
                                 "Device revision (0x%02x) is newer than "
                                 "supported driver revision (0x%02x)\n"),
                         id->revision, dev_table[match_idx].revision));
        }
        dev = &bcmdrd_dev[unit];

        /* Fill out device structure */
        sal_memcpy(&dev->id, id, sizeof(*id));
        dev->name = dev_table[match_idx].name;
        dev->type_str = dev_table[match_idx].type_str;
        dev->type = dev_table[match_idx].type;

        bcmdrd_dev_chip_info_set(unit, dev_table[match_idx].chip_info);

        /* Initialize default valid port bitmap */
        bcmdrd_dev_valid_ports_set(unit,
                                   &dev->chip_info->valid_pbmps[BCMDRD_PND_PHYS]);

        /* Initialize valid pipes accoding to valid port bitmap */
        dev_valid_pipes_set(dev);

        /* Initialize hash table for chip modifiers */
        dev_mod_init(dev);

        /* Reset feature list */
        bcmdrd_feature_disable(unit, BCMDRD_FT_ALL);

        LOG_INFO(BSL_LS_BCMDRD_DEV,
                 (BSL_META_U(unit,
                             "Created %s device "
                             "(%04x:%04x:%02x)\n"),
                  dev->name, id->vendor_id, id->device_id, id->revision));

        rv = SHR_E_NONE;
    }

    /* Return an error on failure or the new unit number when successful */
    return (rv < 0) ? rv : unit;
}

bcmdrd_dev_type_t
bcmdrd_dev_type(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return BCMDRD_DEV_T_NONE;
    }

    return bcmdrd_dev[unit].type;
}

const char *
bcmdrd_dev_type_str(int unit)
{
    const char *type_str = NULL;

    if (BCMDRD_UNIT_VALID(unit)) {
        type_str = bcmdrd_dev[unit].type_str;
    }

    return type_str ? type_str : "";
}

const char *
bcmdrd_dev_name(int unit)
{
    const char *name = NULL;

    if (BCMDRD_UNIT_VALID(unit)) {
        name = bcmdrd_dev[unit].name;
    }

    return name ? name : "";
}

int
bcmdrd_dev_id_get(int unit, bcmdrd_dev_id_t *id)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(id, &bcmdrd_dev[unit].id, sizeof(*id));

    return SHR_E_NONE;
}

int
bcmdrd_dev_id_from_name(const char *dev_name, bcmdrd_dev_id_t *id)
{
    int idx = 0;

    assert(dev_name);
    assert(id);

    while (dev_table[idx].vendor_id) {
        if (sal_strcasecmp(dev_name, dev_table[idx].full_name) == 0) {
            id->vendor_id = dev_table[idx].vendor_id;
            id->device_id = dev_table[idx].device_id;
            id->revision = dev_table[idx].revision;
            id->model = dev_table[idx].model;
            return SHR_E_NONE;
        }
        idx++;
    }
    return SHR_E_NOT_FOUND;
}

bool
bcmdrd_dev_is_variant_sku(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return false;
    }

    dev = &bcmdrd_dev[unit];

    return (dev->chip_info->flags & BCMDRD_CHIP_FLAG_VARIANT) > 0;
}

int
bcmdrd_dev_hal_io_init(int unit, bcmdrd_hal_io_t *io)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(&bcmdrd_dev[unit].io, io, sizeof(*io));

    return SHR_E_NONE;
}

int
bcmdrd_dev_hal_io_get(int unit, bcmdrd_hal_io_t *io)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(io, &bcmdrd_dev[unit].io, sizeof(*io));

    return SHR_E_NONE;
}

int
bcmdrd_dev_hal_dma_init(int unit, bcmdrd_hal_dma_t *dma)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(&bcmdrd_dev[unit].dma, dma, sizeof(*dma));

    return SHR_E_NONE;
}

int
bcmdrd_dev_hal_intr_init(int unit, bcmdrd_hal_intr_t *intr)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return SHR_E_NOT_FOUND;
    }

    sal_memcpy(&bcmdrd_dev[unit].intr, intr, sizeof(*intr));

    return SHR_E_NONE;
}

bcmdrd_dev_t *
bcmdrd_dev_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return NULL;
    }
    if (bcmdrd_dev[unit].type == BCMDRD_DEV_T_NONE) {
        return NULL;
    }

    return &bcmdrd_dev[unit];
}

bool
bcmdrd_dev_byte_swap_pio_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return adj_byte_swap(unit, bcmdrd_dev[unit].io.byte_swap_pio);
}

bool
bcmdrd_dev_byte_swap_packet_dma_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return adj_byte_swap(unit, bcmdrd_dev[unit].io.byte_swap_packet_dma);
}

bool
bcmdrd_dev_byte_swap_non_packet_dma_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return adj_byte_swap(unit, bcmdrd_dev[unit].io.byte_swap_non_packet_dma);
}

bcmdrd_hal_bus_type_t
bcmdrd_dev_bus_type_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return BCMDRD_BT_UNKNOWN;
    }

    return bcmdrd_dev[unit].io.bus_type;
}

uint32_t
bcmdrd_dev_io_flags_get(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return 0;
    }

    return bcmdrd_dev[unit].io.flags;
}

bool
bcmdrd_dev_exists(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }

    return (bcmdrd_dev[unit].chip_info != NULL);
}

int
bcmdrd_dev_chip_info_set(int unit, const bcmdrd_chip_info_t *chip_info)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    dev->chip_info = chip_info;

    return SHR_E_NONE;
}

const bcmdrd_chip_info_t *
bcmdrd_dev_chip_info_get(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info;
}

int
bcmdrd_dev_valid_ports_set(int unit, const bcmdrd_pbmp_t *pbmp)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    assert(pbmp);
    sal_memcpy(&dev->valid_ports, pbmp, sizeof(dev->valid_ports));
    BCMDRD_PBMP_AND(dev->valid_ports,
                    dev->chip_info->valid_pbmps[BCMDRD_PND_PHYS]);

    return SHR_E_NONE;
}

int
bcmdrd_dev_valid_ports_get(int unit, bcmdrd_pbmp_t *pbmp)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];

    assert(pbmp);
    sal_memcpy(pbmp, &dev->valid_ports, sizeof(*pbmp));

    return SHR_E_NONE;
}

int
bcmdrd_dev_valid_pipes_set(int unit, uint32_t pipe_map)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    dev = &bcmdrd_dev[unit];
    dev->valid_pipes = pipe_map;

    return SHR_E_NONE;
}

int
bcmdrd_dev_valid_pipes_get(int unit, uint32_t *pipe_map)
{
    bcmdrd_dev_t *dev;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (pipe_map == NULL) {
        return SHR_E_PARAM;
    }

    dev = &bcmdrd_dev[unit];
    *pipe_map = dev->valid_pipes;

    return SHR_E_NONE;
}

int
bcmdrd_dev_valid_blk_pipes_get(int unit, uint32_t dev_pipe_map,
                               int blktype, uint32_t *blk_pipe_map)
{
    int rv;
    uint32_t pipe_map = dev_pipe_map;
    bcmdrd_pipe_info_t pipe_info, *pi = &pipe_info;

    if (blk_pipe_map == NULL) {
        return SHR_E_PARAM;
    }

    /*
     * Use the bitmap of device valid pipes by default if the
     * to-be-transformed dev_pipe_map is not specified.
     */
    if (pipe_map == 0) {
        rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    pi->blktype = blktype;
    pi->pipe_map = pipe_map;
    *blk_pipe_map = bcmdrd_dev_pipe_info(unit, pi,
                                         BCMDRD_PIPE_INFO_TYPE_BLK_PIPE_MAP);
    if (*blk_pipe_map == 0) {
        *blk_pipe_map = pipe_map;
    }

    return SHR_E_NONE;
}

const bcmdrd_symbols_t *
bcmdrd_dev_symbols_get(int unit, int idx)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    /* Currently we only support a single symbol table */
    if (idx != 0) {
        return NULL;
    }

    return dev->chip_info->symbols;
}

uint32_t
bcmdrd_dev_flags_get(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return 0;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info->flags;
}

void
bcmdrd_dev_bypass_mode_set(int unit, uint32_t bypass_mode)
{
    if (BCMDRD_UNIT_VALID(unit)) {
        LOG_VERBOSE(BSL_LS_BCMDRD_BYPASS,
                    (BSL_META_U(unit,
                                "Bypass mode: 0x%x"PRIu32"\n"),
                     bypass_mode));
        bcmdrd_dev[unit].bypass_mode = bypass_mode;
    }
}

uint32_t
bcmdrd_dev_bypass_mode_get(int unit)
{
    if (BCMDRD_UNIT_VALID(unit)) {
        return bcmdrd_dev[unit].bypass_mode;
    }
    return 0;
}

uint32_t
bcmdrd_dev_pipe_info(int unit, bcmdrd_pipe_info_t *pi,
                     bcmdrd_pipe_info_type_t pi_type)
{
    bcmdrd_dev_t *dev;
    uint32_t pipe_info, pipe_num;

    if (!bcmdrd_dev_exists(unit)) {
        return 0;
    }

    dev = &bcmdrd_dev[unit];

    pipe_info = bcmdrd_chip_pipe_info(dev->chip_info, pi, pi_type);
    if (pipe_info == 0) {
        return 0;
    }

    /*
     * Check the device valid pipes if the queried information is related to
     * pipe mask.
     */
    switch (pi_type) {
    case BCMDRD_PIPE_INFO_TYPE_PMASK:
        pipe_num = bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                         BCMDRD_PIPE_INFO_TYPE_NUM_PIPE_INST);
        if (pipe_num == dev_pipe_num(dev)) {
            pipe_info &= dev->valid_pipes;
        } else {
            int rv;
            uint32_t pipe_map = 0, blk_pipe_map;

            /* Check if there exists the pipe map from device to block. */
            rv = bcmdrd_dev_valid_pipes_get(unit, &pipe_map);
            if (SHR_SUCCESS(rv) && pipe_map != 0) {
                pi->pipe_map = pipe_map;
                blk_pipe_map =
                    bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                          BCMDRD_PIPE_INFO_TYPE_BLK_PIPE_MAP);
                if (blk_pipe_map != 0) {
                    pipe_info &= blk_pipe_map;
                }
            }
        }
        break;
    case BCMDRD_PIPE_INFO_TYPE_BASETYPE_PMASK:
        pipe_num = bcmdrd_chip_pipe_info(dev->chip_info, pi,
                                         BCMDRD_PIPE_INFO_TYPE_LINST);
        if (pipe_num == dev_pipe_num(dev)) {
            pipe_info &= dev->valid_pipes;
        }
        break;
    case BCMDRD_PIPE_INFO_TYPE_SUB_PIPE_MAP:
        pipe_info = bcmdrd_chip_pipe_info(dev->chip_info, pi, pi_type);
        pipe_info &= dev->valid_pipes;
        break;
    default:
        break;
    }

    return pipe_info;
}

bcmdrd_block_port_addr_f
bcmdrd_dev_block_port_addr_func(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info->block_port_addr;
}

bcmdrd_addr_decode_f
bcmdrd_dev_addr_decode_func(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info->addr_decode;
}

bcmdrd_chip_mod_t *
bcmdrd_dev_mod_get(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_dev_t *dev;
    bcmdrd_chip_mod_t key, *mod;

    if (!bcmdrd_dev_exists(unit)) {
        return 0;
    }

    dev = &bcmdrd_dev[unit];

    /* Hash-based lookup is preferred */
    key.sid = sid;
    mod = shr_cht_lookup(dev->chip_mod_cht, &key);
    if (mod) {
        return mod;
    }

    /* Fall back to sequential search */
    return bcmdrd_chip_mod_get(dev->chip_info, sid);
}

uint32_t
bcmdrd_dev_mem_maxidx(int unit, bcmdrd_sid_t sid, uint32_t maxidx)
{
    bcmdrd_chip_mod_t *mod;

    if ((mod = bcmdrd_dev_mod_get(unit, sid)) != NULL) {
        if (mod->index_max.override) {
            maxidx = mod->index_max.value;
        }
    }

    return maxidx;
}

bool
bcmdrd_dev_sym_valid(int unit, bcmdrd_sid_t sid)
{
    bcmdrd_chip_mod_t *mod;

    if ((mod = bcmdrd_dev_mod_get(unit, sid)) != NULL) {
        if (mod->valid.override && mod->valid.value == 0) {
            return false;
        }
    }

    return true;
}

int
bcmdrd_dev_param_get(int unit, const char *prm_name, uint32_t *prm_val)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return -1;
    }

    dev = &bcmdrd_dev[unit];

    return bcmdrd_chip_param_get(dev->chip_info, prm_name, prm_val);
}

const char *
bcmdrd_dev_param_next(int unit, const char *prm_name)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return bcmdrd_chip_param_next(dev->chip_info, prm_name);
}

const char *
bcmdrd_dev_signature_get(int unit)
{
    bcmdrd_dev_t *dev;

    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    dev = &bcmdrd_dev[unit];

    return dev->chip_info->signature;
}
