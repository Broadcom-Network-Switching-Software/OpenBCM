/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#include <soc/types.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dcmn/error.h>
#include <soc/error.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
#include <shared/bsl.h>

#ifdef CRASH_RECOVERY_SUPPORT

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>

#include <bcm/switch.h>

#include <soc/mem.h>
#include <sal/core/libc.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>

#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/hwstate/hw_log.h>
#include <soc/dpp/JER/jer_sbusdma_desc.h>

#include <soc/dcmn/dcmn_mem.h>
#define CR_DMA_INDEX_MAX 500

typedef struct cr_dma_support_buffers_d
{
    /* dma buffers for table and slam dma */
    uint32 *buffer_table;
    uint32 *buffer_slam;
    soc_mem_t mem;

} cr_dma_support_buffers_t;

STATIC cr_dma_support_buffers_t cr_dma_support_buffers[SOC_MAX_NUM_DEVICES];

STATIC int
cr_dma_support_mem_read(int unit, int index)
{
    uint32 mem_val;
    soc_mem_read(unit, cr_dma_support_buffers[unit].mem, MEM_BLOCK_ANY, index, &mem_val);
    return mem_val;
}

STATIC void
cr_dma_support_write_slam(int unit, soc_mem_t mem, int index_min, int index_max, uint32 *buffer_slam, uint32 single_entry_size, uint32 val)
{
    sal_memcpy(buffer_slam, &val, single_entry_size);
    soc_mem_array_fill_range(unit, mem, 0, 0, MEM_BLOCK_ANY, index_min, index_max, buffer_slam);
} 


STATIC int
cr_dma_support_clear_value(uint32 val)
{
    uint32 mask = ~(1 << 19);
    return (val & mask);
}


STATIC int
cr_dma_support_write_descriptor_dma(
      int       unit,
      soc_mem_t mem,
      uint32    index,
      uint32   *data
)
{
    uint32 blk;
    uint32 *cache;
    uint8 *vmap;
    int array_index = 0;
    int entry_dw = soc_mem_entry_words(unit, mem);
    int mem_array_cache_offset = 0;

    SOC_MEM_ALIAS_TO_ORIG(unit,mem);
    MEM_LOCK(unit, mem);
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        cache = SOC_MEM_STATE(unit, mem).cache[blk];
        vmap = SOC_MEM_STATE(unit, mem).vmap[blk];

        /* Add a descriptor to the DMA chain */
        jer_sbusdma_desc_add_mem(unit, mem, array_index, blk, index, (void *)data);

        sal_memcpy(cache + mem_array_cache_offset + index * entry_dw, data, entry_dw * 4);
        CACHE_VMAP_SET(vmap, index);
    }

    MEM_UNLOCK(unit, mem);

    return SOC_E_NONE;
}


