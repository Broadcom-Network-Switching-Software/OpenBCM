/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SBUS DMA Test
 *
 * The SBUS DMA test writes/reads a table with one or more CMC/channel. It
 * initializes a source memory image with random values. Then, it kicks off a
 * register/descriptor based SBUS DMA write from the source memory to the table.
 * As soon as the write finishes, it kicks off another register/descriptor based
 * SBUS DMA read from the table to a destination memory. Data integrity check is
 * done by comparing the source and destination memories are compared with the
 * reserved and parity/ECC fields excluded. Write and read performance in MB/s
 * and transfer/s are calculated from the set up of the first DMA to the finish
 * of the second DMA.
 */

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/mem.h>
#include <soc/register.h>
#include <soc/debug.h>
#include <soc/dma.h>
#include <soc/l2x.h>
#include <soc/schanmsg_internal.h>
#include <soc/sbusdma_internal.h>
#include "testlist.h"
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT)
#include <soc/cmicm.h>
#endif

#if defined (BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif

extern int compare_with_mask(const void *m1_void, const void *m2_void,
                             size_t len);

#define SOC_SBUSDMA_CH_MAX  SOC_CMCS_NUM_MAX * SOC_SBUSDMA_CH_PER_CMC
#define NUM_SBUSDMA_CH_PER_CMC (sb_test_data.max_num_ch)
#define TD_DEFAULT_DUMP_ENTRY_COUNT  10
#define INT_FRAC_4PT(x) (x) / 10000, (x) % 10000

#if defined(BCM_ESW_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT) && \
    (defined(BCM_CMICM_SUPPORT)|| defined(BCM_CMICX_SUPPORT))

typedef struct sbt_data_s {
    int             init_done;
    int             run[SOC_SBUSDMA_CH_MAX];
    int             mem_done[SOC_SBUSDMA_CH_MAX];

    uint32          rnum;
    int             rate_calc_int;
    int             ecc_as_data;/* treat ecc field as regular field */
    int             big_endian;
    int             big_pio;
    int             slam_en;    /* Slam Enable */
    int             slam_dir;   /* Slam direction */
    int             check_data; /* Enable data integrity check */
    int             desc_mode;  /* Descriptor based SBUS DMA mode */
    uint32          cmc_ch_bm;  /* CMC# and channel# bitmap for SBUS DMA */
    int             dma_wr_en;  /* SBUS DMA write enable */
    int             dma_rd_en;  /* SBUS DMA read enable */
    int             test_mode;  /* Test mode: 1->write, 2->read, 3->write&read (default) */
    int             num_mem;    /* Number of memories to be tested in parallel */
    int             verbose;    /* Turn on verbose messages */
    int             debug;      /* Turn on debug messages */
                                /* Map cmc_ch_bm to memory index */
    int             ch_mem_map[SOC_SBUSDMA_CH_MAX];
    uint32          max_num_ch;
} sb_test_data_t;

typedef struct sbt_mem_s {
    char           *mem_str;
    soc_mem_t       mem;
    int             copyno;
    int             index_min;
    int             index_max;
    int             count;
    int             frag_count;
    int             frag_index_max[32768];
    int             frag_index_min[32768];

    uint32         *source;
    uint32         *dma_ed;

    int             bufwords;
    int             entwords;

    uint32          datamask[SOC_MAX_MEM_WORDS];
    uint32          tcammask[SOC_MAX_MEM_WORDS];
    uint32          eccmask[SOC_MAX_MEM_WORDS];
    uint32          forcemask[SOC_MAX_MEM_WORDS];
    uint32          forcedata[SOC_MAX_MEM_WORDS];
    uint32          accum_tcammask;
    uint32          accum_forcemask;

    uint32          mem_cmc_ch_bm;
} sb_test_mem_t;

static sb_test_data_t sb_test_data;

static sb_test_mem_t sb_test_mem[SOC_SBUSDMA_CH_MAX];

static sbusdma_desc_handle_t sbusdma_desc_handle[SOC_SBUSDMA_CH_MAX];

static int sb_test_mem_cmp_fn(int, int, void*, void*);

static int
sb_test_data_init(int unit, int alloc)
{
    int i, n;

    if (!sb_test_data.init_done) {
        sb_test_mem[0].mem = L2Xm;
        sb_test_mem[0].mem_str = sal_strdup("l2_entry");
#ifdef BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(unit)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                sb_test_mem[0].mem = L2_ENTRY_1m;
                sb_test_mem[0].mem_str = sal_strdup("l2_entry_1");
                sb_test_mem[0].count = soc_mem_index_count(unit, L2_ENTRY_1m);
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            if (SOC_MEM_IS_VALID(unit, L2_ENTRY_ONLYm)) {
                sb_test_mem[0].mem = L2_ENTRY_ONLYm;
                sb_test_mem[0].mem_str = sal_strdup("l2_entry_only");
                sb_test_mem[0].count = soc_mem_index_count(unit, L2_ENTRY_ONLYm);
            }
        } else
#endif /* BCM_FIREBOLT_SUPPORT */
        {
            test_error(unit, "SBUS DMA Error:  Invalid SOC type\n");
            return BCM_E_FAIL;
        }
        sb_test_data.init_done     = TRUE;
        sb_test_data.slam_en       = FALSE;
        sb_test_data.slam_dir      = TRUE;  /* SLAM from low to high */
        sb_test_data.ecc_as_data   = TRUE;
        sb_test_data.desc_mode     = 0;
        sb_test_data.cmc_ch_bm     = 0;
        sb_test_mem[0].frag_count  = 0;
        sb_test_data.dma_wr_en     = TRUE;
        sb_test_data.dma_rd_en     = TRUE;
        sb_test_data.test_mode     = 3;
        sb_test_data.num_mem       = 1;
        sb_test_data.verbose       = FALSE;
        sb_test_data.debug         = FALSE;
        sb_test_data.rnum          = sal_rand();
        sb_test_data.rate_calc_int = 10;
        sb_test_data.check_data    = TRUE;
        for (i = 0; i < SOC_SBUSDMA_CH_MAX; i++) {
            sb_test_data.run[i] = FALSE;
            sb_test_data.mem_done[i] = TRUE;
        }
    }

    sb_test_data.max_num_ch = soc_sbusdma_cmc_max_num_ch_get(unit);

    if (!alloc) {
        return SOC_E_NONE;
    }

    for (n = 0; n < sb_test_data.num_mem; n++) {
        sb_test_mem[n].entwords = soc_mem_entry_words(unit, sb_test_mem[n].mem);
        sb_test_mem[n].bufwords = sb_test_mem[n].entwords * sb_test_mem[n].count;
        sb_test_mem[n].source = soc_cm_salloc(unit,
                                              WORDS2BYTES(sb_test_mem[n].bufwords),
                                              "sb_test_data source");
        sb_test_mem[n].dma_ed = soc_cm_salloc(unit,
                                              WORDS2BYTES(sb_test_mem[n].bufwords),
                                              "sb_test_data dma_ed");
        if ((sb_test_mem[n].source == NULL) ||
                (sb_test_mem[n].dma_ed == NULL)) {
            if (sb_test_mem[n].source) {
                soc_cm_sfree(unit, sb_test_mem[n].source);
                sb_test_mem[n].source = NULL;
            }
            if (sb_test_mem[n].dma_ed) {
                soc_cm_sfree(unit, sb_test_mem[n].dma_ed);
                sb_test_mem[n].dma_ed = NULL;
            }
            test_error(unit, "SBUS DMA Error:  Failed to allocate DMA memory\n");
            return BCM_E_FAIL;
        }

        if (sb_test_data.rnum != 0) {
            sal_memset(sb_test_mem[n].source, 0xff,
                       WORDS2BYTES(sb_test_mem[n].bufwords));
            sal_memset(sb_test_mem[n].dma_ed, 0xff,
                       WORDS2BYTES(sb_test_mem[n].bufwords));
        }
        soc_mem_datamask_get(unit, sb_test_mem[n].mem, sb_test_mem[n].datamask);
        soc_mem_tcammask_get(unit, sb_test_mem[n].mem, sb_test_mem[n].tcammask);
        soc_mem_eccmask_get(unit, sb_test_mem[n].mem, sb_test_mem[n].eccmask);
        soc_mem_forcedata_get(unit, sb_test_mem[n].mem, sb_test_mem[n].forcemask,
                              sb_test_mem[n].forcedata);
        sb_test_mem[n].accum_tcammask = 0;
        for (i = 0; i < sb_test_mem[n].entwords; i++) {
            sb_test_mem[n].accum_tcammask |= sb_test_mem[n].tcammask[i];
        }
        sb_test_mem[n].accum_forcemask = 0;
        for (i = 0; i < sb_test_mem[n].entwords; i++) {
            sb_test_mem[n].accum_forcemask |= sb_test_mem[n].forcemask[i];
        }
        if (!sb_test_data.ecc_as_data) {
            for (i = 0; i < sb_test_mem[n].entwords; i++) {
                sb_test_mem[n].datamask[i] &= ~sb_test_mem[n].eccmask[i];
            }
        }
        soc_mem_datamask_memtest(unit, sb_test_mem[n].mem, sb_test_mem[n].datamask);
    }

    return SOC_E_NONE;
}

