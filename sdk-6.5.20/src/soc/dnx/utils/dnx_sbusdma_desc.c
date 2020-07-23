
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */




#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/sbusdma.h>
#include <soc/drv.h> 
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>
#include <sal/core/time.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>

#include <sal/appl/sal.h>

#include <soc/cmicx.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DMA









#define DNX_SBUSDMA_DESC_KB_TO_UINT32 128


#define MAX_DESC_PREFETCH          15
#define MAX_DESC_PREFETCH_OFFSET   24











typedef struct
{                               
    uint32 cntrl;               
    uint32 req;                 
    uint32 count;               
    uint32 opcode;              
    uint32 addr;                
    uint32 hostaddrLo;          
    uint32 hostaddrHi;          
    uint32 rsrv;
} soc_sbusdma_desc_t;






STATIC uint32 dnx_sbusdma_desc_enabled[SOC_MAX_NUM_DEVICES] = { 0 };
STATIC uint32 dnx_sbusdma_desc_module_enabled[SOC_MAX_NUM_DEVICES][SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES] = { {0} };
STATIC uint32 dnx_sbusdma_desc_commit_counter[SOC_MAX_NUM_DEVICES];


STATIC uint32 *dnx_sbusdma_desc_mem_buff_a[SOC_MAX_NUM_DEVICES];
STATIC uint32 *dnx_sbusdma_desc_mem_buff_b[SOC_MAX_NUM_DEVICES];
STATIC uint32 *dnx_sbusdma_desc_mem_buff_main[SOC_MAX_NUM_DEVICES];
STATIC uint32 dnx_sbusdma_desc_mem_buff_counter[SOC_MAX_NUM_DEVICES];

STATIC uint32 dnx_sbusdma_desc_mem_buff_counter_max[SOC_MAX_NUM_DEVICES];

STATIC uint32 dnx_sbusdma_desc_mem_buff_counter_total[SOC_MAX_NUM_DEVICES];


