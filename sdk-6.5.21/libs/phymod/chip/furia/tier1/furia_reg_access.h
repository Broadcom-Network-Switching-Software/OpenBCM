/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
#ifndef   __FURIA_REG_ACCESS_H__
#define   __FURIA_REG_ACCESS_H__

#include <phymod/phymod.h>

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
int furia_reg_read(const phymod_access_t *pa, uint32_t addr, uint32_t *data);

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
int furia_reg_write(const phymod_access_t *pa, uint32_t addr, uint32_t data);

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
                     uint16_t reg_mask);
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
                        uint16_t rd_lane);
/**   Set slice register
 *    This function is used to set the an slice register for indirect
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

int furia_set_an_slice_reg(const phymod_access_t *pa,
                        uint16_t intf_side,
                        uint16_t wr_lane,
                        uint16_t rd_lane);

#define PHY_FURIA_C45_DEV_PMA_PMD     0x01
#define PHY_FURIA_C45_DEV_PCS         0x03
#define PHY_FURIA_C45_DEV_AN          0x07


#define FURIA_CLAUSE45_ADDR(_devad, _regad)     \
            ((((_devad) & 0x3F) << 16) |        \
             ((_regad) & 0xFFFF))


/* WRITE To FURITA Slice Register */

#define WRITE_FURIA_SLICE_REG(_pa, _intf, _wrlane, _rdlane) \
        furia_set_slice_reg(_pa, _intf, _wrlane, _rdlane)
#define WRITE_FURIA_AN_SLICE_REG(_pa, _intf, _wrlane, _rdlane) \
        furia_set_an_slice_reg(_pa, _intf, _wrlane, _rdlane)


/* PMA/PMD Device (Dev Addr 1) */
#define READ_FURIA_PMA_PMD_REG(_pa, _reg, _val) \
        furia_reg_read(_pa,\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_PMA_PMD, (_reg)), _val)
#define WRITE_FURIA_PMA_PMD_REG(_pa, _reg,_val) \
        furia_reg_write(_pa,\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_PMA_PMD, (_reg)), _val)
#define MODIFY_FURIA_PMA_PMD_REG(_pa, _reg, _val, _mask) \
        furia_reg_modify((_pa),\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_PMA_PMD, (_reg)),\
        (_val),\
        (_mask))
/* PCS Device (Dev Addr 3) */
#define READ_FURIA_PCS_REG(_pa, _reg,_val) \
        furia_reg_read(_pa, \
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_PCS, (_reg)), _val)
#define WRITE_FURIA_PCS_REG(_pa, _reg,_val) \
        furia_reg_write(_pa,\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_PCS, (_reg)), _val)
#define MODIFY_FURIA_PCS_REG(_pa, _reg, _val, _mask) \
        furia_reg_modify((_pa),\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_PCS, (_reg)),\
        (_val),\
        (_mask))

/* AN Device (Dev Addr 7) */
#define READ_FURIA_AN_REG(_pa, _reg,_val) \
        furia_reg_read(_pa, \
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_AN, (_reg)), _val)
#define WRITE_FURIA_AN_REG(_pa, _reg,_val) \
        furia_reg_write(_pa,\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_AN, (_reg)), _val)
#define MODIFY_FURIA_AN_REG(_pa, _reg, _val, _mask) \
        furia_reg_modify((_pa),\
        FURIA_CLAUSE45_ADDR(PHY_FURIA_C45_DEV_AN, (_reg)),\
        (_val),\
        (_mask))

#endif /*__FURIA_REG_ACCESS_H__*/
