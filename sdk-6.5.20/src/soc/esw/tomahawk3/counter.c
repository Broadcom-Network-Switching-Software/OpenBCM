/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>

#include <soc/tomahawk.h>
#include <soc/tomahawk3.h>


#ifdef BCM_TOMAHAWK3_SUPPORT

/* TH3 MMU queue number are different for each pipe */
int th3_num_queues_pipe[] = {276, 240, 228, 228, 228, 240, 240, 240};

static soc_reg_t th3_obm_drop_pkt_regs[][4] = {
    {IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr,
     IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr
    },
    {IDB_OBM1_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM1_LOSSY_HI_PKT_DROP_COUNTr,
     IDB_OBM1_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM1_LOSSLESS1_PKT_DROP_COUNTr
    },
    {IDB_OBM2_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM2_LOSSY_HI_PKT_DROP_COUNTr,
     IDB_OBM2_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM2_LOSSLESS1_PKT_DROP_COUNTr
    },
    {IDB_OBM3_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM3_LOSSY_HI_PKT_DROP_COUNTr,
     IDB_OBM3_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM3_LOSSLESS1_PKT_DROP_COUNTr
    },
    {IDB_OBM4_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM4_LOSSY_HI_PKT_DROP_COUNTr,
     IDB_OBM4_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM4_LOSSLESS1_PKT_DROP_COUNTr
    }
};

static soc_reg_t th3_obm_drop_byte_regs[][4] = {
    {IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr,
     IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr
    },
    {IDB_OBM1_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM1_LOSSY_HI_BYTE_DROP_COUNTr,
     IDB_OBM1_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM1_LOSSLESS1_BYTE_DROP_COUNTr
    },
    {IDB_OBM2_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM2_LOSSY_HI_BYTE_DROP_COUNTr,
     IDB_OBM2_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM2_LOSSLESS1_BYTE_DROP_COUNTr
    },
    {IDB_OBM3_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM3_LOSSY_HI_BYTE_DROP_COUNTr,
     IDB_OBM3_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM3_LOSSLESS1_BYTE_DROP_COUNTr
    }
};

static soc_reg_t th3_obm_fc_event_regs[] = {
    IDB_OBM0_FLOW_CONTROL_EVENT_COUNTr,
    IDB_OBM1_FLOW_CONTROL_EVENT_COUNTr,
    IDB_OBM2_FLOW_CONTROL_EVENT_COUNTr,
    IDB_OBM3_FLOW_CONTROL_EVENT_COUNTr
};

int soc_counter_tomahawk3_obm_drop_to_phy_port(int unit, int pipe, soc_reg_t reg, int obm_port) {
    int i, j, phy_port = -1;

    for(i = 0; i < _TH3_PBLKS_PER_PIPE; i++) {
        for(j = 0; j < 4; j++) {
            if (reg == SOC_REG_UNIQUE_ACC(unit, th3_obm_drop_pkt_regs[i][j])[pipe] ||
                reg == SOC_REG_UNIQUE_ACC(unit, th3_obm_drop_byte_regs[i][j])[pipe] ||
                reg == SOC_REG_UNIQUE_ACC(unit, th3_obm_fc_event_regs[i])[pipe]) {
                phy_port = (pipe * _TH3_PORTS_PER_PBLK * _TH3_PBLKS_PER_PIPE) +
                           (i * _TH3_PORTS_PER_PBLK) + obm_port + 1;
                return phy_port;
            }
        }
    }

    return phy_port;
}