STATIC soc_sbusdma_desc_t *dnx_sbusdma_desc_cfg_array_a[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_desc_t *dnx_sbusdma_desc_cfg_array_b[SOC_MAX_NUM_DEVICES];
STATIC soc_sbusdma_desc_t *dnx_sbusdma_desc_cfg_array_main[SOC_MAX_NUM_DEVICES];
STATIC uint32 dnx_sbusdma_desc_counter[SOC_MAX_NUM_DEVICES];

STATIC uint32 dnx_sbusdma_desc_counter_max[SOC_MAX_NUM_DEVICES];

STATIC uint32 dnx_sbusdma_desc_counter_total[SOC_MAX_NUM_DEVICES];


STATIC soc_sbusdma_desc_ctrl_t dnx_sbusdma_desc_ctrl[SOC_MAX_NUM_DEVICES];


STATIC uint32 dnx_sbusdma_desc_cfg_max[SOC_MAX_NUM_DEVICES];
STATIC uint32 dnx_sbusdma_desc_mem_max[SOC_MAX_NUM_DEVICES];
STATIC volatile uint32 dnx_sbusdma_desc_time_out_max[SOC_MAX_NUM_DEVICES] = { 0 };


STATIC volatile sbusdma_desc_handle_t dnx_sbusdma_desc_handle[SOC_MAX_NUM_DEVICES];

STATIC volatile sal_sem_t dnx_sbusdma_desc_timeout_sem[SOC_MAX_NUM_DEVICES];
STATIC volatile sal_mutex_t dnx_sbusdma_desc_timeout_mutex[SOC_MAX_NUM_DEVICES];
STATIC volatile sal_usecs_t dnx_sbusdma_desc_timeout_add_time[SOC_MAX_NUM_DEVICES];     
STATIC volatile sal_thread_t dnx_sbusdma_desc_timeout_tid[SOC_MAX_NUM_DEVICES];
STATIC volatile uint8 dnx_sbusdma_desc_timeout_terminate[SOC_MAX_NUM_DEVICES];


STATIC int dnx_sbusdma_desc_big_other[SOC_MAX_NUM_DEVICES];






uint32
dnx_sbusdma_desc_is_enabled(
    int unit,
    sbusdma_desc_module_enable_e module_enum)
{
    if (module_enum < SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES)
    {
        return dnx_sbusdma_desc_module_enabled[unit][module_enum];
    }
    else
    {
        return FALSE;
    }

}


STATIC shr_error_e
dnx_sbusdma_desc_wait_previous_done(
    int unit)
{
    soc_timeout_t to;
    uint8 state = 1;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_enabled[unit])
    {
        soc_timeout_init(&to, SOC_SBUSDMA_DM_TO(unit) * 2, 0);

        if (dnx_sbusdma_desc_handle[unit] != 0)
        {
            SHR_IF_ERR_EXIT(soc_sbusdma_desc_get_state(unit, dnx_sbusdma_desc_handle[unit], &state));
            
            while (state != 0)
            {
                if (soc_timeout_check(&to))
                {
                    SHR_ERR_EXIT(SOC_E_TIMEOUT, "Timeout waiting for descriptor DMA to finish.");
                }
                SHR_IF_ERR_EXIT(soc_sbusdma_desc_get_state(unit, dnx_sbusdma_desc_handle[unit], &state));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void STATIC
dnx_sbusdma_desc_cb(
    int unit,
    int status,
    sbusdma_desc_handle_t handle,
    void *data)
{
#ifndef DNX_SBUSDMA_DESC_PRINTS_ENABLED
    if (status != SOC_E_NONE)
#endif
    {
        int rv = SOC_E_NONE;
        int i = 0;
        soc_sbusdma_desc_ctrl_t ctrl = { 0 };
        soc_sbusdma_desc_cfg_t *cfg = NULL;

        if (status != SOC_E_NONE)
        {
            LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Desc SBUSDMA failed, handle: %d\n"), handle));
        }
#ifdef DNX_SBUSDMA_DESC_PRINTS_ENABLED
        else
        {
            LOG_CLI((BSL_META_U(0, "Successfully done DESC DMA, handle: %d\n"), handle));
        }
#endif

        cfg = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_cfg_t");
        if (cfg == NULL)
        {
            LOG_ERROR(BSL_LS_SOC_DMA,
                      (BSL_META_U(unit,
                                  "Error: Fail to allocate memory for SBUSDMA desc_cfg for failure log print.\n")));

        }
        else
        {
            rv = soc_sbusdma_desc_get(unit, handle, &ctrl, cfg);
            if (rv != SOC_E_NONE)
            {
                LOG_CLI((BSL_META_U(0, "%s(), soc_sbusdma_desc_get failed.\n"), FUNCTION_NAME()));
            }

            if (status != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit, "The following memory writes have failed (a total of %d):\n"),
                           ctrl.cfg_count));
            }
#ifdef DNX_SBUSDMA_DESC_PRINTS_ENABLED
            else
            {
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit,
                                      "The following memory writes have succeeded (a total of %d):\n"),
                           ctrl.cfg_count));
            }
#endif

            for (i = 0; i < ctrl.cfg_count; i++)
            {
                soc_sbusdma_desc_t *desc_p;
                uint8 is_mem;
                int opcode;
                int dst_blk;
                int acc_type;
                int dlen;

                desc_p = &(((soc_sbusdma_desc_t *) ctrl.hw_desc)[i]);

                soc_schan_header_cmd_get(unit, (schan_header_t *) & desc_p->opcode, &opcode,
                                         &dst_blk, NULL, &acc_type, &dlen, NULL, NULL);

                is_mem = ((opcode & WRITE_REGISTER_CMD_MSG) || (opcode & READ_MEMORY_CMD_MSG)) ? TRUE : FALSE;

                if (is_mem)
                {
                    
                    soc_mem_t mem = INVALIDm;
                    mem = soc_addr_to_mem_extended(unit, dst_blk, acc_type, desc_p->addr);

                    if (mem != INVALIDm)
                    {
                        LOG_ERROR(BSL_LS_SOC_DMA,
                                  (BSL_META_U(unit,
                                              "mem: %d, name = %s, blk: %d, addr: %d, dlen: %d, count: %d\n"),
                                   mem, SOC_MEM_NAME(unit, mem), dst_blk, desc_p->addr, dlen, desc_p->count));
                    }
                    else
                    {
                        LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "mem with addr: %d block: %d not found\n"),
                                                   desc_p->addr, dst_blk));
                    }
                }
                else
                {
                    
                    LOG_ERROR(BSL_LS_SOC_DMA,
                              (BSL_META_U(unit,
                                          "reg: blk: %d, addr: %d, dlen: %d, count: %d\n"),
                               dst_blk, desc_p->addr, dlen, desc_p->count));
                }
            }

            sal_free(cfg);
        }
    }
}

