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
#include "falcon2_madura_interface.h"
#include "falcon2_madura_dependencies.h"
#include "../common2/srds_api_err_code.h"
#include "../../bcmi_madura_defs.h"
#include "../../madura_cfg_seq.h"
/*
#include "falcon2_madura_ram_regs.h"
#include "falcon2_madura_core_lane_access.h"
*/

err_code_t falcon2_madura_pmd_rdt_reg(const phymod_access_t *pa, uint16_t address, uint16_t *val) {
    uint32_t data = 0;

    READ_MADURA_PMA_PMD_REG(pa, (uint32_t)address, data);
    *val = (uint16_t)data;
    return ( ERR_CODE_NONE );
}

err_code_t falcon2_madura_pmd_wr_reg(const phymod_access_t *pa, uint16_t address, uint16_t val) {
    uint32_t data = 0xffff & val;

    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) address, data);
    return(ERR_CODE_NONE);
}

err_code_t falcon2_madura_pmd_mwr_reg(const phymod_access_t *pa, uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {
    uint16_t tmp, otmp;
    uint32_t reg_val;
    val = val << lsb;
    
    val = val & mask ;
    /* Use clause 45 access if supported */
    READ_MADURA_PMA_PMD_REG(pa, addr, reg_val);
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        WRITE_MADURA_PMA_PMD_REG(pa, addr, tmp);
    }
    return(ERR_CODE_NONE);
}

err_code_t falcon2_madura_delay_ns(uint16_t delay_ns) {

  if (delay_ns > 1000) {
    return (ERR_CODE_SERDES_DELAY);
  }
  return (ERR_CODE_NONE);
} 


err_code_t falcon2_madura_delay_us(uint32_t delay_us) {
  PHYMOD_USLEEP(delay_us);
  return (ERR_CODE_NONE);
}

uint8_t falcon2_madura_get_lane(const phymod_access_t *pa) {
    uint8_t i = 0 , lane = -1 ; 
    uint32_t reg_val,val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);
    val = reg_val;
    val = val & 0xF; /*lane from 0-3*/
    for(i=0;i<4;i++)
    {
         if((1<<i)& val)/*considering that only one lane is selected*/
	 {
	      lane=i;
  	      break;
	 }
    } 

    return (lane);
}
err_code_t falcon2_madura_set_lane(const phymod_access_t *pa, uint8_t lane_index)
{
    uint32_t reg_val;
    
    if (lane_index > 3) {
        return  ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }

    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, reg_val);
    reg_val &= 0xFFF0; /*lane from 0-3*/
    reg_val |= (1<< lane_index);

    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) DEV1_SLICE_SLICE_ADR, reg_val);
  return(0);
}

uint8_t falcon2_madura_get_core(const phymod_access_t *pa)
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);
    val = (val >> 8) & 0xF; 

    if( (val & 0x3) == 0x3 ||  (val & 0x3) == 0)
    { 
        /* both cores or none */
	  return ERR_CODE_INVALID_RAM_ADDR;
    }
    if( (val & 0xC) == 0xC ||  (val & 0xC) == 0)
    { 
        /* both sys/line or none */
	  return ERR_CODE_INVALID_RAM_ADDR;
    }
    if( (val & 0x6) == 0x6 )
    { 
        /* both sys core 1  */
	  return ERR_CODE_INVALID_RAM_ADDR;
    }

    switch (val) {
        case 0x5:
            return 0; /* sys core 0 */
        case 0x9:
            return 2; /* line core 0 */
        case 0xA:
            return 3; /* line core 1 */
    }
 return 0;
}

err_code_t falcon2_madura_uc_lane_idx_to_system_id(const phymod_access_t *pa, char string[16], uint8_t uc_lane_idx)
{
  string[0]= '\0';
  return(0);
}

uint8_t falcon2_madura_get_pll(const phymod_access_t *pa)
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);

    return (val & 0x1000) ? 1 : 0;
}

err_code_t falcon2_madura_set_pll(const phymod_access_t *pa, uint8_t pll_index) 
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);

    val  |=   (0x1000) ;

    if (pll_index == 0) {
        val ^= 0x1000;
    }

    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) DEV1_SLICE_SLICE_ADR, val);

    return(0);
}

err_code_t falcon2_madura_set_core(const phymod_access_t *pa, uint8_t core_id)
{
    uint32_t val;
    
    READ_MADURA_PMA_PMD_REG(pa, DEV1_SLICE_SLICE_ADR, val);
    val &=  0xF0FF;  /* clear core / side slection */

    switch (core_id) {
        case 0x0:
            val |= 0x0500; /* sys core 0 */
            break;
        case 0x2:
            val |= 0x0900; /* line core 0 */
            break;
        case 0x3:
            val |= 0x0A00; /* line core 1 */
            break;
        default:
            return  ERR_CODE_BAD_PTR_OR_INVALID_INPUT;
    }
    WRITE_MADURA_PMA_PMD_REG(pa, (uint32_t) DEV1_SLICE_SLICE_ADR, val);
    return 0;
}

