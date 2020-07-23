/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:  CMICM FIFO DMA driver.
 */

#include <shared/bsl.h>
#include <shared/bitop.h>


#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/l2x.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/er_tcam.h>
#include <soc/triumph3.h>
#endif

#ifdef BCM_KATANA2_SUPPORT
#include <soc/katana2.h>
#include <soc/katana.h>
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)

#if (!defined(_LIMITS_H)) && !defined(_LIBC_LIMITS_H_)

#if (!defined(INT_MIN)) && !defined(INT_MAX)
#define INT_MIN (((int)1)<<(sizeof(int)*8-1))
#define INT_MAX (~INT_MIN)
#endif

#ifndef UINT_MAX
#define UINT_MAX ((unsigned)-1)
#endif

#endif

#ifdef BCM_KATANA2_SUPPORT
#define RD_DMA_CFG_REG                   0
#define RD_DMA_HOTMEM_THRESHOLD_REG      1
#define RD_DMA_STAT                      2
#define RD_DMA_STAT_CLR                  3
#endif

#if defined(BCM_CMICM_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)

#define API_DISABLE  0
#define API_ENABLE   1

#define API_USE      1
#define API_NOT_USE  2

typedef struct _l2_mod_fifo_enable {
    uint32      api_val;
    uint32      api_is_set;
} _l2_mod_fifo_enable_t;

typedef struct _aux_arb_cntl {
    uint32      api_val;
    uint32      api_is_set;
} _aux_arb_cntl_t;

static _aux_arb_cntl_t aux_arb_cntl_reg[SOC_MAX_NUM_DEVICES];
static _l2_mod_fifo_enable_t l2_mod_fifo_reg[SOC_MAX_NUM_DEVICES];

#define API_SET_AUX_ARB_CNTL(unit, field, fvalue) do { \
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, \
                              &aux_arb_cntl_reg[unit].api_val, field, \
                              ((fvalue) ? API_ENABLE: API_DISABLE)); \
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, \
                              &aux_arb_cntl_reg[unit].api_is_set, field, API_USE); \
        } while(0)

#define API_GET_AUX_ARB_CNTL(unit, field) \
            ((soc_reg_field_get(unit, AUX_ARB_CONTROLr, \
                                aux_arb_cntl_reg[unit].api_is_set, field)) ? \
                ((soc_reg_field_get(unit, AUX_ARB_CONTROLr, \
                                    aux_arb_cntl_reg[unit].api_val, field)) ? \
                     API_ENABLE :  API_DISABLE) : \
                 API_NOT_USE)


#define API_SET_L2_MOD_FIFO_ENABLE(unit, field, fvalue) do { \
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, \
                              &l2_mod_fifo_reg[unit].api_val, field, \
                              ((fvalue) ? API_ENABLE: API_DISABLE)); \
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, \
                              &l2_mod_fifo_reg[unit].api_is_set, field, API_USE); \
        } while(0)

#define API_GET_L2_MOD_FIFO_ENABLE(unit, field) \
            ((soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, \
                                l2_mod_fifo_reg[unit].api_is_set, field)) ? \
                ((soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, \
                                    l2_mod_fifo_reg[unit].api_val, field)) ? \
                     API_ENABLE :  API_DISABLE) : \
                 API_NOT_USE)
#endif

#ifdef BCM_CMICM_SUPPORT

typedef struct _hostmem_address_remap_allocation{
    int in_use;
    int cmc;
    int ch;
} _hostmem_address_remap_allocation_t;

#define NOF_HOSTMEM_ADDR_REMAP_ENTRIES (16)
#define HOSTMEM_ADDR_REMAP_POSITION (28)
#define HOSTMEM_ADDR_REMAP_MASK (0xF)


_hostmem_address_remap_allocation_t hostmem_address_remap_allocation[SOC_MAX_NUM_DEVICES][NOF_HOSTMEM_ADDR_REMAP_ENTRIES];

static void
_soc_mem_fifo_dma_address_remap_set(
    int unit,
    int cmc,
    int entry_idx,
    uint32 value)
{   
    uint32 remap_address[NOF_HOSTMEM_ADDR_REMAP_ENTRIES];                     
    soc_field_t remap_f[NOF_HOSTMEM_ADDR_REMAP_ENTRIES] = {
                     ADDR_0f, ADDR_1f, ADDR_2f, ADDR_3f, ADDR_4f,
                     ADDR_0f, ADDR_1f, ADDR_2f, ADDR_3f, ADDR_4f,
                     ADDR_0f, ADDR_1f, ADDR_2f, ADDR_3f, ADDR_4f,
                     ADDR_0f};
    uint32 rval;
    
    remap_address[0] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc);
    remap_address[1] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc);
    remap_address[2] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc);
    remap_address[3] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc);
    remap_address[4] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc);
    remap_address[5] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc);
    remap_address[6] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc);
    remap_address[7] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc);
    remap_address[8] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc);
    remap_address[9] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc);
    remap_address[10] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc);
    remap_address[11] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc);
    remap_address[12] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc);
    remap_address[13] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc);
    remap_address[14] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc);
    remap_address[15] = CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc);
    
    rval = soc_pci_read(unit, remap_address[entry_idx]);
    soc_reg_field_set(unit, CMIC_CMC0_HOSTMEM_ADDR_REMAP_0r, &rval, remap_f[entry_idx], value);
    soc_pci_write(unit, remap_address[entry_idx], rval);
}    

