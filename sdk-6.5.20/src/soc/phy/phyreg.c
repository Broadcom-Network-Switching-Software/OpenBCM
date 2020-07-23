/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy5482.c
 * Purpose:     PHY driver for BCM5482 and BCM5482S
 */

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/ll.h>
#include <soc/phy/phyctrl.h>

#include "phydefs.h"      /* Must include before other phy related includes */

#include "phyreg.h"

#define _SOC_PHY_REG_DIRECT \
        ((SOC_PHY_REG_1000X << 1) | (SOC_PHY_REG_1000X >> 1))


#define PHY_MODEL_CHECK(_pc, _oui, _model) \
                                (((_pc)->phy_oui == (_oui)) && \
                                 ((_pc)->phy_model == (_model)))


/* SHAD_EXPD for 54640E (macros) */
#define PHY_IS_BCM54640E(_pc)   (PHY_MODEL_CHECK((_pc), \
                                 0x001be9, \
                                 0x27) \
                                 && (((_pc)->phy_rev & 0x8) == 0))

uint16 phy54640e_save = 0;
#define BCM54640E_SHAD_EXPD_PRE(_unit, _pc, _reg_bank)\
    if (PHY_IS_BCM54640E((_pc))            &&                       \
        ((_pc)->flags & PHYCTRL_IS_PORT0)  &&                       \
        (_reg_bank) != 0x0d01              &&                       \
        ((_reg_bank) & 0xff00) == 0x0d00 ) {                        \
            SOC_IF_ERROR_RETURN                                     \
                (WRITE_PHY_REG((_unit), (_pc), 0x17, 0x0d01));      \
            SOC_IF_ERROR_RETURN                                     \
                (READ_PHY_REG((_unit), (_pc), 0x15, &phy54640e_save));  \
    } else {                                                        \
            phy54640e_save = 0;                                     \
    }

#define BCM54640E_SHAD_EXPD_POST(_unit, _pc, _rv)\
    if ((SOC_SUCCESS(_rv)) && phy54640e_save != 0) {                 \
        _rv = WRITE_PHY_REG((_unit), (_pc), 0x17, 0x0d01);           \
        if ((SOC_SUCCESS(_rv)) ) {                                   \
            _rv = WRITE_PHY_REG((_unit), (_pc), 0x15, phy54640e_save);   \
        }                                                            \
        phy54640e_save = 0;                                          \
    }


int 
phy_reg_modify(int unit, phy_ctrl_t *pc, uint32 reg_addr,
               uint16 reg_data, uint16 reg_mask)
{
    uint16  tmp, otmp;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    reg_data = reg_data & reg_mask;

    SOC_IF_ERROR_RETURN
        (READ_PHY_REG(unit, pc, reg_addr, &tmp));
    otmp = tmp;
    tmp &= ~(reg_mask);
    tmp |= reg_data;

    if (otmp != tmp) {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, reg_addr, tmp));
    }
    return SOC_E_NONE;
}
int 
phy_reg_fe_read(int unit, phy_ctrl_t *pc, uint16 reg_bank,
                uint8 reg_addr, uint16 *data)
{
    uint16 test_reg;
  
    if (reg_bank) {
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, 0x1f, &test_reg));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, test_reg | 0x0080));
    
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, reg_addr, data));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, test_reg));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, reg_addr, data));
    }
    return SOC_E_NONE;
}

int 
phy_reg_fe_write(int unit, phy_ctrl_t *pc, uint16 reg_bank, 
                 uint8 reg_addr, uint16 data)
{
    uint16 test_reg;
 
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }
 
    if (reg_bank) {
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, 0x1f, &test_reg));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, test_reg | 0x0080));
    
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, reg_addr, data));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, test_reg));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, reg_addr, data));
    }
    return SOC_E_NONE;
}