STATIC shr_error_e
dnx_sbusdma_desc_commit(
    int unit,
    uint8 safe)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_sbusdma_desc_timeout_mutex[unit] != NULL) && !safe)
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_previous_done(unit));

    if (dnx_sbusdma_desc_counter[unit] != 0)
    {
        int prefetch_iter;

        dnx_sbusdma_desc_ctrl[unit].cfg_count = dnx_sbusdma_desc_counter[unit];

        
        dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit] - 1].cntrl |= SOC_SBUSDMA_CTRL_LAST;

        
        for (prefetch_iter = 1; prefetch_iter <= MAX_DESC_PREFETCH; prefetch_iter++)
        {
            if (prefetch_iter > dnx_sbusdma_desc_counter[unit])
            {
                break;
            }

            dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit] - prefetch_iter].cntrl &=
                ~(MAX_DESC_PREFETCH << MAX_DESC_PREFETCH_OFFSET);
            dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit] - prefetch_iter].cntrl |=
                (prefetch_iter - 1) << MAX_DESC_PREFETCH_OFFSET;
        }

        
        dnx_sbusdma_desc_ctrl[unit].hw_desc = dnx_sbusdma_desc_cfg_array_main[unit];
        if (dnx_sbusdma_desc_mem_buff_main[unit] == dnx_sbusdma_desc_mem_buff_a[unit])
        {
            dnx_sbusdma_desc_cfg_array_main[unit] = dnx_sbusdma_desc_cfg_array_b[unit];
            dnx_sbusdma_desc_mem_buff_main[unit] = dnx_sbusdma_desc_mem_buff_b[unit];
        }
        else
        {
            dnx_sbusdma_desc_cfg_array_main[unit] = dnx_sbusdma_desc_cfg_array_a[unit];
            dnx_sbusdma_desc_mem_buff_main[unit] = dnx_sbusdma_desc_mem_buff_a[unit];
        }
        
        dnx_sbusdma_desc_mem_buff_counter_total[unit] += dnx_sbusdma_desc_mem_buff_counter[unit];
        if (dnx_sbusdma_desc_mem_buff_counter[unit] > dnx_sbusdma_desc_mem_buff_counter_max[unit])
        {
            dnx_sbusdma_desc_mem_buff_counter_max[unit] = dnx_sbusdma_desc_mem_buff_counter[unit];
        }
        dnx_sbusdma_desc_counter_total[unit] += dnx_sbusdma_desc_counter[unit];
        if (dnx_sbusdma_desc_counter[unit] > dnx_sbusdma_desc_counter_max[unit])
        {
            dnx_sbusdma_desc_counter_max[unit] = dnx_sbusdma_desc_counter[unit];
        }
        dnx_sbusdma_desc_commit_counter[unit]++;

        dnx_sbusdma_desc_mem_buff_counter[unit] = 0;
        dnx_sbusdma_desc_counter[unit] = 0;

        SHR_IF_ERR_CONT(soc_sbusdma_desc_update_ctrl
                        (unit, (sbusdma_desc_handle_t) dnx_sbusdma_desc_handle[unit], &dnx_sbusdma_desc_ctrl[unit]));

        SHR_IF_ERR_CONT(soc_sbusdma_desc_run(unit, (sbusdma_desc_handle_t) dnx_sbusdma_desc_handle[unit]));
    }

exit:
    if ((dnx_sbusdma_desc_timeout_mutex[unit] != NULL) && !safe)
    {
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_wait_done(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_sbusdma_desc_enabled[unit])
    {
        
        SHR_IF_ERR_CONT(dnx_sbusdma_desc_commit(unit, 0  ));
        SHR_IF_ERR_CONT(dnx_sbusdma_desc_commit(unit, 0  ));
    }

    SHR_FUNC_EXIT;
}


void
dnx_sbusdma_desc_prepare(
    int unit,
    soc_sbusdma_desc_t * desc,
    soc_sbusdma_desc_cfg_t * cfg,
    soc_sbusdma_desc_ctrl_t * ctrl)
{
    schan_msg_t msg;
    int opcode, dst_blk, acc_type, dlen;
    uint32 flags;

    sal_memset(desc, 0, sizeof(soc_sbusdma_desc_t));

    schan_msg_clear(&msg);
    flags = (ctrl->flags & SOC_SBUSDMA_CFG_USE_FLAGS) ? cfg->flags : ctrl->flags;

    if (flags & SOC_SBUSDMA_WRITE_CMD_MSG)
    {
        opcode = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ? WRITE_MEMORY_CMD_MSG : WRITE_REGISTER_CMD_MSG;
        dlen = cfg->width * sizeof(uint32);
    }
    else
    {
        opcode = (flags & SOC_SBUSDMA_MEMORY_CMD_MSG) ? READ_MEMORY_CMD_MSG : READ_REGISTER_CMD_MSG;
        dlen = 0;
    }
    acc_type = cfg->acc_type;
    dst_blk = cfg->blk;
    soc_schan_header_cmd_set(unit, &msg.header, opcode, dst_blk, 0, acc_type, dlen, 0, 0);

    
    desc->cntrl |= MAX_DESC_PREFETCH << MAX_DESC_PREFETCH_OFFSET;
    desc->count = cfg->count;
    desc->opcode = msg.dwords[0];
    desc->addr = cfg->addr;

    desc->hostaddrLo = PTR_TO_INT(soc_cm_l2p(unit, cfg->buff));
    desc->hostaddrHi = PTR_HI_TO_INT(soc_cm_l2p(unit, cfg->buff));
    if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE)
    {
        desc->hostaddrHi |= CMIC_PCIE_SO_OFFSET;
    }

    if ((opcode == WRITE_MEMORY_CMD_MSG) || (opcode == WRITE_REGISTER_CMD_MSG))
    {
        
        desc->req |= cfg->width << 5;
        
        if (cfg->mem_clear)
        {
            
            desc->req |= 1 << 30;
        }
        else
        {
            
        }
    }
    else
    {
        
        
        desc->req |= cfg->width << 0;
    }
    
    desc->req |= cfg->addr_shift << 24;

    {
        if (dnx_sbusdma_desc_big_other[unit])
        {
            
            desc->req |= 1 << 11;
            
            desc->req |= 1 << 10;
        }
    }

    return;
}


