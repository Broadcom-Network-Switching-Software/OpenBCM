
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  All Rights Reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif 
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR


#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/fifodma.h>
#include <soc/cmicx.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fifodma.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fifodma_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/dnxc/dnxc_ha.h>




static shr_error_e
soc_dnx_fifodma_chan_verify(
    int unit,
    int chan)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((chan < 0) || (chan >= DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FIFODMA channel id %d is not valid.\n", chan);
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_config_t_init(
    int unit,
    soc_dnx_fifodma_config_t * fifodma_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fifodma_info, _SHR_E_PARAM, "pointer to FIFODMA config info is not valid");
    sal_memset(fifodma_info, 0, sizeof(soc_dnx_fifodma_config_t));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_alloc(
    int unit,
    soc_dnx_fifodma_src_t source,
    int *chan)
{
    uint64 reg64_val;
    const soc_field_t field_name[DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS] =
        { FIFO_DMA_0_SELf, FIFO_DMA_1_SELf, FIFO_DMA_2_SELf, FIFO_DMA_3_SELf,
        FIFO_DMA_4_SELf, FIFO_DMA_5_SELf, FIFO_DMA_6_SELf, FIFO_DMA_7_SELf,
        FIFO_DMA_8_SELf, FIFO_DMA_9_SELf, FIFO_DMA_10_SELf, FIFO_DMA_11_SELf
    };

    const dnx_data_fifodma_general_fifodma_map_t *fifodma_map;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(chan, _SHR_E_PARAM, "pointer to chan is not valid");

    
    fifodma_map = dnx_data_fifodma.general.fifodma_map_get(unit, source);
    *chan = fifodma_map->fifodma_channel;

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_IF_ERR_EXIT(READ_ECI_FIFO_DMA_SELr(unit, &reg64_val));
    soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, field_name[*chan], source);
    SHR_IF_ERR_EXIT(WRITE_ECI_FIFO_DMA_SELr(unit, reg64_val));

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_alloced.set(unit, *chan, 1));

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_get(
    int unit,
    soc_dnx_fifodma_src_t source,
    int *chan)
{
    int is_alloced;
    const dnx_data_fifodma_general_fifodma_map_t *fifodma_map;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(chan, _SHR_E_PARAM, "pointer to chan is not valid");

    
    fifodma_map = dnx_data_fifodma.general.fifodma_map_get(unit, source);
    *chan = fifodma_map->fifodma_channel;

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_alloced.get(unit, *chan, &is_alloced));
    if (!is_alloced)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d has NOT been allocated! please alloc() at first.\n", *chan);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_is_started(
    int unit,
    soc_dnx_fifodma_src_t source,
    int *is_started)
{
    const dnx_data_fifodma_general_fifodma_map_t *fifodma_map;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_started, _SHR_E_PARAM, "pointer to is_started is not valid");

    
    fifodma_map = dnx_data_fifodma.general.fifodma_map_get(unit, source);

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.get(unit, fifodma_map->fifodma_channel, is_started));

exit:
    SHR_FUNC_EXIT;
}