int
_soc_mem_address_remap_allocation_init(
    int unit)
{
    int base_cmc = SOC_PCI_CMC(unit);
    int num_of_cmc = SOC_PCI_CMCS_NUM(unit);
    int cmc, entry_idx;
    char *propkey;
    uint32 remap_entries_reserved;

    propkey = spn_HOST_MEMORY_ADDRESS_REMAP_ENTRIES_RESERVED_CMC;
    sal_memset(hostmem_address_remap_allocation[unit], 0x0, 
        sizeof(_hostmem_address_remap_allocation_t)*NOF_HOSTMEM_ADDR_REMAP_ENTRIES);  

    /* consider remap_entries_reserved!=0 only for cmc=1 and pci_cmc=0 */
    /* this is the only constelation that the remap management is allowed */
    for(cmc=base_cmc; cmc<(num_of_cmc+base_cmc); cmc++)
    {
        remap_entries_reserved = soc_property_port_get(unit, cmc, propkey, 0);
        if(remap_entries_reserved != 0)
        {
           if ((cmc != 1) || (base_cmc != 0))
           {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U
                    (unit, "Soc host_memory_address_remap_entries_cmc_%d!=0 is allowed only for cmc=1 and pci_cmc=0\n"), cmc));
                return SOC_E_PARAM;                
           }
           else
           {
                /* mark the reserved entries in database */
                for(entry_idx = 0; entry_idx < NOF_HOSTMEM_ADDR_REMAP_ENTRIES; entry_idx++)
                {
                    if(SHR_BITGET(&remap_entries_reserved, entry_idx))
                    {
                        hostmem_address_remap_allocation[unit][entry_idx].in_use = TRUE;
                        hostmem_address_remap_allocation[unit][entry_idx].cmc = -1;
                        hostmem_address_remap_allocation[unit][entry_idx].ch = -1;                                                
                    }
                    else
                    {
                        /* clear the HW table for none ukernel (Arm-firmware) entries */
                        _soc_mem_fifo_dma_address_remap_set(unit, cmc, entry_idx, 0);
                    }
                }
           }
        }
    }           
    return SOC_E_NONE;
}

int
_soc_mem_address_remap_dealloc(
    int unit, 
    int cmc, 
    int ch)
{
    int entry_idx;
    
    for(entry_idx = 0; entry_idx < NOF_HOSTMEM_ADDR_REMAP_ENTRIES; entry_idx++)
    {
        if(hostmem_address_remap_allocation[unit][entry_idx].cmc == cmc &&
           hostmem_address_remap_allocation[unit][entry_idx].ch == ch &&
           hostmem_address_remap_allocation[unit][entry_idx].in_use == TRUE)
        {
            hostmem_address_remap_allocation[unit][entry_idx].in_use = FALSE;
            /* clear HW */
            _soc_mem_fifo_dma_address_remap_set(unit, cmc, entry_idx, 0);
        }
    }
    
    return SOC_E_NONE;    
}

int
_soc_mem_address_remap_alloc(
    int unit, 
    int cmc, 
    int ch, 
    int nof_entries, 
    uint32 remap_val,
    uint32 *remap_entry_base)
{
    int entry_idx;
    int base_entry = 0, available_entries = 0;

    if((nof_entries == 0) || (nof_entries > NOF_HOSTMEM_ADDR_REMAP_ENTRIES))
    {
        return SOC_E_INTERNAL;
    }
    /* find free entry. If need to allocate more than one entry, all the entries must be consecutive */
    for(entry_idx = 0; (entry_idx < NOF_HOSTMEM_ADDR_REMAP_ENTRIES); entry_idx++)
    {        
        if(hostmem_address_remap_allocation[unit][entry_idx].in_use == FALSE)
        {
            if(available_entries == 0)
            {
                base_entry = entry_idx;
            }
            available_entries++;
        }
        else
        {
            available_entries = 0;
        }
        if(available_entries == nof_entries)
        {
            break;
        }
    }
    if(entry_idx == NOF_HOSTMEM_ADDR_REMAP_ENTRIES)
    {
        return SOC_E_RESOURCE;
    }
    /* update database and write to HW */
    for (entry_idx = base_entry; entry_idx < (base_entry + nof_entries); entry_idx++)
    {
        hostmem_address_remap_allocation[unit][entry_idx].in_use = TRUE;
        hostmem_address_remap_allocation[unit][entry_idx].cmc = cmc;
        hostmem_address_remap_allocation[unit][entry_idx].ch = ch;    
        /* write to HW: update the remap value into the relevant register */        
        _soc_mem_fifo_dma_address_remap_set(unit, cmc, entry_idx, remap_val);
        remap_val++;        
    }

    * remap_entry_base = base_entry;
    
    return SOC_E_NONE;
}

/**
 * Function used to update HOSTMEM_ADDR_REMAP in case that it wasn't made during init stage.
 * This function is called for manage a scenario when both SDK and Ukernel (ARM) use the same CMC
 * SDK use the DMA channels and ARM use other CMC capbilities.
 * Still, the remap table is common per cmc and need to verify that both application 
 * do not run each other entries in the table. 
 * Here we just write the value into the given entry.
 *
 * @param unit
 * @param cmc - cmc to update
 * @param ch - Channel used. should be between 0 and 3. 
 * @param phy_address - the phiscal address wich need to remap.
 * @param remap_entry_base - indicates what is the base remap entry that need to update. 
 *                           relevant only for cmc1. if remap_entry_base=-1, no remap is required.
 * @param base_entry - output value which indicate which is the base entry that allocated, 
 *                      and will be used in the hostmem start address offset register
 *
 *
 * @return int - success or failure.
 */
