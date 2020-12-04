/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>

#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "../../../phy8481.h"
#include <soc/phyreg.h>
#include <soc/phy.h>
#include "../../../phyreg.h"

#define  KOI_MERLIN_DEPENDENCIES

#include "merlin_koi_dependencies.h"

/* Change XGP table to point to XFI */
#define XGP_POINT_TO_XFI(unit, pc)\
    do {\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x2004));\
    } while(0)

/* Restore XGP table*/
#define RESTORE_XGP(unit, pc)\
    do {\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x3));\
    } while(0)

#define WRITE_KOI_PMAD_REG(unit, pc, addr, val)\
    do {\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x2004));\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_PMAD_REG(unit, pc, addr, val));\
            SOC_IF_ERROR_RETURN\
                (WRITE_PHY8481_TOPLVL1_REG(unit, pc, 0x4110, 0x3));\
    } while(0)

static phy_ctrl_t *pc_int = NULL;

void set_pc(phy_ctrl_t *pc) {
    pc_int = pc;
}

err_code_t merlin_koi_pmd_mwr_reg(uint16_t addr, uint16_t mask, uint8_t lsb, uint16_t val) {

    uint16_t tmp, otmp;
    uint16_t reg_val;
    val = val << lsb;
    val = val & mask ;
 
    XGP_POINT_TO_XFI(pc_int->unit, pc_int);
    SOC_IF_ERROR_RETURN
        (READ_PHY8481_PMAD_REG(pc_int->unit, pc_int, addr, &reg_val)); 
    RESTORE_XGP(pc_int->unit, pc_int);

    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
          WRITE_KOI_PMAD_REG(pc_int->unit, pc_int, addr, tmp);
    }

  return(0);
}

int logger_write(int message_verbose_level, const char *format, ...) {

	return ( 0 );
}

err_code_t merlin_koi_pmd_rdt_reg(uint16_t address, uint16_t *data){
   
   if(pc_int) { 
        uint16_t val=0;
        XGP_POINT_TO_XFI(pc_int->unit, pc_int);
        SOC_IF_ERROR_RETURN
            (READ_PHY8481_PMAD_REG(pc_int->unit, pc_int, address, &val));        
        RESTORE_XGP(pc_int->unit, pc_int);
        *data = (uint16_t)val;
   }
    return ( 0 );
}

err_code_t merlin_koi_pmd_wr_reg(uint16_t address, uint16_t val){
   
    if(pc_int) {
        WRITE_KOI_PMAD_REG(pc_int->unit, pc_int, address, val);    
    }
 
    return(0);
}

err_code_t merlin_koi_delay_us(uint32_t delay_us){

    sal_usleep(delay_us);
    return ( 0 );
}

err_code_t merlin_koi_delay_ns(uint16_t delay_ns) {

    return ( 0 );
}

uint8_t merlin_koi_get_lane(void) {
    return ( 0 );
}

uint8_t merlin_koi_get_core(void) {
	return ( 0 );
}

err_code_t merlin_koi_uc_lane_idx_to_system_id(char string[16], uint8_t uc_lane_idx) {
     return(0);
}
