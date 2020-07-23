

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/defs.h>
#include <soc/iproc.h>
#include <soc/cmicx.h>
#include <soc/sbusdma.h>
#include <soc/l2x.h>
#include <soc/mem.h>
#include <bcm/error.h>
#include <bcm/link.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_SBUSDMA_SUPPORT) \
    && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT))
typedef struct sbusdma_params_s {
    int          unit;
    sal_thread_t tid;
    int          type;
    int          write_secs;
    int          read_secs;
    int          test_time;
    int          status1;
    int          status2;
    int          rd_channel;
    int          wr_channel;
    soc_mem_t    mem;
    uint32 flags;
} sbusdma_params_t;

typedef struct sbusdma_desc_s {
    uint32      control;
    uint32      request;
    uint32      count;
    uint32      opcode;
    uint32      sbus_base;
    uint32      hostmem_phy_base_lo;
    uint32      hostmem_phy_base_hi;
    uint32      reserved;
} sbusdma_desc_t;

#define SBUSDMA_CH_MAX_NUM (3 * SOC_SBUSDMA_CH_PER_CMC)

#define SBUSDMA_WRITE_RUN  (1 << 0)
#define SBUSDMA_READ_RUN   (1 << 1)

#define SBUSDMA_STATUS_INVALID 0
#define SBUSDMA_STATUS_SUCCESS 1
#define SBUSDMA_STATUS_FAILURE 2
#define SBUSDMA_STATUS_RUNNING 3

#define SBUSDMA_TYPE_READ  0
#define SBUSDMA_TYPE_WRITE 1

#define HOST_ADDR_HI_CMICX(_paddr) (PTR_HI_TO_INT(_paddr))
#define HOST_ADDR_LO_CMICX(_paddr) (PTR_TO_INT(_paddr))