STATIC int
soc_counter_th3_extra_ctrs_init(int unit, soc_reg_t id,
                                soc_counter_non_dma_t *non_dma_parent,
                                soc_counter_non_dma_t *non_dma_extra,
                                uint32 extra_ctr_ct,
                                int *total_entries)
{
    soc_mem_t base_mem = INVALIDm;
    int rv = SOC_E_NONE, i, pipe, count = 0;
    int parent_base_index = 0, num_entries = 0;
    int entry_words = 0, dma_offset = 0;
    int entries_per_pipe[_TH3_PIPES_PER_DEV];
    uint8 itm_mem = FALSE;
    soc_info_t *si = &SOC_INFO(unit);

    /* DO SANITY CHECK FOR INPUT PARAMs */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        rv = SOC_E_PARAM;
        goto _err_soc_counter_th3_extra_ctrs_init;
    }

    if(si == NULL) {
        rv = SOC_E_UNIT;
        goto _err_soc_counter_th3_extra_ctrs_init;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    switch (id) {
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC:
        case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
            itm_mem = TRUE;

            base_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, non_dma_parent->mem, 0);
            if(base_mem == INVALIDm) {
                rv = SOC_E_INTERNAL;
                goto _err_soc_counter_th3_extra_ctrs_init;
            }
            entry_words = soc_mem_entry_words(unit, base_mem);
            count = extra_ctr_ct;
            break;
        default:
            rv = SOC_E_INTERNAL;
            goto _err_soc_counter_th3_extra_ctrs_init;
    }

    switch(id) {
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
        case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
            for(i = 0; i < _TH3_PIPES_PER_DEV; i++) {
                entries_per_pipe[i] = th3_num_queues_pipe[i];
            }
            break;
        default:
            for(i = 0; i < _TH3_PIPES_PER_DEV; i++) {
                entries_per_pipe[i] =
                    soc_mem_index_count(unit, base_mem) / _TH3_PIPES_PER_DEV;
            }
            break;
    }

    if (extra_ctr_ct > count) {
        rv = SOC_E_INTERNAL;
        goto _err_soc_counter_th3_extra_ctrs_init;
    }

    for (i = 0; i < count; i++) {
        if (itm_mem == TRUE) {
            /* Inherit few info from the Parent */
            non_dma_extra->flags = (non_dma_parent->flags &
                                    ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                    _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

            non_dma_extra->cname = non_dma_parent->cname;
            non_dma_extra->field = non_dma_parent->field;
            non_dma_extra->reg = non_dma_parent->reg;
            non_dma_extra->entries_per_port = non_dma_parent->entries_per_port;
            non_dma_extra->id = non_dma_parent->id;

            /* Set Child attributes */
            non_dma_extra->base_index = parent_base_index + *total_entries;

            num_entries = 0;
            non_dma_extra->num_valid_pipe = 0;
            /* This for block initialize dma config that can be used in EM 
               half-chip setup by DMA a table per pipe. */
            for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
                if(!SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe)) &&
                   (si->itm_map & (1 << i))) {
                    non_dma_extra->dma_mem[pipe] =
                        SOC_MEM_UNIQUE_ACC_ITM(unit, non_dma_parent->mem, i);
                    non_dma_extra->num_valid_pipe++;
                } else {
                    non_dma_extra->dma_mem[pipe] = INVALIDm;
                }
                non_dma_extra->dma_index_min[pipe] = num_entries;
                non_dma_extra->dma_index_max[pipe] = num_entries + entries_per_pipe[pipe] - 1;
                non_dma_extra->dma_num_entries[pipe] = entries_per_pipe[pipe];
                non_dma_extra->dma_buf[pipe] = non_dma_parent->dma_buf[0] +
                                               dma_offset +
                                               (num_entries * entry_words);
                num_entries += entries_per_pipe[pipe];
            }

            /* When th3_em_halfchip is not set (the normal case), we can DMA the 
               whole table at once for performance  */
            if(SOC_CONTROL(unit)->halfchip == 0) {
                non_dma_extra->dma_index_max[0] = num_entries - 1;
                for (pipe = 1; pipe < _TH3_PIPES_PER_DEV; pipe++) {
                    non_dma_extra->dma_mem[pipe] = INVALIDm;
                }
            }
            *total_entries += num_entries;
            dma_offset += (num_entries * entry_words);
            non_dma_extra->mem = non_dma_parent->mem;
            non_dma_extra->num_entries = num_entries;

            non_dma_extra++;
            /* coverity[check_after_deref] */
            if (!non_dma_extra) {
                /* ERR: Cannot be NULL, until 'i reaches 'count' */
                /* This condition may not hit but intentional for defensive check */
                /* coverity[dead_error_begin] */
                rv = SOC_E_INTERNAL;
                goto _err_soc_counter_th3_extra_ctrs_init;
            }
        }

    }
    return SOC_E_NONE;

_err_soc_counter_th3_extra_ctrs_init:
    LOG_WARN(BSL_LS_SOC_COUNTER,
        (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                    id - SOC_COUNTER_NON_DMA_START));
    return rv;
}