int 
phy_reg_fe_modify(int unit, phy_ctrl_t *pc, uint16 reg_bank, 
                  uint8 reg_addr, uint16 data, uint16 mask)
{
    uint16 test_reg;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }
  
    if (reg_bank) {
        SOC_IF_ERROR_RETURN
            (READ_PHY_REG(unit, pc, 0x1f, &test_reg));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, test_reg | 0x0080));
    
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY_REG(unit, pc, reg_addr, data, mask));
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, test_reg));
    } else {
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY_REG(unit, pc, reg_addr, data, mask));
    }
 
    return SOC_E_NONE;
}

int
phy_reg_ge_read(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 reg_addr, uint16 *data)
{
    int     rv;

    rv       = SOC_E_NONE;

    if (flags & SOC_PHY_REG_1000X) {
        if (reg_addr <= 0x000f) {
            uint16 blk_sel;

            /* Map 1000X page */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, 0x7c00));
            SOC_IF_ERROR_RETURN
                (READ_PHY_REG(unit, pc, 0x1c, &blk_sel));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, blk_sel | 0x8001));

            /* Read 1000X IEEE register */
            SOC_IF_ERROR_RETURN
                (READ_PHY_REG(unit, pc, reg_addr, data));

           /* Restore IEEE mapping */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, 
                                (blk_sel & 0xfffe) | 0x8000));
        } else {
            rv = SOC_E_PARAM;
        }
    } else {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0x15:
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x17, reg_bank));
            break;
        case 0x18:
            if (reg_bank <= 0x0007) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, reg_addr, 
                                    (reg_bank << 12) | 0x7));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if (reg_bank <= 0x001F) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, reg_addr, 
                                   (reg_bank << 10)));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1D:
            if (reg_bank <= 0x0001) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, reg_addr, reg_bank << 15));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not read from reserved registers */ 
                if (reg_addr > 0x001e) {
                   rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = READ_PHY_REG(unit, pc, reg_addr, data);
        }
    } 
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_reg_ge_read failed:"
                              " u=%d phy_id=0x%2x reg_bank=0x%04x reg_addr=0x%02x "
                              " rv=%d\n"), unit, pc->phy_id, reg_bank, reg_addr, rv)); 
    }
    return rv;
}


STATIC int
check_if_necessary_then_fix(int unit, 
			    phy_ctrl_t *pc, 
			    uint32 flags,
			    uint8 reg_addr, 
			    uint16 reg_bank, 
			    uint16 data, 
			    int chk)
{
    uint16 chk_data = 0;
    int rv = 0;


    if (reg_bank == 0xd01) return 0;

    rv = phy_reg_ge_read(unit, pc, flags,reg_bank, reg_addr, &chk_data);

    if (chk_data != data ) {
        /*
        bsl_printf("%s: data= 0x%08x,  chk_data %x, reg_addr=%x, bank=%x, counter %d\n",
        __FUNCTION__, data, chk_data, reg_addr, reg_bank, chk);
        */

        if(!chk) return 1;

        flags |= SOC_PHY_REG_NO_RETRY_ACCESS;
        rv = phy_reg_ge_write(unit, pc, flags, reg_bank, reg_addr, data);

        if (!rv ) {
            return check_if_necessary_then_fix(
                       unit, pc, flags, reg_addr, reg_bank, data, --chk);
        } else {
            return rv;
        }

    } else {
        return rv;
    }

    return 0;
}