static void
sb_test_dump_data(int unit, int n, int entry_cnt)
{
    int     i, base_index, idx;
    uint32 *s, *d;

    cli_out("SBUS DMA test for %s.%s: index_min %d index_max %d\n"
            "    bufwords %d entwords %d rnum %d count %d\n",
            SOC_MEM_UFNAME(unit, sb_test_mem[n].mem),
            SOC_BLOCK_NAME(unit, sb_test_mem[n].copyno),
            sb_test_mem[n].index_min,
            sb_test_mem[n].index_max, sb_test_mem[n].bufwords,
            sb_test_mem[n].entwords, sb_test_data.rnum, sb_test_mem[n].count);

    cli_out("    Datamask: ");
    for (idx = 0; idx < sb_test_mem[n].entwords; idx++) {
        cli_out(" %08x", sb_test_mem[n].datamask[idx]);
    }
    cli_out("\n\n");

    s = sb_test_mem[n].source;
    d = sb_test_mem[n].dma_ed;

    assert(s && d);

    base_index = sb_test_mem[n].index_min;
    s += sb_test_mem[n].index_min;

    for (i = sb_test_mem[n].index_min;
            i <= sb_test_mem[n].index_max && i - base_index < entry_cnt;
            ++i) {
        cli_out("    SBUS index %d: "
                "source entry %d, dma_ed entry %d\n",
                i, i - base_index, i - sb_test_mem[n].index_min);
        cli_out("    Source:  ");
        for (idx = 0; idx < sb_test_mem[n].entwords; idx++) {
            cli_out(" %08x", s[idx]);
        }
        cli_out("\n");
        cli_out("    DMA_ed:  ");
        for (idx = 0; idx < sb_test_mem[n].entwords; idx++) {
            cli_out(" %08x", d[idx]);
        }
        cli_out("\n\n");
        d += sb_test_mem[n].entwords;
        s += sb_test_mem[n].entwords;
    }
}

static void
sb_test_data_clear(int unit)
{
    int n;

    for (n = 0; n < sb_test_data.num_mem; n++) {
        if (sb_test_mem[n].source) {
            soc_cm_sfree(unit, sb_test_mem[n].source);
            sb_test_mem[n].source = NULL;
        }
        if (sb_test_mem[n].dma_ed) {
            soc_cm_sfree(unit, sb_test_mem[n].dma_ed);
            sb_test_mem[n].dma_ed = NULL;
        }
    }
}

static uint32
sb_test_get_curr_rnum(void)
{
    return sb_test_data.rnum;
}

static uint32
sb_test_rand32(uint32 prev)
{
    return sb_test_data.rnum = 1664525L * prev + 1013904223L;
}

#define BSAND_BYTE_SWAP(x) ((((x) << 24)) | (((x) & 0x00ff00) << 8) | \
                            (((x) & 0xff0000) >> 8) | (((x) >> 24)))
#define BSAND_WORD_SWAP(x) ((((x) & 0xFFff0000) >> 16) | \
                            (((x) & 0x0000FFff) << 16) )

static uint32 *
sb_test_random_entry(int unit, soc_mem_t mem, uint32 *entbuf)
{
    int i, n;

    for (n = 0; n < sb_test_data.num_mem; n++) {
        for (i = 0; i < sb_test_mem[n].entwords; ++i) {
            uint32 prev = sb_test_get_curr_rnum();
            entbuf[i] = sb_test_rand32(prev) & sb_test_mem[n].datamask[i];
        }
    }

    return entbuf;
}

static int
sb_test_write_entry(int unit, int cmc, soc_mem_t mem, int copyno,
                    int index, uint32 *entbuf)
{
    int rv = 0;

    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (soc_feature(unit, soc_feature_cmicm)) {
            cmicm_schan_ch_put(unit, cmc);
        }
        rv = soc_mem_array_write(unit, mem, 0, copyno, index, entbuf);
        if (soc_feature(unit, soc_feature_cmicm)) {
            if (cmicm_schan_ch_try_get(unit, cmc) != SOC_E_NONE) {
                test_error(unit, "Failed to reserved schan from cmc %0d\n",
                cmc);
            }
        }
    }
    if (rv < 0) {
        test_error(unit,
                   "Write entry (copyno %d) failed: %s",
                   copyno, soc_errmsg(rv));
    }
    return rv;
}

static int
sb_test_read_entry(int unit, int cmc, soc_mem_t mem, int copyno,
                   int index, uint32 *entbuf)
{
    int rv = 0;

    if (SOC_MEM_IS_VALID(unit, mem)) {
        if (soc_feature(unit, soc_feature_cmicm)) {
            cmicm_schan_ch_put(unit, cmc);
        }
        rv = soc_mem_array_read(unit, mem, 0, copyno, index, entbuf);
        if (soc_feature(unit, soc_feature_cmicm)) {
            if (cmicm_schan_ch_try_get(unit, cmc) != SOC_E_NONE) {
                test_error(unit, "Failed to reserved schan from cmc %0d\n",
                cmc);
            }
        }
    }
    if (rv < 0) {
        test_error(unit,
                   "Read entry (copyno %d) failed: %s",
                   copyno, soc_errmsg(rv));
    }
    return rv;
}

static int
sb_test_verify(int unit, int n)
{
    int rv_comp    = SOC_E_NONE;
    soc_mem_t mem  = sb_test_mem[n].mem;
    int base_index = soc_mem_index_min(unit, mem);
    int index_min  = sb_test_mem[n].index_min;
    int index_max  = sb_test_mem[n].index_max;
    int ent_words  = soc_mem_entry_words(unit, mem);
    int i;
    int idx;
    uint32 *d;
    uint32 *s;
    uint32 ecc_mask;

    if (!sb_test_data.ecc_as_data) {
        for (i = 0; i <= index_max - index_min; i++) {
            for (idx = 0; idx < sb_test_mem[n].entwords; idx++) {
                ecc_mask = (~sb_test_mem[n].eccmask[idx]);
                sb_test_mem[n].dma_ed[i * ent_words + idx] &= ecc_mask;
                sb_test_mem[n].source[i * ent_words + idx] &= ecc_mask;
            }
        }
    }

    if (soc_feature(unit, soc_feature_sbusdma)) {
        if ((sb_test_data.slam_dir == 0) && sb_test_data.slam_en &&
            soc_mem_slamable(unit, mem, sb_test_mem[n].copyno)) {
            /* swap the source buffer */
            uint32 temp[SOC_MAX_MEM_WORDS];
            s = sb_test_mem[n].source;
            d = s + ((index_max - index_min) * ent_words);
            for (i = 0; i < (index_max - index_min + 1) / 2; ++i) {
                sal_memcpy(temp, s + i * ent_words, ent_words * 4);
                sal_memcpy(s + i * ent_words, d - i * ent_words, ent_words * 4);
                sal_memcpy(d - i * ent_words, temp, ent_words * 4);
            }
        }
        rv_comp = sal_memcmp(sb_test_mem[n].dma_ed, sb_test_mem[n].source,
                                 (index_max - index_min + 1) * ent_words * 4);
    } else {
        rv_comp = sal_memcmp(sb_test_mem[n].dma_ed, sb_test_mem[n].source,
                             (index_max - index_min + 1) * ent_words * 4);
    }

    if (rv_comp != SOC_E_NONE) {

        cli_out("Mismatch found\n");
        s = sb_test_mem[n].source;
        d = sb_test_mem[n].dma_ed;
        for (i = index_min; i <= index_max; ++i) {
            if (sb_test_mem_cmp_fn(unit, n, s, d) != 0) {
                cli_out("%s.%s diff entry index %d: "
                        "source entry %d, dma_ed entry %d\n",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, sb_test_mem[n].copyno),
                        i, i - base_index, i - index_min);
                cli_out("Source:");
                for (i = 0; i < ent_words; i++) {
                    cli_out(" 0x%08x = 0x%08x", (PTR_TO_INT(s+i)), s[i]);
                }
                cli_out("\nDMA_ed:");
                for (i = 0; i < ent_words; i++) {
                    cli_out(" 0x%08x = 0x%08x", (PTR_TO_INT(d+i)), d[i]);
                }

                cli_out("\nMask");
                for (i = 0; i < ent_words; i++) {
                    cli_out("  = 0x%08x",
                            (uint32)BSAND_BYTE_SWAP(sb_test_mem[n].datamask[i]));
                }
                cli_out("\n");

                return BCM_E_FAIL;
            }
            d += ent_words;
            s += ent_words;
        }
        /* If get here, something is wrong, dump for inspection */
        sb_test_dump_data(unit, n, TD_DEFAULT_DUMP_ENTRY_COUNT);
    }

    return SOC_E_NONE;
}