shr_error_e
dnx_sbusdma_desc_add_mem_reg(
    int unit,
    uint32 mem,
    uint32 reg,
    uint32 array_index,
    int blk,
    uint32 offset,
    uint32 count,
    int mem_clear,
    void *entry_data)
{
    uint32 cfg_addr;
    uint8 cfg_acc_type;
    int cfg_blk = blk;
    uint32 entry_size;
    uint32 give_sem = 0;
    uint32 rv = 0;
    uint32 entries_to_shift;
    soc_sbusdma_desc_cfg_t cfg;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_enabled[unit] != 1)
    {
        SHR_ERR_EXIT(SOC_E_UNAVAIL, "Desc DMA not enabled.");
    }

    if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    if (mem != INVALIDm)
    {
        SOC_MEM_ALIAS_TO_ORIG(unit, mem);

        cfg_addr = soc_mem_addr_get(unit, mem, array_index, cfg_blk, offset, &cfg_acc_type);
        entry_size = SOC_MEM_WORDS(unit, mem);

        if (soc_feature(unit, soc_feature_new_sbus_format))
        {
            cfg_blk = SOC_BLOCK2SCH(unit, cfg_blk);
        }
    }
    else if (reg != INVALIDr)
    {
        
        count = 1;

        cfg_addr = soc_reg_addr_get(unit, reg, blk, array_index, SOC_REG_ADDR_OPTION_WRITE, &cfg_blk, &cfg_acc_type);

        if (SOC_REG_IS_ABOVE_64(unit, reg))
        {
            entry_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
        }
        else if (SOC_REG_IS_64(unit, reg))
        {
            entry_size = 2;
        }
        else
        {
            entry_size = 1;
        }
    }
    else
    {
        if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        }
        SHR_ERR_EXIT(SOC_E_PARAM, "Descriptor DMA expects either valid mem or valid reg.");
    }

    
    entries_to_shift = mem_clear ? 1 : count;

    
    if ((dnx_sbusdma_desc_counter[unit] == dnx_sbusdma_desc_cfg_max[unit]) ||
        
        (dnx_sbusdma_desc_mem_buff_counter[unit] + entry_size * entries_to_shift +
         (SOC_REG_ABOVE_64_MAX_SIZE_U32 + 2) >= dnx_sbusdma_desc_mem_max[unit]))
    {
        rv = dnx_sbusdma_desc_commit(unit, 1  );
        if (rv != SOC_E_NONE)
        {
            if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
            {
                sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            }
            SHR_ERR_EXIT(rv, "dnx_sbusdma_desc_commit_unsafe failed.");
        }
    }

    if (dnx_sbusdma_desc_counter[unit] == 0)
    {
        dnx_sbusdma_desc_timeout_add_time[unit] = sal_time_usecs();
        give_sem++;
    }

    sal_memcpy(&dnx_sbusdma_desc_mem_buff_main[unit][dnx_sbusdma_desc_mem_buff_counter[unit]], entry_data,
               sizeof(uint32) * entry_size * entries_to_shift);

    if (mem != INVALIDm)
    {
        cfg.flags = SOC_SBUSDMA_MEMORY_CMD_MSG | SOC_SBUSDMA_WRITE_CMD_MSG;
    }
    else if (reg != INVALIDr)
    {
        cfg.flags = SOC_SBUSDMA_WRITE_CMD_MSG;
    }

    cfg.acc_type = cfg_acc_type;
    cfg.addr = cfg_addr;
    cfg.addr_shift = 0;
    cfg.blk = cfg_blk;
    cfg.buff = &dnx_sbusdma_desc_mem_buff_main[unit][dnx_sbusdma_desc_mem_buff_counter[unit]];
    cfg.count = count;
    cfg.width = entry_size;
    cfg.mem_clear = mem_clear;

    dnx_sbusdma_desc_prepare(unit, &dnx_sbusdma_desc_cfg_array_main[unit][dnx_sbusdma_desc_counter[unit]],
                             &cfg, &dnx_sbusdma_desc_ctrl[unit]);

    dnx_sbusdma_desc_counter[unit] += 1;

    dnx_sbusdma_desc_mem_buff_counter[unit] += entry_size * entries_to_shift;

    if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
    }

    if ((dnx_sbusdma_desc_timeout_sem[unit] != 0) && give_sem)
    {
        sal_sem_give(dnx_sbusdma_desc_timeout_sem[unit]);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_reg(
    int unit,
    uint32 reg_u32,
    int instance,
    uint32 array_index,
    void *entry_data)
{
    soc_reg_t reg = reg_u32;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, INVALIDm, reg, array_index, instance, 0  , 1  , FALSE, entry_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_mem(
    int unit,
    uint32 mem_u32,
    int array_index,
    int blk,
    uint32 offset,
    void *entry_data)
{
    soc_mem_t mem = mem_u32;
    int array_index_max, array_index_min, array_id, blk_i;

    SHR_FUNC_INIT_VARS(unit);

    array_index_min = 0;
    array_index_max = 1;

    
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        if (array_index == -1)
        {
            array_index_max = SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            
            array_index_min = array_index;
            array_index_max = array_index + 1;
        }
    }

    for (array_id = array_index_min; array_id < array_index_max; array_id++)
    {
        if (blk == MEM_BLOCK_ALL)
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk_i)
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk_i, offset, 1     ,
                                                             FALSE, entry_data));
                if (SOC_MEM_STATE(unit, mem).cache[blk_i] != NULL)
                {
                    _soc_mem_write_cache_update(unit, mem, blk_i, 0, offset, array_index, entry_data, NULL, NULL, NULL);
                }
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk, offset, 1  ,
                                                         FALSE, entry_data));
            if (SOC_MEM_STATE(unit, mem).cache[blk] != NULL)
            {
                _soc_mem_write_cache_update(unit, mem, blk, 0, offset, array_index, entry_data, NULL, NULL, NULL);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_mem_table(
    int unit,
    uint32 mem_u32,
    int array_index,
    int blk,
    uint32 offset,
    uint32 count,
    void *entry_data)
{
    soc_mem_t mem = mem_u32;
    int array_index_max, array_index_min, array_id, blk_i;
    uint32 final_offset, final_count;

    SHR_FUNC_INIT_VARS(unit);

    final_offset = offset;
    final_count = count;

    array_index_min = 0;
    array_index_max = 1;

    
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        if (array_index == -1)
        {
            array_index_max = SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            
            array_index_min = array_index;
            array_index_max = array_index + 1;
        }
    }

    
    for (array_id = array_index_min; array_id < array_index_max; array_id++)
    {
        if (blk == MEM_BLOCK_ALL)
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk_i)
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk_i, final_offset,
                                                             final_count, FALSE, entry_data));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk, final_offset,
                                                         final_count, FALSE, entry_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_add_mem_clear(
    int unit,
    uint32 mem_u32,
    int array_index,
    int blk,
    void *entry_data)
{
    soc_mem_t mem = mem_u32;
    int array_index_max, array_index_min, array_id, blk_i;
    uint32 offset, count;

    SHR_FUNC_INIT_VARS(unit);

    array_index_min = 0;
    array_index_max = 1;

    
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        if (array_index == -1)
        {
            array_index_max = SOC_MEM_NUMELS(unit, mem) + SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
            array_index_min = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
        }
        else
        {
            
            array_index_min = array_index;
            array_index_max = array_index + 1;
        }
    }

    
    offset = soc_mem_index_min(unit, mem);
    count = soc_mem_index_max(unit, mem) - offset + 1;

    
    for (array_id = array_index_min; array_id < array_index_max; array_id++)
    {
        if (blk == MEM_BLOCK_ALL)
        {
            SOC_MEM_BLOCK_ITER(unit, mem, blk_i)
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk_i, offset, count, TRUE,
                                                             entry_data));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem_reg(unit, mem, INVALIDr, array_id, blk, offset, count, TRUE,
                                                         entry_data));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_cleanup(
    int unit)
{
    int module_iter;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
    {
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
    }

    if (dnx_sbusdma_desc_mem_buff_a[unit] != NULL)
    {
        soc_cm_sfree(unit, dnx_sbusdma_desc_mem_buff_a[unit]);
        dnx_sbusdma_desc_mem_buff_a[unit] = NULL;
    }
    if (dnx_sbusdma_desc_mem_buff_b[unit] != NULL)
    {
        soc_cm_sfree(unit, dnx_sbusdma_desc_mem_buff_b[unit]);
        dnx_sbusdma_desc_mem_buff_b[unit] = NULL;
    }
    dnx_sbusdma_desc_mem_buff_main[unit] = NULL;

    if (dnx_sbusdma_desc_cfg_array_a[unit] != NULL)
    {
        soc_cm_sfree(unit, dnx_sbusdma_desc_cfg_array_a[unit]);
        dnx_sbusdma_desc_cfg_array_a[unit] = NULL;
    }
    if (dnx_sbusdma_desc_cfg_array_b[unit] != NULL)
    {
        soc_cm_sfree(unit, dnx_sbusdma_desc_cfg_array_b[unit]);
        dnx_sbusdma_desc_cfg_array_b[unit] = NULL;
    }
    dnx_sbusdma_desc_cfg_array_main[unit] = NULL;

    
    if ((dnx_sbusdma_desc_timeout_tid[unit] != NULL) && (dnx_sbusdma_desc_timeout_tid[unit] != SAL_THREAD_ERROR))
    {
        dnx_sbusdma_desc_timeout_terminate[unit] = 1;
        sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        sal_sem_give(dnx_sbusdma_desc_timeout_sem[unit]);
    }
    else
    {
        if (dnx_sbusdma_desc_timeout_mutex[unit])
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            sal_mutex_destroy(dnx_sbusdma_desc_timeout_mutex[unit]);
            dnx_sbusdma_desc_timeout_mutex[unit] = NULL;
        }

        if (dnx_sbusdma_desc_timeout_sem[unit])
        {
            sal_sem_destroy(dnx_sbusdma_desc_timeout_sem[unit]);
            dnx_sbusdma_desc_timeout_sem[unit] = NULL;
        }
    }

    
    if (dnx_sbusdma_desc_handle[unit] != 0)
    {
        SHR_IF_ERR_CONT(soc_sbusdma_desc_delete(unit, dnx_sbusdma_desc_handle[unit]));
        dnx_sbusdma_desc_handle[unit] = 0;
    }

    dnx_sbusdma_desc_enabled[unit] = 0;
    for (module_iter = 0; module_iter < SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES; module_iter++)
    {
        dnx_sbusdma_desc_module_enabled[unit][module_iter] = 0;
    }
    dnx_sbusdma_desc_mem_max[unit] = 0;
    dnx_sbusdma_desc_cfg_max[unit] = 0;

    SHR_FUNC_EXIT;
}

