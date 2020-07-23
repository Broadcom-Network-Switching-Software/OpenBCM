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

/*
 *  Defines
 */

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
int phymod_raw_iblk_read(const phymod_access_t *pa, uint32_t reg_addr, uint32_t *data)
{
    int ioerr = 0;
    uint32_t reg_val = 0;
    int32_t dev_add = ((reg_addr>>16) & 0x1f);

    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }
    if (dev_add == 0) {
        ioerr = PHYMOD_BUS_READ(pa, ((1<<16) | reg_addr), &reg_val);
    } else {
        ioerr = PHYMOD_BUS_READ(pa, reg_addr, &reg_val);
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
int phymod_raw_iblk_write(const phymod_access_t *pa, uint32_t addr, uint32_t data)
{

    int ioerr = 0;
    int32_t dev_add = ((addr>>16) & 0x1f);
    if (pa == NULL) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("NULL parameter")));
    }
    data &= 0xffff;
    if (dev_add == 0) {
        ioerr = PHYMOD_BUS_WRITE(pa, ((1<<16) | addr), data);
    } else {
        ioerr = PHYMOD_BUS_WRITE(pa, addr, data);
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
int quadra28_reg_modify(const phymod_access_t *pa,
                     uint32_t addr,
                     uint16_t reg_data,
                     uint16_t reg_mask)
{

    int ioerr = 0;
	uint16_t tmp, otmp;
	uint32_t reg_val;
   
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