static int
soc_fifo_dma_start_internal(
    int unit,
    int ch,
    int is_mem,
    soc_mem_t mem,
    soc_reg_t reg,
    int array_index,
    int copyno,
    int force_entry_size,
    int host_entries,
    void *host_buff)
{
    uint8 at;
    uint32 rval, data_beats, sel;
    int blk;
    schan_msg_t msg;
    int acc_type;
    sal_paddr_t addr;

    LOG_VERBOSE(BSL_LS_SOC_FIFODMA,
                (BSL_META_U(unit, "cmicx fifodma start for chan %d, host entries %d\n"), ch, host_entries));

    if (ch < 0 || ch > DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS || host_buff == NULL)
    {
        return SOC_E_PARAM;
    }

    switch (host_entries)
    {
        case 64:
            sel = 0;
            break;
        case 128:
            sel = 1;
            break;
        case 256:
            sel = 2;
            break;
        case 512:
            sel = 3;
            break;
        case 1024:
            sel = 4;
            break;
        case 2048:
            sel = 5;
            break;
        case 4096:
            sel = 6;
            break;
        case 8192:
            sel = 7;
            break;
        case 16384:
            sel = 8;
            break;
        default:
            return SOC_E_PARAM;
    }

    
    if ((!is_mem) && SOC_REG_IS_VALID(unit, reg))
    {
        data_beats = BYTES2WORDS(soc_reg_bytes(unit, reg));
        rval = soc_reg_addr_get(unit, reg, copyno, array_index, SOC_REG_ADDR_OPTION_NONE, &blk, &at);
    }
    else
    {
        data_beats = soc_mem_entry_words(unit, mem);
        if (copyno == MEM_BLOCK_ANY)
        {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
        rval = soc_mem_addr_get(unit, mem, array_index, copyno, 0, &at);
        blk = SOC_BLOCK2SCH(unit, copyno);
    }

    
    if (force_entry_size > 0)
    {
        data_beats = BYTES2WORDS(force_entry_size);
    }

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(ch), rval);

    schan_msg_clear(&msg);
    if (is_mem)
    {
        acc_type = SOC_MEM_ACC_TYPE(unit, mem);
    }
    else
    {
        acc_type = 0;
    }

    soc_schan_header_cmd_set(unit, &msg.header, FIFO_POP_CMD_MSG, blk, 0, acc_type, 4, 0, 0);
    
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_OPCODE_OFFSET(ch), msg.dwords[0]);

    addr = soc_cm_l2p(unit, host_buff);

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_LO_OFFSET(ch), PTR_TO_INT(addr));

    soc_pci_write(unit,
                  CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_HI_OFFSET(ch),
                  (CMIC_PCIE_SO_OFFSET | PTR_HI_TO_INT(addr)));

    rval = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch));

    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, BEAT_COUNTf, data_beats);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, HOST_NUM_ENTRIES_SELf, sel);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, ABORTf, 0);
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, NACK_FATALf, 1);

    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch), rval);

    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &rval, ENABLEf, 1);
    soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(ch), rval);

    return SOC_E_NONE;
}


shr_error_e
soc_dnx_fifodma_channel_start(
    int unit,
    int chan,
    const soc_dnx_fifodma_config_t * fifodma_info,
    char *callback_name)
{
    uint32 reg;
    uint32 timeout;
    uint32 host_mem_size_in_bytes;
    int dont, care, endian;
    int is_started, is_paused;
    uint32 entry_size;
    void *host_buff;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(soc_dnx_fifodma_chan_verify(unit, chan));

    SHR_NULL_CHECK(fifodma_info, _SHR_E_PARAM, "pointer to fifodma_info is not valid");

    if (utilex_is_power_of_2(fifodma_info->max_entries) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_entries error, number of host entries %d is not power-of-2 number.\n",
                     fifodma_info->max_entries);
        
    }

    
    if (fifodma_info->max_entries < dnx_data_fifodma.general.min_nof_host_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_entries error, minimum number of host entries is %d.\n",
                     dnx_data_fifodma.general.min_nof_host_entries_get(unit));
    }

    
    if (fifodma_info->max_entries > dnx_data_fifodma.general.max_nof_host_entries_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "max_entries error, maximal number of host entries is %d.\n",
                     dnx_data_fifodma.general.max_nof_host_entries_get(unit));
    }

    
    if (fifodma_info->threshold > fifodma_info->max_entries)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "threshold %d should not be more than max number of host entries()%d.\n",
                     fifodma_info->threshold, fifodma_info->max_entries);
    }

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started));

    
    if (is_started && !SOC_WARM_BOOT(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d has been started! please stop current channel at first.\n",
                     chan);
    }
    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_paused.get(unit, chan, &is_paused));
    if (is_paused && !SOC_WARM_BOOT(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d has been paused! please stop current channel at first.\n",
                     chan);
    }

    
    if (fifodma_info->is_poll == 0)
    {
        SHR_NULL_CHECK(callback_name, _SHR_E_PARAM, "Callback handler should be set in interrupt mode!");
        
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.handler.register_cb(unit, chan, callback_name));
    }
    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_poll.set(unit, chan, fifodma_info->is_poll));

    
    if ((!fifodma_info->is_mem) && SOC_REG_IS_VALID(unit, fifodma_info->reg))
    {
        entry_size = 4 * BYTES2WORDS(soc_reg_bytes(unit, fifodma_info->reg));
    }
    else
    {
        entry_size = 4 * soc_mem_entry_words(unit, fifodma_info->mem);
    }
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.entry_size.set(unit, chan, entry_size));
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.max_entries.set(unit, chan, fifodma_info->max_entries));

    
    host_mem_size_in_bytes = entry_size * fifodma_info->max_entries;
    host_buff = soc_cm_salloc(unit, host_mem_size_in_bytes, "FIFO DMA");
    SHR_NULL_CHECK(host_buff, _SHR_E_MEMORY, "Host memory buffer allocation failed!");
    sal_memset(host_buff, 0x0, host_mem_size_in_bytes);
    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_lo.set(unit, chan, PTR_TO_INT(host_buff)));