int
phy_reg_ge_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 reg_addr, uint16 data)
{
    int     rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv       = SOC_E_NONE;

    if (flags & SOC_PHY_REG_1000X) {
        if (reg_addr <= 0x000f) {
            uint16 blk_sel;

            /* Map 1000X page */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, 0x7c00));
            SOC_IF_ERROR_RETURN
                (READ_PHY_REG(unit, pc, 0x1c, &blk_sel));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, blk_sel | 0x8001));

            /* Write 1000X IEEE register */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, reg_addr, data));

           /* Restore IEEE mapping */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, 
                                (blk_sel & 0xfffe) | 0x8000));
        } else if (flags & _SOC_PHY_REG_DIRECT) {
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, reg_addr, data));
        }  else {
            rv = SOC_E_PARAM;
        }
    } else {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0x15:
            /* SHAD_EXPD for 54640E */
            BCM54640E_SHAD_EXPD_PRE(unit, pc, reg_bank);

            SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, 0x17, reg_bank));
            break;
        case 0x18:
            if (reg_bank <= 0x0007) {
                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                }
                data = (data & ~(0x0007)) | reg_bank;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if (reg_bank <= 0x001F) {
                data = 0x8000 | (reg_bank << 10) | (data & 0x03FF);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1D:
            if (reg_bank == 0x0000) {
                data = data & 0x07FFF;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */ 
                if (reg_addr > 0x001e) {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, reg_addr, data);

            /* SHAD_EXPD for 54640E */
            BCM54640E_SHAD_EXPD_POST(unit, pc, rv);

            if  (!(flags & SOC_PHY_REG_NO_RETRY_ACCESS )) {
                if (SOC_SUCCESS(rv)  && PHY_IS_BCM54640E(pc)  && (reg_addr==0x15 && ((reg_bank&0x0f00) == 0x0d00))) {
                    rv = check_if_necessary_then_fix(
                            unit, pc, flags, reg_addr, reg_bank, data,
                            SOC_PHY_SHAD_RETRY_CNT);
                }
            }
        }
    } 

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_reg_ge_write failed:"
                              " u=%d phy_id=0x%2x reg_bank=0x%04x reg_addr=0x%02x "
                              " rv=%d\n"), unit, pc->phy_id, reg_bank, reg_addr, rv)); 
    }

    return rv;
}

int
phy_reg_ge_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 reg_addr, uint16 data, uint16 mask)
{
    int     rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv       = SOC_E_NONE;

    if (flags & SOC_PHY_REG_1000X) {
        if (reg_addr <= 0x000f) {
            uint16 blk_sel;

            /* Map 1000X page */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, 0x7c00));
            SOC_IF_ERROR_RETURN
                (READ_PHY_REG(unit, pc, 0x1c, &blk_sel));
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, blk_sel | 0x8001));

            /* Modify 1000X IEEE register */
            SOC_IF_ERROR_RETURN
                (MODIFY_PHY_REG(unit, pc, reg_addr, data, mask));

           /* Restore IEEE mapping */
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x1c, 
                                (blk_sel & 0xfffe) | 0x8000));
        } else {
            rv = SOC_E_PARAM;
        }
    } else {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0x15:
            /* SHAD_EXPD for 54640E */
            BCM54640E_SHAD_EXPD_PRE(unit, pc, reg_bank);

            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, 0x17, reg_bank));
            break;
        case 0x18:
            if (reg_bank <= 0x0007) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, reg_addr,
                                    (reg_bank << 12) | 0x7));

                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                    mask |= 0x8000;
                }
                mask &= ~(0x0007);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1C:
            if (reg_bank <= 0x001F) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, reg_addr, (reg_bank << 10)));
                data |= 0x8000;
                mask |= 0x8000;
                mask &= ~(0x1F << 10);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0x1D:
            if (reg_bank == 0x0000) {
                mask &= 0x07FFF;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */ 
                if (reg_addr > 0x001e) {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = MODIFY_PHY_REG(unit, pc, reg_addr, data, mask);

            /* SHAD_EXPD for 54640E */
            BCM54640E_SHAD_EXPD_POST(unit, pc, rv);

            if  (!(flags & SOC_PHY_REG_NO_RETRY_ACCESS )) {

                if (SOC_SUCCESS(rv)  && PHY_IS_BCM54640E(pc)  && (reg_addr==0x15 && ((reg_bank&0x0F00) == 0x0d00))) {

                    uint16 tmp;

                    rv = READ_PHY_REG(unit, pc, reg_addr, &tmp);

                    if (SOC_SUCCESS(rv)) {

                        tmp &= ~(mask);
                        tmp |= (data & mask);

                        rv = check_if_necessary_then_fix(
                                unit, pc, flags, reg_addr, reg_bank, tmp,
                                SOC_PHY_SHAD_RETRY_CNT);
                    }

                }
            }

        }
    } 

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_reg_ge_modify failed:"
                              " u=%d phy_id=0x%2x reg_bank=0x%04x reg_addr=0x%02x "
                              " rv=%d\n"), unit, pc->phy_id, reg_bank, reg_addr, rv)); 
    }
    return rv;
}