static int
sb_test_mem_cmp_fn(int unit, int n, void *s, void *d)
{
    soc_mem_t mem  = sb_test_mem[n].mem;
    int ent_words  = soc_mem_entry_words(unit, mem);

    assert(s && d);

    return sal_memcmp(s, d, ent_words * 4);
}


static void
sb_test_sbusdma_cb(int unit, int status, sbusdma_desc_handle_t handle,
                   void *data)
{
    int i, cmc, ch;

    if (status != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit,
                              "SBUS DMA failed: type %d\n"),
                   PTR_TO_INT(data)));
        if (status == SOC_E_TIMEOUT)
            (void)soc_sbusdma_desc_delete(unit, handle);
    }
    for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC; i++) {
        if (sbusdma_desc_handle[i] == handle) {
            sbusdma_desc_handle[i] = 0;
            cmc = i / NUM_SBUSDMA_CH_PER_CMC;
            ch  = i % NUM_SBUSDMA_CH_PER_CMC;
            if (soc_sbusdma_ch_try_get(unit, cmc, ch) != SOC_E_NONE) {
                test_error(unit, "Failed to reserved sbusdma from cmc %0d ch %0d\n",
                           cmc, ch);
            }
        }
    }
}


static int
sb_test_sbusdma_setup(int unit, int cmc, int ch, uint32 *entbuf, int index_min,
                      int index_max, int frag, int write)
{
    /* for soc_sbusdma_desc_create and soc_sbusdma_desc_run */
    soc_sbusdma_desc_ctrl_t ctrl;
    soc_sbusdma_desc_cfg_t cfg;
    soc_sbusdma_desc_cfg_t *cfg_array;
    int vchan = cmc * NUM_SBUSDMA_CH_PER_CMC + ch;
    int n = sb_test_data.ch_mem_map[vchan];
    int rv = SOC_E_NONE;
    int         i;
    uint8       at;

    if (n == -1) {
        test_error(unit, "sb_test_sbusdma_setup detected invalid ch_mem_map[%0d]\n",
                   vchan);
        return BCM_E_FAIL;
    }

    soc_sbusdma_ch_put(unit, cmc, ch);
    if (sb_test_data.desc_mode > 0) {
        /* descriptor based sbus dma mode through sdk api */
        sal_memset(&ctrl, 0, sizeof(soc_sbusdma_desc_ctrl_t));
        sal_memset(&cfg, 0, sizeof(soc_sbusdma_desc_cfg_t));
        cfg_array = soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_cfg_t) *
                                  sb_test_data.desc_mode, "sbusdma_desc_cfg");
        if (write == 0) {
            ctrl.flags = SOC_SBUSDMA_MEMORY_CMD_MSG;
        } else {
            ctrl.flags = SOC_SBUSDMA_WRITE_CMD_MSG | SOC_SBUSDMA_MEMORY_CMD_MSG;
        }
        ctrl.cfg_count = sb_test_data.desc_mode;
        ctrl.cb = sb_test_sbusdma_cb;
        ctrl.data = entbuf;
        sal_strncpy(ctrl.name, sb_test_mem[n].mem_str, sizeof(ctrl.name)-1);
        cfg.acc_type = SOC_MEM_ACC_TYPE(unit, sb_test_mem[n].mem);
        cfg.blk = SOC_BLOCK2SCH(unit, sb_test_mem[n].copyno);
        cfg.addr = soc_mem_addr_get(unit, sb_test_mem[n].mem, 0,
                                    sb_test_mem[n].copyno, index_min, &at);
        cfg.width = sb_test_mem[n].entwords;
        cfg.count = index_max - index_min + 1;
        cfg.buff = entbuf;
        cfg.addr_shift = 0;
        for (i = 0; i < sb_test_data.desc_mode; i++) {
            sal_memcpy(cfg_array + i, &cfg, sizeof(soc_sbusdma_desc_cfg_t));
        }

        if (SOC_FAILURE((rv = (soc_sbusdma_desc_create(unit, &ctrl, cfg_array, TRUE,
                                                       &sbusdma_desc_handle[vchan]))))) {
            _SHR_ERROR_TRACE(rv);
        }
        soc_cm_sfree(unit, cfg_array);
    } else {
        /* register based sbus dma mode through sdk api */
        if (write == 0) {
            rv = soc_mem_read_range_multi_cmc(unit, sb_test_mem[n].mem,
                                              sb_test_mem[n].copyno,
                                              index_min, index_max, entbuf,
                                              vchan);
        } else {
            rv = soc_mem_write_range_multi_cmc(unit, sb_test_mem[n].mem,
                                               sb_test_mem[n].copyno,
                                               index_min, index_max, entbuf,
                                               vchan);
        }
        if (rv < 0) {
            test_error(unit, "SBUS SLAM DMA Error:  Memory %s.%d\n",
                       sb_test_mem[n].mem_str, sb_test_mem[n].copyno);
            rv = BCM_E_FAIL;
        }
        if (soc_sbusdma_ch_try_get(unit, cmc, ch) != SOC_E_NONE) {
            test_error(unit, "Failed to reserved sbusdma from cmc %0d ch %0d\n",
                       cmc, ch);
        }
    }
    return rv;
}