#ifdef PTRS_ARE_64BITS
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_hi.set(unit, chan, PTR_HI_TO_INT(host_buff)));
#else
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_hi.set(unit, chan, 0));
#endif
    
    SHR_IF_ERR_EXIT(soc_fifodma_stop(unit, chan));

    
    reg = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(chan));
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_HOSTMEM_THRESHOLDr, &reg, THRESHOLDf,
                      fifodma_info->threshold);
    SHR_IF_ERR_EXIT(soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(chan), reg));

    
    timeout = fifodma_info->timeout;
    
    CMIC_COMMON_POOL_FIFO_USEC_TO_DMA_CFG_TIMEOUT_COUNT(timeout);
    
    reg = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(chan));
    timeout &= 0x3fff;
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, timeout);
    SHR_IF_ERR_EXIT(soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_OFFSET(chan), reg));

    timeout = fifodma_info->timeout;
    
    CMIC_COMMON_POOL_FIFO_USEC_TO_DMA_CFG_TIMEOUT_COUNT(timeout);
    
    reg = soc_pci_read(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_1_OFFSET(chan));
    timeout &= 0xFFFFC000;
    timeout >>= 14;
    soc_reg_field_set(unit, CMIC_COMMON_POOL_FIFO_CH0_RD_DMA_CFG_1r, &reg, TIMEOUT_COUNT_MSB_BITSf, timeout);
    SHR_IF_ERR_EXIT(soc_pci_write(unit, CMIC_COMMON_POOL_FIFO_CHy_RD_DMA_CFG_1_OFFSET(chan), reg));

    
    soc_endian_get(unit, &dont, &care, &endian);
    SHR_IF_ERR_EXIT(soc_fifodma_ch_endian_set(unit, chan, (endian != 0)));

    if (fifodma_info->is_poll)
    {
        
        SHR_IF_ERR_EXIT(soc_fifodma_intr_disable(unit, chan));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(soc_fifodma_intr_enable(unit, chan));
    }
    
    SHR_IF_ERR_EXIT(soc_fifo_dma_start_internal
                    (unit, chan, fifodma_info->is_mem, fifodma_info->mem,
                     fifodma_info->reg, fifodma_info->array_index, fifodma_info->copyno, 0,
                     fifodma_info->max_entries, host_buff));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.set(unit, chan, 1));
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_paused.set(unit, chan, 0));
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.last_entry_id.set(unit, chan, 0));
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.reg.set(unit, chan, fifodma_info->reg));
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.mem.set(unit, chan, fifodma_info->mem));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_stop(
    int unit,
    int chan)
{
    int is_started, is_poll;
    uint32 host_buff_lo;
    uint32 host_buff_hi;
    sal_vaddr_t host_buff_tmp;
    void *host_buff;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(soc_dnx_fifodma_chan_verify(unit, chan));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started));
    if (is_started)
    {
        
        SHR_IF_ERR_EXIT(soc_fifodma_intr_disable(unit, chan));
        
        SHR_IF_ERR_EXIT(soc_fifodma_stop(unit, chan));

        
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_lo.get(unit, chan, &host_buff_lo));
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_hi.get(unit, chan, &host_buff_hi));
#ifdef PTRS_ARE_64BITS
        host_buff_tmp = ((sal_vaddr_t) host_buff_hi << 32) | host_buff_lo;
#else
        host_buff_tmp = host_buff_lo;
#endif

        host_buff = (void *) host_buff_tmp;
        if (host_buff != NULL)
        {
            soc_cm_sfree(unit, host_buff);
            host_buff = NULL;
            SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_lo.set(unit, chan, PTR_TO_INT(host_buff)));
#ifdef PTRS_ARE_64BITS
            SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_hi.set(unit, chan, PTR_HI_TO_INT(host_buff)));
#else
            SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.host_buff_hi.set(unit, chan, 0));