int
phy_reg_xge_read(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 *data)
{
    int     rv;
    uint32  value;
    uint16  tmp, tmp2;
    rv       = SOC_E_NONE;

    {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0xfff5:
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, 0xfff7), reg_bank));
            break;
        case 0xfff8:
            if (reg_bank <= 0x0007) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), 
                                    (reg_bank << 12) | 0x7));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0xfffc:
            if (reg_bank <= 0x001F) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), 
                                   (reg_bank << 10)));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0xfffd:
            if (reg_bank <= 0x0001) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), reg_bank << 15));
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not read from reserved registers */ 
                if (reg_addr > 0xfffe) {
                   rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            if(!(flags & SOC_PHY_REG_DUMMY_ACCESS)) {
                rv = READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data);
            }
            else {  
                /* Do two dummy reads. Read #3 and #4 gives values */
                READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), &tmp); /* Dummy read #1 */
                READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), &tmp); /* Dummy read #2 */
                READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), &tmp); /* Lower count */
                READ_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), &tmp2); /* Upper count */
                value = tmp2;
                value <<= 16; /*upper count*/
                value |= tmp;
                * ((uint32 *) data) = value;
            }
        }
    } 
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_reg_ge_read failed:"
                              " u=%d phy_id=0x%2x dev_addr=0x%02x reg_bank=0x%04x reg_addr=0x%02x "
                              " rv=%d\n"), unit, pc->phy_id, dev_addr, reg_bank, reg_addr, rv)); 
    }
    return rv;
}

int
phy_reg_xge_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 data)
{
    int     rv;

    rv       = SOC_E_NONE;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)) {
        return rv;
    }

    {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0xfff5:
            SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, 0xfff7), reg_bank));
            break;
        case 0xfff8:
            if (reg_bank <= 0x0007) {
                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                }
                data = (data & ~(0x0007)) | reg_bank;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0xfffc:
            if (reg_bank <= 0x001F) {
                data = 0x8000 | (reg_bank << 10) | (data & 0x03FF);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0xfffd:
            if (reg_bank == 0x0000) {
                data = data & 0x07FFF;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */ 
                if (reg_addr > 0xfffe) {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data);
        }
    } 

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_reg_ge_write failed:"
                              " u=%d phy_id=0x%2x dev_addr=0x%02x reg_bank=0x%04x reg_addr=0x%02x "
                              " rv=%d\n"), unit, pc->phy_id, dev_addr, reg_bank, reg_addr, rv)); 
    }

    return rv;
}