int
cr_dma_support_test(int unit, args_t *a, void *p)
{
    uint32 res;
    uint32 idx;
    uint32 val;
    uint32 mem_val/*, slam_initial_val*/;
    uint32 hw_buffer[CR_DMA_INDEX_MAX];

    /* dma buffers for table and slam dma */
    uint32 *buffer_table;
    uint32 *buffer_slam;

    /* set min and max indexes for the dma intervals */
    int index_min = 0;
    int index_max = CR_DMA_INDEX_MAX - 1;

    /*
     * Predetermined border memory indexes:
     * SLAM DMA : [0,99]
     * Regular memory writes : [100;199]
     * TABLE DMA : [200,299]
     * DMA Chain : [300,399]
     */
    const int slam_dma_border  = 100;
    const int no_dma_border    = 200;
    const int table_dma_border = 300;
    const int dma_chain_border = 400;

    /* use dmaable memory */
    soc_mem_t mem = cr_dma_support_buffers[unit].mem;

    /* calculate the size for SLAM and TABLE dma buffers */
    uint32 single_entry_size = soc_mem_entry_words(unit, mem) * sizeof(uint32);
    uint32 mem_size = ((index_max - index_min + 1) * single_entry_size);

    /* validate that crash recovery on demand transaction handling is on */
    if(!soc_dcmn_cr_is_journaling_on_demand(unit)) {
        printf("ERROR: On demand transcation handling is off \n");
        return -1;
    }

    /* allocate non-volatile memory for the SLAM and TABLE dma buffers */
    buffer_table = cr_dma_support_buffers[unit].buffer_table;
    buffer_slam  = cr_dma_support_buffers[unit].buffer_slam;

    sal_memset(buffer_table, 0, mem_size);
    sal_memset(buffer_slam,  0, single_entry_size);

    /* Do a SLAM DMA outside of the transaction and read the value.
     * Store the value retrieved for later validation after the transaction.
     * Update the hardware buffer */
    val = 1;
    cr_dma_support_write_slam(unit, mem, index_min, index_max, buffer_slam, single_entry_size, val);
    for(idx = index_min; idx < index_max; idx++) {
        hw_buffer[idx] = val;
    }



    /* transaction start */
    bcm_switch_control_set(unit, bcmSwitchCrTransactionStart, 0);

    /* SLAM DMA journaling, index: [0;99] */
    val = 2;
    cr_dma_support_write_slam(unit, mem, 0, slam_dma_border - 1, buffer_slam, single_entry_size, val);
    for(idx = 0; idx < slam_dma_border; idx++) {
        hw_buffer[idx] = val;
    }
    /* read SLAM DMA value from journal */
    mem_val = cr_dma_support_mem_read(unit, 0);
    if(mem_val != hw_buffer[0]) {
        printf("ERROR: SLAM DMA failed, wrong values retrieved \n");
        return -1;
    }

    /* Journal individual memory writes, index: [100;199]
     * Used to validate DMA read */
    val = 3;
    for(idx = slam_dma_border; idx < no_dma_border; idx++) {
        soc_mem_write(unit, mem, MEM_BLOCK_ANY, idx, &val);
        hw_buffer[idx] = val;
    }
    /* DMA read - validation */
    soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, slam_dma_border, no_dma_border - 1, buffer_table);
    for(idx = slam_dma_border; idx < no_dma_border; idx++) {
        if(hw_buffer[idx] != buffer_table[idx - slam_dma_border]) {
            printf("ERROR: DMA read failed, wrong values retrieved, idx:%d \n",idx);
            return -1;
        }
    }

    /* TABLE DMA journaling: [200;299] */
    val = 4;
    sal_memset(buffer_table, 0, mem_size); /* clear the buffer, was used for dma read */
    for(idx = no_dma_border; idx < table_dma_border; idx++) {
        buffer_table[idx - no_dma_border] = val;
        hw_buffer[idx] = val;
    }
    res = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY, no_dma_border, table_dma_border - 1, buffer_table);
    /* read TABLE DMA value from journal */
    mem_val = cr_dma_support_mem_read(unit, no_dma_border);
    if(mem_val != hw_buffer[no_dma_border]) {
        printf("ERROR: TABLE DMA failed, wrong values retrieved \n");
        return -1;
    }

    /* DESCRIPTOR DMA journaling : [300;399] */
    val = 5;
    for(idx = table_dma_border; idx < dma_chain_border; idx++) {
        cr_dma_support_write_descriptor_dma(unit, mem, idx, &val);
        hw_buffer[idx] = val;
    }

    if (!BCM_UNIT_DO_HW_READ_WRITE(unit)) {
        /* make sure that we flush the descriptor dma chain if immediate access is enabled */
        jer_sbusdma_desc_wait_done(unit);
    }

    /* validate that the retrieved values are correct */
    mem_val = cr_dma_support_mem_read(unit, dma_chain_border - 1);
    if(mem_val != val) {
        printf("ERROR: DESCRIPTOR DMA failed, wrong values retrieved \n");
        return -1;
    }

    /* transaction start, commit everything to hardware */
    bcm_switch_control_set(unit, bcmSwitchCrCommit, 0);





    /* compare hardware to the hw buffer from index_min to index_max using DMA read */
    res = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, index_min, index_max, buffer_table);

    for(idx = index_min; idx < index_max; idx++) {
        if(hw_buffer[idx] != cr_dma_support_clear_value(buffer_table[idx])) {
            printf("ERROR (DMA READ): Difference between hardware and buffer detected, idx:%d, hardware value: %d, buffer value: %d \n", idx, cr_dma_support_clear_value(buffer_table[idx]), hw_buffer[idx]);
        }
    }

    mem_val = cr_dma_support_mem_read(unit, dma_chain_border - 1);
    if(mem_val != hw_buffer[dma_chain_border - 1]) {
        printf("ERROR: DESCRIPTOR DMA failed, wrong values retrieved \n");
        return -1;
    }

    return res;
}

int
cr_dma_support_test_init(int unit, args_t *a, void **p)
{
    soc_mem_t mem = IHP_OAMAm;
    uint32 single_entry_size = soc_mem_entry_words(unit, mem) * sizeof(uint32);
    uint32 mem_size = (CR_DMA_INDEX_MAX * single_entry_size);

    printf("CRASH RECOVERY DMA test init\n");

    cr_dma_support_buffers[unit].buffer_table = soc_cm_salloc(unit, mem_size,          "dma table");;
    cr_dma_support_buffers[unit].buffer_slam  = soc_cm_salloc(unit, single_entry_size, "dma slam");;
    cr_dma_support_buffers[unit].mem          = mem;

    return CMD_OK;
}

int cr_dma_support_test_done(int unit, void *p)
{
    soc_cm_sfree(unit, cr_dma_support_buffers[unit].buffer_table);
    soc_cm_sfree(unit, cr_dma_support_buffers[unit].buffer_slam);

    printf("Done\n");
    return CMD_OK;
}

#endif
