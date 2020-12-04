/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    access_pack.c
 * Purpose: Miscellaneous routine for device db access
 */

#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_common.h>
#include <soc/sand/auto_generated/dbx_pre_compiled_jr2_devices.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/access.h>
#endif
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <soc/dpp/JER/jer_reg_access.h>
#endif
#include <soc/sand/sand_aux_access.h>
#include "sand_signals_internal.h"

#include <bcm/types.h>

#define BSL_LOG_MODULE BSL_LS_SOC_COMMON


static device_t *sand_device_array[SOC_MAX_NUM_DEVICES] = {NULL, NULL, NULL, NULL, NULL};

shr_error_e
shr_access_get_sdk(
    char *sdk_location)
{
    static char sdk_location_base[RHFILE_MAX_SIZE] = { 0 };
    char *systems_ptr;
    int unit = 0;

    
    if (sdk_location == NULL)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Invalid pointer for SDK location\n")));
        return _SHR_E_PARAM;
    }
    
    if (ISEMPTY(sdk_location_base))
    {
        sal_getcwd(sdk_location_base, RHFILE_MAX_SIZE);
        
        if((systems_ptr = sal_strstr(sdk_location_base, "systems")) == NULL)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "No systems in:'%s' - SDK location cannot be obtained\n"),
                                                                                                    sdk_location_base));
            return _SHR_E_PARAM;
        }
        
        systems_ptr[0] = 0;
    }
    sal_strncpy(sdk_location, sdk_location_base, RHFILE_MAX_SIZE - 1);
    return _SHR_E_NONE;
}

char*
shr_access_get_sand_name(int unit)
{
    char *sand_name;
    if(SOC_IS_DNX(unit))
    {
        sand_name = "dnx";
    }
    else if(SOC_IS_DNXF(unit))
    {
        sand_name = "dnxf";
    }
    else if(SOC_IS_DPP(unit))
    {
        sand_name = "dpp";
    }
    else if(SOC_IS_DFE(unit))
    {
        sand_name = "dfe";
    }
    else
    {
        sand_name = "unknown";
    }

    return sand_name;
}

int
shr_access_reg_no_read_get(
        int         unit,
        soc_reg_t   reg)
{
    device_t *device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        
        return 0;
    }

    if(reg < 0 || reg >= NUM_SOC_REG)
    {
        
        return 0;
    }

    return ((device->reg_data[reg].flags & ACC_NO_READ) ? 1 : 0);
}

int
shr_access_mem_no_read_get(
        int         unit,
        soc_mem_t   mem)
{
    device_t *device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        
        return 0;
    }

    if(mem < 0 || mem >= NUM_SOC_MEM)
    {
        
        return 0;
    }

    
    return ((device->mem_data[mem].flags & ACC_NO_READ) ? 1 : 0);
}

int
shr_access_reg_no_wb_get(
        int         unit,
        soc_reg_t   reg)
{
    device_t *device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        
        return 0;
    }

    if(reg < 0 || reg >= NUM_SOC_REG)
    {
        
        return 0;
    }

    return ((device->reg_data[reg].flags & ACC_NO_WB) ? 1 : 0);
}

int
shr_access_mem_no_wb_get(
        int         unit,
        soc_mem_t   mem)
{
    device_t *device;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        
        return 0;
    }

    if(mem < 0 || mem >= NUM_SOC_MEM)
    {
        
        return 0;
    }

    
    return ((device->mem_data[mem].flags & ACC_NO_WB) ? 1 : 0);
}

shr_error_e
shr_access_reg_present_in_block(
    int                unit,
    int                cmic_block,
    soc_block_types_t  reg_block_list,
    int*               block_id_p)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_bl;

    for (i_bl = 0; SOC_BLOCK_TYPE(unit, i_bl) >= 0; i_bl++)
    {
       if(SOC_BLOCK2OFFSET(unit, i_bl) == cmic_block)
       {
           if(SOC_BLOCK_IS_TYPE(unit, i_bl, reg_block_list))
           {
               if(block_id_p != NULL)
               {
                   *block_id_p = i_bl;
               }
               rv = _SHR_E_NONE;
           }
           break;
       }
    }

    return rv;
}