static int
_soc_mem_fifo_dma_address_remap_handle(
    int unit,
    int cmc,
    int ch,
    uint32 phy_address,
    uint32 *base_entry)
{
    int nof_entries = 1;
    uint32 remap_entry_base;
    uint32 remamp_base_val, remap_val;
    uint32 rval;    

    /** calculate the remap value: { 1'b0, 1'b1, physical_address_bits_31..28(4) } */
    remamp_base_val = ((phy_address >> HOSTMEM_ADDR_REMAP_POSITION) & HOSTMEM_ADDR_REMAP_MASK);
    remap_val = (0x10 | remamp_base_val);

    /* if the phy_address-bits31..28 is not the max value, we will allocate another entry consecutive to the base entry */
    /* for case that the size of the host_buffer crossing "sections" */
    if(remamp_base_val != HOSTMEM_ADDR_REMAP_MASK)
    {
        nof_entries++;
    }

    rval = _soc_mem_address_remap_alloc(unit, cmc, ch, nof_entries, remap_val, &remap_entry_base);
    if (rval != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "can't alloc hostmem address remap entry\n")));
        return rval;
    }
        
    *base_entry = remap_entry_base;
    return SOC_E_NONE;
}
                                      

/* DMA FIFO init for registers or memories */

/**
 * Function used to (partially) configure the DMA to
 * write to a redirect writes from a register/memory
 * to a host memory.
 *
 *
 * @param unit
 * @param ch - Channel used. should be between 0 and 3.
 * @param is_mem
 * @param mem -name of memory, if memory is used (0 otherwise).
 * @param reg - name of register, if register used (0 otherwise).
 * @param copyno
 * @param force_entry_size - in a case that entry size does not match
 *        the register\memory size  - ignore when equals to 0.
 * @param host_entries  - number of host entries used by the DMA.
 *  Must be a power of 2 between 64 and 16384.
 *  Note that only after host_entries writes does the DMA wrap around the host_buff.
 * @param host_buff - local memory on which the DMA writes. should be big enough to
 *                     allow host_entries writes,
 * in other words whould be the size of host_entries * ( size of memory or
 *                         register used, in bytes).
 * Other register the caller may need to configure seperatly:
 *  CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD - The amount of writes by the DMA until a
 *  threshold based interrupt occurs.
 *
 *  TIMEOUT_COUNT field in CMIC_CMCx_FIFO_CHy_RD_DMA - Time between a DMA write and a
 *                          timeout based interrupt.
 *  (May be set to 0 to disable timeout based interrupts).
 *
 *  Endianess field in CMIC_CMCx_FIFO_CHy_RD_DMA.
 *
 * @return int - success or failure.
 */
int
_soc_mem_sbus_fifo_dma_start_memreg(int unit, int ch,
                                    int is_mem, soc_mem_t mem, soc_reg_t reg,
                                    int copyno, int force_entry_size,
                                    int host_entries, void *host_buff)
{
    uint8 at;
    uint32 rval, data_beats, sel, phy_address;
    int cmc;
    int blk;
    schan_msg_t msg;
    int acc_type;
    uint32 base_remap_entry = 0;
    char *propkey;
    uint32 remap_entries_reserved;

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12 || host_buff == NULL) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3 || host_buff == NULL) {
            return SOC_E_PARAM;
        }
    }

    switch (host_entries) {
    case 64:    sel = 0; break;
    case 128:   sel = 1; break;
    case 256:   sel = 2; break;
    case 512:   sel = 3; break;
    case 1024:  sel = 4; break;
    case 2048:  sel = 5; break;
    case 4096:  sel = 6; break;
    case 8192:  sel = 7; break;
    case 16384: sel = 8; break;
    default:
        return SOC_E_PARAM;
    }

#ifdef BCM_IPFIX_SUPPORT
#ifdef BCM_PETRA_SUPPORT
    if (!SOC_IS_SAND(unit))
#endif
    {
        if (mem != ING_IPFIX_EXPORT_FIFOm && mem != EGR_IPFIX_EXPORT_FIFOm &&
            mem !=  L2_MOD_FIFOm && mem != FT_EXPORT_FIFOm &&
            mem != FT_EXPORT_DATA_ONLYm &&
            mem != EGR_SER_FIFOm && mem != ING_SER_FIFOm &&
            mem != CENTRAL_CTR_EVICTION_FIFOm) {
            return SOC_E_BADID;
        }
    }
