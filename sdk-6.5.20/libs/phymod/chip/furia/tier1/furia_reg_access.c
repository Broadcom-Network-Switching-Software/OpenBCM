/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

/* 
 * Includes
 */
#include <phymod/phymod_acc.h>
#include <phymod/phymod_reg.h>
#include "furia_reg_access.h"
#include "furia_regs_structs.h"
#include "furia_address_defines.h"

/*
 *  Defines
 */
#define FORCE_CL45      0x20


/**   Read Register 
 *    This function is used to read the register content of given 
 *    register address 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param addr               Register address supplied by user 
 *    @param data               Regiser content/value of the given register
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error 
 */
int furia_reg_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t reg_val;
    uint32_t devad = (addr >> 16) & 0x3f;

    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }

    if((devad == 0)||(devad == 1)) {
        ioerr = PHYMOD_BUS_READ(pa, ((1 << 16) | addr), &reg_val);
    } else if(devad == 7){
        ioerr = PHYMOD_BUS_READ(pa, ((7 << 16) | addr), &reg_val);
    }else{
       *data=0xFFFF;
        return PHYMOD_E_NONE;
    }
    
    *data = reg_val;
    return ioerr;
}

/**   Write Register 
 *    This function is used to write the content to given register
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param addr               Register address supplied by user 
 *    @param data               Regiser content/value to be written to register 
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error 
 */
int furia_reg_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{
    int ioerr = 0;
    uint32_t devad = (addr >> 16) & 0xf;
   
    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }

    if((devad == 0)||(devad == 1)) {
        ioerr = PHYMOD_BUS_WRITE(pa, ((1 << 16) | addr), data);
    } else if(devad == 7){
        ioerr = PHYMOD_BUS_WRITE(pa, ((7 << 16) | addr), data);
    }
     

    return ioerr;
}

/**   Modify Register 
 *    This function is used to modify the content without overwriting it 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param addr               Register address supplied by user 
 *    @param reg_data           Regiser data for modified portion 
 *    @param reg_mask           Register mask for the data bits to be modified 
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error 
 */
int furia_reg_modify(const phymod_access_t *pa,
                     uint32_t addr,
                     uint16_t reg_data,
                     uint16_t reg_mask)
{
    int ioerr = 0;
    uint32_t reg_val;
    uint16_t tmp, otmp;
    
    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }

    reg_data = reg_data & reg_mask;

    /* Use clause 45 access if supported */
    ioerr += PHYMOD_BUS_READ(pa, addr, &reg_val);
        
    tmp = (uint16_t) (reg_val & 0xffff);
    otmp = tmp;
    tmp &= ~(reg_mask);
    tmp |= reg_data;

    if (otmp != tmp) {
        ioerr += PHYMOD_BUS_WRITE(pa, addr, tmp);
    }

    return ioerr;
}
/**   Set slice register 
 *    This function is used to set the slice register for indirect 
 *    accessing of register 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param intf_side          Interface side 
 *                              0 - Line side 1 - System side 
 *    @param wr_lane            Write lane information 
 *                              0xF - broadcast
 *                              0x3 - multicast01
 *                              0xC - multicast23
 *                              0x8 - ln3
 *                              0x4 - ln2 
 *                              0x2 - ln1
 *                              0x1 - ln0 
 *    @param rd_lane            Read lane information 
 *                              0x0 = ReadLane_0
 *                              0x1 = ReadLane_1
 *                              0x2 = ReadLane_2
 *                              0x3 = ReadLane_3
 *
 *    @return ioerr             Error return
 *                              0 - Success
 *                              any other value - Error 
 */
int furia_set_slice_reg(const phymod_access_t *pa,
                        uint16_t intf_side,
                        uint16_t wr_lane,
                        uint16_t rd_lane)
{
    int ioerr = 0;
    DEV1_SLICE_SLICE_t slice;

    if (pa == NULL) {
        return -1;
    }
    ioerr += READ_FURIA_PMA_PMD_REG(pa,\
                                    DEV1_SLICE_slice_Adr,\
                                    &slice.data);   
    slice.fields.sysen = intf_side;
    slice.fields.wr_lane = wr_lane;
    slice.fields.rd_lane = rd_lane;
    if(!pa->devad) {
        slice.fields.devid = 1;
    } else {
        slice.fields.devid = pa->devad;
    }
         
    ioerr += WRITE_FURIA_PMA_PMD_REG(pa,\
                                     DEV1_SLICE_slice_Adr,\
                                     slice.data);
    return ioerr;
}
int furia_set_an_slice_reg(const phymod_access_t *pa,
                        uint16_t intf_side,
                        uint16_t wr_lane,
                        uint16_t rd_lane)
{
    int ioerr = 0;
    DEV7_SLICE_SLICE_t slice;

    if (pa == NULL) {
        return -1;
    }
    ioerr += READ_FURIA_PMA_PMD_REG(pa,\
                                    DEV7_SLICE_slice_Adr,\
                                    &slice.data);   
    slice.fields.sysen = intf_side;
    slice.fields.wr_lane = wr_lane;
    slice.fields.rd_lane = rd_lane;
    slice.fields.devid = 7;
         
    ioerr += WRITE_FURIA_AN_REG(pa,\
                                DEV7_SLICE_slice_Adr,\
                                slice.data);
    return ioerr;
}