int
phy_reg_xge_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                uint8 dev_addr, uint16 reg_addr, uint16 data, uint16 mask)
{
    int     rv;

    rv       = SOC_E_NONE;

    {
        switch(reg_addr) {
        /* Map shadow registers */
        case 0xfff5:
            SOC_IF_ERROR_RETURN
                (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, 0xfff7), reg_bank));
            break;
        case 0xfff8:
            if (reg_bank <= 0x0007) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr),
                                    (reg_bank << 12) | 0x7));

                if (reg_bank == 0x0007) {
                    data |= 0x8000;
                    mask |= 0x8000;
                }
                mask &= ~(0x0007);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0xfffc:
            if (reg_bank <= 0x001F) {
                SOC_IF_ERROR_RETURN
                    (WRITE_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), (reg_bank << 10)));
                data |= 0x8000;
                mask |= 0x8000;
                mask &= ~(0x1F << 10);
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        case 0xfffd:
            if (reg_bank == 0x0000) {
                mask &= 0x07FFF;
            } else {
                rv = SOC_E_PARAM;
            }
            break;
        default:
            if (!(flags & SOC_PHY_REG_RESERVE_ACCESS)) {
                /* Must not write to reserved registers */ 
                if (reg_addr > 0xfffe) {
                    rv = SOC_E_PARAM;
                }
            }
            break;
        }
        if (SOC_SUCCESS(rv)) {
            rv = MODIFY_PHY_REG(unit, pc, SOC_PHY_CLAUSE45_ADDR(dev_addr, reg_addr), data, mask);
        }
    } 

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "phy_reg_ge_modify failed:"
                              " u=%d phy_id=0x%2x dev_addr=0x%02x reg_bank=0x%04x reg_addr=0x%02x "
                              " rv=%d\n"), unit, pc->phy_id, dev_addr, reg_bank, reg_addr, rv)); 
    }
    return rv;
}

int 
phy_reg_serdes_read(int unit, phy_ctrl_t *pc,  uint16 reg_bank, 
                    uint8 reg_addr, uint16 *phy_rd_data) 
{ 
    int    rv; 
 
    rv     = SOC_E_NONE; 
 
#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) { 
        if ((reg_bank != 0) || (reg_addr >= 0x0010)) { 
            reg_bank = 0x300 + (reg_bank << 4); 
        } 
    } 
#endif /* INCLUDE_PHY_XGXS6 */

    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank); 
 
    if (SOC_SUCCESS(rv)) { 
        rv = READ_PHY_REG(unit, pc, reg_addr, phy_rd_data);  
    } 
 
    return rv; 
}

int
phy_reg_serdes_write(int unit, phy_ctrl_t *pc, uint16 reg_bank, 
                     uint8 reg_addr, uint16 phy_wr_data)
{
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv     = SOC_E_NONE;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        if ((reg_bank != 0) || (reg_addr >= 0x0010)) {
            reg_bank = 0x300 + (reg_bank << 4);
        }
    }
#endif /* INCLUDE_PHY_XGXS6 */

    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);

    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, reg_addr, phy_wr_data);
    }

    return rv;
}

int
phy_reg_serdes_modify(int unit, phy_ctrl_t *pc,
                      uint16 reg_bank, uint8 reg_addr,
                      uint16 phy_mo_data, uint16 phy_mo_mask)
{
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv     = SOC_E_NONE;

#ifdef INCLUDE_PHY_XGXS6
    if (soc_feature(unit, soc_feature_xgxs_v6)) {
        if ((reg_bank != 0) || (reg_addr >= 0x0010)) {
            reg_bank = 0x300 + (reg_bank << 4);
        }
    }
#endif /* INCLUDE_PHY_XGXS6 */

    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);

    if (SOC_SUCCESS(rv)) {
        rv = MODIFY_PHY_REG(unit, pc, reg_addr, phy_mo_data, phy_mo_mask);
    }

    return rv;
}

#if defined(INCLUDE_PHY_XGXS)
int 
phy_reg_xgxs_read(int unit, phy_ctrl_t *pc, uint16 reg_bank,
                           uint8 reg_addr, uint16 *data)
{
    int    rv;

    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);

    if (SOC_SUCCESS(rv)) {
        rv = READ_PHY_REG(unit, pc, reg_addr, data);
    }

    return rv;
}

int 
phy_reg_xgxs_write(int unit, phy_ctrl_t *pc, uint16 reg_bank,
                            uint8 reg_addr, uint16 data)
{
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);

    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, reg_addr, data);
    }

    return rv;
}