#endif

    /* Differentiate between reg and mem to get address, size and block */
    if ((!is_mem) && SOC_REG_IS_VALID(unit, reg)) {
        data_beats = BYTES2WORDS(soc_reg_bytes(unit, reg));
        rval = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &blk, &at);
    } else {
        data_beats = soc_mem_entry_words(unit, mem);
        if (copyno == MEM_BLOCK_ANY) {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
        rval = soc_mem_addr_get(unit, mem, 0, copyno, 0, &at);
        blk = SOC_BLOCK2SCH(unit, copyno);
    }
    /*Force entry size*/
    if (force_entry_size > 0)
    {
        data_beats = BYTES2WORDS(force_entry_size);
    }

    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(cmc, ch), rval);

    schan_msg_clear(&msg);
    if (is_mem) {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    } else {
        acc_type = 0;
    }
    soc_schan_header_cmd_set(unit, &msg.header, FIFO_POP_CMD_MSG, blk, 0,
                             acc_type, 4, 0, 0);

    /* Set 1st schan ctrl word as opcode */
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE_OFFSET(cmc, ch), msg.dwords[0]);

    phy_address = soc_cm_l2p(unit, host_buff);

    /** workarroud for issue when both ukernel (ARM) and SDK (Pci-e) use cmc-1 and need to share the remap entries*/
    /** in this case, the remap table is not 1x1 and need to update the relevant entries */    
    propkey = spn_HOST_MEMORY_ADDRESS_REMAP_ENTRIES_RESERVED_CMC;    
    remap_entries_reserved = soc_property_port_get(unit, cmc, propkey, 0);
    if ((SOC_IS_JERICHO(unit)) && 
        (SOC_PCI_CMCS_NUM(unit) > 1) && (remap_entries_reserved != 0))
    {    
        rval = _soc_mem_fifo_dma_address_remap_handle(unit, cmc, ch, phy_address, &base_remap_entry); 
        if (rval != SOC_E_NONE)
        {
            return rval;
        }
        
        phy_address = ((phy_address & ((1<<HOSTMEM_ADDR_REMAP_POSITION) - 1)) | (base_remap_entry << HOSTMEM_ADDR_REMAP_POSITION));
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(cmc, ch),
                      phy_address);         
    }
    else
    {
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(cmc, ch),
                      phy_address);    
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, BEAT_COUNTf,
                      data_beats);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval,
                      HOST_NUM_ENTRIES_SELf, sel);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ABORTf, 0);

        /* Note: Following might need to be tuned */
        soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval,
                      TIMEOUT_COUNTf, 1000);
        /* Note: Following could be removed ?? */
        soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval,
                      NACK_FATALf, 1);

    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        
    }
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    if (SOC_IS_SAND(unit)) {
        /* Always 1 in Arad when interrupt mechanism is not enabled */
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),
                     1);
    } else {
        /* Note: Following might need to be tuned */
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),
                      host_entries/10);
    }
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 1);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);
    return SOC_E_NONE;
}

#ifdef BCM_KATANA2_SUPPORT
    int
_soc_kt2_mem_sbus_fifo_dma_start_memreg(int unit, int ch,
        int is_mem, soc_mem_t mem, soc_reg_t reg,
        int copyno, int host_entries, void *host_buff)
{
    uint8 at;
    uint32 rval, data_beats, sel;
    int cmc;
    int blk;
    schan_msg_t msg;
    soc_reg_t cfg_reg;
    int acc_type;
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12 || host_buff == NULL) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3 || host_buff == NULL) {
            return SOC_E_PARAM;
        }
    }

    switch (host_entries) {
        case 64:    sel = 0; break;
        case 128:   sel = 1; break;
        case 256:   sel = 2; break;
        case 512:   sel = 3; break;
        case 1024:  sel = 4; break;
        case 2048:  sel = 5; break;
        case 4096:  sel = 6; break;
        case 8192:  sel = 7; break;
        case 16384: sel = 8; break;
        default:
                    return SOC_E_PARAM;
    }

    if (mem != L2_MOD_FIFOm) {
        return SOC_E_BADID;
    }

    /* Differentiate between reg and mem to get address, size and block */
    if ((!is_mem) && SOC_REG_IS_VALID(unit, reg)) {
        data_beats = BYTES2WORDS(soc_reg_bytes(unit, reg));
        rval = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &blk, &at);
    } else {
        data_beats = soc_mem_entry_words(unit, mem);
        if (copyno == MEM_BLOCK_ANY) {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
        rval = soc_mem_addr_get(unit, mem, 0, copyno, 0, &at);
        blk = SOC_BLOCK2SCH(unit, copyno);
    }

    soc_pci_write(unit,
            CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(cmc, ch), rval);

    schan_msg_clear(&msg);
    if (is_mem) {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    } else {
        acc_type = 0;
    }
    soc_schan_header_cmd_set(unit, &msg.header, FIFO_POP_CMD_MSG,
                             blk, 0, acc_type, 4, 0, 0);


    /* Set 1st schan ctrl word as opcode */
    soc_pci_write(unit,
            CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE_OFFSET(cmc, ch), msg.dwords[0]);

    rval = soc_cm_l2p(unit, host_buff);
    soc_pci_write(unit,
            CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(cmc, ch),
            rval);

    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    cfg_reg = _soc_kt_fifo_reg_get (unit, cmc, ch, RD_DMA_CFG_REG);

    soc_reg_field_set(unit, cfg_reg, &rval, BEAT_COUNTf,
            data_beats);
    soc_reg_field_set(unit, cfg_reg, &rval,
            HOST_NUM_ENTRIES_SELf, sel);
    soc_reg_field_set(unit, cfg_reg, &rval, ABORTf, 0);

    /* Note: Following might need to be tuned */
    soc_reg_field_set(unit, cfg_reg, &rval,
            TIMEOUT_COUNTf, 1000);
    /* Note: Following could be removed ?? */
    soc_reg_field_set(unit, cfg_reg, &rval,
            NACK_FATALf, 1);

    if (soc_feature(unit, soc_feature_multi_sbus_cmds)) {
        
    }
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    /* Note: Following might need to be tuned */
    soc_pci_write(unit,
            CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),
            host_entries/10);
    soc_reg_field_set(unit, cfg_reg, &rval, ENABLEf, 1);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);
    return SOC_E_NONE;
}

#endif

int
_soc_mem_sbus_fifo_dma_start(int unit, int ch, soc_mem_t mem, int copyno,
                             int host_entries, void *host_buff)
{

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        return _soc_kt2_mem_sbus_fifo_dma_start_memreg(unit, ch,
                TRUE /* is_mem */, mem, 0,
                copyno, host_entries, host_buff);
    }