#endif
        }
        
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_poll.get(unit, chan, &is_poll));
        if (is_poll == 0)
        {
            SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.handler.unregister_cb(unit, chan));
        }
        
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.set(unit, chan, 0));
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_paused.set(unit, chan, 0));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_free(
    int unit,
    int chan)
{
    uint64 reg64_val;
    CONST soc_field_t field_name[DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS] =
        { FIFO_DMA_0_SELf, FIFO_DMA_1_SELf, FIFO_DMA_2_SELf, FIFO_DMA_3_SELf,
        FIFO_DMA_4_SELf, FIFO_DMA_5_SELf, FIFO_DMA_6_SELf, FIFO_DMA_7_SELf,
        FIFO_DMA_8_SELf, FIFO_DMA_9_SELf, FIFO_DMA_10_SELf, FIFO_DMA_11_SELf
    };
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(soc_dnx_fifodma_chan_verify(unit, chan));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started));
    if (is_started)
    {
        
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_stop(unit, chan));
    }

    
    SHR_IF_ERR_EXIT(READ_ECI_FIFO_DMA_SELr(unit, &reg64_val));
    soc_reg64_field32_set(unit, ECI_FIFO_DMA_SELr, &reg64_val, field_name[chan], soc_dnx_fifodma_src_reserved);
    SHR_IF_ERR_EXIT(WRITE_ECI_FIFO_DMA_SELr(unit, reg64_val));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_alloced.set(unit, chan, 0));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_info_get(
    int unit,
    int chan,
    soc_dnx_fifodma_config_t * fifodma_info)
{
    uint32 host_buff_lo;
    uint32 host_buff_hi;
    sal_vaddr_t host_buff_tmp;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(soc_dnx_fifodma_chan_verify(unit, chan));

    SHR_NULL_CHECK(fifodma_info, _SHR_E_PARAM, "fifodma_info");

    
    dnx_fifodma_info.entry.max_entries.get(unit, chan, &fifodma_info->max_entries);
    dnx_fifodma_info.entry.is_mem.get(unit, chan, &fifodma_info->is_mem);
    dnx_fifodma_info.entry.mem.get(unit, chan, &fifodma_info->mem);
    dnx_fifodma_info.entry.reg.get(unit, chan, &fifodma_info->reg);
    dnx_fifodma_info.entry.array_index.get(unit, chan, &fifodma_info->array_index);
    dnx_fifodma_info.entry.copyno.get(unit, chan, &fifodma_info->copyno);
    dnx_fifodma_info.entry.threshold.get(unit, chan, &fifodma_info->threshold);
    dnx_fifodma_info.entry.timeout.get(unit, chan, &fifodma_info->timeout);
    dnx_fifodma_info.entry.is_poll.get(unit, chan, &fifodma_info->is_poll);
    dnx_fifodma_info.entry.entry_size.get(unit, chan, &fifodma_info->entry_size);
    dnx_fifodma_info.entry.is_alloced.get(unit, chan, &fifodma_info->is_alloced);
    dnx_fifodma_info.entry.is_started.get(unit, chan, &fifodma_info->is_started);
    dnx_fifodma_info.entry.is_paused.get(unit, chan, &fifodma_info->is_paused);
    dnx_fifodma_info.entry.last_entry_id.get(unit, chan, &fifodma_info->last_entry_id);

    
    dnx_fifodma_info.entry.host_buff_lo.get(unit, chan, &host_buff_lo);
    dnx_fifodma_info.entry.host_buff_hi.get(unit, chan, &host_buff_hi);
#ifdef PTRS_ARE_64BITS
    host_buff_tmp = ((sal_vaddr_t) host_buff_hi << 32) | host_buff_lo;
#else
    host_buff_tmp = host_buff_lo;
#endif
    fifodma_info->host_buff = (uint8 *) host_buff_tmp;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_num_entries_get(
    int unit,
    int chan,
    int *count,
    uint8 **entries,
    uint32 *entry_size)
{
    uint32 max_entries;
    int is_started, is_poll, is_paused;
    int last_entry_id;
    uint32 host_buff_lo;
    uint32 host_buff_hi;
    sal_vaddr_t host_buff_tmp;
    uint8 *host_buff;
    int valid_count;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(count, _SHR_E_PARAM, "pointer to count is not valid");
    SHR_NULL_CHECK(entries, _SHR_E_PARAM, "pointer to entries is not valid");
    SHR_NULL_CHECK(entry_size, _SHR_E_PARAM, "pointer to entry_size is not valid");

    
    dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started);
    if (is_started == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d is not started! please start current channel at first.\n",
                     chan);
    }
    
    dnx_fifodma_info.entry.is_paused.get(unit, chan, &is_paused);
    if (is_paused)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d is paused! please resume current channel at first.\n", chan);
    }

    
    dnx_fifodma_info.entry.is_poll.get(unit, chan, &is_poll);
    if (is_poll == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d is supposed to be working in poll mode.\n", chan);
    }

    dnx_fifodma_info.entry.entry_size.get(unit, chan, entry_size);
    dnx_fifodma_info.entry.max_entries.get(unit, chan, &max_entries);

    
    soc_fifodma_num_entries_get(unit, chan, &valid_count);

    
    if (valid_count > max_entries)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Available entries on FIFODMA channel %d are out of range!\n", chan);
    }

    if (valid_count > 0)
    {
        dnx_fifodma_info.entry.last_entry_id.get(unit, chan, &last_entry_id);

        
        if (last_entry_id + valid_count >= max_entries)
        {
            
            valid_count = max_entries - last_entry_id;
        }
        *count = valid_count;

        
        dnx_fifodma_info.entry.host_buff_lo.get(unit, chan, &host_buff_lo);
        dnx_fifodma_info.entry.host_buff_hi.get(unit, chan, &host_buff_hi);
#ifdef PTRS_ARE_64BITS
        host_buff_tmp = ((sal_vaddr_t) host_buff_hi << 32) | host_buff_lo;
#else
        host_buff_tmp = host_buff_lo;
#endif
        host_buff = (uint8 *) host_buff_tmp;
        *entries = host_buff + (last_entry_id * (*entry_size));
    }
    else
    {
        *count = 0;
        *entries = NULL;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_set_entries_read(
    int unit,
    int chan,
    int count)
{
    int is_started, is_poll, is_paused;
    int last_entry_id = 0;
    uint32 max_entries;

    SHR_FUNC_INIT_VARS(unit);

    
    dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started);
    if (is_started == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d is not started! please start current channel at first.\n",
                     chan);
    }
    
    dnx_fifodma_info.entry.is_paused.get(unit, chan, &is_paused);
    if (is_paused)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d is paused! please resume current channel at first.\n", chan);
    }

    
    dnx_fifodma_info.entry.is_poll.get(unit, chan, &is_poll);
    if (is_poll == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d is supposed to be working in poll mode.\n", chan);
    }

    if (count > 0)
    {
        dnx_fifodma_info.entry.max_entries.get(unit, chan, &max_entries);
        
        dnx_fifodma_info.entry.last_entry_id.get(unit, chan, &last_entry_id);
        last_entry_id += count;
        
        if (last_entry_id == max_entries)
        {
            last_entry_id = 0;
        }
        
        dnx_fifodma_info.entry.last_entry_id.set(unit, chan, last_entry_id);

        
        soc_fifodma_set_entries_read(unit, chan, (uint32) count);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_resume(
    int unit,
    int chan)
{
#ifndef PLISIM
    int is_started;
    soc_dnx_fifodma_config_t fifodma_info;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifndef PLISIM
    
    SHR_INVOKE_VERIFY_DNX(soc_dnx_fifodma_chan_verify(unit, chan));

    
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &fifodma_info));
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_info_get(unit, chan, &fifodma_info));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started));
    if (!is_started)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d cannot be resumed because it is not started!\n", chan);
    }

    
    SHR_IF_ERR_EXIT(soc_fifodma_stop(unit, chan));

    
    SHR_IF_ERR_EXIT(soc_fifo_dma_start_internal
                    (unit, chan, fifodma_info.is_mem, fifodma_info.mem,
                     fifodma_info.reg, fifodma_info.array_index, fifodma_info.copyno, 0,
                     fifodma_info.max_entries, fifodma_info.host_buff));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_paused.set(unit, chan, 0));
    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.last_entry_id.set(unit, chan, 0));