int 
phy_reg_xgxs_modify(int unit, phy_ctrl_t *pc, uint16 reg_bank,
                             uint8 reg_addr, uint16 data, uint16 mask)
{
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);

    if (SOC_SUCCESS(rv)) {
        rv = MODIFY_PHY_REG(unit, pc, reg_addr, data, mask);
    }

    return rv;
}
#endif /* INCLUDE_PHY_XGXS */

#if defined(INCLUDE_PHY_XGXS6)
int 
phy_reg_xgxs6_read(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                           uint8 reg_addr, uint16 *data)
{
    int    rv;

    rv     = SOC_E_NONE;

    if (reg_addr < 0x10) {
        /* Select between SerDes and XAUI mapping */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, 0x0000));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY_REG(unit, pc, 0x1e, 
                             (flags & SOC_PHY_REG_1000X) ? 0x0000 : 0x0001,
                             0x0001)); 
    }

    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);
    }

    if (SOC_SUCCESS(rv)) {
        rv = READ_PHY_REG(unit, pc, reg_addr, data);
    }

    return rv;
}

int 
phy_reg_xgxs6_write(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                            uint8 reg_addr, uint16 data)
{
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv     = SOC_E_NONE;

    if (reg_addr < 0x10) {
        /* Select between SerDes and XAUI mapping */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, 0x0000));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY_REG(unit, pc, 0x1e, 
                             (flags & SOC_PHY_REG_1000X) ? 0x0000 : 0x0001,
                             0x0001)); 
    }

    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);
    }

    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, reg_addr, data);
    }

    return rv;
}

int 
phy_reg_xgxs6_modify(int unit, phy_ctrl_t *pc, uint32 flags, uint16 reg_bank,
                             uint8 reg_addr, uint16 data, uint16 mask)
{
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv     = SOC_E_NONE;

    if (reg_addr < 0x10) {
        /* Select between SerDes and XAUI mapping */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 0x1f, 0x0000));
        SOC_IF_ERROR_RETURN
            (MODIFY_PHY_REG(unit, pc, 0x1e, 
                             (flags & SOC_PHY_REG_1000X) ? 0x0000 : 0x0001,
                             0x0001)); 
    }

    if (SOC_SUCCESS(rv)) {
        rv = WRITE_PHY_REG(unit, pc, 0x1f, reg_bank);
    }

    if (SOC_SUCCESS(rv)) {
        rv = MODIFY_PHY_REG(unit, pc, reg_addr, data, mask);
    }

    return rv;
}
#endif /* INCLUDE_PHY_XGXS6 */