#endif
    return _soc_mem_sbus_fifo_dma_start_memreg(unit, ch,
                                               TRUE /* is_mem */, mem, 0,
                                               copyno, 0, host_entries, host_buff);

}

#ifdef BCM_KATANA2_SUPPORT
int
_soc_kt2_mem_sbus_fifo_dma_stop(int unit, int ch)
{
    int cmc, iter = 0;
    uint32 rval;
    int to = SAL_BOOT_QUICKTURN ? 30000000 : 10000000;
    soc_reg_t cfg_reg , cfg_stat;

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }
    cfg_reg = _soc_kt_fifo_reg_get (unit, cmc, ch, RD_DMA_CFG_REG);
    cfg_stat= _soc_kt_fifo_reg_get (unit, cmc, ch,RD_DMA_STAT);
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    if (!soc_reg_field_get(unit, cfg_reg, rval, ENABLEf)) {
        return SOC_E_NONE;
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, cfg_reg, &rval, ABORTf, 1);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    sal_udelay(1000);
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));

    while (soc_reg_field_get(unit, cfg_stat, rval, DONEf) == 0 &&
           iter++ <to) {
        sal_udelay(1000);
        rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, cfg_reg, &rval, ENABLEf, 0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    if (iter >= to) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "FIFO DMA abort failed !!\n")));
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}
#endif


int
_soc_mem_sbus_fifo_dma_stop(int unit, int ch)
{
    int cmc, iter = 0;
    uint32 rval;
    int to = SAL_BOOT_QUICKTURN ? 30000000 : 10000000;

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) ) {
        return _soc_kt2_mem_sbus_fifo_dma_stop(unit,ch);
    }
#endif

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }

    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    if (!soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, rval, ENABLEf)) {
        return SOC_E_NONE;
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
    if (!soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                                  DONEf)) {
        rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
        soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ABORTf, 1);
        soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

        sal_udelay(1000);
        rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));

        while (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval, DONEf) == 0 &&
               iter++ <to) {
            sal_udelay(1000);
            rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
        }
    }
    rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), rval);

    if (iter >= to) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "FIFO DMA abort failed !!\n")));
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

int
_soc_mem_sbus_fifo_dma_get_num_entries(int unit, int ch, int *count)
{
    uint32 val = 0;
    int cmc;

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }

    val = soc_pci_read(unit,
                  CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(cmc, ch));
    *count = val;
    if (val) {
        return SOC_E_NONE;
    }
    return SOC_E_EMPTY;
}

int
_soc_mem_sbus_fifo_dma_set_entries_read(int unit, int ch, uint32 num)
{
    int cmc;

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        if (ch < 0 || ch > 12) {
            return SOC_E_PARAM;
        }
        cmc = ch / 4;
        ch = ch % 4;
    } else {
        cmc = SOC_PCI_CMC(unit);
        if (ch < 0 || ch > 3) {
            return SOC_E_PARAM;
        }
    }

    

    soc_pci_write(unit,
        CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM_OFFSET(cmc, ch), num);
    return SOC_E_NONE;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT /* DPPCOMPILEENABLE */

STATIC int
_soc_l2mod_sbus_fifo_enable(int unit, uint8 val)
{
    uint32 rval = 0;

    if (val == 0) {
        SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &rval));

        soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval,
                          INTERNAL_L2_ENTRYf, val);
        if (soc_feature(unit, soc_feature_esm_support)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval,
                              EXTERNAL_L2_ENTRYf, val);
        }
        /* enable by default unless API disable explicitly */
        if (API_DISABLE != API_GET_L2_MOD_FIFO_ENABLE(unit, L2_DELETEf)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_DELETEf, val);
        }
        if (API_DISABLE != API_GET_L2_MOD_FIFO_ENABLE(unit, L2_INSERTf)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_INSERTf, val);
        }
        if (API_DISABLE != API_GET_L2_MOD_FIFO_ENABLE(unit, L2_LEARNf)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_LEARNf, val);
        }
        if (API_DISABLE != API_GET_L2_MOD_FIFO_ENABLE(unit, L2_MEMWRf)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_MEMWRf, val);
        }
        SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_td2_l2mod_sbus_fifo_enable(int unit, int enable)
{
    uint32 rval = 0;

    if (enable == 0) {
        SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROLr(unit, rval));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &rval));
        /* enable by default unless API disable explicitly */
        if (API_DISABLE != API_GET_AUX_ARB_CNTL(unit, L2_MOD_FIFO_ENABLE_L2_DELETEf)) {
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval,
                              L2_MOD_FIFO_ENABLE_L2_DELETEf, enable);
        }
        if (API_DISABLE != API_GET_AUX_ARB_CNTL(unit, L2_MOD_FIFO_ENABLE_AGEf)) {
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_AGEf,
                              enable);
        }
        if (API_DISABLE != API_GET_AUX_ARB_CNTL(unit, L2_MOD_FIFO_ENABLE_LEARNf)) {
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_LEARNf,
                              enable);
        }
        /* disable by default unless API enable explicitly */
        if (API_ENABLE == API_GET_AUX_ARB_CNTL(unit, L2_MOD_FIFO_ENABLE_L2_INSERTf)) {
            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, L2_MOD_FIFO_ENABLE_L2_INSERTf,
                              enable);
        }
        SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROLr(unit, rval));
    }
    return SOC_E_NONE;
}