static void
sbusdma_write(void* up)
{
    int         unit = PTR_TO_INT(up) & 0xff;
    int         i, j, loop_cnt;
    uint32      *entbuf;
    int         frag;
    int         index_min, index_max;
    sal_usecs_t stime, etime, td;
    int         entry_per_sec, mb_per_sec;
    char        *access_type;

    uint32      avail_bm;
    int         cmc, ch, n;
    sal_usecs_t dma_timeout = 1000000;
    int         ret;

    if (sb_test_data.dma_wr_en) {
        access_type = sal_strdup("SBUS DMA");
    } else {
        access_type = sal_strdup("SBUS    ");
    }
    loop_cnt = 0;
    cmc = 0;
    ch = 0;

    n = PTR_TO_INT(up) >> 8;
    avail_bm = sb_test_mem[n].mem_cmc_ch_bm;
    for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC; i++) {
        if (avail_bm & (0x1 << i)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "writing mem[%0d] with vchan %0d\n"), n, i));
            if (sb_test_data.ch_mem_map[i] != n) {;
                test_error(unit, "sbusdma_write called with more than one memory\n");
            }
        }
    }

    /* If not a RO memory, write random data into table and hardware */
    if (!soc_mem_is_readonly(unit, sb_test_mem[n].mem)) {
        entbuf = sb_test_mem[n].source;
        if (0 != sb_test_data.rnum) {
            sal_memset(entbuf, 0xff, WORDS2BYTES(sb_test_mem[n].bufwords));
        }

        if (sb_test_data.slam_en &&
            soc_mem_slamable(unit, sb_test_mem[n].mem, sb_test_mem[n].copyno)) {
            for (i = 0; i < sb_test_mem[n].count; i++) {
                sb_test_random_entry(unit, sb_test_mem[n].mem, entbuf);
                if (sb_test_mem[n].accum_tcammask) {
                    /* data read back has dependency on mask */
                    if ((SOC_BLOCK_TYPE(unit, sb_test_mem[n].copyno) ==
                            SOC_BLK_ESM) ||
                            (SOC_BLOCK_TYPE(unit, sb_test_mem[n].copyno) ==
                             SOC_BLK_ETU)) {
                        for (j = 0; j < sb_test_mem[n].entwords; j++) {
                            entbuf[j] &= ~sb_test_mem[n].tcammask[j];
                        }
                    } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                        for (j = 0; j < sb_test_mem[n].entwords; j++) {
                            entbuf[j] |= sb_test_mem[n].tcammask[j];
                        }
                    }
                }
                if (sb_test_mem[n].accum_forcemask) {
                    for (j = 0; j < sb_test_mem[n].entwords; j++) {
                        entbuf[j] &= ~sb_test_mem[n].forcemask[j];
                        entbuf[j] |= sb_test_mem[n].forcedata[j];
                    }
                }
                entbuf += sb_test_mem[n].entwords;
            }
            soc_cm_sflush(unit, (void *)sb_test_mem[n].source,
                          WORDS2BYTES(sb_test_mem[n].bufwords));
            stime = sal_time_usecs();
            for (frag = 0; frag < sb_test_mem[n].frag_count; frag++) {
                index_min = sb_test_mem[n].frag_index_min[frag];
                index_max = sb_test_mem[n].frag_index_max[frag];
                entbuf = sb_test_mem[n].source +
                         (sb_test_mem[n].frag_index_min[frag] -
                          sb_test_mem[n].index_min) * sb_test_mem[n].entwords;
                if (soc_mem_write_range(unit, sb_test_mem[n].mem,
                                        sb_test_mem[n].copyno,
                                        (sb_test_data.slam_dir ?
                                         index_min : index_max),
                                        (sb_test_data.slam_dir ?
                                         index_max : index_min),
                                        entbuf) < 0) {
                    test_error(unit,
                               "SBUS SLAM DMA Error:  Memory %s.%d\n",
                               sb_test_mem[n].mem_str, sb_test_mem[n].copyno);
                }
            }
        } else {
            /* fill the source buffer with random values */
            entbuf = sb_test_mem[n].source;
            for (i = sb_test_mem[n].index_min; i <= sb_test_mem[n].index_max; i++) {
                sb_test_random_entry(unit, sb_test_mem[n].mem, entbuf);
                if (sb_test_mem[n].accum_tcammask) {
                    /* data read back has dependency on mask */
                    if ((SOC_BLOCK_TYPE(unit, sb_test_mem[n].copyno) == SOC_BLK_ESM) ||
                        (SOC_BLOCK_TYPE(unit, sb_test_mem[n].copyno) == SOC_BLK_ETU)) {
                        for (j = 0; j < sb_test_mem[n].entwords; j++) {
                            entbuf[j] &= ~sb_test_mem[n].tcammask[j];
                        }
                    } else if (soc_feature(unit, soc_feature_xy_tcam)) {
                        for (j = 0; j < sb_test_mem[n].entwords; j++) {
                            entbuf[j] |= sb_test_mem[n].tcammask[j];
                        }
                    }
                }
                if (sb_test_mem[n].accum_forcemask) {
                    for (j = 0; j < sb_test_mem[n].entwords; j++) {
                        entbuf[j] &= ~sb_test_mem[n].forcemask[j];
                        entbuf[j] |= sb_test_mem[n].forcedata[j];
                    }
                }
                entbuf += sb_test_mem[n].entwords;
            }
            while (sb_test_data.run[n] == FALSE) {
                sal_usleep(10);
            }
            stime = sal_time_usecs();
            while (sb_test_data.run[n]) {
                for (frag = 0; frag < sb_test_mem[n].frag_count; frag++) {
                    index_min = sb_test_mem[n].frag_index_min[frag];
                    index_max = sb_test_mem[n].frag_index_max[frag];
                    entbuf = sb_test_mem[n].source +
                             (sb_test_mem[n].frag_index_min[frag] -
                              sb_test_mem[n].index_min) * sb_test_mem[n].entwords;
                    if (sb_test_data.verbose || sb_test_data.debug) {
                        cli_out("%s writing from address 0x%08x to %s[%0d..%0d]\n",
                                access_type, PTR_TO_INT(entbuf),
                                sb_test_mem[n].mem_str, index_max, index_min);
                    }
                    if (sb_test_data.debug) {
                        for (i = index_min; i <= index_max; i++) {
                            cli_out("DEBUG: Writing %s[%0d]:",
                                    sb_test_mem[n].mem_str, i);
                            for (j = 0; j < sb_test_mem[n].entwords; j++) {
                                cli_out(" 0x%08x", *(entbuf+j));
                            }
                            cli_out("\n");
                        }
                    }
                    if (sb_test_data.dma_wr_en) {
                        /* sbus dma write */
                        for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC; i++) {
                            if (avail_bm & (0x1 << i)) {
                                cmc = i / NUM_SBUSDMA_CH_PER_CMC;
                                ch  = i % NUM_SBUSDMA_CH_PER_CMC;
                                sb_test_sbusdma_setup(unit, cmc, ch, entbuf,
                                                      index_min, index_max,
                                                      frag, 1);
                                avail_bm &= ~(1 << i);
                                break;
                            }
                        }
                    } else {
                        /* schan write */
                        for (cmc = 0; cmc < SOC_CMCS_NUM_MAX + 1; cmc++) {
                            i = cmc * NUM_SBUSDMA_CH_PER_CMC;
                            if ((avail_bm & (0x1 << i)) &&
                                (SCHAN_CH_MASK & (0x1 << cmc))) {
                                avail_bm &= ~(1 << i);
                                break;
                            }
                        }
                        for (i = index_min; i <= index_max; i++) {
                            if (sb_test_write_entry(unit, cmc,
                                                    sb_test_mem[n].mem,
                                                    sb_test_mem[n].copyno, i,
                                                    entbuf) < 0) {
                                test_error(unit, "SCHAN Write Error: Memory %s.%d\n",
                                           sb_test_mem[n].mem_str,
                                           sb_test_mem[n].copyno);
                            }
                            entbuf += sb_test_mem[n].entwords;
                        }
                        avail_bm |= 1 << (cmc * NUM_SBUSDMA_CH_PER_CMC);
                    }
                    if ((avail_bm == 0) || (frag == sb_test_mem[n].frag_count-1)) {
                        /* kick start descriptor mode sbusdma */
                        if (sb_test_data.dma_wr_en && sb_test_data.desc_mode > 0) {
                            soc_timeout_t to;
                            soc_timeout_init(&to, 2 * dma_timeout, 0);
                            do {
                                ret = soc_sbusdma_desc_run(unit,
                                          sbusdma_desc_handle[cmc * NUM_SBUSDMA_CH_PER_CMC + ch]);
                                if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
                                    if (ret == SOC_E_INIT) {
                                        break;
                                    }
                                    if (soc_timeout_check(&to)) {
                                        LOG_WARN(BSL_LS_APPL_TESTS,
                                                 (BSL_META_U(unit,
                                                             " sbusdma desc run operation timeout\n")));
                                        break;
                                    }
                                    sal_usleep(10);
                                }
                            } while ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT));
                            sb_test_data.run[n] = FALSE;
                            /* poll until descriptor mode sbusdma finishes */
                            do {
                                 sal_usleep(10);
                            } while (sbusdma_desc_handle[cmc * NUM_SBUSDMA_CH_PER_CMC + ch] != 0);
                        }
                        if (sb_test_data.dma_wr_en) {
                            avail_bm = sb_test_mem[n].mem_cmc_ch_bm;
                        }
                    }
                }
                loop_cnt++;
            }
        }
        etime = sal_time_usecs();
        td = SAL_USECS_SUB(etime, stime);
        if (sb_test_data.dma_wr_en && sb_test_data.desc_mode > 0) {
            loop_cnt = sb_test_data.desc_mode;
        }
        entry_per_sec = _shr_div_exp10(sb_test_mem[n].count, td, 6) * loop_cnt;
        mb_per_sec = _shr_div_exp10(WORDS2BYTES(sb_test_mem[n].bufwords) /
                                    0x400, td, 10) / 1024 * loop_cnt;
        LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "%s write loop_cnt %0d, entry %0d, time %0d\n"),
                                 sb_test_mem[n].mem_str, loop_cnt,
                                 sb_test_mem[n].count, td));
        if (sb_test_data.verbose)
            cli_out("%s wrote %s.%s: %3d dw by %5d entry %9d entry/s %4d.%04d MB/s\n",
                    access_type, sb_test_mem[n].mem_str,
                    SOC_BLOCK_NAME(unit, sb_test_mem[n].copyno),
                    sb_test_mem[n].entwords, sb_test_mem[n].count,
                    entry_per_sec, INT_FRAC_4PT(mb_per_sec));
        else
            cli_out("%s wrote %s: %9d entry/s %4d.%04d MB/s\n",
                    access_type, sb_test_mem[n].mem_str, entry_per_sec,
                    INT_FRAC_4PT(mb_per_sec));
    } else {
        cli_out("Note:  memory %s is read only, so verifying a few\n",
                sb_test_mem[n].mem_str);
        cli_out("entries directly.\n");
    }

    if (sb_test_data.verbose || sb_test_data.debug) {
        cli_out("Done writing mem[%0d]\n", n);
    }
    sb_test_data.mem_done[n] = TRUE;
    sal_free(access_type);
    sal_thread_exit(0);
}