STATIC void
dnx_sbusdma_desc_init_timeout_thread(
    void *cookie)
{
    int unit = PTR_TO_INT(cookie);
    int rv = SOC_E_NONE;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;
    sal_usecs_t elapsed_time;
    sal_usecs_t add_time;
    sal_usecs_t timeout_max = dnx_sbusdma_desc_time_out_max[unit];

    while (1)
    {
        sal_sem_take(dnx_sbusdma_desc_timeout_sem[unit], sal_sem_FOREVER);
        sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
        if (dnx_sbusdma_desc_timeout_terminate[unit])
        {
            
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            if (dnx_sbusdma_desc_timeout_mutex[unit])
            {
                sal_mutex_destroy(dnx_sbusdma_desc_timeout_mutex[unit]);
                dnx_sbusdma_desc_timeout_mutex[unit] = NULL;
            }

            if (dnx_sbusdma_desc_timeout_sem[unit])
            {
                sal_sem_destroy(dnx_sbusdma_desc_timeout_sem[unit]);
                dnx_sbusdma_desc_timeout_sem[unit] = NULL;
            }

            dnx_sbusdma_desc_timeout_tid[unit] = NULL;

            sal_thread_exit(0);
        }

        
        if (dnx_sbusdma_desc_counter[unit] == 0)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            continue;
        }
        add_time = dnx_sbusdma_desc_timeout_add_time[unit];
        elapsed_time = sal_time_usecs() - add_time;
        while (elapsed_time < timeout_max)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            sal_usleep(timeout_max - elapsed_time);
            sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
            elapsed_time = sal_time_usecs() - add_time;
        }

        
        if (add_time == dnx_sbusdma_desc_timeout_add_time[unit])
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
            rv = dnx_sbusdma_desc_commit(unit, 0  );
            if (rv != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_SOC_DMA,
                          (BSL_META_U(unit, "dnx_sbusdma_desc_commit returned with error: %d, unit %d\n"), rv, unit));
            }
        }
        else
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        }
    }

    
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof(thread_name));
    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "AbnormalThreadExit:%s, unit %d\n"), thread_name, unit));

    dnx_sbusdma_desc_timeout_tid[unit] = SAL_THREAD_ERROR;

    sal_thread_exit(0);
}