#ifdef BCM_TOMAHAWK_SUPPORT
STATIC int
_soc_th_l2mod_sbus_fifo_enable(int unit, int enable)
{
    uint32 rval = 0;

    if (enable == 0) {
        SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, enable));
        return SOC_E_NONE;
    } else {
        SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &rval));

        /* enable by default unless API disable explicitly */
        if (API_DISABLE != API_GET_L2_MOD_FIFO_ENABLE(unit, L2_DELETEf)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_DELETEf, enable);
        }
        if (API_DISABLE != API_GET_L2_MOD_FIFO_ENABLE(unit, L2_LEARNf)) {
            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, L2_LEARNf, enable);
        }
        SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));
    }

    return SOC_E_NONE;
}
#endif /* BCM_TOMAHAWK_SUPPORT */

extern uint32 soc_mem_fifo_delay_value;

STATIC void
_soc_l2mod_sbus_fifo_dma_thread(void *unit_vp)
{
    uint8 overflow, timeout;
    int i, count, handled_count, adv_threshold;
    int unit = PTR_TO_INT(unit_vp);
    int cmc = SOC_PCI_CMC(unit);
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 entries_per_buf, rval, rclr;
    uint32 max_error;
    int rv, interval, non_empty;
    uint8 ch;
    uint32 *buff_max, intr_mask;
    void *host_entry, *host_buff;
    int entry_words;
    soc_stat_t *stat = SOC_STAT(unit);

    if (SOC_IS_TD2_TT2(unit)) {
        /*
         * Channel 0 is for X pipe
         * Channel 1 is for Y pipe but nothing will go into Y pipe L2_MOD_FIFO.
         */
        ch = SOC_MEM_FIFO_DMA_CHANNEL_0;
    } else {
        ch = SOC_MEM_FIFO_DMA_CHANNEL_1;
    }
    intr_mask = IRQ_CMCx_FIFO_CH_DMA(ch);

    max_error = soc_property_get(unit, spn_L2FIFO_DMA_MAX_ERROR_COUNT, 10);
    stat->err_l2fifo_dma = 0;

    entries_per_buf = soc_property_get(unit, spn_L2XMSG_HOSTBUF_SIZE, 1024);
    adv_threshold = entries_per_buf / 2;

    entry_words = soc_mem_entry_words(unit, L2_MOD_FIFOm);
    host_buff = soc_cm_salloc(unit, entries_per_buf * entry_words * sizeof(uint32),
                             "L2_MOD DMA Buffer");
    if (host_buff == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_L2MOD_DMA, __LINE__,
                           SOC_E_MEMORY);
        goto cleanup_exit;
    }
    (void)_soc_mem_sbus_fifo_dma_stop(unit, ch);

    rv = _soc_mem_sbus_fifo_dma_start(unit, ch, L2_MOD_FIFOm, MEM_BLOCK_ANY,
                                      entries_per_buf, host_buff);
    if (SOC_FAILURE(rv)) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_L2MOD_DMA, __LINE__, rv);
        goto cleanup_exit;
    }

    host_entry = host_buff;
    buff_max = (uint32 *)host_entry + (entries_per_buf * entry_words);

    if (!soc_feature(unit, soc_feature_fifo_dma)) {
        soc_cmicm_intr0_enable(unit,
                IRQ_CMCx_FIFO_CH_DMA(SOC_MEM_FIFO_DMA_CHANNEL_1));
    }
    if (SOC_IS_TRIUMPH3(unit)) {
        _soc_l2mod_sbus_fifo_enable(unit, 1);
#ifdef BCM_TOMAHAWK_SUPPORT
    } else if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        _soc_th_l2mod_sbus_fifo_enable(unit, 1);