shr_error_e
shr_access_reg_get(int unit, uint32 base_address, int cmic_block_id, int offset, uint8 opcode, rhlist_t *hit_list)
{
    soc_reg_t reg;
    rhhandle_t temp = NULL;
    shr_hit_entry_t *hit_entry;
    int block_id, blk;
    uint8 acc_type;

    SHR_FUNC_INIT_VARS(unit);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        soc_reg_info_t *reginfo;

        if (!SOC_REG_IS_VALID(unit, reg))
            continue;

        reginfo = &SOC_REG_INFO(unit, reg);

        if(base_address != SOC_REG_BASE(unit, reg))
            continue;

        if((shr_access_reg_present_in_block(unit, cmic_block_id, reginfo->block, &block_id)) != _SHR_E_NONE)
            continue;

        if((offset < 0) || (offset >= SOC_REG_NUMELS(unit, reg)))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Invalid offset:0x%08x for Register:%s\n"), offset, SOC_REG_NAME(unit, reg)));
            continue;
        }
        else if((block_id < 0) || (block_id >= SOC_MAX_NUM_BLKS))
        {
            LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Invalid block id:%d for Register:%s\n"), block_id, SOC_REG_NAME(unit, reg)));
            continue;
        }

        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(hit_list, SOC_REG_NAME(unit, reg), reg, &temp));

        hit_entry = temp;
        hit_entry->opcode = opcode;
        hit_entry->offset = offset;
        hit_entry->address = soc_reg_addr_get(unit, reg, SOC_BLOCK_PORT(unit, block_id), offset + reginfo->first_array_index, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
        hit_entry->block_id = block_id;
        hit_entry->cmic_block_id = cmic_block_id;
        sal_strncpy(hit_entry->block_n, SOC_BLOCK_NAME(unit, block_id), RHNAME_MAX_SIZE - 1);

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
shr_access_mem_present_in_block(
    int                unit,
    int                cmic_block,
    soc_mem_t          mem,
    int*               block_id_p)
{
    shr_error_e rv = _SHR_E_NOT_FOUND;
    int i_bl;

    SOC_MEM_BLOCK_ITER(unit, mem, i_bl)
    {
        if(SOC_BLOCK2OFFSET(unit, i_bl) == cmic_block)
        {
            if(block_id_p != NULL)
            {
                *block_id_p = i_bl;
            }
            rv = _SHR_E_NONE;
        }
        break;
    }

    return rv;
}

shr_error_e
shr_access_mem_get(int unit, uint32 base_address, int cmic_block_id, int offset, uint8 opcode, rhlist_t *hit_list)
{
    soc_mem_t mem;
    rhhandle_t temp = NULL;
    shr_hit_entry_t *hit_entry;
    soc_mem_info_t *meminfo;
    int block_id, entry_id, index_id;
    uint8 acc_type;

    SHR_FUNC_INIT_VARS(unit);

    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
            continue;

        
        if(base_address != SOC_MEM_INFO(unit, mem).base)
            continue;

        if((shr_access_mem_present_in_block(unit, cmic_block_id, mem, &block_id)) != _SHR_E_NONE)
            continue;

        meminfo = &SOC_MEM_INFO(unit, mem);
        if((meminfo->flags & SOC_MEM_FLAG_IS_ARRAY) && (SOC_MEM_ARRAY_INFOP(unit, mem) != NULL))
        {
            entry_id = offset % SOC_MEM_ELEM_SKIP_SAFE(unit, mem);
            index_id = offset / SOC_MEM_ELEM_SKIP_SAFE(unit, mem);

            if(index_id >= SOC_MEM_NUMELS_SAFE(unit, mem))
            {
                LOG_WARN(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Offset:0x%08x is too big for memory:%s(0x%08x)\n"), offset, SOC_MEM_NAME(unit, mem), base_address));
                continue;
            }
        }
        else
        {
            entry_id = offset;
            index_id = 0;
        }

        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(hit_list, SOC_MEM_NAME(unit, mem), mem, &temp));

        hit_entry = temp;
        hit_entry->opcode = opcode;
        hit_entry->offset = offset;
        hit_entry->address = soc_mem_addr_get(unit, mem, index_id, block_id, entry_id, &acc_type);
        hit_entry->cmic_block_id = cmic_block_id;
        sal_strncpy(hit_entry->block_n, SOC_BLOCK_NAME(unit, block_id), RHNAME_MAX_SIZE - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/adapter/adapter_reg_access.h>
#define ADAPTER_HIT_INFO_SIZE 17

static int
shr_access_hit_exists(int unit, uint32 base_address, int cmic_block_id, int offset, uint8 opcode, rhlist_t *hit_list)
{
    shr_hit_entry_t *hit_entry;
    RHITERATOR(hit_entry, hit_list)
    {
        if((hit_entry->address == base_address) && (hit_entry->offset == offset)
                && (hit_entry->cmic_block_id == cmic_block_id) && (hit_entry->opcode == opcode))
        {
            hit_entry->count++;
            return TRUE;
        }
    }
    return FALSE;
}

char *shr_access_hit_opcode_name(int opcode)
{
    switch (opcode )
    {
    case UPDATE_TO_SOC_REG_READ:
        return "reg read";
    case UPDATE_TO_SOC_MEM_READ:
        return "mem read";
    case UPDATE_TO_SOC_REG_WRITE:
        return "reg write";
    case UPDATE_TO_SOC_MEM_WRITE:
        return "mem write";
    }
    return NULL;
}

shr_error_e
shr_access_hit_get(
    int unit,
    rhlist_t ** hit_list_p,
    int flags)
{
    uint8 opcode;
    int cmic_block_id;
    uint32 base_address;
    int offset;

    char *hit_data;
    uint32 hit_data_length;

    rhlist_t *hit_list = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(adapter_read_hit_bits(unit, 0, &hit_data_length, &hit_data));

    if((hit_data_length % ADAPTER_HIT_INFO_SIZE) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bad hit buffer size:%d\n", hit_data_length);
    }

    if ((hit_list = utilex_rhlist_create("hits", sizeof(shr_hit_entry_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for hits list\n");
    }

    while(hit_data_length >= ADAPTER_HIT_INFO_SIZE)
    {

        opcode = *hit_data++;

        cmic_block_id = bcm_ntohl(*((int *)hit_data));    hit_data += sizeof(int);
        base_address  = bcm_ntohl(*((uint32 *)hit_data)); hit_data += sizeof(uint32);
        offset        = bcm_ntohl(*((int *)hit_data));    hit_data += sizeof(int);

        hit_data_length -= ADAPTER_HIT_INFO_SIZE;

        
        if((flags & HIT_AVOID_REPEAT) &&
                (shr_access_hit_exists(unit, base_address, cmic_block_id, offset, opcode, hit_list) == TRUE))
            continue;

        switch(opcode)
        {
            case UPDATE_TO_SOC_REG_READ:
            case UPDATE_TO_SOC_REG_WRITE:
                SHR_IF_ERR_EXIT(shr_access_reg_get(unit, base_address, cmic_block_id, offset, opcode, hit_list));
                break;
            case UPDATE_TO_SOC_MEM_READ:
            case UPDATE_TO_SOC_MEM_WRITE:
                SHR_IF_ERR_EXIT(shr_access_mem_get(unit, base_address, cmic_block_id, offset, opcode, hit_list));
                break;
            default:
                break;
        }
    }

exit:
    if(SHR_FUNC_ERR())
    {
        if(hit_list != NULL)
            utilex_rhlist_free_all(hit_list);
    }
    else if(hit_list_p != NULL)
    {
        *hit_list_p = hit_list;
    }
    SHR_FUNC_EXIT;
}
#endif 

#if (!defined(NO_FILEIO) && !defined(STATIC_SIGNAL_TEST))

static int
shr_access_reg_init(
    int unit,
    device_t *device,
    xml_node curTop)
{
    xml_node curSubTop, cur;
    SHR_FUNC_INIT_VARS(unit);

    if ((curSubTop = dbx_xml_child_get_first(curTop, "registers")) == NULL)
    {
        
        SHR_EXIT();
    }

    RHDATA_ITERATOR(cur, curSubTop, "reg")
    {
        char reg_name[RHSTRING_MAX_SIZE];
        soc_reg_t reg;

        RHDATA_GET_LSTR_CONT(cur, "name", reg_name);

        for(reg = 0; reg < NUM_SOC_REG; reg++)
        {
            if(!sal_strcmp(SOC_REG_NAME(unit, reg), reg_name))
                break;
        }
        if(reg == NUM_SOC_REG)
        {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Register:%s does not exist\n"), reg_name));
        }
        else
        {
            int tmp;
            if(device->reg_data[reg].flags != 0)
            {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Register:%s appeared twice in auxiliary list\n"), reg_name));
            }
            RHDATA_GET_INT_DEF(cur, "no_read", tmp, 0);
            
            if(tmp == 1)
            {
                device->reg_data[reg].flags = ACC_NO_READ | ACC_NO_WB;
            }
            else
            {
                RHDATA_GET_INT_DEF(cur, "no_wb", tmp, 0);
                device->reg_data[reg].flags = ACC_NO_WB;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
shr_access_mem_init(
    int unit,
    device_t *device,
    xml_node curTop)
{
    xml_node curSubTop, cur;
    SHR_FUNC_INIT_VARS(unit);

    if ((curSubTop = dbx_xml_child_get_first(curTop, "memories")) == NULL)
    {
        
        SHR_EXIT();
    }

    RHDATA_ITERATOR(cur, curSubTop, "mem")
    {
        char mem_name[RHSTRING_MAX_SIZE];
        soc_mem_t mem;

        RHDATA_GET_LSTR_CONT(cur, "name", mem_name);

        for(mem = 0; mem < NUM_SOC_MEM; mem++)
        {
            if(!sal_strcmp(SOC_MEM_NAME(unit, mem), mem_name))
                break;
        }
        if(mem == NUM_SOC_MEM)
        {
            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Memory:%s does not exist\n"), mem_name));
        }
        else
        {
            int tmp;
            if(device->mem_data[mem].flags)
            {
                LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(0, "Memory:%s appeared twice in auxilary list\n"), mem_name));
            }
            RHDATA_GET_INT_DEF(cur, "no_read", tmp, 0);
            
            if(tmp == 1)
            {
                device->mem_data[mem].flags = ACC_NO_READ | ACC_NO_WB;
            }
            else
            {
                RHDATA_GET_INT_DEF(cur, "no_wb", tmp, 0);
                device->mem_data[mem].flags = ACC_NO_WB;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static int
shr_access_aux_init(
    int unit,
    device_t *device)
{
    int res = _SHR_E_NONE;
    xml_node curTop = NULL;

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        res = dnx_access_aux_init(unit, device);
    }
    else 
#endif
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
    if(SOC_IS_JERICHO(unit))
    {
        soc_jer_access_aux_init(unit, device);
    }
    else        
#endif
    {
        
		if ((curTop = dbx_pre_compiled_devices_top_get(unit, "AccessObjects.xml", "top",
														CONF_OPEN_NO_ERROR_REPORT | CONF_OPEN_PER_DEVICE)) == NULL)
        {
            goto exit;
        }

        if((res = shr_access_reg_init(unit, device, curTop)) != _SHR_E_NONE)
        {
            goto exit;
        }

        if((res = shr_access_mem_init(unit, device, curTop)) != _SHR_E_NONE)
        {
            goto exit;
        }
    }

exit:
    dbx_xml_top_close(curTop);
    return res;
}


static shr_error_e
shr_access_device_set(
    int unit,
    device_t *device)
{
    xml_node curTop, curSubTop, cur;

    char device_n[RHNAME_MAX_SIZE];
    char *chip_n = (char *) soc_dev_name(unit);
    char *driver_n = SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type);
    char *family_n = SOC_CHIP_STRING(unit);

    SHR_FUNC_INIT_VARS(unit);

	if ((curTop = dbx_pre_compiled_common_top_get(unit, "DNX-Devices.xml", "top", 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No DNX-Devices.xml in DB on %s init\n", chip_n);
    }

    if ((curSubTop = dbx_xml_child_get_first(curTop, "devices")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No devices in DNX-Devices.xml in DB on %s init\n", chip_n);
    }

    
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        if(!sal_strcasecmp(chip_n, device_n))
        {
            
            RHDATA_GET_INT_DEF(cur, "pp", device->pp_present, 1);
            RHDATA_GET_STR_STOP(cur, "db_name", device->db_name);
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", device->base_db_name);
            SHR_EXIT();
        }
    }
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        if(!sal_strcasecmp(driver_n, device_n))
        {
            
            RHDATA_GET_INT_DEF(cur, "pp", device->pp_present, 1);
            RHDATA_GET_STR_STOP(cur, "db_name", device->db_name);
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", device->base_db_name);
            SHR_EXIT();
        }
    }
    RHDATA_ITERATOR(cur, curSubTop, "device")
    {
        RHDATA_GET_STR_CONT(cur, "chip", device_n);
        if(!sal_strcasecmp(family_n, device_n))
        {
            
            RHDATA_GET_INT_DEF(cur, "pp", device->pp_present, 1);
            RHDATA_GET_STR_STOP(cur, "db_name", device->db_name);
            RHDATA_GET_STR_DEF_NULL(cur, "base_db_name", device->base_db_name);
            SHR_EXIT();
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}


#endif 

device_t *
sand_signal_device_get(
    int unit)
{
    if(sand_device_array[unit] == NULL) {
        LOG_WARN(BSL_LS_SOC_COMMON,
              (BSL_META_U(0, "Signal Data for %s(%s) was not initialized\n"),
                                          SOC_CHIP_NAME(SOC_CONTROL(unit)->chip_driver->type), SOC_CHIP_STRING(unit)));
    }
    return sand_device_array[unit];
}

shr_error_e
shr_access_device_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#if (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST))
    if((sand_device_array[unit] = sand_signal_static_init(unit)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Static Signals init failed");
    }
#else
    
    if(sand_device_array[unit] != NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INIT,  "Auxiliary Access already initialized for:%s\n", (char *) soc_dev_name(unit));
    }
    
    {
        char sdk_location[RHFILE_MAX_SIZE];
        shr_access_get_sdk(sdk_location);
    }

    sand_device_array[unit] = sal_alloc(sizeof(device_t), "Auxiliary Access");
    sal_memset(sand_device_array[unit], 0, sizeof(device_t));

    
    SHR_IF_ERR_EXIT(shr_access_device_set(unit, sand_device_array[unit]));
    
    shr_access_aux_init(unit, sand_device_array[unit]);

    if(sand_device_array[unit]->pp_present == TRUE)
    {
        
        SHR_IF_ERR_EXIT(sand_signal_init(unit, sand_device_array[unit]));
    }
#endif 
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
shr_access_device_deinit(
    int unit)
{
#if (defined(NO_FILEIO) || defined(STATIC_SIGNAL_TEST))
    
#else

    if (sand_device_array[unit] == NULL)
    { 
        goto exit;
    }

    if(sand_device_array[unit]->pp_present == TRUE)
    {
        sand_signal_deinit(unit, sand_device_array[unit]);
    }
    sal_free(sand_device_array[unit]);
    sand_device_array[unit] = NULL;

exit:
#endif 
    return _SHR_E_NONE;
}
