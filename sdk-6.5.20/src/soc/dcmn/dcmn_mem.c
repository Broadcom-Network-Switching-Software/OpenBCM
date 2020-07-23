
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN MEM
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MEM
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/drv.h>
#include <soc/memory.h>
#include <soc/mem.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#endif 
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#ifdef BCM_88950_A0
#include <soc/dfe/fe3200/fe3200_fabric_cell.h>
#endif
#endif 

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif 

#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/dnxc/dnxc_mem.h>
#endif 


STATIC int
dcmn_mem_indirect_access_size(int unit, int blktype){
    switch (blktype) {
    
    case SOC_BLK_IRE:
        return 520;
    case SOC_BLK_IHP:
    case SOC_BLK_IPSEC:
    case SOC_BLK_MMU:
    case SOC_BLK_IHB:
    case SOC_BLK_OCB:
    case SOC_BLK_EPNI:
    case SOC_BLK_SPB:
    
    case SOC_BLK_DCL:
    
    case SOC_BLK_NIF:
    
    case SOC_BLK_FDT:
        return 640;
    }
    return 0;
}


STATIC int
dcmn_reg_access_with_block (int unit, int is_write, soc_reg_t reg, int blk, uint32* data){
    int block, reg_size;
    uint32 addr;
    uint8 at;
    uint32 options = SOC_REG_ADDR_OPTION_NONE;

    if (is_write) {
        options |= SOC_REG_ADDR_OPTION_WRITE;
    }

    addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0, options, &block, &at);
    block = SOC_BLOCK_INFO(unit,blk).cmic; 
    
    if ((reg == ECI_INDIRECT_COMMAND_RD_DATAr) || (reg == ECI_INDIRECT_COMMAND_WR_DATAr)) {
        reg_size = dcmn_mem_indirect_access_size(unit, SOC_BLOCK_INFO(unit,blk).type) / 32;
    } else if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
    } else if (SOC_REG_IS_64(unit, reg)) {
        reg_size = 2;
    } else {
        reg_size = 1;
    }
               
    if (is_write){
        return soc_direct_reg_set(unit, block, addr, reg_size, data);
    } else {
        return soc_direct_reg_get(unit, block, addr, reg_size, data);
    }
}


int
dcmn_mem_array_wide_access(int unit, uint32 flags, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data,
                           unsigned operation) 
{
    uint8
        acc_type;
    int
        rv = -1,
        words_left,
        blk;
    uint32  
        data32[1],
        address,
        indirect_size,
        dynamic_access_orig = 0,
        *entry_words = (uint32*)entry_data;
    soc_reg_above_64_val_t
        rd_data;
    uint8 orig_read_mode = SOC_MEM_FORCE_READ_THROUGH(unit);
    uint32 cache_consistency_check = 0;
    soc_timeout_t to;

    assert(operation == 0 || operation == 1); 

#ifdef CRASH_RECOVERY_SUPPORT
        if (BCM_UNIT_DO_HW_READ_WRITE(unit)){
            soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_wide_mem);
        }
#endif 

    if (index < 0) {
        index = -index; 
    }
    
    words_left = soc_mem_entry_words(unit, mem);
    
    MEM_LOCK(unit, mem);
    indirect_size = dcmn_mem_indirect_access_size(unit, SOC_BLOCK_INFO(unit, SOC_MEM_BLOCK_ANY(unit,mem)).type);
    if (indirect_size == 0) {
        cli_out("unit %d: invalid block for indirect access. blk=%d\n", unit, SOC_MEM_BLOCK_ANY(unit,mem));
        goto done;
    }
#ifdef BCM_PETRA_SUPPORT
    
    if (copyno == soc_mem_broadcast_block_get(unit,mem)){
        copyno = COPYNO_ALL;
    }
