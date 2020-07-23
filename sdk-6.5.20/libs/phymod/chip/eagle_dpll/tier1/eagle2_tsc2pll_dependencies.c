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
#include "common/srds_api_err_code.h"
#include "eagle2_tsc2pll_common.h"
#include <phymod/acc/phymod_tsc_iblk.h>
#include "eagle2_tsc2pll_dependencies.h"


err_code_t eagle2_tsc2pll_delay_us(uint32_t delay_us){
    PHYMOD_USLEEP(delay_us);
    return ( 0 );
}

err_code_t eagle2_tsc2pll_delay_ns(uint16_t delay_ns) {
    uint32_t delay;
    delay = delay_ns / 1000;
    if (!delay ) {
        delay = 1;
    }
    PHYMOD_USLEEP(delay);
    return ( 0 );
}

uint8_t eagle2_tsc2pll_get_lane(const phymod_access_t *pa) {
    if (pa->lane_mask == 0x1) {
       return ( 0 );
    } else if (pa->lane_mask == 0x2) {
       return ( 1 );
    } else if (pa->lane_mask == 0x4) {
       return ( 2 );
    } else if (pa->lane_mask == 0x8) {
       return ( 3 );
    } else if (pa->lane_mask == 0xc) {
       return ( 2 );
    } else {
       return ( 0 );
    }
}

/**
@brief   Eagle PMD Modify-Write
@param   phymod access handle to current Eagle Context
@param   address
@param   mask
@param   lsb
@param   val
@returns The ERR_CODE_NONE upon successful completion, else returns ERR_CODE_DATA_NOTAVAIL
*/
err_code_t eagle2_tsc2pll_pmd_mwr_reg(const phymod_access_t *pa, uint16_t address, uint16_t mask, uint8_t lsb, uint16_t val) {

    uint32_t mymask = ( uint32_t) mask;
    phymod_access_t pa_copy;
    uint32_t i;
    int error_code;

    uint32_t data = ((mymask << 16) & 0xffff0000) | val << lsb;

    error_code = 0;
    PHYMOD_MEMCPY(&pa_copy, pa, sizeof(phymod_access_t));
    for(i=1; i <= 0x8; i = i << 1) {
        if ( i & pa->lane_mask ) {
            pa_copy.lane_mask = i;
            error_code+=phymod_tsc_iblk_write(&pa_copy, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), data);
        }
    }
    if(error_code)
      return  ERR_CODE_DATA_NOTAVAIL;
    return  ERR_CODE_NONE;
}


/* phymod_tsc_iblk_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data) */
uint16_t eagle2_tsc2pll_pmd_rd_reg(const phymod_access_t *pa, uint16_t address){

    uint32_t data;
    phymod_tsc_iblk_read(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), &data);
    data = data & 0xffff;
    return ( (uint16_t)data );
}


err_code_t eagle2_tsc2pll_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data;
    phymod_tsc_iblk_read(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), &data);
    data = data & 0xffff;
    *val = (uint16_t)data;
    return ( 0 );
}

/**
@brief   Eagle PMD Write
@param   phymod access handle to current Eagle Context
@param   address
@param   val
@returns The ERR_CODE_NONE upon successful completion, else returns ERR_CODE_DATA_NOTAVAIL
*/
err_code_t eagle2_tsc2pll_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val){
    uint32_t data = 0xffff & val;
    uint32_t error_code;

    error_code = phymod_tsc_iblk_write(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), data);
    if(error_code)
      return  ERR_CODE_DATA_NOTAVAIL;
    return  ERR_CODE_NONE;
}

uint8_t eagle2_tsc2pll_get_core(void) {
    return(0);
}

err_code_t eagle2_tsc2pll_uc_lane_idx_to_system_id(char *string , uint8_t uc_lane_idx) {
    static char info[256];
   /* Indicates Eagle Core */
    PHYMOD_SPRINTF(info, "%s_%d", "FC_", uc_lane_idx);
    /* PHYMOD_STRNCPY(string,info, PHYMOD_STRLEN(info)); */
    PHYMOD_STRCPY(string,info );
    return ERR_CODE_NONE;
}