STATIC int 
_phy_reg_aer_cl45_read(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                 uint16 *data)
{
    uint16 phy_reg_addr;
    int    cl45_devid;

    phy_reg_addr = PHY_AER_REG_ADDR_CL45_REGAD(reg_addr);
    cl45_devid   = PHY_AER_REG_ADDR_CL45_DEVID(reg_addr);

    if (PHY_AER_REG_ADDR_LANE_NUM(reg_addr)) {
        /* write the lane number to the AER if lane specific */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(cl45_devid,PHY_AER_REG),
                PHY_AER_REG_ADDR_LANE_NUM(reg_addr)));
    }
    SOC_IF_ERROR_RETURN
        (READ_PHY_REG(unit, pc, 
            SOC_PHY_CLAUSE45_ADDR(cl45_devid,phy_reg_addr), data));

    if (PHY_AER_REG_ADDR_LANE_NUM(reg_addr)) {
        /* restore back the lane number to 0 */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(cl45_devid,PHY_AER_REG),
                0));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_reg_aer_cl45_write(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                  uint16 data)
{
    uint16 phy_reg_addr;
    int    cl45_devid;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    phy_reg_addr = PHY_AER_REG_ADDR_CL45_REGAD(reg_addr);
    cl45_devid = PHY_AER_REG_ADDR_CL45_DEVID(reg_addr);

    if (PHY_AER_REG_ADDR_LANE_NUM(reg_addr)) {
        /* write the lane number to the AER if lane specific */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(cl45_devid,PHY_AER_REG),
                PHY_AER_REG_ADDR_LANE_NUM(reg_addr)));
    }
    SOC_IF_ERROR_RETURN
        (WRITE_PHY_REG(unit, pc, 
            SOC_PHY_CLAUSE45_ADDR(cl45_devid,phy_reg_addr), data));

    if (PHY_AER_REG_ADDR_LANE_NUM(reg_addr)) {
        /* write the lane number 0 to the AER */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(cl45_devid,PHY_AER_REG),
                0));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_reg_aer_cl45_modify(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                  uint16 data,uint16 mask)
{
    uint16 phy_reg_addr;
    int    cl45_devid;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    phy_reg_addr = PHY_AER_REG_ADDR_CL45_REGAD(reg_addr);
    cl45_devid = PHY_AER_REG_ADDR_CL45_DEVID(reg_addr);

    if (PHY_AER_REG_ADDR_LANE_NUM(reg_addr)) {
        /* write the lane number to the AER if lane specific */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(cl45_devid,PHY_AER_REG),
                PHY_AER_REG_ADDR_LANE_NUM(reg_addr)));
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_PHY_REG(unit, pc, 
            SOC_PHY_CLAUSE45_ADDR(cl45_devid,phy_reg_addr), data,mask));

    if (PHY_AER_REG_ADDR_LANE_NUM(reg_addr)) {
        /* write the lane number 0 to the AER */
        SOC_IF_ERROR_RETURN
            (WRITE_PHY_REG(unit, pc, 
                SOC_PHY_CLAUSE45_ADDR(cl45_devid,PHY_AER_REG),
                0));
    }
    return SOC_E_NONE;
}

STATIC int 
_phy_reg_aer_cl22_read(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                 uint16 *data)
{
    uint16 phy_reg_aer;
    uint16 phy_reg_blk;
    uint16 phy_reg_addr;
    int    rv;

    rv     = SOC_E_UNAVAIL;

    phy_reg_aer  = PHY_AER_REG_ADDR_AER(reg_addr);
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(reg_addr);
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(reg_addr);
    if (phy_reg_aer == 0) {
        rv = WRITE_PHY_REG(unit, pc, 0x1f, phy_reg_blk);

        if (SOC_SUCCESS(rv)) {
            rv = READ_PHY_REG(unit, pc, phy_reg_addr, data);
        }
    } else {
        /* Set desired AER */
        rv = WRITE_PHY_REG(unit, pc, 0x1f, 0xffd0);
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1e, phy_reg_aer);
        }

        /* Map requested block */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, phy_reg_blk);
        }

        /* Read from the requested register */
        if (SOC_SUCCESS(rv)) {
            rv = READ_PHY_REG(unit, pc, phy_reg_addr, data);
        }

        /* Set AER back to 0 */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, 0xffd0);
        }
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1e, 0);
        }
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, 0x0);
        }
    }
    return rv;
}

STATIC int 
_phy_reg_aer_cl22_write(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                  uint16 data)
{
    uint16 phy_reg_aer;
    uint16 phy_reg_blk;
    uint16 phy_reg_addr;
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv     = SOC_E_UNAVAIL;

    phy_reg_aer  = PHY_AER_REG_ADDR_AER(reg_addr);
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(reg_addr);
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(reg_addr);
    if (phy_reg_aer == 0) {
        rv = WRITE_PHY_REG(unit, pc, 0x1f, phy_reg_blk);

        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, phy_reg_addr, data);
        }
    } else {
        /* Set desired AER */
        rv = WRITE_PHY_REG(unit, pc, 0x1f, 0xffd0);
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1e, phy_reg_aer);
        }

        /* Map requested block */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, phy_reg_blk);
        }

        /* Write to requested register */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, phy_reg_addr, data);
        }

        /* Set AER back to 0 */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, 0xffd0);
        }
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1e, 0);
        }
    }
    return rv;
}