#endif /* BCM_TOMAHAWK_SUPPORT */
    } else {
        _soc_td2_l2mod_sbus_fifo_enable(unit, 1);
    }

    while ((interval = soc->l2x_interval)) {
        overflow = 0; timeout = 0;
        if (soc->l2modDmaIntrEnb) {
            soc_cmicm_intr0_enable(unit, intr_mask);
            if (sal_sem_take(soc->arl_notify, interval) < 0) {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "%s polling timeout soc_mem_fifo_delay_value=%d\n"),
                             soc->l2x_name, soc_mem_fifo_delay_value));
            } else {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "%s woken up soc_mem_fifo_delay_value=%d\n"),
                             soc->l2x_name, soc_mem_fifo_delay_value));
                /* check for timeout or overflow and either process or continue */
                rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
                timeout = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr,
                                            rval, HOSTMEM_TIMEOUTf);
                if (!timeout) {
                    overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr,
                                                 rval, HOSTMEM_OVERFLOWf);
                    timeout |= overflow;
                }
            }

        } else {
            sal_usleep(interval);
        }

        handled_count = 0;
        do {
            non_empty = FALSE;
            /* get entry count, process if nonzero else continue */
            rv = _soc_mem_sbus_fifo_dma_get_num_entries(unit, ch, &count);
            if (SOC_SUCCESS(rv)) {
                non_empty = TRUE;
                if (count > adv_threshold) {
                    count = adv_threshold;
                }
                /* Invalidate DMA memory to read */
                if (((uint32 *)host_entry + count * entry_words) > buff_max) {
                    /* roll-over cases */
                    soc_cm_sinval(unit, host_entry,
                        (buff_max - (uint32 *)host_entry) * sizeof(uint32));
                    soc_cm_sinval(unit, host_buff,
                        ((count * entry_words) - (buff_max - (uint32 *)host_entry)) * sizeof(uint32));
                }
                else {
                    soc_cm_sinval(unit, host_entry, (count * entry_words) * sizeof(uint32));
                }
                for (i = 0; i < count; i++) {
                    if(!soc->l2x_interval) {
                        goto cleanup_exit;
                    }
#ifdef BCM_TRIUMPH3_SUPPORT
                    if (SOC_IS_TRIUMPH3(unit)) {
                        soc_tr3_l2mod_fifo_process(unit, soc->l2x_flags,
                                                   host_entry);
                    } else
#endif
                    {
#ifdef BCM_KATANA2_SUPPORT
                        if (SOC_IS_KATANA2(unit)) {
                            _soc_kt_l2mod_fifo_process(unit, soc->l2x_flags,
                                    host_entry);

                        } else
#endif
#ifdef BCM_HURRICANE2_SUPPORT
                        if (soc_feature(unit, soc_feature_fifo_dma_hu2)) {
                            _soc_hu2_l2mod_fifo_process(unit, soc->l2x_flags,
                                    host_entry);

                        } else
#endif
                        {
#ifdef BCM_TRIDENT2_SUPPORT
                            _soc_td2_l2mod_fifo_process(unit, soc->l2x_flags,
                                    host_entry);
#endif /* BCM_TRIDENT2_SUPPORT*/
                        }
                    }
                    host_entry = (uint32 *)host_entry + entry_words;
                    /* handle roll over */
                    if ((uint32 *)host_entry >= buff_max) {
                        host_entry = host_buff;
                    }

                    handled_count ++;
                    /*
                     * PPA may wait for available space in mod_fifo, lower the
                     * threshold when ppa is running, therefore read point can
                     * be updated sooner.
                     */
                    if (SOC_CONTROL(unit)->l2x_ppa_in_progress && i >= 63) {
                        i++;
                        break;
                    }
                }
                (void)_soc_mem_sbus_fifo_dma_set_entries_read(unit, ch, i);
            }

            /* check and clear error */
            rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch));
            if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                                  DONEf)) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                      "FIFO DMA engine terminated for cmc[%d]:ch[%d]\n"),
                           cmc, ch));
                if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                                  ERRORf)) {
                    stat->err_l2fifo_dma++;
                    if (stat->err_l2fifo_dma > max_error) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                   "FIFO DMA engine encountered error: [0x%x] count = %u\n"),
                                    rval, stat->err_l2fifo_dma));
                        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                                   SOC_SWITCH_EVENT_THREAD_L2MOD_DMA, __LINE__, SOC_E_INTERNAL);
                        goto cleanup_exit;
                    }
                }
                /* Clear Overflow, ECC and timeout error */
                rclr = 0;
                if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                                      HOSTMEM_OVERFLOWf)) {
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &rclr,
                                      HOSTMEM_OVERFLOWf, 1);
                }
                if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                                      HOSTMEM_TIMEOUTf)) {
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &rclr,
                                      HOSTMEM_TIMEOUTf, 1);
                }
                if (soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval,
                                      ECC_2BIT_CHECK_FAILf)) {
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &rclr,
                                      ECC_2BIT_CHECK_FAILf, 1);
                }
                if (rclr > 0) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                          "RD_DMA_STAT_CLR: [0x%x]\n"), rclr));
                    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch),
                                      rclr);
                }
                /* Disable DMA to conclude the operation */
                (void)_soc_mem_sbus_fifo_dma_stop(unit, ch);
                /* Enable DMA */
                rv = _soc_mem_sbus_fifo_dma_start(unit, ch, L2_MOD_FIFOm, MEM_BLOCK_ANY,
                                      entries_per_buf, host_buff);
                if (SOC_FAILURE(rv)) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_L2MOD_DMA, __LINE__, rv);
                    goto cleanup_exit;
                }
            }

            if (!SOC_CONTROL(unit)->l2x_ppa_in_progress) {
                /* For some operating systems, sal_thread_yield is
                 * not sufficient to yield CPU resource to other lower
                 * priority thread
                 */
                if ((handled_count * 2) >= adv_threshold) {
                    sal_usleep(1000);
                    handled_count = 0;
                }
            }
        } while (non_empty);
        /* Clearing of the FIFO_CH_DMA_INT interrupt by resetting
           overflow & timeout status in FIFO_CHy_RD_DMA_STAT_CLR reg */
        if (timeout) {
            rval = 0;
            soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &rval,
                              HOSTMEM_OVERFLOWf, 1);
            soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &rval,
                              HOSTMEM_TIMEOUTf, 1);
            soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch),
                          rval);
        }
    }

cleanup_exit:
    (void)_soc_mem_sbus_fifo_dma_stop(unit, ch);

    if (host_buff != NULL) {
        soc_cm_sfree(unit, host_buff);
    }
    if (SOC_IS_TRIUMPH3(unit)) {
        _soc_l2mod_sbus_fifo_enable(unit, 0);
#ifdef BCM_TOMAHAWK_SUPPORT
    } else if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        _soc_th_l2mod_sbus_fifo_enable(unit, 0);
#endif /* BCM_TOMAHAWK_SUPPORT */
    } else {
        _soc_td2_l2mod_sbus_fifo_enable(unit, 0);
    }

    soc->l2x_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 *     soc_l2mod_running
 * Purpose:
 *     Determine the L2MOD sync thread running parameters
 * Parameters:
 *     unit - unit number.
 *     flags (OUT) - if non-NULL, receives the current flag settings
 *     interval (OUT) - if non-NULL, receives the current pass interval
 * Returns:
 *     Boolean; TRUE if L2MOD sync thread is running
 */