shr_error_e
dnx_sbusdma_desc_init_inner(
    int unit,
    uint32 buff_size_kb,
    uint32 chain_length_max,
    uint32 timeout_usec)
{
    char buffer_name[50];
    int module_iter;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_sbusdma_desc_mem_buff_a[unit] != NULL) || (dnx_sbusdma_desc_mem_buff_b[unit] != NULL))
    {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Descriptor DMA buffers already allocated.");
    }

    
    if ((buff_size_kb == 0) || (chain_length_max == 0))
    {
        SHR_ERR_EXIT(SOC_E_CONFIG,
                     "Descriptor DMA size values are invalid, they should either be all zeros(disabled) or positive."
                     "Current values: buff_size_kb %d, chain_length_max %d, timeout_usec %d", buff_size_kb,
                     chain_length_max, timeout_usec);
    }

    dnx_sbusdma_desc_mem_max[unit] = buff_size_kb * DNX_SBUSDMA_DESC_KB_TO_UINT32;
    dnx_sbusdma_desc_cfg_max[unit] = chain_length_max;
    dnx_sbusdma_desc_time_out_max[unit] = timeout_usec;

    dnx_sbusdma_desc_commit_counter[unit] = 0;

    
    sal_snprintf(buffer_name, sizeof(buffer_name), "SBUSDMA desc buffer a, Unit %d", unit);
    if ((dnx_sbusdma_desc_mem_buff_a[unit] =
         (uint32 *) soc_cm_salloc(unit, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit], buffer_name)) == NULL)
    {
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc buffer a.");
    }
    sal_memset(dnx_sbusdma_desc_mem_buff_a[unit], 0, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit]);

    sal_snprintf(buffer_name, sizeof(buffer_name), "SBUSDMA desc buffer b, Unit %d", unit);
    if ((dnx_sbusdma_desc_mem_buff_b[unit] =
         (uint32 *) soc_cm_salloc(unit, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit], buffer_name)) == NULL)
    {
        dnx_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc buffer b.");
    }
    sal_memset(dnx_sbusdma_desc_mem_buff_b[unit], 0, sizeof(uint32) * dnx_sbusdma_desc_mem_max[unit]);

    dnx_sbusdma_desc_mem_buff_main[unit] = dnx_sbusdma_desc_mem_buff_a[unit];
    dnx_sbusdma_desc_mem_buff_counter[unit] = 0;
    dnx_sbusdma_desc_mem_buff_counter_max[unit] = 0;
    dnx_sbusdma_desc_mem_buff_counter_total[unit] = 0;

    
    dnx_sbusdma_desc_cfg_array_a[unit] =
        soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_t");
    if (dnx_sbusdma_desc_cfg_array_a[unit] == NULL)
    {
        dnx_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc_cfg a");
    }

    dnx_sbusdma_desc_cfg_array_b[unit] =
        soc_cm_salloc(unit, sizeof(soc_sbusdma_desc_t) * dnx_sbusdma_desc_cfg_max[unit], "soc_sbusdma_desc_t");
    if (dnx_sbusdma_desc_cfg_array_b[unit] == NULL)
    {
        dnx_sbusdma_desc_cleanup(unit);
        SHR_ERR_EXIT(SOC_E_MEMORY, "Error: Fail to allocate memory for SBUSDMA desc_cfg b");
    }
    dnx_sbusdma_desc_cfg_array_main[unit] = dnx_sbusdma_desc_cfg_array_a[unit];
    dnx_sbusdma_desc_counter[unit] = 0;
    dnx_sbusdma_desc_counter_max[unit] = 0;
    dnx_sbusdma_desc_counter_total[unit] = 0;

    
    if (timeout_usec != 0)
    {
        dnx_sbusdma_desc_timeout_mutex[unit] = sal_mutex_create("DESC DMA TO mutex");
        if (dnx_sbusdma_desc_timeout_mutex[unit] == NULL)
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_MEMORY, "TO Mutex allocation failure.");
        }

        dnx_sbusdma_desc_timeout_sem[unit] = sal_sem_create("DESC DMA TO sem", sal_sem_COUNTING, 0);
        if (dnx_sbusdma_desc_timeout_sem[unit] == NULL)
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_MEMORY, "TO Sempahore allocation failure.");
        }

        dnx_sbusdma_desc_timeout_add_time[unit] = sal_time_usecs();

        dnx_sbusdma_desc_timeout_terminate[unit] = 0;

        if ((dnx_sbusdma_desc_timeout_tid[unit] == SAL_THREAD_ERROR) || (dnx_sbusdma_desc_timeout_tid[unit] == NULL))
        {
            dnx_sbusdma_desc_timeout_tid[unit] = sal_thread_create("Desc DMA Timeout", SAL_THREAD_STKSZ, 50      ,
                                                                   dnx_sbusdma_desc_init_timeout_thread,
                                                                   INT_TO_PTR(unit));
            if ((dnx_sbusdma_desc_timeout_tid[unit] == NULL)
                || (dnx_sbusdma_desc_timeout_tid[unit] == SAL_THREAD_ERROR))
            {
                dnx_sbusdma_desc_cleanup(unit);
                SHR_ERR_EXIT(SOC_E_INTERNAL, "DESC DMA TO thread create failed. \n");
            }
        }
        else
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_EXISTS, "TO thread already exists.");
        }
    }

    {
        int big_pio, big_packet;

        soc_endian_get(unit, &big_pio, &big_packet, &dnx_sbusdma_desc_big_other[unit]);
    }

    {
        
        soc_sbusdma_desc_cfg_t *desc_cfg_arr_p;

        desc_cfg_arr_p = sal_alloc(sizeof(soc_sbusdma_desc_cfg_t) * chain_length_max, "soc_sbusdma_desc_cfg_t");
        if (desc_cfg_arr_p == NULL)
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_INTERNAL, "desc_cfg allocate failed. \n");
        }

        dnx_sbusdma_desc_ctrl[unit].flags =
            SOC_SBUSDMA_CFG_USE_FLAGS | SOC_SBUSDMA_WRITE_CMD_MSG | SOC_SBUSDMA_CFG_USE_SUPPLIED_DESC |
            SOC_SBUSDMA_CFG_PREFETCH_ENB;
        sal_strncpy(dnx_sbusdma_desc_ctrl[unit].name, "DESC DMA", sizeof(dnx_sbusdma_desc_ctrl[unit].name) - 1);
        dnx_sbusdma_desc_ctrl[unit].cfg_count = chain_length_max;
        dnx_sbusdma_desc_ctrl[unit].hw_desc = dnx_sbusdma_desc_cfg_array_main[unit];
        dnx_sbusdma_desc_ctrl[unit].buff = NULL;
        dnx_sbusdma_desc_ctrl[unit].cb = dnx_sbusdma_desc_cb;
        dnx_sbusdma_desc_ctrl[unit].data = NULL;

        SHR_IF_ERR_CONT(soc_sbusdma_desc_create
                        (unit, &dnx_sbusdma_desc_ctrl[unit], desc_cfg_arr_p, TRUE,
                         (sbusdma_desc_handle_t *) & dnx_sbusdma_desc_handle[unit]));
        sal_free(desc_cfg_arr_p);
        if (dnx_sbusdma_desc_handle[unit] == 0)
        {
            dnx_sbusdma_desc_cleanup(unit);
            SHR_ERR_EXIT(SOC_E_INTERNAL, "DESC DMA handle allocate failed. \n");
        }
    }

    dnx_sbusdma_desc_enabled[unit] = 1;
    for (module_iter = 0; module_iter < SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES; module_iter++)
    {
        dnx_sbusdma_desc_module_enabled[unit][module_iter] = dnx_data_sbusdma_desc.global.enable_module_desc_dma_get(
    unit,
    module_iter)->enable;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    if (dnx_data_sbusdma_desc.global.feature_get(unit, dnx_data_sbusdma_desc_global_desc_dma))
    {
        uint32 buff_size_kb = dnx_data_sbusdma_desc.global.dma_desc_aggregator_buff_size_kb_get(
    unit);
        uint32 chain_length_max = dnx_data_sbusdma_desc.global.dma_desc_aggregator_chain_length_max_get(
    unit);
        uint32 timeout_usec = dnx_data_sbusdma_desc.global.dma_desc_aggregator_timeout_usec_get(
    unit);

        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_init_with_params(unit, buff_size_kb, chain_length_max, timeout_usec));
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_init_with_params(
    int unit,
    uint32 buff_size_kb,
    uint32 chain_length_max,
    uint32 timeout_usec)
{
    SHR_FUNC_INIT_VARS(unit);

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    
    if ((buff_size_kb != 0) && (chain_length_max != 0))
    {
        if ((dnx_data_intr.general.tslam_dma_enable_get(unit) == FALSE)
            || (dnx_data_intr.general.table_dma_enable_get(unit) == FALSE))
        {
            SHR_ERR_EXIT(SOC_E_CONFIG,
                         "%s and %s soc properties must be enabled if dma_desc_aggregator soc properties are enabled.",
                         spn_TSLAM_DMA_ENABLE, spn_TABLE_DMA_ENABLE);
        }
        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_init_inner(unit, buff_size_kb, chain_length_max, timeout_usec));
    }
    else
    {
        if ((buff_size_kb != 0) || (chain_length_max != 0))
        {
            SHR_ERR_EXIT(SOC_E_PARAM,
                         "All three descriptor DMA parameters should be non-zero. buff_size_kb=%d, chain_length_max=%d, timeout_usec=%d (optional).",
                         buff_size_kb, chain_length_max, timeout_usec);
        }
    }

exit:
#endif
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_sbusdma_desc_enabled[unit])
    {
        SHR_IF_ERR_CONT(dnx_sbusdma_desc_wait_done(unit));
        dnx_sbusdma_desc_cleanup(unit);

        dnx_sbusdma_desc_enabled[unit] = 0;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sbusdma_desc_get_stats(
    int unit,
    int clear,
    dnx_sbusdma_desc_stats_t * desc_stats)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_sbusdma_desc_enabled[unit])
    {
        if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
        {
            sal_mutex_take(dnx_sbusdma_desc_timeout_mutex[unit], sal_mutex_FOREVER);
        }
        desc_stats->buff_size_kb = dnx_sbusdma_desc_mem_max[unit] / DNX_SBUSDMA_DESC_KB_TO_UINT32;
        desc_stats->chain_length_max = dnx_sbusdma_desc_cfg_max[unit];
        desc_stats->timeout_usec = dnx_sbusdma_desc_time_out_max[unit];
        desc_stats->enabled = dnx_sbusdma_desc_enabled[unit];

        desc_stats->commit_counter = dnx_sbusdma_desc_commit_counter[unit];
        desc_stats->chain_length_counter_max = dnx_sbusdma_desc_counter_max[unit];
        desc_stats->chain_length_counter_total = dnx_sbusdma_desc_counter_total[unit];
        desc_stats->buff_size_kb_max =
            UTILEX_DIV_ROUND_UP(dnx_sbusdma_desc_mem_buff_counter_max[unit], DNX_SBUSDMA_DESC_KB_TO_UINT32);
        desc_stats->buff_size_kb_total =
            UTILEX_DIV_ROUND_UP(dnx_sbusdma_desc_mem_buff_counter_total[unit], DNX_SBUSDMA_DESC_KB_TO_UINT32);
        if (clear == TRUE)
        {
            dnx_sbusdma_desc_commit_counter[unit] = 0;
            dnx_sbusdma_desc_counter_max[unit] = 0;
            dnx_sbusdma_desc_counter_total[unit] = 0;
            dnx_sbusdma_desc_mem_buff_counter_max[unit] = 0;
            dnx_sbusdma_desc_mem_buff_counter_total[unit] = 0;
        }
        if (dnx_sbusdma_desc_timeout_mutex[unit] != NULL)
        {
            sal_mutex_give(dnx_sbusdma_desc_timeout_mutex[unit]);
        }
    }
    else
    {
        sal_memset(desc_stats, 0x0, sizeof(dnx_sbusdma_desc_stats_t));
    }

    SHR_FUNC_EXIT;
}