/* Function to initialize sFlow extra counters */
STATIC int
soc_counter_th3_extra_sflow_ctrs_init(int unit,
                               soc_counter_non_dma_t *non_dma_parent,
                               soc_counter_non_dma_t *non_dma_extra,
                               uint32 extra_ctr_ct,
                               int *total_entries)
{
    int i, parent_base_index = 0;
    int base_mem_ct = 0;
    int do_dma = 1;
    int pipe, ind_min, pipe_rows, entry_words;
    uint32 *dma_buf;

    soc_field_t sflow_counter_fields[] = {
        SAMPLE_POOLf,
        SAMPLE_POOL_SNAPSHOTf,
        SAMPLE_COUNTf
    };

    /* Do sanity check for input params */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        return SOC_E_PARAM;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    for (i = 0; i < extra_ctr_ct; i++) {
        /* Set Child attributes. Inherit few info from the Parent */
        non_dma_extra->mem = non_dma_parent->mem;
        non_dma_extra->base_index = parent_base_index + *total_entries;
        base_mem_ct = soc_mem_index_count(unit, non_dma_extra->mem);
        non_dma_extra->num_entries = base_mem_ct;
        dma_buf = non_dma_parent->dma_buf[0];

        non_dma_extra->flags = (non_dma_parent->flags &
                                ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

        /* DMA should be done only once to get sFlow data table */
        if (do_dma != 1) {
            non_dma_extra->flags = (non_dma_extra->flags &
                                    ~_SOC_COUNTER_NON_DMA_DO_DMA);
        }

        non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 9,
                                         "Extra ctrs cname");

        if (non_dma_extra->cname == NULL) {
            return SOC_E_MEMORY;
        }

        sal_sprintf(non_dma_extra->cname, "*%s_CNTR%d",
                    non_dma_parent->cname, i);

        non_dma_extra->field = sflow_counter_fields[i];
        non_dma_extra->reg = non_dma_parent->reg;

        ind_min = 0;
        pipe_rows = (soc_mem_index_max(unit,non_dma_parent->mem) + 1) 
                    / _TH3_PIPES_PER_DEV;
        entry_words = soc_mem_entry_words(unit, non_dma_parent->mem);
        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            non_dma_extra->dma_num_entries[pipe] = pipe_rows;
            if(SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
                non_dma_extra->dma_mem[pipe] = INVALIDm;
            } else {
                non_dma_extra->dma_mem[pipe] = non_dma_parent->mem;
                non_dma_extra->dma_index_min[pipe] = ind_min;
                non_dma_extra->dma_index_max[pipe] = ind_min + pipe_rows - 1;
                non_dma_extra->dma_buf[pipe] = dma_buf;
            }
            *total_entries += pipe_rows;
            ind_min += pipe_rows;
            dma_buf += entry_words * pipe_rows;
        }

        /* Allocation of Child Control blocks.
         * [0],..<even indices> - PKT_CTR non_dma ctrl block
         * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
         * Hence in below code, jump by 2, so all similar control blocks are
         * initilized at the same time.
         */
        non_dma_extra += 2;

        do_dma = 0;

        /* coverity[check_after_deref] */
        if (!non_dma_extra) {
            /* ERR: Cannot be NULL, until 'i reaches 'extra_ctr_ct' */
            /* This condition may not hit but intentional for defensive check */
            /* coverity[dead_error_line] */
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

int
soc_counter_tomahawk3_non_dma_init(int unit, int nports,
                                  int non_dma_start_index,
                                  int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2, *temp;
    soc_mem_t child_mem = INVALIDm;
    uint32 *buf;
    int pipe, num_entries, alloc_size, entry_words, num_extra_ctr_entries;
    int pool_count = 0, rv;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    if (soc_ctr_ctrl == NULL) {
        return SOC_E_INIT;
    }

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    /* Registration of Chip specific functions to the Global Counter 
     * control structure.
     */
    soc_ctr_ctrl->get_child_dma = soc_counter_tomahawk_get_child_dma;
    soc_ctr_ctrl->dma_config_update = soc_counter_tomahawk_dma_flags_update;
    soc_ctr_ctrl->port_obm_info_get = soc_tomahawk_port_obm_info_get;

    /* ING_FLEX_CTR_COUNTER_TABLE_0 is the largest table to be DMA'ed */
    num_entries = soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    alloc_size = _TH3_PIPES_PER_DEV * num_entries * sizeof(uint32) * entry_words;

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter_range_read_buffer");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    /* SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_ALLOC |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE_ALL;

    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* cpu port has max number of queues */
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    entry_words = soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);
    non_dma0->num_entries = num_entries * _TH3_PIPES_PER_DEV;

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
        non_dma0->dma_num_entries[pipe] = num_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "MC_PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;


    /* SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma2->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    /* SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma2->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";

    /* SOC_COUNTER_NON_DMA_COSQ_DROP_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_THDO_QUEUE_DROP_COUNTm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = Q_PACKET_TOTAL_DROPf;
    non_dma0->cname = "PERQ_DROP_PKT";

    child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, non_dma0->mem, 0);
    entry_words = soc_mem_entry_words(unit, child_mem );
    non_dma0->num_entries = soc_mem_index_count(unit, child_mem) * NUM_ITM(unit);
    non_dma0->dma_buf[0] = buf;
    non_dma0->extra_ctr_ct = NUM_ITM(unit);
    /* Incl. extra_ctr for: PERQ_DROP_PKT,
                            PERQ_DROP_PKT_UC,
                            PERQ_DROP_BYTE,
                            PERQ_DROP_BYTE_UC,
                            PERQ_DROP_WRED_PKT
       5x alloc size*/
    non_dma0->extra_ctr_fld_ct = 5;
    alloc_size = (non_dma0->extra_ctr_ct * 5) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_perq_drop");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                        non_dma0,
                                        non_dma0->extra_ctrs,
                                        non_dma0->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma0->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma0->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + non_dma0->extra_ctr_ct;
    non_dma1->cname = "PERQ_DROP_PKT_UC";
    if (non_dma0->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                        non_dma1,
                                        non_dma1->extra_ctrs,
                                        non_dma1->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma1->num_entries = num_extra_ctr_entries;
    }

    /* SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = Q_BYTE_TOTAL_DROPf;
    non_dma2->cname = "PERQ_DROP_BYTE";
    non_dma2->extra_ctrs = non_dma1->extra_ctrs + non_dma1->extra_ctr_ct;
    if (non_dma0->extra_ctrs == NULL) {
        non_dma2->extra_ctr_ct = 0;
        non_dma2->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                        non_dma2,
                                        non_dma2->extra_ctrs,
                                        non_dma2->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma2->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma2->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma2;
    non_dma1->id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;
    non_dma1->cname = "PERQ_DROP_BYTE_UC";
    non_dma1->extra_ctrs = non_dma2->extra_ctrs + non_dma2->extra_ctr_ct;
    if (non_dma0->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                        non_dma1,
                                        non_dma1->extra_ctrs,
                                        non_dma1->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma1->num_entries = num_extra_ctr_entries;
    }

    /* SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = Q_WRED_PACKET_TOTAL_DROPf;
    non_dma2->cname = "PERQ_DROP_WRED_PKT";
    non_dma2->extra_ctrs = non_dma1->extra_ctrs + non_dma1->extra_ctr_ct;
    if (non_dma0->extra_ctrs) {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_COSQ_DROP_WRED_PKT,
                                        non_dma2,
                                        non_dma2->extra_ctrs,
                                        non_dma2->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma2->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma2->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDO_PORT_DROP_COUNT_MCm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PORT_PACKET_YELLOW_DROPf;
    non_dma0->cname = "DROP_PKT_YEL";

    child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, non_dma0->mem, 0);
    num_entries = soc_mem_index_count(unit, child_mem) / _TH3_PIPES_PER_DEV;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_ITM(unit);
    /* Incl. buffer for: PORT_PACKET_RED_DROP,
                         PORT_PACKET_YELLOW_DROP,
       2x alloc size */
    non_dma0->extra_ctr_fld_ct = 2;
    alloc_size = (non_dma0->extra_ctr_ct * 2) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_color_drop_pkt_mc");

    /*
     * Below represents how Extra cntrs are arranged
     * for Per Port Color Drop Counters. (2 x ITM in TH3)
     *
     *   [0]     [2]
     *  ------  ------
     * |      ||      |
     * | YEL  || RED  |
     * | [0]  || [0]  |
     *  ------  ------
     * |      ||      |
     * | YEL  || RED  |
     * | [1]  || [1]  |
     *  ------  ------
     *   [1]     [3]
     */
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW,
                                        non_dma0,
                                        non_dma0->extra_ctrs,
                                        non_dma0->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PORT_PACKET_RED_DROPf;
    non_dma1->cname = "DROP_PKT_RED";

    non_dma1->extra_ctrs = non_dma0->extra_ctrs + non_dma0->extra_ctr_ct;
    if (non_dma0->extra_ctrs) {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED,
                                        non_dma1,
                                        non_dma1->extra_ctrs,
                                        non_dma1->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma1->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma1->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDO_PORT_DROP_COUNT_UCm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PORT_WRED_PACKET_YELLOW_DROPf;
    non_dma0->cname = "WRED_PKT_YEL";

    child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, non_dma0->mem, 0);
    num_entries = soc_mem_index_count(unit, child_mem) /_TH3_PIPES_PER_DEV;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_ITM(unit);
    /* Incl. buffer for: PORT_WRED_PACKET_YELLOW_DROP,
                         PORT_WRED_PACKET_RED_DROP,
                         PORT_PACKET_YELLOW_DROP,
                         PORT_PACKET_RED_DROP
       4x alloc size */
    non_dma0->extra_ctr_fld_ct = 4;
    alloc_size = (non_dma0->extra_ctr_ct * 4) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_color_drop_pkt_uc");
    /*
     * Below represents how Extra cntrs are arranged
     * for Per Port Color Drop Counters. (2 x ITM in TH3)
     *
     *   [0]        [2]       [4]   [6]
     *  ---------  --------- ----- -----
     * |         ||         |     |     |
     * | WRED_YEL|| WRED_RED| YEL | RED |
     * | [0]     || [0]     | [0] | [0] |
     *  ---------  --------- ----- -----
     * |         ||         |     |     |
     * | WRED_YEL|| WRED_RED| YEL | RED |
     * | [1]     || [1]     | [1] | [1] |
     *  ---------  --------- ----- -----
     *   [1]        [3]       [5]   [7]
     */
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW,
                                        non_dma0,
                                        non_dma0->extra_ctrs,
                                        non_dma0->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;


    /* SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PORT_WRED_PACKET_RED_DROPf;
    non_dma1->cname = "WRED_PKT_RED";
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + non_dma0->extra_ctr_ct;
    if (non_dma0->extra_ctrs) {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED,
                                        non_dma1,
                                        non_dma1->extra_ctrs,
                                        non_dma1->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma1->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma1->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PORT_PACKET_YELLOW_DROPf;
    non_dma2->cname = "DROP_PKT_YEL_UC";
    non_dma2->extra_ctrs = non_dma1->extra_ctrs + non_dma1->extra_ctr_ct;
    if (non_dma0->extra_ctrs) {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW_UC,
                                        non_dma2,
                                        non_dma2->extra_ctrs,
                                        non_dma2->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma2->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma2->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma2;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PORT_PACKET_RED_DROPf;
    non_dma1->cname = "DROP_PKT_RED_UC";
    non_dma1->extra_ctrs = non_dma2->extra_ctrs + non_dma2->extra_ctr_ct;
    if (non_dma0->extra_ctrs) {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED_UC,
                                        non_dma1,
                                        non_dma1->extra_ctrs,
                                        non_dma1->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma1->num_entries = num_extra_ctr_entries;
        *non_dma_entries += non_dma1->num_entries;
    }

    /* SOC_COUNTER_NON_DMA_QUEUE_CURRENT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_QUEUE_CURRENT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_THDO_COUNTER_QUEUEm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "QUEUE_CUR";

    child_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, non_dma0->mem, 0);
    num_entries = soc_mem_index_count(unit, child_mem) / _TH3_PIPES_PER_DEV;
    entry_words = soc_mem_entry_words(unit, child_mem );
    non_dma0->num_entries = _TH3_PIPES_PER_DEV * num_entries * NUM_ITM(unit);
    non_dma0->dma_buf[0] = buf;

    non_dma0->extra_ctr_ct = NUM_ITM(unit);
    /* Incl. buffer for:
       1x alloc size
       extend when we want to include TOTALCOUNT and MIN_COUNT*/
    non_dma0->extra_ctr_fld_ct = 2;
    alloc_size = (non_dma0->extra_ctr_ct * 2) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_queue");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_QUEUE_CURRENT,
                                        non_dma0,
                                        non_dma0->extra_ctrs,
                                        non_dma0->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma1->cname = "UC_QUEUE_CUR";
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + non_dma0->extra_ctr_ct;
    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        soc_counter_th3_extra_ctrs_init(unit,
                                        SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT,
                                        non_dma1,
                                        non_dma1->extra_ctrs,
                                        non_dma1->extra_ctr_ct,
                                        &num_extra_ctr_entries);
        non_dma1->num_entries = num_extra_ctr_entries;
    }

    /* SOC_COUNTER_NON_DMA_PG0_MIN_CURRENT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG0_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG0_MIN_CURRENT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDI_PORT_PG_MIN_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG0_MIN_COUNTf;
    non_dma0->cname = "PG0_MIN_CUR";

    child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, 0);
    if (child_mem != INVALIDm) {
        num_entries = soc_mem_index_count(unit, child_mem);
        entry_words = soc_mem_entry_words(unit, child_mem);
        non_dma0->num_entries = _TH3_PIPES_PER_DEV * num_entries;

        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, pipe);
            non_dma0->dma_mem[pipe] = child_mem;
            non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
            non_dma0->dma_index_max[pipe] =
                soc_mem_index_max(unit, child_mem);
            non_dma0->dma_index_min[pipe] =
                soc_mem_index_min(unit, child_mem);
            non_dma0->dma_num_entries[pipe] = num_entries;
        }
        *non_dma_entries += non_dma0->num_entries;
    } else {
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
    }

    /* SOC_COUNTER_NON_DMA_PG1_MIN_CURRENT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG1_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PG1_MIN_CURRENT;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG1_MIN_COUNTf;
    non_dma1->cname = "PG1_MIN_CUR";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PG2_MIN_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG2_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG2_MIN_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG2_MIN_COUNTf;
    non_dma2->cname = "PG2_MIN_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG3_MIN_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG3_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG3_MIN_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG3_MIN_COUNTf;
    non_dma2->cname = "PG3_MIN_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG4_MIN_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG4_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG4_MIN_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG4_MIN_COUNTf;
    non_dma2->cname = "PG4_MIN_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG5_MIN_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG5_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG5_MIN_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG5_MIN_COUNTf;
    non_dma2->cname = "PG5_MIN_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG6_MIN_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG6_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG6_MIN_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG6_MIN_COUNTf;
    non_dma2->cname = "PG6_MIN_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG7_MIN_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG7_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG7_MIN_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG7_MIN_COUNTf;
    non_dma2->cname = "PG7_MIN_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG0_SHARED_CURRENT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG0_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG0_SHARED_CURRENT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDI_PORT_PG_SHARED_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG0_SHARED_COUNTf;
    non_dma0->cname = "PG0_SHARED_CUR";

    child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, 0);
    if (child_mem != INVALIDm) {
        num_entries = soc_mem_index_count(unit, child_mem);
        entry_words = soc_mem_entry_words(unit, child_mem);
        non_dma0->num_entries = _TH3_PIPES_PER_DEV * num_entries;

        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, pipe);
            non_dma0->dma_mem[pipe] = child_mem;
            non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
            non_dma0->dma_index_max[pipe] =
                soc_mem_index_max(unit, child_mem);
            non_dma0->dma_index_min[pipe] =
                soc_mem_index_min(unit, child_mem);
            non_dma0->dma_num_entries[pipe] = num_entries;
        }
        *non_dma_entries += non_dma0->num_entries;
    } else {
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
    }

    /* SOC_COUNTER_NON_DMA_PG1_SHARED_CURRENT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG1_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PG1_SHARED_CURRENT;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG1_SHARED_COUNTf;
    non_dma1->cname = "PG1_SHARED_CUR";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PG2_SHARED_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG2_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG2_SHARED_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG2_SHARED_COUNTf;
    non_dma2->cname = "PG2_SHARED_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG3_SHARED_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG3_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG3_SHARED_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG3_SHARED_COUNTf;
    non_dma2->cname = "PG3_SHARED_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG4_SHARED_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG4_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG4_SHARED_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG4_SHARED_COUNTf;
    non_dma2->cname = "PG4_SHARED_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG5_SHARED_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG5_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG5_SHARED_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG5_SHARED_COUNTf;
    non_dma2->cname = "PG5_SHARED_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG6_SHARED_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG6_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG6_SHARED_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG6_SHARED_COUNTf;
    non_dma2->cname = "PG6_SHARED_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG7_SHARED_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG7_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG7_SHARED_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG7_SHARED_COUNTf;
    non_dma2->cname = "PG7_SHARED_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG0_HDRM_CURRENT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG0_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG0_HDRM_CURRENT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDI_PORT_PG_HDRM_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG0_HDRM_COUNTf;
    non_dma0->cname = "PG0_HDRM_CUR";

    child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, 0);
    if (child_mem != INVALIDm) {
        num_entries = soc_mem_index_count(unit, child_mem);
        entry_words = soc_mem_entry_words(unit, child_mem);
        non_dma0->num_entries = _TH3_PIPES_PER_DEV * num_entries;

        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, pipe);
            non_dma0->dma_mem[pipe] = child_mem;
            non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
            non_dma0->dma_index_max[pipe] =
                soc_mem_index_max(unit, child_mem);
            non_dma0->dma_index_min[pipe] =
                soc_mem_index_min(unit, child_mem);
            non_dma0->dma_num_entries[pipe] = num_entries;
        }
        *non_dma_entries += non_dma0->num_entries;
    } else {
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
    }

    /* SOC_COUNTER_NON_DMA_PG1_HDRM_CURRENT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG1_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PG1_HDRM_CURRENT;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG1_HDRM_COUNTf;
    non_dma1->cname = "PG1_HDRM_CUR";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PG2_HDRM_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG2_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG2_HDRM_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG2_HDRM_COUNTf;
    non_dma2->cname = "PG2_HDRM_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG3_HDRM_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG3_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG3_HDRM_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG3_HDRM_COUNTf;
    non_dma2->cname = "PG3_HDRM_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG4_HDRM_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG4_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG4_HDRM_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG4_HDRM_COUNTf;
    non_dma2->cname = "PG4_HDRM_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG5_HDRM_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG5_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG5_HDRM_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG5_HDRM_COUNTf;
    non_dma2->cname = "PG5_HDRM_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG6_HDRM_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG6_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG6_HDRM_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG6_HDRM_COUNTf;
    non_dma2->cname = "PG6_HDRM_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PG7_HDRM_CURRENT */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG7_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG7_HDRM_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PG7_HDRM_COUNTf;
    non_dma2->cname = "PG7_HDRM_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_SP0_CURRENT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SP0_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_SP0_CURRENT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_THDI_PORTSP_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PORTSP0_SHARED_COUNTf;
    non_dma0->cname = "SP0_CUR";

    child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, 0);
    if (child_mem != INVALIDm) {
        num_entries = soc_mem_index_count(unit, child_mem);
        entry_words = soc_mem_entry_words(unit, child_mem);
        non_dma0->num_entries = _TH3_PIPES_PER_DEV * num_entries;

        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, pipe);
            non_dma0->dma_mem[pipe] = child_mem;
            non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
            non_dma0->dma_index_max[pipe] =
                soc_mem_index_max(unit, child_mem);
            non_dma0->dma_index_min[pipe] =
                soc_mem_index_min(unit, child_mem);
            non_dma0->dma_num_entries[pipe] = num_entries;
        }
        *non_dma_entries += non_dma0->num_entries;
    } else {
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
    }

    /* SOC_COUNTER_NON_DMA_SP1_CURRENT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SP1_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_SP1_CURRENT;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PORTSP1_SHARED_COUNTf;
    non_dma1->cname = "SP1_CUR";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_SP2_CURRENT  */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SP2_CURRENT  -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_SP2_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PORTSP2_SHARED_COUNTf;
    non_dma2->cname = "SP2_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_SP3_CURRENT  */
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SP3_CURRENT  -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_SP3_CURRENT;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = PORTSP3_SHARED_COUNTf;
    non_dma2->cname = "SP3_CUR";
    *non_dma_entries += non_dma2->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1; /* 48 for cpu port */
    non_dma0->mem = MMU_THDO_SRC_PORT_DROP_COUNTm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SRC_PORT_PACKET_TOTAL_DROPf;
    non_dma0->cname = "DROP_PKT_ING";

    child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, 0);
    if (child_mem != INVALIDm) {
        num_entries = soc_mem_index_count(unit, child_mem);
        entry_words = soc_mem_entry_words(unit, child_mem);
        non_dma0->num_entries = _TH3_PIPES_PER_DEV * num_entries;

        for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
            child_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, non_dma0->mem, pipe);
            non_dma0->dma_mem[pipe] = child_mem;
            non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
            non_dma0->dma_index_max[pipe] =
                soc_mem_index_max(unit, child_mem);
            non_dma0->dma_index_min[pipe] =
                soc_mem_index_min(unit, child_mem);
            non_dma0->dma_num_entries[pipe] = num_entries;
        }
        *non_dma_entries += non_dma0->num_entries;
    } else {
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
    }

    /* SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->id = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->entries_per_port = 0;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = MMU_THDR_QE_DROP_COUNT_PKT_PRIQr;
    non_dma0->field = PKT_COUNTf;
    non_dma0->num_entries = SOC_REG_NUMELS(unit, non_dma0->reg) * NUM_ITM(unit);
    non_dma0->cname = "PRIQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_PRIQ_DROP_BYTE */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PRIQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PRIQ_DROP_BYTE;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = MMU_THDR_QE_DROP_COUNT_BYTE_PRIQr;
    non_dma1->field = BYTE_COUNTf;
    non_dma1->num_entries = SOC_REG_NUMELS(unit, non_dma1->reg) * NUM_ITM(unit);
    non_dma1->cname = "PRIQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = MMU_THDR_QE_DROP_COUNT_YELLOW_PKT_PRIQr;
    non_dma1->field = PKT_COUNTf;
    non_dma1->num_entries = SOC_REG_NUMELS(unit, non_dma1->reg) * NUM_ITM(unit);
    non_dma1->cname = "PRIQ_DROP_PKT_YEL";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = MMU_THDR_QE_DROP_COUNT_RED_PKT_PRIQr;
    non_dma1->field = PKT_COUNTf;
    non_dma1->num_entries = SOC_REG_NUMELS(unit, non_dma1->reg) * NUM_ITM(unit);
    non_dma1->cname = "PRIQ_DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = MMU_THDI_POOL_DROP_COUNT_HPr;
    non_dma1->field = PACKET_DROP_COUNTf;
    non_dma1->num_entries = SOC_REG_NUMELS(unit, non_dma1->reg) * NUM_ITM(unit);
    non_dma1->cname = "HDRM_POOL_DROP_PKT";
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_ING_FLEX_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_ING_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    pool_count = 20;
    non_dma0->num_entries =
        pool_count * _TH3_PIPES_PER_DEV *
                        soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    non_dma0->mem = ING_FLEX_CTR_COUNTER_TABLE_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "ING_FLEX_PKT";

    num_entries = soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    for (pipe = 0; pipe <_TH3_PIPES_PER_DEV; pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }
    non_dma0->extra_ctr_ct = pool_count;
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_flex");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit, SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                            SOC_COUNTER_NON_DMA_ING_FLEX_PKT -
                            SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_ING_FLEX_BYTE */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_FLEX_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_ING_FLEX_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "ING_FLEX_BYTE";
    non_dma1->extra_ctr_ct = pool_count; /* Taken care by PKT_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;
    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE -
                            SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_EGR_FLEX_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EGR_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    pool_count = 4;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        pool_count * _TH3_PIPES_PER_DEV *
            soc_mem_index_count(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    non_dma0->mem = EGR_FLEX_CTR_COUNTER_TABLE_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "EGR_FLEX_PKT";

    num_entries = soc_mem_index_count(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }
    non_dma0->extra_ctr_ct = pool_count;
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_flex");
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit, SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT -
                            SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "EGR_FLEX_BYTE";
    non_dma1->extra_ctr_ct = pool_count; /* Taken care by PKT_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;
    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE -
                            SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_EFP_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EFP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EFP_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        _TH3_PIPES_PER_DEV * soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    non_dma0->mem = EFP_COUNTER_TABLEm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "EFP_PKT";

    num_entries = soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    entry_words = soc_mem_entry_words(unit, EFP_COUNTER_TABLEm);
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        non_dma0->dma_mem[pipe] =
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
        non_dma0->dma_num_entries[pipe] =
            soc_mem_index_count(unit, non_dma0->dma_mem[0]);
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_EFP_BYTE */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EFP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EFP_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_NO_SHOW_C;

    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "EFP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    /* cSOC_COUNTER_NON_DMA_SFLOW_ING_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SFLOW_ING_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_SFLOW_ING_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA
                      | _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = SFLOW_ING_DATA_SOURCEm;

    num_entries = soc_mem_index_count(unit, SFLOW_ING_DATA_SOURCEm);
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = SOC_SFLOW_CTR_TYPE_COUNT * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->cname = "SFLOW_ING_PKT";
    non_dma0->field = SAMPLE_POOLf;
    non_dma0->dma_buf[0] = &buf[0];
    non_dma0->extra_ctr_ct = SOC_SFLOW_CTR_TYPE_COUNT;
    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports BYTE Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_sflow_ing");

    /*
     * Below represents how Extra cntrs are arranged
     * for sFlow counters.
     *
     *       --------------------------
     * [0]  |      SAMPLE_POOL [0]     |
     *       --------------------------
     * [1]  |           N/A [0]        |
     *       --------------------------
     * [2]  | SAMPLE_POOL_SNAPSHOT [0] |
     *       --------------------------
     * [3]  |           N/A [0]        |
     *       --------------------------
     * [4]  |      SAMPLE_COUNT [0]    |
     *       --------------------------
     * [5]  |           N/A [0]        |
     *       --------------------------
 */
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th3_extra_sflow_ctrs_init(unit,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                            SOC_COUNTER_NON_DMA_SFLOW_ING_PKT  -
                            SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA
                      | _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = SFLOW_ING_FLEX_DATA_SOURCEm;

    num_entries = soc_mem_index_count(unit, SFLOW_ING_FLEX_DATA_SOURCEm);
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = SOC_SFLOW_CTR_TYPE_COUNT * num_entries;
    non_dma0->reg = INVALIDr;
    non_dma0->cname = "SFLOW_FLEX_PKT";
    non_dma0->field = SAMPLE_POOLf;
    non_dma0->dma_buf[0] = &buf[0];
    non_dma0->extra_ctr_ct = SOC_SFLOW_CTR_TYPE_COUNT;

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports BYTE Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_sflow_flex");
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th3_extra_sflow_ctrs_init(unit,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit, "ERR: Extra ctrs Init FAILED for id %d\n"),
                            SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT -
                            SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO */
    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->reg = IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr;
    non_dma0->cname = "OBM_LOSSY_LO_DRP_PKT";
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->entries_per_port = _TH3_PORTS_PER_PBLK;
    non_dma0->num_entries = _TH3_PORTS_PER_PBLK * NUM_PIPE(unit);
    non_dma0->mem = INVALIDm;
    non_dma0->field = COUNTERf;
    non_dma0->dma_buf[0] = buf;

    /*
     * Below represents how Extra cntrs are arranged
     * for OBM 0-3 Drop Counters.
     * n = NUM_OBMs (4)
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------
     * |      ||      |...|      |
     * | BYTE || BYTE |...| BYTE |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */
    non_dma0->extra_ctr_ct = _TH3_PBLKS_PER_PIPE;
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    temp = non_dma0->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossy_lo");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma0,
                                                non_dma0->extra_ctrs,
                                                non_dma0->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                        non_dma0->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSY_LO_DRP_BYTE";

    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSY_HI_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossy_hi");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;


    /* SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSY_HI_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;


    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0 */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSLESS0_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossless0");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0 */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSLESS0_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1 */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSLESS1_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossless1");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1 */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSLESS1_DRP_BYTE";

    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_FLOW_CONTROL_EVENT_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_FC_EVENTS";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_fc");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;


    /* eviction table init */
    SOC_IF_ERROR_RETURN(soc_counter_tomahawk_eviction_init(unit));

    soc_counter_tomahawk_ctr_dma_post_init(unit);

    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