exit:
#endif
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_channel_pause(
    int unit,
    int chan)
{
#ifndef PLISIM
    int is_started;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifndef PLISIM
    
    SHR_INVOKE_VERIFY_DNX(soc_dnx_fifodma_chan_verify(unit, chan));

    
    SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started));
    if (is_started)
    {
        
        SHR_IF_ERR_EXIT(soc_fifodma_stop(unit, chan));
        
        SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_paused.set(unit, chan, 1));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FIFODMA channel %d cannot be paused because it's not started.\n", chan);
    }

exit:
#endif
    SHR_FUNC_EXIT;
}


void
dnx_fifodma_intr_handler(
    void *unit_ptr,
    void *chan_ptr,
    void *unused_p3,
    void *unused_p4,
    void *unused_p5)
{
    int unit;
    int chan;
    uint32 entry_size;
    uint32 max_entries;
    int is_started, is_poll, is_paused;
    int last_entry_id = 0;
    uint32 host_buff_lo;
    uint32 host_buff_hi;
    sal_vaddr_t host_buff_tmp;
    uint8 *host_buff;
    soc_dnx_fifodma_interrupt_handler callback;
    
    int valid_count;
    
    uint32 handled_count;
    void *user_data = NULL;

    unit = PTR_TO_INT(unit_ptr);
    chan = PTR_TO_INT(chan_ptr);

    dnx_fifodma_info.entry.is_started.get(unit, chan, &is_started);
    dnx_fifodma_info.entry.is_paused.get(unit, chan, &is_paused);
    dnx_fifodma_info.entry.is_poll.get(unit, chan, &is_poll);
    
    if (is_poll || (is_started == 0) || is_paused)
    {
        return;
    }

    
    soc_fifodma_num_entries_get(unit, chan, &valid_count);
    if (valid_count > 0)
    {
        int i;
        uint8 *next_entry_in_host_memory;

        
        dnx_fifodma_info.entry.max_entries.get(unit, chan, &max_entries);
        dnx_fifodma_info.entry.entry_size.get(unit, chan, &entry_size);
        
        dnx_fifodma_info.entry.host_buff_lo.get(unit, chan, &host_buff_lo);
        dnx_fifodma_info.entry.host_buff_hi.get(unit, chan, &host_buff_hi);
#ifdef PTRS_ARE_64BITS
        host_buff_tmp = ((sal_vaddr_t) host_buff_hi << 32) | host_buff_lo;
#else
        host_buff_tmp = host_buff_lo;
#endif
        host_buff = (uint8 *) host_buff_tmp;
        dnx_fifodma_info.entry.last_entry_id.get(unit, chan, &last_entry_id);
        dnx_fifodma_info.entry.handler.get_cb(unit, chan, &callback);

        handled_count = 0;
        for (i = 0; i < valid_count; i++)
        {
            
            if (last_entry_id + i >= max_entries)
            {
                last_entry_id -= max_entries;
            }
            next_entry_in_host_memory = host_buff + (last_entry_id + i) * entry_size;
            if (callback)
            {
                
                callback(unit, next_entry_in_host_memory, entry_size, i + 1, valid_count, &user_data);
            }
            handled_count++;
        }
        last_entry_id += i;
        
        if (last_entry_id == max_entries)
        {
            last_entry_id = 0;
        }
        dnx_fifodma_info.entry.last_entry_id.set(unit, chan, last_entry_id);

        
        soc_fifodma_set_entries_read(unit, chan, handled_count);
    }

    
    soc_fifodma_intr_enable(unit, chan);

    return;
}


void
dnx_fifodma_interrupt_handler_example(
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data)
{
    return;
}


shr_error_e
soc_dnx_fifodma_init(
    int unit)
{
    uint8 is_init;
    SHR_FUNC_INIT_VARS(unit);

    
    soc_fifodma_init(unit);

    
    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dnx_fifodma_info.is_init(unit, &is_init));
        if (!is_init)
        {
            SHR_IF_ERR_EXIT(dnx_fifodma_info.init(unit));
            SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.alloc(unit, DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS + 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_fifodma_deinit(
    int unit)
{
    int chan;
    int is_alloced;
    uint8 is_init = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_CONT(dnx_fifodma_info.is_init(unit, &is_init));

    
    if (is_init == TRUE)
    {
        for (chan = 0; chan < DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS; chan++)
        {
            
            SHR_IF_ERR_EXIT(dnx_fifodma_info.entry.is_alloced.get(unit, chan, &is_alloced));
            if (is_alloced)
            {
                SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, chan));
            }
        }
    }

    
    soc_fifodma_deinit(unit);

    

exit:
    SHR_FUNC_EXIT;
}