static void
sbusdma_read(void* up)
{
    int         unit = PTR_TO_INT(up) & 0xff;
    int         i, j, loop_cnt;
    uint32      *entbuf;
    int         frag;
    int         index_min, index_max;
    sal_usecs_t stime, etime, td;
    int         entry_per_sec, mb_per_sec;
    char        *access_type;

    uint32      avail_bm;
    int         cmc, ch, n;
    sal_usecs_t dma_timeout = 1000000;
    int         ret;

    if (sb_test_data.dma_rd_en) {
        access_type = sal_strdup("SBUS DMA");
    } else {
        access_type = sal_strdup("SBUS    ");
    }
    loop_cnt = 0;
    cmc = 0;
    ch = 0;

    n = PTR_TO_INT(up) >> 8;
    avail_bm = sb_test_mem[n].mem_cmc_ch_bm;
    for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC; i++) {
        if (avail_bm & (0x1 << i)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "reading mem[%0d] with vchan %0d\n"), n, i));
            if (sb_test_data.ch_mem_map[i] != n) {;
                test_error(unit, "sbusdma_read called with more than one memory\n");
            }
        }
    }

    while (sb_test_data.run[n] == FALSE) {
        sal_usleep(10);
    }
    stime = sal_time_usecs();
    while (sb_test_data.run[n]) {
        for (frag = 0; frag < sb_test_mem[n].frag_count; frag++) {
            index_min = sb_test_mem[n].frag_index_min[frag];
            index_max = sb_test_mem[n].frag_index_max[frag];
            entbuf = sb_test_mem[n].dma_ed +
                     (sb_test_mem[n].frag_index_min[frag] -
                      sb_test_mem[n].index_min) * sb_test_mem[n].entwords;
            if (sb_test_data.verbose || sb_test_data.debug) {
                cli_out("%s reading from %s[%0d..%0d] to address 0x%08x\n",
                        access_type, sb_test_mem[n].mem_str, index_max,
                        index_min, PTR_TO_INT(entbuf));
            }
            if (sb_test_data.dma_rd_en) {
                for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC; i++) {
                    if (avail_bm & (0x1 << i)) {
                        cmc = i / NUM_SBUSDMA_CH_PER_CMC;
                        ch  = i % NUM_SBUSDMA_CH_PER_CMC;
                        sb_test_sbusdma_setup(unit, cmc, ch, entbuf, index_min,
                                              index_max, frag, 0);
                        avail_bm &= ~(1 << i);
                        break;
                    }
                }
            } else {
                for (cmc = 0; cmc < SOC_CMCS_NUM_MAX + 1; cmc++) {
                    i = cmc * NUM_SBUSDMA_CH_PER_CMC;
                    if ((avail_bm & (0x1 << i)) &&
                        (SCHAN_CH_MASK & (0x1 << cmc))) {
                        avail_bm &= ~(1 << i);
                        break;
                    }
                }
                for (i = index_min; i <= index_max; i++) {
                    if (sb_test_read_entry(unit, cmc, sb_test_mem[n].mem,
                                           sb_test_mem[n].copyno, i, entbuf) < 0) {
                        test_error(unit, "SCHAN Read Error: Memory %s.%d\n",
                                   sb_test_mem[n].mem_str, sb_test_mem[n].copyno);
                    }
                    entbuf += sb_test_mem[n].entwords;
                }
                avail_bm |= 1 << (cmc * NUM_SBUSDMA_CH_PER_CMC);
            }
            if (sb_test_data.debug) {
                entbuf = sb_test_mem[n].dma_ed +
                         sb_test_mem[n].frag_index_min[frag] *
                         sb_test_mem[n].entwords;
                for (i = index_min; i <= index_max; i++) {
                    cli_out("DEBUG: %s[%0d]:", sb_test_mem[n].mem_str, i);
                    for (j = 0; j < sb_test_mem[n].entwords; j++) {
                        cli_out(" 0x%08x", *(entbuf+i*sb_test_mem[n].entwords+j));
                    }
                    cli_out("\n");
                }
            }
            if ((avail_bm == 0) || (frag == sb_test_mem[n].frag_count-1)) {
                if (sb_test_data.dma_rd_en && sb_test_data.desc_mode > 0) {
                    soc_timeout_t to;
                    soc_timeout_init(&to, 2 * dma_timeout, 0);
                    do {
                        ret = soc_sbusdma_desc_run(unit,
                                  sbusdma_desc_handle[cmc * NUM_SBUSDMA_CH_PER_CMC + ch]);
                        if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
                            if (ret == SOC_E_INIT) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                LOG_WARN(BSL_LS_APPL_TESTS,
                                         (BSL_META_U(unit,
                                                     " sbusdma desc run operation timeout\n")));
                                break;
                            }
                            sal_usleep(10);
                        }
                    } while ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT));
                    sb_test_data.run[n] = FALSE;
                    /* poll until descriptor mode sbusdma finishes */
                    do {
                         sal_usleep(10);
                    } while (sbusdma_desc_handle[cmc * NUM_SBUSDMA_CH_PER_CMC + ch] != 0);
                }
                if (sb_test_data.dma_rd_en) {
                    avail_bm = sb_test_mem[n].mem_cmc_ch_bm;
                }
            }
        }
        loop_cnt++;
    }
    etime = sal_time_usecs();
    td = SAL_USECS_SUB(etime, stime);
    if (sb_test_data.dma_rd_en && sb_test_data.desc_mode > 0) {
        loop_cnt = sb_test_data.desc_mode;
    }
    entry_per_sec = _shr_div_exp10(sb_test_mem[n].count, td, 6) * loop_cnt;
    mb_per_sec = _shr_div_exp10(WORDS2BYTES(sb_test_mem[n].bufwords) / 0x400,
                                td, 10) / 1024 * loop_cnt;
    LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "%s read loop_cnt %0d, entry %0d, time %0d\n"),
                             sb_test_mem[n].mem_str, loop_cnt,
                             sb_test_mem[n].count, td));
    if (sb_test_data.verbose)
        cli_out("%s read  %s.%s: %3d dw by %5d entry %9d entry/s %4d.%04d MB/s\n",
                access_type, sb_test_mem[n].mem_str,
                SOC_BLOCK_NAME(unit, sb_test_mem[n].copyno),
                sb_test_mem[n].entwords, sb_test_mem[n].count,
                entry_per_sec, INT_FRAC_4PT(mb_per_sec));
    else
        cli_out("%s read  %s: %9d entry/s %4d.%04d MB/s\n",
                access_type, sb_test_mem[n].mem_str, entry_per_sec,
                INT_FRAC_4PT(mb_per_sec));

    if (sb_test_data.verbose || sb_test_data.debug) {
        cli_out("Done reading mem[%0d]\n", n);
    }
    sb_test_data.mem_done[n] = TRUE;
    sal_free(access_type);
    sal_thread_exit(0);
}


