/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *         
 * This file present an example of replacing the registers\memories access functions.
 * It assumes using the portmod register DB bcm2801pb_a0
 */

#include <shared/bsl.h>
#include <soc/register.h>
#include <soc/mcm/driver.h>
#include <bcm/error.h>

typedef struct portmod_port_access_info_s {
    int block_id;
    int inner_index;
} portmod_port_access_info_t;

static portmod_port_access_info_t *port2info[SOC_MAX_NUM_DEVICES] = {0};

#define PORTMOD_NO_DEVICE

#ifdef PORTMOD_NO_DEVICE
STATIC int 
portmod_memreg_access(int unit, int cmic_block, uint32 addr, uint32 dwc_read, int is_mem, int is_get, uint32 *data) 
{
    if(is_get) {
        LOG_INFO(BSL_LS_APPL_PORT,
                 (BSL_META_U(unit,
                             "Read operation: block[%d] addr[0x%x] dwc_read[%d] is_mem[%d]\n"), 
                                cmic_block, addr, dwc_read, is_mem));
    } else {
        LOG_INFO(BSL_LS_APPL_PORT,
                 (BSL_META_U(unit,
                             "Write operation: block[%d] addr[0x%x] dwc_read[%d] is_mem[%d] data[0x%x]\n"), 
                                cmic_block, addr, dwc_read, is_mem, data[0]));
    }

    return 0;
}
#else /*PORTMOD_NO_DEVICE*/
STATIC int 
portmod_memreg_access(int unit, int cmic_block, uint32 addr, uint32 dwc_read, int is_mem, int is_get, uint32 *data) 
{
    if(is_get) {
        return soc_direct_memreg_get(unit, blk_id, addr, reg_size, 0, data);
    } else {
        return soc_direct_memreg_set(unit, blk_id, addr, reg_size, 0, data);
    }
}

#endif /*PORTMOD_NO_DEVICE*/

STATIC int 
portmod_reg_address_get(int unit, soc_reg_t reg, int port, uint32* addr)
{
    soc_driver_t* d = &soc_driver_bcm2801pm_a0;
    soc_reg_info_t* reg_info = d->reg_info[reg];

    (*addr) = reg_info->offset;

    if(reg_info->regtype == soc_portreg) {
       
        /* From block type and port user should find the register offset
           The example here should be adjust per user architecture */

        (*addr) += port2info[unit][port].inner_index;
    }

    return BCM_E_NONE;

}


STATIC int
portmod_reg_access_handle(int unit, soc_reg_t reg, int port, int index, int is_get, soc_reg_above_64_val_t data)
{
    uint32 addr, reg_size;
    int rv;
    soc_driver_t* d = &soc_driver_bcm2801pm_a0;
    soc_reg_info_t* reg_info;
    int blk_id, schan_id;

    if(reg >= NUM_SOC_REG) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "invalid register")));
        return BCM_E_INTERNAL;
    }

    reg_info = d->reg_info[reg];

    if(!reg_info) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "invalid register")));
        return BCM_E_INTERNAL;
    }

    if(!port2info[unit]) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "unit isn't initialized")));
        return BCM_E_UNIT;
    }

    rv = portmod_reg_address_get(unit, reg, port, &addr);
    if(rv < 0) {
        return rv;
    }

    /* CMIC functionality - should be replaced by user access*/
    if (reg_info->flags & SOC_REG_FLAG_ABOVE_64_BITS) {
        reg_size = d->reg_above_64_info[reg]->size;
    } else if (reg_info->flags & SOC_REG_FLAG_64_BITS) {
        reg_size = 2;
    } else {
        reg_size = 1;
    }

    if(port & SOC_REG_ADDR_SCHAN_ID_MASK) {
        schan_id = port & (~SOC_REG_ADDR_SCHAN_ID_MASK);
    } else if(port & SOC_REG_ADDR_BLOCK_ID_MASK) {
        blk_id = port & (~SOC_REG_ADDR_BLOCK_ID_MASK);
        schan_id = d->block_info[blk_id].schan;
    } else {
        schan_id = port2info[unit][port].block_id;
    }

    return portmod_memreg_access(unit, schan_id, addr, reg_size, 0, is_get, data);
}

STATIC int
portmod_mem_access_handle(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, int is_read, void *entry_data)
{
    uint32 mem_size, addr, schan_block;
    soc_driver_t* d = &soc_driver_bcm2801pm_a0;
    soc_mem_info_t* mem_info;

    if(mem >= NUM_SOC_REG) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "invalid memory")));
        return BCM_E_INTERNAL;
    }

    mem_info = d->mem_info[mem];

    if(!mem_info) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "invalid memory")));
        return BCM_E_INTERNAL;
    }

    if(!port2info[unit]) {
        LOG_ERROR(BSL_LS_SOC_PHYMOD,
                  (BSL_META_U(unit,
                              "unit isn't initialized")));
        return BCM_E_UNIT;
    }

    addr = mem_info->base + index;
    mem_size = (mem_info->bytes + 3) / 4;

    schan_block = d->block_info[copyno].schan;

    return portmod_memreg_access(unit, schan_block, addr, mem_size, 1, is_read, (uint32*)entry_data);
}

STATIC int 
portmod_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32* data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;

    rv = portmod_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if(rv<0) {
        return rv;
    }

    data[0] = data_a64[0];

    return BCM_E_NONE;
}

STATIC int 
portmod_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64* data)
{
    soc_reg_above_64_val_t data_a64;
    int rv;

    rv = portmod_reg_access_handle(unit, reg, port, index, 1, data_a64);
    if(rv<0) {
        return rv;
    }

    COMPILER_64_SET(*data, data_a64[1], data_a64[0]);

    return BCM_E_NONE;
}

STATIC int 
portmod_reg_above64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    return portmod_reg_access_handle(unit, reg, port, index, 1, data);
}

STATIC int 
portmod_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data)
{
    soc_reg_above_64_val_t data_a64;

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[0] = data;

    return portmod_reg_access_handle(unit, reg, port, index, 0, data_a64);
}

STATIC int 
portmod_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    soc_reg_above_64_val_t data_a64;

    SOC_REG_ABOVE_64_CLEAR(data_a64);
    data_a64[1] = COMPILER_64_HI(data);
    data_a64[0] = COMPILER_64_LO(data);

    return portmod_reg_access_handle(unit, reg, port, index, 0, data_a64);
}

STATIC int 
portmod_reg_above64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    return portmod_reg_access_handle(unit, reg, port, index, 0, data);
}

STATIC int 
portmod_mem_array_read(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, void *entry_data)
{
    return portmod_mem_access_handle(unit, mem, array_index, copyno, index, 1, entry_data);
}

STATIC int 
portmod_mem_array_write(int unit, soc_mem_t mem, unsigned int array_index, int copyno, int index, void *entry_data)
{
    return portmod_mem_access_handle(unit, mem, array_index, copyno, index, 0, entry_data);
}

soc_reg_access_t portmod_access =  {
    portmod_reg32_get,
    portmod_reg64_get,
    portmod_reg_above64_get,

    portmod_reg32_set,
    portmod_reg64_set,
    portmod_reg_above64_set,

    portmod_mem_array_read,
    portmod_mem_array_write

};

int
portmod_reg_access_init(int unit)
{
    int rv;
    soc_driver_t* d = &soc_driver_bcm2801pm_a0;

    if(port2info[unit]) {
        return BCM_E_EXISTS;
    }

    /* Required for field operations */
    SOC_DRIVER(unit) = d;

    port2info[unit] = sal_alloc(sizeof(portmod_port_access_info_t) * SOC_MAX_NUM_PORTS, "portmod reg access");
    sal_memset(port2info[unit], 0, sizeof(portmod_port_access_info_t) * SOC_MAX_NUM_PORTS);

    if(!port2info[unit]) {
        return BCM_E_MEMORY;
    }

    rv = soc_reg_access_func_register(unit, &portmod_access);
    if(rv < 0) {
        return rv;
    }

    return BCM_E_NONE;
}

int
portmod_reg_access_deinit(int unit)
{
    if(!port2info[unit]) {
        return BCM_E_INIT;
    }

    sal_free(port2info[unit]);
    port2info[unit] = NULL;

    return BCM_E_NONE;
}

int
portmod_reg_access_port_bindex_set(int unit, int port, int block_index, int internal_index)
{
    if(!port2info[unit]) {
        return BCM_E_INIT;
    }

    port2info[unit][port].inner_index = internal_index;
    port2info[unit][port].block_id = block_index; 

    return BCM_E_NONE;
}