static void
cmicx_continuous_dma_write(void *void_param)
{
    struct sbusdma_params_s *const params =
      (struct sbusdma_params_s *) void_param;
    const int   JUMP_OFFSET = 29;
    const int   REMAINING_OFFSET = 24;
    const int   DESCRIPTOR_COUNT = 33;  /* Many to allow for prefetching */

    int         endian;
    int         entries;
    int         entry_bytes;
    int         entry_width;
    int         idx;
    int         tmp;
    sbusdma_desc_t *dma_desc;
    soc_mem_info_t *minfo;
    uint32     *mdata1;
    uint32     *mdata2;
    uint32      rval;
    uint8      *ptr1;
    uint8      *ptr2;
    soc_timeout_t to;
    int         desc_num;
    int         desc_remaining;
    int         ch, cmc;
    params->status1 = SBUSDMA_STATUS_RUNNING;
   
    cmc = 1; 
    ch = params->wr_channel;

    /* Sanity checks */
    if (!SOC_MEM_IS_VALID(params->unit, params->mem)) {
#if !defined(SOC_NO_NAMES)
        cli_out("This device does not have the %s memory used by this test\n",
                soc_mem_name[params->mem]);
#else 
        cli_out("This device does not have the memory used by this test\n");
#endif
        params->status1 = SBUSDMA_STATUS_FAILURE;
        return;
    }

    if (!SOC_REG_IS_VALID(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr)) {
        cli_out
          ("This device does not support the SBUS descriptor chaining used by this test\n");
        params->status1 = SBUSDMA_STATUS_FAILURE;
        return;
    }

    /* Allocate memory for the SBUS DMA descriptors and the table data */
    dma_desc =
      soc_cm_salloc(params->unit, DESCRIPTOR_COUNT * sizeof(sbusdma_desc_t),
                    "sbus_dma_descriptors");
    if (!dma_desc) {
        cli_out("Could not allocate memory for SBUS DMA descriptors\n");
        params->status1 = SBUSDMA_STATUS_FAILURE;
        return;
    }
    sal_memset(dma_desc, 0, DESCRIPTOR_COUNT * sizeof(sbusdma_desc_t));

    minfo = &SOC_MEM_INFO(params->unit, params->mem);
    entries = minfo->index_max - minfo->index_min + 1;  /* Compute mem size */
    entry_width = ((minfo->bytes + 3) / 4) * 4;
    entry_bytes = entries * entry_width;
    mdata1 = soc_cm_salloc(params->unit, entry_bytes, "table_data");
    if (!mdata1) {
        cli_out("Could not allocate memory for table data 1\n");
        soc_cm_sfree(params->unit, dma_desc);
        params->status1 = SBUSDMA_STATUS_FAILURE;
        return;
    }

    mdata2 = soc_cm_salloc(params->unit, entry_bytes, "table_data");
    if (!mdata2) {
        cli_out("Could not allocate memory for table data 2\n");
        soc_cm_sfree(params->unit, dma_desc);
        soc_cm_sfree(params->unit, mdata1);
        params->status1 = SBUSDMA_STATUS_FAILURE;
        return;
    }

    /* Fill the table data with a pattern */
    ptr1 = (uint8 *) mdata1;
    ptr2 = (uint8 *) mdata2;
    for (idx = 0; idx < entry_bytes; idx++) {
        const uint8 val = (0x3 - (idx & 0x3)) | (idx & 0xFC);

        *ptr1++ = val;
        *ptr2++ = 0xFF - val;
    }

    /* Form the SBUS DMA descriptors
     * desc[0]: DMA mdata1 to the table
     * desc[1]: DMA mdata2 to the table
     * desc[2]: DMA mdata1 to the table
     * ...
     * desc[n-2]: DMA mdata1 to the table
     * desc[n-1]: DMA mdata2 to the table
     * desc[n]: Jump back to desc[0] */
    soc_endian_get(params->unit, &tmp, &tmp, &endian);

    desc_remaining = DESCRIPTOR_COUNT - 2;      /* Used for prefetch, does not include reload descriptor */
    if (desc_remaining > 15) {
        desc_remaining = 15;
    }

    /* Fully form a single descriptor */
    dma_desc[0].control = (desc_remaining << REMAINING_OFFSET);

    rval = 0;
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_REQUESTr,
                      &rval, WORDSWAP_IN_64BIT_SBUSDATAf, 0);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_REQUESTr,
                      &rval, HOSTMEMWR_ENDIANESSf, endian);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_REQUESTr,
                      &rval, HOSTMEMRD_ENDIANESSf, endian);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_REQUESTr,
                      &rval, REQ_WORDSf, entry_width / 4);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_REQUESTr,
                      &rval, REP_WORDSf, 0);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_REQUESTr,
                      &rval, PEND_CLOCKSf, 0);
    dma_desc[0].request = rval;
    dma_desc[0].count = entries;

    soc_schan_header_cmd_set(params->unit, (schan_header_t *) & dma_desc[0].opcode,
                             WRITE_MEMORY_CMD_MSG, SOC_BLOCK2SCH(params->unit,
                                                                 minfo->minblock), 0,
                             SOC_MEM_ACC_TYPE(params->unit, params->mem),
                             entry_width, 0, 0);

    dma_desc[0].sbus_base = minfo->base + minfo->index_min;

    dma_desc[0].hostmem_phy_base_lo =
      HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit, mdata1));
    dma_desc[0].hostmem_phy_base_hi =
      HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit, mdata1));
    if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
        dma_desc[0].hostmem_phy_base_hi |= CMIC_PCIE_SO_OFFSET;
    }
    /* Copy the fully formed descriptor to the other descriptors and update as necessary */
    for (desc_num = 1; desc_num < (DESCRIPTOR_COUNT - 1); desc_num++) {
        sal_memcpy(&dma_desc[desc_num], &dma_desc[0], sizeof(sbusdma_desc_t));

        desc_remaining = DESCRIPTOR_COUNT - desc_num - 2;
        if (desc_remaining > 15) {
            desc_remaining = 15;
        }
        dma_desc[desc_num].control = (desc_remaining << REMAINING_OFFSET);

        if (desc_num % 2) {
            dma_desc[0].hostmem_phy_base_lo =
              HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit, mdata2));
            dma_desc[0].hostmem_phy_base_hi =
              HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit, mdata2));
            if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
                dma_desc[0].hostmem_phy_base_hi |= CMIC_PCIE_SO_OFFSET;
            }
        }
    }

    /* Create the reload descriptor */
    dma_desc[desc_num].control = 1 << JUMP_OFFSET;
    dma_desc[desc_num].hostmem_phy_base_lo =
      HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit, dma_desc));
    dma_desc[desc_num].hostmem_phy_base_hi =
      HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit, dma_desc));
    if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
        dma_desc[desc_num].hostmem_phy_base_hi |= CMIC_PCIE_SO_OFFSET;
    }