static void
reserve_all_schan_channels(int unit)
{
    int cmc;
    int rv = SOC_E_NONE;

    for (cmc = 0; cmc < SOC_CMCS_NUM_MAX + 1; cmc++) {
        if (SCHAN_CH_MASK & (0x01 << cmc)) {
            rv = cmicm_schan_ch_try_get(unit, cmc);
            if (rv != SOC_E_NONE) {
                test_error(unit, "Failed to reserved schan from cmc %0d\n", cmc);
            }
        }
    }
    if (rv == SOC_E_NONE) {
        cli_out("Reserved all SCHANs successfully\n");
    }
}


static void
release_all_schan_channels(int unit)
{
    int cmc;

    for (cmc = 0; cmc < SOC_CMCS_NUM_MAX + 1; cmc++) {
        if (SCHAN_CH_MASK & (0x01 << cmc)) {
            cmicm_schan_ch_put(unit, cmc);
        }
    }
}


static void
reserve_all_sbusdma_channels(int unit)
{
    int cmc, ch;
    int rv = SOC_E_NONE;

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < NUM_SBUSDMA_CH_PER_CMC; ch++) {
            rv = soc_sbusdma_ch_try_get(unit, cmc, ch);
            if (rv != SOC_E_NONE) {
                test_error(unit, "Failed to reserved sbusdma from cmc %0d ch %0d\n",
                           cmc, ch);
            }
        }
    }
    if (rv == SOC_E_NONE) {
        cli_out("Reserved all SBUSDMA channels successfully\n");
    }
}


static void
release_all_sbusdma_channels(int unit)
{
    int cmc, ch;

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < NUM_SBUSDMA_CH_PER_CMC; ch++) {
            soc_sbusdma_ch_put(unit, cmc, ch);
        }
    }
}


void
sb_test_write(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int n, done = FALSE;
    char str[80];
    /* sal_thread_t pid_sbusdma[SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC]; */
    int ch_max;
    sal_thread_t *pid_sbusdma;

    ch_max = SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC;
    pid_sbusdma = sal_alloc(sizeof(sal_thread_t) * ch_max, "pid_sbusdma");
    sal_memset(pid_sbusdma, 0, sizeof(sal_thread_t) * ch_max);

    /* sbus dma write */
    if (!soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
        sb_test_data.desc_mode == 0) {
        sb_test_data.cmc_ch_bm = 0x1 << soc->tslam_ch;
    }
    for (n = 0; n < sb_test_data.num_mem && n < ch_max; n++) {
        sal_sprintf(str, "SBUSDMA_MEM_%0d", n);
        sb_test_data.mem_done[n] = FALSE;
        pid_sbusdma[n] = sal_thread_create(str, 32 * 1024 * 1024, 200,
                                           sbusdma_write,
                                           INT_TO_PTR(n << 8 | unit));
        if (sb_test_data.verbose || sb_test_data.debug) {
            cli_out("\npid_sbusdma[%0d] = %p", n, pid_sbusdma[n]);
        }
        sal_usleep(10);
    }
    for (n = 0; n < sb_test_data.num_mem; n++) {
        sb_test_data.run[n] = TRUE;
    }
    if (sb_test_data.desc_mode == 0) {
        sal_sleep(sb_test_data.rate_calc_int);
        for (n = 0; n < sb_test_data.num_mem; n++) {
            sb_test_data.run[n] = FALSE;
        }
    }
    do {
        sal_usleep(100000);
        for (n = 0; n < sb_test_data.num_mem; n++) {
            if (n == 0) {
                done = sb_test_data.mem_done[n];
            } else {
                done &= sb_test_data.mem_done[n];
            }
        }
    } while (done == FALSE);
    if (sb_test_data.verbose || sb_test_data.debug) {
        cli_out("Finished waiting for sbusdma_write done\n");
    }
}


int
sb_test_read(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int n, done = FALSE;
    int rv = SOC_E_NONE;
    char str[80];
    /* sal_thread_t pid_sbusdma[SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC]; */
    int ch_max;
    sal_thread_t *pid_sbusdma;

    ch_max = SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC;
    pid_sbusdma = sal_alloc(sizeof(sal_thread_t) * ch_max, "pid_sbusdma");
    sal_memset(pid_sbusdma, 0, sizeof(sal_thread_t) * ch_max);

    /* sbus dma read */
    if (!soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
        sb_test_data.desc_mode == 0) {
        sb_test_data.cmc_ch_bm = 0x1 << soc->tdma_ch;
    }
    for (n = 0; n < sb_test_data.num_mem && n < ch_max; n++) {
        sal_sprintf(str, "SBUSDMA_MEM_%0d", n);
        sb_test_data.mem_done[n] = FALSE;
        pid_sbusdma[n] = sal_thread_create(str, 32 * 1024 * 1024, 200,
                                           sbusdma_read,
                                           INT_TO_PTR(n << 8 | unit));
        if (sb_test_data.verbose || sb_test_data.debug) {
            cli_out("\npid_sbusdma[%0d] = %p", n, pid_sbusdma[n]);
        }
        sal_usleep(10);
    }
    for (n = 0; n < sb_test_data.num_mem; n++) {
        sb_test_data.run[n] = TRUE;
    }
    if (sb_test_data.desc_mode == 0) {
        sal_sleep(sb_test_data.rate_calc_int);
        for (n = 0; n < sb_test_data.num_mem; n++) {
            sb_test_data.run[n] = FALSE;
        }
    }
    do {
        sal_usleep(100000);
        for (n = 0; n < sb_test_data.num_mem; n++) {
            if (n == 0) {
                done = sb_test_data.mem_done[n];
            } else {
                done &= sb_test_data.mem_done[n];
            }
        }
    } while (done == FALSE);
    if (sb_test_data.verbose || sb_test_data.debug) {
        cli_out("Finished waiting for sbusdma_read done\n");
    }

    if (sb_test_data.dma_wr_en || sb_test_data.dma_rd_en) {
        release_all_sbusdma_channels(unit);
    }
    if (!sb_test_data.dma_wr_en || !sb_test_data.dma_rd_en) {
        if (soc_feature(unit, soc_feature_cmicm)) {
            release_all_schan_channels(unit);
        }
    }

    /* verify data integity */
    if (sb_test_data.check_data == TRUE) {
        for (n = 0; n < sb_test_data.num_mem; n++) {
            if ((rv = sb_test_verify(unit, n)) < 0) {
                test_error(unit, "SBUS DMA failed: inconsistency observed\n");
                if (!sb_test_data.debug) {
                    sb_test_dump_data(unit, n, TD_DEFAULT_DUMP_ENTRY_COUNT);
                }
            }
        }
    }

    return rv;
}


int
sb_test_main(int unit, args_t *a, void *pa)
{
    int rv = SOC_E_NONE;

    if (sb_test_data.dma_wr_en || sb_test_data.dma_rd_en) {
        reserve_all_sbusdma_channels(unit);
    }
    if (!sb_test_data.dma_wr_en || !sb_test_data.dma_rd_en) {
        if (soc_feature(unit, soc_feature_cmicm)) {
            reserve_all_schan_channels(unit);
        }
    }

    if (sb_test_data.test_mode & 0x1) {
        sb_test_write(unit);
    }
    if (sb_test_data.test_mode & 0x2) {
        rv = sb_test_read(unit);
    }

    return rv;
}


char sb_test_usage[] =
    "SBUS DMA test usage:\n"
    " \n"
    "NumMem=<int>        -  number of table to be tested; default is 1\n"
    "Mem=<name>          -  name of the first table to be tested; default is L2X\n"
    "Mem<int>=<name>     -  name of tables to be tested\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "\nFull documentation cannot be displayed with -pedantic compiler\n";
#else
    "Start=<int>         -  first entry to be tested; default is 0\n"
    "Count=<int>         -  number of entries to be tested; default is the table size\n"
    "Frag=<1..32768>     -  split the table SBUS DMA in fragment; default is 1\n"
    "                       CmcChanBitmap has priority over frag\n"
    "RateCalcInt=<int>   -  Interval in seconds over which rate is to be calculated.\n"
    "Seed=<int>          -  random seed for the random write values\n"
    "EccAsData=<1/0>     -  do not mask off the ECC field for testing; default is 0\n"
    "Desc=<int>          -  descriptor mode chain length; default is register mode\n"
    "CmcChanBitmap=<hex> -  specify which of the 9 available CMC/channels to use\n"
    "                       default is 2 for descriptor based write/read, 1 for\n"
    "                       register based write, and 0 for register based read\n"
    "                       CmcChanBitmap has priority over frag\n"
    "DmaWrEn=<1/0>       -  enable SBUS DMA write. Otherwise SCHAN; default is 1\n"
    "DmaRdEn=<1/0>       -  enable SBUS DMA write. Otherwise SCHAN; default is 1\n"
    "TestMode=<1/2/3>    -  1: write only; 2: read only; default is 3 write and read\n"
    "Verbose=<1/0>       -  print more debug information; default is 0\n"
    ;