STATIC int 
_phy_reg_aer_cl22_modify(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                   uint16 data, uint16 mask)
{
    uint16 phy_reg_aer;
    uint16 phy_reg_blk;
    uint16 phy_reg_addr;
    int    rv;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    rv     = SOC_E_UNAVAIL;

    phy_reg_aer  = PHY_AER_REG_ADDR_AER(reg_addr);
    phy_reg_blk  = PHY_AER_REG_ADDR_BLK(reg_addr);
    phy_reg_addr = PHY_AER_REG_ADDR_REGAD(reg_addr);
    if (phy_reg_aer == 0) {
        /* Only support AER = 0 for now */
        rv = WRITE_PHY_REG(unit, pc, 0x1f, phy_reg_blk);

        if (SOC_SUCCESS(rv)) {
            rv = MODIFY_PHY_REG(unit, pc, phy_reg_addr, data, mask);
        }
    } else {
        rv = WRITE_PHY_REG(unit, pc, 0x1f, 0xffd0);
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1e, phy_reg_aer);
        }

        /* Map requested block */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, phy_reg_blk);
        }

        /* Modify requested register */
        if (SOC_SUCCESS(rv)) {
            rv = MODIFY_PHY_REG(unit, pc, phy_reg_addr, data, mask);
        }

        /* Set AER back to 0 */
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1f, 0xffd0);
        }
        if (SOC_SUCCESS(rv)) {
            rv = WRITE_PHY_REG(unit, pc, 0x1e, 0);
        }

    }
    return rv;

}

int
phy_reg_aer_read(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                 uint16 *data)
{
    int rv;
    if (pc->flags & PHYCTRL_MDIO_CL45) {
        rv = _phy_reg_aer_cl45_read(unit,pc,reg_addr,data);
    } else {
        rv = _phy_reg_aer_cl22_read(unit,pc,reg_addr,data);
    }
    return rv;
}

int
phy_reg_aer_write(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                 uint16 data)
{
    int rv;
    if (pc->flags & PHYCTRL_MDIO_CL45) {
        rv = _phy_reg_aer_cl45_write(unit,pc,reg_addr,data);
    } else {
        rv = _phy_reg_aer_cl22_write(unit,pc,reg_addr,data);
    }
    return rv;
}
     
int
phy_reg_aer_modify(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                 uint16 data,uint16 mask)
{
    int rv;
    if (pc->flags & PHYCTRL_MDIO_CL45) {
        rv = _phy_reg_aer_cl45_modify(unit,pc,reg_addr,data,mask);
    } else {
        rv = _phy_reg_aer_cl22_modify(unit,pc,reg_addr,data,mask);
    }
    return rv;
}
     
#if defined(INCLUDE_XGXS_QSGMII65)
int 
phy_reg_qsgmii_aer_read(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                 uint16 *data)
{
    int    rv = SOC_E_UNAVAIL;

    reg_addr = reg_addr | (pc->lane_num << 16);

    rv = phy_reg_aer_read(unit, pc, reg_addr, data);
    return rv;
}

int 
phy_reg_qsgmii_aer_write(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                  uint16 data)
{
    int    rv = SOC_E_UNAVAIL;

    reg_addr = reg_addr | (pc->lane_num << 16);

    rv = phy_reg_aer_write(unit, pc, reg_addr, data);
    return rv;
}

int 
phy_reg_qsgmii_aer_modify(int unit, phy_ctrl_t *pc, uint32 reg_addr,
                   uint16 data, uint16 mask)
{
    int    rv = SOC_E_UNAVAIL;

    reg_addr = reg_addr | (pc->lane_num << 16);
    
    rv = phy_reg_aer_modify(unit, pc, reg_addr, data, mask);
    return rv;
}
#endif /* INCLUDE_XGXS_QSGMII65 */