#ifdef TEST_DEBUG
    cli_out("DMA WRITE: cmc: %d: ch: %d\n", cmc, ch);
    for (desc_num = 0;
         desc_num < DESCRIPTOR_COUNT;
         desc_num++) {
        cli_out("DMA WRITE: desc num:%d, Control: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)));
        cli_out("DMA WRITE  desc num:%d, Request: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+1));
        cli_out("DMA WRITE: desc num:%d, Count: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+2));
        cli_out("DMA WRITE: desc num:%d, Opcode: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+3));
        cli_out("DMA WRITE: desc num:%d, sbus addr: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+4));
        cli_out("DMA WRITE: desc num:%d, haddr_low: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+5));
        cli_out("DMA WRITE: desc num:%d, haddr_hi: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+6));
        cli_out("DMA WRITE: desc num:%d, Reserved: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+7));
            cli_out("\n");
    }
#endif

    /* Configure descriptor mode */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval,
                        DESCRIPTOR_ENDIANESSf, endian);
    if (soc_reg_field_valid(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr,
                        DESC_PREFETCH_ENABLEf)) {
        soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval,
                           DESC_PREFETCH_ENABLEf, 1);
    }
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval, MODEf, 1);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);


    /* Set the starting descriptor address */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_LO(cmc,ch));
    if (SOC_REG_IS_VALID(params->unit,
                CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_LOr)) {
        soc_reg_field_set(params->unit,
                          CMIC_CMC1_SBUSDMA_CH1_DESC_START_ADDRESS_LOr, &rval,
                          ADDRESSf,
                          HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit,
                                                dma_desc)));
    }
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_LO(cmc,ch), rval);

    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(cmc,ch));
    if (SOC_REG_IS_VALID(params->unit,
                CMIC_CMC1_SBUSDMA_CH1_DESC_START_ADDRESS_HIr)) {
        soc_reg_field_set(params->unit,
                          CMIC_CMC1_SBUSDMA_CH1_DESC_START_ADDRESS_HIr, &rval,
                          ADDRESSf,
                          HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit,
                                                dma_desc)));
    }
    if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
        rval |= CMIC_PCIE_SO_OFFSET;
    }
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(cmc,ch), rval);

    /* Start the DMA */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval, STARTf, 1);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);




    cli_out("Continuous Write DMA has been started: Runtime %d seconds\n",
            params->test_time);
    cli_out("Use the following to see the values changing in the table:\n");
    cli_out("    loop 100 \'d raw nocache NONUCAST_TRUNK_BLOCK_MASK 0 2\'\n");

    for (params->write_secs = 0; params->write_secs < params->test_time;
         params->write_secs++) {
        if ((params->flags & SBUSDMA_WRITE_RUN) == 0) {
            break;
        }
        sal_sleep(1);
        rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc,ch));
        if (rval & 0x7FE) {
            /* One of the error bits is set */
            cli_out("Error in SBUS DMA STATUS: %08X\n", rval);
            params->status1 = SBUSDMA_STATUS_FAILURE;
            break;
        }
    }

    if (params->status1 == SBUSDMA_STATUS_RUNNING) {
        params->status1 = SBUSDMA_STATUS_SUCCESS;
    }

    cli_out("About to stop the  Write DMA: %d seconds run time\n", params->write_secs );
    params->flags &= ~SBUSDMA_WRITE_RUN;

    /* Stop the DMA */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval, ABORTf, 1);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);

    soc_timeout_init(&to, 1000000, 0);
    do {
        rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc,ch));
        if (soc_reg_field_get(params->unit, CMIC_CMC1_SBUSDMA_CH1_STATUSr, rval, DONEf)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            cli_out("DMA read abort timeout!\n");
            params->status1 = SBUSDMA_STATUS_FAILURE;
            break;
        }
    } while (1);

    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval, STARTf, 0);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr, &rval, ABORTf, 0);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);
    soc_cm_sfree(params->unit, dma_desc);
    soc_cm_sfree(params->unit, mdata1);
    soc_cm_sfree(params->unit, mdata2);

    return;
}
static void
cmicx_continuous_dma_read(void *void_param)
{
    struct sbusdma_params_s *const params =
      (struct sbusdma_params_s *) void_param;
    const int   JUMP_OFFSET = 29;
    const int   REMAINING_OFFSET = 24;
    const int   DESCRIPTOR_COUNT = 33;  /* Many to allow for prefetching */

    int         endian;
    int         entries;
    int         entry_bytes;
    int         entry_width;
    int         tmp;
    sbusdma_desc_t *dma_desc;
    soc_mem_info_t *minfo;
    uint32      check[8];
    uint32     *mdata1;
    uint32      rval;
    int         print_count = 0;
    soc_timeout_t to;
    int         desc_num;
    int         desc_remaining;
    int         ch, cmc;

    cmc = 0;
    ch = params->rd_channel;
    params->status2 = SBUSDMA_STATUS_RUNNING;

    /* Sanity checks */
    if (!SOC_MEM_IS_VALID(params->unit, params->mem)) {
#if !defined(SOC_NO_NAMES)
        cli_out("This device does not have the %s memory used by this test\n",
                soc_mem_name[params->mem]);
#else 
        cli_out("This device does not have the memory used by this test\n");
#endif
        params->status2 = SBUSDMA_STATUS_FAILURE;
        return;
    }

    /* Allocate memory for the SBUS DMA descriptors and the table data */
    dma_desc =
      soc_cm_salloc(params->unit, DESCRIPTOR_COUNT * sizeof(sbusdma_desc_t),
                    "sbus_dma_descriptors");
    if (!dma_desc) {
        cli_out("Could not allocate memory for SBUS DMA descriptors\n");
        params->status2 = SBUSDMA_STATUS_FAILURE;
        return;
    }
    sal_memset(dma_desc, 0, DESCRIPTOR_COUNT * sizeof(sbusdma_desc_t));

    minfo = &SOC_MEM_INFO(params->unit, params->mem);
    entries = minfo->index_max - minfo->index_min + 1;  /* Compute mem size */
    entry_width = ((minfo->bytes + 3) / 4) * 4;
    entry_bytes = entries * entry_width;
    mdata1 = soc_cm_salloc(params->unit, entry_bytes, "table_data");
    if (!mdata1) {
        cli_out("Could not allocate memory for table data 1\n");
        soc_cm_sfree(params->unit, dma_desc);
        params->status2 = SBUSDMA_STATUS_FAILURE;
        return;
    }
    sal_memset(mdata1, 0xA5, entry_bytes);
    sal_memcpy(check, mdata1, sizeof(check));

    /* Form the SBUS DMA descriptors
     * desc[0]: DMA table entry to mdata1
     * desc[1]: DMA table entry to mdata1
     * desc[2]: DMA table entry to mdata1
     * ...
     * desc[n-2]: DMA table entry to mdata1
     * desc[n-1]: DMA table entry to mdata1
     * desc[n]: Jump back to desc[0] */
    soc_endian_get(params->unit, &tmp, &tmp, &endian);

    desc_remaining = DESCRIPTOR_COUNT - 2;      /* Used for prefetch, does not include reload descriptor */
    if (desc_remaining > 15) {
        desc_remaining = 15;
    }

    /* Fully form a single descriptor */
    dma_desc[0].control = (desc_remaining << REMAINING_OFFSET);

    rval = 0;
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_REQUESTr,
                      &rval, WORDSWAP_IN_64BIT_SBUSDATAf, 0);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_REQUESTr,
                      &rval, HOSTMEMWR_ENDIANESSf, endian);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_REQUESTr,
                      &rval, HOSTMEMRD_ENDIANESSf, endian);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_REQUESTr,
                      &rval, REQ_WORDSf, 0);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_REQUESTr,
                      &rval, REP_WORDSf, entry_width / 4);
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_REQUESTr,
                      &rval, PEND_CLOCKSf, 0);
    dma_desc[0].request = rval;
    dma_desc[0].count = entries;

    soc_schan_header_cmd_set(params->unit, (schan_header_t *) & dma_desc[0].opcode,
                             READ_MEMORY_CMD_MSG, SOC_BLOCK2SCH(params->unit,
                                                                minfo->minblock), 0,
                             SOC_MEM_ACC_TYPE(params->unit, params->mem),
                             entry_width, 0, 0);

    dma_desc[0].sbus_base = minfo->base + minfo->index_min;

    dma_desc[0].hostmem_phy_base_lo =
      HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit, mdata1));
    dma_desc[0].hostmem_phy_base_hi =
      HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit, mdata1));
    if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
        dma_desc[0].hostmem_phy_base_hi |= CMIC_PCIE_SO_OFFSET;
    }

    /* Copy the fully formed descriptor to the other descriptors and update as necessary */
    for (desc_num = 1; desc_num < (DESCRIPTOR_COUNT - 1); desc_num++) {
        sal_memcpy(&dma_desc[desc_num], &dma_desc[0], sizeof(sbusdma_desc_t));

        desc_remaining = DESCRIPTOR_COUNT - desc_num - 2;
        if (desc_remaining > 15) {
            desc_remaining = 15;
        }
        dma_desc[desc_num].control = (desc_remaining << REMAINING_OFFSET);
    }

    /* Create the reload descriptor */
    dma_desc[desc_num].control = 1 << JUMP_OFFSET;
    dma_desc[desc_num].hostmem_phy_base_lo =
      HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit, dma_desc));
    dma_desc[desc_num].hostmem_phy_base_hi =
      HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit, dma_desc));
    if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
        dma_desc[desc_num].hostmem_phy_base_hi |= CMIC_PCIE_SO_OFFSET;
    }