#endif


int
sb_test_init(int unit, args_t *a, void **pa)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    parse_table_t pt;
    int i, n, cmc;
    int index_min, index_max;
    int count_arg;
    int frag, delta;
    int big_packet;
    uint32 addr_remap;

    if (sb_test_data_init(unit, 0) < 0) {
        return BCM_E_FAIL;
    }

    SOC_IF_ERROR_RETURN(soc_counter_stop(unit));
    if (soc_feature(unit, soc_feature_arl_hashed)) {
        SOC_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }

    parse_table_init(unit, &pt);
    sb_test_data.cmc_ch_bm = 0;
    sb_test_mem[0].frag_count = 0;

    parse_table_add(&pt, "NumMem", PQ_INT|PQ_DFL, 0,
                    &sb_test_data.num_mem, NULL);
    parse_table_add(&pt, "Mem", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[0].mem_str, NULL);
    parse_table_add(&pt, "Mem0", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[0].mem_str, NULL);
    parse_table_add(&pt, "Mem1", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[1].mem_str, NULL);
    parse_table_add(&pt, "Mem2", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[2].mem_str, NULL);
    parse_table_add(&pt, "Mem3", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[3].mem_str, NULL);
    parse_table_add(&pt, "Mem4", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[4].mem_str, NULL);
    parse_table_add(&pt, "Mem5", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[5].mem_str, NULL);
    parse_table_add(&pt, "Mem6", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[6].mem_str, NULL);
    parse_table_add(&pt, "Mem7", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[7].mem_str, NULL);
    parse_table_add(&pt, "Mem8", PQ_DFL|PQ_STRING|PQ_STATIC, 0,
                    &sb_test_mem[8].mem_str, NULL);
    parse_table_add(&pt, "Start", PQ_DFL|PQ_INT, 0,
                    &sb_test_mem[0].index_min, NULL);
    parse_table_add(&pt, "Count", PQ_DFL|PQ_INT, (void *)(-1),
                    &count_arg, NULL);
    parse_table_add(&pt, "Frag", PQ_DFL|PQ_INT, (void *)(-1),
                    &sb_test_mem[0].frag_count, NULL);
    parse_table_add(&pt, "RateCalcInt", PQ_INT|PQ_DFL, 0,
                    &sb_test_data.rate_calc_int, NULL);
    parse_table_add(&pt, "Seed", PQ_INT|PQ_DFL, 0,
                    &sb_test_data.rnum, NULL);
    parse_table_add(&pt, "EccAsData", PQ_BOOL|PQ_DFL, (void *)1,
                    &sb_test_data.ecc_as_data, NULL);
    parse_table_add(&pt, "Desc", PQ_INT|PQ_DFL, 0,
                    &sb_test_data.desc_mode, NULL);
    parse_table_add(&pt, "CmcChanBitmap", PQ_HEX|PQ_DFL, 0,
                    &sb_test_data.cmc_ch_bm, NULL);
    parse_table_add(&pt, "DmaWrEn", PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.dma_wr_en, NULL);
    parse_table_add(&pt, "DmaRdEn", PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.dma_rd_en, NULL);
    parse_table_add(&pt, "TestMode", PQ_INT|PQ_DFL, 0,
                    &sb_test_data.test_mode, NULL);
    parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.verbose, NULL);
    parse_table_add(&pt, "debug", PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.debug, NULL);
    parse_table_add(&pt, "SlamEnable", PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.slam_en, NULL);
    parse_table_add(&pt, "SlamLowToHigh", PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.slam_dir, NULL);
    parse_table_add(&pt, "CheckData",   PQ_BOOL|PQ_DFL, 0,
                    &sb_test_data.check_data, NULL);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_msg("%s", sb_test_usage);
        test_error(unit,
                   "%s: Invalid option: %s\n", ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return BCM_E_FAIL;
    }

    if (soc_feature(unit, soc_feature_cmicm) &&
        !soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if ((sb_test_data.num_mem > 1)) {
            test_error(unit, "Cannot test %0d memories in parallel since cmicm_multi_dma_cmc is disabled.\n",
                       sb_test_data.num_mem);
            parse_arg_eq_done(&pt);
            return BCM_E_FAIL;
        }
    } else {
        if ((sb_test_data.num_mem < 1) ||
            (sb_test_data.num_mem > SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC)) {
            test_error(unit, "NumMem \"%d\" is invalid\n",
                       sb_test_data.num_mem);
            parse_arg_eq_done(&pt);
            return BCM_E_FAIL;
        }
    }

    for (n = 0; n < sb_test_data.num_mem; n++) {
        if (parse_memory_name(unit, &sb_test_mem[n].mem, sb_test_mem[n].mem_str,
                              &sb_test_mem[n].copyno, 0) < 0) {
            test_error(unit, "Memory \"%s\" is invalid\n",
                       sb_test_mem[n].mem_str);
            parse_arg_eq_done(&pt);
            return BCM_E_FAIL;
        }

        if (count_arg > -1) {
            sb_test_mem[n].count = count_arg;
        }

        if (sb_test_mem[n].copyno == COPYNO_ALL) {
            sb_test_mem[n].copyno = SOC_MEM_BLOCK_ANY(unit, sb_test_mem[n].mem);
        }
        if (!SOC_MEM_BLOCK_VALID(unit, sb_test_mem[n].mem, sb_test_mem[n].copyno)) {
            test_error(unit,
                       "Invalid copyno %d specified in %s\n",
                       sb_test_mem[n].copyno, sb_test_mem[n].mem_str);
            parse_arg_eq_done(&pt);
            return BCM_E_FAIL;
        }

        index_min = soc_mem_index_min(unit, sb_test_mem[n].mem);
        index_max = soc_mem_index_max(unit, sb_test_mem[n].mem);

        if (sb_test_mem[n].index_min <= index_min) {
            sb_test_mem[n].index_min = index_min;
        }
        if (sb_test_mem[n].index_min > index_max) {
            cli_out("Min index out of range: %d\n",  sb_test_mem[n].index_min);
            sb_test_mem[n].index_min = index_min;
            cli_out("Changed to %d\n", sb_test_mem[n].index_min);
        }

        if (sb_test_mem[n].count <= 0) {
            /*
             * do not use soc_mem_index_count, index_max may have been,
             * index_max may have been modified above
             */
            sb_test_mem[n].count = index_max - index_min + 1;
        }

        if (sb_test_mem[n].index_min + sb_test_mem[n].count - 1 > index_max) {
            sb_test_mem[n].count = index_max - sb_test_mem[n].index_min + 1;
            if (sb_test_data.verbose || sb_test_data.debug) {
                cli_out("Reduced the count to %d\n", sb_test_mem[n].count);
            }
        }

        if (sb_test_mem[n].count == 0) {
            if (soc_feature(unit, soc_feature_esm_support)) {
                if ((sb_test_mem[n].mem == L3_DEFIPm && SOC_MEM_IS_ENABLED(unit, L3_DEFIPm)) ||
                    (sb_test_mem[n].mem == L3_DEFIP_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_ONLYm)) ||
                    (sb_test_mem[n].mem == L3_DEFIP_DATA_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_DATA_ONLYm)) ||
                    (sb_test_mem[n].mem == L3_DEFIP_HIT_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_DATA_ONLYm)) ||
                    (sb_test_mem[n].mem == L3_DEFIP_PAIR_128_DATA_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_PAIR_128_DATA_ONLYm)) ||
                    (sb_test_mem[n].mem == L3_DEFIP_PAIR_128_HIT_ONLYm && SOC_MEM_IS_ENABLED(unit, L3_DEFIP_PAIR_128_HIT_ONLYm))) {
                    /* The above internal memories will not be supported if External TCAM is present.
                    These internal memories are set to 0 and hence can not run the test on these memories.*/
                    return BCM_E_UNAVAIL;
                } else {
                    test_error(unit,
                               "Cannot test memory %s:  No entries.\n",
                               SOC_MEM_UFNAME(unit, sb_test_mem[n].mem));
                    parse_arg_eq_done(&pt);
                    return BCM_E_FAIL;
                }
            } else {
                test_error(unit,
                           "Cannot test memory %s:  No entries.\n",
                           SOC_MEM_UFNAME(unit, sb_test_mem[n].mem));
                parse_arg_eq_done(&pt);
                return BCM_E_FAIL;
            }
        }

        sb_test_mem[n].index_max = sb_test_mem[n].index_min +
                                 sb_test_mem[n].count - 1;

        if (sb_test_mem[n].frag_count <= sb_test_data.num_mem) {
            sb_test_mem[n].frag_count = sb_test_data.num_mem;
            sb_test_mem[n].frag_index_min[0] = sb_test_mem[n].index_min;
            sb_test_mem[n].frag_index_max[0] = sb_test_mem[n].index_max;
        } else {
            if (sb_test_mem[n].frag_count > sb_test_mem[n].count) {
                sb_test_mem[n].frag_count = sb_test_mem[n].count;
                cli_out("Reduced the fragment count to %d\n",
                        sb_test_mem[n].frag_count);
            }
            if (sb_test_mem[n].frag_count > 32768) {
                sb_test_mem[n].frag_count = 32768;
                cli_out("Reduced the fragment count to %d\n",
                        sb_test_mem[n].frag_count);
            }
            delta = (sb_test_mem[n].index_max - sb_test_mem[n].index_min) /
                    sb_test_mem[n].frag_count;
            for (frag = 0; frag < sb_test_mem[n].frag_count; frag++) {
                sb_test_mem[n].frag_index_min[frag] = sb_test_mem[n].index_min +
                                                    frag * delta;
                sb_test_mem[n].frag_index_max[frag] = sb_test_mem[n].index_min +
                                                    (frag + 1) * delta - 1;
            }
            sb_test_mem[n].frag_index_max[sb_test_mem[n].frag_count - 1] =
                sb_test_mem[n].index_max;
        }
    }

    if (soc_feature(unit, soc_feature_cmicm)) {
        if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
            if (sb_test_mem[0].frag_count > SOC_PCI_CMCS_NUM(unit) *
                                            NUM_SBUSDMA_CH_PER_CMC) {
                sb_test_data.cmc_ch_bm = 0x777;
            } else if (sb_test_data.cmc_ch_bm == 0) {
                switch (sb_test_mem[0].frag_count) {
                    case 1: sb_test_data.cmc_ch_bm =   0x1; break;
                    case 2: sb_test_data.cmc_ch_bm =  0x11; break;
                    case 3: sb_test_data.cmc_ch_bm = 0x111; break;
                    case 4: sb_test_data.cmc_ch_bm = 0x113; break;
                    case 5: sb_test_data.cmc_ch_bm = 0x133; break;
                    case 6: sb_test_data.cmc_ch_bm = 0x333; break;
                    case 7: sb_test_data.cmc_ch_bm = 0x733; break;
                    case 8: sb_test_data.cmc_ch_bm = 0x773; break;
                    case 9: sb_test_data.cmc_ch_bm = 0x777; break;
                }
            }
        } else {
            if (sb_test_data.desc_mode > 0) {
                sb_test_data.cmc_ch_bm = 0x1 << soc->desc_ch;
            } else {
                sb_test_data.cmc_ch_bm = 0x1 << soc->tdma_ch |
                                         0x1 <<soc->tslam_ch;
            }
        }
    } else {
        if (sb_test_mem[0].frag_count > SOC_PCI_CMCS_NUM(unit) *
                                        NUM_SBUSDMA_CH_PER_CMC) {
            sb_test_data.cmc_ch_bm = 0x77;
        } else if (sb_test_data.cmc_ch_bm == 0) {
            switch (sb_test_mem[0].frag_count) {
                case 1: sb_test_data.cmc_ch_bm =  0x1; break;
                case 2: sb_test_data.cmc_ch_bm = 0x11; break;
                case 3: sb_test_data.cmc_ch_bm = 0x13; break;
                case 4: sb_test_data.cmc_ch_bm = 0x33; break;
                case 5: sb_test_data.cmc_ch_bm = 0x37; break;
                case 6: sb_test_data.cmc_ch_bm = 0x77; break;
            }
        }
    }

    frag = 0;
    for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC + 1; i++) {
        if (sb_test_data.cmc_ch_bm & (0x1 << i)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Testing CMC %0d channel %0d\n"), i / NUM_SBUSDMA_CH_PER_CMC,
                             i % NUM_SBUSDMA_CH_PER_CMC));
            frag++;
        }
    }
    if (sb_test_data.num_mem > 1) {
        sb_test_mem[0].frag_count = 1;
    } else if (frag > sb_test_mem[0].frag_count) {
        sb_test_mem[0].frag_count = frag;
    }

    if (frag < sb_test_data.num_mem) {
        test_error(unit,"%0d CMC channels insufficient to test %0d memories\n",
                   frag, sb_test_data.num_mem);
        return BCM_E_FAIL;
    }
    for (i = 0; i < sb_test_data.num_mem; i++) {
        sb_test_mem[i].mem_cmc_ch_bm = 0;
    }
    frag = 0;
    for (i = 0; i < SOC_PCI_CMCS_NUM(unit) * NUM_SBUSDMA_CH_PER_CMC + 1; i++) {
        if (sb_test_data.cmc_ch_bm & (0x1 << i)) {
            sb_test_data.ch_mem_map[i] = frag % sb_test_data.num_mem;
            sb_test_mem[sb_test_data.ch_mem_map[i]].mem_cmc_ch_bm |= 0x1 << i;
                LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "sb_test_data.ch_mem_map[%0d] = 0x%x\n"),
                                 i, sb_test_data.ch_mem_map[i]));
                LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "sb_test_mem[%0d].mem_cmc_ch_bm = 0x%x\n"),
                                 sb_test_data.ch_mem_map[i],
                                 sb_test_mem[sb_test_data.ch_mem_map[i]].mem_cmc_ch_bm));
            frag++;
        } else {
            sb_test_data.ch_mem_map[i] = -1;
        }
    }

    soc_endian_get(unit, &sb_test_data.big_pio, &big_packet, &sb_test_data.big_endian);

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
        soc_feature(unit, soc_feature_cmicm)) {
        for (cmc = 1; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            if ((sb_test_data.cmc_ch_bm &
                 (CMIC_CMC0_SBUSDMA_CH_MASK << (cmc * NUM_SBUSDMA_CH_PER_CMC))) != 0) {
                addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(0));
                soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc),
                              addr_remap);
                addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(0));
                soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc),
                              addr_remap);
                addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(0));
                soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc),
                              addr_remap);
                addr_remap = soc_pci_read(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(0));
                soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc),
                              addr_remap);
            }
        }
    }

    parse_arg_eq_done(&pt);

    if (!SOC_IS_XGS_SWITCH(unit)) {
        test_error(unit, "Chip type not supported for SBUS DMA test\n");
    }

    for (n = 0; n < sb_test_data.num_mem; n++) {
        if (n > 0) {
            sb_test_mem[n].frag_count = 1;
        }
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "SBUS DMA %s: copy %d, from entry %d for %d entries SEED = %d\n"),
                  sb_test_mem[n].mem_str,
                  sb_test_mem[n].copyno,
                  sb_test_mem[n].index_min,
                  sb_test_mem[n].count,
                  sb_test_data.rnum));
        if (!soc_mem_dmaable(unit, sb_test_mem[n].mem, sb_test_mem[n].copyno)) {
            cli_out("WARNING: DMA will not be used for memory %s.%d.\n",
                    sb_test_mem[n].mem_str, sb_test_mem[n].copyno);
        }
    }

    /* integrity check: For read-only mem, ignore integrity check.
     *                  Because there is no writing operation on mem.
     */
    if (sb_test_data.test_mode == 2) {
        sb_test_data.check_data = FALSE;
    }

    if (sb_test_data_init(unit, 1) < 0) {
        test_error(unit, "Test initialization failed\n");
    }

    return SOC_E_NONE;
}

int
sb_test_done(int unit, void *pa)
{
    sb_test_data_clear(unit);

    return SOC_E_NONE;
}

#endif /* BCM_ESW_SUPPORT && BCM_CMICM_SUPPORT BCM_SBUSDMA_SUPPORT */