#endif

    
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != SOC_CORE_ALL && copyno != blk) {
            continue;
        }

        if ((flags & SOC_MEM_DONT_USE_CACHE) != SOC_MEM_DONT_USE_CACHE) {
            if (operation == 0) {
                _soc_mem_write_cache_update(unit, mem, blk, 0, index, array_index, entry_data, NULL, NULL, NULL);
            } else {
                SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0);
                if (TRUE == _soc_mem_read_cache_attempt(unit, flags, mem, blk, index, array_index, entry_data, NULL, &cache_consistency_check)) {
                    rv = 0;
                    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_mode);
                    goto done;
                }
            }
        }

        if (soc_mem_is_signal(unit, mem)) {
            
            if (dcmn_reg_access_with_block(unit, 0, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d\n", unit, blk);
                    goto done;
            }
            dynamic_access_orig = soc_reg_field_get(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, *data32, ENABLE_DYNAMIC_MEMORY_ACCESSf);

            if (dynamic_access_orig == 0) {
                
                *data32 = 0;
                soc_reg_field_set(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, data32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
                if (dcmn_reg_access_with_block(unit, 1, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, data32) != SOC_E_NONE){
                    cli_out("unit %d: Failed writing to reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d (data: %d)\n", unit, blk, *data32);
                    goto done;
                }
            }
        }

        address = soc_mem_addr_get(unit, mem, array_index, blk, index, &acc_type);
        
        *data32 = 0;
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, data32, INDIRECT_COMMAND_ADDRf, address);
        soc_reg_field_set(unit, ECI_INDIRECT_COMMAND_ADDRESSr, data32, INDIRECT_COMMAND_TYPEf, operation);
        if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMAND_ADDRESSr, blk, data32) != SOC_E_NONE){
            cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND_ADDRESS blk=%d (data: %d)\n", unit, blk, *data32);
            goto done;
        }
        
        
        *data32 = 0;
        if(operation == 0)
            soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGER_ON_DATAf, 1);
        else
            soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TRIGGERf, 1);

        soc_reg_field_set(unit, ECI_INDIRECT_COMMANDr, data32, INDIRECT_COMMAND_TIMEOUTf, 0x7fff);
        if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMANDr, blk, data32) != SOC_E_NONE){
            cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND blk=%d (data: %d)\n", unit, blk, *data32);
            goto done;
        }

        
        if (operation == 1) {
            *data32 = 0;
            soc_timeout_init(&to, 50000, 10);
            while (1) {
                if (dcmn_reg_access_with_block(unit, 0, ECI_INDIRECT_COMMANDr, blk, data32) != SOC_E_NONE) {
                    cli_out("unit %d: Failed reading reg=INDIRECT_COMMAND blk=%d (data: %d)\n", unit, blk, *data32);
                    goto done;
                }

                if (soc_reg_field_get(unit, ECI_INDIRECT_COMMANDr, *data32, INDIRECT_COMMAND_TRIGGERf)) {
                    if (soc_timeout_check(&to)) {
                        cli_out("unit %d: Indirect wide memory for block %d operation timed out\n", unit, blk);
                        goto done;
                    }
                } else {
                    break;
                }
            }
        }

        while (words_left > 0)
        {
            
            if (operation == 0) {
                if (dcmn_reg_access_with_block(unit, 1, ECI_INDIRECT_COMMAND_WR_DATAr, blk, entry_words) != SOC_E_NONE){
                    cli_out("unit %d: Failed writing to reg=INDIRECT_COMMAND_WR_DATA blk=%d (data: %u)\n", unit, blk, *entry_words);
                    goto done;
                }
            }
            
            if (operation == 1) { 
                if (dcmn_reg_access_with_block(unit, 0, ECI_INDIRECT_COMMAND_RD_DATAr, blk, rd_data) != SOC_E_NONE){
                    cli_out("unit %d: Failed reading from reg=INDIRECT_COMMAND_RD_DATA blk=%d\n", unit, blk);
                    goto done;
                }
                memcpy(entry_words, rd_data, (words_left > (indirect_size/32))? indirect_size/8 : words_left*4);
            }
            
            entry_words += indirect_size/32; 
            words_left  -= indirect_size/32;
        }

        if (soc_mem_is_signal(unit, mem) && dynamic_access_orig == 0) {
            
            *data32 = 0;
            soc_reg_field_set(unit, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, data32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 0);
            if (dcmn_reg_access_with_block(unit, 1, ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr, blk, data32) != SOC_E_NONE){
                cli_out("unit %d: Failed writing to reg=ENABLE_DYNAMIC_MEMORY_ACCESSr blk=%d (data: %d)\n", unit, blk, *data32);
                goto done;
            }
        }

    } 

    rv = 0;

 done:
    MEM_UNLOCK(unit, mem);
    return rv;
}



#ifdef DCMN_RUNTIME_DEBUG
dcmn_runtime_debug_t dcmn_runtime_debug_per_device[SOC_MAX_NUM_DEVICES] = {{0}};


void dcmn_runtime_debug_update_print_time(int unit, const char *string_to_print) {
    unsigned secs_s, mins_s; 
    unsigned secs_l, mins_l; 
    dcmn_runtime_debug_t *debug = dcmn_runtime_debug_per_device + unit;
    sal_time_t current_secs = sal_time();
    if (debug->run_stage == dcmn_runtime_debug_state_loading) {
        debug->run_stage = dcmn_runtime_debug_state_initializing;
        debug->last_time = debug->start_time = sal_time();
    }

    secs_s = current_secs - debug->start_time;
    secs_l = current_secs - debug->last_time;
    mins_s = secs_s / 60;
    mins_l = secs_l / 60;
    debug->last_time = current_secs;
    LOG_INFO(BSL_LS_SOC_INIT, ("==> u:%d %s from start: %u:%2.2u:%2.2u  from last: %u:%2.2u:%2.2u\n",
      unit, string_to_print, mins_s / 60, mins_s % 60, secs_s % 60, mins_l / 60, mins_l % 60, secs_l % 60));
}
#endif 

#undef _ERR_MSG_MODULE_NAME