#ifdef TEST_DEBUG
    cli_out("DMA READ: cmc: %d: ch: %d\n", cmc, ch);
    for (desc_num = 0;
         desc_num < DESCRIPTOR_COUNT;
         desc_num++) {
        cli_out("DMA READ: desc num:%d, Control: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)));
        cli_out("DMA READ  desc num:%d, Request: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+1));
        cli_out("DMA READ: desc num:%d, Count: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+2));
        cli_out("DMA READ: desc num:%d, Opcode: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+3));
        cli_out("DMA READ: desc num:%d, sbus addr: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+4));
        cli_out("DMA READ: desc num:%d, haddr_low: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+5));
        cli_out("DMA READ: desc num:%d, haddr_hi: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+6));
        cli_out("DMA READ: desc num:%d, Reserved: 0x%08x\n", desc_num, *((uint32 *) dma_desc + (desc_num*8)+7));
            cli_out("\n");
    }
#endif

    /* Configure descriptor mode */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_CONTROLr, &rval,
                      DESCRIPTOR_ENDIANESSf, endian);
    if (soc_reg_field_valid(params->unit, CMIC_CMC1_SBUSDMA_CH1_CONTROLr,
                        DESC_PREFETCH_ENABLEf)) {
        soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_CONTROLr, &rval,
                          DESC_PREFETCH_ENABLEf, 1);
    }
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_CONTROLr, &rval, MODEf, 1);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);

    /* Set the starting descriptor address */
    
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_LO(cmc,ch));
    if (SOC_REG_IS_VALID(params->unit,
                            CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_LOr)) {
        soc_reg_field_set(params->unit,
                          CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_LOr, &rval,
                          ADDRESSf,
                          HOST_ADDR_LO_CMICX(soc_cm_l2p(params->unit,
                                                dma_desc)));
    }
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_LO(cmc,ch), rval);

    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(cmc,ch));
    if (SOC_REG_IS_VALID(params->unit,
                                CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_HIr)) {
        soc_reg_field_set(params->unit,
                          CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_HIr, &rval,
                          ADDRESSf,
                          HOST_ADDR_HI_CMICX(soc_cm_l2p(params->unit,
                                                dma_desc)));
    }
    if (soc_cm_get_bus_type(params->unit) & SOC_PCI_DEV_TYPE) {
        rval |= CMIC_PCIE_SO_OFFSET;
    }
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_DESCADDR_HI(cmc,ch), rval);

    /* Start the DMA */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    (soc_reg32_get(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), REG_PORT_ANY,
                        0, &rval));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_CONTROLr, &rval, STARTf, 1);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);

    cli_out("Continuous Read DMA has been started: Runtime %d seconds\n",
            params->test_time);

    for (params->read_secs = 0;params->read_secs < params->test_time;
         params->read_secs++) {
        if ((params->flags & SBUSDMA_READ_RUN) == 0) {
            /* Test aborted */
            break;
        }
        sal_sleep(1);
        rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc,ch));
        if (rval & 0x7FE) {
            /* One of the error bits is set */
            cli_out("Error in SBUS DMA STATUS: %08X\n", rval);
            params->status2 = SBUSDMA_STATUS_FAILURE;
            break;
        }
        if (sal_memcmp(mdata1, check, sizeof(check))) {
            if (print_count++ < 5) {
                cli_out("DMA Read: %08X %08X %08X %08X %08X %08X %08X %08X\n",
                        mdata1[0], mdata1[1], mdata1[2], mdata1[3], mdata1[4],
                        mdata1[5], mdata1[6], mdata1[7]);
            }
            sal_memcpy(check, mdata1, sizeof(check));
        }
    }
    cli_out("About to stop the Read DMA: %d seconds run time\n", params->read_secs);
    params->flags &= ~SBUSDMA_READ_RUN;

    /* Stop the DMA */
    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_CONTROLr, &rval, ABORTf, 1);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);

    soc_timeout_init(&to, 1000000, 0);
    do {
        rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_STATUS(cmc,ch));
        if (soc_reg_field_get(params->unit, CMIC_CMC1_SBUSDMA_CH0_STATUSr, rval, DONEf)) {
            break;
        }

        if (soc_timeout_check(&to)) {
            cli_out("DMA read abort timeout!\n");
            params->status2 = SBUSDMA_STATUS_FAILURE;
            break;
        }
    } while (1);
    if (params->status2 != SBUSDMA_STATUS_FAILURE) {
        params->status2 = SBUSDMA_STATUS_SUCCESS;
    }

    rval = soc_pci_read(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch));
    soc_reg_field_set(params->unit, CMIC_CMC1_SBUSDMA_CH0_CONTROLr, &rval, STARTf, 0);
    soc_pci_write(params->unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc,ch), rval);

    soc_cm_sfree(params->unit, dma_desc);
    soc_cm_sfree(params->unit, mdata1);

    return;
}
static const char sbusdma_stress_test_usage[] =
  "SBUS DMA stress test usage:\n"
  "RdChanBitmap=<hex> - specify which of the 9 available CMC/channels to read\n"
  "                     default is channel 0 for read, multi channels can be assigned to read\n"
  "WrChanBitmap=<hex> - specify which of the 9 available CMC/channels to write\n"
  "                     default is channel 1 for write, multi channels can be assigned to write\n"
  "Seconds=<int>      - specify test time, default is 10 seconds\n";

