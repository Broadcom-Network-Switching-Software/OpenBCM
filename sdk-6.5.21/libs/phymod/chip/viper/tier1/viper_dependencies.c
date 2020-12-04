/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/  

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include "viper_common.h"
#include "viper_err_code.h"
#include <phymod/acc/phymod_tsc_iblk.h>

err_code_t viper_delay_us(uint32_t delay_us){
	PHYMOD_USLEEP(delay_us);
	return ( 0 );
}

err_code_t viper_delay_ns(uint16_t delay_ns) {
	uint32_t delay;
	delay = delay_ns / 1000; 
	if (!delay ) {
		delay = 1;
	}
	PHYMOD_USLEEP(delay);
	return ( 0 );
}

uint8_t viper_get_lane(const phymod_access_t *pa) {
    if (pa->lane_mask == 0x1) {
	    return ( 0 );
    } else if (pa->lane_mask == 0x2) {
	    return ( 1 );
    } else if (pa->lane_mask == 0x4) {
	    return ( 2 );
    } else if (pa->lane_mask == 0x8) {
	    return ( 3 );
    } else {
	    return ( 0 );
    }
}

void viper_pmd_mwr_reg (PHYMOD_ST *pa, uint16_t address, 
                        uint16_t mask, uint8_t lsb, uint16_t val) {

    PHYMOD_ST pa_copy;	
    uint32_t  mymask  = (uint32_t)mask;
    uint32_t  i, data = ((mymask << 16) & 0xffff0000) | val << lsb;

    PHYMOD_MEMCPY((void*)&pa_copy, (void*)pa, sizeof(phymod_access_t));
    for(i=1; i <= 0x8; i = i << 1) {
        if ( i & pa->lane_mask ) {
            pa_copy.lane_mask = i;
            phymod_tsc_iblk_write(&pa_copy, 
                   (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | (uint32_t) address), data);
        }
    }
}


uint16_t viper_pmd_rd_reg(PHYMOD_ST *pa, uint16_t address){

	uint32_t data;
	phymod_tsc_iblk_read(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 
                              | (uint32_t) address), &data);
	data = data & 0xffff; 
	return ( (uint16_t)data);
}


err_code_t viper_pmd_rdt_reg(PHYMOD_ST *pa, uint16_t address, uint16_t *val) {
	uint32_t data;
	phymod_tsc_iblk_read(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 | 
                                        (uint32_t) address), &data);
	data = data & 0xffff; 
	*val = (uint16_t)data;
	return ( 0 );
}

void viper_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val){
	uint32_t data = 0xffff & val;
	phymod_tsc_iblk_write(pa, (PHYMOD_REG_ACC_TSC_IBLK | 0x10000 
                                     | (uint32_t) address), data);
}
