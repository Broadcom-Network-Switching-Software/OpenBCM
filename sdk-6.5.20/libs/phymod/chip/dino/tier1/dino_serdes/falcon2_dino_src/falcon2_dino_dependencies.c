/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 */

/***************************************************************************
****************************************************************************
*  File Name     :  falcon_furia_dependencies.c                            *
*  Created On    :  05/29/2014                                             *
*  Created By    :  Sharad Sharma                                          *
*  Description   :  Version of Dependency APIs used in Eval                *
*  Revision      :  $Id: $                                                 *
*                                                                          *
*  Copyright 2013  Broadcom Corporation                                    *
*  All Rights Reserved                                                     *
*  No portions of this material may be reproduced in any form without      *
*  the written permission of:                                              *
*      Broadcom Corporation                                                *
*      5300 California Avenue                                              *
*      Irvine, CA  92617                                                   *
*                                                                          *
*  All information contained in this document is Broadcom Corporation      *
*  company private proprietary, and trade secret.                          *
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include "falcon2_dino_interface.h"
#include "falcon2_dino_dependencies.h"
#include "../common/srds_api_err_code.h"
#include "../../bcmi_dino_defs.h"
#include "../../dino_cfg_seq.h"


err_code_t falcon2_dino_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data = 0;

    READ_DINO_PMA_PMD_REG(pa, (uint32_t)address, data);
    *val = (uint16_t)data;

    return ( ERR_CODE_NONE );
}

err_code_t falcon2_dino_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val) {
    uint32_t data = 0xffff & val;

    WRITE_DINO_PMA_PMD_REG(pa, (uint32_t) address, data);

    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    uint16_t tmp, otmp;
    uint32_t reg_val;
    val = val << lsb;
    
    val = val & mask ;
    /* Use clause 45 access if supported */
    READ_DINO_PMA_PMD_REG(pa, addr, reg_val);
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        WRITE_DINO_PMA_PMD_REG(pa, addr, tmp);
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_dino_delay_ns(uint16_t delay_ns) {
    if (delay_ns > 1000) {
        return (ERR_CODE_SERDES_DELAY);
    }

    return (ERR_CODE_NONE);
} 


err_code_t falcon2_dino_delay_us(uint32_t delay_us) {
    PHYMOD_USLEEP(delay_us);
    return (ERR_CODE_NONE);
}

uint8_t falcon2_dino_get_lane(const phymod_access_t *pa) {
    uint8_t lane_mask = 0, lane = 0; 
    uint32_t reg_val = 0;
    
    READ_DINO_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);

    /*lane from 0-3*/
    lane_mask = (reg_val & 0xF);

    switch (lane_mask) {
        case 1:
            lane = 0;
        break;
        case 2:
            lane = 1;
        break;
        case 4:
            lane = 2;
        break;
        case 8:
            lane = 3;
        break;
        default:
        break;
    }

    return (lane);
}

uint8_t falcon2_dino_get_core(void)
{
    return 0;
}

err_code_t falcon2_dino_uc_lane_idx_to_system_id(char string[16], uint8_t uc_lane_idx)
{
    switch (uc_lane_idx) {
        case 0:
            string = "ZERO";
        break;
        case 1:
            string = "ONE";
        break;
        case 2:
            string = "TWO";
        break;
        case 3:
            string = "THREE";
        break;
        case 4:
            string = "FOUR";
        break;
        case 5:
            string = "FIVE";
        break;
        case 6:
            string = "SIX";
        break;
        case 7:
            string = "SEVEN";
        break;
        case 8:
            string = "EIGHT";
        break;
        case 9:
            string = "NINE";
        break;
        case 10:
            string = "TEN";
        break;
        case 11:
            string = "ELEVEN";
        break;
        default:
            string = "ZERO";
        break;
    }
    USR_UNUSED_VAR(string);

    return(0);
}