int sb_stress_test(int unit, args_t *a, void *pa)
{
    static sbusdma_params_t sbusdma_params;
    static int  needs_init = 1;
    int wrchbmp = 0, rdchbmp = 0;
    parse_table_t pt;
    const int   DEFAULT_TEST_TIME = 60;
    int index, count;

    int         seconds = DEFAULT_TEST_TIME;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "WrChanBitmap", PQ_DFL | PQ_INT, 0, &wrchbmp, 0);
    parse_table_add(&pt, "RdChanBitmap", PQ_DFL | PQ_INT, 0, &rdchbmp, 0);
    parse_table_add(&pt, "seconds", PQ_DFL | PQ_INT, 0, &seconds, 0);

   if ((parse_arg_eq(a, &pt)) < 0 || (ARG_CNT(a) > 0)) {
        cli_out("%s", sbusdma_stress_test_usage);
        cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(a),
                ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return BCM_E_FAIL;
    }

    parse_arg_eq_done(&pt);


    if (needs_init) {
        sal_memset(&sbusdma_params, 0, sizeof(sbusdma_params_t));
        needs_init = 0;
    }

    if (wrchbmp & rdchbmp) {
        cli_out("WrChanBitmap=0x%x and RdChanBitmap=0x%x are conflict\n", wrchbmp, rdchbmp);
        return BCM_E_FAIL;
    } else if ((wrchbmp == 0) && (rdchbmp == 0)) {
        /* Default channel */
        rdchbmp = 0x1;
        wrchbmp = 0x2;
    }
    count = 0;
    for (index = 0; index < SBUSDMA_CH_MAX_NUM; index++) {

        if (wrchbmp & (1 << index)) {
            const soc_mem_t mem = NONUCAST_TRUNK_BLOCK_MASKm;
            const int   priority = 100;

            char        thread_name[64];
    
            if (sbusdma_params.status1 == SBUSDMA_STATUS_RUNNING) {
                cli_out("SBUSDMA Write test in progress\n");
                return CMD_OK;
            }
    
            sbusdma_params.unit = unit;
            sbusdma_params.mem = mem;
            sbusdma_params.flags |= SBUSDMA_WRITE_RUN;
            sbusdma_params.test_time = seconds;
            sbusdma_params.wr_channel = index;
#if !defined(SOC_NO_NAMES)
            sal_sprintf(thread_name, "Write SBUSDMA-%s", soc_mem_name[mem]);
#else
            sal_sprintf(thread_name, "Write SBUSDMA-%d", index);
#endif
            /* Start DMA thread. */
            sbusdma_params.tid = sal_thread_create(thread_name,
                                                         SAL_THREAD_STKSZ,
                                                         priority,
                                                         cmicx_continuous_dma_write,
                                                         &sbusdma_params);
            count++;
        } else if (rdchbmp & (1<< index)) {
            const soc_mem_t mem = NONUCAST_TRUNK_BLOCK_MASKm;
            const int   priority = 100;
    
            char        thread_name[64];
    
            if (sbusdma_params.status2 == SBUSDMA_STATUS_RUNNING) {
                cli_out("SBUSDMA Read test in progress\n");
                return CMD_OK;
            }
            sbusdma_params.unit = unit;
            sbusdma_params.mem = mem;
            sbusdma_params.flags |= SBUSDMA_READ_RUN;
            sbusdma_params.test_time = seconds;
            sbusdma_params.rd_channel = index;
#if !defined(SOC_NO_NAMES)
            sal_sprintf(thread_name, "Read SBUSDMA-%s", soc_mem_name[mem]);
#else
            sal_sprintf(thread_name, "Read SBUSDMA-%d", index);
#endif

            /* Start DMA thread. */
            sbusdma_params.tid = sal_thread_create(thread_name,
                                                        SAL_THREAD_STKSZ,
                                                        priority, cmicx_continuous_dma_read,
                                                        &sbusdma_params);
            count++;
        } 

    #if 0
else if (!sal_strcasecmp(subcmd, "stopw")) {
            sbusdma_params.flags &= ~SBUSDMA_WRITE_RUN;
            sal_sleep(2);
            cli_out("Write test status: %s\n", status_strings[sbusdma_params.status]);
        } else if (!sal_strcasecmp(subcmd, "stopr")) {
            sbusdma_params.flags &= ~SBUSDMA_READ_RUN;
            sal_sleep(2);
            cli_out("Read test status: %s\n", status_strings[sbusdma_params.status]);
        } else if (!sal_strcasecmp(subcmd, "status")) {
            if (sbusdma_params.status == SBUSDMA_STATUS_RUNNING) {
                cli_out("Read DMA in progress:  seconds out of %d\n",
                        sbusdma_params.test_time);
            } else {
                cli_out("Read DMA last status: %s\n",
                            status_strings[sbusdma_params.status]);
            }
            if (sbusdma_params.status == SBUSDMA_STATUS_RUNNING) {
                cli_out("Write DMA in progress: seconds out of %d\n",
                             sbusdma_params.test_time);
            } else {
                    cli_out("Write DMA last status: %s\n",
                            status_strings[sbusdma_params.status]);
            }
        } else {
            cli_out("Unknown sub-command\n");
            return CMD_USAGE;
        }
#endif
    }   
    
    if (count == 0) {
        cli_out("No valid channel assigned\n");
        return BCM_E_FAIL;
    }
    return CMD_OK;
}
#endif