int
_soc_l2mod_running(int unit, uint32 *flags, sal_usecs_t *interval)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    soc_control_t *soc = SOC_CONTROL(unit);

    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (soc->l2x_pid != SAL_THREAD_ERROR) {
            if (flags != NULL) {
                *flags = soc->l2x_flags;
            }
            if (interval != NULL) {
                *interval = soc->l2x_interval;
            }
        }

        return (soc->l2x_pid != SAL_THREAD_ERROR);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *     _soc_l2mod_stop
 * Purpose:
 *     Stop L2MOD-related thread
 * Parameters:
 *     unit - unit number.
 * Returns:
 *     SOC_E_XXX
 */
int
_soc_l2mod_stop(int unit)
{
    soc_control_t * soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    uint8 ch = SOC_MEM_FIFO_DMA_CHANNEL_1;

    if (SOC_IS_XGS3_SWITCH(unit)) {
        LOG_INFO(BSL_LS_SOC_ARL,
                 (BSL_META_U(unit,
                             "soc_l2mod_stop: unit=%d\n"), unit));

        if (SOC_IS_TRIUMPH3(unit)) {
            _soc_l2mod_sbus_fifo_enable(unit, 0);
#ifdef BCM_TOMAHAWK_SUPPORT
        } else if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            _soc_th_l2mod_sbus_fifo_enable(unit, 0);
#endif /* BCM_TOMAHAWK_SUPPORT */
        } else {
            _soc_td2_l2mod_sbus_fifo_enable(unit, 0);
        }
        if (!soc_feature(unit, soc_feature_fifo_dma)) {
            soc_cmicm_intr0_disable(unit, IRQ_CMCx_FIFO_CH_DMA(ch));
            soc->l2x_interval = 0;  /* Request exit */
        }

        if (soc->l2modDmaIntrEnb) {
            /* Wake up thread so it will check the exit flag */
            sal_sem_give(soc->arl_notify);
        }

        return rv;
    }
    return SOC_E_UNAVAIL;
}

int
_soc_l2mod_start(int unit, uint32 flags, sal_usecs_t interval)
{
    soc_control_t * soc = SOC_CONTROL(unit);
    int             pri;

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
        return SOC_E_UNAVAIL;
    }

    if (soc->l2x_interval != 0) {
        SOC_IF_ERROR_RETURN(_soc_l2mod_stop(unit));
    }

    sal_snprintf(soc->l2x_name, sizeof(soc->l2x_name), "bcmL2MOD.%d", unit);

    soc->l2x_flags = flags;
    soc->l2x_interval = interval;

    if (interval == 0) {
        return SOC_E_NONE;
    }

    if (soc->l2x_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, spn_L2XMSG_THREAD_PRI, 50);

        soc->l2x_pid =
                sal_thread_create(soc->l2x_name, SAL_THREAD_STKSZ, pri,
                                  _soc_l2mod_sbus_fifo_dma_thread, INT_TO_PTR(unit));
        if (soc->l2x_pid == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_l2mod_start: Could not start L2MOD thread\n")));
            return SOC_E_MEMORY;
        }
    }

    return SOC_E_NONE;
}

int
_soc_l2mod_sbus_fifo_field_set(int unit, soc_field_t field, int enable)
{
    uint32 rval = 0;
    uint32 fval = enable?1:0;

    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TITAN2(unit) || SOC_IS_TITAN(unit)|| 
        SOC_IS_HURRICANE2(unit)|| SOC_IS_HURRICANE3(unit)||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit) ||
        SOC_IS_KATANA2(unit)) {
        if(soc_reg_field_valid(unit, AUX_ARB_CONTROLr, field)) {
            SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &rval));

            soc_reg_field_set(unit, AUX_ARB_CONTROLr, &rval, field, fval);

            SOC_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROLr(unit, rval));

            API_SET_AUX_ARB_CNTL(unit, field, fval);
        }
        return SOC_E_NONE;
    }
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TOMAHAWKX(unit)) {
        if(soc_reg_field_valid(unit, L2_MOD_FIFO_ENABLEr, field)) {
            SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &rval));

            soc_reg_field_set(unit, L2_MOD_FIFO_ENABLEr, &rval, field, fval);

            SOC_IF_ERROR_RETURN(WRITE_L2_MOD_FIFO_ENABLEr(unit, rval));

            API_SET_L2_MOD_FIFO_ENABLE(unit, field, fval);
        }
        return SOC_E_NONE;
    }
    return SOC_E_UNAVAIL;
}

int
_soc_l2mod_sbus_fifo_field_get(int unit, soc_field_t field, int *enable)
{
    uint32 reg = 0;
    uint32 fval = 0;

    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TRIDENT(unit) ||
        SOC_IS_TITAN2(unit) || SOC_IS_TITAN(unit) ||
        SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit) ||
        SOC_IS_KATANA2(unit)) {
        if(soc_reg_field_valid(unit, AUX_ARB_CONTROLr, field)) {
            SOC_IF_ERROR_RETURN(READ_AUX_ARB_CONTROLr(unit, &reg));

            fval = soc_reg_field_get(unit, AUX_ARB_CONTROLr, reg, field);
        }
        *enable = fval?1:0;
        return SOC_E_NONE;
    }
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TOMAHAWKX(unit)) {
        if(soc_reg_field_valid(unit, L2_MOD_FIFO_ENABLEr, field)) {
            SOC_IF_ERROR_RETURN(READ_L2_MOD_FIFO_ENABLEr(unit, &reg));

            fval = soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, reg, field);

        }
        *enable = fval?1:0;
        return SOC_E_NONE;
    }

    return SOC_E_UNAVAIL;
}

#endif /* BCM_XGS3_SWITCH_SUPPORT */ /* DPPCOMPILEENABLE */
#endif /* BCM_CMICM_SUPPORT */
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */


