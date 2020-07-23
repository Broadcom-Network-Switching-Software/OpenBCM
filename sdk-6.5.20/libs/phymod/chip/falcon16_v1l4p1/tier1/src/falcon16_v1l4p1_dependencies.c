/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/

#ifdef LINUX
/* #include <stdint.h> */
#endif

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/acc/phymod_tsc_iblk.h>
#include "../include/falcon16_v1l4p1_dependencies.h"
#include "../include/common/srds_api_err_code.h"
#include "../include/falcon16_v1l4p1_common.h"

err_code_t falcon16_v1l4p1_pmd_mwr_reg(srds_access_t *sa__, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val)
{
    uint32_t mymask = ( uint32_t) mask;
    phymod_access_t sa_copy;
    uint32_t i;
    uint32_t error_code;

    uint32_t data = ((mymask << 16) & 0xffff0000) | val << lsb;

    error_code=0;
    PHYMOD_MEMCPY(&sa_copy, sa__, sizeof(srds_access_t));
    for(i=1; i <= 0x8; i = i << 1) {
        if ( i & sa__->lane_mask ) {
            sa_copy.lane_mask = i;
            error_code+=phymod_tsc_iblk_write(&sa_copy, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) addr), data);
        }
    }
    if(error_code) {
        return  ERR_CODE_DATA_NOTAVAIL;
    }

    return  ERR_CODE_NONE;
}

uint8_t falcon16_v1l4p1_get_lane(srds_access_t *sa__)
{
    if (sa__->lane_mask & 0x1) {
        return (ERR_CODE_NONE);
    } else if (sa__->lane_mask & 0x2) {
        return ( 1 );
    } else if (sa__->lane_mask & 0x4) {
        return ( 2 );
    } else if (sa__->lane_mask & 0x8) {
        return ( 3 );
    } else {
        return ERR_CODE_NONE;
    }
}

uint16_t falcon16_v1l4p1_pmd_rd_reg(srds_access_t *sa__, uint16_t address)
{
    uint32_t data;

    phymod_tsc_iblk_read(sa__, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), &data);
    data = data & 0xffff;

    return ( (uint16_t)data );
}

err_code_t falcon16_v1l4p1_delay_ns(uint16_t delay_ns)
{
    uint32_t delay;

    delay = delay_ns / 1000; 
    if (!delay ) {
        delay = 1;
    }
    PHYMOD_USLEEP(delay);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_delay_ms(uint32_t delay_ms)
{
    uint32_t delay;

    delay = delay_ms * 1000;
    if (!delay ) {
        delay = 1;
    }
    PHYMOD_USLEEP(delay);

    return ERR_CODE_NONE;
}

/**
@brief   Falcon PMD Write
@param   phymod access handle to current Falcon Context
@param   address
@param   val
@returns The ERR_CODE_NONE upon successful completion, else returns ERR_CODE_DATA_NOTAVAIL
*/
err_code_t falcon16_v1l4p1_pmd_wr_reg(srds_access_t *sa__, uint16_t address, uint16_t val)
{
    uint32_t data = 0xffff & val;
    uint32_t error_code;

    error_code = phymod_tsc_iblk_write(sa__, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), data);
    if(error_code) {
        return  ERR_CODE_DATA_NOTAVAIL;
    }

    return  ERR_CODE_NONE; 
}

err_code_t falcon16_v1l4p1_delay_us(uint32_t delay_us)
{
    PHYMOD_USLEEP(delay_us);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val)
{
    uint32_t data;

    phymod_tsc_iblk_read(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), &data);
    data = data & 0xffff; 
    *val = (uint16_t)data;

    return ERR_CODE_NONE;
}

uint8_t falcon16_v1l4p1_get_core(srds_access_t *sa__)
{
    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_uc_lane_idx_to_system_id(srds_access_t *sa__, char string[16], uint8_t uc_lane_idx)
{
    static char info[256];

    /* Indicates Falcon Core */
    PHYMOD_SPRINTF(info, "%s_%d", "FC_", uc_lane_idx);
    PHYMOD_STRNCPY(string,info, PHYMOD_STRLEN(info)+1);

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_pmd_wr_pram(srds_access_t *sa__, uint8_t val)
{

    return ERR_CODE_NONE;
}

err_code_t falcon16_v1l4p1_set_lane(srds_access_t *sa__, uint8_t lane_index){

    return ERR_CODE_NONE;

}



